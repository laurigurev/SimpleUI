#include "sui.h"

#include <stdio.h>
#include <assert.h>

#define WIN32_MEAN_AND_LEAN
#define COBJMACROS
#include <d3d11.h>
#include <windows.h>

// TODO(begin)
struct ex_window;
struct ex_gfx_context;

struct mouse_t {
	i32 x;
	i32 y;
	u8 ldown;
	u8 lup;
	u8 rdown;
	u8 rup;
};

void ex_window_init();
void ex_window_terminate();

void ex_gdx_context_init();
void ex_gdx_context_terminate();
// TODO(end)

LRESULT CALLBACK WindowProc0(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
LRESULT CALLBACK WindowProc1(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

int main()
{
	puts("This is example application using d11 and winapi");

	// --- Win32 window init
	
	WNDCLASSEX wc;
	HINSTANCE hInstance;
	HWND hwnd;
	struct mouse_t mouse;

	{
		const i32 x = 60;
		const i32 y = 60;
		const i32 w = 600;
		const i32 h = 600;

		const wchar_t class_name[] = L"SimpleUI";
		hInstance = GetModuleHandle(NULL);
		
		RECT rect = { 0, 0, w, h };
		AdjustWindowRect(
			&rect, WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU, 0
		);

		wc.cbSize = sizeof(wc);
		wc.style = CS_OWNDC;
		wc.lpfnWndProc = WindowProc0;
		wc.cbClsExtra = 0;
		wc.cbWndExtra = 0;
		wc.hInstance = hInstance;
		wc.hIcon = NULL;
		wc.hCursor = NULL;
		wc.hbrBackground = NULL;
		wc.lpszMenuName = NULL;
		wc.lpszClassName = class_name;
		wc.hIconSm = NULL;

		if (!RegisterClassEx(&wc)) {
			return 1;
		}

		hwnd = CreateWindowEx(
			0,
			class_name,
			L"SimpleUI example",
			WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU,
			x, y,
			rect.right - rect.left, rect.bottom - rect.top,
			NULL,
			NULL,
			hInstance,
			&mouse
		);

		if (!hwnd) {
			return 1;
		}

		ShowWindow(hwnd, SW_SHOW);
	}

	// --- D3D11 init

	ID3D11Device* device;
	ID3D11DeviceContext* context;
	IDXGISwapChain* swapchain;
	ID3D11RenderTargetView* target;

	{
		HRESULT hr;

		DXGI_MODE_DESC buffer_descriptor = {
			0, 0, { 0, 0 },
			DXGI_FORMAT_B8G8R8A8_UNORM,
			DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED,
			DXGI_MODE_SCALING_UNSPECIFIED
		};

		DXGI_SWAP_CHAIN_DESC swapchain_descriptor = {
			buffer_descriptor, 1, 0,
			DXGI_USAGE_RENDER_TARGET_OUTPUT,
			1, hwnd, TRUE,
			DXGI_SWAP_EFFECT_DISCARD, 0
		};

		hr = D3D11CreateDeviceAndSwapChain(
			NULL,
			D3D_DRIVER_TYPE_HARDWARE,
			NULL,
			D3D11_CREATE_DEVICE_SINGLETHREADED | 
			D3D11_CREATE_DEVICE_DEBUG,
			NULL,
			0,
			D3D11_SDK_VERSION,
			&swapchain_descriptor,
			&swapchain,
			&device,
			NULL,
			&context
		);
		assert(hr == 0);

		ID3D11Texture2D* backbuffer = NULL;
		IDXGISwapChain_GetBuffer(
			swapchain, 0, &IID_ID3D11Texture2D, 
			(void**)&backbuffer
		);
		hr = ID3D11Device_CreateRenderTargetView(
			device,
			(ID3D11Resource*)backbuffer,
			NULL,
			&target
		);
		assert(hr == 0);
		ID3D11Texture2D_Release(backbuffer);	

		RECT wrect;
		GetClientRect(hwnd, &wrect);
		D3D11_VIEWPORT view = {
			0.0f, 0.0f,
			(float)(wrect.right - wrect.left),
			(float)(wrect.bottom - wrect.top),
			0.0f, 1.0f
		};
		ID3D11DeviceContext_RSSetViewports(context, 1, &view);
		ID3D11DeviceContext_OMSetRenderTargets(
			context, 1, &target, NULL
		);
	}

	struct sui_context sui;
	sui_init(&sui, device, 600, 600);
	char ws[] = "window";
	struct sui_window swin0 = sui_create_window(ws, 
						   (struct sui_color){ 255, 255, 255, 255 }, 
						   (struct sui_color){ 230, 230, 230, 255 }, 
						   (struct sui_color){ 255, 255, 255, 255 }, 
						   (struct sui_color){ 200, 200, 200, 255 }
	);
	struct sui_window swin1 = sui_create_window(ws, 
						   (struct sui_color){ 255, 255, 255, 255 }, 
						   (struct sui_color){ 100, 100, 255, 255 }, 
						   (struct sui_color){ 255, 255, 255, 255 }, 
						   (struct sui_color){ 200, 200, 200, 255 }
	);
	char bs[] = "button";
	struct sui_widget sbtn = sui_create_widget(bs, 
						   (struct sui_color){ 200, 200, 200, 255 }, 
						   (struct sui_color){ 255, 0, 255, 255 }, 
						   (struct sui_color){ 0, 0, 0, 255 }, 
						   (struct sui_color){ 0, 0, 100, 255 }
	);
	struct sui_state btn00, btn01, btn02, cbox00, slid00;
	memset(&btn00, 0, sizeof(struct sui_state));
	memset(&btn01, 0, sizeof(struct sui_state));
	memset(&btn02, 0, sizeof(struct sui_state));
	memset(&cbox00, 0, sizeof(struct sui_state));
	memset(&slid00, 0, sizeof(struct sui_state));
	struct sui_state btn10, btn11, btn12, cbox10, slid10;
	memset(&btn10, 0, sizeof(struct sui_state));
	memset(&btn11, 0, sizeof(struct sui_state));
	memset(&btn12, 0, sizeof(struct sui_state));
	memset(&cbox10, 0, sizeof(struct sui_state));
	memset(&slid10, 0, sizeof(struct sui_state));
	f32 val0 = 0.5f;
	f32 val1 = 0.5f;
	char lbuf0[64];
	memset(lbuf0, 0, 64);
	char lbuf1[64];
	memset(lbuf1, 0, 64);

	f32 colors[] = { 0.0f, 0.0f, 0.2f, 1.0f };
	
	while (1) {
		memset(&mouse.ldown, 0, 4);
		MSG msg;
		while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
			if (msg.message == WM_QUIT) {
				goto EXIT;
			}

			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		ID3D11DeviceContext_ClearRenderTargetView(context, target, colors);

		sui_input(&sui, mouse.x, mouse.y, mouse.rdown, mouse.rup, mouse.ldown, mouse.lup);
		// window 0
		sui_begin(&sui, &swin1, 100, 50);
		
		// window 0.1
		sui_begin(&sui, &swin0, 0, 20);
		sui_button(&sui, &sbtn, "button00", &btn00);
		sui_button(&sui, &sbtn, "button01", &btn01);
		sui_row(&sui);
		sui_button(&sui, &sbtn, "button02", &btn02);
		sui_slider(&sui, &sbtn, &slid00, &val0);
		sui_row(&sui);
		sui_checkbox(&sui, &sbtn, &cbox00);
		_gcvt_s(lbuf0, 64, (d64)val0, 2);
		sui_label(&sui, &sbtn, lbuf0);
		memset(lbuf0, 0, 64);
		sui_end(&sui);
		
		// window 0.2
		sui_begin(&sui, &swin0, 0, 0);
		sui_button(&sui, &sbtn, "button10", &btn10);
		sui_button(&sui, &sbtn, "button11", &btn11);
		sui_row(&sui);
		sui_button(&sui, &sbtn, "button12", &btn12);
		sui_slider(&sui, &sbtn, &slid10, &val1);
		sui_row(&sui);
		sui_checkbox(&sui, &sbtn, &cbox10);
		_gcvt_s(lbuf1, 64, (d64)val1, 2);
		sui_label(&sui, &sbtn, lbuf1);
		memset(lbuf1, 0, 64);
		sui_end(&sui);
		
		sui_end(&sui);
		sui_render(&sui);

		IDXGISwapChain_Present(swapchain, 1, 0);
	}

EXIT:
	sui_terminate(&sui);
	return 0;
}


LRESULT CALLBACK WindowProc0(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	// setup message handling system
	if (msg == WM_NCCREATE) {
		CREATESTRUCT* p_create = (CREATESTRUCT*)lparam;
		struct mouse_t* p_m = (struct mouse_t*)p_create->lpCreateParams;
		SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)p_m);
		SetWindowLongPtr(hwnd, GWLP_WNDPROC, (LONG_PTR)&WindowProc1);
		return WindowProc1(hwnd, msg, wparam, lparam);
	}
	return DefWindowProc(hwnd, msg, wparam, lparam);
}

LRESULT CALLBACK WindowProc1(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	// handle messages
	struct mouse_t* p_mouse = (struct mouse_t*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
	// p_mouse->flags = 0;

	switch (msg) {
	case WM_CLOSE:
		PostQuitMessage(0);
		break;
	case WM_MOUSEMOVE:
		POINTS p = MAKEPOINTS(lparam);
		p_mouse->x = p.x;
		p_mouse->y = p.y;
		break;
	case WM_LBUTTONDOWN:
		p_mouse->ldown = 1;
		break;
	case WM_RBUTTONDOWN:
		p_mouse->rdown = 1;
		break;
	case WM_LBUTTONUP:
		p_mouse->lup = 1;
		break;
	case WM_RBUTTONUP:
		p_mouse->rup = 1;
		break;
	}
	return DefWindowProc(hwnd, msg, wparam, lparam);
}
