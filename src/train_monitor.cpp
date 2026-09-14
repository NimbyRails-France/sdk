// Research-only external reader. Layout evidence: docs/research/train-observation.md.
// Explicit one-shot speed experiments; never calls target code or suspends threads.
#include "engine/binary_identity.h"
#include "engine/live_state.h"
#include "engine/network.h"
#include <windows.h>
#include <commctrl.h>
#include <tlhelp32.h>
#include <algorithm>
#include <array>
#include <chrono>
#include <cmath>
#include <cstdio>
#include <cstring>
#include <cwctype>
#include <map>
#include <set>
#include <string>
#include <vector>

using Bytes = std::vector<unsigned char>;
template<class T> T at(const unsigned char* p, size_t off) {
    T result{}; std::memcpy(&result, p + off, sizeof result); return result;
}
struct Reader {
    HANDLE handle{};
    ~Reader() { if (handle) CloseHandle(handle); }
    bool read(uint64_t address, void* out, size_t length) const {
        SIZE_T received{};
        return address >= 0x10000 && address <= 0x7fffffffffffULL - length &&
            ReadProcessMemory(handle, reinterpret_cast<const void*>(address), out, length, &received) && received == length;
    }
};
struct Pool {
    uint64_t address{}, begin{}, end{};
    uint32_t shift{}, size{}, mask{};
    std::array<unsigned char, 48> header{};
    nimby::engine::LiveState owner{};
};
bool live_read(void* context, uint64_t address, void* out, size_t size) {
    return static_cast<const Reader*>(context)->read(address, out, size);
}
bool current_owner(const Reader& reader, const Pool& pool) {
    if (!pool.owner.module_base) return true; // Synthetic fixtures / read-only discovery.
    nimby::engine::LiveState current{};
    return nimby::engine::resolve_live_state(live_read, const_cast<Reader*>(&reader), pool.owner.module_base, true, current) && current == pool.owner;
}
uint64_t image_base(const Reader& reader) {
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, GetProcessId(reader.handle));
    if (snapshot == INVALID_HANDLE_VALUE) return 0;
    MODULEENTRY32W module{}; module.dwSize = sizeof module;
    const auto result = Module32FirstW(snapshot, &module) ? reinterpret_cast<uint64_t>(module.modBaseAddr) : 0;
    CloseHandle(snapshot); return result;
}
bool decode(const unsigned char* p, Pool& pool) {
    pool.shift = at<uint32_t>(p, 4); pool.size = at<uint32_t>(p, 8);
    pool.mask = at<uint32_t>(p, 16); pool.begin = at<uint64_t>(p, 24); pool.end = at<uint64_t>(p, 32);
    const auto capacity = at<uint64_t>(p, 40);
    return pool.shift > 0 && pool.shift <= 16 && pool.size == (1u << pool.shift) &&
        pool.mask == pool.size - 1 && pool.begin >= 0x10000 && pool.begin % 8 == 0 &&
        pool.end > pool.begin && pool.end <= capacity && capacity - pool.begin <= 8192 &&
        (pool.end - pool.begin) % 8 == 0;
}
bool string_at(const Reader& r, const unsigned char* p, std::string& out) {
    const auto length = at<uint64_t>(p, 16), capacity = at<uint64_t>(p, 24);
    if (length > 256 || capacity < 15 || capacity < length || capacity > 4096) return false;
    Bytes text(static_cast<size_t>(length) + 1);
    if (capacity <= 15) std::memcpy(text.data(), p, text.size());
    else if (!r.read(at<uint64_t>(p, 0), text.data(), text.size())) return false;
    if (text.back() != 0) return false;
    for (size_t i = 0; i < length; ++i) if (text[i] < 32 || text[i] == 127) return false;
    out.assign(reinterpret_cast<char*>(text.data()), static_cast<size_t>(length));
    return true;
}
struct Item { uint64_t id{}, address{}; std::string name, code; bool present{}; double speed{};
    bool positioned{}; nimby::engine::TrainPosition position{}; };
