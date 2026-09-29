#pragma once
#include <nimby/detail/diagnostics.hpp>
#include <nimby/signal_settings_store.hpp>

namespace nimby::detail {
// Called by the native UI bridge, on its UI thread. A single owned Frame is
// retained for the corresponding layout and interactive passes. Never rebuild
// it between these passes: each control consumes a position in native layout.
// UI supplies checkbox(label, description, uint32_t&) synchronously and must
// not retain the value pointer. Native checkboxes use a 32-bit value, not bool*.
template<class Ui>
size_t drawSignalSettings(SignalSettingsStore& store,
                        const SignalSettingsStore::Frame& frame,
                        Ui& ui,bool interactive) {
    size_t failedWrites=0;
    const auto drawNumber=[&](const SignalSettingsStore::NumberControl& number){
        if constexpr(requires(NumberInputDraft& draft){ui.numberField("",draft,0,0,0,true);}){
            const bool enabled=frame.available&&store.accepts(frame.editor);
            const auto edit=ui.numberField(number.field.label.c_str(),*number.draft,number.value,0,int(number.field.maximum),enabled);
            if(interactive&&enabled&&edit.value&&(edit.changed||(number.draft->modified&&*edit.value!=number.value)))
                try{store.setNumber(frame.editor,number.field.name,*edit.value);}
                catch(...){nimby::detail::diagnostics::exception("mods",__func__);++failedWrites;}
        }
    };
    for(const auto& control:frame.controls){
        uint32_t value=control.value?1u:0u;
        ui.checkbox(control.checkbox.label.c_str(),control.checkbox.description.c_str(),value);
        if(interactive && (value!=0)!=control.value){
            // Session/selection validation happens again when accepting a click.
            // A discarded click must not interrupt the remaining layout items.
            try {store.setBoolean(frame.editor,control.checkbox.name,value!=0);}
            catch(...) { nimby::detail::diagnostics::exception("mods", __func__); ++failedWrites;} // Still consume the remaining native layout slots.
        }
        // Conditional fields belong immediately beneath their enabling checkbox.
        // Use the captured frame in both passes, even when that box was just toggled.
        for(const auto& number:frame.numbers)
            if(number.field.visibleWhen==control.checkbox.name)drawNumber(number);
    }
    for(const auto& number:frame.numbers)
        if(number.field.visibleWhen.empty()||std::none_of(frame.controls.begin(),frame.controls.end(),
            [&](const auto& control){return control.checkbox.name==number.field.visibleWhen;}))drawNumber(number);
    return failedWrites;
}

// One instance per UI thread. Layout replaces an unfinished previous cycle
// (a hidden window may never have an interactive pass). The capture address
// is only an invocation key, never dereferenced or used as a save identity.
class SignalSettingsPresentation {
public:
    template<class Ui>
    bool layout(SignalSettingsStore& store,uint64_t invocation,uint64_t session,
                uint64_t signal,Ui& ui) {
        pending_.reset();invocation_=0;
        if(!invocation)return false;
        auto frame=store.frame(store.selectSignal(session,signal));
        if(!frame)return false;
        pending_=std::move(frame);invocation_=invocation;
        drawSignalSettings(store,*pending_,ui,false);
        return true;
    }

    template<class Ui>
    bool interactive(SignalSettingsStore& store,uint64_t invocation,uint64_t session,
                     uint64_t signal,Ui& ui) {
        if(!pending_||invocation!=invocation_)return false;
        auto frame=std::move(*pending_);
        pending_.reset();invocation_=0;
        // Consume every reserved layout item even if selection/session changed.
        // Only matching identities may submit edits; the store revalidates too.
        const bool allowEdits=session==frame.editor.session&&signal==frame.editor.signal;
        drawSignalSettings(store,frame,ui,allowEdits);
        return true;
    }
private:
    uint64_t invocation_=0;
    std::optional<SignalSettingsStore::Frame> pending_;
};
}
