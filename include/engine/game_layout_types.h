#pragma once
#include <cstddef>
#include <cstdint>

namespace nimby::engine {
enum class LiveStateProfile { Windows119, Linux119 };

// Data describing the fingerprinted game binary, not the compiler running this
// SDK. Both profiles can be read by portable fixture tests on either host OS.
// Zero root_rva denotes an unsupported profile; never infer it from memory.
struct GameLayout {
    uint64_t root_rva, versioning, passenger_query, texture_query;
    uint64_t reservations, reservation_stride, reservation_header, occupations;
    uint64_t rules, local_mod_root;
    size_t string_size, string_capacity, string_inline;
    bool string_inline_by_pointer, local_path_utf8;
    uint64_t ui_layout_table, ui_interactive_table;
    uint64_t ui_layout_checkbox, ui_interactive_checkbox, ui_checkbox_slot;
    uint64_t editor_signal_capture; // 0 means this capability is unqualified.
};
}
