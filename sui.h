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
// TODO
//  - replace sui_widget with sui_window, because you 
//    don't really need to save other widgets into
//    memory.
struct sui_widget;
// struct sui_window_handler;
struct sui_viewport;
struct sui_input_state;
struct sui_vertex;

enum sui_widget_type;

/*
 *	USER FUNCTIONS
 */

void sui_init(struct sui_context* sui, ID3D11Device* d11device, i32 w, i32 h);
void sui_terminate(struct sui_context* sui);
void sui_inputs(struct sui_context* sui);
void sui_begin(struct sui_context* sui, i32 i, i16 x, i16 y, i16 w, i16 h);
void sui_end(struct sui_context* sui);
i32 sui_button(struct sui_context* sui);
void sui_render(struct sui_context* sui);

/*
 *	ENUMS
 */

enum sui_widget_type {
	SUI_WIDGET_TYPE_WINDOW
};

/*
 *	STRUCTURES
 */

struct sui_vertex {
	f32 x; f32 y;
	u8 r; u8 g; u8 b; u8 a;
};

struct sui_input_state {
	i16 mx;
	i16 my;

	// u64 frequency
	u64 prev_time;
	u64 delta_time;
	u64 click_time;

	u8 lpressed;
	u8 ldown;
	u8 lup;
	u8 lclicked;

	u8 rpressed;
	u8 rdown;
	u8 rup;
	u8 rclicked;

	// i8 scroll;
};

struct sui_viewport { i32 w; i32 h; };

struct sui_widget {
	enum sui_widget_type type;
	i16 x; i16 y; i16 w; i16 h;
	// name[MAXLEN];
	// struct sui_widget* next;
	// struct sui_widget* prev;
};

struct sui_context {
	// user function handling
	struct sui_widget* current_window;
	i32 voff;
	i16 current_row;
	i16 window_border;
	i16 window_title_bar_h;
	i16 window_child_h;

	// widgets
	struct sui_widget* widgets;
	i32 wlen;
	struct sui_widget** p_windows;

	// vertices
	struct sui_vertex* vertices;
	i32 vlen;

	// backend
	struct sui_viewport viewport;
	struct sui_input_state input_state;

	// graphics api backend
	ID3D11Device* d11device;
	ID3D11DeviceContext* d11context;
	ID3D11VertexShader* d11vs;
	ID3D11InputLayout* d11il;
	ID3D11PixelShader* d11ps;
	ID3D11Buffer* d11vb;
	ID3D11Buffer* d11ib;
};
