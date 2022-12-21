#include "sui_backend.h"
#include "sui_defines.h"
#include <d3dcompiler.h>
#include <stdio.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

void sui_backend_init(struct sui_backend* backend, ID3D11Device* device, i32 w, i32 h)
{
        backend->device = device;
        ID3D11Device_GetImmediateContext(device, &backend->context);

        HRESULT   hr;
        FILE*     fstream;
        ID3DBlob* blob = NULL;
        char      buffer[4096];
        memset(buffer, 0, 4096);

        sui_assert(fopen_s(&fstream, "shaders\\sui_vertex.txt", "r") == 0);
        // TODO: fread_s
        fread(buffer, 1, 4096, fstream);
        hr = D3DCompile(buffer, strlen(buffer), NULL, NULL, NULL, "main", "vs_4_0",
                        0 /* deal with shader flags later */, 0, &blob, NULL);
        sui_assert(hr == 0);

        hr = ID3D11Device_CreateVertexShader(device, (void*)ID3D10Blob_GetBufferPointer(blob),
                                             ID3D10Blob_GetBufferSize(blob), NULL, &backend->vs);
        sui_assert(hr == 0);

        D3D11_INPUT_ELEMENT_DESC ieds[] = {
            {"POSITION", 0, DXGI_FORMAT_R32G32_FLOAT,   0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
            {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,   0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
            {"COLOR",    0, DXGI_FORMAT_R8G8B8A8_UNORM, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
        };
        hr = ID3D11Device_CreateInputLayout(device, ieds, 3, ID3D10Blob_GetBufferPointer(blob),
                                            ID3D10Blob_GetBufferSize(blob), &backend->il);
        sui_assert(hr == 0);

        memset(buffer, 0, 4096);
        sui_assert(fopen_s(&fstream, "shaders\\sui_pixel.txt", "r") == 0);
        fread(buffer, 1, 4096, fstream);
        hr = D3DCompile(buffer, strlen(buffer), NULL, NULL, NULL, "main", "ps_4_0",
                        0 /* deal with shader flags later */, 0, &blob, NULL);
        sui_assert(hr == 0);

        hr = ID3D11Device_CreatePixelShader(device, ID3D10Blob_GetBufferPointer(blob), ID3D10Blob_GetBufferSize(blob),
                                            NULL, &backend->ps);
        sui_assert(hr == 0);

        ID3D10Blob_Release(blob);

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

        hr = ID3D11Device_CreateBlendState(device, &bdesc, &backend->bs);
        sui_assert(hr == 0);

        // font atlas
        i32         in;
        const char* file = "Fixedsys.png";
        u8*         bmp = stbi_load(file, &backend->iw, &backend->ih, &in, 0);

        D3D11_TEXTURE2D_DESC texture_desc;
        texture_desc.Width = backend->iw;
        texture_desc.Height = backend->ih;
        texture_desc.MipLevels = 1;
        texture_desc.ArraySize = 1;
        texture_desc.Format = DXGI_FORMAT_R8_UNORM;
        texture_desc.SampleDesc = (DXGI_SAMPLE_DESC){1, 0};
        texture_desc.Usage = D3D11_USAGE_DEFAULT;
        texture_desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
        texture_desc.CPUAccessFlags = 0;
        texture_desc.MiscFlags = 0;

        D3D11_SUBRESOURCE_DATA sub_data;
        sub_data.pSysMem = bmp;
        sub_data.SysMemPitch = backend->iw;
        sub_data.SysMemSlicePitch = 0;

        ID3D11Texture2D* texture;
        hr = ID3D11Device_CreateTexture2D(device, &texture_desc, &sub_data, &texture);
        sui_assert(hr == 0);

        D3D11_SHADER_RESOURCE_VIEW_DESC srvd;
        srvd.Format = texture_desc.Format;
        srvd.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
        srvd.Texture2D = (D3D11_TEX2D_SRV){0, 1};

        hr = ID3D11Device_CreateShaderResourceView(device, texture, &srvd, &backend->view);
        sui_assert(hr == 0);

        D3D11_SAMPLER_DESC sampler_desc;
        memset(&sampler_desc, 0, sizeof(D3D11_SAMPLER_DESC));
        sampler_desc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
        sampler_desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
        sampler_desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
        sampler_desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
        // sampler_desc.MipLODBias = 0;
        // sampler_desc.MaxAnisotropy = 0;
        // sampler_desc.ComparisonFunc = 0;
        // sampler_desc.BorderColor = 0;
        // sampler_desc.MinLOD;
        // sampler_desc.MaxLOD;

        hr = ID3D11Device_CreateSamplerState(device, &sampler_desc, &backend->sampler);
        sui_assert(hr == 0);

        ID3D11Texture2D_Release(texture);
        stbi_image_free(bmp);
        
        D3D11_BUFFER_DESC bufdsc;
        bufdsc.ByteWidth = sizeof(struct sui_vertex) * 2048;
        bufdsc.Usage = D3D11_USAGE_DYNAMIC;
        bufdsc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
        bufdsc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
        bufdsc.MiscFlags = 0;
        bufdsc.StructureByteStride = 0;

        hr = ID3D11Device_CreateBuffer(device, &bufdsc, NULL, &backend->vb);
        sui_assert(hr == 0);

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

        hr = ID3D11Device_CreateBuffer(device, &bufdsc, &subrsc, &backend->ib);
        sui_assert(hr == 0);

        // orthographic projection matrix
        float L = 0;
        float R = w;
        float T = 0;
        float B = h;
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

        hr = ID3D11Device_CreateBuffer(device, &bufdsc, &subrsc, &backend->cb);
        sui_assert(hr == 0);
        backend->n = 0;
}

void sui_backend_free(struct sui_backend* backend)
{
        backend->device = NULL;
        backend->context = NULL;

        ID3D11VertexShader_Release(backend->vs);
        ID3D11InputLayout_Release(backend->il);
        ID3D11PixelShader_Release(backend->ps);

        ID3D11Buffer_Release(backend->vb);
        ID3D11Buffer_Release(backend->ib);
        ID3D11Buffer_Release(backend->cb);

        ID3D11BlendState_Release(backend->bs);
        ID3D11ShaderResourceView_Release(backend->view);
        ID3D11SamplerState_Release(backend->sampler);

        memset(backend, 0, sizeof(struct sui_backend));
}

void sui_backend_push_vertices(struct sui_backend* backend, i32 n, struct sui_vertex* vertices)
{
        HRESULT                  hr;
        D3D11_MAPPED_SUBRESOURCE vtx_rsc;
        hr = ID3D11DeviceContext_Map(backend->context, backend->vb, 0, D3D11_MAP_WRITE_DISCARD, 0, &vtx_rsc);
        sui_assert(hr == 0);
        memcpy(vtx_rsc.pData + n * sizeof(struct sui_vertex), vertices, n * sizeof(struct sui_vertex));
        ID3D11DeviceContext_Unmap(backend->context, backend->vb, 0);
        backend->n += n;
}

void sui_backend_draw(struct sui_backend* backend)
{
        ID3D11DeviceContext_IASetPrimitiveTopology(backend->context, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        ID3D11DeviceContext_IASetInputLayout(backend->context, backend->il);
        ID3D11DeviceContext_VSSetShader(backend->context, backend->vs, NULL, 0);
        ID3D11DeviceContext_PSSetShader(backend->context, backend->ps, NULL, 0);
        ID3D11DeviceContext_IASetIndexBuffer(backend->context, backend->ib, DXGI_FORMAT_R32_UINT, 0);
        ID3D11DeviceContext_VSSetConstantBuffers(backend->context, 0, 1, &backend->cb);
        ID3D11DeviceContext_OMSetBlendState(backend->context, backend->bs, 0, 0xffffffff);
        ID3D11DeviceContext_PSSetShaderResources(backend->context, 0, 1, &backend->view);
        ID3D11DeviceContext_PSSetSamplers(backend->context, 0, 1, &backend->sampler);

        u32 stride = sizeof(struct sui_vertex);
        u32 offset = 0;
        for (i32 i = 0; i < backend->n; i += 4) {
                offset = i * stride;
                ID3D11DeviceContext_IASetVertexBuffers(backend->context, 0, 1, &backend->vb, &stride, &offset);
                ID3D11DeviceContext_DrawIndexed(backend->context, 6, 0, 0);
        }

        backend->n = 0;
}
