#include "platform/windows/runtime/driving_bridge.h"
#include "platform/windows/loader/loader.h"
#include "engine/binary_identity.h"
#include <iostream>
#include <iomanip>
#include <cmath>
int wmain(int argc,wchar_t** argv){
 if(argc!=6)return 2;
 const auto train=std::stoull(argv[3]);const auto cap=std::stod(argv[4]);const auto duration=std::stoll(argv[5]);
 if(train>>48!=5||!std::isfinite(cap)||cap<=0||cap>600||duration<=0||duration>120000)return 2;
 NimbyBinaryInfo identity{};
 if(nimby::engine::identify(argv[1],identity)!=NIMBY_OK||!identity.recognized_research_build)return 3;
 nimby::loader::Monitor monitor(argv[1],identity.sha256,argv[2]);
 for(const auto& event:monitor.poll()){
  if(!event.success){std::cerr<<event.message<<"\n";return 4;}
  HANDLE mapping=OpenFileMappingW(FILE_MAP_ALL_ACCESS,FALSE,nimby::driving_bridge::name(event.pid).c_str());
  if(!mapping)return 5;
  auto* data=static_cast<nimby::driving_bridge::Shared*>(MapViewOfFile(mapping,FILE_MAP_ALL_ACCESS,0,0,0));
  if(!data){CloseHandle(mapping);return 6;}
  if(data->size!=sizeof(*data)||data->version!=1||data->state!=0){UnmapViewOfFile(data);CloseHandle(mapping);return 7;}
  data->train=train;data->ceilingMps=cap/3.6;data->brakeUse=0.8;data->durationMs=duration;
  data->expiresWallMs=GetTickCount64()+20000;InterlockedExchange(&data->state,1);
  long cursor=0;std::cout<<std::setprecision(17);
  while(GetTickCount64()<data->expiresWallMs){
   while(cursor<1024&&InterlockedCompareExchange(&data->samples[cursor].ready,0,0)){
    const auto& s=data->samples[cursor];
    std::cout<<"{\"train\":\""<<s.train<<"\",\"wall_ms\":"<<s.wallMs<<",\"budget\":"<<s.budget<<",\"used\":"<<s.used
      <<",\"speed_before\":"<<s.speedBefore<<",\"speed_after\":"<<s.speedAfter<<",\"head_before\":"<<s.headBefore
      <<",\"head_after\":"<<s.headAfter<<",\"track_before\":\""<<s.trackBefore<<"\",\"track_after\":\""<<s.trackAfter
      <<"\",\"fraction_before\":"<<s.fractionBefore<<",\"fraction_after\":"<<s.fractionAfter<<",\"ceiling\":"<<s.ceiling
      <<",\"braking\":"<<s.braking<<",\"extra_mass\":"<<s.extraMass<<"}\n";++cursor;
   }
   if(InterlockedCompareExchange(&data->state,0,0)==3)break;
   Sleep(10);
  }
  InterlockedExchange(&data->state,3);
  std::cout<<"{\"summary\":true,\"samples\":"<<cursor<<",\"applied\":"<<data->applied<<"}\n";
  UnmapViewOfFile(data);CloseHandle(mapping);return cursor?0:8;
 }
 return 9;
}
