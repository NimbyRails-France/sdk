#pragma once
#include <nimby/detail/signal_settings_presenter.hpp>
#include <memory>
#include <nimby/detail/translations.hpp>
#include "runtime/signal_actions.h"
#include <nimby/detail/number_input_draft.hpp>

namespace nimby::runtime {
// Owned by the resident SDK UI bridge. It copies mod declarations and never
// keeps callbacks, string views or executable addresses belonging to a mod DLL.
class SignalUiHost {
public:
    using Token=uint64_t;
    std::shared_ptr<SignalActions> actions=std::make_shared<SignalActions>();
    Token add(const SignalSettingsPanel& panel) {
        auto store=std::make_shared<SignalSettingsStore>();store->configure(panel);
        if(panel.id.empty())throw std::invalid_argument("Missing signal panel");
        std::lock_guard lock(mutex_);
        if(owners_.size()>=16)throw std::length_error("Too many signal panels");
        for(const auto& [token,owner]:owners_){(void)token;
            if(owner->panel().id==panel.id)throw std::invalid_argument("Signal panel already registered");
        }
        const auto token=++generation_;actions->addPanel(token,store);
        owners_.emplace(token,std::move(store));return token;
    }
    std::shared_ptr<SignalSettingsStore> store(Token token) const {
        std::lock_guard lock(mutex_);const auto found=owners_.find(token);
        return found==owners_.end()?nullptr:found->second;
    }
    bool remove(Token token) {
        std::lock_guard lock(mutex_);const auto found=owners_.find(token);
        if(found==owners_.end())return false;
        // A frame already being drawn retains owned labels, but its editor
        // token is invalidated before the owner disappears from the registry.
        found->second->endSession();catalogs_.erase({false,token});actions->removePanel(token);owners_.erase(found);return true;
    }
    bool translations(bool provider,Token token,std::shared_ptr<const detail::Translations> catalog) {
        std::lock_guard lock(mutex_);
        if(provider?!actions->hasProvider(token):!owners_.contains(token))return false;
        catalogs_[{provider,token}]=std::move(catalog);return true;
    }
    bool removeProvider(Token token) {
        std::lock_guard lock(mutex_);catalogs_.erase({true,token});return actions->removeProvider(token);
    }
    struct Frame {
        uint64_t invocation=0;
        struct Panel {Token owner;std::shared_ptr<SignalSettingsStore> store;SignalSettingsStore::Frame controls;};
        std::vector<Panel> panels;
        std::shared_ptr<SignalActions> registry;
        std::vector<SignalActions::Selection> actions;
        std::vector<std::shared_ptr<detail::NumberInputDraft>> drafts;
    };
    Frame prepare(uint64_t invocation,uint64_t signal,std::string_view language={}) const {
        Frame frame{invocation,{},{},{},{}};
        frame.registry=actions;
        if(!invocation)return frame;
        std::lock_guard lock(mutex_);
        for(const auto& [token,owner]:owners_){
            auto controls=owner->selectFrame(signal);
            if(controls){
                const auto translate=[&](bool provider,Token owner,std::string& text){
                    const auto found=catalogs_.find({provider,owner});
                    text=detail::Translations::resolve(found==catalogs_.end()?nullptr:found->second.get(),text,language);
                };
                translate(false,token,controls->title);
                for(auto& field:controls->controls){
                    translate(false,token,field.checkbox.label);translate(false,token,field.checkbox.description);
                }
                auto buttons=actions->prepare(token,controls->editor);
                for(auto& button:buttons){
                    const bool provider=button.revision!=0;const auto owner=provider?button.provider:token;
                    translate(provider,owner,button.action.label);
                    if(button.input)translate(provider,owner,button.input->label);
                }
                frame.actions.insert(frame.actions.end(),buttons.begin(),buttons.end());
                frame.panels.push_back({token,owner,std::move(*controls)});
            }
        }
        return frame;
    }
    template<class Ui> static void layout(const Frame& frame,Ui& ui) {
        for(const auto& panel:frame.panels){
            if constexpr(requires{ui.heading("");})ui.heading(panel.controls.title.c_str());
            detail::drawSignalSettings(*panel.store,panel.controls,ui,false);
        }
        if constexpr(requires{ui.separator();})if(!frame.actions.empty())ui.separator();
        if constexpr(requires{ui.button("",false);})
            for(size_t i=0;i<frame.actions.size();++i){const auto& a=frame.actions[i];
                if constexpr(requires(detail::NumberInputDraft& draft){ui.numberField("",draft,0,0,0,true);})
                    if(a.input&&i<frame.drafts.size()&&frame.drafts[i]){const auto& n=*a.input;
                        ui.numberField(n.label.c_str(),*frame.drafts[i],n.value,n.minimum,n.maximum,a.enabled);continue;}
                if constexpr(requires{ui.numberInput("",0,0,0,true);})if(a.input){const auto& n=*a.input;
                    ui.numberInput(n.label.c_str(),n.value,n.minimum,n.maximum,a.enabled);continue;}
                if constexpr(requires{ui.message("");})if(a.action.id.empty()){ui.message(a.action.label.c_str());continue;}
                ui.button(a.action.label.c_str(),true);
            }
    }
    template<class Ui> static size_t interactive(const Frame& frame,uint64_t signal,Ui& ui) {
        size_t failedWrites=0;
        for(const auto& panel:frame.panels){
            if constexpr(requires{ui.heading("");})ui.heading(panel.controls.title.c_str());
            failedWrites+=detail::drawSignalSettings(*panel.store,panel.controls,ui,panel.controls.available&&signal==panel.controls.editor.signal);
        }
        if constexpr(requires{ui.separator();})if(!frame.actions.empty())ui.separator();
        bool invalidDraft=false;
        if constexpr(requires{ui.button("",false);})for(size_t i=0;i<frame.actions.size();++i){const auto& a=frame.actions[i];
            const bool enabled=signal==a.editor.signal&&frame.registry->available(a)&&!invalidDraft;
            if constexpr(requires(detail::NumberInputDraft& draft){ui.numberField("",draft,0,0,0,true);})
                if(a.input&&i<frame.drafts.size()&&frame.drafts[i]){const auto& n=*a.input;
                    const auto edit=ui.numberField(n.label.c_str(),*frame.drafts[i],n.value,n.minimum,n.maximum,enabled);
                    if(enabled&&!edit.value)frame.registry->beginNumberEdit(a);
                    // If a worker publication won the revision race, retry
                    // the still-unacknowledged valid draft on the next frame.
                    // Queue coalescing retains just its most recent value.
                    if(enabled&&edit.value&&(edit.changed||(frame.drafts[i]->modified&&*edit.value!=n.value)))
                        frame.registry->editNumber(a,*edit.value);
                    invalidDraft|=!edit.value;
                    continue;
                }
            if constexpr(requires{ui.numberInput("",0,0,0,true);})if(a.input){const auto& n=*a.input;
                // Use the frozen control shape even if the provider expired
                // between passes. An edit is accepted only by the registry.
                const auto value=ui.numberInput(n.label.c_str(),n.value,n.minimum,n.maximum,a.enabled);
                if(enabled&&value!=n.value)frame.registry->editNumber(a,value);
                continue;
            }
            if constexpr(requires{ui.message("");})if(a.action.id.empty()){ui.message(a.action.label.c_str());continue;}
            if(ui.button(a.action.label.c_str(),enabled)&&enabled)frame.registry->click(a);
        }
        return failedWrites;
    }
private:
    mutable std::mutex mutex_;
    Token generation_=0;
    std::map<Token,std::shared_ptr<SignalSettingsStore>> owners_;
    std::map<std::pair<bool,Token>,std::shared_ptr<const detail::Translations>> catalogs_;
};

// One instance per native UI thread. Preserve the complete panel list between
// passes, including a panel whose mod stops while the editor is open.
class SignalUiPresentation {
public:
    template<class Ui> bool layout(const SignalUiHost& host,uint64_t invocation,
                                   uint64_t signal,Ui& ui,std::string_view language={}) {
        pending_.reset();
        auto frame=host.prepare(invocation,signal,language);
        if(!frame.invocation||frame.panels.empty())return false;
        pending_=std::move(frame);
        std::map<std::string,std::shared_ptr<detail::NumberInputDraft>> retained;
        pending_->drafts.resize(pending_->actions.size());
        for(size_t i=0;i<pending_->actions.size();++i){const auto& a=pending_->actions[i];if(!a.input)continue;
            // Revision and worker epoch intentionally excluded: recovery must
            // not replace the user's empty/partially typed text or selection.
            const auto key=std::to_string(a.panel)+":"+std::to_string(a.provider)+":"+
                std::to_string(a.editor.session)+":"+std::to_string(a.editor.signal)+":"+a.action.id;
            auto old=drafts_.find(key);auto draft=old==drafts_.end()?std::make_shared<detail::NumberInputDraft>():old->second;
            draft->synchronize(a.input->value);retained.emplace(key,draft);pending_->drafts[i]=std::move(draft);
        }
        drafts_=std::move(retained); // Only visible fields are retained; no cache grows with the map.
        drawFrame(*pending_,ui,[&](auto& content){SignalUiHost::layout(*pending_,content);});
        return true;
    }
    template<class Ui> bool interactive(uint64_t invocation,uint64_t signal,Ui& ui) {
        if(!pending_||pending_->invocation!=invocation)return false;
        auto frame=std::move(*pending_);
        pending_.reset();
        drawFrame(frame,ui,[&](auto& content){failedWrites_=SignalUiHost::interactive(frame,signal,content);});
        return true;
    }
    size_t takeFailedWrites(){const auto count=failedWrites_;failedWrites_=0;return count;}
private:
    template<class Ui,class Draw> static void drawFrame(const SignalUiHost::Frame& frame,Ui& ui,Draw draw){
        // Freeze the row budget with the frame so a provider disappearing
        // between layout and interaction cannot change the parent layout.
        size_t rows=frame.actions.size();
        for(const auto& panel:frame.panels)rows+=panel.controls.controls.size();
        float extra=0;
        if constexpr(requires{ui.heading("");})extra+=32.f*float(frame.panels.size());
        for(const auto& panel:frame.panels)for(const auto& control:panel.controls.controls)
            if(!control.checkbox.description.empty())extra+=56.f;
        if constexpr(requires{ui.message("");})for(const auto& a:frame.actions)if(a.action.id.empty())extra+=72.f;
        if constexpr(requires(detail::NumberInputDraft& draft){ui.numberField("",draft,0,0,0,true);})
            for(const auto& a:frame.actions)if(a.input)extra+=32.f;
        const float height=std::min(320.f,28.f*float(rows)+48.f+extra);
        // Native edit focus and scroll offsets belong to this signal/session,
        // not to whichever signal happens to occupy the editor next.
        const auto& editor=frame.panels.front().controls.editor;
        std::string key="##nrf_signal_extensions_"+std::to_string(editor.signal)+"_"+std::to_string(editor.session);
        for(const auto& a:frame.actions)if(a.input)key+="_"+std::to_string(a.provider)+"_"+a.action.id;
        if constexpr(requires{ui.textWidth("");ui.scroll("",height,0.f,draw);}){
            float width=0;
            for(const auto& panel:frame.panels)for(const auto& c:panel.controls.controls)width=std::max(width,ui.textWidth(c.checkbox.label.c_str()));
            // Status text wraps inside its own column. It must never force all
            // controls wider than the viewport and hide the numeric editor.
            for(const auto& a:frame.actions)if(!a.action.id.empty())width=std::max(width,ui.textWidth(a.action.label.c_str()));
            ui.scroll(key.c_str(),height,width,draw);
        }
        else if constexpr(requires{ui.scroll("",height,draw);})ui.scroll(key.c_str(),height,draw);
        else draw(ui);
    }
    size_t failedWrites_=0;
    std::optional<SignalUiHost::Frame> pending_;
    std::map<std::string,std::shared_ptr<detail::NumberInputDraft>> drafts_;
};
}
