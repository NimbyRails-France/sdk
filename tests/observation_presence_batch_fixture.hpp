#pragma once
// Observation ABI contract: only the process read boundary is synthetic.
// Snapshot/session/epoch, scoped capture and copying are the production code.
namespace {
uint64_t bytesHash(const void* data,size_t bytes){
    auto hash=uint64_t(1469598103934665603ULL);const auto* p=static_cast<const unsigned char*>(data);
    for(size_t i=0;i<bytes;++i){hash^=p[i];hash*=1099511628211ULL;}return hash;
}
struct AbiRows {std::vector<NimbyTrain> trains;std::vector<NimbyTrainService> services;};
AbiRows snapshotRows(NimbySnapshot snapshot,unsigned n){
    AbiRows rows;rows.trains.resize(n);rows.services.resize(n);uint32_t count=0;
    CHECK(NimbyInternal_CopyTrains(snapshot,rows.trains.data(),n,&count)==NIMBY_OK&&count==n);
    CHECK(NimbyInternal_CopyTrainServices(snapshot,rows.services.data(),n,&count)==NIMBY_OK&&count==n);
    CHECK(NimbyInternal_ReleaseSnapshot(snapshot)==NIMBY_OK);return rows;
}
void presenceBatchObservationContracts(){
    for(const auto n:{50u,500u,4095u,4096u,5000u}){
        Session session(10000+n);SignallingWorld memory(10000+n,n,10);
        const auto publicReads=memory.connection->reads.load(),publicBytes=memory.connection->bytesRequested.load();
        const auto publicPrefix=memory.connection->motionPrefixReads.load(),publicBatches=memory.connection->motionBatchReads.load();
        const auto publicRows=snapshotRows(memory.capture(session.value,false),n);
        CHECK(memory.connection->motionBatchReads==publicBatches&&memory.connection->motionPrefixReads-publicPrefix==n);
        const auto genericCalls=memory.connection->reads-publicReads,genericBytes=memory.connection->bytesRequested-publicBytes;
        const auto runtimePrefix=memory.connection->motionPrefixReads.load(),runtimeBatches=memory.connection->motionBatchReads.load();
        const auto runtimeRows=snapshotRows(memory.capture(session.value,true),n);
        CHECK(std::memcmp(publicRows.trains.data(),runtimeRows.trains.data(),n*sizeof(NimbyTrain))==0);
        CHECK(std::memcmp(publicRows.services.data(),runtimeRows.services.data(),n*sizeof(NimbyTrainService))==0);
        const bool bulk=n>=4096;
        CHECK(memory.connection->motionBatchReads-runtimeBatches==(bulk?(n+511)/512:0));
        CHECK(memory.connection->motionPrefixReads-runtimePrefix==(bulk?0:n));
        std::cout<<"{\"scenario\":\"abi_generic_contract\",\"trains\":"<<n<<",\"calls\":"<<genericCalls
            <<",\"bytes\":"<<genericBytes<<",\"trains_hash\":"<<bytesHash(publicRows.trains.data(),n*sizeof(NimbyTrain))
            <<",\"services_hash\":"<<bytesHash(publicRows.services.data(),n*sizeof(NimbyTrainService))<<"}\n";
    }
    // Successful read corruption is transient in copied bytes only. A prefix
    // retry would heal it; runtime capture must retain the unknown row instead.
    for(const bool selected:{false,true})for(const size_t offset:{size_t(0),size_t(0x1d0),size_t(0x3a8),size_t(0x3b8)}){
        const uint32_t pid=18000+unsigned(offset)+unsigned(selected)*0x1000;
        Session session(pid);SignallingWorld memory(pid,4096,10);memory.selected(selected);
        const auto target=SignallingWorld::motionBlock+13*0x638;
        memory.world->afterRead=[&](uint64_t address,void* output,size_t size){
            const bool recheck=size==0x4b1||(size>0x638&&size%0x638==0x4b1);
            if(recheck&&target>=address&&target-address+0x4b1<=size){
                static_cast<unsigned char*>(output)[target-address+offset]^=offset==0x1d0?2:1;
            }
        };
        const auto beforePrefix=memory.connection->motionPrefixReads.load(),beforeBatch=memory.connection->motionBatchReads.load();
        const auto rows=snapshotRows(memory.capture(session.value,true),4096);
        CHECK(!(rows.trains[13].flags&NIMBY_TRAIN_POSITION_VALID));
        if(selected){CHECK(rows.trains[12].flags&NIMBY_TRAIN_POSITION_VALID);CHECK(rows.trains[14].flags&NIMBY_TRAIN_POSITION_VALID);}
        else{CHECK(rows.services[12].flags&NIMBY_SERVICE_PRESENCE_VALID);CHECK(rows.services[14].flags&NIMBY_SERVICE_PRESENCE_VALID);}
        if(offset==0||offset==0x1d0)CHECK(!(rows.services[13].flags&NIMBY_SERVICE_PRESENCE_VALID));
        CHECK(memory.connection->motionPrefixReads==beforePrefix);
        CHECK(memory.connection->motionBatchReads-beforeBatch>=8); // Pair retries remain whole observations.
        memory.world->afterRead={};
        const auto recovered=snapshotRows(memory.capture(session.value,true),4096);
        if(selected)CHECK(recovered.trains[13].flags&NIMBY_TRAIN_POSITION_VALID);
        else CHECK(recovered.services[13].flags&NIMBY_SERVICE_PRESENCE_VALID); // New observation may legitimately recover.
    }
    {
        Session session(23001);SignallingWorld memory(23001,4096,10);memory.selected(true);
        memory.world->refusedRangeAddress=SignallingWorld::motionBlock;
        memory.world->refusedRangeSize=511*0x638+0x4b1;
        const auto prefix=memory.connection->motionPrefixReads.load();
        const auto rows=snapshotRows(memory.capture(session.value,true),4096);
        CHECK(rows.trains[13].flags&NIMBY_TRAIN_POSITION_VALID);
        CHECK(memory.connection->motionPrefixReads-prefix==512);
        CHECK(memory.world->refusedReads==1); // Only failed read permits prefix fallback.
    }
    {
        Session session(23002);SignallingWorld memory(23002,4096,10);
        memory.world->refusedRangeAddress=SignallingWorld::motionBlock;
        memory.world->refusedRangeSize=511*0x638+0x4b1;
        memory.world->refusedAddress=SignallingWorld::motionBlock+13*0x638;
        const auto rows=snapshotRows(memory.capture(session.value,true),4096);
        CHECK(!(rows.trains[13].flags&NIMBY_TRAIN_POSITION_VALID)&&!(rows.services[13].flags&NIMBY_SERVICE_PRESENCE_VALID));
        CHECK(rows.services[12].flags&NIMBY_SERVICE_PRESENCE_VALID);
    }
    {
        Session session(23003);SignallingWorld memory(23003,5000,10);
        for(unsigned i=0;i<5000;++i)memory.text(SignallingWorld::trainBlock,size_t(i)*0x178+0x10,"Train");
        const auto beforeBatch=memory.connection->motionBatchReads.load(),beforePrefix=memory.connection->motionPrefixReads.load();
        const auto beforeReads=memory.connection->reads.load(),beforeBytes=memory.connection->bytesRequested.load();
        NimbySnapshot snapshot=0;uint32_t stage=0;
        CHECK(NimbyInternal_CaptureTrainDataSnapshotWithOptions(session.value,NIMBY_TRAIN_DATA_LOCATIONS,&snapshot,&stage)==NIMBY_OK);
        const auto rows=snapshotRows(snapshot,5000);
        CHECK(memory.connection->motionBatchReads==beforeBatch&&memory.connection->motionPrefixReads==beforePrefix);
        CHECK(rows.trains[13].flags&NIMBY_TRAIN_POSITION_VALID);
        std::cout<<"{\"scenario\":\"abi_full_locations_contract\",\"trains\":5000,\"calls\":"<<memory.connection->reads-beforeReads
            <<",\"bytes\":"<<memory.connection->bytesRequested-beforeBytes<<",\"trains_hash\":"<<bytesHash(rows.trains.data(),5000*sizeof(NimbyTrain))
            <<",\"services_hash\":"<<bytesHash(rows.services.data(),5000*sizeof(NimbyTrainService))<<"}\n";
    }
}
}
