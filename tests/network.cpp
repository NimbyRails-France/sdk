#include "engine/network.h"
#include <array>
#include <cmath>
#include <cstdio>
#include <cstring>
#include <map>
#include <vector>
using namespace nimby::engine;
struct Memory {
    std::map<uint64_t,std::vector<unsigned char>> regions;
    uint64_t fail{},replace_header{};unsigned header_reads{},reads{};
    template<class T> void put(uint64_t base,size_t off,T value) {
        auto& b=regions[base];if(b.size()<off+sizeof value)b.resize(off+sizeof value);
        std::memcpy(b.data()+off,&value,sizeof value);
    }
};
bool read(void* ctx,uint64_t address,void* out,size_t size) {
    auto& m=*static_cast<Memory*>(ctx);++m.reads;
    if(address==m.fail)return false;
    auto i=m.regions.upper_bound(address);if(i==m.regions.begin())return false;--i;
    if(address-i->first>i->second.size()||size>i->second.size()-(address-i->first))return false;
    std::memcpy(out,i->second.data()+address-i->first,size);
    if(address==m.replace_header && ++m.header_reads==2)static_cast<unsigned char*>(out)[0]^=1;
    return true;
}
int main() {
    LiveState state{0x140000000,0x200000000,0x200010000,0x200020000,0x200030000};
    Memory m;m.put(state.module_base+0xb81998,0,state.root);m.put(state.root,0x540,state.database);
    m.put(state.root,0x5c0,state.copy);m.put(state.root,0x680,state.simulation);
    const uint64_t track=0x1000000000001,station=0x2000000000001,signal=0x8000000000001;
    auto pool=[&](uint64_t off,uint64_t block,size_t stride,uint64_t id){
        auto header=state.database+off,table=block-0x1000;
        m.regions[header].resize(48);m.put(header,4,uint32_t(1));m.put(header,8,uint32_t(2));m.put(header,16,uint32_t(1));
        m.put(header,24,table);m.put(header,32,table+8);m.put(header,40,table+8);m.put(table,0,block);
        m.regions[block].resize(stride*2);m.put(block,0,id);
    };
    const uint64_t tb=0x300000000,sb=0x300010000,gb=0x300020000;
    pool(0,tb,0x4e8,track);pool(0x80,sb,0x3e8,station);pool(0x380,gb,0xc8,signal);
    m.put(tb,0xd0,station);m.put(tb,0x80,50.f);m.put(tb,0x84,30.f);
    m.put(sb,0x40,uint8_t(1)); // Automatic name intentionally unavailable.
    m.put(gb,0x30,int32_t(3));m.put(gb,0x40,track);m.put(gb,0x48,0.5);m.put(gb,0x50,int8_t(-1));
    Network out;
    if(!read_network(read,&m,state,true,out)||out.tracks.size()!=1||out.tracks[0].limit_mps!=30||out.signals[0].kind!=3)return 1;
    m.put(tb,0x84,0.f);
    if(!read_network(read,&m,state,true,out)||out.tracks[0].limit_mps!=50)return 2;
    m.reads=0;if(read_network(read,&m,state,false,out)||m.reads||!out.tracks.empty())return 3;
    m.put(gb,0x40,track+1);if(read_network(read,&m,state,true,out)||!out.tracks.empty())return 4;
    m.put(gb,0x40,track);m.put(tb,0x80,std::nanf(""));if(read_network(read,&m,state,true,out))return 5;
    m.put(tb,0x80,50.f);m.fail=gb;if(read_network(read,&m,state,true,out))return 6;m.fail=0;
    m.replace_header=state.database;if(read_network(read,&m,state,true,out))return 7;m.replace_header=0;
    m.put(tb,0,track+0x10000);if(read_network(read,&m,state,true,out))return 8;m.put(tb,0,track);
    m.put(state.root,0x540,state.database+0x1000);if(read_network(read,&m,state,true,out))return 9;
    // Large-save capacity: 486 blocks of 512 slots, mostly unused in this fixture.
    // Reuse an empty backing block to avoid allocating a 300 MB synthetic world.
    m.put(state.root,0x540,state.database);
    const auto oldHeader=m.regions[state.database];
    const auto oldTable=m.regions[tb-0x1000];
    const auto oldBlock=m.regions[tb];
    const uint64_t emptyBlock=0x400000000;
    m.regions[emptyBlock].resize(512*0x4e8);m.regions[tb].resize(512*0x4e8);
    m.put(state.database,4,uint32_t(9));m.put(state.database,8,uint32_t(512));m.put(state.database,16,uint32_t(511));
    m.put(state.database,32,tb-0x1000+486*8);m.put(state.database,40,tb-0x1000+486*8);
    for(size_t b=1;b<486;++b)m.put(tb-0x1000,b*8,emptyBlock);
    if(!read_network(read,&m,state,true,out)||out.tracks.size()!=1)return 18;
    // A malformed pool above the bounded capacity must still be refused.
    m.put(state.database,4,uint32_t(16));m.put(state.database,8,uint32_t(65536));m.put(state.database,16,uint32_t(65535));
    if(read_network(read,&m,state,true,out)||!out.tracks.empty())return 19;
    m.regions[state.database]=oldHeader;m.regions[tb-0x1000]=oldTable;m.regions[tb]=oldBlock;
    std::array<unsigned char,0x638> motion{};
    auto put=[&](size_t off,auto value){std::memcpy(motion.data()+off,&value,sizeof value);};
    put(0x3a8,track);put(0x3b0,0.5);put(0x3b8,int8_t(1));put(0x4b0,uint8_t(1));TrainPosition pos;
    if(!decode_train_position(motion.data(),motion.size(),pos)||pos.track_id!=track)return 10;
    put(0x4b0,uint8_t(0));if(decode_train_position(motion.data(),motion.size(),pos)||pos.track_id)return 11;
    put(0x4b0,uint8_t(1));put(0x3b0,std::nan(""));if(decode_train_position(motion.data(),motion.size(),pos))return 12;
    put(0x3b0,0.5);put(0x3b8,int8_t(0));if(decode_train_position(motion.data(),motion.size(),pos))return 13;
    // New reusable train reader: model/motion join by complete ID, no game process.
    m.put(state.root,0x540,state.database);
    const uint64_t train=0x5000000000001,trainBlock=0x300030000,motionBlock=0x300040000;
    pool(0x200,trainBlock,0x178,train);
    pool(state.simulation+0xa0-state.database,motionBlock,0x638,train);
    m.put(trainBlock,0x10,'T');m.put(trainBlock,0x20,uint64_t(1));m.put(trainBlock,0x28,uint64_t(15));
    m.put(motionBlock,0x3a8,track);m.put(motionBlock,0x3b0,0.25);m.put(motionBlock,0x3b8,int8_t(-1));
    m.put(motionBlock,0x3c8,20.0);m.put(motionBlock,0x4b0,uint8_t(1));
    std::vector<Train> trains;
    if(!read_trains(read,&m,state,true,trains)||trains.size()!=1||trains[0].name!="T"||trains[0].speed_mps!=20||!trains[0].positioned)return 14;
    m.put(motionBlock,0,train+1);if(read_trains(read,&m,state,true,trains)||!trains.empty())return 15;
    m.put(motionBlock,0,train);m.put(trainBlock,0x20,uint64_t(257));if(read_trains(read,&m,state,true,trains))return 16;
    m.put(trainBlock,0x20,uint64_t(1));m.put(motionBlock,0x4b0,uint8_t(0));
    if(!read_trains(read,&m,state,true,trains)||trains[0].present||trains[0].positioned)return 17;
    m.put(motionBlock,0x4b0,uint8_t(1));m.put(motionBlock,0x320,uint8_t(1));
    const uint64_t pathData=0x500000000;
    m.put(motionBlock,0x338,pathData);m.put(motionBlock,0x340,pathData+8);m.put(motionBlock,0x348,pathData+8);m.put(pathData,0,track);
    if(!read_trains(read,&m,state,true,trains)||!trains[0].path_available||trains[0].path!=std::vector<uint64_t>{track})return 20;
    m.fail=pathData;if(!read_trains(read,&m,state,true,trains)||trains[0].path_available)return 21;m.fail=0;
    m.put(pathData,0,station);if(!read_trains(read,&m,state,true,trains)||trains[0].path_available)return 22;
    m.put(pathData,0,track);m.put(motionBlock,0x340,pathData+7);
    if(!read_trains(read,&m,state,true,trains)||trains[0].path_available)return 23;
    m.put(motionBlock,0x4b0,uint8_t(0));m.put(motionBlock,0x1d0,uint8_t(1));
    m.put(motionBlock,0xb8,track);m.put(motionBlock,0xc0,0.75);m.put(motionBlock,0xc8,int8_t(-1));
    if(!read_trains(read,&m,state,true,trains)||trains[0].present||!trains[0].positioned||trains[0].position.fraction!=0.75)return 24;
    m.put(motionBlock,0xc0,std::nan(""));
    if(!read_trains(read,&m,state,true,trains)||trains[0].positioned)return 25;
    std::puts("Synthetic network: full ID joins, pool replacement, read failure, unknown version, invalid position and limits rejected.");
}
