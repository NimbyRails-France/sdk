#pragma once
#include <nimby/detail/platform/windows/system.hpp>
#include <tlhelp32.h>
#include <cwchar>
#include <filesystem>
#include <cstdint>
#include <string_view>
#include <stdexcept>
namespace nimby::detail::platform {
// Discovery is advisory. Opening a session still validates binary identity.
inline Discovery discoverProcess() {
    const HANDLE handle = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (handle == INVALID_HANDLE_VALUE) return {NIMBY_IO_ERROR,0};
    struct Close { HANDLE value; ~Close() { CloseHandle(value); } } owner{handle};
    PROCESSENTRY32W entry{};
    entry.dwSize = sizeof entry;
    std::uint32_t found{};
    if (!Process32FirstW(handle, &entry)) return {NIMBY_IO_ERROR,0};
    do {
        if (_wcsicmp(entry.szExeFile, L"NIMBYRails.exe") != 0) continue;
        if (found) return {NIMBY_INVALID_ARGUMENT,0};
        found = entry.th32ProcessID;
    } while (Process32NextW(handle, &entry));
    if (GetLastError() != ERROR_NO_MORE_FILES) return {NIMBY_IO_ERROR,0};
    if (!found) return {NIMBY_IO_ERROR,0};
    return {NIMBY_OK,found};
}
inline bool hostedByGame() {
    wchar_t path[32768]{};
    const auto length = GetModuleFileNameW(nullptr, path, 32768);
    if (!length || length >= 32768) return false;
    const auto name = std::wcsrchr(path, L'\\');
    return _wcsicmp(name ? name+1 : path, L"NIMBYRails.exe") == 0;
}
inline uint32_t currentProcessId() noexcept {
    return GetCurrentProcessId();
}
inline std::filesystem::path stateDirectory() {
        wchar_t root[32768]{};
        const auto n=GetEnvironmentVariableW(L"LOCALAPPDATA",root,32768);
        if(!n||n>=32768)throw std::runtime_error("LOCALAPPDATA unavailable for settings");
    return std::filesystem::path(root);
}
inline constexpr const char* signalUiLibrary="NimbySignalUiBridge-experimental-v1.dll";
// Caller validates payload before creating files. The temporary is a sibling:
// replacement cannot cross volumes. Cleanup covers failures before replacement.
inline void atomicWrite(const std::filesystem::path& path, std::string_view bytes) {
    const auto folder=path.has_parent_path()?path.parent_path():std::filesystem::path(".");
    std::filesystem::create_directories(folder);
        wchar_t temporary[MAX_PATH]{};
        if(!GetTempFileNameW(folder.c_str(),L"nss",0,temporary))throw std::runtime_error("Cannot create settings temporary file");
        HANDLE file=INVALID_HANDLE_VALUE;
        try {
            file=CreateFileW(temporary,GENERIC_WRITE,0,nullptr,TRUNCATE_EXISTING,FILE_ATTRIBUTE_NORMAL,nullptr);
            DWORD written{};
            if(file==INVALID_HANDLE_VALUE||!WriteFile(file,bytes.data(),static_cast<DWORD>(bytes.size()),&written,nullptr)||
               written!=bytes.size()||!FlushFileBuffers(file))throw std::runtime_error("Cannot write settings file");
            CloseHandle(file);file=INVALID_HANDLE_VALUE;
            if(!MoveFileExW(temporary,path.c_str(),MOVEFILE_REPLACE_EXISTING|MOVEFILE_WRITE_THROUGH))
                throw std::runtime_error("Cannot replace settings file");
        } catch(...) {
            if(file!=INVALID_HANDLE_VALUE)CloseHandle(file);
            DeleteFileW(temporary);throw;
        }
    
}
}
