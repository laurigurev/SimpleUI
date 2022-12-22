#include "sui_internal.h"
#define WIN32_MEAN_AND_LEAN
#include "sui_cdata.h"
#include "sui_defines.h"
#include <windows.h>

i64 sui_time_begin()
{
        LARGE_INTEGER frequency;
        QueryPerformanceFrequency(&frequency);
        LARGE_INTEGER t;
        QueryPerformanceCounter(&t);
        return t.QuadPart;
}

i64 sui_time_end(i64 begin)
{
        LARGE_INTEGER frequency;
        QueryPerformanceFrequency(&frequency);
        LARGE_INTEGER time;
        QueryPerformanceCounter(&time);
        i64 res = time.QuadPart;
        res -= begin;
        res *= 1000000;
        res /= frequency.QuadPart;
        return res;
}

struct sui_uvmap sui_glyph_get_uv(char c, i32 w, i32 h)
{
        struct sui_glyph glyph = cdata[c - 32];
        // TODO: change format
        struct sui_uvmap uv;
        uv.u0 = glyph.x / (f32)w;
        uv.u1 = glyph.x / (f32)w + glyph.width / (f32)w;
        uv.v0 = glyph.y / (f32)h;
        uv.v1 = glyph.y / (f32)h + glyph.height / (f32)h;
        return uv;
}

struct sui_widget* sui_widget_create(struct sui_arena* arena, struct sui_ht* ht, char* name, i32 x, i32 y, i32 w, i32 h)
{
        sui_assert(arena);
        sui_assert(ht);
        struct sui_widget* widget = sui_arena_push(arena, sizeof(struct sui_widget));
        sui_assert(widget);
        sui_assert(sui_ht_insert(ht, name, widget));
        widget->rect = (struct sui_rect){x, y, w, h};
        widget->bbox = (struct sui_rect){x, y, w, h};
        widget->color = (struct sui_color){200, 200, 200, 255};
        return widget;
}

void sui_widget_to_vertices(struct sui_widget* widget, i32* n, struct sui_vertex* vertices)
{
        sui_assert(widget);
        struct sui_rect rect = widget->rect;

        f32 x0 = (f32)rect.x;
        f32 x1 = (f32)rect.x + (f32)rect.w;
        f32 y0 = (f32)rect.y;
        f32 y1 = (f32)rect.y + (f32)rect.h;

        // TODO: firgure a good way to bring font atlas dimensions to this function
        struct sui_uvmap uv = sui_glyph_get_uv(127, 128, 64);

        *vertices++ = (struct sui_vertex){x0, y0, uv.u0, uv.v0, widget->color};
        *vertices++ = (struct sui_vertex){x1, y1, uv.u1, uv.v1, widget->color};
        *vertices++ = (struct sui_vertex){x0, y1, uv.u0, uv.v1, widget->color};
        *vertices++ = (struct sui_vertex){x1, y0, uv.u1, uv.v0, widget->color};
        *n += 4;
}
