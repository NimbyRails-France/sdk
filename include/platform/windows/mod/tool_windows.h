#pragma once
#include <windows.h>
#include <nimby/detail/diagnostics.hpp>
#include <nimby/detail/translations.hpp>
#include <platform/windows/game_language.h>
#include <platform/windows/runtime/mod_host_client.h>
#include <nimby/detail/observation_values.hpp>
#include <charconv>
#include <cstring>
#include <deque>
#include <mutex>
#include <thread>
#include <optional>
#include <algorithm>

namespace nimby::platform::windows {
// Windows-owned tool forms. The UI thread never calls Kotlin or reads/writes
// simulation objects. It queues immutable events for the mod's existing worker.
class ToolWindows {
public:
    struct Definition {std::string id,title,shortcut;};
    struct Button {std::string id,label;bool enabled=true;};
    struct Input {std::string id,label;int32_t value{},minimum{},maximum{};bool enabled=true;};
    struct Content {std::string message;std::vector<Button> buttons;std::vector<Input> inputs;};
    struct Event {size_t index{};uint64_t sequence{};GameSession game;std::string action;std::vector<std::pair<std::string,int32_t>> values;};
    ToolWindows(std::vector<Definition> definitions,std::string_view catalog):definitions_(std::move(definitions)),states_(definitions_.size()) {
        if(!catalog.empty())translations_.emplace(catalog);
    }
    ~ToolWindows(){stop();}
    void observe(const GameSession& game) {
        bool changed=false;
        {std::lock_guard lock(mutex_);
            if(!game_||*game_!=game){changed=true;game_=game;events_.clear();for(auto& s:states_){s.sequence=++sequence_;s.ready=false;s.content={};++s.revision;}}
            observed_=GetTickCount64();
        }
        if(!thread_.joinable())thread_=std::jthread([this](std::stop_token stop){run(stop);});
        if(changed)uiWake_.signal();
    }
    void invalidate(){
        {std::lock_guard lock(mutex_);game_.reset();events_.clear();for(auto& s:states_){s.ready=false;++s.revision;}}
        uiWake_.signal();
    }
    void stop(){if(thread_.joinable()){thread_.request_stop();uiWake_.signal();thread_.join();}}
    std::optional<Event> poll(){
        std::lock_guard lock(mutex_);
        while(!events_.empty()){
            auto event=std::move(events_.front());events_.pop_front();
            if(freshLocked()&&event.game==*game_&&event.index<states_.size()&&
               event.sequence==states_[event.index].sequence)return event;
        }
        return {};
    }
    bool publish(std::string_view id,uint64_t sequence,const GameSession& game,Content content) {
        bool published=false;
        {
            std::lock_guard lock(mutex_);
            if(!game_||*game_!=game)return false;
            for(size_t i=0;i<definitions_.size();++i)if(definitions_[i].id==id){auto& s=states_[i];
                if(s.sequence!=sequence)return false;
                s.content=std::move(content);s.ready=true;++s.revision;published=true;break;}
        }
        if(published)uiWake_.signal();
        return published;
    }
private:
    friend struct ToolWindowsTest;
    struct UiWake {
        HANDLE event=CreateEventW(nullptr,FALSE,FALSE,nullptr);
        ~UiWake(){if(event)CloseHandle(event);}
        void signal()const noexcept {if(event)SetEvent(event);}
    } uiWake_;
    struct State {Content content;uint64_t sequence{},revision{};bool ready=false;};
    struct View {ToolWindows* owner{};size_t index{};HWND window{},message{};std::vector<HWND> children,inputs,labels,buttons;uint64_t revision=~uint64_t{},sequence{};Content content;std::string language;int scroll{},contentHeight{};};
    std::vector<Definition> definitions_;std::vector<State> states_;std::vector<View> views_;
    std::optional<detail::Translations> translations_;
    std::mutex mutex_;std::optional<GameSession> game_;std::deque<Event> events_;
    uint64_t sequence_=0,observed_=0;std::jthread thread_;HMODULE module_{};std::wstring className_;HWND gameWindow_{};
    DWORD gamePid_{};uint64_t languageChecked_{};std::string language_;
    // SDK-local notifier only: never execute a Kotlin callback on the UI
    // thread. This event belongs to this child, not another provider/process.
    void (*wake_)() noexcept=mod_host::client::wakeLocal;
    static std::wstring wide(std::string_view text){
        const auto n=MultiByteToWideChar(CP_UTF8,MB_ERR_INVALID_CHARS,text.data(),int(text.size()),nullptr,0);
        if(n<=0)return {};
        std::wstring out(size_t(n),L'\0');MultiByteToWideChar(CP_UTF8,MB_ERR_INVALID_CHARS,text.data(),int(text.size()),out.data(),n);return out;
    }
    static bool read(void*,uint64_t address,void* out,size_t size){SIZE_T got{};return ReadProcessMemory(GetCurrentProcess(),reinterpret_cast<void*>(address),out,size,&got)&&got==size;}
    bool acceptsForeground(HWND window)const {
        DWORD pid{};GetWindowThreadProcessId(window,&pid);
        // Reopening the shortcut while one of this mod's forms has focus is
        // allowed, but unrelated foreground applications never open a form.
        return pid&&((gamePid_&&pid==gamePid_)||std::any_of(views_.begin(),views_.end(),
            [&](const auto& view){return window==view.window||IsChild(view.window,window);}));
    }
    const std::string& language() {
        const auto now=GetTickCount64();
        if(now-languageChecked_<500)return language_;
        languageChecked_=now;
        const auto value=mod_host::client::target()?mod_host::client::language():
            gameLanguage(read,nullptr,reinterpret_cast<uint64_t>(GetModuleHandleW(nullptr)));
        if(value)language_=*value;
        return language_;
    }
    std::wstring translated(std::string_view text,std::string_view language){
        return wide(detail::Translations::resolve(translations_?&*translations_:nullptr,text,language));
    }
    HWND control(View& view,const wchar_t* kind,std::wstring label,DWORD style,int x,int y,int width,int height,int id=0){
        auto h=CreateWindowExW(kind==std::wstring_view(L"EDIT")?WS_EX_CLIENTEDGE:0,kind,label.c_str(),WS_CHILD|WS_VISIBLE|style,x,y,width,height,view.window,reinterpret_cast<HMENU>(INT_PTR(id)),module_,nullptr);
        if(!h)throw std::runtime_error("Cannot create tool control");
        SendMessageW(h,WM_SETFONT,reinterpret_cast<WPARAM>(GetStockObject(DEFAULT_GUI_FONT)),TRUE);view.children.push_back(h);return h;
    }
    bool freshLocked()const{return game_.has_value()&&GetTickCount64()-observed_<5000;}
    static int textHeight(HWND window,const std::wstring& text,int width){
        RECT bounds{0,0,width,0};HDC dc=GetDC(window);
        const auto previous=SelectObject(dc,GetStockObject(DEFAULT_GUI_FONT));
        DrawTextW(dc,text.c_str(),int(text.size()),&bounds,DT_CALCRECT|DT_WORDBREAK|DT_NOPREFIX);
        SelectObject(dc,previous);ReleaseDC(window,dc);return int(bounds.bottom);
    }
    static void scrollTo(View& view,int position){
        RECT client{};GetClientRect(view.window,&client);const int height=std::max(1,int(client.bottom));
        position=std::clamp(position,0,std::max(0,view.contentHeight-height));
        const int delta=view.scroll-position;view.scroll=position;
        SCROLLINFO info{sizeof info,SIF_RANGE|SIF_PAGE|SIF_POS|SIF_DISABLENOSCROLL,0,std::max(0,view.contentHeight-1),UINT(height),position,0};
        SetScrollInfo(view.window,SB_VERT,&info,TRUE);
        if(delta)ScrollWindowEx(view.window,0,delta,nullptr,nullptr,nullptr,nullptr,SW_SCROLLCHILDREN|SW_INVALIDATE|SW_ERASE);
    }
    static void showFocus(View& view){
        const HWND focus=GetFocus();if(!IsChild(view.window,focus))return;
        RECT bounds{},client{};GetWindowRect(focus,&bounds);MapWindowPoints(nullptr,view.window,reinterpret_cast<POINT*>(&bounds),2);
        GetClientRect(view.window,&client);
        if(bounds.top<0)scrollTo(view,view.scroll+int(bounds.top)-8);
        else if(bounds.bottom>client.bottom)scrollTo(view,view.scroll+int(bounds.bottom-client.bottom)+8);
    }
    bool enqueueOpen(size_t index){
        {
            std::lock_guard lock(mutex_);if(index>=states_.size()||!freshLocked())return false;
            auto& s=states_[index];s.sequence=++sequence_;s.ready=false;s.content={};++s.revision;
            std::erase_if(events_,[&](const auto& event){return event.index==index;});
            events_.push_back({index,s.sequence,*game_,"open",{}});
        }
        wake_();
        return true;
    }
    void open(size_t index){
        if(!enqueueOpen(index))return;
        ShowWindow(views_[index].window,SW_SHOWNORMAL);SetForegroundWindow(views_[index].window);
    }
    void update(View& view){
        State state;bool fresh;
        {std::lock_guard lock(mutex_);state=states_[view.index];fresh=freshLocked();}
        if(!fresh){ShowWindow(view.window,SW_HIDE);return;}
        refresh(view,state,language());
    }
    void refresh(View& view,const State& state,const std::string& language){
        const bool changed=view.revision!=state.revision;
        if(!changed&&view.language==language)return;
        const int scroll=changed?0:view.scroll;
        scrollTo(view,0);
        if(changed){
            for(auto child:view.children)DestroyWindow(child);
            view.children.clear();view.inputs.clear();view.labels.clear();view.buttons.clear();
            view.revision=state.revision;view.sequence=state.sequence;view.content=state.content;
            view.message=control(view,L"STATIC",L"",SS_LEFT|SS_NOPREFIX,0,0,0,0);
            for(size_t i=0;i<state.content.inputs.size();++i){const auto& field=state.content.inputs[i];
                view.labels.push_back(control(view,L"STATIC",L"",SS_LEFT|SS_NOPREFIX,0,0,0,0));
                auto edit=control(view,L"EDIT",std::to_wstring(field.value),WS_TABSTOP|ES_AUTOHSCROLL,0,0,0,0,100+int(i));
                SendMessageW(edit,EM_SETLIMITTEXT,16,0);EnableWindow(edit,field.enabled&&state.ready);view.inputs.push_back(edit);
            }
            for(size_t i=0;i<state.content.buttons.size();++i){
                auto button=control(view,L"BUTTON",L"",WS_TABSTOP|BS_PUSHBUTTON|BS_MULTILINE,0,0,0,0,200+int(i));
                EnableWindow(button,state.content.buttons[i].enabled&&state.ready);view.buttons.push_back(button);
            }
        }
        // A language switch is cosmetic: reuse the controls so unfinished
        // input (even empty/invalid text), selection, focus and pending replies
        // survive. Only a new mod response replaces the form and its values.
        view.language=language;
        SetWindowTextW(view.window,translated(definitions_[view.index].title,language).c_str());
        // Measure wrapped text with the same font as the control. Confirmation
        // messages must remain readable in every supported language.
        const auto message = translated(state.content.message,language);
        const int messageHeight=std::max(32,textHeight(view.window,message,500)+8);
        SetWindowTextW(view.message,message.c_str());MoveWindow(view.message,16,12,500,messageHeight,TRUE);
        int y=messageHeight+28;
        for(size_t i=0;i<state.content.inputs.size();++i){const auto& field=state.content.inputs[i];
            const auto label=translated(field.label,language);const int height=std::max(26,textHeight(view.window,label,235)+5);
            SetWindowTextW(view.labels[i],label.c_str());MoveWindow(view.labels[i],16,y+5,235,height,TRUE);
            MoveWindow(view.inputs[i],260,y,250,26,TRUE);y+=height+10;
        }
        for(size_t i=0;i<state.content.buttons.size();++i){const auto& b=state.content.buttons[i];
            const auto label=translated(b.label,language);const int height=std::max(30,textHeight(view.window,label,480)+12);
            std::wstring caption;for(const auto c:label){caption+=c;if(c==L'&')caption+=c;}
            SetWindowTextW(view.buttons[i],caption.c_str());MoveWindow(view.buttons[i],16,y,500,height,TRUE);
            y+=height+8;
        }
        view.contentHeight=y+16;
        MONITORINFO monitor{};monitor.cbSize=sizeof monitor;GetMonitorInfoW(MonitorFromWindow(view.window,MONITOR_DEFAULTTONEAREST),&monitor);
        const int height=std::min(view.contentHeight+40,std::max(120,int(monitor.rcWork.bottom-monitor.rcWork.top)-32));
        RECT position{};GetWindowRect(view.window,&position);
        const int x=std::clamp(int(position.left),int(monitor.rcWork.left),std::max(int(monitor.rcWork.left),int(monitor.rcWork.right)-552));
        const int top=std::clamp(int(position.top),int(monitor.rcWork.top),std::max(int(monitor.rcWork.top),int(monitor.rcWork.bottom)-height));
        SetWindowPos(view.window,nullptr,x,top,552,height,SWP_NOZORDER|SWP_NOACTIVATE);scrollTo(view,scroll);
    }
    void click(View& view,size_t button){
        if(button>=view.content.buttons.size()||!view.content.buttons[button].enabled)return;
        std::vector<std::pair<std::string,int32_t>> values;
        for(size_t i=0;i<view.inputs.size();++i){char buffer[32]{};GetWindowTextA(view.inputs[i],buffer,sizeof buffer);
            int32_t value{};const auto end=buffer+std::strlen(buffer);const auto parsed=std::from_chars(buffer,end,value);const auto& input=view.content.inputs[i];
            if(parsed.ec!=std::errc{}||parsed.ptr!=end||end==buffer||value<input.minimum||value>input.maximum){
                SetFocus(view.inputs[i]);SendMessageW(view.inputs[i],EM_SETSEL,0,-1);MessageBeep(MB_ICONWARNING);return;
            }
            values.emplace_back(input.id,value);
        }
        {
            std::lock_guard lock(mutex_);auto& s=states_[view.index];
            if(!freshLocked()||!s.ready||s.sequence!=view.sequence||s.revision!=view.revision)return;
            s.ready=false; // Reject repeat clicks until the worker publishes its reply.
            s.sequence=++sequence_; // Every request has its own response token.
            std::erase_if(events_,[&](const auto& event){return event.index==view.index;});
            events_.push_back({view.index,s.sequence,*game_,view.content.buttons[button].id,std::move(values)});
        }
        wake_();
        for(auto b:view.buttons)EnableWindow(b,FALSE);
    }
    static LRESULT CALLBACK procedure(HWND window,UINT message,WPARAM w,LPARAM l){
        auto* view=reinterpret_cast<View*>(GetWindowLongPtrW(window,GWLP_USERDATA));
        if(message==WM_NCCREATE){view=static_cast<View*>(reinterpret_cast<CREATESTRUCTW*>(l)->lpCreateParams);SetWindowLongPtrW(window,GWLP_USERDATA,reinterpret_cast<LONG_PTR>(view));view->window=window;}
        if(view)try {
            if(message==WM_CLOSE){ShowWindow(window,SW_HIDE);return 0;}
            if(message==WM_SIZE){scrollTo(*view,view->scroll);return 0;}
            if(message==WM_MOUSEWHEEL){scrollTo(*view,view->scroll-int(static_cast<short>(HIWORD(w)))/WHEEL_DELTA*48);return 0;}
            if(message==WM_VSCROLL){
                SCROLLINFO info{};info.cbSize=sizeof info;info.fMask=SIF_ALL;GetScrollInfo(window,SB_VERT,&info);int position=view->scroll;
                switch(LOWORD(w)){case SB_LINEUP:position-=32;break;case SB_LINEDOWN:position+=32;break;
                    case SB_PAGEUP:position-=int(info.nPage);break;case SB_PAGEDOWN:position+=int(info.nPage);break;
                    case SB_THUMBTRACK:position=info.nTrackPos;break;case SB_TOP:position=0;break;case SB_BOTTOM:position=info.nMax;break;}
                scrollTo(*view,position);return 0;
            }
            if(message==WM_COMMAND&&HIWORD(w)==BN_CLICKED&&LOWORD(w)>=200){view->owner->click(*view,LOWORD(w)-200);return 0;}
        }catch(...){detail::diagnostics::exception("mods","Tool window input");}
        return DefWindowProcW(window,message,w,l);
    }
    void run(std::stop_token stop)noexcept {try{
        gamePid_=mod_host::client::target();if(!gamePid_)gamePid_=GetCurrentProcessId();
        EnumWindows(+[](HWND window,LPARAM arg)->BOOL{auto& tools=*reinterpret_cast<ToolWindows*>(arg);DWORD pid{};GetWindowThreadProcessId(window,&pid);if(pid==tools.gamePid_&&IsWindowVisible(window)&&!GetWindow(window,GW_OWNER)){tools.gameWindow_=window;return FALSE;}return TRUE;},reinterpret_cast<LPARAM>(this));
        if(!gameWindow_)throw std::runtime_error("Game window unavailable");
        GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS|GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,reinterpret_cast<LPCWSTR>(&procedure),&module_);
        className_=L"NRF.Tool."+std::to_wstring(reinterpret_cast<uintptr_t>(this));
        WNDCLASSW cls{};cls.lpfnWndProc=procedure;cls.hInstance=module_;cls.hCursor=LoadCursorW(nullptr,MAKEINTRESOURCEW(32512));cls.hbrBackground=reinterpret_cast<HBRUSH>(COLOR_WINDOW+1);cls.lpszClassName=className_.c_str();
        if(!RegisterClassW(&cls))throw std::runtime_error("Cannot register tool window class");
        views_.resize(definitions_.size());
        for(size_t i=0;i<views_.size();++i){auto& view=views_[i];view.owner=this;view.index=i;
            view.window=CreateWindowExW(WS_EX_CONTROLPARENT,className_.c_str(),L"NRF",WS_OVERLAPPED|WS_CAPTION|WS_SYSMENU|WS_VSCROLL,CW_USEDEFAULT,CW_USEDEFAULT,552,220,gameWindow_,nullptr,module_,&view);
            if(!view.window)throw std::runtime_error("Cannot create tool window");
            const auto& key=definitions_[i].shortcut;UINT modifiers=MOD_NOREPEAT,vk{};
            if(key.starts_with("Ctrl+Shift+")&&key.size()==12){modifiers|=MOD_CONTROL|MOD_SHIFT;vk=UINT(key.back());}
            else if(key.starts_with("F")){const int number=std::stoi(key.substr(1));if(number>=1&&number<=12)vk=VK_F1+number-1;}
            if(!vk||!RegisterHotKey(nullptr,int(i+1),modifiers,vk))throw std::runtime_error("Tool shortcut unavailable: "+key);
        }
        while(!stop.stop_requested()){
            MSG msg{};while(PeekMessageW(&msg,nullptr,0,0,PM_REMOVE)){
                if(msg.message==WM_HOTKEY){if(acceptsForeground(GetForegroundWindow())&&msg.wParam>=1&&msg.wParam<=views_.size())open(size_t(msg.wParam-1));}
                else{bool handled=false;for(auto& view:views_)if(IsWindowVisible(view.window)&&IsDialogMessageW(view.window,&msg)){showFocus(view);handled=true;break;}if(!handled){TranslateMessage(&msg);DispatchMessageW(&msg);}}
            }
            for(auto& view:views_)update(view);
            // Worker publications can repaint immediately. The same bounded
            // timeout still checks freshness/language, and a failed event
            // creation preserves the previous polling behavior.
            MsgWaitForMultipleObjects(uiWake_.event?1:0,uiWake_.event?&uiWake_.event:nullptr,FALSE,50,QS_ALLINPUT);
        }
    }catch(...){detail::diagnostics::exception("mods","Tool windows");}
        for(size_t i=0;i<views_.size();++i){UnregisterHotKey(nullptr,int(i+1));if(views_[i].window)DestroyWindow(views_[i].window);}
        if(!className_.empty())UnregisterClassW(className_.c_str(),module_);
    }
};
}
