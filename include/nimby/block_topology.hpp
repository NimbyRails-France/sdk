#pragma once
#include <nimby/blocks.hpp>
#include <nimby/client.hpp>

namespace nimby {

enum class BlockBoundaryIssue { None, MissingSignal, AmbiguousSignal, UnresolvedRoute };

// A geometric block ending at the next facing boundary signal. This does not
// assert that a route is reserved, that occupation coverage is complete, or that
// the native game permits a train to pass the starting signal.
struct SignalBlock {
    Id signal = 0;
    Id nextSignal = 0;
    std::vector<BlockSection> sections;
    std::optional<BlockEntry> entry;
    BlockBoundaryIssue issue = BlockBoundaryIssue::UnresolvedRoute;
    SignalTraceStop traceStop = SignalTraceStop::UnknownTrack;
    bool hasBoundary() const noexcept { return issue == BlockBoundaryIssue::None; }
    // Partial geometry cannot establish the absence of a train in the full block.
    BlockOccupancy occupation(const BlockReader& reader) const {
        return hasBoundary() ? reader.read(sections,entry) : BlockOccupancy::Unknown;
    }
    // Native footprints include the tail: a train can belong to two blocks.
    // Unresolved block limits cannot produce a complete block observation.
    BlockObservation observe(const BlockReader& reader) const {
        return hasBoundary()?reader.inspect(sections,entry):BlockObservation{};
    }
};

// Build once per observed topology, then reuse for every starting signal.
// The caller selects boundary signals: a balise/marker need not delimit a block.
// Facing junctions remain unresolved rather than silently choosing a branch.
class BlockTopology {
public:
    BlockTopology(std::span<const Signal> boundaries, std::span<const TrackNode> nodes,
        std::span<const TrackJunction> junctions = {})
        : topology_(boundaries, nodes, junctions, SignalDirectionConvention::Forward) {
        signals_.reserve(boundaries.size());
        for (const auto& signal : boundaries) {
            if (!signal.getId() || !std::isfinite(signal.getFraction())
                || signal.getFraction() < 0 || signal.getFraction() > 1
                || (signal.getDirection() != 1 && signal.getDirection() != -1)
                || !signals_.emplace(signal.getId(), signal).second)
                throw std::invalid_argument("Invalid or duplicate block boundary signal");
        }
    }

    SignalBlock read(Id signalId, std::size_t maxSections = 256) const {
        SignalBlock result;
        result.signal = signalId;
        const auto found = signals_.find(signalId);
        if (found == signals_.end()) {
            result.issue = BlockBoundaryIssue::MissingSignal;
            return result;
        }
        const auto& start = found->second;
        result.entry=BlockEntry{start.getTrackId(),start.getFraction(),start.getForwardDirection()};
        const auto trace = topology_.traceToNextSignal(
            Position{start.getTrackId(), start.getFraction(), start.getForwardDirection()}, signalId, maxSections);
        result.traceStop = trace.stop;
        for (const auto& section : trace.sections) {
            std::optional<double> boundaryFraction;
            Id boundaryId = 0;
            for (const auto& signal : section.orderedSignals) {
                if (signal.getId() == signalId) continue;
                if (boundaryFraction && signal.getFraction() != *boundaryFraction) break;
                if (boundaryFraction || (section.trackId == start.getTrackId()
                    && signal.getFraction() == start.getFraction())) {
                    result.issue = BlockBoundaryIssue::AmbiguousSignal;
                    return result;
                }
                boundaryFraction = signal.getFraction();
                boundaryId = signal.getId();
            }
            const double end = boundaryFraction.value_or(section.toFraction);
            if (end != section.fromFraction) {
                result.sections.push_back({section.trackId,
                    std::min(section.fromFraction, end), std::max(section.fromFraction, end)});
            }
            if (boundaryFraction) {
                // A zero-length interval alone cannot prove block clearance.
                if (result.sections.empty()) {
                    result.issue = BlockBoundaryIssue::AmbiguousSignal;
                    return result;
                }
                result.nextSignal = boundaryId;
                result.issue = BlockBoundaryIssue::None;
                return result;
            }
        }
        // Partial sections are diagnostic only. Never use them to assert Clear.
        return result;
    }

private:
    SignalTopology topology_;
    std::unordered_map<Id, Signal> signals_;
};

} // namespace nimby
