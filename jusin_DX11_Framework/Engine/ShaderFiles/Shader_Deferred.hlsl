#include "Engine_Shader_Defines.hlsli"

float4x4 g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
float4x4 g_ViewInvMatrix, g_ProjInvMatrix;

texture2D g_Texture;
texture2D g_TexNorm;
texture2D g_TexDepth;
texture2D g_TexDiff;
texture2D g_TexSpec;
texture2D g_TexShade;

float g_fFarZ;
vector g_vCamPos;

vector g_vLightDir;
vector g_vLightPos;
float g_fLightRange;

vector g_vLightDiff;
vector g_vLightAmbt;
vector g_vLightSpec;

vector g_vMtrlAmbt = 1.f;
vector g_vMtrlSpec = 1.f;



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

PS_OUT_BACKBUFFER PS_MAIN_DEBUG(PS_IN In)
{
	PS_OUT_BACKBUFFER Out;

	Out.vBackBuffer = g_Texture.Sample(LinearSampler, In.vTex);

	return Out;
};



struct PS_OUT_LIGHT
{
	float4 vShade : SV_TARGET0;
	float4 vSpec : SV_TARGET1;
};

PS_OUT_LIGHT PS_MAIN_DIRECTIONAL(PS_IN In)
{
	PS_OUT_LIGHT Out = (PS_OUT_LIGHT)0;

	vector vNormalDesc = g_TexNorm.Sample(LinearSampler, In.vTex);
	vector vDepthDesc = g_TexDepth.Sample(LinearSampler, In.vTex);
	float4 vNormal = vector(vNormalDesc.xyz * 2.f - 1.f, 0.f);
	float fViewZ = vDepthDesc.y * g_fFarZ;

	// Åõ¿µ
	vector vWorldPos;
	vWorldPos.x = In.vTex.x * 2.f - 1.f;
	vWorldPos.y = In.vTex.y * -2.f + 1.f;
	vWorldPos.z = vDepthDesc.x;
	vWorldPos.w = 1.f;

	// ºä
	vWorldPos *= fViewZ;
	vWorldPos = mul(vWorldPos, g_ProjInvMatrix);

	// ¿ùµå
	vWorldPos = mul(vWorldPos, g_ViewInvMatrix);

	vector vReflect = reflect(normalize(g_vLightDir), normalize(vNormal));
	vector vLook = vWorldPos - g_vCamPos;
	Out.vSpec = (g_vLightSpec * g_vMtrlSpec) * pow(saturate(dot(normalize(vReflect) * -1.f, normalize(vLook))), 50.f);

	Out.vShade = g_vLightDiff * (saturate(dot(normalize(g_vLightDir) * -1.f, normalize(vNormal))) + (g_vLightAmbt * g_vMtrlAmbt));

	return Out;
}

PS_OUT_LIGHT PS_MAIN_POINT(PS_IN In)
{
	PS_OUT_LIGHT Out = (PS_OUT_LIGHT)0;

	vector vNormalDesc = g_TexNorm.Sample(LinearSampler, In.vTex);
	vector vDepthDesc = g_TexDepth.Sample(LinearSampler, In.vTex);
	float4 vNormal = vector(vNormalDesc.xyz * 2.f - 1.f, 0.f);
	float fViewZ = vDepthDesc.y * g_fFarZ;
	
	// Åõ¿µ
	vector vWorldPos;
	vWorldPos.x = In.vTex.x * 2.f - 1.f;
	vWorldPos.y = -(In.vTex.y * 2.f - 1.f);
	vWorldPos.z = vDepthDesc.x;
	vWorldPos.w = 1.f;

	// ºä
	vWorldPos *= fViewZ;
	vWorldPos = mul(vWorldPos, g_ProjInvMatrix);

	// ¿ùµå
	vWorldPos = mul(vWorldPos, g_ViewInvMatrix);

	vector vLightDir = vWorldPos - g_vLightPos;
	float fAtt = saturate((g_fLightRange - length(vLightDir)) / g_fLightRange);

	vector vReflect = reflect(normalize(vLightDir), normalize(vNormal));
	vector vLook = vWorldPos - g_vCamPos;
	Out.vSpec = (g_vLightSpec * g_vMtrlSpec) * pow(saturate(dot(normalize(vReflect) * -1.f, normalize(vLook))), 50.f) * fAtt;

	Out.vShade = g_vLightDiff * (saturate(dot(normalize(vLightDir) * -1.f, normalize(vNormal))) + (g_vLightAmbt * g_vMtrlAmbt)) * fAtt;
	
	return Out;
}

PS_OUT_BACKBUFFER PS_MAIN_COMBINED(PS_IN In)
{
	PS_OUT_BACKBUFFER Out;

	vector vDiffuse = g_TexDiff.Sample(LinearSampler, In.vTex);
	if (0.5f > vDiffuse.a)
		discard;

	vector vShade = g_TexShade.Sample(LinearSampler, In.vTex);
	vector vSpec = g_TexSpec.Sample(LinearSampler, In.vTex);

	Out.vBackBuffer = vDiffuse * vShade + vSpec;
	return Out;
}

technique11 DefaultTechnique
{
	pass Debug
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_DEBUG();
	}

	pass Directional
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Z_Disable, 0);
		SetBlendState(BS_Blend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_DIRECTIONAL();
	}

	pass Point
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Z_Disable, 0);
		SetBlendState(BS_Blend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_POINT();
	}

	pass Spot
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Z_Disable, 0);
		SetBlendState(BS_Blend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_DIRECTIONAL();
	}

	pass Combined
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Z_Disable, 0);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_COMBINED();
	}
}