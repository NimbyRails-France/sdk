#pragma once
#include "engine/live_state.h"
#include <cstdint>
#include <optional>
#include <cstring>
#include <stdexcept>
#include <cmath>
#include <algorithm>
#include <array>
#include <nimby/detail/number_input_draft.hpp>

namespace nimby::engine {
// Internal adapter for the fingerprinted 1.19.10 binary. The bootstrap must
// verify the full executable identity before using these RVAs. A UI object is
// borrowed for ONE synchronous editor callback, on the game's UI thread.
class SignalUi {
public:
    enum class Pass { Layout, Interactive };
    // The Windows runtime supplies QPC ticks, the same clock used by the
    // game's SDL text-input lease. No OS clock dependency in this adapter.
    using TextInputClock=int64_t(*)();

    static std::optional<SignalUi> bind(ReadMemory read,void* context,
                                        uint64_t module,uint64_t object,
                                        LiveStateProfile profile=LiveStateProfile::Windows119,TextInputClock textClock=nullptr) noexcept {
        const auto& layout=gameLayout(profile);
        if(!layout.root_rva)return std::nullopt;
        const auto layoutTable=layout.ui_layout_table,interactiveTable=layout.ui_interactive_table;
        const auto layoutCheckbox=layout.ui_layout_checkbox,interactiveCheckbox=layout.ui_interactive_checkbox;
        const auto slot=layout.ui_checkbox_slot;
        constexpr uint64_t addressLimit=0x7fffffff0000ULL;
        const uint64_t lastTable=layoutTable>interactiveTable?layoutTable:interactiveTable;
        if(!read || module<0x10000 || module>addressLimit-lastTable-slot-sizeof(uint64_t) ||
           object<0x10000 || object>addressLimit-sizeof(uint64_t))return std::nullopt;
        uint64_t table{},checkbox{};
        if(!read(context,object,&table,sizeof table))return std::nullopt;
        Pass pass;
        uint64_t expected;
        if(table==module+layoutTable){pass=Pass::Layout;expected=module+layoutCheckbox;}
        else if(table==module+interactiveTable){pass=Pass::Interactive;expected=module+interactiveCheckbox;}
        else return std::nullopt;
        if(!read(context,table+slot,&checkbox,sizeof checkbox)||checkbox!=expected)return std::nullopt;
        uint64_t button=0;
        if(layout.ui_button_slot){
            const auto expectedButton=module+(pass==Pass::Layout?layout.ui_layout_button:layout.ui_interactive_button);
            if(!read(context,table+layout.ui_button_slot,&button,sizeof button)||button!=expectedButton)return std::nullopt;
        }
        uint64_t group=0,groupEnd=0,box=0,boxEnd=0;
        if(layout.ui_group_slot){
            const auto expectedGroup=module+(pass==Pass::Layout?layout.ui_layout_group:layout.ui_interactive_group);
            const auto expectedEnd=module+(pass==Pass::Layout?layout.ui_layout_group_end:layout.ui_interactive_group_end);
            if(!read(context,table+layout.ui_group_slot,&group,sizeof group)||group!=expectedGroup||
               !read(context,table+layout.ui_group_end_slot,&groupEnd,sizeof groupEnd)||groupEnd!=expectedEnd)return std::nullopt;
            const auto expectedBox=module+(pass==Pass::Layout?layout.ui_layout_box:layout.ui_interactive_box);
            const auto expectedBoxEnd=module+(pass==Pass::Layout?layout.ui_layout_box_end:layout.ui_interactive_box_end);
            if(!read(context,table+layout.ui_box_slot,&box,sizeof box)||box!=expectedBox||
               !read(context,table+layout.ui_box_end_slot,&boxEnd,sizeof boxEnd)||boxEnd!=expectedBoxEnd)return std::nullopt;
        }
        uint64_t number=0;
        if(layout.ui_number_slot){
            const auto expectedNumber=module+(pass==Pass::Layout?layout.ui_layout_number:layout.ui_interactive_number);
            if(!read(context,table+layout.ui_number_slot,&number,sizeof number)||number!=expectedNumber)return std::nullopt;
        }
        for(const auto binding: {std::array<uint64_t,3>{layout.ui_label_slot,layout.ui_layout_label,layout.ui_interactive_label},
             {layout.ui_wrapped_label_slot,layout.ui_layout_wrapped_label,layout.ui_interactive_wrapped_label},
             {layout.ui_space_slot,layout.ui_layout_space,layout.ui_interactive_space}}){
            uint64_t function{};
            if(binding[0]&&(!read(context,table+binding[0],&function,sizeof function)||
               function!=module+binding[pass==Pass::Layout?1:2]))return std::nullopt;
        }
        return SignalUi(object,checkbox,button,group,groupEnd,box,boxEnd,number,pass,read,context,module,profile,textClock);
    }

