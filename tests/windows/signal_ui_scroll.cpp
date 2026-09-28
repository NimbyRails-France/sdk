// Exercise SignalUi itself through a synthetic, fingerprint-shaped function
// table. These trampolines call fixtures, never the game or an installed mod.
#include <windows.h>
#include "engine/signal_ui.h"
#include <array>
#include <vector>
#include <string>
#include <iostream>
#include <limits>
#define CHECK(x) do{if(!(x))throw std::runtime_error("Failed line "+std::to_string(__LINE__)+": " #x);}while(false)
namespace {
template<class T>T get(uint64_t p,size_t offset){T v;std::memcpy(&v,reinterpret_cast<void*>(p+offset),sizeof v);return v;}
template<class T>void put(uint64_t p,size_t offset,T v){std::memcpy(reinterpret_cast<void*>(p+offset),&v,sizeof v);}
struct Fixture {
    std::array<unsigned char,0x230> parent{},child{};
    std::array<unsigned char,0x24> root{};
    std::array<unsigned char,0x100> style{};
    std::array<unsigned char,0x18> font{};
    std::array<unsigned char,0x5000> context{};
    std::array<unsigned char,0x100> window{};
    std::array<unsigned char,0x6600> owner{};
    std::optional<std::string> edited;
    std::vector<std::string> calls;
    bool interactive=false,column=false,brokenChild=false;
    size_t rows=0;float width=0;
    uint64_t image=0;
    uint64_t p()const{return reinterpret_cast<uint64_t>(parent.data());}
    uint64_t c()const{return reinterpret_cast<uint64_t>(child.data());}
    uint64_t r()const{return reinterpret_cast<uint64_t>(root.data());}
    uint64_t nk()const{return reinterpret_cast<uint64_t>(context.data());}
    uint64_t w()const{return reinterpret_cast<uint64_t>(window.data());}
    uint64_t o()const{return reinterpret_cast<uint64_t>(owner.data());}
    void begin(bool render){
        interactive=render;column=false;rows=0;calls.clear();parent={};child={};root={};
        auto table=image+(render?0xa83470:0xa83818);
        put(p(),0,table);put(c(),0,table);
        put(p(),0x1f0,480.f);put(c(),0xb0+0xb0,1.5f);put(c(),0xb0+0xc0,r());
        put(p(),0x160,1.5f);put(p(),0x1d8,nk());put(c(),0x1d8,nk());
        put(p(),0x08,o());put(c(),0x08,o());put(o(),0x64e0,int64_t{0});
        put(nk(),0x248c,24.f);put(nk(),0x2490,24.f);put(nk(),0x4778,w());put(nk(),0x2420,uint32_t{0xff556677});
        put(r(),0,uint32_t{3|0x800});put(r(),0x1c,1.f);
    }
}*f;
uint64_t beginGroup(uint64_t obj,const char* key,uint32_t flags){
    CHECK(obj==f->p());CHECK(std::string(key)=="panel");CHECK(flags==0); // Both scroll axes enabled.
    CHECK(get<uint8_t>(obj,0x30)==1&&get<float>(obj,0x34)==280);
    if(f->interactive)CHECK(get<float>(f->nk(),0x248c)==15&&get<float>(f->nk(),0x2490)==15);
    f->calls.emplace_back("scroll");return f->brokenChild?0:f->c();
}
void endGroup(uint64_t obj){CHECK(obj==f->p()&&!f->column);f->calls.emplace_back("endScroll");}
void beginColumn(uint64_t obj){
    CHECK(obj==f->c()&&!f->column);CHECK(get<uint8_t>(obj,0x20)==1&&get<uint32_t>(obj,0x24)==3);
    CHECK(get<uint8_t>(obj,0x28)==1&&get<float>(obj,0x2c)>0);
    CHECK(get<uint8_t>(obj,0x30)==0); // Content height must not inherit viewport height.
    f->column=true;f->calls.emplace_back("column");
    for(size_t offset:{size_t(0x18),size_t(0x20),size_t(0x28),size_t(0x30)})put(obj,offset,uint8_t{0});
}
void endColumn(uint64_t obj){CHECK(obj==f->c()&&f->column);f->column=false;f->calls.emplace_back("endColumn");}
void row(uint64_t obj){
    CHECK(obj==f->c()&&f->column); // Regression: old scroll emitted rows without a root.
    CHECK(get<uint8_t>(obj,0x18)==1&&(get<uint32_t>(obj,0x1c)==0xa0||get<uint32_t>(obj,0x1c)==0x20));
    CHECK(get<uint8_t>(obj,0x38)==1&&get<float>(obj,0x3c)==10&&get<float>(obj,0x44)==10);
    ++f->rows;put(obj,0x18,uint8_t{0});put(obj,0x30,uint8_t{0});
}
void checkbox(uint64_t obj,const char*,uint32_t* value){row(obj);f->calls.emplace_back("checkbox");if(f->interactive)*value=1;}
uint8_t button(uint64_t obj,const char* label,uint32_t flags){
    CHECK(*label);row(obj);f->calls.emplace_back("button");return f->interactive&&!flags;
}
void label(uint64_t obj,const char*,uint32_t flags){CHECK(flags==0x11);row(obj);f->calls.emplace_back("label");}
void wrapped(uint64_t obj,float width,const char* text){CHECK(width==260&&*text);row(obj);f->calls.emplace_back("message");}
void space(uint64_t obj){const auto height=get<float>(obj,0x34);row(obj);f->calls.emplace_back(height==1?"separator":"input");}
void nextRect(uint64_t obj,float* rect){
    const auto height=get<float>(obj,0x34);row(obj);rect[0]=10;rect[1]=40;rect[2]=260;rect[3]=height;
}
uint8_t widget(float* rect,uint64_t context){CHECK(context==f->nk());rect[0]+=100;rect[1]+=200;return 1;}
void fill(uint64_t buffer,const float* rect,float rounding,uint32_t colour){
    CHECK(buffer==f->w()+0x68&&rect[0]==110&&rect[1]==240&&rect[3]==1&&rounding==0&&colour==0xff556677);
    f->calls.emplace_back("separator");
}
void reset(uint64_t obj){for(size_t at:{size_t(0x18),size_t(0x28),size_t(0x30),size_t(0x38)})put(obj,at,uint8_t{0});}
uint32_t edit(uint64_t context,uint32_t flags,char* buffer,int* size,int capacity,void* filter){
    CHECK(context==f->nk()&&(flags==0x264||flags==0x265)&&capacity==32&&!filter);
    // The real native editor remains in VIEW mode without ALWAYS_INSERT_MODE.
    // A focused caret is not proof that keyboard editing has been enabled.
    CHECK(flags&(1u<<9));
    if(f->edited&&!(flags&1)){CHECK(f->edited->size()<size_t(capacity));*size=int(f->edited->size());std::memcpy(buffer,f->edited->data(),*size);}
    f->calls.emplace_back("input");return 1;
}
void calculate(uint64_t tree){CHECK(tree==f->c()+0xb0);f->width=get<float>(f->r(),0x1c);f->calls.emplace_back("calculate");}
int32_t number(uint64_t obj,const char*,int32_t min,int32_t value,int32_t max,int32_t step){
    row(obj);CHECK(min==3&&max==100000&&step==1);return f->interactive?725:value;
}
float measure(uint64_t handle,float height,const char* text,int count){
    CHECK(handle==42&&height==18.f&&count==int(std::strlen(text)));return count*9.f;
}
bool read(void*,uint64_t p,void* out,size_t n){SIZE_T got=0;return ReadProcessMemory(GetCurrentProcess(),reinterpret_cast<void*>(p),out,n,&got)&&got==n;}
class Image {
public:
    Image(){
        address=reinterpret_cast<uint64_t>(VirtualAlloc(nullptr,0xa84000,MEM_RESERVE|MEM_COMMIT,PAGE_READWRITE));CHECK(address);
        for(bool render:{false,true}){
            uint64_t table=address+(render?0xa83470:0xa83818);
            slot(table,0x30,render?0x55d620:0x55c950,beginGroup);
            slot(table,0x38,render?0x55dc40:0x55c980,endGroup);
            slot(table,0x08,render?0x55d2c0:0x55c6f0,beginColumn);
            slot(table,0x18,render?0x55d430:0x55c770,endColumn);
            slot(table,0xf0,render?0x560870:0x55cd20,checkbox);
            slot(table,0x90,render?0x55ef00:0x55c9c0,button);
            slot(table,0x138,render?0x562a10:0x55ce50,number);
            slot(table,0xa8,render?0x55f570:0x55ca70,label);
            slot(table,0xb8,render?0x55f640:0x55cb30,wrapped);
            slot(table,0xd0,render?0x55f930:0x55cc10,space);
        }
        trampoline(0x55c690,calculate);
        trampoline(0x55d140,nextRect);trampoline(0x50abd0,widget);trampoline(0x4fc370,fill);
        trampoline(0x55ba40,reset);trampoline(0x5137d0,edit);
        DWORD old;CHECK(VirtualProtect(reinterpret_cast<void*>(address),0xa84000,PAGE_EXECUTE_READ,&old));
        CHECK(FlushInstructionCache(GetCurrentProcess(),reinterpret_cast<void*>(address),0xa84000));
    }
    ~Image(){VirtualFree(reinterpret_cast<void*>(address),0,MEM_RELEASE);}
    uint64_t address{};
private:
    template<class Fn>void trampoline(uint64_t rva,Fn function){
        // Windows x64: mov rax, function; jmp rax. Arguments remain untouched.
        const std::array<unsigned char,12> jump{0x48,0xb8,0,0,0,0,0,0,0,0,0xff,0xe0};
        std::memcpy(reinterpret_cast<void*>(address+rva),jump.data(),jump.size());
        put(address+rva,2,reinterpret_cast<uint64_t>(function));
    }
    template<class Fn>void slot(uint64_t table,size_t at,uint64_t rva,Fn function){put(table,at,address+rva);trampoline(rva,function);}
};
}
int main(){try{
    Image image;Fixture fixture;f=&fixture;f->image=image.address;
    using Ui=nimby::engine::SignalUi;
    for(bool render:{false,true}){
        f->begin(render);auto ui=Ui::bind(read,nullptr,image.address,f->p());CHECK(ui);
        uint32_t checked=0;bool clicked=false;
        ui->scroll("panel",280,[&](auto& child){child.checkbox("One","",checked);child.separator();clicked=child.button("Place",true);});
        CHECK(f->rows==3&&checked==uint32_t(render)&&clicked==render);
        auto expected=std::vector<std::string>{"scroll","column","checkbox","separator","button","endColumn","endScroll"};
        if(render){expected.insert(expected.begin()+1,"calculate");CHECK(f->width==432.f);}
        CHECK(f->calls==expected);
        CHECK(get<float>(f->nk(),0x248c)==24&&get<float>(f->nk(),0x2490)==24); // Restored outside the group.
        // Unwinding must restore BOTH native stacks, including a failed draw.
        f->begin(render);bool caught=false;
        try{ui->scroll("panel",280,[](auto&){throw std::runtime_error("draw failed");});}catch(...){caught=true;}
        CHECK(caught&&!f->column&&f->calls.back()=="endScroll"&&f->calls[f->calls.size()-2]=="endColumn");
        CHECK(get<float>(f->nk(),0x2490)==24); // Also restored on exception.
    }
    f->begin(true);auto ui=Ui::bind(read,nullptr,image.address,f->p(),nimby::engine::LiveStateProfile::Windows119,+[]()->int64_t{return 12345;});CHECK(ui);
    const auto style=reinterpret_cast<uint64_t>(f->style.data()),font=reinterpret_cast<uint64_t>(f->font.data());
    put(f->p(),0x1e0,style);put(style,0xf0,font);put(font,0,uint64_t{42});put(font,8,18.f);
    put(font,0x10,reinterpret_cast<uint64_t>(&measure));put(f->p(),0x160,1.5f);
    CHECK(ui->textWidth("Long label")==90.f+96.f);
    for(float width:{220.f,680.f}){
        f->begin(true);put(f->p(),0x1f0,width);ui->scroll("panel",280,[](auto& child){uint32_t value=0;child.checkbox("Width","",value);});
        CHECK(f->width==width-48.f);
    }
    f->begin(true);ui->scroll("panel",280,1200.f,[](auto& child){
        CHECK(child.numberInput("Spacing",1000,3,100000,true)==725);
        CHECK(child.numberInput("Spacing",1000,3,100000,false)==1000);
    });
    CHECK(f->width==1200.f&&f->rows==2); // Long labels enlarge content, not the viewport.
    // Help consumes one wrapped row in BOTH passes, immediately after its
    // checkbox. Empty help consumes none and does not alter click semantics.
    for(bool interactive:{false,true}) {
        f->begin(interactive);
        auto helpUi=nimby::engine::SignalUi::bind(read,nullptr,f->image,f->p());CHECK(helpUi);
        helpUi->scroll("panel",280,[](auto& child){
            uint32_t value=0;child.checkbox("Option","Translated help below the option",value);
            CHECK(value==(f->interactive?1u:0u));child.checkbox("Other","",value);
        });
        CHECK(f->rows==3);
        const auto at=std::find(f->calls.begin(),f->calls.end(),"checkbox");
        CHECK(at!=f->calls.end()&&*(at+1)=="message"&&*(at+2)=="checkbox");
    }
    nimby::detail::NumberInputDraft draft;
    for(const auto& text:std::vector<std::string>{"","7","725","2","100001","2147483648","12x","750"}){
        f->begin(true);f->edited=text;
        ui->scroll("panel",280,[&](auto& child){
            const auto result=child.numberField("Espacement (m)",draft,1000,3,100000,true);
            CHECK(result.changed&&std::string(draft.text.data(),draft.length)==text);
            CHECK(bool(result.value)==(text=="7"||text=="725"||text=="750"));
        });
        draft.synchronize(1000);CHECK(std::string(draft.text.data(),draft.length)==text); // Async old value cannot erase the draft.
        CHECK(get<int64_t>(f->o(),0x64e0)==12345); // Native SDL lease is refreshed for a focused field.
    }
    f->edited.reset();draft.synchronize(750);draft.synchronize(900);CHECK(draft.parse()==900);
    f->begin(true);f->brokenChild=true;bool caught=false;
    try{ui->scroll("panel",280,[](auto&){CHECK(false);});}catch(...){caught=true;}
    CHECK(caught&&f->calls==std::vector<std::string>({"scroll","endScroll"}));f->brokenChild=false;
    f->begin(true);put(f->p(),0x1f0,std::numeric_limits<float>::quiet_NaN());caught=false;
    try{ui->scroll("panel",280,[](auto&){CHECK(false);});}catch(...){caught=true;}
    CHECK(caught&&f->calls==std::vector<std::string>({"scroll","endScroll"}));
    std::cout<<"PASS: actual SignalUi adapter, scroll child hierarchy, row width, resize and stack unwinding\n";
}catch(const std::exception& e){std::cerr<<e.what()<<'\n';return 1;}}
