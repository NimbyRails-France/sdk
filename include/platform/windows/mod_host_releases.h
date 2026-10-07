#pragma once
#include <windows.h>
#include <array>
#include <cstdint>
#include <mutex>
#include <stdexcept>

namespace nimby::mod_host {
// Reserve bounded retirement capacity before accepting a publication. A dead
// worker cannot lose its cleanup when the native publisher is temporarily busy.
// The resident timer retains SDK code and opaque owner IDs, never mod callbacks.
class OwnerReleases {
public:
    using Release = unsigned (*)(uint64_t owner,unsigned pending) noexcept;
private:
    struct Slot {uint64_t owner{};unsigned pending{};};
    std::array<Slot,64> slots_{};
    std::mutex mutex_;
    PTP_TIMER timer_{};
    Release release_;
    unsigned all_;
    bool servicing_=false;
    size_t next_=0;
    static void CALLBACK tick(PTP_CALLBACK_INSTANCE,void* context,PTP_TIMER) noexcept {
        static_cast<OwnerReleases*>(context)->service();
    }
    void service() noexcept {
        // Neither the registry lock nor overlapping timer callbacks can cover
        // native work. Four attempts per tick keep a stalled owner bounded.
        {std::lock_guard lock(mutex_);if(servicing_)return;servicing_=true;}
        for(unsigned attempt=0;attempt<4;++attempt){
            uint64_t owner{};unsigned pending{};size_t index{};
            {std::lock_guard lock(mutex_);
                for(size_t n=0;n<slots_.size();++n){index=next_;next_=(next_+1)%slots_.size();
                    if(slots_[index].pending){owner=slots_[index].owner;pending=slots_[index].pending;break;}}
            }
            if(!owner)break;
            const unsigned complete=release_(owner,pending);
            {std::lock_guard lock(mutex_);
                auto& slot=slots_[index];slot.pending&=~complete;if(!slot.pending)slot={};}
        }
        {std::lock_guard lock(mutex_);servicing_=false;
            bool pending=false;for(const auto& slot:slots_)pending|=slot.pending!=0;
            if(!pending)SetThreadpoolTimer(timer_,nullptr,0,0);
        }
    }
public:
    OwnerReleases(Release release,unsigned pending):release_(release),all_(pending){
        HMODULE module{};
        if(!release||!pending)throw std::invalid_argument("Missing owner retirement action");
        if(!GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS|GET_MODULE_HANDLE_EX_FLAG_PIN,
            reinterpret_cast<LPCWSTR>(&tick),&module))throw std::runtime_error("Cannot retain owner cleanup module");
        timer_=CreateThreadpoolTimer(&tick,this,nullptr);
        if(!timer_)throw std::runtime_error("Cannot create owner cleanup timer");
    }
    bool track(uint64_t owner){
        if(!owner)return false;
        std::lock_guard lock(mutex_);
        for(const auto& slot:slots_)if(slot.owner==owner)return !slot.pending;
        for(auto& slot:slots_)if(!slot.owner){slot.owner=owner;return true;}
        return false;
    }
    void retire(uint64_t owner) noexcept {
        std::lock_guard lock(mutex_);
        for(auto& slot:slots_)if(slot.owner==owner){
            slot.pending=all_;
            const ULARGE_INTEGER due{.QuadPart=static_cast<ULONGLONG>(-100000ll)};
            FILETIME when{due.LowPart,due.HighPart};
            SetThreadpoolTimer(timer_,&when,10,0);
            return;
        }
    }
};
}
