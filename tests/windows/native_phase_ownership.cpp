// Portable synthetic fixture: native ABI, lock ownership and world completeness
// are NOT certified by these tests. No process memory or game is accessed.
#include "platform/windows/runtime/native_phase_ownership.h"
#include <cstdlib>
#include <iostream>
#include <new>
#include <stdexcept>
#include <string>
#include <thread>
#include <type_traits>

namespace {
bool watchAllocations=false;
size_t allocations{},checks{};
}
#if defined(__GNUC__)
#define OWNERSHIP_NOINLINE __attribute__((noinline))
#else
#define OWNERSHIP_NOINLINE __declspec(noinline)
#endif
OWNERSHIP_NOINLINE void* operator new(std::size_t size) {
    if(watchAllocations)++allocations;
    if(void* value=std::malloc(size?size:1))return value;
    throw std::bad_alloc();
}
OWNERSHIP_NOINLINE void* operator new[](std::size_t size){return ::operator new(size);}
OWNERSHIP_NOINLINE void operator delete(void* value)noexcept{std::free(value);}
OWNERSHIP_NOINLINE void operator delete[](void* value)noexcept{std::free(value);}
OWNERSHIP_NOINLINE void operator delete(void* value,std::size_t)noexcept{std::free(value);}
OWNERSHIP_NOINLINE void operator delete[](void* value,std::size_t)noexcept{std::free(value);}
#define CHECK(x) do{++checks;if(!(x))throw std::runtime_error("line "+std::to_string(__LINE__)+": " #x);}while(false)
namespace {
using namespace nimby::windows::phase_ownership;
constexpr uintptr_t base=0x140000000;
struct Fixture {
    ControllerFrame controller{0x200000,0x210000,true};
    AdvanceFrame advance;
    SimulationFrame simulation;
    TrainPhaseFrame train;
    Fixture(int64_t elapsed=50) {
        advance.controllerReference=controller.controllerReference;
        advance.controller=controller.controller;
        advance.controllerSimulation=advance.simulation=0x300000;
        advance.controllerNetwork=advance.network=0x400000;
        advance.resourceReference=0x500000;advance.descriptor=0x600000;
        advance.returnAddress=base+0x3495fe;advance.resource={0x800000,0x810000};
        advance.controllerResource=advance.resource;
        advance.ordinal=91;advance.elapsed=elapsed;advance.readComplete=true;
        simulation={advance.simulation,advance.network,0x510000,base+0x4784f9,
                    advance.resource,elapsed,true};
        train.trainState=advance.simulation+0xa0;train.network=advance.network;
        train.simulation=advance.simulation;train.returnAddress=base+0x477f05;
        train.controllerReference=advance.controllerReference;train.controller=advance.controller;
        train.controllerSimulation=advance.simulation;train.controllerNetwork=advance.network;
        train.controllerResource=train.advanceResource=train.simulationResource=advance.resource;
        train.ordinal=advance.ordinal;train.requestedElapsed=elapsed;
        train.elapsed=elapsed>166?int64_t{166}:elapsed;train.readComplete=true;
    }
};
template<class Inspect>void path(Context& context,const Fixture& fixture,Inspect&& inspect) {
    Scope controller(context,fixture.controller);CHECK(controller.reason()==Reason::QualifiedWindow);
    Scope advance(context,fixture.advance);CHECK(advance.reason()==Reason::QualifiedWindow);
    Scope simulation(context,fixture.simulation);CHECK(simulation.reason()==Reason::QualifiedWindow);
    Scope train(context,fixture.train);CHECK(train.reason()==Reason::QualifiedWindow);
    inspect(train);
}
void happyPath() {
    static_assert(!std::is_copy_constructible_v<Scope> && !std::is_move_constructible_v<Scope>);
    static_assert(!std::is_copy_constructible_v<Context>);
    static_assert(!Window::completeCoverageCertified);
    for(const auto elapsed:{int64_t{1},int64_t{50},int64_t{166},int64_t{167},INT64_MAX}) {
        Context context({base,true});Fixture fixture(elapsed);Token saved;
        path(context,fixture,[&](Scope& train) {
            CHECK(fixture.advance.resourceReference!=fixture.simulation.resourceReference);
            auto first=train.afterReturn(fixture.train);CHECK(first.qualified());
            auto second=train.afterReturn(fixture.train);CHECK(second.qualified());
            CHECK(first.token==second.token);CHECK(context.contains(first.token));
            CHECK(first.token.phaseElapsed==(elapsed>166?166:elapsed));
            CHECK(first.token.requestedElapsed==elapsed);
            CHECK(first.token.advanceResourceReference==fixture.advance.resourceReference);
            CHECK(first.token.simulationResourceReference==fixture.simulation.resourceReference);
            CHECK(first.token.descriptor==fixture.advance.descriptor);saved=first.token;
            CHECK(first.token.advanceReturn==fixture.advance.returnAddress-base);
            CHECK(first.token.simulationReturn==fixture.simulation.returnAddress-base);
            CHECK(first.token.trainPhaseReturn==fixture.train.returnAddress-base);
        });
        CHECK(context.empty());CHECK(!context.contains(saved));
        fixture.advance.returnAddress=base+0x349b64;
        path(context,fixture,[&](Scope& train){const auto next=train.afterReturn(fixture.train);
            CHECK(next.qualified());CHECK(next.token.sequence>saved.sequence);
            CHECK(!context.contains(saved));});
        CHECK(context.empty());
    }
    Context context({base,true});Fixture fixture;
    fixture.advance.resource={};fixture.advance.controllerResource={};fixture.simulation.resource={};
    fixture.train.controllerResource={};fixture.train.advanceResource={};fixture.train.simulationResource={};
    path(context,fixture,[&](Scope& train){CHECK(train.afterReturn(fixture.train).qualified());});
}
void missingAndForeign() {
    Fixture fixture;Context context({base,true});
    {Scope train(context,fixture.train);CHECK(train.reason()==Reason::MissingAncestor);
        CHECK(!train.afterReturn(fixture.train).qualified());}
    CHECK(context.empty());
    for(auto reason:{Reason::ReadIncomplete,Reason::InvalidAddress,Reason::ControllerChanged,
                     Reason::SimulationChanged,Reason::NetworkChanged,Reason::ResourceChanged,Reason::ForeignCallsite,
                     Reason::ElapsedChanged}) {
        auto modified=fixture.advance;
        switch(reason) {
        case Reason::ReadIncomplete:modified.readComplete=false;break;
        case Reason::InvalidAddress:modified.descriptor=UINTPTR_MAX-8;break;
        case Reason::ControllerChanged:modified.controllerReference+=8;break;
        case Reason::SimulationChanged:modified.controllerSimulation+=8;break;
        case Reason::NetworkChanged:modified.controllerNetwork+=8;break;
        case Reason::ResourceChanged:modified.controllerResource.value+=8;break;
        case Reason::ForeignCallsite:modified.returnAddress=base+0x4f3eae;break; // server caller
        default:modified.elapsed=0;break;
        }
        {Scope controller(context,fixture.controller);Scope rejected(context,modified);
            CHECK(rejected.reason()==reason);CHECK(!rejected.afterReturn(fixture.train).qualified());}
        CHECK(context.empty());
    }
    for(auto reason:{Reason::ReadIncomplete,Reason::InvalidAddress,Reason::SimulationChanged,
                     Reason::NetworkChanged,Reason::ResourceChanged,Reason::ForeignCallsite,
                     Reason::ElapsedChanged}) {
        auto modified=fixture.simulation;
        switch(reason) {
        case Reason::ReadIncomplete:modified.readComplete=false;break;
        case Reason::InvalidAddress:modified.resourceReference=3;break;
        case Reason::SimulationChanged:modified.simulation+=8;break;
        case Reason::NetworkChanged:modified.network+=8;break;
        case Reason::ResourceChanged:modified.resource.control+=8;break;
        case Reason::ForeignCallsite:++modified.returnAddress;break;
        default:++modified.elapsed;break;
        }
        {Scope controller(context,fixture.controller);Scope advance(context,fixture.advance);
            Scope rejected(context,modified);CHECK(rejected.reason()==reason);}
        CHECK(context.empty());
    }
    for(auto profile:{Profile{base,false},Profile{0,true},Profile{UINTPTR_MAX-8,true}}) {
        Context invalid(profile);Scope controller(invalid,fixture.controller);
        CHECK(controller.reason()==Reason::UnqualifiedProfile);
    }
    for(auto changed:{ControllerFrame{0,0x210000,true},ControllerFrame{0x200000,3,true},
                       ControllerFrame{0x200000,0x210000,false}}) {
        Context invalid({base,true});Scope controller(invalid,changed);
        CHECK(controller.reason()==(changed.readComplete?Reason::InvalidAddress:Reason::ReadIncomplete));
    }
}
void readbackChanges() {
    for(auto reason:{Reason::ReadIncomplete,Reason::InvalidAddress,Reason::ControllerChanged,
                     Reason::SimulationChanged,Reason::NetworkChanged,Reason::ResourceChanged,
                     Reason::DescriptorChanged,Reason::ElapsedChanged,Reason::ForeignCallsite}) {
        Context context({base,true});Fixture fixture;auto changed=fixture.train;
        switch(reason) {
        case Reason::ReadIncomplete:changed.readComplete=false;break;
        case Reason::InvalidAddress:changed.trainState=UINTPTR_MAX;break;
        case Reason::ControllerChanged:changed.controller+=8;break;
        case Reason::SimulationChanged:changed.controllerSimulation+=8;break;
        case Reason::NetworkChanged:changed.controllerNetwork+=8;break;
        case Reason::ResourceChanged:changed.advanceResource.value+=8;break;
        case Reason::DescriptorChanged:++changed.ordinal;break;
        case Reason::ElapsedChanged:++changed.elapsed;break;
        default:++changed.returnAddress;break;
        }
        path(context,fixture,[&](Scope& train) {
            CHECK(train.afterReturn(fixture.train).qualified());
            CHECK(train.afterReturn(changed).reason==reason);
            CHECK(!train.afterReturn(fixture.train).qualified()); // same Advance remains poisoned
        });
        CHECK(context.empty());
        path(context,fixture,[&](Scope& train){CHECK(train.afterReturn(fixture.train).qualified());});
    }
    Context context({base,true});Fixture fixture;
    path(context,fixture,[&](Scope& train) {
        auto changed=fixture.train;changed.simulationResource.control+=8;
        CHECK(train.afterReturn(changed).reason==Reason::ResourceChanged);
    });
    for(unsigned which=0;which<5;++which) {
        Context separate({base,true});
        path(separate,fixture,[&](Scope& train) {
            auto changed=fixture.train;
            switch(which) {
            case 0:changed.trainState+=8;break;
            case 1:changed.network+=8;break;
            case 2:changed.simulation+=8;changed.trainState+=8;break;
            case 3:changed.controllerResource.control+=8;break;
            default:++changed.requestedElapsed;break;
            }
            const auto result=train.afterReturn(changed);
            CHECK(!result.qualified());
            CHECK(!train.afterReturn(fixture.train).qualified());
        });
    }
}
void nestedAndUnwind() {
    Fixture fixture;Context context({base,true});
    {Scope controller(context,fixture.controller);
        {Scope advance(context,fixture.advance);Scope simulation(context,fixture.simulation);
            Scope train(context,fixture.train);
            {Scope nested(context,fixture.train);CHECK(nested.reason()==Reason::Reentrant);}
            CHECK(train.afterReturn(fixture.train).reason==Reason::Reentrant);
        }
        // Recovery only after the failed Advance unwinds; Controller entry alone
        // still provides no diagnostic window or native lock certificate.
        {Scope advance(context,fixture.advance);Scope simulation(context,fixture.simulation);
            Scope train(context,fixture.train);CHECK(train.afterReturn(fixture.train).qualified());}
    }
    CHECK(context.empty());
    path(context,fixture,[&](Scope& train) {
        {Scope nestedController(context,fixture.controller);CHECK(nestedController.reason()==Reason::Reentrant);}
        CHECK(train.afterReturn(fixture.train).reason==Reason::Reentrant);
    });
    CHECK(context.empty());
    {Scope controller(context,fixture.controller);Scope advance(context,fixture.advance);
        {Scope simulation(context,fixture.simulation);
            try{Scope train(context,fixture.train);throw 5;}catch(int){}
            Scope rejected(context,fixture.train);CHECK(rejected.reason()==Reason::ExceptionUnwind);
        }
    }
    CHECK(context.empty());
    try{path(context,fixture,[](Scope&){throw 7;});}catch(int){}
    CHECK(context.empty());
    path(context,fixture,[&](Scope& train){CHECK(train.afterReturn(fixture.train).qualified());});
    // Explicitly crossed lifetimes, with each scope still stored in fixed local
    // storage: no stale or resurrected token is allowed after parent destruction.
    alignas(Scope) std::array<std::byte,sizeof(Scope)> controllerStorage{},advanceStorage{};
    auto* controller=::new(controllerStorage.data())Scope(context,fixture.controller);
    auto* advance=::new(advanceStorage.data())Scope(context,fixture.advance);
    controller->~Scope();
    {Scope simulation(context,fixture.simulation);CHECK(simulation.reason()==Reason::ScopeOrder);}
    advance->~Scope();CHECK(context.empty());
}
void boundedNoHeap() {
    Context context({base,true});Fixture fixture;allocations=0;watchAllocations=true;
    for(size_t i=0;i<1000;++i)
        path(context,fixture,[&](Scope& train){CHECK(train.afterReturn(fixture.train).qualified());});
    watchAllocations=false;CHECK(allocations==0);CHECK(context.empty());
}
void refreshProfile() {
    Context context({base,true});Fixture fixture;Token saved;
    path(context,fixture,[&](Scope& train) {
        saved=train.afterReturn(fixture.train).token;
        CHECK(!context.profileWhenEmpty({base+0x100000,false}));
        CHECK(train.afterReturn(fixture.train).qualified());
    });
    CHECK(context.empty());CHECK(context.profileWhenEmpty({base,true}));
    path(context,fixture,[&](Scope& train) {
        const auto current=train.afterReturn(fixture.train);CHECK(current.qualified());
        CHECK(current.token.sequence>saved.sequence);CHECK(!context.contains(saved));
    });
    {Scope controller(context,fixture.controller);
        CHECK(!context.profileWhenEmpty({0,false}));
        Scope advance(context,fixture.advance);Scope simulation(context,fixture.simulation);
        Scope train(context,fixture.train);CHECK(train.afterReturn(fixture.train).qualified());}
    CHECK(context.profileWhenEmpty({0,false}));
    {Scope rejected(context,fixture.controller);CHECK(rejected.reason()==Reason::UnqualifiedProfile);
        CHECK(!context.profileWhenEmpty({base,true}));}
    CHECK(context.empty());CHECK(context.profileWhenEmpty({base,true}));
    path(context,fixture,[&](Scope& train) {
        const auto current=train.afterReturn(fixture.train);CHECK(current.qualified());
        CHECK(current.token.sequence>saved.sequence);CHECK(!context.contains(saved));
    });
}
void threadLocalContexts() {
    // No shared Context is handed across threads. The evaluator needs no lock.
    bool okay=false;
    std::thread peer([&]{thread_local Context context({base,true});Fixture fixture;
        Scope controller(context,fixture.controller);Scope advance(context,fixture.advance);
        Scope simulation(context,fixture.simulation);Scope train(context,fixture.train);
        okay=train.afterReturn(fixture.train).qualified();});
    Fixture fixture;thread_local Context context({base,true});
    {Scope controller(context,fixture.controller);Scope advance(context,fixture.advance);
        Scope simulation(context,fixture.simulation);Scope train(context,fixture.train);
        peer.join();CHECK(okay);CHECK(train.afterReturn(fixture.train).qualified());}
    CHECK(context.empty());
    Context first({base,true}),second({base,true});
    path(first,fixture,[&](Scope& outer) {
        const auto owned=outer.afterReturn(fixture.train);CHECK(owned.qualified());
        path(second,fixture,[&](Scope& inner) {
            const auto peerToken=inner.afterReturn(fixture.train);CHECK(peerToken.qualified());
            CHECK(owned.token.sequence==peerToken.token.sequence);
            CHECK(!first.contains(peerToken.token));CHECK(!second.contains(owned.token));
        });
        CHECK(first.contains(owned.token));
    });
}
}
int main() {
    try {
        happyPath();missingAndForeign();readbackChanges();nestedAndUnwind();
        boundedNoHeap();refreshProfile();threadLocalContexts();
        std::cout<<"native_phase_ownership: "<<checks<<" checks passed (diagnostic only)\n";
        return 0;
    }catch(const std::exception& error){watchAllocations=false;std::cerr<<error.what()<<'\n';return 1;}
}
