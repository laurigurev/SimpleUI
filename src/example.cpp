#include "sui.hpp"
#include <stdio.h>

struct App {
        WNDCLASSEX     wc;
        HINSTANCE      hInstance;
        HWND           hwnd; 
        ID3D11Device*           device;
        ID3D11DeviceContext*    context;
        IDXGISwapChain*         swapchain;
        ID3D11RenderTargetView* target;

        App(const char* title, const i32 w, const i32 h);
        void _win32(const char* title, const i32 w, const i32 h);
        void _d3d11();
        void clear(const f32 red, const f32 green, const f32 blue);
        void present();
};

void printcmdr(const SuiCommandRect& cmdrect)
{
        printf("SuiCommandRect({%i, %i, %i, %i}, {%u, %u, %u, %u})\n", cmdrect.rect.x, cmdrect.rect.y, cmdrect.rect.w, cmdrect.rect.h, cmdrect.color.r,
               cmdrect.color.g, cmdrect.color.b, cmdrect.color.a);
}

int main()
{
        SuiContext sui;
        sui.begin("window", SuiRect(50, 50, 100, 100));
        i32 row0[] = {20, 40};
        sui.row(2, row0, 16);
        sui.rect();
        sui.rect();
        sui.end();
        for (i32 i = 0; i < sui.cmdrects.idx; i++) printcmdr(sui.cmdrects.data[i]);
        return 0;
}

App::App(const char* title, const i32 w, const i32 h)
{
        
}

void App::_win32(const char* title, const i32 w, const i32 h)
{
        
}

void App::_d3d11()
{
        
}

void App::clear(const f32 red, const f32 green, const f32 blue)
{
        
}

void App::present()
{
        
}
