cbuffer ConstantBuffer : register(b0) {
	float4x4 proj;
};

struct VS_INPUT {
	float2 position : POSITION0;
	float4 color : COLOR0;
};

struct VS_OUTPUT {
	float4 color : COLOR0;
	float4 position : SV_POSITION;
};

VS_OUTPUT main(VS_INPUT vsin) {
	VS_OUTPUT vsout;
	vsout.position = mul(proj, float4(vsin.position, 0.0, 1.0));
	vsout.color = vsin.color;
	return vsout;
}