#pragma once
#include "engine/signal_ui.h"
#include "runtime/signal_ui_host.h"
#include <nimby/detail/signal_settings_presenter.hpp>

namespace nimby::runtime {
// Resident bridge path: declarations and pending frames belong to the SDK,
// so drawing never calls back into a mod that may already have stopped.
inline bool draw_signal_settings_panels(engine::ReadMemory read,void* context,
        uint64_t module,uint64_t capture,uint64_t declaration,
        const SignalUiHost& host,SignalUiPresentation& presentation) {
    auto ui=engine::SignalUi::bind(read,context,module,declaration);
    if(!ui)return false;
    const auto signal=engine::SignalUi::editorSignal(read,context,capture);
    if(ui->pass()==engine::SignalUi::Pass::Layout)
        return presentation.layout(host,capture,signal.value_or(0),*ui);
    return presentation.interactive(capture,signal.value_or(0),*ui);
}

// Called AFTER the original signal-editor body returns, while its caller's
// outer layout group remains open. No hook is installed by this function.
// The owning bridge must validate the binary, provide the current store session
// token and keep one presentation object per UI thread. It must drain native
// callbacks before unloading either this code or the mod's owned store.
inline bool draw_signal_settings_panel(engine::ReadMemory read,void* context,
        uint64_t module,uint64_t capture,uint64_t declaration,uint64_t session,
        SignalSettingsStore& store,detail::SignalSettingsPresentation& presentation) {
    auto ui=engine::SignalUi::bind(read,context,module,declaration);
    if(!ui)return false;
    const auto signal=engine::SignalUi::editorSignal(read,context,capture);
    if(ui->pass()==engine::SignalUi::Pass::Layout)
        return presentation.layout(store,capture,session,signal.value_or(0),*ui);
    // Missing identity must still consume an existing frame's layout; it is
    // passed as zero so that no edits can be accepted for that invocation.
    return presentation.interactive(store,capture,session,signal.value_or(0),*ui);
}
}
