#pragma once
#include <nimby/detail/signal_routes.h>
#include <nimby/block_topology.hpp>

namespace nimby::detail {
// One targeted batch for every signal model in this consumer. The buffers are
// reused; a transient unavailable bridge clears facts instead of retaining an
// old route. Missing rows mean unobserved, never "this train cannot go there".
class SignalRoutes {
    std::vector<NimbySignalRouteObservation> rows_;
    std::vector<NimbySignalRouteSection> sections_;
    std::vector<NimbySignalRouteAnnotation> annotations_;
    size_t rowCapacity_=128,sectionCapacity_=1024,annotationCapacity_=512;
    bool directed_=false;
    static bool identity(uint64_t value,uint64_t kind)noexcept {
        return value>>48==kind&&(value&0x0000ffffffffffffULL)!=0;
    }
public:
    SignalRoutes()=default;
    SignalRoutes(std::vector<NimbySignalRouteObservation> rows,std::vector<NimbySignalRouteSection> sections)
        :rows_(std::move(rows)),sections_(std::move(sections)){index();}
    void index(){
        std::erase_if(rows_,[](const auto& row){return !valid(row);});
        if(rows_.size()>1)std::sort(rows_.begin(),rows_.end(),[](const auto& a,const auto& b){return std::tie(a.signal_id,a.blocks,a.train_id)<std::tie(b.signal_id,b.blocks,b.train_id);});
        if(sections_.size()>1){
            const auto less=[](const auto& a,const auto& b){return std::tie(a.signal_id,a.train_id)<std::tie(b.signal_id,b.train_id);};
            if(directed_)std::stable_sort(sections_.begin(),sections_.end(),less);
            else std::sort(sections_.begin(),sections_.end(),less);
        }
        if(annotations_.size()>1)std::sort(annotations_.begin(),annotations_.end(),[](const auto& a,const auto& b){return std::tie(a.signal_id,a.train_id,a.ordinal)<std::tie(b.signal_id,b.train_id,b.ordinal);});
    }
    uint32_t capture(std::span<const uint64_t> signals,uint32_t blocks,bool annotations=false){
        rows_.clear();sections_.clear();annotations_.clear();directed_=annotations;
        if(signals.empty())return NIMBY_OK;
        rows_.resize(rowCapacity_);sections_.resize(sectionCapacity_);uint32_t count=0,sectionCount=0;
        uint32_t annotationCount=0;
        if(annotations)annotations_.resize(annotationCapacity_);
        const auto status=annotations?NimbyInternal_CopySignalRoutesV2(signals.data(),static_cast<uint32_t>(signals.size()),blocks,
            rows_.data(),static_cast<uint32_t>(rows_.size()),&count,sections_.data(),static_cast<uint32_t>(sections_.size()),&sectionCount,
            annotations_.data(),static_cast<uint32_t>(annotations_.size()),&annotationCount):
            NimbyInternal_CopySignalRoutes(signals.data(),static_cast<uint32_t>(signals.size()),blocks,
            rows_.data(),static_cast<uint32_t>(rows_.size()),&count,sections_.data(),static_cast<uint32_t>(sections_.size()),&sectionCount);
        if(status==NIMBY_OK){
            if(count>rows_.size()||sectionCount>sections_.size()||annotationCount>annotations_.size()){rows_.clear();sections_.clear();annotations_.clear();return NIMBY_INVALID_BINARY;}
            rows_.resize(count);sections_.resize(sectionCount);annotations_.resize(annotationCount);index();return status;
        }
        rows_.clear();sections_.clear();annotations_.clear();
        // Grow for the NEXT observation. Repeating a native capture here would
        // multiply its read/time budget and delay every model in this consumer.
        // A failed batch supplies no partial route or fabricated clearance.
        if(status==NIMBY_BUFFER_TOO_SMALL){
            rowCapacity_=std::min(size_t{65536},rowCapacity_*2);
            sectionCapacity_=std::min(size_t{131072},sectionCapacity_*2);
            // Together with the V1 caps this stays below the 16 MiB isolated
            // RPC payload limit; growing all three arrays must not get stuck
            // permanently above that transport's budget.
            annotationCapacity_=std::min(size_t{32768},annotationCapacity_*2);
        }
        return status;
    }
    std::span<const NimbySignalRouteObservation> rows()const noexcept{return rows_;}
    bool directed()const noexcept{return directed_;}
    std::span<const NimbySignalRouteSection> sections(uint64_t signal,uint64_t train)const noexcept {
        const auto key=std::pair{signal,train};
        const auto begin=std::lower_bound(sections_.begin(),sections_.end(),key,[](const auto& row,const auto& value){return std::pair{row.signal_id,row.train_id}<value;});
        auto end=begin;while(end!=sections_.end()&&std::pair{end->signal_id,end->train_id}==key)++end;
        return {begin,end};
    }
    std::span<const NimbySignalRouteAnnotation> annotations(uint64_t signal,uint64_t train)const noexcept {
        const auto key=std::pair{signal,train};
        const auto begin=std::lower_bound(annotations_.begin(),annotations_.end(),key,[](const auto& row,const auto& value){return std::pair{row.signal_id,row.train_id}<value;});
        auto end=begin;while(end!=annotations_.end()&&std::pair{end->signal_id,end->train_id}==key)++end;
        return {begin,end};
    }
    const NimbySignalRouteObservation* at(uint64_t signal,uint64_t train)const noexcept {
        const auto begin=std::lower_bound(rows_.begin(),rows_.end(),signal,[](const auto& row,uint64_t id){return row.signal_id<id;});
        for(auto i=begin;i!=rows_.end()&&i->signal_id==signal;++i)if(i->train_id==train)return &*i;
        return nullptr;
    }
    static bool valid(const NimbySignalRouteObservation& row)noexcept {
        return identity(row.train_id,5)&&identity(row.signal_id,8)&&(!row.next_signal_id||identity(row.next_signal_id,8))&&
            row.route_revision&&identity(row.head_track_id,1)&&std::isfinite(row.head_fraction)&&row.head_fraction>=0&&row.head_fraction<=1&&
            (row.head_direction==1||row.head_direction==-1)&&row.blocks>=1&&row.blocks<=16&&
            std::isfinite(row.distance_m)&&row.distance_m>=0&&!row.reserved&&(row.flags&NIMBY_SIGNAL_ROUTE_ORDERED)&&
            identity(row.source_track_id,1)&&std::isfinite(row.source_fraction)&&row.source_fraction>=0&&row.source_fraction<=1&&
            (row.source_direction==1||row.source_direction==-1)&&row.source_kind==NIMBY_SIGNAL_PATH&&
            !(row.flags&~63u)&&!((row.flags&NIMBY_SIGNAL_ROUTE_NATIVE_CLEAR)&&(row.flags&NIMBY_SIGNAL_ROUTE_NATIVE_BLOCKED))&&
            !((row.flags&NIMBY_SIGNAL_ROUTE_CROSSING_OCCUPIED)&&(row.flags&NIMBY_SIGNAL_ROUTE_CROSSING_RESERVED))&&
            !((row.flags&NIMBY_SIGNAL_ROUTE_NATIVE_CLEAR)&&(row.flags&(NIMBY_SIGNAL_ROUTE_CROSSING_OCCUPIED|NIMBY_SIGNAL_ROUTE_CROSSING_RESERVED)))&&
            ((row.flags&NIMBY_SIGNAL_ROUTE_BLOCK_COMPLETE)?
                row.next_signal_id&&identity(row.next_track_id,1)&&std::isfinite(row.next_fraction)&&row.next_fraction>=0&&row.next_fraction<=1&&
                    (row.next_direction==1||row.next_direction==-1)&&row.next_kind==NIMBY_SIGNAL_PATH:
                !row.next_signal_id&&!row.next_track_id&&row.next_fraction==0&&!row.next_direction&&!row.next_kind);
    }
    // The native route is captured after the immutable snapshot. A moved,
    // reversed or replaced source must not donate an approach to that old
    // snapshot, even when its downstream block would be rejected separately.
    static bool sourceMatches(const NimbySignalRouteObservation& row,const Signal& source)noexcept {
        return valid(row)&&row.signal_id==source.getId()&&source.getKind()==NIMBY_SIGNAL_PATH&&
            row.source_track_id==source.getTrackId()&&row.source_direction==source.getForwardDirection()&&
            row.source_fraction==source.getFraction();
    }
    std::optional<SignalBlock> block(const NimbySignalRouteObservation& row,const Signal& source,const Signal* next=nullptr)const {
        if(!sourceMatches(row,source))return {};
        if(row.flags&NIMBY_SIGNAL_ROUTE_BLOCK_COMPLETE){
            if(!next||row.next_signal_id!=next->getId()||next->getKind()!=NIMBY_SIGNAL_PATH||
               row.next_track_id!=next->getTrackId()||row.next_direction!=next->getForwardDirection()||
               row.next_fraction!=next->getFraction())return {};
        }
        const auto key=std::pair{row.signal_id,row.train_id};
        const auto begin=std::lower_bound(sections_.begin(),sections_.end(),key,[](const auto& part,const auto& id){return std::pair{part.signal_id,part.train_id}<id;});
        SignalBlock block;block.signal=row.signal_id;block.nextSignal=row.next_signal_id;
        block.entry=BlockEntry{source.getTrackId(),source.getFraction(),source.getForwardDirection()};
        block.issue=(row.flags&NIMBY_SIGNAL_ROUTE_BLOCK_COMPLETE)?BlockBoundaryIssue::None:BlockBoundaryIssue::UnresolvedRoute;
        block.traceStop=block.hasBoundary()?SignalTraceStop::SignalReached:SignalTraceStop::TrackLimit;
        for(auto i=begin;i!=sections_.end()&&std::pair{i->signal_id,i->train_id}==key;++i){
            if(block.sections.size()==4096||!identity(i->track_id,1)||!std::isfinite(i->begin_fraction)||!std::isfinite(i->end_fraction)||
               i->begin_fraction<0||i->begin_fraction>1||i->end_fraction<0||i->end_fraction>1||
               (directed_?i->begin_fraction==i->end_fraction:i->begin_fraction>=i->end_fraction))return {};
            block.sections.push_back({i->track_id,std::min(i->begin_fraction,i->end_fraction),std::max(i->begin_fraction,i->end_fraction)});
        }
        if(block.sections.empty())return {};
        // The qualified source anchor also handles a signal at the exact track
        // endpoint: its outgoing section can start on the following track.
        return block;
    }
};
}
