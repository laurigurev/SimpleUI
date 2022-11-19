#pragma once

#include <stdint.h>

typedef uint8_t 	u8;
typedef int8_t 		i8;
typedef uint16_t 	u16;
typedef int16_t 	i16;
typedef uint32_t 	u32;
typedef int32_t 	i32;
typedef uint64_t 	u64;
typedef int64_t 	i64;

typedef float 		f32;
typedef double		d64;

#ifdef DEBUG

#include <assert.h>
#define sui_assert(x) assert(x)

#else

#define sui_assert(x)

#endif

#include <stdio.h>
#define sui_todo(x) puts("feature '"x"' not implemented yet");assert(0);

#define WIN32_MEAN_AND_LEAN
#define COBJMACROS
#include <d3d11.h>
#include <windows.h>

struct sui_context;
// struct sui_window;
// struct sui_button;
// struct sui_label;
// struct sui_slider;
struct sui_vertex;
struct sui_rect;



void sui_init_fixed(struct sui_context* sui, ID3D11Device* d11device, i32 w, i32 h);
void sui_terminate(struct sui_context* sui);
void sui_mouse_input(struct sui_context* sui, i32 x, i32 y);
void sui_begin(struct sui_context* sui, struct sui_rect* rect);
void sui_end(struct sui_context* sui);

// i32 sui_button(struct sui_context* sui);
// void sui_label();
// void sui_slider(f32* slider_value);



struct sui_vertex {
	f32 x;
	f32 y;
	u8 r;
	u8 g;
	u8 b;
	u8 a;
};

struct sui_rect {
	union { i32 x; i32 x0; };
	union { i32 y; i32 y0; };
	union { i32 w; i32 x1; };
	union { i32 h; i32 y1; };
};

struct sui_context {
	ID3D11Device* d11device;
	ID3D11DeviceContext* d11context;
	ID3D11VertexShader* d11vs;
	ID3D11InputLayout* d11il;
	ID3D11PixelShader* d11ps;
	ID3D11Buffer* d11vb;
	ID3D11Buffer* d11ib;

	i32 screen_w;
	i32 screen_h;

	i32 len;
	struct sui_vertex* vertices;
	// i32 window_offsets;

	i32 mx;
	i32 my;
};
