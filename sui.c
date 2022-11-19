#include "sui.h"

#include <string.h>
#include <d3dcompiler.h>


void sui_init_fixed(struct sui_context* sui, ID3D11Device* d11device, i32 w, i32 h)
{
	// memset(context, 0, sizeof(struct sui_context);
	sui->d11device = d11device;
	ID3D11Device_GetImmediateContext(d11device, &sui->d11context);

	{
		HRESULT hr;

		const char d11shaders[] = {
			"struct VS_INPUT {						\n"
			"	float2 position : POSITION0;				\n"
			"	float4 color : COLOR0;					\n"
			"};								\n"
			"								\n"
			"struct VS_OUTPUT {						\n"
			"	float4 color : COLOR0;					\n"
			"	float4 position : SV_POSITION;				\n"
			"};								\n"
			"								\n"
			"VS_OUTPUT vs_main(VS_INPUT vsin) {				\n"
			"	VS_OUTPUT vsout;					\n"
			"	vsout.position = float4(vsin.position, 0.0f, 1.0f);	\n"
			"	vsout.color = vsin.color;				\n"
			"	return vsout;						\n"
			"}								\n"
			"								\n"
			"float4 ps_main(float4 color : COLOR0) : SV_TARGET {		\n"
			"	return color;						\n"
			"}								\n"
		};

		ID3DBlob* blob = NULL;
		hr = D3DCompile(
			d11shaders, sizeof(d11shaders)-1, NULL, NULL, NULL, "vs_main", 
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

		hr = D3DCompile(
			d11shaders, sizeof(d11shaders)-1, NULL, NULL, NULL, "ps_main", 
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
	}

	sui->screen_w = w;
	sui->screen_h = h;

	sui->len = 0;
	sui->vertices = (struct sui_vertex*)malloc(sizeof(struct sui_vertex) * 2048);
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

	sui->len = 0;
	free(sui->vertices);

	memset(sui, 0, sizeof(struct sui_context));
}
void sui_mouse_input(struct sui_context* sui, i32 x, i32 y)
{
	// update states
	sui->mx = x;
	sui->my = y;
}

void sui_begin(struct sui_context* sui, struct sui_rect* rect)
{
	// preparing variables
	const i32 hh = 12;
	i32 x = rect->x - 300;
	i32 y = rect->y - 300;

	i32 header_x0 = x;
	i32 header_x1 = x + rect->w;
	i32 header_y0 = y;
	i32 header_y1 = y + hh;
	
	i32 body_x0 = x;
	i32 body_x1 = x + rect->w;
	i32 body_y0 = y + hh;
	i32 body_y1 = y + rect->h + hh;

	// creating vertices
	struct sui_vertex* vertex = sui->vertices + sui->len;
	{
		const f32 swv =   sui->screen_w / 2.0f;
		const f32 shv = -(sui->screen_h / 2.0f);

		// header
		u8 hr = 0;
		u8 hg = 0;
		u8 hb = 0;

		*vertex = (struct sui_vertex){
			header_x0 / swv, header_y0 / shv, hr, hg, hb, 255
		};
		vertex++;

		*vertex = (struct sui_vertex){
			header_x1 / swv, header_y1 / shv, hr, hg, hb, 255
		};
		vertex++;
		
		*vertex = (struct sui_vertex){
			header_x0 / swv, header_y1 / shv, hr, hg, hb, 255
		};
		vertex++;

		*vertex = (struct sui_vertex){
			header_x1 / swv, header_y0 / shv, hr, hg, hb, 255
		};
		vertex++;

		// body
		u8 br = 150;
		u8 bg = 150;
		u8 bb = 150;

		*vertex = (struct sui_vertex){ 
			body_x0 / swv, body_y0 / shv, br, bg, bb, 255 
		};
		vertex++;

		*vertex = (struct sui_vertex){ 
			body_x1 / swv, body_y1 / shv, br, bg, bb, 255 
		};
		vertex++;
		
		*vertex = (struct sui_vertex){ 
			body_x0 / swv, body_y1 / shv, br, bg, bb, 255 
		};
		vertex++;

		*vertex = (struct sui_vertex){ 
			body_x1 / swv, body_y0 / shv, br, bg, bb, 255 
		};

	}
	
	sui->len += 8;
}

void sui_end(struct sui_context* sui)
{
	HRESULT hr;
	D3D11_MAPPED_SUBRESOURCE vrsc;
	hr = ID3D11DeviceContext_Map(
		sui->d11context, sui->d11vb, 0, D3D11_MAP_WRITE_DISCARD, 0, &vrsc
	);
	sui_assert(hr == 0);
	memcpy(vrsc.pData, sui->vertices, sizeof(struct sui_vertex) * sui->len);
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
	
	u32 stride = sizeof(struct sui_vertex);
	for (i32 i = 0; i < sui->len; i += 4) {
		u32 offset = stride * i;
		ID3D11DeviceContext_IASetVertexBuffers(
			sui->d11context, 0, 1, &sui->d11vb, &stride, &offset
		);
		ID3D11DeviceContext_DrawIndexed(sui->d11context, 6, 0, 0);
	}

	memset(sui->vertices, 0, sizeof(struct sui_vertex) * sui->len);
	sui->len = 0;
}
