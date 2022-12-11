#include "sui.h"

#include <string.h>
#include <d3dcompiler.h>
#define HANDMADE_MATH_IMPLEMENTATION
#include "HandmadeMath.h"

void sui_init(struct sui_context* sui, ID3D11Device* d11device, i32 w, i32 h)
{
	memset(sui, 0, sizeof(struct sui_context));

	sui->vertices = (struct sui_vertex*)malloc(sizeof(struct sui_vertex) * 2048);
	sui->vlen = 0;

	sui->viewport = (struct sui_viewport){ w, h };

	/* sui->io.rclick_delta = CLICK_DELTA_RESET;
	sui->io.lclick_delta = CLICK_DELTA_RESET; */

	sui->d11device = d11device;
	ID3D11Device_GetImmediateContext(d11device, &sui->d11context);

	{
		HRESULT hr;

		const char* vs =
			"cbuffer ConstantBuffer : register(b0) {\
				float4x4 proj;\
			};\
			\
			struct VS_INPUT {\
				float2 position : POSITION0;\
				float4 color : COLOR0;\
			};\
			\
			struct VS_OUTPUT {\
				float4 color : COLOR0;\
				float4 position : SV_POSITION;\
			};\
			\
			VS_OUTPUT main(VS_INPUT vsin) {\
				VS_OUTPUT vsout;\
				vsout.position = mul(proj, float4(vsin.position, 0.0, 1.0));\
				vsout.color = vsin.color;\
				return vsout;\
			}";

		ID3DBlob* blob = NULL;
		hr = D3DCompile(
			vs, strlen(vs), NULL, NULL, NULL, "main",
			"vs_4_0", 0 /* deal with shader flags later */, 0, &blob, NULL
		);
		sui_assert(hr == 0);

		hr = ID3D11Device_CreateVertexShader(
			d11device,
			(void*)ID3D10Blob_GetBufferPointer(blob),
			ID3D10Blob_GetBufferSize(blob),
			NULL,
			&sui->d11vs
		);
		sui_assert(hr == 0);

		D3D11_INPUT_ELEMENT_DESC ieds[] = {
			{ 
				"POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 
				D3D11_APPEND_ALIGNED_ELEMENT, 
				D3D11_INPUT_PER_VERTEX_DATA, 0
			},
			{ 
				"COLOR", 0, DXGI_FORMAT_R8G8B8A8_UNORM, 0, 
				D3D11_APPEND_ALIGNED_ELEMENT, 
				D3D11_INPUT_PER_VERTEX_DATA, 0
			},
		};
		hr = ID3D11Device_CreateInputLayout(
			d11device, ieds, 2,
			ID3D10Blob_GetBufferPointer(blob),
			ID3D10Blob_GetBufferSize(blob),
			&sui->d11il
		);
		sui_assert(hr == 0);
		
		const char* ps =
			"float4 main(float4 color : COLOR0) : SV_TARGET {\
				return color;\
			}";

		hr = D3DCompile(
			ps, strlen(ps), NULL, NULL, NULL, "main",
			"ps_4_0", 0 /* deal with shader flags later */, 0, &blob, NULL
		);
		sui_assert(hr == 0);

		hr = ID3D11Device_CreatePixelShader(
			d11device,
			ID3D10Blob_GetBufferPointer(blob),
			ID3D10Blob_GetBufferSize(blob),
			NULL,
			&sui->d11ps
		);
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
		
		hr = ID3D11Device_CreateBlendState(d11device, &bdesc, &sui->d11bs);
		sui_assert(hr == 0);
	}

	{
		HRESULT hr;

		D3D11_BUFFER_DESC bufdsc;
		bufdsc.ByteWidth = sizeof(struct sui_vertex) * 2048;
		bufdsc.Usage = D3D11_USAGE_DYNAMIC;
		bufdsc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bufdsc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		bufdsc.MiscFlags = 0;
		bufdsc.StructureByteStride = 0;
		
		hr = ID3D11Device_CreateBuffer(d11device, &bufdsc, NULL, &sui->d11vb);
		sui_assert(hr == 0);

		u32 indices[] = { 0, 1, 2, 0, 3, 1 };

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

		hr = ID3D11Device_CreateBuffer(d11device, &bufdsc, &subrsc, &sui->d11ib);
		sui_assert(hr == 0);

		// orthographic projection matrix
		float L = 0;
		float R = sui->viewport.w;
		float T = 0;
		float B = sui->viewport.h;
		float proj[4][4] = {
			{ 2.0f/(R-L),  0.0f,        0.0f, 0.0f },
			{ 0.0f,        2.0f/(T-B),  0.0f, 0.0f },
			{ 0.0f,        0.0f,        0.5f, 0.0f },
			{ (R+L)/(L-R), (T+B)/(B-T), 0.5f, 1.0f }
		}; 

		// hmm_mat4 proj = HMM_Orthographic(L, R, B, T, 0.5f, 0.5f);
		bufdsc.ByteWidth = sizeof(proj);
		bufdsc.Usage = D3D11_USAGE_DEFAULT;
		bufdsc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		bufdsc.CPUAccessFlags = 0;
		bufdsc.MiscFlags = 0;
		bufdsc.StructureByteStride = 0;
		
		subrsc.pSysMem = proj;
		subrsc.SysMemPitch = 0;
		subrsc.SysMemSlicePitch = 0;

		hr = ID3D11Device_CreateBuffer(d11device, &bufdsc, &subrsc, &sui->d11cb);
		sui_assert(hr == 0);

	}
}

