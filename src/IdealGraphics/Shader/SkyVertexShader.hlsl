#include "Buffers.hlsl"

struct SkyboxInput
{
    float3 PosL : POSITION;
    float2 panding : TEXCOORD;
};

struct SkyboxOutput
{
    float4 PosH : SV_POSITION;
    float3 PosL : POSITION;
};

SkyboxOutput main(SkyboxInput _input)
{
    SkyboxOutput vout;
    
    float4x4 viewNoTranslate = g_view;
    viewNoTranslate._41 = 0.0f;
    viewNoTranslate._42 = 0.0f;
    viewNoTranslate._43 = 0.0f;
    
    vout.PosH = mul(float4(_input.PosL, 1.0f), mul(viewNoTranslate, g_proj)).xyww;
    vout.PosH.z = vout.PosH.w * 0.99999f;
    vout.PosL = _input.PosL;

    return vout;
}