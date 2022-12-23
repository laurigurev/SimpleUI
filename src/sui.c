#include "sui.h"
#include "sui_defines.h"
#include <stdio.h>

void sui_init(struct sui_context* sui, ID3D11Device* device, i32 w, i32 h)
{
        sui_assert(sui);
        memset(&sui->io, 0, sizeof(struct sui_io));
        sui->active_widget = NULL;
        sui->hot_widget = NULL;
        sui->current_window = NULL;
        sui->current_w = 0;
        sui->current_h = 0;
        const u64 megabyte = 1024 * 1024;
        sui->arena = sui_arena_init(malloc(megabyte), megabyte);
        const u64 size = sui_hti_mem(128);
        void*     mem = malloc(size);
        sui->ht = sui_ht_init(mem, 128);
        sui->viewport = (struct sui_viewport){w, h};
        sui_backend_init(&sui->backend, device, w, h);
        sui->vertices_len = 0;
        sui->vertices = (struct sui_vertex*)malloc(2048 * sizeof(struct sui_vertex));
}

void sui_inputs(struct sui_context* sui, i32 mx, i32 my, u8 ldown, u8 lup, u8 rdown, u8 rup)
{
        sui->io.dmx = sui->io.mx - mx;
        sui->io.dmy = sui->io.my - my;
        sui->io.mx = mx;
        sui->io.my = my;

        sui->io.ldown = ldown;
        sui->io.lup = lup;
        sui->io.rdown = rdown;
        sui->io.rup = rup;
}

void sui_begin(struct sui_context* sui, char* name)
{
        sui_assert(sui);
        /* struct sui_widget* widget = sui_ht_find(&sui->ht, name);
        if (!widget) widget = sui_widget_create(&sui->arena, &sui->ht, name, x, y, w, h, (struct sui_color){255, 255, 255, 255});
        sui_widget_to_vertices(widget, &sui->vertices_len, sui->vertices + sui->vertices_len);
        sui->current_window = widget; */
}

void sui_end(struct sui_context* sui)
{
        sui->current_window = NULL;
        sui->current_w = 0;
        sui->current_h = 0;
}

void sui_row(struct sui_context* sui)
{
        sui->current_h += 16;
}

i32 sui_button(struct sui_context* sui, char* name)
{
        sui_assert(sui);
        // sui_assert(sui->current_window);

        struct sui_widget* widget = sui_ht_find(&sui->ht, name);
        if (!widget) {
                i32 x = sui->current_w;
                i32 y = sui->current_h;
                printf("button, x %i, y %i\n", x, y);
                widget = sui_button_create(&sui->arena, &sui->ht, name, x, y);
                sui->current_w += widget->rect.w;
        }

        widget->color = (struct sui_color){0, 0, 0, 255};
        if (sui_overlap(sui->io, widget->bbox)) {
                widget->color = (struct sui_color){100, 100, 100, 255};
                if (sui->io.ldown) sui->hot_widget = widget;
                if (sui->io.lup && sui->hot_widget == widget) sui->active_widget = widget;
        }

        sui_button_to_vertices(widget, name, &sui->vertices_len, sui->vertices + sui->vertices_len);

        if (sui->active_widget == widget) return 1;
        return 0;
}

void sui_checkbox(struct sui_context* sui, char* name, i32* value)
{
        sui_assert(sui);
        struct sui_widget* widget = sui_ht_find(&sui->ht, name);
        if (!widget) {
                i32 x = sui->current_w;
                i32 y = sui->current_h;
                printf("checkbox, x %i, y %i\n", x, y);
                widget = sui_checkbox_create(&sui->arena, &sui->ht, name, x, y);
                sui->current_w += widget->rect.w;
        }

        if (sui_overlap(sui->io, widget->bbox)) {
                if (sui->io.ldown) sui->hot_widget = widget;
                if (sui->io.lup && sui->hot_widget == widget) sui->active_widget = widget;
        }
        sui_checkbox_to_vertices(widget, *value, &sui->vertices_len, sui->vertices + sui->vertices_len);
        if (sui->active_widget == widget) {
                if (*value) *value = 0;
                else *value = 1;
        }
}

void sui_render(struct sui_context* sui)
{
        sui_assert(sui);
        sui_backend_push_vertices(&sui->backend, sui->vertices_len, sui->vertices);
        sui_backend_draw(&sui->backend);
        sui->vertices_len = 0;
        if (sui->io.lup) sui->hot_widget = NULL;
        sui->active_widget = NULL;
}
