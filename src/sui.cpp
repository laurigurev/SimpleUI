#include "sui.hpp"
#include <stdio.h>

u32 SuiHash(const char* s)
{
        u32 hash = 5381;
        while (*s) hash = ((hash << 5) + hash) + *s++;
        return hash;
}

SuiColor::SuiColor(const u8 _r, const u8 _g, const u8 _b, const u8 _a) : r(_r), g(_g), b(_b), a(_a) {}
SuiRect::SuiRect(const i32 _x, const i32 _y, const i32 _w, const i32 _h) : x(_x), y(_y), w(_w), h(_h) {}

SuiRect SuiRect::scale(const i32 px)
{
        return SuiRect(x + px, y + px, w - 2 * px, h - 2 * px);
}

SuiUV::SuiUV(const i32 x, const i32 y, const i32 w, const i32 h)
{
        const f32 image_w = 128;
        const f32 image_h = 64;

        u0 = (f32)x / image_w;
        v0 = (f32)y / image_h;
        u1 = u0 + (f32)w / image_w;
        v1 = v0 + (f32)h / image_h;
}

SuiGlyph::SuiGlyph(const i32 _w, const i32 _h, const i32 _xoff, const i32 _yoff) : w(_w), h(_h), xoff(_xoff), yoff(_yoff) {}

