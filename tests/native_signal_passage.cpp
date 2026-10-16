#include <engine/native_signal_passage.h>
#include <array>
#include <cstdio>
#include <cstring>
#include <limits>

using namespace nimby::engine::native_passage;
namespace {
constexpr uint64_t train=0x5000000010001,signal=0x8000000010001;
constexpr uint64_t track=0x1000000010001,otherTrack=0x1000000020001;
Observation fixture() {
    return {train,train,{signal,track,.5,1,NIMBY_SIGNAL_PATH,1},
        {track,.5,1,100,1},{track,.6,1,110,1},90,true,true,true,true,true};
}
bool matches(const Observation& value,Status status,Reason reason) {
    const auto actual=assess(value);return actual.status==status&&actual.reason==reason;
}
}
#define CHECK(x) do{if(!(x)){std::fprintf(stderr,"FAIL line %d: %s\n",__LINE__,#x);return 1;}}while(false)
int main() {
    CHECK(matches(fixture(),Status::Crossed,Reason::CrossedSource));
    {auto f=fixture();f.source.direction=f.candidate.direction=f.finalPresence.direction=-1;
     f.finalPresence.fraction=.4;CHECK(matches(f,Status::Crossed,Reason::CrossedSource));}
    {auto f=fixture();f.beforeHead=f.candidate.head;CHECK(assess(f).status==Status::Crossed);}
    {auto f=fixture();f.finalPresence=f.candidate;CHECK(matches(f,Status::AtBoundary,Reason::OnSourceBoundary));}
    {auto f=fixture();f.beforeHead=f.candidate.head;f.finalPresence=f.candidate;
     // Permission at rest is not entry, even when a pass_by notification exists.
     CHECK(assess(f).status==Status::AtBoundary);}
    {auto f=fixture();f.finalPresence.head=f.candidate.head+headToleranceMeters*.5;
     f.finalPresence.fraction=f.source.fraction+5e-9;
     CHECK(matches(f,Status::AtBoundary,Reason::OnSourceBoundary));}
    {auto f=fixture();f.finalPresence={track,.4,1,90,1};
     // An integrator may advance, then collision handling restores this state.
     CHECK(matches(f,Status::BeforeSource,Reason::StillBeforeSource));}
    {auto f=fixture();f.source.direction=f.candidate.direction=f.finalPresence.direction=-1;
     f.finalPresence.fraction=.6;f.finalPresence.head=95;
     CHECK(matches(f,Status::BeforeSource,Reason::StillBeforeSource));}
    for(const int direction:{1,-1}) {
        auto f=fixture();f.source.direction=f.candidate.direction=f.finalPresence.direction=direction;
        f.finalPresence=f.candidate;
        f.finalPresence.fraction=std::nextafter(.5,direction==1?1.:0.);
        CHECK(assess(f).status==Status::AtBoundary);
        f.finalPresence.head=std::nextafter(100.,101.);
        CHECK(assess(f).status==Status::AtBoundary);
        // A metric advance inconsistent with a rounding-only fraction is unknown.
        f.finalPresence.head=110;CHECK(matches(f,Status::Unavailable,Reason::HeadFractionDisagreement));
    }
    for(const double rounding:{-fractionTolerance*.5,fractionTolerance*.5}) {
        auto f=fixture();f.candidate.fraction+=rounding;CHECK(assess(f).status==Status::Crossed);
        f.finalPresence=f.candidate;CHECK(assess(f).status==Status::AtBoundary);
    }
    {auto f=fixture();f.candidate.fraction+=fractionTolerance*2;
     CHECK(matches(f,Status::Unavailable,Reason::CandidateNotAtSource));}
    for(unsigned field=0;field<5;++field) {
        auto f=fixture();
        if(field==0)f.readComplete=false;
        if(field==1)f.candidatesComplete=false;
        if(field==2)f.pathUnchanged=false;
        if(field==3)f.sourceUnchanged=false;
        if(field==4)f.finalPresenceMatches=false;
        constexpr std::array reasons{Reason::ReadIncomplete,Reason::CandidatesIncomplete,Reason::PathChanged,
            Reason::SourceChanged,Reason::FinalPresenceMismatch};
        CHECK(matches(f,Status::Unavailable,reasons[field]));
    }
    CHECK(matches(Observation{},Status::Unavailable,Reason::ReadIncomplete));
    {auto f=fixture();f.trainAfter++;CHECK(matches(f,Status::Unavailable,Reason::TrainIdentityChanged));}
    {auto f=fixture();f.trainAfter^=uint64_t{1}<<32;
     // Complete train generations are compared, never ordinal-masked.
     CHECK(matches(f,Status::Unavailable,Reason::TrainIdentityChanged));}
    for(const uint64_t wrong:{uint64_t{0},track,signal,std::numeric_limits<uint64_t>::max()}) {
        auto f=fixture();f.trainBefore=wrong;CHECK(matches(f,Status::Unavailable,Reason::InvalidTrainIdentity));
        f=fixture();f.trainAfter=wrong;CHECK(matches(f,Status::Unavailable,Reason::InvalidTrainIdentity));
    }
    {auto f=fixture();f.source.signal=train;CHECK(matches(f,Status::Unavailable,Reason::InvalidSignalIdentity));}
    for(unsigned field=0;field<3;++field) {
        auto f=fixture();
        if(field==0)f.source.track=0;
        if(field==1)f.candidate.track=train;
        if(field==2)f.finalPresence.track=signal;
        CHECK(matches(f,Status::Unavailable,Reason::InvalidTrackIdentity));
    }
    for(unsigned field=0;field<3;++field) {
        auto f=fixture();
        if(field==0)f.source.track=otherTrack;
        if(field==1)f.candidate.track=otherTrack;
        if(field==2)f.finalPresence.track=otherTrack;
        CHECK(matches(f,Status::Unavailable,Reason::TrackTransition));
    }
    {auto f=fixture();f.finalPresence.track=track+1;
     // A recycled/alias track is not the actual source track.
     CHECK(matches(f,Status::Unavailable,Reason::TrackTransition));}
    for(const uint32_t kind:{uint32_t{NIMBY_SIGNAL_MARKER},uint32_t{NIMBY_SIGNAL_BALISE},uint32_t{0},uint32_t{99}}) {
        auto f=fixture();f.source.kind=kind;CHECK(matches(f,Status::Unavailable,Reason::WrongSignalKind));
    }
    for(unsigned field=0;field<3;++field)for(const int direction:{0,2,std::numeric_limits<int>::min()}) {
        auto f=fixture();
        if(field==0)f.source.direction=direction;
        if(field==1)f.candidate.direction=direction;
        if(field==2)f.finalPresence.direction=direction;
        CHECK(matches(f,Status::Unavailable,Reason::InvalidDirection));
    }
    for(unsigned field=0;field<3;++field) {
        auto f=fixture();
        if(field==0)f.source.direction=-1;
        if(field==1)f.candidate.direction=-1;
        if(field==2)f.finalPresence.direction=-1;
        CHECK(matches(f,Status::Unavailable,Reason::DirectionMismatch));
    }
    for(unsigned field=0;field<3;++field) {
        auto f=fixture();
        // At fraction .5 opposite frames can appear to touch the same source;
        // equal raw directions must not hide that mismatch and certify entry.
        if(field==0)f.source.orientation=2;
        if(field==1)f.candidate.orientation=2;
        if(field==2)f.finalPresence.orientation=2;
        CHECK(matches(f,Status::Unavailable,Reason::OrientationMismatch));
    }
    for(const int orientation:{0,1,2}) {
        auto f=fixture();f.source.orientation=f.candidate.orientation=f.finalPresence.orientation=orientation;
        CHECK(matches(f,Status::Crossed,Reason::CrossedSource));
    }
    for(unsigned field=0;field<3;++field)for(const double value:{-.001,1.001,
        std::numeric_limits<double>::quiet_NaN(),std::numeric_limits<double>::infinity(),
        -std::numeric_limits<double>::infinity(),std::numeric_limits<double>::max()}) {
        auto f=fixture();
        if(field==0)f.source.fraction=value;
        if(field==1)f.candidate.fraction=value;
        if(field==2)f.finalPresence.fraction=value;
        CHECK(matches(f,Status::Unavailable,Reason::InvalidFraction));
    }
    for(unsigned field=0;field<3;++field)for(const double value:{-1.,std::numeric_limits<double>::lowest(),
        std::numeric_limits<double>::quiet_NaN(),std::numeric_limits<double>::infinity()}) {
        auto f=fixture();
        if(field==0)f.beforeHead=value;
        if(field==1)f.candidate.head=value;
        if(field==2)f.finalPresence.head=value;
        CHECK(matches(f,Status::Unavailable,Reason::InvalidHead));
    }
    for(unsigned field=0;field<3;++field) {
        auto f=fixture();
        if(field==0)f.beforeHead=std::numeric_limits<double>::max();
        if(field==1)f.candidate.head=std::numeric_limits<double>::max();
        if(field==2)f.finalPresence.head=std::numeric_limits<double>::max();
        CHECK(matches(f,Status::Unavailable,Reason::HeadPrecisionUnavailable));
    }
    {auto f=fixture();f.beforeHead=101;CHECK(matches(f,Status::Unavailable,Reason::BeforeHeadAfterCandidate));}
    {auto f=fixture();f.finalPresence.head=89;CHECK(matches(f,Status::Unavailable,Reason::FinalHeadBeforeStep));}
    {auto f=fixture();f.finalPresence.fraction=.4;CHECK(matches(f,Status::Unavailable,Reason::HeadFractionDisagreement));}
    {auto f=fixture();f.finalPresence.head=95;CHECK(matches(f,Status::Unavailable,Reason::HeadFractionDisagreement));}
    {auto f=fixture();f.finalPresence.head=100;CHECK(matches(f,Status::Unavailable,Reason::HeadFractionDisagreement));}
    {auto f=fixture();f.source.fraction=f.candidate.fraction=1.;f.finalPresence.fraction=1.;
     CHECK(matches(f,Status::Unavailable,Reason::HeadFractionDisagreement));}
    {auto f=fixture();f.source.fraction=f.candidate.fraction=0.;f.finalPresence.fraction=.1;
     CHECK(assess(f).status==Status::Crossed);}
    {auto f=fixture();f.source.fraction=f.candidate.fraction=1.;f.finalPresence.fraction=.9;
     f.source.direction=f.candidate.direction=f.finalPresence.direction=-1;
     CHECK(assess(f).status==Status::Crossed);}
    for(const auto status:{Status::Crossed,Status::AtBoundary,Status::BeforeSource,Status::Unavailable})
        CHECK(std::strlen(statusName(status))&&std::strcmp(statusName(status),"invalid_status")!=0);
    for(unsigned value=0;value<static_cast<unsigned>(Reason::Count);++value)
        CHECK(std::strlen(reasonName(static_cast<Reason>(value)))&&
            std::strcmp(reasonName(static_cast<Reason>(value)),"invalid_reason")!=0);
    CHECK(std::strcmp(statusName(static_cast<Status>(99)),"invalid_status")==0);
    CHECK(std::strcmp(reasonName(static_cast<Reason>(99)),"invalid_reason")==0);
    CHECK(std::strcmp(reasonName(Reason::Count),"invalid_reason")==0);
    std::puts("native_signal_passage: local post-step qualification passed");
    return 0;
}
