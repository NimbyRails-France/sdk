#include "platform/windows/runtime/texture_dispatch.h"
#include <cassert>

int main() {
    using namespace nimby::texture_bridge;
    Shared shared;
    Table table;
    shared.pending=1;shared.callbacks=17;
    shared.operation=1;shared.request_database=10;shared.request_simulation=20;
    shared.request_signal=123;shared.request_hash=456;shared.request_expiry=1000;
    shared.request_index=2;shared.request_alternate_index=3;shared.request_half_period_ms=500;
    assert(dispatch(shared,table,100)==NIMBY_OK);
    assert(table.count()==1 && frame_index(table.entries[0],500)==3);
    assert(shared.expected_database==10 && shared.expected_simulation==20);
    assert(shared.signal==123 && shared.expires==1000);
    shared.operation=3;
    assert(dispatch(shared,table,100)==NIMBY_OK);
    assert(shared.active==1 && shared.active_count==1 && shared.result_index==2 && shared.result_expiry==1000);
    shared.request_database=11;
    assert(dispatch(shared,table,100)==NIMBY_OK && shared.active==0 && shared.active_count==0);
    // Dispatch owns only command state, never IPC publication or telemetry.
    assert(shared.pending==1 && shared.callbacks==17);
}
