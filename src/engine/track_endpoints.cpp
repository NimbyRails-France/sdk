#include <engine/track_endpoints.h>
#include <engine/detail/memory_reader.h>
#include <algorithm>
#include <array>
#include <cmath>
#include <cstring>
#include <memory>
#include <unordered_map>
#include <unordered_set>

namespace nimby::engine {
namespace {
using memory::field;using memory::pointer;
using Kind=TrackEndpointKind;using Reason=TrackEndpointReason;
class TrackPool {
    ReadMemory read_;void* context_;uint64_t address_,table_{};
    std::array<unsigned char,48> header_{};std::vector<uint64_t> blocks_;
    uint32_t shift_{},mask_{};
public:
    TrackPool(ReadMemory read,void* context,uint64_t address):read_(read),context_(context),address_(address){}
    bool load(){
        if(!read_(context_,address_,header_.data(),header_.size()))return false;
        shift_=field<uint32_t>(header_.data(),4);mask_=field<uint32_t>(header_.data(),16);
        const auto size=field<uint32_t>(header_.data(),8);
        table_=field<uint64_t>(header_.data(),24);
        const auto end=field<uint64_t>(header_.data(),32),cap=field<uint64_t>(header_.data(),40);
        if(!shift_||shift_>16||size!=(1u<<shift_)||mask_!=size-1||end<table_||cap<end||
           cap-table_>8192||(end-table_)%8||(cap-table_)%8||((end-table_)/8)*size>1048576||
           (cap&&(!pointer(table_)||cap>=0x7fffffff0000ULL)))return false;
        blocks_.resize(static_cast<size_t>((end-table_)/8));
        return blocks_.empty()||read_(context_,table_,blocks_.data(),blocks_.size()*8);
    }
    bool resolve(uint64_t id,uint64_t& address)const{
        if(id>>48!=1)return false;
        const auto index=(id>>16)&0xffffffffULL,block=index>>shift_;
        constexpr size_t stride=0x4e8;
        if(block>=blocks_.size()||!pointer(blocks_[block])||
           blocks_[block]>0x7fffffff0000ULL-size_t(mask_+1)*stride)return false;
        address=blocks_[block]+(index&mask_)*stride;return true;
    }
    bool stable(){
        std::array<unsigned char,48> after{};std::vector<uint64_t> blocks(blocks_.size());
        return read_(context_,address_,after.data(),after.size())&&after==header_&&
            (blocks.empty()||(read_(context_,table_,blocks.data(),blocks.size()*8)&&blocks==blocks_))&&
            read_(context_,address_,after.data(),after.size())&&after==header_;
    }
};
struct Record {
    uint64_t id{},address{},station{};
    std::array<unsigned char,0x30> identity{},attachment{};
    Reason reason=Reason::TrackUnavailable,childrenReason=Reason::DescriptorInvalid;
    bool childrenRead=false,stable=false;
    std::vector<uint64_t> children;
    uint64_t link(unsigned side)const{return field<uint64_t>(identity.data(),8+side*8);}
    uint64_t parent()const{return field<uint64_t>(attachment.data(),0);}
    double fraction()const{return field<double>(attachment.data(),8);}
    int direction()const{return field<int32_t>(attachment.data(),16);}
    bool validBranch()const{
        return parent()&&parent()!=id&&parent()>>48==1&&
            (link(0)==0)!=(link(1)==0)&&std::isfinite(fraction())&&
            fraction()>=0&&fraction()<=1&&(direction()==1||direction()==-1);
    }
};
class Capture {
    ReadMemory read_;void* context_;TrackPool& pool_;
    size_t remaining_=NIMBY_TRACK_ENDPOINTS_MAX_REFERENCES;
    // Heap ownership preserves Record addresses across map rehashes. One
    // first read/final verification per referenced ID, shared by the batch.
    std::unordered_map<uint64_t,std::unique_ptr<Record>> records_;
public:
    Capture(ReadMemory read,void* context,TrackPool& pool):read_(read),context_(context),pool_(pool){}
    std::unordered_set<uint64_t> dependencies;
    Record* record(uint64_t id,Reason& reason){
        if(id>>48!=1){reason=Reason::ReferenceUnavailable;return nullptr;}
        dependencies.insert(id);
        if(const auto found=records_.find(id);found!=records_.end()){
            reason=found->second->reason;return reason==Reason::Complete?found->second.get():nullptr;
        }
        if(!remaining_){reason=Reason::BudgetExceeded;return nullptr;}
        --remaining_;
        auto owned=std::make_unique<Record>();auto& row=*owned;row.id=id;
        if(pool_.resolve(id,row.address)&&
           read_(context_,row.address,row.identity.data(),row.identity.size())&&
           field<uint64_t>(row.identity.data(),0)==id){
            row.reason=Reason::ReadUnavailable;
            if(read_(context_,row.address+0x3f0,row.attachment.data(),row.attachment.size())&&
               read_(context_,row.address+0xd0,&row.station,sizeof row.station))
                row.reason=(!row.station||row.station>>48==2)?Reason::Complete:Reason::AttachmentInvalid;
        }
        reason=row.reason;auto* result=&row;records_.emplace(id,std::move(owned));
        return reason==Reason::Complete?result:nullptr;
    }
    Reason children(Record& row){
        if(row.childrenRead)return row.childrenReason;
        row.childrenRead=true;
        const auto begin=field<uint64_t>(row.attachment.data(),24),end=field<uint64_t>(row.attachment.data(),32),cap=field<uint64_t>(row.attachment.data(),40);
        // Both native all-zero empty storage and retained empty storage count
        // as complete; a non-null malformed descriptor never does.
        if(end<begin||cap<end||cap-begin>4096*8||(end-begin)%8||(cap-begin)%8||
           (cap&&(!pointer(begin)||cap>=0x7fffffff0000ULL)))return row.childrenReason;
        const auto count=static_cast<size_t>((end-begin)/8);
        if(count>remaining_)return row.childrenReason=Reason::BudgetExceeded;
        remaining_-=count;
        row.children.resize(count);std::vector<uint64_t> again(count);
        if(count&&(!read_(context_,begin,row.children.data(),count*8)||!read_(context_,begin,again.data(),count*8)))
            return row.childrenReason=Reason::ReadUnavailable;
        if(row.children!=again)return row.childrenReason=Reason::Unstable;
        std::unordered_set<uint64_t> unique;unique.reserve(count);
        for(auto id:row.children)if(id>>48!=1||id==row.id||!unique.insert(id).second)
            return row.childrenReason=Reason::AttachmentInvalid;
        return row.childrenReason=Reason::Complete;
    }
    TrackEndpointState endpoint(Record& row,unsigned side){
        Reason reason{};
        const auto reference=[&](uint64_t id){
            auto* result=record(id,reason);
            if(reason==Reason::TrackUnavailable)reason=Reason::ReferenceUnavailable;
            return result;
        };
        if(const auto link=row.link(side)){
            if(link==row.id)return {Kind::Unknown,Reason::AttachmentInvalid};
            if(!reference(link))return {Kind::Unknown,reason};
            return {Kind::Connected,Reason::Complete};
        }
        // A branch's missing primary joins its parent, even if the parent
        // attachment is at fraction 0/1. It must never become a false terminal.
        if(row.parent()){
            if(!row.validBranch())return {Kind::Unknown,Reason::AttachmentInvalid};
            auto* main=reference(row.parent());if(!main)return {Kind::Unknown,reason};
            reason=children(*main);if(reason!=Reason::Complete)return {Kind::Unknown,reason};
            if(std::count(main->children.begin(),main->children.end(),row.id)!=1)
                return {Kind::Unknown,Reason::AttachmentInvalid};
            return {Kind::Connected,Reason::Complete};
        }
        reason=children(row);if(reason!=Reason::Complete)return {Kind::Unknown,reason};
        bool outgoing=false;
        for(const auto childId:row.children){
            auto* child=reference(childId);if(!child)return {Kind::Unknown,reason};
            if(!child->validBranch()||child->parent()!=row.id)return {Kind::Unknown,Reason::AttachmentInvalid};
            // Native RVA 0x379d10 follows only same-direction children ahead;
            // only an attachment exactly at this endpoint continues beyond it.
            if(child->fraction()==double(side)&&child->direction()==(side?1:-1))outgoing=true;
        }
        return {outgoing?Kind::Connected:Kind::Terminal,Reason::Complete};
    }
    void verify(){
        for(auto& [id,owned]:records_){
            (void)id;auto& row=*owned;if(row.reason!=Reason::Complete)continue;
            std::array<unsigned char,0x30> identity{},attachment{};uint64_t station{};
            row.stable=read_(context_,row.address,identity.data(),identity.size())&&identity==row.identity&&
                read_(context_,row.address+0x3f0,attachment.data(),attachment.size())&&attachment==row.attachment&&
                read_(context_,row.address+0xd0,&station,sizeof station)&&station==row.station;
            if(row.stable&&row.childrenRead&&row.childrenReason==Reason::Complete&&!row.children.empty()){
                std::vector<uint64_t> again(row.children.size());const auto begin=field<uint64_t>(attachment.data(),24);
                row.stable=read_(context_,begin,again.data(),again.size()*8)&&again==row.children;
                // Recheck descriptor/identity after this dependent array, too.
                row.stable=row.stable&&read_(context_,row.address,identity.data(),identity.size())&&identity==row.identity&&
                    read_(context_,row.address+0x3f0,attachment.data(),attachment.size())&&attachment==row.attachment;
            }
        }
    }
    bool stable(uint64_t id)const{
        const auto found=records_.find(id);
        // Missing/read-invalid references already have their precise Unknown
        // reason. Only a first-read-valid reference can later become unstable.
        return found==records_.end()||found->second->reason!=Reason::Complete||found->second->stable;
    }
};
bool same_root(ReadMemory read,void* context,const LiveState& state){
    LiveState current{};return resolve_live_state(read,context,state.module_base,true,state.profile,current)&&current==state;
}
}
bool read_track_endpoints(ReadMemory read,void* context,const LiveState& state,bool recognized,
    std::span<const uint64_t> ids,std::vector<NimbyTrackEndpointStates>& out) noexcept {
    out.clear();
    // Offset qualification is Windows119/SHA-256 FFF49... only. A qualified
    // Linux root does not qualify this downstream native object layout.
    if(!read||!recognized||state.profile!=LiveStateProfile::Windows119||ids.empty()||ids.size()>NIMBY_TRACK_ENDPOINTS_MAX_TRACKS)return false;
    for(auto id:ids)if(id>>48!=1)return false;
    try{
        if(!same_root(read,context,state))return false;
        TrackPool pool(read,context,state.database);if(!pool.load())return false;
        Capture capture(read,context,pool);
        std::vector<NimbyTrackEndpointStates> result;result.reserve(ids.size());
        std::vector<std::unordered_set<uint64_t>> dependencies;dependencies.reserve(ids.size());
        std::unordered_set<uint64_t> requested;
        for(auto id:ids){
            if(!requested.insert(id).second)continue;
            capture.dependencies.clear();Reason reason{};auto* row=capture.record(id,reason);
            TrackEndpointState a{},b{};uint64_t station{};
            if(row){station=row->station;a=capture.endpoint(*row,0);b=capture.endpoint(*row,1);}
            else a.reason=b.reason=reason;
            result.push_back({id,station,static_cast<uint32_t>(a.kind),static_cast<uint32_t>(b.kind),static_cast<uint32_t>(a.reason),static_cast<uint32_t>(b.reason)});
            dependencies.push_back(std::move(capture.dependencies));
        }
        capture.verify();
        for(size_t i=0;i<result.size();++i){
            auto& row=result[i];
            if(!capture.stable(row.track_id))row.station_id=0;
            if(std::any_of(dependencies[i].begin(),dependencies[i].end(),[&](auto id){return !capture.stable(id);} )){
                row.state_a=row.state_b=NIMBY_TRACK_ENDPOINT_UNKNOWN;
                row.reason_a=row.reason_b=NIMBY_TRACK_ENDPOINT_UNSTABLE;
            }
        }
        if(!pool.stable()||!same_root(read,context,state))return false;
        out=std::move(result);return true;
    }catch(...){out.clear();return false;}
}
}
