#include "Engine_Shader_Defines.hlsli"

float4x4 g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
float4x4 g_WITMatrix;
float g_fFarZ;

texture2D g_TexDiff;

vector g_vDefaultAmbt = vector(0.f, 0.f, 0.5f, 0.f);

struct VS_IN
{
	float3 vPos : POSITION;
	float3 vNorm : NORMAL;
	float2 vTex : TEXCOORD0;

	float3 vTangent : TANGENT;
	float3 vBinormal : BINORMAL;

	float4 vInstRight : WORLD0;
	float4 vInstUp : WORLD1;
	float4 vInstLook : WORLD2;
	float4 vInstTranslation : WORLD3;
};

struct VS_OUT
{
	float4 vPos : SV_POSITION;
	float4 vNorm : NORMAL;
	float2 vTex : TEXCOORD0;
	float4 vWorldPos : TEXCOORD1;
	float4 vProjPos : TEXCOORD2;
};

VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT Out;

	float4x4 InstanceMatrix = float4x4(
		In.vInstRight,
		In.vInstUp,
		In.vInstLook,
		In.vInstTranslation);

	float4 vWorldPos = mul(float4(In.vPos, 1.f), InstanceMatrix);
	vWorldPos = mul(vWorldPos, g_WorldMatrix);

	float3 vWorldNorm = mul(In.vNorm, (float3x3)InstanceMatrix);
	vWorldNorm = mul(vWorldNorm, (float3x3)g_WITMatrix);

	float4 vViewPos = mul(vWorldPos, g_ViewMatrix);
	float4 vProjPos = mul(vViewPos, g_ProjMatrix);

	Out.vPos = vProjPos;
	Out.vNorm = float4(normalize(vWorldNorm), 0.f);
	Out.vTex = In.vTex;
	Out.vWorldPos = vWorldPos;
	Out.vProjPos = vProjPos;

	return Out;
}

struct PS_IN
{
	float4 vPos : SV_POSITION;
	float4 vNorm : NORMAL;
	float2 vTex : TEXCOORD0;
	float4 vWorldPos : TEXCOORD1;
	float4 vProjPos : TEXCOORD2;
};

struct PS_OUT
{
	float4 vDiff : SV_TARGET0;
	float4 vNorm : SV_TARGET1;
	float4 vDepth : SV_TARGET2;
	float4 vAmbt : SV_TARGET3;
	float4 vPickPos : SV_TARGET4;
};

PS_OUT PS_GRASS(PS_IN In)
{
	PS_OUT Out;
	float4 vDiff = g_TexDiff.Sample(LinearSampler, In.vTex);

	if (vDiff.a < 0.4f)
		discard;

	Out.vDiff = vector(vDiff.rgb, 1.f);
	Out.vNorm = vector(normalize(In.vNorm.xyz) * 0.5f + 0.5f, 1.f);
	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fFarZ, 0.f, 0.f);
	Out.vPickPos = vector(In.vWorldPos.xyz, 1.f);
	Out.vAmbt = g_vDefaultAmbt;

	return Out;
}

technique11 DefaultTechnique
{
	pass Pass_GrassInst
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_GRASS();
	}
};