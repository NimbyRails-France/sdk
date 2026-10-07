#pragma once
#include <nimby/detail/automatic_driving.h>
#include <algorithm>
#include <map>
#include <memory>
#include <span>
#include <vector>

namespace nimby::engine::automatic {
// A publication replaces only one owner's complete batch. Callers validate
// values and serialize access; no mod callbacks or native reads run here.
// Expired batches retain ownership until release: a timeout must not let a
// second mod replace an expired stop with an unrelated permission.
template<class Row> class DrivingPublishers {
public:
    struct Batch { std::shared_ptr<const std::vector<Row>> rows; uint64_t expiry; uint32_t options; };
    static uint64_t id(const Row& row) {
        if constexpr(requires{row.signal;})return row.signal;
        else return row.train;
    }
    uint32_t publish(uint64_t owner,std::vector<Row> next,uint64_t expiry,uint32_t options=0) {
        if(!owner)return NIMBY_INVALID_ARGUMENT;
        const auto& current=*state_;
        const auto previous=current.batches.find(owner);
        if(previous==current.batches.end()&&next.empty()&&!options)return NIMBY_OK;
        // The common 50 Hz heartbeat changes only its owner's deadline. Do not
        // copy row batches or rebuild indexes for identical instructions.
        if(previous!=current.batches.end()&&previous->second.options==options&&
           std::equal(next.begin(),next.end(),previous->second.rows->begin(),previous->second.rows->end(),same)) {
            auto updated=std::make_shared<State>(current);updated->batches.at(owner).expiry=expiry;
            state_=std::move(updated);return NIMBY_OK;
        }
        for(const auto& row:next){const auto existing=current.owners->find(id(row));
            if(existing!=current.owners->end()&&existing->second!=owner)return NIMBY_RESOURCE_LIMIT;}
        const size_t count=current.rows->size()+next.size()-(previous==current.batches.end()?0:previous->second.rows->size());
        constexpr size_t maximum=[] {if constexpr(requires(Row row){row.signal;})return 32768;else return 8192;}();
        if(count>maximum||(!current.batches.contains(owner)&&current.batches.size()>=64&&(!next.empty()||options)))
            return NIMBY_RESOURCE_LIMIT;
        // Build replacement indexes before committing, including allocations.
        auto updated=std::make_shared<State>(current);
        if(next.empty()&&!options)updated->batches.erase(owner);
        else updated->batches[owner]={std::make_shared<const std::vector<Row>>(std::move(next)),expiry,options};
        auto rows=std::make_shared<std::vector<Row>>();
        auto owners=std::make_shared<std::map<uint64_t,uint64_t>>();
        rows->reserve(count);
        for(const auto& [token,batch]:updated->batches)for(const auto& row:*batch.rows){
            rows->push_back(row);owners->emplace(id(row),token);
        }
        std::sort(rows->begin(),rows->end(),[](const auto& a,const auto& b){return id(a)<id(b);});
        updated->rows=std::move(rows);updated->owners=std::move(owners);state_=std::move(updated);
        return NIMBY_OK;
    }
    // Snapshot copies are O(1), immutable until publish. A bridge may prepare
    // a complete replacement outside its native-state lock and commit only
    // when sameVersion still matches. No failed preparation renews a lease.
    bool sameVersion(const DrivingPublishers& other)const noexcept{return state_==other.state_;}
    bool sameOwnerVersion(const DrivingPublishers& other,uint64_t owner)const noexcept {
        const auto a=state_->batches.find(owner),b=other.state_->batches.find(owner);
        if(a==state_->batches.end()||b==other.state_->batches.end())return a==state_->batches.end()&&b==other.state_->batches.end();
        return a->second.rows==b->second.rows&&a->second.expiry==b->second.expiry&&a->second.options==b->second.options;
    }
    void swap(DrivingPublishers& other)noexcept{state_.swap(other.state_);}
    std::vector<Row>& rows(){
        if(!state_.unique())state_=std::make_shared<State>(*state_);
        if(!state_->rows.unique())state_->rows=std::make_shared<std::vector<Row>>(*state_->rows);
        return *state_->rows;
    }
    const std::vector<Row>& rows()const{return *state_->rows;}
    bool empty()const{return state_->batches.empty();}
    bool fresh(uint64_t id,uint64_t now)const {
        const auto owner=state_->owners->find(id);
        return owner!=state_->owners->end()&&now<state_->batches.at(owner->second).expiry;
    }
    bool contains(uint64_t id)const{return state_->owners->contains(id);}
    std::span<const Row> ownedRows(uint64_t owner)const {
        const auto found=state_->batches.find(owner);
        return found==state_->batches.end()?std::span<const Row>{}:std::span<const Row>{*found->second.rows};
    }
    uint64_t owner(uint64_t id)const {
        const auto found=state_->owners->find(id);return found==state_->owners->end()?0:found->second;
    }
    uint32_t options(uint64_t id,uint64_t now)const {
        const auto owner=state_->owners->find(id);
        if(owner==state_->owners->end())return 0;
        const auto& batch=state_->batches.at(owner->second);
        return now<batch.expiry?batch.options:0;
    }
    uint32_t soleOptions(uint64_t now)const {
        if(state_->batches.size()!=1)return 0;
        const auto& batch=state_->batches.begin()->second;
        return now<batch.expiry?batch.options:0;
    }
    void clear()noexcept{state_=emptyState();}
private:
    static bool same(const Row& a,const Row& b){
        if constexpr(requires{a.signal;})return a.signal==b.signal&&a.speed_mps==b.speed_mps&&
            a.reopened_speed_mps==b.reopened_speed_mps&&a.signals_ahead==b.signals_ahead&&a.flags==b.flags;
        else return a.train==b.train&&a.exit_signal==b.exit_signal&&a.revision==b.revision&&
            a.speed_mps==b.speed_mps&&a.mode==b.mode&&a.flags==b.flags;
    }
    struct State {
        std::map<uint64_t,Batch> batches;
        std::shared_ptr<std::vector<Row>> rows=std::make_shared<std::vector<Row>>();
        std::shared_ptr<std::map<uint64_t,uint64_t>> owners=std::make_shared<std::map<uint64_t,uint64_t>>();
    };
    static const std::shared_ptr<State>& emptyState(){static const auto value=std::make_shared<State>();return value;}
    std::shared_ptr<State> state_=emptyState();
};
}