SuiFont::SuiFont()
    : uvs{SuiUV(63, 48, 24, 1), SuiUV(83, 31, 4, 9),  SuiUV(105, 39, 6, 3), SuiUV(8, 14, 7, 9),   SuiUV(0, 0, 6, 13),   SuiUV(26, 0, 8, 11),
          SuiUV(16, 13, 7, 9),  SuiUV(125, 10, 2, 3), SuiUV(54, 0, 4, 11),  SuiUV(59, 0, 4, 11),  SuiUV(63, 42, 7, 5),  SuiUV(71, 41, 6, 5),
          SuiUV(78, 41, 3, 4),  SuiUV(121, 39, 6, 1), SuiUV(117, 39, 3, 2), SuiUV(86, 0, 6, 10),  SuiUV(62, 12, 6, 9),  SuiUV(77, 31, 5, 9),
          SuiUV(69, 11, 6, 9),  SuiUV(76, 11, 6, 9),  SuiUV(32, 12, 7, 9),  SuiUV(83, 11, 6, 9),  SuiUV(90, 11, 6, 9),  SuiUV(97, 11, 6, 9),
          SuiUV(48, 12, 6, 9),  SuiUV(104, 11, 6, 9), SuiUV(122, 30, 3, 7), SuiUV(88, 31, 3, 9),  SuiUV(111, 11, 6, 9), SuiUV(91, 41, 6, 3),
          SuiUV(118, 10, 6, 9), SuiUV(0, 24, 6, 9),   SuiUV(114, 0, 8, 9),  SuiUV(7, 24, 6, 9),   SuiUV(14, 24, 6, 9),  SuiUV(21, 23, 6, 9),
          SuiUV(55, 12, 6, 9),  SuiUV(28, 23, 6, 9),  SuiUV(35, 22, 6, 9),  SuiUV(42, 22, 6, 9),  SuiUV(49, 22, 6, 9),  SuiUV(123, 0, 4, 9),
          SuiUV(56, 22, 6, 9),  SuiUV(63, 22, 6, 9),  SuiUV(70, 21, 6, 9),  SuiUV(0, 14, 7, 9),   SuiUV(40, 12, 7, 9),  SuiUV(77, 21, 6, 9),
          SuiUV(21, 33, 6, 9),  SuiUV(35, 0, 6, 11),  SuiUV(91, 21, 6, 9),  SuiUV(98, 21, 6, 9),  SuiUV(105, 21, 6, 9), SuiUV(112, 21, 6, 9),
          SuiUV(119, 20, 6, 9), SuiUV(24, 13, 7, 9),  SuiUV(0, 34, 6, 9),   SuiUV(7, 34, 6, 9),   SuiUV(14, 34, 6, 9),  SuiUV(18, 0, 4, 12),
          SuiUV(100, 0, 6, 10), SuiUV(13, 0, 4, 12),  SuiUV(98, 39, 6, 3),  SuiUV(88, 45, 8, 1),  SuiUV(112, 39, 4, 3), SuiUV(108, 31, 6, 7),
          SuiUV(28, 33, 6, 9),  SuiUV(115, 31, 6, 7), SuiUV(35, 32, 6, 9),  SuiUV(0, 44, 6, 7),   SuiUV(42, 32, 6, 9),  SuiUV(107, 0, 6, 10),
          SuiUV(49, 32, 6, 9),  SuiUV(93, 0, 6, 10),  SuiUV(7, 0, 5, 13),   SuiUV(56, 32, 6, 9),  SuiUV(63, 32, 6, 9),  SuiUV(92, 31, 7, 7),
          SuiUV(7, 44, 6, 7),   SuiUV(14, 44, 6, 7),  SuiUV(79, 0, 6, 10),  SuiUV(72, 0, 6, 10),  SuiUV(21, 43, 6, 7),  SuiUV(56, 42, 6, 7),
          SuiUV(70, 31, 6, 9),  SuiUV(28, 43, 6, 7),  SuiUV(35, 42, 6, 7),  SuiUV(100, 31, 7, 7), SuiUV(42, 42, 6, 7),  SuiUV(64, 0, 7, 10),
          SuiUV(49, 42, 6, 7),  SuiUV(48, 0, 5, 11),  SuiUV(23, 0, 2, 12),  SuiUV(42, 0, 5, 11),  SuiUV(82, 41, 8, 3),  SuiUV(84, 21, 6, 9)},
      glyphs{SuiGlyph(24, 1, -8, 14), SuiGlyph(4, 9, 2, 3),  SuiGlyph(6, 3, 1, 3),  SuiGlyph(7, 9, 1, 3),  SuiGlyph(6, 13, 1, 1), SuiGlyph(8, 11, 0, 2),
             SuiGlyph(7, 9, 1, 3),    SuiGlyph(2, 3, 3, 3),  SuiGlyph(4, 11, 2, 3), SuiGlyph(4, 11, 2, 3), SuiGlyph(7, 5, 1, 5),  SuiGlyph(6, 5, 1, 5),
             SuiGlyph(3, 4, 3, 10),   SuiGlyph(6, 1, 1, 7),  SuiGlyph(3, 2, 3, 10), SuiGlyph(6, 10, 1, 3), SuiGlyph(6, 9, 2, 3),  SuiGlyph(5, 9, 1, 3),
             SuiGlyph(6, 9, 1, 3),    SuiGlyph(6, 9, 1, 3),  SuiGlyph(7, 9, 1, 3),  SuiGlyph(6, 9, 1, 3),  SuiGlyph(6, 9, 1, 3),  SuiGlyph(6, 9, 1, 3),
             SuiGlyph(6, 9, 1, 3),    SuiGlyph(6, 9, 1, 3),  SuiGlyph(3, 7, 3, 5),  SuiGlyph(3, 9, 3, 5),  SuiGlyph(6, 9, 1, 3),  SuiGlyph(6, 3, 1, 6),
             SuiGlyph(6, 9, 1, 3),    SuiGlyph(6, 9, 1, 3),  SuiGlyph(8, 9, 0, 3),  SuiGlyph(6, 9, 1, 3),  SuiGlyph(6, 9, 1, 3),  SuiGlyph(6, 9, 1, 3),
             SuiGlyph(6, 9, 1, 3),    SuiGlyph(6, 9, 1, 3),  SuiGlyph(6, 9, 1, 3),  SuiGlyph(6, 9, 1, 3),  SuiGlyph(6, 9, 1, 3),  SuiGlyph(4, 9, 2, 3),
             SuiGlyph(6, 9, 1, 3),    SuiGlyph(6, 9, 1, 3),  SuiGlyph(6, 9, 1, 3),  SuiGlyph(7, 9, 1, 3),  SuiGlyph(7, 9, 1, 3),  SuiGlyph(6, 9, 1, 3),
             SuiGlyph(6, 9, 1, 3),    SuiGlyph(6, 11, 1, 3), SuiGlyph(6, 9, 1, 3),  SuiGlyph(6, 9, 1, 3),  SuiGlyph(6, 9, 1, 3),  SuiGlyph(6, 9, 1, 3),
             SuiGlyph(6, 9, 1, 3),    SuiGlyph(7, 9, 1, 3),  SuiGlyph(6, 9, 1, 3),  SuiGlyph(6, 9, 1, 3),  SuiGlyph(6, 9, 1, 3),  SuiGlyph(4, 12, 2, 3),
             SuiGlyph(6, 10, 1, 3),   SuiGlyph(4, 12, 2, 3), SuiGlyph(6, 3, 1, 1),  SuiGlyph(8, 1, 0, 14), SuiGlyph(4, 3, 2, 1),  SuiGlyph(6, 7, 1, 5),
             SuiGlyph(6, 9, 1, 3),    SuiGlyph(6, 7, 1, 5),  SuiGlyph(6, 9, 1, 3),  SuiGlyph(6, 7, 1, 5),  SuiGlyph(6, 9, 1, 3),  SuiGlyph(6, 10, 1, 5),
             SuiGlyph(6, 9, 1, 3),    SuiGlyph(6, 10, 1, 2), SuiGlyph(5, 13, 1, 2), SuiGlyph(6, 9, 1, 3),  SuiGlyph(6, 9, 1, 3),  SuiGlyph(7, 7, 1, 5),
             SuiGlyph(6, 7, 1, 5),    SuiGlyph(6, 7, 1, 5),  SuiGlyph(6, 10, 1, 5), SuiGlyph(6, 10, 1, 5), SuiGlyph(6, 7, 1, 5),  SuiGlyph(6, 7, 1, 5),
             SuiGlyph(6, 9, 1, 3),    SuiGlyph(6, 7, 1, 5),  SuiGlyph(6, 7, 1, 5),  SuiGlyph(7, 7, 1, 5),  SuiGlyph(6, 7, 1, 5),  SuiGlyph(7, 10, 0, 5),
             SuiGlyph(6, 7, 1, 5),    SuiGlyph(5, 11, 1, 3), SuiGlyph(2, 12, 3, 3), SuiGlyph(5, 11, 2, 3), SuiGlyph(8, 3, 0, 3),  SuiGlyph(6, 9, 1, 3)},
      xadvs{8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
            8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8},
      name("Fixedsys")
{
        // SuiUV uvs[95];
        // SuiGlyph glyphs[96];
        // i32 xadvs[95];

        // VERY UGLY STUFF
}

