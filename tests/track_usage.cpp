#include "engine/track_usage.h"
#include <array>
#include <map>
#include <vector>
#include <cstring>
#include <cstdio>
#include <cmath>
using namespace nimby::engine;
struct Memory {
 std::map<uint64_t,std::vector<unsigned char>> regions;uint64_t fail{},unstable{};unsigned reads{},mutations{};
 template<class T>void put(uint64_t base,size_t off,T value){auto& v=regions[base];if(v.size()<off+sizeof value)v.resize(off+sizeof value);std::memcpy(v.data()+off,&value,sizeof value);}
};
bool read(void* c,uint64_t at,void* out,size_t n){auto& m=*static_cast<Memory*>(c);++m.reads;if(at==m.fail)return false;auto i=m.regions.upper_bound(at);if(i==m.regions.begin())return false;--i;if(at-i->first>i->second.size()||n>i->second.size()-(at-i->first))return false;std::memcpy(out,i->second.data()+at-i->first,n);if(at==m.unstable&&n&&++m.mutations%2==0)static_cast<unsigned char*>(out)[0]^=1;return true;}
#define CHECK(x) do{if(!(x)){std::fprintf(stderr,"FAIL line %d: %s\n",__LINE__,#x);return 1;}}while(false)
int main(){
 static_assert(sizeof(TrackUsage)==32);
 LiveState s{0x140000000,0x200000000,0x200010000,0x200020000,0x200030000};Memory m;
 m.put(s.module_base+0xb81998,0,s.root);m.put(s.root,0x540,s.database);m.put(s.root,0x5c0,s.copy);m.put(s.root,0x680,s.simulation);m.regions[s.simulation].resize(0x3000);
 const uint64_t ctrl=0x300000000,slot=0x300001000,rows=0x300002000,train=0x5000000000001,track=0x1000000000001;
 const size_t header=0xd98+0x50;
 m.put(s.simulation,header,ctrl);m.put(s.simulation,header+8,slot);m.put(s.simulation,header+16,uint64_t(1));m.put(s.simulation,header+24,uint64_t(1));
 m.put(ctrl,0,uint8_t(1));m.put(slot,0,track);m.put(slot,8,rows);m.put(slot,16,rows+32);m.put(slot,24,rows+32);
 m.put(rows,0,train);m.put(rows,8,track);m.put(rows,16,.8);m.put(rows,24,.2);
 std::vector<TrackUsage> out;CHECK(read_reservations(read,&m,s,true,out)&&out.size()==1&&out[0].begin==.2&&out[0].end==.8);
 m.put(slot,16,rows);CHECK(read_reservations(read,&m,s,true,out)&&out.empty());m.put(slot,16,rows+32);
 m.unstable=rows;CHECK(!read_reservations(read,&m,s,true,out)&&out.empty());m.unstable=0;
 m.put(rows,24,std::nan(""));CHECK(!read_reservations(read,&m,s,true,out)&&out.empty());m.put(rows,24,.2);
 m.put(s.simulation,header+24,uint64_t(2));CHECK(!read_reservations(read,&m,s,true,out));m.put(s.simulation,header+24,uint64_t(1));
 m.put(slot,24,rows+31);CHECK(!read_reservations(read,&m,s,true,out));m.put(slot,24,rows+32);
 m.put(rows,0,uint64_t(0));CHECK(!read_reservations(read,&m,s,true,out));m.put(rows,0,train);
 m.fail=ctrl;CHECK(!read_reservations(read,&m,s,true,out)&&out.empty());m.fail=0;
 m.reads=0;CHECK(!read_reservations(read,&m,s,false,out)&&m.reads==0);
 const uint64_t keys=0x300003000,values=0x300004000,buckets=0x300005000,overflow=0x300006000,extra=0x300007000;
 m.put(s.simulation,0x2c8,keys);m.put(s.simulation,0x2d0,keys+8);m.put(s.simulation,0x2d8,keys+8);
 m.put(s.simulation,0x2e0,values);m.put(s.simulation,0x2e8,values+32);m.put(s.simulation,0x2f0,values+32);
 m.put(s.simulation,0x300,buckets);m.put(s.simulation,0x308,uint64_t(1));m.put(buckets,0,uint64_t(0));
 m.put(keys,0,track);m.put(values,0,train);m.put(values,8,.1);m.put(values,16,.3);m.put(values,24,uint64_t(0));
 CHECK(read_occupations(read,&m,s,true,out)&&out.size()==1&&out[0].begin==.1);
 m.put(buckets,0,overflow);m.put(overflow,0,track);m.put(overflow,8,extra);m.put(overflow,16,extra+32);m.put(overflow,24,extra+32);m.put(overflow,32,uint64_t(0));
 m.put(extra,0,train);m.put(extra,8,.7);m.put(extra,16,.5);m.put(extra,24,uint64_t(0));
 CHECK(read_occupations(read,&m,s,true,out)&&out.size()==2&&out[1].begin==.5&&out[1].end==.7);
 m.put(overflow,32,overflow);CHECK(!read_occupations(read,&m,s,true,out)&&out.empty());m.put(overflow,32,uint64_t(0));
 m.unstable=values;m.mutations=0;CHECK(!read_occupations(read,&m,s,true,out)&&out.empty());m.unstable=0;
 m.put(values,16,1.1);CHECK(!read_occupations(read,&m,s,true,out));m.put(values,16,.3);
 m.put(s.root,0x680,s.simulation+8);CHECK(!read_occupations(read,&m,s,true,out)&&out.empty());CHECK(!read_reservations(read,&m,s,true,out)&&out.empty());
 std::puts("Synthetic reservation/occupation lifecycle and read guards passed.");
}
