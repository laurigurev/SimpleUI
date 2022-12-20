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
	struct sui_window swin = sui_create_window(ws, 
						   (struct sui_color){ 255, 255, 255, 255 }, 
						   (struct sui_color){ 255, 255, 255, 255 }, 
						   (struct sui_color){ 255, 255, 255, 255 }, 
						   (struct sui_color){ 255, 255, 255, 255 }
	);
	char bs[] = "button";
	struct sui_widget sbtn = sui_create_widget(bs, 
						   (struct sui_color){ 200, 200, 200, 255 }, 
						   (struct sui_color){ 255, 0, 255, 255 }, 
						   (struct sui_color){ 0, 0, 0, 255 }, 
						   (struct sui_color){ 0, 0, 100, 255 }
	);
	struct sui_state bst0 = { 0, 0, 0, 0, 0, 0 };
	struct sui_state bst1 = { 0, 0, 0, 0, 0, 0 };
	struct sui_state bst2 = { 0, 0, 0, 0, 0, 0 };
	struct sui_state cst = { 0, 0, 0, 0, 0, 0 };
	struct sui_state sst = { 0, 0, 0, 0, 0, 0, 0 };
	f32 val = 0.5f;

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
		sui_begin(&sui, &swin, 100, 100);
		sui_button(&sui, &sbtn, "button0", &bst0);
		if (bst0.pressed) printf("button0 PRESSED ");
		if (bst0.released) printf("button0 RELEASED ");
		if (bst0.clicked) printf("button0 CLICKED ");
		if (bst0.released || bst0.clicked) printf("\n");
		
		sui_button(&sui, &sbtn, "button1", &bst1);
		if (bst1.pressed) printf("button1 PRESSED ");
		if (bst1.released) printf("button1 RELEASED ");
		if (bst1.clicked) printf("button1 CLICKED ");
		if (bst1.released || bst1.clicked) printf("\n");
		
		sui_row(&sui);
		
		sui_button(&sui, &sbtn, "button2", &bst2);
		if (bst2.pressed) printf("button2 PRESSED ");
		if (bst2.released) printf("button2 RELEASED ");
		if (bst2.clicked) printf("button2 CLICKED ");
		if (bst2.released || bst2.clicked) printf("\n");

		sui_slider(&sui, &sbtn, &sst, &val);

		sui_row(&sui);
		
		sui_checkbox(&sui, &sbtn, &cst);
		
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
