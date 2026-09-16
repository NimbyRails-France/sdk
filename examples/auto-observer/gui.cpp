#include <windows.h>
#include <commctrl.h>
#include <nimby/client.hpp>
#include <algorithm>
#include <array>
#include <condition_variable>
#include <cwctype>
#include <mutex>
#include <thread>
#include <vector>

namespace {
using namespace std::chrono_literals;
std::wstring wide(const std::string& text) {
    if(text.empty())return {};
    const auto length=MultiByteToWideChar(CP_UTF8,0,text.data(),static_cast<int>(text.size()),nullptr,0);
    std::wstring result(length,L' ');
    MultiByteToWideChar(CP_UTF8,0,text.data(),static_cast<int>(text.size()),result.data(),length);
    return result;
}
std::wstring lower(std::wstring text){for(auto& c:text)c=static_cast<wchar_t>(std::towlower(c));return text;}
std::wstring number(double value){wchar_t text[64];swprintf_s(text,L"%.1f",value);return text;}
std::wstring serviceName(const nimby::TrainService& service){
    const std::string name=service.getStatusName();
    if(name=="driving")return L"En circulation";
    if(name=="stopped at station")return L"Arrêt en gare";
    if(name=="timed stop")return L"Arrêt temporisé";
    if(name=="at depot (hidden)")return L"Au dépôt (masqué)";
    if(name=="waiting for dispatch")return L"Attente de départ";
    if(name=="waiting at signal")return L"Arrêt au signal";
    if(name=="mothballed")return L"Remisé";
    if(name=="not present on tracks")return L"Hors réseau";
    return L"Inconnu";
}
using Row=std::array<std::wstring,8>;
struct App {
    HWND window{},status{},clock{},date{},time{},apply{},current{},notice{},message{},view{},filter{},list{},count{};
    HFONT font{};
    std::mutex mutex;
    std::condition_variable wake;
    bool stop=false,connected=false,busy=false;
    std::optional<std::chrono::sys_seconds> requested;
    nimby::Snapshot::Ptr latest,shown;
    std::wstring connection=L"Connexion au jeu…",feedback;
    std::thread worker;
    std::vector<Row> rows;
    bool datesInitialized=false;
    int mode=0;

