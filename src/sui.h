#pragma once

#include "sui_backend.h"
#include "sui_internal.h"
#include "sui_types.h"
#include "sui_utils.h"

struct sui_context;

struct sui_context {
        struct sui_io       io;
        struct sui_widget*  hot_widget;
        struct sui_widget*  active_widget;
        struct sui_widget*  current_window;
        struct sui_row      row;
        struct sui_arena    arena;
        struct sui_ht       ht;
        struct sui_viewport viewport;
        struct sui_backend  backend;
        i32                 vertices_len;
        struct sui_vertex*  vertices;
};

void sui_init(struct sui_context* sui, ID3D11Device* device, i32 w, i32 h);
void sui_inputs(struct sui_context* sui, i32 mx, i32 my, u8 ldown, u8 lup, u8 rdown, u8 rup);
void sui_begin(struct sui_context* sui, char* name, i32 x, i32 y);
void sui_end(struct sui_context* sui);
void sui_row(struct sui_context* sui);
i32  sui_button(struct sui_context* sui, char* name);
void sui_text(struct sui_context* sui, char* text, ...);
void sui_checkbox(struct sui_context* sui, char* name, i32* value);
void sui_slider(struct sui_context* sui, char* name, f32 value);
void sui_render(struct sui_context* sui);

/*
struct sui_style {
        struct sui_color window_color;
        struct sui_color button_bg_color;
        struct sui_color button_hover_bg_color;
        struct sui_color button_color;
        struct sui_color button_hover_color;
        struct sui_color label_bg_color;
        struct sui_color label_hover_bg_color;
        struct sui_color label_color;
        struct sui_color label_hover_color;
        struct sui_color text_bg_color;
        struct sui_color text_color;
        struct sui_color slider_hover_color;
        struct sui_color slider_color;
        struct sui_color checkbox_hover_color;
        struct sui_color checkbox_color;
        i32 margin;
        i32 max_button_width;
        i32 min_button_width;
        i32 max_label_width;
        i32 min_label_width;
        i32 slider_width;
        i32 max_text_width;
        i32 max_text_rows;
        i32 min_text_width;
        i32 min_text_rows;
        i32 max_column_width;
        i32 max_window_width;
        i32 max_window_height;
        i32 min_window_width;
        i32 min_window_height;
        f32 scale;
};

struct sui_column {
        i32 row_count;
        i32 x;
        i32 y;
        i32 width;
        i32 height;
};

struct sui_context {
        ...
        struct sui_row row;
        struct sui_column column;
        i32 rows_on;
        struct sui_style style;
};

void sui_end(struct sui_context* sui);
void sui_row(struct sui_context* sui);
void sui_column(struct sui_context* sui, i32 rows, i32 width);
void sui_button(struct sui_context* sui, char* name);
void sui_checkbox(struct sui_context* sui, char* name);
void sui_label(struct sui_context* sui, char* name);
void sui_text(struct sui_context* sui, char* txt, ...);
void sui_empty(struct sui_context* sui, i32 space);
 */
