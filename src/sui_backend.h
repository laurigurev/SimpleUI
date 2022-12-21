#pragma once

#include "sui_internal.h"
#include "sui_types.h"

#define WIN32_MEAN_AND_LEAN
#define COBJMACROS
#include <d3d11.h>
#include <windows.h>

struct sui_backend;
// struct sui_backend_stats;
// struct sui_backend_profiler;

struct sui_backend {
        ID3D11Device*             device;
        ID3D11DeviceContext*      context;
        ID3D11VertexShader*       vs;
        ID3D11InputLayout*        il;
        ID3D11PixelShader*        ps;
        ID3D11Buffer*             vb;
        ID3D11Buffer*             ib;
        ID3D11Buffer*             cb;
        ID3D11BlendState*         bs;
        ID3D11ShaderResourceView* view;
        ID3D11SamplerState*       sampler;

        i32 n;
        i32 iw; // image width
        i32 ih; // image height
};

void sui_backend_init(struct sui_backend* backend, ID3D11Device* device, i32 w, i32 h);
void sui_backend_free(struct sui_backend* backend);
void sui_backend_push_vertices(struct sui_backend* backend, i32 n, struct sui_vertex* vertices);
void sui_backend_draw(struct sui_backend* backend);
