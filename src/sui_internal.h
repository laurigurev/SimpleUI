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
// struct sui_recti;
// struct sui_rectf;

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
        struct sui_color color;
};

struct sui_widget* sui_widget_create(struct sui_arena* arena, struct sui_ht* ht, char* name, i32 x, i32 y, i32 w, i32 h);
void               sui_widget_to_vertices(struct sui_widget* widget, i32* n, struct sui_vertex* vertices);
