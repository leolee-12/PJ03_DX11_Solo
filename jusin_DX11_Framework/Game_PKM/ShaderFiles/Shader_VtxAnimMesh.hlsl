#include "Engine_Shader_Defines.hlsli"

float4x4 g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
float4x4 g_WITMatrix;
vector g_vCamPos;
float g_fAlpha;

// 광원
vector g_vLightDir;
vector g_vLightDiff;
vector g_vLightAmbt;
vector g_vLightSpec;

// 재질
texture2D g_TexDiff;
vector g_vMtrlAmbt = vector(0.4f, 0.4f, 0.4f, 1.f);
vector g_vMtrlSpec = vector(1.f, 1.f, 1.f, 1.f);

float4x4 g_BoneMatrices[512];

struct VS_IN
{
	float3 vPos : POSITION;
	float3 vNorm : NORMAL;
	float2 vTex : TEXCOORD0;

	float3 vTangent : TANGENT;
	float3 vBinormal : BINORMAL;

	uint4 vBlendIndex : BLENDINDICES;
	float4 vBlendWeight : BLENDWEIGHT;
};

struct VS_OUT
{
	float4 vPos : SV_POSITION;
	float4 vNorm : NORMAL;
	float2 vTex : TEXCOORD0;
	float4 vWorldPos : TEXCOORD1;
};

VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT Out;

	In.vBlendWeight.w = 1.f - (In.vBlendWeight.x + In.vBlendWeight.y + In.vBlendWeight.z);

	float4x4 BoneMatrix =	g_BoneMatrices[In.vBlendIndex.x] * In.vBlendWeight.x +
							g_BoneMatrices[In.vBlendIndex.y] * In.vBlendWeight.y +
							g_BoneMatrices[In.vBlendIndex.z] * In.vBlendWeight.z +
							g_BoneMatrices[In.vBlendIndex.w] * In.vBlendWeight.w;

	float4 vPosition = mul(float4(In.vPos, 1.f), BoneMatrix);
	float4 vNormal = mul(float4(In.vNorm, 0.f), BoneMatrix);

	float4x4 matWV, matWVP;
	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);
	
	Out.vPos = mul(vPosition, matWVP);
	Out.vNorm = normalize(mul(vNormal, g_WITMatrix));
	Out.vTex = In.vTex;
	Out.vWorldPos = mul(vPosition, g_WorldMatrix);
	return Out;
}

struct PS_IN
{
	float4 vPos : SV_POSITION;
	float4 vNorm : NORMAL;
	float2 vTex : TEXCOORD0;
	float4 vWorldPos : TEXCOORD1;
};

struct PS_OUT
{
	float4 vCol : SV_TARGET0;
};

PS_OUT PS_MAIN(PS_IN In)	// Phong Model
{
	PS_OUT Out;
	vector vMtrlDiff = g_TexDiff.Sample(LinearSampler, In.vTex);

	if (vMtrlDiff.a < 0.1f)	// 일정 a값 미만은 버림 (알파테스트)
		discard;

	vector Normal = normalize(In.vNorm);
	vector Light = normalize(g_vLightDir);

	vector vView = normalize(g_vCamPos - In.vWorldPos);
	vector vReflect = reflect(Light, Normal);
	float fSpec = pow(max(dot(vView, normalize(vReflect)), 0.f), 50.f);

	// ---------- Phong 모델 ----------
	float fShade = max(dot(Light * -1.f, Normal), 0.f);
	vector Diff = (g_vLightDiff * vMtrlDiff) * fShade;
	vector Ambt = (g_vLightAmbt * g_vMtrlAmbt) * vMtrlDiff;
	vector Spec = (g_vLightSpec * g_vMtrlSpec) * fSpec;
	Out.vCol = saturate(Diff + Ambt + Spec);
	// 정확한 Phong 모델은 Diff + Ambt + Spec이지만, Ambt에 vMtrlDiff를 곱해주면 좀 더 자연스러운 결과가 나옴 (Ambt도 텍스처 색상값에 비례하도록)
	// --------------------------------

	// ---------- 학원 모델 -----------
	//vector vShade = max(dot(Light * -1.f, Normal), 0.f) + (g_vLightAmbt * g_vMtrlAmbt);
	//Out.vCol = g_vLightDiff * vMtrlDiff * saturate(vShade) + (g_vLightSpec * g_vMtrlSpec) * fSpec;
	// --------------------------------

	return Out;
}

PS_OUT PS_MAIN_BLINNPHONG(PS_IN In)	// Blinn-Phong Model
{
	PS_OUT Out;
	vector vMtrlDiff = g_TexDiff.Sample(LinearSampler, In.vTex);

	if (vMtrlDiff.a < 0.1f)	// 일정 a값 미만은 버림 (알파테스트)
		discard;

	vector Normal = normalize(In.vNorm);
	vector Light = normalize(g_vLightDir);
	
	vector vView = normalize(g_vCamPos - In.vWorldPos);
	vector vHalf = normalize((-Light) + vView);
	float fSpec = pow(max(dot(Normal, vHalf), 0.f), 150.f);

	// ---------- Blinn-Phong 모델 ----------
	float fShade = max(dot(Light * -1.f, Normal), 0.f);
	vector Diff = (g_vLightDiff * vMtrlDiff) * fShade;
	vector Ambt = (g_vLightAmbt * g_vMtrlAmbt) * vMtrlDiff; // Ambt*TexDiff 보정
	vector Spec = (g_vLightSpec * g_vMtrlSpec) * fSpec;
	Out.vCol = saturate(Diff + Ambt + Spec);
	// --------------------------------

	// ---------- 학원 모델 -----------
	//vector vShade = max(dot(Light * -1.f, Normal), 0.f) + (g_vLightAmbt * g_vMtrlAmbt);
	//Out.vCol = g_vLightDiff * vMtrlDiff * saturate(vShade) + (g_vLightSpec * g_vMtrlSpec) * fSpec;
	// --------------------------------

	Out.vCol.a = g_fAlpha;

	return Out;
}

technique11 DefaultTechnique
{
	pass DefaultPass	// 0. Phong Model
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		PixelShader = compile ps_5_0 PS_MAIN();
	}
	pass BlinnPhongPass	// 1. Blinn-Phong Model
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		PixelShader = compile ps_5_0 PS_MAIN_BLINNPHONG();
	}
};