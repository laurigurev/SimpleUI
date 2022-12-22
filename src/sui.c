#include "sui.h"
#include "sui_defines.h"

void sui_init(struct sui_context* sui, ID3D11Device* device, i32 w, i32 h)
{
        sui_assert(sui);
        sui->active_widget = -1;
        sui->hot_widget = -1;
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

void sui_test(struct sui_context* sui, char* name, i32 x, i32 y, i32 w, i32 h)
{
        sui_assert(sui);
        struct sui_widget* widget = sui_ht_find(&sui->ht, name);
        if (!widget) widget = sui_widget_create(&sui->arena, &sui->ht, name, x, y, w, h);
        sui_widget_to_vertices(widget, &sui->vertices_len, sui->vertices + sui->vertices_len);
}

void sui_render(struct sui_context* sui)
{
        sui_assert(sui);
        sui_backend_push_vertices(&sui->backend, sui->vertices_len, sui->vertices);
        sui_backend_draw(&sui->backend);
        sui->vertices_len = 0;
}
