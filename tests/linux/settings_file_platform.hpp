#pragma once
// Included after CHECK by the common settings roundtrip test.
struct Temporary {
    std::filesystem::path path;
    Temporary() {
        auto pattern=(std::filesystem::temp_directory_path()/"nrf-settings-test-XXXXXX").string();
        int file=mkstemp(pattern.data());
        if(file<0)throw std::runtime_error("Temporary path failed");
        ::close(file);path=pattern;
    }
    ~Temporary(){std::error_code ignored;std::filesystem::remove(path,ignored);}
};
inline void verifyReplacement(const Temporary& target,
    const nimby::SignalSettingsStore::SavedSettings& data,
    const nimby::SignalSettingsStore::SavedSettings& changed) {
    using File=nimby::detail::SignalSettingsFile;
    (void)data;
    // POSIX replacement keeps an already open reader on the old complete inode.
    std::ifstream held(target.path,std::ios::binary);
    File::save(target.path,changed);
    std::string oldBytes((std::istreambuf_iterator<char>(held)),{});
    CHECK(File::decode(oldBytes).signals[0].values.at("active"));
    CHECK(!File::load(target.path)->signals[0].values.at("active"));
    const auto directory=target.path.string()+"-directory";
    std::filesystem::create_directory(directory);
    bool failed=false;try{File::save(directory,data);}catch(const std::exception&){failed=true;}CHECK(failed);
    CHECK(std::filesystem::is_directory(directory));
    std::filesystem::remove(directory);
}
