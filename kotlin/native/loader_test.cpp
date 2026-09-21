#include <nimby/kotlin_mod.hpp>
#include <nimby/detail/native_library.hpp>
#include <filesystem>
#include <iostream>
#include <stdexcept>
#ifdef _WIN32
#define NRF_CALL WINAPI
#else
#define NRF_CALL
#endif
namespace native=nimby::detail::native;
#define CHECK(x) do{if(!(x))throw std::runtime_error("Check failed: " #x);}while(false)
// Output buffers need a local placeholder, not a second in-process mod adapter.
nimby::kotlin::Decision nimby::kotlin::Rules::unknownDecision(){return {};}
int main(int argc,char** argv) {
 try {
    CHECK(argc==2 || (argc==3 && std::string(argv[2])=="--smoke"));
    const auto dll=std::filesystem::absolute(argv[1]);
    auto sdk=native::load(dll.parent_path()/("NimbyRailsFranceSDK"+dll.extension().string()));
    auto handle=native::load(dll);
    using Fn=uint32_t(NRF_CALL*)(void*);
    auto start=reinterpret_cast<Fn>(native::symbol(handle,"NRFMod_StartV1"));
    auto stop=reinterpret_cast<Fn>(native::symbol(handle,"NRFMod_StopV1"));
    auto state=reinterpret_cast<Fn>(native::symbol(handle,"NRFMod_IsInitializedV1"));
    CHECK(start&&stop&&state);CHECK(state(nullptr)==0);CHECK(start(reinterpret_cast<void*>(1))==1);
    CHECK(start(nullptr)==0);CHECK(state(nullptr)==1);CHECK(start(nullptr)==4);
    using Invoke=uint32_t(NRF_CALL*)(const char*,const void*,uint32_t,void*,uint32_t);
    auto invoke=reinterpret_cast<Invoke>(native::symbol(handle,"NRFMod_InvokeV1"));CHECK(invoke);
    // Consumer verification exercises the loader contract, without assumptions
    // about national aspects, texture choices or driving plans of a mod.
    if(argc==3) {
        CHECK(stop(nullptr)==0);CHECK(state(nullptr)==0);
        CHECK(start(nullptr)==0);CHECK(state(nullptr)==1);CHECK(stop(nullptr)==0);
        native::unload(handle);
        handle=native::load(dll);CHECK(handle);
        start=reinterpret_cast<Fn>(native::symbol(handle,"NRFMod_StartV1"));
        stop=reinterpret_cast<Fn>(native::symbol(handle,"NRFMod_StopV1"));
        CHECK(start&&stop);CHECK(start(nullptr)==0);CHECK(stop(nullptr)==0);
        native::unload(handle);native::unload(sdk);
        std::cout<<"PASS: mod exports, start, stop, restart and reload\n";
        return 0;
    }
    using Runtime=nimby::kotlin::Runtime;
    Runtime::SignalRequest request;Runtime::SignalResult response;
    CHECK(invoke("nrf.kotlin.evaluate.v1",&request,sizeof request,&response,sizeof response)==0);
    CHECK(!response.texturePath.view().empty());
    auto core=native::load(dll.parent_path()/(dll.stem().string()+"Kotlin"+dll.extension().string()));CHECK(core);
    using Direct=int(*)(int,int64_t,int,int64_t,int64_t,const int*,int,int,int*);
    auto direct=reinterpret_cast<Direct>(native::symbol(core,"NRFKotlin_Decide"));CHECK(direct);
    // Compare the loader transport with direct Kotlin calls, without national rules in this test.
    for(unsigned mask=0;mask<1024;++mask)for(unsigned bits=0;bits<32;++bits) {
        request.settings.mask=mask;
        request.observation={nimby::BlockOccupancy((mask/8)%3),bool(bits&1),bool(bits&2),bool(bits&4),bool(bits&8),bool(bits&16),int(mask%8)};
        const auto& o=request.observation;
        const int input[]{int(o.block),o.fresh,o.routeKnown,o.forcedStop,o.lampFailed,o.redFlashCondition,o.next};int expected[2]{};
        const auto expectedStatus=direct(0,mask,2,0,0,input,-1,0,expected);
        const auto actualStatus=invoke("nrf.kotlin.evaluate.v1",&request,sizeof request,&response,sizeof response);
        CHECK((expectedStatus==0)==(actualStatus==0));
        if(expectedStatus==0)CHECK(response.decision.aspect==expected[0]&&response.decision.reason==expected[1]);
    }
    request.observation.block=static_cast<nimby::BlockOccupancy>(99);
    CHECK(invoke("nrf.kotlin.evaluate.v1",&request,sizeof request,&response,sizeof response)!=0);
    CHECK(response.texturePath.view().empty()); // Kotlin exception becomes an SDK error, not a process abort.
    request.observation.block=nimby::BlockOccupancy::Unknown;request.simulationMs=-1;
    CHECK(invoke("nrf.kotlin.evaluate.v1",&request,sizeof request,&response,sizeof response)!=0);
    request.simulationMs=0;
    CHECK(invoke("nrf.kotlin.evaluate.v1",&request,sizeof request-1,&response,sizeof response)!=0);
    auto network=std::make_unique<Runtime::NetworkRequest>();auto result=std::make_unique<Runtime::NetworkResult>();
    nimby::kotlin::Signal signal;signal.id=UINT64_C(0xf000000000000001);network->signals.push_back(signal);
    CHECK(invoke("nrf.kotlin.network.v1",network.get(),sizeof *network,result.get(),sizeof *result)==0);
    CHECK(result->signals.count==1&&result->signals.items[0].signal==signal.id);
    network->signals.count=513;
    CHECK(invoke("nrf.kotlin.network.v1",network.get(),sizeof *network,result.get(),sizeof *result)!=0);
    auto plan=std::make_unique<Runtime::DrivingRequest>();Runtime::DrivingResult planned;
    CHECK(invoke("nrf.kotlin.plan.v1",plan.get(),sizeof *plan,&planned,sizeof planned)==0);
    CHECK(!planned.commandApplied); // Diagnostic calculations never drive a train.
    plan->vehicle={160.0/3.6,1,0.65,160000,1900000,163200,0,72.36};
    plan->fresh=true;plan->routeKnown=true;plan->lineSpeedMps=200.0/3.6;plan->headM=1000;plan->speedMps=30;
    plan->constraints.push_back({UINT64_C(0xf000000000000001),1000,2000,0,false});
    CHECK(invoke("nrf.kotlin.plan.v1",plan.get(),sizeof *plan,&planned,sizeof planned)==0);
    if(planned.plan.available)CHECK(planned.plan.limitingSource==UINT64_C(0xf000000000000001)&&planned.plan.speedCeilingMps==0);
    CHECK(stop(nullptr)==0);CHECK(state(nullptr)==0);CHECK(start(nullptr)==0);CHECK(stop(nullptr)==0);
    native::unload(handle);
    handle=native::load(dll);CHECK(handle);
    start=reinterpret_cast<Fn>(native::symbol(handle,"NRFMod_StartV1"));stop=reinterpret_cast<Fn>(native::symbol(handle,"NRFMod_StopV1"));
    CHECK(start(nullptr)==0);CHECK(stop(nullptr)==0);native::unload(handle);
    native::unload(core);native::unload(sdk);
    std::cout<<"PASS: Kotlin mod load, commands, invalid input/exception isolation, 64-bit IDs, stop, restart and reload\n";
    return 0;
 }catch(const std::exception& e){std::cerr<<e.what()<<'\n';return 1;}
}
