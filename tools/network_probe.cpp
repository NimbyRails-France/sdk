// Read-only discovery utility, not an SDK API. Reuses the monitor's guarded reader.
#define WinMain MonitorWinMain
#include "../src/train_monitor.cpp"
#undef WinMain
int main(int argc, char** argv) {
    if (argc != 2) return 1;
    Reader r{OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, std::strtoul(argv[1], nullptr, 10))};
    if (!r.handle) return 2;
    wchar_t path[32768]{}; DWORD length=32768;
    if (!QueryFullProcessImageNameW(r.handle,0,path,&length)) return 3;
    NimbyBinaryInfo identity{};
    if (nimby::engine::identify(path,identity)!=NIMBY_OK || !identity.recognized_research_build) return 4;
    nimby::engine::LiveState state{};
    if (!nimby::engine::resolve_live_state(live_read,&r,image_base(r),true,state)) return 5;
    std::printf("base=%llx root=%llx db=%llx sim=%llx\n",state.module_base,state.root,state.database,state.simulation);
    float threshold{};r.read(state.module_base+0xaab904,&threshold,4);std::printf("speed_threshold=%g\n",threshold);
    for (uint64_t offset=0;offset<0x400;offset+=8) {
        Pool pool{}; pool.address=state.database+offset;
        if (!r.read(pool.address,pool.header.data(),48)||!decode(pool.header.data(),pool)) continue;
        uint64_t block{}; if (!r.read(pool.begin,&block,8)) continue;
        std::array<unsigned char,0x100> b{}; if (!r.read(block,b.data(),b.size())) continue;
        std::printf("POOL_CANDIDATE db+%llx shift=%u size=%u first=%llx bytes=",offset,pool.shift,pool.size,block);
        for (auto byte:b) std::printf("%02x",byte); std::puts("");
    }
    Pool pool{}; pool.address=state.simulation+0xa0;pool.owner=state;
    if(!r.read(pool.address,pool.header.data(),48)||!decode(pool.header.data(),pool)) return 6;
    std::vector<Item> rows; if(!items(r,pool,true,rows)) return 7;
    for(const auto& row:rows) {
        std::array<unsigned char,0x50> p{};r.read(row.address+0x390,p.data(),p.size());
        std::printf("PRESENCE id=%llx address=%llx speed=%f bytes=",row.id,row.address,row.speed);
        for(auto byte:p) std::printf("%02x",byte); std::puts("");
    }
    for (auto spec : {std::array<uint64_t,3>{0,0x4e8,1}, {0x80,0x3e8,2}, {0x380,0xc8,8}}) {
        Pool np{};np.address=state.database+spec[0];
        if(!r.read(np.address,np.header.data(),48)||!decode(np.header.data(),np)) return 8;
        size_t count=0,invalid=0;
        for(uint64_t b=0;b<(np.end-np.begin)/8;++b) {
            uint64_t block{};if(!r.read(np.begin+b*8,&block,8)) return 9;
            Bytes data(np.size*spec[1]);if(!r.read(block,data.data(),data.size())) return 10;
            for(size_t slot=0;slot<np.size;++slot) {
                auto* p=data.data()+slot*spec[1];auto id=at<uint64_t>(p,0);
                if((id>>48)==0||(id>>48)==0xffff) continue;
                if((id>>48)!=spec[2]||((id>>16)&0xffffffff)!=(b<<np.shift)+slot){++invalid;continue;}
                ++count;
                if(spec[2]==1) std::printf("TRACK id=%llx physical=%g manual=%g station=%llx\n",id,at<float>(p,0x80),at<float>(p,0x84),at<uint64_t>(p,0xd0));
                if(spec[2]==2) {std::string name;string_at(r,p+0x20,name);std::printf("STATION id=%llx name=%s flag=%u\n",id,name.c_str(),p[0x40]);}
                if(spec[2]==8) std::printf("SIGNAL id=%llx kind=%u track=%llx fraction=%g direction=%d\n",id,at<uint32_t>(p,0x30),at<uint64_t>(p,0x40),at<double>(p,0x48),at<int8_t>(p,0x50));
            }
        }
        std::printf("COLLECTION type=%llu count=%zu invalid=%zu\n",spec[2],count,invalid);
    }
}
