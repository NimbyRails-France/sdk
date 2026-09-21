#include <nimby/detail/automatic_driving.h>
#include <engine/binary_identity.h>
#include <windows.h>
#include <array>
#include <filesystem>
#include <mutex>
#include <bit>
#include <cstring>
extern "C" NIMBY_API uint32_t __cdecl NimbyInternal_PublishDrivingRulesV2(const NimbySignalDrivingRule* rules,uint32_t count,uint32_t lease,uint32_t options) noexcept {
 try {
  static std::mutex mutex;std::lock_guard lock(mutex);
  using Publish=uint32_t(__cdecl*)(const NimbySignalDrivingRule*,uint32_t,uint32_t,uint32_t);
  static Publish publish{};
  if(!publish){
   if(!count&&!options)return NIMBY_OK; // Stop before first publication does not install hooks.
   std::array<wchar_t,32768> executable{},sdkPath{};NimbyBinaryInfo game{};
   if(!GetModuleFileNameW(nullptr,executable.data(),static_cast<DWORD>(executable.size()))||
      nimby::engine::identify(executable.data(),game)!=NIMBY_OK||!game.recognized_research_build)return NIMBY_INVALID_BINARY;
   HMODULE sdk{};
   if(!GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS|GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
      reinterpret_cast<LPCWSTR>(&NimbyInternal_PublishDrivingRulesV2),&sdk)||
      !GetModuleFileNameW(sdk,sdkPath.data(),static_cast<DWORD>(sdkPath.size())))return NIMBY_IO_ERROR;
   const auto path=std::filesystem::path(sdkPath.data()).parent_path()/L"NimbyAutomaticDrivingBridge-v1.dll";
   const auto module=LoadLibraryExW(path.c_str(),nullptr,LOAD_LIBRARY_SEARCH_DLL_LOAD_DIR|LOAD_LIBRARY_SEARCH_SYSTEM32);
   if(!module)return NIMBY_IO_ERROR;
   using Bootstrap=DWORD(WINAPI*)(void*);
   const auto bootstrap=std::bit_cast<Bootstrap>(GetProcAddress(module,"NimbyInternal_Bootstrap"));
   const auto candidate=std::bit_cast<Publish>(GetProcAddress(module,"NimbyDriving_PublishV2"));
   const auto status=bootstrap&&candidate?bootstrap(nullptr):NIMBY_INVALID_BINARY;
   if(status!=NIMBY_OK&&status!=NIMBY_ALREADY_INITIALIZED){FreeLibrary(module);return status;}
   publish=candidate;FreeLibrary(module); // Successful bridge is pinned by its bootstrap.
  }
  return publish(rules,count,lease,options);
 }catch(...){return NIMBY_INTERNAL_ERROR;}
}

extern "C" NIMBY_API uint32_t __cdecl NimbyInternal_PublishDrivingRules(const NimbySignalDrivingRule* rules,uint32_t count,uint32_t lease) noexcept {
 return NimbyInternal_PublishDrivingRulesV2(rules,count,lease,0);
}
