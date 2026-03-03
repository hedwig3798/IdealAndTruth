#include "Buffers.hlsl"

VertexOut main(VertexIn vin)
{
	// output
    VertexOut vout;
    
	// viewport position
    vout.posH = mul(float4(vin.posL, 1.0f), g_world);
    vout.worldPos = vout.posH;
    vout.posH = mul(vout.posH, g_view);
    vout.posH = mul(vout.posH, g_proj);
    
	// textuer position
    vout.tex = vin.tex;
    
    // normal transform (normal matrix = transpose(inverse(world)))
    float3 N = normalize(mul(vin.normal, (float3x3) transpose(g_worldInvers)));
    // Transform tangent/binormal with same normal-matrix
    float3 T = normalize(mul(vin.tangent, (float3x3) transpose(g_worldInvers)));
    float3 B = normalize(mul(vin.binormal, (float3x3) transpose(g_worldInvers)));

    // Orthonormalize T against N (Gram?Schmidt) and recompute B to ensure a proper TBN
    T = normalize(T - dot(T, N) * N);

    // Compute handedness from original B (to preserve bitangent sign)
    float handedness = sign(dot(cross(N, T), B));
    B = cross(N, T) * handedness;

    vout.normal = N;
    vout.tangent = T;
    vout.binormal = B;
    
    return vout;
}