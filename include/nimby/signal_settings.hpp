#pragma once
#include <map>
#include <optional>
#include <string>
#include <string_view>
#include <span>

namespace nimby {
// Declarative C++ UI contract. The SDK owns rendering, events and persistence.
// Declaration alone does not install a native renderer.
struct SignalCheckbox {
    std::string_view name,label,description;
    bool defaultValue=false;
    bool onlyWhenEnabled=false;
};
// A named optional provider handles the action. This declaration does not add
// a loader dependency, and it never contains a callback from another mod.
struct SignalAction {std::string_view id,label,provider,service;};
struct SignalSettingsPanel {
    std::string_view id,title,textureSet;
    std::span<const SignalCheckbox> checkboxes;
    // Called by this mod's persistence client before importing a saved profile.
    // Never copied into or called by the resident game's UI thread.
    void (*migrate)(std::string_view panelId,std::map<std::string,bool,std::less<>>& values)=nullptr;
    std::span<const SignalAction> actions{};
};
// Absence is an observed fact; an unsuccessful read is a different state.
enum class SettingsStatus { Unavailable, Absent, Present };
template<class Settings> struct BooleanSetting { std::string_view name;bool Settings::*member; };
struct SignalSettings {
    SettingsStatus status=SettingsStatus::Unavailable;
    std::map<std::string,std::optional<bool>,std::less<>> booleans;
    std::optional<bool> getBoolean(std::string_view name) const {
        if(status!=SettingsStatus::Present)return std::nullopt;
        const auto found=booleans.find(name);
        return found==booleans.end()?std::nullopt:found->second;
    }
    template<class Settings> std::optional<Settings> readBooleans(std::span<const BooleanSetting<Settings>> fields) const {
        if(status!=SettingsStatus::Present)return std::nullopt;
        Settings result{};
        for(const auto& field:fields){const auto value=getBoolean(field.name);if(!value||!field.member)return std::nullopt;result.*(field.member)=*value;}
        return result;
    }
};
}
