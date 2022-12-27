#include "sui.h"

#include <assert.h>
#include <stdio.h>

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
        u8  ldown;
        u8  lup;
        u8  rdown;
        u8  rup;
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

        WNDCLASSEX     wc;
        HINSTANCE      hInstance;
        HWND           hwnd;
        struct mouse_t mouse;

        {
                const i32 x = 60;
                const i32 y = 60;
                const i32 w = 600;
                const i32 h = 600;

                const wchar_t class_name[] = L"SimpleUI";
                hInstance = GetModuleHandle(NULL);

                RECT rect = {0, 0, w, h};
                AdjustWindowRect(&rect, WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU, 0);

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

                hwnd = CreateWindowEx(0, class_name, L"SimpleUI example", WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU, x, y, rect.right - rect.left,
                                      rect.bottom - rect.top, NULL, NULL, hInstance, &mouse);

                if (!hwnd) {
                        return 1;
                }

                ShowWindow(hwnd, SW_SHOW);
        }

        // --- D3D11 init

        ID3D11Device*           device;
        ID3D11DeviceContext*    context;
        IDXGISwapChain*         swapchain;
        ID3D11RenderTargetView* target;

        {
                HRESULT hr;

                DXGI_MODE_DESC buffer_descriptor = {
                    0, 0, {0, 0},
                      DXGI_FORMAT_B8G8R8A8_UNORM, DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED, DXGI_MODE_SCALING_UNSPECIFIED
                };

                DXGI_SWAP_CHAIN_DESC swapchain_descriptor = {buffer_descriptor,        1, 0, DXGI_USAGE_RENDER_TARGET_OUTPUT, 1, hwnd, TRUE,
                                                             DXGI_SWAP_EFFECT_DISCARD, 0};

                hr = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, D3D11_CREATE_DEVICE_SINGLETHREADED | D3D11_CREATE_DEVICE_DEBUG, NULL,
                                                   0, D3D11_SDK_VERSION, &swapchain_descriptor, &swapchain, &device, NULL, &context);
                assert(hr == 0);

                ID3D11Texture2D* backbuffer = NULL;
                IDXGISwapChain_GetBuffer(swapchain, 0, &IID_ID3D11Texture2D, (void**)&backbuffer);
                hr = ID3D11Device_CreateRenderTargetView(device, (ID3D11Resource*)backbuffer, NULL, &target);
                assert(hr == 0);
                ID3D11Texture2D_Release(backbuffer);

                RECT wrect;
                GetClientRect(hwnd, &wrect);
                D3D11_VIEWPORT view = {0.0f, 0.0f, (float)(wrect.right - wrect.left), (float)(wrect.bottom - wrect.top), 0.0f, 1.0f};
                ID3D11DeviceContext_RSSetViewports(context, 1, &view);
                ID3D11DeviceContext_OMSetRenderTargets(context, 1, &target, NULL);
        }

        struct sui_context sui;
        sui_init(&sui, device, 600, 600);
        f32 colors[] = {0.0f, 0.0f, 0.2f, 1.0f};
        f32 red = 0.0f;
        f32 green = 0.0f;
        f32 blue = 0.2f;
        i64 start, time = 0;
        i64 avg = 0;
        d64 davg = 0.0;
        i32 counter = 0;

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

                struct sui_profile profile = sui.profile;
                sui_profile_new(&sui);

                start = sui_time_begin();

                sui_inputs(&sui, mouse.x, mouse.y, mouse.ldown, mouse.lup, mouse.rdown, mouse.rup);
                sui_begin(&sui, "clear_color_settings", 200, 250);
                sui_column(&sui);
                sui_label(&sui, "red");
                sui_label(&sui, "green");
                sui_label(&sui, "blue");
                sui_column(&sui);
                sui_slider(&sui, "red slider", &red);
                sui_slider(&sui, "green slider", &green);
                sui_slider(&sui, "blue slider", &blue);
                sui_row(&sui);
                sui_row(&sui);
                sui_text(&sui, "rgb (%.2f, %.2f, %.2f)", red, green, blue);
                sui_row(&sui);
                if (sui_button(&sui, "apply")) {
                        colors[0] = red;
                        colors[1] = green;
                        colors[2] = blue;
                }
                sui_end(&sui);

                // us is mikroseconds
                
                sui_begin(&sui, "sui profile", 0, 0);
                sui_text(&sui, "[%i] sui_button() time %llius", profile.button_count, profile.button_avg_time);
                sui_row(&sui);
                sui_text(&sui, "[%i] sui_checkbox() time %llius", profile.checkbox_count, profile.checkbox_avg_time);
                sui_row(&sui);
                sui_text(&sui, "[%i] sui_slider() time %llius", profile.slider_count, profile.slider_avg_time);
                sui_row(&sui);
                sui_text(&sui, "[%i] sui_label() time %llius", profile.label_count, profile.label_avg_time);
                sui_row(&sui);
                sui_text(&sui, "[%i] sui_text() time %llius", profile.text_count, profile.text_avg_time);
                sui_row(&sui);
                sui_text(&sui, "memory [a/u/p] [%llu/%llu/%llu]", profile.memory_allocated, profile.memory_used, profile.memory_pushed);
                sui_row(&sui);
                sui_text(&sui, "widget count %i", profile.hash_table_item_count);
                sui_row(&sui);
                sui_text(&sui, "vertex count %i", profile.vertex_count);
                sui_row(&sui);
                sui_text(&sui, "draw calls %i", profile.draw_calls);
                // sui_row(&sui);
                // sui_text(&sui, "frame time %ius", avg);
                sui_row(&sui);
                sui_text(&sui, "frame time %.3lfms", davg);
                sui_end(&sui);
                
                sui_render(&sui);

                time += sui_time_end(start);
                if (counter == 20) {
                        avg = time / 20;
                        davg = (d64)avg / 1000.0;
                        time = 0;
                        counter = 0;
                }
                counter++;

                IDXGISwapChain_Present(swapchain, 1, 0);
        }

EXIT:
        return 0;
}

LRESULT CALLBACK WindowProc0(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
        // setup message handling system
        if (msg == WM_NCCREATE) {
                CREATESTRUCT*   p_create = (CREATESTRUCT*)lparam;
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
