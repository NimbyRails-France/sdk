#pragma once
#include <array>
#include <algorithm>
#include <charconv>
#include <cstdint>
#include <optional>
#include <string>

namespace nimby::detail {
// The UI owns a text draft, independently of the last integer acknowledged by
// the mod. Empty/partial input must survive frames and worker publications.
// Only complete, in-range integers cross the existing numeric event bridge.
struct NumberInputDraft {
    std::array<char,32> text{};
    int length=0;
    bool initialized=false,modified=false;
    int32_t observed=0;

    void synchronize(int32_t value) {
        if(!initialized||(!modified&&value!=observed)) {
            const auto formatted=std::to_string(value);
            text={};std::copy(formatted.begin(),formatted.end(),text.begin());
            length=int(formatted.size());initialized=true;
        }
        observed=value;
        if(parse()==value)modified=false;
    }
    std::optional<int32_t> parse()const {
        if(length<=0||length>=int(text.size()))return {};
        int32_t value{};const auto result=std::from_chars(text.data(),text.data()+length,value);
        if(result.ec!=std::errc{}||result.ptr!=text.data()+length)return {};
        return value;
    }
    std::optional<int32_t> value(int32_t minimum,int32_t maximum)const {
        const auto number=parse();return number&&*number>=minimum&&*number<=maximum?number:std::nullopt;
    }
};
struct NumberInputResult {bool changed=false;std::optional<int32_t> value;};
}
