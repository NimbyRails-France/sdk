#pragma once
#include <array>
#include <cstring>
#include <span>
#include <stdexcept>
#include <string_view>
#include <type_traits>
#include <cstdint>

namespace nimby {
// Value-only messages within one Windows x64 build. No pointers or owning STL
// containers may cross this bridge. Change the command name/version on schema changes.
struct ModCommand {
    const char* name;
    std::uint32_t requestSize, responseSize;
    void (*call)(const void*, void*);
};
template<class Request, class Response, auto Handler>
constexpr ModCommand command(const char* name) {
    static_assert(std::is_trivially_copyable_v<Request> && std::is_trivially_copyable_v<Response>);
    static_assert(sizeof(Request) <= 1024*1024 && sizeof(Response) <= 1024*1024);
    return {name, sizeof(Request), sizeof(Response), [](const void* input, void* output) {
        Request request{};
        std::memcpy(&request, input, sizeof request);
        const Response response = Handler(request);
        std::memcpy(output, &response, sizeof response);
    }};
}
template<class T, std::size_t Capacity> struct FixedList {
    std::array<T, Capacity> items{};
    std::uint32_t count = 0;
    void push_back(const T& value) {
        if (count >= Capacity) throw std::invalid_argument("Mod list capacity exceeded");
        items[count++] = value;
    }
    std::span<const T> values() const {
        if (count > Capacity) throw std::invalid_argument("Invalid mod list count");
        return {items.data(), count};
    }
};
template<std::size_t Capacity> struct FixedText {
    std::array<char, Capacity> data{};
    void assign(std::string_view text) {
        if (text.size() >= Capacity) throw std::invalid_argument("Mod text capacity exceeded");
        data.fill(0); std::memcpy(data.data(), text.data(), text.size());
    }
    std::string_view view() const {
        const auto end = std::char_traits<char>::find(data.data(), Capacity, '\0');
        if (!end) throw std::invalid_argument("Unterminated mod text");
        return {data.data(), static_cast<std::size_t>(end-data.data())};
    }
};
}
