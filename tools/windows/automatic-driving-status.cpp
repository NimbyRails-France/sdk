#include <platform/windows/runtime/automatic_driving_status.h>
#include <iostream>
#include <iomanip>
int main(int argc,char** argv){
 if(argc!=2)return 2;
 const auto pid=std::stoul(argv[1]);
 const auto mapping=OpenFileMappingW(FILE_MAP_READ,FALSE,nimby::automatic_status::name(pid).c_str());
 if(!mapping){std::cerr<<"No automatic driving bridge\n";return 3;}
 const auto* data=static_cast<const nimby::automatic_status::Shared*>(MapViewOfFile(mapping,FILE_MAP_READ,0,0,0));
 if(!data){CloseHandle(mapping);return 4;}
 if(data->size!=sizeof(*data)||data->version!=4){UnmapViewOfFile(data);CloseHandle(mapping);return 5;}
 std::cout<<std::setprecision(17);
 for(long i=0;i<4096&&i<data->count;++i){const auto& s=data->samples[i];if(!s.ready)continue;
  std::cout<<"{\"train\":\""<<s.train<<"\",\"before\":"<<s.before<<",\"after\":"<<s.after<<",\"head_before\":"<<s.headBefore
   <<",\"head_after\":"<<s.headAfter<<",\"ceiling\":"<<s.ceiling<<",\"native_ceiling\":"<<s.nativeCeiling<<",\"braking\":"<<s.braking<<",\"ticks\":"<<s.ticks
   <<",\"restricted_source\":\""<<s.restrictedSource<<"\",\"free_distance\":"<<s.freeDistance<<",\"visibility_verified\":"<<s.visibilityVerified<<"}\n";
 }
 for(long i=0;i<512&&i<data->permissionCount;++i){const auto& s=data->permissions[i];if(!s.ready)continue;
  std::cout<<"{\"permission\":true,\"train\":\""<<s.train<<"\",\"signal\":\""<<s.signal<<"\",\"proof\":\""<<s.proof
   <<"\",\"flags\":"<<s.flags<<",\"state\":"<<s.state<<",\"ranges\":"<<s.ranges<<",\"head\":"<<s.head
   <<",\"source\":"<<s.source<<",\"covered\":"<<s.covered<<",\"free\":"<<s.free<<"}\n";
 }
 std::cout<<"{\"summary\":true,\"session\":\""<<data->session<<"\",\"rules\":"<<data->ruleCount
  <<",\"scans\":"<<data->scans<<",\"applied\":"<<data->applied<<",\"stops\":"<<data->stops<<",\"last_signal\":\""<<data->lastSignal
  <<"\",\"restricted_checks\":"<<data->restrictedChecks<<",\"restricted_granted\":"<<data->restrictedGranted
  <<",\"restricted_denied\":"<<data->restrictedDenied<<",\"restricted_steps\":"<<data->restrictedSteps<<"}\n";
 UnmapViewOfFile(data);CloseHandle(mapping);return 0;
}
