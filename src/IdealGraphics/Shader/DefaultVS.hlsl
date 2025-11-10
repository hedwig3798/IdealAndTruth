#include "Buffers.hlsl"

VertexOut main(VertexIn vin)
{
	// 출력
    VertexOut vout;
    
	// 정점의 뷰포트 좌표
    vout.posH = mul(float4(vin.posL, 1.0f), g_world);
    vout.worldPos = vout.posH;
    vout.posH = mul(vout.posH, g_view);
    vout.posH = mul(vout.posH, g_proj);

	// 텍스쳐의 좌표 (texcoord를 그대로 픽셀쉐이더로 넘겨 준다)
    vout.tex = vin.tex;
    vout.normal = vin.normal;
    return vout;
}