void sui_terminate(struct sui_context* sui)
{
	sui->d11device = NULL;
	sui->d11context = NULL;

	ID3D11VertexShader_Release(sui->d11vs);
	ID3D11InputLayout_Release(sui->d11il);
	ID3D11PixelShader_Release(sui->d11ps);

	ID3D11Buffer_Release(sui->d11vb);
	ID3D11Buffer_Release(sui->d11ib);
	ID3D11Buffer_Release(sui->d11cb);

	ID3D11BlendState_Release(sui->d11bs);

	sui->vlen = 0;
	free(sui->vertices);

	memset(sui, 0, sizeof(struct sui_context));
}

 /* void sui_input(struct sui_context* sui, i16 mx, i16 my, u8 rdown, u8 rup, u8 ldown, u8 lup)
{
	sui->io.dmx = sui->io.mx - mx;
	sui->io.dmy = sui->io.my - my;

	sui->io.mx = mx;
	sui->io.my = my;

	sui->io.rdown = rdown;
	sui->io.rup = rup;
	sui->io.ldown = ldown;
	sui->io.lup = lup;

	sui->io.rclick = 0;
	sui->io.lclick = 0;

	if (rdown) { 
		sui->io.rheld = 1; 
		sui->io.rclick_delta = sui_timer_begin(); 
	}
	else if (rup) { 
		sui->io.rheld = 0; 
		sui->io.rclick_delta = sui_timer_end(sui->io.rclick_delta);
	}
	if (sui->io.rclick_delta < 200000) { 
		sui->io.rclick = 1; 
		sui->io.rclick_delta = CLICK_DELTA_RESET;
	}

	if (ldown) { 
		sui->io.lheld = 1; 
		sui->io.lclick_delta = sui_timer_begin(); 
	}
	else if (lup) { 
		sui->io.lheld = 0; 
		sui->io.lclick_delta = sui_timer_end(sui->io.lclick_delta);
	}
	if (sui->io.lclick_delta < 200000) { 
		sui->io.lclick = 1; 
		sui->io.lclick_delta = CLICK_DELTA_RESET;
	}

}

void sui_begin(struct sui_context* sui, i16* x, i16* y)
{
	sui->window = (struct sui_window){ *x, *y, 0, 0, 0, sui->vlen, 4, 2, 0 };
	sui->vlen += 4;
	memset(sui->vertices + sui->window.vi, 0, sizeof(struct sui_window) * 4);
}

void sui_end(struct sui_context* sui)
{
	sui->window.w = max(sui->window.w, sui->window.max_w);
	sui_rect_insert(
		sui, sui->window.vi,
		sui->window.x - sui->window.pad,
		sui->window.y - sui->window.pad,
		sui->window.w + 2 * sui->window.pad,
		sui->window.h + 2 * sui->window.pad + sui->window.row * sui->window.pad,
		255, 255, 255, 255
	);

	memset(&sui->window, 0, sizeof(struct sui_window));
}

void sui_row(struct sui_context* sui)
{
	sui->window.max_w = sui_max(sui->window.w, sui->window.max_w);
	sui->window.row++;
	sui->window.w = 0;
	sui->window.h += 16;
}

i32 sui_button(struct sui_context* sui, const char* name)
{
	// assumed letter width and height is 16 px
	i32 slen = strlen(name);

	if (sui->window.w) sui->window.w += sui->window.child_margin;
	i16 btn_x = sui->window.x + sui->window.w;
	i16 btn_y = sui->window.y + sui->window.row * (16 + sui->window.pad);
	i16 btn_w = slen * (16 + 2) - 2;
	i16 btn_h = 16;

	u8 r = 255; u8 g = 0; u8 b = 0;
	i32 hover = sui_hover(&sui->io, btn_x, btn_y, btn_w, btn_h);
	if (hover) {
		r = 0;
		g = 255;
	}


	i16 xoff = sui->window.x + sui->window.w;
	for (i32 i = 0; i < slen; i++) {
		sui_rect_insert(
			sui, sui->vlen,
			xoff, btn_y, btn_h, btn_h,
			r, g, b, 255
		);
		xoff += 16 + 2;
		sui->vlen += 4;
	}
	
	// update window
	sui->window.w += slen * (16 + 2) - 2;
	if (!sui->window.h) sui->window.h += 16;

	if (hover && sui->io.lclick) return 1;
	return 0;
} */

