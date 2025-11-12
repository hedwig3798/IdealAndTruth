#include "Buffers.hlsl"

float4 main(VertexOut pin) 
	: SV_TARGET
{
	// textuer sampling
	float4 texColor = g_Texture.Sample(g_Sampler, pin.tex);
    texColor.a = 1.0f;
    
    // direction light
    float3 N = normalize(pin.normal);
    float3 L = normalize(-g_DirectionalLights[0].m_direction);
    float3 V = normalize(g_CameraPos - pin.worldPos.xyz);
    float3 H = normalize(L + V);
    
    // calculate light
    float diff = saturate(dot(N, L));
    float spec = pow(saturate(dot(N, H)), 1.0f);
    
    // a, d, s
    float3 ambient = 0.1f * g_DirectionalLights[0].m_color;
    float3 diffuse = diff * g_DirectionalLights[0].m_color;
    float3 specular = spec * g_DirectionalLights[0].m_color;
    
    float3 finalColor = (ambient + diffuse + specular) * texColor.rgb;
    
    float4 result = float4(finalColor.xyz, 1);
    
    return result;
}