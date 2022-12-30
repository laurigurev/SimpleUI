#include "sui.hpp"
#include <stdio.h>

SuiColor::SuiColor(const u8 _r, const u8 _g, const u8 _b, const u8 _a) : r(_r), g(_g), b(_b), a(_a) {}
SuiRect::SuiRect(const i32 _x, const i32 _y, const i32 _w, const i32 _h) : x(_x), y(_y), w(_w), h(_h) {}
SuiStyle::SuiStyle(const i32 _spacing, const SuiColor windowbg, const SuiColor rect, const SuiColor recthover, const SuiColor rectfocus, const SuiColor box)
    : spacing(_spacing), colors{windowbg, rect, recthover, rectfocus, box}
{
}
SuiLayout::SuiLayout(const SuiRect _rect) : rect(_rect) {}
SuiCommandRect::SuiCommandRect(const SuiRect _rect, const SuiColor _color) : rect(_rect), color(_color) {}

SuiContext::SuiContext()
    : style(2, SuiColor(255, 255, 255, 255), SuiColor(100, 0, 0, 255), SuiColor(255, 0, 0, 255), SuiColor(150, 0, 0, 255), SuiColor(0, 0, 0, 255))
{
}

void SuiContext::begin(const char* name, const SuiRect rect)
{
        const i32 spacing = style.spacing;
        SuiRect   body = {rect.x + spacing, rect.y + spacing, rect.w - 2 * spacing, rect.h - 2 * spacing};
        layouts.push(SuiLayout(body));
        cmdrects.push(SuiCommandRect(rect, style.colors[SUI_COLOR_WINDOWBG]));
}

void SuiContext::end()
{
        layouts.pop();
}

void SuiContext::row(const i32 n, const i32* widths, i32 height)
{
        SuiAssert(0 < n && n < 16);
        SuiAssert(widths);

        // Declare variables
        SuiLayout curr_layout = layouts.get();
        layouts.pop();
        if (height == -1) height = curr_layout.rect.h;
        SuiAssert(height <= curr_layout.rect.h);

        // get last assert
        i32 ws[16];
        i32 leftover_width, summed_width = 0, body_width = 0, neg1counter = 0;

        body_width = curr_layout.rect.w - (n - 1) * style.spacing;
        leftover_width = body_width;

        memcpy(ws, widths, n * sizeof(i32));
        for (i32 i = 0; i < n; i++) {
                if (widths[i] == -1) {
                        neg1counter++;
                        continue;
                }
                summed_width += widths[i];
                leftover_width -= widths[i];
        }
        summed_width += neg1counter * SUI_MIN_RECT_WIDTH;
        SuiAssert(summed_width <= body_width);

        SuiLayout new_layouts[16];
        i32       x, y, width, neg1w = 0, leftover = 0, xoff = 0;

        x = curr_layout.rect.x;
        y = curr_layout.rect.y;
        if (neg1counter) {
                neg1w = leftover_width / neg1counter;
                leftover = leftover_width - neg1w * neg1counter;
        }
        for (i32 i = 0; i < n; i++) {
                width = widths[i];
                if (width == -1) width = neg1w;
                if (i == n - 1) width += leftover;
                SuiRect rect = {x + xoff, y, width, height};
                xoff += width + style.spacing;
                new_layouts[i] = SuiLayout(rect);
        }

        i32 h0, h1;

        h0 = height + style.spacing;
        h1 = curr_layout.rect.h - h0;
        curr_layout.rect = {x, y + h0, curr_layout.rect.w, h1};
        layouts.push(curr_layout);

        for (i32 i = n - 1; 0 <= i; i--) layouts.push(new_layouts[i]);
}

