Texture2D tex;
SamplerState sampl;

struct PS_INPUT {
	float2 uv : TEXCOORD0;
	float4 color : COLOR0;
};

float4 main(PS_INPUT psin) : SV_TARGET {
	float a = tex.Sample(sampl, psin.uv).x;
	return psin.color * a;
}