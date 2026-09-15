// Read-only native render-state evidence. Never calls or modifies game code.
#define WinMain MonitorWinMain
#include "../src/train_monitor.cpp"
#undef WinMain
#include <map>
int main(int argc,char** argv) {
 if(argc!=2)return 1;
 Reader r{OpenProcess(PROCESS_QUERY_INFORMATION|PROCESS_VM_READ,FALSE,std::strtoul(argv[1],nullptr,10))};if(!r.handle)return 2;
 wchar_t exe[32768]{};DWORD len=32768;NimbyBinaryInfo bi{};
 if(!QueryFullProcessImageNameW(r.handle,0,exe,&len)||nimby::engine::identify(exe,bi)!=NIMBY_OK||!bi.recognized_research_build)return 3;
 std::map<uint64_t,int> previous;unsigned changes=0;
 for(unsigned frame=0;frame<100;++frame){
  nimby::engine::LiveState s{};if(!nimby::engine::resolve_live_state(live_read,&r,image_base(r),true,s))return 4;
  uint64_t query{},h[4]{};if(!r.read(s.simulation+0x2200,&query,8)||!r.read(query+0x378,h,sizeof h))return 5;
  if(h[3]>1048575||h[2]>h[3])return 6;
  std::vector<unsigned char> ctrl(h[3]),slots(h[3]*16);
  if(h[3]&&(!r.read(h[0],ctrl.data(),ctrl.size())||!r.read(h[1],slots.data(),slots.size())))return 7;
  std::map<uint64_t,int> states;
  for(size_t i=0;i<ctrl.size();++i)if(ctrl[i]<128){auto* v=slots.data()+i*16;auto id=at<uint64_t>(v,0);if((id>>48)!=8)return 8;states[id]=at<int32_t>(v,8);}
  if(frame==0)std::printf("TABLE size=%llu mask=%llu observed=%zu\n",h[2],h[3],states.size());
  if(frame==0||states!=previous){for(auto [id,value]:states)if(frame==0||!previous.contains(id)||previous[id]!=value){std::printf("FRAME %u signal=%llx texture_state=%d\n",frame,id,value);++changes;}std::fflush(stdout);}
  previous=states;Sleep(100);
 }
 std::printf("Changes including initial values: %u\n",changes);
}
