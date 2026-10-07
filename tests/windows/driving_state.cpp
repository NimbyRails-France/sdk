#include <platform/windows/runtime/driving_state.h>
#include <chrono>
#include <condition_variable>
#include <future>
#include <iostream>
#include <memory>
#include <mutex>
#include <stdexcept>
#include <vector>

namespace {
using namespace nimby::windows::automatic;
using namespace std::chrono_literals;
#define CHECK(condition) do { if(!(condition))throw std::runtime_error(#condition); } while(false)

constexpr uint64_t trainId(uint64_t ordinal,uint64_t generation=1) {
 return (uint64_t{5}<<48)|(ordinal<<16)|generation;
}
std::shared_ptr<const Publication> snapshot(Publications& owner) {
 AcquireSRWLockExclusive(&owner.writers);
 const auto result=owner.snapshot();
 ReleaseSRWLockExclusive(&owner.writers);
 return result;
}
bool publish(Publications& owner,uint64_t revision) {
 const auto previous=snapshot(owner);
 auto next=std::make_shared<Publication>(*previous);
 next->revision=revision;
 std::array<std::shared_ptr<Publication>,32> garbage{};
 AcquireSRWLockExclusive(&owner.writers);
 const bool result=owner.commit(previous,next,garbage);
 ReleaseSRWLockExclusive(&owner.writers);
 return result; // Retired images are destroyed only after releasing writers.
}

void publicationPinsAndRetirement() {
 Publications owner;
 auto initial=snapshot(owner);
 std::weak_ptr<const Publication> initialLifetime=initial;
 auto pinned=std::make_unique<Publications::View>(owner);
 initial.reset();
 CHECK(publish(owner,1));
 CHECK((*pinned)->revision==0&&!initialLifetime.expired());
 // One suspended image does not pin all later images or stop publication.
 for(uint64_t version=2;version<=96;++version)CHECK(publish(owner,version));
 CHECK((*pinned)->revision==0&&!initialLifetime.expired());
 pinned.reset();
 CHECK(publish(owner,97));
 CHECK(initialLifetime.expired());

 // Distinct suspended generations exhaust the explicitly bounded retirement
 // table. Failure leaves the current image intact; releasing one pin recovers.
 std::vector<std::unique_ptr<Publications::View>> generations;
 for(uint64_t i=0;i<32;++i){
  generations.push_back(std::make_unique<Publications::View>(owner));
  CHECK((*generations.back())->revision==97+i);
  CHECK(publish(owner,98+i));
 }
 const auto before=snapshot(owner);
 CHECK(!publish(owner,130)&&snapshot(owner)==before);
 for(size_t i=0;i<generations.size();++i)CHECK((*generations[i])->revision==97+i);
 generations.front().reset();
 CHECK(publish(owner,130)&&snapshot(owner)->revision==130);
 generations.clear();
 CHECK(publish(owner,131));

 // A hook's reader cannot depend on acquiring the publication writer lock.
 std::future<uint64_t> reader;
 bool independent=false;
 AcquireSRWLockExclusive(&owner.writers);
 reader=std::async(std::launch::async,[&]{Publications::View view(owner);return view->revision;});
 independent=reader.wait_for(2s)==std::future_status::ready;
 ReleaseSRWLockExclusive(&owner.writers);
 CHECK(independent&&reader.get()==131);

 // The expected immutable image, not just its revision number, guards commit.
 auto obsolete=snapshot(owner);
 auto staleNext=std::make_shared<Publication>(*obsolete);
 CHECK(publish(owner,132));
 std::array<std::shared_ptr<Publication>,32> garbage{};
 AcquireSRWLockExclusive(&owner.writers);
 const bool staleCommitted=owner.commit(obsolete,staleNext,garbage);
 ReleaseSRWLockExclusive(&owner.writers);
 CHECK(!staleCommitted&&snapshot(owner)->revision==132);
}

void concurrentPublicationHandoff() {
 Publications owner;
 constexpr uint64_t mask=0x5a5a0123abcd9876ULL;
 const auto makeImage=[&](uint64_t revision){
  auto image=std::make_shared<Publication>();
  image->revision=revision;image->worldRevision=revision*7;
  image->session=static_cast<uintptr_t>(revision^mask);
  image->signalLifetimes={{1,revision},{3,revision^mask},{7,revision*7}};
  return image;
 };
 {
  auto previous=snapshot(owner);auto initial=makeImage(0);
  std::array<std::shared_ptr<Publication>,32> garbage{};
  AcquireSRWLockExclusive(&owner.writers);
  const bool committed=owner.commit(previous,initial,garbage);
  ReleaseSRWLockExclusive(&owner.writers);
  CHECK(committed);
 }
 std::atomic<bool> done=false,valid=true;
 std::mutex readyMutex;std::condition_variable readyChanged;unsigned ready=0;
 std::vector<std::future<uint64_t>> readers;
 // Unwind the writer before joining futures if an allocation itself fails.
 struct StopReaders {std::atomic<bool>& done;~StopReaders(){done.store(true);}} stop{done};
 for(unsigned n=0;n<4;++n)readers.push_back(std::async(std::launch::async,[&]{
  uint64_t count=0,last=0;
  do {
   {
    Publications::View image(owner);
    const auto revision=image->revision;
    // Force some old images to remain pinned across several writer commits.
    if(!(count%64))SwitchToThread();
    const auto& rows=image->signalLifetimes;
    if(revision<last||image->worldRevision!=revision*7||image->session!=(revision^mask)||
       rows.size()!=3||rows[0].id!=1||rows[0].value!=revision||rows[1].id!=3||
       rows[1].value!=(revision^mask)||rows[2].id!=7||rows[2].value!=revision*7)
     valid.store(false);
    last=revision;
   }
   if(++count==1){std::lock_guard lock(readyMutex);++ready;readyChanged.notify_one();}
  }while(!done.load());
  return count;
 }));
 bool allReady=false;
 {std::unique_lock lock(readyMutex);allReady=readyChanged.wait_for(lock,2s,[&]{return ready==4;});}
 uint64_t revision=0;unsigned attempts=0;
 if(allReady)while(revision<4000&&attempts++<200000&&valid.load()){
  const auto previous=snapshot(owner);auto next=makeImage(revision+1);
  std::array<std::shared_ptr<Publication>,32> garbage{};
  AcquireSRWLockExclusive(&owner.writers);
  const bool committed=owner.commit(previous,next,garbage);
  ReleaseSRWLockExclusive(&owner.writers);
  if(committed)++revision;else SwitchToThread();
 }
 done.store(true);
 bool allRead=true;
 for(auto& reader:readers)allRead=reader.get()>0&&allRead;
 CHECK(allReady&&allRead&&valid.load()&&revision==4000);
 CHECK(snapshot(owner)->revision==4000);
}

void trainIdentityAndReentrance() {
 Publication publication;publication.world=std::make_shared<TrainWorld>();
 constexpr auto first=trainId(12),second=trainId(13);
 constexpr uintptr_t motion=0x12340000,otherMotion=0x12350000;
 {
  TrainAccess outer(publication,first,motion);
  CHECK(outer.state&&!outer.missing);
  outer.state->memory.stops.push_back({0x8000000000001,100,0,10});
  {
   TrainAccess other(publication,second,otherMotion);
   CHECK(other.state&&other.state!=outer.state);
   TrainAccess nested(publication,first,motion);
   CHECK(nested.state==outer.state&&nested.state->memory.stops.size()==1);
   nested.state->memory.lastHead=25;
   // A recycled generation/motion cannot replace an object still in use by
   // an outer native callback, even on the same thread and pool ordinal.
   TrainAccess recycled(publication,trainId(12,2),motion+8);
   CHECK(!recycled.state&&!recycled.missing);
   CHECK(outer.state->motion==motion&&outer.state->memory.lastHead==25);
  }
  TrainAccess nestedAfterPeer(publication,first,motion);
  CHECK(nestedAfterPeer.state==outer.state);
 }
 auto* slot=publication.world->find(first,false);
 CHECK(slot);
 {
  TrainAccess recycled(publication,trainId(12,2),motion+8);
  CHECK(recycled.state&&recycled.state->memory.stops.empty());
  CHECK(recycled.state->motion==motion+8&&recycled.state->memory.lastHead==-1);
  CHECK(publication.world->find(trainId(12,2),false)==slot);
 }
 {
  TrainAccess replacement(publication,trainId(12,2),motion+16);
  CHECK(replacement.state&&replacement.state->memory.stops.empty());
 }
 Publication newWorld;newWorld.world=std::make_shared<TrainWorld>();
 TrainAccess oldState(publication,trainId(12,2),motion+16);
 oldState.state->memory.lastHead=50;
 TrainAccess freshState(newWorld,trainId(12,2),motion+16);
 CHECK(freshState.state&&freshState.state!=oldState.state&&freshState.state->memory.lastHead==-1);
}

void busyIsOnlyThisTrain() {
 Publication publication;publication.world=std::make_shared<TrainWorld>();
 constexpr auto first=trainId(21),second=trainId(22);
 std::future<bool> reader;bool independent=false;
 {
  TrainAccess held(publication,first,0x32100000);
  CHECK(held.state);
  held.state->memory.lastHead=123;
  reader=std::async(std::launch::async,[&]{
   TrainAccess busy(publication,first,0x32100000);
   TrainAccess peer(publication,second,0x32200000);
   return !busy.state&&!busy.missing&&peer.state&&peer.state->memory.lastHead==-1;
  });
  independent=reader.wait_for(2s)==std::future_status::ready;
 }
 CHECK(independent&&reader.get());
 TrainAccess recovered(publication,first,0x32100000);
 CHECK(recovered.state&&recovered.state->memory.lastHead==123);
}

void retainedSourceIncarnations() {
 constexpr uint64_t source=0x8000000000001,target=0x8000000010001;
 Publication publication;publication.signalLifetimes={{source,10},{target,20}};
 RuntimeTrain state;
 state.memory.stops.push_back({target,100,0,10,false,{},source});
 state.memory.held.push_back({0,10,0,false,source});
 state.memory.stopped={target,100};
 state.entrySignal=target;state.entry={100,200,200,0,true};
 rememberSources(state,publication);
 CHECK(state.retainedSources.size()==2);
 Publication renewed(publication);renewed.revision=5;
 pruneReleased(state,renewed);
 CHECK(state.memory.stops.size()==1&&state.memory.held.size()==1);
 CHECK(state.memory.stopped.signal==target&&state.entrySignal==target);
 // Releasing then re-adding identical IDs is a new incarnation, regardless
 // of whether the same publisher token and identical rule bytes are reused.
 Publication replaced(renewed);replaced.signalLifetimes={{source,11},{target,20}};
 pruneReleased(state,replaced);
 CHECK(state.memory.stops.empty()&&state.memory.held.empty());
 CHECK(state.memory.stopped.signal==target&&state.entrySignal==target);
 replaced.signalLifetimes.clear();
 pruneReleased(state,replaced);
 CHECK(!state.memory.stopped.signal&&!state.entrySignal&&!state.entry.valid);
 rememberSources(state,replaced);
 CHECK(state.retainedSources.empty());
}

void interestDoesNotAllocateControllers() {
 auto world=std::make_shared<TrainWorld>();
 constexpr auto first=trainId(31),peer=trainId(32),replacement=trainId(31,2);
 CHECK(!world->isManaged(first)&&!world->isManaged(peer));
 world->markManaged(first);
 CHECK(world->isManaged(first)&&!world->isManaged(peer));
 CHECK(!world->find(first,false));
 // A new native generation has no inherited controller or negative cache.
 CHECK(!world->isManaged(replacement));
 world->markManaged(replacement);
 CHECK(world->isManaged(replacement));
 // A delayed callback for the retired incarnation must not erase the newer
 // incarnation's interest. Ambiguity stays conservative until the next world.
 world->markManaged(first);
 CHECK(world->isManaged(replacement)&&world->isManaged(first));
 CHECK(!world->find(replacement,false));
 auto nextWorld=std::make_shared<TrainWorld>();
 CHECK(!nextWorld->isManaged(replacement));
 // Unknown pool ordinals are conservative, never mistaken for unmanaged.
 CHECK(world->isManaged(trainId(TrainWorld::maximumOrdinal)));
 CHECK(world->isManaged(trainId(TrainWorld::maximumOrdinal+1)));
}

void boundedRegistrySaturation() {
 auto world=std::make_shared<TrainWorld>();
 std::vector<uint64_t> colliding;
 for(uint64_t ordinal=0;ordinal<16000000&&colliding.size()<=TrainWorld::maximumProbe;++ordinal)
  if(TrainWorld::hash(ordinal+1)==0)colliding.push_back(trainId(ordinal));
 CHECK(colliding.size()==TrainWorld::maximumProbe+1);
 for(size_t i=0;i<TrainWorld::maximumProbe;++i){world->markManaged(colliding[i]);CHECK(world->find(colliding[i],true));}
 world->markManaged(colliding.back());
 CHECK(!world->find(colliding.back(),true));
 for(size_t i=0;i<TrainWorld::maximumProbe;++i)CHECK(world->find(colliding[i],false));
 uint64_t peer=0;
 for(uint64_t ordinal=0;ordinal<16000000;++ordinal)
  if(TrainWorld::hash(ordinal+1)>TrainWorld::maximumProbe){peer=trainId(ordinal);break;}
 CHECK(peer&&world->find(peer,true));

 // Fill every bucket without depending on random collision-chain lengths.
 // Each chosen ordinal hashes directly to its own previously empty bucket.
 world=std::make_shared<TrainWorld>();
 std::vector<uint64_t> bucketIds(TrainWorld::capacity);
 size_t remaining=bucketIds.size();
 for(uint64_t ordinal=0;ordinal<16000000&&remaining;++ordinal){
  auto& id=bucketIds[TrainWorld::hash(ordinal+1)];
  if(!id){id=trainId(ordinal);--remaining;}
 }
 CHECK(remaining==0);
 for(const auto id:bucketIds){world->markManaged(id);CHECK(world->find(id,true));}
 world->markManaged(trainId(16000001));
 CHECK(!world->find(trainId(16000001),true));
 for(const auto id:bucketIds)CHECK(world->find(id,false));
 Publication publication;publication.world=world;
 TrainAccess existing(publication,bucketIds.front(),0x55550000);
 TrainAccess missing(publication,trainId(16000001),0x55560000);
 CHECK(existing.state&&!missing.state&&missing.missing);
}
}

int main() {
 try {
  publicationPinsAndRetirement();
  concurrentPublicationHandoff();
  trainIdentityAndReentrance();
  busyIsOnlyThisTrain();
  retainedSourceIncarnations();
  interestDoesNotAllocateControllers();
  boundedRegistrySaturation();
  std::cout<<"PASS: publication pins/retirement, train identity/reentrance, peer progress and bounded saturation\n";
  return 0;
 }catch(const std::exception& error){std::cerr<<"FAIL: "<<error.what()<<'\n';return 1;}
}
