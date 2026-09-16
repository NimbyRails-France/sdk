#include "loader/loader.h"
#include "engine/binary_identity.h"
#include <tlhelp32.h>
#include <algorithm>
#include <array>
#include <bit>
#include <cstring>
#include <stdexcept>

namespace nimby::loader {
namespace {
struct Handle {
    HANDLE value = INVALID_HANDLE_VALUE;
    ~Handle() { if (value && value != INVALID_HANDLE_VALUE) CloseHandle(value); }
};
[[noreturn]] void fail(const char* operation) {
    throw std::runtime_error(std::string(operation) + " (Windows error " + std::to_string(GetLastError()) + ")");
}
uint64_t creation_time(HANDLE process) {
    FILETIME creation{}, exit{}, kernel{}, user{};
    if (!GetProcessTimes(process, &creation, &exit, &kernel, &user)) fail("GetProcessTimes");
    return (uint64_t(creation.dwHighDateTime) << 32) | creation.dwLowDateTime;
}
std::wstring process_path(HANDLE process) {
    std::wstring path(32768, L'\0');
    DWORD count = static_cast<DWORD>(path.size());
    if (!QueryFullProcessImageNameW(process, 0, path.data(), &count)) fail("QueryFullProcessImageNameW");
    path.resize(count);
    return canonical_file(path);
}
bool same_path(const std::wstring& a, const std::wstring& b) { return _wcsicmp(a.c_str(), b.c_str()) == 0; }
void require_x64(HANDLE process) {
    USHORT machine{}, native{};
    if (!IsWow64Process2(process, &machine, &native)) fail("IsWow64Process2");
    if (machine != IMAGE_FILE_MACHINE_UNKNOWN || native != IMAGE_FILE_MACHINE_AMD64)
        throw std::runtime_error("Native AMD64 process required");
}
struct Module { uintptr_t base; DWORD size; std::wstring path; };
std::vector<Module> modules(DWORD pid) {
    HANDLE snapshot = INVALID_HANDLE_VALUE;
    for (int retry=0; retry<10; ++retry) {
        snapshot=CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, pid);
        if (snapshot != INVALID_HANDLE_VALUE || GetLastError()!=ERROR_BAD_LENGTH) break;
        Sleep(10);
    }
    Handle owner{snapshot};
    if (snapshot==INVALID_HANDLE_VALUE) fail("Module snapshot");
    MODULEENTRY32W entry{}; entry.dwSize=sizeof entry;
    if (!Module32FirstW(snapshot, &entry)) fail("Module32FirstW");
    std::vector<Module> result;
    do { result.push_back({reinterpret_cast<uintptr_t>(entry.modBaseAddr), entry.modBaseSize, entry.szExePath}); }
    while (Module32NextW(snapshot,&entry));
    if (GetLastError()!=ERROR_NO_MORE_FILES) fail("Module32NextW");
    return result;
}
bool executable_page(DWORD protection) {
    if (protection & (PAGE_GUARD | PAGE_NOACCESS)) return false;
    const auto p=protection & 0xff;
    return p==PAGE_EXECUTE || p==PAGE_EXECUTE_READ || p==PAGE_EXECUTE_READWRITE || p==PAGE_EXECUTE_WRITECOPY;
}
uintptr_t checked_remote_address(HANDLE process, const Module& module, uintptr_t rva, const void* local) {
    constexpr SIZE_T size=16;
    if (rva >= module.size || size > module.size-rva) throw std::runtime_error("Export RVA outside module");
    const auto address=module.base+rva;
    MEMORY_BASIC_INFORMATION page{};
    if (!VirtualQueryEx(process,reinterpret_cast<void*>(address),&page,sizeof page)) fail("VirtualQueryEx");
    if (page.State!=MEM_COMMIT || page.Type!=MEM_IMAGE || !executable_page(page.Protect) ||
        reinterpret_cast<uintptr_t>(page.AllocationBase)!=module.base)
        throw std::runtime_error("Export is not executable image memory");
    if (local) {
        std::array<unsigned char,size> bytes{};
        SIZE_T read{};
        if (!ReadProcessMemory(process,reinterpret_cast<void*>(address),bytes.data(),size,&read) || read!=size)
            fail("ReadProcessMemory export");
        if (std::memcmp(bytes.data(),local,size)!=0) throw std::runtime_error("SDK export bytes differ; refusing remote call");
    }
    return address;
}
uintptr_t remote_load_library(HANDLE process, DWORD pid) {
    const auto function=GetProcAddress(GetModuleHandleW(L"kernel32.dll"),"LoadLibraryW");
    if (!function) fail("GetProcAddress LoadLibraryW");
    HMODULE owner{};
    if (!GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
        std::bit_cast<LPCWSTR>(function),&owner)) fail("Locate LoadLibraryW owner");
    std::wstring path(32768,L'\0');
    const auto length=GetModuleFileNameW(owner,path.data(),static_cast<DWORD>(path.size()));
    if (!length || length>=path.size()) fail("System module path");
    path.resize(length); path=canonical_file(path);
    const auto rva=std::bit_cast<uintptr_t>(function)-reinterpret_cast<uintptr_t>(owner);
    for (const auto& module:modules(pid)) {
        if (same_path(canonical_file(module.path),path)) {
            // Validate the system image identity before transferring the RVA.
            // Its exported entry can legitimately be instrumented in the game.
            // Call that existing entry normally; never unhook it or jump past it.
            const auto* dos=reinterpret_cast<const IMAGE_DOS_HEADER*>(owner);
            const auto* local_nt=reinterpret_cast<const IMAGE_NT_HEADERS64*>(reinterpret_cast<uintptr_t>(owner)+dos->e_lfanew);
            IMAGE_NT_HEADERS64 remote_nt{};
            SIZE_T read{};
            if (!ReadProcessMemory(process,reinterpret_cast<const void*>(module.base+dos->e_lfanew),&remote_nt,sizeof remote_nt,&read) || read!=sizeof remote_nt)
                fail("Read system image headers");
            if (remote_nt.Signature!=IMAGE_NT_SIGNATURE || remote_nt.FileHeader.Machine!=IMAGE_FILE_MACHINE_AMD64 ||
                remote_nt.FileHeader.TimeDateStamp!=local_nt->FileHeader.TimeDateStamp ||
                remote_nt.OptionalHeader.SizeOfImage!=local_nt->OptionalHeader.SizeOfImage || module.size!=local_nt->OptionalHeader.SizeOfImage)
                throw std::runtime_error("System module version mismatch");
            return checked_remote_address(process,module,rva,nullptr);
        }
    }
    throw std::runtime_error("LoadLibraryW owner not found in target");
}
// The only target allocation is a UTF-16 file path, never executable code.
struct RemotePath {
    HANDLE process;
    void* address{};
    bool safe_to_free=true;
    ~RemotePath() { if (address && safe_to_free) VirtualFreeEx(process,address,0,MEM_RELEASE); }
};
DWORD call_remote(HANDLE process, uintptr_t function, void* argument, RemotePath* allocation=nullptr) {
    Handle thread{CreateRemoteThread(process,nullptr,0,std::bit_cast<LPTHREAD_START_ROUTINE>(function),argument,0,nullptr)};
    if (!thread.value) fail("CreateRemoteThread");
    const DWORD wait=WaitForSingleObject(thread.value,15000);
    if (wait!=WAIT_OBJECT_0) {
        // The remote thread might still read the path. Do not free it or terminate
        // the thread; do not retry this instance. Process exit reclaims the memory.
        if (allocation) allocation->safe_to_free=false;
        throw std::runtime_error("Remote call did not finish in 15 seconds; no retry until game restart");
    }
    DWORD status{};
    if (!GetExitCodeThread(thread.value,&status)) fail("GetExitCodeThread");
    return status;
}
}

