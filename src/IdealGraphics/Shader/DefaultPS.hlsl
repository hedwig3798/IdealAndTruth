#include "Buffers.hlsl"
#include "Light.hlsli"

float4 main(VertexOut pin) 
	: SV_TARGET
{
	// textuer sampling
    float4 texColor = g_diffuse.Sample(g_Sampler, pin.tex);
    texColor.a = 1.0f;
    
    // result diff, spec
    float diff = 0;
    float spec = 0;
    
    // direction light
    LightInput lightInput;
    
    // normal
    lightInput.N = normalize(pin.normal);
    float3 normalMap = g_normal.Sample(g_Sampler, pin.tex).rgb;
    normalMap = (normalMap * 2) - 1;
    
    float4 testNormal = g_normal.Sample(g_Sampler, pin.tex);
    
    lightInput.N 
    = normalMap.x * pin.tangent 
    + normalMap.y * pin.binormal 
    + normalMap.z * lightInput.N;
    
    
    lightInput.V = normalize(g_CameraPos - pin.worldPos.xyz);
    [loop]
    for (int d = 0; d < g_NumDirectional; ++d)
    {
        lightInput.L = normalize(-g_DirectionalLights[d].m_direction);
        lightInput.H = normalize(lightInput.L + lightInput.V);
        DirectionLigthCalculate(lightInput, diff, spec);
    }
    
    // point light
    [loop]
    for (int p = 0; p < g_NumPoint; ++p)
    {
        float3 lightDirVec = g_PointLights[p].m_position - pin.worldPos.xyz;
        lightInput.D = length(lightDirVec);
        lightInput.L = normalize(lightDirVec);
        lightInput.H = normalize(lightInput.L + lightInput.V);
        lightInput.range = g_PointLights[p].m_range;
        lightInput.att = g_PointLights[p].m_attenuation;
        
        // PointLightCalculate(lightInput, diff, spec);
    }
    
    
    // spot light
    [loop]
    for (int s = 0; s < g_NumSpot; ++s)
    {
        float3 lightDirVec = g_PointLights[p].m_position - pin.worldPos.xyz;
        lightInput.D = length(lightDirVec);
        lightInput.L = normalize(lightDirVec);
        lightInput.H = normalize(lightInput.L + lightInput.V);
        lightInput.att = g_SpotLights[s].m_attenuation;
        
        // SpotLightCalculate(lightInput, diff, spec);
    }
    
    // a, d, s
    float3 ambient = 0.2f;
    float3 diffuse = diff;
    float3 specular = spec;
    
    float3 finalColor = (ambient + diffuse + specular) * texColor.rgb;
    // finalColor = pow(finalColor, 1.0f / 2.2f);
    
    float4 result = float4(finalColor.xyz, 1);
    // float4 result = float4(texColor.xyz, 1);
    
    return result;
}