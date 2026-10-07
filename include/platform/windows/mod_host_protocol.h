#pragma once
#include <nimby/detail/mod_host.h>
#include <nimby/detail/observation.h>
#include <array>
#include <cstdint>
#include <cstring>
#include <stdexcept>
#include <vector>
#include <unordered_set>

namespace nimby::mod_host {
inline constexpr uint32_t protocol = 1;
inline constexpr uint32_t payloadLimit = 16 * 1024 * 1024 + 65536;
// Operation ranges: 1 ensure UI, 100..199 UI, 200..299 driving, 300..399 textures.
struct Request {uint32_t operation{}; std::array<uint64_t,8> args{}; std::vector<uint8_t> data;};
struct Reply {std::array<uint64_t,8> args{}; std::vector<uint8_t> data;};
struct Owners {
    struct EpochTicket {uint64_t id{},generation{};int64_t ticks{};uint64_t expires{};};
    std::unordered_set<uint64_t> panels, providers;
    // Parent-owned wake event for this channel. Never populated from an RPC.
    uint64_t actionWake{};
    uint64_t driving{};
    bool drivingTracked=false;
    uint64_t textures{};
    bool texturesTracked=false;
    std::vector<EpochTicket> epochs;
};
// Implemented by SDK-owned facades. No mod function pointer crosses this ABI.
uint32_t dispatchUi(const Request&, Reply&, Owners&);
uint32_t dispatchTools(const Request&, Reply&, Owners&);
void cleanupUi(Owners&) noexcept;
uint32_t dispatchDriving(const Request&, Reply&, Owners&);
void cleanupDriving(Owners&) noexcept;
uint32_t dispatchTextures(const Request&, Reply&, Owners&);
void cleanupTextures(Owners&) noexcept;
inline uint32_t invoke(const Request& request, Reply& reply, uint32_t capacity=payloadLimit) {
    if(request.data.size()>payloadLimit||capacity>payloadLimit)return NIMBY_RESOURCE_LIMIT;
    reply.args=request.args;reply.data.resize(capacity);uint32_t written{};
    const auto status=NimbyInternal_ModHostCall(request.operation,reply.args.data(),
        request.data.data(),static_cast<uint32_t>(request.data.size()),reply.data.data(),capacity,&written);
    if(written>capacity){reply.data.clear();return NIMBY_INVALID_BINARY;}
    reply.data.resize(written);return status;
}
template<class T> void append(Request& request,const T* values,size_t count=1) {
    if(count>payloadLimit/sizeof(T)||(!values&&count)||request.data.size()>payloadLimit-count*sizeof(T))
        throw std::invalid_argument("Invalid isolated mod payload");
    const auto offset=request.data.size();request.data.resize(offset+count*sizeof(T));
    if(count)std::memcpy(request.data.data()+offset,values,count*sizeof(T));
}
template<class T> void output(Reply& reply,const T* values,size_t count=1) {
    if(count>payloadLimit/sizeof(T)||(!values&&count))throw std::invalid_argument("Invalid isolated mod reply");
    reply.data.resize(count*sizeof(T));if(count)std::memcpy(reply.data.data(),values,count*sizeof(T));
}
// Copy out of the untrusted wire representation: never dereference an
// unaligned structure or a pointer supplied by a mod process.
template<class T> bool read(const Request& request,size_t offset,T& result) {
    if(offset>request.data.size()||sizeof(T)>request.data.size()-offset)return false;
    std::memcpy(&result,request.data.data()+offset,sizeof(T));return true;
}
}
