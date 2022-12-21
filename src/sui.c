#include "sui.h"

#include <string.h>
#define HANDMADE_MATH_IMPLEMENTATION
#include "HandmadeMath.h"

// INTERNAL FUNCTION

i64 sui_time_begin()
{
	LARGE_INTEGER frequency;
	QueryPerformanceFrequency(&frequency);
	LARGE_INTEGER t;
	QueryPerformanceCounter(&t);
	return t.QuadPart;
}

i64 sui_time_end(i64 begin)
{
	LARGE_INTEGER frequency;
	QueryPerformanceFrequency(&frequency);
	LARGE_INTEGER time;
	QueryPerformanceCounter(&time);
	i64 res = time.QuadPart;
	res -= begin;
	res *= 1000000;
	res /= frequency.QuadPart;
	return res;
}

i32 sui_hash(char* str, i32 mask)
{
	i32 hash = 0x4e910a7c;
	while (*str) {
		hash += ((hash << 11) + (hash << 7) - (mask ^ str) + (i32)*str << 3);
		hash ^= mask;
	}
	return hash;
}

// USER FUNCTIONS

void sui_init(struct sui_context* sui, ID3D11Device* d11device, i32 w, i32 h)
{
	memset(sui, 0, sizeof(struct sui_context));

	sui->vertices = (struct sui_vertex*)malloc(sizeof(struct sui_vertex) * 2048);
	sui->vlen = 0;

	sui->mask = 0x11335577;
	sui->layouts_len = 32;
	sui->layouts = (struct sui_layout*)malloc(32 * sizeof(struct sui_layout));
	sui->items_len = 128;
	sui->items = (struct sui_item*)malloc(128 * sizeof(struct sui_item));
	sui->viewport = (struct sui_viewport){ w, h };
}

void sui_terminate(struct sui_context* sui)
{
	sui->vlen = 0;
	free(sui->vertices);

	memset(sui, 0, sizeof(struct sui_context));
}

void sui_input(struct sui_context* sui, i16 mx, i16 my, u8 rdown, u8 rup, u8 ldown, u8 lup)
{
	sui->io.dmx = sui->io.mx - mx;
	sui->io.dmy = sui->io.my - my;

	sui->io.mx = mx;
	sui->io.my = my;

	sui->io.rdown = rdown;
	sui->io.rup = rup;
	sui->io.ldown = ldown;
	sui->io.lup = lup;
	sui->io.rclicked = 0;
	sui->io.lclicked = 0;

	if (rdown) sui->io.rheld = 1; 
	else if (rup) sui->io.rheld = 0; 

	if (ldown) sui->io.lheld = 1; 
	else if (lup) sui->io.lheld = 0; 

	if (rdown) sui->io.rclick_time = sui_time_begin();
	if (rup) sui->io.rclick_time = sui_time_end(sui->io.rclick_time);
	
	if (ldown) sui->io.lclick_time = sui_time_begin();
	if (lup) sui->io.lclick_time = sui_time_end(sui->io.lclick_time);

	if (sui->io.rclick_time != 0 && sui->io.rclick_time < 200000) {
		sui->io.rclicked = 1;
		sui->io.rclick_time = 0;
	}
	if (sui->io.lclick_time != 0 && sui->io.lclick_time < 200000) {
		sui->io.lclicked = 1;
		sui->io.lclick_time = 0;
	}
}

void sui_begin(struct sui_context* sui, const char* name)
{
	i32 id = sui_hash((char*)name, sui->mask) % sui->items_len;
	if (!sui->layouts[id]) sui_create_layout(sui, name, id);
	sui->current_layout = id;
}

void sui_end(struct sui_context* sui)
{
	struct sui_layout* layout = sui->layouts[sui->current_id];
	// sui_draw_rect(layout->rect, layout->color);
	sui_draw_layout(sui, layout);
	sui->current_layout = -1;
}

void sui_style(struct sui_context* sui, struct sui_style* style)
{
	sui->style = *style;
}

void sui_row(struct sui_context* sui)
{
	// add a new row and/or change item insertion mode into row oriented
}

void sui_empty_row(struct sui_context* sui)
{
	// add a new empty row
}

void sui_column(struct sui_context* sui)
{
	// add a new column and/or change item insertion mode into column oriented
}

void sui_empty_column(struct sui_context* sui)
{
	// add a new empty column
}

void sui_label(struct sui_context* sui, const char* name)
{
	i32 id = sui_hash(name, sui->mask) % sui->items_len;
	if (!sui->items[id]) sui_create_button(sui, name, id);
	struct sui_item* item = sui->items[id];
	sui_draw_text(sui, item, name);
}

