#pragma once
#include <nimby/detail/signal_settings_presenter.hpp>
#include <memory>

namespace nimby::runtime {
// Owned by the resident SDK UI bridge. It copies mod declarations and never
// keeps callbacks, string views or executable addresses belonging to a mod DLL.
class SignalUiHost {
public:
    using Token=uint64_t;
    Token add(const SignalSettingsPanel& panel) {
        auto store=std::make_shared<SignalSettingsStore>();store->configure(panel);
        if(panel.id.empty())throw std::invalid_argument("Missing signal panel");
        std::lock_guard lock(mutex_);
        if(owners_.size()>=16)throw std::length_error("Too many signal panels");
        for(const auto& [token,owner]:owners_){(void)token;
            if(owner->panel().id==panel.id)throw std::invalid_argument("Signal panel already registered");
        }
        const auto token=++generation_;owners_.emplace(token,std::move(store));return token;
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
        found->second->endSession();owners_.erase(found);return true;
    }
    struct Frame {
        uint64_t invocation=0;
        struct Panel {Token owner;std::shared_ptr<SignalSettingsStore> store;SignalSettingsStore::Frame controls;};
        std::vector<Panel> panels;
    };
    Frame prepare(uint64_t invocation,uint64_t signal) const {
        Frame frame{invocation,{}};
        if(!invocation)return frame;
        std::lock_guard lock(mutex_);
        for(const auto& [token,owner]:owners_){
            auto controls=owner->selectFrame(signal);
            if(controls)frame.panels.push_back({token,owner,std::move(*controls)});
        }
        return frame;
    }
    template<class Ui> static void layout(const Frame& frame,Ui& ui) {
        for(const auto& panel:frame.panels)detail::drawSignalSettings(*panel.store,panel.controls,ui,false);
    }
    template<class Ui> static size_t interactive(const Frame& frame,uint64_t signal,Ui& ui) {
        size_t failedWrites=0;
        for(const auto& panel:frame.panels)
            failedWrites+=detail::drawSignalSettings(*panel.store,panel.controls,ui,signal==panel.controls.editor.signal);
        return failedWrites;
    }
private:
    mutable std::mutex mutex_;
    Token generation_=0;
    std::map<Token,std::shared_ptr<SignalSettingsStore>> owners_;
};

// One instance per native UI thread. Preserve the complete panel list between
// passes, including a panel whose mod stops while the editor is open.
class SignalUiPresentation {
public:
    template<class Ui> bool layout(const SignalUiHost& host,uint64_t invocation,
                                   uint64_t signal,Ui& ui) {
        pending_.reset();
        auto frame=host.prepare(invocation,signal);
        if(!frame.invocation||frame.panels.empty())return false;
        pending_=std::move(frame);
        SignalUiHost::layout(*pending_,ui);
        return true;
    }
    template<class Ui> bool interactive(uint64_t invocation,uint64_t signal,Ui& ui) {
        if(!pending_||pending_->invocation!=invocation)return false;
        auto frame=std::move(*pending_);
        pending_.reset();
        failedWrites_=SignalUiHost::interactive(frame,signal,ui);
        return true;
    }
    size_t takeFailedWrites(){const auto count=failedWrites_;failedWrites_=0;return count;}
private:
    size_t failedWrites_=0;
    std::optional<SignalUiHost::Frame> pending_;
};
}
