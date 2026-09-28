#include "construction-probe.hpp"
#include "platform/windows/loader/loader.h"
#include "engine/binary_identity.h"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <memory>
#include <cstring>

// Explicit PID, bounded observation and a dedicated file; no construction verb.
int wmain(int argc,wchar_t** argv){try {
    using namespace nimby::construction_probe;
    if(argc!=6){std::cerr<<"Usage: construction_probe EXE BRIDGE PID SECONDS OUTPUT.jsonl\n";return 1;}
    size_t used{};const auto processId=std::stoul(argv[3],&used);
    if(!processId||used!=std::wcslen(argv[3]))return 1;
    const auto seconds=std::stoul(argv[4],&used);
    if(seconds<1||seconds>1800||used!=std::wcslen(argv[4]))return 1;
    NimbyBinaryInfo binary{};
    if(nimby::engine::identify(argv[1],binary)!=NIMBY_OK||!binary.recognized_research_build)return 2;
    // Fail before injection when the result file cannot be created. Existing
    // records are retained; every attachment writes a new session header.
    std::ofstream output(std::filesystem::path(argv[5]),std::ios::app);
    if(!output)return 3;
    nimby::loader::Monitor monitor(argv[1],binary.sha256,argv[2]);
    const auto attached=monitor.attach_process(static_cast<DWORD>(processId));
    if(!attached.success){std::cerr<<attached.message<<'\n';return 4;}
    const auto mapping=OpenFileMappingW(FILE_MAP_ALL_ACCESS,FALSE,name(static_cast<DWORD>(processId)).c_str());
    if(!mapping)return 5;
    auto shared=static_cast<Shared*>(MapViewOfFile(mapping,FILE_MAP_ALL_ACCESS,0,0,sizeof(Shared)));
    if(!shared){CloseHandle(mapping);return 5;}
    struct Cleanup{HANDLE mapping;Shared* shared;~Cleanup(){InterlockedExchange64(&shared->expires,0);UnmapViewOfFile(shared);CloseHandle(mapping);}} cleanup{mapping,shared};
    if(shared->version!=1||shared->size!=sizeof(Shared))return 6;
    output<<"{\"session\":true,\"pid\":"<<processId<<",\"base\":"<<shared->base<<",\"hash\":\""<<binary.sha256<<"\"}\n";
    output.flush();std::cout<<"Observing editor commands, pid="<<processId<<" seconds="<<seconds<<std::endl;
    auto snapshot=std::make_unique<Shared>();uint64_t next=shared->count;
    const auto end=GetTickCount64()+seconds*1000;
    InterlockedExchange64(&shared->expires,static_cast<LONG64>(end));
    while(GetTickCount64()<end){
        bool captured=false;
        for(int retry=0;retry<20;++retry){
            const LONG sequence=InterlockedCompareExchange(&shared->sequence,0,0);if(sequence&1)continue;
            std::memcpy(snapshot.get(),shared,sizeof(Shared));MemoryBarrier();
            if(sequence==InterlockedCompareExchange(&shared->sequence,0,0)){captured=true;break;}
        }
        if(captured){
            const auto first=snapshot->count>capacity?snapshot->count-capacity:0;
            if(next<first){output<<"{\"lost\":"<<first-next<<"}\n";next=first;}
            for(;next<snapshot->count;++next){
                const auto& e=snapshot->events[next%capacity];
                output<<"{\"event\":"<<next<<",\"kind\":"<<e.kind<<",\"thread\":"<<e.thread<<",\"tick\":"<<e.tick
                    <<",\"command\":"<<e.command<<",\"context\":"<<e.context<<",\"result\":"<<e.result<<",\"db\":"<<e.db
                    <<",\"historyCount\":"<<e.historyCount<<",\"valid\":"<<e.valid;
                const auto array=[&](const char* key,const uint64_t* data,size_t count){
                    output<<",\""<<key<<"\":[";for(size_t i=0;i<count;++i){if(i)output<<',';output<<data[i];}output<<']';};
                array("header",e.header,16);array("resultWords",e.resultWords,64);array("deltaWords",e.deltaWords,192);array("stack",e.stack,e.stackCount);
                output<<"}\n";std::cout<<"event="<<next<<" kind="<<e.kind<<" thread="<<e.thread<<std::endl;
            }
            output.flush();if(!output)return 7;
        }
        Sleep(25);
    }
    return 0;
}catch(const std::exception& error){std::cerr<<error.what()<<'\n';return 9;}}
