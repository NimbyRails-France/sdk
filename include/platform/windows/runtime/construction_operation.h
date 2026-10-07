#pragma once
#include "platform/windows/runtime/bridge_request.h"
#include <nimby/detail/construction.h>

namespace nimby::construction_bridge {
struct Identity {uint64_t owner{},stamp{},sequence{};bool operator==(const Identity&) const=default;};
struct Operation {
    uint64_t token{},root{},simulation{},editor{},context{},preparedRevision{},command{},history{};
    Identity identity{};
    NimbyConstructionRequest request{};
    NimbyConstructionResult result{};
    platform::windows::BridgeRequestLease lease{};
    bool waiting{},undo{},executed{},authorized{},singleCommand{};
};

// Evidence collected on the editor thread before changing the current token.
// Invalid preparation must not discard another operation's result/undo handle.
struct Preparation {
    uint64_t root{},simulation{},editor{},context{},revision{};
    bool sourceValid{};
};

// This is the production transition, also exercised without game hooks in tests.
// A lease reserves an unused READY ticket, not the native history indefinitely.
// Expiry permits a new preparation; it does not execute, undo or proactively
// invalidate the old ticket. The next valid preparation replaces that ticket.
// An enqueued command is never replaced, even after expiry or requester death:
// its exact result/history identity must first be acknowledged by the editor.
template<class Alive>
uint32_t prepareOperation(Operation& operation,uint64_t& nextToken,const Preparation& next,
                          const NimbyConstructionRequest& request,
                          const platform::windows::BridgeRequestLease& lease,
                          uint64_t now,Alive&& requesterAlive) {
    if(operation.waiting)return 8;
    if(!next.root||!next.simulation||!next.editor||!next.context)return 4;
    if(!next.sourceValid)return 5;
    if(now>=lease.expires)return 1;
    const bool sameSession=operation.root==next.root&&operation.simulation==next.simulation&&
        operation.preparedRevision==next.revision;
    if(operation.token&&operation.result.state==NIMBY_CONSTRUCTION_READY&&sameSession&&
       now<operation.lease.expires&&
       !platform::windows::sameBridgeRequester(operation.lease,lease)&&requesterAlive(operation.lease))return 8;
    if(!nextToken)return 8; // Never recycle zero or an earlier token after wrap.
    operation={};operation.token=nextToken++;
    operation.root=next.root;operation.simulation=next.simulation;
    operation.editor=next.editor;operation.context=next.context;operation.preparedRevision=next.revision;
    operation.request=request;operation.lease=lease;
    operation.result={sizeof(NimbyConstructionResult),1,NIMBY_CONSTRUCTION_READY,0,operation.token,0,0,{}};
    return 0;
}
}
