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

i32 sui_glyphs_width(char* str)
{
        i32 w = 0;
        while (*str) {
                w += cdata[*str - 32].xadvance;
                str++;
        }
        return w;
}

struct sui_widget* sui_widget_create(struct sui_arena* arena, struct sui_ht* ht, char* name)
{
        sui_assert(arena);
        sui_assert(ht);
        struct sui_widget* widget = sui_arena_push(arena, sizeof(struct sui_widget));
        sui_assert(widget);
        sui_assert(sui_ht_insert(ht, name, widget));
        widget->rect = (struct sui_rect){0, 0, 0, 0};
        widget->bbox = (struct sui_rect){0, 0, 0, 0};
        widget->color0 = (struct sui_color){0, 0, 0, 0};
        widget->color1 = (struct sui_color){0, 0, 0, 0};
        return widget;
}

void sui_widget_set(struct sui_widget* widget, i32 x, i32 y, i32 w, i32 h, struct sui_color color)
{
        sui_assert(widget);
        widget->rect = (struct sui_rect){x, y, w, h};
        widget->bbox = (struct sui_rect){x, y, w, h};
        widget->color0 = color;
        widget->color1 = (struct sui_color){0, 0, 0, 0};
}

void sui_button_set(struct sui_widget* widget, char* name, i32 x, i32 y)
{
        sui_assert(widget);
        i32 w = widget->rect.w;
        if (!w) w = sui_glyphs_width(name);
        widget->rect = (struct sui_rect){x, y, w, 16};
        widget->bbox = (struct sui_rect){x, y, w, 16};
        widget->color0 = (struct sui_color){0, 0, 0, 255};
        widget->color1 = (struct sui_color){255, 255, 255, 255};
}

void sui_checkbox_set(struct sui_widget* widget, i32 x, i32 y)
{
        sui_assert(widget);
        widget->rect = (struct sui_rect){x, y, 16, 16};
        widget->bbox = (struct sui_rect){x, y, 16, 16};
        // widget->color0 = (struct sui_color){0, 0, 0, 255};
        // widget->color1 = (struct sui_color){255, 255, 255, 255};
}

void sui_slider_set(struct sui_widget* widget, f32 value, i32 x, i32 y, i32 w)
{
        sui_assert(widget);
        // TODO: control width and height
        i32       bbx = (f32)w * value - 8.0f;
        if (value <= 0.0f) bbx = 0;
        if (value >= 1.0f) bbx = w - 16;
        widget->rect = (struct sui_rect){x, y, w, 16};
        widget->bbox = (struct sui_rect){x + bbx, y, 16, 16};
        widget->color0 = (struct sui_color){0, 0, 0, 255};
        widget->color1 = (struct sui_color){255, 255, 255, 255};
}

void sui_widget_to_vertices(struct sui_widget* widget, i32* n, struct sui_vertex* vertices)
{
        sui_assert(widget);
        sui_assert(vertices);
        struct sui_rect rect = widget->rect;

        f32 x0 = (f32)rect.x;
        f32 x1 = (f32)rect.x + (f32)rect.w;
        f32 y0 = (f32)rect.y;
        f32 y1 = (f32)rect.y + (f32)rect.h;

        // TODO: figure a good way to bring font atlas dimensions to this function
        struct sui_uvmap uv = sui_glyph_get_uv(127, 128, 64);

        *vertices++ = (struct sui_vertex){x0, y0, uv.u0, uv.v0, widget->color0};
        *vertices++ = (struct sui_vertex){x1, y1, uv.u1, uv.v1, widget->color0};
        *vertices++ = (struct sui_vertex){x0, y1, uv.u0, uv.v1, widget->color0};
        *vertices++ = (struct sui_vertex){x1, y0, uv.u1, uv.v0, widget->color0};
        *n += 4;
}

i32 sui_glyph_to_vertices(char c, i32 x, i32 y, struct sui_color color, i32* n, struct sui_vertex* vertices)
{
        struct sui_glyph glyph = cdata[c - 32];
        f32              x0 = (f32)x + glyph.xoffset;
        f32              y0 = (f32)y + glyph.yoffset;
        f32              x1 = x0 + glyph.width;
        f32              y1 = y0 + glyph.height;
        // TODO: figure a good way to bring font atlas dimensions to this function
        struct sui_uvmap uv = sui_glyph_get_uv(c, 128, 64);
        // TODO: bring color from widget
        *vertices++ = (struct sui_vertex){x0, y0, uv.u0, uv.v0, color};
        *vertices++ = (struct sui_vertex){x1, y1, uv.u1, uv.v1, color};
        *vertices++ = (struct sui_vertex){x0, y1, uv.u0, uv.v1, color};
        *vertices++ = (struct sui_vertex){x1, y0, uv.u1, uv.v0, color};
        *n += 4;
        return glyph.xadvance;
}

void sui_button_to_vertices(struct sui_widget* widget, char* str, i32* n, struct sui_vertex* vertices)
{
        sui_assert(widget);
        sui_assert(vertices);
        struct sui_rect rect = widget->rect;

        f32 x0 = (f32)rect.x;
        f32 x1 = (f32)rect.x + (f32)rect.w;
        f32 y0 = (f32)rect.y;
        f32 y1 = (f32)rect.y + (f32)rect.h;

        i32 xadvance = 0;
        while (*str) {
                xadvance += sui_glyph_to_vertices(*str, rect.x + xadvance, rect.y, widget->color1, n, vertices);
                vertices += 4;
                str++;
        }

        // TODO: figure a good way to bring font atlas dimensions to this function
        struct sui_uvmap uv = sui_glyph_get_uv(127, 128, 64);

        *vertices++ = (struct sui_vertex){x0, y0, uv.u0, uv.v0, widget->color0};
        *vertices++ = (struct sui_vertex){x1, y1, uv.u1, uv.v1, widget->color0};
        *vertices++ = (struct sui_vertex){x0, y1, uv.u0, uv.v1, widget->color0};
        *vertices++ = (struct sui_vertex){x1, y0, uv.u1, uv.v0, widget->color0};
        *n += 4;
}