    [[nodiscard]] Pass pass() const noexcept { return pass_; }
    [[nodiscard]] bool buttonsAvailable()const noexcept{return button_!=0;}
    void heading(const char* text)const {
        prepareRow();margins(10,8,10,4);
        label(text);
    }
    void message(const char* text)const {
        prepareRow();margins(10,6,10,8);
        const auto& l=gameLayout(profile_);
        // Native word wrapping computes its own height during layout. It does
        // not widen the entire group and push the numeric field out of view.
        using Wrapped=void(*)(uint64_t,float,const char*);
        reinterpret_cast<Wrapped>(module_+(pass_==Pass::Layout?l.ui_layout_wrapped_label:l.ui_interactive_wrapped_label))(object_,260.f,text);
    }
    detail::NumberInputResult numberField(const char* name,detail::NumberInputDraft& draft,
            int32_t value,int32_t minimum,int32_t maximum,bool enabled)const {
        draft.synchronize(value);
        prepareRow();label(name);
        prepareRow();nextHeight(28.f);
        const auto& l=gameLayout(profile_);
        option(l.ui_width_enabled,l.ui_width_value,180.f);
        option(l.ui_align_enabled,l.ui_align_value,uint32_t{0x20});
        if(pass_==Pass::Layout){
            reinterpret_cast<void(*)(uint64_t)>(module_+l.ui_layout_space)(object_);
            return {false,draft.value(minimum,maximum)};
        }
        const auto before=draft.text;const auto beforeLength=draft.length;
        std::array<float,4> rect{};
        reinterpret_cast<void(*)(uint64_t,float*)>(module_+l.ui_next_rect)(object_,rect.data());
        const auto context=nativeContext();
        struct Reset {uint64_t object,function;~Reset(){reinterpret_cast<void(*)(uint64_t)>(function)(object);}} reset{object_,module_+l.ui_reset_declaration};
        // A field must explicitly enter insertion mode (bit 9). Without it,
        // Nuklear accepts focus and draws the caret but stays in VIEW mode:
        // both text input and Backspace/Delete are ignored. Selectable,
        // clipboard and Enter alone do not enable editing.
        // Keep the buffer in SDK memory; no foreign std::string crosses here.
        constexpr uint32_t fieldFlags=(1u<<9)|(1u<<6)|(1u<<5)|(1u<<2);
        using Edit=uint32_t(*)(uint64_t,uint32_t,char*,int*,int,void*);
        const auto state=reinterpret_cast<Edit>(module_+l.ui_edit_string)(context,fieldFlags|(enabled?0u:1u),
            draft.text.data(),&draft.length,int(draft.text.size()),nullptr);
        if(enabled&&(state&1))requestTextInput(rect);
        if(draft.length<0||draft.length>=int(draft.text.size()))throw std::runtime_error("Invalid native edit length");
        draft.text[size_t(draft.length)]=0;
        const bool changed=draft.length!=beforeLength||draft.text!=before;
        if(changed)draft.modified=true;
        return {changed,draft.value(minimum,maximum)};
    }
    // Native integer property: click its value to type; range is mod-owned.
    int32_t numberInput(const char* label,int32_t value,int32_t minimum,int32_t maximum,bool enabled)const {
        if(!enabled||!number_){(void)button(label,false);return value;}
        prepareRow();
        using Number=int32_t(*)(uint64_t,const char*,int32_t,int32_t,int32_t,int32_t);
        return reinterpret_cast<Number>(number_)(object_,label,minimum,value,maximum,1);
    }
    float textWidth(const char* text)const {
        if(pass_!=Pass::Interactive)return 0;
        const auto& l=gameLayout(profile_);uint64_t style{},font{},callback{},handle{};float height{},scale{};
        if(!l.ui_interactive_style||!read_(context_,object_+l.ui_interactive_style,&style,sizeof style)||
           !read_(context_,style+l.ui_style_font,&font,sizeof font)||!read_(context_,font,&handle,sizeof handle)||
           !read_(context_,font+l.ui_font_height,&height,sizeof height)||
           !read_(context_,font+l.ui_font_width,&callback,sizeof callback)||!callback||
           !read_(context_,object_+l.ui_interactive_layout+l.ui_layout_scale,&scale,sizeof scale)||
           !std::isfinite(height)||height<=0||!std::isfinite(scale)||scale<=0)
            throw std::runtime_error("Invalid native UI font");
        using Measure=float(*)(uint64_t,float,const char*,int);
        const auto width=reinterpret_cast<Measure>(callback)(handle,height,text,int(std::strlen(text)));
        if(!std::isfinite(width)||width<0)throw std::runtime_error("Invalid native text width");
        return width+64.f*scale; // Checkbox, property value and horizontal padding.
    }
    // Same text/flags signature in both passes; layout ignores text and flags.
    // Return value is one byte. Bit 0 disables clicks in the interactive pass.
    bool button(const char* label,bool enabled)const {
        if(!button_)return false;
        prepareRow();
        if(fillRows_){const auto& l=gameLayout(profile_);
            option(l.ui_width_enabled,l.ui_width_value,260.f);
            option(l.ui_align_enabled,l.ui_align_value,uint32_t{0x20});}
        using Button=uint8_t(*)(uint64_t,const char*,uint32_t);
        return reinterpret_cast<Button>(button_)(object_,label,enabled?0u:1u)!=0;
    }

