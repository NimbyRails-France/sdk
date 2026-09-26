#pragma once
#include "engine/game_layout_types.h"

namespace nimby::platform::windows {
// Windows 1.19 research profile. Executable SHA-256 validation precedes access.
// Evidence: docs/research/stable-resolution.md and signal-settings-persistence.md.
inline constexpr engine::GameLayout game119{
    .root_rva=0xb81998, .versioning=0xa48,
    .passenger_query=0x2208, .texture_query=0x2200,
    .reservations=0xd98, .reservation_stride=0x88,
    .reservation_header=0x50, .occupations=0x2c8,
    .rules=0xa80, .local_mod_root=0xb77d50,
    .string_size=16, .string_capacity=24, .string_inline=0,
    .string_inline_by_pointer=false, .local_path_utf8=false,
    .ui_layout_table=0xa83818, .ui_interactive_table=0xa83470,
    .ui_layout_checkbox=0x55cd20, .ui_interactive_checkbox=0x560870,
    .ui_checkbox_slot=0xf0, .editor_signal_capture=0x38
};
}
