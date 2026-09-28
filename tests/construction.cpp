#ifdef NDEBUG
#undef NDEBUG
#endif
#include <engine/construction.h>
#include <initializer_list>
#include <cassert>
#include <cstddef>
#include <limits>

int main() {
    using nimby::engine::construction::valid;
    static_assert(sizeof(NimbyConstructionPosition)==24);
    static_assert(sizeof(NimbyConstructionRequest)==1568);
    static_assert(sizeof(NimbyConstructionResult)==544);
    static_assert(offsetof(NimbyConstructionRequest,positions)==32);
    static_assert(offsetof(NimbyConstructionResult,ids)==32);
    NimbyConstructionRequest r{};r.size=sizeof r;r.version=1;
    r.action=NIMBY_CONSTRUCTION_PREPARE;r.source_signal=0x8000000000001;
    assert(valid(r));r.token=1;assert(!valid(r));
    r.action=NIMBY_CONSTRUCTION_CREATE;r.count=1;
    r.positions[0]={0x1000000000001,.25,-1,0};assert(valid(r));
    auto bad=r;bad.positions[0].fraction=std::numeric_limits<double>::quiet_NaN();assert(!valid(bad));
    for(double fraction:{0.,1.,-1.,std::numeric_limits<double>::infinity()}) {
        bad=r;bad.positions[0].fraction=fraction;assert(!valid(bad));
    }
    bad=r;bad.positions[0].direction=0;assert(!valid(bad));
    bad=r;bad.positions[0].reserved=1;assert(!valid(bad));
    bad=r;bad.positions[0].track_id=r.source_signal;assert(!valid(bad));
    bad=r;bad.count=65;assert(!valid(bad));
    bad=r;bad.count=2;bad.positions[1]=r.positions[0];assert(!valid(bad));
    bad.positions[1].direction=1;assert(!valid(bad)); // Both directions occupy this location.
    bad.positions[1].fraction=.5;assert(valid(bad));
    r.action=NIMBY_CONSTRUCTION_UNDO;r.count=0;r.source_signal=0;assert(valid(r));
    r.token=0;assert(!valid(r));
}