SuiStyle::SuiStyle()
    : spacing(2), colors{SuiColor(255, 255, 255, 255), SuiColor(100, 0, 0, 255),     SuiColor(255, 255, 255, 255), SuiColor(100, 100, 100, 255),
                         SuiColor(50, 50, 50, 255),    SuiColor(100, 100, 100, 255), SuiColor(0, 0, 0, 255),       SuiColor(255, 255, 255, 255),
                         SuiColor(0, 0, 0, 255),       SuiColor(100, 100, 100, 255)}
{
}
SuiLayout::SuiLayout(const SuiRect _rect) : rect(_rect) {}
SuiRectCommand::SuiRectCommand(const SuiRect _rect, const SuiColor _color, const i32 _c) : rect(_rect), color(_color), c(_c) {}
SuiIO::SuiIO() : mx(0), my(0), dmx(0), dmy(0), ldown(0), lup(0), lheld(0), lclick(0), rdown(0), rup(0), rheld(0), rclick(0) {}

i32 SuiIO::mxy_in_rect(const SuiRect rect)
{
        const i32 x0 = rect.x;
        const i32 x1 = rect.x + rect.w;
        const i32 y0 = rect.y;
        const i32 y1 = rect.y + rect.h;
        if (x0 < mx && mx < x1 && y0 < my && my < y1) return 1;
        return 0;
}

SuiContext::SuiContext() : hot_id(0), active_id(0) {}

void SuiContext::inputs(i32 mx, i32 my, u8 ldown, u8 lup, u8 rdown, u8 rup)
{
        io.dmx = io.mx - mx;
        io.dmy = io.my - my;
        io.mx = mx;
        io.my = my;

        io.ldown = ldown;
        io.lup = lup;
        io.rdown = rdown;
        io.rup = rup;

        // TODO: held and click
}

void SuiContext::begin(const char* name, const SuiRect rect)
{
        const i32 spacing = style.spacing;
        SuiRect   body = {rect.x + spacing, rect.y + spacing, rect.w - 2 * spacing, rect.h - 2 * spacing};
        layouts.push(SuiLayout(body));
        rectcmds.push(SuiRectCommand(rect, style.colors[SUI_COLOR_WINDOWBG], 127));
}

void SuiContext::end()
{
        layouts.pop();
}

void SuiContext::row(const i32 n, const i32* widths, i32 height)
{
        SuiAssert(0 < n && n < 16);
        SuiAssert(widths);

        // Declare variables
        SuiLayout curr_layout = layouts.pop();
        if (height == -1) height = curr_layout.rect.h;
        SuiAssert(height <= curr_layout.rect.h);

        // get last assert
        i32 ws[16];
        i32 leftover_width, summed_width = 0, body_width = 0, neg1counter = 0;

        body_width = curr_layout.rect.w - (n - 1) * style.spacing;
        leftover_width = body_width;

        memcpy(ws, widths, n * sizeof(i32));
        for (i32 i = 0; i < n; i++) {
                if (widths[i] == -1) {
                        neg1counter++;
                        continue;
                }
                summed_width += widths[i];
                leftover_width -= widths[i];
        }
        summed_width += neg1counter * SUI_MIN_RECT_WIDTH;
        SuiAssert(summed_width <= body_width);

        SuiLayout new_layouts[16];
        i32       x, y, width, neg1w = 0, leftover = 0, xoff = 0;

        x = curr_layout.rect.x;
        y = curr_layout.rect.y;
        if (neg1counter) {
                neg1w = leftover_width / neg1counter;
                leftover = leftover_width - neg1w * neg1counter;
        }
        for (i32 i = 0; i < n; i++) {
                width = widths[i];
                if (width == -1) width = neg1w;
                if (i == n - 1) width += leftover;
                SuiRect rect = {x + xoff, y, width, height};
                xoff += width + style.spacing;
                new_layouts[i] = SuiLayout(rect);
        }

        i32 h0, h1;

        h0 = height + style.spacing;
        h1 = curr_layout.rect.h - h0;
        curr_layout.rect = {x, y + h0, curr_layout.rect.w, h1};
        layouts.push(curr_layout);

        for (i32 i = n - 1; 0 <= i; i--) layouts.push(new_layouts[i]);
}

