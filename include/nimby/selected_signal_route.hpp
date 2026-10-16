#pragma once
#include <nimby/blocks.hpp>
#include <string>

namespace nimby {
// Facts about the route the game analysed, never ownership or permission.
enum class RouteDestination { Unknown, ProtectingSignal, NativeBoundary };
struct SignalRouteMarker {
    std::uint64_t id=0;
    std::string textureSet;
    int textureState=0;
};
struct SelectedSignalRoute {
    std::uint64_t trainId=0,boundarySignalId=0;
    RouteDestination destination=RouteDestination::Unknown;
    BlockOccupancy occupation=BlockOccupancy::Unknown;
    std::vector<SignalRouteMarker> markers;
    bool complete=false;
};
}
