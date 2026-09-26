#pragma once
#include <nimby/signal_settings_store.hpp>
#include <nimby/detail/platform/host.hpp>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <sstream>

namespace nimby::detail {
// SDK sidecar only. Never pass the native game-save path here. The caller owns
// save/revision identity and invokes disk operations on its persistence worker.
// A missing file is distinct from malformed data; malformed data is not defaults.
class SignalSettingsFile {
    static constexpr size_t maxBytes=16*1024*1024;
    static void validate(const SignalSettingsStore::SavedSettings& data) {
        const auto text=[](const std::string& value,size_t limit){
            if(value.empty()||value.size()>limit||value.find('\0')!=std::string::npos)
                throw std::invalid_argument("Invalid settings text");
        };
        text(data.sessionId,512);text(data.panelId,128);
        if(data.signals.size()>16384)throw std::invalid_argument("Too many saved signals");
        std::set<uint64_t> ids;
        for(const auto& signal:data.signals){
            if(signal.id>>48!=8||!ids.insert(signal.id).second||signal.values.size()>64)
                throw std::invalid_argument("Invalid saved signal");
            for(const auto& [name,value]:signal.values){(void)value;text(name,128);}
        }
    }
    // Detect accidental truncation/corruption. This is not an authentication code.
    static uint32_t checksum(std::string_view bytes) {
        uint32_t crc=0xffffffff;
        for(unsigned char byte:bytes){crc^=byte;for(int bit=0;bit<8;++bit)crc=(crc>>1)^(0xedb88320u & (0u-(crc&1u)));}
        return ~crc;
    }
public:
    static std::string encode(const SignalSettingsStore::SavedSettings& data) {
        validate(data);
        std::ostringstream body;body.imbue(std::locale::classic());
        body<<std::quoted(data.sessionId)<<'\n'<<std::quoted(data.panelId)<<'\n'<<data.signals.size()<<'\n';
        for(const auto& signal:data.signals){
            body<<signal.id<<' '<<signal.values.size()<<'\n';
            for(const auto& [name,value]:signal.values)body<<std::quoted(name)<<' '<<(value?1:0)<<'\n';
        }
        const auto payload=body.str();
        std::ostringstream file;file.imbue(std::locale::classic());
        file<<"NIMBY-SIGNAL-SETTINGS 1\n"<<checksum(payload)<<'\n'<<payload;
        auto result=file.str();if(result.size()>maxBytes)throw std::length_error("Settings file too large");
        return result;
    }
    static SignalSettingsStore::SavedSettings decode(std::string_view bytes) {
        if(bytes.size()>maxBytes)throw std::length_error("Settings file too large");
        constexpr std::string_view magic="NIMBY-SIGNAL-SETTINGS 1\n";
        if(!bytes.starts_with(magic))throw std::invalid_argument("Unsupported settings file");
        bytes.remove_prefix(magic.size());const auto newline=bytes.find('\n');
        if(newline==std::string_view::npos||newline>10)throw std::invalid_argument("Missing settings checksum");
        uint32_t expected{};std::istringstream hash{std::string(bytes.substr(0,newline))};
        hash.imbue(std::locale::classic());
        if(!(hash>>expected)||hash.peek()!=EOF)throw std::invalid_argument("Invalid settings checksum");
        bytes.remove_prefix(newline+1);
        if(checksum(bytes)!=expected)throw std::invalid_argument("Corrupt settings file");
        std::istringstream input{std::string(bytes)};input.imbue(std::locale::classic());
        SignalSettingsStore::SavedSettings data;size_t count{};
        if(!(input>>std::quoted(data.sessionId)>>std::quoted(data.panelId)>>count)||count>16384)
            throw std::invalid_argument("Invalid settings header");
        data.signals.reserve(count); // Count is validated before allocating.
        for(size_t i=0;i<count;++i){
            SignalSettingsStore::SavedSignal signal;size_t fields{};
            if(!(input>>signal.id>>fields)||fields>64)throw std::invalid_argument("Invalid saved signal");
            for(size_t j=0;j<fields;++j){
                std::string name;int value{};
                if(!(input>>std::quoted(name)>>value)||(value!=0&&value!=1)||!signal.values.emplace(name,value!=0).second)
                    throw std::invalid_argument("Invalid saved checkbox");
            }
            data.signals.push_back(std::move(signal));
        }
        input>>std::ws;if(input.peek()!=EOF)throw std::invalid_argument("Trailing settings data");
        validate(data);return data;
    }
    static std::optional<SignalSettingsStore::SavedSettings> load(const std::filesystem::path& path) {
        std::error_code error;const auto size=std::filesystem::file_size(path,error);
        if(error==std::errc::no_such_file_or_directory)return std::nullopt;
        if(error)throw std::filesystem::filesystem_error("Read settings size",path,error);
        if(size>maxBytes)throw std::length_error("Settings file too large");
        std::ifstream file(path,std::ios::binary);std::string bytes(static_cast<size_t>(size),'\0');
        if(!file.read(bytes.data(),static_cast<std::streamsize>(bytes.size()))||file.peek()!=EOF)
            throw std::runtime_error("Cannot read complete settings file");
        return decode(bytes);
    }
    static void save(const std::filesystem::path& path,const SignalSettingsStore::SavedSettings& data) {
        const auto bytes=encode(data); // Validate before touching the existing file.
        platform::atomicWrite(path,bytes);
    }
};
}