    // Use the game's own scroll group: wheel, clipping and draggable scrollbar
    // share its normal input handling. The child emitter belongs to the parent
    // and is borrowed only until endGroup, in BOTH native rendering passes.
    template<class Draw> void scroll(const char* key,float height,Draw draw) const {
        scroll(key,height,0.f,draw);
    }
    template<class Draw> void scroll(const char* key,float height,float minimumWidth,Draw draw) const {
        if(!group_){draw(*this);return;}
        nextHeight(height);
        // Scope only the extension group's scrollbar dimensions. Native theme
        // colours, hover/drag behaviour and every other game panel are kept.
        struct ScrollStyle {
            uint64_t address=0;std::array<float,2> previous{};
            ~ScrollStyle(){if(address)std::memcpy(reinterpret_cast<void*>(address),previous.data(),sizeof previous);}
        } style;
        if(pass_==Pass::Interactive){
            const auto& l=gameLayout(profile_);const auto context=nativeContext();float scale{};
            if(!read_(context_,object_+l.ui_interactive_layout+l.ui_layout_scale,&scale,sizeof scale)||!std::isfinite(scale)||scale<=0||
               !read_(context_,context+l.ui_context_scroll_size,style.previous.data(),sizeof style.previous))
                throw std::runtime_error("Invalid native scrollbar style");
            for(float size:style.previous)if(!std::isfinite(size)||size<=0)throw std::runtime_error("Invalid scrollbar size");
            style.address=context+l.ui_context_scroll_size;
            const std::array<float,2> sizes{std::min(style.previous[0],10.f*scale),std::min(style.previous[1],10.f*scale)};
            std::memcpy(reinterpret_cast<void*>(style.address),sizes.data(),sizeof sizes);
        }
        using Begin=uint64_t(*)(uint64_t,const char*,uint32_t);
        // This game fork uses 0x800 to suppress the horizontal scrollbar.
        const auto childObject=reinterpret_cast<Begin>(group_)(object_,key,0u);
        struct End {uint64_t object,function;~End(){reinterpret_cast<void(*)(uint64_t)>(function)(object);}} end{object_,groupEnd_};
        auto child=bind(read_,context_,module_,childObject,profile_,textClock_);
        if(!child||child->pass()!=pass_)throw std::runtime_error("Invalid native scroll emitter");
        // A scroll child starts with an EMPTY layout tree, not with the
        // parent's column. Without a root, only item zero is calculated and
        // the remaining controls receive zero rectangles (an empty panel).
        // The root's height must remain automatic: its full content height
        // drives scrolling, independently of the capped viewport above.
        const auto& layout=gameLayout(profile_);
        if(pass_==Pass::Interactive)child->fitScrollWidth(*this,minimumWidth);
        child->option(layout.ui_width_enabled,layout.ui_width_value,1.f);
        child->option(layout.ui_flow_enabled,layout.ui_flow_value,uint32_t{3}); // Native vertical column.
        child->option(layout.ui_align_enabled,layout.ui_align_value,uint32_t{0xa0}); // Rows fill horizontally.
        reinterpret_cast<void(*)(uint64_t)>(child->box_)(childObject);
        End content{childObject,child->boxEnd_}; // Close the child BEFORE the parent's scroll group.
        child->fillRows_=true;
        draw(*child);
    }

