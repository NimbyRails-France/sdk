// Read-only research of the game's loaded SignalTextures table.
#define WinMain MonitorWinMain
#include "../src/train_monitor.cpp"
#undef WinMain
int main(int argc,char** argv){
 if(argc!=2)return 1;
 Reader r{OpenProcess(PROCESS_QUERY_INFORMATION|PROCESS_VM_READ,FALSE,std::strtoul(argv[1],nullptr,10))};if(!r.handle)return 2;
 wchar_t exe[32768]{};DWORD len=32768;NimbyBinaryInfo bi{};
 if(!QueryFullProcessImageNameW(r.handle,0,exe,&len)||nimby::engine::identify(exe,bi)!=NIMBY_OK||!bi.recognized_research_build)return 3;
 nimby::engine::LiveState s{};if(!nimby::engine::resolve_live_state(live_read,&r,image_base(r),true,s))return 4;
 for(auto offset:{0xb77bc0,0xb77d50}){std::array<unsigned char,32> value{};if(r.read(s.module_base+offset,value.data(),value.size())){
  const auto length=at<uint64_t>(value.data(),16),cap=at<uint64_t>(value.data(),24);
  std::printf("PATHROOT %x len=%llu cap=%llu\n",offset,length,cap);
  if(length<1000){std::vector<wchar_t> text(length+1);if(cap<=7)std::memcpy(text.data(),value.data(),(length+1)*2);else r.read(at<uint64_t>(value.data(),0),text.data(),(length+1)*2);std::printf("ROOT %ls\n",text.data());}
 }}
 for(uint64_t offset=0x500;offset<0x548;offset+=8){uint64_t value{};r.read(s.root+offset,&value,8);std::printf("root+%llx=%llx\n",offset,value);}
 uint64_t rules=s.database+0xa80,h[4]{};if(!r.read(rules+0x138,h,sizeof h))return 5;
 std::printf("rules=%llx table=%llx buckets=%llu size=%llu\n",rules,h[1],h[2],h[3]);
 if(h[2]>65536)return 6;
 std::vector<uint64_t> buckets(h[2]);if(!r.read(h[1],buckets.data(),buckets.size()*8))return 7;
 unsigned count=0;
 for(auto node:buckets)while(node&&count++<65536){
  std::array<unsigned char,0xa0> data{};if(!r.read(node,data.data(),data.size()))return 8;
  std::string name,label;string_at(r,data.data()+8,name);string_at(r,data.data()+0x60,label);
  auto begin=at<uint64_t>(data.data(),0x80),end=at<uint64_t>(data.data(),0x88);
  std::printf("ATLAS %llx name=%s label=%s count=%llu\n",at<uint64_t>(data.data(),0),name.c_str(),label.c_str(),(end-begin)/0x50);
  if(end<begin||end-begin>256*0x50)return 9;
  for(uint64_t pos=begin;pos<end;pos+=0x50){std::array<unsigned char,0x50> file{};if(!r.read(pos,file.data(),file.size()))return 10;
   std::string mod,path;string_at(r,file.data()+8,mod);string_at(r,file.data()+0x28,path);
   std::printf(" TEXTURE %llu source=%d mod=%s path=%s hash=%llx\n",(pos-begin)/0x50,at<int>(file.data(),0),mod.c_str(),path.c_str(),at<uint64_t>(file.data(),0x48));
  }
  node=at<uint64_t>(data.data(),0x98);
 }
}
