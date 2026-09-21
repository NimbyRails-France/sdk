#pragma once
#include <cstdint>
#include <optional>
#include <span>
#include <stdexcept>
#include <unordered_map>
#include <vector>

namespace nimby {
// One outgoing link on the selected route. This does not select switch positions.
struct SignalLink { std::uint64_t id, next; };
enum class SignalLinkIssue { MissingNext, UnresolvedCycle };
class SignalNetwork {
    std::vector<SignalLink> links_;
    std::unordered_map<std::uint64_t,std::size_t> index_;
public:
    explicit SignalNetwork(std::span<const SignalLink> links) {
        if (links.size()>4096) throw std::invalid_argument("Signal network limit exceeded");
        links_.assign(links.begin(),links.end());
        for (std::size_t i=0;i<links.size();++i)
            if (!links[i].id || !index_.emplace(links[i].id,i).second)
                throw std::invalid_argument("Missing or duplicate signal ID");
    }
    // local(index) -> optional<Result>: a locally decidable state/boundary, or nullopt
    // to request the downstream result. combine(index,next) and invalid(index,issue)
    // supply ALL national rules. Results keep the original input order.
    template<class Result,class Local,class Combine,class Invalid>
    std::vector<Result> resolve(Local local,Combine combine,Invalid invalid) const {
        std::vector<Result> results(links_.size());
        std::vector<unsigned char> state(links_.size(),0);
        std::vector<std::size_t> pending; pending.reserve(links_.size());
        for (std::size_t start=0;start<links_.size();++start) {
            if (state[start]==2) continue;
            pending.clear(); auto current=start;
            while (state[current]!=2) {
                if (state[current]==1) {
                    for(auto i:pending) { results[i]=invalid(i,SignalLinkIssue::UnresolvedCycle); state[i]=2; }
                    break;
                }
                if(auto value=local(current)) { results[current]=*value; state[current]=2; break; }
                const auto next=index_.find(links_[current].next);
                if (next==index_.end()) {
                    results[current]=invalid(current,SignalLinkIssue::MissingNext); state[current]=2; break;
                }
                state[current]=1; pending.push_back(current); current=next->second;
            }
            for(auto it=pending.rbegin();it!=pending.rend();++it) {
                if(state[*it]==2) continue;
                results[*it]=combine(*it,results[index_.at(links_[*it].next)]);
                state[*it]=2;
            }
        }
        return results;
    }
};

template<class Decision> struct SignalEvaluation {
    std::uint64_t id;
    Decision decision;
};

// High-level evaluation: nodes expose id/nextSignal; all other fields belong to
// the mod. rule(node, optional downstream) returns a decision, or nullopt when
// downstream information is required. No indices or traversal callbacks in mods.
// Missing links/cycles use unresolved. The input is read only and must stay stable.
template<class Nodes,class Rule,class Decision>
std::vector<SignalEvaluation<Decision>> evaluateSignals(const Nodes& nodes,Rule rule,
    const Decision& unresolved,std::size_t limit=4096) {
    if(nodes.size()>limit || nodes.size()>4096) throw std::invalid_argument("Signal evaluation limit exceeded");
    std::vector<SignalLink> links;
    links.reserve(nodes.size());
    for(const auto& node:nodes) links.push_back({node.id,node.nextSignal});
    const auto decisions=SignalNetwork(links).template resolve<Decision>(
        [&](std::size_t i) { return rule(nodes[i],std::optional<Decision>{}); },
        [&](std::size_t i,const Decision& next) {
            return rule(nodes[i],std::optional<Decision>{next}).value_or(unresolved);
        },
        [&](std::size_t,SignalLinkIssue) { return unresolved; });
    std::vector<SignalEvaluation<Decision>> result;
    result.reserve(nodes.size());
    for(std::size_t i=0;i<nodes.size();++i) result.push_back({nodes[i].id,decisions[i]});
    return result;
}
}