    // A flat theme-coloured rule with breathing room, never a disabled button.
    void separator() const {
        prepareRow();margins(10,8,10,8);nextHeight(1.f);
        const auto& l=gameLayout(profile_);
        if(pass_==Pass::Layout){reinterpret_cast<void(*)(uint64_t)>(module_+l.ui_layout_space)(object_);return;}
        std::array<float,4> rect{};
        reinterpret_cast<void(*)(uint64_t,float*)>(module_+l.ui_next_rect)(object_,rect.data());
        struct Reset {uint64_t object,function;~Reset(){reinterpret_cast<void(*)(uint64_t)>(function)(object);}} reset{object_,module_+l.ui_reset_declaration};
        const auto context=nativeContext();uint64_t window{};uint32_t colour{};
        if(!read_(context_,context+l.ui_context_window,&window,sizeof window)||!window||
           !read_(context_,context+l.ui_context_border_color,&colour,sizeof colour))throw std::runtime_error("Invalid native separator context");
        if(reinterpret_cast<uint8_t(*)(float*,uint64_t)>(module_+l.ui_widget)(rect.data(),context))
            reinterpret_cast<void(*)(uint64_t,const float*,float,uint32_t)>(module_+l.ui_fill_rect)(window+l.ui_window_commands,rect.data(),0.f,colour);
    }

    // In the editor body at RVA 0x7a0a40, capture +0x38 borrows the original
    // Signal*. Copy only its full identity; never retain that native pointer.
    static std::optional<uint64_t> editorSignal(ReadMemory read,void* context,uint64_t capture,
            LiveStateProfile profile=LiveStateProfile::Windows119) noexcept {
        const auto captureOffset=gameLayout(profile).editor_signal_capture;
        if(!captureOffset)return std::nullopt;
        constexpr uint64_t limit=0x7fffffff0000ULL;
        uint64_t signal{},id{},again{};
        if(!read||capture<0x10000||capture>limit-0x40||
           !read(context,capture+captureOffset,&signal,sizeof signal)||signal<0x10000||signal>limit-8||
           !read(context,signal,&id,sizeof id)||id>>48!=8||
           !read(context,capture+captureOffset,&again,sizeof again)||again!=signal)return std::nullopt;
        return id;
    }