void SuiContext::column(const i32 n, i32 width, const i32* heights)
{
        SuiAssert(0 < n && n < 16);
        SuiAssert(heights);

        // Declare variables
        SuiLayout curr_layout = layouts.pop();
        if (width == -1) width = curr_layout.rect.w;
        SuiAssert(width <= curr_layout.rect.w);

        // get last assert
        i32 hs[16];
        i32 leftover_height, summed_height = 0, body_height = 0, neg1counter = 0;

        body_height = curr_layout.rect.h - (n - 1) * style.spacing;
        leftover_height = body_height;

        memcpy(hs, heights, n * sizeof(i32));
        for (i32 i = 0; i < n; i++) {
                if (heights[i] == -1) {
                        neg1counter++;
                        continue;
                }
                summed_height += heights[i];
                leftover_height -= heights[i];
        }
        summed_height += neg1counter * SUI_MIN_RECT_HEIGHT;
        SuiAssert(summed_height <= body_height);

        SuiLayout new_layouts[16];
        i32       x, y, height, neg1h = 0, leftover = 0, yoff = 0;

        x = curr_layout.rect.x;
        y = curr_layout.rect.y;
        if (neg1counter) {
                neg1h = leftover_height / neg1counter;
                leftover = leftover_height - neg1h * neg1counter;
        }
        for (i32 i = 0; i < n; i++) {
                height = heights[i];
                if (height == -1) height = neg1h;
                // TODO: change this to last neg1
                if (i == n - 1) height += leftover;
                SuiRect rect = {x, y + yoff, width, height};
                yoff += height + style.spacing;
                new_layouts[i] = SuiLayout(rect);
        }

        i32 w0, w1;

        if (summed_height != body_height) {
                curr_layout.rect = {x, y + yoff, curr_layout.rect.w, curr_layout.rect.h - yoff};
                layouts.push(curr_layout);
        }
        w0 = width + style.spacing;
        w1 = curr_layout.rect.w - w0;
        curr_layout.rect = {x + w0, y, w1, yoff - style.spacing};
        layouts.push(curr_layout);

        for (i32 i = n - 1; 0 <= i; i--) layouts.push(new_layouts[i]);
}

void SuiContext::reveal_layout()
{
        SuiLayout layout = layouts.pop();
        rectcmds.push(SuiRectCommand(layout.rect, style.colors[SUI_COLOR_LAYOUT], 127));
}

SuiRect SuiContext::get_rect(const i32 w, const i32 h, const SuiAlignmentFlags alignment, const SuiLayout& layout)
{
        SuiAssert(w <= layout.rect.w && h <= layout.rect.h);

        // SUI_ALIGNMENT_FLAG_LEFT && SUI_ALIGNMENT_FLAG_TOP
        i32 xoff = 0, yoff = 0;
        if (alignment & SUI_ALIGNMENT_FLAG_RIGHT) {
                xoff = layout.rect.w - w;
        }
        else if (!(alignment & SUI_ALIGNMENT_FLAG_LEFT)) {
                xoff = (layout.rect.w / 2) - (w / 2);
        }

        if (alignment & SUI_ALIGNMENT_FLAG_BOTTOM) {
                yoff = layout.rect.h - h;
        }
        else if (!(alignment & SUI_ALIGNMENT_FLAG_TOP)) {
                yoff = (layout.rect.h / 2) - (h / 2);
        }

        return SuiRect(layout.rect.x + xoff, layout.rect.y + yoff, w, h);
}

void SuiContext::label(const char* s)
{
        SuiAssert(s);
        const char* c = s;
        i32         len = 0;
        while (*c) len += style.font.xadvs[*c++ - 32];

        SuiAssert(layouts.idx != 0);
        SuiLayout layout = layouts.pop();
        SuiRect   rect = get_rect(len, 16, SUI_ALIGNMENT_FLAG_LEFT, layout);
        SuiColor  color = style.colors[SUI_COLOR_LABELBG];

        // SuiLayoutAction action = SUI_LAYOUT_ACTION_NEXT;
        rectcmds.push(SuiRectCommand(rect, color, 127));

        c = s;
        len = 0;
        while (*c) {
                const SuiGlyph glyph = style.font.glyphs[*c - 32];
                const SuiRect  grect = {rect.x + len + glyph.xoff, rect.y + glyph.yoff, glyph.w, glyph.h};
                rectcmds.push(SuiRectCommand(grect, style.colors[SUI_COLOR_TEXT], *c));
                len += style.font.xadvs[*c - 32];
                c++;
        }
}

