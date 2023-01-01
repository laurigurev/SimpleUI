// #pragma once

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
#define SUI_VERTEX_SIZE       1024
#define SUI_MIN_RECT_WIDTH    16
#define SUI_MIN_RECT_HEIGHT   16

#include <stdlib.h>

#define SuiAssert(x)                                                                                                               \
        {                                                                                                                          \
                if (!(x)) {                                                                                                        \
                        fprintf(stderr, "Error: %s:%i, assertion '%s' in %s(...) failed\n", __FILE__, __LINE__, #x, __FUNCTION__); \
                        abort();                                                                                                   \
                }                                                                                                                  \
        }

#define SuiMax(a, b) ((a) > (b) ? (a) : (b))
#define SuiMin(a, b) ((a) < (b) ? (a) : (b))

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

        T pop()
        {
                return data[--idx];
        }

        T peek()
        {
                return data[idx - 1];
        }

        i32 empty()
        {
                if (idx) return 0;
                return 1;
        }

        i32 full()
        {
                if (idx == n) return 1;
                return 0;
        }

        void reset()
        {
                idx = 0;
        }
};

u32 SuiHash(const char* s);

// FRONTEND
// ENUMS

enum {
        SUI_COLOR_WINDOWBG,
        SUI_COLOR_LAYOUT,
        SUI_COLOR_TEXT,
        SUI_COLOR_LABELBG,
        SUI_COLOR_BUTTON,
        SUI_COLOR_BUTTON_HOT,
        SUI_COLOR_MAX
};

enum {
        SUI_ALIGNMENT_FLAG_LEFT = (1 << 0),
        SUI_ALIGNMENT_FLAG_RIGHT = (1 << 1),
        SUI_ALIGNMENT_FLAG_TOP = (1 << 2),
        SUI_ALIGNMENT_FLAG_BOTTOM = (1 << 3),
};

enum SuiLayoutAction {
        SUI_LAYOUT_ACTION_SPLIT,
        SUI_LAYOUT_ACTION_NEXT
};

// TYPEDEFS

typedef u32 SuiAlignmentFlags;

// FORWARD DECLARATIONS

// struct SuiVector;
struct SuiColor;
struct SuiRect;

struct SuiUV;
struct SuiGlyph;
struct SuiFont;
struct SuiStyle;
struct SuiLayout;
struct SuiRectCommand;
struct SuiIO;

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

struct SuiUV {
        f32 u0;
        f32 u1;
        f32 v0;
        f32 v1;

        SuiUV(const i32 x, const i32 y, const i32 w, const i32 h);
};

struct SuiGlyph {
        i32 w;
        i32 h;
        i32 xoff;
        i32 yoff;

        SuiGlyph(const i32 _w, const i32 _h, const i32 _xoff, const i32 _yoff);
};

struct SuiFont {
        // 127 - 32 = 95
        const char* name;
        SuiUV       uvs[96];
        SuiGlyph    glyphs[96];
        i32         xadvs[96];

        SuiFont();
};

struct SuiStyle {
        // TODO
        // i32      padding;
        SuiFont  font;
        i32      spacing;
        SuiColor colors[SUI_COLOR_MAX];

        SuiStyle();
};

struct SuiLayout {
        // u64 hash;
        SuiRect rect;

        SuiLayout() = default;
        SuiLayout(const SuiRect _rect);
};

struct SuiRectCommand {
        SuiRect  rect;
        SuiColor color;
        i32      c;

        SuiRectCommand() = default;
        SuiRectCommand(const SuiRect _rect, const SuiColor _color, const i32 _c);
};

struct SuiIO {
        i32 mx, my, dmx, dmy;
        u8  ldown, lup, lheld, lclick;
        u8  rdown, rup, rheld, rclick;

        SuiIO();
        i32 mxy_in_rect(const SuiRect rect);
};

struct SuiContext {
        u32   hot_id;
        u32   active_id;
        SuiIO io;

