#pragma once
#include <nimby/client.hpp>
#include <nimby/mod_commands.hpp>
#include <filesystem>
#include <bit>
#include <nimby/detail/native_library.hpp>

namespace nimby {
// Diagnostic host: load/start/call/stop a mod without exposing Win32 to consumers.
// Loading here does not install/inject a mod in the game. Do not share one host
// concurrently or create multiple hosts for the same DLL.
class ModModule {
#ifdef _WIN32
    using Lifecycle = DWORD (WINAPI*)(void*);
    using Invoke = DWORD (WINAPI*)(const char*, const void*, uint32_t, void*, uint32_t);
#else
    using Lifecycle = uint32_t (*)(void*);
    using Invoke = uint32_t (*)(const char*, const void*, uint32_t, void*, uint32_t);
#endif
    detail::native::Module module_ = nullptr;
    Lifecycle stop_ = nullptr;
    Invoke invoke_ = nullptr;
public:
    explicit ModModule(const std::filesystem::path& path) {
        module_ = detail::native::load(path);
        if (!module_) detail::check(NIMBY_IO_ERROR, "Load mod module");
        try {
            auto start = std::bit_cast<Lifecycle>(detail::native::symbol(module_,"NRFMod_StartV1"));
            stop_ = std::bit_cast<Lifecycle>(detail::native::symbol(module_,"NRFMod_StopV1"));
            invoke_ = std::bit_cast<Invoke>(detail::native::symbol(module_,"NRFMod_InvokeV1"));
            if (!start || !stop_ || !invoke_) detail::check(NIMBY_HOOKS_UNAVAILABLE,"Mod command adapter");
            detail::check(start(nullptr),"Start mod module");
        } catch (...) { detail::native::unload(module_); module_ = nullptr; throw; }
    }
    static ModModule besideExecutable(const std::filesystem::path& filename) {
        if (filename.empty() || filename.has_parent_path()) throw std::invalid_argument("Expected a module filename");
        return ModModule(detail::native::modulePath(nullptr).parent_path()/filename);
    }
    ModModule(const ModModule&) = delete;
    ModModule& operator=(const ModModule&) = delete;
    ~ModModule() { try { close(); } catch (...) { /* Retain a module whose stop failed; unloading is unsafe. */ } }
    void close() {
        if (!module_) return;
        detail::check(stop_(nullptr),"Stop mod module");
        detail::native::unload(module_); module_ = nullptr;
    }
    template<class Response, class Request> Response call(const char* name, const Request& request) const {
        static_assert(std::is_trivially_copyable_v<Request> && std::is_trivially_copyable_v<Response>);
        static_assert(sizeof(Request) <= 1024*1024 && sizeof(Response) <= 1024*1024);
        if (!module_) detail::check(NIMBY_INVALID_HANDLE,"Call stopped mod module");
        Response result{};
        detail::check(invoke_(name,&request,sizeof request,&result,sizeof result),"Call mod command");
        return result;
    }
};
}
