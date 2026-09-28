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
    .ui_checkbox_slot=0xf0, .editor_signal_capture=0x38,
    .track_metric_offset=0x88,
    .ui_layout_button=0x55c9c0,.ui_interactive_button=0x55ef00,.ui_button_slot=0x90,
    .ui_layout_group=0x55c950,.ui_interactive_group=0x55d620,.ui_group_slot=0x30,
    .ui_layout_group_end=0x55c980,.ui_interactive_group_end=0x55dc40,.ui_group_end_slot=0x38,
    .ui_height_enabled=0x30,.ui_height_value=0x34,
    .ui_layout_box=0x55c6f0,.ui_interactive_box=0x55d2c0,.ui_box_slot=0x08,
    .ui_layout_box_end=0x55c770,.ui_interactive_box_end=0x55d430,.ui_box_end_slot=0x18,
    .ui_width_enabled=0x28,.ui_width_value=0x2c,
    .ui_flow_enabled=0x20,.ui_flow_value=0x24,.ui_align_enabled=0x18,.ui_align_value=0x1c,
    .ui_layout_calculate=0x55c690,
    .ui_interactive_layout=0xb0,.ui_layout_items=0xc0,.ui_layout_scale=0xb0,.ui_interactive_width=0x1f0,
    .ui_layout_item_width=0x1c,
    .ui_layout_number=0x55ce50,.ui_interactive_number=0x562a10,.ui_number_slot=0x138,
    .ui_interactive_style=0x1e0,.ui_style_font=0xf0,.ui_font_height=0x08,.ui_font_width=0x10,
    .ui_layout_label=0x55ca70,.ui_interactive_label=0x55f570,.ui_label_slot=0xa8,
    .ui_layout_wrapped_label=0x55cb30,.ui_interactive_wrapped_label=0x55f640,.ui_wrapped_label_slot=0xb8,
    .ui_layout_space=0x55cc10,.ui_interactive_space=0x55f930,.ui_space_slot=0xd0,
    .ui_margin_enabled=0x38,.ui_margin_value=0x3c,
    .ui_next_rect=0x55d140,.ui_widget=0x50abd0,.ui_fill_rect=0x4fc370,
    .ui_reset_declaration=0x55ba40,.ui_edit_string=0x5137d0,
    .ui_interactive_context=0x1d8,.ui_context_window=0x4778,.ui_window_commands=0x68,
    .ui_context_border_color=0x2420,.ui_context_scroll_size=0x248c,
    .ui_owner=0x08,.ui_owner_text_rect=0x64cc,.ui_owner_text_counter=0x64e0
};
}
