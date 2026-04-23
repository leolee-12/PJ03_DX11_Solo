#include "Engine_Shader_Defines.hlsli"

float4x4 g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
float4x4 g_WITMatrix;
vector g_vCamPos;

// 광원
vector g_vLightDir;
vector g_vLightDiff;
vector g_vLightAmbt;
vector g_vLightSpec;

// 재질
texture2D g_TexDiff;
texture2D g_TexSpec;
texture2D g_TexAmbt_R;
texture2D g_TexAmbt_G;
texture2D g_TexAmbt_B;
texture2D g_TexEmit;
texture2D g_TexLycl;

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

PS_OUT PS_DS(PS_IN In)	// 0번 패스
{
	PS_OUT Out;
	vector vMtrlDiff = g_TexDiff.Sample(LinearSampler, In.vTex);
	vector vMtrlSpec = g_TexSpec.Sample(LinearSampler, In.vTex);

	vector Normal = normalize(In.vNorm);
	vector Light = normalize(g_vLightDir);
	vector vView = normalize(g_vCamPos - In.vWorldPos);
	vector vHalf = normalize((-Light) + vView);
	float fSpec = pow(max(dot(Normal, vHalf), 0.f), 150.f);
	float fShade = max(dot(Light * -1.f, Normal), 0.f);

	vector Diff = (g_vLightDiff * vMtrlDiff) * fShade;
	vector Ambt = g_vLightAmbt * vMtrlDiff;
	vector Spec = (g_vLightSpec * vMtrlSpec) * fSpec;
	Out.vCol = saturate(Diff + Ambt + Spec);
	return Out;
}

PS_OUT PS_DSEL(PS_IN In)	// 1번 패스
{
	PS_OUT Out;
	vector vMtrlDiff = g_TexDiff.Sample(LinearSampler, In.vTex);
	vector vMtrlSpec = g_TexSpec.Sample(LinearSampler, In.vTex);
	vector vMtrlEmit = float4(0.f, 0.f, 0.f, 1.f);
	
	if (vMtrlDiff.a < 0.01f)
	{
		float2 vTexEyes = float2(In.vTex.x * 2.f, In.vTex.y * 4.f);
		vMtrlDiff = g_TexLycl.Sample(LinearSampler, vTexEyes);
		vMtrlEmit = g_TexEmit.Sample(LinearSampler, vTexEyes);
	}
	
	vector Normal = normalize(In.vNorm);
	vector Light = normalize(g_vLightDir);
	vector vView = normalize(g_vCamPos - In.vWorldPos);
	vector vHalf = normalize((-Light) + vView);
	float fSpec = pow(max(dot(Normal, vHalf), 0.f), 150.f);
	float fShade = max(dot(Light * -1.f, Normal), 0.f);
	
	vector Diff = (g_vLightDiff * vMtrlDiff) * fShade;
	vector Ambt = g_vLightAmbt * vMtrlDiff;
	vector Spec = (g_vLightSpec * vMtrlSpec) * fSpec;
	Out.vCol = saturate(Diff + Ambt + Spec + vMtrlEmit);
	return Out;
}

PS_OUT PS_DSAAA(PS_IN In)	// 2번 패스
{
	PS_OUT Out;
	vector vMtrlDiff = g_TexDiff.Sample(LinearSampler, In.vTex);
	vector vMtrlSpec = g_TexSpec.Sample(LinearSampler, In.vTex);
	vector vMtrlAmbt = float4(	g_TexAmbt_R.Sample(LinearSampler, In.vTex).r,
								g_TexAmbt_G.Sample(LinearSampler, In.vTex).r, 
								g_TexAmbt_B.Sample(LinearSampler, In.vTex).r, 
								1.f);

	vector Normal = normalize(In.vNorm);
	vector Light = normalize(g_vLightDir);
	vector vView = normalize(g_vCamPos - In.vWorldPos);
	vector vHalf = normalize((-Light) + vView);
	float fSpec = pow(max(dot(Normal, vHalf), 0.f), 150.f);
	float fShade = max(dot(Light * -1.f, Normal), 0.f);

	vector Diff = (g_vLightDiff * vMtrlDiff) * fShade;
	vector Ambt = (g_vLightAmbt * vMtrlAmbt) * vMtrlDiff;
	vector Spec = (g_vLightSpec * vMtrlSpec) * fSpec;
	Out.vCol = saturate(Diff + Ambt + Spec);
	return Out;
}

technique11 DefaultTechnique
{
	pass Pass_DS	// 0. BAG, BOTTOMS, CAP
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_DS();
	}
	pass Pass_DSEL	// 1. R_EYE, L_EYE
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_DSEL();
	}
	pass Pass_DSAAA	// 2. SKIN, HAIR, SHOES, TOPS
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_DSAAA();
	}

};