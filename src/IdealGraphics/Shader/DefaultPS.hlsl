Texture2D g_Texture : register(t0);
SamplerState g_Sampler : register(s0);

struct VertexOut
{
	float4 Pos : SV_POSITION;
	float3 PosW : POSITION;
	float2 Tex : TEXCOORD0;
};


float4 main(VertexOut pin) 
	: SV_TARGET
{
	// 텍스처에서 색상을 샘플링
	float4 texColor = g_Texture.Sample(g_Sampler, pin.Tex);
    texColor.a = 1.0f;
	
    return texColor;
}