#pragma once
// Actual observation ABI/engine over deterministic process memory. The public
// and runtime captures see the SAME world; timings are offline fixture costs,
// not a promise about game latency, OS ReadProcessMemory or worker scheduling.
namespace {
struct SignallingWorld {
    std::shared_ptr<fixture::World> world;
    std::shared_ptr<fixture::Connection> connection;
    static constexpr uint64_t trainBlock=0x80000000,motionBlock=0x81000000,
        signalBlock=0x82000000,trackBlock=0x83000000,catalogNode=0x84000000,
        catalogBuckets=0x84100000,catalogFile=0x84200000,occupationKeys=0x85000000,
        occupationValues=0x86000000,occupationBuckets=0x87000000,
        renderQuery=0x88000000,renderControls=0x88100000,renderValues=0x88200000;
    static constexpr uint64_t trackId=0x1000000000001,signalId=0x8000000000001;
    uint64_t hash=nimby::engine::detail::textureNameHash("test_atlas");
    explicit SignallingWorld(uint32_t pid,unsigned trains,unsigned signals=1)
        :world(fixture::world(pid)),connection(fixture::connection(pid)){
        pool(fixture::db+0x200,trainBlock,0x178,trains,5);
        pool(fixture::simulation+0xa0,motionBlock,0x638,trains,5);
        pool(fixture::db+0x380,signalBlock,0xc8,signals,8);
        pool(fixture::db,trackBlock,0x4e8,1,1);pool(fixture::db+0x80,0x89000000,0x2c8,0,2);
        world->put(trackBlock,0x88,1000.);
        for(unsigned i=0;i<trains;++i){
            const auto offset=size_t(i)*0x638;
            world->put(motionBlock,offset+0x1d0,uint8_t(1));world->put(motionBlock,offset+0x4b0,uint8_t(1));
            world->put(motionBlock,offset+0x3a8,trackId);world->put(motionBlock,offset+0x3b0,.25);world->put(motionBlock,offset+0x3b8,int8_t(1));
        }
        for(unsigned i=0;i<signals;++i){
            const auto offset=size_t(i)*0xc8;
            world->put(signalBlock,offset+0x30,int32_t(4));world->put(signalBlock,offset+0x40,trackId);
            world->put(signalBlock,offset+0x48,.5);world->put(signalBlock,offset+0x50,int8_t(1));
        }
        const auto catalog=fixture::db+0xa80+0x138;
        world->regions[catalog].resize(32);world->put(catalog,8,catalogBuckets);
        world->put(catalog,16,uint64_t(257));world->put(catalog,24,uint64_t(1));
        world->regions[catalogBuckets].resize(258*8);world->put(catalogBuckets,(hash%257)*8,catalogNode);
        world->regions[catalogNode].resize(0xa0);world->put(catalogNode,0,hash);
        text(catalogNode,8,"test_atlas");world->put(catalogNode,0x80,catalogFile);
        world->put(catalogNode,0x88,catalogFile+0x50);world->put(catalogNode,0x90,catalogFile+0x50);
        world->regions[catalogFile].resize(0x50);world->put(catalogFile,0,int32_t(1));
        text(catalogFile,8,"fixture");text(catalogFile,0x28,"tex00.svg");
        // Spare native capacity is real work for existing readers, even if
        // most slots are unused. Only the first trainCount rows are occupied.
        const unsigned slots=std::max(1u,trains*16);
        const auto occupations=fixture::simulation+0x2c8;
        world->regions[occupations].resize(80);world->put(occupations,0,occupationKeys);
        world->put(occupations,8,occupationKeys+uint64_t(slots)*8);world->put(occupations,16,occupationKeys+uint64_t(slots)*8);
        world->put(occupations,24,occupationValues);world->put(occupations,32,occupationValues+uint64_t(slots)*32);
        world->put(occupations,40,occupationValues+uint64_t(slots)*32);
        world->put(occupations,56,occupationBuckets);world->put(occupations,64,uint64_t(1));
        world->regions[occupationKeys].resize(size_t(slots)*8);world->regions[occupationValues].resize(size_t(slots)*32);
        world->regions[occupationBuckets].resize(8);
        for(unsigned i=0;i<trains;++i){
            world->put(occupationKeys,size_t(i)*8,trackId);
            world->put(occupationValues,size_t(i)*32,fullId(5,i));
            world->put(occupationValues,size_t(i)*32+8,.2);world->put(occupationValues,size_t(i)*32+16,.25);
        }
        world->put(fixture::simulation+0x2200,0,renderQuery);world->regions[renderQuery+0x378].resize(32);
        constexpr uint64_t mask=65535;
        world->put(renderQuery+0x378,0,renderControls);world->put(renderQuery+0x378,8,renderValues);
        world->put(renderQuery+0x378,16,uint64_t(signals));world->put(renderQuery+0x378,24,mask);
        world->regions[renderControls].assign(mask,255);world->regions[renderValues].resize(mask*16);
        for(unsigned i=0;i<signals;++i){
            world->regions[renderControls][i]=0;world->put(renderValues,size_t(i)*16,fullId(8,i));
        }
    }
    static uint64_t fullId(uint64_t tag,unsigned slot){return (tag<<48)|(uint64_t(slot)<<16)|1;}
    void text(uint64_t address,size_t offset,const char* text){
        const auto size=std::strlen(text);CHECK(size<=15);
        auto& bytes=world->regions[address];if(bytes.size()<offset+32)bytes.resize(offset+32);
        std::memcpy(bytes.data()+offset,text,size+1);world->put(address,offset+16,uint64_t(size));world->put(address,offset+24,uint64_t(15));
    }
    void pool(uint64_t descriptor,uint64_t block,size_t stride,unsigned count,uint64_t tag){
        constexpr unsigned size=512;const auto blocks=(count+size-1)/size;const auto table=block-0x10000;
        world->regions[descriptor].resize(48);world->put(descriptor,4,uint32_t(9));
        world->put(descriptor,8,uint32_t(size));world->put(descriptor,16,uint32_t(size-1));
        if(!blocks)return;
        world->put(descriptor,24,table);world->put(descriptor,32,table+blocks*8);world->put(descriptor,40,table+blocks*8);
        world->regions[table].resize(blocks*8);
        world->regions[block].resize(size_t(blocks)*size*stride);
        for(unsigned b=0;b<blocks;++b)world->put(table,size_t(b)*8,block+size_t(b)*size*stride);
        for(unsigned i=0;i<count;++i)world->put(block,size_t(i)*stride,fullId(tag,i));
    }
    NimbySnapshot capture(NimbySession session,bool runtime,uint32_t expected=NIMBY_OK){
        NimbySignalCaptureScope scope{};std::strcpy(scope.texture_set,"test_atlas");
        NimbySnapshot snapshot=123;uint32_t stage=123;
        const auto status=runtime?NimbyInternal_CaptureRuntimeSignallingScope(session,&scope,1,&snapshot,&stage):
            NimbyInternal_CaptureSignallingScope(session,&scope,1,&snapshot,&stage);
        CHECK(status==expected);if(status!=NIMBY_OK)CHECK(snapshot==0);return snapshot;
    }
    void selected(bool present){world->put(signalBlock,0x38,present?hash:uint64_t(0));}
};
void runtimeEmptySignallingContract(){
    Session session(114);SignallingWorld memory(114,500,10);uint32_t count{};
    auto old=memory.capture(session.value,false);
    std::vector<NimbyTrain> oldTrains(500);std::vector<NimbyTrainService> oldServices(500);
    CHECK(NimbyInternal_CopyTrains(old,oldTrains.data(),500,&count)==NIMBY_OK&&count==500);
    CHECK(NimbyInternal_CopyTrainServices(old,oldServices.data(),500,&count)==NIMBY_OK&&count==500);
    CHECK(NimbyInternal_CopyTrackOccupations(old,nullptr,0,&count)==NIMBY_OK&&count==500);
    CHECK(oldTrains.front().flags&NIMBY_TRAIN_ACTIVE_DRIVE);
    CHECK(oldServices.front().flags&NIMBY_SERVICE_PRESENCE_VALID);
    const auto beforeOccupations=memory.connection->occupationReads.load(),beforeRender=memory.connection->renderReads.load();
    const auto beforeTrains=memory.connection->trainPoolReads.load();
    auto filtered=memory.capture(session.value,true);
    CHECK(memory.connection->occupationReads==beforeOccupations&&memory.connection->renderReads==beforeRender);
    CHECK(memory.connection->trainPoolReads>beforeTrains); // Global control roster retained.
    std::vector<NimbyTrain> trains(500);std::vector<NimbyTrainService> services(500);
    CHECK(NimbyInternal_CopyTrains(filtered,trains.data(),500,&count)==NIMBY_OK&&count==500);
    CHECK(NimbyInternal_CopyTrainServices(filtered,services.data(),500,&count)==NIMBY_OK&&count==500);
    CHECK(std::memcmp(trains.data(),oldTrains.data(),trains.size()*sizeof(NimbyTrain))==0);
    CHECK(std::memcmp(services.data(),oldServices.data(),services.size()*sizeof(NimbyTrainService))==0);
    CHECK(NimbyInternal_CopyTrackOccupations(filtered,nullptr,0,&count)==NIMBY_DATA_UNAVAILABLE&&count==0);
    CHECK(NimbyInternal_CopySignals(filtered,nullptr,0,&count)==NIMBY_OK&&count==0);
    NimbySimulationClock clock{};clock.struct_size=sizeof clock;NimbyGameSession game{};game.struct_size=sizeof game;
    CHECK(NimbyInternal_GetSimulationClock(filtered,&clock)==NIMBY_OK&&clock.ticks==10000);
    CHECK(NimbyInternal_GetGameSession(filtered,&game)==NIMBY_OK&&game.generation);
    CHECK(NimbyInternal_ReleaseSnapshot(old)==NIMBY_OK&&NimbyInternal_ReleaseSnapshot(filtered)==NIMBY_OK);
    // Fresh discovery each time: adding the selected model immediately restores
    // ordinary occupations and states; removing it returns to unavailable.
    memory.selected(true);old=memory.capture(session.value,false);filtered=memory.capture(session.value,true);
    NimbySnapshotInfo oldInfo{},info{};oldInfo.struct_size=sizeof oldInfo;info.struct_size=sizeof info;
    CHECK(NimbyInternal_GetSnapshotInfo(old,&oldInfo)==NIMBY_OK&&NimbyInternal_GetSnapshotInfo(filtered,&info)==NIMBY_OK);
    CHECK(info.signal_count==10&&info.track_count==1&&info.train_count==oldInfo.train_count);
    CHECK(NimbyInternal_CopyTrackOccupations(filtered,nullptr,0,&count)==NIMBY_OK&&count==500);
    CHECK(memory.connection->occupationReads>beforeOccupations&&memory.connection->renderReads>beforeRender);
    NimbySignalTexture texture{};CHECK(NimbyInternal_CopySignalTextures(filtered,&texture,1,&count)==NIMBY_BUFFER_TOO_SMALL&&count==10);
    std::vector<NimbySignalTexture> textures(count);CHECK(NimbyInternal_CopySignalTextures(filtered,textures.data(),count,&count)==NIMBY_OK);
    CHECK(textures.front().flags&NIMBY_SIGNAL_TEXTURE_REFERENCE_VALID);
    CHECK(textures.front().textures_hash==memory.hash);
    CHECK(NimbyInternal_ReleaseSnapshot(old)==NIMBY_OK&&NimbyInternal_ReleaseSnapshot(filtered)==NIMBY_OK);
    memory.selected(false);filtered=memory.capture(session.value,true);
    CHECK(NimbyInternal_CopyTrackOccupations(filtered,nullptr,0,&count)==NIMBY_DATA_UNAVAILABLE&&count==0);
    CHECK(NimbyInternal_ReleaseSnapshot(filtered)==NIMBY_OK);
    // Omitting unused mutable traffic is allowed, never omitting discovery.
    for(const auto address:{fixture::simulation+0x2c8,fixture::simulation+0x2200}){
        memory.world->refusedAddress=address;memory.world->refusedReads=0;
        filtered=memory.capture(session.value,true);CHECK(memory.world->refusedReads==0);
        CHECK(NimbyInternal_ReleaseSnapshot(filtered)==NIMBY_OK);
    }
    memory.world->refusedAddress=fixture::simulation+0x2c8;memory.world->refusedReads=0;
    const auto trainReads=memory.connection->trainPoolReads.load();
    old=memory.capture(session.value,false);
    CHECK(memory.world->refusedReads>0&&memory.connection->trainPoolReads-trainReads>4);
    CHECK(NimbyInternal_CopyTrackOccupations(old,nullptr,0,&count)==NIMBY_DATA_UNAVAILABLE&&count==0);
    CHECK(NimbyInternal_ReleaseSnapshot(old)==NIMBY_OK); // Existing diagnostic retry contract unchanged.
    memory.world->refusedAddress=fixture::simulation+0x20;
    memory.capture(session.value,true,NIMBY_DATA_UNAVAILABLE);memory.world->refusedAddress=0;
    filtered=memory.capture(session.value,true);CHECK(NimbyInternal_ReleaseSnapshot(filtered)==NIMBY_OK);
    memory.world->refusedAddress=fixture::db+0x380;
    memory.capture(session.value,true,NIMBY_DATA_UNAVAILABLE);
    memory.world->refusedAddress=SignallingWorld::catalogNode;
    memory.capture(session.value,true,NIMBY_DATA_UNAVAILABLE);memory.world->refusedAddress=0;
    // A replacement after discovery invalidates this observation. No negative
    // certificate is inferred from the two descriptor reads.
    for(const auto address:{fixture::db+0x380,fixture::db+0xa80+0x138}){
        const auto original=memory.world->regions.at(address);bool changed=false;
        memory.world->beforeRead=[&](uint64_t at,size_t){if(at==fixture::simulation+0xa0&&!changed){changed=true;memory.world->regions[address][0]^=1;}};
        memory.capture(session.value,true,NIMBY_DATA_UNAVAILABLE);CHECK(changed);
        memory.world->beforeRead={};memory.world->regions[address]=original;
    }
    // An in-place edit may occur after discovery without replacing its header.
    // This non-atomic snapshot is still unavailable for traffic, and the next
    // poll must immediately discover the new model; never cache the absence.
    bool added=false;memory.world->beforeRead=[&](uint64_t at,size_t){if(at==fixture::simulation+0xa0&&!added){added=true;memory.selected(true);}};
    filtered=memory.capture(session.value,true);CHECK(added);
    CHECK(NimbyInternal_CopySignals(filtered,nullptr,0,&count)==NIMBY_OK&&count==0);
    CHECK(NimbyInternal_CopyTrackOccupations(filtered,nullptr,0,&count)==NIMBY_DATA_UNAVAILABLE&&count==0);
    CHECK(NimbyInternal_ReleaseSnapshot(filtered)==NIMBY_OK);memory.world->beforeRead={};
    filtered=memory.capture(session.value,true);
    CHECK(NimbyInternal_CopySignals(filtered,nullptr,0,&count)==NIMBY_OK&&count==10);
    CHECK(NimbyInternal_CopyTrackOccupations(filtered,nullptr,0,&count)==NIMBY_OK&&count==500);
    CHECK(NimbyInternal_ReleaseSnapshot(filtered)==NIMBY_OK);memory.selected(false);
    bool changed=false;memory.world->beforeRead=[&](uint64_t at,size_t){if(at==fixture::simulation+0xa0&&!changed){changed=true;memory.world->identity=43;}};
    memory.capture(session.value,true,NIMBY_DATA_UNAVAILABLE);CHECK(changed);memory.world->beforeRead={};
    filtered=memory.capture(session.value,true);CHECK(NimbyInternal_ReleaseSnapshot(filtered)==NIMBY_OK);
    memory.world->put(SignallingWorld::catalogNode,0x88,uint64_t(1));
    memory.capture(session.value,true,NIMBY_DATA_UNAVAILABLE); // Broken requested catalogue remains fatal.
}
void runtimeEmptySignallingBenchmark(){
    for(unsigned n:{50u,500u,5000u}){
        const uint32_t pid=200+n;Session session(pid);SignallingWorld memory(pid,n,1000);
        for(const bool selected:{false,true}){
            memory.selected(selected);
            for(const bool runtime:{false,true}){
                auto warm=memory.capture(session.value,runtime);CHECK(NimbyInternal_ReleaseSnapshot(warm)==NIMBY_OK);
                const auto reads=memory.connection->reads.load(),bytes=memory.connection->bytesRequested.load();
                const auto occupation=memory.connection->occupationReads.load(),render=memory.connection->renderReads.load();
                const auto trainReads=memory.connection->trainPoolReads.load();std::vector<double> times;
                for(unsigned i=0;i<8;++i){const auto start=Clock::now();const auto snapshot=memory.capture(session.value,runtime);
                    CHECK(NimbyInternal_ReleaseSnapshot(snapshot)==NIMBY_OK);
                    times.push_back(std::chrono::duration<double,std::micro>(Clock::now()-start).count());}
                std::sort(times.begin(),times.end());
                std::cout<<"{\"scenario\":\"scoped_signalling\",\"trains\":"<<n<<",\"selected\":"<<(selected?"true":"false")
                    <<",\"runtime_filtered\":"<<(runtime?"true":"false")<<",\"samples\":8,\"p50_us\":"<<times[3]
                    <<",\"max_us\":"<<times.back()<<",\"reads_per_capture\":"<<(memory.connection->reads-reads)/8
                    <<",\"requested_bytes_per_capture\":"<<(memory.connection->bytesRequested-bytes)/8
                    <<",\"occupation_header_reads\":"<<(memory.connection->occupationReads-occupation)/8
                    <<",\"render_root_reads\":"<<(memory.connection->renderReads-render)/8
                    <<",\"train_pool_header_reads\":"<<(memory.connection->trainPoolReads-trainReads)/8<<",\"samples_us\":[";
                for(size_t i=0;i<times.size();++i){if(i)std::cout<<',';std::cout<<times[i];}
                std::cout<<"]}\n";
                if(runtime&&!selected)CHECK(memory.connection->occupationReads==occupation&&memory.connection->renderReads==render);
            }
        }
    }
}
}
