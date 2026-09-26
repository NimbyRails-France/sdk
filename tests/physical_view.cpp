#include "engine/physical_view.h"
#include <cassert>
#include <limits>

int main() {
    using nimby::engine::automatic::PhysicalView;
    const PhysicalView view{100,150,120,1000,true};
    assert(view.at(110,1250).verified && view.at(110,1250).distanceM==110);
    assert(!view.at(110,1251).verified); // Freshness boundary is inclusive.
    assert(!view.at(110,999).verified); // A clock reset invalidates evidence.
    assert(!view.at(99,1100).verified); // Reversing cannot reuse forward coverage.
    assert(!view.at(std::numeric_limits<double>::quiet_NaN(),1100).verified);
    assert(view.at(250,1100).distanceM==0);
    assert(!PhysicalView{}.at(0,0).verified);
}
