#pragma once

// TYPES
#include <stdint.h>

typedef uint8_t  u8;
typedef int8_t   i8;
typedef uint16_t u16;
typedef int16_t  i16;
typedef uint32_t u32;
typedef int32_t  i32;
typedef uint64_t u64;
typedef int64_t  i64;
typedef float    f32;
typedef double   d64;

// DEFINES

#define SUI_PANELSTACK_SIZE  16
#define SUI_LAYOUTSTACK_SIZE 16
#define SUI_RECTSTACK_SIZE   64

#define SuiAssert(x)                                                                                     \
        {                                                                                                \
                if (!(x)) {                                                                              \
                        fprintf(stderr, "Error: %s:%i assertion '%s' failed\n", __FILE__, __LINE__, #x); \
                        abort();                                                                         \
                }                                                                                        \
        }

// UTILS
template <i32 n, typename T>
struct SuiStack {
        i32 idx;
        T   data[n];

        SuiStack() : idx(0) {}

        void push(T t)
        {
                if (idx < n) data[idx++] = t;
        }

        void pop()
        {
                if (idx) idx--;
        }
};

// ENUMS

enum {
        SUI_COLOR_WINDOWBG,
        SUI_COLOR_RECT,
        SUI_COLOR_RECTHOVER,
        SUI_COLOR_RECTFOCUS,
        SUI_COLOR_MAX
};

// FORWARD DECLARATIONS

struct SuiColor;
struct SuiRect;

struct SuiFont;
struct SuiStyle;
struct SuiLayout;
struct SuiPanel;

struct SuiContext;

// DECLARATIONS

struct SuiColor {
        u8 r, g, b, a;
        SuiColor(const u8 _r, const u8 _g, const u8 _b, const u8 _a);
};

struct SuiRect {
        i32 x, y, w, h;
        SuiRect(const i32 _x, const i32 _y, const i32 _w, const i32 _h);
};

struct SuiFont {
        // TODO
};

struct SuiStyle {
        // TODO
        // SuiFont font;
        i32      padding;
        i32      spacing;
        SuiColor colors[SUI_COLOR_MAX];
};

struct SuiLayout {
        // TODO
        SuiRect rect;
};

struct SuiPanel {
        i32     layout;
        SuiRect rect;
        SuiRect bbox;
        // i32 zindex;
};

struct SuiContext {
        // TODO
        // u32 hover_id;
        // u32 focus_id;

        // TODO: figure out where should you put SuiStyle
        SuiStack<SUI_PANELSTACK_SIZE, SuiPanel>   panels;
        SuiStack<SUI_LAYOUTSTACK_SIZE, SuiLayout> layouts;
        SuiStack<SUI_RECTSTACK_SIZE, SuiRect>     rects;

        SuiContext();
        // void init();
        void begin(const char* name, SuiRect rect);
        void end();
        void row(const i32 n, const i32* widths, const i32 height);
        void column(const i32 n, const i32 width, const i32 height);
        void rect();
};
