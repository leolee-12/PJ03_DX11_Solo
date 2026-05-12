#include "Engine_Shader_Defines.hlsli"

float4x4 g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
float4x4 g_WITMatrix;
vector g_vCamPos;
float g_fFarZ;
float g_fShadowFarZ;

// 재질
texture2D g_TexDiff;
texture2D g_TexAO;
texture2D g_TexNorm;
texture2D g_TexRough;

texture2D g_TexLycl;
texture2D g_TexMask;

float4 g_vLymColorR = float4(1.f, 0.f, 0.f, 1.f);
float4 g_vLymColorG = float4(0.f, 1.f, 0.f, 1.f);
float4 g_vLymColorB = float4(0.f, 0.f, 1.f, 1.f);
float4 g_vLymColorA = float4(1.f, 1.f, 1.f, 1.f);

float4 g_vEyeHighlightColor = float4(1.f, 1.f, 1.f, 1.f);  // mask 흰 부분 색 (보통 흰색)

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

	float4 vTangent : TANGENT;
	float4 vBinormal : BINORMAL;
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
	float3 vNormal = mul(float4(In.vNorm, 0.f), BoneMatrix).xyz;
	float3 vTangent = mul(float4(In.vTangent, 0.f), BoneMatrix).xyz;
	float3 vBinormal = mul(float4(In.vBinormal, 0.f), BoneMatrix).xyz;

	float4x4 matWV, matWVP;
	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);
	
	Out.vPos = mul(vPosition, matWVP);
	Out.vTex = In.vTex;
	Out.vWorldPos = mul(vPosition, g_WorldMatrix);
	Out.vProjPos = Out.vPos;

	Out.vNorm = float4(normalize(mul(vNormal, (float3x3)g_WITMatrix)), 0.f);
	Out.vTangent = float4(normalize(mul(vTangent, (float3x3)g_WITMatrix)), 0.f);
	Out.vBinormal = float4(normalize(mul(vBinormal, (float3x3)g_WITMatrix)), 0.f);
	return Out;
}



struct PS_IN
{
	float4 vPos : SV_POSITION;
	float4 vNorm : NORMAL;
	float2 vTex : TEXCOORD0;
	float4 vWorldPos : TEXCOORD1;
	float4 vProjPos : TEXCOORD2;
	float4 vTangent : TANGENT;
	float4 vBinormal : BINORMAL;
};

struct PS_OUT
{
	float4 vDiff : SV_TARGET0;
	float4 vNorm : SV_TARGET1;
	float4 vDepth : SV_TARGET2;
};

struct PS_OUT_SHADOW
{
	float4 vLightDepth : SV_TARGET0;
};

float g_fLymBlendPower = 1.5f;
float g_fLymAlphaBegin = 0.01f;
float g_fLymAlphaEnd = 0.15f;

float3 ResolveLymColor(float4 vLym)
{
	float3 w = saturate(vLym.rgb);

	w = pow(w, g_fLymBlendPower);

	float fSum = max(w.r + w.g + w.b, 0.0001f);
	w /= fSum;

	float3 vRGBColor =
		g_vLymColorR.rgb * w.r +
		g_vLymColorG.rgb * w.g +
		g_vLymColorB.rgb * w.b;

	float fAlphaMask = smoothstep(g_fLymAlphaBegin, g_fLymAlphaEnd, saturate(vLym.a));
	return lerp(vRGBColor, g_vLymColorA.rgb, fAlphaMask);
}

PS_OUT PS_Default(PS_IN In)	// 0번 패스
{
	PS_OUT Out = (PS_OUT)0;

	float4 vMtrlDiff = g_TexDiff.Sample(LinearSampler, In.vTex);

	Out.vDiff = float4(vMtrlDiff.rgb, 1.0f);
	Out.vNorm = vector(normalize(In.vNorm.xyz) * 0.5f + 0.5f, 1.f);
	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fFarZ, 0.f, 0.f);
	return Out;
}

PS_OUT_SHADOW PS_SHADOW(PS_IN In)
{
	PS_OUT_SHADOW Out;

	Out.vLightDepth = vector(In.vProjPos.z / In.vProjPos.w, 0.f, 0.f, 0.f);

	return Out;
}

PS_OUT PS_DANR(PS_IN In)	// 2번 패스
{
	PS_OUT Out;
	
	vector vMtrlDiff = g_TexDiff.Sample(LinearSampler, In.vTex);
	if (vMtrlDiff.a < 0.1f)
		discard;
	
	Out.vDiff = vector(vMtrlDiff.rgb, 1.f);
	Out.vNorm = vector(normalize(In.vNorm.xyz) * 0.5f + 0.5f, 1.f);
	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fFarZ, 0.f, 0.f);
	return Out;
}