void sui_test(struct sui_context* sui)
{
	struct sui_vertex* vertex = sui->vertices;	
	sui->vlen+=4;

	u8 r = 255;
	u8 g = 255;
	u8 b = 255;
	u8 a = 255;

	float x = 100;
	float y = 100;
	float w = 100;
	float h = 100;

	float tx = - (w / 2.0f);
	float ty = - (h / 2.0f);

	hmm_vec4 v0 = { tx,     ty     , 0.0f, 0.0f };
	hmm_vec4 v1 = { tx + w, ty + h , 0.0f, 0.0f };
	hmm_vec4 v2 = { tx,     ty + h , 0.0f, 0.0f };
	hmm_vec4 v3 = { tx + w, ty     , 0.0f, 0.0f };

	float scale = 1.0f;
	float rotation = 0.0f;

	hmm_mat4 rot = HMM_Rotate(rotation, (hmm_vec3){ 0.0f, 0.0f, 1.0f } );
	hmm_mat4 sca = HMM_Scale((hmm_vec3){ scale, scale, 0.0f });
	hmm_mat4 mat = HMM_MultiplyMat4(rot, sca);
	
	v0 = HMM_MultiplyMat4ByVec4(mat, v0);
	v1 = HMM_MultiplyMat4ByVec4(mat, v1);
	v2 = HMM_MultiplyMat4ByVec4(mat, v2);
	v3 = HMM_MultiplyMat4ByVec4(mat, v3);

	v0 = HMM_AddVec4(v0, (hmm_vec4){ x - tx, y - ty, 0.0f, 0.0f });
	v1 = HMM_AddVec4(v1, (hmm_vec4){ x - tx, y - ty, 0.0f, 0.0f });
	v2 = HMM_AddVec4(v2, (hmm_vec4){ x - tx, y - ty, 0.0f, 0.0f });
	v3 = HMM_AddVec4(v3, (hmm_vec4){ x - tx, y - ty, 0.0f, 0.0f });

	*vertex = (struct sui_vertex){ v0.X, v0.Y, r, g, b, a };
	vertex++;
	*vertex = (struct sui_vertex){ v1.X, v1.Y, r, g, b, a };
	vertex++;
	*vertex = (struct sui_vertex){ v2.X, v2.Y, r, g, b, a };
	vertex++;
	*vertex = (struct sui_vertex){ v3.X, v3.Y, r, g, b, a };
}

