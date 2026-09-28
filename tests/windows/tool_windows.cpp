// Hidden Win32 controls: exercise relocalisation without a game, hotkey,
// clock command, focus grab or visible window.
#include <platform/windows/mod/tool_windows.h>
#include <cassert>
#include <iostream>

namespace nimby::platform::windows {
struct ToolWindowsTest {
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
    }
};
}
int main(){nimby::platform::windows::ToolWindowsTest::run();std::cout<<"PASS: live tool translations preserve drafts, selection and pending commands\n";}
