#pragma once
#include <engine/network.h>
#include <algorithm>
#include <array>
#include <cmath>
#include <cstring>
#include <cwchar>
#include <cwctype>
#include <string>
#include <vector>
namespace nimby::research::monitor {
// Read-only model shared by diagnostics. Reader supplies read(address,buffer,size);
// the host supplies a qualified object layout and native-string decoder. This
// never implies that a Windows game ABI can be used to decode a Linux process.
struct ObjectLayout {size_t motionStride,trainStride,present,speed,name,code;};
using Bytes = std::vector<unsigned char>;
template<class T> T at(const unsigned char* p, size_t off) {
    T result{}; std::memcpy(&result, p + off, sizeof result); return result;
}
struct Pool {
    uint64_t address{}, begin{}, end{};
    uint32_t shift{}, size{}, mask{};
    std::array<unsigned char, 48> header{};
    nimby::engine::LiveState owner{};
};
template<class Reader> bool current_owner(const Reader& reader, const Pool& pool) {
    if (!pool.owner.module_base) return true; // Synthetic fixtures / read-only discovery.
    nimby::engine::LiveState current{};
    return nimby::engine::resolve_live_state([](void* context,uint64_t address,void* out,size_t size){return static_cast<const Reader*>(context)->read(address,out,size);}, const_cast<Reader*>(&reader), pool.owner.module_base, true, pool.owner.profile, current) && current == pool.owner;
}
inline bool decode(const unsigned char* p, Pool& pool) {
    pool.shift = at<uint32_t>(p, 4); pool.size = at<uint32_t>(p, 8);
    pool.mask = at<uint32_t>(p, 16); pool.begin = at<uint64_t>(p, 24); pool.end = at<uint64_t>(p, 32);
    const auto capacity = at<uint64_t>(p, 40);
    return pool.shift > 0 && pool.shift <= 16 && pool.size == (1u << pool.shift) &&
        pool.mask == pool.size - 1 && pool.begin >= 0x10000 && pool.begin % 8 == 0 &&
        pool.end > pool.begin && pool.end <= capacity && capacity - pool.begin <= 8192 &&
        (pool.end - pool.begin) % 8 == 0;
}
struct Item { uint64_t id{}, address{}; std::string name, code; bool present{}; double speed{};
    bool positioned{}; nimby::engine::TrainPosition position{}; };
// A successful scan is still an optimistic external snapshot, not a simulation-thread snapshot.
template<class Reader,class DecodeString>
bool items(const Reader& r,const Pool& pool,bool motion,std::vector<Item>& out,
           const ObjectLayout& layout,DecodeString decodeString) {
    out.clear();
    if (!current_owner(r, pool)) return false;
    std::array<unsigned char, 48> header{};
    if (!r.read(pool.address, header.data(), header.size()) || header != pool.header) return false;
    std::vector<uint64_t> blocks((pool.end - pool.begin) / 8);
    if (!r.read(pool.begin, blocks.data(), blocks.size() * 8)) return false;
    const size_t stride = motion ? layout.motionStride : layout.trainStride;
    if (blocks.size() * pool.size > 100000) return false;
    // Descriptors are trusted build data, but reject malformed fixtures before
    // indexing the object buffer. The string adapter consumes a 32-byte object.
    if((motion&&(layout.present>=stride||stride<8||layout.speed>stride-8))||
       (!motion&&(stride<32||layout.name>stride-32||layout.code>stride-32)))return false;
    Bytes block(pool.size * stride);
    for (size_t b = 0; b < blocks.size(); ++b) {
        if (!blocks[b]) continue;
        if (!r.read(blocks[b], block.data(), block.size())) return false;
        for (size_t i = 0; i < pool.size; ++i) {
            const auto* p = block.data() + i * stride;
            const auto id = at<uint64_t>(p, 0), type = id >> 48;
            if (type == 0 || type == 0xffff) continue;
            if (type != 5 || ((id >> 16) & 0xffffffffULL) != (b << pool.shift) + i) return false;
            Item item{}; item.id = id; item.address = blocks[b] + i * stride;
            if (motion) {
                if (p[layout.present] > 1) return false;
                item.present = p[layout.present] != 0;
                if (item.present) {
                    item.speed = at<double>(p, layout.speed);
                    if (!std::isfinite(item.speed) || std::abs(item.speed) > 10000) return false;
                    item.positioned = nimby::engine::decode_train_position(p,stride,item.position);
                }
            } else if (!decodeString(r, p + layout.name, item.name) || !decodeString(r, p + layout.code, item.code)) return false;
            out.push_back(item);
        }
    }
    std::vector<uint64_t> after(blocks.size());
    if (!r.read(pool.begin, after.data(), after.size() * 8) || blocks != after ||
        !r.read(pool.address, header.data(), header.size()) || header != pool.header) return false;
    return !out.empty() && current_owner(r,pool);
}
inline bool parse_speed(const wchar_t* text, double& kmh) {
    if (!text || !*text) return false;
    std::wstring input(text);
    std::replace(input.begin(), input.end(), L',', L'.');
    wchar_t* end{};
    kmh = std::wcstod(input.c_str(), &end);
    if (end == input.c_str()) return false;
    while (std::iswspace(*end)) ++end;
    return !*end && std::isfinite(kmh) && kmh >= 0 && kmh <= 600;
}
}
