#pragma once
#include "observation.h"

// Explicit, versioned recipe-control protocol. IDs are values, never pointers.
// A command is accepted at most once by its caller: timeouts are not retried.
#define NIMBY_CONTROL_VERSION 1u
#define NIMBY_CONTROL_STATUS 0u
#define NIMBY_CONTROL_ACQUIRE 1u
#define NIMBY_CONTROL_RENEW 2u
#define NIMBY_CONTROL_RELEASE 3u
#define NIMBY_CONTROL_FORCE_SIGNAL 4u
#define NIMBY_CONTROL_RESTORE_SIGNAL 5u
#define NIMBY_CONTROL_TRAIN 6u
#define NIMBY_CONTROL_RESTORE_TRAIN 7u
#define NIMBY_CONTROL_SETTING 8u
#define NIMBY_CONTROL_RESTORE_SETTING 9u
#define NIMBY_CONTROL_CLEAR 10u
#define NIMBY_CONTROL_READ_SIGNAL 11u
#define NIMBY_CONTROL_READ_TRAIN 12u

#define NIMBY_CONTROL_SPEED_LIMIT 0u
#define NIMBY_CONTROL_PHYSICAL_CLEARANCE 1u
#define NIMBY_CONTROL_STOP 2u
#define NIMBY_CONTROL_RELEASE_BY_REAR 1u

typedef struct NimbyControlRequest {
    uint32_t size,version,operation,lease_ms;
    uint64_t owner,generation,object,exit_signal;
    double speed_mps;
    uint32_t mode,flags;
    int32_t value,index;
} NimbyControlRequest;
typedef struct NimbyControlResponse {
    uint32_t size,version,result,capabilities;
    uint64_t generation,remaining_ms;
    uint32_t signal_count,train_count,setting_count,active;
    int32_t aspect,reason;
    double speed_mps;
    uint64_t exit_signal;
    char detail[256];
} NimbyControlResponse;
typedef uint32_t (*NimbyControlHandler)(const NimbyControlRequest*,NimbyControlResponse*);

#ifdef __cplusplus
extern "C" {
#endif
NIMBY_API uint32_t __cdecl NimbyInternal_ModControl(uint32_t pid,const char* mod_id,
    const NimbyControlRequest* request,NimbyControlResponse* response) NIMBY_NOEXCEPT;
#ifdef __cplusplus
}
static_assert(sizeof(NimbyControlRequest)==72);
static_assert(sizeof(NimbyControlResponse)==328);
#endif
