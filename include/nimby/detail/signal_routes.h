#pragma once
#include <nimby/detail/observation.h>
#include <nimby/detail/sdk.h>

// Private copied facts from the game's own ordered traversal. A path's raw
// vector of track IDs is never interpreted as an ordered or oriented route.
// Native admission is one observed stage, not the game's final permission.
typedef struct NimbySignalRouteObservation {
    uint64_t train_id,signal_id,next_signal_id,route_revision,observed_at_ms;
    uint64_t head_track_id;
    double head_fraction,distance_m;
    int32_t head_direction;
    uint32_t blocks,flags,reserved;
    uint64_t source_track_id;
    double source_fraction;
    int32_t source_direction;
    uint32_t source_kind;
    // An ordered downstream boundary is complete only while its snapshot
    // anchor still agrees with these independently revalidated native facts.
    uint64_t next_track_id;
    double next_fraction;
    int32_t next_direction;
    uint32_t next_kind;
} NimbySignalRouteObservation;
#define NIMBY_SIGNAL_ROUTE_ORDERED 1u
#define NIMBY_SIGNAL_ROUTE_NATIVE_CLEAR 2u
#define NIMBY_SIGNAL_ROUTE_NATIVE_BLOCKED 4u
#define NIMBY_SIGNAL_ROUTE_BLOCK_COMPLETE 8u
// Specific refusals at the pinned crossing call sites, never inferred from
// NATIVE_BLOCKED. Neither establishes route clearance or movement authority.
#define NIMBY_SIGNAL_ROUTE_CROSSING_OCCUPIED 16u
#define NIMBY_SIGNAL_ROUTE_CROSSING_RESERVED 32u
typedef struct NimbySignalRouteSection {
    uint64_t train_id,signal_id,track_id;
    // V1: normalized low/high. V2: oriented FROM/TO in native traversal order;
    // decreasing fractions are valid. Never sort V2 sections within a route.
    double begin_fraction,end_fraction;
} NimbySignalRouteSection;

// V2 additive selected-path facts. These are objects actually encountered by
// the existing native scan, NOT reservations owned by this train. The native
// range discriminator (6) must not be confused with the object's native_kind.
// distance_m is measured downstream from signal_id; ordinal preserves scan
// order, including coincident objects. direction is travel direction on track,
// not the object's stored orientation. No pointer or native callback escapes.
typedef struct NimbySignalRouteAnnotation {
    uint64_t train_id,signal_id,object_id,track_id;
    double track_fraction,distance_m;
    int32_t direction,native_kind;
    uint32_t ordinal,flags;
} NimbySignalRouteAnnotation;
#define NIMBY_SELECTED_ROUTE_FIRST_BOUNDARY 1u
#define NIMBY_SELECTED_ROUTE_PROTECTING_SIGNAL 2u
#define NIMBY_SELECTED_ROUTE_NATIVE_BOUNDARY 4u
// BALISE only: its existing native filter descriptor was read back unchanged,
// default mode applies and its exception vector is empty. This establishes
// applicability, never a reservation or permission. Absent means unknown/filter.
#define NIMBY_SELECTED_ROUTE_UNCONDITIONAL_BALISE 8u

#ifdef __cplusplus
extern "C" {
#endif
// Sorted unique full Signal IDs; one bounded batch shared by a mod's models.
// No call to a game predicate and no reservation mutation is made by readback.
NIMBY_API uint32_t __cdecl NimbyInternal_CopySignalRoutes(const uint64_t* signals,
    uint32_t signal_count,uint32_t blocks,NimbySignalRouteObservation* out,
    uint32_t capacity,uint32_t* count,NimbySignalRouteSection* sections,
    uint32_t section_capacity,uint32_t* section_count) NIMBY_NOEXCEPT;
// One capture returns both the unchanged V1 rows/ranges and ordered V2 object
// annotations. FIRST_BOUNDARY marks the first encountered PATH or BALISE;
// a passive MARKER does not end the selected route. It is never an inferred
// end of an incomplete scan. NATIVE_BOUNDARY marks a BALISE record, never
// ReservationEnd, ownership or movement permission.
// No annotations means unobserved; it does not establish a terminal route.
NIMBY_API uint32_t __cdecl NimbyInternal_CopySignalRoutesV2(const uint64_t* signals,
    uint32_t signal_count,uint32_t blocks,NimbySignalRouteObservation* out,
    uint32_t capacity,uint32_t* count,NimbySignalRouteSection* sections,
    uint32_t section_capacity,uint32_t* section_count,NimbySignalRouteAnnotation* annotations,
    uint32_t annotation_capacity,uint32_t* annotation_count) NIMBY_NOEXCEPT;
#ifdef __cplusplus
}
static_assert(sizeof(NimbySignalRouteObservation)==128);
static_assert(sizeof(NimbySignalRouteSection)==40);
static_assert(sizeof(NimbySignalRouteAnnotation)==64);
#endif
