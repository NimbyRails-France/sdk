#pragma once
#include <stdint.h>

// Private, additive SDK ABI. JSON carries copied declarations/values, never
// executable callbacks or game pointers. A token belongs to one mod host.
#define NIMBY_OPTIONS_SCHEMA_LIMIT (2u * 1024u * 1024u)
#define NIMBY_OPTIONS_VALUES_LIMIT (64u * 4096u)
typedef uint32_t (*NimbyOptionsRegisterV1)(const char* declaration,uint32_t bytes,uint64_t* owner);
typedef uint32_t (*NimbyOptionsRemoveV1)(uint64_t owner);
// Forget queued shortcut actions when observations or the game session change.
// Values and their revision remain unchanged.
typedef uint32_t (*NimbyOptionsDiscardV1)(uint64_t owner);
// An unchanged snapshot with no input events writes zero bytes. Changed values
// and queued actions are returned together as a bounded, immutable snapshot.
typedef uint32_t (*NimbyOptionsReadV1)(uint64_t owner,uint64_t known_revision,
    char* output,uint32_t capacity,uint32_t* written,uint64_t* revision);
// Parent-only binding: no child-supplied handle may reach this export.
typedef uint32_t (*NimbyOptionsWakeV1)(uint64_t owner,uint64_t parent_event);
