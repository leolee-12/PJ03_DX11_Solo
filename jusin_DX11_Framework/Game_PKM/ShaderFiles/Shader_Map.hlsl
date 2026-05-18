#include "Engine_Shader_Defines.hlsli"

float4x4 g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
float4x4 g_WITMatrix;
float g_fFarZ;

// ÀçÁú
texture2D g_TexDiff;
texture2D g_TexDiff2;
texture2D g_TexDiff3;
texture2D g_TexDiff4;

texture2D g_TexOpct;
texture2D g_TexData;
texture2D g_TexMask;

vector g_vDefaultAmbt = vector(0.f, 0.f, 0.5f, 0.f);

struct VS_IN
{
	float3 vPos : POSITION;
	float3 vNorm : NORMAL;
	float2 vTex : TEXCOORD0;

	float3 vTangent : TANGENT;
	float3 vBinormal : BINORMAL;
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

	float4 vPos = mul(float4(In.vPos, 1.0f), g_WorldMatrix);
	vPos = mul(vPos, g_ViewMatrix);
	vPos = mul(vPos, g_ProjMatrix);

	Out.vPos = vPos;
	Out.vNorm = float4(normalize(mul(In.vNorm, (float3x3)g_WITMatrix)), 0.f);
	Out.vTex = In.vTex;
	Out.vWorldPos = mul(float4(In.vPos, 1.f), g_WorldMatrix);
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
	float4 vPickPos : SV_TARGET4;
};

PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT Out;
	vector vMtrlDiff = g_TexDiff.Sample(LinearSampler, In.vTex);

	Out.vDiff = vector(vMtrlDiff.rgb, 1.f);
	Out.vNorm = vector(normalize(In.vNorm.xyz) * 0.5f + 0.5f, 1.f);
	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fFarZ, 0.f, 0.f);
	Out.vPickPos = vector(In.vWorldPos.xyz, 1.f);
	Out.vAmbt = g_vDefaultAmbt;
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

PS_OUT PS_DISCARD(PS_IN In)
{
	discard;
	PS_OUT Out = (PS_OUT)0;
	return Out;
}

PS_OUT PS_TREE(PS_IN In)
{
	PS_OUT Out;
	vector vMtrlDiff = g_TexDiff.Sample(LinearSampler, In.vTex);
	float fOpacity = g_TexOpct.Sample(LinearSampler, In.vTex).a;

	if (fOpacity < 0.5f)
		discard;
	
	Out.vDiff = vector(vMtrlDiff.rgb, 1.f);
	Out.vNorm = vector(normalize(In.vNorm.xyz) * 0.5f + 0.5f, 1.f);
	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fFarZ, 0.f, 0.f);
	Out.vPickPos = vector(In.vWorldPos.xyz, 1.f);
	Out.vAmbt = g_vDefaultAmbt;
	return Out;
}

PS_OUT PS_TREE2(PS_IN In)
{
	PS_OUT Out;
	vector vMtrlDiff = g_TexDiff.Sample(LinearSampler, In.vTex);
	float fOpacity = g_TexOpct.Sample(LinearSampler, In.vTex).a;

	if (fOpacity < 0.5f)
		discard;

	vector vData = g_TexData.Sample(LinearSampler, In.vTex);

	Out.vDiff = vector(vMtrlDiff.rgb + vData.rgb, 1.f);
	Out.vNorm = vector(normalize(In.vNorm.xyz) * 0.5f + 0.5f, 1.f);
	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fFarZ, 0.f, 0.f);
	Out.vPickPos = vector(In.vWorldPos.xyz, 1.f);
	Out.vAmbt = g_vDefaultAmbt;
	return Out;
}

PS_OUT PS_ALPHATEST(PS_IN In)
{
	PS_OUT Out;
	vector vMtrlDiff = g_TexDiff.Sample(LinearSampler, In.vTex);

	if (vMtrlDiff.a < 0.1f)
		discard;

	Out.vDiff = vector(vMtrlDiff.rgb, 1.f);
	Out.vNorm = vector(normalize(In.vNorm.xyz) * 0.5f + 0.5f, 1.f);
	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fFarZ, 0.f, 0.f);
	Out.vPickPos = vector(In.vWorldPos.xyz, 1.f);
	Out.vAmbt = g_vDefaultAmbt;
	return Out;
}

