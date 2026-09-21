#include <nimby/mod.hpp>
#include <nimby/detail/signal_settings_runtime.hpp>
#include <stdexcept>
#include <cstdio>

extern "C" DWORD WINAPI NRFMod_StartV1(void*) noexcept;
extern "C" DWORD WINAPI NRFMod_StopV1(void*) noexcept;
extern "C" DWORD WINAPI NRFMod_IsInitializedV1(void*) noexcept;
extern "C" DWORD WINAPI NRFMod_ShowTextureV1(uint64_t, const char*) noexcept;
extern "C" DWORD WINAPI NRFMod_RestoreTextureV1(uint64_t) noexcept;
extern "C" DWORD WINAPI NRFMod_ReadTrainV1(uint64_t, NimbyDrivingObservation*) noexcept;
extern "C" DWORD WINAPI NRFMod_InvokeV1(const char*,const void*,uint32_t,void*,uint32_t) noexcept;
namespace {
bool fail_start = false, fail_stop = false;
int starts = 0, stops = 0, shows = 0, restores = 0, observations = 0;
struct Request { int value; };
struct Response { int value; };
Response echo(const Request& request) {
    if(request.value==-1) throw std::invalid_argument("bad command");
    if(request.value==-2) throw std::runtime_error("failed command");
    return {request.value};
}
constexpr std::array commands{nimby::command<Request,Response,echo>("test.echo.v1")};
constexpr nimby::SignalCheckbox checkboxes[]{{"active","Active","",false}};
}
nimby::Mod nimby::createMod() {
    return {
        .showTexture = [](Id signal, const char*) {
            if (signal == 2) throw Exception({3, "test", "typed error"});
            if (signal == 3) throw std::runtime_error("unexpected error");
            ++shows;
        },
        .restoreTexture = [](Id) { ++restores; },
        .start = [] { if (fail_start) throw std::runtime_error("start failed"); ++starts; },
        .stop = [] { if (fail_stop) throw std::runtime_error("stop failed"); ++stops; },
        .readTrain = [](Id id) -> std::optional<DrivingObservation> {
            if(id==0x5000000000002ULL)return std::nullopt;
            if(id==0x5000000000003ULL)throw std::runtime_error("read failed");
            NimbyDrivingObservation data{};data.struct_size=sizeof data;data.train_id=id;
            return DrivingObservation{data};
        },
        .commands = commands,
        .observe = [](const nimby::Snapshot&) { ++observations; },
        .signalSettings={"test.panel","Test","test.atlas",checkboxes}
    };
}
#define CHECK(x) do { if (!(x)) { std::fprintf(stderr, "Failed line %d: %s\n", __LINE__, #x); return 1; } } while (false)
int main() {
    // SDK automatic loader refuses a diagnostic host before loading any bridge.
    CHECK(NimbyInternal_EnsureSignalUiBridge()==NIMBY_INVALID_BINARY);
    CHECK(!GetModuleHandleW(L"NimbySignalUiBridge-experimental-v1.dll"));
    // Reject invalid IDs before trying to discover or connect to a game.
    try {
        (void)nimby::readTrain(1);
        CHECK(false);
    } catch (const nimby::Exception& error) {
        CHECK(error.code() == nimby::ErrorCode::InvalidArgument);
    }
    CHECK(NRFMod_IsInitializedV1(nullptr) == 0);
    CHECK(NRFMod_ShowTextureV1(1, "image.svg") == NIMBY_INVALID_HANDLE);
    CHECK(NRFMod_RestoreTextureV1(1) == NIMBY_INVALID_HANDLE);
    CHECK(NRFMod_StartV1(reinterpret_cast<void*>(1)) == NIMBY_INVALID_ARGUMENT);
    fail_start = true;
    CHECK(NRFMod_StartV1(nullptr) == NIMBY_INTERNAL_ERROR && NRFMod_IsInitializedV1(nullptr) == 0);
    fail_start = false;
    CHECK(NRFMod_StartV1(nullptr) == NIMBY_OK && starts == 1);
    CHECK(!nimby::modObservationStatus().running && observations == 0); // Diagnostic host must never attach.
    auto& settings=nimby::detail::signalSettingsStore();
    CHECK(settings.panel().id=="test.panel");
    constexpr uint64_t settingsSignal=0x8000000000001;
    CHECK(nimby::readSignalSettings(settingsSignal).status==nimby::SettingsStatus::Unavailable);
    const auto settingsSession=settings.beginSession("fixture-save");
    const std::array<nimby::SignalSettingsStore::Signal,1> configured{{{settingsSignal,"test.atlas"}}};
    CHECK(settings.observeSignals(settingsSession,configured));
    CHECK(settings.setBoolean(settings.selectSignal(settingsSession,settingsSignal),"active",true));
    CHECK(nimby::readSignalSettings(settingsSignal).getBoolean("active")==true);
    CHECK(NRFMod_StartV1(nullptr) == NIMBY_ALREADY_INITIALIZED && starts == 1);
    Request input{42}; Response output{123};
    CHECK(NRFMod_InvokeV1("test.echo.v1",&input,sizeof input,&output,sizeof output)==NIMBY_OK && output.value==42);
    CHECK(NRFMod_InvokeV1("test.echo.v1",&input,sizeof input-1,&output,sizeof output)==NIMBY_INVALID_ARGUMENT && output.value==0);
    CHECK(NRFMod_InvokeV1("missing",&input,sizeof input,&output,sizeof output)==NIMBY_HOOKS_UNAVAILABLE);
    input.value=-1;
    CHECK(NRFMod_InvokeV1("test.echo.v1",&input,sizeof input,&output,sizeof output)==NIMBY_INVALID_ARGUMENT && output.value==0);
    input.value=-2;
    CHECK(NRFMod_InvokeV1("test.echo.v1",&input,sizeof input,&output,sizeof output)==NIMBY_INTERNAL_ERROR && output.value==0);
    CHECK(NRFMod_InvokeV1(nullptr,&input,sizeof input,&output,sizeof output)==NIMBY_INVALID_ARGUMENT);
    nimby::FixedList<int,2> list;
    list.push_back(1); list.push_back(2);
    CHECK(list.values().size()==2);
    try { list.push_back(3); CHECK(false); } catch (const std::invalid_argument&) {}
    list.count=3;
    try { (void)list.values(); CHECK(false); } catch (const std::invalid_argument&) {}
    nimby::FixedText<4> text;
    text.assign("abc"); CHECK(text.view()=="abc");
    try { text.assign("abcd"); CHECK(false); } catch (const std::invalid_argument&) {}
    NimbyDrivingObservation observation{};observation.struct_size=sizeof observation;
    CHECK(NRFMod_ReadTrainV1(0x5000000000001ULL,&observation)==NIMBY_OK);
    CHECK(observation.train_id==0x5000000000001ULL);
    CHECK(NRFMod_ReadTrainV1(0x5000000000002ULL,&observation)==NIMBY_DATA_UNAVAILABLE && observation.train_id==0);
    CHECK(NRFMod_ReadTrainV1(0x5000000000003ULL,&observation)==NIMBY_INTERNAL_ERROR && observation.train_id==0);
    CHECK(NRFMod_ReadTrainV1(1,&observation)==NIMBY_INVALID_ARGUMENT);
    CHECK(NRFMod_ShowTextureV1(0, "image.svg") == NIMBY_INVALID_ARGUMENT);
    CHECK(NRFMod_ShowTextureV1(1, nullptr) == NIMBY_INVALID_ARGUMENT);
    CHECK(NRFMod_ShowTextureV1(1, "") == NIMBY_INVALID_ARGUMENT);
    CHECK(NRFMod_ShowTextureV1(1, "image.svg") == NIMBY_OK && shows == 1);
    CHECK(NRFMod_ShowTextureV1(2, "image.svg") == NIMBY_INVALID_BINARY);
    CHECK(NRFMod_ShowTextureV1(3, "image.svg") == NIMBY_INTERNAL_ERROR);
    CHECK(NRFMod_RestoreTextureV1(0) == NIMBY_INVALID_ARGUMENT);
    CHECK(NRFMod_RestoreTextureV1(1) == NIMBY_OK && restores == 1);
    CHECK(NRFMod_StopV1(reinterpret_cast<void*>(1)) == NIMBY_INVALID_ARGUMENT);
    fail_stop = true;
    CHECK(NRFMod_StopV1(nullptr) == NIMBY_INTERNAL_ERROR && NRFMod_IsInitializedV1(nullptr) == 1);
    fail_stop = false;
    CHECK(NRFMod_StopV1(nullptr) == NIMBY_OK && NRFMod_IsInitializedV1(nullptr) == 0 && stops == 1);
    CHECK(NRFMod_ReadTrainV1(0x5000000000001ULL,&observation)==NIMBY_INVALID_HANDLE);
    CHECK(NRFMod_InvokeV1("test.echo.v1",&input,sizeof input,&output,sizeof output)==NIMBY_INVALID_HANDLE);
    CHECK(NRFMod_StopV1(nullptr) == NIMBY_OK && stops == 1);
    CHECK(NRFMod_StartV1(nullptr) == NIMBY_OK && starts == 2);
    CHECK(NRFMod_StopV1(nullptr) == NIMBY_OK && stops == 2);
    std::puts("PASS adapter lifecycle, command dispatch, validation and exception boundaries");
}
