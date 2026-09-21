#include <cstdint>
namespace { bool active=false; unsigned starts=0,stops=0; }
extern "C" uint32_t NRFMod_StartV1(void* reserved) {
    if(reserved)return 1;
    if(active)return 4;
    active=true;++starts;return 0;
}
extern "C" uint32_t NRFMod_StopV1(void* reserved) {
    if(reserved)return 1;
    if(active){active=false;++stops;}
    return 0;
}
extern "C" uint32_t Fixture_Starts(){return starts;}
extern "C" uint32_t Fixture_Stops(){return stops;}
