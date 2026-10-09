#include "runtime/signal_ui_endpoint.h"
#include "runtime/signal_preview_diagnostics.h"
#include <cassert>
#include <cstring>
#include <iostream>

using namespace nimby::runtime;
using Reason=SignalActions::PreviewReason;
using Status=SignalActions::PreviewStatus;
using namespace std::chrono_literals;

namespace {
void failureEvidenceAndRecovery(){
    constexpr uint64_t signal=0x8000000000021,track=0x1000000000021;
    SignalUiEndpoint endpoint;
    const auto panel=endpoint.host.add({"signals","Signals","atlas",{}});
    const auto store=endpoint.host.store(panel);const auto session=store->beginSession("world");
    const nimby::SignalSettingsStore::Signal row{signal,"atlas"};
    assert(store->observeSignals(session,{&row,1}));
    const auto actions=endpoint.host.actions;
    const auto provider=actions->addProvider("placement",{"repeat"});
    std::vector<SignalActions::Action> definitions;
    definitions.emplace_back("repeat","Repeat","placement","repeat");
    assert(actions->configure(panel,std::move(definitions)));
    assert(actions->panelContext(panel,{"world",7}));
    assert(actions->observeProvider(provider,{"world",7}));
    NimbyUiSignalPreviewV1 wire{};wire.size=sizeof wire;wire.version=1;
    wire.panel=panel;wire.signal=signal;wire.count=1;wire.positions[0]={track,.5,1,0};
    std::strcpy(wire.origin,"repeat");std::strcpy(wire.service,"repeat");
    SignalActions::PreviewResult detail{};
    uint64_t publication{};
    auto status=endpoint.publishPreview(provider,&wire,&publication,&detail);
    assert(status==NIMBY_OK&&publication&&detail.publication==publication);
    SignalPreviewDiagnostics diagnostics;
    unsigned emitted=0;
    diagnostics.report(provider,&wire,status,detail,[&](const char*){++emitted;});
    assert(emitted==0); // Healthy renewals never enter the log limiter or sink.
    assert(actions->panelContext(panel,{}));
    status=endpoint.publishPreview(provider,&wire,&publication,&detail);
    assert(status==NIMBY_INVALID_HANDLE&&!publication&&detail.diagnostic.reason==Reason::PanelInactive);
    std::string logged;
    diagnostics.report(provider,&wire,status,detail,[&](const char* line){
        ++emitted;logged=line;
        // Re-enter both registries from the sink. The validation snapshot and
        // limiter must have released their locks before writing a diagnostic.
        assert(store->signalState(signal,true,"world")==nimby::SignalSettingsStore::SignalState::Known);
        assert(actions->panelContext(panel,{"world",7}));
        SignalActions::PreviewResult fresh{};
        assert(endpoint.publishPreview(provider,&wire,nullptr,&fresh)==NIMBY_OK);
        diagnostics.report(provider+100,&wire,NIMBY_INVALID_ARGUMENT,{},[](const char*){});
    });
    assert(emitted==1&&logged.find("status=9 reason=panel_inactive")!=std::string::npos);
    assert(logged.find("mod=placement")!=std::string::npos&&logged.find("count=1")!=std::string::npos);
    assert(logged.find("generation=7 providerGeneration=7 panelGeneration=7")!=std::string::npos);
    assert(actions->preview(signal)); // Diagnostics did not revoke the recovery.
    status=endpoint.publishPreview(provider,nullptr,&publication,&detail);
    assert(status==NIMBY_INVALID_ARGUMENT&&!publication&&detail.diagnostic.reason==Reason::None);
    diagnostics.report(provider,nullptr,status,detail,[&](const char* line){logged=line;});
    assert(logged.find("reason=invalid_argument")!=std::string::npos&&logged.find("count=0")!=std::string::npos);
}

void repeatedFailuresAreBounded(){
    SignalPreviewDiagnostics diagnostics;
    const auto now=SignalActions::Clock::now();
    NimbyUiSignalPreviewV1 wire{};wire.size=sizeof wire;wire.version=1;wire.panel=1;wire.signal=2;wire.count=6;
    SignalActions::PreviewResult detail{Status::Invalid};detail.diagnostic.reason=Reason::PanelInactive;
    unsigned emitted=0;std::string line;
    const auto sink=[&](const char* text){++emitted;line=text;};
    diagnostics.report(1,&wire,NIMBY_INVALID_HANDLE,detail,sink,now);
    for(unsigned i=0;i<1000;++i){
        // Changing counters cannot defeat suppression of the same failure.
        detail.diagnostic.revision=i;
        diagnostics.report(1,&wire,NIMBY_INVALID_HANDLE,detail,sink,now+1ms);
    }
    assert(emitted==1);
    detail.diagnostic.reason=Reason::ProviderEpochChanged;
    diagnostics.report(1,&wire,NIMBY_INVALID_HANDLE,detail,sink,now+2ms);
    assert(emitted==2&&line.find("suppressed=1000")!=std::string::npos);
    detail.diagnostic.reason=Reason::StoreBusy;
    diagnostics.report(1,&wire,NIMBY_RESOURCE_LIMIT,detail,sink,now+3ms);
    assert(emitted==3&&line.find("reason=store_busy")!=std::string::npos);
    detail.diagnostic.reason=Reason::SignalMissing;
    diagnostics.report(1,&wire,NIMBY_INVALID_HANDLE,detail,sink,now+4ms);
    for(unsigned i=0;i<1000;++i){wire.signal=i;diagnostics.report(1,&wire,NIMBY_INVALID_HANDLE,detail,sink,now+5ms);}
    assert(emitted==4); // A changing source/reason cannot bypass the burst cap.
    diagnostics.report(2,&wire,NIMBY_INVALID_HANDLE,detail,sink,now+6ms);
    assert(emitted==5); // One failing provider does not suppress another one.
    diagnostics.report(1,&wire,NIMBY_INVALID_HANDLE,detail,sink,now+5s);
    assert(emitted==6&&line.find("suppressed=1000")!=std::string::npos);
}

void malformedTextIsBounded(){
    SignalPreviewDiagnostics diagnostics;
    NimbyUiSignalPreviewV1 wire{};wire.size=sizeof wire;wire.version=1;
    std::memset(wire.origin,'o',sizeof wire.origin);std::memset(wire.service,'s',sizeof wire.service);
    std::string line;
    diagnostics.report(1,&wire,NIMBY_INVALID_ARGUMENT,{},[&](const char* text){line=text;});
    assert(line.size()<1024&&line.find("reason=invalid_argument")!=std::string::npos);
    assert(line.find("origin="+std::string(128,'o')+" service=")!=std::string::npos);
    assert(line.find("suppressed=0")!=std::string::npos);
}
void providerChurnCannotRestartLimits(){
    SignalPreviewDiagnostics diagnostics;
    const auto now=SignalActions::Clock::now();
    for(uint64_t provider=1;provider<=SignalPreviewDiagnostics::capacity;++provider)
        assert(diagnostics.admit(provider,NIMBY_INVALID_HANDLE,Reason::PanelInactive,1,2,now).emit);
    // A full registry of failing mods keeps independent windows. A cycling or
    // removed/readded token cannot bypass those windows by forcing eviction.
    for(uint64_t provider=1;provider<=SignalPreviewDiagnostics::capacity+100;++provider)
        assert(!diagnostics.admit(provider,NIMBY_INVALID_HANDLE,Reason::PanelInactive,1,2,now+1ms).emit);
    assert(diagnostics.admit(SignalPreviewDiagnostics::capacity+1,NIMBY_INVALID_HANDLE,Reason::PanelInactive,1,2,now+5s).emit);
}
}

int main(){
    failureEvidenceAndRecovery();repeatedFailuresAreBounded();malformedTextIsBounded();providerChurnCannotRestartLimits();
    std::cout<<"PASS preview refusal evidence, reentrant logging, bounded repeats and malformed requests\n";
}
