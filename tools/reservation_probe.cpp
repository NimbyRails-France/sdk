// Read-only candidate-layout probe. Offsets derived from RVA 0x6eb6d0 and 0x452250.
#define WinMain MonitorWinMain
#include "../src/train_monitor.cpp"
#undef WinMain
#include <unordered_set>
int main(int argc,char** argv){
 if(argc!=2)return 1;Reader r{OpenProcess(PROCESS_QUERY_INFORMATION|PROCESS_VM_READ,FALSE,std::strtoul(argv[1],nullptr,10))};if(!r.handle)return 2;
 wchar_t exe[32768]{};DWORD len=32768;NimbyBinaryInfo bi{};
 if(!QueryFullProcessImageNameW(r.handle,0,exe,&len)||nimby::engine::identify(exe,bi)!=NIMBY_OK||!bi.recognized_research_build)return 3;
 for(unsigned frame=0;frame<6;++frame){
 nimby::engine::LiveState s{};if(!nimby::engine::resolve_live_state(live_read,&r,image_base(r),true,s))return 4;
 uint64_t query{},head[3]{};if(!r.read(s.simulation+0x2200,&query,8)||!r.read(query+0x410,head,24))return 5;
 std::printf("FRAME %u sim=%llx query=%llx partitions=%llx,%llx,%llx\n",frame,s.simulation,query,head[0],head[1],head[2]);
 if(head[1]<head[0]||(head[1]-head[0])%0x40||(head[1]-head[0])/0x40>512)return 6;
 size_t count=0,bad=0;std::unordered_set<uint64_t> owners,trackIds,visited;
 for(uint64_t part=head[0];part<head[1];part+=0x40){
  uint64_t h[8]{};if(!r.read(part,h,64))return 7;const auto buckets=h[3],n=h[4];
  if(n>1048576)return 8;std::vector<uint64_t> bs(n);if(n&&!r.read(buckets,bs.data(),n*8))return 9;
  for(auto node:bs)while(node){
   if(!visited.insert(node).second||visited.size()>1048576)return 10;
   unsigned char entry[48]{};if(!r.read(node,entry,sizeof entry))return 11;
   auto trackId=at<uint64_t>(entry,0),owner=at<uint64_t>(entry,8);double from=at<double>(entry,24),to=at<double>(entry,32);
   bad+=(trackId>>48)!=1||(owner>>48)!=5||!std::isfinite(from)||!std::isfinite(to)||from<0||from>1||to<0||to>1;
   owners.insert(owner);trackIds.insert(trackId);
   if(frame==0&&count<12)std::printf("RES track=%llx train=%llx aux=%llx from=%.9f to=%.9f\n",trackId,owner,at<uint64_t>(entry,16),from,to);
   ++count;node=at<uint64_t>(entry,40);
  }
 }
 uint64_t qAfter{};r.read(s.simulation+0x2200,&qAfter,8);
 std::printf("RES total=%zu bad=%zu owners=%zu tracks=%zu queryChanged=%u\n",count,bad,owners.size(),trackIds.size(),query!=qAfter);
  uint64_t snapshotBase{};r.read(s.simulation+0x2208,&snapshotBase,8); for(auto candidate:{s.simulation}){
  size_t rv=0,invalid=0;std::unordered_set<uint64_t> own;
  for(unsigned shard=0;shard<16;++shard){
   uint64_t h[7]{};if(!r.read(candidate+0xd98+shard*0x88+0x50,h,sizeof h))continue;
   if(frame==0&&shard<2)std::printf("SWISS base=%llx shard=%u ctrl=%llx slots=%llx size=%llu mask=%llu\n",candidate,shard,h[0],h[1],h[2],h[3]);
   if(h[3]>1048575||!h[0]||!h[1]||h[2]>h[3])continue;
   std::vector<unsigned char> ctrl(h[3]),slots(h[3]*32);
   if(h[3]&&(!r.read(h[0],ctrl.data(),ctrl.size())||!r.read(h[1],slots.data(),slots.size())))continue;
   for(size_t i=0;i<ctrl.size();++i)if(ctrl[i]<128){
    auto* v=slots.data()+i*32;auto key=at<uint64_t>(v,0),begin=at<uint64_t>(v,8),end=at<uint64_t>(v,16);
    if((key>>48)!=1||end<begin||(end-begin)%32||end-begin>1048576){++invalid;continue;}
    std::vector<unsigned char> records(end-begin);if(!records.empty()&&!r.read(begin,records.data(),records.size())){++invalid;continue;}
    for(size_t j=0;j<records.size();j+=32){auto* a=records.data()+j;auto tr=at<uint64_t>(a,0);double from=at<double>(a,16),to=at<double>(a,24);invalid+=(tr>>48)!=5||!std::isfinite(from)||!std::isfinite(to)||from<0||from>1||to<0||to>1;own.insert(tr);if(frame==0&&rv<8)std::printf("SWISSRES track=%llx train=%llx aux=%llx from=%.8f to=%.8f\n",key,tr,at<uint64_t>(a,8),from,to);++rv;}
   }
  }
  std::printf("SWISS base=%llx reservations=%zu invalid=%zu owners=%zu\n",candidate,rv,invalid,own.size());
 }
 uint64_t occ[10]{};if(!r.read(s.simulation+0x2c8,occ,sizeof occ))return 12;
 std::printf("OCC header=");for(auto x:occ)std::printf("%llx,",x);std::puts("");
 if(occ[1]<occ[0]||(occ[1]-occ[0])%8||(occ[1]-occ[0])/8>1048576)return 13;
 size_t cells=(occ[1]-occ[0])/8;std::vector<uint64_t> keys(cells);std::vector<unsigned char> values(cells*32);
 if(cells&&(!r.read(occ[0],keys.data(),cells*8)||!r.read(occ[3],values.data(),values.size())))return 14;
 size_t occupied=0,occBad=0;for(size_t i=0;i<cells;++i)if(keys[i]){auto* v=values.data()+i*32;auto train=at<uint64_t>(v,0);double a=at<double>(v,8),b=at<double>(v,16);occBad+=(keys[i]>>48)!=1||(train>>48)!=5||!std::isfinite(a)||!std::isfinite(b)||a<0||b>1;if(frame==0&&occupied<12)std::printf("OCC track=%llx train=%llx from=%.9f to=%.9f aux=%llx\n",keys[i],train,a,b,at<uint64_t>(v,24));++occupied;}
 std::printf("OCC count=%zu bad=%zu\n",occupied,occBad);Sleep(250);
 }
}
