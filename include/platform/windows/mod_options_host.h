#pragma once
#include <runtime/mod_options_registry.h>
#include <filesystem>
#include <memory>
#include <string_view>

namespace nimby::platform::windows::mod_options {
// Resident SDK service. The UI only edits the in-memory registry; a separate
// SDK worker persists changes. No mod callback or file IO runs in a UI hook.
class Host {
public:
    runtime::mod_options::Registry registry;
    explicit Host(std::filesystem::path directory={});
    ~Host();
    Host(const Host&)=delete;
    Host& operator=(const Host&)=delete;
    runtime::mod_options::Result add(std::string_view declaration);
    runtime::mod_options::Result remove(uint64_t token);
    runtime::mod_options::Result change(uint64_t token,std::string_view field,std::string value);
    runtime::mod_options::Result reset(uint64_t token,std::string_view field={});
    std::string translate(uint64_t token,std::string_view text,std::string_view language)const;
    std::string storageError(uint64_t token)const;
    uint64_t catalogueRevision()const noexcept;
    // Broker-only snapshot serialization; never called from game hooks.
    uint32_t read(uint64_t token,uint64_t known,char* output,uint32_t capacity,uint32_t* written,uint64_t* revision);
private:
    struct State;
    std::unique_ptr<State> state_;
};
Host& host();
}
