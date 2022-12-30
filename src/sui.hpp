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
#define SUI_VERTEX_SIZE       1024
#define SUI_MIN_RECT_WIDTH    16

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

        void pop()
        {
                if (idx) idx--;
        }

        T get()
        {
                return data[idx - 1];
        }

        void reset()
        {
                idx = 0;
        }
};

// FRONTEND
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
        // u64 hash;
        SuiRect rect;

        SuiLayout() = default;
        SuiLayout(const SuiRect _rect);
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
        void row(const i32 n, const i32* widths, i32 height);
        void column(const i32 n, i32 width, const i32* heights);
        void rect();
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
        SuiColor color;

        SuiVertex() = default;
        SuiVertex(const f32 _x, const f32 _y, const SuiColor _color);
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
        i32                  screen_x;
        i32                  screen_y;
        ID3D11Device*        device;
        ID3D11DeviceContext* context;
        ID3D11InputLayout*   input_layout;
        ID3D11VertexShader*  vertex_shader;
        ID3D11PixelShader*   pixel_shader;
        ID3D11Buffer*        vertex_buffer;
        ID3D11Buffer*        index_buffer;
        ID3D11Buffer*        constant_buffer;
        ID3D11BlendState*    blend_state;
        i32                  vertices_count;
        SuiBackendProfiler   profiler;

        SuiBackend(ID3D11Device* _device, const i32 x, const i32 y);
        void record(i32 n, const SuiCommandRect* cmdrects);
        void draw();
};
