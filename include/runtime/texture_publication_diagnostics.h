#pragma once
#include <nimby/detail/texture_updates.h>
#include <engine/live_state.h>
#include <chrono>
#include <cstdio>
#include <cstring>

namespace nimby::runtime {
enum class TexturePublicationReason {
    None, InvalidBatch, InvalidSignal, InvalidFlags, TextureSetUnterminated, TextureSetEmpty,
    FirstPathUnterminated, FirstPathEmpty, AlternatePathUnterminated, InvalidDuration,
    InvalidAnimation, DuplicateSignal, BrokerRejected, OpenRejected, CatalogueUnreadable,
    MembershipUnreadable, SignalMissing, NumericCatalogueAmbiguous, NumericIndexOutOfRange,
    CatalogueMissing, FirstPathMissing, FirstPathAmbiguous, AlternatePathMissing,
    AlternatePathAmbiguous, AnimationCatalogueMismatch, WorldUnreadable, WorldChanged, BridgeRejected,
    AllocationFailed, InternalError
};
inline const char* texturePublicationReason(TexturePublicationReason reason) noexcept {
    using R=TexturePublicationReason;
    switch(reason){
        case R::None:return "none";
        case R::InvalidBatch:return "invalid_batch";
        case R::InvalidSignal:return "invalid_signal_id";
        case R::InvalidFlags:return "invalid_flags";
        case R::TextureSetUnterminated:return "texture_set_unterminated";
        case R::TextureSetEmpty:return "texture_set_empty";
        case R::FirstPathUnterminated:return "first_path_unterminated";
        case R::FirstPathEmpty:return "first_path_empty";
        case R::AlternatePathUnterminated:return "alternate_path_unterminated";
        case R::InvalidDuration:return "invalid_duration";
        case R::InvalidAnimation:return "invalid_animation";
        case R::DuplicateSignal:return "duplicate_signal";
        case R::BrokerRejected:return "broker_rejected";
        case R::OpenRejected:return "connection_open_rejected";
        case R::CatalogueUnreadable:return "catalogue_unreadable";
        case R::MembershipUnreadable:return "signal_membership_unreadable";
        case R::SignalMissing:return "signal_missing";
        case R::NumericCatalogueAmbiguous:return "numeric_catalogue_ambiguous";
        case R::NumericIndexOutOfRange:return "numeric_index_out_of_range";
        case R::CatalogueMissing:return "catalogue_missing";
        case R::FirstPathMissing:return "first_path_missing";
        case R::FirstPathAmbiguous:return "first_path_ambiguous";
        case R::AlternatePathMissing:return "alternate_path_missing";
        case R::AlternatePathAmbiguous:return "alternate_path_ambiguous";
        case R::AnimationCatalogueMismatch:return "animation_catalogue_mismatch";
        case R::WorldUnreadable:return "world_unreadable";
        case R::WorldChanged:return "world_changed";
        case R::BridgeRejected:return "bridge_publication_rejected";
        case R::AllocationFailed:return "allocation_failed";
        case R::InternalError:return "internal_error";
    }
    return "unspecified";
}
// Preserve the publication ABI validation, while naming the first failed check.
inline TexturePublicationReason invalidTextureUpdate(const NimbyTextureUpdate& row) noexcept {
    using R=TexturePublicationReason;
    if(row.signal>>48!=8)return R::InvalidSignal;
    if(row.flags>1)return R::InvalidFlags;
    if(!std::memchr(row.texture_set,0,sizeof row.texture_set))return R::TextureSetUnterminated;
    if(!row.texture_set[0])return R::TextureSetEmpty;
    if(!std::memchr(row.first_path,0,sizeof row.first_path))return R::FirstPathUnterminated;
    if(!std::memchr(row.alternate_path,0,sizeof row.alternate_path))return R::AlternatePathUnterminated;
    if(!(row.flags&1)&&!row.first_path[0])return R::FirstPathEmpty;
    if(row.duration_ms&&(row.duration_ms<1000||row.duration_ms>60000))return R::InvalidDuration;
    if(row.half_period_ms&&(row.half_period_ms<100||row.half_period_ms>10000||!row.alternate_path[0]||(row.flags&1)))return R::InvalidAnimation;
    return R::None;
}

// One instance per publication thread: no locks, allocation, extra process reads
// or successful-tick logging. Changing rows/reasons cannot defeat the burst cap.
class TexturePublicationDiagnostics {
public:
    using Clock=std::chrono::steady_clock;
    struct Context {
        uint32_t pid=0;uint64_t owner=0;uint32_t count=0;int64_t row=-1;
        const NimbyTextureUpdate* update=nullptr;
        const engine::LiveState* world=nullptr;
        const engine::LiveState* after=nullptr;
    };
    template<class Sink>
    void report(uint32_t status,TexturePublicationReason reason,const Context& context,
                Sink&& sink,Clock::time_point now=Clock::now()) noexcept {
        if(status==NIMBY_OK)return;
        const auto signal=context.update?context.update->signal:0;
        const bool elapsed=used_&&now-window_>=std::chrono::seconds(5);
        if(!used_||elapsed){window_=now;burst_=0;}
        const bool changed=!used_||status!=status_||reason!=reason_||context.owner!=owner_||signal!=signal_;
        if(burst_>=4||(!changed&&!elapsed&&burst_)){++suppressed_;return;}
        used_=true;status_=status;reason_=reason;owner_=context.owner;signal_=signal;++burst_;
        char set[97]{},first[161]{},alternate[161]{},message[1536]{};
        if(context.update){
            copyText(set,context.update->texture_set);
            copyText(first,context.update->first_path);
            copyText(alternate,context.update->alternate_path);
        }
        const engine::LiveState empty{};
        const auto& world=context.world?*context.world:empty;
        const auto& after=context.after?*context.after:empty;
        std::snprintf(message,sizeof message,
            "Texture publication rejected: status=%u reason=%s targetPid=%u owner=%llu count=%u row=%lld signal=%llu "
            "textureSet=\"%s\" first=\"%s\" alternate=\"%s\" flags=%u index=%u durationMs=%u halfPeriodMs=%u "
            "root=%llu database=%llu copy=%llu simulation=%llu afterRoot=%llu afterDatabase=%llu afterCopy=%llu afterSimulation=%llu suppressed=%llu",
            status,texturePublicationReason(reason),context.pid,number(context.owner),context.count,static_cast<long long>(context.row),number(signal),
            set,first,alternate,context.update?context.update->flags:0,context.update?context.update->index:0,
            context.update?context.update->duration_ms:0,context.update?context.update->half_period_ms:0,
            number(world.root),number(world.database),number(world.copy),number(world.simulation),
            number(after.root),number(after.database),number(after.copy),number(after.simulation),number(suppressed_));
        suppressed_=0;
        sink(message);
    }
private:
    static unsigned long long number(uint64_t value) noexcept {return static_cast<unsigned long long>(value);}
    template<size_t N,size_t M> static void copyText(char (&out)[N],const char (&in)[M]) noexcept {
        size_t i=0;
        for(;i+1<N&&i<M&&in[i];++i){
            const auto c=static_cast<unsigned char>(in[i]);
            out[i]=c<32||c==127||c=='"'?'?':in[i];
        }
        out[i]=0;
    }
    bool used_=false;Clock::time_point window_{};unsigned burst_=0;
    uint32_t status_=0;TexturePublicationReason reason_=TexturePublicationReason::None;
    uint64_t owner_=0,signal_=0,suppressed_=0;
};
}
