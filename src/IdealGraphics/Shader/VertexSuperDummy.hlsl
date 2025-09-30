// InputLayout 생성을 위한 더미 VS
struct VSInput
{
    float3 pos : POSITION;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float2 uv : TEXCOORD0;
    float4 color : COLOR0;
    uint4 boneIdx : BLENDINDICES;
    float4 boneW : BLENDWEIGHT;
};

float4 main(VSInput input) 
    : SV_POSITION
{
    return float4(0, 0, 0, 1);
}