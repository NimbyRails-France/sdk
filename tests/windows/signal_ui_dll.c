#include <windows.h>
#include <nimby/detail/sdk.h>
#include <nimby/detail/observation.h>
#include <nimby/detail/signal_ui_bridge.h>
#include <stdio.h>
#include <string.h>

#define CHECK(x) do {if(!(x)){fprintf(stderr,"Failed line %d\n",__LINE__);return 1;}} while(0)
#define LOAD(type,var,name) type var; do { FARPROC address=GetProcAddress(dll,name); CHECK(sizeof(var)==sizeof(address)); memcpy(&var,&address,sizeof(var)); } while(0)
// Runs in an ordinary executable. Registration/read exports are exercised across
// the real DLL boundary; bootstrap must refuse to hook this non-game process.
int main(int argc,char** argv) {
    CHECK(argc==2);
    HMODULE dll=LoadLibraryA(argv[1]);CHECK(dll);
    LOAD(NimbyUiRegisterV1,add,"NimbyUi_RegisterV1");
    LOAD(NimbyUiRemoveV1,removeOwner,"NimbyUi_RemoveV1");
    LOAD(NimbyUiBeginV1,begin,"NimbyUi_BeginV1");
    LOAD(NimbyUiObserveV1,observe,"NimbyUi_ObserveV1");
    LOAD(NimbyUiSuspendV1,suspend,"NimbyUi_SuspendV1");
    LOAD(NimbyUiReadV1,readValues,"NimbyUi_ReadV1");
    typedef DWORD (WINAPI *Bootstrap)(void*);
    LOAD(Bootstrap,bootstrap,"NimbyInternal_Bootstrap");
    CHECK(add&&removeOwner&&begin&&observe&&suspend&&readValues&&bootstrap);
    CHECK(bootstrap(NULL)==NIMBY_INVALID_BINARY);
    NimbyUiPanelV1 panel={0};panel.size=sizeof(panel);panel.version=1;panel.count=1;
    strcpy(panel.id,"c-test");strcpy(panel.title,"C test");strcpy(panel.texture_set,"atlas");
    strcpy(panel.checkboxes[0].name,"active");strcpy(panel.checkboxes[0].label,"Active");
    panel.checkboxes[0].default_value=1;
    uint64_t owner=0,session=0;
    CHECK(add(&panel,&owner)==NIMBY_OK&&owner);
    memset(&panel,0,sizeof(panel)); // No borrowed declaration may survive add().
    CHECK(begin(owner,"test-save",9,&session)==NIMBY_OK&&session);
    NimbyUiSignalV1 signal={0};signal.id=0x8000000000001ULL;strcpy(signal.texture_set,"atlas");
    CHECK(observe(owner,session,&signal,1)==NIMBY_OK);
    NimbyUiValuesV1 values={0};values.size=sizeof(values);values.version=1;
    CHECK(readValues(owner,signal.id,&values)==NIMBY_OK);
    CHECK(values.status==2&&values.count==1&&values.fields[0].value==1);
    CHECK(strcmp(values.fields[0].name,"active")==0);
    CHECK(suspend(owner)==NIMBY_OK);
    CHECK(readValues(owner,signal.id,&values)==NIMBY_OK&&values.status==0);
    CHECK(removeOwner(owner)==NIMBY_OK);
    CHECK(readValues(owner,signal.id,&values)==NIMBY_INVALID_HANDLE&&values.count==0);
    CHECK(FreeLibrary(dll));
    puts("PASS real signal UI DLL C exports; non-game hook refused");
    return 0;
}
