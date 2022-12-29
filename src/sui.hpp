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

#define SUI_LAYOUTSTACK_SIZE  16
#define SUI_CMDRECTSTACK_SIZE 128

#include <stdlib.h>

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

        void push(const T t)
        {
                if (idx < n) data[idx++] = t;
        }

        void pop()
        {
                if (idx) idx--;
        }

        T get()
        {
                return data[idx - 1];
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
struct SuiCommandRect;

struct SuiContext;

// DECLARATIONS

struct SuiColor {
        u8 r, g, b, a;
        SuiColor() = default;
        SuiColor(const u8 _r, const u8 _g, const u8 _b, const u8 _a);
};

struct SuiRect {
        i32 x, y, w, h;
        SuiRect() = default;
        SuiRect(const i32 _x, const i32 _y, const i32 _w, const i32 _h);
};

struct SuiFont {
        // TODO
};

struct SuiStyle {
        // TODO
        // SuiFont font;
        // i32      padding;
        i32      spacing;
        SuiColor colors[SUI_COLOR_MAX];

        SuiStyle() = default;
        SuiStyle(const i32 _spacing, const SuiColor windowbg, const SuiColor rect, const SuiColor recthover, const SuiColor rectfocus);
};

struct SuiLayout {
        // TODO
        SuiRect rect;
        SuiRect body;

        SuiLayout() = default;
        SuiLayout(const SuiRect _rect, const SuiRect _body);
};

struct SuiCommandRect {
        SuiRect  rect;
        SuiColor color;

        SuiCommandRect() = default;
        SuiCommandRect(const SuiRect _rect, const SuiColor _color);
};

struct SuiContext {
        // TODO
        // u32 hover_id;
        // u32 focus_id;

        SuiStyle                                        style;
        SuiStack<SUI_LAYOUTSTACK_SIZE, SuiLayout>       layouts;
        SuiStack<SUI_CMDRECTSTACK_SIZE, SuiCommandRect> cmdrects;

        SuiContext();
        void begin(const char* name, const SuiRect rect);
        void end();
        void row(const i32 n, const i32* widths, const i32 height);
        // void column(const i32 n, const i32 width, const i32 height);
        void rect();
};
