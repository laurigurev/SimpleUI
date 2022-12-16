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

#define CLICK_DELTA_RESET 9223372036854775807
#define sui_max(a,b) ((a)>(b)?(a):(b))

/*
 *	FORWARD DECLARATIONS
 */

struct sui_context;
struct sui_viewport;
struct sui_vertex;
struct sui_glyph;
struct sui_color;
// struct sui_image;
// TODO: struct sui_widget;
// struct sui_window;
// struct sui_io;

/*
 *	USER FUNCTIONS
 */

void sui_init(struct sui_context* sui, ID3D11Device* d11device, i32 w, i32 h);
void sui_terminate(struct sui_context* sui);
void sui_test(struct sui_context* sui);
void sui_txt_test(struct sui_context* sui, const char* str);

/* void sui_input(struct sui_context* sui, i16 mx, i16 my, u8 rdown, u8 rup, u8 ldown, u8 lup);

void sui_begin(struct sui_context* sui, i16* x, i16* y);
void sui_end(struct sui_context* sui);
void sui_row(struct sui_context* sui);

i32 sui_button(struct sui_context* sui, const char* name);
void sui_label(struct sui_context* sui, const char* str); */
// void sui_slider(struct sui_context* sui);
// void sui_canvas(struct sui_context* sui);
// void sui_radiobtn(struct sui_context* sui);
// void sui_checkbox(struct sui_context* sui);

void sui_render(struct sui_context* sui);
i16 sui_putc(struct sui_context* sui, char c, f32 x, f32 y);
void sui_rect(
	struct sui_context* sui, struct sui_vertex* vertex, f32 x, f32 y, f32 w, f32 h, 
	struct sui_color color, f32 rotation, f32 scale
);

/* void sui_rect_insert(
	struct sui_context* sui, i32 vi, 
	i16 x, i16 y, i16 w, i16 h, 
	u8 r, u8 g, u8 b, u8 a
);
i32 sui_hover(struct sui_io* io, i16 x, i16 y, i16 w, i16 h);
i64 sui_timer_begin();
i64 sui_timer_end(i64 begin); */

/*
 *	STRUCTURES
 */

/* struct sui_io {
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

	u8 rclick;
	u8 lclick;
	i64 rclick_delta;
	i64 lclick_delta;
	
	// TODO: double click
};

struct sui_window {
	i16 x;
	i16 y;
	i16 w;
	i16 h;

	i32 vi;
	i16 max_w;

	// margin is space we leave around the object
	// padding is space we leave between the object and its borders
	// align
	// border
	
	i16 pad;
	i16 child_margin;

	i16 row;
}; */

struct sui_color {
	u8 r; u8 g; u8 b; u8 a;
};

struct sui_glyph {
	i16 id;
	i16 x;
	i16 y;
	i16 width;
	i16 height;
	i16 xoffset;
	i16 yoffset;
	i16 xadvance;
};

struct sui_vertex {
	f32 x; f32 y;
	f32 u; f32 v;
	u8 r; u8 g; u8 b; u8 a;
};

struct sui_viewport { i32 w; i32 h; };

struct sui_context {
	// vertices
	struct sui_vertex* vertices;
	i32 vlen;

	// backend
	struct sui_viewport viewport;
	i32 img_w;
	i32 img_h;
	f32 img_wf;
	f32 img_hf;

	// graphics api backend
	ID3D11Device* d11device;
	ID3D11DeviceContext* d11context;
	ID3D11VertexShader* d11vs;
	ID3D11InputLayout* d11il;
	ID3D11PixelShader* d11ps;
	ID3D11Buffer* d11vb;
	ID3D11Buffer* d11ib;
	ID3D11Buffer* d11cb;
	ID3D11BlendState* d11bs;
	ID3D11ShaderResourceView* view;
	ID3D11SamplerState* sampler;
};
