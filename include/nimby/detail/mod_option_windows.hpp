#pragma once
#include <array>
#include <cstdint>
#include <map>
#include <stdexcept>
#include <string>
#include <string_view>

namespace nimby::detail {
// Window IDs already shipped with ABI 7 may occupy all 128 bytes. Preserve
// readable persisted keys where possible; longer IDs get a stable FNV-1a-128
// suffix. Never infer a window from a prefix or from its declaration order.
class ModOptionWindows {
public:
    std::string add(std::string_view window) {
        if(window.empty()||window.size()>128||window.find('\0')!=std::string_view::npos)
            throw std::invalid_argument("Invalid tool window option identity");
        for(const char c:window)if(!((c>='A'&&c<='Z')||(c>='a'&&c<='z')||(c>='0'&&c<='9')||c=='_'||c=='.'||c=='-'))
            throw std::invalid_argument("Invalid tool window option identity");
        auto key=window.size()<=121?"window."+std::string(window):"window.h"+fingerprint(window);
        if(!windows_.emplace(key,window).second)
            throw std::invalid_argument("Duplicate or colliding tool window option identity");
        return key;
    }
    std::string_view resolve(std::string_view option)const noexcept {
        const auto found=windows_.find(option);
        return found==windows_.end()?std::string_view{}:std::string_view(found->second);
    }
private:
    static std::string fingerprint(std::string_view text) {
        // Four little-endian 32-bit limbs keep the standard 128-bit algorithm
        // portable on MSVC and MinGW; no compiler-specific wide integer type.
        std::array<uint32_t,4> hash{0x6295c58d,0x62b82175,0x07bb0142,0x6c62272e};
        constexpr std::array<uint32_t,4> prime{0x13b,0,0x01000000,0};
        for(const unsigned char byte:text){
            hash[0]^=byte;std::array<uint32_t,4> product{};
            for(size_t i=0;i<hash.size();++i){
                uint64_t carry=0;
                for(size_t j=0;i+j<hash.size();++j){
                    const uint64_t value=uint64_t(hash[i])*prime[j]+product[i+j]+carry;
                    product[i+j]=uint32_t(value);carry=value>>32;
                }
            }
            hash=product;
        }
        constexpr char hex[]="0123456789abcdef";
        std::string result;result.reserve(32);
        for(size_t i=hash.size();i-->0;)for(int shift=28;shift>=0;shift-=4)result+=hex[(hash[i]>>shift)&15];
        return result;
    }
    std::map<std::string,std::string,std::less<>> windows_;
};
}