void SuiContext::column(const i32 n, i32 width, const i32* heights)
{
        SuiAssert(0 < n && n < 16);
        SuiAssert(heights);

        // Declare variables
        SuiLayout curr_layout = layouts.get();
        layouts.pop();
        if (width == -1) width = curr_layout.rect.w;
        SuiAssert(width <= curr_layout.rect.w);

        // get last assert
        i32 hs[16];
        i32 leftover_height, summed_height = 0, body_height = 0, neg1counter = 0;

        body_height = curr_layout.rect.h - (n - 1) * style.spacing;
        leftover_height = body_height;

        memcpy(hs, heights, n * sizeof(i32));
        for (i32 i = 0; i < n; i++) {
                if (heights[i] == -1) {
                        neg1counter++;
                        continue;
                }
                summed_height += heights[i];
                leftover_height -= heights[i];
        }
        summed_height += neg1counter * SUI_MIN_RECT_HEIGHT;
        SuiAssert(summed_height <= body_height);

        SuiLayout new_layouts[16];
        i32       x, y, height, neg1h = 0, leftover = 0, yoff = 0;

        x = curr_layout.rect.x;
        y = curr_layout.rect.y;
        if (neg1counter) {
                neg1h = leftover_height / neg1counter;
                leftover = leftover_height - neg1h * neg1counter;
        }
        for (i32 i = 0; i < n; i++) {
                height = heights[i];
                if (height == -1) height = neg1h;
                if (i == n - 1) height += leftover;
                SuiRect rect = {x, y + yoff, width, height};
                yoff += height + style.spacing;
                new_layouts[i] = SuiLayout(rect);
        }

        i32 w0, w1;

        if (summed_height != body_height) {
                curr_layout.rect = {x, y + yoff, curr_layout.rect.w, curr_layout.rect.h - yoff};
                layouts.push(curr_layout);
        }
        w0 = width + style.spacing;
        w1 = curr_layout.rect.w - w0;
        curr_layout.rect = {x + w0, y, w1, yoff - style.spacing};
        layouts.push(curr_layout);

        for (i32 i = n - 1; 0 <= i; i--) layouts.push(new_layouts[i]);
}

void SuiContext::rect()
{
        // TODO: stronger error catching
        if (layouts.idx == 0) return;
        SuiLayout layout = layouts.get();
        layouts.pop();
        cmdrects.push(SuiCommandRect(layout.rect, style.colors[SUI_COLOR_RECT]));
}

void SuiContext::box(SuiAlignmentFlags flags)
{
        // TODO: stronger error catching
        if (layouts.idx == 0) return;
        SuiLayout layout = layouts.get();
        layouts.pop();

        const i32 x = layout.rect.x, y = layout.rect.y;
        const i32 w = 16, h = 16, halfw = 8, halfh = 8;
        i32 xoff, yoff;

        // SUI_ALIGNMENT_FLAG_HMIDDLE
        xoff = (layout.rect.w / 2) - halfw;
        if (flags & SUI_ALIGNMENT_FLAG_LEFT) {
                xoff = 0;
        }
        else if (flags & SUI_ALIGNMENT_FLAG_RIGHT) {
                xoff = layout.rect.w - w;
        }

        // SUI_ALIGNMENT_FLAG_VMIDDLE
        yoff = (layout.rect.h / 2) - halfh;
        if (flags & SUI_ALIGNMENT_FLAG_TOP) {
                yoff = 0;
        }
        else if (flags & SUI_ALIGNMENT_FLAG_BOTTOM) {
                yoff = layout.rect.h - h;
        }
        
        SuiRect rect(x + xoff, y + yoff, w, h);

        cmdrects.push(SuiCommandRect(layout.rect, style.colors[SUI_COLOR_RECT]));
        cmdrects.push(SuiCommandRect(rect, style.colors[SUI_COLOR_BOX]));
}

void SuiContext::reset()
{
        layouts.reset();
        cmdrects.reset();
}

// --------------------------------------------------------------------------------------------------------------------

#include <d3dcompiler.h>

SuiVertex::SuiVertex(const f32 _x, const f32 _y, const SuiColor _color) : x(_x), y(_y), color(_color) {}

