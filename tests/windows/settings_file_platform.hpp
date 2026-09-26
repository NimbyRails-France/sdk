#pragma once
// Included after CHECK by the common settings roundtrip test.
struct Temporary {
    std::filesystem::path path;
    Temporary() {
        wchar_t folder[MAX_PATH]{},file[MAX_PATH]{};
        if(!GetTempPathW(MAX_PATH,folder)||!GetTempFileNameW(folder,L"nst",0,file))throw std::runtime_error("Temporary path failed");
        path=file;
    }
    ~Temporary(){std::error_code ignored;std::filesystem::remove(path,ignored);}
};
inline void verifyReplacement(const Temporary& target,
    const nimby::SignalSettingsStore::SavedSettings& data,
    const nimby::SignalSettingsStore::SavedSettings& changed) {
    using File=nimby::detail::SignalSettingsFile;
    (void)data;
    // A failed replacement must preserve the previously durable values.
    HANDLE held=CreateFileW(target.path.c_str(),GENERIC_READ,FILE_SHARE_READ,nullptr,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,nullptr);
    CHECK(held!=INVALID_HANDLE_VALUE);
    bool failed=false;try{File::save(target.path,changed);}catch(const std::exception&){failed=true;}
    CloseHandle(held);CHECK(failed);
    CHECK(File::load(target.path)->signals[0].values.at("active"));
}
