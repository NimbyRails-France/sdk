// Cosmetic metadata localisation for the qualified Windows 1.19 executable.
// Discovery keeps the game's cache unchanged. UI copies/arguments are translated
// for the picker and the in-game manager. No save, signal ID or file is rewritten.
#include <engine/binary_identity.h>
#include <engine/mod_metadata.h>
#include <engine/signal_construction_defaults.h>
#include <platform/windows/game_language.h>
#include <nimby/detail/diagnostics.hpp>
#include <MinHook.h>
#include <windows.h>
#include <filesystem>
#include <fstream>
#include <array>
#if defined(_MSC_VER)
#include <intrin.h>
#define NRF_CALLER() reinterpret_cast<uintptr_t>(_ReturnAddress())
#else
#define NRF_CALLER() reinterpret_cast<uintptr_t>(__builtin_return_address(0))
#endif

namespace {
uintptr_t base{};
bool enabled=false;
SRWLOCK initialization=SRWLOCK_INIT;
nimby::engine::ModMetadataRegistry registry;
nimby::engine::ConstructionDefaultsRegistry constructionDefaults;
using BuildSignal=uintptr_t(*)(uintptr_t,uintptr_t,uintptr_t,uintptr_t,uintptr_t,uintptr_t,uint8_t);
BuildSignal originalBuildSignal{};
using HashString=uint64_t(*)(const void*);
HashString hashString{};
void registerConstruction(int,const std::string&,const nimby::engine::ModMetadataCatalog&);
using ReadMeta=uintptr_t(*)(uintptr_t,int,uintptr_t);
using GetMeta=uintptr_t(*)(uintptr_t,uintptr_t);
using ListMeta=uintptr_t(*)(uintptr_t,uint8_t);
ReadMeta originalRead{};GetMeta originalGet{};ListMeta originalList{};
using DrawDetails=void(*)(uintptr_t,uintptr_t,uintptr_t);
using DrawLabel=void(*)(uintptr_t,uintptr_t,uint32_t);
DrawDetails originalDetails{};DrawLabel originalLabel{};
using Localize=const char*(*)(const char*,const char*);
Localize originalLocalize{};
// The native manager draws the selected details immediately before its content
// rows on the same UI thread. Keep only that frame's immutable catalogue.
thread_local std::shared_ptr<const nimby::engine::ModMetadataCatalog> detailsCatalog;
using ConstructString=void(*)(void*,const char*,size_t);
using DestroyString=void(*)(void*);
using MoveString=void*(*)(void*,void*);
ConstructString constructString{};DestroyString destroyString{};MoveString moveString{};
bool read(void*,uint64_t address,void* out,size_t size){
    SIZE_T got{};return ReadProcessMemory(GetCurrentProcess(),reinterpret_cast<void*>(address),out,size,&got)&&got==size;
}
template<class T>T value(uintptr_t at){T out{};if(!read(nullptr,at,&out,sizeof out))throw std::runtime_error("Metadata read unavailable");return out;}
// Only decode an owned native string. Never pass a MinGW std::string to MSVC.
template<class Char>std::basic_string<Char> string(uintptr_t at,size_t limit){
    const auto words=value<std::array<uint64_t,4>>(at);
    const auto size=words[2],capacity=words[3],small=16/sizeof(Char);
    if(size>limit||capacity<size||capacity>1024*1024||(capacity<small&&size>=small))throw std::runtime_error("Metadata string bounds");
    std::basic_string<Char> result(size,Char{});
    if(size&&!read(nullptr,capacity<small?at:words[0],result.data(),size*sizeof(Char)))throw std::runtime_error("Metadata string unavailable");
    if(result.find(Char{})!=result.npos)throw std::runtime_error("Embedded zero in metadata");
    return result;
}
std::shared_ptr<const nimby::engine::ModMetadataCatalog> catalogue(uintptr_t path){
    const auto file=std::filesystem::path(string<wchar_t>(path,32767))/L"nrf-metadata.json";
    std::error_code error;
    const auto size=std::filesystem::file_size(file,error);
    if(error==std::errc::no_such_file_or_directory)return {}; // Ordinary mods have no SDK sidecar.
    if(error)throw std::system_error(error,"Cannot read metadata sidecar");
    if(!size||size>nimby::detail::Translations::maximumBytes)throw std::runtime_error("Metadata file size");
    std::ifstream input(file,std::ios::binary);std::string bytes(size,'\0');
    if(!input.read(bytes.data(),std::streamsize(size))||input.peek()!=std::char_traits<char>::eof())throw std::runtime_error("Metadata file changed during read");
    return std::make_shared<const nimby::engine::ModMetadataCatalog>(bytes);
}
uintptr_t discover(uintptr_t out,int kind,uintptr_t path){
    const auto result=originalRead(out,kind,path);
    try {
        if(!value<uint8_t>(out+0x110))return result;
        const auto id=string<char>(out+8,32767);
        // Forget a previous catalogue first: invalid/removed files cannot leave
        // stale translations attached to a reinstalled mod with the same ID.
        registry.replace(kind,id,{});
        constructionDefaults.replace(kind,id,{});
        auto texts=catalogue(path);
        if(texts){
            const nimby::engine::MetadataText fallback{string<char>(out+0x28,16384),string<char>(out+0x68,16384)};
            if(texts->fallback()!=fallback)throw std::runtime_error("Metadata sidecar differs from mod.txt fallback");
            registerConstruction(kind,id,*texts);
            registry.replace(kind,id,std::move(texts));
            nimby::detail::diagnostics::write("sdk","INFO",("Metadata translations registered: "+id).c_str());
        }
    }catch(...){nimby::detail::diagnostics::exception("sdk","Metadata discovery; original text retained");}
    return result;
}
// Construct both replacements using the game's allocator before changing the
// copy. The move operation consumes each temporary. No foreign allocation is
// ever transferred to the game, even for long UTF-8 translations.
struct NativeString {
    alignas(8) std::array<uint64_t,4> words{0,0,0,15};
    explicit NativeString(const std::string& text){constructString(words.data(),text.data(),text.size());}
    NativeString(const NativeString&)=delete;
    NativeString& operator=(const NativeString&)=delete;
    ~NativeString(){destroyString(words.data());}
    void moveTo(uintptr_t destination){moveString(reinterpret_cast<void*>(destination),words.data());}
};
void registerConstruction(int kind,const std::string& id,const nimby::engine::ModMetadataCatalog& catalog){
    std::set<uint64_t> hashes;
    for(const auto& texture:catalog.leftConstruction()){
        NativeString native(texture);
        if(!hashes.insert(hashString(native.words.data())).second)throw std::runtime_error("Ambiguous construction catalogue");
    }
    constructionDefaults.replace(kind,id,std::move(hashes));
}
uintptr_t buildSignal(uintptr_t placement,uintptr_t out,uintptr_t database,uintptr_t flag,
                      uintptr_t preferences,uintptr_t editor,uint8_t reverse){
    const auto result=originalBuildSignal(placement,out,database,flag,preferences,editor,reverse);
    try{
        // This routine builds both the cursor preview and the value subsequently
        // moved into the game's normal construction command (RVA 0x7832e0).
        auto signal=value<std::array<uint8_t,0x64>>(out);
        uint64_t hash{};std::memcpy(&hash,signal.data()+0x38,8);
        if(constructionDefaults.left(hash)&&nimby::engine::leftConstruction(signal,value<uint64_t>(editor+0x148)))
            std::memcpy(reinterpret_cast<void*>(out+0x58),signal.data()+0x58,4);
    }catch(...){nimby::detail::diagnostics::exception("sdk","Signal construction default; native position retained");}
    return result;
}
std::shared_ptr<const nimby::engine::ModMetadataCatalog> sourceCatalogue(uintptr_t meta){
    const auto found=registry.find(value<int>(meta),string<char>(meta+8,32767));
    if(!found)return {};
    const nimby::engine::MetadataText source{string<char>(meta+0x28,16384),string<char>(meta+0x68,16384)};
    return source==found->fallback()?found:nullptr;
}
void translate(uintptr_t meta,std::string_view language){
    const auto found=sourceCatalogue(meta);
    if(!found)return;
    const auto& text=found->resolve(language);
    if(text==found->fallback())return;
    NativeString name(text.name),description(text.description);
    name.moveTo(meta+0x28);description.moveTo(meta+0x68);
}
// The in-game manager reads ModMeta copied from the save and can later use
// those records in enable/disable commands. Never localise that vector itself.
// Its details renderer is read-only: supply a temporary view with two owned
// native strings; all other bytes borrow the source only for this one call.
struct MetadataView {
    alignas(8) std::array<uint8_t,0x110> bytes;
    NativeString name,description;
    MetadataView(uintptr_t meta,const nimby::engine::MetadataText& text)
        :bytes(value<decltype(bytes)>(meta)),name(text.name),description(text.description){
        std::memcpy(bytes.data()+0x28,name.words.data(),32);
        std::memcpy(bytes.data()+0x68,description.words.data(),32);
    }
};
void drawDetails(uintptr_t unused,uintptr_t ui,uintptr_t meta,std::string_view language){
    std::optional<MetadataView> view;
    detailsCatalog.reset();
    try{if(const auto catalog=sourceCatalogue(meta)){detailsCatalog=catalog;view.emplace(meta,catalog->resolve(language));}}
    catch(...){nimby::detail::diagnostics::exception("sdk","In-game metadata details; original text retained");}
    // Original rendering is outside the recovery block: never replay a renderer
    // if the game's own callback raises an exception after starting its work.
    originalDetails(unused,ui,view?reinterpret_cast<uintptr_t>(view->bytes.data()):meta);
}
void details(uintptr_t unused,uintptr_t ui,uintptr_t meta){
    drawDetails(unused,ui,meta,nimby::platform::windows::gameLanguage(read,nullptr,base).value_or(""));
}
void drawLabel(uintptr_t ui,uintptr_t text,uint32_t flags,uintptr_t caller,std::string_view language){
    std::optional<NativeString> localized;
    if(caller==0x66a56a)try{
        // Exact name-cell call in the manager, not a value in an editor,
        // a train name or Steam's publishing form. The pointer is meta+0x28.
        if(const auto catalog=sourceCatalogue(text-0x28))localized.emplace(catalog->resolve(language).name);
    }catch(...){nimby::detail::diagnostics::exception("sdk","In-game metadata row; original text retained");}
    if(caller==0x66ad59&&detailsCatalog)try{
        // RuleInfo tree node: kind +0x20, immutable resource ID +0x28,
        // display name +0x48. Only generated signal-texture resources (10)
        // are qualified here; don't translate by matching arbitrary names.
        if(value<int>(text-0x28)==10){
            const auto id=string<char>(text-0x20,96),source=string<char>(text,16384);
            for(const auto& resource:detailsCatalog->resources())
                if(resource.kind=="textures"&&resource.id==id&&resource.english==source){
                    localized.emplace(detailsCatalog->resolve(resource,language));break;
                }
        }
    }catch(...){nimby::detail::diagnostics::exception("sdk","In-game resource name; original text retained");}
    originalLabel(ui,localized?reinterpret_cast<uintptr_t>(localized->words.data()):text,flags);
}
void label(uintptr_t ui,uintptr_t text,uint32_t flags){
    const auto caller=NRF_CALLER()-base;
    if(caller!=0x66a56a&&caller!=0x66ad59){originalLabel(ui,text,flags);return;}
    drawLabel(ui,text,flags,caller,nimby::platform::windows::gameLanguage(read,nullptr,base).value_or(""));
}
const char* localize(const char* key,const char* fallback){
    // name_loc is the game's existing lookup mechanism in construction menus,
    // signal properties and texture selectors. Handle only generated SDK keys;
    // every other key continues through the game's native language catalogue.
    if(key&&fallback&&nimby::engine::ModMetadataCatalog::nativeKey(key))try{
        if(const auto text=registry.resolveNative(key,fallback,nimby::platform::windows::gameLanguage(read,nullptr,base).value_or("")))return text;
    }catch(...){nimby::detail::diagnostics::exception("sdk","Native resource translation; original text retained");}
    return originalLocalize(key,fallback);
}
uintptr_t get(uintptr_t out,uintptr_t id){
    const auto caller=NRF_CALLER()-base;
    const auto result=originalGet(out,id);
    // Precise call site in the picker, not multiplayer/save/resource readers
    // or the Steam uploader (whose editable metadata must remain the original).
    if(caller==0x5e00f5)try{
        if(value<uint8_t>(out+0x110))translate(out,nimby::platform::windows::gameLanguage(read,nullptr,base).value_or(""));
    }catch(...){nimby::detail::diagnostics::exception("sdk","Metadata selection translation");}
    return result;
}
uintptr_t list(uintptr_t out,uint8_t usableOnly){
    const auto caller=NRF_CALLER()-base;
    const auto result=originalList(out,usableOnly);
    if(caller==0x5dfe84)try{
        const auto span=value<std::array<uintptr_t,3>>(out);
        if(span[1]<span[0]||span[2]<span[1]||(span[1]-span[0])%0x110||(span[1]-span[0])/0x110>100000)throw std::runtime_error("Metadata list bounds");
        const auto language=nimby::platform::windows::gameLanguage(read,nullptr,base).value_or("");
        for(auto at=span[0];at<span[1];at+=0x110)translate(at,language);
    }catch(...){nimby::detail::diagnostics::exception("sdk","Metadata list translation");}
    return result;
}
}