        SuiStyle                                        style;
        SuiStack<SUI_LAYOUTSTACK_SIZE, SuiLayout>       layouts;
        SuiStack<SUI_CMDRECTSTACK_SIZE, SuiRectCommand> rectcmds;

        SuiContext();
        void inputs(i32 mx, i32 my, u8 ldown, u8 lup, u8 rdown, u8 rup);
        void begin(const char* name, const SuiRect rect);
        void end();
        void row(const i32 n, const i32* widths, i32 height);
        void column(const i32 n, i32 width, const i32* heights);
        // void box_ex(const char* name, const i32 w, const i32 h, const SuiAlignmentFlags flags, const SuiLayoutAction action);
        void  reveal_layout();
        SuiRect get_rect(const i32 w, const i32 h, const SuiAlignmentFlags alignment, const SuiLayout& layout);
        void    label(const char* s);
        i32 button(const char* s);
        void    next();
        // TODO: change to void finish();
        void reset();
};

// BACKEND

// FORWARD DECLARATIONS

struct SuiVertex;
struct SuiBackendProfiler;
struct SuiBackend;

// DECLARATIONS

#define WIN32_MEAN_AND_LEAN
#include <d3d11.h>

struct SuiVertex {
        f32      x, y;
        f32      u, v;
        SuiColor color;

        SuiVertex() = default;
        SuiVertex(const f32 _x, const f32 _y, const f32 _u, const f32 _v, const SuiColor _color);
};

struct SuiBackendProfiler {
        ID3D11Query* timestamp0;
        ID3D11Query* timestamp1;
        ID3D11Query* disjointed;
        ID3D11Query* pipeline_stats;
        ID3D11Query* so_stats;

        d64 time;

        // D3D11_QUERY_DATA_PIPELINE_STATISTICS
        u64 ia_vertices;
        u64 min_ia_vertices;
        u64 max_ia_vertices;
        // // u64 avg_ia_vertices;

        u64 ia_primitives;
        u64 min_ia_primitives;
        u64 max_ia_primitives;
        // u64 avg_ia_primitives;

        u64 vs_invocations;
        u64 min_vs_invocations;
        u64 max_vs_invocations;
        // u64 avg_vs_invocations;

        u64 ps_invocations;
        u64 min_ps_invocations;
        u64 max_ps_invocations;
        // u64 avg_ps_invocations;

        u64 cs_invocations;
        u64 min_cs_invocations;
        u64 max_cs_invocations;
        // u64 avg_cs_invocations;

        // D3D11_QUERY_DATA_SO_STATISTICS
        u64 num_primitives_written;
        u64 min_num_primitives_written;
        u64 max_num_primitives_written;
        // u64 avg_num_primitives_written;

        u64 primitives_storage_needed;
        u64 min_primitives_storage_needed;
        u64 max_primitives_storage_needed;
        // u64 avg_primitives_storage_needed;

        SuiBackendProfiler(ID3D11Device* device);
        void begin(ID3D11DeviceContext* context);
        void end(ID3D11DeviceContext* context);
        void update(ID3D11DeviceContext* context);
};

struct SuiBackend {
        i32                       screen_x;
        i32                       screen_y;
        ID3D11Device*             device;
        ID3D11DeviceContext*      context;
        ID3D11InputLayout*        input_layout;
        ID3D11VertexShader*       vertex_shader;
        ID3D11PixelShader*        pixel_shader;
        ID3D11Buffer*             vertex_buffer;
        ID3D11Buffer*             index_buffer;
        ID3D11Buffer*             constant_buffer;
        ID3D11BlendState*         blend_state;
        ID3D11ShaderResourceView* view;
        ID3D11SamplerState*       sampler;
        i32                       vertices_count;
        SuiBackendProfiler        profiler;
        const SuiUV*              uvs;

        SuiBackend(ID3D11Device* _device, const i32 x, const i32 y, const SuiUV* _uvs);
        void record(i32 n, const SuiRectCommand* rectcmds);
        void draw();
};
