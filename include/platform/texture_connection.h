#pragma once
#include <engine/live_state.h>
#include <nimby/texture_preview.hpp>
#include <nimby/signal_textures.hpp>
#include <memory>
#include <span>
#include <runtime/texture_table.h>

namespace nimby::platform {
// A serialized texture command session. The backend owns process resources,
// publication barriers and interrupted-request recovery. Destruction releases
// resources but never cancels a command already published to the game.
// Linux has no qualified transport yet; it must implement this contract before
// enabling the shared texture API in its build.
class TextureConnection {
    struct Impl;
    std::unique_ptr<Impl> impl_;
public:
    TextureConnection();
    ~TextureConnection();
    TextureConnection(const TextureConnection&)=delete;
    TextureConnection& operator=(const TextureConnection&)=delete;
    uint32_t open(uint32_t pid);
    const engine::LiveState& live() const;
    static bool read(void* context,uint64_t address,void* out,size_t size);
    uint32_t submit(uint32_t operation,uint64_t signal,uint64_t hash=0,uint64_t expiry=0,
                    uint32_t index=0,uint32_t alternate=0,uint32_t halfPeriod=0);
    uint32_t publish(uint64_t owner,std::span<const texture_bridge::Command> updates);
    uint32_t clear(uint64_t owner,std::span<const uint64_t> signals);
    uint32_t release(uint64_t owner);
    uint32_t statuses(std::span<const uint64_t> signals,NimbySignalTextureOverrideStatus* out);
    void previewStatus(NimbyTexturePreviewStatus& out) const;
    void overrideStatus(NimbySignalTextureOverrideStatus& out) const;
    // Must share the bridge's monotonic time origin (expiry is an IPC value).
    static uint64_t now();
    static uint32_t currentPid();
    static uint32_t releaseInGameOwner(uint64_t owner);
    static uint32_t hostTarget();
    static uint32_t forward(uint32_t operation,uint32_t pid,const void* data,size_t bytes,uint64_t count,
                            std::span<uint8_t> output,uint32_t& written);
};
}
