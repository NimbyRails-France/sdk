#include <nimby/detail/diagnostics.hpp>
#include <nimby/detail/automatic_driving.h>
#include <engine/binary_identity.h>
#include <platform/windows/mod_host_protocol.h>
#include <platform/windows/bridge_installation.h>
#include <windows.h>
#include <array>
#include <atomic>
#include <filesystem>
#include <mutex>
#include <bit>
#include <cstring>
#ifdef _MSC_VER
#include <intrin.h>
#define NIMBY_DRIVING_CALLER _ReturnAddress()
#else
#define NIMBY_DRIVING_CALLER __builtin_return_address(0)
#endif
namespace {
std::mutex bridgeMutex;
std::atomic<HMODULE> bridge{};
uint64_t legacyPublisher(void* address){
 HMODULE module{};
 return GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS|GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
  reinterpret_cast<LPCWSTR>(address),&module)?reinterpret_cast<uintptr_t>(module):0;
}
uint32_t loadBridge(bool needed) {
 if(bridge.load(std::memory_order_acquire)||!needed)return NIMBY_OK;
 std::unique_lock lock(bridgeMutex,std::try_to_lock);
 if(!lock.owns_lock())return NIMBY_RESOURCE_LIMIT;
 if(bridge.load(std::memory_order_relaxed))return NIMBY_OK;
 nimby::platform::windows::BridgeInstallation installation;
 if(!installation)return installation.status();
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
 if(status==NIMBY_OK||status==NIMBY_ALREADY_INITIALIZED)bridge.store(module,std::memory_order_release);
 FreeLibrary(module); // Successful bootstrap pins the module.
 return bridge.load(std::memory_order_relaxed)?NIMBY_OK:status;
}
}
extern "C" NIMBY_API uint32_t __cdecl NimbyInternal_PublishDrivingRulesV3(const NimbySignalDrivingRule* rules,uint32_t count,uint32_t lease,uint32_t options,uint64_t publisher) noexcept {
 try {
  if(!publisher)return NIMBY_INVALID_ARGUMENT;
  if(NimbyInternal_ModHostTarget()){
   if(count>32768||(!rules&&count)||lease<100||lease>5000||(options&~NIMBY_DRIVING_MAXIMUM_LINE_SPEED))return NIMBY_INVALID_ARGUMENT;
   nimby::mod_host::Request request;request.operation=200;request.args[0]=count;request.args[1]=lease;request.args[2]=options;
   nimby::mod_host::append(request,rules,count);nimby::mod_host::Reply reply;
   return nimby::mod_host::invoke(request,reply,0);
  }
  const auto status=loadBridge(count||options);
  const auto module=bridge.load(std::memory_order_acquire);
  if(status!=NIMBY_OK||!module)return status;
  using Publish=uint32_t(__cdecl*)(const NimbySignalDrivingRule*,uint32_t,uint32_t,uint32_t,uint64_t);
  const auto publish=std::bit_cast<Publish>(GetProcAddress(module,"NimbyDriving_PublishV3"));
  // Never fall back to a resident bridge that has process-global ownership.
  return publish?publish(rules,count,lease,options,publisher):NIMBY_HOOKS_UNAVAILABLE;
 }catch(...){ nimby::detail::diagnostics::exception("sdk", __func__); return NIMBY_INTERNAL_ERROR;}
}
extern "C" NIMBY_API uint32_t __cdecl NimbyInternal_PublishDrivingRulesV2(const NimbySignalDrivingRule* rules,uint32_t count,uint32_t lease,uint32_t options) noexcept {
 return NimbyInternal_PublishDrivingRulesV3(rules,count,lease,options,legacyPublisher(NIMBY_DRIVING_CALLER));
}
extern "C" NIMBY_API uint32_t __cdecl NimbyInternal_PublishDrivingRules(const NimbySignalDrivingRule* rules,uint32_t count,uint32_t lease) noexcept {
 return NimbyInternal_PublishDrivingRulesV3(rules,count,lease,0,legacyPublisher(NIMBY_DRIVING_CALLER));
}
extern "C" NIMBY_API uint32_t __cdecl NimbyInternal_PublishTrainConstraints(const NimbyTrainConstraint* input,uint32_t count,uint32_t lease,uint64_t publisher) noexcept {
 try {
  if(!publisher)return NIMBY_INVALID_ARGUMENT;
  if(NimbyInternal_ModHostTarget()){
   if(count>8192||(!input&&count)||lease<100||lease>5000)return NIMBY_INVALID_ARGUMENT;
   nimby::mod_host::Request request;request.operation=201;request.args[0]=count;request.args[1]=lease;
   nimby::mod_host::append(request,input,count);nimby::mod_host::Reply reply;
   return nimby::mod_host::invoke(request,reply,0);
  }
  const auto status=loadBridge(count!=0);
  const auto module=bridge.load(std::memory_order_acquire);
  if(status!=NIMBY_OK||!module)return status;
  using Publish=uint32_t(__cdecl*)(const NimbyTrainConstraint*,uint32_t,uint32_t,uint64_t);
  const auto publish=std::bit_cast<Publish>(GetProcAddress(module,"NimbyDriving_TrainConstraints"));
  return publish?publish(input,count,lease,publisher):NIMBY_HOOKS_UNAVAILABLE;
 }catch(...){ nimby::detail::diagnostics::exception("sdk", __func__); return NIMBY_INTERNAL_ERROR;}
}
extern "C" NIMBY_API uint32_t __cdecl NimbyInternal_ReadTrainConstraint(uint64_t train,NimbyTrainConstraintStatus* out) noexcept {
 try {
  if(!out||out->size!=sizeof *out||train>>48!=5)return NIMBY_INVALID_ARGUMENT;
  if(NimbyInternal_ModHostTarget()){
   nimby::mod_host::Request request;request.operation=202;request.args[0]=train;nimby::mod_host::Reply reply;
   const auto status=nimby::mod_host::invoke(request,reply,sizeof *out);
   if(status!=NIMBY_OK)return status;
   if(reply.data.size()!=sizeof *out)return NIMBY_INVALID_BINARY;
   std::memcpy(out,reply.data.data(),sizeof *out);return NIMBY_OK;
  }
  const auto module=bridge.load(std::memory_order_acquire);
  if(!module)return NIMBY_DATA_UNAVAILABLE;
  using Read=uint32_t(__cdecl*)(uint64_t,NimbyTrainConstraintStatus*);
  const auto read=std::bit_cast<Read>(GetProcAddress(module,"NimbyDriving_ReadTrainConstraint"));
  return read?read(train,out):NIMBY_HOOKS_UNAVAILABLE;
 }catch(...){ nimby::detail::diagnostics::exception("sdk", __func__); return NIMBY_INTERNAL_ERROR;}
}
