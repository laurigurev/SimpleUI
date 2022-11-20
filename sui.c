#include "sui.h"

#include <string.h>
#include <d3dcompiler.h>

void sui_init(struct sui_context* sui, ID3D11Device* d11device, i32 w, i32 h)
{
	sui->current_window = NULL;
	sui->voff = -1;
	sui->current_row = -1;
	sui->window_border = 0;
	sui->window_title_bar_h = 0;
	sui->window_child_h = 0;

	sui->widgets = (struct sui_widget*)malloc(sizeof(struct sui_widget) * 128);
	sui->wlen = 0;
	sui->p_windows = (struct sui_widget**)malloc(sizeof(struct sui_widget*) * 32);
	memset(sui->p_windows, 0, sizeof(struct sui_widget*) * 32);

	sui->vertices = (struct sui_vertex*)malloc(sizeof(struct sui_vertex) * 2048);
	sui->vlen = 0;

	sui->viewport = (struct sui_viewport) { w, h };
	sui->input_state = (struct sui_input_state) {
		-1, -1, 0, 200, 0, 0, 0, 0, 0, 0, 0, 0
	};

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
}

void sui_terminate(struct sui_context* sui)
{
	// NOT UP TO DATE
	sui->d11device = NULL;
	sui->d11context = NULL;

	ID3D11VertexShader_Release(sui->d11vs);
	ID3D11InputLayout_Release(sui->d11il);
	ID3D11PixelShader_Release(sui->d11ps);

	ID3D11Buffer_Release(sui->d11vb);
	ID3D11Buffer_Release(sui->d11ib);

	sui->vlen = 0;
	free(sui->vertices);

	memset(sui, 0, sizeof(struct sui_context));
}

void sui_inputs(struct sui_context* sui)
{
	sui_todo("sui_inputs() function");
}

void sui_begin(struct sui_context* sui, i32 i, i16 x, i16 y, i16 w, i16 h)
{
	// get index;
	// get window
	struct sui_widget* window = sui->p_windows[i];

	// if null, create window
	if (window == NULL) {
		window = sui->widgets + sui->wlen;
		sui->wlen++;
		window->type = SUI_WIDGET_TYPE_WINDOW;
		window->x = x;
		window->y = y;
		window->w = w;
		window->h = h;
		sui->p_windows[i] = window;
	}

	// update sui_context and window
	sui->current_window = window;
	sui->voff = sui->vlen;
	sui->current_row = 0;
	sui->window_border = 2;
	sui->window_title_bar_h = 16;
	sui->window_child_h = 16;

	sui->vlen += (4 * 4);
}

