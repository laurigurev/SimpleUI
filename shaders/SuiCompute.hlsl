struct Vertex {
	float2 position;
	float2 uv;
	float4 color;
};

struct UV {
	float u0;
	float u1;
	float v0;
	float v1;
};

struct Rect {
	int x;
	int y;
	int w;
	int h;
	uint color;
	int c;
};

StructuredBuffer<UV> inBuffer0 : register(t0);
StructuredBuffer<Rect> inBuffer1 : register(t1);
RWStructuredBuffer<Vertex> outBuffer : register(u0);

float4 read_color(uint char_color) {
	float4 out_color;
	out_color.r = float(char_color & 0x000000ff) / 255.0f;
	out_color.g = float((char_color & 0x0000ff00) >> 8) / 255.0f;
	out_color.b = float((char_color & 0x00ff0000) >> 16) / 255.0f;
	out_color.a = float((char_color & 0xff000000) >> 24) / 255.0f;
	return out_color;
}

[numthreads(1, 1, 1)]
void main(uint3 dtid : SV_DispatchThreadID) {
	// load rect
	Rect rect = inBuffer1[dtid.x];
	UV uv = inBuffer0[rect.c - 32];
	
	float x0 = float(rect.x);
	float y0 = float(rect.y);
	float x1 = float(rect.w) + x0;
	float y1 = float(rect.h) + y0;
	
	Vertex vertices[4];
	
	vertices[0].position = float2(x0, y0);
	vertices[0].uv = float2(uv.u0, uv.v0);
	vertices[0].color = read_color(rect.color);
	
	vertices[1].position = float2(x1, y1);
	vertices[1].uv = float2(uv.u1, uv.v1);
	vertices[1].color = read_color(rect.color);
	
	vertices[2].position = float2(x0, y1);
	vertices[2].uv = float2(uv.u0, uv.v1);
	vertices[2].color = read_color(rect.color);
	
	vertices[3].position = float2(x1, y0);
	vertices[3].uv = float2(uv.u1, uv.v0);
	vertices[3].color = read_color(rect.color);

	outBuffer[(dtid.x * 4) + 0] = vertices[0];
	outBuffer[(dtid.x * 4) + 1] = vertices[1];
	outBuffer[(dtid.x * 4) + 2] = vertices[2];
	outBuffer[(dtid.x * 4) + 3] = vertices[3];
}
