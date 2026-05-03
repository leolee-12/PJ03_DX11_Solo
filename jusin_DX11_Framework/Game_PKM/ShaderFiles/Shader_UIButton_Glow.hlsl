#include "Engine_Shader_Defines.hlsli"

float4x4 g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
texture2D g_TexDiff;
texture2D g_TexGlow;

float4 g_vColor = float4(1.f, 1.f, 1.f, 1.f);
float g_fGlowPhase = 0.f;
float g_fGlowAmount = 0.f;

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

	float4 vBase = g_TexDiff.Sample(LinearSampler, In.vTex) * g_vColor;
	float4 vGlow = g_TexGlow.Sample(LinearSampler, In.vTex);

	float fPulse = 0.5f + 0.5f * sin(g_fGlowPhase);
	float fGlowK = saturate(g_fGlowAmount) * fPulse;

	Out.vCol.rgb = vBase.rgb + vGlow.rgb * vGlow.a * fGlowK;
	Out.vCol.a = vBase.a;

	if (Out.vCol.a < 0.01f)
		discard;

	return Out;
}

technique11 DefaultTechnique
{
pass DefaultPass
	  {
		  SetRasterizerState(RS_Cull_None);
		  SetDepthStencilState(DSS_Z_Disable, 0);
		  SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		  VertexShader = compile vs_5_0 VS_MAIN();
		  PixelShader = compile ps_5_0 PS_MAIN();
	  }
};