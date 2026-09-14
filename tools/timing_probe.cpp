#define WinMain MonitorWinMain
#include "../src/train_monitor.cpp"
#undef WinMain
int main(int argc,char** argv){
 if(argc!=2)return 1;Reader r{OpenProcess(PROCESS_QUERY_INFORMATION|PROCESS_VM_READ,FALSE,std::strtoul(argv[1],nullptr,10))};
 wchar_t file[32768]{};DWORD n=32768;NimbyBinaryInfo id{};
 if(!r.handle||!QueryFullProcessImageNameW(r.handle,0,file,&n)||nimby::engine::identify(file,id)!=NIMBY_OK||!id.recognized_research_build)return 2;
 for(int tick=0;tick<4;++tick){nimby::engine::LiveState state{};if(!nimby::engine::resolve_live_state(live_read,&r,image_base(r),true,state))return 3;
 for(int offset=0;offset<0x1800;offset+=8){uint64_t owner{};r.read(state.root+offset,&owner,8);Pool pool{};pool.address=owner+0xa0;
 if(!r.read(pool.address,pool.header.data(),48)||!decode(pool.header.data(),pool))continue;
 std::vector<Item> rows;if(!items(r,pool,true,rows))continue;
 unsigned presence=0;for(auto& row:rows){std::array<unsigned char,0x1d1> b{};if(r.read(row.address,b.data(),b.size())&&b[0x1d0]==1){++presence;if(!row.present&&presence<400)std::printf("PRESENCE_ONLY id=%llx track=%llx fraction=%g dirs=%d,%d\n",row.id,at<uint64_t>(b.data(),0xb8),at<double>(b.data(),0xc0),int(int8_t(b[0xc8])),int(int8_t(b[0xc9])));}}
 if(!rows.empty())std::printf("tick=%d root+%x=%llx count=%zu first=%llx speed=%g track=%llx f=%.8f Presence=%u\n",tick,offset,owner,rows.size(),rows[0].id,rows[0].speed,rows[0].position.track_id,rows[0].position.fraction,presence);
 }Sleep(700);}
}
