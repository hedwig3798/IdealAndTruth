
// 상수 버퍼를 통합으로 관리하기 위한 헤더 파일

cbuffer cbPerObject : register(b0)
{
    matrix g_world;
};

cbuffer cbVP : register(b1)
{
    matrix g_view;
    matrix g_proj;
    float3 g_CameraPos;
    
    float pad;
}

cbuffer LightCount : register(b2)
{
    int g_NumDirectional;
    int g_NumPoint;
    int g_NumSpot;
};

struct DirectionLight
{
    float3 m_direction;
    float3 m_color;
    float m_intensity;
    
    float pad;
};

struct PointLight
{
    float3 m_position;
    float3 m_color;
    float m_attenuation;
    float m_intensity;
    float m_range;
    
    float3 pad;
};

struct SpotLight
{
    float3 m_direction;
    float3 m_position;
    float3 m_color;
    float m_attenuation;
    float m_inAngle;
    float m_outAngle;
    float m_intensity;
    
    float3 pad;
};

Texture2D g_Texture : register(t0);

StructuredBuffer<DirectionLight> g_DirectionalLights : register(t1);
StructuredBuffer<PointLight> g_PointLights : register(t2);
StructuredBuffer<SpotLight> g_SpotLights : register(t3);

TextureCube g_skyTextuer : register(t4);


SamplerState g_Sampler : register(s0);


struct VertexIn
{
    float3 posL : POSITION;
    float3 normal : NORMAL;
    float2 tex : TEXCOORD;
};

struct VertexOut
{
    float4 posH : SV_POSITION;
    float3 normal : NORMAL;
    float2 tex : TEXCOORD0;
    float4 worldPos : TEXCOORD1;
};