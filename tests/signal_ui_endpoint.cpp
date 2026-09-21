#include "runtime/signal_ui_endpoint.h"
#include <iostream>
#include <stdexcept>
#define CHECK(x) do{if(!(x))throw std::runtime_error("Failed line "+std::to_string(__LINE__));}while(false)
int main(){try{
    nimby::runtime::SignalUiEndpoint endpoint;
    NimbyUiPanelV1 panel{};panel.size=sizeof(panel);panel.version=NIMBY_SIGNAL_UI_ABI;panel.count=1;
    std::strcpy(panel.id,"test");std::strcpy(panel.title,"Test");std::strcpy(panel.texture_set,"atlas");
    std::strcpy(panel.checkboxes[0].name,"active");std::strcpy(panel.checkboxes[0].label,"Active");
    uint64_t owner=99,session=99;
    CHECK(endpoint.add(nullptr,&owner)==NIMBY_INVALID_ARGUMENT&&owner==0);
    panel.version=2;
    CHECK(endpoint.add(&panel,&owner)==NIMBY_INVALID_ARGUMENT&&owner==0);
    panel.version=1;panel.checkboxes[0].default_value=2;
    CHECK(endpoint.add(&panel,&owner)==NIMBY_INVALID_ARGUMENT);
    panel.checkboxes[0].default_value=0;
    std::memset(panel.checkboxes[0].description,'a',sizeof(panel.checkboxes[0].description));
    CHECK(endpoint.add(&panel,&owner)==NIMBY_INVALID_ARGUMENT);
    panel.checkboxes[0].description[0]=0;
    CHECK(endpoint.add(&panel,&owner)==NIMBY_OK&&owner);
    uint64_t duplicate{};CHECK(endpoint.add(&panel,&duplicate)==NIMBY_INVALID_ARGUMENT&&duplicate==0);
    NimbyUiValuesV1 values{};values.size=sizeof(values);values.version=1;
    constexpr uint64_t id=0x8000000000001;
    CHECK(endpoint.read(owner,id,&values)==NIMBY_OK&&values.status==0);
    CHECK(endpoint.begin(owner,"save-A",6,&session)==NIMBY_OK&&session);
    NimbyUiSignalV1 signal{};signal.id=id;std::strcpy(signal.texture_set,"atlas");
    CHECK(endpoint.observe(owner,session,nullptr,1)==NIMBY_INVALID_ARGUMENT);
    CHECK(endpoint.observe(owner,session+1,&signal,1)==NIMBY_INVALID_HANDLE);
    CHECK(endpoint.observe(owner,session,&signal,1)==NIMBY_OK);
    CHECK(endpoint.read(owner,id,&values)==NIMBY_OK&&values.status==2&&values.count==1);
    CHECK(std::strcmp(values.fields[0].name,"active")==0&&values.fields[0].value==0);
    // Exercise the same resident store through the renderer, then the C ABI.
    auto frame=endpoint.host.prepare(1,id);
    struct Click {void checkbox(const char*,const char*,uint32_t& value){value=1;}} click;
    nimby::runtime::SignalUiHost::interactive(frame,id,click);
    CHECK(endpoint.read(owner,id,&values)==NIMBY_OK&&values.fields[0].value==1);
    uint32_t needed=99;
    CHECK(endpoint.exportSettings(owner,session,nullptr,0,&needed)==NIMBY_OK&&needed);
    std::string saved(needed,'?');uint32_t written=99;
    CHECK(endpoint.exportSettings(owner,session,saved.data(),needed-1,&written)==NIMBY_RESOURCE_LIMIT);
    CHECK(saved==std::string(needed,'?')&&written==needed); // No partial payload.
    CHECK(endpoint.exportSettings(owner,session,saved.data(),needed,&written)==NIMBY_OK&&written==needed);
    CHECK(endpoint.exportSettings(owner,session+1,saved.data(),needed,&written)==NIMBY_INVALID_ARGUMENT&&written==0);
    uint64_t restored=99;
    CHECK(endpoint.beginSaved(owner,"save-B",6,saved.data(),needed,&restored)==NIMBY_INVALID_ARGUMENT&&restored==0);
    CHECK(endpoint.read(owner,id,&values)==NIMBY_OK&&values.fields[0].value==1);
    auto corrupt=saved;corrupt.back()^=1;
    CHECK(endpoint.beginSaved(owner,"save-A",6,corrupt.data(),needed,&restored)==NIMBY_INVALID_ARGUMENT);
    CHECK(endpoint.read(owner,id,&values)==NIMBY_OK&&values.fields[0].value==1);
    CHECK(endpoint.beginSaved(owner,"save-A",6,saved.data(),needed,&restored)==NIMBY_OK&&restored!=session);
    CHECK(endpoint.read(owner,id,&values)==NIMBY_OK&&values.status==0); // Await catalog.
    CHECK(endpoint.observe(owner,session,&signal,1)==NIMBY_INVALID_HANDLE);
    CHECK(endpoint.exportSettings(owner,session,nullptr,0,&written)==NIMBY_INVALID_ARGUMENT);
    CHECK(endpoint.observe(owner,restored,&signal,1)==NIMBY_OK);
    CHECK(endpoint.read(owner,id,&values)==NIMBY_OK&&values.fields[0].value==1);
    session=restored;
    CHECK(endpoint.suspend(owner)==NIMBY_OK);
    CHECK(endpoint.read(owner,id,&values)==NIMBY_OK&&values.status==0&&values.count==0);
    CHECK(endpoint.observe(owner,session,&signal,1)==NIMBY_OK);
    CHECK(endpoint.read(owner,id,&values)==NIMBY_OK&&values.fields[0].value==1);
    CHECK(endpoint.remove(owner)==NIMBY_OK);
    CHECK(endpoint.exportSettings(owner,session,nullptr,0,&written)==NIMBY_INVALID_HANDLE&&written==0);
    CHECK(endpoint.begin(owner,"save-B",6,&session)==NIMBY_INVALID_HANDLE&&session==0);
    CHECK(endpoint.read(owner,id,&values)==NIMBY_INVALID_HANDLE&&values.status==0&&values.count==0);
    nimby::runtime::SignalUiHost::interactive(frame,id,click); // Removed owner: no dangling mod data.
    CHECK(endpoint.remove(owner)==NIMBY_INVALID_HANDLE);
    std::cout<<"PASS signal UI C ABI model and renderer exchange\n";
}catch(const std::exception& error){std::cerr<<error.what()<<'\n';return 1;}}
