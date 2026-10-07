#pragma once
#include <windows.h>
#include <cstdio>
#include <cstring>

namespace nimby::detail::diagnostics {
// Kernel32-only sink: usable even by the SDL proxy built without exceptions.
// Called only from normal runtime entry points, never DllMain. A named mutex
// serializes writes/rotation across processes sharing a component/module log.
// Contended diagnostics are dropped, never waited on by an observation worker.
inline void write(const char* component,const char* level,const char* message) noexcept {
    const auto saved=GetLastError();
    static SRWLOCK local=SRWLOCK_INIT;
    static char previous[2048]{};
    static ULONGLONG when=0,repeated=0;
    static ULONGLONG burstAt=0,omitted=0;
    static unsigned burst=0;
    static volatile LONG64 concurrent=0;
    if(!TryAcquireSRWLockExclusive(&local)){InterlockedIncrement64(&concurrent);SetLastError(saved);return;}
    const auto now=GetTickCount64();
    if(!message)message="";
    const auto length=strnlen(message,65537);
    if(length<sizeof previous&&std::strcmp(message,previous)==0&&now-when<30000) {
        ++repeated;ReleaseSRWLockExclusive(&local);SetLastError(saved);return;
    }
    // Also bound changing exception text: alternating/unique messages must not
    // defeat duplicate suppression and force constant disk writes/rotation.
    if(now-burstAt>=1000){burstAt=now;burst=0;}
    if(burst>=16){++omitted;ReleaseSRWLockExclusive(&local);SetLastError(saved);return;}
    ++burst;
    wchar_t root[4096]{},path[4096]{},module[4096]{},part[128]{};
    HANDLE file=INVALID_HANDLE_VALUE,mutex=nullptr;bool owned=false;
    do {
        auto size=GetEnvironmentVariableW(L"NRF_LOG_DIR",root,4096);
        if(size>=3500)break;
        if(!size) {
            size=GetEnvironmentVariableW(L"LOCALAPPDATA",root,4096);
            if(!size||size>=3500)break;
            lstrcatW(root,L"\\NimbyRailsFrance");CreateDirectoryW(root,nullptr);
            lstrcatW(root,L"\\logs");
        }
        CreateDirectoryW(root,nullptr);
        // Only fixed internal component names are accepted; no path fragments.
        if(!MultiByteToWideChar(CP_UTF8,MB_ERR_INVALID_CHARS,component,-1,part,128))break;
        for(auto* c=part;*c;++c)if(!((*c>=L'a'&&*c<=L'z')||(*c>=L'A'&&*c<=L'Z')||(*c>=L'0'&&*c<=L'9')||*c==L'-'||*c==L'_'))*c=L'_';
        lstrcatW(root,L"\\");lstrcatW(root,part);CreateDirectoryW(root,nullptr);
        HMODULE self{};
        if(!GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS|GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
            reinterpret_cast<LPCWSTR>(&write),&self)||!GetModuleFileNameW(self,module,4096))break;
        wchar_t* base=module;
        for(auto* c=module;*c;++c)if(*c==L'\\'||*c==L'/')base=c+1;
        wchar_t name[128]{};lstrcpynW(name,base,110);
        for(auto* c=name;*c;++c)if(!((*c>=L'a'&&*c<=L'z')||(*c>=L'A'&&*c<=L'Z')||(*c>=L'0'&&*c<=L'9')||*c==L'-'||*c==L'_'))*c=L'_';
        wchar_t key[300]=L"Local\\NRF.Diagnostics.";lstrcatW(key,part);lstrcatW(key,L".");lstrcatW(key,name);
        mutex=CreateMutexW(nullptr,FALSE,key);if(!mutex)break;
        const auto wait=WaitForSingleObject(mutex,0);
        if(wait!=WAIT_OBJECT_0&&wait!=WAIT_ABANDONED)break;
        owned=true;
        lstrcpyW(path,root);lstrcatW(path,L"\\");lstrcatW(path,name);lstrcatW(path,L".log");
        WIN32_FILE_ATTRIBUTE_DATA info{};
        if(GetFileAttributesExW(path,GetFileExInfoStandard,&info)&&
           (info.nFileSizeHigh||info.nFileSizeLow>=1024*1024)) {
            wchar_t older[4096]{},newer[4096]{};
            lstrcpyW(newer,path);lstrcatW(newer,L".3");DeleteFileW(newer);
            for(int i=2;i>=0;--i) {
                lstrcpyW(older,path);lstrcpyW(newer,path);
                if(i==2){lstrcatW(older,L".2");lstrcatW(newer,L".3");}
                else if(i==1){lstrcatW(older,L".1");lstrcatW(newer,L".2");}
                else lstrcatW(newer,L".1");
                MoveFileExW(older,newer,MOVEFILE_REPLACE_EXISTING);
            }
        }
        file=CreateFileW(path,FILE_APPEND_DATA,FILE_SHARE_READ|FILE_SHARE_WRITE|FILE_SHARE_DELETE,nullptr,OPEN_ALWAYS,FILE_ATTRIBUTE_NORMAL,nullptr);
        if(file==INVALID_HANDLE_VALUE)break;
        SYSTEMTIME utc{};GetSystemTime(&utc);char stamp[256]{};
        std::snprintf(stamp,sizeof stamp,"%04u-%02u-%02uT%02u:%02u:%02u.%03uZ [pid=%lu tid=%lu] [%s] %s ",
            utc.wYear,utc.wMonth,utc.wDay,utc.wHour,utc.wMinute,utc.wSecond,utc.wMilliseconds,GetCurrentProcessId(),GetCurrentThreadId(),component,level);
        DWORD written{};
        const auto missed=InterlockedExchange64(&concurrent,0);
        if(omitted) {
            char summary[96]{};std::snprintf(summary,sizeof summary,"Diagnostic flood omitted: %llu events\r\n",omitted);
            WriteFile(file,stamp,DWORD(std::strlen(stamp)),&written,nullptr);WriteFile(file,summary,DWORD(std::strlen(summary)),&written,nullptr);
            omitted=0;
        }
        if(missed) {
            char summary[96]{};std::snprintf(summary,sizeof summary,"Concurrent diagnostics omitted: %lld\r\n",missed);
            WriteFile(file,stamp,DWORD(std::strlen(stamp)),&written,nullptr);WriteFile(file,summary,DWORD(std::strlen(summary)),&written,nullptr);
        }
        if(repeated) {
            char summary[96]{};std::snprintf(summary,sizeof summary,"Previous event repeated %llu times\r\n",repeated);
            WriteFile(file,stamp,DWORD(std::strlen(stamp)),&written,nullptr);WriteFile(file,summary,DWORD(std::strlen(summary)),&written,nullptr);
        }
        WriteFile(file,stamp,DWORD(std::strlen(stamp)),&written,nullptr);
        // Bound one faulty message; normal exception stacks fit within 64 KiB.
        auto count=length<65536?length:size_t(65536);
        if(count<length)while(count&&(static_cast<unsigned char>(message[count])&0xc0)==0x80)--count;
        const bool success=WriteFile(file,message,DWORD(count),&written,nullptr)!=FALSE&&written==count;
        if(count<length)WriteFile(file," [truncated]",12,&written,nullptr);
        // Close publishes the append to other readers. Durability flushing on
        // every exception stalls the worker on physical storage unnecessarily.
        WriteFile(file,"\r\n",2,&written,nullptr);
        if(success){previous[0]=0;if(length<sizeof previous)std::memcpy(previous,message,length+1);when=now;repeated=0;}
    } while(false);
    if(file!=INVALID_HANDLE_VALUE)CloseHandle(file);
    else OutputDebugStringA("NRF diagnostic log unavailable\n");
    if(owned)ReleaseMutex(mutex);
    if(mutex)CloseHandle(mutex);
    ReleaseSRWLockExclusive(&local);SetLastError(saved);
}
}
#if defined(__cpp_exceptions)
#include <exception>
#include <string>
namespace nimby::detail::diagnostics {
// Call only from a catch block. Kotlin stacks arrive through std::exception.
inline void exception(const char* component,const char* operation) noexcept {
    try { throw; }
    catch(const std::exception& error) {
        try { write(component,"ERROR",(std::string(operation)+": "+error.what()).c_str()); }
        catch(...){write(component,"ERROR",operation);}
    }catch(...){write(component,"ERROR",operation);}
}
}
#endif
