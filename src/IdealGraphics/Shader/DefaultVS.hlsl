cbuffer cbPerObject : register(b0)
{
    matrix g_world;
};

cbuffer cbVP : register(b1)
{
    matrix g_view;
    matrix g_proj;
}

struct VertexIn
{
    float3 PosL : POSITION;
    float2 Tex : TEXCOORD;
};

struct VertexOut
{
    float4 PosH : SV_POSITION;
    float2 Tex : TEXCOORD;
};

VertexOut main(VertexIn vin)
{
    matrix wvp = g_world * g_view * g_proj;
    
	// 출력
    VertexOut vout;
    
	// 정점의 뷰포트 좌표
    vout.PosH = mul(float4(vin.PosL, 1.0f), wvp);

	// 텍스쳐의 좌표 (texcoord를 그대로 픽셀쉐이더로 넘겨 준다)
    vout.Tex = vin.Tex;
    
    return vout;
}