void sui_end(struct sui_context* sui)
{
	sui_assert(sui->current_window);
	sui_assert(sui->voff > (-1));

	// init variables
	struct sui_widget window = *sui->current_window;
	i16 x = window.x;
	i16 y = window.y;
	i16 w = window.w;
	i16 h = window.h;

	// update or add vertices
	x -= (sui->viewport.w / 2);
	y -= (sui->viewport.h / 2);
	f32 xdiv =  (sui->viewport.w / 2.0f);
	f32 ydiv = -(sui->viewport.h / 2.0f);
	struct sui_vertex* vertex = sui->vertices + sui->voff;	

	// border
	i16 border = sui->window_border;
	i16 title_bar_h = sui->window_title_bar_h;
	i16 x0 = x - border;
	i16 x1 = x + w + border;
	i16 y0 = y - border;
	i16 y1 = y + h + border + title_bar_h;
	u8 rgb = 255; u8 a = 255;
	*vertex = (struct sui_vertex) { x0/xdiv, y0/ydiv, rgb, rgb, rgb, a };
	vertex++;
	*vertex = (struct sui_vertex) { x1/xdiv, y1/ydiv, rgb, rgb, rgb, a };
	vertex++;
	*vertex = (struct sui_vertex) { x0/xdiv, y1/ydiv, rgb, rgb, rgb, a };
	vertex++;
	*vertex = (struct sui_vertex) { x1/xdiv, y0/ydiv, rgb, rgb, rgb, a };
	vertex++;

	// title bar
	x0 = x;
	x1 = x + w;
	y0 = y;
	y1 = y + title_bar_h;
	rgb = 0;
	*vertex = (struct sui_vertex) { x0/xdiv, y0/ydiv, rgb, rgb, rgb, a };
	vertex++;
	*vertex = (struct sui_vertex) { x1/xdiv, y1/ydiv, rgb, rgb, rgb, a };
	vertex++;
	*vertex = (struct sui_vertex) { x0/xdiv, y1/ydiv, rgb, rgb, rgb, a };
	vertex++;
	*vertex = (struct sui_vertex) { x1/xdiv, y0/ydiv, rgb, rgb, rgb, a };
	vertex++;

	// minimize
	i16 minimize_pad = 2;
	i16 minimize_w = 12;
	x0 = x + w - minimize_pad - minimize_w;
	x1 = x + w - minimize_pad;
	y0 = y + minimize_pad;
	y1 = y + minimize_pad + minimize_w;
	rgb = 255;
	*vertex = (struct sui_vertex) { x0/xdiv, y0/ydiv, rgb, rgb, rgb, a };
	vertex++;
	*vertex = (struct sui_vertex) { x1/xdiv, y1/ydiv, rgb, rgb, rgb, a };
	vertex++;
	*vertex = (struct sui_vertex) { x0/xdiv, y1/ydiv, rgb, rgb, rgb, a };
	vertex++;
	*vertex = (struct sui_vertex) { x1/xdiv, y0/ydiv, rgb, rgb, rgb, a };
	vertex++;

	// body
	x0 = x;
	x1 = x + w;
	y0 = y + title_bar_h;
	y1 = y + h + title_bar_h;
	rgb = 90;
	*vertex = (struct sui_vertex) { x0/xdiv, y0/ydiv, rgb, rgb, rgb, a };
	vertex++;
	*vertex = (struct sui_vertex) { x1/xdiv, y1/ydiv, rgb, rgb, rgb, a };
	vertex++;
	*vertex = (struct sui_vertex) { x0/xdiv, y1/ydiv, rgb, rgb, rgb, a };
	vertex++;
	*vertex = (struct sui_vertex) { x1/xdiv, y0/ydiv, rgb, rgb, rgb, a };
	vertex++;
	
	//cleanup
	sui->current_window = NULL;
	sui->voff = -1;
}

i32 sui_button(struct sui_context* sui)
{
	// TODO
	//  - add button to widgets
	//  - save current state of a window to somekind of handler
	
	sui_assert(sui->current_window);
	sui_assert(sui->voff > (-1));

	// init variables
	struct sui_widget window = *sui->current_window;
	i16 child_pad = 3;
	i16 x = window.x + sui->window_border + child_pad;
	i16 y = window.y + sui->window_border + sui->window_title_bar_h + child_pad;
	y += (sui->current_row * (child_pad + sui->window_child_h));
	i16 w = 70;
	i16 h = sui->window_child_h;

	// update or add vertices
	x -= (sui->viewport.w / 2);
	y -= (sui->viewport.h / 2);
	f32 xdiv =  (sui->viewport.w / 2.0f);
	f32 ydiv = -(sui->viewport.h / 2.0f);
	struct sui_vertex* vertex = sui->vertices + sui->vlen;

	// button
	i16 x0 = x;
	i16 x1 = x + w;
	i16 y0 = y;
	i16 y1 = y + h;
	u8 r = 0;
	u8 g = 10;
	u8 b = 70;
	u8 a = 255;
	*vertex = (struct sui_vertex) { x0/xdiv, y0/ydiv, r, g, b, a };
	vertex++; sui->vlen++;
	*vertex = (struct sui_vertex) { x1/xdiv, y1/ydiv, r, g, b, a };
	vertex++; sui->vlen++;
	*vertex = (struct sui_vertex) { x0/xdiv, y1/ydiv, r, g, b, a };
	vertex++; sui->vlen++;
	*vertex = (struct sui_vertex) { x1/xdiv, y0/ydiv, r, g, b, a };
	vertex++; sui->vlen++;

	return 0;
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

	u32 stride = sizeof(struct sui_vertex);
	u32 offset = 0;
	for (i32 i = 0; i < sui->vlen; i += 4) {
		offset = i * stride;
		ID3D11DeviceContext_IASetVertexBuffers(
			sui->d11context, 0, 1, &sui->d11vb, &stride, &offset
		);
		ID3D11DeviceContext_DrawIndexed(sui->d11context, 6, 0, 0);
	}
	memset(sui->vertices, 0, sizeof(struct sui_vertex) * sui->vlen);
	sui->vlen = 0;
}
