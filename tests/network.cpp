#include "engine/network.h"
#include "engine/simulation_clock.h"
#include <array>
#include <algorithm>
#include <cmath>
#include <cstdio>
#include <cstring>
#include <map>
#include <vector>
using namespace nimby::engine;
struct Memory {
    std::map<uint64_t,std::vector<unsigned char>> regions;
    uint64_t fail{},replace_header{},changing_slots{},changing_motion{},changing_station{},changing_service{};unsigned header_reads{},reads{},slot_reads{};
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
    if(address==m.changing_motion && size==0x4b1)static_cast<unsigned char*>(out)[0x4b0]=1;
    if(address==m.changing_station && size==0x28)static_cast<unsigned char*>(out)[0]^=1;
    if(address==m.changing_service && size==0x638)static_cast<unsigned char*>(out)[0x4d0]^=1;
    if(address==m.replace_header && ++m.header_reads==2)static_cast<unsigned char*>(out)[0]^=1;
    if(address==m.changing_slots && (++m.slot_reads%2)==0 && size>8)static_cast<unsigned char*>(out)[8]^=1;
    return true;
}
int main() {
    {
    // Optional-state storage can contain old station/clock values: ignore it.
    std::array<unsigned char,0x638> motion{};
    auto set=[&]<class T>(size_t off,T value){std::memcpy(motion.data()+off,&value,sizeof value);};
    NimbyTrainService service{};set(0,uint64_t(0x5000000000001));
    set(0x4c0,int64_t(123000000));set(0x488,int64_t(122000000));
    if(!decode_train_service(motion.data(),motion.size(),0,service)||service.status!=NIMBY_SERVICE_NOT_PRESENT||
       service.flags&(NIMBY_SERVICE_DEPARTURE_VALID|NIMBY_SERVICE_ARRIVAL_VALID))return 70;
    set(0x598,uint8_t(1));set(0x560,int64_t(125000000));
    if(!decode_train_service(motion.data(),motion.size(),0,service)||service.status!=NIMBY_SERVICE_DISPATCH_WAIT||
       !(service.flags&NIMBY_SERVICE_COOLDOWN_VALID)||service.location_track_id)return 71;
    set(0x598,uint8_t(0));set(0x1d0,uint8_t(1));set(0xb8,uint64_t(0x1000000000001));set(0xc0,0.5);set(0xc8,int8_t(1));
    set(0x4d0,uint8_t(1));set(0x510,uint8_t(1));
    if(!decode_train_service(motion.data(),motion.size(),0,service)||service.status!=NIMBY_SERVICE_STATION_STOP||
       service.departure_time_us!=123000000||service.arrival_time_us)return 72;
    // Hidden location is read from Blackhole, never from the stale Presence.
    set(0x1d0,uint8_t(0));set(0x218,uint8_t(1));set(0x1f8,uint64_t(0x1000000010001));set(0x200,0.2);set(0x208,int8_t(-1));
    if(!decode_train_service(motion.data(),motion.size(),0,service)||service.location_track_id!=0x1000000010001||
       service.status==NIMBY_SERVICE_DEPOT)return 73; // Depot requires resolved line and station.
    set(0x218,uint8_t(2));if(decode_train_service(motion.data(),motion.size(),0,service))return 74;
    set(0x218,uint8_t(0));set(0x4d0,uint8_t(0));set(0x4b0,uint8_t(1));set(0x458,uint64_t(1));
    if(!decode_train_service(motion.data(),motion.size(),0,service)||service.status!=NIMBY_SERVICE_SIGNAL_WAIT||service.alert!=6)return 75;
    if(!decode_train_service(motion.data(),motion.size(),2,service)||service.status!=NIMBY_SERVICE_MOTHBALLED)return 76;
    if(decode_train_service(motion.data(),0x500,0,service)||decode_train_service(motion.data(),motion.size(),3,service))return 77;
    }
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
    if(!out.stations[0].name.empty())return 50; // Missing optional cache is allowed.
    {
        auto fm=m;const uint64_t tags=0x300090000;
        fm.put(gb,0x70,uint32_t(0));fm.put(gb,0x78,tags);fm.put(gb,0x80,tags+16);fm.put(gb,0x88,tags+16);
        fm.put(tags,0,uint64_t(10115));fm.put(tags,8,uint64_t(10400)); // Tag keys are scalar IDs, not object-generation IDs.
        Network fn;
        if(!read_network(read,&fm,state,true,fn)||!fn.signals[0].filter_available||fn.signals[0].exception_count!=2||fn.signals[0].filter_default_ignored)return 110;
        fm.put(gb,0x70,uint32_t(1));
        if(!read_network(read,&fm,state,true,fn)||!fn.signals[0].filter_default_ignored)return 111;
        fm.put(gb,0x80,tags+3);
        if(!read_network(read,&fm,state,true,fn)||fn.signals[0].filter_available)return 112;
        fm.put(gb,0x80,tags+16);fm.fail=tags;
        if(!read_network(read,&fm,state,true,fn)||fn.signals[0].filter_available)return 113;
        fm.fail=0;fm.put(gb,0x78,uint64_t(0));fm.put(gb,0x80,uint64_t(0));fm.put(gb,0x88,uint64_t(0));
        if(!read_network(read,&fm,state,true,fn)||!fn.signals[0].filter_available||fn.signals[0].exception_count)return 114;
    }
    {
        auto jm=m;const uint64_t branch=track+0x10000,offset=0x4e8;
        jm.put(tb,offset,branch);jm.put(tb,offset+0x10,track);
        jm.put(tb,offset+0x3f0,track);jm.put(tb,offset+0x3f8,.4);jm.put(tb,offset+0x400,int32_t(1));
        jm.put(tb,0x408,tb+0x428);jm.put(tb,0x410,tb+0x430);jm.put(tb,0x418,tb+0x438);
        jm.put(tb,0x428,branch);
        Network jn;
        if(!read_network(read,&jm,state,true,jn)||jn.junctions.size()!=1||
           jn.junctions[0].branch_direction!=1||jn.junctions[0].main_fraction!=.4)return 90;
        auto valid=jm;
        jm.put(tb,offset+0x400,int32_t(0));
        if(!read_network(read,&jm,state,true,jn)||!jn.junctions.empty())return 91;
        jm=valid;jm.put(tb,offset+0x3f8,std::nan(""));
        if(!read_network(read,&jm,state,true,jn)||!jn.junctions.empty())return 92;
        jm=valid;jm.put(tb,0x428,branch+1); // Recycled slot: require full-generation identity.
        if(!read_network(read,&jm,state,true,jn)||!jn.junctions.empty())return 93;
        jm=valid;jm.put(tb,offset+8,track); // Both primary ends connected: not an attachment exit.
        if(!read_network(read,&jm,state,true,jn)||!jn.junctions.empty())return 94;
        jm=valid;jm.put(tb,0x410,tb+0x427); // Invalid native vector bounds.
        if(!read_network(read,&jm,state,true,jn)||!jn.junctions.empty())return 95;
        jm=valid;jm.fail=tb+offset+0x3f0;
        if(!read_network(read,&jm,state,true,jn)||!jn.junctions.empty())return 96;
        jm=valid;jm.put(tb,offset+8,track);jm.put(tb,offset+0x10,uint64_t(0));jm.put(tb,offset+0x400,int32_t(-1));
        if(!read_network(read,&jm,state,true,jn)||jn.junctions.size()!=1||
           jn.junctions[0].branch_direction!=-1||jn.junctions[0].main_direction!=-1)return 97;
    }
    const uint64_t nameBlock=0x300050000,nameText=0x300060000;
    pool(0x430,nameBlock,0xf8,station);
    std::memcpy(m.regions[nameBlock].data()+8,"Tours",6);
    m.put(nameBlock,0x18,uint64_t(5));m.put(nameBlock,0x20,uint64_t(15));
    if(!read_network(read,&m,state,true,out)||out.stations[0].name!="Tours")return 51;
    // Manual names take precedence even when the automatic cache has another name.
    m.put(sb,0x40,uint8_t(0));std::memcpy(m.regions[sb].data()+0x20,"Manual",7);
    m.put(sb,0x30,uint64_t(6));m.put(sb,0x38,uint64_t(15));
    if(!read_network(read,&m,state,true,out)||out.stations[0].name!="Manual")return 52;
    m.put(sb,0x40,uint8_t(1));m.put(nameBlock,0,station+1);
    if(!read_network(read,&m,state,true,out)||!out.stations[0].name.empty())return 53;
    m.put(nameBlock,0,station);m.changing_station=nameBlock;
    if(!read_network(read,&m,state,true,out)||!out.stations[0].name.empty())return 54;
    m.changing_station=0;
    const std::string longName="Saint-\xc3\x89tienne Ch\xc3\xa2teaucreux";
    m.regions[nameText]=std::vector<unsigned char>(longName.begin(),longName.end());m.regions[nameText].push_back(0);
    m.put(nameBlock,8,nameText);m.put(nameBlock,0x18,uint64_t(longName.size()));m.put(nameBlock,0x20,uint64_t(63));
    if(!read_network(read,&m,state,true,out)||out.stations[0].name!=longName)return 55;
    m.fail=nameText;
    if(!read_network(read,&m,state,true,out)||!out.stations[0].name.empty())return 56;
    m.fail=0;m.changing_slots=nameText;m.slot_reads=0;
    if(!read_network(read,&m,state,true,out)||!out.stations[0].name.empty())return 57;
    m.changing_slots=0;
    m.regions[nameText].back()='X';
    if(!read_network(read,&m,state,true,out)||!out.stations[0].name.empty())return 58;
    m.regions[nameText].back()=0;m.put(nameBlock,0x18,uint64_t(257));
    if(!read_network(read,&m,state,true,out)||!out.stations[0].name.empty())return 59;
    m.put(nameBlock,0x18,uint64_t(longName.size()));
    m.replace_header=state.database+0x430;m.header_reads=0;
    if(!read_network(read,&m,state,true,out)||!out.stations[0].name.empty())return 60;
    m.replace_header=0;m.header_reads=0;
    const uint64_t query=0x500000000,ctrl=0x500010000,slots=0x500020000;
    m.put(state.simulation,0x2200,query);m.put(query,0x378,ctrl);m.put(query,0x380,slots);
    m.put(query,0x388,uint64_t(1));m.put(query,0x390,uint64_t(3));
    m.regions[ctrl]={0,0x80,0xfe};m.regions[slots].resize(48);
    m.put(slots,0,signal);m.put(slots,8,int32_t(10));
    std::vector<SignalTextureState> states;
    if(!read_signal_texture_states(read,&m,state,true,states)||states.size()!=1||states[0].id!=signal||states[0].state!=10)return 30;
    m.put(slots,8,int32_t(0));
    if(!read_signal_texture_states(read,&m,state,true,states)||states[0].state!=0)return 31;
    m.put(query,0x390,uint64_t(2));if(read_signal_texture_states(read,&m,state,true,states)||!states.empty())return 32;
    m.put(query,0x390,uint64_t(3));m.put(slots,0,track);if(read_signal_texture_states(read,&m,state,true,states))return 33;
    m.put(slots,0,signal);m.fail=slots;if(read_signal_texture_states(read,&m,state,true,states))return 34;m.fail=0;
    m.changing_slots=slots;
    if(read_signal_texture_states(read,&m,state,true,states)||!states.empty())return 37;
    m.changing_slots=0;
    m.put(query,0x388,uint64_t(2));m.regions[ctrl][1]=0;m.put(slots,16,signal);
    if(read_signal_texture_states(read,&m,state,true,states))return 38;
    m.put(query,0x388,uint64_t(1));m.regions[ctrl][1]=0x80;
    m.reads=0;if(read_signal_texture_states(read,&m,state,false,states)||m.reads)return 35;
    m.put(query,0x388,uint64_t(0));m.regions[ctrl][0]=0x80;
    if(!read_signal_texture_states(read,&m,state,true,states)||!states.empty())return 36;
    // Manual and automatic native platform labels, including direction suffixes.
    m.put(tb,0xa8,'F');m.put(tb,0xb8,uint64_t(1));m.put(tb,0xc0,uint64_t(15));
    if(!read_network(read,&m,state,true,out)||out.platforms.size()!=1||std::strcmp(out.platforms[0].name_utf8,"F"))return 90;
    m.put(tb,0xc8,uint8_t(1));m.put(tb,0xa0,int32_t(13));
    if(!read_network(read,&m,state,true,out)||std::strcmp(out.platforms[0].name_utf8,"14"))return 91;
    const uint64_t neighbor=0x1000000010001,points=0x600010000;
    m.put(tb,0x4e8,neighbor);m.put(tb,8,neighbor);m.put(tb,16,track);
    m.put(tb,0x1b0,points);m.put(tb,0x1b8,points+32);
    m.put(points,0,0.0);m.put(points,8,0.0);m.put(points,16,0.0);m.put(points,24,10.0);
    if(!read_network(read,&m,state,true,out)||std::strcmp(out.platforms[0].name_utf8,"14S"))return 92;
    m.put(points,24,-10.0);
    if(!read_network(read,&m,state,true,out)||std::strcmp(out.platforms[0].name_utf8,"14N"))return 93;
    m.put(points,16,20.0);
    if(!read_network(read,&m,state,true,out)||std::strcmp(out.platforms[0].name_utf8,"14W"))return 94;
    m.put(points,16,-20.0);
    if(!read_network(read,&m,state,true,out)||std::strcmp(out.platforms[0].name_utf8,"14E"))return 95;
    m.fail=points;
    if(!read_network(read,&m,state,true,out)||out.platforms[0].flags)return 96;
    m.fail=0;m.put(tb,8,uint64_t(0));m.put(tb,16,uint64_t(0));m.put(tb,0x4e8,uint64_t(0));
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
    if(!read_trains(read,&m,state,true,trains)||trains.size()!=1||trains[0].name!="T"||trains[0].speed_mps!=20||!trains[0].positioned||!trains[0].speed_available)return 14;
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
    // Disengaged Drive bytes can be stale/non-finite: native UI uses zero instead.
    m.put(motionBlock,0xc0,0.75);m.put(motionBlock,0x3c8,std::nan(""));
    if(!read_trains(read,&m,state,true,trains)||!trains[0].speed_available||trains[0].speed_mps!=0||trains[0].present||!trains[0].positioned)return 40;
    m.put(motionBlock,0x1d0,uint8_t(0));
    if(!read_trains(read,&m,state,true,trains)||!trains[0].speed_available||trains[0].speed_mps!=0||trains[0].positioned)return 41;
    m.changing_motion=motionBlock;
    if(read_trains(read,&m,state,true,trains)||!trains.empty())return 42;
    m.changing_motion=0;
    // No Motion is different from a confirmed Motion without Drive.
    m.put(motionBlock,0,uint64_t(0));
    if(!read_trains(read,&m,state,true,trains)||trains[0].speed_available||trains[0].present)return 43;
    m.put(motionBlock,0,train);m.put(motionBlock,0x4b0,uint8_t(2));
    if(read_trains(read,&m,state,true,trains)||!trains.empty())return 44;
    m.put(motionBlock,0x4b0,uint8_t(1));
    if(read_trains(read,&m,state,true,trains)||!trains.empty())return 45;
    m.put(motionBlock,0x3c8,0.0);
    if(!read_trains(read,&m,state,true,trains)||!trains[0].present||!trains[0].speed_available||trains[0].speed_mps!=0)return 46;
    m.put(state.simulation,0x28,int64_t(10000));m.put(state.simulation,0x20,int64_t(1624876580));
    m.put(motionBlock,0x4b0,uint8_t(0));m.put(motionBlock,0x4d0,uint8_t(1));m.put(motionBlock,0x4c0,int64_t(125000000));
    if(!read_trains(read,&m,state,true,trains)||trains[0].service.departure_remaining_seconds!=25||
       trains[0].service.game_time_us!=100000000||!(trains[0].service.flags&NIMBY_SERVICE_CALENDAR_VALID))return 78;
    m.put(state.simulation,0x20,int64_t(-946771300));
    if(!read_trains(read,&m,state,true,trains)||trains[0].service.departure_remaining_seconds!=25||
       !(trains[0].service.flags&NIMBY_SERVICE_CALENDAR_VALID))return 178;
    m.put(motionBlock,0x4b8,int64_t(1786572360));
    std::vector<CalendarWrite> calendarEdits;
    if(!plan_simulation_calendar(read,&m,state.simulation,-2713737487LL,calendarEdits))return 179;
    const auto planned=std::find_if(calendarEdits.begin(),calendarEdits.end(),[&](const auto& e){return e.address==motionBlock+0x4b8;});
    if(planned==calendarEdits.end()||planned->after!=1786572360LL-2713737487LL)return 180;
    m.fail=state.simulation+0xa0;
    if(plan_simulation_calendar(read,&m,state.simulation,1,calendarEdits))return 181;
    m.fail=0;
    m.put(state.simulation,0x28,int64_t(20000));
    if(!read_trains(read,&m,state,true,trains)||trains[0].service.departure_remaining_seconds!=0)return 79;
    m.fail=state.simulation+0x28;
    if(!read_trains(read,&m,state,true,trains)||!(trains[0].service.flags&NIMBY_SERVICE_STATE_VALID)||
       trains[0].service.flags&NIMBY_SERVICE_CLOCK_VALID)return 80;
    m.fail=0;
    const uint64_t line=0x4000000000001,lineBlock=0x300070000,stopData=0x300080000;
    pool(0x180,lineBlock,0x280,line);
    m.put(lineBlock,0xfc,int32_t(1));m.put(lineBlock,0x88,uint64_t(0));m.put(lineBlock,0x90,uint64_t(15));
    m.put(lineBlock,0x118,stopData);m.put(lineBlock,0x120,stopData+0x158);m.put(lineBlock,0x128,stopData+0x158);
    m.regions[stopData].resize(0x158);m.put(stopData,0x78,track);m.put(stopData,0x110,station);
    m.put(motionBlock,0x5d0,uint8_t(1));m.put(motionBlock,0x5a8,line);m.put(motionBlock,0x5c8,int32_t(0));
    m.put(motionBlock,0x510,uint8_t(1));m.put(motionBlock,0x218,uint8_t(1));
    m.put(motionBlock,0x1f8,track);m.put(motionBlock,0x200,0.5);m.put(motionBlock,0x208,int8_t(1));
    if(!read_trains(read,&m,state,true,trains)||trains[0].service.status!=NIMBY_SERVICE_DEPOT||
       trains[0].service.stop_station_id!=station||trains[0].service.stop_track_id!=track)return 81;
    m.put(stopData,0xb8,int32_t(120));m.put(stopData,0xbc,int32_t(150));
    m.put(motionBlock,0x1d0,uint8_t(1));m.put(motionBlock,0x48,int32_t(1564)); // Capacity differs from occupancy.
    const uint64_t paxQuery=0x600020000,paxBuckets=0x600030000,paxNode=0x600040000;
    m.put(state.simulation+0x2208,0,paxQuery);m.put(paxQuery,0x90,paxBuckets);m.put(paxQuery,0x98,uint64_t(1));
    m.put(paxBuckets,0,paxNode);m.put(paxBuckets,8,uint64_t(0));
    m.put(paxNode,0,train);m.put(paxNode,8,int32_t(123));m.put(paxNode,12,9840.f);m.put(paxNode,16,uint64_t(0));
    m.put(motionBlock,0x5f0,uint8_t(1));m.put(motionBlock,0x5d8,uint64_t(0x6000000000001));
    m.put(motionBlock,0x5e0,uint64_t(42));m.put(motionBlock,0x5e8,int32_t(7));
    if(!read_trains(read,&m,state,true,trains)||!trains[0].line_stops_available||trains[0].line_stops.size()!=1||
       trains[0].line_stops[0].departure_offset_seconds!=150||trains[0].details.passenger_count!=123||
       trains[0].details.flags!=(NIMBY_TRAIN_PASSENGERS_VALID|NIMBY_TRAIN_ASSIGNMENT_VALID)||trains[0].details.order_index!=7)return 85;
    m.put(paxNode,8,int32_t(0));
    if(!read_trains(read,&m,state,true,trains)||!(trains[0].details.flags&NIMBY_TRAIN_PASSENGERS_VALID)||trains[0].details.passenger_count)return 100;
    m.fail=paxQuery+0x90;
    if(!read_trains(read,&m,state,true,trains)||trains[0].details.flags&NIMBY_TRAIN_PASSENGERS_VALID)return 101;
    m.fail=0;m.put(paxNode,8,int32_t(-1));
    if(!read_trains(read,&m,state,true,trains)||trains[0].details.flags&NIMBY_TRAIN_PASSENGERS_VALID)return 102;
    m.put(paxNode,8,int32_t(123));m.put(paxNode,16,paxNode);
    if(!read_trains(read,&m,state,true,trains)||trains[0].details.flags&NIMBY_TRAIN_PASSENGERS_VALID)return 103;
    m.put(paxNode,16,uint64_t(0));m.put(paxNode,0,train+1); // Different generation is never joined.
    if(!read_trains(read,&m,state,true,trains)||!(trains[0].details.flags&NIMBY_TRAIN_PASSENGERS_VALID)||trains[0].details.passenger_count)return 104;
    m.put(paxNode,0,train);m.changing_slots=paxNode;m.slot_reads=0;
    if(!read_trains(read,&m,state,true,trains)||trains[0].details.flags&NIMBY_TRAIN_PASSENGERS_VALID)return 105;
    m.changing_slots=0;
    m.put(motionBlock,0x1d0,uint8_t(0));m.put(motionBlock,0x5f0,uint8_t(0));
    m.put(stopData,0xbc,int32_t(110));
    if(!read_trains(read,&m,state,true,trains)||trains[0].details.flags||trains[0].line_stops[0].flags)return 86;
    m.put(stopData,0x110,uint64_t(0));
    if(!read_trains(read,&m,state,true,trains)||!trains[0].line_stops_available||trains[0].line_stops[0].station_id)return 88;
    m.put(stopData,0x110,station);
    m.put(stopData,0xbc,int32_t(150));m.fail=stopData;
    if(!read_trains(read,&m,state,true,trains)||trains[0].line_stops_available)return 87;
    m.fail=0;
    m.put(lineBlock,0,line+1);
    if(!read_trains(read,&m,state,true,trains)||trains[0].service.status==NIMBY_SERVICE_DEPOT||
       trains[0].service.flags&NIMBY_SERVICE_LINE_VALID)return 82;
    m.put(lineBlock,0,line);m.fail=stopData;
    if(!read_trains(read,&m,state,true,trains)||trains[0].service.status==NIMBY_SERVICE_DEPOT||
       trains[0].service.flags&NIMBY_SERVICE_STOP_VALID)return 83;
    m.fail=0;m.changing_service=motionBlock;
    if(!read_trains(read,&m,state,true,trains)||trains[0].service.flags||!trains[0].speed_available)return 84;
    m.changing_service=0;
    std::puts("Synthetic network: full ID joins, pool replacement, read failure, unknown version, invalid position and limits rejected.");
}
