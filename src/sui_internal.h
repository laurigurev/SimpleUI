#pragma once

#include "sui_backend.h"
#include "sui_types.h"
#include "sui_utils.h"

struct sui_uvmap;
struct sui_glyph;
struct sui_color;
struct sui_vertex;
struct sui_viewport;
struct sui_rect;
struct sui_widget;
struct sui_io;
struct sui_layout;
struct sui_style;
struct sui_profile;

enum sui_layout_type { SUI_LAYOUT_TYPE_ROW, SUI_LAYOUT_TYPE_COLUMN };

i64 sui_time_begin();
i64 sui_time_end(i64 begin);

struct sui_uvmap {
        f32 u0;
        f32 u1;
        f32 v0;
        f32 v1;
};

struct sui_glyph {
        i16 id;
        i16 x;
        i16 y;
        i16 width;
        i16 height;
        i16 xoffset;
        i16 yoffset;
        i16 xadvance;
};

struct sui_uvmap sui_glyph_get_uv(char c, i32 w, i32 h);
i32              sui_glyphs_width(char* str);

struct sui_color {
        u8 r;
        u8 g;
        u8 b;
        u8 a;
};

struct sui_vertex {
        f32              x;
        f32              y;
        f32              u;
        f32              v;
        struct sui_color color;
};

struct sui_viewport {
        i16 w;
        i16 h;
};

struct sui_rect {
        i32 x;
        i32 y;
        i32 w;
        i32 h;
};

struct sui_widget {
        struct sui_rect  rect;
        struct sui_rect  bbox;
        struct sui_color color0;
        struct sui_color color1;
};

struct sui_widget* sui_widget_create(struct sui_arena* arena, struct sui_ht* ht, char* name);
void               sui_widget_set(struct sui_widget* widget, i32 x, i32 y, i32 w, i32 h, struct sui_color color);
void               sui_button_set(struct sui_widget* widget, char* name, i32 x, i32 y);
void               sui_checkbox_set(struct sui_widget* widget, i32 x, i32 y);
void               sui_slider_set(struct sui_widget* widget, f32 value, i32 x, i32 y, i32 w);
void               sui_text_set(struct sui_widget* widget, char* text, i32 x, i32 y);
void               sui_widget_to_vertices(struct sui_widget* widget, i32* n, struct sui_vertex* vertices);
i32                sui_glyph_to_vertices(char c, i32 x, i32 y, struct sui_color color, i32* n, struct sui_vertex* vertices);
void               sui_button_to_vertices(struct sui_widget* widget, char* str, i32* n, struct sui_vertex* vertices);
void               sui_checkbox_to_vertices(struct sui_widget* widget, i32 value, i32* n, struct sui_vertex* vertices);
void               sui_slider_to_vertices(struct sui_widget* widget, f32 value, i32* n, struct sui_vertex* vertices);

struct sui_io {
        i32 mx;
        i32 my;
        i32 dmx;
        i32 dmy;

        u8 ldown;
        u8 lup;
        u8 rdown;
        u8 rup;
};

i32 sui_overlap(struct sui_io io, struct sui_rect bbox);

struct sui_layout {
        enum sui_layout_type type;
        i32                  acc;
        i32                  x;
        i32                  y;
        i32                  w;
        i32                  h;
};

void sui_handle_layout(struct sui_layout* layout, i32 w, i32 h);

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
        struct sui_color slider_color;
        struct sui_color slider_hover_color;
        struct sui_color checkbox_color;
        struct sui_color checkbox_bg_color;
        i32              margin;
        // i32 max_button_width;
        // i32 min_button_width;
        // i32 max_label_width;
        // i32 min_label_width;
        i32 slider_width;
        i32 max_text_width;
        i32 max_text_rows;
        i32 min_text_width;
        i32 min_text_rows;
        // i32 max_column_width;
        // i32 max_window_width;
        // i32 max_window_height;
        // i32 min_window_width;
        // i32 min_window_height;
        // f32 scale;
};

struct sui_profile {
        i32 frame_count;
        
        i32 button_count;
        i64 button_delta;
        i64 button_total_time;
        i64 button_avg_time;
        
        i32 checkbox_count;
        i64 checkbox_delta;
        i64 checkbox_total_time;
        i64 checkbox_avg_time;
        
        i32 slider_count;
        i64 slider_delta;
        i64 slider_total_time;
        i64 slider_avg_time;
        
        i32 label_count;
        i64 label_delta;
        i64 label_total_time;
        i64 label_avg_time;
        
        i32 text_count;
        i64 text_delta;
        i64 text_total_time;
        i64 text_avg_time;
        
        u64 memory_allocated;
        u64 memory_used;
        u64 memory_pushed;
        
        i32 hash_table_item_count;
        i32 hash_table_misses; // TODO
        
        i32 vertex_count;
        i32 draw_calls;
};
