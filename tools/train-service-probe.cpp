// Read-only probe of service changes between pool capture and record recheck.
#include "engine/binary_identity.h"
#include "engine/network.h"
#include "engine/track_usage.h"
#include "engine/simulation_clock.h"
#include <windows.h>
#include <tlhelp32.h>
#include <charconv>
#include <chrono>
#include <cmath>
#include <algorithm>
#include <cstdio>
#include <string_view>
#include <array>
#include <map>
#include <cstring>

struct Process {
    HANDLE handle{};
    uint64_t reads{},bytes{};
    std::map<uint64_t,std::array<unsigned char,0x638>> motions;
    uint64_t compared{},changed{},presenceChanged{},arrivalChanged{},locationChanged{},otherChanged{};
    uint64_t driveRecheckRejected{};
    ~Process(){if(handle)CloseHandle(handle);}
};
static bool read(void* context,uint64_t address,void* out,size_t size){
    auto& process=*static_cast<Process*>(context);
    ++process.reads;process.bytes+=size;
    SIZE_T received{};
    if(!ReadProcessMemory(process.handle,reinterpret_cast<void*>(address),out,size,&received)||received!=size)return false;
    auto* bytes=static_cast<unsigned char*>(out);
    // This read shape is the native reader's no-Drive speed recheck. A Drive
    // appearing here causes read_trains to reject the whole non-atomic capture.
    if(size==0x4b1&&bytes[0x4b0]!=0)++process.driveRecheckRejected;
    if(size>0x638&&size%0x638==0){
        for(size_t offset=0;offset<size;offset+=0x638){
            uint64_t id{};std::memcpy(&id,bytes+offset,8);
            if((id>>48)==5)std::memcpy(process.motions[address+offset].data(),bytes+offset,0x638);
        }
    }else if(size==0x638){
        const auto found=process.motions.find(address);
        if(found!=process.motions.end()){
            NimbyTrainService before{},after{};
            // Mode zero only for comparing fields; actual read_trains still uses the model mode.
            if(nimby::engine::decode_train_service(found->second.data(),0x638,0,before)&&
               nimby::engine::decode_train_service(out,size,0,after)){
                ++process.compared;
                if(std::memcmp(&before,&after,sizeof before)){
                    ++process.changed;
                    constexpr auto mask=NIMBY_MOTION_PRESENCE|NIMBY_MOTION_HIDDEN|NIMBY_MOTION_DRIVE;
                    process.presenceChanged+=(before.motion_flags&mask)!=(after.motion_flags&mask);
                    process.arrivalChanged+=before.arrival_time_us!=after.arrival_time_us;
                    process.locationChanged+=before.location_track_id!=after.location_track_id;
                    before.arrival_time_us=after.arrival_time_us;
                    before.location_track_id=after.location_track_id;
                    process.otherChanged+=std::memcmp(&before,&after,sizeof before)!=0;
                }
            }
            process.motions.erase(found); // Only the first service recheck, not the later path recheck.
        }
    }
    return true;
}
template<class T> bool parse(const char* text,T& value){
    const std::string_view input=text;
    const auto [end,error]=std::from_chars(input.data(),input.data()+input.size(),value);
    return error==std::errc{}&&end==input.data()+input.size()&&value;
}
int main(int argc,char** argv){try{
    uint32_t pid{};
    if((argc!=2&&argc!=6)||!parse(argv[1],pid)){
        std::puts("Usage: nimby_train_service_probe PID [TRACK_ID BEGIN END COUNT]");return 1;
    }
    Process process;process.handle=OpenProcess(PROCESS_QUERY_INFORMATION|PROCESS_VM_READ,FALSE,pid);
    if(!process.handle)return 2;
    wchar_t path[32768]{};DWORD length=32768;NimbyBinaryInfo binary{};
    if(!QueryFullProcessImageNameW(process.handle,0,path,&length)||
       nimby::engine::identify(path,binary)!=NIMBY_OK||!binary.recognized_research_build)return 3;
    HANDLE modules=CreateToolhelp32Snapshot(TH32CS_SNAPMODULE,pid);
    if(modules==INVALID_HANDLE_VALUE)return 4;
    MODULEENTRY32W module{};module.dwSize=sizeof module;
    const bool moduleFound=Module32FirstW(modules,&module)!=0;CloseHandle(modules);
    if(!moduleFound)return 4;
    nimby::engine::LiveState state{};
    if(!nimby::engine::resolve_live_state(read,&process,reinterpret_cast<uint64_t>(module.modBaseAddr),true,state))return 5;
    if(argc==6){
        uint64_t track{};double begin{},end{};unsigned count{};
        if(!parse(argv[2],track)||(track>>48)!=1||!parse(argv[3],begin)||!parse(argv[4],end)||
           !parse(argv[5],count)||count>1200||!std::isfinite(begin)||!std::isfinite(end)||begin<0||begin>=end||end>1)return 1;
        // Diagnostic cible en lecture seule : aucun resultat vide ne certifie un canton libre.
        // Topologie fournie explicitement par une capture precedente, jamais reutilisee par le mod.
        for(unsigned i=0;i<count;++i){
            std::vector<nimby::engine::TrackUsage> rows;
            nimby::engine::SimulationClock clock{};
            const bool ok=nimby::engine::read_occupations(read,&process,state,true,rows);
            const bool timed=nimby::engine::read_simulation_clock(read,&process,state.simulation,clock);
            std::printf("{\"wall_ms\":%llu,\"ok\":%s,\"ticks\":%lld,\"trains\":[",
                GetTickCount64(),ok?"true":"false",timed?clock.ticks:-1LL);
            bool first=true;
            if(ok)for(const auto& row:rows)if(row.track_id==track&&std::max(begin,row.begin)<=std::min(end,row.end)){
                std::printf("%s\"%llu\"",first?"":",",row.train_id);first=false;
            }
            std::puts("]}");std::fflush(stdout);Sleep(100);
        }
        return 0;
    }
    auto measure=[&](const char* label,auto operation){
        process.reads=0;process.bytes=0;
        const auto start=std::chrono::steady_clock::now();
        const bool ok=operation();
        const auto ms=std::chrono::duration<double,std::milli>(std::chrono::steady_clock::now()-start).count();
        std::printf("%s ok=%d elapsed_ms=%.3f reads=%llu bytes=%llu\n",label,ok,ms,
            static_cast<unsigned long long>(process.reads),static_cast<unsigned long long>(process.bytes));
        return ok;
    };
    std::vector<nimby::engine::Train> trains;
    const bool whole=measure("trains",[&]{return nimby::engine::read_trains(read,&process,state,true,trains);});
    size_t unknown=0,unknownPresence=0;for(const auto& train:trains){
        unknown+=!(train.service.flags&NIMBY_SERVICE_STATE_VALID);
        unknownPresence+=!(train.service.flags&(NIMBY_SERVICE_STATE_VALID|NIMBY_SERVICE_PRESENCE_VALID));
    }
    std::printf("unknown_service=%zu unknown_presence=%zu compared=%llu changed=%llu presence_changed=%llu arrival_changed=%llu location_changed=%llu other_changed=%llu rejected_no_drive_recheck=%llu\n",
        unknown,unknownPresence,process.compared,process.changed,process.presenceChanged,process.arrivalChanged,process.locationChanged,process.otherChanged,process.driveRecheckRejected);
    return whole?0:6;
}catch(...){return 7;}}
