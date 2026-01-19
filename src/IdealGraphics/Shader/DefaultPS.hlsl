#include "Buffers.hlsl"
#include "Light.hlsli"

float4 main(VertexOut pin) 
	: SV_TARGET
{
	// textuer sampling
    float4 texColor = g_diffuse.Sample(samAnisotropic, pin.tex);
    texColor.a = 1.0f;
    
    float metalic = g_metalize.Sample(samAnisotropic, pin.tex).x;
    // metalic = 0;
    
    float AO = g_AO.Sample(samAnisotropic, pin.tex).x;
    
    float3 F0 = lerp(Fdielectric, texColor.xyz, metalic);
    
    float roughness = g_roughness.Sample(samAnisotropic, pin.tex).x;
    // roughness = 0;
    
    // direction light
    LightInput lightInput;
    float3 directLighting = 0.0;
    
    // normal
    float3 normalMap = g_normal.Sample(samAnisotropic, pin.tex).rgb;
    normalMap = normalMap * 2.0f - 1.0f;
    float3x3 TBN = float3x3(pin.tangent, pin.binormal, pin.normal);
    float3 N = normalize(mul(normalMap, TBN));
    
    float3 Lo = normalize(g_CameraPos - pin.worldPos.xyz);
    
    float cosLo = max(0.0f, dot(N, Lo));
    
    float3 Lr = 2.0 * cosLo * N - Lo;
    
    float3 Li = -g_DirectionalLights[0].m_direction;
    float3 Lradiance = g_DirectionalLights[0].m_intensity;
    
    float3 Lh = normalize(Li + Lo);
    
    float cosLi = max(0.0, dot(N, Li));
    float cosLh = max(0.0, dot(N, Lh));
    
    float3 F = fresnelSchlick(F0, max(0.0, dot(Lh, Lo))); //최소값 F0 , 최대값은 1.0,1.0,1.0  
    float D = ndfGGX(cosLh, max(0.1, roughness)); // 러프니스 0 이되면 값이0이 되므로 0이면 최소값사용 쉐이더토이도 0.1이다 
    float G = gaSchlickGGX(cosLi, cosLo, roughness);
    float3 kd = lerp(float3(1, 1, 1) - F, float3(0, 0, 0), metalic);
    float3 diffuseBRDF = kd * texColor.rgb / PI;
    float3 specularBRDF = (F * D * G) / max(Epsilon, 4.0 * cosLi * cosLo);
    directLighting += (diffuseBRDF + specularBRDF) * Lradiance * cosLi;
    
    float3 ambientLighting = 0;
    
    {
        float3 irradiance = g_skyIrradianceMap.Sample(samLinear, N).rgb;
        float3 F = fresnelSchlick(F0, cosLo);
        float3 kd = lerp(1.0 - F, 0.0, metalic);
        float3 diffuseIBL = kd * texColor.rgb * irradiance; // IBL의 diffuse 항
        uint specularTextureLevels = querySpecularTextureLevels(g_skySpecularMap); // 전체 LOD 밉맵 레벨수 
        float3 specularIrradiance = g_skySpecularMap.SampleLevel(samLinear, Lr, roughness * (specularTextureLevels - 1)).rgb;
        float2 specularBRDF = g_skyBrdfMap.Sample(samPointClamp, float2(cosLo, roughness)).rg;
        float3 specularIBL = (F0 * specularBRDF.x + specularBRDF.y) * specularIrradiance;
        // IBL 강도 조절 및 AO 적용
        float iblIntensity = 0.5f; // 이 값을 통해 주변광 밝기 조절
        ambientLighting = (diffuseIBL + specularIBL) * iblIntensity * AO;
    }
    float3 final =  ambientLighting;
    final = pow(final, 1.0 / 2.2);
    return float4(final, 1);
    
    
    // [loop]
    // for (int d = 0; d < g_NumDirectional; ++d)
    // {
    //     lightInput.L = normalize(-g_DirectionalLights[d].m_direction);
    //     lightInput.H = normalize(lightInput.L + lightInput.V);
    //     DirectionLightPBR(lightInput, F0, roughness, metalic);
    // }
    
    // point light
    // [loop]
    // for (int p = 0; p < g_NumPoint; ++p)
    // {
    //     float3 lightDirVec = g_PointLights[p].m_position - pin.worldPos.xyz;
    //     lightInput.D = length(lightDirVec);
    //     lightInput.L = normalize(lightDirVec);
    //     lightInput.H = normalize(lightInput.L + lightInput.V);
    //     lightInput.range = g_PointLights[p].m_range;
    //     lightInput.att = g_PointLights[p].m_attenuation;
    //     
    //     // PointLightCalculate(lightInput, diff, spec);
    // }
    
    
    // spot light
    // [loop]
    // for (int s = 0; s < g_NumSpot; ++s)
    // {
    //     float3 lightDirVec = g_PointLights[s].m_position - pin.worldPos.xyz;
    //     lightInput.D = length(lightDirVec);
    //     lightInput.L = normalize(lightDirVec);
    //     lightInput.H = normalize(lightInput.L + lightInput.V);
    //     lightInput.att = g_SpotLights[s].m_attenuation;
    //     
    //     // SpotLightCalculate(lightInput, diff, spec);
    // }
    
    // // a, d, s
    // float3 finalDiffuse = diff * texColor.rgb;
    // float3 finalSpecular = spec;
    // 
    // // 1. IBL을 위한 공통 벡터 계산
    // float3 N = lightInput.N;
    // float3 V = lightInput.V;
    // float3 R = reflect(-V, N); // 반사 벡터
    // float NdV = max(dot(N, V), 0.0001f);
    // 
    // // 2. Fresnel 계산 (IBL용)
    // float3 F = FresnelSchlickRoughness(F0, NdV, roughness);
    // float3 kS = F;
    // float3 kD = (1.0f - kS) * (1.0f - metalic);
    // 
    // // 3. Diffuse IBL (Irradiance Map 사용)
    // // Irradiance Map은 법선 방향(N)으로 샘플링합니다.
    // float3 irradiance = g_skyIrradianceMap.Sample(samAnisotropic, N).rgb;
    // float3 iblDiffuse = irradiance * texColor.rgb * kD;
    // 
    // // 4. Specular IBL (Pre-filtered Map & BRDF LUT 사용)
    // // 거칠기에 따른 Mipmap Level 계산 (보통 5~7단계)
    // float MAX_REFLECTION_LOD = 8.0;
    // float3 prefilteredColor = g_skySpecularMap.SampleLevel(samAnisotropic, R, roughness * MAX_REFLECTION_LOD).rgb;
    // // prefilteredColor *= 0.1;
    // // BRDF LUT 샘플링 (x: NdV, y: roughness)
    // float2 envBRDF = g_skyBrdfMap.Sample(samAnisotropic, float2(NdV, roughness)).rg;
    // float3 iblSpecular = prefilteredColor * (F * envBRDF.x + envBRDF.y);
    // 
    // // 5. 최종 결과 결합
    // float3 indirectLighting = iblDiffuse + iblSpecular;
    // float3 finalColor = finalDiffuse + finalSpecular + indirectLighting;
    // finalColor = finalColor / (finalColor + float3(1.0, 1.0, 1.0));
    // 
    // float4 result = float4(finalColor, 1.0f);
    // return result;
}