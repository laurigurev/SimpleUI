#pragma once

#include "sui_types.h"

struct sui_glyph;
struct sui_color;
struct sui_vertex;
struct sui_viewport;
// struct sui_recti;
// struct sui_rectf;

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
