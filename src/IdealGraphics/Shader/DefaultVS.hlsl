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
    
    // normal transform
    vout.normal = normalize(mul(vin.normal, (float3x3) (transpose(g_worldInvers))));
    vout.tangent = normalize(vin.tangent);
    vout.binormal = normalize(cross(vout.normal, vout.tangent));
    
    return vout;
}