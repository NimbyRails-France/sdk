#pragma once
#include <platform/windows/runtime/signal_route.h>
#include <limits>

namespace nimby::windows::automatic {
inline constexpr size_t routeCopyReasonCount=static_cast<size_t>(RouteCopyReason::Count);
inline const char* routeCopyReasonName(RouteCopyReason reason) noexcept {
    switch(reason){
        case RouteCopyReason::Ready:return "ready";
        case RouteCopyReason::NoSelectedBoundary:return "no_selected_boundary";
        case RouteCopyReason::NoScan:return "no_scan";
        case RouteCopyReason::InvalidatedScan:return "invalidated_scan";
        case RouteCopyReason::Expired:return "expired";
        case RouteCopyReason::CaptureClockUnqualified:return "capture_clock_unqualified";
        case RouteCopyReason::PathUnqualified:return "path_unqualified";
        case RouteCopyReason::TopologyUnqualified:return "topology_unqualified";
        case RouteCopyReason::HeadUnqualified:return "head_unqualified";
        case RouteCopyReason::BoundaryUnqualified:return "boundary_unqualified";
        case RouteCopyReason::ReadbackUnqualified:return "readback_unqualified";
        default:return "unknown";
    }
}
struct RouteDiagnosticSample {
    uint64_t train{};RouteCopyReason reason=RouteCopyReason::NoScan;
    bool operator==(const RouteDiagnosticSample&)const=default;
};
// Fixed storage only. Samples are whole-route qualification failures, not a
// claim about a particular signal. Busy slots must never donate an identity.
struct RouteBatchDiagnostic {
    uint32_t status{},requestedSignals{},blocks{},busySlots{},rows{},sections{};
    std::array<uint32_t,routeCopyReasonCount> reasons{};
    std::array<RouteDiagnosticSample,4> samples{};
    uint32_t sampleCount{};
    void note(RouteCopyReason reason,uint64_t lockedTrain) noexcept {
        const auto index=static_cast<size_t>(reason);if(index>=reasons.size())return;
        ++reasons[index];
        if(reason==RouteCopyReason::Ready||reason==RouteCopyReason::NoSelectedBoundary||
           sampleCount==samples.size())return;
        samples[sampleCount++]={lockedTrain,reason};
    }
    bool operator==(const RouteBatchDiagnostic&)const=default;
};
// Observation-worker policy. The window retains brief failures during the
// throttle interval, even when the next batch has already recovered. Native
// scan callbacks never touch it or format text. No allocation is required.
class RouteDiagnosticWindow {
    RouteBatchDiagnostic last_{};
    uint64_t lastAt_{};bool emitted_=false,pending_=false;
    static void add(uint64_t& total,uint64_t value) noexcept {
        total+=std::min(value,std::numeric_limits<uint64_t>::max()-total);
    }
public:
    uint64_t batches{},busySlots{},failedBatches{};
    std::array<uint64_t,routeCopyReasonCount> reasons{};
    std::array<RouteDiagnosticSample,4> samples{};uint32_t sampleCount{};
    bool observe(const RouteBatchDiagnostic& batch,uint64_t now) noexcept {
        add(batches,1);add(busySlots,batch.busySlots);add(failedBatches,batch.status!=NIMBY_OK);
        for(size_t i=0;i<reasons.size();++i)add(reasons[i],batch.reasons[i]);
        for(size_t i=0;i<std::min(size_t{batch.sampleCount},batch.samples.size())&&sampleCount<samples.size();++i){
            const auto& value=batch.samples[i];
            if(std::find(samples.begin(),samples.begin()+sampleCount,value)==samples.begin()+sampleCount)
                samples[sampleCount++]=value;
        }
        pending_|=!emitted_||batch!=last_;
        if(emitted_&&now>=lastAt_&&now-lastAt_<30000&&(!pending_||now-lastAt_<1000))return false;
        last_=batch;lastAt_=now;emitted_=true;pending_=false;return true;
    }
    // Called after the worker attempts to emit; never repeatedly retry a failed
    // log sink within the throttle interval or carry old samples indefinitely.
    void clearWindow() noexcept {
        batches=busySlots=failedBatches=0;reasons.fill(0);samples.fill({});sampleCount=0;
    }
};
}
