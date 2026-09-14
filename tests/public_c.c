#include <nimby/observation.h>
_Static_assert(sizeof(NimbyTrain)==304,"Train ABI");
_Static_assert(sizeof(NimbyTrack)==24,"Track ABI");
_Static_assert(sizeof(NimbyStation)==272,"Station ABI");
_Static_assert(sizeof(NimbySignal)==32,"Signal ABI");
_Static_assert(sizeof(NimbySnapshotInfo)==112,"Snapshot ABI");
int main(void) {
    NimbySession session=123;
    return NimbySdk_OpenProcess(99,0,&session)!=NIMBY_INVALID_ARGUMENT || session!=0;
}
