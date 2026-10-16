#pragma once
#include <cstdlib>
#include <limits>
#include <new>

// Real, thread-local operator-new failures. Other workers and diagnostic/storage
// threads keep their normal allocator; arming/capturing never allocates itself.
namespace allocation_fault_fixture {
struct Stats {std::size_t allocations{},failures{};};
inline thread_local bool watching=false;
inline thread_local std::size_t countdown=std::numeric_limits<std::size_t>::max();
inline thread_local unsigned remaining=0;
inline thread_local Stats stats;
inline void arm(std::size_t successful,unsigned failures)noexcept{
    countdown=successful;remaining=failures;stats={};watching=true;
}
inline Stats stop()noexcept{watching=false;remaining=0;return stats;}
inline bool refuse()noexcept{
    if(!watching)return false;
    ++stats.allocations;
    if(!remaining)return false;
    if(countdown){--countdown;return false;}
    --remaining;++stats.failures;return true;
}
}
#ifdef __GNUC__
#define FIXTURE_ALLOCATION_NOINLINE __attribute__((noinline))
#else
#define FIXTURE_ALLOCATION_NOINLINE __declspec(noinline)
#endif
FIXTURE_ALLOCATION_NOINLINE void* operator new(std::size_t bytes){
    if(allocation_fault_fixture::refuse())throw std::bad_alloc();
    if(auto* value=std::malloc(bytes?bytes:1))return value;
    throw std::bad_alloc();
}
FIXTURE_ALLOCATION_NOINLINE void* operator new[](std::size_t bytes){return ::operator new(bytes);}
FIXTURE_ALLOCATION_NOINLINE void operator delete(void* value)noexcept{std::free(value);}
FIXTURE_ALLOCATION_NOINLINE void operator delete[](void* value)noexcept{std::free(value);}
FIXTURE_ALLOCATION_NOINLINE void operator delete(void* value,std::size_t)noexcept{std::free(value);}
FIXTURE_ALLOCATION_NOINLINE void operator delete[](void* value,std::size_t)noexcept{std::free(value);}
#undef FIXTURE_ALLOCATION_NOINLINE