void sui_text(struct sui_context* sui, const char* text, ...)
{
	i32 id = sui_hash(name, sui->mask) % sui->items_len;
	if (!sui->items[id]) sui_create_button(sui, name, id);
	struct sui_item* item = sui->items[id];
	char buffer[SUI_BUFFER_SIZE];
	// memset(buffer, 0, SUI_BUFFER_SIZE);
	snprintf(buffer, SUI_BUFFER_SIZE, name, ...);
	sui_draw_text(sui, item, buffer);
}

i32 sui_button(struct sui_context* sui, const char* name)
{
	i32 id = sui_hash(name, sui->mask) % sui->items_len;
	if (!sui->items[id]) sui_create_button(sui, name, id);
	struct sui_item* item = sui->items[id];

	if (sui_region_hit(sui->io, item) && sui->active_item == -1) {
		sui->hot_item = id;
		if (sui->io.pressed) sui->active_item = id;
	}
	i32 ret = 0;
	if (sui->io.released && sui->hot_item == id && sui->active_item == id) { 
		ret = 1;
		sui->active_item = -1;
	}
	sui_draw_button(sui, item, ret);
	return 1;
}

void sui_checkbox(struct sui_context* sui, const char* name, i32* bool)
{
	i32 id = sui_hash(name, sui->mask) % sui->items_len;
	if (!sui->items[id]) sui_create_checkbox(sui, name, id);
	struct sui_item* item = sui->items[id];

	if (sui_region_hit(sui->io, item) && sui->active_item == -1) {
		sui->hot_item = id;
		if (sui->io.pressed) sui->active_item = id;
	}
	if (sui->io.released && sui->hot_item == id && sui->active_item == id) {
		if (*bool) *bool = 0;
		else *bool = 1;
		sui->active_item = -1;
	}
	sui_draw_checkbox(sui, item, *bool);
}

void sui_slider(struct sui_context* sui, const char* name, f32* value)
{
	i32 id = sui_hash(name, sui->mask) % sui->items_len;
	if (!sui->items[id]) sui_create_slider(sui, name, id);
	struct sui_item* item = sui->items[id];

	if (sui_region_hit(sui->io, item) && sui->active_item == -1) {
		sui->hot_item = id;
		if (sui->io.pressed) sui->active_item = id;
	}
	if (sui->hot_item == id && sui->active_item == i) {
		value = sui_slide_value(sui, *value);
	}
	if (sui->io.released && sui->hot_item == id && sui->active_item == id) {
		sui->active_item = -1;
	}
	sui_draw_slider(sui, item, *value);
}

