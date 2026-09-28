#pragma once
#include <stdint.h>

// Internal, same-process C ABI. No STL objects, borrowed mod callbacks or native
// game pointers cross this boundary. All input strings are copied during calls.
// Buffers must belong to the caller and remain valid for the synchronous call.
#define NIMBY_SIGNAL_UI_ABI 1u
typedef struct NimbyUiCheckboxV1 {
    char name[129],label[257],description[1025];
    uint32_t default_value;
} NimbyUiCheckboxV1;
typedef struct NimbyUiPanelV1 {
    uint32_t size,version,count,reserved;
    char id[129],title[257],texture_set[257];
    NimbyUiCheckboxV1 checkboxes[64];
} NimbyUiPanelV1;
typedef struct NimbyUiSignalV1 {
    uint64_t id;
    char texture_set[257];
} NimbyUiSignalV1;
typedef struct NimbyUiValuesV1 {
    uint32_t size,version,status,count; // status: 0 unavailable, 1 absent, 2 present
    struct {char name[129];uint32_t value;} fields[64];
} NimbyUiValuesV1;

// Function types for GetProcAddress. Owner/session are opaque SDK tokens.
typedef uint32_t (*NimbyUiRegisterV1)(const NimbyUiPanelV1*,uint64_t* owner);
typedef uint32_t (*NimbyUiRemoveV1)(uint64_t owner);
typedef uint32_t (*NimbyUiConditionalVisibilityV1)(uint64_t owner,uint64_t mask);
typedef uint32_t (*NimbyUiBeginV1)(uint64_t owner,const char* identity,uint32_t length,uint64_t* session);
typedef uint32_t (*NimbyUiObserveV1)(uint64_t owner,uint64_t session,const NimbyUiSignalV1*,uint32_t count);
typedef uint32_t (*NimbyUiSuspendV1)(uint64_t owner);
typedef uint32_t (*NimbyUiReadV1)(uint64_t owner,uint64_t signal,NimbyUiValuesV1*);
// Optional persistence exports. Payload is the bounded SDK settings codec, not
// a game save. Export(nullptr,0) queries size; a short buffer receives no bytes.
typedef uint32_t (*NimbyUiExportV1)(uint64_t owner,uint64_t session,char* bytes,uint32_t capacity,uint32_t* written);
typedef uint32_t (*NimbyUiBeginSavedV1)(uint64_t owner,const char* identity,uint32_t identityLength,
    const char* bytes,uint32_t length,uint64_t* session);

// Additive optional-service ABI. All names are bounded UTF-8, all tokens opaque.
// No previous structure changes size; older UI bridges may ignore this feature.
typedef struct NimbyUiActionV1 {char id[129],label[257],provider[129],service[129];} NimbyUiActionV1;
typedef struct NimbyUiProviderV1 {
    uint32_t size,version,count,reserved;
    char id[129],services[32][129];
} NimbyUiProviderV1;
typedef struct NimbyUiActionEventV1 {
    uint32_t size,version;
    uint64_t sequence,signal,generation,panel;
    char action[129],service[129],world[513],origin[129];
} NimbyUiActionEventV1;
typedef struct NimbyUiToolPanelV1 {
    uint32_t size,version,count,reserved;
    uint64_t panel,signal;
    char service[129],origin[129],message[257];
    struct {char id[129],label[257];uint32_t enabled;} buttons[12];
} NimbyUiToolPanelV1;
typedef uint32_t (*NimbyUiActionsV1)(uint64_t owner,const NimbyUiActionV1*,uint32_t count);
typedef uint32_t (*NimbyUiPanelContextV1)(uint64_t owner,uint64_t session,uint64_t generation);
typedef uint32_t (*NimbyUiProviderAddV1)(const NimbyUiProviderV1*,uint64_t* token);
typedef uint32_t (*NimbyUiProviderRemoveV1)(uint64_t token);
typedef uint32_t (*NimbyUiProviderObserveV1)(uint64_t token,const char* world,uint32_t length,uint64_t generation);
typedef uint32_t (*NimbyUiProviderSuspendV1)(uint64_t token);
typedef uint32_t (*NimbyUiProviderPollV1)(uint64_t token,NimbyUiActionEventV1*);
typedef uint32_t (*NimbyUiModPresentV1)(const char* id,uint32_t length,uint32_t* present);
typedef uint32_t (*NimbyUiToolPanelPublishV1)(uint64_t provider,const NimbyUiToolPanelV1*);

// Additive numeric controls: V1 layouts and exports remain unchanged.
typedef struct NimbyUiNumberInputV1 {
    char id[129],label[257];
    int32_t value,minimum,maximum;
    uint32_t enabled;
} NimbyUiNumberInputV1;
typedef struct NimbyUiToolPanelV2 {
    NimbyUiToolPanelV1 base;
    uint32_t input_count,reserved;
    NimbyUiNumberInputV1 inputs[4];
} NimbyUiToolPanelV2;
typedef struct NimbyUiActionEventV2 {
    NimbyUiActionEventV1 base;
    uint32_t has_value;
    int32_t value;
} NimbyUiActionEventV2;
typedef uint32_t (*NimbyUiToolPanelPublishV2)(uint64_t,const NimbyUiToolPanelV2*);
typedef uint32_t (*NimbyUiProviderPollV2)(uint64_t,NimbyUiActionEventV2*);

// Additive, transient map preview. No construction command is issued. A zero
// count clears this provider's preview; at most 64 ghosts can be active globally.
typedef struct NimbyUiPreviewPositionV1 {
    uint64_t track;
    double fraction;
    int32_t direction;
    uint32_t reserved;
} NimbyUiPreviewPositionV1;
typedef struct NimbyUiSignalPreviewV1 {
    uint32_t size,version,count,reserved;
    uint64_t panel,signal;
    char service[129],origin[129];
    NimbyUiPreviewPositionV1 positions[64];
} NimbyUiSignalPreviewV1;
// Copied UTF-8 catalogue; kind 0 = panel owner, 1 = service provider.
typedef uint32_t (*NimbyUiTranslationsV1)(uint32_t,uint64_t,const char*,uint32_t);
typedef uint32_t (*NimbyUiSignalPreviewPublishV1)(uint64_t,const NimbyUiSignalPreviewV1*);
// Internal native construction handoff. No STL objects cross DLL boundaries.
typedef uint32_t (*NimbyUiSettingsCopyBeginV1)(uint64_t,uint64_t*);
typedef uint32_t (*NimbyUiSettingsCopyFinishV1)(uint64_t,const uint64_t*,uint32_t);
