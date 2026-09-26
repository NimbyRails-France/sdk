#pragma once
#include "engine/game_layout_types.h"

namespace nimby::platform::linux_os {
// ELF 1.19.10.5bfaea3 research profile. Data evidence does not authorize hooks.
// UI tables are Itanium address points, after their ABI headers. The editor
// capture remains unqualified, explicitly represented by zero below.
inline constexpr engine::GameLayout game119{
    .root_rva=0x10ee020, .versioning=0xa40,
    .passenger_query=0x17e8, .texture_query=0x17e0,
    .reservations=0x9d0, .reservation_stride=0x58,
    .reservation_header=0x28, .occupations=0x278,
    .rules=0xa78, .local_mod_root=0x10e6678,
    .string_size=8, .string_capacity=16, .string_inline=16,
    .string_inline_by_pointer=true, .local_path_utf8=true,
    .ui_layout_table=0x1088938, .ui_interactive_table=0x1088b00,
    .ui_layout_checkbox=0x7a1280, .ui_interactive_checkbox=0x7a2490,
    .ui_checkbox_slot=0xf8, .editor_signal_capture=0
};
}
