// Real in-game bridge entry points with synthetic roots; no hook installed.
#include "../../src/platform/windows/runtime/texture_bridge.cpp"
#include <stdexcept>
#include <iostream>
#include <future>
#include <chrono>
#include <thread>
#define CHECK(x) do{if(!(x))throw std::runtime_error("line "+std::to_string(__LINE__)+": " #x);}while(false)
int main(){try{
    using namespace nimby::texture_bridge;
    std::vector<unsigned char> module(0xb81998+8),root(0x688),databaseBytes(0xb00),simulationBytes(0x40),nextDatabase(0xb00);Shared wire;shared=&wire;
    base=reinterpret_cast<uintptr_t>(module.data());
    const uintptr_t rootAddress=reinterpret_cast<uintptr_t>(root.data()),database=reinterpret_cast<uintptr_t>(databaseBytes.data()),simulation=reinterpret_cast<uintptr_t>(simulationBytes.data()),copy=0x45670000;
    std::memcpy(module.data()+0xb81998,&rootAddress,8);std::memcpy(root.data()+0x540,&database,8);std::memcpy(root.data()+0x680,&simulation,8);
    std::memcpy(root.data()+0x5c0,&copy,8);databaseBytes[0xa48]=1;
    nimby::engine::SimulationClock clock{123456,10};std::memcpy(simulationBytes.data()+0x20,&clock,sizeof clock);
    auto begin=[&](uint64_t db){Mailbox request;request.operation=4;request.request_database=db;request.request_simulation=simulation;
        CHECK(NimbyTexture_Execute(&request)==NIMBY_OK&&request.result_generation);return request.result_generation;};
    auto publish=[&](uint64_t owner,uint64_t db,uint64_t sim,const Command* rows,uint32_t count){
        if(db!=database&&db!=reinterpret_cast<uintptr_t>(nextDatabase.data()))return uint32_t{NIMBY_DATA_UNAVAILABLE};
        return NimbyTexture_PublishV2(owner,db,sim,begin(db),rows,count);};
    const auto now=GetTickCount64();const uint64_t a=0x8000000000001,b=0x8000000000002;
    const Command first{a,123,now+2500,1,2,500},second{b,456,now+2500,3};
    CHECK(publish(1,database,simulation,&first,1)==NIMBY_OK);
    CHECK(publish(2,database,simulation,&second,1)==NIMBY_OK);
    CHECK(publish(3,database,simulation,&first,1)==NIMBY_RESOURCE_LIMIT&&publications.snapshot()->table.count()==2);
    CHECK(publish(1,database+1,simulation,&first,1)==NIMBY_DATA_UNAVAILABLE&&publications.snapshot()->table.count()==2);
    const uint64_t ids[]{a,b};NimbySignalTextureOverrideStatus out[2]{};
    CHECK(NimbyTexture_Statuses(database,simulation,ids,2,out)==NIMBY_OK);
    CHECK(out[0].active&&out[1].active&&out[0].active_count==2&&out[0].index==1&&out[1].index==3);
    Mailbox legacy;legacy.operation=2;legacy.request_signal=a;legacy.request_database=database;legacy.request_simulation=simulation;
    CHECK(NimbyTexture_Execute(&legacy)==NIMBY_RESOURCE_LIMIT);
    const auto beforeForeignClear=publications.snapshot();
    CHECK(NimbyTexture_Clear(2,&a,1)==NIMBY_OK&&publications.snapshot()==beforeForeignClear);
    CHECK(NimbyTexture_Release(3)==NIMBY_OK&&publications.snapshot()->table.count()==2);
    const uint64_t mixedClear[]{a,b,b+1};
    CHECK(NimbyTexture_Clear(1,mixedClear,3)==NIMBY_OK&&publications.snapshot()->table.count()==1);
    CHECK(publications.read(b).command.owner==2&&publications.read(b).command.index==3);
    const auto beforeRepeatedClear=publications.snapshot();
    CHECK(NimbyTexture_Clear(1,mixedClear,3)==NIMBY_OK&&publications.snapshot()==beforeRepeatedClear);
    CHECK(NimbyTexture_Release(1)==NIMBY_OK&&publications.snapshot()->table.count()==1&&publications.snapshot()->table.entries[0].signal==b);
    CHECK(NimbyTexture_Clear(2,&b,1)==NIMBY_OK&&publications.snapshot()->table.count()==0);
    // Exercise the real render callback. Its native fallback has a different
    // address: a busy publication must keep the previous restrictive image.
    static std::array<unsigned char,0x90> texture;
    static std::array<unsigned char,4*0x50> files;
    auto fileBegin=reinterpret_cast<uintptr_t>(files.data()),fileEnd=fileBegin+files.size();
    std::memcpy(texture.data()+0x78,&fileBegin,8);std::memcpy(texture.data()+0x80,&fileEnd,8);
    static unsigned nativeLookups=0;
    original=+[](uintptr_t,int,uint64_t hash)->uintptr_t{++nativeLookups;return hash==123?reinterpret_cast<uintptr_t>(texture.data()):0x7777;};
    std::array<uint64_t,8> record{};record[0]=a;record[6]=1;record[7]=999;
    const Command solid{a,123,now+60000,1};
    CHECK(publish(1,database,simulation,&solid,1)==NIMBY_OK);
    // Both paths must choose the same frame; an own-catalogue override only
    // needs the first native lookup, a foreign catalogue still needs both.
    for(const auto catalogue:{uint64_t{123},uint64_t{999}}){
        record[7]=catalogue;nativeLookups=0;
        const auto rendered=texture_select(database+0xa80,1,catalogue,reinterpret_cast<uintptr_t>(record.data()),base+0x62062b);
        uintptr_t selected{};CHECK(rendered!=0x7777);
        std::memcpy(&selected,reinterpret_cast<const void*>(rendered+0x78),8);
        CHECK(selected==fileBegin+0x50&&nativeLookups==(catalogue==123?1u:2u));
    }
    record[7]=999;
    std::promise<void> preparing,resume;
    auto resumed=resume.get_future().share();
    auto update=std::async(std::launch::async,[&]{return publications.update(1,[&](auto& state){
        state.table.entries.front().index=2;preparing.set_value();resumed.wait();return uint32_t{NIMBY_OK};
    });});
    preparing.get_future().wait();
    bool preserved=true;
    const auto started=std::chrono::steady_clock::now();
    for(unsigned i=0;i<10000;++i){
        const auto rendered=texture_select(database+0xa80,1,999,reinterpret_cast<uintptr_t>(record.data()),base+0x62062b);
        uintptr_t selected{};if(rendered==0x7777){preserved=false;break;}
        std::memcpy(&selected,reinterpret_cast<const void*>(rendered+0x78),8);
        if(selected!=fileBegin+0x50){preserved=false;break;}
    }
    const auto renderUs=std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now()-started).count();
    resume.set_value();CHECK(update.get()==NIMBY_OK);CHECK(preserved);
    std::cout<<"10000 native renders keep prior image while writer is paused: "<<renderUs<<" us\n";
    // Same heap pointers: a different world value retires every old command,
    // including a client which started decoding before that change.
    const auto oldGeneration=begin(database);
    databaseBytes[0xa48]=2;
    CHECK(texture_select(database+0xa80,1,999,reinterpret_cast<uintptr_t>(record.data()),base+0x62062b)==0x7777);
    databaseBytes[0xa48]=1; // Restoring the old bytes cannot revive its generation.
    CHECK(texture_select(database+0xa80,1,999,reinterpret_cast<uintptr_t>(record.data()),base+0x62062b)==0x7777);
    CHECK(NimbyTexture_PublishV2(1,database,simulation,oldGeneration,&solid,1)==NIMBY_DATA_UNAVAILABLE);
    CHECK(publish(1,database,simulation,&solid,1)==NIMBY_OK);
    // Detect a real rewind against the most recent rendered tick, not only the
    // initial publication (10): 50 -> 40 must retire even though 40 > 10.
    clock.ticks=50;std::memcpy(simulationBytes.data()+0x20,&clock,sizeof clock);
    CHECK(texture_select(database+0xa80,1,999,reinterpret_cast<uintptr_t>(record.data()),base+0x62062b)!=0x7777);
    clock.ticks=40;std::memcpy(simulationBytes.data()+0x20,&clock,sizeof clock);
    CHECK(texture_select(database+0xa80,1,999,reinterpret_cast<uintptr_t>(record.data()),base+0x62062b)==0x7777);
    clock.ticks=60;std::memcpy(simulationBytes.data()+0x20,&clock,sizeof clock);
    CHECK(texture_select(database+0xa80,1,999,reinterpret_cast<uintptr_t>(record.data()),base+0x62062b)==0x7777);
    CHECK(NimbyTexture_Statuses(database,simulation,ids,2,out)==NIMBY_OK&&!out[0].active&&!out[1].active);
    // Legacy permanent force uses exactly the same ticket and retirement fence.
    Mailbox force;force.operation=1;force.request_signal=a;force.request_hash=123;force.request_expiry=UINT64_MAX;
    force.request_index=1;force.request_database=database;force.request_simulation=simulation;force.request_generation=begin(database);
    CHECK(NimbyTexture_Execute(&force)==NIMBY_OK);
    CHECK(texture_select(database+0xa80,1,999,reinterpret_cast<uintptr_t>(record.data()),base+0x62062b)!=0x7777);
    clock.ticks=59;std::memcpy(simulationBytes.data()+0x20,&clock,sizeof clock);
    CHECK(texture_select(database+0xa80,1,999,reinterpret_cast<uintptr_t>(record.data()),base+0x62062b)==0x7777);
    CHECK(NimbyTexture_Execute(&force)==NIMBY_DATA_UNAVAILABLE);
    // Reads which overlap have no trustworthy temporal order, in either
    // completion order. Only a subsequent non-overlapping rewind invalidates.
    for(bool reverse:{false,true}){
        RenderClock ordered(10);const auto one=ordered.begin(),two=ordered.begin();
        CHECK(!ordered.rewind(reverse?two:one,50));
        CHECK(!ordered.rewind(reverse?one:two,40));
        CHECK(!ordered.rewind(ordered.begin(),50));
        CHECK(ordered.rewind(ordered.begin(),40));
    }
    const auto previousWorld=begin(database);
    nextDatabase[0xa48]=3;
    const auto changedDatabase=reinterpret_cast<uintptr_t>(nextDatabase.data());std::memcpy(root.data()+0x540,&changedDatabase,8);
    CHECK(texture_select(database+0xa80,1,999,reinterpret_cast<uintptr_t>(record.data()),base+0x62062b)==0x7777);
    CHECK(NimbyTexture_PublishV2(1,database,simulation,previousWorld,&solid,1)==NIMBY_DATA_UNAVAILABLE);
    CHECK(publish(1,changedDatabase,simulation,&solid,1)==NIMBY_OK);
    CHECK(publications.snapshot()->table.count()==1&&publications.snapshot()->database==changedDatabase);
    const auto healthyWorld=publications.snapshot()->world;
    CHECK(worlds.current(healthyWorld->generation));
    CHECK(NimbyTexture_PublishV2(1,database,simulation,previousWorld,&solid,1)==NIMBY_DATA_UNAVAILABLE);
    CHECK(worlds.current(healthyWorld->generation)&&publications.snapshot()->world==healthyWorld);
    // Full command validation also checks in-place history changes with an
    // unchanged world value, vector bounds and heap roots.
    std::array<unsigned char,32> history{};history[0]=1;
    const auto historyBegin=reinterpret_cast<uintptr_t>(history.data()),historyEnd=historyBegin+history.size();
    std::memcpy(nextDatabase.data()+0xa68,&historyBegin,8);std::memcpy(nextDatabase.data()+0xa70,&historyEnd,8);
    std::memcpy(nextDatabase.data()+0xa78,&historyEnd,8);
    const auto historyTicket=begin(changedDatabase);history[0]=2;
    CHECK(NimbyTexture_PublishV2(1,changedDatabase,simulation,historyTicket,&solid,1)==NIMBY_DATA_UNAVAILABLE);
    CHECK(publish(1,changedDatabase,simulation,&solid,1)==NIMBY_OK);
    const auto beforeUnavailable=publications.snapshot()->world->generation;
    const uint64_t missing=0;std::memcpy(root.data()+0x5c0,&missing,8);
    Mailbox unavailable;unavailable.operation=4;unavailable.request_database=changedDatabase;unavailable.request_simulation=simulation;
    CHECK(NimbyTexture_Execute(&unavailable)==NIMBY_DATA_UNAVAILABLE);
    std::memcpy(root.data()+0x5c0,&copy,8);
    CHECK(!worlds.current(beforeUnavailable));
    CHECK(texture_select(changedDatabase+0xa80,1,999,reinterpret_cast<uintptr_t>(record.data()),base+0x62062b)==0x7777);
    CHECK(publish(1,changedDatabase,simulation,&solid,1)==NIMBY_OK);
    shared=nullptr;
    std::cout<<"PASS actual texture bridge ownership, world guards, bulk statuses and scoped cleanup\n";
}catch(const std::exception& e){std::cerr<<e.what()<<'\n';return 1;}}
