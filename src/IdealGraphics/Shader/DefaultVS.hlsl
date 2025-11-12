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
    vout.normal = vin.normal;
    return vout;
}