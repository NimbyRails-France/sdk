#include <nimby/blocks.hpp>
#include <nimby/block_coverage.hpp>
#include <nimby/signal_network.hpp>
#include <iostream>
#include <array>
#include <stdexcept>

#define CHECK(x) do { if (!(x)) throw std::runtime_error("Failed line " + std::to_string(__LINE__) + ": " #x); } while(false)
using namespace nimby;
int main() {
    try {
        const std::array presence{TrainPresence{1,true},TrainPresence{2,false}};
        const std::array nativeRows{TrainFootprint{1,10,.2,.8}};
        CHECK(checkBlockCoverage(presence,nativeRows,true).verified);
        CHECK(!checkBlockCoverage(presence,nativeRows,false).verified); // A sample cannot prove coverage.
        CHECK(!checkBlockCoverage(presence,{},true).verified); // Present train omitted.
        const std::array unknownPresence{TrainPresence{1,std::nullopt}};
        CHECK(!checkBlockCoverage(unknownPresence,nativeRows,true).verified);
        const std::array hidden{TrainPresence{1,false}};
        CHECK(!checkBlockCoverage(hidden,nativeRows,true).verified); // Inconsistent presence/components.
        CHECK(!checkBlockCoverage({},nativeRows,true).verified); // Unknown train owner.
        CHECK(checkBlockCoverage(hidden,{},true).verified); // Empty network with known absent train.
        const std::array duplicatePresence{TrainPresence{1,true},TrainPresence{1,true}};
        CHECK(!checkBlockCoverage(duplicatePresence,nativeRows,true).verified);
        const BlockSection block[]{ {10,0.2,1}, {11,0,0.4} };
        std::vector<TrainFootprint> trains{{1,10,0.8,1},{1,11,0,0.2},{2,11,0.3,0.5}};
        BlockReader reader(trains,true);
        CHECK(reader.read(block)==BlockOccupancy::Occupied);
        const auto contents=reader.inspect(block);
        CHECK(contents.complete&&contents.occupation==BlockOccupancy::Occupied);
        CHECK(contents.trains==std::vector<std::uint64_t>({1,2}));
        const BlockSection upstream[]{ {10,0,1} }, downstream[]{ {11,0,1} };
        CHECK(reader.inspect(upstream).trains==std::vector<std::uint64_t>({1}));
        CHECK(reader.inspect(downstream).trains==std::vector<std::uint64_t>({1,2}));
        const auto incomplete=BlockReader(trains,false).inspect(block);
        CHECK(!incomplete.complete&&incomplete.trains==contents.trains&&incomplete.occupation==BlockOccupancy::Occupied);
        const auto stale=BlockReader(trains,true,false).inspect(block);
        CHECK(!stale.complete&&stale.trains.empty()&&stale.occupation==BlockOccupancy::Unknown);
        CHECK(!reader.inspect({}).complete&&reader.inspect({}).trains.empty());
        CHECK(BlockReader({},true).inspect(block).complete);
        CHECK(BlockReader({},true).inspect(block).occupation==BlockOccupancy::Clear);
        CHECK(BlockReader({},false).inspect(block).occupation==BlockOccupancy::Unknown);
        CHECK(reader.read(upstream)==BlockOccupancy::Occupied);
        CHECK(reader.read(downstream)==BlockOccupancy::Occupied);
        std::erase_if(trains,[](auto t){return t.train==1;});
        CHECK(BlockReader(trains,true).read(block)==BlockOccupancy::Occupied);
        trains={{2,11,0.4,0.6}};
        CHECK(BlockReader(trains,true).read(block)==BlockOccupancy::Occupied);
        trains[0].begin=0.40001;
        CHECK(BlockReader(trains,true).read(block)==BlockOccupancy::Clear);
        CHECK(BlockReader(trains).read(block)==BlockOccupancy::Unknown);
        CHECK(BlockReader({},false).read(block)==BlockOccupancy::Unknown);
        CHECK(BlockReader({},true).read(block)==BlockOccupancy::Clear);
        CHECK(BlockReader(trains,true).read({})==BlockOccupancy::Unknown);
        trains[0].end=2;
        CHECK(BlockReader(trains,true).read(block)==BlockOccupancy::Unknown);
        CHECK(reader.read(block)==BlockOccupancy::Occupied); // Own copy, not a borrowed cache.
        const std::array old{TrainFootprint{1,10,0.8,1}};
        CHECK(BlockReader(old,true,false).read(block)==BlockOccupancy::Unknown);

        // Live regression: R2N R079 at signal 11319.2 was misclassified as
        // entering the downstream canton by a 2.8e-17 endpoint difference.
        const BlockSection liveSection[]{ {77,0,0.16125698806003211} };
        const BlockEntry liveEntry{77,0.16125698806003211,-1};
        std::vector<TrainFootprint> waiting{{79,77,0.16125698806003208,0.23069516311159272}};
        CHECK(BlockReader(waiting,true).read(liveSection,liveEntry)==BlockOccupancy::Clear);
        CHECK(BlockReader(waiting,true).inspect(liveSection,liveEntry).trains.empty());
        CHECK(BlockReader(waiting,false).read(liveSection,liveEntry)==BlockOccupancy::Unknown);
        waiting[0].begin-=1e-10;
        CHECK(BlockReader(waiting,true).read(liveSection,liveEntry)==BlockOccupancy::Occupied);
        const BlockSection forwardSection[]{ {77,.4,.8} };
        waiting={{79,77,.3,std::nextafter(.4,1.0)}};
        CHECK(BlockReader(waiting,true).read(forwardSection,BlockEntry{77,.4,1})==BlockOccupancy::Clear);
        waiting[0].end=.400001;
        CHECK(BlockReader(waiting,true).read(forwardSection,BlockEntry{77,.4,1})==BlockOccupancy::Occupied);
        waiting={{79,77,.8,.9}}; // Tail touching exit must still keep upstream red.
        CHECK(BlockReader(waiting,true).read(forwardSection,BlockEntry{77,.4,1})==BlockOccupancy::Occupied);

        // Generic topology tests use numbers, not national signalling aspects.
        auto resolve=[](std::span<const SignalLink> links,std::optional<std::size_t> anchor) {
            return SignalNetwork(links).resolve<int>(
                [anchor](std::size_t i)->std::optional<int> { return anchor && *anchor==i ? std::optional<int>{1} : std::nullopt; },
                [](std::size_t,const int& next) { return next<0 ? next : next+1; },
                [](std::size_t,SignalLinkIssue issue) { return issue==SignalLinkIssue::UnresolvedCycle ? -2 : -1; });
        };
        const std::array chain{SignalLink{3,0},SignalLink{1,2},SignalLink{2,3}};
        CHECK(resolve(chain,0)==std::vector<int>({1,3,2}));
        CHECK(resolve(chain,std::nullopt)==std::vector<int>({-1,-1,-1}));
        const std::array cycle{SignalLink{1,2},SignalLink{2,1}};
        CHECK(resolve(cycle,std::nullopt)==std::vector<int>({-2,-2}));
        CHECK(resolve(cycle,1)==std::vector<int>({2,1}));
        struct Signal { std::uint64_t id,nextSignal; int value; };
        const auto rule=[](const Signal& signal,const std::optional<int>& next)->std::optional<int> {
            if(signal.value) return signal.value;
            if(next) return *next<0 ? *next : *next+1;
            return std::nullopt;
        };
        const std::array inputs{Signal{3,0,7},Signal{1,2,0},Signal{2,3,0}};
        const auto values=evaluateSignals(inputs,rule,-1);
        CHECK(values[0].id==3 && values[0].decision==7);
        CHECK(values[1].id==1 && values[1].decision==9 && values[2].decision==8);
        const std::array unresolved{Signal{1,2,0},Signal{2,1,0}};
        CHECK(evaluateSignals(unresolved,rule,-1)[0].decision==-1);
        const std::array missing{Signal{1,99,0}};
        CHECK(evaluateSignals(missing,rule,-1)[0].decision==-1);
        try { evaluateSignals(inputs,rule,-1,2); CHECK(false); } catch(const std::invalid_argument&) {}
        const std::array duplicates{Signal{1,0,1},Signal{1,0,2}};
        try { evaluateSignals(duplicates,rule,-1); CHECK(false); } catch(const std::invalid_argument&) {}
        try { const std::array bad{SignalLink{1,2},SignalLink{1,0}}; SignalNetwork network(bad); CHECK(false); }
        catch(const std::invalid_argument&) {}
        std::cout << "PASS block coverage, tails, multiple trains, stale data and generic downstream resolution\n";
    } catch(const std::exception& e) { std::cerr << e.what() << '\n'; return 1; }
}
