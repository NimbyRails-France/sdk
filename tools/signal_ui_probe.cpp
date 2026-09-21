#include "loader/loader.h"
#include "engine/binary_identity.h"
#include "runtime/signal_ui_probe.h"
#include <filesystem>
#include <iostream>
#include <cstring>
#include <cwchar>
int wmain(int argc,wchar_t** argv){try{
    if(argc!=4){std::cerr<<"Usage: signal_ui_probe EXE BRIDGE PID\n";return 1;}
    size_t used{};const auto pid=std::stoul(argv[3],&used);
    if(!pid||used!=std::wcslen(argv[3]))return 1;
    NimbyBinaryInfo binary{};
    if(nimby::engine::identify(argv[1],binary)!=NIMBY_OK||!binary.recognized_research_build)return 2;
    nimby::loader::Monitor monitor(argv[1],binary.sha256,argv[2]);
    const auto event=monitor.attach_process(pid);
    if(!event.success){std::cerr<<event.message<<'\n';return 3;}
    const auto mapping=OpenFileMappingW(FILE_MAP_ALL_ACCESS,FALSE,nimby::signal_ui_probe::name(pid).c_str());
    if(!mapping)return 4;
    const auto shared=static_cast<nimby::signal_ui_probe::Shared*>(MapViewOfFile(mapping,FILE_MAP_ALL_ACCESS,0,0,sizeof(nimby::signal_ui_probe::Shared)));
    if(!shared){CloseHandle(mapping);return 5;}
    struct Cleanup{HANDLE mapping;nimby::signal_ui_probe::Shared* shared;~Cleanup(){InterlockedExchange64(&shared->expires,0);UnmapViewOfFile(shared);CloseHandle(mapping);}} cleanup{mapping,shared};
    if(shared->version!=1||shared->size!=sizeof(*shared))return 6;
    InterlockedExchange64(&shared->expires,static_cast<LONG64>(GetTickCount64()+10000));
    Sleep(2000);
    nimby::signal_ui_probe::Shared snapshot{};bool captured=false;
    for(int retry=0;retry<100;++retry){
        const LONG before=InterlockedCompareExchange(&shared->sequence,0,0);if(before&1){Sleep(1);continue;}
        std::memcpy(&snapshot,shared,sizeof snapshot);MemoryBarrier();
        if(before==InterlockedCompareExchange(&shared->sequence,0,0)){captured=true;break;}
    }
    if(!captured)return 7;
    std::cout<<"callbacks="<<snapshot.count<<'\n';
    const auto start=snapshot.count>16?snapshot.count-16:0;
    for(auto i=start;i<snapshot.count;++i){const auto& e=snapshot.events[i%16];
        std::cout<<"event="<<i<<" pass="<<e.pass<<" thread="<<e.thread<<" signal="<<e.signal<<" capture="<<e.capture<<'\n';}
    return snapshot.count?0:8;
}catch(const std::exception& error){std::cerr<<error.what()<<'\n';return 9;}}
