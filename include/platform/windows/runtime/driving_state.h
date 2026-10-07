#pragma once
#include <engine/automatic_controller.h>
#include <engine/driving_publishers.h>
#include <platform/windows/runtime/physical_route.h>
#include <windows.h>
#include <atomic>
#include <array>
#include <memory>
#include <vector>

namespace nimby::windows::automatic {
using namespace nimby::engine::automatic;
struct SourceLifetime {uint64_t id=0,value=0;};
inline uint64_t lifetime(std::span<const SourceLifetime> rows,uint64_t id) noexcept {
 const auto at=std::lower_bound(rows.begin(),rows.end(),id,[](const auto& row,uint64_t key){return row.id<key;});
 return at!=rows.end()&&at->id==id?at->value:0;
}
struct RuntimeTrain : Train {
 PhysicalRoute physicalRoute;
 std::vector<SourceLifetime> retainedSources;
 uint64_t constraintLifetime=0;
 uint64_t routeRevision=0;
};
// Slots never move while this world is alive. The pool ordinal, rather than its
// generation, is the lookup key: native object recycling cannot grow the table.
// A full table/long collision chain only prevents a NEW slot, never an existing
// train's publication or a peer's acquired state. There is no global train lock.
struct TrainWorld {
 static constexpr size_t capacity=8192,maximumProbe=128;
 struct Slot {std::atomic<uint64_t> key{0};SRWLOCK lock=SRWLOCK_INIT;uint64_t id=0;RuntimeTrain state;};
 std::array<Slot,capacity> slots;
 // Qualified native pools contain at most 2^20 ordinals. This compact identity
 // index avoids allocating a full controller for every unrelated train. It is
 // conservative outside that bound; failure is never treated as non-membership.
 static constexpr size_t maximumOrdinal=1048576;
 std::array<std::atomic<uint64_t>,maximumOrdinal> interested{};
 bool isManaged(uint64_t id)const noexcept {
  const auto ordinal=(id>>16)&0xffffffffULL;
  if(ordinal>=maximumOrdinal)return true;
  const auto identity=interested[ordinal].load(std::memory_order_acquire);
  return identity==id||identity==UINT64_MAX;
 }
 void markManaged(uint64_t id)noexcept {
  const auto ordinal=(id>>16)&0xffffffffULL;
  if(ordinal>=maximumOrdinal)return;
  auto previous=interested[ordinal].load(std::memory_order_acquire);
  for(;;){
   if(previous==id||previous==UINT64_MAX)return;
   // Generation ordering/wrap is not a time proof. If callbacks of different
   // incarnations overlap, never let an older one erase newer managed interest.
   const auto next=previous?UINT64_MAX:id;
   if(interested[ordinal].compare_exchange_strong(previous,next,std::memory_order_acq_rel))return;
  }
 }
 static size_t hash(uint64_t value) noexcept {
  value^=value>>30;value*=0xbf58476d1ce4e5b9ULL;value^=value>>27;value*=0x94d049bb133111ebULL;
  return static_cast<size_t>(value^(value>>31))&(capacity-1);
 }
 Slot* find(uint64_t id,bool create) noexcept {
  const auto key=((id>>16)&0xffffffffULL)+1;
  const auto begin=hash(key);
  for(size_t n=0;n<maximumProbe;++n){auto& slot=slots[(begin+n)&(capacity-1)];
   auto seen=slot.key.load(std::memory_order_acquire);
   if(seen==key)return &slot;
   if(!seen){if(!create)return nullptr;
    if(slot.key.compare_exchange_strong(seen,key,std::memory_order_acq_rel)||seen==key)return &slot;
   }
  }
  return nullptr;
 }
};
struct Publication {
 mutable std::atomic<unsigned> readers{0};
 uintptr_t session=0;
 uint64_t worldRevision=0,revision=0;
 std::shared_ptr<TrainWorld> world;
 DrivingPublishers<NimbySignalDrivingRule> rules;
 DrivingPublishers<NimbyTrainConstraint> constraints;
 std::vector<SourceLifetime> signalLifetimes,constraintLifetimes;
 Publication()=default;
 Publication(const Publication& value):session(value.session),worldRevision(value.worldRevision),revision(value.revision),
  world(value.world),rules(value.rules),constraints(value.constraints),signalLifetimes(value.signalLifetimes),constraintLifetimes(value.constraintLifetimes){}
 bool active()const noexcept{return session&&(!rules.empty()||!constraints.empty());}
};
// A short entrance pin protects the pointer-to-reader-count handoff. Long native
// steps pin only THEIR immutable image. In seq_cst order, a reader that loaded
// an old image either still owns an entrance pin or has incremented its image
// pin before a writer can reclaim it. New readers after publication see the new
// image. Destruction is exclusively on writers, after their lock is released.
class Publications {
 std::atomic<unsigned> entering_{0};
 std::shared_ptr<Publication> current_=std::make_shared<Publication>();
 std::atomic<const Publication*> visible_{current_.get()};
 std::array<std::shared_ptr<Publication>,32> retired_{};
public:
 SRWLOCK writers=SRWLOCK_INIT;
 static_assert(std::atomic<const Publication*>::is_always_lock_free);
 static_assert(std::atomic<unsigned>::is_always_lock_free);
 class View {
  const Publication* value_;
 public:
  explicit View(Publications& owner) noexcept {
   owner.entering_.fetch_add(1,std::memory_order_seq_cst);
   value_=owner.visible_.load(std::memory_order_seq_cst);
   value_->readers.fetch_add(1,std::memory_order_seq_cst);
   owner.entering_.fetch_sub(1,std::memory_order_seq_cst);
  }
  View(const View&)=delete;
  ~View(){value_->readers.fetch_sub(1,std::memory_order_seq_cst);}
  const Publication* operator->()const noexcept{return value_;}
  const Publication& operator*()const noexcept{return *value_;}
 };
 // Caller owns writers. Copies and old destructors are retained by the caller;
 // no table construction, game read or train state runs under this lock.
 std::shared_ptr<const Publication> snapshot()const noexcept{return current_;}
 bool commit(const std::shared_ptr<const Publication>& expected,std::shared_ptr<Publication>& next,
             std::array<std::shared_ptr<Publication>,32>& garbage) noexcept {
  if(current_!=expected)return false;
  if(!entering_.load(std::memory_order_seq_cst))for(size_t n=0;n<retired_.size();++n)
   if(retired_[n]&&!retired_[n]->readers.load(std::memory_order_seq_cst))garbage[n]=std::move(retired_[n]);
  const auto free=std::find_if(retired_.begin(),retired_.end(),[](const auto& value){return !value;});
  if(free==retired_.end())return false;
  *free=std::move(current_);current_=std::move(next);
  visible_.store(current_.get(),std::memory_order_seq_cst);
  return true;
 }
};
// Same-thread native callbacks for the same train borrow the existing guard.
// Holding it across native integration guarantees that the actual crossing can
// be committed; a peer train never waits for it. Other-thread access to THIS
// train is a conservative, bounded refusal rather than concurrent mutation.
class TrainAccess {
 inline static thread_local TrainAccess* held_=nullptr;
 TrainAccess* previous_=nullptr;
 TrainWorld* world_=nullptr;
 TrainWorld::Slot* slot_=nullptr;
 bool owns_=false;
public:
 RuntimeTrain* state=nullptr;
 bool missing=false;
 TrainAccess(const Publication& publication,uint64_t id,uintptr_t motion,bool create=true):world_(publication.world.get()) {
  if(!world_){missing=true;return;}
  for(auto* held=held_;held;held=held->previous_)if(held->world_==world_&&held->slot_->id==id&&held->state->motion==motion){
   slot_=held->slot_;state=held->state;return;
  }
  slot_=world_->find(id,create);if(!slot_){missing=true;return;}
  if(!TryAcquireSRWLockExclusive(&slot_->lock))return;
  owns_=true;
  if(slot_->id!=id||slot_->state.motion!=motion){slot_->state=RuntimeTrain{};slot_->id=id;slot_->state.motion=motion;}
  state=&slot_->state;previous_=held_;held_=this;
 }
 TrainAccess(const TrainAccess&)=delete;
 ~TrainAccess(){if(owns_){held_=previous_;ReleaseSRWLockExclusive(&slot_->lock);}}
};
inline void pruneReleased(RuntimeTrain& state,const Publication& publication) {
 const auto released=[&](uint64_t source){const auto old=lifetime(state.retainedSources,source);
  return !lifetime(publication.signalLifetimes,source)||(old&&old!=lifetime(publication.signalLifetimes,source));};
 std::erase_if(state.memory.stops,[&](const auto& row){return released(row.source);});
 std::erase_if(state.memory.held,[&](const auto& row){return released(row.source);});
 std::erase_if(state.boundary,[&](const auto& row){return released(row.source.signal);});
 if(state.memory.sight&&released(state.memory.sight->source))state.memory.sight.reset();
 if(state.memory.stopped.signal&&released(state.memory.stopped.signal))state.memory.stopped={};
 if(state.entrySignal&&released(state.entrySignal)){state.entrySignal=0;state.entry={};}
 state.managed=std::any_of(state.ahead.begin(),state.ahead.end(),[&](const auto& row){return publication.rules.contains(row.signal);});
}
inline void rememberSources(RuntimeTrain& state,const Publication& publication) {
 state.retainedSources.clear();
 const auto remember=[&](uint64_t id){if(id)state.retainedSources.push_back({id,lifetime(publication.signalLifetimes,id)});};
 for(const auto& row:state.memory.stops)remember(row.source);
 for(const auto& row:state.memory.held)remember(row.source);
 for(const auto& row:state.boundary)remember(row.source.signal);
 if(state.memory.sight)remember(state.memory.sight->source);
 remember(state.memory.stopped.signal);remember(state.entrySignal);
 std::sort(state.retainedSources.begin(),state.retainedSources.end(),[](const auto& a,const auto& b){return a.id<b.id;});
 state.retainedSources.erase(std::unique(state.retainedSources.begin(),state.retainedSources.end(),[](const auto& a,const auto& b){return a.id==b.id;}),state.retainedSources.end());
}
}
