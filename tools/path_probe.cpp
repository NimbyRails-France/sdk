// Read-only research. Candidate offsets from serializers; not a public path API.
#define WinMain MonitorWinMain
#include "../src/train_monitor.cpp"
#undef WinMain
#include <unordered_set>
int main(int argc,char** argv) {
    if(argc!=2)return 1;
    Reader r{OpenProcess(PROCESS_QUERY_INFORMATION|PROCESS_VM_READ,FALSE,std::strtoul(argv[1],nullptr,10))};
    if(!r.handle)return 2;
    wchar_t path[32768]{};DWORD length=32768;
    if(!QueryFullProcessImageNameW(r.handle,0,path,&length))return 3;
    NimbyBinaryInfo identity{};
    if(nimby::engine::identify(path,identity)!=NIMBY_OK||!identity.recognized_research_build)return 4;
    nimby::engine::LiveState state{},after{};
    if(!nimby::engine::resolve_live_state(live_read,&r,image_base(r),true,state))return 5;
    nimby::engine::Network network;
    if(!nimby::engine::read_network(live_read,&r,state,true,network))return 6;
    std::unordered_set<uint64_t> tracks;for(auto& t:network.tracks)tracks.insert(t.id);
    Pool pool{};pool.address=state.simulation+0xa0;pool.owner=state;
    if(!r.read(pool.address,pool.header.data(),48)||!decode(pool.header.data(),pool))return 7;
    std::vector<Item> rows;if(!items(r,pool,true,rows))return 8;
    std::printf("sha256=%s root=%llx db=%llx sim=%llx tracks=%zu\n",identity.sha256,state.root,state.database,state.simulation,tracks.size());
    unsigned printed=0;
    for(auto& row:rows) {
        if(!row.present)continue;
        std::array<unsigned char,0x110> p{},again{};
        if(!r.read(row.address+0x290,p.data(),p.size()))return 9;
        const auto begin=at<uint64_t>(p.data(),0xa8),end=at<uint64_t>(p.data(),0xb0),cap=at<uint64_t>(p.data(),0xb8);
        if(end<begin||cap<end||end-begin>8*65536||(end-begin)%8)continue;
        std::vector<uint64_t> entries((end-begin)/8);
        if(!entries.empty()&&!r.read(begin,entries.data(),entries.size()*8))continue;
        if(!r.read(row.address+0x290,again.data(),again.size())||again!=p)continue;
        size_t match=0;for(auto e:entries)match+=tracks.contains(e);
        for(size_t i=0;i<std::min<size_t>(entries.size(),16);++i){
            Pool tp{};tp.address=state.database;
            if(!r.read(tp.address,tp.header.data(),48)||!decode(tp.header.data(),tp))return 11;
            const auto index=(entries[i]>>16)&0xffffffffULL;uint64_t block{};
            if(!r.read(tp.begin+(index>>tp.shift)*8,&block,8))return 12;
            std::array<unsigned char,0x88> t{};
            if(!r.read(block+(index&tp.mask)*0x4e8,t.data(),t.size())||at<uint64_t>(t.data(),0)!=entries[i])return 13;
            std::printf("node seq=%zu id=%llx a=%llx b=%llx c=%llx xy=%g,%g\n",i,entries[i],at<uint64_t>(t.data(),8),at<uint64_t>(t.data(),16),at<uint64_t>(t.data(),24),at<double>(t.data(),0x30),at<double>(t.data(),0x38));
        }
        std::printf("candidate train=%llx found=%u start=%llx goal=%llx entries=%zu track_id_matches=%zu first=",row.id,p[0x90],at<uint64_t>(p.data(),0x60),at<uint64_t>(p.data(),0x78),entries.size(),match);
        for(size_t i=0;i<std::min<size_t>(entries.size(),12);++i)std::printf("%llx,",entries[i]);std::puts("");
        if(++printed==24)break;
    }
    if(!nimby::engine::resolve_live_state(live_read,&r,state.module_base,true,after)||state!=after)return 10;
    std::puts("Candidate data only; sequence semantics and reservations NOT validated.");
}