i32 SuiContext::button(const char* s)
{
        SuiAssert(s);
        u32 id = SuiHash(s);

        const char* c = s;
        i32         len = 0;
        while (*c) len += style.font.xadvs[*c++ - 32];

        SuiAssert(layouts.idx != 0);
        SuiLayout layout = layouts.pop();
        SuiRect   rect = get_rect(len, 16, SUI_ALIGNMENT_FLAG_LEFT, layout);
        SuiColor  color = style.colors[SUI_COLOR_BUTTON];

        if (io.mxy_in_rect(rect)) {
                hot_id = id;
                if (active_id == 0 && io.ldown) active_id = id;
        }

        if (hot_id == id) color = style.colors[SUI_COLOR_BUTTON_HOT];

        // SuiLayoutAction action = SUI_LAYOUT_ACTION_NEXT;
        rectcmds.push(SuiRectCommand(rect, color, 127));

        c = s;
        len = 0;
        while (*c) {
                SuiGlyph glyph = style.font.glyphs[*c - 32];
                SuiRect  rect = {layout.rect.x + len + glyph.xoff, layout.rect.y + glyph.yoff, glyph.w, glyph.h};
                rectcmds.push(SuiRectCommand(rect, style.colors[SUI_COLOR_TEXT], *c));
                len += style.font.xadvs[*c - 32];
                c++;
        }

        if (hot_id == id && active_id == id && io.lup) return 1;
        return 0;
}

void SuiContext::checkbox(const char* s, i32* const value)
{
        SuiAssert(s);
        u32 id = SuiHash(s);

        SuiAssert(layouts.idx != 0);
        SuiLayout layout = layouts.pop();
        SuiRect   rect = get_rect(16, 16, 0, layout);
        SuiRect   bbox = rect.scale(2);
        SuiRect   check = bbox.scale(2);

        if (io.mxy_in_rect(bbox)) {
                hot_id = id;
                if (active_id == 0 && io.ldown) active_id = id;
        }

        if (hot_id == id && active_id == id && io.lup) {
                if (*value) *value = 0;
                else *value = 1;
        }

        rectcmds.push(SuiRectCommand(rect, style.colors[SUI_COLOR_CHECKBOX], 127));
        rectcmds.push(SuiRectCommand(bbox, style.colors[SUI_COLOR_CHECKBOXBG], 127));
        if (*value) rectcmds.push(SuiRectCommand(check, style.colors[SUI_COLOR_CHECKBOX], 127));
}

void SuiContext::slider(const char* s, f32* const value)
{
        SuiAssert(s);
        u32 id = SuiHash(s);

        SuiAssert(layouts.idx != 0);
        SuiLayout layout = layouts.pop();

        const i32 w = 128, h = 2;
        // const i32 xoff = static_cast<const i32>(128.0f * *value);
        const i32 xoff = static_cast<const i32>(112.0f * *value);
        SuiRect   rect = get_rect(w, h, 0, layout);
        SuiRect   control = get_rect(16, 16, 0, layout);
        control.x += xoff - 56;

        if (io.mxy_in_rect(control)) {
                hot_id = id;
                if (active_id == 0 && io.ldown) active_id = id;
        }

        if (active_id == id) {
                *value -= (io.dmx / 128.0f);
                if (*value < 0.0f) *value = 0.0f;
                if (1.0f < *value) *value = 1.0f;
                if (io.mx < rect.x) *value = 0.0f;
                if (rect.x + rect.w < io.mx) *value = 1.0f;
        }

        rectcmds.push(SuiRectCommand(rect, style.colors[SUI_COLOR_SLIDERBG], 127));
        rectcmds.push(SuiRectCommand(control, style.colors[SUI_COLOR_SLIDER], 127));
}

void SuiContext::labelf(const char* s, ...)
{
        SuiAssert(s);
        
        char buffer[64];
        va_list args;
        va_start(args, s);
        i32 n = vsnprintf(buffer, 64 - 1, s, args);
        SuiAssert(n != -1);
        
        const char* c = buffer;
        i32         len = 0;
        while (*c) len += style.font.xadvs[*c++ - 32];

        SuiAssert(layouts.idx != 0);
        SuiLayout layout = layouts.pop();
        SuiRect   rect = get_rect(len, 16, SUI_ALIGNMENT_FLAG_LEFT, layout);
        SuiColor  color = style.colors[SUI_COLOR_LABELBG];

        rectcmds.push(SuiRectCommand(rect, color, 127));

        c = buffer;
        len = 0;
        while (*c) {
                const SuiGlyph glyph = style.font.glyphs[*c - 32];
                const SuiRect  grect = {rect.x + len + glyph.xoff, rect.y + glyph.yoff, glyph.w, glyph.h};
                rectcmds.push(SuiRectCommand(grect, style.colors[SUI_COLOR_TEXT], *c));
                len += style.font.xadvs[*c - 32];
                c++;
        }
}

