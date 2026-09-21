#pragma once
#include "engine/signal_extensions.h"
#include <map>
#include <string>
#include <stdexcept>

namespace nimby::engine {
struct NamedSignalExtension {
    uint64_t script{};
    std::string typeName;
    std::map<std::string,ExtensionValue> fields;
};

namespace extension_detail {
// Bounded optimistic read transaction. A second pass verifies all source bytes;
// this detects replacement, but does not claim engine-thread atomicity.
class ReadAudit {
    struct Region { uint64_t address;std::vector<unsigned char> bytes; };
    ReadMemory read_;void* context_;size_t total_=0;
    std::vector<Region> regions_;
public:
    ReadAudit(ReadMemory read,void* context):read_(read),context_(context){}
    static uint64_t word(const void* data,size_t offset=0){uint64_t v;std::memcpy(&v,static_cast<const unsigned char*>(data)+offset,8);return v;}
    static bool pointer(uint64_t p){return p>=0x10000 && p<0x7fffffff0000ULL && p%8==0;}
    std::vector<unsigned char> bytes(uint64_t address,size_t size) {
        if(!read_||!pointer(address)||size>0x7fffffff0000ULL-address||size>1024*1024-total_)
            throw std::runtime_error("Invalid extension read range");
        std::vector<unsigned char> result(size);
        if(size&&!read_(context_,address,result.data(),size))throw std::runtime_error("Extension read unavailable");
        total_+=size;regions_.push_back({address,result});return result;
    }
    uint64_t wordAt(uint64_t address){const auto value=bytes(address,8);return word(value.data());}
    size_t vectorSize(const unsigned char* p,size_t stride,size_t limit) {
        const auto begin=word(p),end=word(p,8),cap=word(p,16);
        if((!begin && (end||cap)) || (begin&&!pointer(begin)) || end<begin || cap<end ||
           cap-begin>limit*stride || (end-begin)%stride || (cap-begin)%stride)
            throw std::runtime_error("Invalid extension vector");
        return (end-begin)/stride;
    }
    std::string string(const unsigned char* p) {
        const auto size=word(p,16),capacity=word(p,24);
        if(!size||size>256||capacity<15||capacity<size||capacity>4096)
            throw std::runtime_error("Invalid extension name");
        std::vector<unsigned char> storage;
        if(capacity!=15){storage=bytes(word(p),size+1);p=storage.data();}
        if(p[size]||std::memchr(p,0,size))throw std::runtime_error("Invalid extension name terminator");
        return {reinterpret_cast<const char*>(p),static_cast<size_t>(size)};
    }
    // Native std::unordered_map list. Key is at node+0x10, value at +0x18.
    // Validate the entire bounded list so a duplicate key never wins by order.
    uint64_t mapValue(uint64_t map,uint64_t key) {
        const auto header=bytes(map+8,16);
        const auto head=word(header.data()),count=word(header.data(),8);
        if(!pointer(head)||count>4096)throw std::runtime_error("Invalid extension map");
        auto node=wordAt(head);uint64_t found=0;std::set<uint64_t> visited;
        while(node!=head) {
            if(visited.size()>=count||!visited.insert(node).second)throw std::runtime_error("Invalid extension map chain");
            const auto entry=bytes(node,24);
            if(word(entry.data(),16)==key){if(found)throw std::runtime_error("Duplicate extension map key");found=node+24;}
            node=word(entry.data());
        }
        if(visited.size()!=count)throw std::runtime_error("Incomplete extension map");
        return found;
    }
    uint64_t scriptAddress(uint64_t database,uint64_t id) {
        if(id>>48!=7)throw std::runtime_error("Invalid script ID");
        const auto header=bytes(database+0x300,48);
        uint32_t shift{},slots{},mask{};
        std::memcpy(&shift,header.data()+4,4);std::memcpy(&slots,header.data()+8,4);std::memcpy(&mask,header.data()+16,4);
        if(!shift||shift>16||slots!=(1u<<shift)||mask!=slots-1)throw std::runtime_error("Invalid script pool");
        const auto count=vectorSize(header.data()+24,8,1024);
        const uint64_t index=(id>>16)&0xffffffffULL;
        if((index>>shift)>=count)throw std::runtime_error("Missing script slot");
        const auto block=wordAt(word(header.data(),24)+(index>>shift)*8);
        if(!pointer(block)||block>=0x7fffffff0000ULL-slots*0x150ULL)throw std::runtime_error("Invalid script block");
        const auto address=block+(index&mask)*0x150;
        if(wordAt(address)!=id)throw std::runtime_error("Recycled script ID");
        return address;
    }
    bool stable() const {
        for(const auto& region:regions_){std::vector<unsigned char> after(region.bytes.size());
            if(!after.empty()&&(!read_(context_,region.address,after.data(),after.size())||after!=region.bytes))return false;}
        return true;
    }
};
}

// Resolve names against the *current compiled schema*, not field positions alone.
// No fallback to names from a previous compilation when schema fingerprints differ.
inline bool read_named_signal_extensions(ReadMemory read,void* context,const LiveState& state,
    uint64_t signalAddress,uint64_t signalId,bool recognized,std::vector<NamedSignalExtension>& out) noexcept {
    out.clear();
    try {
        std::vector<SignalExtension> raw,again;
        if(!read_signal_extensions(read,context,signalAddress,signalId,recognized,raw))return false;
        extension_detail::ReadAudit audit(read,context);
        std::vector<NamedSignalExtension> result;
        for(const auto& extension:raw) {
            const auto script=audit.scriptAddress(state.database,extension.script);
            const auto status=audit.bytes(script+0xd8,24);
            if(status[0]!=1||audit.word(status.data(),8)!=audit.word(status.data(),16))return false;
            const auto library=audit.mapValue(audit.wordAt(state.database+0x1518),extension.script);
            if(!library)return false;
            const auto errors=audit.bytes(library,16);
            if(audit.word(errors.data())!=audit.word(errors.data(),8))return false;
            // Own script types only. An inherited builtin is not a user extension.
            const auto type=audit.mapValue(library+0x70+0x48,extension.type);
            if(!type)return false;
            const auto definition=audit.bytes(type,0x90);
            if(audit.word(definition.data(),0x20)!=extension.type ||
               audit.word(definition.data(),0x88)!=extension.opaqueHeader)return false;
            const auto count=audit.vectorSize(definition.data()+0x70,0x60,512);
            if(count!=extension.values.size()||count!=extension.metadata.size())return false;
            NamedSignalExtension named{extension.script,audit.string(definition.data()),{}};
            const auto fields=count?audit.bytes(audit.word(definition.data(),0x70),count*0x60):std::vector<unsigned char>{};
            for(size_t i=0;i<count;++i){
                const auto* field=fields.data()+i*0x60;
                if(audit.word(field,0x20)!=extension.metadata[i][0])return false;
                if(!named.fields.emplace(audit.string(field),extension.values[i]).second)return false;
            }
            result.push_back(std::move(named));
        }
        if(!audit.stable() || !read_signal_extensions(read,context,signalAddress,signalId,recognized,again)||raw!=again)return false;
        out=std::move(result);return true;
    }catch(...){out.clear();return false;}
}
}
