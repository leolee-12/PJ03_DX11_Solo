#include "Engine_Shader_Defines.hlsli"

float4x4 g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
texture2D g_Texture;
texture2D g_TexNoise;
texture2D g_TexGrad;

float4 g_vColor = float4(1.f, 1.f, 1.f, 1.f);

struct VS_IN
{
	float3 vPos : POSITION;
	float2 vTex : TEXCOORD0;
};

struct VS_OUT
{
	float4 vPos : SV_POSITION;
	float2 vTex : TEXCOORD0;
};

VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT Out;

	float4 vPos = mul(float4(In.vPos, 1.0f), g_WorldMatrix);
	vPos = mul(vPos, g_ViewMatrix);
	vPos = mul(vPos, g_ProjMatrix);
	
	Out.vPos = vPos;
	Out.vTex = In.vTex;
	return Out;
}

struct PS_IN
{
	float4 vPos : SV_POSITION;
	float2 vTex : TEXCOORD0;
};

struct PS_OUT
{
	float4 vCol : SV_TARGET0;
};

PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT Out;
	Out.vCol = g_Texture.Sample(LinearSampler, In.vTex) * g_vColor;

	if (Out.vCol.a < 0.01f)
		discard;

	return Out;
}

PS_OUT PS_MAIN_DEBUG(PS_IN In)
{
	PS_OUT Out;
	Out.vCol = g_Texture.Sample(LinearSampler, In.vTex) * g_vColor;

	if (Out.vCol.a < 0.01f)
		discard;

	float4 vNoise = g_TexNoise.Sample(LinearSampler, In.vTex);
	Out.vCol = Out.vCol * vNoise;
	return Out;
}

PS_OUT PS_MAIN_GRAD(PS_IN In)
{
	PS_OUT Out;
	float4 vMain = g_Texture.Sample(LinearSampler, In.vTex);
	float4 vGrad = g_TexGrad.Sample(LinearSampler, In.vTex);

	float fProgress = saturate(g_vColor.a);
	float fFeather = 0.08f;
	float fGrad = 1.f - vGrad.a;
	float fReveal = fProgress * (1.f + fFeather);
	float fMask = saturate((fReveal - fGrad) / fFeather);

	Out.vCol = vMain * float4(g_vColor.rgb, 1.f);
	Out.vCol.a *= fMask;

	if (Out.vCol.a < 0.01f)
		discard;

	return Out;
}

technique11 DefaultTechnique
{
	pass DefaultPass	// 0
	{
		SetRasterizerState(RS_Cull_None);
		SetDepthStencilState(DSS_Z_Disable, 0);
		SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		PixelShader = compile ps_5_0 PS_MAIN();
	}

	pass Pass_Debug	// 1
	{
		SetRasterizerState(RS_Cull_None);
		SetDepthStencilState(DSS_Z_Disable, 0);
		SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		PixelShader = compile ps_5_0 PS_MAIN_DEBUG();
	}

	  pass Pass_Grad	// 2
  {
	  SetRasterizerState(RS_Cull_None);
	  SetDepthStencilState(DSS_Z_Disable, 0);
	  SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

	  VertexShader = compile vs_5_0 VS_MAIN();
	  PixelShader = compile ps_5_0 PS_MAIN_GRAD();
  }
};