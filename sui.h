#pragma once

/*
 *	SECTIONS:
 *	 - DECLARATIONS
 *	 - FORWARD DECLARATIONS
 *	 - USER FUNCTIONS
 *	 - STRUCTURES
 */

/*
 *	DECLARATIONS
 */

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

/*
 *	FORWARD DECLARATIONS
 */

struct sui_context;
struct sui_viewport;
struct sui_vertex;
struct sui_window;
struct sui_io;

/*
 *	USER FUNCTIONS
 */

void sui_init(struct sui_context* sui, ID3D11Device* d11device, i32 w, i32 h);
void sui_terminate(struct sui_context* sui);

void sui_input(struct sui_context* sui, i16 mx, i16 my);

void sui_begin(struct sui_context* sui, i16* x, i16* y);
void sui_end(struct sui_context* sui);

// TODO
void sui_button(struct sui_context* sui, const char* name);
// void sui_label(struct sui_context* sui);
// void sui_slider(struct sui_context* sui);
// void sui_canvas(struct sui_context* sui);
// void sui_radiobtn(struct sui_context* sui);
// void sui_checkbox(struct sui_context* sui);

void sui_render(struct sui_context* sui);
void sui_rect_insert(
	struct sui_context* sui, i32 vi, 
	i16 x, i16 y, i16 w, i16 h, 
	u8 r, u8 g, u8 b, u8 a
);
i32 sui_hover(struct sui_io* io, i16 x, i16 y, i16 w, i16 h);

/*
 *	STRUCTURES
 */

struct sui_io {
	i16 mx;
	i16 my;
	// delta mouse coordinates to previous coordinates
	i16 dmx;
	i16 dmy;

	u8 rdown;
	u8 rheld;
	u8 rup;

	u8 ldown;
	u8 lheld;
	u8 lup;

	// u8 rclicked;
	// u8 lclicked;
	// u64 click_delta;
	
	// TODO: double click
};

struct sui_window {
	i16 x;
	i16 y;
	i16 w;
	i16 h;

	i32 vi;

	// margin is space we leave around the object
	// padding is space we leave between the object and its borders
	// align
	// border
	
	i16 pad;
	i16 child_margin;
};

struct sui_vertex {
	f32 x; f32 y;
	u8 r; u8 g; u8 b; u8 a;
};

struct sui_viewport { i32 w; i32 h; };

struct sui_context {
	// window
	struct sui_window window;

	// vertices
	struct sui_vertex* vertices;
	i32 vlen;

	// backend
	struct sui_io io;
	struct sui_viewport viewport;

	// graphics api backend
	ID3D11Device* d11device;
	ID3D11DeviceContext* d11context;
	ID3D11VertexShader* d11vs;
	ID3D11InputLayout* d11il;
	ID3D11PixelShader* d11ps;
	ID3D11Buffer* d11vb;
	ID3D11Buffer* d11ib;
};
