#include <runtime/texture_publications.h>
#include <runtime/texture_commands.h>
#include <nimby/detail/live_texture_tracking.hpp>
#include <stdexcept>
#include <iostream>
#include <chrono>
#include <array>
#include <future>
#include <thread>
#define CHECK(x) do{if(!(x))throw std::runtime_error("line "+std::to_string(__LINE__)+": " #x);}while(false)
int main(){try{
    using namespace nimby::texture_bridge;
    constexpr uint64_t first=0x8000000000001,other=0x8000000100001;
    Table table;Command a{first,123,1000,1},b{other,456,1500,2};
    CHECK(publishTextures(table,1,{&a,1},10)==NIMBY_OK);
    CHECK(publishTextures(table,2,{&b,1},10)==NIMBY_OK&&table.count()==2);
    CHECK(publishTextures(table,2,{&a,1},10)==NIMBY_RESOURCE_LIMIT);
    CHECK(table.entries[table.lower(first)].owner==1);
    Command invalids[]{a,b};invalids[1].half_period_ms=99;
    CHECK(publishTextures(table,1,invalids,10)==NIMBY_INVALID_ARGUMENT&&table.entries[0].index==1);
    CHECK(clearTextures(table,2,{&first,1})==NIMBY_OK&&table.count()==2&&table.entries[table.lower(first)].owner==1);
    releaseTextures(table,3);CHECK(table.count()==2);
    a.expires=2000;a.index=3;CHECK(publishTextures(table,1,{&a,1},10)==NIMBY_OK);
    CHECK(table.entries[table.lower(other)].expires==1500&&table.entries[table.lower(other)].index==2);
    Mailbox legacy;legacy.operation=1;legacy.request_signal=first;legacy.request_hash=999;legacy.request_expiry=9999;
    CHECK(execute(legacy,table,10)==NIMBY_RESOURCE_LIMIT&&table.entries[0].set_hash==123);
    legacy.operation=2;CHECK(execute(legacy,table,10)==NIMBY_RESOURCE_LIMIT);
    releaseTextures(table,2);CHECK(table.count()==1&&table.entries[0].owner==1);
    std::vector<Command> flood;
    for(uint64_t i=0;i<maxOwnerTextures;++i)flood.push_back({other+i,456,3000,2});
    CHECK(publishTextures(table,2,flood,10)==NIMBY_OK);
    const Command extra{other+maxOwnerTextures,456,3000,2};
    CHECK(publishTextures(table,2,{&extra,1},10)==NIMBY_RESOURCE_LIMIT);
    CHECK(publishTextures(table,1,{&a,1},10)==NIMBY_OK); // Existing A still updates.
    auto duplicate=std::array{a,a};CHECK(publishTextures(table,1,duplicate,10)==NIMBY_INVALID_ARGUMENT);
    const auto start=std::chrono::steady_clock::now();
    for(unsigned i=0;i<1000;++i){a.expires=4000+i;CHECK(publishTextures(table,1,{&a,1},10)==NIMBY_OK);}
    const auto us=std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now()-start).count();
    releaseTextures(table,2);CHECK(table.count()==1);
    CHECK(clearTextures(table,1,{&first,1})==NIMBY_OK&&table.entries.empty());
    for(uint64_t owner=1;owner<=maxTextureOwners;++owner){Command row{first+owner,owner,3000,0};CHECK(publishTextures(table,owner,{&row,1},10)==NIMBY_OK);}
    CHECK(publishTextures(table,65,{&b,1},10)==NIMBY_RESOURCE_LIMIT);
    releaseTextures(table,32);CHECK(publishTextures(table,65,{&b,1},10)==NIMBY_OK);
    {
        TexturePublications recovery;
        std::vector<Command> initial;
        std::vector<uint64_t> owned;std::unordered_set<uint64_t> ownedIds;
        for(uint64_t index=0;index<maxOwnerTextures;++index){
            const auto id=first+index;
            initial.push_back({id,123,index?1000u:100u,1});
            CHECK(nimby::detail::trackLiveTexture(id,owned,ownedIds));
        }
        CHECK(recovery.update(1,[&](auto& state){return publishTextures(state.table,1,initial,10);})==NIMBY_OK);
        Command takeover{first,456,3000,2};
        CHECK(recovery.update(2,[&](auto& state){return publishTextures(state.table,2,{&takeover,1},101);})==NIMBY_OK);
        uint32_t clearStatus=NIMBY_OK;size_t retired=0;
        auto cleanup=[&]{nimby::detail::restoreTrackedTextures(owned,[](auto){return true;},[&](auto batch){
            clearStatus=recovery.update(1,[&](auto& state){return clearTextures(state.table,1,batch);},true);
            if(clearStatus!=NIMBY_OK)throw std::runtime_error("Publication unavailable");
        },[&](auto id){++retired;ownedIds.erase(id);});};
        {
            // A genuinely pinned render view exhausts the bounded retirement
            // queue. Cleanup failure must keep every pending ID and both mods.
            TexturePublications::View pinned(recovery);size_t renewals=0;
            while(recovery.update(2,[&](auto& state){return publishTextures(state.table,2,{&takeover,1},101);})==NIMBY_OK)
                CHECK(++renewals<=TexturePublications::maxRetired);
            cleanup();CHECK(clearStatus==NIMBY_RESOURCE_LIMIT&&retired==0&&owned.size()==4096);
            CHECK(!nimby::detail::trackLiveTexture(other,owned,ownedIds));
            CHECK(recovery.snapshot()->table.count()==4096&&recovery.read(first).command.owner==2);
        }
        cleanup();CHECK(clearStatus==NIMBY_OK&&owned.empty()&&ownedIds.empty()&&retired==4096);
        CHECK(recovery.snapshot()->table.count()==1);
        CHECK(recovery.read(first).command.owner==2&&recovery.read(first).command.index==2&&recovery.read(first).command.expires==3000);
        std::vector<Command> replacement;
        for(uint64_t index=0;index<maxOwnerTextures;++index){
            const auto id=other+index;CHECK(nimby::detail::trackLiveTexture(id,owned,ownedIds));
            replacement.push_back({id,123,3000,1});
        }
        CHECK(recovery.update(1,[&](auto& state){return publishTextures(state.table,1,replacement,101);})==NIMBY_OK);
        CHECK(owned.size()==4096&&recovery.snapshot()->table.count()==4097&&recovery.read(first).command.owner==2);
        // Invalid input still rejects the whole mixed request before mutation.
        const uint64_t invalidBatch[]{other,first,1};
        CHECK(recovery.update(1,[&](auto& state){return clearTextures(state.table,1,invalidBatch);},true)==NIMBY_INVALID_ARGUMENT);
        CHECK(recovery.snapshot()->table.count()==4097&&recovery.read(other).command.owner==1&&recovery.read(first).command.owner==2);
    }
    {
        // Cleanup prepared by A must retry after B acquires an expired lease,
        // removing A's remaining entry without undoing B's new publication.
        TexturePublications transferred;
        const Command initial[]{ {first,123,100,1}, {other,123,1000,1} };
        CHECK(transferred.update(1,[&](auto& state){return publishTextures(state.table,1,initial,10);})==NIMBY_OK);
        const uint64_t requested[]{first,other,other+1}; // Includes an absent ID.
        std::promise<void> readyToClear,finishClear;
        auto continuation=finishClear.get_future().share();unsigned attempts=0;
        auto clear=[&](auto& state){
            const auto count=state.table.count();
            const auto status=clearTextures(state.table,1,requested);
            if(++attempts==1){readyToClear.set_value();continuation.wait();}
            return TexturePublications::PreparedUpdate{status,state.table.count()!=count};
        };
        auto pending=std::async(std::launch::async,[&]{return transferred.update(1,clear,true);});
        readyToClear.get_future().wait();
        const Command acquired{first,456,3000,2};
        const auto acquiredStatus=transferred.update(2,[&](auto& state){return publishTextures(state.table,2,{&acquired,1},101);});
        finishClear.set_value();const auto clearStatus=pending.get();
        CHECK(acquiredStatus==NIMBY_OK&&clearStatus==NIMBY_OK&&attempts==2);
        CHECK(transferred.snapshot()->table.count()==1&&transferred.read(first).command.owner==2);
        CHECK(transferred.read(first).command.index==2&&transferred.read(first).command.expires==3000);
        const auto beforeRepeated=transferred.snapshot();
        CHECK(transferred.update(1,clear,true)==NIMBY_OK&&transferred.snapshot()==beforeRepeated);
    }
    TexturePublications publications;
    const auto set=[&](TextureState& state){return publishTextures(state.table,1,{&a,1},10);};
    CHECK(publications.update(1,set)==NIMBY_OK);
    {
        TexturePublications::View suspended(publications);
        // A reader suspended indefinitely cannot pin an unbounded history.
        size_t committed=0;
        while(publications.update(1,set)==NIMBY_OK)CHECK(++committed<=TexturePublications::maxRetired);
        CHECK(committed>0&&publications.read(first).command.index==a.index);
        CHECK(suspended.state->table.entries.front().signal==first);
    }
    CHECK(publications.update(1,set)==NIMBY_OK); // Reclamation resumes on a writer.
    std::promise<void> prepared,resume;
    auto resumeFuture=resume.get_future().share();
    auto delayed=std::async(std::launch::async,[&]{return publications.update(1,[&](TextureState& state){
        const auto result=publishTextures(state.table,1,{&a,1},10);
        prepared.set_value();resumeFuture.wait();return result;
    });});
    prepared.get_future().wait();
    const Command peer{other,456,9000,2};
    const auto peerPublished=publications.update(2,[&](TextureState& state){return publishTextures(state.table,2,{&peer,1},10);});
    const auto ownerReleased=publications.update(1,[&](TextureState& state){releaseTextures(state.table,1);return uint32_t{NIMBY_OK};},true);
    const auto released=publications.read(first).command.signal==0&&publications.read(other).command.index==2;
    resume.set_value();CHECK(delayed.get()==NIMBY_RESOURCE_LIMIT);
    CHECK(peerPublished==NIMBY_OK&&ownerReleased==NIMBY_OK&&released);
    CHECK(publications.read(first).command.signal==0); // No late resurrection.
    CHECK(publications.update(1,set)==NIMBY_OK);
    std::promise<void> ready,finish;
    auto finishFuture=finish.get_future().share();
    bool firstPrepare=true;
    unsigned prepareCount=0;
    auto healthy=std::async(std::launch::async,[&]{return publications.update(1,[&](TextureState& state){
        ++prepareCount;
        state.table.entries[state.table.lower(first)].index=7;
        if(firstPrepare){firstPrepare=false;ready.set_value();finishFuture.wait();}return uint32_t{NIMBY_OK};
    });});
    ready.get_future().wait();
    const auto beforeForeign=publications.snapshot();
    const auto foreign=publications.update(2,[&](TextureState& state){
        const auto before=state.table.count();const auto status=clearTextures(state.table,2,{&first,1});
        return TexturePublications::PreparedUpdate{status,state.table.count()!=before};
    },true);
    CHECK(publications.snapshot()==beforeForeign);
    finish.set_value();CHECK(healthy.get()==NIMBY_OK);
    CHECK(foreign==NIMBY_OK&&publications.read(first).command.index==7&&prepareCount==1);
    {
        const uint64_t pendingId=other+8192;
        const Command pending{pendingId,123,9000,1};
        std::promise<void> waiting,continueWrite;auto continuation=continueWrite.get_future().share();
        auto late=std::async(std::launch::async,[&]{return publications.update(7,[&](auto& state){
            const auto status=publishTextures(state.table,7,{&pending,1},10);
            waiting.set_value();continuation.wait();return status;
        });});
        waiting.get_future().wait();const auto before=publications.snapshot();
        CHECK(publications.update(7,[&](auto& state){
            const auto count=state.table.count();const auto status=clearTextures(state.table,7,{&pendingId,1});
            return TexturePublications::PreparedUpdate{status,state.table.count()!=count};
        },true)==NIMBY_OK&&publications.snapshot()==before);
        continueWrite.set_value();CHECK(late.get()==NIMBY_RESOURCE_LIMIT&&publications.read(pendingId).command.signal==0);

        // A no-op prepared from an old snapshot must retry if a newer A
        // publication acquires the requested ID before cleanup commits.
        std::promise<void> cleanupReady,resumeCleanup;auto cleanupResume=resumeCleanup.get_future().share();
        unsigned attempts=0;
        auto cleanup=std::async(std::launch::async,[&]{return publications.update(7,[&](auto& state){
            const auto count=state.table.count();const auto status=clearTextures(state.table,7,{&pendingId,1});
            if(++attempts==1){cleanupReady.set_value();cleanupResume.wait();}
            return TexturePublications::PreparedUpdate{status,state.table.count()!=count};
        },true);});
        cleanupReady.get_future().wait();
        CHECK(publications.update(7,[&](auto& state){return publishTextures(state.table,7,{&pending,1},10);})==NIMBY_OK);
        resumeCleanup.set_value();CHECK(cleanup.get()==NIMBY_OK&&attempts==2&&publications.read(pendingId).command.signal==0);
        CHECK(publications.read(first).command.index==7&&publications.read(other).command.owner==2);
    }
    std::cout<<"PASS texture owners, bounded floods, atomic validation, release; 1000 A renewals beside 4096 B signals: "<<us<<" us\n";
}catch(const std::exception& e){std::cerr<<e.what()<<'\n';return 1;}}