std::wstring canonical_file(const std::wstring& path) {
    Handle file{CreateFileW(path.c_str(),FILE_READ_ATTRIBUTES,FILE_SHARE_READ|FILE_SHARE_WRITE|FILE_SHARE_DELETE,
        nullptr,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,nullptr)};
    if (file.value==INVALID_HANDLE_VALUE) fail("Open file path");
    std::wstring result(32768,L'\0');
    const auto count=GetFinalPathNameByHandleW(file.value,result.data(),static_cast<DWORD>(result.size()),FILE_NAME_NORMALIZED);
    if (!count || count>=result.size()) fail("Canonical file path");
    result.resize(count);
    return result;
}
std::wstring executable_path() {
    std::wstring path(32768,L'\0');
    const auto count=GetModuleFileNameW(nullptr,path.data(),static_cast<DWORD>(path.size()));
    if (!count || count>=path.size()) fail("Loader executable path");
    path.resize(count); return path;
}
Monitor::Monitor(std::wstring executable,std::string expected_sha256,std::wstring sdk)
    : executable_(canonical_file(executable)), sdk_(canonical_file(sdk)), expected_sha256_(std::move(expected_sha256)) {
    require_x64(GetCurrentProcess());
    // Lock the SDK file against writes/replacement for the whole monitor lifetime.
    sdk_file_=CreateFileW(sdk_.c_str(),GENERIC_READ,FILE_SHARE_READ,nullptr,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,nullptr);
    if (sdk_file_==INVALID_HANDLE_VALUE) fail("Lock SDK file");
    try {
        NimbyBinaryInfo info{};
        if (engine::identify(sdk_.c_str(),info)!=NIMBY_OK) throw std::runtime_error("SDK is not a readable AMD64 PE");
        local_sdk_=LoadLibraryExW(sdk_.c_str(),nullptr,LOAD_LIBRARY_SEARCH_DLL_LOAD_DIR|LOAD_LIBRARY_SEARCH_SYSTEM32);
        if (!local_sdk_) fail("Load SDK locally");
        const auto bootstrap=GetProcAddress(local_sdk_,"NimbyInternal_Bootstrap");
        if (!bootstrap) throw std::runtime_error("SDK bootstrap export is missing");
        HMODULE owner{};
        if (!GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS|GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
            std::bit_cast<LPCWSTR>(bootstrap),&owner) || owner!=local_sdk_)
            throw std::runtime_error("Forwarded bootstrap export refused");
        bootstrap_rva_=std::bit_cast<uintptr_t>(bootstrap)-reinterpret_cast<uintptr_t>(local_sdk_);
    } catch (...) {
        if (local_sdk_) FreeLibrary(local_sdk_);
        CloseHandle(sdk_file_); throw;
    }
}
Monitor::~Monitor() {
    if (local_sdk_) FreeLibrary(local_sdk_); // DllMain only; no local initialization.
    if (sdk_file_!=INVALID_HANDLE_VALUE) CloseHandle(sdk_file_);
}
Event Monitor::attach(DWORD pid,uint64_t creation) {
    try {
        Handle process{OpenProcess(PROCESS_CREATE_THREAD|PROCESS_QUERY_INFORMATION|PROCESS_VM_OPERATION|
            PROCESS_VM_WRITE|PROCESS_VM_READ|SYNCHRONIZE,FALSE,pid)};
        if (!process.value) fail("Open target process (run loader and game at the same privilege level)");
        if (creation_time(process.value)!=creation || WaitForSingleObject(process.value,0)!=WAIT_TIMEOUT)
            throw std::runtime_error("Process instance has exited");
        require_x64(process.value);
        if (!same_path(process_path(process.value),executable_)) throw std::runtime_error("Executable path mismatch");
        // Pin the game file while checking its identity and initiating the load.
        Handle game_file{CreateFileW(executable_.c_str(),GENERIC_READ,FILE_SHARE_READ,nullptr,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,nullptr)};
        if (game_file.value==INVALID_HANDLE_VALUE) fail("Lock game executable");
        NimbyBinaryInfo identity{};
        if (engine::identify(executable_.c_str(),identity)!=NIMBY_OK || expected_sha256_!=identity.sha256)
            throw std::runtime_error("Unrecognized game SHA-256; no DLL loaded");
        bool loaded=false;
        Module sdk_module{};
        for (const auto& module:modules(pid)) {
            const auto slash=module.path.find_last_of(L"\\/");
            const auto sdk_slash=sdk_.find_last_of(L"\\/");
            if (_wcsicmp(module.path.c_str()+slash+1,sdk_.c_str()+sdk_slash+1)==0) {
                if (!same_path(canonical_file(module.path),sdk_)) throw std::runtime_error("Another SDK path is already loaded");
                sdk_module=module; loaded=true; break;
            }
        }
        if (!loaded) {
            const auto load_library=remote_load_library(process.value,pid);
            RemotePath path{process.value};
            const SIZE_T bytes=(sdk_.size()+1)*sizeof(wchar_t);
            path.address=VirtualAllocEx(process.value,nullptr,bytes,MEM_COMMIT|MEM_RESERVE,PAGE_READWRITE);
            if (!path.address) fail("VirtualAllocEx SDK path");
            SIZE_T written{};
            if (!WriteProcessMemory(process.value,path.address,sdk_.c_str(),bytes,&written) || written!=bytes) fail("Write SDK path");
            // LoadLibrary returns an x64 HMODULE. Never reconstruct it from the
            // 32-bit remote thread exit status: enumerate the actual module instead.
            call_remote(process.value,load_library,path.address,&path);
            for (const auto& module:modules(pid)) {
                if (same_path(canonical_file(module.path),sdk_)) { sdk_module=module; loaded=true; break; }
            }
            if (!loaded) throw std::runtime_error("LoadLibrary did not load the SDK; inspect its dependencies");
        }
        const void* bootstrap=reinterpret_cast<const void*>(reinterpret_cast<uintptr_t>(local_sdk_)+bootstrap_rva_);
        const auto remote=checked_remote_address(process.value,sdk_module,bootstrap_rva_,bootstrap);
        const auto status=call_remote(process.value,remote,nullptr);
        if (status!=NIMBY_OK && status!=NIMBY_ALREADY_INITIALIZED)
            throw std::runtime_error("SDK initialization failed, status="+std::to_string(status)+"; DLL left loaded until process exit");
        return {pid,true,status==NIMBY_ALREADY_INITIALIZED ? "SDK already initialized; game hooks disabled" : "SDK loaded and initialized; game hooks disabled"};
    } catch (const std::exception& error) { return {pid,false,error.what()}; }
}
std::vector<Event> Monitor::poll() {
    Handle snapshot{CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,0)};
    if (snapshot.value==INVALID_HANDLE_VALUE) fail("Process snapshot");
    PROCESSENTRY32W entry{}; entry.dwSize=sizeof entry;
    if (!Process32FirstW(snapshot.value,&entry)) fail("Process32FirstW");
    const auto filename=executable_.substr(executable_.find_last_of(L"\\/")+1);
    std::vector<Event> events;
    do {
        if (_wcsicmp(filename.c_str(),entry.szExeFile)!=0 || entry.th32ProcessID==GetCurrentProcessId()) continue;
        Handle process{OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION|SYNCHRONIZE,FALSE,entry.th32ProcessID)};
        if (!process.value) {
            const auto error=GetLastError();
            if (attempted_.insert({entry.th32ProcessID,0}).second)
                events.push_back({entry.th32ProcessID,false,"Cannot inspect matching process name (Windows error "+std::to_string(error)+"); no load attempted"});
            continue;
        }
        try {
            if (!same_path(process_path(process.value),executable_)) continue;
            const auto created=creation_time(process.value);
            if (!attempted_.insert({entry.th32ProcessID,created}).second) continue;
            events.push_back(attach(entry.th32ProcessID,created));
        } catch (const std::exception& error) { events.push_back({entry.th32ProcessID,false,error.what()}); }
    } while (Process32NextW(snapshot.value,&entry));
    return events;
}
Event Monitor::attach_process(DWORD pid) {
    try {
        Handle process{OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION|SYNCHRONIZE,FALSE,pid)};
        if(!process.value)fail("Open selected process");
        return attach(pid,creation_time(process.value));
    }catch(const std::exception& error){return {pid,false,error.what()};}
}
}
