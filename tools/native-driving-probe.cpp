#include "native-driving-probe.hpp"
#include "loader/loader.h"
#include "engine/binary_identity.h"
#include <iostream>
#include <iomanip>
int wmain(int argc,wchar_t** argv){
 if(argc!=4)return 2;
 NimbyBinaryInfo identity{};
 if(nimby::engine::identify(argv[1],identity)!=NIMBY_OK||!identity.recognized_research_build)return 3;
 nimby::loader::Monitor monitor(argv[1],identity.sha256,argv[2]);
 for(const auto& event:monitor.poll()){
  std::cerr<<event.pid<<" "<<event.success<<" "<<event.message<<"\n";
  if(!event.success)return 4;
  HANDLE mapping=OpenFileMappingW(FILE_MAP_ALL_ACCESS,FALSE,driving_probe::name(event.pid).c_str());
  if(!mapping)return 5;
  auto* data=static_cast<driving_probe::Shared*>(MapViewOfFile(mapping,FILE_MAP_ALL_ACCESS,0,0,0));
  if(!data){CloseHandle(mapping);return 6;}
  if(data->size!=sizeof(*data)||data->version!=1){UnmapViewOfFile(data);CloseHandle(mapping);return 7;}
  // One collection per game process: published rows are immutable.
  if(data->count){UnmapViewOfFile(data);CloseHandle(mapping);return 8;}
  data->target=std::stoull(argv[3]);InterlockedExchange(&data->enabled,1);
  const auto deadline=GetTickCount64()+15000;
  long cursor=0;std::cout<<std::setprecision(17);
  while(cursor<2048&&GetTickCount64()<deadline){
   auto& s=data->samples[cursor];
   if(!InterlockedCompareExchange(&s.ready,0,0)){Sleep(10);continue;}
   std::cout<<"{\"train\":\""<<s.train<<"\",\"thread\":"<<s.thread<<",\"context\":\""<<s.context
    <<"\",\"motion\":\""<<s.motion<<"\",\"simulation\":\""<<s.simulation<<"\",\"ticks\":"<<s.ticks
    <<",\"budget_before\":"<<s.budgetBefore<<",\"budget_after\":"<<s.budgetAfter
    <<",\"speed_before\":"<<s.speedBefore<<",\"speed_after\":"<<s.speedAfter
    <<",\"track_before\":\""<<s.trackBefore<<"\",\"track_after\":\""<<s.trackAfter
    <<"\",\"fraction_before\":"<<s.fractionBefore<<",\"fraction_after\":"<<s.fractionAfter
    <<",\"signal\":\""<<s.signal<<"\",\"distance_m\":"<<s.distanceM<<",\"head_m\":"<<s.headM<<",\"dynamics\":[";
   for(int i=0;i<8;++i)std::cout<<(i?",":"")<<s.dynamics[i];
   std::cout<<"]}\n";++cursor;
  }
  InterlockedExchange(&data->enabled,0);
  std::cerr<<"samples="<<cursor<<"\n";UnmapViewOfFile(data);CloseHandle(mapping);
  return cursor?0:9;
 }
 return 10;
}
