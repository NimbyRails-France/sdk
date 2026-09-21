#pragma once
#include <nimby/signal_settings_store.hpp>
#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#include <fcntl.h>
#include <cerrno>
#include <cstdio>
#include <cstdlib>
#endif
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
        const auto folder=path.has_parent_path()?path.parent_path():std::filesystem::path(L".");
        std::filesystem::create_directories(folder);
#ifdef _WIN32
        wchar_t temporary[MAX_PATH]{};
        if(!GetTempFileNameW(folder.c_str(),L"nss",0,temporary))throw std::runtime_error("Cannot create settings temporary file");
        HANDLE file=INVALID_HANDLE_VALUE;
        try {
            file=CreateFileW(temporary,GENERIC_WRITE,0,nullptr,TRUNCATE_EXISTING,FILE_ATTRIBUTE_NORMAL,nullptr);
            DWORD written{};
            if(file==INVALID_HANDLE_VALUE||!WriteFile(file,bytes.data(),static_cast<DWORD>(bytes.size()),&written,nullptr)||
               written!=bytes.size()||!FlushFileBuffers(file))throw std::runtime_error("Cannot write settings file");
            CloseHandle(file);file=INVALID_HANDLE_VALUE;
            if(!MoveFileExW(temporary,path.c_str(),MOVEFILE_REPLACE_EXISTING|MOVEFILE_WRITE_THROUGH))
                throw std::runtime_error("Cannot replace settings file");
        } catch(...) {
            if(file!=INVALID_HANDLE_VALUE)CloseHandle(file);
            DeleteFileW(temporary);throw;
        }
#else
        auto temporary=(folder/".nrf-settings-XXXXXX").string();
        int file=mkstemp(temporary.data());
        if(file<0)throw std::runtime_error("Cannot create settings temporary file");
        int directory=-1;
        try {
            size_t offset=0;
            while(offset<bytes.size()) {
                const auto written=::write(file,bytes.data()+offset,bytes.size()-offset);
                if(written<0&&errno==EINTR)continue;
                if(written<=0)throw std::runtime_error("Cannot write settings file");
                offset+=static_cast<size_t>(written);
            }
            if(::fsync(file)!=0)throw std::runtime_error("Cannot flush settings file");
            const auto closed=::close(file);file=-1;
            if(closed!=0)throw std::runtime_error("Cannot close settings file");
            directory=::open(folder.c_str(),O_RDONLY|O_DIRECTORY|O_CLOEXEC);
            if(directory<0)throw std::runtime_error("Cannot open settings directory");
            if(::rename(temporary.c_str(),path.c_str())!=0)throw std::runtime_error("Cannot replace settings file");
            // Persist the directory entry as well as the file contents.
            if(::fsync(directory)!=0)throw std::runtime_error("Cannot flush settings directory");
            ::close(directory);directory=-1;
        } catch(...) {
            if(file>=0)::close(file);
            if(directory>=0)::close(directory);
            ::unlink(temporary.c_str());throw;
        }
#endif
    }
};
}