extern "C" __declspec(dllexport) DWORD WINAPI NimbyInternal_Bootstrap(void* argument)noexcept {
    if(argument)return NIMBY_INVALID_ARGUMENT;
    AcquireSRWLockExclusive(&initialization);
    struct Unlock{~Unlock(){ReleaseSRWLockExclusive(&initialization);}} unlock;
    if(enabled)return NIMBY_ALREADY_INITIALIZED;
    try{
        std::array<wchar_t,32768> path{};NimbyBinaryInfo binary{};
        if(!GetModuleFileNameW(nullptr,path.data(),DWORD(path.size()))||nimby::engine::identify(path.data(),binary)!=NIMBY_OK||!binary.recognized_research_build)return NIMBY_INVALID_BINARY;
        base=reinterpret_cast<uintptr_t>(GetModuleHandleW(nullptr));
        struct Entry{uintptr_t rva;std::array<uint8_t,16> bytes;};
        const Entry entries[]{
            {0x2df0c0,{0x48,0x8b,0xc4,0x4c,0x89,0x40,0x18,0x89,0x50,0x10,0x48,0x89,0x48,0x08,0x55,0x53}},
            {0x2e0b70,{0x40,0x53,0x56,0x48,0x83,0xec,0x48,0x48,0x89,0x6c,0x24,0x60,0x48,0x8b,0xf1,0x48}},
            {0x2e0c80,{0x48,0x89,0x5c,0x24,0x10,0x48,0x89,0x74,0x24,0x18,0x48,0x89,0x4c,0x24,0x08,0x57}},
            {0x668ca0,{0x48,0x89,0x5c,0x24,0x08,0x48,0x89,0x74,0x24,0x10,0x57,0x48,0x83,0xec,0x20,0x48}},
            {0x55bc50,{0x48,0x83,0x7a,0x18,0x0f,0x48,0x8b,0x01,0x76,0x03,0x48,0x8b,0x12,0x48,0xff,0xa0}},
            {0x2d82e0,{0x41,0x54,0x41,0x56,0x48,0x83,0xec,0x68,0x4c,0x8b,0xe2,0x4c,0x8b,0xf1,0x48,0x85}},
            {0x7830a0,{0x48,0x89,0x5c,0x24,0x08,0x48,0x89,0x74,0x24,0x10,0x48,0x89,0x7c,0x24,0x18,0x41}},
            {0x2c00,{0x48,0x89,0x6c,0x24,0x10,0x48,0x89,0x74,0x24,0x18,0x48,0x89,0x7c,0x24,0x20,0x41}},
            {0x25630,{0x48,0x89,0x5c,0x24,0x08,0x57,0x48,0x83,0xec,0x30,0x48,0x8b,0xfa,0x48,0x8b,0xd9}},
            {0x2d30,{0x40,0x53,0x48,0x83,0xec,0x30,0x48,0x8b,0x51,0x18,0x48,0x8b,0xd9,0x48,0x83,0xfa}},
            {0x241780,{0x56,0x57,0x48,0x83,0xec,0x48,0x48,0x83,0x79,0x18,0x0f,0x48,0x8b,0x79,0x10,0x76}},
        };
        for(const auto& entry:entries)if(std::memcmp(reinterpret_cast<void*>(base+entry.rva),entry.bytes.data(),16))return NIMBY_INVALID_BINARY;
        constructString=reinterpret_cast<ConstructString>(base+0x2c00);
        moveString=reinterpret_cast<MoveString>(base+0x25630);
        destroyString=reinterpret_cast<DestroyString>(base+0x2d30);
        hashString=reinterpret_cast<HashString>(base+0x241780);
        if(MH_Initialize()!=MH_OK)return NIMBY_INTERNAL_ERROR;
        size_t created=0;
        struct Cleanup{size_t& count;const Entry* entries;~Cleanup(){if(enabled)return;for(size_t i=0;i<count;++i)MH_RemoveHook(reinterpret_cast<void*>(base+entries[i].rva));MH_Uninitialize();}} cleanup{created,entries};
        void* replacements[]{reinterpret_cast<void*>(&discover),reinterpret_cast<void*>(&get),reinterpret_cast<void*>(&list),reinterpret_cast<void*>(&details),reinterpret_cast<void*>(&label),reinterpret_cast<void*>(&localize),reinterpret_cast<void*>(&buildSignal)};
        void** originals[]{reinterpret_cast<void**>(&originalRead),reinterpret_cast<void**>(&originalGet),reinterpret_cast<void**>(&originalList),reinterpret_cast<void**>(&originalDetails),reinterpret_cast<void**>(&originalLabel),reinterpret_cast<void**>(&originalLocalize),reinterpret_cast<void**>(&originalBuildSignal)};
        for(size_t i=0;i<std::size(replacements);++i){const auto target=reinterpret_cast<void*>(base+entries[i].rva);
            if(MH_CreateHook(target,replacements[i],originals[i])!=MH_OK)return NIMBY_INTERNAL_ERROR;
            ++created;if(MH_QueueEnableHook(target)!=MH_OK)return NIMBY_INTERNAL_ERROR;}
        HMODULE self{};
        if(!GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS|GET_MODULE_HANDLE_EX_FLAG_PIN,reinterpret_cast<LPCWSTR>(&discover),&self)||MH_ApplyQueued()!=MH_OK)return NIMBY_INTERNAL_ERROR;
        enabled=true;
        nimby::detail::diagnostics::write("sdk","INFO","Mod metadata localisation active for the picker and in-game manager");
        return NIMBY_OK;
    }catch(...){nimby::detail::diagnostics::exception("sdk","Metadata bootstrap");return NIMBY_INTERNAL_ERROR;}
}
