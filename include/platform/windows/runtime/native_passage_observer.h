#pragma once
#include <engine/native_signal_passage.h>
#include <algorithm>
#include <array>
#include <cstring>
#include <span>

namespace nimby::windows::automatic {
// Private layouts of the hash-pinned Windows119 profile. These records belong
// to ONE synchronous 0x448710 call. Native pointers never enter the mailbox.
struct PassageNativePresence {
    int32_t index{},padding{};uint64_t track{};double fraction{};
    int8_t direction{},orientation{};std::array<unsigned char,6> reserved{};double head{};
};
static_assert(sizeof(PassageNativePresence)==0x28);
class NativePassageTransaction {
    template<class T> static T field(const auto& bytes,size_t offset)noexcept {
        T value{};std::memcpy(&value,bytes.data()+offset,sizeof value);return value;
    }
    struct PathSnapshot {
        static constexpr size_t maximumTracks=512;
        std::array<unsigned char,0xc0> header{};
        std::array<uint64_t,maximumTracks> tracks{};
        size_t count{};
        template<class Read> bool capture(uintptr_t motion,Read&& read)noexcept {
            if(!read(motion+0x290,header.data(),header.size())||header[0x90]!=1)return false;
            const auto begin=field<uintptr_t>(header,0xa8),end=field<uintptr_t>(header,0xb0),cap=field<uintptr_t>(header,0xb8);
            constexpr uintptr_t limit=0x7fffffff0000ULL;
            if(begin<0x10000||begin%8||end<=begin||end>=limit||cap<end||cap>=limit||
               (end-begin)%8||(cap-begin)%8||end-begin>tracks.size()*8)return false;
            count=static_cast<size_t>((end-begin)/8);
            if(!read(begin,tracks.data(),count*8))return false;
            for(size_t i=0;i<count;++i)if(tracks[i]>>48!=1)return false;
            std::array<unsigned char,0xc0> confirmed{};
            return read(motion+0x290,confirmed.data(),confirmed.size())&&confirmed==header;
        }
        template<class Read> bool matches(uintptr_t motion,Read&& read)const noexcept {
            PathSnapshot after;
            if(!after.capture(motion,read)||after.header!=header||after.count!=count)return false;
            return std::equal(tracks.begin(),tracks.begin()+count,after.tracks.begin());
        }
    };
    struct Candidate {
        uintptr_t address{};
        nimby::engine::native_passage::SourceAnchor source;
        nimby::engine::native_passage::Presence presence;
        bool readable=false;
    };
    uintptr_t trainAddress_{},motion_{};
    uint64_t train_{};
    PassageNativePresence start_{};
    PathSnapshot path_;
    bool started_=false,pathCaptured_=false,complete_=true;
    std::array<Candidate,8> candidates_{};
    size_t count_=0;
    template<class Read> static bool source(uintptr_t address,nimby::engine::native_passage::SourceAnchor& out,Read&& read)noexcept {
        if(address<0x10000||address>0x7fffffff0000ULL-0x58)return false;
        std::array<unsigned char,0x58> data{};
        if(!read(address,data.data(),data.size()))return false;
        out.signal=field<uint64_t>(data,0);out.kind=field<uint32_t>(data,0x30);
        out.track=field<uint64_t>(data,0x40);out.fraction=field<double>(data,0x48);
        // PATH stores the panel-facing direction, opposite to train travel.
        out.direction=-static_cast<int>(field<int8_t>(data,0x50));
        out.orientation=static_cast<int>(field<int8_t>(data,0x51));return true;
    }
    static auto presence(const PassageNativePresence& value)noexcept {
        return nimby::engine::native_passage::Presence{value.track,value.fraction,value.direction,value.head,value.orientation};
    }
public:
    static constexpr size_t maximumCandidates=8;
    template<class Read> NativePassageTransaction(uintptr_t train,uintptr_t motion,Read&& read)noexcept
        :trainAddress_(train),motion_(motion) {
        if(train<0x10000||train>0x7fffffff0000ULL-0x178||motion<0x10000||motion>0x7fffffff0000ULL-0x638)return;
        uint64_t dbTrain{};
        started_=read(train,&dbTrain,sizeof dbTrain)&&read(motion,&train_,sizeof train_)&&
            dbTrain==train_&&train_>>48==5&&read(motion+0x3a0,&start_,sizeof start_)&&
            start_.track>>48==1&&std::isfinite(start_.fraction)&&start_.fraction>=0&&start_.fraction<=1&&
            (start_.direction==1||start_.direction==-1);
    }
    uint64_t train()const noexcept{return train_;}
    size_t count()const noexcept{return count_;}
    bool complete()const noexcept{return complete_;}
    // pass_by can precede motion and can be followed by collision restoration.
    // Copy a candidate before dispatch; call the original dispatcher once in
    // the wrapper, then qualify only when the entire native step has returned.
    template<class Read> void candidate(uintptr_t signal,Read&& read)noexcept {
        if(count_==candidates_.size()){complete_=false;return;}
        auto& row=candidates_[count_++];row.address=signal;
        PassageNativePresence cursor{};
        row.readable=source(signal,row.source,read)&&read(motion_+0x3a0,&cursor,sizeof cursor);
        row.presence=presence(cursor);
        if(!pathCaptured_){pathCaptured_=true;complete_=path_.capture(motion_,read)&&complete_;}
    }
    template<class Read,class Emit> void finish(Read&& read,Emit&& emit)const noexcept {
        using namespace nimby::engine::native_passage;
        if(!count_)return;
        uint64_t finalTrain{},dbTrain{};PassageNativePresence final{};
        struct Published {uint64_t track;double fraction;int8_t direction,orientation;std::array<unsigned char,6> reserved;};
        static_assert(sizeof(Published)==0x18);
        Published published{};
        const bool finalRead=read(motion_,&finalTrain,sizeof finalTrain)&&read(trainAddress_,&dbTrain,sizeof dbTrain)&&
            read(motion_+0x3a0,&final,sizeof final)&&read(motion_+0xb8,&published,sizeof published);
        // The mirror is copied by 448710 after any collision rollback. Merely
        // reading the integrator's temporary Presence would certify too early.
        const bool matched=finalRead&&dbTrain==finalTrain&&published.track==final.track&&
            published.fraction==final.fraction&&published.direction==final.direction&&published.orientation==final.orientation;
        const bool samePath=pathCaptured_&&path_.matches(motion_,read);
        // Stage every result before emitting: a failure on the last source
        // cannot leave a positive row from the first half of an incomplete read.
        std::array<Observation,maximumCandidates> observations{};
        bool allRead=started_&&finalRead;
        for(size_t i=0;i<count_;++i){const auto& candidate=candidates_[i];auto& value=observations[i];
            SourceAnchor after{};const bool sourceRead=source(candidate.address,after,read);
            value={train_,finalTrain,candidate.source,candidate.presence,presence(final),start_.head,
                samePath,sourceRead&&after.signal==candidate.source.signal&&after.track==candidate.source.track&&
                    after.kind==candidate.source.kind&&after.fraction==candidate.source.fraction&&after.direction==candidate.source.direction&&
                    after.orientation==candidate.source.orientation,
                matched,started_&&finalRead&&candidate.readable&&sourceRead,complete_};
            allRead=allRead&&value.readComplete;
        }
        for(size_t i=0;i<count_;++i){auto value=observations[i];value.readComplete=value.readComplete&&allRead;
            emit(value,assess(value),start_.fraction);}
    }
};
} // namespace nimby::windows::automatic