    // This is an in-process call, never a remote-process operation. The native
    // primitive updates uint32_t synchronously; neither bool* nor game-owned
    // signal bytes are passed. The caller keeps label storage alive for the
    // entire layout/render cycle. Tooltip rendering is not wired yet.
    void checkbox(const char* label,const char* /*description*/,uint32_t& value) const {
        prepareRow();
        using Checkbox=void(*)(uint64_t,const char*,uint32_t*);
        reinterpret_cast<Checkbox>(checkbox_)(object_,label,&value);
    }
private:
    void prepareRow()const {
        if(fillRows_){const auto& layout=gameLayout(profile_);
            option(layout.ui_align_enabled,layout.ui_align_value,uint32_t{0xa0});margins(10,2,10,2);}
    }
    void label(const char* text)const {
        const auto& l=gameLayout(profile_);
        reinterpret_cast<void(*)(uint64_t,const char*,uint32_t)>(module_+(pass_==Pass::Layout?l.ui_layout_label:l.ui_interactive_label))(object_,text,0x11);
    }
    void margins(float left,float top,float right,float bottom)const {
        const auto& l=gameLayout(profile_);option(l.ui_margin_enabled,l.ui_margin_value,std::array<float,4>{left,top,right,bottom});
    }
    uint64_t nativeContext()const {
        uint64_t context{};const auto& l=gameLayout(profile_);
        if(!read_(context_,object_+l.ui_interactive_context,&context,sizeof context)||context<0x10000||context>0x7fffffff0000ULL-0x5000)
            throw std::runtime_error("Invalid native UI context");
        return context;
    }
    void requestTextInput(const std::array<float,4>& rect)const {
        if(!textClock_)throw std::runtime_error("Native text input clock unavailable");
        const auto& l=gameLayout(profile_);uint64_t owner{};std::array<int32_t,4> area{},previous{};int64_t previousTick{};
        if(!read_(context_,object_+l.ui_owner,&owner,sizeof owner)||owner<0x10000||owner>0x7fffffff0000ULL-l.ui_owner_text_counter-8||
           !read_(context_,owner+l.ui_owner_text_rect,previous.data(),sizeof previous)||
           !read_(context_,owner+l.ui_owner_text_counter,&previousTick,sizeof previousTick))
            throw std::runtime_error("Invalid native text input owner");
        for(size_t i=0;i<area.size();++i){
            if(!std::isfinite(rect[i])||std::abs(rect[i])>1e7f)throw std::runtime_error("Invalid native text input area");
            area[i]=int32_t(rect[i]);
        }
        const auto now=textClock_();
        // Refresh only while this field is active. The game's normal update
        // starts/stops SDL text input and handles the OS input area itself.
        std::memcpy(reinterpret_cast<void*>(owner+l.ui_owner_text_rect),area.data(),sizeof area);
        std::memcpy(reinterpret_cast<void*>(owner+l.ui_owner_text_counter),&now,sizeof now);
    }
    template<class T> void option(size_t enabledOffset,size_t valueOffset,T value)const {
        const uint8_t enabled=1;
        std::memcpy(reinterpret_cast<void*>(object_+enabledOffset),&enabled,sizeof enabled);
        std::memcpy(reinterpret_cast<void*>(object_+valueOffset),&value,sizeof value);
    }
    void fitScrollWidth(const SignalUi& parent,float minimumWidth)const {
        const auto& layout=gameLayout(profile_);
        // The first pass does not yet know the viewport's resolved width.
        // The interactive child owns a copy of that layout. Resize its root
        // using the parent's now-resolved rectangle, then ask the native
        // layout engine to recalculate it BEFORE any child consumes a row.
        // All touched data is transient UI layout, never the saved network.
        const auto tree=object_+layout.ui_interactive_layout;
        float width{},scale{},oldWidth{};uint64_t items{};uint32_t rootFlags{};
        if(!read_(context_,parent.object_+layout.ui_interactive_width,&width,sizeof width)||
           !read_(context_,tree+layout.ui_layout_scale,&scale,sizeof scale)||
           !read_(context_,tree+layout.ui_layout_items,&items,sizeof items)||
           items<0x10000||items>0x7fffffff0000ULL-0x24||
           !read_(context_,items,&rootFlags,sizeof rootFlags)||(rootFlags&7)!=3||
           !read_(context_,items+layout.ui_layout_item_width,&oldWidth,sizeof oldWidth)||
           !std::isfinite(width)||!std::isfinite(scale)||scale<=0||width<0)
            throw std::runtime_error("Invalid native scroll dimensions");
        // Keep a right-hand gutter for the native scrollbar and group padding.
        // Widths here are already physical pixels, including the game's scale.
        width=std::max({1.f,width-32.f*scale,minimumWidth});
        std::memcpy(reinterpret_cast<void*>(items+layout.ui_layout_item_width),&width,sizeof width);
        reinterpret_cast<void(*)(uint64_t)>(module_+layout.ui_layout_calculate)(tree);
    }
    void nextHeight(float value) const {
        const auto& layout=gameLayout(profile_);
        if(!layout.ui_height_enabled||!layout.ui_height_value)return;
        const uint8_t enabled=1;
        std::memcpy(reinterpret_cast<void*>(object_+layout.ui_height_enabled),&enabled,sizeof enabled);
        std::memcpy(reinterpret_cast<void*>(object_+layout.ui_height_value),&value,sizeof value);
    }
    SignalUi(uint64_t object,uint64_t checkbox,uint64_t button,uint64_t group,uint64_t groupEnd,
             uint64_t box,uint64_t boxEnd,uint64_t number,Pass pass,
             ReadMemory read,void* context,uint64_t module,LiveStateProfile profile,TextInputClock textClock)
        :object_(object),checkbox_(checkbox),button_(button),group_(group),groupEnd_(groupEnd),box_(box),boxEnd_(boxEnd),number_(number),pass_(pass),
         read_(read),context_(context),module_(module),profile_(profile),textClock_(textClock){}
    uint64_t object_,checkbox_,button_,group_,groupEnd_,box_,boxEnd_,number_;
    Pass pass_;
    ReadMemory read_;void* context_;uint64_t module_;LiveStateProfile profile_;
    bool fillRows_=false;
    TextInputClock textClock_=nullptr;
};
}
