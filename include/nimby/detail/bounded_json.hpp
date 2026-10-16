#pragma once
#include <nimby/detail/vendor/json.hpp>
namespace nimby::detail {
// Internal JSON DOM for guarded parsers (maximum accepted nesting depth 8).
// Container destruction recurses over that bounded depth and never allocates.
using BoundedJson=nlohmann::basic_json<std::map,std::vector,std::string,bool,
    std::int64_t,std::uint64_t,double,std::allocator,nlohmann::adl_serializer,
    std::vector<std::uint8_t>,nlohmann::nrf_bounded_json_base>;
static_assert(sizeof(BoundedJson)==sizeof(nlohmann::json));
static_assert(alignof(BoundedJson)==alignof(nlohmann::json));
}
