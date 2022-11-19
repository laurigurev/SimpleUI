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

void ex_window_init();
void ex_window_terminate();

void ex_gdx_context_init();
void ex_gdx_context_terminate();
// TODO(end)

LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

int main()
{
	puts("This is example application using d11 and winapi");

	// --- Win32 window init
	
	WNDCLASSEX wc;
	HINSTANCE hInstance;
	HWND hwnd;

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
		wc.lpfnWndProc = WindowProc;
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
			NULL
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
	sui_init_fixed(&sui, device, 600, 600);
	struct sui_window swin;

	f32 colors[] = { 0.0f, 0.0f, 0.2f, 1.0f };
	
	while (1) {
		MSG msg;
		while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
			if (msg.message == WM_QUIT) {
				goto EXIT;
			}

			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		ID3D11DeviceContext_ClearRenderTargetView(context, target, colors);
		// sui_button(&sui);
		sui_window_begin(&swin, &sui, 100, 100, 200, 100);
		sui_window_end(&sui);
		IDXGISwapChain_Present(swapchain, 1, 0);
	}

EXIT:
	sui_terminate(&sui);
	return 0;
}


LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	switch (msg) {
	case WM_CLOSE:
		PostQuitMessage(0);
		break;
	}
	return DefWindowProc(hwnd, msg, wparam, lparam);
}
