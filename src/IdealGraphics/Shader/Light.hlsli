#include "PBRHelper.hlsli"

struct LightInput
{
    float3 N; // pixel normal
    float3 L; // light vector
    float3 V; // view vector
    float3 H; // half vector of L, V
    float3 D; // direction of light
    
    float dis; // distance of pixel, object
    
    float att; // attenuation of light
    float range; // light range
    
    float inAngle; // in angle
    float outAngle; // out angle
};

void DirectionLigthCalculate(LightInput _input, inout float3 _diff, inout float3 _spec)
{
    _diff += max(dot(_input.N, _input.L), 0);
    _spec += pow(saturate(dot(_input.N, _input.H)), 32.0f);
    
    return;
}

void PointLightCalculate(LightInput _input, inout float3 _diff, inout float3 _spec)
{
    float ratio = saturate(_input.dis / _input.range);
    float baseA = 1.0 - ratio;
    float A = pow(baseA, _input.att);
    
    float diff = saturate(dot(_input.N, _input.L));
    float spec = pow(saturate(dot(_input.N, _input.H)), 16.0f);
    
    _diff += diff * A;
    _spec += spec * A;
    
    return;
}

void SpotLightCalculate(LightInput _input, inout float3 _diff, inout float3 _spec)
{
    float ratio = saturate(_input.dis / _input.range);
    float baseA = 1.0 - ratio;
    float A = pow(baseA, _input.att);
    
    float theta = dot(_input.L, normalize(-_input.D));
    
    float inner = _input.inAngle - _input.outAngle;
    float intensity = clamp((theta - _input.outAngle) / inner, 0.0f, 1.0f);
    
    float diff = saturate(dot(_input.N, _input.L));
    float spec = pow(saturate(dot(_input.N, _input.H)), 16.0f);
    
    _diff += diff * A * intensity;
    _spec += spec * A * intensity;
    
    return;
}

void DirectionLightPBR(
    LightInput _input
    , float3 F0
    , float _roughness
    , float _metalic
    , inout float3 _diff
    , inout float3 _spec
)
{
    float NdL = max(dot(_input.N, _input.L), 0.0f);
    float NdV = max(dot(_input.N, _input.V), 0.0001f);
    float NdH = max(dot(_input.N, _input.H), 0.0f);
    float HdV = max(dot(_input.H, _input.V), 0.0f);

    float D = ndfGGX(NdH, _roughness);
    float G = gaSchlickGGX(NdL, NdV, _roughness);
    float3 F = fresnelSchlick(F0, HdV);

    float3 BRDF = (D * G * F) / (4.0f * NdV * NdL + 0.0001f);
    
    float3 ks = F;
    float3 kd = (1.0f - ks) * (1.0f - _metalic);

    _diff += kd * NdL;
    _spec += BRDF * NdL;
    
    return;
}

