#include <platform/windows/runtime/physical_route.h>
#include <iostream>
#include <stdexcept>
#include <limits>

#define CHECK(x) do {if(!(x))throw std::runtime_error("line "+std::to_string(__LINE__)+": " #x);}while(false)
namespace {
constexpr uint64_t train=0x5000000000001,track=0x1000000000001,other=track+0x10000;
template<class T> void put(auto& bytes,size_t offset,T value){std::memcpy(bytes.data()+offset,&value,sizeof value);}
struct Fixture {
 std::array<unsigned char,0x500> motion{};
 std::array<unsigned char,0x100> first{},second{};
 std::array<uint64_t,2> ids{track,other};
 nimby::windows::automatic::PhysicalRoute route;
 double obstacle=std::numeric_limits<double>::infinity();
 unsigned queries=0;
 uintptr_t unreadable=0;
 Fixture(){
  put(motion,0,train);put(motion,0x4b0,uint8_t{1});put(motion,0x320,uint8_t{1});
  put(motion,0x338,reinterpret_cast<uintptr_t>(ids.data()));
  put(motion,0x340,reinterpret_cast<uintptr_t>(ids.data()+ids.size()));
  put(motion,0x348,reinterpret_cast<uintptr_t>(ids.data()+ids.size()));
  put(first,0,track);put(second,0,other);put(first,0x88,5000.0);put(second,0x88,1000.0);
  head(track,0,1);
 }
 uintptr_t address()const{return reinterpret_cast<uintptr_t>(motion.data());}
 auto reader(){return [this](uintptr_t at,void* out,size_t n){
  if(at==unreadable)return false;
  auto contained=[&](const auto& storage){auto begin=reinterpret_cast<uintptr_t>(storage.data());
   return at>=begin&&at-begin<=sizeof storage&&n<=sizeof storage-(at-begin);};
  if(!contained(motion)&&!contained(first)&&!contained(second)&&!contained(ids))return false;
  std::memcpy(out,reinterpret_cast<void*>(at),n);return true;
 };}
 void head(uint64_t id,double fraction,int8_t direction){put(motion,0x3a8,id);put(motion,0x3b0,fraction);put(motion,0x3b8,direction);}
 void begin(double position=1000){route.begin(address(),train,position,reader());}
 void append(auto& section,double from,double to,double length,double offset=0){
  route.append(reinterpret_cast<uintptr_t>(section.data()),from,to,length,offset,reader());
 }
 auto refresh(double position=1000,uint64_t now=1000000){
  return route.refresh(address(),position,now,reader(),[&](uintptr_t,double,double,double length){
   ++queries;return std::min(length,obstacle);
  });
 }
};
}
int main(){try{
 Fixture f;f.begin();f.append(f.first,0,1,5000);
 auto view=f.refresh();CHECK(view.at(1000,1000000).verified&&view.free==200&&view.covered==200);
 // A long block / a long pause has no duration limit. Geometry survives, but
 // current physical occupation is queried again, even without a new scan.
 auto before=f.queries;view=f.refresh(1000,3600000);
 CHECK(view.valid&&view.observed==3600000&&f.queries>before);
 f.obstacle=14;view=f.refresh(1000,7200000);CHECK(view.free==14);
 f.obstacle=0;view=f.refresh();CHECK(view.free==0);
 f.obstacle=10000;f.head(track,.8,1);view=f.refresh(5000,9999999);
 CHECK(view.valid&&view.covered==200&&view.free==200);
 f.head(track,.99,1);view=f.refresh(5950);CHECK(view.valid&&std::abs(view.covered-50)<1e-8);
 f.head(track,1,1);CHECK(!f.refresh(6000).valid); // No invented continuation.
 f.head(track,0,1);CHECK(!f.refresh(999).valid); // Distance reset.
 f.head(other,0,1);CHECK(!f.refresh().valid); // Another route.
 f.head(track,0,-1);CHECK(!f.refresh().valid); // Reversal.
 f.head(track,.1,1);CHECK(!f.refresh().valid); // Head/odometer mismatch.
 f.head(track,0,1);f.ids[1]=other+1;CHECK(!f.refresh().valid); // In-place path edit.
 f.ids[1]=other;put(f.motion,0x2a0,uint64_t{1});CHECK(!f.refresh().valid);
 put(f.motion,0x2a0,uint64_t{0});put(f.first,0,track+1);CHECK(!f.refresh().valid);
 put(f.first,0,track);put(f.first,0x88,5001.0);CHECK(!f.refresh().valid);
 put(f.first,0x88,5000.0);put(f.motion,0x4b0,uint8_t{0});CHECK(!f.refresh().valid);
 put(f.motion,0x4b0,uint8_t{1});f.unreadable=f.address()+0x290;CHECK(!f.refresh().valid);
 f.unreadable=0;CHECK(f.refresh().valid);
 // Reverse travel and short successive native ranges retain their direction
 // and accumulate coverage; a clear first range must not cap the next one.
 f.head(track,.02,-1);f.begin();f.append(f.first,.02,0,100);f.append(f.second,1,.8,200,100);
 view=f.refresh();CHECK(view.valid&&std::abs(view.covered-200)<1e-8&&view.free==200);
 f.head(other,.98,-1);view=f.refresh(1120);CHECK(view.valid&&std::abs(view.covered-180)<1e-8);
 // Unknown native geometry is never bridged, and an obstacle query failure
 // never returns the former clearance as a fresh observation.
 f.head(track,0,1);f.begin();f.append(f.first,0,.02,100);f.append(f.second,0,.1,100,101);
 CHECK(!f.refresh().valid);
 f.begin();f.append(f.first,0,1,50);CHECK(!f.refresh().valid);
 f.begin();f.append(f.first,0,1,5000);f.obstacle=-1;CHECK(!f.refresh().valid);
 auto changed=f.route.refresh(f.address(),1000,1000,f.reader(),[&](uintptr_t,double,double,double length){f.ids[1]++;return length;});
 CHECK(!changed.valid); // Recheck after native queries as well.
 std::cout<<"PASS: long blocks, no timer, fresh occupation, both directions, path identity and bounded coverage\n";
 }catch(const std::exception& e){std::cerr<<e.what()<<'\n';return 1;}}
