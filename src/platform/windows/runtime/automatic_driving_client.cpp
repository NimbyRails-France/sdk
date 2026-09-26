#include <nimby/detail/diagnostics.hpp>
#include <nimby/detail/automatic_driving.h>
#include <engine/binary_identity.h>
#include <windows.h>
#include <array>
#include <filesystem>
#include <mutex>
#include <bit>
#include <cstring>
namespace {
std::mutex bridgeMutex;
HMODULE bridge{};
uint32_t loadBridge(bool needed) {
 if(bridge||!needed)return NIMBY_OK;
 std::array<wchar_t,32768> executable{},sdkPath{};NimbyBinaryInfo game{};
 if(!GetModuleFileNameW(nullptr,executable.data(),static_cast<DWORD>(executable.size()))||
    nimby::engine::identify(executable.data(),game)!=NIMBY_OK||!game.recognized_research_build)return NIMBY_INVALID_BINARY;
 HMODULE sdk{};
 if(!GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS|GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
    reinterpret_cast<LPCWSTR>(&loadBridge),&sdk)||!GetModuleFileNameW(sdk,sdkPath.data(),static_cast<DWORD>(sdkPath.size())))return NIMBY_IO_ERROR;
 const auto path=std::filesystem::path(sdkPath.data()).parent_path()/L"NimbyAutomaticDrivingBridge-v1.dll";
 const auto module=LoadLibraryExW(path.c_str(),nullptr,LOAD_LIBRARY_SEARCH_DLL_LOAD_DIR|LOAD_LIBRARY_SEARCH_SYSTEM32);
 if(!module)return NIMBY_IO_ERROR;
 using Bootstrap=DWORD(WINAPI*)(void*);
 const auto bootstrap=std::bit_cast<Bootstrap>(GetProcAddress(module,"NimbyInternal_Bootstrap"));
 const auto status=bootstrap?bootstrap(nullptr):NIMBY_INVALID_BINARY;
 if(status==NIMBY_OK||status==NIMBY_ALREADY_INITIALIZED)bridge=module;
 FreeLibrary(module); // Successful bootstrap pins the module.
 return bridge?NIMBY_OK:status;
}
}
extern "C" NIMBY_API uint32_t __cdecl NimbyInternal_PublishDrivingRulesV2(const NimbySignalDrivingRule* rules,uint32_t count,uint32_t lease,uint32_t options) noexcept {
 try {
  std::lock_guard lock(bridgeMutex);const auto status=loadBridge(count||options);
  if(status!=NIMBY_OK||!bridge)return status;
  using Publish=uint32_t(__cdecl*)(const NimbySignalDrivingRule*,uint32_t,uint32_t,uint32_t);
  const auto publish=std::bit_cast<Publish>(GetProcAddress(bridge,"NimbyDriving_PublishV2"));
  return publish?publish(rules,count,lease,options):NIMBY_HOOKS_UNAVAILABLE;
 }catch(...){ nimby::detail::diagnostics::exception("sdk", __func__); return NIMBY_INTERNAL_ERROR;}
}
extern "C" NIMBY_API uint32_t __cdecl NimbyInternal_PublishDrivingRules(const NimbySignalDrivingRule* rules,uint32_t count,uint32_t lease) noexcept {
 return NimbyInternal_PublishDrivingRulesV2(rules,count,lease,0);
}
extern "C" NIMBY_API uint32_t __cdecl NimbyInternal_PublishTrainConstraints(const NimbyTrainConstraint* input,uint32_t count,uint32_t lease,uint64_t publisher) noexcept {
 try {
  std::lock_guard lock(bridgeMutex);const auto status=loadBridge(count!=0);
  if(status!=NIMBY_OK||!bridge)return status;
  using Publish=uint32_t(__cdecl*)(const NimbyTrainConstraint*,uint32_t,uint32_t,uint64_t);
  const auto publish=std::bit_cast<Publish>(GetProcAddress(bridge,"NimbyDriving_TrainConstraints"));
  return publish?publish(input,count,lease,publisher):NIMBY_HOOKS_UNAVAILABLE;
 }catch(...){ nimby::detail::diagnostics::exception("sdk", __func__); return NIMBY_INTERNAL_ERROR;}
}
extern "C" NIMBY_API uint32_t __cdecl NimbyInternal_ReadTrainConstraint(uint64_t train,NimbyTrainConstraintStatus* out) noexcept {
 try {
  if(!out||out->size!=sizeof *out||train>>48!=5)return NIMBY_INVALID_ARGUMENT;
  std::lock_guard lock(bridgeMutex);
  if(!bridge)return NIMBY_DATA_UNAVAILABLE;
  using Read=uint32_t(__cdecl*)(uint64_t,NimbyTrainConstraintStatus*);
  const auto read=std::bit_cast<Read>(GetProcAddress(bridge,"NimbyDriving_ReadTrainConstraint"));
  return read?read(train,out):NIMBY_HOOKS_UNAVAILABLE;
 }catch(...){ nimby::detail::diagnostics::exception("sdk", __func__); return NIMBY_INTERNAL_ERROR;}
}
