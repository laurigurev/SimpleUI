#pragma once

#include "sui_backend.h"
#include "sui_internal.h"
#include "sui_types.h"
#include "sui_utils.h"

struct sui_context;

struct sui_context {
        i32                 hot_widget;
        i32                 active_widget;
        struct sui_arena    arena;
        struct sui_ht       ht;
        struct sui_viewport viewport;
        struct sui_backend  backend;
        i32                 vertices_len;
        struct sui_vertex*  vertices;
};

void sui_init(struct sui_context* sui, ID3D11Device* device, i32 w, i32 h);
void sui_test(struct sui_context* sui, char* name, i32 x, i32 y, i32 w, i32 h);
void sui_render(struct sui_context* sui);
