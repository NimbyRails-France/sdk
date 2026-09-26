#pragma once
#include <string_view>
namespace nimby::platform {
// Native naming policy only; discovery, manifest parsing and ordering are common.
std::string_view mod_library_suffix() noexcept;
bool mod_library_case_insensitive() noexcept;
}
