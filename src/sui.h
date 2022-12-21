#pragma once

/*
 *	DECLARATIONS
 */

#include <stdio.h>

/*
 *	FORWARD DECLARATIONS
 */

struct sui_context;
struct sui_io;
struct sui_color;
struct sui_rect;
struct sui_layout;
struct sui_item;
struct sui_style;

/* struct sui_viewport;
struct sui_vertex;
struct sui_glyph;
struct sui_window;
struct sui_widget;
struct sui_color;
union sui_rect;
struct sui_state; */

/*
 *	USER FUNCTIONS
 */

i64 sui_time_begin();
i64 sui_time_end(i64 begin);
i32 sui_hash(char* str, i32 mask);

void sui_init(struct sui_context *sui, ID3D11Device *d11device, i32 w, i32 h);
void sui_terminate(struct sui_context *sui);

void sui_input(struct sui_context* sui, i16 mx, i16 my, u8 rdown, u8 rup, u8 ldown, u8 lup);

void sui_begin(struct sui_context* sui, const char* name);
void sui_end(struct sui_context* sui);

void sui_style(struct sui_context* sui, struct sui_style* style);
void sui_row(struct sui_context* sui);
void sui_empty_row(struct sui_context* sui);
void sui_column(struct sui_context* sui);
void sui_empty_column(struct sui_context* sui);

void sui_label(struct sui_context* sui, const char* name);
void sui_text(struct sui_context* sui, const char* text, ...);
i32 sui_button(struct sui_context* sui, const char* name);
void sui_checkbox(struct sui_context* sui, const char* name, i32* bool);
void sui_slider(struct sui_context* sui, const char* name, f32* value);

// TODO: attempt to remove these two functions
/* struct sui_widget sui_create_widget(char* str, struct sui_color color, struct sui_color hover_color,
                                    struct sui_color bg_color, struct sui_color hover_bg_color);

struct sui_window sui_create_window(char* str, struct sui_color color, struct sui_color hover_color,
                                    struct sui_color bg_color, struct sui_color hover_bg_color);
void sui_begin(struct sui_context* sui, struct sui_window* window, f32 x, f32 y);
void sui_end(struct sui_context* sui);
void sui_row(struct sui_context* sui);
void sui_update_window(struct sui_window* window);
union sui_rect sui_getuv(char c, f32 w, f32 h);
void sui_putr(struct sui_vertex* vertex, union sui_rect rect, struct sui_color color);
f32 sui_putc(struct sui_vertex* vertex, char c, f32 x, f32 y, struct sui_color color);
struct sui_state sui_handle_state(union sui_rect bbox, struct sui_state prev, struct sui_io io);
i32 sui_string_len(char* str);
void sui_get_params(struct sui_window* root, struct sui_widget* widget, char* str, union sui_rect* rect, union sui_rect* bbox);
void sui_draw_rect(struct sui_context* sui, union sui_rect rect, struct sui_color color);
void sui_draw_string(char* str, struct sui_context* sui, struct sui_widget* widget, union sui_rect rect, struct sui_color color);
void sui_update_root(struct sui_window* root, struct sui_widget* widget, union sui_rect rect);
void sui_button(struct sui_context* sui, struct sui_widget* widget, const char* str, struct sui_state* state);
void sui_label(struct sui_context* sui, struct sui_widget* widget, const char* str);
void sui_checkbox(struct sui_context* sui, struct sui_widget* widget, struct sui_state* state);
void sui_slider(struct sui_context* sui, struct sui_widget* widget, struct sui_state* state, f32* value); */

void sui_render(struct sui_context *sui);


/*
 *	STRUCTURES
 */

struct sui_color { u8 r; u8 g; u8 b; u8 a; };
struct sui_rect { f32 x; f32 y; f32 w; f32 h; };
struct sui_layout { struct sui_rect rect; struct sui_color color; i32 root; };

struct sui_item {
        char* name;
        i32 buffer_offset;
        struct sui_rect rect;
        struct sui_rect bbox;
        struct sui_color color;
};

struct sui_style {
        struct sui_color color0;
        struct sui_color color1;
        struct sui_color color2;
        struct sui_color color3;
        f32 margin;
        f32 padding;
        f32 font_size;
        f32 size;
        // text alignment;
};

/* struct sui_state {
        u8 pressed;
        u8 released;
        u8 hovering;
        u8 clicked;
        u8 held;
        u8 on;
        u8 dragged;
}; */

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

/* union sui_rect {
        struct { f32 x; f32 y; f32 w; f32 h; };
        struct { f32 x0; f32 x1; f32 y0; f32 y1; }; 
        struct { f32 u0; f32 u1; f32 v0; f32 v1; };
};

struct sui_widget {
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
        struct sui_state state;
}; */

struct sui_context {
        // window
        struct sui_window* current_window;
        
        // vertices
        struct sui_vertex *vertices;
        i32 vlen;

        // backend
        i32 mask;
        i32 layouts_len;
        struct sui_layout* layouts;
        i32 items_len;
        struct sui_item* items;
        
        struct sui_io io;
        struct sui_viewport viewport;
        i32 img_w;
        i32 img_h;
        f32 img_wf;
        f32 img_hf;
};
