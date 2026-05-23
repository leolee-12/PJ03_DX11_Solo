#include "Engine_Shader_Defines.hlsli"

float4x4 g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;

texture2D g_TexMask;   // 형태 마스크 (Entry_Plate BASE)
texture2D g_TexLine;   // 테두리 (Entry_Plate LINE)
texture2D g_TexGlow;   // Selected glow (Entry_Plate GLOW)
texture2D g_TexDiff;   // 색상 Diffuse (Entry_BG_Plate)

float4 g_vColDiff = float4(1.f, 1.f, 1.f, 1.f);
float4 g_vColLine = float4(1.f, 1.f, 1.f, 1.f);

float g_fGlowPhase = 0.f;
float g_fGlowAmount = 0.f;
float g_bMirrorUV = 0.f;



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

	float2 maskUV = In.vTex;
	if (g_bMirrorUV > 0.5f)
	{
		maskUV = 1.f - abs(In.vTex * 2.f - 1.f);

		float fW, fH;
		g_TexMask.GetDimensions(fW, fH);
		float2 halfTexel = 0.5f / float2(fW, fH);
		maskUV = clamp(maskUV, halfTexel, 1.f - halfTexel);
	}

	float  fMaskA = g_TexMask.Sample(ClampSampler, maskUV).a;
	float  fLineA = g_TexLine.Sample(ClampSampler, maskUV).a;
	float3 vDiff = g_TexDiff.Sample(ClampSampler, In.vTex).rgb;

	// b = 흰색 마스크, g = 연두 마스크 (텍스처를 색이 아닌 가중치로 사용)
	float wWhite = vDiff.b * 2.f;   // 흰색 가중 (>1 → 연두보다 흰색 우세)
	float wGreen = vDiff.g;
	float t = wWhite / max(wWhite + wGreen, 1e-4f);
	vDiff = lerp(float3(0.45f, 1.0f, 0.40f), float3(1.f, 1.f, 1.f), t);

	// 마스크(BASE) 영역엔 Diffuse 색, Line 영역엔 g_vColLine
	float3 cMix = lerp(vDiff * g_vColDiff.rgb, g_vColLine.rgb, fLineA);
	float  aMix = max(fMaskA, fLineA);

	if (g_fGlowAmount > 0.f)
	{
		float4 vGlow = g_TexGlow.Sample(ClampSampler, maskUV);
		float  fPulse = 0.5f + 0.5f * sin(g_fGlowPhase);
		float  fGlowK = saturate(g_fGlowAmount) * fPulse;

		cMix = lerp(cMix, saturate(cMix + vGlow.rgb * vGlow.a), fGlowK);
	}

	aMix *= g_vColDiff.a;

	if (aMix < 0.01f)
		discard;

	Out.vCol = float4(cMix, aMix);
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