#include <engine/track_conflicts.h>
#include <unordered_map>
#include <unordered_set>
namespace nimby::engine {
namespace {
using memory::field;using memory::pointer;
struct BudgetedReader {
    ReadMemory read;void* context;TrackConflictReadBudget& budget;
    static bool copy(void* context,uint64_t address,void* output,size_t bytes){
        auto& source=*static_cast<BudgetedReader*>(context);
        return read_track_conflict_memory(source.read,source.context,address,output,bytes,source.budget);
    }
};
class TrackPool {
    ReadMemory read_;void* context_;uint64_t address_,table_{};
    TrackConflictReadBudget& budget_;
    std::array<unsigned char,48> header_{};std::vector<uint64_t> blocks_;
    uint32_t shift_{},mask_{};
public:
    TrackPool(ReadMemory read,void* context,uint64_t address,TrackConflictReadBudget& budget)
        :read_(read),context_(context),address_(address),budget_(budget){}
    bool load(){
        if(!read_(context_,address_,header_.data(),header_.size()))return false;
        shift_=field<uint32_t>(header_.data(),4);mask_=field<uint32_t>(header_.data(),16);
        const auto size=field<uint32_t>(header_.data(),8);
        table_=field<uint64_t>(header_.data(),24);
        const auto end=field<uint64_t>(header_.data(),32),cap=field<uint64_t>(header_.data(),40);
        if(!shift_||shift_>16||size!=(1u<<shift_)||mask_!=size-1||end<table_||cap<end||
           cap-table_>8192||(end-table_)%8||(cap-table_)%8||((end-table_)/8)*size>1048576||
           (cap&&(!pointer(table_)||cap>=0x7fffffff0000ULL)))return false;
        if(!budget_.available(static_cast<size_t>(end-table_)))return false;
        blocks_.resize(static_cast<size_t>((end-table_)/8));
        return blocks_.empty()||read_(context_,table_,blocks_.data(),blocks_.size()*8);
    }
    bool resolve(uint64_t id,uint64_t& address){
        if(!budget_.available())return false;
        if(id>>48!=1)return false;
        const auto index=(id>>16)&0xffffffffULL,block=index>>shift_;
        constexpr size_t stride=0x4e8;
        if(block>=blocks_.size()||!pointer(blocks_[block])||
           blocks_[block]>0x7fffffff0000ULL-size_t(mask_+1)*stride)return false;
        address=blocks_[block]+(index&mask_)*stride;
        return identity(id,address);
    }
    bool identity(uint64_t id,uint64_t address){
        uint64_t actual{};return read_(context_,address,&actual,sizeof actual)&&actual==id;
    }
    bool stable(){
        if(!budget_.available())return false;
        // The table is bounded to 1024 block pointers. Its readback does not
        // need another heap allocation after the batch has consumed its quota.
        std::array<unsigned char,48> after{};std::array<uint64_t,1024> blocks;
        return read_(context_,address_,after.data(),after.size())&&after==header_&&
            (blocks_.empty()||(read_(context_,table_,blocks.data(),blocks_.size()*8)&&
                std::equal(blocks_.begin(),blocks_.end(),blocks.begin())))&&
            read_(context_,address_,after.data(),after.size())&&after==header_;
    }
};
bool same_root(ReadMemory read,void* context,const LiveState& state){
    LiveState current{};return resolve_live_state(read,context,state.module_base,true,state.profile,current)&&current==state;
}
}
bool read_track_conflicts(ReadMemory read,void* context,const LiveState& state,bool recognized,
                         std::span<const uint64_t> ids,TrackConflicts& out,
                         TrackConflictReadBudget* suppliedBudget) noexcept {
    out={};
    if(!read||!recognized||state.profile!=LiveStateProfile::Windows119||ids.empty()||
       ids.size()>NIMBY_TRACK_CONFLICTS_MAX_TRACKS)return false;
    try{
        TrackConflictReadBudget unlimited;auto& budget=suppliedBudget?*suppliedBudget:unlimited;
        if(!budget.available())return false;
        for(auto id:ids)if(!budget.available()||id>>48!=1)return false;
        BudgetedReader reader{read,context,budget};
        const auto checkedRead=BudgetedReader::copy;auto* checkedContext=&reader;
        if(!same_root(checkedRead,checkedContext,state))return false;
        TrackPool pool(checkedRead,checkedContext,state.database,budget);if(!pool.load())return false;
        if(!budget.available())return false;
        TrackConflicts result;result.states.reserve(ids.size());
        std::unordered_set<uint64_t> requested;
        std::unordered_map<uint64_t,uint64_t> sources;
        struct Reference {uint64_t address{};bool valid{};};
        std::unordered_map<uint64_t,Reference> references;
        const auto validReference=[&](uint64_t id){
            if(!budget.available())return false;
            const auto known=references.find(id);if(known!=references.end())return known->second.valid;
            uint64_t address{};const bool valid=pool.resolve(id,address);
            if(!budget.available())return false;
            references.emplace(id,Reference{address,valid});return valid;
        };
        size_t remainingRows=NIMBY_TRACK_CONFLICTS_MAX_ROWS;
        TrackConflictDecodeWorkspace workspace;std::vector<NimbyTrackConflict> rows;
        for(auto id:ids){
            if(!budget.available())return false;
            if(!requested.insert(id).second)continue;
            if(!budget.available())return false;
            uint64_t address{};rows.clear();
            auto reason=TrackConflictReason::TrackUnavailable;
            if(pool.resolve(id,address)){
                if(!budget.available())return false;
                sources.emplace(id,address);
                reason=decode_track_conflicts(read,context,id,address,validReference,remainingRows,rows,workspace,&budget);
            }
            if(!budget.available())return false;
            const bool complete=reason==TrackConflictReason::Complete;
            result.states.push_back({id,complete?NIMBY_TRACK_CONFLICTS_VALID:0,static_cast<uint32_t>(reason)});
            if(complete){if(!budget.available())return false;result.rows.insert(result.rows.end(),rows.begin(),rows.end());}
        }
        // Recheck referenced incarnations after the whole batch. A stale
        // conflict target invalidates its source list, never unrelated lists.
        std::unordered_set<uint64_t> invalidSources;
        for(auto& status:result.states)if(status.flags&NIMBY_TRACK_CONFLICTS_VALID){
            if(!budget.available())return false;
            if(!pool.identity(status.track_id,sources.at(status.track_id))){
                if(!budget.available())return false;
                invalidSources.insert(status.track_id);status.flags=0;status.reason=NIMBY_TRACK_CONFLICTS_UNSTABLE;
            }
        }
        for(auto& [id,reference]:references){
            if(!budget.available())return false;
            if(reference.valid&&!pool.identity(id,reference.address))reference.valid=false;
        }
        for(const auto& row:result.rows){
            if(!budget.available())return false;
            if(!references.at(row.other_track_id).valid)invalidSources.insert(row.track_id);
        }
        for(auto& status:result.states){
            if(!budget.available())return false;
            if(status.flags&&invalidSources.contains(status.track_id)){
                status.flags=0;status.reason=NIMBY_TRACK_CONFLICTS_REFERENCE_UNAVAILABLE;
            }
        }
        if(!budget.available())return false;
        // No allocation in the filter. Stop iteration as soon as a cooperative
        // budget denies more work, then discard the whole partial result.
        size_t kept=0;
        for(const auto& row:result.rows){
            if(!budget.available())return false;
            if(!invalidSources.contains(row.track_id))result.rows[kept++]=row;
        }
        result.rows.resize(kept);
        if(!pool.stable()||!same_root(checkedRead,checkedContext,state)||!budget.available())return false;
        out=std::move(result);return true;
    }catch(...){return false;}
}
}