void SuiContext::next()
{
        layouts.pop();
}

void SuiContext::reset()
{
        hot_id = 0;
        if (io.lup) active_id = 0;
        layouts.reset();
        rectcmds.reset();
}

// --------------------------------------------------------------------------------------------------------------------

#include <d3dcompiler.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

SuiVertex::SuiVertex(const f32 _x, const f32 _y, const f32 _u, const f32 _v, const SuiColor _color) : x(_x), y(_y), u(_u), v(_v), color(_color) {}

SuiBackendProfiler::SuiBackendProfiler(ID3D11Device* device)
{
        SuiAssert(device);
        memset(this, 0, sizeof(SuiBackendProfiler));

        HRESULT          hr;
        D3D11_QUERY_DESC query_desc = {D3D11_QUERY_TIMESTAMP, 0};

        hr = device->CreateQuery(&query_desc, &timestamp0);
        SuiAssert(hr == 0);
        query_desc.Query = D3D11_QUERY_TIMESTAMP;
        hr = device->CreateQuery(&query_desc, &timestamp1);
        SuiAssert(hr == 0);
        query_desc.Query = D3D11_QUERY_TIMESTAMP_DISJOINT;
        hr = device->CreateQuery(&query_desc, &disjointed);
        SuiAssert(hr == 0);
        query_desc.Query = D3D11_QUERY_PIPELINE_STATISTICS;
        hr = device->CreateQuery(&query_desc, &pipeline_stats);
        SuiAssert(hr == 0);
        query_desc.Query = D3D11_QUERY_SO_STATISTICS;
        hr = device->CreateQuery(&query_desc, &so_stats);
        SuiAssert(hr == 0);
}

void SuiBackendProfiler::begin(ID3D11DeviceContext* context)
{
        context->Begin(disjointed);
        context->End(timestamp0);

        context->Begin(pipeline_stats);
        context->Begin(so_stats);
}

void SuiBackendProfiler::end(ID3D11DeviceContext* context)
{
        context->End(timestamp1);
        context->End(disjointed);

        context->End(pipeline_stats);
        context->End(so_stats);
}

void SuiBackendProfiler::update(ID3D11DeviceContext* context)
{
        // get data
        u64 time0;
        while (context->GetData(timestamp0, &time0, sizeof(u64), 0) != S_OK)
                ;
        u64 time1;
        while (context->GetData(timestamp1, &time1, sizeof(u64), 0) != S_OK)
                ;
        D3D11_QUERY_DATA_TIMESTAMP_DISJOINT disjoint_data;
        while (context->GetData(disjointed, &disjoint_data, sizeof(D3D11_QUERY_DATA_TIMESTAMP_DISJOINT), 0) != S_OK)
                ;
        D3D11_QUERY_DATA_PIPELINE_STATISTICS pipeline_stats_data;
        while (context->GetData(pipeline_stats, &pipeline_stats_data, sizeof(D3D11_QUERY_DATA_PIPELINE_STATISTICS), 0) != S_OK)
                ;
        D3D11_QUERY_DATA_SO_STATISTICS so_stats_data;
        while (context->GetData(so_stats, &so_stats_data, sizeof(D3D11_QUERY_DATA_SO_STATISTICS), 0) != S_OK)
                ;

        // update data
        d64 delta = static_cast<d64>(time1 - time0);
        if (disjoint_data.Disjoint == FALSE) {
                d64 frequency = static_cast<d64>(disjoint_data.Frequency);
                time = (delta / frequency) * 1000.0;
        }

        // D3D11_QUERY_DATA_PIPELINE_STATISTICS
        ia_vertices = pipeline_stats_data.IAVertices;
        min_ia_vertices = SuiMin(min_ia_vertices, pipeline_stats_data.IAVertices);
        max_ia_vertices = SuiMax(max_ia_vertices, pipeline_stats_data.IAVertices);

        ia_primitives = pipeline_stats_data.IAPrimitives;
        min_ia_primitives = SuiMin(min_ia_primitives, pipeline_stats_data.IAPrimitives);
        max_ia_primitives = SuiMax(max_ia_primitives, pipeline_stats_data.IAPrimitives);

        vs_invocations = pipeline_stats_data.VSInvocations;
        min_vs_invocations = SuiMin(min_vs_invocations, pipeline_stats_data.VSInvocations);
        max_vs_invocations = SuiMax(max_vs_invocations, pipeline_stats_data.VSInvocations);

        ps_invocations = pipeline_stats_data.PSInvocations;
        min_ps_invocations = SuiMin(min_ps_invocations, pipeline_stats_data.PSInvocations);
        max_ps_invocations = SuiMax(max_ps_invocations, pipeline_stats_data.PSInvocations);

        cs_invocations = pipeline_stats_data.CSInvocations;
        min_cs_invocations = SuiMin(min_cs_invocations, pipeline_stats_data.CSInvocations);
        max_cs_invocations = SuiMax(max_cs_invocations, pipeline_stats_data.CSInvocations);

        // D3D11_QUERY_DATA_SO_STATISTICS
        num_primitives_written = so_stats_data.NumPrimitivesWritten;
        min_num_primitives_written = SuiMin(min_num_primitives_written, num_primitives_written);
        max_num_primitives_written = SuiMax(max_num_primitives_written, num_primitives_written);

        primitives_storage_needed = so_stats_data.PrimitivesStorageNeeded;
        min_primitives_storage_needed = SuiMin(min_primitives_storage_needed, primitives_storage_needed);
        max_primitives_storage_needed = SuiMax(max_primitives_storage_needed, primitives_storage_needed);
}

