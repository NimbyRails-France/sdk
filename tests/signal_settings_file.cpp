#include <nimby/detail/signal_settings_file.hpp>
#include <iostream>
#define CHECK(x) do{if(!(x))throw std::runtime_error("Failed line "+std::to_string(__LINE__));}while(false)
struct Temporary {
    std::filesystem::path path;
    Temporary(){
#ifdef _WIN32
        wchar_t folder[MAX_PATH]{},file[MAX_PATH]{};
        if(!GetTempPathW(MAX_PATH,folder)||!GetTempFileNameW(folder,L"nst",0,file))throw std::runtime_error("Temporary path failed");
        path=file;
#else
        auto pattern=(std::filesystem::temp_directory_path()/"nrf-settings-test-XXXXXX").string();
        int file=mkstemp(pattern.data());
        if(file<0)throw std::runtime_error("Temporary path failed");
        ::close(file);path=pattern;
#endif
    }
    ~Temporary(){std::error_code ignored;std::filesystem::remove(path,ignored);}
};
template<class Action> void rejects(Action action){bool rejected=false;try{action();}catch(const std::exception&){rejected=true;}CHECK(rejected);}
int main(){try{
    using File=nimby::detail::SignalSettingsFile;
    using Store=nimby::SignalSettingsStore;
    Store::SavedSettings data{"save \"A\"","sfr.bal-a",{{0x8000000000001,{{"active",true},{"green",false}}}}};
    const auto encoded=File::encode(data);
    const auto restored=File::decode(encoded);
    CHECK(restored.sessionId==data.sessionId&&restored.panelId==data.panelId);
    CHECK(restored.signals.size()==1&&restored.signals[0].values==data.signals[0].values);
    for(size_t cut:{size_t{0},size_t{10},encoded.size()-1})rejects([&]{File::decode(encoded.substr(0,cut));});
    auto damaged=encoded;damaged.back()='x';rejects([&]{File::decode(damaged);});
    damaged=encoded;damaged[22]='9';rejects([&]{File::decode(damaged);});
    Temporary target;
    rejects([&]{File::load(target.path);}); // An empty file is corrupt, not absent.
    File::save(target.path,data);
    CHECK(File::load(target.path)->signals[0].values.at("active"));
    auto invalid=data;invalid.signals.push_back(invalid.signals[0]);
    rejects([&]{File::save(target.path,invalid);});
    CHECK(File::load(target.path)->signals.size()==1);
    auto changed=data;changed.signals[0].values["active"]=false;
#ifdef _WIN32
    // A failed replacement must preserve the previously durable values.
    HANDLE held=CreateFileW(target.path.c_str(),GENERIC_READ,FILE_SHARE_READ,nullptr,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,nullptr);
    CHECK(held!=INVALID_HANDLE_VALUE);
    bool failed=false;try{File::save(target.path,changed);}catch(const std::exception&){failed=true;}
    CloseHandle(held);CHECK(failed);
    CHECK(File::load(target.path)->signals[0].values.at("active"));
#else
    // POSIX replacement keeps an already open reader on the old complete inode.
    std::ifstream held(target.path,std::ios::binary);
    File::save(target.path,changed);
    std::string oldBytes((std::istreambuf_iterator<char>(held)),{});
    CHECK(File::decode(oldBytes).signals[0].values.at("active"));
    CHECK(!File::load(target.path)->signals[0].values.at("active"));
    const auto directory=target.path.string()+"-directory";
    std::filesystem::create_directory(directory);
    rejects([&]{File::save(directory,data);});
    CHECK(std::filesystem::is_directory(directory));
    std::filesystem::remove(directory);
#endif
    File::save(target.path,changed);CHECK(!File::load(target.path)->signals[0].values.at("active"));
    CHECK(std::filesystem::remove(target.path));CHECK(!File::load(target.path));
    std::cout<<"PASS settings file roundtrip, corruption and failed replacement\n";
}catch(const std::exception& error){std::cerr<<error.what()<<'\n';return 1;}}
