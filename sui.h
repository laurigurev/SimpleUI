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

typedef uint8_t u8;
typedef int8_t i8;
typedef uint16_t u16;
typedef int16_t i16;
typedef uint32_t u32;
typedef int32_t i32;
typedef uint64_t u64;
typedef int64_t i64;

typedef float f32;
typedef double d64;

#ifdef DEBUG

#include <assert.h>
#define sui_assert(x) assert(x)

#else

#define sui_assert(x)

#endif

#include <stdio.h>
#define WIN32_MEAN_AND_LEAN
#define COBJMACROS
#include <d3d11.h>
#include <windows.h>

#define sui_max(a, b) ((a) > (b) ? (a) : (b))

/*
 *	FORWARD DECLARATIONS
 */

struct sui_context;
struct sui_viewport;
struct sui_vertex;
struct sui_glyph;
struct sui_window;
struct sui_widget;
struct sui_color;
union sui_rect;
// struct sui_image;
struct sui_io;
// struct sui_state;

/*
 *	USER FUNCTIONS
 */

void sui_init(struct sui_context *sui, ID3D11Device *d11device, i32 w, i32 h);
void sui_terminate(struct sui_context *sui);

void sui_input(struct sui_context* sui, i16 mx, i16 my, u8 rdown, u8 rup, u8 ldown, u8 lup);
// TODO: attempt to remove these two functions
struct sui_widget sui_create_widget(char* str, struct sui_color color, struct sui_color hover_color,
                                    struct sui_color bg_color, struct sui_color hover_bg_color);

struct sui_window sui_create_window(char* str, struct sui_color color, struct sui_color hover_color,
                                    struct sui_color bg_color, struct sui_color hover_bg_color);
void sui_begin(struct sui_context* sui, struct sui_window* window, f32 x, f32 y);
void sui_end(struct sui_context* sui);
void sui_row(struct sui_context* sui);
union sui_rect sui_getuv(char c, f32 w, f32 h);
void sui_putr(struct sui_vertex* vertex, union sui_rect rect, struct sui_color color);
f32 sui_putc(struct sui_vertex* vertex, char c, f32 x, f32 y, struct sui_color color);
i32 sui_overlap(union sui_rect bbox, f32 mx, f32 my);
i64 sui_time_begin();
i64 sui_time_end(i64 begin);
// TODO: redo sui_button as:
//       struct sui_state sui_button(struct sui_context* sui, struct sui_widget* widget, const char* str);
void sui_button(struct sui_context* sui, struct sui_widget* widget);

// void sui_slider(struct sui_context* sui);
// void sui_canvas(struct sui_context* sui);
// void sui_radiobtn(struct sui_context* sui);
// void sui_checkbox(struct sui_context* sui);

void sui_render(struct sui_context *sui);


/*
 *	STRUCTURES
 */

struct sui_io {
        i16 mx;
        i16 my;
        // delta mouse coordinates to previous coordinates
        i16 dmx;
        i16 dmy;

        i64 rclick_time;
        i64 lclick_time;

        u8 rdown;
        u8 rheld;
        u8 rup;
        u8 rclicked;

        u8 ldown;
        u8 lheld;
        u8 lup;
        u8 lclicked;
};

union sui_rect {
        struct { f32 x; f32 y; f32 w; f32 h; };
        struct { f32 x0; f32 x1; f32 y0; f32 y1; }; 
        struct { f32 u0; f32 u1; f32 v0; f32 v1; };
};

struct sui_color {
        u8 r;
        u8 g;
        u8 b;
        u8 a;
};

struct sui_vertex {
        f32 x;
        f32 y;
        f32 u;
        f32 v;
        u8 r;
        u8 g;
        u8 b;
        u8 a;
};

struct sui_widget {
        // TODO: separate public and private data into different structures
        // user fields
        char* str;
        struct sui_color color;
        struct sui_color hover_color;
        struct sui_color bg_color;
        struct sui_color hover_bg_color;
        // padding is space we leave between the object and its borders
        f32 pad_left;
        f32 pad_right;
        f32 pad_top;
        f32 pad_bottom;
        // margin is space we leave around the object
        f32 margin_left;
        f32 margin_right;
        f32 margin_top;
        f32 margin_bottom;
        // f32 rotation;
        // f32 size;
        
        // generated fields
        u8 held;
        u8 pressed;
        u8 released;
        u8 hovering;
        u8 clicked;
};

struct sui_window {
        struct sui_widget widget;
        struct sui_vertex* p_vertex;
        struct sui_window* root;
        union sui_rect rect;
        union sui_rect bbox;
        f32 current_w;
        f32 current_h;
        f32 current_max_h;
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

struct sui_viewport {
        i32 w;
        i32 h;
};

struct sui_context {
        // window
        struct sui_window* current_window;
        
        // vertices
        struct sui_vertex *vertices;
        i32 vlen;

        // backend
        struct sui_io io;
        struct sui_viewport viewport;
        i32 img_w;
        i32 img_h;
        f32 img_wf;
        f32 img_hf;

        // graphics api backend
        ID3D11Device *d11device;
        ID3D11DeviceContext *d11context;
        ID3D11VertexShader *d11vs;
        ID3D11InputLayout *d11il;
        ID3D11PixelShader *d11ps;
        ID3D11Buffer *d11vb;
        ID3D11Buffer *d11ib;
        ID3D11Buffer *d11cb;
        ID3D11BlendState *d11bs;
        ID3D11ShaderResourceView *view;
        ID3D11SamplerState *sampler;
};
