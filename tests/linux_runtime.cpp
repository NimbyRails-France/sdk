#include <nimby/detail/sdk.h>
#include <cassert>
#include <cstring>
#include <iostream>
int main() {
    NimbySdkVersion version{};version.struct_size=sizeof version;
    assert(NimbyInternal_GetVersion(&version)==NIMBY_OK&&version.abi_version==NIMBY_ABI_VERSION);
    NimbyBinaryInfo host{};host.struct_size=sizeof host;
    assert(NimbyInternal_GetHostInfo(&host)==NIMBY_INVALID_ARGUMENT);
    assert(NimbyInternal_Initialize(999,0)==NIMBY_INVALID_ARGUMENT);
    assert(NimbyInternal_Initialize(NIMBY_ABI_VERSION,NIMBY_REQUEST_HOOKS)==NIMBY_HOOKS_UNAVAILABLE);
    assert(NimbyInternal_Initialize(NIMBY_ABI_VERSION,0)==NIMBY_OK);
    assert(NimbyInternal_Initialize(NIMBY_ABI_VERSION,0)==NIMBY_ALREADY_INITIALIZED);
    assert(NimbyInternal_GetHostInfo(&host)==NIMBY_OK&&host.file_size>0&&std::strlen(host.sha256)==64&&!host.recognized_research_build);
    NimbyBinaryInfo inspected{};inspected.struct_size=sizeof inspected;
    assert(NimbyInternal_InspectBinary(L"/proc/self/exe",&inspected)==NIMBY_OK&&std::strcmp(inspected.sha256,host.sha256)==0);
    assert(NimbyInternal_InspectBinary(nullptr,&inspected)==NIMBY_INVALID_ARGUMENT);
    assert(NimbyInternal_Shutdown()==NIMBY_OK);
    assert(NimbyInternal_Shutdown()==NIMBY_OK);
    assert(NimbyInternal_GetHostInfo(&host)==NIMBY_INVALID_ARGUMENT);
    std::cout<<"Linux SDK ABI, host identity, initialization and shutdown passed\n";
}
