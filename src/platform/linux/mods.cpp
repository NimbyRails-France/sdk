#include <loader/mods.h>
#include <platform/mod_library.h>
#include <nimby/detail/native_library.hpp>
#include <bit>
namespace nimby::platform {
std::string_view mod_library_suffix() noexcept{return ".so";}
bool mod_library_case_insensitive() noexcept{return false;}
}
namespace nimby::loader {
void Mods::load(const std::filesystem::path& path,Log log) {
    namespace native=nimby::detail::native;
    native::Module handle{};
    try {
        handle=native::load(path);
        auto start=std::bit_cast<Entry>(native::symbol(handle,"NRFMod_StartV1"));
        auto stop=std::bit_cast<Entry>(native::symbol(handle,"NRFMod_StopV1"));
        if(!start||!stop){native::unload(handle);log("ERROR: NRF mod V1 exports missing");return;}
        modules_.push_back({handle,start,stop,false});
    }catch(...){native::unload(handle);log("ERROR: NRF mod library or dependency could not be loaded");}
}
}
