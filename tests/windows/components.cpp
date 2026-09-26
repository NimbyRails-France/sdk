#include "engine/binary_identity.h"
#include "hooks/policy.h"
#include <windows.h>
#include <cstdio>
#include <cstring>
#include <array>

#define CHECK(x) do { if (!(x)) { std::fprintf(stderr,"FAIL line %d: %s\n",__LINE__,#x); return 1; } } while(false)
int main() {
    NimbyBinaryInfo b{};
    // Synthetic identity fixture tests policy only, never represents a runtime observation.
    b.file_size=12583424;
    std::strcpy(b.sha256,"FFF49AC21720ABFC824C2B4F68B862727630EB0DB71CFE1F9EA8F685D0DB10AE");
    CHECK(nimby::engine::is_research_build(b));
    CHECK(nimby::hooks::request_activation()==NIMBY_HOOKS_UNAVAILABLE);
    b.sha256[0]='0'; CHECK(!nimby::engine::is_research_build(b));
    b.sha256[0]='F'; --b.file_size; CHECK(!nimby::engine::is_research_build(b));
    wchar_t temp[MAX_PATH]{}, path[MAX_PATH]{};
    CHECK(GetTempPathW(MAX_PATH,temp)>0);
    CHECK(GetTempFileNameW(temp,L"nrs",0,path));
    struct Cleanup { const wchar_t* path; ~Cleanup(){DeleteFileW(path);} } cleanup{path};
    CHECK(nimby::engine::identify(path,b)==NIMBY_INVALID_BINARY);
    // A valid minimal PE header fixture tests hashing against an independent digest below.
    std::array<unsigned char,512> bytes{};
    IMAGE_DOS_HEADER dos{}; dos.e_magic=IMAGE_DOS_SIGNATURE; dos.e_lfanew=128;
    IMAGE_NT_HEADERS64 nt{}; nt.Signature=IMAGE_NT_SIGNATURE;
    nt.FileHeader.Machine=IMAGE_FILE_MACHINE_AMD64;
    nt.FileHeader.SizeOfOptionalHeader=sizeof(IMAGE_OPTIONAL_HEADER64);
    nt.OptionalHeader.Magic=IMAGE_NT_OPTIONAL_HDR64_MAGIC;
    std::memcpy(bytes.data(),&dos,sizeof dos); std::memcpy(bytes.data()+128,&nt,sizeof nt);
    auto write = [&] {
        HANDLE f=CreateFileW(path,GENERIC_WRITE,0,nullptr,TRUNCATE_EXISTING,FILE_ATTRIBUTE_NORMAL,nullptr);
        if(f==INVALID_HANDLE_VALUE) return false;
        DWORD count{}; bool ok=WriteFile(f,bytes.data(),static_cast<DWORD>(bytes.size()),&count,nullptr) && count==bytes.size();
        CloseHandle(f); return ok;
    };
    CHECK(write()); CHECK(nimby::engine::identify(path,b)==NIMBY_OK);
    CHECK(b.file_size==512 && !b.recognized_research_build);
    // Independent Node crypto SHA-256 over the same 512-byte fixture.
    CHECK(std::strcmp(b.sha256,"9909385D896BABB8E9FE49B4F7812069D6B7466FE0782960502820F4A026C7FF")==0);
    std::printf("Synthetic PE SHA256=%s\n",b.sha256);
    bytes[0]=0; CHECK(write()); CHECK(nimby::engine::identify(path,b)==NIMBY_INVALID_BINARY);
    CHECK(b.sha256[0]==0 && !b.recognized_research_build);
    bytes[0]='M'; bytes[132]=0x4c; bytes[133]=1; CHECK(write()); CHECK(nimby::engine::identify(path,b)==NIMBY_INVALID_BINARY);
    bytes[132]=0x64; bytes[133]=0x86;
    bytes[0]='M'; bytes[0x3f]=0x7f; CHECK(write()); CHECK(nimby::engine::identify(path,b)==NIMBY_INVALID_BINARY);
    std::puts("PASS identity fixtures, truncated/malformed PE, version refusal, hook refusal");
}