/* struct sui_widget sui_create_widget(char* str, struct sui_color color, struct sui_color hover_color,
                                    struct sui_color bg_color, struct sui_color hover_bg_color)
{
        struct sui_widget widget;
        widget.color = color;
        widget.hover_color = hover_color;
        widget.bg_color = bg_color;
        widget.hover_bg_color = hover_bg_color;
        widget.pad_left = 0.0f;
        widget.pad_right = 0.0f;
        widget.pad_top = 0.0f;
        widget.pad_bottom = 0.0f;
        widget.margin_left = 10.0f;
        widget.margin_right = 10.0f;
        widget.margin_top = 10.0f;
        widget.margin_bottom = 10.0f;
        return widget;
}

struct sui_window sui_create_window(char* str, struct sui_color color, struct sui_color hover_color,
                                    struct sui_color bg_color, struct sui_color hover_bg_color)
{
        struct sui_window window;
        window.widget = sui_create_widget(str, color, hover_color, bg_color, hover_bg_color);
        window.p_vertex = 0;
        window.root = NULL;
        window.rect = (union sui_rect){ 0.0f, 0.0f, 0.0f, 0.0f };        
        window.bbox = (union sui_rect){ 0.0f, 0.0f, 0.0f, 0.0f };        
	window.current_w = 0.0f;
	window.current_h = 0.0f;
	window.current_max_h = 0.0f;
	window.state = (struct sui_state){ 0, 0, 0, 0, 0, 0, 0 };
        return window;
}

void sui_begin(struct sui_context* sui, struct sui_window* window, f32 x, f32 y)
{
        sui_assert(sui);
        sui_assert(window);
        
        window->root = sui->current_window;
	if (sui->current_window) {
		// update root
		x += window->widget.margin_left;
		y += window->widget.margin_top;
		sui->current_window->current_w += x;
		sui->current_window->current_h += y;
		sui_update_window(sui->current_window);
		
		// update position relative to root window
		x += sui->current_window->rect.x;
		x += sui->current_window->current_w;
		y += sui->current_window->rect.y;
		y += sui->current_window->current_h;

	}
        window->rect.x = x;
        window->rect.y = y;
        window->p_vertex = sui->vertices + sui->vlen;
	window->current_w = 0.0f;
	window->current_h = 0.0f;
	window->current_max_h = 0.0f;
        
        sui->current_window = window;       
        sui->vlen += 4;
}

void sui_end(struct sui_context* sui)
{
        sui_assert(sui);
        
        struct sui_window* window = sui->current_window;
	sui_update_window(window);
	
	union sui_rect rect, bbox;
	rect = window->rect;
	bbox = (union sui_rect){ rect.x, rect.x + rect.w, rect.y, rect.y + rect.h };
	
	window->state = sui_handle_state(bbox, window->state, sui->io);
	
	struct sui_color color = window->widget.color;
	if (window->state.hovering) {
		color = window->widget.hover_color;
	}
	
        sui_putr(window->p_vertex, window->rect, color);
	
        window->rect = (union sui_rect){ 0.0f, 0.0f, 0.0f, 0.0f };
	window->current_w = 0.0f;
	window->current_h = 0.0f;
	window->current_max_h = 0.0f;
	
        sui->current_window = window->root;
	if (window->root) {
		// update root
		sui_update_root(window->root, &window->widget, rect);
		sui_update_window(window->root);
		window->root = NULL;
	}
}

void sui_row(struct sui_context* sui)
{
	sui_assert(sui);
        struct sui_window* window = sui->current_window;
	sui_update_window(window);
}

void sui_update_window(struct sui_window* window)
{
	sui_assert(window);
	
	// update current rect
	// window->rect.x += window->widget.margin_left;
	// window->rect.y += window->widget.margin_top;
	window->rect.w = sui_max(window->current_w, window->rect.w);
	window->rect.h += window->current_max_h;
	// reset/update control points
	window->current_w = 0.0f;
	window->current_h = window->rect.h;
	window->current_max_h = 0.0f;
}

union sui_rect sui_getuv(char c, f32 w, f32 h)
{
        struct sui_glyph glyph = cdata[c - 32];
        
        f32 tmp_x = glyph.x / w;
        f32 tmp_y = glyph.y / h;
        f32 tmp_w = glyph.width / w;
        f32 tmp_h = glyph.height / h;    
        
        union sui_rect uv;
        uv.u0 = tmp_x;
        uv.u1 = tmp_x + tmp_w;
        uv.v0 = tmp_y;
        uv.v1 = tmp_y + tmp_h;
        
        return uv;
}

void sui_putr(struct sui_vertex* vertex, union sui_rect rect, struct sui_color color)
{
        f32 x0 = rect.x;
        f32 x1 = rect.x + rect.w;
        f32 y0 = rect.y;
        f32 y1 = rect.y + rect.h;

        // TODO: deal with different w ja h for different font atlases
        union sui_rect uv = sui_getuv(127, 128.0f, 64.0f);

	*vertex++ = (struct sui_vertex){ x0, y0, uv.u0, uv.v0, color.r, color.g, color.b, color.a };
	*vertex++ = (struct sui_vertex){ x1, y1, uv.u1, uv.v1, color.r, color.g, color.b, color.a };
	*vertex++ = (struct sui_vertex){ x0, y1, uv.u0, uv.v1, color.r, color.g, color.b, color.a };
	*vertex++ = (struct sui_vertex){ x1, y0, uv.u1, uv.v0, color.r, color.g, color.b, color.a };
}

f32 sui_putc(struct sui_vertex* vertex, char c, f32 x, f32 y, struct sui_color color)
{
        const f32 img_w = 128.0f;
        const f32 img_h = 64.0f;
        struct sui_glyph glyph = cdata[c - 32];

        union sui_rect rect = { x + glyph.xoffset, y + glyph.yoffset, glyph.width, glyph.height };
        
        f32 tmp_x = glyph.x / img_w;
        f32 tmp_y = glyph.y / img_h;
        f32 tmp_w = glyph.width / img_w;
        f32 tmp_h = glyph.height / img_h;
        
        union sui_rect uv;
        uv.u0 = tmp_x;
        uv.u1 = tmp_x + tmp_w;
        uv.v0 = tmp_y;
        uv.v1 = tmp_y + tmp_h;
        
	*vertex++ = (struct sui_vertex){ rect.x, rect.y, uv.u0, uv.v0, color.r, color.g, color.b, color.a };
	*vertex++ = (struct sui_vertex){ rect.x + rect.w, rect.y + rect.h, uv.u1, uv.v1, color.r, color.g, color.b, color.a };
	*vertex++ = (struct sui_vertex){ rect.x, rect.y + rect.h, uv.u0, uv.v1, color.r, color.g, color.b, color.a };
	*vertex++ = (struct sui_vertex){ rect.x + rect.w, rect.y, uv.u1, uv.v0, color.r, color.g, color.b, color.a };

        return (f32)glyph.xadvance;
} */

