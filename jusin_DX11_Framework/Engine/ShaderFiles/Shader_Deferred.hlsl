#include "Engine_Shader_Defines.hlsli"

float4x4 g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
texture2D g_Texture;

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

	float4 vPos = mul(float4(In.vPos, 1.f), g_WorldMatrix);
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

struct PS_OUT_BACKBUFFER
{
	float4 vBackBuffer : SV_TARGET0;
};


/* 픽셀셰이더 : 픽셀의 최종적인 색을 결정해준다. */
PS_OUT_BACKBUFFER PS_MAIN_DEBUG(PS_IN In)
{
	PS_OUT_BACKBUFFER Out;

	Out.vBackBuffer = g_Texture.Sample(LinearSampler, In.vTex);

	return Out;
};


technique11 DefaultTechnique
{
	pass Debug
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_DEBUG();
	}
}