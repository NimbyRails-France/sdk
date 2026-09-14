#pragma once
#include <nimby/sdk.h>
namespace nimby::engine {
uint32_t identify(const wchar_t* path, NimbyBinaryInfo& out) noexcept;
bool is_research_build(const NimbyBinaryInfo& info) noexcept;
}
