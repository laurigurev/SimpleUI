#include "sui_internal.h"
#define WIN32_MEAN_AND_LEAN
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

struct sui_widget* sui_widget_create(struct sui_arena* arena, struct sui_ht* ht, char* name, i32 x, i32 y, i32 w, i32 h)
{
        sui_assert(arena);
        sui_assert(ht);
        struct sui_widget* widget = sui_arena_push(arena, sizeof(struct sui_widget));
        sui_assert(widget);
        sui_assert(sui_ht_insert(ht, name, widget));
        *widget = (struct sui_widget){
                (struct sui_rect) {x,   y,   w,   h  },
                (struct sui_rect) {x,   y,   w,   h  },
                (struct sui_color){200, 200, 200, 255}
        };
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

        f32 u0 = 0.0f;
        f32 u1 = 1.0f;
        f32 v0 = 0.0f;
        f32 v1 = 1.0f;

        *vertices++ = (struct sui_vertex){x0, y0, u0, v0, widget->color};
        *vertices++ = (struct sui_vertex){x1, y1, u1, v1, widget->color};
        *vertices++ = (struct sui_vertex){x0, y1, u0, v1, widget->color};
        *vertices++ = (struct sui_vertex){x1, y0, u1, v0, widget->color};
        *n += 4;
}
