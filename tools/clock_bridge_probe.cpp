#include "loader/loader.h"
#include "engine/binary_identity.h"
#include "runtime/clock_bridge.h"
#include <iostream>
int wmain(int argc,wchar_t** argv) {
    if(argc!=3)return 2;
    NimbyBinaryInfo identity{};
    if(nimby::engine::identify(argv[1],identity)!=NIMBY_OK || !identity.recognized_research_build)return 3;
    nimby::loader::Monitor monitor(argv[1],identity.sha256,argv[2]);
    for(const auto& event:monitor.poll()) {
        std::cout<<event.pid<<" "<<event.success<<" "<<event.message<<std::endl;
        if(!event.success)return 4;
        const auto mapping=OpenFileMappingW(FILE_MAP_READ,FALSE,nimby::clock_bridge::name(event.pid).c_str());
        if(!mapping)return 5;
        const auto data=static_cast<const nimby::clock_bridge::Shared*>(MapViewOfFile(mapping,FILE_MAP_READ,0,0,0));
        if(!data)return 6;
        Sleep(1000);
        std::cout<<"callbacks="<<data->callbacks<<" thread="<<data->thread_id<<" sim="<<std::hex<<data->observed_sim
            <<" db="<<data->observed_db<<std::dec<<" epoch="<<data->epoch<<" ticks="<<data->ticks<<std::endl;
        UnmapViewOfFile(data);CloseHandle(mapping);
    }
    return 0;
}
