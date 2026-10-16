#include <platform/windows/mod_host_protocol.h>
#include <nimby/detail/mod_options_bridge.h>
#include <nimby/detail/diagnostics.hpp>
#include <windows.h>
#include <array>
#include <atomic>
#include <bit>
#include <mutex>
#include <stdexcept>

namespace nimby::mod_host {
namespace {
constexpr uint32_t addOperation=150,removeOperation=151,readOperation=152,discardOperation=153;
template<class F> uint32_t boundary(F&& f) noexcept {
    try{return f();}catch(const std::bad_alloc&){return NIMBY_RESOURCE_LIMIT;}
    catch(const std::invalid_argument&){return NIMBY_INVALID_ARGUMENT;}catch(...){return NIMBY_INTERNAL_ERROR;}
}
template<class T,class... Args> uint32_t call(const char* name,Args... args) {
    const auto module=GetModuleHandleW(L"NimbySignalUiBridge-experimental-v1.dll");
    const auto fn=module?std::bit_cast<T>(GetProcAddress(module,name)):nullptr;
    return fn?fn(args...):NIMBY_HOOKS_UNAVAILABLE;
}
// Registry removal builds an immutable replacement snapshot. Memory pressure
// can therefore reject it without revoking the registration. Retain its token
// independently of the worker rather than forgetting a failed final cleanup.
// Reserve capacity BEFORE Register: even a failed Wake/rollback owns a slot.
class OptionsReleases {
    struct Slot {uint64_t owner{};bool reserved=false,pending=false;};
    std::array<Slot,64> slots_{};
    std::mutex mutex_;
    PTP_TIMER timer_{};
    bool servicing_=false;
    bool cleanupReported_=false,saturationReported_=false;
    size_t next_=0;
    static void CALLBACK tick(PTP_CALLBACK_INSTANCE,void* context,PTP_TIMER) noexcept {
        static_cast<OptionsReleases*>(context)->service();
    }
    void service() noexcept {
        {std::lock_guard lock(mutex_);if(servicing_)return;servicing_=true;}
        for(unsigned attempt=0;attempt<4;++attempt){
            uint64_t owner{};size_t index{};
            {std::lock_guard lock(mutex_);
                for(size_t n=0;n<slots_.size();++n){index=next_;next_=(next_+1)%slots_.size();
                    if(slots_[index].pending){owner=slots_[index].owner;break;}}
            }
            if(!owner)break;
            // Never hold the capacity lock while invoking the resident SDK.
            // No callback, pointer or handle owned by a mod is retained here.
            const auto status=call<NimbyOptionsRemoveV1>("NimbyOptions_RemoveV1",owner);
            if(status==NIMBY_OK||status==NIMBY_INVALID_HANDLE){
                std::lock_guard lock(mutex_);slots_[index]={};saturationReported_=false;
            }
        }
        bool recovered=false;
        {std::lock_guard lock(mutex_);servicing_=false;
            bool pending=false;for(const auto& slot:slots_)pending|=slot.pending;
            if(!pending){SetThreadpoolTimer(timer_,nullptr,0,0);recovered=cleanupReported_;cleanupReported_=false;}
        }
        if(recovered)detail::diagnostics::write("loader","INFO","Options cleanup recovered: all deferred registrations retired");
    }
public:
    static constexpr size_t capacity=64;
    OptionsReleases(){
        HMODULE module{};
        if(!GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS|GET_MODULE_HANDLE_EX_FLAG_PIN,
            reinterpret_cast<LPCWSTR>(&tick),&module))throw std::runtime_error("Cannot retain options cleanup module");
        timer_=CreateThreadpoolTimer(&tick,this,nullptr);
        if(!timer_)throw std::runtime_error("Cannot create options cleanup timer");
    }
    size_t reserve(){
        bool announce=false;
        {std::lock_guard lock(mutex_);
            for(size_t i=0;i<slots_.size();++i)if(!slots_[i].reserved){slots_[i].reserved=true;return i;}
            announce=!saturationReported_;saturationReported_=true;
        }
        if(announce)detail::diagnostics::write("loader","WARN","Options cleanup capacity reached: 64 retained registrations; new registration refused before publication");
        return capacity;
    }
    void attach(size_t index,uint64_t owner){std::lock_guard lock(mutex_);slots_[index].owner=owner;}
    void cancel(size_t index) noexcept {std::lock_guard lock(mutex_);slots_[index]={};saturationReported_=false;}
    void forget(uint64_t owner) noexcept {
        std::lock_guard lock(mutex_);
        for(auto& slot:slots_)if(slot.owner==owner&&!slot.pending){slot={};saturationReported_=false;return;}
    }
    bool retire(uint64_t owner) noexcept {
        bool retained=false,announce=false;
        {std::lock_guard lock(mutex_);
            for(auto& slot:slots_)if(slot.owner==owner){
                slot.pending=true;retained=true;announce=!cleanupReported_;cleanupReported_=true;
                const ULARGE_INTEGER due{.QuadPart=static_cast<ULONGLONG>(-100000ll)};
                FILETIME when{due.LowPart,due.HighPart};SetThreadpoolTimer(timer_,&when,10,0);
                break;
            }
        }
        if(announce)detail::diagnostics::write("loader","WARN","Options cleanup deferred: failed removal retained by bounded SDK timer");
        return retained;
    }
};
std::atomic<OptionsReleases*> retainedOptions{};
OptionsReleases& optionsReleases(){
    // One timer for all workers, retained even after the last worker exits.
    static auto* queue=[](){auto* value=new OptionsReleases;
        retainedOptions.store(value,std::memory_order_release);return value;}();
    return *queue;
}
struct OptionsReservation {
    OptionsReleases& queue;size_t index;bool retained=false;
    ~OptionsReservation(){if(index!=OptionsReleases::capacity&&!retained)queue.cancel(index);}
};
bool releaseOptions(OptionsReleases& queue,uint64_t owner) noexcept {
    const auto status=call<NimbyOptionsRemoveV1>("NimbyOptions_RemoveV1",owner);
    if(status==NIMBY_OK||status==NIMBY_INVALID_HANDLE){queue.forget(owner);return true;}
    return queue.retire(owner);
}
uint32_t add(const char* bytes,uint32_t size,uint64_t* owner) noexcept {
    if(owner)*owner=0;
    if(!owner||!bytes||!size||size>NIMBY_OPTIONS_SCHEMA_LIMIT)return NIMBY_INVALID_ARGUMENT;
    return boundary([&]{Request request;request.operation=addOperation;append(request,bytes,size);
        Reply reply;const auto status=invoke(request,reply,0);if(status==NIMBY_OK)*owner=reply.args[0];return status;});
}
uint32_t remove(uint64_t owner) noexcept {
    return boundary([&]{Request request;request.operation=removeOperation;request.args[0]=owner;
        Reply reply;return invoke(request,reply,0);});
}
uint32_t discard(uint64_t owner) noexcept {
    return boundary([&]{Request request;request.operation=discardOperation;request.args[0]=owner;
        Reply reply;return invoke(request,reply,0);});
}
uint32_t readOptions(uint64_t owner,uint64_t known,char* out,uint32_t capacity,uint32_t* written,uint64_t* revision) noexcept {
    if(written)*written=0;
    if(revision)*revision=0;
    if(!written||!revision||capacity>NIMBY_OPTIONS_VALUES_LIMIT||(!out&&capacity))return NIMBY_INVALID_ARGUMENT;
    return boundary([&]{Request request;request.operation=readOperation;request.args={owner,known,capacity};
        Reply reply;const auto status=invoke(request,reply,capacity);
        if(status==NIMBY_OK){if(reply.data.size()>capacity)return uint32_t(NIMBY_INVALID_BINARY);
            if(!reply.data.empty())std::memcpy(out,reply.data.data(),reply.data.size());
            *written=static_cast<uint32_t>(reply.data.size());*revision=reply.args[1];}
        return status;});
}
}
uint32_t dispatchOptions(const Request& request,Reply& reply,Owners& owners) {
    const auto& a=request.args;
    if(request.operation==addOperation){
        // One registry entry per isolated mod, irrespective of how many times
        // a faulty child asks to register. Failed setup never exposes a token
        // to the child; a failed rollback remains owned by the SDK queue.
        if(owners.options||request.data.empty()||request.data.size()>NIMBY_OPTIONS_SCHEMA_LIMIT)return NIMBY_INVALID_ARGUMENT;
        auto& queue=optionsReleases();OptionsReservation reservation{queue,queue.reserve()};
        if(reservation.index==OptionsReleases::capacity)return NIMBY_RESOURCE_LIMIT;
        uint64_t owner{};
        auto status=call<NimbyOptionsRegisterV1>("NimbyOptions_RegisterV1",reinterpret_cast<const char*>(request.data.data()),
            static_cast<uint32_t>(request.data.size()),&owner);
        if(status!=NIMBY_OK)return status;
        if(!owner)return NIMBY_INVALID_BINARY;
        queue.attach(reservation.index,owner);reservation.retained=true;
        status=call<NimbyOptionsWakeV1>("NimbyOptions_WakeV1",owner,owners.actionWake);
        if(status!=NIMBY_OK){releaseOptions(queue,owner);return status;}
        owners.options=owner;reply.args[0]=owner;return NIMBY_OK;
    }
    if(!a[0]||a[0]!=owners.options)return NIMBY_INVALID_HANDLE;
    if(!request.data.empty())return NIMBY_INVALID_ARGUMENT;
    if(request.operation==discardOperation)
        return call<NimbyOptionsDiscardV1>("NimbyOptions_DiscardV1",a[0]);
    if(request.operation==removeOperation){
        const auto status=call<NimbyOptionsRemoveV1>("NimbyOptions_RemoveV1",a[0]);
        if(status==NIMBY_OK||status==NIMBY_INVALID_HANDLE){
            if(auto* queue=retainedOptions.load(std::memory_order_acquire))queue->forget(a[0]);
            owners.options=0;
        }
        return status;
    }
    if(request.operation==readOperation){
        if(a[2]>NIMBY_OPTIONS_VALUES_LIMIT)return NIMBY_INVALID_ARGUMENT;
        reply.data.resize(static_cast<size_t>(a[2]));uint32_t written{};uint64_t revision{};
        const auto status=call<NimbyOptionsReadV1>("NimbyOptions_ReadV1",a[0],a[1],
            reinterpret_cast<char*>(reply.data.data()),static_cast<uint32_t>(reply.data.size()),&written,&revision);
        if(status!=NIMBY_OK){reply.data.clear();return status;}
        if(written>reply.data.size()){reply.data.clear();return NIMBY_INVALID_BINARY;}
        reply.data.resize(written);reply.args[1]=revision;return NIMBY_OK;
    }
    return NIMBY_INVALID_ARGUMENT;
}
void cleanupOptions(Owners& owners) noexcept {
    if(!owners.options)return;
    if(auto* queue=retainedOptions.load(std::memory_order_acquire)){
        if(!releaseOptions(*queue,owners.options))return;
    }
    else {
        // Only legacy/untracked callers can reach this branch. Never allocate
        // a retirement manager from a noexcept cleanup during memory pressure.
        const auto status=call<NimbyOptionsRemoveV1>("NimbyOptions_RemoveV1",owners.options);
        if(status!=NIMBY_OK&&status!=NIMBY_INVALID_HANDLE)return;
    }
    owners.options=0;
}
void* optionsSymbol(const char* name) noexcept {
    if(!name)return nullptr;
    if(std::strcmp(name,"NimbyOptions_RegisterV1")==0)return std::bit_cast<void*>(&add);
    if(std::strcmp(name,"NimbyOptions_RemoveV1")==0)return std::bit_cast<void*>(&remove);
    if(std::strcmp(name,"NimbyOptions_DiscardV1")==0)return std::bit_cast<void*>(&discard);
    if(std::strcmp(name,"NimbyOptions_ReadV1")==0)return std::bit_cast<void*>(&readOptions);
    return nullptr;
}
}
