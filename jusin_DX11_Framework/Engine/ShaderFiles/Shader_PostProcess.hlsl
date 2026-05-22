#include "Engine_Shader_Defines.hlsli"

float4x4 g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;

texture2D g_TexCombined;
texture2D g_TexNorm;
texture2D g_TexDepth;
texture2D g_TexOutlineMask;

int   g_iOutlineMode;
float g_fOutlineStrength;
float g_fOutlineDepthStrength;
float g_fOutlineNormalStrength;
float g_fOutlineThresholdLow;
float g_fOutlineThresholdHigh;
float g_fOutlineThicknessPx;
float g_fOutlineDarkenFactor;
float g_fOutlineMaskBias;

float2 g_vTexelSize;
float  g_fFarZ;



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

float SampleDepth(float2 uv)
{
	return g_TexDepth.Sample(PointSampler, uv).y;
}

float3 SampleNormal(float2 uv)
{
	float3 n = g_TexNorm.Sample(PointSampler, uv).xyz * 2.f - 1.f;
	return normalize(n);
}

float SampleMask(float2 uv)
{
	return g_TexOutlineMask.Sample(PointSampler, uv).r;
}

PS_OUT_BACKBUFFER PS_MAIN_COPY(PS_IN In)
{
	PS_OUT_BACKBUFFER Out;

	Out.vBackBuffer = g_TexCombined.Sample(PointSampler, In.vTex);
	if (0.5f > Out.vBackBuffer.a)
		discard;

	return Out;
}

PS_OUT_BACKBUFFER PS_MAIN_OUTLINE_DEBUG(PS_IN In)
{
	PS_OUT_BACKBUFFER Out;

	float2 uv = In.vTex;
	float2 texel = g_vTexelSize * g_fOutlineThicknessPx;

	float dC = SampleDepth(uv);
	float dR = SampleDepth(uv + float2(texel.x, 0));
	float dL = SampleDepth(uv + float2(-texel.x, 0));
	float dU = SampleDepth(uv + float2(0, texel.y));
	float dD = SampleDepth(uv + float2(0, -texel.y));

	float depthEdge =
		max(max(abs(dC - dR), abs(dC - dL)),
			max(abs(dC - dU), abs(dC - dD)));

	float3 nC = SampleNormal(uv);
	float3 nR = SampleNormal(uv + float2(texel.x, 0));
	float3 nL = SampleNormal(uv + float2(-texel.x, 0));
	float3 nU = SampleNormal(uv + float2(0, texel.y));
	float3 nD = SampleNormal(uv + float2(0, -texel.y));

	float normalEdge =
		max(max(1.f - dot(nC, nR), 1.f - dot(nC, nL)),
			max(1.f - dot(nC, nU), 1.f - dot(nC, nD)));

	float skyGuard = (dC >= 0.999f) ? 0.f : 1.f;

	float combinedEdge = (depthEdge * g_fOutlineDepthStrength
		+ normalEdge * g_fOutlineNormalStrength) * skyGuard;

	combinedEdge = smoothstep(g_fOutlineThresholdLow, g_fOutlineThresholdHigh, combinedEdge);
	combinedEdge *= g_fOutlineStrength;

	if (g_iOutlineMode == 1)
	{
		Out.vBackBuffer = float4(depthEdge.xxx, 1.f);
	}
	else if (g_iOutlineMode == 2)
	{
		Out.vBackBuffer = float4(normalEdge.xxx, 1.f);
	}
	else if (g_iOutlineMode == 3)
	{
		Out.vBackBuffer = float4(combinedEdge.xxx, 1.f);
	}
	else
	{
		Out.vBackBuffer = g_TexCombined.Sample(PointSampler, uv);
		if (0.5f > Out.vBackBuffer.a)
			discard;
	}

	return Out;
}

PS_OUT_BACKBUFFER PS_MAIN_OUTLINE(PS_IN In)
{
	PS_OUT_BACKBUFFER Out;

	float2 uv = In.vTex;

	float4 vColor = g_TexCombined.Sample(PointSampler, uv);
	if (0.5f > vColor.a)
		discard;

	float2 texel = g_vTexelSize * g_fOutlineThicknessPx;

	float dC = SampleDepth(uv);
	float dR = SampleDepth(uv + float2(texel.x, 0));
	float dL = SampleDepth(uv + float2(-texel.x, 0));
	float dU = SampleDepth(uv + float2(0, texel.y));
	float dD = SampleDepth(uv + float2(0, -texel.y));

	float depthEdge =
		max(max(abs(dC - dR), abs(dC - dL)),
			max(abs(dC - dU), abs(dC - dD)));

	float3 nC = SampleNormal(uv);
	float3 nR = SampleNormal(uv + float2(texel.x, 0));
	float3 nL = SampleNormal(uv + float2(-texel.x, 0));
	float3 nU = SampleNormal(uv + float2(0, texel.y));
	float3 nD = SampleNormal(uv + float2(0, -texel.y));

	float normalEdge =
		max(max(1.f - dot(nC, nR), 1.f - dot(nC, nL)),
			max(1.f - dot(nC, nU), 1.f - dot(nC, nD)));

	float skyGuard = (dC >= 0.999f) ? 0.f : 1.f;

	float edge = (depthEdge * g_fOutlineDepthStrength
		+ normalEdge * g_fOutlineNormalStrength) * skyGuard;

	edge = smoothstep(g_fOutlineThresholdLow, g_fOutlineThresholdHigh, edge);
	edge *= g_fOutlineStrength;

	if (g_iOutlineMode == 5)
	{
		float mC = SampleMask(uv);
		float mR = SampleMask(uv + float2(texel.x, 0));
		float mL = SampleMask(uv + float2(-texel.x, 0));
		float mU = SampleMask(uv + float2(0, texel.y));
		float mD = SampleMask(uv + float2(0, -texel.y));

		float maskEdge = max(max(abs(mC - mR), abs(mC - mL)),
			max(abs(mC - mU), abs(mC - mD)));

		edge *= step(g_fOutlineMaskBias, maskEdge);
	}

	vColor.rgb = lerp(vColor.rgb, vColor.rgb * g_fOutlineDarkenFactor, edge);
	Out.vBackBuffer = vColor;

	return Out;
}



technique11 DefaultTechnique
{
	pass Copy
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Z_Disable, 0);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_COPY();
	}

	pass Outline_Debug
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Z_Disable, 0);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_OUTLINE_DEBUG();
	}
	pass Outline_Darken
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Z_Disable, 0);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_OUTLINE();
	}
}