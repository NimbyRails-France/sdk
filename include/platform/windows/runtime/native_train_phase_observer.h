#pragma once
#include <algorithm>
#include <array>
#include <cmath>
#include <cstring>
#include <cstdint>

namespace nimby::windows::automatic {
// Diagnostic values copied after the hash-pinned 44D6B0 train phase. Nothing
// here certifies an immutable world, complete consist or rear clearance.
// Reading the same bytes twice does not establish those certificates.
enum class PhaseCaptureReason : uint32_t {
    Ready, InvalidAddress, Unreadable, TrainChanged, InvalidPresence,
    InvalidLength, InvalidHistory, HistoryCapacity, ChangedReadback,
    InvalidComposition, CompositionCapacity, InvalidAttachments,
    AttachmentsCapacity, InvalidCoupling, Count
};
inline const char* phaseCaptureReasonName(PhaseCaptureReason reason) noexcept {
    switch(reason) {
    case PhaseCaptureReason::Ready:return "local_readback_ready";
    case PhaseCaptureReason::InvalidAddress:return "invalid_address";
    case PhaseCaptureReason::Unreadable:return "unreadable";
    case PhaseCaptureReason::TrainChanged:return "train_identity_changed";
    case PhaseCaptureReason::InvalidPresence:return "invalid_presence";
    case PhaseCaptureReason::InvalidLength:return "invalid_native_length";
    case PhaseCaptureReason::InvalidHistory:return "invalid_history";
    case PhaseCaptureReason::HistoryCapacity:return "history_capacity";
    case PhaseCaptureReason::ChangedReadback:return "changed_readback";
    case PhaseCaptureReason::InvalidComposition:return "invalid_retained_composition";
    case PhaseCaptureReason::CompositionCapacity:return "retained_composition_capacity";
    case PhaseCaptureReason::InvalidAttachments:return "invalid_attachment_records";
    case PhaseCaptureReason::AttachmentsCapacity:return "attachment_records_capacity";
    case PhaseCaptureReason::InvalidCoupling:return "invalid_coupling_identity";
    default:return "unknown";
    }
}
struct TrainPhaseSample {
    uint64_t train{},track{},historyFingerprint{};
    double fraction{};
    float nativeLength{};
    uint32_t historyCount{};
    int8_t direction{},orientation{};
    bool active{},coupled{};
    // Local native lists are diagnostic only. In particular attachmentCount
    // can omit a child whose parent flag was changed by a command this phase.
    // Fingerprints detect changes; they are never completeness/revision proofs.
    uint64_t parent{},compositionFingerprint{},attachmentFingerprint{};
    int64_t couplingOrder{};
    uint32_t retainedCars{},ownRetainedCars{},foreignRetainedCars{},unownedRetainedCars{},attachmentCount{};
    bool couplingReversed{};
};
struct TrainPhaseCapture {
    PhaseCaptureReason reason=PhaseCaptureReason::Unreadable;
    TrainPhaseSample sample;
};
namespace train_phase_detail {
template<class T> T field(const auto& bytes,size_t offset) noexcept {
    T value{};std::memcpy(&value,bytes.data()+offset,sizeof value);return value;
}
inline constexpr uintptr_t addressLimit=0x7fffffff0000ULL;
inline bool address(uintptr_t value,size_t size) noexcept {
    return value>=0x10000&&value%8==0&&size<addressLimit&&value<addressLimit-size;
}
}
// Fixed storage; no allocations, native invocation or retained native pointer.
// Callers impose a shared read/time budget. A refused read has no retry loop.
template<class Read> TrainPhaseCapture captureTrainPhase(uintptr_t motion,uint64_t train,Read&& read) noexcept {
    using namespace train_phase_detail;
    const auto failed=[&](PhaseCaptureReason reason){return TrainPhaseCapture{reason,{train}};};
    if(train>>48!=5)return failed(PhaseCaptureReason::TrainChanged);
    if(!address(motion,0x638))return failed(PhaseCaptureReason::InvalidAddress);
    struct Header {
        uint64_t id{};
        float length{};
        std::array<unsigned char,0x18> published{};
        std::array<uintptr_t,3> history{};
        std::array<uintptr_t,3> composition{},attachments{};
        uint64_t parent{};
        int64_t couplingOrder{};
        uint8_t couplingReversed{};
        uint8_t active{},coupled{};
        bool operator==(const Header&) const = default;
    };
    const auto header=[&](Header& out){
        return read(motion,&out.id,sizeof out.id)&&read(motion+0x40,&out.length,sizeof out.length)&&
            read(motion+0xb8,out.published.data(),out.published.size())&&
            read(motion+0xe0,out.history.data(),sizeof out.history)&&
            read(motion+0x1d0,&out.active,sizeof out.active)&&read(motion+0x1f0,&out.coupled,sizeof out.coupled)&&
            read(motion+8,out.composition.data(),sizeof out.composition)&&
            read(motion+0x88,out.attachments.data(),sizeof out.attachments)&&
            read(motion+0x1d8,&out.parent,sizeof out.parent)&&
            read(motion+0x1e0,&out.couplingOrder,sizeof out.couplingOrder)&&
            read(motion+0x1e8,&out.couplingReversed,sizeof out.couplingReversed);
    };
    Header before,after;
    if(!header(before))return failed(PhaseCaptureReason::Unreadable);
    if(before.id!=train)return failed(PhaseCaptureReason::TrainChanged);
    TrainPhaseSample sample{train,field<uint64_t>(before.published,0),0,
        field<double>(before.published,8),before.length,0,
        field<int8_t>(before.published,0x10),field<int8_t>(before.published,0x11),
        before.active!=0,before.coupled!=0};
    if(sample.track>>48!=1||!std::isfinite(sample.fraction)||sample.fraction<0||sample.fraction>1||
       (sample.direction!=1&&sample.direction!=-1))return failed(PhaseCaptureReason::InvalidPresence);
    if(!std::isfinite(before.length)||before.length<=0)return failed(PhaseCaptureReason::InvalidLength);
    // An unattached Motion may keep old parent/order bytes after unhitch. Copy
    // them as-is; only an active attached flag requires a valid distinct parent.
    if(sample.coupled&&(before.parent>>48!=5||!(before.parent&0x0000ffffffffffffULL)||before.parent==train))
        return failed(PhaseCaptureReason::InvalidCoupling);
    sample.parent=before.parent;sample.couplingOrder=before.couplingOrder;
    sample.couplingReversed=before.couplingReversed!=0;
    const auto vectorCount=[](const auto& vector,size_t stride,size_t& count) noexcept {
        const auto [begin,end,cap]=vector;
        if(end<begin||cap<end||(end-begin)%stride||(cap-begin)%stride||
           cap-begin>4096*stride||(cap&&!address(begin,cap-begin)))return false;
        count=(end-begin)/stride;return true;
    };
    constexpr size_t maximumCars=128,maximumAttachments=64;
    size_t carCount{},attachmentCount{};
    if(!vectorCount(before.composition,0x20,carCount))return failed(PhaseCaptureReason::InvalidComposition);
    if(carCount>maximumCars)return failed(PhaseCaptureReason::CompositionCapacity);
    if(!vectorCount(before.attachments,0x20,attachmentCount))return failed(PhaseCaptureReason::InvalidAttachments);
    if(attachmentCount>maximumAttachments)return failed(PhaseCaptureReason::AttachmentsCapacity);
    // Initialize only the prefixes supplied by successful reads. Clearing the
    // full 16 KiB scratch for every small consist would be wasted per-phase work.
    std::array<std::array<unsigned char,0x20>,maximumCars> cars,carsAgain;
    std::array<std::array<unsigned char,0x20>,maximumAttachments> attachments,attachmentsAgain;
    const size_t carBytes=carCount*0x20,attachmentBytes=attachmentCount*0x20;
    if((carBytes&&!read(before.composition[0],cars.data(),carBytes))||
       (attachmentBytes&&!read(before.attachments[0],attachments.data(),attachmentBytes)))
        return failed(PhaseCaptureReason::Unreadable);
    const auto hash=[](uint64_t& value,const auto& row) noexcept {
        for(auto byte:row){value^=byte;value*=0x100000001b3ULL;}
    };
    sample.retainedCars=static_cast<uint32_t>(carCount);
    sample.attachmentCount=static_cast<uint32_t>(attachmentCount);
    sample.compositionFingerprint=sample.attachmentFingerprint=0xcbf29ce484222325ULL;
    for(size_t i=0;i<carCount;++i){
        const auto owner=field<uint64_t>(cars[i],8);
        if(!owner)++sample.unownedRetainedCars;
        else if(owner==train)++sample.ownRetainedCars;
        else {
            if(owner>>48!=5||!(owner&0x0000ffffffffffffULL))return failed(PhaseCaptureReason::InvalidComposition);
            ++sample.foreignRetainedCars;
        }
        hash(sample.compositionFingerprint,cars[i]);
    }
    for(size_t i=0;i<attachmentCount;++i){
        const auto child=field<uint64_t>(attachments[i],0);
        if(child>>48!=5||!(child&0x0000ffffffffffffULL)||child==train)
            return failed(PhaseCaptureReason::InvalidAttachments);
        hash(sample.attachmentFingerprint,attachments[i]);
    }
    const auto [begin,end,cap]=before.history;
    constexpr size_t maximumHistory=128,maximumCapacity=4096;
    if(end<begin||cap<end||(end-begin)%0x10||(cap-begin)%0x10||
       cap-begin>maximumCapacity*0x10||(cap&&!address(begin,cap-begin)))
        return failed(PhaseCaptureReason::InvalidHistory);
    sample.historyCount=static_cast<uint32_t>((end-begin)/0x10);
    if(sample.historyCount>maximumHistory)return failed(PhaseCaptureReason::HistoryCapacity);
    std::array<std::array<unsigned char,0x10>,maximumHistory> rows,confirmed;
    const auto bytes=static_cast<size_t>(end-begin);
    if(bytes&&!read(begin,rows.data(),bytes))return failed(PhaseCaptureReason::Unreadable);
    sample.historyFingerprint=0xcbf29ce484222325ULL;
    for(size_t i=0;i<sample.historyCount;++i){
        // Native history may contain direction zero at a switch. It remains
        // diagnostic; zero is never canonicalized into a guessed rear route.
        const auto id=field<uint64_t>(rows[i],0);const auto direction=field<int32_t>(rows[i],8);
        if(id>>48!=1||direction<-1||direction>1)return failed(PhaseCaptureReason::InvalidHistory);
        for(auto byte:rows[i]){sample.historyFingerprint^=byte;sample.historyFingerprint*=0x100000001b3ULL;}
    }
    if(!header(after))return failed(PhaseCaptureReason::Unreadable);
    if(after.id!=train)return failed(PhaseCaptureReason::TrainChanged);
    if(before!=after)return failed(PhaseCaptureReason::ChangedReadback);
    if(bytes&&(!read(begin,confirmed.data(),bytes)||!std::equal(rows.begin(),rows.begin()+sample.historyCount,confirmed.begin())))
        return failed(PhaseCaptureReason::ChangedReadback);
    if((carBytes&&(!read(before.composition[0],carsAgain.data(),carBytes)||
                  !std::equal(cars.begin(),cars.begin()+carCount,carsAgain.begin())))||
       (attachmentBytes&&(!read(before.attachments[0],attachmentsAgain.data(),attachmentBytes)||
                          !std::equal(attachments.begin(),attachments.begin()+attachmentCount,attachmentsAgain.begin()))))
        return failed(PhaseCaptureReason::ChangedReadback);
    if(!header(after))return failed(PhaseCaptureReason::Unreadable);
    if(after.id!=train)return failed(PhaseCaptureReason::TrainChanged);
    if(before!=after)return failed(PhaseCaptureReason::ChangedReadback);
    return {PhaseCaptureReason::Ready,sample};
}
} // namespace nimby::windows::automatic
