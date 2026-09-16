// Read-only temporal probe through the public SDK.
#include <nimby/detail/observation.h>
#include <chrono>
#include <thread>
#include <vector>
#include <map>
#include <cstdio>
#include <cstdlib>
int main(int argc,char** argv){
 if(argc<2)return 1;NimbySession session{};
 if(NimbyInternal_OpenProcess(NIMBY_OBSERVATION_ABI_VERSION,std::strtoul(argv[1],nullptr,10),&session)!=NIMBY_OK)return 2;
 const auto start=std::chrono::steady_clock::now();std::map<uint64_t,int> previous;
 for(int frame=0;frame<160;++frame){NimbySnapshot snapshot{};
  if(NimbyInternal_CaptureSnapshot(session,&snapshot)==NIMBY_OK){uint32_t n{};
   if(frame==0){NimbyInternal_CopySignals(snapshot,nullptr,0,&n);std::vector<NimbySignal> signals(n);NimbyInternal_CopySignals(snapshot,signals.data(),n,&n);
    for(auto& s:signals)std::printf("POSITION signal=%llx track=%llx fraction=%.3f kind=%d\n",static_cast<unsigned long long>(s.id),static_cast<unsigned long long>(s.track_id),s.track_fraction,s.kind);
   }
   NimbyInternal_CopySignalTextures(snapshot,nullptr,0,&n);std::vector<NimbySignalTexture> rows(n);
   NimbyInternal_CopySignalTextures(snapshot,rows.data(),n,&n);
   for(const auto& row:rows)if(row.selected_index==9||row.selected_index==10){
    if(!previous.contains(row.signal_id)||previous[row.signal_id]!=row.selected_index){
     auto ms=std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now()-start).count();
     std::printf("%lld ms signal=%llx state=%d file=%s\n",static_cast<long long>(ms),static_cast<unsigned long long>(row.signal_id),row.selected_index,row.relative_path_utf8);std::fflush(stdout);
     previous[row.signal_id]=row.selected_index;
    }
   }
   NimbyInternal_ReleaseSnapshot(snapshot);
  }
  std::this_thread::sleep_for(std::chrono::milliseconds(40));
 }
 NimbyInternal_CloseSession(session);return 0;
}
