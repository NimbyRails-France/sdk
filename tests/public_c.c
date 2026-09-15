#include <nimby/observation.h>
_Static_assert(sizeof(NimbyTrain)==304,"Train ABI");
_Static_assert(sizeof(NimbyTrack)==24,"Track ABI");
_Static_assert(sizeof(NimbyStation)==272,"Station ABI");
_Static_assert(sizeof(NimbySignal)==32,"Signal ABI");
_Static_assert(sizeof(NimbySignalState)==120,"Signal state ABI");
_Static_assert(sizeof(NimbySignalTexture)==1960,"Signal texture ABI");
_Static_assert(sizeof(NimbySnapshotInfo)==112,"Snapshot ABI");
int main(void) {
    NimbySdkVersion version={sizeof(NimbySdkVersion),0,0,0,0};
    if(NimbySdk_GetVersion(&version)!=NIMBY_OK || version.abi_version!=1 || version.major!=0 || version.minor!=6)return 1;
    if(NimbySdk_GetVersion(0)!=NIMBY_INVALID_ARGUMENT)return 1;
    version.struct_size=0;if(NimbySdk_GetVersion(&version)!=NIMBY_INVALID_ARGUMENT)return 1;
    NimbySession session=123;
    return NimbySdk_OpenProcess(99,0,&session)!=NIMBY_INVALID_ARGUMENT || session!=0;
}