SuiBackendProfiler::SuiBackendProfiler(ID3D11Device* device)
{
        SuiAssert(device);
        memset(this, 0, sizeof(SuiBackendProfiler));

        HRESULT          hr;
        D3D11_QUERY_DESC query_desc = {D3D11_QUERY_TIMESTAMP, 0};

        hr = device->CreateQuery(&query_desc, &timestamp0);
        SuiAssert(hr == 0);
        query_desc.Query = D3D11_QUERY_TIMESTAMP;
        hr = device->CreateQuery(&query_desc, &timestamp1);
        SuiAssert(hr == 0);
        query_desc.Query = D3D11_QUERY_TIMESTAMP_DISJOINT;
        hr = device->CreateQuery(&query_desc, &disjointed);
        SuiAssert(hr == 0);
        query_desc.Query = D3D11_QUERY_PIPELINE_STATISTICS;
        hr = device->CreateQuery(&query_desc, &pipeline_stats);
        SuiAssert(hr == 0);
        query_desc.Query = D3D11_QUERY_SO_STATISTICS;
        hr = device->CreateQuery(&query_desc, &so_stats);
        SuiAssert(hr == 0);
}

void SuiBackendProfiler::begin(ID3D11DeviceContext* context)
{
        context->Begin(disjointed);
        context->End(timestamp0);

        context->Begin(pipeline_stats);
        context->Begin(so_stats);
}

void SuiBackendProfiler::end(ID3D11DeviceContext* context)
{
        context->End(timestamp1);
        context->End(disjointed);

        context->End(pipeline_stats);
        context->End(so_stats);
}

void SuiBackendProfiler::update(ID3D11DeviceContext* context)
{
        // get data
        u64 time0;
        while (context->GetData(timestamp0, &time0, sizeof(u64), 0) != S_OK)
                ;
        u64 time1;
        while (context->GetData(timestamp1, &time1, sizeof(u64), 0) != S_OK)
                ;
        D3D11_QUERY_DATA_TIMESTAMP_DISJOINT disjoint_data;
        while (context->GetData(disjointed, &disjoint_data, sizeof(D3D11_QUERY_DATA_TIMESTAMP_DISJOINT), 0) != S_OK)
                ;
        D3D11_QUERY_DATA_PIPELINE_STATISTICS pipeline_stats_data;
        while (context->GetData(pipeline_stats, &pipeline_stats_data, sizeof(D3D11_QUERY_DATA_PIPELINE_STATISTICS), 0) != S_OK)
                ;
        D3D11_QUERY_DATA_SO_STATISTICS so_stats_data;
        while (context->GetData(so_stats, &so_stats_data, sizeof(D3D11_QUERY_DATA_SO_STATISTICS), 0) != S_OK)
                ;

        // update data
        d64 delta = static_cast<d64>(time1 - time0);
        if (disjoint_data.Disjoint == FALSE) {
                d64 frequency = static_cast<d64>(disjoint_data.Frequency);
                time = (delta / frequency) * 1000.0;
        }

        // D3D11_QUERY_DATA_PIPELINE_STATISTICS
        ia_vertices = pipeline_stats_data.IAVertices;
        min_ia_vertices = SuiMin(min_ia_vertices, pipeline_stats_data.IAVertices);
        max_ia_vertices = SuiMax(max_ia_vertices, pipeline_stats_data.IAVertices);

        ia_primitives = pipeline_stats_data.IAPrimitives;
        min_ia_primitives = SuiMin(min_ia_primitives, pipeline_stats_data.IAPrimitives);
        max_ia_primitives = SuiMax(max_ia_primitives, pipeline_stats_data.IAPrimitives);

        vs_invocations = pipeline_stats_data.VSInvocations;
        min_vs_invocations = SuiMin(min_vs_invocations, pipeline_stats_data.VSInvocations);
        max_vs_invocations = SuiMax(max_vs_invocations, pipeline_stats_data.VSInvocations);

        ps_invocations = pipeline_stats_data.PSInvocations;
        min_ps_invocations = SuiMin(min_ps_invocations, pipeline_stats_data.PSInvocations);
        max_ps_invocations = SuiMax(max_ps_invocations, pipeline_stats_data.PSInvocations);

        cs_invocations = pipeline_stats_data.CSInvocations;
        min_cs_invocations = SuiMin(min_cs_invocations, pipeline_stats_data.CSInvocations);
        max_cs_invocations = SuiMax(max_cs_invocations, pipeline_stats_data.CSInvocations);

        // D3D11_QUERY_DATA_SO_STATISTICS
        num_primitives_written = so_stats_data.NumPrimitivesWritten;
        min_num_primitives_written = SuiMin(min_num_primitives_written, num_primitives_written);
        max_num_primitives_written = SuiMax(max_num_primitives_written, num_primitives_written);

        primitives_storage_needed = so_stats_data.PrimitivesStorageNeeded;
        min_primitives_storage_needed = SuiMin(min_primitives_storage_needed, primitives_storage_needed);
        max_primitives_storage_needed = SuiMax(max_primitives_storage_needed, primitives_storage_needed);
}