PS_OUT PS_GLASS(PS_IN In)
{
	PS_OUT Out;
	float4 vMtrlDiff = g_TexDiff.Sample(LinearSampler, In.vTex);

	float fLuma = dot(vMtrlDiff.rgb, float3(0.299f, 0.587f, 0.114f));
	float fEdge = smoothstep(0.03f, 0.28f, fLuma);

	float fVertical = saturate(In.vTex.y);
	float3 vGlassTop = float3(0.52f, 0.84f, 0.90f);
	float3 vGlassBottom = float3(0.86f, 0.97f, 0.99f);
	float3 vGlassColor = lerp(vGlassTop, vGlassBottom, fVertical);

	vGlassColor = lerp(vGlassColor, float3(1.f, 1.f, 1.f), fEdge * 0.30f);

	Out.vDiff = vector(vGlassColor, 1.f);
	Out.vNorm = vector(normalize(In.vNorm.xyz) * 0.5f + 0.5f, 1.f);
	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fFarZ, 0.f, 0.f);
	Out.vPickPos = vector(In.vWorldPos.xyz, 1.f);
	Out.vAmbt = g_vDefaultAmbt;
	return Out;
}

PS_OUT PS_GRASS(PS_IN In)
{
	PS_OUT Out;
	float4 vDiff1 = g_TexDiff.Sample(LinearSampler, In.vTex);

	if (vDiff1.a < 0.4f)
		discard;

	Out.vDiff = vector(vDiff1.rgb, 1.f);
	Out.vNorm = vector(normalize(In.vNorm.xyz) * 0.5f + 0.5f, 1.f);
	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fFarZ, 0.f, 0.f);
	Out.vPickPos = vector(In.vWorldPos.xyz, 1.f);
	Out.vAmbt = g_vDefaultAmbt;
	return Out;
}

PS_OUT PS_SOIL(PS_IN In)
{
	PS_OUT Out;
	float4 vDiff1 = g_TexDiff.Sample(LinearSampler, In.vTex);
	float4 vDiff2 = g_TexDiff2.Sample(LinearSampler, In.vTex);
	float4 vDiff3 = g_TexDiff3.Sample(LinearSampler, In.vTex);
	float4 vDiff4 = g_TexDiff4.Sample(LinearSampler, In.vTex);

	Out.vDiff = vector(vDiff3.rgb, 1.f);
	Out.vNorm = vector(normalize(In.vNorm.xyz) * 0.5f + 0.5f, 1.f);
	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fFarZ, 0.f, 0.f);
	Out.vPickPos = vector(In.vWorldPos.xyz, 1.f);
	Out.vAmbt = g_vDefaultAmbt;
	return Out;
}

PS_OUT PS_SOIL2(PS_IN In)
{
	PS_OUT Out;
	float4 vDiff1 = g_TexDiff.Sample(LinearSampler, In.vTex);
	float4 vDiff2 = g_TexDiff2.Sample(LinearSampler, In.vTex);

	Out.vDiff = vector(vDiff2.rgb, 1.f);
	Out.vNorm = vector(normalize(In.vNorm.xyz) * 0.5f + 0.5f, 1.f);
	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fFarZ, 0.f, 0.f);
	Out.vPickPos = vector(In.vWorldPos.xyz, 1.f);
	Out.vAmbt = g_vDefaultAmbt;
	return Out;
}

technique11 DefaultTechnique
{
	pass DefaultPass
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN();
	}
	pass Pass_Shadow	// 1. Shadow
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_SHADOW();
	}
	pass Pass_Discard       // 2. Hidden temp material
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_DISCARD();
	}
	pass Pass_Tree	// 3. Tree Diff, Opct
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_TREE();
	}
	pass Pass_Tree2	// 4. Tree Diff, Opct, Data
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_TREE2();
	}
	pass Pass_AlphaTest	// 5. Flower
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_ALPHATEST();
	}
	pass Pass_Glass	// 6. Glass
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_GLASS();
	}
	pass Pass_Grass	// 7. Grass
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_GRASS();
	}
	pass Pass_Soil	// 8. Ground - Diff * 4
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_SOIL();
	}
	pass Pass_Soil2	// 9. Ground - Diff * 2
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_SOIL2();
	}
};