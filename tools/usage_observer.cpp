#include <nimby/observation.h>
#include <windows.h>
#include <cstdio>
#include <cstdlib>
#include <vector>
#include <set>
#include <tuple>
#include <map>
#include <string>
#include <chrono>
using Key=std::tuple<uint64_t,uint64_t,double,double>;
int main(int argc,char** argv){if(argc!=2)return 1;NimbySession s{};if(NimbySdk_OpenProcess(1,std::strtoul(argv[1],nullptr,10),&s))return 2;
 std::set<Key> previous;unsigned success=0,removed=0,added=0;
 for(int frame=0;frame<20;++frame){auto start=std::chrono::steady_clock::now();NimbySnapshot snap{};auto st=NimbySdk_CaptureSnapshot(s,&snap);if(st){std::printf("frame=%d snapshot=%u\n",frame,st);Sleep(100);continue;}
  uint32_t n=0;NimbySdk_CopyTrains(snap,nullptr,0,&n);std::vector<NimbyTrain> trains(n);NimbySdk_CopyTrains(snap,trains.data(),n,&n);std::map<uint64_t,NimbyTrain> trainMap;for(auto t:trains)trainMap.emplace(t.id,t);
  uint32_t rc=0,oc=0;auto rs=NimbySdk_CopyTrackReservations(snap,nullptr,0,&rc),os=NimbySdk_CopyTrackOccupations(snap,nullptr,0,&oc);
  std::vector<NimbyTrackUsage> r(rc),o(oc);if(!rs)NimbySdk_CopyTrackReservations(snap,r.data(),rc,&rc);if(!os)NimbySdk_CopyTrackOccupations(snap,o.data(),oc,&oc);
  std::set<Key> current;std::set<uint64_t> owners;for(auto v:r){current.emplace(v.train_id,v.track_id,v.fraction_begin,v.fraction_end);owners.insert(v.train_id);}
  unsigned inc=0,dec=0;for(auto k:current)inc+=!previous.contains(k);for(auto k:previous)dec+=!current.contains(k);
  if(rs==0){if(success){added+=inc;removed+=dec;}previous=std::move(current);++success;}
  auto ms=std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now()-start).count();
  std::printf("frame=%d reservations_status=%u count=%u owners=%zu occupations_status=%u count=%u added=%u removed=%u ms=%lld\n",frame,rs,rc,owners.size(),os,oc,inc,dec,ms);
  if(frame==0)for(size_t i=0;i<std::min<size_t>(r.size(),8);++i){auto v=r[i];std::printf("sample owner=%llx name=%s track=%llx interval=%.6f..%.6f\n",v.train_id,trainMap[v.train_id].name_utf8,v.track_id,v.fraction_begin,v.fraction_end);}
  NimbySdk_ReleaseSnapshot(snap);Sleep(100);
 }
 NimbySdk_CloseSession(s);std::printf("RESULT available=%u added=%u removed=%u\n",success,added,removed);return success&&added&&removed?0:3;
}
