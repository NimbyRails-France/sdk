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
typedef uint32_t (*NimbyUiBeginV1)(uint64_t owner,const char* identity,uint32_t length,uint64_t* session);
typedef uint32_t (*NimbyUiObserveV1)(uint64_t owner,uint64_t session,const NimbyUiSignalV1*,uint32_t count);
typedef uint32_t (*NimbyUiSuspendV1)(uint64_t owner);
typedef uint32_t (*NimbyUiReadV1)(uint64_t owner,uint64_t signal,NimbyUiValuesV1*);
// Optional persistence exports. Payload is the bounded SDK settings codec, not
// a game save. Export(nullptr,0) queries size; a short buffer receives no bytes.
typedef uint32_t (*NimbyUiExportV1)(uint64_t owner,uint64_t session,char* bytes,uint32_t capacity,uint32_t* written);
typedef uint32_t (*NimbyUiBeginSavedV1)(uint64_t owner,const char* identity,uint32_t identityLength,
    const char* bytes,uint32_t length,uint64_t* session);
