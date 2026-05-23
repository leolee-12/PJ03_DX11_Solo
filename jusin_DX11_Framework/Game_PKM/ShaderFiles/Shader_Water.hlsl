#include "Engine_Shader_Defines.hlsli"

float4x4 g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
float4x4 g_WITMatrix;
float g_fFarZ;
float g_fTime = 0.f;

texture2D g_TexWaterNet02;
texture2D g_TexNorm;
texture2D g_TexWaterNet01;
texture2D g_TexWaterLight;
texture2D g_TexSky;

float g_fMin = 0.4f;
float g_fMax = 1.0f;
float g_fNormalStrength = 1.0f;

float2 g_vTex02Speed = float2(-0.363333344f, -0.363333344f);
float2 g_vTex01Speed = float2(-0.545f, -0.545f);
float2 g_vTex03Speed = float2(0.363333344f, 0.363333344f);

float4 g_vBaseColor1 = float4(0.f, 0.190469623f, 0.8985151f, 1.f);
float4 g_vBaseColor2 = float4(0.f, 0.2917766f, 1.00002408f, 1.f);
float4 g_vWaterNet01Color = float4(1.15f, 1.15f, 1.15f, 1.f);
float4 g_vWaterNet02Color = float4(1.05f, 1.05f, 1.05f, 1.f);
float4 g_vWaterLightColor = float4(1.25f, 1.25f, 1.25f, 1.f);
float4 g_vWaterAmbt = float4(0.f, 0.f, 0.5f, 0.f);

struct VS_IN {
	float3 vPos : POSITION;
	float3 vNorm : NORMAL;
	float2 vTex : TEXCOORD0;
	float3 vTangent : TANGENT;
	float3 vBinormal : BINORMAL;
};

struct VS_OUT {
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
	float4 vWorldPos = mul(float4(In.vPos, 1.f), g_WorldMatrix);
	float4 vViewPos = mul(vWorldPos, g_ViewMatrix);

	Out.vPos = mul(vViewPos, g_ProjMatrix);
	Out.vNorm = float4(normalize(mul(In.vNorm, (float3x3)g_WITMatrix)), 0.f);
	Out.vTangent = float4(normalize(mul(In.vTangent, (float3x3)g_WITMatrix)), 0.f);
	Out.vBinormal = float4(normalize(mul(In.vBinormal, (float3x3)g_WITMatrix)), 0.f);
	Out.vTex = In.vTex;
	Out.vWorldPos = vWorldPos;
	Out.vProjPos = Out.vPos;
	return Out;
}

struct PS_OUT {
	float4 vDiff : SV_TARGET0;
	float4 vNorm : SV_TARGET1;
	float4 vDepth : SV_TARGET2;
	float4 vAmbt : SV_TARGET3;
	float4 vPickPos : SV_TARGET4;
};

float RemapMask(float fValue)
{
	return saturate((fValue - g_fMin) / max(g_fMax - g_fMin, 0.0001f));
}

float3 ResolveNormal(float2 vTex, float3 T, float3 B, float3 N)
{
	float2 vNormDesc = g_TexNorm.Sample(LinearSampler, vTex).rg * 2.f - 1.f;
	vNormDesc *= g_fNormalStrength;

	float3 vNormalTS;
	vNormalTS.xy = vNormDesc;
	vNormalTS.z = sqrt(saturate(1.f - dot(vNormalTS.xy, vNormalTS.xy)));

	return normalize(mul(normalize(vNormalTS), float3x3(T, B, N)));
}

PS_OUT PS_MAIN(VS_OUT In)
{
	PS_OUT Out;

	float2 uv01 = In.vTex + g_vTex01Speed * g_fTime;
	float2 uv02 = In.vTex + g_vTex02Speed * g_fTime;
	float2 uv03 = In.vTex + g_vTex03Speed * g_fTime;

	float net01 = RemapMask(g_TexWaterNet01.Sample(LinearSampler, uv01).r);
	float net02 = RemapMask(g_TexWaterNet02.Sample(LinearSampler, uv02).r);
	float light = g_TexWaterLight.Sample(LinearSampler, uv03).r;

	float3 baseColor = lerp(g_vBaseColor1.rgb, g_vBaseColor2.rgb, saturate(net01 * 0.5f + net02 *
		0.5f));
	float3 skyColor = g_TexSky.Sample(LinearSampler, In.vTex * 0.25f + g_vTex03Speed * g_fTime *
		0.1f).rgb;

	float3 waterColor = baseColor;
	waterColor += net01 * g_vWaterNet01Color.rgb * 0.32f;
	waterColor += net02 * g_vWaterNet02Color.rgb * 0.38f;
	waterColor += light * g_vWaterLightColor.rgb * 0.25f;
	waterColor = lerp(waterColor, skyColor, 0.10f);

	float3 T = normalize(In.vTangent.xyz);
	float3 B = normalize(In.vBinormal.xyz);
	float3 N = normalize(In.vNorm.xyz);
	float3 waterNormal = ResolveNormal(In.vTex, T, B, N);

	Out.vDiff = float4(saturate(waterColor), 1.f);
	Out.vNorm = float4(waterNormal * 0.5f + 0.5f, 1.f);
	Out.vDepth = float4(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fFarZ, 0.f, 0.f);
	Out.vAmbt = g_vWaterAmbt;
	Out.vPickPos = float4(In.vWorldPos.xyz, 1.f);
	return Out;
}

technique11 DefaultTechnique
{
	  pass DefaultPass
	  {
			SetRasterizerState(RS_Cull_None);
			SetDepthStencilState(DSS_Default, 0);
			SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

			VertexShader = compile vs_5_0 VS_MAIN();
			GeometryShader = NULL;
			PixelShader = compile ps_5_0 PS_MAIN();
	  }
}