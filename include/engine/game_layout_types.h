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
    size_t track_metric_offset = 0; // Native distance per unit track fraction; 0 = unqualified.
    uint64_t ui_layout_button=0,ui_interactive_button=0,ui_button_slot=0;
    uint64_t ui_layout_group=0,ui_interactive_group=0,ui_group_slot=0;
    uint64_t ui_layout_group_end=0,ui_interactive_group_end=0,ui_group_end_slot=0;
    size_t ui_height_enabled=0,ui_height_value=0;
    uint64_t ui_layout_box=0,ui_interactive_box=0,ui_box_slot=0;
    uint64_t ui_layout_box_end=0,ui_interactive_box_end=0,ui_box_end_slot=0;
    size_t ui_width_enabled=0,ui_width_value=0;
    size_t ui_flow_enabled=0,ui_flow_value=0,ui_align_enabled=0,ui_align_value=0;
    uint64_t ui_layout_calculate=0;
    size_t ui_interactive_layout=0,ui_layout_items=0,ui_layout_scale=0,ui_interactive_width=0;
    size_t ui_layout_item_width=0;
    uint64_t ui_layout_number=0,ui_interactive_number=0,ui_number_slot=0;
    size_t ui_interactive_style=0,ui_style_font=0,ui_font_height=0,ui_font_width=0;
    uint64_t ui_layout_label=0,ui_interactive_label=0,ui_label_slot=0;
    uint64_t ui_layout_wrapped_label=0,ui_interactive_wrapped_label=0,ui_wrapped_label_slot=0;
    uint64_t ui_layout_space=0,ui_interactive_space=0,ui_space_slot=0;
    size_t ui_margin_enabled=0,ui_margin_value=0;
    uint64_t ui_next_rect=0,ui_widget=0,ui_fill_rect=0,ui_reset_declaration=0,ui_edit_string=0;
    size_t ui_interactive_context=0,ui_context_window=0,ui_window_commands=0;
    size_t ui_context_border_color=0,ui_context_scroll_size=0;
    size_t ui_owner=0,ui_owner_text_rect=0,ui_owner_text_counter=0;
};
}
