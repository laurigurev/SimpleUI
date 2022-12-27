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
        struct sui_layout   layout;
        struct sui_arena    arena;
        struct sui_ht       ht;
        struct sui_viewport viewport;
        struct sui_backend  backend;
        i32                 vertices_len;
        struct sui_vertex*  vertices;
        struct sui_style    style;
};

void sui_init(struct sui_context* sui, ID3D11Device* device, i32 w, i32 h);
void sui_inputs(struct sui_context* sui, i32 mx, i32 my, u8 ldown, u8 lup, u8 rdown, u8 rup);
void sui_begin(struct sui_context* sui, char* name, i32 x, i32 y);
void sui_end(struct sui_context* sui);
void sui_row(struct sui_context* sui);
void sui_column(struct sui_context* sui);
i32  sui_button(struct sui_context* sui, char* name);
void sui_checkbox(struct sui_context* sui, char* name, i32* value);
void sui_slider(struct sui_context* sui, char* name, f32* value);
void sui_render(struct sui_context* sui);

/*
void sui_label(struct sui_context* sui, char* name);
void sui_text(struct sui_context* sui, char* txt, ...);
void sui_empty(struct sui_context* sui, i32 space);
 */
