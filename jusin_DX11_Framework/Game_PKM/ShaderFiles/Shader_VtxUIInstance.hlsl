#include "Engine_Shader_Defines.hlsli"

float4x4 g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
texture2D g_TexDiff;
texture2D g_TexMask;
texture2D g_TexSub;

struct VS_IN
{
	float3 vPos : POSITION;
	float2 vTex : TEXCOORD0;

	float4 vRight : TEXCOORD1;
	float4 vUp : TEXCOORD2;
	float4 vTranslation : TEXCOORD3;
	float4 vColor : TEXCOORD4;
	float4 vUVTransform : TEXCOORD5;
	float4 vMaskUVTransform : TEXCOORD6;
	float4 vParams : TEXCOORD7;
};

struct VS_OUT
{
	float4 vPos : SV_POSITION;
	float2 vTex : TEXCOORD0;
	float2 vMaskTex : TEXCOORD1;
	float4 vColor : TEXCOORD2;
	float4 vParams : TEXCOORD3;
};

VS_OUT VS_MAIN_STAR(VS_IN In)
{
	VS_OUT Out;

	float4 vPos = In.vRight * In.vPos.x + In.vUp * In.vPos.y + In.vTranslation;

	vPos = mul(vPos, g_WorldMatrix);
	vPos = mul(vPos, g_ViewMatrix);
	vPos = mul(vPos, g_ProjMatrix);

	Out.vPos = vPos;
	float2 vBaseTex = In.vTex * In.vUVTransform.xy + In.vUVTransform.zw;

	float2 vCenteredTex = In.vTex - float2(0.5f, 0.5f);
	float2 vRotatedMaskTex;
	vRotatedMaskTex.x = dot(vCenteredTex, In.vMaskUVTransform.xy);
	vRotatedMaskTex.y = dot(vCenteredTex, In.vMaskUVTransform.zw);

	Out.vTex = vBaseTex;
	Out.vMaskTex = vRotatedMaskTex + float2(0.5f, 0.5f);
	Out.vColor = In.vColor;
	Out.vParams = In.vParams;

	return Out;
}

struct PS_IN
{
	float4 vPos : SV_POSITION;
	float2 vTex : TEXCOORD0;
	float2 vMaskTex : TEXCOORD1;
	float4 vColor : TEXCOORD2;
	float4 vParams : TEXCOORD3;
};

struct PS_OUT
{
	float4 vCol : SV_TARGET0;
};

float2 ApplyUVInset(float2 vTex)
{
	const float fInset = 0.015f;
	return lerp(float2(fInset, fInset), float2(1.f - fInset, 1.f - fInset), saturate(vTex));
}

float2 ResolveSampleUV(float2 vTex, float fSampleMode)
{
	float2 vResolved;

	if (fSampleMode < 0.5f)	// Single
	{
		vResolved = vTex;
	}
	else if (fSampleMode < 1.5f)	// Bi_Vertical
	{
		float fFoldY = 1.f - abs(vTex.y * 2.f - 1.f);
		vResolved = float2(vTex.x, fFoldY);
	}
	else if (fSampleMode < 2.5f)	// Quad
	{
		float fFoldX = 1.f - abs(vTex.x * 2.f - 1.f);
		float fFoldY = 1.f - abs(vTex.y * 2.f - 1.f);
		vResolved = float2(fFoldX, fFoldY);
	}

	return ApplyUVInset(vResolved);
}

PS_OUT PS_MAIN_STAR(PS_IN In)
{
	PS_OUT Out;

	const bool bUseDiffMask = (In.vParams.w > 0.5f && In.vParams.w < 1.5f);
	const bool bUseSubOnly = (In.vParams.w > 1.5f && In.vParams.w < 2.5f);
	const bool bUseSubMask = (In.vParams.w > 2.5f);

	const bool bUseSub = bUseSubOnly || bUseSubMask;
	const bool bUseMask = bUseDiffMask || bUseSubMask;

	float2 vBaseUV = ResolveSampleUV(In.vTex, In.vParams.y);
	float4 vSample = bUseSub ? g_TexSub.Sample(LinearSampler, vBaseUV) : g_TexDiff.Sample(LinearSampler, vBaseUV);
	float fAlpha = vSample.a * In.vColor.a;

	float4 vBase;
	vBase.rgb = In.vColor.rgb;
	vBase.a = fAlpha;

	if (bUseMask)
	{
		float2 vMaskUV = ResolveSampleUV(In.vMaskTex, In.vParams.z);
		float4 vMask = g_TexMask.Sample(LinearSampler, vMaskUV);

		float fMask = saturate(vMask.a * 1.6f);
		fMask = pow(fMask, 0.75f);

		float fMaskStrength = bUseSub ? In.vParams.x * 0.5f : In.vParams.x;
		vBase.rgb += vMask.rgb * fMask * fMaskStrength * vBase.a;
	}

	Out.vCol.rgb = saturate(vBase.rgb);
	Out.vCol.a = saturate(vBase.a);
	return Out;
}

technique11 DefaultTechnique
{
	pass Pass_Star	// 0
	{
		SetRasterizerState(RS_Cull_None);
		SetDepthStencilState(DSS_Z_Disable, 0);
		SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN_STAR();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_STAR();
	}
};