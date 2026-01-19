
// 상수 버퍼와 공용 구조체를 관리하기 위한 헤더

/// 공용 구조체
struct VertexIn
{
    float3 posL : POSITION;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float3 binormal : BINORMAL;
    float2 tex : TEXCOORD;
};

struct VertexOut
{
    float4 posH : SV_POSITION;
    float3 normal : NORMAL;
    float2 tex : TEXCOORD0;
    float4 worldPos : TEXCOORD1;
    float3 tangent : TANGENT;
    float3 binormal : BINORMAL;
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

/// 레지스터 영역
// 월드 변환 행렬
cbuffer cbPerObject : register(b0)
{
    matrix g_world;
    matrix g_worldInvers;
};

// 카메라 관련 구조체
cbuffer cbVP : register(b1)
{
    matrix g_view;
    matrix g_proj;
    float3 g_CameraPos;
    
    float pad;
}

// 빛의 갯수
cbuffer LightCount : register(b2)
{
    int g_NumDirectional;
    int g_NumPoint;
    int g_NumSpot;
};

// 사용할 텍스쳐
Texture2D g_diffuse : register(t0);
Texture2D g_normal : register(t1);
Texture2D g_roughness : register(t2);
Texture2D g_metalize : register(t3);
Texture2D g_AO : register(t4);

// 빛 배열
StructuredBuffer<DirectionLight> g_DirectionalLights : register(t5);
StructuredBuffer<PointLight> g_PointLights : register(t6);
StructuredBuffer<SpotLight> g_SpotLights : register(t7);

// 스카이박스 텍스쳐
TextureCube g_skyTextuer : register(t8);
TextureCube g_skyIrradianceMap : register(t9);
TextureCube g_skySpecularMap : register(t10);
Texture2D g_skyBrdfMap : register(t11);

// 샘플러
SamplerState samAnisotropic
{
    Filter = ANISOTROPIC;
    MaxAnisotropy = 4;

    AddressU = WRAP;
    AddressV = WRAP;
};

SamplerState samLinear : register(s1)
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = CLAMP;
    AddressV = CLAMP;
    AddressW = CLAMP;
};

SamplerState samPointClamp : register(s2)
{
    Filter = MIN_MAG_MIP_POINT;
    AddressU = CLAMP;
    AddressV = CLAMP;
    AddressW = CLAMP;
};


