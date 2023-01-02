#include "sui.hpp"
#include <stdio.h>

#define WIN32_MEAN_AND_LEAN
#include <d3d11.h>
#include <windows.h>

struct App {
        WNDCLASSEX              wc;
        HINSTANCE               hInstance;
        HWND                    hwnd;
        ID3D11Device*           device;
        ID3D11DeviceContext*    context;
        IDXGISwapChain*         swapchain;
        ID3D11RenderTargetView* target;

        struct Mouse {
                i32 x, y;
                u8  ldown, lup;
                u8  rdown, rup;
        };
        Mouse mouse;

        // TODO: change title into const char*
        App(const wchar_t* title, const i32 w, const i32 h);
        void _win32(const wchar_t* title, const i32 w, const i32 h);
        void _d3d11();
        i32  close();
        void clear(const f32 red, const f32 green, const f32 blue);
        void present();

        static LRESULT CALLBACK SetupMessageCallback(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
        static LRESULT CALLBACK MessageCallback(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
};

int main()
{
        App        app(L"SimpleUI example.cpp", 600, 600);
        SuiContext sui;
        SuiBackend backend(app.device, 600, 600, sui.style.font.uvs);

        i32 value0 = 0;
        f32 value1 = 0.5f;

        while (app.close()) {
                sui.reset();
                sui.inputs(app.mouse.x, app.mouse.y, app.mouse.ldown, app.mouse.lup, app.mouse.rdown, app.mouse.rup);
                sui.begin("window", SuiRect(50, 50, 200, 200));

                i32 ws[] = {-1, 16};
                sui.row(2, ws, 16);
                // sui.reveal_layout();
                sui.label("label");
                sui.reveal_layout();

                i32 hs[] = {16, 16, 16};
                sui.column(3, 40, hs);
                // sui.reveal_layout();
                sui.checkbox("checkbox0", &value0);
                // sui.reveal_layout();
                if (sui.button("btn0")) printf("btn0 pressed\n");
                sui.reveal_layout();
                
                if (value0) sui.reveal_layout();
                else sui.slider("slider0", &value1);
                // sui.reveal_layout();
                sui.labelf("slider %.2f", value1);

                sui.end();

                app.clear(0.0f, 0.0f, value1);
                backend.record(sui.rectcmds.idx, sui.rectcmds.data);
                backend.draw();
                app.present();
        }

        printf("--------------------------------------\n");
        printf("d3d11 time %fms\n", backend.profiler.time);
        printf("d3d11 profiler.ia_vertices    %llu\n", backend.profiler.ia_vertices);
        printf("d3d11 profiler.ia_primitives  %llu\n", backend.profiler.ia_primitives);
        printf("d3d11 profiler.vs_invocations %llu\n", backend.profiler.vs_invocations);
        printf("d3d11 profiler.ps_invocations %llu\n", backend.profiler.ps_invocations);
        printf("d3d11 profiler.cs_invocations %llu\n\n", backend.profiler.cs_invocations);
        printf("d3d11 profiler.num_primitives_written   %llu\n", backend.profiler.num_primitives_written);
        printf("d3d11 profiler.primitives_storage_needed %llu\n", backend.profiler.primitives_storage_needed);
        printf("--------------------------------------\n");

        return 0;
}

/* LRESULT CALLBACK DefaultMessageCallback(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
        switch (msg) {
        case WM_CLOSE:
                PostQuitMessage(0);
                break;
        }
        return DefWindowProc(hwnd, msg, wparam, lparam);
} */

App::App(const wchar_t* title, const i32 w, const i32 h)
{
        _win32(title, w, h);
        _d3d11();
}

void App::_win32(const wchar_t* title, const i32 w, const i32 h)
{
        const i32     x = 60;
        const i32     y = 60;
        const wchar_t class_name[] = L"SimpleUI";

        hInstance = GetModuleHandle(NULL);

        RECT rect = {0, 0, w, h};
        AdjustWindowRect(&rect, WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU, 0);

        wc.cbSize = sizeof(wc);
        wc.style = CS_OWNDC;
        wc.lpfnWndProc = SetupMessageCallback;
        wc.cbClsExtra = 0;
        wc.cbWndExtra = 0;
        wc.hInstance = hInstance;
        wc.hIcon = NULL;
        wc.hCursor = NULL;
        wc.hbrBackground = NULL;
        wc.lpszMenuName = NULL;
        wc.lpszClassName = class_name;
        wc.hIconSm = NULL;

        SuiAssert(RegisterClassEx(&wc));

        hwnd = CreateWindowEx(0, class_name, title, WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU, x, y, rect.right - rect.left, rect.bottom - rect.top, NULL, NULL,
                              hInstance, this);

        SuiAssert(hwnd);
        ShowWindow(hwnd, SW_SHOW);
}

void App::_d3d11()
{
        HRESULT hr;

        DXGI_MODE_DESC buffer_descriptor = {
            0, 0, {0, 0},
              DXGI_FORMAT_B8G8R8A8_UNORM, DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED, DXGI_MODE_SCALING_UNSPECIFIED
        };

        DXGI_SWAP_CHAIN_DESC swapchain_descriptor = {buffer_descriptor, 1, 0, DXGI_USAGE_RENDER_TARGET_OUTPUT, 1, hwnd, TRUE, DXGI_SWAP_EFFECT_DISCARD, 0};

        hr = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, D3D11_CREATE_DEVICE_SINGLETHREADED | D3D11_CREATE_DEVICE_DEBUG, NULL, 0,
                                           D3D11_SDK_VERSION, &swapchain_descriptor, &swapchain, &device, NULL, &context);
        SuiAssert(hr == 0);

        ID3D11Texture2D* backbuffer = NULL;
        swapchain->GetBuffer(0, IID_ID3D11Texture2D, (void**)&backbuffer);
        hr = device->CreateRenderTargetView((ID3D11Resource*)backbuffer, NULL, &target);
        SuiAssert(hr == 0);
        backbuffer->Release();

        RECT wrect;
        GetClientRect(hwnd, &wrect);
        D3D11_VIEWPORT view = {0.0f, 0.0f, (float)(wrect.right - wrect.left), (float)(wrect.bottom - wrect.top), 0.0f, 1.0f};
        context->RSSetViewports(1, &view);
        context->OMSetRenderTargets(1, &target, NULL);
}

i32 App::close()
{
        MSG msg;
        while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
                if (msg.message == WM_QUIT) {
                        return 0;
                }

                TranslateMessage(&msg);
                DispatchMessage(&msg);
        }
        return 1;
}

