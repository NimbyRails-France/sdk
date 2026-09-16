#pragma once
#include <nimby/detail/sdk.h>
namespace nimby::engine {
uint32_t identify(const wchar_t* path, NimbyBinaryInfo& out) noexcept;
bool is_research_build(const NimbyBinaryInfo& info) noexcept;
}
