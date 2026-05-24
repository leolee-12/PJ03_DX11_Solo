#include "Engine_Shader_Defines.hlsli"

float4x4 g_WorldMatrix;   // identity (ribbon¿∫ ¿ÃπÃ world space)
float4x4 g_ViewMatrix;
float4x4 g_ProjMatrix;
texture2D g_Texture;

struct VS_IN { float3 vPos : POSITION; float2 vTex : TEXCOORD0; float4 vColor : COLOR0; };
struct VS_OUT { float4 vPos : SV_POSITION; float2 vTex : TEXCOORD0; float4 vColor : TEXCOORD1; };

VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT Out;
	float4 vWorld = mul(float4(In.vPos, 1.f), g_WorldMatrix);
	Out.vPos = mul(mul(vWorld, g_ViewMatrix), g_ProjMatrix);
	Out.vTex = In.vTex;
	Out.vColor = In.vColor;
	return Out;
}

float4 PS_MAIN(VS_OUT In) : SV_TARGET0
{
	float4 vTex = g_Texture.Sample(LinearSampler, In.vTex);

	const bool bSingleChannelMask =
		(vTex.g <= 0.0001f && vTex.b <= 0.0001f && vTex.a >= 0.999f);
	if (bSingleChannelMask)
	{
		const float fMask = vTex.r;
		return float4(In.vColor.rgb * fMask, In.vColor.a * fMask);
	}
	return vTex * In.vColor;
}

technique11 DefaultTechnique
{
	pass Alpha             // 0
	{
		SetRasterizerState(RS_Cull_None);
		SetDepthStencilState(DSS_DepthReadNoWrite, 0);
		SetBlendState(BS_AlphaBlend, float4(0,0,0,0), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN();
	}
	pass Additive          // 1
	{
		SetRasterizerState(RS_Cull_None);
		SetDepthStencilState(DSS_DepthReadNoWrite, 0);
		SetBlendState(BS_Additive, float4(0,0,0,0), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN();
	}
	pass Alpha_DepthOff    // 2
	{
		SetRasterizerState(RS_Cull_None);
		SetDepthStencilState(DSS_DepthOff, 0);
		SetBlendState(BS_AlphaBlend, float4(0,0,0,0), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN();
	}
	pass Additive_DepthOff // 3
	{
		SetRasterizerState(RS_Cull_None);
		SetDepthStencilState(DSS_DepthOff, 0);
		SetBlendState(BS_Additive, float4(0,0,0,0), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN();
	}
};