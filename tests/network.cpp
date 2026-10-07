#include "engine/network.h"
#include "engine/simulation_clock.h"
#include <nimby/signal_approach.hpp>
#include <array>
#include <algorithm>
#include <cmath>
#include <cstdio>
#include <cstring>
#include <map>
#include <vector>
#include <chrono>
using namespace nimby::engine;
struct Memory {
    std::map<uint64_t,std::vector<unsigned char>> regions;
    uint64_t fail{},replace_header{},changing_slots{},changing_motion{},changing_station{},changing_service{},changing_presence{},changing_identity{};unsigned header_reads{},reads{},slot_reads{};
    uint64_t changing_metric{},changing_metric_identity{};
    uint64_t metric_block{},failed_metric_row{};size_t metric_block_size{};
    unsigned metric_block_reads{},metric_prefix_reads{};bool fail_metric_verification{};
    uint64_t changing_attachment{},attachment_after_children{};
    uint64_t changing_retired_motion{},changing_retired_model{};size_t retired_flag_offset{};unsigned retired_model_reads{};
    uint64_t changing_head{};TrainPosition latest_head{};
    uint64_t changing_pool_on_track{},pool_target{};unsigned pool_track_reads{};size_t pool_target_offset{};
    uint64_t watched_table{};size_t watched_table_size{};unsigned watched_table_reads{};uint64_t watched_table_bytes{};
    uint64_t change_after_plan{},different_shared_stop{};size_t change_after_plan_offset{};
    uint64_t watched_record{},changing_record{};size_t record_size{};unsigned record_reads{},changing_record_reads{};
    std::vector<uint64_t> forbiddenAddresses;unsigned forbiddenReads{};
    template<class T> void put(uint64_t base,size_t off,T value) {
        auto& b=regions[base];if(b.size()<off+sizeof value)b.resize(off+sizeof value);
        std::memcpy(b.data()+off,&value,sizeof value);
    }
};
bool read(void* ctx,uint64_t address,void* out,size_t size) {
    auto& m=*static_cast<Memory*>(ctx);++m.reads;
    if(std::find(m.forbiddenAddresses.begin(),m.forbiddenAddresses.end(),address)!=m.forbiddenAddresses.end()) {++m.forbiddenReads;return false;}
    if(size==0x90)++m.metric_prefix_reads;
    if(address==m.metric_block&&size==m.metric_block_size&&++m.metric_block_reads==2&&m.fail_metric_verification)return false;
    if(address==m.failed_metric_row&&size==0x90)return false;
    if(address==m.watched_record&&size==m.record_size)++m.record_reads;
    if(address==m.watched_table&&size==m.watched_table_size){++m.watched_table_reads;m.watched_table_bytes+=size;}
    if(address==m.fail)return false;
    auto i=m.regions.upper_bound(address);if(i==m.regions.begin())return false;--i;
    if(address-i->first>i->second.size()||size>i->second.size()-(address-i->first))return false;
    std::memcpy(out,i->second.data()+address-i->first,size);
    if(address==m.watched_table&&size==m.watched_table_size&&m.watched_table_reads==3&&m.change_after_plan){
        m.regions.at(m.change_after_plan)[m.change_after_plan_offset]^=1;
        // Include an in-place edit when it affects the table being returned.
        if(m.change_after_plan==address)static_cast<unsigned char*>(out)[m.change_after_plan_offset]^=1;
    }
    if(address==m.watched_table&&size==m.watched_table_size&&m.watched_table_reads==2&&m.attachment_after_children){
        auto changed=m.regions.upper_bound(m.attachment_after_children);--changed;
        changed->second[m.attachment_after_children-changed->first]^=1;
    }
    if(address==m.changing_attachment&&size==0x30)static_cast<unsigned char*>(out)[0]^=1;
    if(address==m.different_shared_stop&&size==0x158)static_cast<unsigned char*>(out)[0x110]^=1;
    if(address==m.changing_pool_on_track&&(size==0x4e8||size==0xd8)&&++m.pool_track_reads==2)
        m.regions.at(m.pool_target)[m.pool_target_offset]^=1;
    if(address==m.changing_record&&size==m.record_size&&++m.changing_record_reads==2)static_cast<unsigned char*>(out)[0]^=1;
    // The bulk Motion read and the later per-train validation can straddle a
    // simulation tick. Change only the head in the second complete record.
    if(address==m.changing_head && size==0x638){
        auto* p=static_cast<unsigned char*>(out);
        std::memcpy(p+0x3a8,&m.latest_head.track_id,sizeof(uint64_t));
        std::memcpy(p+0x3b0,&m.latest_head.fraction,sizeof(double));
        const auto direction=static_cast<int8_t>(m.latest_head.direction);
        std::memcpy(p+0x3b8,&direction,sizeof direction);
    }
    if(address==m.changing_metric&&size==0x90)static_cast<unsigned char*>(out)[0x88]^=1;
    if(address==m.changing_metric_identity&&size==0x90)static_cast<unsigned char*>(out)[0]^=1;
    if(address==m.metric_block&&size==m.metric_block_size&&m.metric_block_reads==2){
        if(m.changing_metric>=address&&m.changing_metric-address+0x90<=size)
            static_cast<unsigned char*>(out)[m.changing_metric-address+0x88]^=1;
        if(m.changing_metric_identity>=address&&m.changing_metric_identity-address+0x90<=size)
            static_cast<unsigned char*>(out)[m.changing_metric_identity-address]^=1;
    }
    if(address==m.changing_motion && size==0x638)static_cast<unsigned char*>(out)[0x4b0]=1;
    if(address==m.changing_station && size==0x28)static_cast<unsigned char*>(out)[0]^=1;
    if(address==m.changing_service && size==0x638)static_cast<unsigned char*>(out)[0x4d0]^=1;
    if(address==m.changing_presence && size==0x638)static_cast<unsigned char*>(out)[0x1d0]^=1;
    if(address==m.changing_identity && size==0x638)static_cast<unsigned char*>(out)[0]^=1;
    if(address==m.changing_retired_motion && size==0x638)static_cast<unsigned char*>(out)[m.retired_flag_offset]=1;
    if(address==m.changing_retired_model && size==8 && ++m.retired_model_reads==2)static_cast<unsigned char*>(out)[7]=5;
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
    {
        // Tool topology retains the complete network used by the planner,
        // including both signal directions and every branch attachment. It
        // does not dereference station names, platform labels or signal filters.
        auto topology=m;
        const uint64_t branch=track+0x10000,secondSignal=signal+0x10000,children=0x340000000,filter=0x340001000;
        topology.put(tb,0x88,1234.5);
        topology.put(tb,0x4e8,branch);topology.put(tb,0x4e8+16,track);topology.put(tb,0x4e8+0x88,500.);
        topology.put(tb,0x4e8+0x3f0,track);topology.put(tb,0x4e8+0x3f8,.6);topology.put(tb,0x4e8+0x400,int32_t(-1));
        topology.put(tb,0x408,children);topology.put(tb,0x410,children+8);topology.put(tb,0x418,children+8);topology.put(children,0,branch);
        topology.put(gb,0xc8,secondSignal);topology.put(gb,0xc8+0x30,int32_t(4));topology.put(gb,0xc8+0x40,branch);
        topology.put(gb,0xc8+0x48,.25);topology.put(gb,0xc8+0x50,int8_t(1));
        topology.put(gb,0x78,filter);topology.put(gb,0x80,filter+8);topology.put(gb,0x88,filter+8);topology.put(filter,0,uint64_t(123));
        auto fullMemory=topology;Network full,reduced;
        if(!read_network(read,&fullMemory,state,true,full)||!read_network(read,&topology,state,true,reduced,NetworkScope::Topology))return 340;
        if(full.tracks.size()!=2||reduced.tracks.size()!=2||full.signals.size()!=2||reduced.signals.size()!=2||full.junctions.size()!=1||reduced.junctions.size()!=1)return 341;
        for(size_t i=0;i<full.tracks.size();++i){const auto& a=full.tracks[i];const auto& b=reduced.tracks[i];
            if(a.id!=b.id||a.links[0]!=b.links[0]||a.links[1]!=b.links[1]||a.native_length_m!=b.native_length_m)return 342;
        }
        for(size_t i=0;i<full.signals.size();++i){const auto& a=full.signals[i];const auto& b=reduced.signals[i];
            if(a.id!=b.id||a.track_id!=b.track_id||a.fraction!=b.fraction||a.direction!=b.direction||a.kind!=b.kind||b.filter_available)return 343;
        }
        const auto& a=full.junctions.front();const auto& b=reduced.junctions.front();
        if(a.branch_track_id!=b.branch_track_id||a.main_track_id!=b.main_track_id||a.main_fraction!=b.main_fraction||
           a.main_direction!=b.main_direction||a.branch_direction!=b.branch_direction)return 344;
        if(!reduced.stations.empty()||!reduced.platforms.empty())return 345;
        topology.forbiddenAddresses={state.database+0x80,sb,filter,tb+0xa8};
        if(!read_network(read,&topology,state,true,reduced,NetworkScope::Topology)||topology.forbiddenReads)return 346;
        topology.put(gb,0x48,.75);
        if(!read_network(read,&topology,state,true,reduced,NetworkScope::Topology)||reduced.signals.front().fraction!=.75||full.signals.front().fraction!=.5)return 347;
        for(double invalid:{0.,-1.,std::nan(""),double(INFINITY)}){
            auto unknown=topology;unknown.put(tb,0x88,invalid);
            if(!read_network(read,&unknown,state,true,reduced,NetworkScope::Topology)||reduced.tracks.front().native_length_m!=0)return 348;
        }
        auto metricChanged=topology;metricChanged.metric_block=tb;metricChanged.metric_block_size=2*0x4e8;metricChanged.changing_metric=tb;
        if(!read_network(read,&metricChanged,state,true,reduced,NetworkScope::Topology)||reduced.tracks.front().native_length_m!=0)return 349;
        for(const auto address:{state.database,state.database+0x380,state.root+0x540}){
            auto replaced=topology;replaced.replace_header=address;
            if(read_network(read,&replaced,state,true,reduced,NetworkScope::Topology)||!reduced.tracks.empty()||!reduced.signals.empty())return 350;
        }
        auto staleReference=topology;staleReference.put(gb,0x40,track+1);
        if(read_network(read,&staleReference,state,true,reduced,NetworkScope::Topology))return 351;
        auto deleted=topology;deleted.put(gb,0,uint64_t(0));
        if(!read_network(read,&deleted,state,true,reduced,NetworkScope::Topology)||reduced.signals.size()!=1||reduced.signals.front().id!=secondSignal)return 352;
    }
    {
        // A 102,400-slot track pool contains unrelated unreadable backing
        // regions. Train data reads just five unique referenced prefixes,
        // regardless of the 128 trains sharing their current line plan.
        auto targeted=m;
        constexpr uint64_t table=0x410000000,tracksBase=0x420000000;
        constexpr unsigned blockCount=100,blockSize=1024;
        targeted.put(state.database,4,uint32_t(10));targeted.put(state.database,8,uint32_t(blockSize));
        targeted.put(state.database,16,uint32_t(blockSize-1));targeted.put(state.database,24,table);
        targeted.put(state.database,32,table+blockCount*8);targeted.put(state.database,40,table+blockCount*8);
        for(unsigned i=0;i<blockCount;++i)targeted.put(table,i*8,tracksBase+uint64_t(i)*0x1000000);
        const std::array<unsigned,5> indexes{0,100,30000,70000,99999};
        std::array<uint64_t,5> ids{},addresses{};
        for(size_t i=0;i<indexes.size();++i){
            const auto index=indexes[i];ids[i]=track+uint64_t(index)*0x10000;
            addresses[i]=tracksBase+uint64_t(index/blockSize)*0x1000000+(index%blockSize)*0x4e8;
            targeted.regions[addresses[i]].resize(0xd8);targeted.put(addresses[i],0,ids[i]);
            targeted.put(addresses[i],0x80,50.f);targeted.put(addresses[i],0x84,30.f);targeted.put(addresses[i],0xd0,station);
        }
        targeted.put(sb,0x40,uint8_t(0));std::memcpy(targeted.regions[sb].data()+0x20,"Depot",6);
        targeted.put(sb,0x30,uint64_t(5));targeted.put(sb,0x38,uint64_t(15));
        std::vector<Train> selected(128);
        for(auto& train:selected){
            train.positioned=true;train.position={ids[0],.5,1};
            train.service.flags=NIMBY_SERVICE_LOCATION_VALID|NIMBY_SERVICE_STOP_VALID;
            train.service.location_track_id=ids[1];train.service.stop_track_id=ids[2];
            train.service.line_id=0x4000000000001;train.line_stops_available=true;
            train.line_stops={{train.service.line_id,ids[3],station,0,0,0,0},{train.service.line_id,ids[4],station,1,0,0,0}};
        }
        targeted.fail=gb;targeted.watched_record=addresses[0];targeted.record_size=0xd8;
        targeted.watched_table=table;targeted.watched_table_size=blockCount*8;
        Network result;
        if(!read_train_network(read,&targeted,state,true,selected,result)||result.tracks.size()!=5||
           result.stations.size()!=1||result.stations[0].name!="Depot"||result.tracks[0].limit_mps!=30||
           !result.signals.empty()||!result.junctions.empty()||!result.platforms.empty())return 300;
        if(targeted.record_reads!=2||targeted.watched_table_reads!=2||targeted.reads>45)return 301;
        std::printf("train network: 102400 pool slots, 128 trains, 5 referenced tracks, %u process reads\n",targeted.reads);
        for(const auto address:addresses){
            auto missing=targeted;missing.fail=address;
            if(!read_train_network(read,&missing,state,true,selected,result)||result.tracks.size()!=4)return 302;
            auto generation=targeted;generation.put(address,0,uint64_t(0));
            if(!read_train_network(read,&generation,state,true,selected,result)||result.tracks.size()!=4)return 303;
            auto changing=targeted;changing.changing_record=address;changing.changing_record_reads=0;
            if(!read_train_network(read,&changing,state,true,selected,result)||result.tracks.size()!=4)return 304;
        }
        for(const auto changed:{state.database,table}){
            auto replacement=targeted;replacement.changing_pool_on_track=addresses[0];replacement.pool_target=changed;
            if(!read_train_network(read,&replacement,state,true,selected,result)||!result.tracks.empty())return 305;
        }
        auto stationGone=targeted;stationGone.put(sb,0,station+1);
        if(!read_train_network(read,&stationGone,state,true,selected,result)||result.tracks.size()!=5||!result.stations.empty())return 306;
        auto stationPool=targeted;stationPool.replace_header=state.database+0x80;
        if(!read_train_network(read,&stationPool,state,true,selected,result)||!result.stations.empty())return 307;
        auto rootChanged=targeted;rootChanged.changing_pool_on_track=addresses[0];rootChanged.pool_target=state.root;
        rootChanged.pool_target_offset=0x680;
        if(read_train_network(read,&rootChanged,state,true,selected,result))return 308;
        if(read_train_network(read,&targeted,state,false,selected,result))return 309;
        auto empty=targeted;empty.fail=state.database;
        if(!read_train_network(read,&empty,state,true,{},result)||!result.tracks.empty()||!result.stations.empty())return 310;
        // Oversized optional references are bounded before any memory reads.
        auto bounded=targeted;bounded.reads=0;
        Train many;many.service.line_id=0x4000000000001;many.line_stops_available=true;
        for(unsigned i=0;i<40000;++i)many.line_stops.push_back({many.service.line_id,track+uint64_t(i)*0x10000,0,i,0,0,0});
        if(!read_train_network(read,&bounded,state,true,std::span(&many,1),result)||result.tracks.size()!=3||
           bounded.reads>32810)return 318;
    }
    {
        auto automatic=m;const uint64_t cache=0x330000000,table=cache-0x1000,text=cache+0x1000;
        const auto header=state.database+0x430;
        automatic.regions[header].resize(48);automatic.put(header,4,uint32_t(1));automatic.put(header,8,uint32_t(2));
        automatic.put(header,16,uint32_t(1));automatic.put(header,24,table);automatic.put(header,32,table+8);automatic.put(header,40,table+8);
        automatic.put(table,0,cache);automatic.regions[cache].resize(0xf8*2);automatic.put(cache,0,station);
        const std::string name="Long automatic station name";
        automatic.regions[text]=std::vector<unsigned char>(name.begin(),name.end());automatic.regions[text].push_back(0);
        automatic.put(cache,8,text);automatic.put(cache,0x18,uint64_t(name.size()));automatic.put(cache,0x20,uint64_t(63));
        Train train;train.positioned=true;train.position={track,.5,1};Network result;
        const auto capture=[&](Memory& memory){return read_train_network(read,&memory,state,true,std::span(&train,1),result);};
        if(!capture(automatic)||result.stations.size()!=1||result.stations[0].name!=name)return 311;
        auto missing=automatic;missing.fail=text;
        if(!capture(missing)||result.stations.size()!=1||!result.stations[0].name.empty())return 312;
        auto oldGeneration=automatic;oldGeneration.put(cache,0,station+1);
        if(!capture(oldGeneration)||result.stations.size()!=1||!result.stations[0].name.empty())return 313;
        auto changed=automatic;changed.changing_slots=text;
        if(!capture(changed)||result.stations.size()!=1||!result.stations[0].name.empty())return 314;
        for(const auto replaced:{header,table}){
            auto edited=automatic;edited.replace_header=replaced;
            if(!capture(edited)||result.stations.size()!=1||!result.stations[0].name.empty())return 315;
        }
        auto stationChanged=automatic;stationChanged.changing_record=sb;stationChanged.record_size=0x48;
        if(!capture(stationChanged)||!result.stations.empty())return 316;
        // Explicit service/line station references survive an unavailable rail.
        train.positioned=false;train.service.flags=NIMBY_SERVICE_STOP_VALID;train.service.stop_station_id=station;
        automatic.fail=tb;
        if(!capture(automatic)||!result.tracks.empty()||result.stations.size()!=1)return 317;
    }
    {
        // Optional native length must survive independently from map coordinates.
        auto metric=m;metric.put(tb,0x88,1234.5);Network measured;
        if(!read_network(read,&metric,state,true,measured)||measured.tracks[0].native_length_m!=1234.5)return 230;
        for(double invalid:{0.,-1.,std::nan(""),double(INFINITY)}){
            metric=m;metric.put(tb,0x88,invalid);
            if(!read_network(read,&metric,state,true,measured)||measured.tracks[0].native_length_m!=0)return 231;
        }
        metric=m;metric.put(tb,0x88,1234.5);metric.changing_metric=tb;
        metric.metric_block=tb;metric.metric_block_size=2*0x4e8;
        if(!read_network(read,&metric,state,true,measured)||measured.tracks[0].native_length_m!=0)return 232;
        metric.changing_metric=0;metric.changing_metric_identity=tb;metric.metric_block_reads=0;
        if(!read_network(read,&metric,state,true,measured)||measured.tracks[0].native_length_m!=0)return 233;
        // The existing scoped signalling reader does not pay for a metric scan.
        metric.changing_metric_identity=0;
        if(!read_network(read,&metric,state,true,measured,NetworkScope::Signalling)||measured.tracks[0].native_length_m!=0)return 234;
        static_assert(gameLayout(LiveStateProfile::Linux119).track_metric_offset==0);
    }
    {
        // Verify real pool traversal at map scale: lengths require one second
        // read per block, not one syscall per track. A changed row affects
        // only itself; a failed optional block still preserves readable rows.
        auto large=m;constexpr unsigned blockSize=1024,blockCount=4,count=blockSize*blockCount;
        constexpr uint64_t metricBase=0x400000000;const auto table=metricBase-0x1000;
        large.put(state.database,4,uint32_t(10));large.put(state.database,8,uint32_t(blockSize));
        large.put(state.database,16,uint32_t(blockSize-1));large.put(state.database,24,table);large.put(state.database,32,table+blockCount*8);
        large.put(state.database,40,table+blockCount*8);
        for(unsigned b=0;b<blockCount;++b){
            const auto block=metricBase+uint64_t(b)*0x1000000;large.put(table,b*8,block);
            large.regions[block].assign(blockSize*0x4e8,0);
            for(unsigned row=0;row<blockSize;++row){
                const auto i=b*blockSize+row;const auto offset=size_t(row)*0x4e8;
                large.put(block,offset,track+uint64_t(i)*0x10000);
                large.put(block,offset+0x80,50.f);large.put(block,offset+0x84,30.f);
                large.put(block,offset+0x88,1000.+i);
            }
        }
        large.metric_block=metricBase;large.metric_block_size=blockSize*0x4e8;
        Network measured;
        if(!read_network(read,&large,state,true,measured)||measured.tracks.size()!=count)return 320;
        for(unsigned i=0;i<count;++i)if(measured.tracks[i].native_length_m!=1000.+i)return 321;
        if(large.metric_prefix_reads||large.metric_block_reads!=2||large.reads>=100)return 322;
        std::printf("full network metrics: %u tracks, %u process reads, %u per-row metric reads\n",count,large.reads,large.metric_prefix_reads);
        const auto reset=[&](Memory& fixture){fixture.reads=fixture.metric_prefix_reads=fixture.metric_block_reads=0;};
        for(bool identity:{false,true}){
            auto changed=large;reset(changed);
            (identity?changed.changing_metric_identity:changed.changing_metric)=metricBase+7*0x4e8;
            if(!read_network(read,&changed,state,true,measured))return 323;
            for(unsigned i=0;i<count;++i)if(measured.tracks[i].native_length_m!=(i==7?0.:1000.+i))return 324;
        }
        auto unreadable=large;reset(unreadable);unreadable.fail_metric_verification=true;
        unreadable.failed_metric_row=metricBase+11*0x4e8;
        if(!read_network(read,&unreadable,state,true,measured)||unreadable.metric_prefix_reads!=blockSize)return 325;
        for(unsigned i=0;i<count;++i)if(measured.tracks[i].native_length_m!=(i==11?0.:1000.+i))return 326;
        auto replaced=large;reset(replaced);replaced.replace_header=state.database;
        if(read_network(read,&replaced,state,true,measured)||!measured.tracks.empty())return 327;
        auto signalling=large;reset(signalling);
        if(!read_network(read,&signalling,state,true,measured,NetworkScope::Signalling)||signalling.metric_block_reads!=1||signalling.metric_prefix_reads)return 328;
    }
    {
        auto scoped=m;scoped.put(gb,0x30,int32_t(4));scoped.put(gb,0x38,uint64_t(77));
        Network local;
        scoped.fail=sb; // Stations are not dependencies of this scope.
        if(!read_signalling_network(read,&scoped,state,77,local)||local.tracks.size()!=1||local.signals.size()!=1)return 220;
        scoped.fail=0;scoped.put(gb,0x48,0.7);
        if(!read_signalling_network(read,&scoped,state,77,local)||local.signals[0].fraction!=0.7)return 221;
        scoped.put(tb,0,track+1);
        if(read_signalling_network(read,&scoped,state,77,local))return 222;
        scoped=m;scoped.put(gb,0x30,int32_t(4));scoped.put(gb,0x38,uint64_t(77));
        scoped.fail=tb;
        if(read_signalling_network(read,&scoped,state,77,local))return 223;
        if(!read_signalling_network(read,&scoped,state,99,local)||!local.tracks.empty()||!local.signals.empty())return 224;
    }
    {
        // Empty signal filters are independent of indirect tag arrays. Verify
        // them per block while retaining per-signal validity and fallback.
        auto filters=m;constexpr unsigned count=4096;
        constexpr uint64_t base=0x600000000,table=base-0x1000;
        const auto header=state.database+0x380;
        filters.put(header,4,uint32_t(12));filters.put(header,8,uint32_t(count));filters.put(header,16,uint32_t(count-1));
        filters.put(header,24,table);filters.put(header,32,table+8);filters.put(header,40,table+8);filters.put(table,0,base);
        filters.regions[base].resize(count*0xc8);
        for(unsigned i=0;i<count;++i){
            const auto offset=size_t(i)*0xc8;
            filters.put(base,offset,signal+uint64_t(i)*0x10000);filters.put(base,offset+0x30,int32_t(4));
            filters.put(base,offset+0x40,track);filters.put(base,offset+0x48,.5);filters.put(base,offset+0x50,int8_t(-1));
            filters.put(base,offset+0x70,uint32_t(i%2));
        }
        filters.metric_block=base;filters.metric_block_size=count*0xc8;Network captured;
        if(!read_network(read,&filters,state,true,captured)||captured.signals.size()!=count||filters.reads>=100)return 329;
        for(unsigned i=0;i<count;++i)if(!captured.signals[i].filter_available||captured.signals[i].filter_default_ignored!=bool(i%2))return 330;
        std::printf("full network filters: %u signals, %u process reads\n",count,filters.reads);
        for(bool identity:{false,true}){
            auto changed=filters;changed.metric_block_reads=0;
            (identity?changed.changing_metric_identity:changed.changing_metric)=base+7*0xc8;
            if(!read_network(read,&changed,state,true,captured))return 331;
            for(unsigned i=0;i<count;++i)if(captured.signals[i].filter_available!=(i!=7))return 332;
        }
        auto unreadable=filters;unreadable.metric_block_reads=0;unreadable.fail_metric_verification=true;
        unreadable.fail=base+11*0xc8+0x70;
        if(!read_network(read,&unreadable,state,true,captured))return 333;
        for(unsigned i=0;i<count;++i)if(captured.signals[i].filter_available!=(i!=11))return 334;
    }
    {
        // Thousands of signals share the same track-pool descriptors. The
        // capture must scale by unique records, while detecting a descriptor
        // or block-table replacement during the read itself.
        Memory large;
        large.put(state.module_base+0xb81998,0,state.root);
        large.put(state.root,0x540,state.database);large.put(state.root,0x5c0,state.copy);large.put(state.root,0x680,state.simulation);
        constexpr unsigned count=4096;
        constexpr uint64_t tracksBase=0x400000000,signalsBase=0x500000000;
        auto largePool=[&](uint64_t offset,uint64_t block,size_t stride){
            const auto header=state.database+offset,table=block-0x1000;
            large.regions[header].resize(48);large.put(header,4,uint32_t(12));large.put(header,8,uint32_t(count));large.put(header,16,uint32_t(count-1));
            large.put(header,24,table);large.put(header,32,table+8);large.put(header,40,table+8);large.put(table,0,block);
            large.regions[block].resize(count*stride);
        };
        largePool(0,tracksBase,0x4e8);largePool(0x380,signalsBase,0xc8);
        const auto tid=[&](unsigned i){return track+uint64_t(i)*0x10000;};
        for(unsigned i=0;i<count;++i){
            const auto t=size_t(i)*0x4e8,s=size_t(i)*0xc8;
            large.put(tracksBase,t,tid(i));large.put(tracksBase,t+8,i?tid(i-1):uint64_t(0));
            large.put(tracksBase,t+16,i+1<count?tid(i+1):uint64_t(0));
            large.put(signalsBase,s,signal+uint64_t(i)*0x10000);large.put(signalsBase,s+0x30,int32_t(4));large.put(signalsBase,s+0x38,uint64_t(77));
            large.put(signalsBase,s+0x40,tid(i));large.put(signalsBase,s+0x48,.5);large.put(signalsBase,s+0x50,int8_t(-1));
        }
        Network captured;const SignallingScope scope{77,2};
        if(!read_signalling_network(read,&large,state,std::span(&scope,1),captured)||captured.signals.size()!=count||captured.tracks.size()!=count)return 290;
        std::printf("signalling capture: %u signals, %u process reads\n",count,large.reads);
        if(large.reads>count*3)return 291;
        for(auto target:{state.database,tracksBase-0x1000}){
            auto changed=large;changed.changing_pool_on_track=tracksBase;changed.pool_target=target;
            if(read_signalling_network(read,&changed,state,std::span(&scope,1),captured))return 292;
        }
    }
    {
        auto minimal=m;minimal.fail=sb;
        Network signalling;
        if(read_network(read,&minimal,state,true,signalling))return 214;
        if(!read_network(read,&minimal,state,true,signalling,NetworkScope::Signalling)||
           signalling.tracks.size()!=1||signalling.signals.size()!=1||
           !signalling.stations.empty()||!signalling.platforms.empty())return 215;
        minimal.fail=tb;
        if(read_network(read,&minimal,state,true,signalling,NetworkScope::Signalling))return 216;
    }
    {
        // Accelerated-world regression: multi-model capture must retain two
        // upstream blocks without reading the unrelated remainder of the map.
        auto regional=m;
        const auto configurePool=[&](uint64_t header,uint64_t block,size_t stride){
            regional.put(header,4,uint32_t(3));regional.put(header,8,uint32_t(8));regional.put(header,16,uint32_t(7));
            regional.regions[block].assign(stride*8,0);
        };
        configurePool(state.database,tb,0x4e8);configurePool(state.database+0x380,gb,0xc8);
        const auto tid=[&](unsigned i){return track+uint64_t(i)*0x10000;};
        const auto sid=[&](unsigned i){return signal+uint64_t(i)*0x10000;};
        for(unsigned i=0;i<7;++i){
            const size_t offset=i*0x4e8;
            regional.put(tb,offset,tid(i));regional.put(tb,offset+8,i?tid(i-1):uint64_t(0));
            regional.put(tb,offset+16,i<6?tid(i+1):uint64_t(0));regional.put(tb,offset+0x30,double(i));
        }
        const unsigned positions[]{0,1,3,5};const uint64_t hashes[]{99,66,77,99};
        for(unsigned i=0;i<4;++i){
            const size_t offset=i*0xc8;regional.put(gb,offset,sid(i));regional.put(gb,offset+0x30,int32_t(4));
            regional.put(gb,offset+0x38,hashes[i]);regional.put(gb,offset+0x40,tid(positions[i]));
            regional.put(gb,offset+0x48,.5);regional.put(gb,offset+0x50,int8_t(-1));
        }
        Network local;
        const SignallingScope scopes[]{{77,2},{66,0}};
        regional.fail=tb+6*0x4e8;
        if(!read_signalling_network(read,&regional,state,scopes,local)||local.tracks.size()!=6||local.signals.size()!=4)return 260;
        const auto topologyFor=[](const Network& network){
            std::vector<nimby::TrackNode> nodes;std::vector<nimby::Signal> signals;
            for(const auto& t:network.tracks)nodes.emplace_back(NimbyTrackNode{t.id,t.links[0],t.links[1],t.x,t.y});
            for(const auto& s:network.signals)signals.emplace_back(NimbySignal{s.id,s.track_id,s.fraction,s.direction,s.kind});
            return nimby::SignalTopology(signals,nodes,{},nimby::SignalDirectionConvention::Forward);
        };
        const auto topology=topologyFor(local);
        if(nimby::approachedSignals(topology,{tid(0),.6,1},2)!=std::vector<nimby::Id>{sid(1),sid(2)})return 261;
        if(nimby::approachedSignals(topology,{tid(2),.5,1},2)!=std::vector<nimby::Id>{sid(2),sid(3)})return 262;
        {
            auto merge=regional;merge.fail=0;
            const auto child=tid(6),beyond=tid(7),children=uint64_t(0x600090000);
            merge.put(tb,6*0x4e8+8,uint64_t(0));merge.put(tb,6*0x4e8+16,beyond);
            merge.put(tb,6*0x4e8+0x3f0,tid(2));merge.put(tb,6*0x4e8+0x3f8,.4);
            merge.put(tb,6*0x4e8+0x400,int32_t(-1));
            merge.put(tb,7*0x4e8,beyond);merge.put(tb,7*0x4e8+8,child);
            merge.put(tb,2*0x4e8+0x408,children);merge.put(tb,2*0x4e8+0x410,children+8);
            merge.put(tb,2*0x4e8+0x418,children+8);merge.put(children,0,child);
            for(unsigned i=4;i<6;++i){
                merge.put(gb,i*0xc8,sid(i));merge.put(gb,i*0xc8+0x30,int32_t(4));
                merge.put(gb,i*0xc8+0x38,uint64_t(99));merge.put(gb,i*0xc8+0x40,tid(i+2));
                merge.put(gb,i*0xc8+0x48,.5);merge.put(gb,i*0xc8+0x50,int8_t(1));
            }
            Network merged;
            if(!read_signalling_network(read,&merge,state,scopes,merged)||merged.tracks.size()!=8||merged.junctions.size()!=1)return 268;
            std::vector<nimby::TrackNode> nodes;std::vector<nimby::Signal> signals;std::vector<nimby::TrackJunction> junctions;
            for(const auto& t:merged.tracks)nodes.emplace_back(NimbyTrackNode{t.id,t.links[0],t.links[1],t.x,t.y});
            for(const auto& s:merged.signals)signals.emplace_back(NimbySignal{s.id,s.track_id,s.fraction,s.direction,s.kind});
            for(const auto& j:merged.junctions)junctions.emplace_back(j);
            const nimby::SignalTopology joined(signals,nodes,junctions,nimby::SignalDirectionConvention::Forward);
            if(nimby::approachedSignals(joined,{beyond,.4,-1},2)!=std::vector<nimby::Id>{sid(4),sid(2)})return 269;
            if(nimby::approachedSignals(joined,{tid(0),.6,1},2)!=std::vector<nimby::Id>{sid(1),sid(2)})return 270;
        }
        const SignallingScope immediate[]{{77,1}};
        if(!read_signalling_network(read,&regional,state,immediate,local)||local.tracks.size()!=5)return 263;
        const SignallingScope invalid[]{{77,17}};
        if(read_signalling_network(read,&regional,state,invalid,local))return 264;
        const SignallingScope duplicate[]{{77,1},{77,2}};
        if(read_signalling_network(read,&regional,state,duplicate,local))return 265;
        // Same geometry in the opposite travel direction: upstream is reversed.
        for(unsigned i=0;i<4;++i)regional.put(gb,i*0xc8+0x50,int8_t(1));
        regional.put(gb,0x38,uint64_t(99));regional.put(gb,0xc8+0x38,uint64_t(77));
        regional.put(gb,2*0xc8+0x38,uint64_t(66));regional.fail=0;
        if(!read_signalling_network(read,&regional,state,scopes,local)||local.tracks.size()!=6)return 266;
        if(nimby::approachedSignals(topologyFor(local),{tid(4),.5,-1},2)!=std::vector<nimby::Id>{sid(2),sid(1)})return 267;
    }
    {
        bool found=false;
        auto membership=m;
        // Texture validation must not depend on unrelated tracks or station names.
        membership.fail=tb;
        if(!read_signal_membership(read,&membership,state,true,signal,found)||!found)return 190;
        if(!read_signal_membership(read,&membership,state,true,signal+1,found)||found)return 191;
        if(!read_signal_membership(read,&membership,state,true,signal+0x10000,found)||found)return 192;
        if(!read_signal_membership(read,&membership,state,true,signal+0x20000,found)||found)return 193;
        if(read_signal_membership(read,&membership,state,false,signal,found)||found)return 194;
        if(read_signal_membership(read,&membership,state,true,track,found)||found)return 195;
        membership.fail=gb;
        if(read_signal_membership(read,&membership,state,true,signal,found)||found)return 196;
        membership.fail=0;membership.replace_header=state.database+0x380;membership.header_reads=0;
        if(read_signal_membership(read,&membership,state,true,signal,found)||found)return 197;
        membership=m;membership.put(state.database+0x380,4,uint32_t(32));
        if(read_signal_membership(read,&membership,state,true,signal,found)||found)return 198;
        membership=m;membership.replace_header=gb;membership.header_reads=0;
        if(read_signal_membership(read,&membership,state,true,signal,found)||found)return 201;
        membership=m;membership.replace_header=gb-0x1000;membership.header_reads=0;
        if(read_signal_membership(read,&membership,state,true,signal,found)||found)return 202;
        membership=m;membership.put(state.root,0x540,state.database+0x10000);
        if(read_signal_membership(read,&membership,state,true,signal,found)||found)return 203;
        // A nonzero slot and block exercise the index split, beyond slot zero.
        membership=m;membership.put(gb,0xc8,signal+0x10000);
        if(!read_signal_membership(read,&membership,state,true,signal+0x10000,found)||!found)return 199;
        const auto table=gb-0x1000,secondBlock=gb+0x1000;
        membership.put(state.database+0x380,32,table+16);membership.put(state.database+0x380,40,table+16);
        membership.put(table,8,secondBlock);membership.put(secondBlock,0,signal+0x20000);
        if(!read_signal_membership(read,&membership,state,true,signal+0x20000,found)||!found)return 200;
    }
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
        {
            auto scoped=valid;scoped.put(gb,0x30,int32_t(4));scoped.put(gb,0x38,uint64_t(77));
            Network local;
            if(!read_signalling_network(read,&scoped,state,77,local)||local.junctions.size()!=1||local.tracks.size()!=2)return 225;
            scoped.put(tb,offset+0x3f8,std::nan(""));
            if(read_signalling_network(read,&scoped,state,77,local))return 226;
        }
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
    {
        // Many branches share one parent's vector. Read that vector twice,
        // then validate every object after all indirect data was collected.
        auto joined=m;constexpr unsigned count=128;
        constexpr uint64_t base=0x700000000,table=base-0x1000,children=0x710000000;
        joined.put(state.database,4,uint32_t(8));joined.put(state.database,8,uint32_t(256));joined.put(state.database,16,uint32_t(255));
        joined.put(state.database,24,table);joined.put(state.database,32,table+8);joined.put(state.database,40,table+8);joined.put(table,0,base);
        joined.regions[base].resize(256*0x4e8);joined.put(base,0,track);
        joined.put(base,0x408,children);joined.put(base,0x410,children+count*8);joined.put(base,0x418,children+count*8);
        for(unsigned i=1;i<=count;++i){
            const auto offset=size_t(i)*0x4e8,id=track+uint64_t(i)*0x10000;
            joined.put(base,offset,id);joined.put(base,offset+0x10,track);joined.put(base,offset+0x3f0,track);
            joined.put(base,offset+0x3f8,.4);joined.put(base,offset+0x400,int32_t(1));joined.put(children,(i-1)*8,id);
        }
        joined.watched_table=children;joined.watched_table_size=count*8;Network captured;
        if(!read_network(read,&joined,state,true,captured)||captured.junctions.size()!=count||joined.watched_table_reads!=2||joined.reads>=count*3)return 338;
        std::printf("shared track junctions: %u branches, %u process reads\n",count,joined.reads);
        auto branch=joined;branch.watched_table_reads=0;branch.changing_attachment=base+7*0x4e8;
        if(!read_network(read,&branch,state,true,captured)||captured.junctions.size()!=count-1)return 339;
        auto parent=joined;parent.watched_table_reads=0;parent.attachment_after_children=base+0x408;
        if(!read_network(read,&parent,state,true,captured)||!captured.junctions.empty())return 340;
        auto list=joined;list.watched_table_reads=0;list.changing_slots=children;list.slot_reads=0;
        if(!read_network(read,&list,state,true,captured)||!captured.junctions.empty())return 341;
        auto duplicate=joined;duplicate.watched_table_reads=0;duplicate.put(children,8,track+0x10000);
        if(!read_network(read,&duplicate,state,true,captured)||captured.junctions.size()!=count-2)return 342;
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
    {
        auto endpoints=m;endpoints.watched_table=points;endpoints.watched_table_size=32;
        if(!read_network(read,&endpoints,state,true,out)||endpoints.watched_table_reads!=2||std::strcmp(out.platforms[0].name_utf8,"14S"))return 335;
        endpoints.changing_slots=points;endpoints.slot_reads=0;
        if(!read_network(read,&endpoints,state,true,out)||out.platforms[0].flags)return 336;
        auto hole=m;hole.regions[points].resize(16);hole.put(points+64,0,0.);hole.put(points+64,8,10.);
        hole.put(tb,0x1b8,points+80);
        if(!read_network(read,&hole,state,true,out)||std::strcmp(out.platforms[0].name_utf8,"14S"))return 337;
    }
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
    {
        // Live regression: deleted trains kept inactive Motion records forever,
        // rejecting every capture and hiding all signal-extension checkboxes.
        const uint64_t retired=train+0x10000,modelSlot=trainBlock+0x178,motionSlot=motionBlock+0x638;
        for(bool signallingOnly:{false,true}){
            auto deleted=m;
            deleted.put(trainBlock,0x178,retired|0xffff000000000000ULL);
            deleted.put(motionBlock,0x638,retired);
            if(!read_trains(read,&deleted,state,true,trains,signallingOnly)||trains.size()!=1||trains[0].id!=train)return 240;
            // All three optionals matter, including a hidden train without Drive.
            for(size_t offset:{size_t(0x1d0),size_t(0x218),size_t(0x4b0)}){
                for(uint8_t value:{uint8_t(1),uint8_t(2)}){
                    auto active=deleted;active.put(motionBlock,0x638+offset,value);
                    if(read_trains(read,&active,state,true,trains,signallingOnly)||!trains.empty())return 241;
                }
                auto entering=deleted;entering.changing_retired_motion=motionSlot;entering.retired_flag_offset=offset;
                if(read_trains(read,&entering,state,true,trains,signallingOnly)||!trains.empty())return 242;
            }
            // Unknown/reused slots and other generations are never a deletion proof.
            for(uint64_t replacement:{uint64_t(0),retired+1,(retired+1)|0xffff000000000000ULL}){
                auto reused=deleted;reused.put(trainBlock,0x178,replacement);
                // A live replacement needs a valid name for the full capture.
                reused.put(trainBlock,0x178+0x28,uint64_t(15));
                if(read_trains(read,&reused,state,true,trains,signallingOnly)||!trains.empty())return 243;
            }
            auto unstable=deleted;unstable.changing_retired_model=modelSlot;
            if(read_trains(read,&unstable,state,true,trains,signallingOnly)||!trains.empty())return 244;
            unstable=deleted;unstable.changing_identity=motionSlot;
            if(read_trains(read,&unstable,state,true,trains,signallingOnly)||!trains.empty())return 245;
            unstable=deleted;unstable.fail=modelSlot;
            if(read_trains(read,&unstable,state,true,trains,signallingOnly)||!trains.empty())return 246;
            unstable=deleted;unstable.fail=motionSlot;
            if(read_trains(read,&unstable,state,true,trains,signallingOnly)||!trains.empty())return 247;
            unstable=deleted;unstable.replace_header=state.database+0x200;unstable.header_reads=0;
            if(read_trains(read,&unstable,state,true,trains,signallingOnly)||!trains.empty())return 248;
        }
    }
    m.put(motionBlock,0,train+1);if(read_trains(read,&m,state,true,trains)||!trains.empty())return 15;
    m.put(motionBlock,0,train);m.put(trainBlock,0x20,uint64_t(257));if(read_trains(read,&m,state,true,trains))return 16;
    // Un nom corrompu n'empeche pas de verifier la presence pour le BAL.
    if(!read_trains(read,&m,state,true,trains,true)||trains.size()!=1||
       trains[0].service.flags!=NIMBY_SERVICE_PRESENCE_VALID||trains[0].speed_available||
       !trains[0].name.empty()||trains[0].path_available||!trains[0].positioned||trains[0].position.fraction!=.25)return 210;
    {
        // A moving train must remain visible to two-block approach detection.
        // Formerly any changed fraction erased its head until a stable/stopped
        // sample happened to arrive, opening a resting signal too late.
        for(int8_t direction:{int8_t(1),int8_t(-1)}){
            auto moving=m;moving.changing_head=motionBlock;
            moving.put(motionBlock,0x3b0,direction==1?.25:.75);
            moving.put(motionBlock,0x3b8,direction);
            moving.latest_head={track,direction==1?.30:.70,direction};
            if(!read_trains(read,&moving,state,true,trains,true)||trains.size()!=1||!trains[0].positioned){
                std::puts("FAIL: signalling lost the head of a moving train");return 249;
            }
            if(trains[0].position.fraction!=moving.latest_head.fraction)return 250;
            const std::vector<nimby::TrackNode> nodes{nimby::TrackNode{{track,0,0,0,0}}};
            const std::vector<nimby::Signal> signals{
                nimby::Signal{{80,track,direction==1?.40:.60,-direction,NIMBY_SIGNAL_PATH}},
                nimby::Signal{{81,track,direction==1?.60:.40,-direction,NIMBY_SIGNAL_PATH}}};
            const nimby::SignalTopology topology(signals,nodes,{},nimby::SignalDirectionConvention::Forward);
            auto approaches=[&]{const auto& head=trains[0].position;
                return nimby::approachedSignals(topology,{head.track_id,head.fraction,head.direction},2);};
            if(approaches()!=std::vector<nimby::Id>({80,81}))return 251;
            moving.latest_head.fraction=direction==1?.45:.55;
            if(!read_trains(read,&moving,state,true,trains,true)||!trains[0].positioned||
               approaches()!=std::vector<nimby::Id>({81}))return 252;
            moving.latest_head.fraction=direction==1?.65:.35;
            if(!read_trains(read,&moving,state,true,trains,true)||!trains[0].positioned||!approaches().empty())return 253;
            // Movement does not relax identity, direction or position validity.
            for(const TrainPosition invalid:std::vector<TrainPosition>{
                {track,direction==1?.20:.80,direction}, {track,.5,-direction},
                {track+1,.5,direction}, {track,std::nan(""),direction}, {track,1.1,direction}}){
                moving.latest_head=invalid;
                if(!read_trains(read,&moving,state,true,trains,true)||trains[0].positioned)return 254;
            }
            moving.latest_head={track,.5,direction};moving.changing_identity=motionBlock;
            if(!read_trains(read,&moving,state,true,trains,true)||trains[0].positioned)return 255;
            moving.changing_identity=0;moving.put(motionBlock,0x4b0,uint8_t(0));
            if(!read_trains(read,&moving,state,true,trains,true)||trains[0].positioned)return 256;
        }
    }
    m.put(motionBlock,0,train+1);
    if(read_trains(read,&m,state,true,trains,true))return 211;
    m.put(motionBlock,0,train);
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
    if(!read_trains(read,&m,state,true,trains)||trains.size()!=1||trains[0].speed_available||trains[0].service.flags)return 42;
    if(!read_trains(read,&m,state,true,trains,true)||trains[0].service.flags)return 212;
    // Already physically present: starting Drive must not invalidate membership.
    m.put(motionBlock,0x1d0,uint8_t(1));
    if(!read_trains(read,&m,state,true,trains)||trains[0].speed_available||
       trains[0].service.flags!=NIMBY_SERVICE_PRESENCE_VALID)return 206;
    if(!read_trains(read,&m,state,true,trains,true)||
       trains[0].service.flags!=NIMBY_SERVICE_PRESENCE_VALID)return 213;
    m.put(motionBlock,0x1d0,uint8_t(0));
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
    if(!read_trains(read,&m,state,true,trains)||trains[0].service.flags!=NIMBY_SERVICE_PRESENCE_VALID||!trains[0].speed_available)return 84;
    m.changing_service=0;
    m.changing_presence=motionBlock;
    m.put(motionBlock,0x218,uint8_t(0)); // Test a real membership change, not hidden presence bookkeeping.
    if(!read_trains(read,&m,state,true,trains)||trains[0].service.flags)return 204;
    m.changing_presence=0;m.changing_identity=motionBlock;
    if(!read_trains(read,&m,state,true,trains)||trains[0].service.flags)return 205;
    m.changing_identity=0;
    {
        // Many trains share one immutable line plan in a capture. The public
        // per-train results remain owned copies, with the existing total budget.
        auto large=m;
        constexpr uint32_t trainCount=256,stopCount=2048;
        constexpr uint64_t models=0x710000000,motions=0x720000000,stops=0x730000000;
        auto sharedPool=[&](uint64_t header,uint64_t block,size_t stride){
            large.regions[header].resize(48);
            large.put(header,4,uint32_t(8));large.put(header,8,trainCount);large.put(header,16,trainCount-1);
            large.put(header,24,block-0x1000);large.put(header,32,block-0x1000+8);large.put(header,40,block-0x1000+8);
            large.put(block-0x1000,0,block);large.regions[block].resize(stride*trainCount);
        };
        sharedPool(state.database+0x200,models,0x178);sharedPool(state.simulation+0xa0,motions,0x638);
        for(uint32_t i=0;i<trainCount;++i){
            const auto id=train+(uint64_t(i)<<16);
            large.put(models,i*0x178,id);large.put(models,i*0x178+0x28,uint64_t(15));
            large.put(motions,i*0x638,id);large.put(motions,i*0x638+0x5d0,uint8_t(1));
            large.put(motions,i*0x638+0x5a8,line);large.put(motions,i*0x638+0x5c8,int32_t(i));
        }
        large.put(lineBlock,0x118,stops);large.put(lineBlock,0x120,stops+stopCount*0x158);large.put(lineBlock,0x128,stops+stopCount*0x158);
        large.regions[stops].resize(stopCount*0x158);
        for(uint32_t i=0;i<stopCount;++i){
            large.put(stops,i*0x158+0x78,track);large.put(stops,i*0x158+0x110,station);
            large.put(stops,i*0x158+0xb8,int32_t(i*10));large.put(stops,i*0x158+0xbc,int32_t(i*10+5));
        }
        large.watched_table=stops;large.watched_table_size=stopCount*0x158;
        const auto started=std::chrono::steady_clock::now();
        if(!read_trains(read,&large,state,true,trains)||trains.size()!=trainCount)return 257;
        const auto elapsed=std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now()-started).count();
        for(const auto& value:trains)if(!value.line_stops_available||value.line_stops.size()!=stopCount||
            value.line_stops.back().departure_offset_seconds!=20475)return 258;
        std::printf("Shared line: trains=%u stops=%u full_plan_reads=%u bytes=%llu elapsed_us=%lld\n",trainCount,stopCount,
            large.watched_table_reads,static_cast<unsigned long long>(large.watched_table_bytes),static_cast<long long>(elapsed));
        if(large.watched_table_reads>3)return 259;
        for(const auto [changed,offset]:std::array<std::pair<uint64_t,size_t>,2>{{
            {stops,(stopCount-1)*0x158+0x110},{lineBlock,0}}}){
            auto edited=large;edited.watched_table_reads=0;edited.change_after_plan=changed;edited.change_after_plan_offset=offset;
            if(!read_trains(read,&edited,state,true,trains)||trains.size()!=trainCount)return 260;
            for(const auto& value:trains)if(value.line_stops_available||!value.line_stops.empty()||
                (value.service.flags&(NIMBY_SERVICE_STOP_VALID|NIMBY_SERVICE_LINE_VALID)))return 261;
        }
        auto inconsistent=large;inconsistent.watched_table_reads=0;inconsistent.different_shared_stop=stops+0x158;
        if(!read_trains(read,&inconsistent,state,true,trains))return 262;
        for(const auto& value:trains)if(value.line_stops_available||value.service.flags&NIMBY_SERVICE_STOP_VALID)return 263;
        // A same-slot, different-generation line must not reuse cached data.
        auto generation=large;generation.watched_table_reads=0;generation.put(motions,0x638+0x5a8,line+1);
        if(!read_trains(read,&generation,state,true,trains)||trains[1].line_stops_available||
           trains[1].service.flags&NIMBY_SERVICE_LINE_VALID||!trains[0].line_stops_available)return 264;
        const auto earlier=trains[0].line_stops;
        generation.put(stops,(stopCount-1)*0x158+0x110,station+1);generation.watched_table_reads=0;
        if(!read_trains(read,&generation,state,true,trains)||!trains[0].line_stops_available||
           trains[0].line_stops.back().station_id!=station+1||earlier.back().station_id!=station)return 265;
        // The shared read must not turn the existing per-train copy quota into
        // an unbounded N(trains) * N(stops) output allocation.
        auto budgeted=large;constexpr size_t manyStops=8192;
        budgeted.put(lineBlock,0x120,stops+manyStops*0x158);budgeted.put(lineBlock,0x128,stops+manyStops*0x158);
        budgeted.regions[stops].resize(manyStops*0x158);
        for(size_t i=stopCount;i<manyStops;++i){budgeted.put(stops,i*0x158+0x78,track);budgeted.put(stops,i*0x158+0x110,station);}
        if(!read_trains(read,&budgeted,state,true,trains))return 266;
        size_t copied=0,available=0;for(const auto& value:trains){copied+=value.line_stops.size();available+=value.line_stops_available;}
        if(copied!=1048576||available!=128||!(trains.back().service.flags&NIMBY_SERVICE_STOP_VALID))return 267;
    }
    std::puts("Synthetic network: full ID joins, pool replacement, read failure, unknown version, invalid position and limits rejected.");
}
