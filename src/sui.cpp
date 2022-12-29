#include "sui.hpp"
#include <stdio.h>

SuiColor::SuiColor(const u8 _r, const u8 _g, const u8 _b, const u8 _a) : r(_r), g(_g), b(_b), a(_a) {}
SuiRect::SuiRect(const i32 _x, const i32 _y, const i32 _w, const i32 _h) : x(_x), y(_y), w(_w), h(_h) {}
SuiStyle::SuiStyle(const i32 _spacing, const SuiColor windowbg, const SuiColor rect, const SuiColor recthover, const SuiColor rectfocus)
    : spacing(_spacing), colors{windowbg, rect, recthover, rectfocus}
{
}
SuiLayout::SuiLayout(const SuiRect _rect, const SuiRect _body) : rect(_rect), body(_body) {}
SuiCommandRect::SuiCommandRect(const SuiRect _rect, const SuiColor _color) : rect(_rect), color(_color) {}

SuiContext::SuiContext() : style(2, SuiColor(255, 255, 255, 255), SuiColor(100, 0, 0, 255), SuiColor(255, 0, 0, 255), SuiColor(150, 0, 0, 255)) {}

void SuiContext::begin(const char* name, const SuiRect rect)
{
        const i32 spacing = style.spacing;
        SuiRect body = {rect.x + spacing, rect.y + spacing, rect.w - spacing, rect.h - spacing};
        layouts.push(SuiLayout(rect, body));
        cmdrects.push(SuiCommandRect(rect, style.colors[SUI_COLOR_WINDOWBG]));
}

void SuiContext::end()
{
        layouts.pop();
}

void SuiContext::rect()
{
        SuiLayout layout = layouts.get();
        layouts.pop();
        cmdrects.push(SuiCommandRect(layout.body, style.colors[SUI_COLOR_RECT]));
}
