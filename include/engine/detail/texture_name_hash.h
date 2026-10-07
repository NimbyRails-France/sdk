#pragma once
#include <cstdint>
#include <cstring>
#include <string_view>
#if defined(_MSC_VER) && defined(_M_X64)
#include <intrin.h>
#endif

namespace nimby::engine::detail {
// Qualified Windows 1.19 string hash: RVA 0x241780 and tail 0x2416b0.
// The construction metadata bridge already uses this native function. Keep
// byte order explicit; no remote code is executed by observation clients.
inline uint64_t textureNameHash(std::string_view text) noexcept {
    constexpr uint64_t p1=0xe7037ed1a0b428dbULL,p2=0x8ebc6af09c88c6e3ULL;
    constexpr uint64_t p3=0x589965cc75374cc3ULL,p4=0x1d8e4e27c47d124fULL;
    const auto mix=[](uint64_t a,uint64_t b){
#if defined(_MSC_VER) && defined(_M_X64)
        uint64_t high;const auto low=_umul128(a,b,&high);return low^high;
#elif defined(__SIZEOF_INT128__)
        __extension__ using Wide=unsigned __int128;
        const Wide product=Wide(a)*b;return uint64_t(product)^uint64_t(product>>64);
#else
        const uint64_t a0=uint32_t(a),a1=a>>32,b0=uint32_t(b),b1=b>>32;
        const uint64_t low=a0*b0,middle=a1*b0+(low>>32),second=a0*b1+uint32_t(middle);
        const uint64_t high=a1*b1+(middle>>32)+(second>>32);
        return ((second<<32)|uint32_t(low))^high;
#endif
    };
    const auto word=[](const unsigned char* p,size_t count){uint64_t value=0;for(size_t i=0;i<count;++i)value|=uint64_t(p[i])<<(i*8);return value;};
    auto* p=reinterpret_cast<const unsigned char*>(text.data());auto left=text.size();
    uint64_t seed=0x9c3805fc2c85caccULL;
    if(left>64){uint64_t second=seed;
        do{
            seed=mix(word(p,8)^p1,word(p+8,8)^seed)^mix(word(p+16,8)^p2,word(p+24,8)^seed);
            second=mix(word(p+32,8)^p3,word(p+40,8)^second)^mix(word(p+48,8)^p4,word(p+56,8)^second);
            p+=64;left-=64;
        }while(left>64);
        seed^=second;
    }
    while(left>16){seed=mix(word(p,8)^p1,word(p+8,8)^seed);p+=16;left-=16;}
    uint64_t a=0,b=0;
    if(left>8){a=word(p,8);b=word(p+left-8,8);}
    else if(left>=4){a=word(p,4);b=word(p+left-4,4);}
    else if(left){a=uint64_t(p[0])<<16|uint64_t(p[left/2])<<8|p[left-1];}
    return mix(mix(a^p1,b^seed),uint64_t(text.size())^p1);
}
}
