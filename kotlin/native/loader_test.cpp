#include <nimby/kotlin_mod.hpp>
#include <nimby/detail/native_library.hpp>
#include <algorithm>
#include <filesystem>
#include <iostream>
#include <stdexcept>
#include <nimby/detail/platform/mod_abi.hpp>
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
    // An intentionally invalid Kotlin call must retain its operation and stack
    // without aborting the native host. It is diagnostic-only, no game needed.
    {
        auto core=native::load(dll.parent_path()/(dll.stem().string()+"Kotlin"+dll.extension().string()));CHECK(core);
        auto info=reinterpret_cast<int(*)(int*)>(native::symbol(core,"NRFKotlin_Info"));CHECK(info);
        auto error=reinterpret_cast<int(*)(char*,int)>(native::symbol(core,"NRFKotlin_LastError"));CHECK(error);
        CHECK(info(nullptr)<0);
        std::array<char,32768> text{};CHECK(error(text.data(),int(text.size()))>0);
        CHECK(std::string(text.data()).find("Info:")!=std::string::npos);
        CHECK(std::string(text.data()).find("IllegalArgumentException")!=std::string::npos);
        const auto version=reinterpret_cast<int(*)()>(native::symbol(core,"NRFKotlin_Version"));CHECK(version);
        const auto abi=version();CHECK(abi>=1&&abi<=14);
        if(abi>=13){
            const auto lengthVersion=reinterpret_cast<int(*)()>(native::symbol(core,"NRFKotlin_TrainLengthDeclarationVersion"));
            CHECK(lengthVersion);const auto length=lengthVersion();CHECK(length==0||length==11||length==12);
            const auto visibility=reinterpret_cast<int(*)(int,int,char*,int)>(native::symbol(core,"NRFKotlin_CheckboxVisibleWhen"));
            const auto countTypes=reinterpret_cast<int(*)()>(native::symbol(core,"NRFKotlin_TypeCount"));
            const auto typeInfo=reinterpret_cast<int(*)(int,int64_t*)>(native::symbol(core,"NRFKotlin_TypeInfo"));
            const auto typeMetadata=reinterpret_cast<int(*)(int,int,int,char*,int)>(native::symbol(core,"NRFKotlin_TypeMetadata"));
            CHECK(visibility&&countTypes&&typeInfo&&typeMetadata);const auto count=countTypes();CHECK(count>=1&&count<=16);
            bool dependencySeen=false;
            for(int type=0;type<count;++type){
                std::array<int64_t,4> description{};CHECK(typeInfo(type,description.data())==0);
                CHECK(description[0]>=0&&description[0]<=64);std::vector<std::string> names;
                for(int box=0;box<description[0];++box){
                    std::array<char,129> name{};const auto bytes=typeMetadata(type,4,box,name.data(),int(name.size()));
                    CHECK(bytes>0&&bytes<int(name.size())&&name[bytes]==0);names.emplace_back(name.data(),bytes);
                }
                for(int box=0;box<description[0];++box){
                    std::array<char,129> parent{};const auto bytes=visibility(type,box,parent.data(),int(parent.size()));
                    CHECK(bytes>=0&&bytes<int(parent.size())&&parent[bytes]==0);
                    CHECK(visibility(type,box,nullptr,0)<0);
                    if(bytes){
                        dependencySeen=true;CHECK(std::string(parent.data())!=names[box]);
                        CHECK(std::find(names.begin(),names.end(),parent.data())!=names.end());
                        char shortBuffer[1]{};CHECK(visibility(type,box,shortBuffer,1)<0);
                    }
                }
                std::array<char,129> invalid{};CHECK(visibility(type,int(description[0]),invalid.data(),int(invalid.size()))<0);
            }
            if(abi==13)CHECK(dependencySeen);
            if(abi==14){
                const auto routeVersion=reinterpret_cast<int(*)()>(native::symbol(core,"NRFKotlin_ObservationRouteVersion"));
                const auto observesRoute=reinterpret_cast<int(*)(int)>(native::symbol(core,"NRFKotlin_ObservesRoute"));
                CHECK(routeVersion&&routeVersion()==1&&observesRoute);
                CHECK(native::symbol(core,"NRFKotlin_DecideTypeWithRoute"));
                CHECK(native::symbol(core,"NRFKotlin_PrepareNetworkWithRoute"));
                bool requested=false;
                for(int type=0;type<count;++type){
                    const auto enabled=observesRoute(type);CHECK(enabled==0||enabled==1);
                    std::array<int64_t,4> description{};CHECK(typeInfo(type,description.data())==0);
                    if(enabled){CHECK(description[3]>0);requested=true;}
                }
                CHECK(requested);
            }
        }
        native::unload(core);
    }
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
    auto restrictionsVersion=reinterpret_cast<int(*)()>(native::symbol(core,"NRFKotlin_ObservationRestrictionsVersion"));
    const bool restrictions=restrictionsVersion!=nullptr;
    if(restrictions)CHECK(restrictionsVersion()==1);
    auto observationTrainsVersion=reinterpret_cast<int(*)()>(native::symbol(core,"NRFKotlin_ObservationTrainsVersion"));
    const bool observationTrains=observationTrainsVersion!=nullptr;
    if(observationTrains)CHECK(observationTrainsVersion()==1);
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
    // Exercise each model through the actual C++ -> Kotlin native boundary.
    // Masks are local to the selected model, never a concatenation of panels.
    auto countTypes=reinterpret_cast<int(*)()>(native::symbol(core,"NRFKotlin_TypeCount"));
    if(countTypes) {
        using Typed=int(*)(int,int,int64_t,int,int64_t,int64_t,int64_t,const int*,int,int,int*);
        auto typed=reinterpret_cast<Typed>(native::symbol(core,"NRFKotlin_DecideType"));CHECK(typed);
        const int count=countTypes();CHECK(count>=1&&count<=16);
        for(int type=0;type<count;++type)for(int mask=0;mask<4;++mask) {
            network->signals.count=0;signal.typeIndex=type;signal.settings.mask=mask;
            network->signals.push_back(signal);
            const int observation[]{0,0,0,0,0,0,0};int expected[2]{};
            const int directStatus=typed(type,1,mask,2,signal.id,0,0,observation,-1,0,expected);
            CHECK(directStatus==0||directStatus==1);
            CHECK(invoke("nrf.kotlin.network.v1",network.get(),sizeof *network,result.get(),sizeof *result)==0);
            if(directStatus==0)CHECK(result->signals.items[0].result.decision.aspect==expected[0]&&
                result->signals.items[0].result.decision.reason==expected[1]);
        }
        // Real loader/core round trips, including prepareNetwork's grouped
        // observation. Running this adapter against a pre-capability core must
        // retain strict 0/1 forcedStop instead of treating a cause bit as true.
        for(int type=0;type<count;++type)for(int cause=0;cause<3;++cause)for(int forced=0;forced<2;++forced){
            signal.typeIndex=type;signal.settings.mask=1;
            signal.observation={nimby::BlockOccupancy::Clear,true,true,forced!=0,false,false,0};
            signal.approachingTrain=UINT64_C(0x5000000010001);
            if(cause)signal.observation.routeConflict=static_cast<nimby::RouteConflict>(cause);
            network->signals.count=0;network->signals.push_back(signal);
            const int bits=forced|(restrictions?(cause==1?2:cause==2?4:0):0);
            const int observation[]{1,1,1,bits,0,0,0};int expected[2]{};
            const int directStatus=typed(type,1,1,2,signal.id,0,INT64_C(0x5000000010001),observation,-1,0,expected);
            CHECK(directStatus==0||directStatus==1);
            CHECK(invoke("nrf.kotlin.network.v1",network.get(),sizeof *network,result.get(),sizeof *result)==0);
            if(directStatus==0)CHECK(result->signals.items[0].result.decision.aspect==expected[0]&&
                result->signals.items[0].result.decision.reason==expected[1]);
        }
        if(restrictions){
            int expected[2]{};
            for(int invalid:{6,7,8,-1}){
                const int observation[]{1,1,1,invalid,0,0,0};
                CHECK(typed(0,1,1,2,signal.id,0,INT64_C(0x5000000010001),observation,-1,0,expected)<0);
            }
        }
        // These optional exports belong to the owned live-observation path.
        // Command-v1 Signal stays value-only and continues to send Unknown.
        if(observationTrains){
            using WithTrains=int(*)(int,int,int64_t,int,int64_t,int64_t,int64_t,const int*,int,int,const int64_t*,int,int,int*);
            using PrepareWithTrains=int(*)(int,const int64_t*,const int*,const int*,const int64_t*,int,const int*,int64_t*,int*);
            const auto withTrains=reinterpret_cast<WithTrains>(native::symbol(core,"NRFKotlin_DecideTypeWithTrains"));
            const auto prepareWithTrains=reinterpret_cast<PrepareWithTrains>(native::symbol(core,"NRFKotlin_PrepareNetworkWithTrains"));
            CHECK(withTrains&&prepareWithTrains);
            const std::array<int64_t,2> trains{INT64_C(0x5000000010001),INT64_C(0x5000000010002)};
            const int observation[]{2,1,1,0,0,0,0};int expected[2]{};
            const auto decideObserved=[&](const int64_t* ids,int size,int complete){
                return withTrains(0,1,1,2,signal.id,0,trains[0],observation,-1,0,ids,size,complete,expected);
            };
            for(int complete:{0,1})for(int size:{0,2}){
                const auto status=decideObserved(size?trains.data():nullptr,size,complete);
                CHECK(status==0||status==1);
            }
            CHECK(decideObserved(trains.data(),-1,0)<0);
            CHECK(decideObserved(nullptr,1,0)<0);
            CHECK(decideObserved(trains.data(),4097,1)<0);
            for(int complete:{-1,2})CHECK(decideObserved(trains.data(),2,complete)<0);
            for(int64_t invalid:{INT64_C(0),INT64_C(0x5000000000000),INT64_C(0x1000000010001),INT64_C(-1)})
                CHECK(decideObserved(&invalid,1,1)<0);

            const int64_t identities[]{int64_t(signal.id),0,1,trains[0]};
            const int fields[]{0,2,2,1,1,0,0,0,0};
            const int offsets[]{0,2},complete[]{1};int64_t mask{};int status{};
            CHECK(prepareWithTrains(1,identities,fields,offsets,trains.data(),2,complete,&mask,&status)==0);
            CHECK(status==int(nimby::SettingsStatus::Present));
            const auto rejectBatch=[&](const int* positions,int size,const int* completeness){
                CHECK(prepareWithTrains(1,identities,fields,positions,trains.data(),size,completeness,&mask,&status)<0);
            };
            for(const std::array<int,2> invalid: {std::array{1,2},std::array{0,-1},std::array{0,1},std::array{2,0}})
                rejectBatch(invalid.data(),2,complete);
            const int excessive[]{0,4097};rejectBatch(excessive,4097,complete);
            rejectBatch(offsets,131073,complete);
            const int invalidComplete[]{2};rejectBatch(offsets,2,invalidComplete);
        }
        for(const std::uint32_t invalidType:{static_cast<std::uint32_t>(count),UINT32_MAX}){
            network->signals.items[0].typeIndex=invalidType;
            auto* outputBytes=reinterpret_cast<unsigned char*>(result.get());
            std::fill_n(outputBytes,sizeof *result,static_cast<unsigned char>(0xa5));
            CHECK(invoke("nrf.kotlin.network.v1",network.get(),sizeof *network,result.get(),sizeof *result)==NIMBY_INVALID_ARGUMENT);
            CHECK(std::all_of(outputBytes,outputBytes+sizeof *result,[](unsigned char value){return value==0;}));
        }
    }
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