void sui_render(struct sui_context* sui)
{
	HRESULT hr;
	D3D11_MAPPED_SUBRESOURCE vtx_rsc;
	hr = ID3D11DeviceContext_Map(
		sui->d11context, sui->d11vb, 0, D3D11_MAP_WRITE_DISCARD, 0, &vtx_rsc
	);
	sui_assert(hr == 0);
	memcpy(vtx_rsc.pData, sui->vertices, sui->vlen * sizeof(struct sui_vertex));
	ID3D11DeviceContext_Unmap(sui->d11context, sui->d11vb, 0);

	ID3D11DeviceContext_IASetPrimitiveTopology(
		sui->d11context, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST
	);
	ID3D11DeviceContext_IASetInputLayout(sui->d11context, sui->d11il);
	ID3D11DeviceContext_VSSetShader(sui->d11context, sui->d11vs, NULL, 0);
	ID3D11DeviceContext_PSSetShader(sui->d11context, sui->d11ps, NULL, 0);
	ID3D11DeviceContext_IASetIndexBuffer(
		sui->d11context, sui->d11ib, DXGI_FORMAT_R32_UINT, 0
	);
	ID3D11DeviceContext_VSSetConstantBuffers(sui->d11context, 0, 1, &sui->d11cb);
	ID3D11DeviceContext_OMSetBlendState(sui->d11context, sui->d11bs, 0, 0xffffffff);

	u32 stride = sizeof(struct sui_vertex);
	u32 offset = 0;
	for (i32 i = 0; i < sui->vlen; i += 4) {
		offset = i * stride;
		ID3D11DeviceContext_IASetVertexBuffers(
			sui->d11context, 0, 1, &sui->d11vb, &stride, &offset
		);
		ID3D11DeviceContext_DrawIndexed(sui->d11context, 6, 0, 0);
	}
	// memset(sui->vertices, 0, sizeof(struct sui_vertex) * sui->vlen);
	sui->vlen = 0;
}

/* void sui_rect_insert(
	struct sui_context* sui, i32 vi, 
	i16 x, i16 y, i16 w, i16 h, 
	u8 r, u8 g, u8 b, u8 a)
{
	// get vertex
	struct sui_vertex* vertex = sui->vertices + vi;	

	// convert position to d11
	x -= (sui->viewport.w / 2);
	y -= (sui->viewport.h / 2);
	f32 xdiv =  (sui->viewport.w / 2.0f);
	f32 ydiv = -(sui->viewport.h / 2.0f);
	
	// convert to vertex points
	i16 x0 = x;
	i16 x1 = x + w;
	i16 y0 = y;
	i16 y1 = y + h;

	// create vertices
	*vertex = (struct sui_vertex) { x0/xdiv, y0/ydiv, r, g, b, a };
	vertex++;
	*vertex = (struct sui_vertex) { x1/xdiv, y1/ydiv, r, g, b, a };
	vertex++;
	*vertex = (struct sui_vertex) { x0/xdiv, y1/ydiv, r, g, b, a };
	vertex++;
	*vertex = (struct sui_vertex) { x1/xdiv, y0/ydiv, r, g, b, a };
	// vertex++;
}

i32 sui_hover(struct sui_io* io, i16 x, i16 y, i16 w, i16 h)
{
	i16 x0 = x;
	i16 x1 = x + w;
	i16 y0 = y;
	i16 y1 = y + h;

	if (x0 < io->mx && io->mx < x1 &&
	    y0 < io->my && io->my < y1) {
		return 1;
	}

	return 0;
}

i64 Frequency = 0;
// QueryPerformanceFrequency(&Frequency);

i64 sui_timer_begin()
{
	if (!Frequency) QueryPerformanceFrequency(&Frequency);
	i64 t;
	QueryPerformanceCounter(&t);
	return t;
}

i64 sui_timer_end(i64 begin)
{
	i64 t;
	QueryPerformanceCounter(&t);
	t -= begin;
	t *= 1000000;
	t /= Frequency;
	return t;
} */