SuiBackend::SuiBackend(ID3D11Device* _device, const i32 x, const i32 y, const SuiUV* _uvs) : profiler(SuiBackendProfiler(_device)), uvs(_uvs)
{
        screen_x = x;
        screen_y = y;

        device = _device;
        device->GetImmediateContext(&context);

        // TODO: error blob and compile flags
        HRESULT   hr;
        ID3DBlob* blob = NULL;
        hr = D3DCompileFromFile(L"shaders\\SuiVertex.hlsl", NULL, NULL, "main", "vs_4_0", 0, 0, &blob, NULL);
        SuiAssert(hr == 0);
        hr = device->CreateVertexShader((void*)blob->GetBufferPointer(), blob->GetBufferSize(), NULL, &vertex_shader);
        SuiAssert(hr == 0);

        D3D11_INPUT_ELEMENT_DESC ieds[] = {
            {"POSITION", 0, DXGI_FORMAT_R32G32_FLOAT,   0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
            {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,   0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
            {"COLOR",    0, DXGI_FORMAT_R8G8B8A8_UNORM, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
        };
        hr = device->CreateInputLayout(ieds, 3, blob->GetBufferPointer(), blob->GetBufferSize(), &input_layout);
        SuiAssert(hr == 0);

        hr = D3DCompileFromFile(L"shaders\\SuiPixel.hlsl", NULL, NULL, "main", "ps_4_0", 0, 0, &blob, NULL);
        SuiAssert(hr == 0);
        hr = device->CreatePixelShader(blob->GetBufferPointer(), blob->GetBufferSize(), NULL, &pixel_shader);
        SuiAssert(hr == 0);

        blob->Release();

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

        hr = device->CreateBlendState(&bdesc, &blend_state);
        SuiAssert(hr == 0);

        // buffers
        D3D11_BUFFER_DESC bufdsc;
        bufdsc.ByteWidth = sizeof(SuiVertex) * SUI_VERTEX_SIZE;
        bufdsc.Usage = D3D11_USAGE_DYNAMIC;
        bufdsc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
        bufdsc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
        bufdsc.MiscFlags = 0;
        bufdsc.StructureByteStride = 0;

        hr = device->CreateBuffer(&bufdsc, NULL, &vertex_buffer);
        SuiAssert(hr == 0);

        u32 indices[] = {0, 1, 2, 0, 3, 1};

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

        hr = device->CreateBuffer(&bufdsc, &subrsc, &index_buffer);
        SuiAssert(hr == 0);

        // orthographic projection matrix
        float L = 0;
        float R = (f32)x;
        float T = 0;
        float B = (f32)y;
        float proj[4][4] = {
            {2.0f / (R - L),    0.0f,              0.0f, 0.0f},
            {0.0f,              2.0f / (T - B),    0.0f, 0.0f},
            {0.0f,              0.0f,              0.5f, 0.0f},
            {(R + L) / (L - R), (T + B) / (B - T), 0.5f, 1.0f}
        };

        bufdsc.ByteWidth = sizeof(proj);
        bufdsc.Usage = D3D11_USAGE_DEFAULT;
        bufdsc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        bufdsc.CPUAccessFlags = 0;
        bufdsc.MiscFlags = 0;
        bufdsc.StructureByteStride = 0;

        subrsc.pSysMem = proj;
        subrsc.SysMemPitch = 0;
        subrsc.SysMemSlicePitch = 0;

        hr = device->CreateBuffer(&bufdsc, &subrsc, &constant_buffer);
        SuiAssert(hr == 0);

        i32         w, h, n;
        const char* file = "Fixedsys.png";
        u8*         bmp = stbi_load(file, &w, &h, &n, 0);

        D3D11_TEXTURE2D_DESC texture_desc;
        texture_desc.Width = w;
        texture_desc.Height = h;
        texture_desc.MipLevels = 1;
        texture_desc.ArraySize = 1;
        texture_desc.Format = DXGI_FORMAT_R8_UNORM;
        texture_desc.SampleDesc = {1, 0};
        texture_desc.Usage = D3D11_USAGE_DEFAULT;
        texture_desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
        texture_desc.CPUAccessFlags = 0;
        texture_desc.MiscFlags = 0;

        D3D11_SUBRESOURCE_DATA sub_data;
        sub_data.pSysMem = bmp;
        sub_data.SysMemPitch = w;
        sub_data.SysMemSlicePitch = 0;

        ID3D11Texture2D* texture;
        hr = _device->CreateTexture2D(&texture_desc, &sub_data, &texture);
        SuiAssert(hr == 0);

        D3D11_SHADER_RESOURCE_VIEW_DESC srvd;
        srvd.Format = texture_desc.Format;
        srvd.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
        srvd.Texture2D = {0, 1};

        hr = _device->CreateShaderResourceView(texture, &srvd, &view);
        SuiAssert(hr == 0);

        D3D11_SAMPLER_DESC sampler_desc;
        memset(&sampler_desc, 0, sizeof(D3D11_SAMPLER_DESC));
        sampler_desc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
        sampler_desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
        sampler_desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
        sampler_desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
        // sampler_desc.MipLODBias = 0;
        // sampler_desc.MaxAnisotropy = 0;
        // sampler_desc.ComparisonFunc = 0;
        // sampler_desc.BorderColor = 0;
        // sampler_desc.MinLOD;
        // sampler_desc.MaxLOD;

        hr = _device->CreateSamplerState(&sampler_desc, &sampler);
        SuiAssert(hr == 0);

        texture->Release();
        stbi_image_free(bmp);
}

void SuiBackend::record(i32 n, const SuiRectCommand* rectcmds)
{
        SuiAssert(n * 4 < SUI_VERTEX_SIZE);

        HRESULT                  hr;
        D3D11_MAPPED_SUBRESOURCE vtx_rsc;
        hr = context->Map(vertex_buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &vtx_rsc);
        SuiAssert(hr == 0);

        // memcpy((char*)vtx_rsc.pData + n * sizeof(struct sui_vertex), vertices, n * sizeof(struct sui_vertex));
        SuiVertex* vertex = reinterpret_cast<SuiVertex*>(vtx_rsc.pData);
        for (i32 i = 0; i < n; i++) {
                f32 x0 = (f32)rectcmds[i].rect.x;
                f32 y0 = (f32)rectcmds[i].rect.y;
                f32 x1 = (f32)rectcmds[i].rect.x + (f32)rectcmds[i].rect.w;
                f32 y1 = (f32)rectcmds[i].rect.y + (f32)rectcmds[i].rect.h;

                SuiUV uv = uvs[rectcmds[i].c - 32];

                *vertex++ = SuiVertex(x0, y0, uv.u0, uv.v0, rectcmds[i].color);
                *vertex++ = SuiVertex(x1, y1, uv.u1, uv.v1, rectcmds[i].color);
                *vertex++ = SuiVertex(x0, y1, uv.u0, uv.v1, rectcmds[i].color);
                *vertex++ = SuiVertex(x1, y0, uv.u1, uv.v0, rectcmds[i].color);
        }

        context->Unmap(vertex_buffer, 0);
        vertices_count = n * 4;
}

void SuiBackend::draw()
{
        profiler.begin(context);

        context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        context->IASetInputLayout(input_layout);
        context->VSSetShader(vertex_shader, NULL, 0);
        context->PSSetShader(pixel_shader, NULL, 0);
        context->IASetIndexBuffer(index_buffer, DXGI_FORMAT_R32_UINT, 0);
        context->VSSetConstantBuffers(0, 1, &constant_buffer);
        context->OMSetBlendState(blend_state, 0, 0xffffffff);
        context->PSSetShaderResources(0, 1, &view);
        context->PSSetSamplers(0, 1, &sampler);

        u32 stride = sizeof(SuiVertex);
        u32 offset = 0;
        for (i32 i = 0; i < vertices_count; i += 4) {
                offset = i * stride;
                context->IASetVertexBuffers(0, 1, &vertex_buffer, &stride, &offset);
                context->DrawIndexed(6, 0, 0);
        }

        profiler.end(context);
        profiler.update(context);
}