void App::clear(const f32 red, const f32 green, const f32 blue)
{
        f32 colors[4] = {red, green, blue, 1.0f};
        context->ClearRenderTargetView(target, colors);
}

void App::present()
{
        swapchain->Present(1, 0);
}

LRESULT CALLBACK App::SetupMessageCallback(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
        if (msg == WM_NCCREATE) {
                CREATESTRUCT* create = (CREATESTRUCT*)lparam;
                App*          app = (App*)create->lpCreateParams;
                SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)app);
                SetWindowLongPtr(hwnd, GWLP_WNDPROC, (LONG_PTR)MessageCallback);
                return MessageCallback(hwnd, msg, wparam, lparam);
        }
        return DefWindowProc(hwnd, msg, wparam, lparam);
}

LRESULT CALLBACK App::MessageCallback(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
        App* app = (App*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
        
        app->mouse.ldown = 0;
        app->mouse.lup = 0;
        app->mouse.rdown = 0;
        app->mouse.rup = 0;
        
        switch (msg) {
        case WM_CLOSE: {
                PostQuitMessage(0);
                break;
        }
        case WM_MOUSEMOVE: {
                POINTS p = MAKEPOINTS(lparam);
                app->mouse.x = p.x;
                app->mouse.y = p.y;
                break;
        }
        case WM_LBUTTONDOWN: {
                app->mouse.ldown = 1;
                break;
        }
        case WM_RBUTTONDOWN: {
                app->mouse.rdown = 1;
                break;
        }
        case WM_LBUTTONUP: {
                app->mouse.lup = 1;
                break;
        }
        case WM_RBUTTONUP: {
                app->mouse.rup = 1;
                break;
        }
        }
        return DefWindowProc(hwnd, msg, wparam, lparam);
}