PS_OUT PS_DALNR(PS_IN In)	// 3번 패스
{
	PS_OUT Out;
	vector vMtrlDiff = g_TexDiff.Sample(LinearSampler, In.vTex);
	if (vMtrlDiff.a < 0.1f)
		discard;

	Out.vDiff = vector(vMtrlDiff.rgb, 1.f);
	Out.vNorm = vector(normalize(In.vNorm.xyz) * 0.5f + 0.5f, 1.f);
	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fFarZ, 0.f, 0.f);
	return Out;
}

PS_OUT PS_DLN(PS_IN In)	// 4번 패스
{
	PS_OUT Out = (PS_OUT)0;

	// Sampling
	float4 vMtrlDiff = g_TexDiff.Sample(LinearSampler, In.vTex);
	float4 vLym = g_TexLycl.Sample(LinearSampler, In.vTex);

	// Lym 해석
	float3 vRegionColor = ResolveLymColor(vLym) * vMtrlDiff.rgb;
	float3 vEyeColor = vRegionColor;

	// Normal 구성
	float2 vNormDesc = g_TexNorm.Sample(LinearSampler, In.vTex).rg * 2.0f - 1.0f;
	float3 vEyeNormal;
	vEyeNormal.xy = vNormDesc.xy;
	vEyeNormal.z = sqrt(saturate(1.0f - dot(vEyeNormal.xy, vEyeNormal.xy)));
	vEyeNormal = normalize(vEyeNormal);

	float3 T = normalize(In.vTangent.xyz);
	float3 B = normalize(In.vBinormal.xyz) * -1.f;
	float3 N = normalize(In.vNorm.xyz);
	float3x3 WorldMatrix = float3x3(T, B, N);

	vEyeNormal = normalize(mul(vEyeNormal, WorldMatrix));

	// Output
	Out.vDiff = float4(vEyeColor, 1.0f);
	Out.vNorm = vector(vEyeNormal.xyz * 0.5f + 0.5f, 1.f);
	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fFarZ, 0.f, 0.f);
	return Out;
}

PS_OUT PS_DLMN(PS_IN In)	// 5번 패스
{
	PS_OUT Out = (PS_OUT)0;

	// Sampling
	float4 vMtrlDiff = g_TexDiff.Sample(LinearSampler, In.vTex);
	float fMask = g_TexMask.Sample(LinearSampler, In.vTex).r;
	float4 vLym = g_TexLycl.Sample(LinearSampler, In.vTex);
	float fHighlightMask = saturate(fMask);

	// Lym 해석
	float3 vRegionColor = ResolveLymColor(vLym) * vMtrlDiff.rgb;
	float3 vHighlightColor = g_vEyeHighlightColor.rgb * vMtrlDiff.rgb;
	float3 vEyeColor = lerp(vRegionColor, vHighlightColor, fHighlightMask);

	// Normal 구성
	float2 vNormDesc = g_TexNorm.Sample(LinearSampler, In.vTex).rg * 2.0f - 1.0f;
	float3 vEyeNormal;
	vEyeNormal.xy = vNormDesc.xy;
	vEyeNormal.z = sqrt(saturate(1.0f - dot(vEyeNormal.xy, vEyeNormal.xy)));
	vEyeNormal = normalize(vEyeNormal);

	float3 T = normalize(In.vTangent.xyz);
	float3 B = normalize(In.vBinormal.xyz) * -1.f;
	float3 N = normalize(In.vNorm.xyz);
	float3x3 WorldMatrix = float3x3(T, B, N);

	vEyeNormal = normalize(mul(vEyeNormal, WorldMatrix));

	// Output
	Out.vDiff = float4(vEyeColor, 1.0f);
	Out.vNorm = vector(vEyeNormal.xyz * 0.5f + 0.5f, 1.f);
	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fFarZ, 0.f, 0.f);
	return Out;
}

PS_OUT PS_DL(PS_IN In)	// 6번 패스
{
	PS_OUT Out = (PS_OUT)0;

	float4 vMtrlDiff = g_TexDiff.Sample(LinearSampler, In.vTex);
	float4 vLym = g_TexLycl.Sample(LinearSampler, In.vTex);

	float3 vLymColor = ResolveLymColor(vLym);
	Out.vDiff = float4(vMtrlDiff.rgb * vLymColor, 1.0f);
	Out.vNorm = vector(normalize(In.vNorm.xyz) * 0.5f + 0.5f, 1.f);
	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fFarZ, 0.f, 0.f);
	return Out;
}

technique11 DefaultTechnique
{
	pass Pass_Default		// 0. Body
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_Default();
	}
	pass Pass_Shadow		// 1. Shadow
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_SHADOW();
	}
	pass Pass_DANR			// 2. Body_a, Body_b
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_DANR();
	}
	pass Pass_PS_DALNR		// 3. Body_c
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_DALNR();
	}
	pass Pass_DLN			// 4. eye1
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_DLN();
	}
	pass Pass_DLMN			// 5. eye2
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_DLMN();
	}
	pass Pass_DL			// 6. fire
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_DL();
	}
};