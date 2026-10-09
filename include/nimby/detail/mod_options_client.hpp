#pragma once
#include <nimby/detail/mod_options_bridge.h>
#include <nimby/detail/native_library.hpp>
#include <nimby/detail/platform/host.hpp>
#include <nimby/detail/vendor/json.hpp>
#include <array>
#include <string>
#include <vector>

namespace nimby::detail {
// Adapter-owned: a single mod worker reads changes once per observation, never
// once per signal. No filesystem operations or Kotlin callbacks on the game UI.
class ModOptionsClient {
public:
    struct Changes {std::vector<std::string> values,events;bool changed=false;};
    ModOptionsClient()=default;
    ModOptionsClient(const ModOptionsClient&)=delete;
    ModOptionsClient& operator=(const ModOptionsClient&)=delete;
    bool connect(std::string_view declaration){
        if(module_)return true;
        if(declaration.empty()||declaration.size()>NIMBY_OPTIONS_SCHEMA_LIMIT)return false;
        auto module=native::existing(platform::signalUiLibrary);if(!module)return false;
        const auto add=resolve<NimbyOptionsRegisterV1>(module,"NimbyOptions_RegisterV1");
        const auto remove=resolve<NimbyOptionsRemoveV1>(module,"NimbyOptions_RemoveV1");
        const auto read=resolve<NimbyOptionsReadV1>(module,"NimbyOptions_ReadV1");
        const auto discard=resolve<NimbyOptionsDiscardV1>(module,"NimbyOptions_DiscardV1");
        if(!add||!remove||!read||!discard){native::unload(module);return false;}
        uint64_t token{};
        if(add(declaration.data(),static_cast<uint32_t>(declaration.size()),&token)!=NIMBY_OK||!token){native::unload(module);return false;}
        module_=module;token_=token;remove_=remove;read_=read;discard_=discard;revision_=0;return true;
    }
    Changes refresh(){return refresh([](const Changes&){});}
    // A schema/value rejection must leave the old revision pending, so the
    // next observation retries the full snapshot instead of losing the change.
    template<class Validate> Changes refresh(Validate&& validate){
        Changes changes;if(!module_)return changes;
        uint32_t written{};uint64_t revision{};
        const auto status=read_(token_,revision_,buffer_.data(),static_cast<uint32_t>(buffer_.size()),&written,&revision);
        if(status!=NIMBY_OK)throw std::runtime_error("Cannot read mod options from the SDK");
        if(written>buffer_.size()||!revision||revision<revision_||(!written&&revision!=revision_))
            throw std::runtime_error("Invalid mod options response");
        if(written){
            const auto data=nlohmann::json::parse(buffer_.data(),buffer_.data()+written);
            if(!data.is_object()||(revision!=revision_&&!data.contains("values")))
                throw std::runtime_error("Missing changed mod option values");
            if(data.contains("values")){
                changes.values=data.at("values").get<std::vector<std::string>>();changes.changed=true;
                if(changes.values.size()>64)throw std::runtime_error("Too many mod option values");
                for(const auto& value:changes.values)if(value.size()>256||value.find('\0')!=std::string::npos)
                    throw std::runtime_error("Invalid mod option value");
            }
            changes.events=data.value("events",std::vector<std::string>{});
            if(changes.events.size()>32)throw std::runtime_error("Too many mod shortcut events");
            for(const auto& event:changes.events)if(event.size()>128||event.find('\0')!=std::string::npos)
                throw std::runtime_error("Invalid mod shortcut event");
        }
        validate(changes);revision_=revision;return changes;
    }
    void discardEvents(){
        if(module_&&discard_(token_)!=NIMBY_OK)throw std::runtime_error("Cannot discard stale mod shortcut events");
    }
    bool connected()const noexcept{return module_!=nullptr;}
    // Preserve ownership if removal temporarily fails. Dropping the token here
    // would leave an orphan registration and make a later connect impossible.
    bool close(){
        if(!module_)return true;
        if(remove_(token_)!=NIMBY_OK)return false;
        releaseModule();return true;
    }
    ~ModOptionsClient(){
        // The broker is also responsible for cleanup when the host exits.
        // Destruction must still balance our local module reference.
        close();releaseModule();
    }
private:
    void releaseModule(){
        if(module_)native::unload(module_);
        module_=nullptr;token_=revision_=0;remove_=nullptr;read_=nullptr;discard_=nullptr;
    }
    template<class T> static T resolve(native::Module module,const char* name){
        const auto address=native::symbol(module,name);T result{};static_assert(sizeof(result)==sizeof(address));
        std::memcpy(&result,&address,sizeof result);return result;
    }
    native::Module module_{};uint64_t token_{},revision_{};
    NimbyOptionsRemoveV1 remove_{};NimbyOptionsReadV1 read_{};NimbyOptionsDiscardV1 discard_{};
    std::array<char,NIMBY_OPTIONS_VALUES_LIMIT> buffer_{};
};
}
