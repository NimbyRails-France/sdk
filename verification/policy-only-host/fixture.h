#pragma once
#include <cstdint>

// Shared only by the isolated adapter verification executable and its fake
// SDK facade. It contains counters, never game pointers or callbacks.
namespace policy_host_fixture {
enum Mode : std::uint32_t { Normal=0, FailPreferenceRead=1, FailRegistration=2, MissingActionWait=4 };
struct Stats {
    std::uint64_t sequence{}, optionAdds{}, optionReads{}, optionRemoves{};
    std::uint64_t limitAdds{}, limitUpdates{}, limitRemoves{};
    std::uint64_t editorAdds{}, declarationBytes{}, resolvedMessages{};
    std::uint64_t readSequence{}, registerSequence{}, lastKnownRevision{}, revision{};
    std::uint64_t optionOwner{}, limitOwner{}, openProcessCalls{}, captureCalls{};
    std::uint32_t savedMeters{}, registeredMeters{}, publishedMeters{}, busyUpdates{};
};
using Configure=std::uint32_t(*)(std::uint32_t,std::uint32_t);
using Snapshot=std::uint32_t(*)(Stats*);
using Change=std::uint32_t(*)(std::uint32_t,std::uint32_t);
using Busy=std::uint32_t(*)(std::uint32_t);
}
