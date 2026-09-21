#pragma once
#include "engine/live_state.h"
#include <array>
#include <cstring>
#include <optional>
#include <set>
#include <vector>

namespace nimby::engine {
// Experimental native records, deliberately not part of the mod API.
// Evidence and unresolved field identities: docs/research/signal-extensions.md.
struct ExtensionValue {
    std::array<unsigned char,16> bytes{};
    bool operator==(const ExtensionValue&) const = default;
    // MSVC variant<i64,f64,bool,...>: only the bool alternative is decoded here.
    std::optional<bool> boolean() const {
        if(bytes[8]!=2 || bytes[0]>1) return std::nullopt;
        return bytes[0]!=0;
    }
};
struct SignalExtension {
    uint64_t script{}, opaqueHeader{}, type{};
    std::vector<ExtensionValue> values;
    std::vector<std::array<uint64_t,2>> metadata;
    bool operator==(const SignalExtension&) const = default;
};

// The caller supplies a freshly resolved Signal address and full generation ID.
// No native call, write, or retained process pointer. False means unavailable,
// while true + empty means a stable empty extension list was observed.
inline bool read_signal_extensions(ReadMemory read,void* context,uint64_t address,
    uint64_t id,bool recognized,std::vector<SignalExtension>& out) noexcept {
    out.clear();
    try {
        const auto pointer=[](uint64_t p){return p>=0x10000 && p<0x7fffffff0000ULL && p%8==0;};
        if(!recognized || !read || !pointer(address) || id>>48!=8) return false;
        const auto word=[](const void* p,size_t offset){uint64_t v;std::memcpy(&v,static_cast<const unsigned char*>(p)+offset,8);return v;};
        const auto vectorCount=[&](uint64_t begin,uint64_t end,uint64_t cap,size_t stride,size_t limit)->std::optional<size_t>{
            if(!begin) return end==0 && cap==0 ? std::optional<size_t>{0} : std::nullopt;
            if(!pointer(begin) || end<begin || cap<end || cap-begin>limit*stride ||
               (end-begin)%stride || (cap-begin)%stride) return std::nullopt;
            return (end-begin)/stride;
        };
        std::array<unsigned char,0xa8> signal{},after{};
        if(!read(context,address,signal.data(),signal.size()) || word(signal.data(),0)!=id) return false;
        // RVA 0x79fb8d passes Signal+0x90 to ScriptStructInstancesTabs.
        const auto begin=word(signal.data(),0x90);
        const auto count=vectorCount(begin,word(signal.data(),0x98),word(signal.data(),0xa0),0x50,64);
        if(!count) return false;
        std::vector<std::array<unsigned char,0x50>> instances(*count),instancesAfter(*count);
        if(*count && !read(context,begin,instances.data(),*count*0x50)) return false;
        std::set<std::pair<uint64_t,uint64_t>> identities;
        std::vector<SignalExtension> result;
        for(const auto& instance:instances) {
            const auto* p=instance.data();
            SignalExtension extension{word(p,0),word(p,8),word(p,0x40),{}, {}};
            // Script ID tag verified by native pool accessor RVA 0x33f6a0.
            if(extension.script>>48!=7 || !identities.emplace(extension.script,extension.type).second) return false;
            const auto values=vectorCount(word(p,0x10),word(p,0x18),word(p,0x20),16,512);
            const auto meta=vectorCount(word(p,0x28),word(p,0x30),word(p,0x38),16,512);
            if(!values || !meta) return false;
            extension.values.resize(*values);extension.metadata.resize(*meta);
            std::vector<ExtensionValue> valuesAfter(*values);
            std::vector<std::array<uint64_t,2>> metaAfter(*meta);
            if(*values && (!read(context,word(p,0x10),extension.values.data(),*values*16) ||
               !read(context,word(p,0x10),valuesAfter.data(),*values*16) ||
               std::memcmp(extension.values.data(),valuesAfter.data(),*values*16))) return false;
            if(*meta && (!read(context,word(p,0x28),extension.metadata.data(),*meta*16) ||
               !read(context,word(p,0x28),metaAfter.data(),*meta*16) || extension.metadata!=metaAfter)) return false;
            result.push_back(std::move(extension));
        }
        if(*count && (!read(context,begin,instancesAfter.data(),*count*0x50) || instances!=instancesAfter)) return false;
        if(!read(context,address,after.data(),after.size()) || word(after.data(),0)!=id ||
           std::memcmp(signal.data()+0x90,after.data()+0x90,24)) return false;
        out=std::move(result);return true;
    } catch(...) { out.clear();return false; }
}
}
