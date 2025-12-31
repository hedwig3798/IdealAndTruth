
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

void DirectionLigthCalculate(LightInput _input, inout float _diff, inout float _spec)
{
    _diff += max(dot(_input.N, _input.L), 0);
    _spec += pow(saturate(dot(_input.N, _input.H)), 32.0f);
    
    return;
}

void PointLightCalculate(LightInput _input, inout float _diff, inout float _spec)
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

void SpotLightCalculate(LightInput _input, inout float _diff, inout float _spec)
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
