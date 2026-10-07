#pragma once
#include <unistd.h>
#include <fcntl.h>
#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <filesystem>
#include <cstdint>
#include <string_view>
#include <stdexcept>
namespace nimby::detail::platform {
// Discovery is advisory. Opening a session still validates binary identity.
inline Discovery discoverProcess() {
    std::uint32_t found=0;
    for(const auto& entry:std::filesystem::directory_iterator("/proc")) {
        const auto name=entry.path().filename().string();
        if(name.empty() || name.find_first_not_of("0123456789")!=std::string::npos)continue;
        std::error_code error;
        const auto executable=std::filesystem::read_symlink(entry.path()/"exe",error);
        if(error || executable.filename()!="nimbyrails")continue;
        if(found)return {NIMBY_INVALID_ARGUMENT,0};
        found=static_cast<std::uint32_t>(std::stoul(name));
    }
    if(!found)return {NIMBY_IO_ERROR,0};
    return {NIMBY_OK,found};
}
inline bool hostedByGame() {
    try{return nimby::detail::native::modulePath(nullptr).filename()=="nimbyrails";}
    catch(...){return false;}
}
inline uint32_t currentProcessId() noexcept {
    return static_cast<uint32_t>(getpid());
}
struct ModWork {};
inline std::filesystem::path stateDirectory() {
        std::filesystem::path root;
        const auto xdg=std::getenv("XDG_STATE_HOME");
        const auto home=std::getenv("HOME");
        if(xdg&&*xdg&&std::filesystem::path(xdg).is_absolute())root=xdg;
        else if(home&&*home&&std::filesystem::path(home).is_absolute())root=std::filesystem::path(home)/".local/state";
        else throw std::runtime_error("User state directory unavailable for settings");
    return std::filesystem::path(root);
}
inline constexpr const char* signalUiLibrary="libNimbySignalUiBridge-experimental-v1.so";
// Caller validates payload before creating files. The temporary is a sibling:
// replacement cannot cross volumes. Cleanup covers failures before replacement.
inline void atomicWrite(const std::filesystem::path& path, std::string_view bytes) {
    const auto folder=path.has_parent_path()?path.parent_path():std::filesystem::path(".");
    std::filesystem::create_directories(folder);
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
    
}
}