SuiBackend::SuiBackend(ID3D11Device* _device, const i32 x, const i32 y) : profiler(SuiBackendProfiler(_device))
{
        screen_x = x;
        screen_y = y;

        device = _device;
        device->GetImmediateContext(&context);

        // TODO: error blob and compile flags
        HRESULT   hr;
        ID3DBlob* blob = NULL;
        hr = D3DCompileFromFile(L"shaders\\SuiVertex.hlsl", NULL, NULL, "main", "vs_4_0", 0, 0, &blob, NULL);
        SuiAssert(hr == 0);
        hr = device->CreateVertexShader((void*)blob->GetBufferPointer(), blob->GetBufferSize(), NULL, &vertex_shader);
        SuiAssert(hr == 0);

        D3D11_INPUT_ELEMENT_DESC ieds[] = {
            {"POSITION", 0, DXGI_FORMAT_R32G32_FLOAT,   0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
            {"COLOR",    0, DXGI_FORMAT_R8G8B8A8_UNORM, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
        };
        hr = device->CreateInputLayout(ieds, 2, blob->GetBufferPointer(), blob->GetBufferSize(), &input_layout);
        SuiAssert(hr == 0);

        hr = D3DCompileFromFile(L"shaders\\SuiPixel.hlsl", NULL, NULL, "main", "ps_4_0", 0, 0, &blob, NULL);
        SuiAssert(hr == 0);
        hr = device->CreatePixelShader(blob->GetBufferPointer(), blob->GetBufferSize(), NULL, &pixel_shader);
        SuiAssert(hr == 0);

        blob->Release();

        // blending
        D3D11_BLEND_DESC bdesc;
        memset(&bdesc, 0, sizeof(D3D11_BLEND_DESC));
        bdesc.RenderTarget[0].BlendEnable = 1;
        bdesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
        bdesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
        bdesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
        bdesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
        bdesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
        bdesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
        bdesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

        hr = device->CreateBlendState(&bdesc, &blend_state);
        SuiAssert(hr == 0);

        // buffers
        D3D11_BUFFER_DESC bufdsc;
        bufdsc.ByteWidth = sizeof(SuiVertex) * SUI_VERTEX_SIZE;
        bufdsc.Usage = D3D11_USAGE_DYNAMIC;
        bufdsc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
        bufdsc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
        bufdsc.MiscFlags = 0;
        bufdsc.StructureByteStride = 0;

        hr = device->CreateBuffer(&bufdsc, NULL, &vertex_buffer);
        SuiAssert(hr == 0);

        u32 indices[] = {0, 1, 2, 0, 3, 1};

        bufdsc.ByteWidth = sizeof(u32) * 6;
        bufdsc.Usage = D3D11_USAGE_DEFAULT;
        bufdsc.BindFlags = D3D11_BIND_INDEX_BUFFER;
        bufdsc.CPUAccessFlags = 0;
        bufdsc.MiscFlags = 0;
        bufdsc.StructureByteStride = 0;

        D3D11_SUBRESOURCE_DATA subrsc;
        subrsc.pSysMem = indices;
        subrsc.SysMemPitch = 0;
        subrsc.SysMemSlicePitch = 0;

        hr = device->CreateBuffer(&bufdsc, &subrsc, &index_buffer);
        SuiAssert(hr == 0);

        // orthographic projection matrix
        float L = 0;
        float R = (f32)x;
        float T = 0;
        float B = (f32)y;
        float proj[4][4] = {
            {2.0f / (R - L),    0.0f,              0.0f, 0.0f},
            {0.0f,              2.0f / (T - B),    0.0f, 0.0f},
            {0.0f,              0.0f,              0.5f, 0.0f},
            {(R + L) / (L - R), (T + B) / (B - T), 0.5f, 1.0f}
        };

        bufdsc.ByteWidth = sizeof(proj);
        bufdsc.Usage = D3D11_USAGE_DEFAULT;
        bufdsc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        bufdsc.CPUAccessFlags = 0;
        bufdsc.MiscFlags = 0;
        bufdsc.StructureByteStride = 0;

        subrsc.pSysMem = proj;
        subrsc.SysMemPitch = 0;
        subrsc.SysMemSlicePitch = 0;

        hr = device->CreateBuffer(&bufdsc, &subrsc, &constant_buffer);
        SuiAssert(hr == 0);
}

void SuiBackend::record(i32 n, const SuiCommandRect* cmdrects)
{
        SuiAssert(n * 4 < SUI_VERTEX_SIZE);

        HRESULT                  hr;
        D3D11_MAPPED_SUBRESOURCE vtx_rsc;
        hr = context->Map(vertex_buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &vtx_rsc);
        SuiAssert(hr == 0);

        // memcpy((char*)vtx_rsc.pData + n * sizeof(struct sui_vertex), vertices, n * sizeof(struct sui_vertex));
        SuiVertex* vertex = reinterpret_cast<SuiVertex*>(vtx_rsc.pData);
        for (i32 i = 0; i < n; i++) {
                f32 x0 = (f32)cmdrects[i].rect.x;
                f32 y0 = (f32)cmdrects[i].rect.y;
                f32 x1 = (f32)cmdrects[i].rect.x + (f32)cmdrects[i].rect.w;
                f32 y1 = (f32)cmdrects[i].rect.y + (f32)cmdrects[i].rect.h;
                *vertex++ = SuiVertex(x0, y0, cmdrects[i].color);
                *vertex++ = SuiVertex(x1, y1, cmdrects[i].color);
                *vertex++ = SuiVertex(x0, y1, cmdrects[i].color);
                *vertex++ = SuiVertex(x1, y0, cmdrects[i].color);
        }

        context->Unmap(vertex_buffer, 0);
        vertices_count = n * 4;
}

void SuiBackend::draw()
{
        profiler.begin(context);

        context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        context->IASetInputLayout(input_layout);
        context->VSSetShader(vertex_shader, NULL, 0);
        context->PSSetShader(pixel_shader, NULL, 0);
        context->IASetIndexBuffer(index_buffer, DXGI_FORMAT_R32_UINT, 0);
        context->VSSetConstantBuffers(0, 1, &constant_buffer);
        context->OMSetBlendState(blend_state, 0, 0xffffffff);

        u32 stride = sizeof(SuiVertex);
        u32 offset = 0;
        for (i32 i = 0; i < vertices_count; i += 4) {
                offset = i * stride;
                context->IASetVertexBuffers(0, 1, &vertex_buffer, &stride, &offset);
                context->DrawIndexed(6, 0, 0);
        }

        profiler.end(context);
        profiler.update(context);
}
