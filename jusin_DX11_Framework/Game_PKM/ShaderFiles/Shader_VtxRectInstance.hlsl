#include "Engine_Shader_Defines.hlsli"

float4x4 g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
texture2D g_Texture;

struct VS_IN
{
	float3 vPos : POSITION;
	float2 vTex : TEXCOORD0;

	float4 vRight : TEXCOORD1;
	float4 vUp : TEXCOORD2;
	float4 vLook : TEXCOORD3;
	float4 vTranslation : TEXCOORD4;
	float2 vLifeTime : TEXCOORD5;
};

struct VS_OUT
{
	float4 vPos : SV_POSITION;
	float2 vTex : TEXCOORD0;
	float2 vLifeTime : TEXCOORD1;
};

VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT Out;

	float4x4 TransformMatrix = float4x4(In.vRight, In.vUp, In.vLook, In.vTranslation);

	float4 vPos = mul(float4(In.vPos, 1.f), TransformMatrix);

	vPos = mul(vPos, g_WorldMatrix);
	vPos = mul(vPos, g_ViewMatrix);
	vPos = mul(vPos, g_ProjMatrix);
	
	Out.vPos = vPos;
	Out.vTex = In.vTex;
	Out.vLifeTime = In.vLifeTime;
	return Out;
}

struct PS_IN
{
	float4 vPos : SV_POSITION;
	float2 vTex : TEXCOORD0;
	float2 vLifeTime : TEXCOORD1;
};

struct PS_OUT
{
	float4 vCol : SV_TARGET0;
};

PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT Out;
	Out.vCol = g_Texture.Sample(LinearSampler, In.vTex);
	Out.vCol.rgb += 1.f - saturate(In.vLifeTime.x - In.vLifeTime.y);
	Out.vCol.a = Out.vCol.a * saturate(In.vLifeTime.x - In.vLifeTime.y);
	return Out;
}

technique11 DefaultTechnique
{
	pass DefaultPass
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		PixelShader = compile ps_5_0 PS_MAIN();
	}
};