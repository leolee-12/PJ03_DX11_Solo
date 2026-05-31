#include "Engine_Shader_Defines.hlsli"

float4x4 g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
float4x4 g_WITMatrix;
vector g_vCamPos;
float g_fFarZ;

// 재질
texture2D g_TexDiff;
texture2D g_TexSpec;
texture2D g_TexAmbt;
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
	float4 vProjPos : TEXCOORD2;
};

VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT Out;

	In.vBlendWeight.w = 1.f - (In.vBlendWeight.x + In.vBlendWeight.y + In.vBlendWeight.z);

	float4x4 BoneMatrix = g_BoneMatrices[In.vBlendIndex.x] * In.vBlendWeight.x +
		g_BoneMatrices[In.vBlendIndex.y] * In.vBlendWeight.y +
		g_BoneMatrices[In.vBlendIndex.z] * In.vBlendWeight.z +
		g_BoneMatrices[In.vBlendIndex.w] * In.vBlendWeight.w;

	float4 vPosition = mul(float4(In.vPos, 1.f), BoneMatrix);
	float3 vNormal = mul(float4(In.vNorm, 0.f), BoneMatrix).xyz;

	float4x4 matWV, matWVP;
	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);

	Out.vPos = mul(vPosition, matWVP);
	Out.vNorm = float4(normalize(mul(vNormal, (float3x3)g_WITMatrix)), 0.f);
	Out.vTex = In.vTex;
	Out.vWorldPos = mul(vPosition, g_WorldMatrix);
	Out.vProjPos = Out.vPos;
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

};

PS_OUT PS_DS(PS_IN In)	// 0번 패스
{
	PS_OUT Out;

	vector vMtrlDiff = g_TexDiff.Sample(LinearSampler, In.vTex);
	if (vMtrlDiff.a < 0.1f)
		discard;

	Out.vDiff = vector(vMtrlDiff.rgb, 1.f);
	Out.vNorm = vector(normalize(In.vNorm.xyz) * 0.5f + 0.5f, 1.f);
	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fFarZ, 0.f, 0.f);
	Out.vAmbt = vector(1.f, 1.f, 1.f, g_TexSpec.Sample(LinearSampler, In.vTex).r);
	return Out;
}

PS_OUT PS_DSEL(PS_IN In)	// 1번 패스
{
	PS_OUT Out;
	vector vMtrlDiff = g_TexDiff.Sample(LinearSampler, In.vTex);
	vector vMtrlEmit = float4(0.f, 0.f, 0.f, 1.f);

	if (vMtrlDiff.a < 0.1f)
	{
		float2 vTexEyes = float2(In.vTex.x * 2.f, In.vTex.y * 4.f);
		vMtrlDiff = g_TexLycl.Sample(LinearSampler, vTexEyes);
		vMtrlEmit = g_TexEmit.Sample(LinearSampler, vTexEyes);
	}

	Out.vDiff = vector((vMtrlDiff + vMtrlEmit).rgb, 1.f);
	Out.vNorm = vector(normalize(In.vNorm.xyz) * 0.5f + 0.5f, 1.f);
	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fFarZ, 0.f, 0.f);
	Out.vAmbt = vector(1.f, 1.f, 1.f, g_TexSpec.Sample(LinearSampler, In.vTex).r);
	return Out;
}

PS_OUT PS_DSAAA(PS_IN In)	// 2번 패스
{
	PS_OUT Out;
	vector vMtrlDiff = g_TexDiff.Sample(LinearSampler, In.vTex);

	Out.vDiff = vector(vMtrlDiff.rgb, 1.f);
	Out.vNorm = vector(normalize(In.vNorm.xyz) * 0.5f + 0.5f, 1.f);
	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fFarZ, 0.f, 0.f);
	Out.vAmbt = vector(
		g_TexAmbt.Sample(LinearSampler, In.vTex).rgb,
		g_TexSpec.Sample(LinearSampler, In.vTex).r);
	return Out;
}

struct PS_OUT_SHADOW
{
	float4 vLightDepth : SV_TARGET0;
};

PS_OUT_SHADOW PS_SHADOW(PS_IN In)
{
	PS_OUT_SHADOW Out;

	Out.vLightDepth = vector(In.vProjPos.z / In.vProjPos.w, 0.f, 0.f, 0.f);

	return Out;
}

float4 PS_OUTLINEMASK(PS_IN In) : SV_TARGET	// 4
{
	return float4(1.f, 0.f, 0.f, 0.f);
}

technique11 DefaultTechnique
{
	pass Pass_DS		// 0. BAG, BOTTOMS, CAP
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_DS();
	}
	pass Pass_DSEL		// 1. R_EYE, L_EYE
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_DSEL();
	}
	pass Pass_DSAAA		// 2. SKIN, HAIR, SHOES, TOPS
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_DSAAA();
	}
	pass Pass_Shadow	// 3. Shadow
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_SHADOW();
	}
	pass OutlineMask	// 4
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_DepthReadNoWrite, 0);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_OUTLINEMASK();
	}
};