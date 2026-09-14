#include "engine/track_usage.h"
#include <algorithm>
#include <array>
#include <cmath>
#include <cstring>
#include <unordered_set>
namespace nimby::engine {
namespace {
constexpr size_t max_records=1048576,max_slots=1048576;
using Bytes=std::vector<unsigned char>;
template<class T>T field(const void* data,size_t offset){T value;std::memcpy(&value,static_cast<const unsigned char*>(data)+offset,sizeof value);return value;}
bool ptr(uint64_t p){return p>=0x10000&&p<0x7fffffff0000ULL&&p%8==0;}
bool span(uint64_t begin,uint64_t end,uint64_t cap,size_t stride,size_t limit){return end>=begin&&cap>=end&&(end-begin)%stride==0&&(cap-begin)%stride==0&&(cap-begin)/stride<=limit&&(!cap||ptr(begin));}
bool stable(ReadMemory read,void* ctx,uint64_t at,const void* data,size_t n){Bytes again(n);return !n||(read(ctx,at,again.data(),n)&&std::memcmp(again.data(),data,n)==0);}
bool add(std::vector<TrackUsage>& out,uint64_t track,const unsigned char* data,size_t offset){
 const auto train=field<uint64_t>(data,0);double a=field<double>(data,offset),b=field<double>(data,offset+8);
 if((track>>48)!=1||(train>>48)!=5||!std::isfinite(a)||!std::isfinite(b)||a<0||a>1||b<0||b>1||out.size()>=max_records)return false;
 out.push_back({train,track,std::min(a,b),std::max(a,b)});return true;
}
bool same_root(ReadMemory r,void* c,const LiveState& s){LiveState now{};return resolve_live_state(r,c,s.module_base,true,now)&&now==s;}
bool reservations(ReadMemory read,void* ctx,const LiveState& s,std::vector<TrackUsage>& out){
 // RVA 0x47a5f0: Sim+0xd98; RVA 0x45f860: 16 Swiss-map shards, stride 0x88.
 size_t slots_budget=max_slots;
 for(size_t shard=0;shard<16;++shard){
  const auto table=s.simulation+0xd98+shard*0x88+0x50;std::array<uint64_t,4> h{};
  if(!read(ctx,table,h.data(),sizeof h))return false;
  const auto ctrl=h[0],slots=h[1],size=h[2],mask=h[3];
  if(!ctrl&&!slots&&!size&&!mask)continue;
  if(mask>slots_budget||size>mask||(mask&(mask+1))||!ptr(ctrl)||(!ptr(slots)&&mask))return false;
  slots_budget-=mask;Bytes controls(mask),values(mask*32);
  if(mask&&(!read(ctx,ctrl,controls.data(),controls.size())||!read(ctx,slots,values.data(),values.size())))return false;
  size_t filled=0;
  for(size_t i=0;i<mask;++i)if(controls[i]<128){
   ++filled;auto* slot=values.data()+i*32;const auto track=field<uint64_t>(slot,0);
   const auto begin=field<uint64_t>(slot,8),end=field<uint64_t>(slot,16),cap=field<uint64_t>(slot,24);
   if((track>>48)!=1||!span(begin,end,cap,32,16384)||(end-begin)/32>max_records-out.size())return false;
   Bytes entries(end-begin);
   if(!entries.empty()){
    if(!read(ctx,begin,entries.data(),entries.size())||!stable(read,ctx,begin,entries.data(),entries.size())||!stable(read,ctx,slots+i*32,slot,32))return false;
    // RVA 0x452250 / 0x452860: reservation train +0, interval +0x10/+0x18.
    for(size_t j=0;j<entries.size();j+=32)if(!add(out,track,entries.data()+j,16))return false;
   }
  }
  if(filled!=size||!stable(read,ctx,table,h.data(),sizeof h)||!stable(read,ctx,ctrl,controls.data(),controls.size()))return false;
 }
 return true;
}
bool occupations(ReadMemory read,void* ctx,const LiveState& s,std::vector<TrackUsage>& out){
 // RVA 0x6eb6d0: UI's car-occupation map at Sim+0x2c8; query iterator RVA 0x459ea0.
 const auto address=s.simulation+0x2c8;std::array<uint64_t,10> h{};
 if(!read(ctx,address,h.data(),sizeof h))return false;
 if(!span(h[0],h[1],h[2],8,max_slots)||!span(h[3],h[4],h[5],32,max_slots))return false;
 size_t n=(h[1]-h[0])/8;if((h[4]-h[3])/32!=n)return false;
 std::vector<uint64_t> keys(n);Bytes values(n*32);
 if(n&&(!read(ctx,h[0],keys.data(),n*8)||!read(ctx,h[3],values.data(),values.size())))return false;
 for(size_t i=0;i<n;++i)if(keys[i]&&!add(out,keys[i],values.data()+i*32,8))return false;
 // Overflow is a chained map of track -> vector<CarOccupancy>, node next at +0x20.
 const auto buckets=h[7];const auto count=uint32_t(h[8]);
 if(count>max_slots||!ptr(buckets)||!count)return false;
 std::vector<uint64_t> heads(count);if(!read(ctx,buckets,heads.data(),heads.size()*8))return false;
 std::unordered_set<uint64_t> seen;
 for(auto node:heads)while(node){
  if(!ptr(node)||!seen.insert(node).second||seen.size()>max_records)return false;
  std::array<uint64_t,5> entry{};if(!read(ctx,node,entry.data(),sizeof entry))return false;
  if(!span(entry[1],entry[2],entry[3],32,max_records)||(entry[2]-entry[1])/32>max_records-out.size())return false;
  Bytes rows(entry[2]-entry[1]);
  if(!rows.empty()&&(!read(ctx,entry[1],rows.data(),rows.size())||!stable(read,ctx,entry[1],rows.data(),rows.size())))return false;
  for(size_t j=0;j<rows.size();j+=32)if(!add(out,entry[0],rows.data()+j,8))return false;
  if(!stable(read,ctx,node,entry.data(),sizeof entry))return false;
  node=entry[4];
 }
 return stable(read,ctx,address,h.data(),sizeof h)&&stable(read,ctx,h[0],keys.data(),keys.size()*8)&&stable(read,ctx,h[3],values.data(),values.size())&&stable(read,ctx,buckets,heads.data(),heads.size()*8);
}
template<class F>bool capture(ReadMemory read,void* ctx,const LiveState& s,bool recognized,std::vector<TrackUsage>& out,F function) noexcept{
 out.clear();if(!read||!recognized)return false;
 try{for(int attempt=0;attempt<3;++attempt){std::vector<TrackUsage> result;if(!same_root(read,ctx,s))return false;
  if(function(read,ctx,s,result)&&same_root(read,ctx,s)){out=std::move(result);return true;}
 }}catch(...){}return false;
}
}
bool read_reservations(ReadMemory r,void* c,const LiveState& s,bool recognized,std::vector<TrackUsage>& out) noexcept{return capture(r,c,s,recognized,out,reservations);}
bool read_occupations(ReadMemory r,void* c,const LiveState& s,bool recognized,std::vector<TrackUsage>& out) noexcept{return capture(r,c,s,recognized,out,occupations);}
}
