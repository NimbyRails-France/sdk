#pragma once
#include "runtime/texture_table.h"
#include <nimby/detail/observation.h>
#include <map>
#include <span>
#include <atomic>
#include <memory>
#include <mutex>
#include <array>

namespace nimby::texture_bridge {
inline constexpr size_t maxTextureBatch=4096,maxOwnerTextures=4096,maxTextureOwners=64,maxOwnedTextures=65536;
// Input is a delta. Validate the entire batch and all ownership conflicts
// before changing the render table. Allocation/merge is linear in table size.
inline uint32_t publishTextures(Table& table,uint64_t owner,std::span<const Command> updates,uint64_t now) {
    if(!owner||updates.empty()||updates.size()>maxTextureBatch)return NIMBY_INVALID_ARGUMENT;
    std::vector<Command> sorted(updates.begin(),updates.end());
    for(auto& row:sorted){
        if(row.signal>>48!=8||!row.set_hash||row.expires<=now||
           (row.half_period_ms&&(row.half_period_ms<100||row.half_period_ms>10000)))return NIMBY_INVALID_ARGUMENT;
        row.owner=owner;
    }
    std::sort(sorted.begin(),sorted.end(),[](const auto& a,const auto& b){return a.signal<b.signal;});
    for(size_t i=1;i<sorted.size();++i)if(sorted[i-1].signal==sorted[i].signal)return NIMBY_INVALID_ARGUMENT;
    bool replacement=true;
    for(const auto& row:sorted){
        const auto at=table.lower(row.signal);
        if(at==table.count()||table.entries[at].signal!=row.signal||table.entries[at].expires<=now)replacement=false;
        else if(table.entries[at].owner!=owner)return NIMBY_RESOURCE_LIMIT;
    }
    if(replacement){
        // Stable ownership/cardinality: changes and renewals touch only this
        // mod's entries and never copy another mod's render table.
        for(const auto& row:sorted)table.entries[table.lower(row.signal)]=row;
        return NIMBY_OK;
    }
    std::map<uint64_t,size_t> owners;
    size_t total=0;
    for(const auto& row:table.entries)if(row.expires>now){++owners[row.owner];++total;}
    for(const auto& row:sorted){
        const auto at=table.lower(row.signal);
        if(at<table.count()&&table.entries[at].signal==row.signal&&table.entries[at].expires>now){
            if(table.entries[at].owner!=owner)return NIMBY_RESOURCE_LIMIT;
        }else{++owners[owner];++total;}
    }
    if(owners[owner]>maxOwnerTextures||owners.size()>maxTextureOwners||total>maxOwnedTextures)return NIMBY_RESOURCE_LIMIT;
    std::vector<Command> merged;merged.reserve(total);
    size_t incoming=0;
    for(const auto& row:table.entries){
        if(row.expires<=now)continue;
        while(incoming<sorted.size()&&sorted[incoming].signal<row.signal)merged.push_back(sorted[incoming++]);
        if(incoming<sorted.size()&&sorted[incoming].signal==row.signal)merged.push_back(sorted[incoming++]);
        else merged.push_back(row);
    }
    while(incoming<sorted.size())merged.push_back(sorted[incoming++]);
    table.entries.swap(merged);return NIMBY_OK;
}
inline uint32_t clearTextures(Table& table,uint64_t owner,std::span<const uint64_t> signals) {
    if(!owner||signals.size()>maxTextureBatch)return NIMBY_INVALID_ARGUMENT;
    std::vector<uint64_t> sorted;sorted.reserve(signals.size());
    for(const auto id:signals){
        if(id>>48!=8)return NIMBY_INVALID_ARGUMENT;
        sorted.push_back(id);
    }
    std::sort(sorted.begin(),sorted.end());
    // Scoped cleanup is idempotent: an absent ID or a lease now owned by a
    // peer is already released for this caller. Validate the entire request
    // before erasing; publication conflicts remain strict in publishTextures.
    std::erase_if(table.entries,[&](const auto& row){return row.owner==owner&&std::binary_search(sorted.begin(),sorted.end(),row.signal);});
    return NIMBY_OK;
}
inline void releaseTextures(Table& table,uint64_t owner) noexcept {
    if(owner)std::erase_if(table.entries,[&](const auto& row){return row.owner==owner;});
}

struct WorldGuard;
struct TextureState {
    Table table;
    uint64_t database=0,simulation=0,signal=0,expires=0;
    std::shared_ptr<WorldGuard> world;
};

// Writers prepare owned copies; rendering only pins and copies one immutable
// entry. A stalled writer cannot hide an already-published restrictive aspect.
class TexturePublications {
    struct Retired {
        std::shared_ptr<const TextureState> state;
        std::unique_ptr<Retired> next;
    };
    mutable std::mutex writers_;
    std::shared_ptr<const TextureState> current_=std::make_shared<TextureState>();
    std::atomic<const TextureState*> visible_{current_.get()};
    std::atomic<unsigned> readers_{0};
    std::unique_ptr<Retired> retired_;
    size_t retiredCount_=0;
    struct OwnerWrites {uint64_t owner=0,revision=0;size_t active=0;};
    std::array<OwnerWrites,maxTextureOwners+1> ownerWrites_{};
    class WriteTicket {
        TexturePublications& publications_;
    public:
        size_t index=maxTextureOwners+1;
        uint64_t revision=0;
        WriteTicket(TexturePublications& publications,uint64_t owner,bool retiring):publications_(publications){
            std::lock_guard lock(publications_.writers_);
            auto& slots=publications_.ownerWrites_;
            for(size_t i=0;i<slots.size();++i)if(slots[i].active&&slots[i].owner==owner){index=i;break;}
            if(index==slots.size())for(size_t i=0;i<slots.size();++i)if(!slots[i].active){index=i;slots[i].owner=owner;break;}
            if(index==slots.size())return;
            auto& slot=slots[index];++slot.active;if(retiring)++slot.revision;revision=slot.revision;
        }
        WriteTicket(const WriteTicket&)=delete;
        ~WriteTicket(){
            if(index==publications_.ownerWrites_.size())return;
            std::lock_guard lock(publications_.writers_);
            auto& slot=publications_.ownerWrites_[index];if(!--slot.active)slot={};
        }
    };
public:
    struct PreparedUpdate {
        uint32_t status;bool changed;
        PreparedUpdate(uint32_t value,bool modified=true)noexcept:status(value),changed(modified){}
    };
    static constexpr size_t maxRetired=8;
    static_assert(std::atomic<const TextureState*>::is_always_lock_free);
    static_assert(std::atomic<unsigned>::is_always_lock_free);
    class View {
        TexturePublications& publications_;
    public:
        const TextureState* state;
        explicit View(TexturePublications& publications):publications_(publications) {
            // Sequential consistency orders reader entry before pointer load.
            // Reclamation observes zero AFTER pointer publication: a later
            // reader must see the new pointer; an earlier reader pins old data.
            publications_.readers_.fetch_add(1,std::memory_order_seq_cst);
            state=publications_.visible_.load(std::memory_order_seq_cst);
        }
        View(const View&)=delete;
        ~View(){publications_.readers_.fetch_sub(1,std::memory_order_seq_cst);}
    };
    struct Selected {Command command;uint64_t database=0,simulation=0;};
    template<class Validate> Selected read(uint64_t signal,Validate validate) noexcept {
        const View view(*this);
        const auto& value=*view.state;
        Selected result{{},value.database,value.simulation};
        const auto at=value.table.lower(signal);
        if(at<value.table.count()&&value.table.entries[at].signal==signal&&validate(value.world.get()))result.command=value.table.entries[at];
        return result;
    }
    Selected read(uint64_t signal) noexcept {return read(signal,[](const auto*){return true;});}
    std::shared_ptr<const TextureState> snapshot() const {
        std::lock_guard lock(writers_);return current_;
    }
    template<class Prepare,class Committed>
    uint32_t update(uint64_t owner,Prepare prepare,Committed committed,bool retiring=false) {
        // Only in-flight writers need retirement barriers. Fixed slots disappear
        // at quiescence, so reloads cannot accumulate owner tombstones forever.
        const WriteTicket ticket(*this,owner,retiring);
        if(ticket.index==ownerWrites_.size())return NIMBY_RESOURCE_LIMIT;
        for(unsigned attempt=0;attempt<3;++attempt){
            const auto before=snapshot();
            auto next=std::make_shared<TextureState>(*before);
            const PreparedUpdate prepared=prepare(*next);
            if(prepared.status!=NIMBY_OK)return prepared.status;
            auto retired=prepared.changed?std::make_unique<Retired>():nullptr;
            std::unique_ptr<Retired> garbage;
            {
                std::lock_guard lock(writers_);
                // A release invalidates only its owner's prepared publications.
                // Even an idempotent foreign clear cannot cancel a healthy peer.
                if(ticket.revision!=ownerWrites_[ticket.index].revision)return NIMBY_RESOURCE_LIMIT;
                if(current_!=before)continue;
                // Keep this owner's retirement barrier but do not force other
                // publishers to retry when cleanup changed no owned entries.
                if(!prepared.changed)return NIMBY_OK;
                if(readers_.load(std::memory_order_seq_cst)==0){
                    garbage=std::move(retired_);retiredCount_=0;
                }
                // Even a suspended renderer cannot grow retained memory without
                // bound. Refusal leaves the last valid publication untouched.
                if(retiredCount_==maxRetired)return NIMBY_RESOURCE_LIMIT;
                retired->state=std::move(current_);retired->next=std::move(retired_);
                retired_=std::move(retired);++retiredCount_;
                current_=std::move(next);
                visible_.store(current_.get(),std::memory_order_seq_cst);
                committed(*current_); // Only small diagnostic fields, no allocation.
            }
            // Replaced vectors and retired nodes are destroyed on this writer,
            // after releasing its mutex, never in the render callback.
            return NIMBY_OK;
        }
        return NIMBY_RESOURCE_LIMIT;
    }
    template<class Prepare> uint32_t update(uint64_t owner,Prepare prepare,bool retiring=false) {
        return update(owner,prepare,[](const TextureState&)noexcept{},retiring);
    }
};
}
