#include "loader/loader.h"
#include "engine/binary_identity.h"
#include <filesystem>
#include <iostream>
#include <stdexcept>

namespace {
HANDLE stop_event{};
constexpr auto stop_event_name=L"Local\\NimbyRailsFranceSDK.Loader.Stop.v1";
BOOL WINAPI stop_handler(DWORD event) {
    if (event==CTRL_C_EVENT || event==CTRL_BREAK_EVENT || event==CTRL_CLOSE_EVENT) {
        SetEvent(stop_event); return TRUE;
    }
    return FALSE;
}
}
int wmain(int argc,wchar_t** argv) {
    try {
        if (argc==2 && std::wstring(argv[1])==L"--stop") {
            HANDLE event=OpenEventW(EVENT_MODIFY_STATE,FALSE,stop_event_name);
            if (!event) { std::cout << "No active loader stop event found.\n"; return 1; }
            const bool stopped=SetEvent(event)!=FALSE;
            CloseHandle(event);
            if (!stopped) throw std::runtime_error("Cannot signal loader stop");
            std::cout << "Stop requested. The SDK stays loaded until the game exits.\n";
            return 0;
        }
        std::wstring game=L"C:\\Program Files (x86)\\Steam\\steamapps\\common\\NIMBY Rails\\NimbyRails.exe";
        bool once=false;
        for (int i=1;i<argc;++i) {
            const std::wstring arg=argv[i];
            if (arg==L"--game" && i+1<argc) game=argv[++i];
            else if(arg==L"--once") once=true;
            else if(arg==L"--help") {
                std::cout << "NimbyRailsFranceLoader [--game <full exe path>] [--once] | --stop\n"
                    "Keep this console open. Start/restart the game normally through Steam.\n"
                    "Ctrl+C stops monitoring. The SDK stays loaded until the game exits.\n";
                return 0;
            } else throw std::runtime_error("Unknown argument; use --help");
        }
        NimbyBinaryInfo identity{};
        if (nimby::engine::identify(game.c_str(),identity)!=NIMBY_OK || !identity.recognized_research_build)
            throw std::runtime_error("Game path missing or game build unrecognized; loader refuses activation");
        const auto sdk=std::filesystem::path(nimby::loader::executable_path()).parent_path()/L"NimbyRailsFranceSDK.dll";
        // Avoid two supervisors injecting concurrently. No startup/service registration.
        HANDLE singleton=CreateMutexW(nullptr,FALSE,L"Local\\NimbyRailsFranceSDK.Loader.v1");
        if (!singleton) throw std::runtime_error("Cannot create loader instance guard");
        if (GetLastError()==ERROR_ALREADY_EXISTS) { CloseHandle(singleton); throw std::runtime_error("Loader already running"); }
        struct Close { HANDLE h; ~Close(){CloseHandle(h);} } singleton_owner{singleton};
        nimby::loader::Monitor monitor(game,identity.sha256,sdk.wstring());
        stop_event=CreateEventW(nullptr,TRUE,FALSE,stop_event_name);
        if (!stop_event) throw std::runtime_error("Cannot create stop event");
        struct Stop { ~Stop(){SetConsoleCtrlHandler(stop_handler,FALSE); CloseHandle(stop_event);} } stop_owner;
        if (!SetConsoleCtrlHandler(stop_handler,TRUE)) throw std::runtime_error("Cannot register console control handler");
        std::wcout << L"Watching: " << monitor.executable() << L"\nSDK: " << sdk.wstring() << L"\n" << std::flush;
        std::cout << "Diagnostics only. Leave this window open; Ctrl+C stops monitoring.\n" << std::flush;
        int result=0;
        do {
            const auto events=monitor.poll();
            if (once && events.empty()) { std::cout << "No matching running game found.\n"; result=2; }
            for (const auto& event:events) {
                std::cout << "PID " << event.pid << (event.success ? " OK: " : " ERROR: ") << event.message << '\n' << std::flush;
                if (!event.success) result=1;
            }
            if (once) break;
        } while (WaitForSingleObject(stop_event,1000)==WAIT_TIMEOUT);
        std::cout << "Monitoring stopped. Any initialized SDK remains loaded until game exit.\n";
        return result;
    } catch (const std::exception& error) { std::cerr << "Loader: " << error.what() << '\n'; return 1; }
}
