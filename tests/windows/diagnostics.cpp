#include <nimby/detail/diagnostics.hpp>
#include <filesystem>
#include <fstream>
#include <stdexcept>
#include <string>
#include <iostream>
#define CHECK(x) do { if(!(x))throw std::runtime_error(#x); } while(false)
int main() {
    wchar_t temporary[MAX_PATH]{};CHECK(GetTempPathW(MAX_PATH,temporary));
    const auto root=std::filesystem::path(temporary)/(L"nrf-logs-"+std::to_wstring(GetCurrentProcessId()));
    std::filesystem::create_directories(root);CHECK(SetEnvironmentVariableW(L"NRF_LOG_DIR",root.c_str()));
    SetLastError(9876);
    nimby::detail::diagnostics::write("fixture","ERROR","Erreur UTF-8: \xc3\xa9" "chec");
    CHECK(GetLastError()==9876);
    nimby::detail::diagnostics::write("fixture","ERROR","Erreur UTF-8: \xc3\xa9" "chec");
    nimby::detail::diagnostics::write("fixture","INFO","Recovered");
    std::filesystem::path file;
    for(const auto& entry:std::filesystem::directory_iterator(root/L"fixture"))if(entry.path().extension()==L".log")file=entry.path();
    CHECK(!file.empty());
    std::ifstream in(file,std::ios::binary);std::string text((std::istreambuf_iterator<char>(in)),{});in.close();
    CHECK(text.find("repeated 1 times")!=std::string::npos&&text.find("[pid=")!=std::string::npos&&text.find("Recovered")!=std::string::npos);
    {std::ofstream out(file,std::ios::binary|std::ios::app);out<<std::string(1024*1024,'x');}
    nimby::detail::diagnostics::write("fixture","INFO","After rotation");
    CHECK(std::filesystem::exists(file.wstring()+L".1")&&std::filesystem::file_size(file)<4096);
    // Clean up only the files created under our verified temporary root.
    std::filesystem::remove(file);std::filesystem::remove(file.wstring()+L".1");
    std::filesystem::remove(root/L"fixture");std::filesystem::remove(root);
    std::cout<<"PASS: persistent UTF-8 diagnostic, Win32 error preservation, repeat count and rotation\n";
}
