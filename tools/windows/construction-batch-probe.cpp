#include "construction-batch-protocol.hpp"
#include "platform/windows/loader/loader.h"
#include "engine/binary_identity.h"
#include <iostream>
#include <cmath>

// Deliberately diagnostic: arm one manual placement on a test track only.
// Args use explicit PID/track and invariant decimal fractions, no discovery.
int wmain(int argc,wchar_t** argv) {try {
    using namespace nimby::construction_batch_probe;
    if(argc<7||argc>5+static_cast<int>(capacity)){
        std::cerr<<"Usage: batch_probe EXE BRIDGE PID TRACK_ID FRACTION... (2..16)\n";return 1;
    }
    size_t used{};const auto pid=std::stoul(argv[3],&used);
    if(!pid||used!=std::wcslen(argv[3]))return 1;
    const auto track=std::stoull(argv[4],&used,0);
    if((track>>48)!=1||used!=std::wcslen(argv[4]))return 1;
    Shared input{};input.track=track;input.count=argc-5;
    for(uint32_t i=0;i<input.count;++i){input.fractions[i]=std::stod(argv[i+5],&used);
        if(used!=std::wcslen(argv[i+5])||!std::isfinite(input.fractions[i])||input.fractions[i]<=0||input.fractions[i]>=1)return 1;
        for(uint32_t j=0;j<i;++j)if(std::abs(input.fractions[i]-input.fractions[j])<=1e-6)return 1;
    }
    NimbyBinaryInfo binary{};
    if(nimby::engine::identify(argv[1],binary)!=NIMBY_OK||!binary.recognized_research_build)return 2;
    nimby::loader::Monitor monitor(argv[1],binary.sha256,argv[2]);
    const auto attached=monitor.attach_process(static_cast<DWORD>(pid));
    if(!attached.success){std::cerr<<attached.message<<'\n';return 3;}
    const auto mapping=OpenFileMappingW(FILE_MAP_ALL_ACCESS,FALSE,name(static_cast<DWORD>(pid)).c_str());
    if(!mapping)return 4;
    auto shared=static_cast<Shared*>(MapViewOfFile(mapping,FILE_MAP_ALL_ACCESS,0,0,sizeof(Shared)));
    if(!shared){CloseHandle(mapping);return 4;}
    struct Cleanup {HANDLE handle;Shared* data;~Cleanup(){InterlockedCompareExchange(&data->state,idle,armed);UnmapViewOfFile(data);CloseHandle(handle);}} cleanup{mapping,shared};
    if(shared->version!=protocol||shared->size!=sizeof(Shared)||InterlockedCompareExchange(&shared->state,idle,idle)!=idle)return 5;
    shared->track=input.track;shared->count=input.count;shared->created=0;shared->error=0;
    for(uint32_t i=0;i<input.count;++i)shared->fractions[i]=input.fractions[i];
    shared->expires=GetTickCount64()+120000;
    InterlockedExchange(&shared->state,armed);
    std::cout<<"ARMED: next manual placement on track "<<track<<"; 120 seconds\n"<<std::flush;
    while(GetTickCount64()<shared->expires&&InterlockedCompareExchange(&shared->state,complete,complete)!=complete)Sleep(25);
    if(InterlockedCompareExchange(&shared->state,complete,complete)!=complete){std::cerr<<"No completed batch; do not infer success\n";return 6;}
    std::cout<<"created="<<shared->created<<" error="<<shared->error<<" thread="<<shared->thread<<'\n';
    for(uint32_t i=0;i<shared->created&&i<capacity;++i)std::cout<<"id="<<shared->ids[i]<<'\n';
    return shared->error?7:0;
}catch(const std::exception& e){std::cerr<<e.what()<<'\n';return 9;}}
