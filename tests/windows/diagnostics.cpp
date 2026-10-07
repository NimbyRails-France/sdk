#include <nimby/detail/diagnostics.hpp>
#include <filesystem>
#include <fstream>
#include <stdexcept>
#include <string>
#include <iostream>
#include <thread>
#include <chrono>
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
    // A writer in another process/thread may retain the named log mutex.
    // Diagnostics must not add its former 50 ms wait to an observation.
    auto name=file.stem().wstring();
    const auto mutexName=L"Local\\NRF.Diagnostics.fixture."+name;
    HANDLE entered=CreateEventW(nullptr,TRUE,FALSE,nullptr),release=CreateEventW(nullptr,TRUE,FALSE,nullptr);
    CHECK(entered&&release);
    std::thread holder([&]{
        const auto mutex=CreateMutexW(nullptr,TRUE,mutexName.c_str());SetEvent(entered);
        WaitForSingleObject(release,5000);ReleaseMutex(mutex);CloseHandle(mutex);
    });
    WaitForSingleObject(entered,5000);
    const auto started=std::chrono::steady_clock::now();
    nimby::detail::diagnostics::write("fixture","ERROR","Contended log must not wait");
    const auto elapsed=std::chrono::steady_clock::now()-started;
    SetEvent(release);holder.join();CloseHandle(entered);CloseHandle(release);
    CHECK(elapsed<std::chrono::milliseconds(25));
    const auto beforeFlood=std::filesystem::file_size(file);
    for(unsigned i=0;i<200;++i){const auto message="Different failure "+std::to_string(i);
        nimby::detail::diagnostics::write("fixture","ERROR",message.c_str());}
    CHECK(std::filesystem::file_size(file)-beforeFlood<8192);
    // Clean up only the files created under our verified temporary root.
    std::filesystem::remove(file);std::filesystem::remove(file.wstring()+L".1");
    std::filesystem::remove(root/L"fixture");std::filesystem::remove(root);
    std::cout<<"PASS: persistent UTF-8 diagnostic, error preservation, rotation, nonblocking contention and varying-message flood bound\n";
}