/* struct sui_state sui_handle_state(union sui_rect bbox, struct sui_state prev, struct sui_io io)
{
	// TODO: refactor
	struct sui_state state = { 0, 0, 0, 0, 0, prev.on, prev.dragged };
	if (io.lup) state.dragged = 0;
	if (!(bbox.x0 < io.mx && bbox.x1 > io.mx && bbox.y0 < io.my && bbox.y1 > io.my)) {
		return state;
	}
	
	state.pressed = io.ldown;
	state.held = prev.held;
	if (io.ldown) { state.held = 1; state.dragged = 1; }
	if (io.lup && state.held) state.released = 1;
	state.hovering = 1;
	if (state.released && io.lclicked) state.clicked = 1;
	
	if (state.clicked && state.on == 0) state.on = 1;
	else if (state.clicked && state.on == 1) state.on = 0;
	
	return state;
}

i32 sui_string_len(char* str)
{
	i32 len = 0;
	while (*str) { len += cdata[*str++ - 32].xadvance; }
	return len;
}

void sui_get_params(struct sui_window* root, struct sui_widget* widget, char* str, union sui_rect* rect, union sui_rect* bbox)
{
	// NOTE: this is only for a button and a label
	sui_assert(root);
	
        union sui_rect root_rect = root->rect;
	f32 x = root_rect.x + root->current_w + widget->margin_left;
	f32 y = root_rect.y + root->current_h + widget->margin_top;
	f32 w = widget->pad_left + widget->pad_right + (f32)sui_string_len((char*)str);
	f32 h = 16.0f + widget->pad_top + widget->pad_bottom;
	*rect = (union sui_rect){ x, y, w, h };
	*bbox = (union sui_rect){ x, x + w, y, y + h };
}

void sui_draw_rect(struct sui_context* sui, union sui_rect rect, struct sui_color color)
{
	struct sui_vertex* vertex = sui->vertices + sui->vlen;
	sui->vlen += 4;
	sui_putr(vertex, rect, color);
}

void sui_draw_string(char* str, struct sui_context* sui, struct sui_widget* widget, union sui_rect rect, struct sui_color color)
{
	struct sui_vertex* vertex = sui->vertices + sui->vlen;
	sui->vlen += 4;
	
	f32 x = rect.x + widget->pad_left;
	f32 y = rect.y + widget->pad_top;
	f32 w = 0.0f;
	while (*str) { 
		w += sui_putc(vertex, *str++, x + w, y, color); 
		vertex +=4; 
		sui->vlen += 4;
	}
}

void sui_update_root(struct sui_window* root, struct sui_widget* widget, union sui_rect rect)
{
	// paddings should already be summed into width
	root->current_w = (root->current_w + rect.w) + (widget->margin_left + widget->margin_right);
	root->current_max_h = sui_max((rect.h + widget->margin_top + widget->margin_bottom), root->current_max_h);
}

void sui_button(struct sui_context* sui, struct sui_widget* widget, const char* str, struct sui_state* state)
{
        sui_assert(sui);
        sui_assert(widget);
	sui_assert(state);

	struct sui_window* root = sui->current_window;
	
	union sui_rect rect, bbox;
	sui_get_params(root, widget, (char*)str, &rect, &bbox);

	*state = sui_handle_state(bbox, *state, sui->io);
        struct sui_color bg_color = widget->bg_color;
	struct sui_color color = widget->color;
	if (state->hovering) {
		bg_color = widget->hover_bg_color;
		color = widget->hover_color;
	}
	
	sui_draw_rect(sui, rect, bg_color);
	sui_draw_string((char*)str, sui, widget, rect, color);
	sui_update_root(root, widget, rect);
}

void sui_label(struct sui_context* sui, struct sui_widget* widget, const char* str)
{
        sui_assert(sui);
        sui_assert(widget);

	struct sui_window* root = sui->current_window;
	
	union sui_rect rect, bbox;
	sui_get_params(root, widget, (char*)str, &rect, &bbox);

        struct sui_color bg_color = widget->bg_color;
	struct sui_color color = widget->color;
	
	sui_draw_rect(sui, rect, bg_color);
	sui_draw_string((char*)str, sui, widget, rect, color);
	sui_update_root(root, widget, rect);
}

void sui_checkbox(struct sui_context* sui, struct sui_widget* widget, struct sui_state* state)
{
        sui_assert(sui);
        sui_assert(widget);
	sui_assert(state);

	struct sui_window* root = sui->current_window;
	
        union sui_rect root_rect = root->rect;
	union sui_rect rect0, rect1, rect2, bbox;
	
	// add padding to rects
	f32 x = root_rect.x + root->current_w + widget->margin_left;
	f32 y = root_rect.y + root->current_h + widget->margin_top;
	f32 w = 16.0f + widget->pad_left + widget->pad_right;
	f32 h = 16.0f + widget->pad_top + widget->pad_bottom;
	rect0 = (union sui_rect){ x, y, w, h };
	rect1 = (union sui_rect){ x + 2.0f, y + 2.0f, w - 4.0f, h - 4.0f };
	rect2 = (union sui_rect){ x + 3.0f, y + 3.0f, w - 6.0f, h - 6.0f };
	bbox = (union sui_rect){ rect1.x, rect1.x + rect1.w, rect1.y, rect1.y + rect1.h };
	
	*state = sui_handle_state(bbox, *state, sui->io);
	
        struct sui_color bg_color = widget->bg_color;
	struct sui_color color = widget->color;
	if (state->hovering) {
		bg_color = widget->hover_bg_color;
		color = widget->hover_color;
	}
	
	sui_draw_rect(sui, rect0, bg_color);
	sui_draw_rect(sui, rect1, color);
	if (state->on) sui_draw_rect(sui, rect2, bg_color);
	sui_update_root(root, widget, rect0);
}

void sui_slider(struct sui_context* sui, struct sui_widget* widget, struct sui_state* state, f32* value)
{
        sui_assert(sui);
        sui_assert(widget);
	sui_assert(state);
	sui_assert(value);
	sui_assert(0.0f <= *value && *value <= 1.0f);

	struct sui_window* root = sui->current_window;
	
        union sui_rect root_rect = root->rect;
	union sui_rect rect, slider0, slider1, control, bbox;

	// whole slider width
	f32 wsw = 150.0f;
	// whole slider height
	f32 wsh = 16.0f;
	// slider control offset
	f32 sco = wsw * *value;
	
	// TODO: add padding to rects
	f32 x = root_rect.x + root->current_w + widget->margin_left;
	f32 y = root_rect.y + root->current_h + widget->margin_top;
	f32 w = wsw + widget->pad_left + widget->pad_right;
	f32 h = wsh + widget->pad_top + widget->pad_bottom;
	// rect is a box, in where slider resides
	rect = (union sui_rect){ x, y, w, h };
	slider0 = (union sui_rect){ x, y + 6.0f, w, 4.0f };
	slider1 = (union sui_rect){ x + sco, y + 6.0f, w - sco, 4.0f };
	control = (union sui_rect){ x + sco - 8.0f, y, 16.0f, 16.0f };
	bbox = (union sui_rect){ control.x, control.x + control.w, control.y, control.y + control.h };
	
	*state = sui_handle_state(bbox, *state, sui->io);

	if (state->dragged) {
		if (sui->io.mx < x && 0.0f == *value) goto SUI_SLIDER_FINAL;
		if (x + w < sui->io.mx && 1.0f == *value) goto SUI_SLIDER_FINAL;
		*value = (sco - sui->io.dmx) / wsw;
		if (*value < 0.0f) *value = 0.0f;
		if (1.0f < *value) *value = 1.0f;
	}
	
	SUI_SLIDER_FINAL:
        struct sui_color bg_color = widget->bg_color;
        struct sui_color hover_bg_color = widget->hover_bg_color;
	struct sui_color color = widget->color;
	if (state->hovering) color = widget->hover_color;
	
	sui_draw_rect(sui, slider0, bg_color);
	sui_draw_rect(sui, slider1, hover_bg_color);
	sui_draw_rect(sui, control, color);
	sui_update_root(root, widget, rect);
} */

void sui_render(struct sui_context* sui)
{
	sui->vlen = 0;
}