#include "runtime/signal_ui_endpoint.h"
#include <array>
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
    uint64_t revision{};CHECK(endpoint.settingsRevision(owner,session,&revision)==NIMBY_OK&&revision);
    const auto initialRevision=revision;
    CHECK(endpoint.settingsRevision(owner,session+1,&revision)==NIMBY_INVALID_HANDLE&&revision==0);
    NimbyUiSignalV1 signal{};signal.id=id;std::strcpy(signal.texture_set,"atlas");
    CHECK(endpoint.observe(owner,session,nullptr,1)==NIMBY_INVALID_ARGUMENT);
    CHECK(endpoint.observe(owner,session+1,&signal,1)==NIMBY_INVALID_HANDLE);
    CHECK(endpoint.observe(owner,session,&signal,1)==NIMBY_OK);
    CHECK(endpoint.settingsRevision(owner,session,&revision)==NIMBY_OK&&revision==initialRevision);
    CHECK(endpoint.read(owner,id,&values)==NIMBY_OK&&values.status==2&&values.count==1);
    CHECK(std::strcmp(values.fields[0].name,"active")==0&&values.fields[0].value==0);
    const uint64_t batchIds[]{id,id+99};NimbyUiReadBatchRowV1 batchRows[2]{};
    NimbyUiReadBatchHeaderV1 batchHeader{};batchHeader.size=sizeof batchHeader;batchHeader.version=1;
    CHECK(endpoint.readBatch(owner,batchIds,2,&batchHeader,batchRows)==NIMBY_OK);
    CHECK(batchHeader.count==2&&batchHeader.field_count==1&&std::string(batchHeader.names[0])=="active");
    CHECK(batchRows[0].status==2&&batchRows[0].values==0&&batchRows[1].status==1);
    // Exercise the same resident store through the renderer, then the C ABI.
    auto frame=endpoint.host.prepare(1,id);
    struct Click {void checkbox(const char*,const char*,uint32_t& value){value=1;}} click;
    nimby::runtime::SignalUiHost::interactive(frame,id,click);
    CHECK(endpoint.settingsRevision(owner,session,&revision)==NIMBY_OK&&revision>initialRevision);
    CHECK(endpoint.read(owner,id,&values)==NIMBY_OK&&values.fields[0].value==1);
    CHECK(endpoint.readBatch(owner,batchIds,2,&batchHeader,batchRows)==NIMBY_OK&&batchRows[0].values==1);
    CHECK(endpoint.readBatch(owner,batchIds,16385,&batchHeader,batchRows)==NIMBY_INVALID_ARGUMENT);
    {
        uint64_t copy{};const uint64_t target=id+0x10000;
        CHECK(endpoint.beginCopy(id,&copy)==NIMBY_OK&&copy);
        CHECK(endpoint.finishCopy(copy,&target,1)==NIMBY_OK);
        CHECK(endpoint.finishCopy(copy,&target,1)==NIMBY_INVALID_HANDLE);
        CHECK(endpoint.observe(owner,session,&signal,1)==NIMBY_OK);
        std::array<NimbyUiSignalV1,2> created{signal,signal};created[1].id=target;
        CHECK(endpoint.observe(owner,session,created.data(),2)==NIMBY_OK);
        CHECK(endpoint.read(owner,target,&values)==NIMBY_OK&&values.status==2&&values.fields[0].value==1);
        CHECK(endpoint.observe(owner,session,&signal,1)==NIMBY_OK);
        CHECK(endpoint.beginCopy(id,&copy)==NIMBY_OK);
        CHECK(endpoint.finishCopy(copy,nullptr,0)==NIMBY_OK);
    }
    // Two models in one mod: same setting name, separate catalogues/defaults,
    // no checkbox/persistence leakage when the editor changes selection.
    auto secondPanel=panel;std::strcpy(secondPanel.id,"test.second");
    std::strcpy(secondPanel.texture_set,"second.atlas");secondPanel.checkboxes[0].default_value=1;
    uint64_t secondOwner{},secondSession{};
    CHECK(endpoint.add(&secondPanel,&secondOwner)==NIMBY_OK);
    CHECK(endpoint.begin(secondOwner,"save-A",6,&secondSession)==NIMBY_OK);
    std::array<NimbyUiSignalV1,2> models{signal,signal};models[1].id=id+1;
    std::strcpy(models[1].texture_set,"second.atlas");
    CHECK(endpoint.observe(owner,session,models.data(),2)==NIMBY_OK);
    CHECK(endpoint.observe(secondOwner,secondSession,models.data(),2)==NIMBY_OK);
    CHECK(endpoint.read(owner,id+1,&values)==NIMBY_OK&&values.status==1);
    CHECK(endpoint.read(secondOwner,id,&values)==NIMBY_OK&&values.status==1);
    CHECK(endpoint.read(secondOwner,id+1,&values)==NIMBY_OK&&values.status==2&&values.fields[0].value==1);
    CHECK(endpoint.host.prepare(2,id).panels.size()==1);
    CHECK(endpoint.host.prepare(3,id+1).panels.front().owner==secondOwner);
    CHECK(endpoint.remove(secondOwner)==NIMBY_OK);
    CHECK(endpoint.read(owner,id,&values)==NIMBY_OK&&values.fields[0].value==1);
    // A migration acknowledgement occupies no UI row in ordinary profiles.
    CHECK(endpoint.add(&secondPanel,&secondOwner)==NIMBY_OK);
    CHECK(endpoint.conditionalVisibility(secondOwner,2)==NIMBY_INVALID_ARGUMENT);
    CHECK(endpoint.conditionalVisibility(secondOwner,1)==NIMBY_OK);
    CHECK(endpoint.begin(secondOwner,"save-A",6,&secondSession)==NIMBY_OK);
    CHECK(endpoint.observe(secondOwner,secondSession,models.data(),2)==NIMBY_OK);
    CHECK(endpoint.conditionalVisibility(secondOwner,0)==NIMBY_INVALID_ARGUMENT);
    auto conditional=endpoint.host.prepare(4,id+1);
    CHECK(conditional.panels.front().controls.controls.size()==1);
    auto secondStore=endpoint.host.store(secondOwner);
    CHECK(secondStore->setBoolean(conditional.panels.front().controls.editor,"active",false));
    CHECK(endpoint.host.prepare(5,id+1).panels.front().controls.controls.empty());
    CHECK(endpoint.read(secondOwner,id+1,&values)==NIMBY_OK&&values.fields[0].value==0);
    CHECK(endpoint.remove(secondOwner)==NIMBY_OK);
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
