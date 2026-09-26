#include <nimby/detail/native_library.hpp>
#include <cassert>
#include <bit>
#include <cstdint>
int main(int argc,char** argv){
    using namespace nimby::detail::native;
    assert(argc==2);
    const auto path=std::filesystem::absolute(argv[1]);
    auto first=loadIsolated(path);
    auto second=load(path);
    const auto value=std::bit_cast<std::uint32_t(*)()>(symbol(second,"Fixture_Value"));
    assert(value&&value()==73);
    assert(!symbol(first,"missing_export"));
    assert(std::filesystem::equivalent(modulePath(reinterpret_cast<const void*>(value)),path));
    assert(std::filesystem::is_regular_file(modulePath(nullptr)));
    unload(first);assert(value()==73);unload(second);
    auto pinned=load(path,true);
    const auto retained=std::bit_cast<std::uint32_t(*)()>(symbol(pinned,"Fixture_Value"));
    unload(pinned);assert(retained()==73);
    bool rejected=false;
    try{load(path.parent_path()/"absent-nrf-test-library");}catch(const std::runtime_error&){rejected=true;}
    assert(rejected);
}
