#include "sui.hpp"
#include <stdio.h>

SuiColor::SuiColor(const u8 _r, const u8 _g, const u8 _b, const u8 _a) : r(_r), g(_g), b(_b), a(_a) {}
SuiRect::SuiRect(const i32 _x, const i32 _y, const i32 _w, const i32 _h) : x(_x), y(_y), w(_w), h(_h) {}
SuiStyle::SuiStyle(const i32 _spacing, const SuiColor windowbg, const SuiColor rect, const SuiColor recthover, const SuiColor rectfocus)
    : spacing(_spacing), colors{windowbg, rect, recthover, rectfocus}
{
}
SuiLayout::SuiLayout(const SuiRect _rect, const SuiRect _body) : rect(_rect), body(_body) {}
SuiCommandRect::SuiCommandRect(const SuiRect _rect, const SuiColor _color) : rect(_rect), color(_color) {}

SuiContext::SuiContext() : style(2, SuiColor(255, 255, 255, 255), SuiColor(100, 0, 0, 255), SuiColor(255, 0, 0, 255), SuiColor(150, 0, 0, 255)) {}

void SuiContext::begin(const char* name, const SuiRect rect)
{
        const i32 spacing = style.spacing;
        SuiRect   body = {rect.x + spacing, rect.y + spacing, rect.w - spacing, rect.h - spacing};
        layouts.push(SuiLayout(rect, body));
        cmdrects.push(SuiCommandRect(rect, style.colors[SUI_COLOR_WINDOWBG]));
}

void SuiContext::end()
{
        layouts.pop();
}

void SuiContext::row(const i32 n, const i32* widths, const i32 height)
{
        SuiAssert(n < 16);
        SuiAssert(widths);

        // get current layout
        SuiLayout current_layout = layouts.get();

        // create new layouts
        i32 x = current_layout.body.x;
        i32 y = current_layout.body.y;
        i32 sumw = 0;
        // TODO: create define variable for this
        SuiLayout new_layouts[16];
        for (i32 i = 0; i < n; i++) {
                SuiRect rect = {x + sumw, y + style.spacing, widths[i], height};
                SuiRect body = {x + sumw, y + style.spacing, widths[i], height};
                sumw += widths[i] + style.spacing;
                new_layouts[i] = SuiLayout(rect, body);
        }

        // update current layout
        current_layout.body = {x, y + height + style.spacing, current_layout.rect.w, current_layout.rect.h};
        layouts.pop();
        layouts.push(current_layout);

        // push new layouts
        for (i32 i = n - 1; 0 <= i; i--) layouts.push(new_layouts[i]);
}

void SuiContext::rect()
{
        SuiLayout layout = layouts.get();
        layouts.pop();
        cmdrects.push(SuiCommandRect(layout.body, style.colors[SUI_COLOR_RECT]));
}

// --------------------------------------------------------------------------------------------------------------------

#include <d3dcompiler.h>

SuiVertex::SuiVertex(const f32 _x, const f32 _y, const SuiColor _color) : x(_x), y(_y), color(_color) {}

SuiBackend::SuiBackend(ID3D11Device* _device, const i32 x, const i32 y)
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
}