// A successful scan is still an optimistic external snapshot, not a simulation-thread snapshot.
bool items(const Reader& r, const Pool& pool, bool motion, std::vector<Item>& out) {
    out.clear();
    if (!current_owner(r, pool)) return false;
    std::array<unsigned char, 48> header{};
    if (!r.read(pool.address, header.data(), header.size()) || header != pool.header) return false;
    std::vector<uint64_t> blocks((pool.end - pool.begin) / 8);
    if (!r.read(pool.begin, blocks.data(), blocks.size() * 8)) return false;
    const size_t stride = motion ? 0x638 : 0x178;
    if (blocks.size() * pool.size > 100000) return false;
    Bytes block(pool.size * stride);
    for (size_t b = 0; b < blocks.size(); ++b) {
        if (!blocks[b]) continue;
        if (!r.read(blocks[b], block.data(), block.size())) return false;
        for (size_t i = 0; i < pool.size; ++i) {
            const auto* p = block.data() + i * stride;
            const auto id = at<uint64_t>(p, 0), type = id >> 48;
            if (type == 0 || type == 0xffff) continue;
            if (type != 5 || ((id >> 16) & 0xffffffffULL) != (b << pool.shift) + i) return false;
            Item item{}; item.id = id; item.address = blocks[b] + i * stride;
            if (motion) {
                if (p[0x4b0] > 1) return false;
                item.present = p[0x4b0] != 0;
                if (item.present) {
                    item.speed = at<double>(p, 0x3c8);
                    if (!std::isfinite(item.speed) || std::abs(item.speed) > 10000) return false;
                    item.positioned = nimby::engine::decode_train_position(p,stride,item.position);
                }
            } else if (!string_at(r, p + 0x10, item.name) || !string_at(r, p + 0x30, item.code)) return false;
            out.push_back(item);
        }
    }
    std::vector<uint64_t> after(blocks.size());
    if (!r.read(pool.begin, after.data(), after.size() * 8) || blocks != after ||
        !r.read(pool.address, header.data(), header.size()) || header != pool.header) return false;
    return !out.empty() && current_owner(r,pool);
}
struct Monitor {
    const Reader* reader{};
    Pool trains;
    std::vector<Pool> motions;
    HWND status{}, table{};
    HWND source{}, speed{}, apply{}, result{};
};
bool parse_speed(const wchar_t* text, double& kmh) {
    if (!text || !*text) return false;
    std::wstring input(text);
    std::replace(input.begin(), input.end(), L',', L'.');
    wchar_t* end{};
    kmh = std::wcstod(input.c_str(), &end);
    if (end == input.c_str()) return false;
    while (std::iswspace(*end)) ++end;
    return !*end && std::isfinite(kmh) && kmh >= 0 && kmh <= 600;
}
// Re-resolve the full ID immediately before writing. These checks do not make
// the operation atomic with the simulation: this remains an explicit experiment.
bool write_speed(const Reader& reader, HANDLE writer, const Pool& pool, uint64_t id,
                 double kmh, double& before, double& after) {
    if (!std::isfinite(kmh) || kmh < 0 || kmh > 600) return false;
    if (pool.owner.module_base && pool.address != pool.owner.simulation + 0xa0) return false;
    std::vector<Item> rows;
    if (!items(reader, pool, true, rows)) return false;
    const auto found = std::find_if(rows.begin(), rows.end(), [&](const auto& row) { return row.id == id; });
    if (found == rows.end() || !found->present) return false;
    const auto address = found->address + 0x3c8;
    MEMORY_BASIC_INFORMATION region{};
    if (VirtualQueryEx(reader.handle, reinterpret_cast<void*>(address), &region, sizeof region) != sizeof region ||
        region.State != MEM_COMMIT || region.Type != MEM_PRIVATE || region.Protect != PAGE_READWRITE ||
        address + sizeof(double) > reinterpret_cast<uint64_t>(region.BaseAddress) + region.RegionSize) return false;
    std::array<unsigned char, 48> header{};
    uint64_t currentId{}, block{};
    unsigned char present{};
    const auto index = (id >> 16) & 0xffffffffULL;
    if (!reader.read(pool.address, header.data(), header.size()) || header != pool.header ||
        !reader.read(pool.begin + (index >> pool.shift) * 8, &block, 8) ||
        block + (index & pool.mask) * 0x638 != found->address ||
        !reader.read(found->address, &currentId, 8) || currentId != id ||
        !reader.read(found->address + 0x4b0, &present, 1) || present != 1 ||
        !reader.read(address, &before, 8) || !std::isfinite(before)) return false;
    if (!current_owner(reader, pool)) return false;
    const double value = kmh / 3.6;
    SIZE_T written{};
    if (!WriteProcessMemory(writer, reinterpret_cast<void*>(address), &value, sizeof value, &written) || written != sizeof value) return false;
    after = std::nan("");
    if (reader.read(found->address, &currentId, 8) && currentId == id) reader.read(address, &after, 8);
    return true; // Transfer completed, not proof that the simulation accepted it.
}
void apply_speed(Monitor& monitor) {
    const int selected = ListView_GetNextItem(monitor.table, -1, LVNI_SELECTED);
    const auto source = SendMessageW(monitor.source, CB_GETCURSEL, 0, 0);
    wchar_t text[80]{}; GetWindowTextW(monitor.speed, text, 80);
    double kmh{};
    if (selected < 0 || source < 0 || static_cast<size_t>(source) >= monitor.motions.size() || !parse_speed(text, kmh)) {
        SetWindowTextW(monitor.result, L"Selectionner un train, une source et une vitesse entre 0 et 600 km/h."); return;
    }
    LVITEMW row{}; row.mask = LVIF_PARAM; row.iItem = selected;
    if (!SendMessageW(monitor.table, LVM_GETITEMW, 0, reinterpret_cast<LPARAM>(&row))) return;
    const auto id = static_cast<uint64_t>(row.lParam);
    std::vector<Item> trains;
    if (!items(*monitor.reader, monitor.trains, false, trains) ||
        std::none_of(trains.begin(), trains.end(), [&](const auto& t) { return t.id == id; })) {
        SetWindowTextW(monitor.result, L"Refus : train ou collection modifie."); return;
    }
    Reader writer{OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ | PROCESS_VM_WRITE | PROCESS_VM_OPERATION,
                              FALSE, GetProcessId(monitor.reader->handle))};
    FILETIME oldCreated{}, newCreated{}, exited{}, kernel{}, user{};
    std::array<wchar_t, 32768> path{}; DWORD size = static_cast<DWORD>(path.size());
    NimbyBinaryInfo identity{}; DWORD exitCode{};
    if (!writer.handle || !GetProcessTimes(monitor.reader->handle, &oldCreated, &exited, &kernel, &user) ||
        !GetProcessTimes(writer.handle, &newCreated, &exited, &kernel, &user) || CompareFileTime(&oldCreated, &newCreated) != 0 ||
        !GetExitCodeProcess(writer.handle, &exitCode) || exitCode != STILL_ACTIVE ||
        !QueryFullProcessImageNameW(writer.handle, 0, path.data(), &size) ||
        nimby::engine::identify(path.data(), identity) != NIMBY_OK || !identity.recognized_research_build) {
        SetWindowTextW(monitor.result, L"Refus : processus inconnu, termine ou droits d'ecriture indisponibles."); return;
    }
    double before{}, after{};
    const bool written = write_speed(writer, writer.handle, monitor.motions[source], id, kmh, before, after);
    wchar_t result[300]{};
    if (!written) wcscpy(result, L"Ecriture refusee ou echouee : cible modifiee, absente ou non accessible.");
    else if (!std::isfinite(after)) swprintf(result, 300, L"Ecriture envoyee : %.1f km/h. Relecture indisponible ; effet moteur non confirme.", kmh);
    else swprintf(result, 300, L"Source %c : avant %.2f, demande %.2f, relu %.2f km/h. Le moteur peut recalculer la vitesse.",
                  wchar_t(L'A' + source), before * 3.6, kmh, after * 3.6);
    SetWindowTextW(monitor.result, result);
    SYSTEMTIME time{}; GetSystemTime(&time);
    char log[500]{};
    const int length = std::snprintf(log, sizeof log, "%04u-%02u-%02uT%02u:%02u:%02uZ pid=%lu id=0x%llx source=%lld pool=0x%llx written=%d before_ms=%.9g requested_kmh=%.9g readback_ms=%.9g\r\n",
        time.wYear, time.wMonth, time.wDay, time.wHour, time.wMinute, time.wSecond,
        GetProcessId(writer.handle), id, static_cast<long long>(source), monitor.motions[source].address, written, before, kmh, after);
    std::array<wchar_t, 32768> local{};
    const DWORD localSize = GetEnvironmentVariableW(L"LOCALAPPDATA", local.data(), static_cast<DWORD>(local.size()));
    if (localSize && localSize < local.size()) {
        std::wstring dir = std::wstring(local.data()) + L"\\NimbyRailsSDK"; CreateDirectoryW(dir.c_str(), nullptr);
        HANDLE file = CreateFileW((dir + L"\\speed-tests.log").c_str(), FILE_APPEND_DATA, FILE_SHARE_READ, nullptr, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
        if (file != INVALID_HANDLE_VALUE) { DWORD bytes{}; WriteFile(file, log, static_cast<DWORD>(std::clamp(length, 0, static_cast<int>(sizeof log - 1))), &bytes, nullptr); CloseHandle(file); }
    }
}
std::wstring wide(const std::string& value) {
    const auto size = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, value.data(), static_cast<int>(value.size()), nullptr, 0);
    if (!size) return L"?";
    std::wstring result(size, L'\0');
    MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, value.data(), static_cast<int>(value.size()), result.data(), size);
    return result;
}
void update(Monitor& monitor) {
    uint64_t selectedId{};
    const int selected = ListView_GetNextItem(monitor.table, -1, LVNI_SELECTED);
    if (selected >= 0) {
        LVITEMW item{}; item.mask = LVIF_PARAM; item.iItem = selected;
        if (SendMessageW(monitor.table, LVM_GETITEMW, 0, reinterpret_cast<LPARAM>(&item))) selectedId = static_cast<uint64_t>(item.lParam);
    }
    std::vector<Item> trains;
    std::vector<std::map<uint64_t, Item>> sources(monitor.motions.size());
    DWORD exitCode{};
    bool valid = GetExitCodeProcess(monitor.reader->handle, &exitCode) && exitCode == STILL_ACTIVE &&
        items(*monitor.reader, monitor.trains, false, trains);
    for (size_t source = 0; source < monitor.motions.size(); ++source) {
        std::vector<Item> motion;
        if (!items(*monitor.reader, monitor.motions[source], true, motion)) { valid = false; continue; }
        for (const auto& item : motion) sources[source].emplace(item.id, item);
    }
    nimby::engine::Network network;
    const bool networkValid = valid && monitor.trains.owner.module_base &&
        nimby::engine::read_network(live_read,const_cast<Reader*>(monitor.reader),monitor.trains.owner,true,network);
    SendMessageW(monitor.table, WM_SETREDRAW, FALSE, 0);
    ListView_DeleteAllItems(monitor.table);
    if (valid) for (size_t i = 0; i < trains.size(); ++i) {
        auto name = wide(trains[i].name);
        LVITEMW row{}; row.mask = LVIF_TEXT | LVIF_PARAM; row.iItem = static_cast<int>(i); row.pszText = name.data(); row.lParam = static_cast<LPARAM>(trains[i].id);
        SendMessageW(monitor.table, LVM_INSERTITEMW, 0, reinterpret_cast<LPARAM>(&row));
        if (trains[i].id == selectedId) ListView_SetItemState(monitor.table, i, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);
        wchar_t text[80]{};
        swprintf(text, 80, L"%016llX", trains[i].id);
        row.iSubItem = 1; row.pszText = text;
        SendMessageW(monitor.table, LVM_SETITEMTEXTW, i, reinterpret_cast<LPARAM>(&row));
        for (size_t source = 0; source < sources.size(); ++source) {
            const auto found = sources[source].find(trains[i].id);
            if (found == sources[source].end() || !found->second.present) wcscpy(text, L"--");
            else swprintf(text, 80, L"%.1f", found->second.speed * 3.6);
            row.iSubItem = static_cast<int>(source + 2);
            SendMessageW(monitor.table, LVM_SETITEMTEXTW, i, reinterpret_cast<LPARAM>(&row));
        }
        std::array<std::wstring,5> extra{L"--",L"--",L"--",L"--",L"--"};
        if(networkValid && sources.size()==2) {
            const auto motion=sources[1].find(trains[i].id);
            if(motion!=sources[1].end()&&motion->second.positioned) {
                const auto& pos=motion->second.position;
                const auto track=std::find_if(network.tracks.begin(),network.tracks.end(),[&](const auto& t){return t.id==pos.track_id;});
                if(track!=network.tracks.end()) {
                    swprintf(text,80,L"%016llX",pos.track_id);extra[0]=text;
                    swprintf(text,80,L"%.2f %% / %+d",pos.fraction*100,pos.direction);extra[1]=text;
                    swprintf(text,80,L"%.1f",track->limit_mps*3.6);extra[2]=text;
                    extra[3]=L"Hors gare";
                    if(track->station_id) {
                        const auto station=std::find_if(network.stations.begin(),network.stations.end(),[&](const auto& s){return s.id==track->station_id;});
                        if(station!=network.stations.end()&&!station->name.empty()) extra[3]=wide(station->name);
                        else {swprintf(text,80,L"Gare %016llX",track->station_id);extra[3]=text;}
                    }
                    extra[4]=L"Aucun";
                    for(const auto& signal:network.signals) if(signal.track_id==pos.track_id) {
                        if(extra[4]==L"Aucun") extra[4].clear();else extra[4]+=L", ";
                        swprintf(text,80,L"%016llX ",signal.id);extra[4]+=text;extra[4]+=wide(nimby::engine::signal_kind_name(signal.kind));
                    }
                }
            }
        }
        for(size_t column=0;column<extra.size();++column) {
            row.iSubItem=static_cast<int>(sources.size()+2+column);row.pszText=extra[column].data();
            SendMessageW(monitor.table,LVM_SETITEMTEXTW,i,reinterpret_cast<LPARAM>(&row));
        }
    }
    SendMessageW(monitor.table, WM_SETREDRAW, TRUE, 0);
    InvalidateRect(monitor.table, nullptr, TRUE);
    wchar_t status[300]{};
    if (networkValid) swprintf(status,300,L"%zu trains | %zu voies | %zu gares | %zu signaux (%zu balises) | Reseau experimental : lecture seule",trains.size(),network.tracks.size(),network.stations.size(),network.signals.size(),static_cast<size_t>(std::count_if(network.signals.begin(),network.signals.end(),[](const auto& s){return s.kind==3;})));
    else if (valid) swprintf(status, 300, L"%zu trains | Reseau indisponible | Observation 4 fois/s", trains.size());
    else wcscpy(status, L"Donnees indisponibles ou collection modifiee : relancer la detection. Aucune ancienne vitesse conservee.");
    SetWindowTextW(monitor.status, status);
    EnableWindow(monitor.apply, valid);
}
LRESULT CALLBACK window_proc(HWND window, UINT message, WPARAM wparam, LPARAM lparam) {
    auto* monitor = reinterpret_cast<Monitor*>(GetWindowLongPtrW(window, GWLP_USERDATA));
    if (message == WM_CREATE) {
        monitor = static_cast<Monitor*>(reinterpret_cast<CREATESTRUCTW*>(lparam)->lpCreateParams);
        SetWindowLongPtrW(window, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(monitor));
        monitor->status = CreateWindowW(L"STATIC", L"Lecture...", WS_CHILD | WS_VISIBLE, 12, 12, 850, 38, window, nullptr, nullptr, nullptr);
        monitor->table = CreateWindowW(WC_LISTVIEWW, L"", WS_CHILD | WS_VISIBLE | LVS_REPORT | LVS_SINGLESEL,
            12, 132, 850, 270, window, nullptr, nullptr, nullptr);
        monitor->source = CreateWindowW(L"COMBOBOX", L"", WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST, 12, 52, 145, 150, window, nullptr, nullptr, nullptr);
        for (size_t i = 0; i < monitor->motions.size(); ++i) {
            const std::wstring label = monitor->motions[i].owner.module_base ? (i == 1 ? L"Simulation" : L"Copie (lecture)") : std::wstring(L"Source ") + wchar_t(L'A' + i);
            SendMessageW(monitor->source, CB_ADDSTRING, 0, reinterpret_cast<LPARAM>(label.c_str()));
        }
        if (monitor->motions.size() == 2 && monitor->motions[1].owner.module_base) SendMessageW(monitor->source, CB_SETCURSEL, 1, 0);
        monitor->speed = CreateWindowExW(WS_EX_CLIENTEDGE, L"EDIT", L"", WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL, 168, 52, 95, 26, window, nullptr, nullptr, nullptr);
        SendMessageW(monitor->speed, EM_SETLIMITTEXT, 30, 0);
        CreateWindowW(L"STATIC", L"km/h", WS_CHILD | WS_VISIBLE, 270, 56, 50, 24, window, nullptr, nullptr, nullptr);
        monitor->apply = CreateWindowW(L"BUTTON", L"Appliquer une fois", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 330, 50, 185, 30, window, reinterpret_cast<HMENU>(1001), nullptr, nullptr);
        monitor->result = CreateWindowW(L"STATIC", L"Test experimental : choisir une source A/B et un train. La simulation peut ecraser la valeur.", WS_CHILD | WS_VISIBLE, 12, 88, 850, 38, window, nullptr, nullptr, nullptr);
        for (HWND control : {monitor->source, monitor->speed, monitor->apply, monitor->result}) SendMessageW(control, WM_SETFONT, reinterpret_cast<WPARAM>(GetStockObject(DEFAULT_GUI_FONT)), TRUE);
        SendMessageW(monitor->table, LVM_SETEXTENDEDLISTVIEWSTYLE, 0, LVS_EX_FULLROWSELECT | LVS_EX_DOUBLEBUFFER);
        SendMessageW(monitor->status, WM_SETFONT, reinterpret_cast<WPARAM>(GetStockObject(DEFAULT_GUI_FONT)), TRUE);
        SendMessageW(monitor->table, WM_SETFONT, reinterpret_cast<WPARAM>(GetStockObject(DEFAULT_GUI_FONT)), TRUE);
        std::vector<std::wstring> columns{L"Train", L"Identifiant"};
        for (size_t i = 0; i < monitor->motions.size(); ++i) columns.push_back(monitor->motions[i].owner.module_base ? (i == 1 ? L"Simulation (km/h)" : L"Copie (km/h)") : std::wstring(L"Vitesse ") + wchar_t(L'A' + i) + L" (km/h)");
        for(const auto* label:{L"Voie actuelle",L"Position / sens",L"Limite voie (km/h)",L"Gare de la voie",L"Signaux sur cette voie"}) columns.emplace_back(label);
        for (size_t i = 0; i < columns.size(); ++i) {
            LVCOLUMNW column{}; column.mask = LVCF_TEXT | LVCF_WIDTH; column.cx = i==0?110:(i==7?190:(i==8?330:145)); column.pszText = columns[i].data();
            SendMessageW(monitor->table, LVM_INSERTCOLUMNW, i, reinterpret_cast<LPARAM>(&column));
        }
        update(*monitor); SetTimer(window, 1, 250, nullptr); return 0;
    }
    if (message == WM_TIMER && monitor) { update(*monitor); return 0; }
    if (message == WM_COMMAND && LOWORD(wparam) == 1001 && HIWORD(wparam) == BN_CLICKED && monitor) { apply_speed(*monitor); return 0; }
    if (message == WM_SIZE && monitor) {
        MoveWindow(monitor->status, 12, 12, std::max(1, static_cast<int>(LOWORD(lparam)) - 24), 38, TRUE);
        MoveWindow(monitor->result, 12, 88, std::max(1, static_cast<int>(LOWORD(lparam)) - 24), 38, TRUE);
        MoveWindow(monitor->table, 12, 132, std::max(1, static_cast<int>(LOWORD(lparam)) - 24), std::max(1, static_cast<int>(HIWORD(lparam)) - 144), TRUE); return 0;
    }
    if (message == WM_DESTROY) { KillTimer(window, 1); PostQuitMessage(0); return 0; }
    return DefWindowProcW(window, message, wparam, lparam);
}
HWND create_monitor(Monitor& monitor) {
    INITCOMMONCONTROLSEX controls{sizeof controls, ICC_LISTVIEW_CLASSES}; InitCommonControlsEx(&controls);
    WNDCLASSW cls{}; cls.lpfnWndProc = window_proc; cls.hInstance = GetModuleHandleW(nullptr);
    cls.lpszClassName = L"NimbyTrainResearchMonitor"; cls.hCursor = LoadCursorW(nullptr, IDC_ARROW);
    cls.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1);
    if (!RegisterClassW(&cls)) return nullptr;
    return CreateWindowW(cls.lpszClassName, L"NimbyRailsSDK - Observation des trains (experimental)", WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 1500, 460, nullptr, nullptr, cls.hInstance, &monitor);
}
int show_monitor(const Reader& reader, const Pool& trainPool, const std::vector<Pool>& motionPools) {
    Monitor monitor{&reader, trainPool, motionPools};
    HWND window = create_monitor(monitor);
    if (!window) return 6;
    ShowWindow(window, SW_SHOW); UpdateWindow(window);
    std::printf("MONITOR visible=%d rows=%d refresh_ms=250\n", IsWindowVisible(window), ListView_GetItemCount(monitor.table));
    std::fflush(stdout);
    MSG message{};
    while (GetMessageW(&message, nullptr, 0, 0) > 0) { TranslateMessage(&message); DispatchMessageW(&message); }
    return 0;
}
int self_test() {
    Reader reader{OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, GetCurrentProcessId())};
    if (!reader.handle) return 1;
    auto put = [](unsigned char* p, size_t off, auto value) { std::memcpy(p + off, &value, sizeof value); };
    std::array<unsigned char, 48> header{};
    Bytes motion(512 * 0x638);
    std::array<uint64_t, 1> blocks{reinterpret_cast<uint64_t>(motion.data())};
    put(header.data(), 4, uint32_t(9)); put(header.data(), 8, uint32_t(512)); put(header.data(), 16, uint32_t(511));
    put(header.data(), 24, reinterpret_cast<uint64_t>(blocks.data()));
    put(header.data(), 32, reinterpret_cast<uint64_t>(blocks.data() + 1));
    put(header.data(), 40, reinterpret_cast<uint64_t>(blocks.data() + 1));
    Pool pool{};
    if (!decode(header.data(), pool)) return 10;
    pool.address = reinterpret_cast<uint64_t>(header.data()); pool.header = header;
    put(motion.data(), 0, uint64_t(0x5000000000001));
    put(motion.data(), 0x3c8, 12.5); motion[0x4b0] = 1;
    std::vector<Item> rows;
    if (!items(reader, pool, true, rows) || rows.size() != 1 || rows[0].speed != 12.5) return 11;
    // Wrong ID index must be rejected even if its type tag and speed are plausible.
    put(motion.data(), 0, uint64_t(0x5000000010001));
    if (items(reader, pool, true, rows)) return 12;
    put(motion.data(), 0, uint64_t(0x5000000000001));
    put(motion.data(), 0x3c8, std::nan(""));
    if (items(reader, pool, true, rows)) return 13;
    motion[0x4b0] = 0;
    if (!items(reader, pool, true, rows) || rows[0].present) return 14;
    header[8] ^= 1;
    if (decode(header.data(), pool) || items(reader, pool, true, rows)) return 15;
    header[8] ^= 1;
    if (!decode(header.data(), pool)) return 16;
    put(motion.data(), 0x3c8, 12.5); motion[0x4b0] = 1;
    Bytes train(512 * 0x178);
    put(train.data(), 0, uint64_t(0x5000000000001));
    std::memcpy(train.data() + 0x10, "Test", 5);
    put(train.data(), 0x20, uint64_t(4)); put(train.data(), 0x28, uint64_t(15));
    put(train.data(), 0x40, uint64_t(0)); put(train.data(), 0x48, uint64_t(15));
    std::array<uint64_t, 1> trainBlocks{reinterpret_cast<uint64_t>(train.data())};
    auto trainHeader = header;
    put(trainHeader.data(), 24, reinterpret_cast<uint64_t>(trainBlocks.data()));
    put(trainHeader.data(), 32, reinterpret_cast<uint64_t>(trainBlocks.data() + 1));
    put(trainHeader.data(), 40, reinterpret_cast<uint64_t>(trainBlocks.data() + 1));
    Pool trainPool{}; if (!decode(trainHeader.data(), trainPool)) return 17;
    trainPool.header = trainHeader; trainPool.address = reinterpret_cast<uint64_t>(trainHeader.data());
    Monitor monitor{&reader, trainPool, {pool}};
    HWND window = create_monitor(monitor); // Hidden synthetic UI; no game process involved.
    if (!window) return 18;
    wchar_t cell[40]{};
    LVITEMW request{}; request.iSubItem = 2; request.pszText = cell; request.cchTextMax = 40;
    SendMessageW(monitor.table, LVM_GETITEMTEXTW, 0, reinterpret_cast<LPARAM>(&request));
    const bool rowCorrect = ListView_GetItemCount(monitor.table) == 1 && std::wcscmp(cell, L"45.0") == 0;
    ListView_SetItemState(monitor.table, 0, LVIS_SELECTED, LVIS_SELECTED);
    update(monitor);
    if (ListView_GetNextItem(monitor.table, -1, LVNI_SELECTED) != 0 ||
        SendMessageW(monitor.source, CB_GETCURSEL, 0, 0) != CB_ERR) return 20;
    double parsed{};
    if (!parse_speed(L"30,5", parsed) || parsed != 30.5 || parse_speed(L"nan", parsed) ||
        parse_speed(L"", parsed) || parse_speed(L"20oops", parsed) || parse_speed(L"601", parsed) || parse_speed(L"-1", parsed)) return 21;
    Reader writer{OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ | PROCESS_VM_WRITE | PROCESS_VM_OPERATION, FALSE, GetCurrentProcessId())};
    double before{}, after{};
    const Bytes baseline = motion;
    if (!write_speed(reader, writer.handle, pool, 0x5000000000001, 72, before, after) || before != 12.5 || after != 20) return 22;
    // Exactly eight bytes may differ; no second source or other train is touched.
    for (size_t i = 0; i < motion.size(); ++i) if ((i < 0x3c8 || i >= 0x3d0) && motion[i] != baseline[i]) return 23;
    if (write_speed(reader, writer.handle, pool, 0x5000000000002, 30, before, after) ||
        write_speed(reader, writer.handle, pool, 0x5000000000001, std::nan(""), before, after)) return 24;
    motion[0x4b0] = 0;
    if (write_speed(reader, writer.handle, pool, 0x5000000000001, 30, before, after)) return 25;
    motion[0x4b0] = 1;
    header[8] ^= 1;
    if (write_speed(reader, writer.handle, pool, 0x5000000000001, 30, before, after)) return 26;
    header[8] ^= 1;
    // The button path itself must refuse our unrecognized synthetic host.
    SendMessageW(monitor.source, CB_SETCURSEL, 0, 0); SetWindowTextW(monitor.speed, L"30");
    apply_speed(monitor);
    if (at<double>(motion.data(), 0x3c8) != 20) return 27;
    // Real local memory with a synthetic relocated global verifies that writes
    // require the current owner and reject the read-only copy collection.
    alignas(8) std::array<unsigned char, 0x688> rootBytes{};
    uint64_t rootSlot = reinterpret_cast<uint64_t>(rootBytes.data());
    const auto fakeBase = reinterpret_cast<uint64_t>(&rootSlot) - 0xb81998;
    put(rootBytes.data(), 0x540, trainPool.address - 0x200);
    put(rootBytes.data(), 0x5c0, pool.address - 0xa0);
    put(rootBytes.data(), 0x680, pool.address + 0x1000);
    if (!nimby::engine::resolve_live_state(live_read, &reader, fakeBase, true, pool.owner)) return 28;
    if (write_speed(reader, writer.handle, pool, 0x5000000000001, 30, before, after)) return 29;
    put(rootBytes.data(), 0x5c0, pool.address + 0x1000);
    put(rootBytes.data(), 0x680, pool.address - 0xa0);
    if (!nimby::engine::resolve_live_state(live_read, &reader, fakeBase, true, pool.owner) ||
        !write_speed(reader, writer.handle, pool, 0x5000000000001, 36, before, after) || after != 10) return 41;
    rootSlot = 0;
    if (write_speed(reader, writer.handle, pool, 0x5000000000001, 50, before, after) || at<double>(motion.data(), 0x3c8) != 10) return 42;
    trainHeader[8] ^= 1;
    update(monitor);
    const bool staleCleared = ListView_GetItemCount(monitor.table) == 0;
    DestroyWindow(window);
    if (!rowCorrect || !staleCleared) return 19;
    std::puts("Synthetic tests passed: observation, guarded single-double write, stale ID/header/owner, copy refusal, unknown-host refusal.");
    return 0;
}
int experiment(const Reader& reader, const Pool& trainPool, const std::vector<Pool>& pools, uint64_t id, size_t source) {
    if (pools.size() != 2 || source >= pools.size()) return 30;
    std::vector<Item> trains;
    if (!items(reader, trainPool, false, trains)) return 31;
    const auto train = std::find_if(trains.begin(), trains.end(), [&](const auto& t) { return t.id == id; });
    if (train == trains.end()) return 32;
    Reader writer{OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ | PROCESS_VM_WRITE | PROCESS_VM_OPERATION, FALSE, GetProcessId(reader.handle))};
    FILETIME a{}, b{}, exit{}, kernel{}, user{};
    if (!writer.handle || !GetProcessTimes(reader.handle, &a, &exit, &kernel, &user) ||
        !GetProcessTimes(writer.handle, &b, &exit, &kernel, &user) || CompareFileTime(&a, &b)) return 33;
    auto speeds = [&]() {
        std::array<double, 2> result{std::nan(""), std::nan("")};
        for (size_t i = 0; i < 2; ++i) {
            std::vector<Item> motion;
            if (!items(reader, pools[i], true, motion)) continue;
            auto found = std::find_if(motion.begin(), motion.end(), [&](const auto& t) { return t.id == id; });
            if (found != motion.end() && found->present) result[i] = found->speed * 3.6;
        }
        return result;
    };
    SYSTEMTIME utc{}; GetSystemTime(&utc);
    std::printf("EXPERIMENT utc=%04u-%02u-%02uT%02u:%02u:%02uZ id=0x%llx name=%s source=%zu pool=0x%llx target_kmh=5 single_write=1\n",
        utc.wYear, utc.wMonth, utc.wDay, utc.wHour, utc.wMinute, utc.wSecond, id, train->name.c_str(), source, pools[source].address);
    const auto baseline = std::chrono::steady_clock::now();
    for (unsigned n = 0; n < 20; ++n) {
        const auto values = speeds();
        std::printf("TRACE,baseline,%lld,%.6f,%.6f\n", static_cast<long long>(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - baseline).count()), values[0], values[1]);
        if (!std::isfinite(values[0]) || !std::isfinite(values[1])) return 34;
        Sleep(50);
    }
    const auto values = speeds();
    if (!std::isfinite(values[source]) || values[source] < 20) {
        std::printf("EXPERIMENT skipped: source speed below 20 km/h; no write\n"); return 35;
    }
    double before{}, after{};
    const auto start = std::chrono::steady_clock::now();
    if (!write_speed(reader, writer.handle, pools[source], id, 5, before, after)) return 36;
    std::printf("WRITE before_kmh=%.6f requested_kmh=5 readback_kmh=%.6f\n", before * 3.6, after * 3.6);
    for (unsigned n = 0; n < 101; ++n) {
        const auto sample = speeds();
        const auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - start).count();
        std::printf("TRACE,after,%lld,%.6f,%.6f\n", static_cast<long long>(elapsed), sample[0], sample[1]);
        std::fflush(stdout);
        if (!std::isfinite(sample[0]) || !std::isfinite(sample[1])) return 37;
        if (n < 100) Sleep(50);
    }
    return 0;
}
void find_roots(const Reader& reader, const Pool& trains, const std::vector<Pool>& motions) {
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, GetProcessId(reader.handle));
    MODULEENTRY32W module{}; module.dwSize = sizeof module;
    if (snapshot == INVALID_HANDLE_VALUE) return;
    const bool found = Module32FirstW(snapshot, &module); CloseHandle(snapshot); if (!found) return;
    const auto base = reinterpret_cast<uint64_t>(module.modBaseAddr);
    IMAGE_DOS_HEADER dos{}; IMAGE_NT_HEADERS64 nt{};
    if (!reader.read(base, &dos, sizeof dos) || !reader.read(base + dos.e_lfanew, &nt, sizeof nt)) return;
    std::map<uint64_t, std::string> targets{{trains.address - 0x200, "database"}};
    for (size_t i=0;i<motions.size();++i) targets[motions[i].address - 0xa0] = "simulation_" + std::to_string(i);
    auto match = [&](uint64_t value, uint64_t slot, int offset) {
        auto target=targets.find(value); if(target!=targets.end()) std::printf("ROOT candidate module_rva=0x%llx dereference_offset=%d target=%s value=0x%llx\n", slot-base, offset, target->second.c_str(), value);
    };
    const auto sections = base + dos.e_lfanew + 24 + nt.FileHeader.SizeOfOptionalHeader;
    for (unsigned s=0;s<nt.FileHeader.NumberOfSections;++s) {
        IMAGE_SECTION_HEADER section{}; if(!reader.read(sections+s*sizeof section,&section,sizeof section)) return;
        if(!(section.Characteristics & IMAGE_SCN_MEM_WRITE) || section.Misc.VirtualSize > 16*1024*1024) continue;
        Bytes bytes(section.Misc.VirtualSize); if(!reader.read(base+section.VirtualAddress,bytes.data(),bytes.size())) continue;
        for(size_t off=0;off+8<=bytes.size();off+=8) {
            auto value=at<uint64_t>(bytes.data(),off); const auto slot=base+section.VirtualAddress+off; match(value,slot,-1);
            MEMORY_BASIC_INFORMATION region{};
            if(VirtualQueryEx(reader.handle,reinterpret_cast<void*>(value),&region,sizeof region)!=sizeof region || region.State!=MEM_COMMIT || region.Type!=MEM_PRIVATE || region.Protect!=PAGE_READWRITE) continue;
            const auto length=std::min<uint64_t>(0x4000,reinterpret_cast<uint64_t>(region.BaseAddress)+region.RegionSize-value);
            Bytes object(length); if(!reader.read(value,object.data(),object.size())) continue;
            for(size_t n=0;n+8<=object.size();n+=8) match(at<uint64_t>(object.data(),n),slot,static_cast<int>(n));
        }
    }
}
int run(int argc, char** argv) {
    if (argc == 1) {
        HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
        PROCESSENTRY32W entry{}; entry.dwSize = sizeof entry;
        DWORD gamePid{}; unsigned count{};
        if (snapshot != INVALID_HANDLE_VALUE) {
            if (Process32FirstW(snapshot, &entry)) do {
                if (_wcsicmp(entry.szExeFile, L"NimbyRails.exe") == 0) { gamePid = entry.th32ProcessID; ++count; }
            } while (Process32NextW(snapshot, &entry));
            CloseHandle(snapshot);
        }
        if (count != 1) { MessageBoxW(nullptr, L"Ouvrir une seule instance de NIMBY Rails avec une partie chargee, puis relancer le moniteur.", L"NimbyRailsSDK", MB_OK); return 2; }
        char pidText[20]{}; std::snprintf(pidText, sizeof pidText, "%lu", gamePid);
        char windowFlag[] = "--window";
        char* arguments[]{argv[0], pidText, windowFlag};
        const auto result = run(3, arguments);
        if (result) MessageBoxW(nullptr, L"Observation indisponible : version inconnue, lecture impossible ou collections ambigues. Consulter le rapport de recherche.", L"NimbyRailsSDK", MB_OK | MB_ICONWARNING);
        return result;
    }
    if (argc == 2 && std::strcmp(argv[1], "--self-test") == 0) return self_test();
    const bool window = argc == 3 && std::strcmp(argv[2], "--window") == 0;
    const bool sample = argc == 3 && std::strcmp(argv[2], "--sample") == 0;
    const bool roots = argc == 3 && std::strcmp(argv[2], "--roots") == 0;
    const bool trial = argc == 5 && std::strcmp(argv[2], "--experiment") == 0;
    uint64_t trialId{}; unsigned long trialSource{};
    if (trial) {
        char* end{}; trialId = std::strtoull(argv[3], &end, 0);
        if (!trialId || *end) return 2;
        trialSource = std::strtoul(argv[4], &end, 10);
        if (*end || !*argv[4] || trialSource > 1) return 2;
    }
    if (argc != 2 && !window && !sample && !trial && !roots) { std::fprintf(stderr, "Usage: NimbyTrainMonitor <pid> [--window|--sample|--roots|--experiment <id> <0|1>]\n"); return 2; }
    char* tail{}; const auto pid = std::strtoul(argv[1], &tail, 10);
    if (!pid || *tail || pid > MAXDWORD) return 2;
    Reader r{OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, static_cast<DWORD>(pid))};
    if (!r.handle) { std::fprintf(stderr, "OpenProcess failed: %lu\n", GetLastError()); return 3; }
    std::array<wchar_t, 32768> path{}; DWORD pathSize = static_cast<DWORD>(path.size());
    NimbyBinaryInfo identity{};
    if (!QueryFullProcessImageNameW(r.handle, 0, path.data(), &pathSize) ||
        nimby::engine::identify(path.data(), identity) != NIMBY_OK || !identity.recognized_research_build) {
        std::fprintf(stderr, "REFUSED: unknown executable\n"); return 4;
    }
    std::printf("Research PID=%lu SHA256=%s\n", pid, identity.sha256);
    const auto started = std::chrono::steady_clock::now();
    constexpr size_t chunk = 1024 * 1024;
    Bytes buffer(chunk + 48);
    uint64_t address = 0, scanned = 0;
    bool complete = true;
    std::set<uint64_t> visited;
    std::vector<Pool> trains, motions;
    std::vector<Item> rows;
    MEMORY_BASIC_INFORMATION region{};
    if (!roots) {
        nimby::engine::LiveState state{};
        const auto base = image_base(r);
        if (!nimby::engine::resolve_live_state(live_read, &r, base, identity.recognized_research_build, state)) {
            std::fprintf(stderr, "REFUSED: live state absent or changing; load a game and retry\n"); return 39;
        }
        auto load = [&](uint64_t location, bool motion, std::vector<Pool>& pools) {
            Pool pool{};
            if (!r.read(location, pool.header.data(), pool.header.size()) || !decode(pool.header.data(), pool)) return false;
            pool.address = location; pool.owner = state;
            if (!items(r, pool, motion, rows)) return false;
            pools.push_back(pool); return true;
        };
        if (!load(state.database + 0x200, false, trains) || !load(state.copy + 0xa0, true, motions) || !load(state.simulation + 0xa0, true, motions)) {
            std::fprintf(stderr, "REFUSED: owned collections invalid or empty\n"); return 40;
        }
        std::printf("RESOLVED base=0x%llx root=0x%llx database=0x%llx copy=0x%llx simulation=0x%llx global_rva=0xb81998\n",
            state.module_base,state.root,state.database,state.copy,state.simulation);
        items(r,trains[0],false,rows);
        for(const auto& row: rows) std::printf("TRAIN id=0x%llx name=%s\n", row.id,row.name.c_str());
    }
    while (roots && VirtualQueryEx(r.handle, reinterpret_cast<void*>(address), &region, sizeof region) == sizeof region) {
        const auto base = reinterpret_cast<uint64_t>(region.BaseAddress);
        const auto next = base + region.RegionSize;
        if (next <= address) break;
        if (region.State == MEM_COMMIT && region.Type == MEM_PRIVATE &&
            !(region.Protect & (PAGE_GUARD | PAGE_NOACCESS)) &&
            (region.Protect & (PAGE_READWRITE | PAGE_WRITECOPY))) {
            for (uint64_t start = base; start < next; start += chunk) {
                if (scanned > 4ULL * 1024 * 1024 * 1024 ||
                    std::chrono::steady_clock::now() - started > std::chrono::seconds(60)) { complete = false; break; }
                const auto length = static_cast<size_t>(std::min<uint64_t>(buffer.size(), next - start));
                scanned += length;
                if (!r.read(start, buffer.data(), length)) continue;
                for (size_t offset = 0; offset + 48 <= length && offset < chunk; offset += 8) {
                    Pool pool{};
                    if (!decode(buffer.data() + offset, pool) || !visited.insert(start + offset).second) continue;
                    pool.address = start + offset;
                    std::memcpy(pool.header.data(), buffer.data() + offset, pool.header.size());
                    for (const bool motion : {false, true}) {
                        if (!items(r, pool, motion, rows)) continue;
                        (motion ? motions : trains).push_back(pool);
                        std::printf("CANDIDATE %s pool=0x%llx blocks=%llu slots=%u live=%zu\n", motion ? "Motion" : "Train", pool.address,
                            (pool.end - pool.begin) / 8, pool.size, rows.size());
                        for (size_t n = 0; n < std::min<size_t>(rows.size(), 8); ++n) {
                            const auto& item = rows[n];
                            if (motion) std::printf("  id=0x%llx address=0x%llx presence=%d raw_speed=%.9g\n", item.id, item.address, item.present, item.speed);
                            else std::printf("  id=0x%llx address=0x%llx name=%s code=%s\n", item.id, item.address, item.name.c_str(), item.code.c_str());
                        }
                        std::fflush(stdout);
                    }
                }
            }
        }
        if (!complete) break;
        address = next;
    }
    std::printf("Discovery scanned=%llu MiB limit_reached=%d train_candidates=%zu motion_candidates=%zu\n",
        scanned / (1024 * 1024), !complete, trains.size(), motions.size());
    // UI function RVA 0x804000 obtains world owner at +0x258 and simulation at
    // +0x260; world owner +0x890 points to the database containing Train pool +0x200.
    // Correlation only: a matching pair is not proof that a UI call reached it.
    if (roots && complete && trains.size() == 1 && !motions.empty()) {
        address = 0;
        while (VirtualQueryEx(r.handle, reinterpret_cast<void*>(address), &region, sizeof region) == sizeof region) {
            const auto base = reinterpret_cast<uint64_t>(region.BaseAddress), next = base + region.RegionSize;
            if (next <= address || std::chrono::steady_clock::now() - started > std::chrono::seconds(60)) break;
            if (region.State == MEM_COMMIT && region.Type == MEM_PRIVATE &&
                !(region.Protect & (PAGE_GUARD | PAGE_NOACCESS)) && (region.Protect & PAGE_READWRITE)) {
                for (uint64_t start = base; start < next; start += chunk) {
                    const auto length = static_cast<size_t>(std::min<uint64_t>(buffer.size(), next - start));
                    if (!r.read(start, buffer.data(), length)) continue;
                    for (size_t offset = 0; offset + 16 <= length && offset < chunk; offset += 8) {
                        const auto sim = at<uint64_t>(buffer.data() + offset, 8);
                        for (const auto& motion : motions) {
                            if (sim + 0xa0 != motion.address) continue;
                            const auto owner = at<uint64_t>(buffer.data() + offset, 0);
                            uint64_t database{}, vtable{};
                            const auto context = start + offset - 0x258;
                            if (r.read(owner + 0x890, &database, 8) && database + 0x200 == trains[0].address &&
                                r.read(context, &vtable, 8)) {
                                MEMORY_BASIC_INFORMATION vtRegion{};
                                if (VirtualQueryEx(r.handle, reinterpret_cast<void*>(vtable), &vtRegion, sizeof vtRegion) != sizeof vtRegion ||
                                    vtRegion.Type != MEM_IMAGE || !(vtRegion.Protect & PAGE_READONLY)) continue;
                                std::printf("CONTEXT_CANDIDATE address=0x%llx vtable=0x%llx world_owner=0x%llx motion_pool=0x%llx\n",
                                    context, vtable, owner, motion.address);
                            }
                        }
                    }
                }
            }
            address = next;
        }
    }
    std::printf("Candidates require UI correlation and lifetime/thread validation; no hooks installed.\n");
    if (roots && complete && trains.size() == 1) find_roots(r, trains[0], motions);
    if (trial) {
        if (!complete || trains.size() != 1) return 38;
        return experiment(r, trains[0], motions, trialId, trialSource);
    }
    if (sample && complete && trains.size() == 1 && !motions.empty()) {
        nimby::engine::Network network;
        if(nimby::engine::read_network(live_read,&r,trains[0].owner,true,network))
            std::printf("NETWORK tracks=%zu stations=%zu signals=%zu balises=%zu\n",network.tracks.size(),network.stations.size(),network.signals.size(),static_cast<size_t>(std::count_if(network.signals.begin(),network.signals.end(),[](const auto& s){return s.kind==3;})));
        for (unsigned tick = 0; tick < 5; ++tick) {
            std::vector<Item> trainRows;
            if (!items(r, trains[0], false, trainRows)) return 7;
            for (size_t source = 0; source < motions.size(); ++source) {
                if (!items(r, motions[source], true, rows)) return 7;
                for (const auto& item : rows) {
                    const auto train = std::find_if(trainRows.begin(), trainRows.end(), [&](const auto& t) { return t.id == item.id; });
                    if (train != trainRows.end()) std::printf("SAMPLE tick=%u source=%zu id=0x%llx name=%s presence=%d km_h=%.6f position_valid=%d track=0x%llx fraction=%.9f direction=%d\n",
                        tick, source, item.id, train->name.c_str(), item.present, item.present ? item.speed * 3.6 : std::nan(""),item.positioned,item.position.track_id,item.position.fraction,item.position.direction);
                }
            }
            std::fflush(stdout); if (tick < 4) Sleep(500);
        }
    }
    if (window) {
        if (!complete || trains.size() != 1 || motions.empty() || motions.size() > 4) {
            std::fprintf(stderr, "Monitor refused: discovery incomplete or ambiguous train database.\n"); return 5;
        }
        return show_monitor(r, trains[0], motions);
    }
    return 0;
}
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) { return run(__argc, __argv); }