    void start(std::uint32_t pid) {
        worker=std::thread([this,pid]{
            while(true){
                {std::lock_guard lock(mutex);if(stop)return;}
                try {
                    auto client=pid?nimby::Client::connect(pid):nimby::Client::connect();
                    while(true){
                        std::optional<std::chrono::sys_seconds> command;
                        {std::lock_guard lock(mutex);if(stop)return;command=std::exchange(requested,std::nullopt);}
                        if(command){
                            try {
                                auto result=client.setSimulationDateTimeAndRecalculateTrains(*command);
                                std::lock_guard lock(mutex);
                                feedback=L"Date appliquée : "+wide(result.clock.getDateTimeUtcString())+L" — intervention sur "+std::to_wstring(result.interventions)+L" trains. Reprise selon les horaires du jeu.";
                            } catch(const std::exception& error){
                                std::lock_guard lock(mutex);feedback=L"Modification refusée : "+wide(error.what());
                            }
                            {std::lock_guard lock(mutex);busy=false;}
                        }
                        auto snapshot=client.capture();
                        {
                            std::unique_lock lock(mutex);
                            latest=std::move(snapshot);connected=true;
                            connection=L"Connecté au jeu · PID "+std::to_wstring(latest->getProcessId())+L" · actualisation automatique";
                            wake.wait_for(lock,1s,[this]{return stop||requested.has_value();});
                            if(stop)return;
                        }
                    }
                }catch(const std::exception& error){
                    std::unique_lock lock(mutex);
                    connected=false;latest.reset();
                    if(busy){feedback=L"Date non appliquée : connexion interrompue.";busy=false;requested.reset();}
                    connection=L"En attente du jeu / d’une partie chargée : "+wide(error.what());
                    wake.wait_for(lock,3s,[this]{return stop;});
                    if(stop)return;
                }
            }
        });
    }
    ~App(){
        {std::lock_guard lock(mutex);stop=true;}wake.notify_all();
        if(worker.joinable())worker.join();
        if(font)DeleteObject(font);
    }
    HWND control(const wchar_t* type,const wchar_t* text,DWORD style,int id=0){
        auto h=CreateWindowExW(0,type,text,WS_CHILD|WS_VISIBLE|style,0,0,10,10,window,
                              reinterpret_cast<HMENU>(static_cast<INT_PTR>(id)),GetModuleHandleW(nullptr),nullptr);
        if(!h)throw std::runtime_error("Cannot create observer control");
        SendMessageW(h,WM_SETFONT,reinterpret_cast<WPARAM>(font),TRUE);return h;
    }
    void create(){
        font=CreateFontW(-16,0,0,0,FW_NORMAL,FALSE,FALSE,FALSE,DEFAULT_CHARSET,OUT_DEFAULT_PRECIS,
                         CLIP_DEFAULT_PRECIS,CLEARTYPE_QUALITY,DEFAULT_PITCH,L"Segoe UI");
        status=control(L"STATIC",L"Connexion au jeu…",SS_LEFT);
        clock=control(L"STATIC",L"Date de simulation (UTC) : indisponible",SS_LEFT);
        date=control(DATETIMEPICK_CLASSW,L"",WS_TABSTOP|DTS_SHORTDATECENTURYFORMAT,101);
        time=control(DATETIMEPICK_CLASSW,L"",WS_TABSTOP|DTS_TIMEFORMAT,102);
        DateTime_SetFormat(date,L"dd/MM/yyyy");DateTime_SetFormat(time,L"HH:mm:ss");
        current=control(L"BUTTON",L"Date actuelle",WS_TABSTOP,103);
        apply=control(L"BUTTON",L"Appliquer et recaler les trains",WS_TABSTOP,104);
        notice=control(L"STATIC",L"Déclenche « All trains intervention » : services recalculés, trains replacés à leur prochaine destination et voyageurs transférés. Coût normal d’intervention du jeu.",SS_LEFT);
        message=control(L"STATIC",L"Fonction expérimentale. Date et heure saisies en UTC.",SS_LEFT);
        view=control(L"COMBOBOX",L"",WS_TABSTOP|CBS_DROPDOWNLIST,105);
        SendMessageW(view,CB_ADDSTRING,0,reinterpret_cast<LPARAM>(L"Trains"));
        SendMessageW(view,CB_ADDSTRING,0,reinterpret_cast<LPARAM>(L"Gares / quais"));
        SendMessageW(view,CB_SETCURSEL,0,0);
        filter=control(L"EDIT",L"",WS_TABSTOP|WS_BORDER|ES_AUTOHSCROLL,106);
        SendMessageW(filter,EM_SETCUEBANNER,TRUE,reinterpret_cast<LPARAM>(L"Rechercher un train, une gare, un quai ou un identifiant…"));
        SendMessageW(filter,EM_SETLIMITTEXT,256,0);
        list=control(WC_LISTVIEWW,L"",WS_TABSTOP|WS_BORDER|LVS_REPORT|LVS_OWNERDATA|LVS_SHOWSELALWAYS,107);
        ListView_SetExtendedListViewStyle(list,LVS_EX_FULLROWSELECT|LVS_EX_DOUBLEBUFFER|LVS_EX_GRIDLINES);
        count=control(L"STATIC",L"",SS_LEFT);
        columns();EnableWindow(apply,FALSE);EnableWindow(current,FALSE);
        SetTimer(window,1,250,nullptr);
    }
    void columns(){
        ListView_SetItemCount(list,0);
        while(ListView_DeleteColumn(list,0)){}
        const std::array<const wchar_t*,8> trains={L"Train",L"Lieu / quai",L"État",L"Vitesse (km/h)",L"Ligne",L"Voyageurs",L"Échéance (s de jeu)",L"Identifiant"};
        const std::array<const wchar_t*,8> platforms={L"Gare",L"Quai",L"Occupation",L"Trains présents",L"Réservations",L"Tronçons",L"",L""};
        const int widths[]={190,240,170,130,160,100,180,185};
        for(int i=0;i<(mode?6:8);++i){LVCOLUMNW c{};c.mask=LVCF_TEXT|LVCF_WIDTH;
            c.pszText=const_cast<wchar_t*>((mode?platforms:trains)[i]);c.cx=mode?(i==0?230:i==3||i==4?260:160):widths[i];
            ListView_InsertColumn(list,i,&c);
        }
    }
    void layout(){
        RECT r{};GetClientRect(window,&r);int w=r.right,h=r.bottom;
        MoveWindow(status,18,14,w-36,24,TRUE);MoveWindow(clock,18,44,w-36,28,TRUE);
        MoveWindow(date,18,82,160,30,TRUE);MoveWindow(time,188,82,125,30,TRUE);
        MoveWindow(current,326,82,130,30,TRUE);MoveWindow(apply,466,82,210,30,TRUE);
        MoveWindow(notice,18,124,w-36,42,TRUE);
        MoveWindow(message,18,168,w-36,42,TRUE);
        MoveWindow(view,18,220,185,220,TRUE);MoveWindow(filter,215,220,w-233,28,TRUE);
        MoveWindow(list,18,260,w-36,std::max(50,h-300),TRUE);MoveWindow(count,18,h-30,w-36,24,TRUE);
    }
    void useCurrent(){
        if(!shown)return;auto c=shown->getSimulationClock();if(!c)return;
        using namespace std::chrono;
        auto instant=c->getDateTimeUtc();auto day=floor<days>(instant);year_month_day ymd{day};hh_mm_ss tod{instant-day};
        SYSTEMTIME st{};st.wYear=static_cast<WORD>(int(ymd.year()));st.wMonth=static_cast<WORD>(unsigned(ymd.month()));st.wDay=static_cast<WORD>(unsigned(ymd.day()));
        st.wHour=static_cast<WORD>(tod.hours().count());st.wMinute=static_cast<WORD>(tod.minutes().count());st.wSecond=static_cast<WORD>(tod.seconds().count());
        DateTime_SetSystemtime(date,GDT_VALID,&st);DateTime_SetSystemtime(time,GDT_VALID,&st);datesInitialized=true;
    }
    void setDate(){
        SYSTEMTIME d{},t{};
        if(DateTime_GetSystemtime(date,&d)!=GDT_VALID||DateTime_GetSystemtime(time,&t)!=GDT_VALID)return;
        using namespace std::chrono;
        year_month_day ymd{year{d.wYear},month{d.wMonth},day{d.wDay}};
        if(!ymd.ok())return;
        {std::lock_guard lock(mutex);
            if(!connected||busy||!latest||latest->isOlderThan(5s))return;
            requested=sys_days{ymd}+hours{t.wHour}+minutes{t.wMinute}+seconds{t.wSecond};
            busy=true;feedback=L"Changement d’heure et intervention des trains en cours…";
        }
        wake.notify_all();EnableWindow(apply,FALSE);
    }
    void rebuild(){
        rows.clear();wchar_t query[257]{};GetWindowTextW(filter,query,257);auto needle=lower(query);
        auto append=[&](Row row){
            if(!needle.empty()){
                bool match=false;for(const auto& cell:row)if(lower(cell).find(needle)!=std::wstring::npos){match=true;break;}
                if(!match)return;
            }
            rows.push_back(std::move(row));
        };
        if(shown&&!mode)for(const auto& train:shown->getAllTrains()){
            Row row{};row[0]=wide(train.getName());row[7]=std::to_wstring(train.getId());row[1]=L"Lieu inconnu";row[2]=L"Inconnu";
            auto service=shown->getTrainServiceById(train.getId());
            auto track=service?service->getLocationTrackId():std::nullopt;
            if(!track)if(auto p=train.getPosition())track=p->getTrackId();
            if(track){
                row[1]=L"Voie "+std::to_wstring(*track);
                if(auto station=shown->getStationForTrack(*track)){
                    row[1]=wide(station->getName().value_or("Gare sans nom"));
                    if(auto platforms=shown->getPlatformOccupationsForStation(station->getId()))for(const auto& p:*platforms)
                        if(p.platform.containsTrack(*track)){row[1]+=L" / "+wide(p.platform.getName().value_or("quai sans nom"));break;}
                }
            }else if(service&&service->isOnNetwork()==false)row[1]=L"Hors réseau";
            row[3]=L"Non mesurée";if(!train.isSpeedDefaulted())if(auto speed=train.getSpeedKmh())row[3]=number(*speed);
            if(service){
                row[2]=serviceName(*service);row[4]=wide(service->getLineName().value_or(""));
                if(auto value=service->getDepartureRemainingSeconds())row[6]=L"Départ : "+number(*value);
                else if(auto value=service->getArrivalRemainingSeconds())row[6]=L"Arrivée : "+number(*value);
                else if(auto value=service->getDispatchRemainingSeconds())row[6]=L"Dispatch : "+number(*value);
                if(service->isHidden().value_or(false))row[1]+=L" (masqué)";
                if(auto alert=service->getAlert();alert&&*alert)row[2]+=L" · alerte "+std::to_wstring(*alert);
            }
            row[5]=L"—";if(auto details=shown->getTrainDetailsById(train.getId()))if(auto n=details->getPassengerCount())row[5]=std::to_wstring(*n);
            append(std::move(row));
        }
        if(shown&&mode)for(const auto& station:shown->getAllStations()){
            auto platforms=shown->getPlatformOccupationsForStation(station.getId());
            if(!platforms||platforms->empty()){
                Row row{};row[0]=wide(station.getName().value_or("Gare sans nom"));row[2]=platforms?L"Aucun quai":L"Données indisponibles";append(std::move(row));continue;
            }
            for(const auto& p:*platforms){
                Row row{};row[0]=wide(station.getName().value_or("Gare sans nom"));row[1]=wide(p.platform.getName().value_or("Sans nom"));
                auto occupied=p.isOccupied();row[2]=!occupied?L"Inconnue":*occupied?L"Occupé":L"Libre";
                auto names=[](const auto& trains){std::wstring text;if(!trains)return std::wstring{L"Inconnu"};for(const auto& train:*trains){if(!text.empty())text+=L", ";text+=wide(train.getName());}return text;};
                row[3]=names(p.occupying_trains);row[4]=names(p.reserving_trains);
                for(auto id:p.platform.getTrackIds()){if(!row[5].empty())row[5]+=L", ";row[5]+=std::to_wstring(id);}
                append(std::move(row));
            }
        }
        ListView_SetItemCountEx(list,static_cast<int>(rows.size()),LVSICF_NOSCROLL);
        InvalidateRect(list,nullptr,FALSE);
        auto label=std::to_wstring(rows.size())+(mode?L" quais / gares affichés":L" trains affichés");SetWindowTextW(count,label.c_str());
    }
    void tick(){
        nimby::Snapshot::Ptr snapshot;std::wstring state,result;bool ready{},pending{};
        {std::lock_guard lock(mutex);snapshot=latest;state=connection;result=feedback;ready=connected;pending=busy;}
        if(snapshot&&snapshot->isOlderThan(5s)){ready=false;state=L"Données anciennes — en attente d’une nouvelle capture.";}
        SetWindowTextW(status,state.c_str());
        if(!result.empty())SetWindowTextW(message,result.c_str());
        bool hasClock=snapshot&&snapshot->getSimulationClock().has_value();
        EnableWindow(apply,ready&&hasClock&&!pending);EnableWindow(current,ready&&hasClock&&!pending);
        if(snapshot!=shown){
            shown=std::move(snapshot);
            auto c=shown?shown->getSimulationClock():std::nullopt;
            const auto text=c?L"Date de simulation (UTC) : "+wide(c->getDateTimeUtcString()):L"Date de simulation (UTC) : indisponible";
            SetWindowTextW(clock,text.c_str());if(c&&!datesInitialized)useCurrent();rebuild();
        }
    }
};
LRESULT CALLBACK windowProc(HWND window,UINT message,WPARAM w,LPARAM l){
    auto app=reinterpret_cast<App*>(GetWindowLongPtrW(window,GWLP_USERDATA));
    if(message==WM_NCCREATE){app=static_cast<App*>(reinterpret_cast<CREATESTRUCTW*>(l)->lpCreateParams);app->window=window;SetWindowLongPtrW(window,GWLP_USERDATA,reinterpret_cast<LONG_PTR>(app));}
    if(!app)return DefWindowProcW(window,message,w,l);
    try {
        switch(message){
        case WM_CREATE:app->create();return 0;
        case WM_SIZE:app->layout();return 0;
        case WM_GETMINMAXINFO:{auto info=reinterpret_cast<MINMAXINFO*>(l);info->ptMinTrackSize={820,450};return 0;}
        case WM_TIMER:app->tick();return 0;
        case WM_COMMAND:
            if(LOWORD(w)==103)app->useCurrent();
            if(LOWORD(w)==104)app->setDate();
            if(LOWORD(w)==105&&HIWORD(w)==CBN_SELCHANGE){app->mode=static_cast<int>(SendMessageW(app->view,CB_GETCURSEL,0,0));app->columns();app->rebuild();}
            if(LOWORD(w)==106&&HIWORD(w)==EN_CHANGE)app->rebuild();
            return 0;
        case WM_NOTIFY:{auto header=reinterpret_cast<NMHDR*>(l);
            if(header->hwndFrom==app->list&&header->code==LVN_GETDISPINFOW){
                auto info=reinterpret_cast<NMLVDISPINFOW*>(l);auto& item=info->item;
                if((item.mask&LVIF_TEXT)&&item.iItem>=0&&static_cast<size_t>(item.iItem)<app->rows.size()&&item.iSubItem>=0&&item.iSubItem<8)
                    lstrcpynW(item.pszText,app->rows[item.iItem][item.iSubItem].c_str(),item.cchTextMax);
                return 0;
            }break;
        }
        case WM_DESTROY:KillTimer(window,1);PostQuitMessage(0);return 0;
        }
    }catch(const std::exception& error){
        if(message==WM_CREATE)return -1;
        SetWindowTextW(app->message,wide(error.what()).c_str());
    }
    return DefWindowProcW(window,message,w,l);
}
}
int runObserverWindow(std::uint32_t pid){
    FreeConsole();
    INITCOMMONCONTROLSEX controls{sizeof controls,ICC_LISTVIEW_CLASSES|ICC_DATE_CLASSES};InitCommonControlsEx(&controls);
    App app;const auto instance=GetModuleHandleW(nullptr);
    WNDCLASSW cls{};cls.lpfnWndProc=windowProc;cls.hInstance=instance;cls.lpszClassName=L"NRF.AutoObserver";
    cls.hCursor=LoadCursorW(nullptr,IDC_ARROW);cls.hbrBackground=reinterpret_cast<HBRUSH>(COLOR_WINDOW+1);
    RegisterClassW(&cls);
    auto window=CreateWindowExW(0,cls.lpszClassName,L"NimbyRailsFranceSDK — Auto-observer",WS_OVERLAPPEDWINDOW,
                                CW_USEDEFAULT,CW_USEDEFAULT,1320,800,nullptr,nullptr,instance,&app);
    if(!window){MessageBoxW(nullptr,L"Impossible de créer la fenêtre.",L"Auto-observer",MB_ICONERROR);return 1;}
    ShowWindow(window,SW_SHOW);app.start(pid);
    MSG message{};int result{};
    while((result=GetMessageW(&message,nullptr,0,0))>0){
        if(!IsDialogMessageW(window,&message)){TranslateMessage(&message);DispatchMessageW(&message);}
    }
    return result==-1?1:static_cast<int>(message.wParam);
}
