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
    
	// ���
    VertexOut vout;
    
	// ������ ����Ʈ ��ǥ
    vout.PosH = mul(float4(vin.PosL, 1.0f), wvp);

	// �ؽ����� ��ǥ (texcoord�� �״�� �ȼ����̴��� �Ѱ� �ش�)
    vout.Tex = vin.Tex;
    
    return vout;
}