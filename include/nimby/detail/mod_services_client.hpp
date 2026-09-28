#pragma once
#include <nimby/detail/signal_ui_bridge.h>
#include <nimby/detail/native_library.hpp>
#include <nimby/detail/platform/host.hpp>
#include <nimby/detail/observation_values.hpp>
#include <cstring>
#include <span>

namespace nimby::detail {
// Used only by one SDK observation worker. Stop joins that worker before close.
// Optional exports keep old UI bridges usable by ordinary signal-only mods.
class ModServicesClient {
public:
    bool connect(std::string_view id,std::span<const std::string> services,std::string_view translations={}) {
        if(module_)return true;
        if(id.empty()||services.size()>32)return false;
        auto module=native::existing(platform::signalUiLibrary);if(!module)return false;
        const auto add=resolve<NimbyUiProviderAddV1>(module,"NimbyUi_ProviderAddV1");
        auto remove=resolve<NimbyUiProviderRemoveV1>(module,"NimbyUi_ProviderRemoveV1");
        auto observe=resolve<NimbyUiProviderObserveV1>(module,"NimbyUi_ProviderObserveV1");
        auto suspend=resolve<NimbyUiProviderSuspendV1>(module,"NimbyUi_ProviderSuspendV1");
        auto poll=resolve<NimbyUiProviderPollV1>(module,"NimbyUi_ProviderPollV1");
        if(!add||!remove||!observe||!suspend||!poll){native::unload(module);return false;}
        try {
            NimbyUiProviderV1 wire{};wire.size=sizeof wire;wire.version=1;wire.count=static_cast<uint32_t>(services.size());
            copy(wire.id,id);for(size_t i=0;i<services.size();++i)copy(wire.services[i],services[i]);
            uint64_t token{};if(add(&wire,&token)!=NIMBY_OK||!token){native::unload(module);return false;}
            if(!translations.empty()){
                const auto set=resolve<NimbyUiTranslationsV1>(module,"NimbyUi_TranslationsV1");
                if(!set||set(1,token,translations.data(),static_cast<uint32_t>(translations.size()))!=NIMBY_OK){
                    remove(token);throw std::runtime_error("SDK UI bridge lacks compatible mod translations; update the SDK");
                }
            }
            module_=module;token_=token;remove_=remove;observe_=observe;suspend_=suspend;poll_=poll;
            publish_=resolve<NimbyUiToolPanelPublishV1>(module,"NimbyUi_ToolPanelPublishV1");
            publishV2_=resolve<NimbyUiToolPanelPublishV2>(module,"NimbyUi_ToolPanelPublishV2");
            pollV2_=resolve<NimbyUiProviderPollV2>(module,"NimbyUi_ProviderPollV2");
            preview_=resolve<NimbyUiSignalPreviewPublishV1>(module,"NimbyUi_SignalPreviewPublishV1");return true;
        }catch(...){native::unload(module);throw;}
    }
    bool observe(const GameSession& session){
        return module_&&observe_(token_,session.worldId.data(),static_cast<uint32_t>(session.worldId.size()),session.generation)==NIMBY_OK;
    }
    void suspend(){if(module_)suspend_(token_);}
    std::optional<NimbyUiActionEventV1> poll(){
        NimbyUiActionEventV1 out{};out.size=sizeof out;out.version=1;
        if(!module_||poll_(token_,&out)!=NIMBY_OK)return {};
        if(out.size!=sizeof out||out.version!=1||out.signal>>48!=8||!out.sequence||!out.generation||
            !out.panel||!std::memchr(out.origin,0,sizeof out.origin)||!std::memchr(out.action,0,sizeof out.action)||!std::memchr(out.service,0,sizeof out.service)||!std::memchr(out.world,0,sizeof out.world))return {};
        return out;
    }
    bool connected()const{return module_!=nullptr;}
    std::optional<NimbyUiActionEventV2> pollWithValue(){
        if(!module_)return {};
        NimbyUiActionEventV2 out{};out.base.size=sizeof out;out.base.version=2;
        if(!pollV2_){const auto old=poll();if(!old)return {};out.base=*old;return out;}
        if(pollV2_(token_,&out)!=NIMBY_OK)return {};
        const auto& b=out.base;
        if(b.size!=sizeof out||b.version!=2||out.has_value>1||b.signal>>48!=8||!b.sequence||!b.generation||!b.panel||
           !std::memchr(b.origin,0,sizeof b.origin)||!std::memchr(b.action,0,sizeof b.action)||
           !std::memchr(b.service,0,sizeof b.service)||!std::memchr(b.world,0,sizeof b.world))return {};
        return out;
    }
    uint32_t publish(const NimbyUiToolPanelV1& panel){return module_&&publish_?publish_(token_,&panel):NIMBY_HOOKS_UNAVAILABLE;}
    uint32_t publish(const NimbyUiToolPanelV2& panel){return module_&&publishV2_&&pollV2_?publishV2_(token_,&panel):NIMBY_HOOKS_UNAVAILABLE;}
    uint32_t preview(const NimbyUiSignalPreviewV1& value){return module_&&preview_?preview_(token_,&value):NIMBY_HOOKS_UNAVAILABLE;}
    void close(){if(module_){remove_(token_);native::unload(module_);}module_=nullptr;token_=0;}
private:
    template<class T> static T resolve(native::Module module,const char* name){
        const auto address=native::symbol(module,name);static_assert(sizeof(T)==sizeof(address));
        T result{};std::memcpy(&result,&address,sizeof result);return result;
    }
    template<size_t N> static void copy(char (&out)[N],std::string_view in){
        if(in.empty()||in.size()>=N||in.find('\0')!=std::string_view::npos)throw std::invalid_argument("Invalid mod service name");
        std::memcpy(out,in.data(),in.size());out[in.size()]=0;
    }
    native::Module module_{};
    uint64_t token_=0;
    NimbyUiProviderRemoveV1 remove_{};
    NimbyUiProviderObserveV1 observe_{};
    NimbyUiProviderSuspendV1 suspend_{};
    NimbyUiProviderPollV1 poll_{};
    NimbyUiToolPanelPublishV1 publish_{};
    NimbyUiToolPanelPublishV2 publishV2_{};
    NimbyUiProviderPollV2 pollV2_{};
    NimbyUiSignalPreviewPublishV1 preview_{};
};
}
