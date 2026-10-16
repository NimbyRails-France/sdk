#include <platform/windows/mod_options_host.h>
#include <nimby/detail/mod_options_bridge.h>
#include <nimby/detail/observation.h>
#include <windows.h>
#include <cassert>
#include <exception>
#include <iostream>
#include "allocation_fault_fixture.h"
extern "C" uint32_t NimbyOptions_RegisterV1(const char*,uint32_t,uint64_t*) noexcept;
extern "C" uint32_t NimbyOptions_ReadV1(uint64_t,uint64_t,char*,uint32_t,uint32_t*,uint64_t*) noexcept;
extern "C" uint32_t NimbyOptions_RemoveV1(uint64_t) noexcept;
namespace service=nimby::platform::windows::mod_options;
namespace {service::Host* activeTestHost{};}
namespace nimby::platform::windows::mod_options {
Host& modOptionsHostForTest(){assert(activeTestHost);return *activeTestHost;}
}
int main(){
    SetErrorMode(SEM_NOGPFAULTERRORBOX);
    const auto temporary=std::filesystem::canonical(std::filesystem::temp_directory_path());
    const auto directory=temporary/("nrf-options-allocation-"+std::to_string(GetCurrentProcessId())+"-"+std::to_string(GetTickCount64()));
    assert(directory.is_absolute()&&directory.parent_path()==temporary&&std::filesystem::create_directory(directory));
    struct Cleanup{std::filesystem::path path;~Cleanup(){std::error_code error;std::filesystem::remove_all(path,error);}} cleanup{directory};
    // Host destruction stops/joins its persistence worker before directory
    // cleanup. Production's pinned singleton is not created by this target.
    service::Host host(directory);assert(host.initializeDrivingOptions());activeTestHost=&host;
    const std::string declaration=R"({"id":"private-json-export","title":"Test","fields":[{"id":"enabled","label":"Enabled","description":"","kind":0,"default":"true"},{"id":"count","label":"Count","kind":1,"default":"4","minimum":1,"maximum":8},{"id":"window.main","label":"Window","kind":3,"default":"F8"}]})";
    uint64_t owner{};
    assert(NimbyOptions_RegisterV1(declaration.data(),static_cast<uint32_t>(declaration.size()),&owner)==NIMBY_OK&&owner);
    assert(NimbyOptions_RemoveV1(owner)==NIMBY_OK);
    const auto healthy=R"({"id":"private-json-healthy","title":"Sibling","fields":[{"id":"enabled","label":"Enabled","kind":0,"default":"true"}]})";
    uint64_t healthyOwner{};
    assert(NimbyOptions_RegisterV1(healthy,static_cast<uint32_t>(strlen(healthy)),&healthyOwner)==NIMBY_OK&&healthyOwner);
    // Warm saved-state reads before counting the identical registration path.
    assert(NimbyOptions_RegisterV1(declaration.data(),static_cast<uint32_t>(declaration.size()),&owner)==NIMBY_OK&&owner);
    assert(NimbyOptions_RemoveV1(owner)==NIMBY_OK);
    allocation_fault_fixture::arm(SIZE_MAX,0);
    assert(NimbyOptions_RegisterV1(declaration.data(),static_cast<uint32_t>(declaration.size()),&owner)==NIMBY_OK&&owner);
    const auto baseline=allocation_fault_fixture::stop();
    assert(NimbyOptions_RemoveV1(owner)==NIMBY_OK);
    size_t faults=0;
    for(const unsigned failureCount:{2u,std::numeric_limits<unsigned>::max()}){
        for(size_t at=0;at<baseline.allocations;++at){
            allocation_fault_fixture::arm(at,failureCount);
            const auto status=NimbyOptions_RegisterV1(declaration.data(),static_cast<uint32_t>(declaration.size()),&owner);
            const auto fault=allocation_fault_fixture::stop();
            faults+=fault.failures!=0;
            if(status==NIMBY_OK){assert(owner);assert(NimbyOptions_RemoveV1(owner)==NIMBY_OK);}
            else {
                assert(status==NIMBY_RESOURCE_LIMIT&&owner==0);
                assert(nimby::platform::windows::mod_options::host().registry.snapshot()->mods.size()==2);
                assert(NimbyOptions_RegisterV1(declaration.data(),static_cast<uint32_t>(declaration.size()),&owner)==NIMBY_OK&&owner);
                assert(NimbyOptions_RemoveV1(owner)==NIMBY_OK);
            }
        }
    }
    std::vector<char> response(NIMBY_OPTIONS_VALUES_LIMIT);uint32_t written{};uint64_t revision{};
    allocation_fault_fixture::arm(SIZE_MAX,0);
    assert(NimbyOptions_ReadV1(healthyOwner,0,response.data(),static_cast<uint32_t>(response.size()),&written,&revision)==NIMBY_OK&&written&&revision);
    const auto readBaseline=allocation_fault_fixture::stop();size_t readFaults=0;
    for(const unsigned failureCount:{2u,std::numeric_limits<unsigned>::max()}){
        for(size_t at=0;at<readBaseline.allocations;++at){
            written=999;revision=999;allocation_fault_fixture::arm(at,failureCount);
            const auto status=NimbyOptions_ReadV1(healthyOwner,0,response.data(),static_cast<uint32_t>(response.size()),&written,&revision);
            const auto fault=allocation_fault_fixture::stop();readFaults+=fault.failures!=0;
            if(status!=NIMBY_OK){assert(status==NIMBY_RESOURCE_LIMIT&&written==0&&revision==0);}
            assert(NimbyOptions_ReadV1(healthyOwner,0,response.data(),static_cast<uint32_t>(response.size()),&written,&revision)==NIMBY_OK&&written&&revision);
        }
    }
    std::cout<<"PASS exact ReadV1 export: "<<readFaults<<" double/persistent allocation fault attempts; zero failed outputs and retry succeeds\n";
    assert(NimbyOptions_RemoveV1(healthyOwner)==NIMBY_OK);
    assert(faults);
    std::cout<<"PASS exact RegisterV1 export: "<<faults<<" double/persistent allocation fault attempts; RESOURCE_LIMIT owner0, same-ID retry and healthy owner survive\n";
}
