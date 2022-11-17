#pragma once

#include "sui_def.h"

#define WIN32_MEAN_AND_LEAN
#define COBJMACROS
#include <d3d11.h>
#include <windows.h>

struct sui_context;
struct sui_window;

void sui_init_fixed(struct sui_context* sui, ID3D11Device* d11device);
void sui_terminate(struct sui_context* sui);
void sui_window_init();
void sui_window_terminate();

i32 sui_button(struct sui_context* sui);
void sui_label();
void sui_slider(f32* slider_value);

struct sui_context {
	ID3D11Device* d11device;
	ID3D11DeviceContext* d11context;
	ID3D11VertexShader* d11vs;
	ID3D11InputLayout* d11il;
	ID3D11PixelShader* d11ps;
	ID3D11Buffer* d11vb;
	ID3D11Buffer* d11ib;
};
