// Read-only candidate research, not a public SDK contract or command interface.
#include "engine/binary_identity.h"
#include "engine/live_state.h"
#include <windows.h>
#include <tlhelp32.h>
#include <array>
#include <charconv>
#include <chrono>
#include <cmath>
#include <cstdio>
#include <cstring>
#include <string_view>

struct Reader {
    HANDLE process{};
    ~Reader() { if (process) CloseHandle(process); }
};
bool read(void* context, uint64_t address, void* output, size_t size) {
    SIZE_T received{};
    return address >= 0x10000 && address <= 0x7fffffffffffULL - size &&
        ReadProcessMemory(static_cast<Reader*>(context)->process,
            reinterpret_cast<void*>(address), output, size, &received) && received == size;
}
template<class T> T field(const unsigned char* bytes, size_t offset) {
    T value{}; std::memcpy(&value, bytes + offset, sizeof value); return value;
}
uint64_t number(std::string_view text) {
    uint64_t value{};
    auto [end, error] = std::from_chars(text.data(), text.data() + text.size(), value);
    return error == std::errc{} && end == text.data() + text.size() ? value : 0;
}
bool dynamics(Reader& r, uint64_t pool, uint64_t id, size_t stride, size_t offset,
              std::array<unsigned char, 0x44>& result) {
    std::array<unsigned char,48> header{}, after{};
    if (!read(&r,pool,header.data(),header.size())) return false;
    auto shift=field<uint32_t>(header.data(),4), count=field<uint32_t>(header.data(),8);
    auto mask=field<uint32_t>(header.data(),16);
    auto begin=field<uint64_t>(header.data(),24), end=field<uint64_t>(header.data(),32);
    auto cap=field<uint64_t>(header.data(),40);
    if ((id>>48)!=5 || shift<1 || shift>16 || count!=(1u<<shift) || mask!=count-1 ||
        begin%8 || end<begin || cap<end || cap-begin>8192 || (end-begin)%8) return false;
    const auto index=(id>>16)&0xffffffffULL, blockIndex=index>>shift;
    uint64_t block{}, verifiedBlock{}, firstId{}, secondId{};
    if(blockIndex >= (end-begin)/8 || !read(&r,begin+blockIndex*8,&block,8) || block<0x10000) return false;
    const auto address=block+(index&mask)*stride;
    std::array<unsigned char,0x44> again{};
    if(!read(&r,address,&firstId,8) || firstId!=id ||
       !read(&r,address+offset,result.data(),result.size()) ||
       !read(&r,address+offset,again.data(),again.size()) || result!=again ||
       !read(&r,address,&secondId,8) || secondId!=id ||
       !read(&r,begin+blockIndex*8,&verifiedBlock,8) || verifiedBlock!=block ||
       !read(&r,pool,after.data(),after.size()) || after!=header) return false;
    return true;
}
void print(const char* source, const std::array<unsigned char,0x44>& data) {
    constexpr const char* labels[]{"max_speed", "max_acceleration", "max_regular_braking",
        "max_emergency_braking", "max_tractive_effort", "total_power", "empty_mass", "length"};
    std::printf("%s",source);
    for(size_t i=0;i<8;++i)
        std::printf(" %s=%.9g",labels[i],field<float>(data.data(),0x1c+i*4));
    std::puts("");
}
int main(int argc,char** argv) {
    if(argc!=3) {std::fprintf(stderr,"driving_probe PID TRAIN_ID_DECIMAL (read-only)\n");return 1;}
    const auto pid=number(argv[1]), id=number(argv[2]);
    if(!pid || pid>MAXDWORD || !id)return 2;
    Reader r{OpenProcess(PROCESS_QUERY_INFORMATION|PROCESS_VM_READ,FALSE,static_cast<DWORD>(pid))};
    if(!r.process)return 3;
    wchar_t path[32768]{};DWORD size=32768;NimbyBinaryInfo identity{};
    if(!QueryFullProcessImageNameW(r.process,0,path,&size) ||
       nimby::engine::identify(path,identity)!=NIMBY_OK || !identity.recognized_research_build)return 4;
    HANDLE modules=CreateToolhelp32Snapshot(TH32CS_SNAPMODULE,static_cast<DWORD>(pid));
    if(modules==INVALID_HANDLE_VALUE)return 5;
    MODULEENTRY32W module{};module.dwSize=sizeof module;
    const auto base=Module32FirstW(modules,&module)?reinterpret_cast<uint64_t>(module.modBaseAddr):0;
    CloseHandle(modules);
    const auto start=std::chrono::steady_clock::now();
    nimby::engine::LiveState state{},after{};
    std::array<unsigned char,0x44> purchased{},current{};
    if(!nimby::engine::resolve_live_state(read,&r,base,true,state) ||
       !dynamics(r,state.database+0x200,id,0x178,0xc0,purchased) ||
       !dynamics(r,state.simulation+0xa0,id,0x638,8,current) ||
       !nimby::engine::resolve_live_state(read,&r,base,true,after) || state!=after)return 6;
    for (const auto* bytes : {&purchased, &current}) {
        for (size_t offset=0x1c;offset<=0x38;offset+=4)
            if (!std::isfinite(field<float>(bytes->data(),offset)) || field<float>(bytes->data(),offset)<0) return 7;
    }
    std::printf("sha256=%s train=%llu elapsed_us=%lld\n",identity.sha256,
        static_cast<unsigned long long>(id),static_cast<long long>(
        std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now()-start).count()));
    print("Train+0xc0",purchased);print("Motion+8",current);
    return 0;
}
