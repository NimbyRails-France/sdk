// Hidden Win32 controls: exercise relocalisation without a game, hotkey,
// clock command, focus grab or visible window.
#include <platform/windows/mod/tool_windows.h>
#include <cassert>
#include <iostream>

namespace nimby::platform::windows {
struct ToolWindowsTest {
    static inline ToolWindows* notified{};
    static inline unsigned wakes{};
    static void wake()noexcept {
        ++wakes;assert(notified&&notified->mutex_.try_lock());notified->mutex_.unlock();
    }
    static std::wstring text(HWND window){
        std::wstring result(size_t(GetWindowTextLengthW(window))+1,L'\0');
        const auto length=GetWindowTextW(window,result.data(),int(result.size()));
        result.resize(size_t(length));return result;
    }
    static std::string ref(const char* key){return std::string("\x1eNRF:[\"")+key+"\",{}]";}
    static void run(){
        ToolWindows tools({{"clock",ref("title"),"Ctrl+Shift+T"}},R"({"fallback":"fr","languages":{
          "fr":{"title":"Horloge","message":"Date choisie","year":"Année","apply":"Confirmer"},
          "en":{"title":"Clock","message":"Selected date","year":"Year","apply":"Confirm"}}})");
        tools.module_=GetModuleHandleW(nullptr);
        ToolWindows::View view;view.owner=&tools;
        view.window=CreateWindowExW(0,L"STATIC",L"",WS_OVERLAPPED|WS_VSCROLL,0,0,552,220,nullptr,nullptr,tools.module_,nullptr);
        assert(view.window);
        struct Cleanup{HWND handle;~Cleanup(){DestroyWindow(handle);}} cleanup{view.window};
        // A worker's PID is not the game's PID. Only the selected target or
        // one of this tool's own forms may handle its shortcut.
        tools.gamePid_=GetCurrentProcessId()+1;
        assert(!tools.acceptsForeground(view.window));
        tools.gamePid_=GetCurrentProcessId();assert(tools.acceptsForeground(view.window));
        tools.gamePid_=GetCurrentProcessId()+1;tools.views_.push_back(view);
        assert(tools.acceptsForeground(view.window));tools.views_.clear();
        assert(!tools.acceptsForeground(nullptr));
        ToolWindows::State state;state.ready=true;state.revision=1;state.sequence=7;
        state.content={ref("message"),{{"apply",ref("apply"),true}},{{"year",ref("year"),2026,1,9999,true}}};
        tools.refresh(view,state,"fr");
        const auto edit=view.inputs.front(),button=view.buttons.front();
        assert(text(view.window)==L"Horloge"&&text(edit)==L"2026");
        SetWindowTextW(edit,L""); // Deleting a year is a valid editing draft.
        EnableWindow(button,FALSE); // Request in flight: a translation must not re-enable it.
        tools.refresh(view,state,"eng");
        assert(view.inputs.front()==edit&&view.buttons.front()==button);
        assert(text(view.window)==L"Clock"&&text(view.labels.front())==L"Year");
        assert(text(view.message)==L"Selected date"&&text(button)==L"Confirm");
        assert(text(edit).empty()&&!IsWindowEnabled(button));
        assert(view.sequence==7&&view.revision==1&&tools.events_.empty());
        SetWindowTextW(edit,L"2048");SendMessageW(edit,EM_SETSEL,1,3);
        tools.refresh(view,state,"de"); // Undeclared language -> French fallback.
        DWORD start{},end{};SendMessageW(edit,EM_GETSEL,reinterpret_cast<WPARAM>(&start),reinterpret_cast<LPARAM>(&end));
        assert(text(edit)==L"2048"&&start==1&&end==3&&text(button)==L"Confirmer");
        ++state.revision;state.sequence=8;state.content.inputs.front().value=2030;
        tools.refresh(view,state,"en");
        assert(text(view.inputs.front())==L"2030"&&IsWindowEnabled(view.buttons.front()));
        assert(!IsWindowVisible(view.window)&&tools.events_.empty());
        // The form queues a validated command, then wakes its own worker
        // outside the state mutex. Repeated/stale clicks cannot wake or submit.
        notified=&tools;tools.wake_=wake;tools.game_=GameSession{1,"test-world"};
        tools.observed_=GetTickCount64();tools.states_[0]=state;
        tools.click(view,0);assert(wakes==1&&tools.events_.size()==1&&!tools.states_[0].ready);
        tools.click(view,0);assert(wakes==1&&tools.events_.size()==1);
        const auto action=tools.poll();assert(action&&action->action=="apply"&&action->values[0].second==2030);
        assert(!tools.poll());
        const auto current=tools.states_[0].sequence;
        assert(!tools.publish("clock",current-1,*tools.game_,state.content));
        assert(WaitForSingleObject(tools.uiWake_.event,0)==WAIT_TIMEOUT);
        assert(tools.publish("clock",current,*tools.game_,state.content));
        assert(WaitForSingleObject(tools.uiWake_.event,0)==WAIT_OBJECT_0);
        assert(WaitForSingleObject(tools.uiWake_.event,0)==WAIT_TIMEOUT);
        // Several accepted replies coalesce into one repaint hint.
        for(int i=0;i<32;++i)assert(tools.publish("clock",current,*tools.game_,state.content));
        assert(WaitForSingleObject(tools.uiWake_.event,0)==WAIT_OBJECT_0);
        assert(WaitForSingleObject(tools.uiWake_.event,0)==WAIT_TIMEOUT);
        tools.invalidate();assert(WaitForSingleObject(tools.uiWake_.event,0)==WAIT_OBJECT_0);
        assert(!tools.publish("clock",current,GameSession{1,"test-world"},state.content));
        assert(WaitForSingleObject(tools.uiWake_.event,0)==WAIT_TIMEOUT);
        tools.click(view,0);assert(wakes==1&&!tools.poll());
        assert(!tools.enqueueOpen(0)&&wakes==1);
        tools.game_=GameSession{2,"other-world"};tools.observed_=GetTickCount64();
        assert(!tools.enqueueOpen(4)&&wakes==1);
        assert(tools.enqueueOpen(0)&&wakes==2);
        const auto opened=tools.poll();assert(opened&&opened->action=="open"&&opened->game==*tools.game_);
        assert(!IsWindowVisible(view.window)); // No focus/window automation in this test.
        // SDK-driven open requests do not reserve OS hotkeys or activate a
        // window on this worker thread. Duplicate requests remain bounded.
        assert(!tools.requestOpen("unknown"));
        for(unsigned i=0;i<1000;++i)assert(tools.requestOpen("clock"));
        assert(tools.pendingOpens_.size()==1&&tools.events_.empty()&&wakes==2);
        assert(WaitForSingleObject(tools.uiWake_.event,0)==WAIT_OBJECT_0);
        const auto pending=tools.takeOpenRequests();
        assert(pending.size()==1&&pending.front().index==0&&tools.takeOpenRequests().empty());
        assert(!IsWindowVisible(view.window)&&tools.events_.empty());
        assert(tools.requestOpen("clock"));tools.invalidate();
        assert(tools.pendingOpens_.empty()&&!tools.requestOpen("clock")&&tools.takeOpenRequests().empty());
        tools.game_=GameSession{3,"next-world"};tools.observed_=GetTickCount64();
        assert(!tools.enqueueOpen(pending.front().index,pending.front().epoch));
        assert(tools.requestOpen("clock"));tools.observed_=GetTickCount64()-5001;
        assert(tools.takeOpenRequests().empty()&&tools.pendingOpens_.empty());
        assert(!tools.requestOpen("clock"));
        notified=nullptr;
    }
};
}
int main(){nimby::platform::windows::ToolWindowsTest::run();std::cout<<"PASS: live tool translations preserve drafts, selection and pending commands\n";}