void sui_checkbox_to_vertices(struct sui_widget* widget, i32 value, i32* n, struct sui_vertex* vertices)
{
        sui_assert(widget);
        sui_assert(vertices);
        struct sui_rect rect = widget->rect;

        f32 x0 = (f32)rect.x;
        f32 x1 = (f32)rect.x + (f32)rect.w;
        f32 y0 = (f32)rect.y;
        f32 y1 = (f32)rect.y + (f32)rect.h;

        // TODO: figure a good way to bring font atlas dimensions to this function
        struct sui_uvmap uv = sui_glyph_get_uv(127, 128, 64);

        f32 pad = 0.0f;
        if (value) {
                pad = 4.0f;
                *vertices++ = (struct sui_vertex){x0 + pad, y0 + pad, uv.u0, uv.v0, widget->color0};
                *vertices++ = (struct sui_vertex){x1 - pad, y1 - pad, uv.u1, uv.v1, widget->color0};
                *vertices++ = (struct sui_vertex){x0 + pad, y1 - pad, uv.u0, uv.v1, widget->color0};
                *vertices++ = (struct sui_vertex){x1 - pad, y0 + pad, uv.u1, uv.v0, widget->color0};
                *n += 4;
        }

        pad = 2.0f;
        *vertices++ = (struct sui_vertex){x0 + pad, y0 + pad, uv.u0, uv.v0, widget->color1};
        *vertices++ = (struct sui_vertex){x1 - pad, y1 - pad, uv.u1, uv.v1, widget->color1};
        *vertices++ = (struct sui_vertex){x0 + pad, y1 - pad, uv.u0, uv.v1, widget->color1};
        *vertices++ = (struct sui_vertex){x1 - pad, y0 + pad, uv.u1, uv.v0, widget->color1};
        *n += 4;

        *vertices++ = (struct sui_vertex){x0, y0, uv.u0, uv.v0, widget->color0};
        *vertices++ = (struct sui_vertex){x1, y1, uv.u1, uv.v1, widget->color0};
        *vertices++ = (struct sui_vertex){x0, y1, uv.u0, uv.v1, widget->color0};
        *vertices++ = (struct sui_vertex){x1, y0, uv.u1, uv.v0, widget->color0};
        *n += 4;
}

void sui_slider_to_vertices(struct sui_widget* widget, f32 value, i32* n, struct sui_vertex* vertices)
{
        sui_assert(widget);
        sui_assert(vertices);

        // control
        f32 x0 = (f32)widget->bbox.x;
        f32 x1 = (f32)widget->bbox.x + (f32)widget->bbox.w;
        f32 y0 = (f32)widget->bbox.y;       
        f32 y1 = (f32)widget->bbox.y + widget->bbox.h;

        // TODO: figure a good way to bring font atlas dimensions to this function
        struct sui_uvmap uv = sui_glyph_get_uv(127, 128, 64);

        *vertices++ = (struct sui_vertex){x0, y0, uv.u0, uv.v0, widget->color0};
        *vertices++ = (struct sui_vertex){x1, y1, uv.u1, uv.v1, widget->color0};
        *vertices++ = (struct sui_vertex){x0, y1, uv.u0, uv.v1, widget->color0};
        *vertices++ = (struct sui_vertex){x1, y0, uv.u1, uv.v0, widget->color0};
        *n += 4;

        // slider
        x0 = (f32)widget->rect.x;
        x1 = (f32)widget->rect.x + (f32)widget->rect.w;
        y0 = (f32)widget->rect.y + 7.0f;
        y1 = (f32)widget->rect.y + 9.0f;

        *vertices++ = (struct sui_vertex){x0, y0, uv.u0, uv.v0, widget->color0};
        *vertices++ = (struct sui_vertex){x1, y1, uv.u1, uv.v1, widget->color0};
        *vertices++ = (struct sui_vertex){x0, y1, uv.u0, uv.v1, widget->color0};
        *vertices++ = (struct sui_vertex){x1, y0, uv.u1, uv.v0, widget->color0};
        *n += 4;
}

i32 sui_overlap(struct sui_io io, struct sui_rect bbox)
{
        i32 x0 = bbox.x;
        i32 x1 = bbox.x + bbox.w;
        i32 y0 = bbox.y;
        i32 y1 = bbox.y + bbox.h;
        if (x0 < io.mx && io.mx < x1 && y0 < io.my && io.my < y1) return 1;
        return 0;
}

void sui_handle_layout(struct sui_layout* layout, i32 w, i32 h)
{
        if (layout->type == SUI_LAYOUT_TYPE_ROW) {
                layout->x += w;
                layout->w += w;
                layout->h = sui_max(layout->h, h);
        }
        else if (layout->type == SUI_LAYOUT_TYPE_COLUMN) {
                layout->y += h;
                layout->w = sui_max(layout->w, w);
                layout->h += h;
        }
}
