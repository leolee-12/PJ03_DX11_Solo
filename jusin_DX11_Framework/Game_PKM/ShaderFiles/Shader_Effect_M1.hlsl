#include "Engine_Shader_Defines.hlsli"

// M1 temporary shader. Replaced by Shader_Particle3D.hlsl at M4.

float4x4 g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
texture2D g_Texture;

float4 g_vColor = float4(1.f, 1.f, 1.f, 1.f);
float  g_fAlpha = 1.f;

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

float4 PS_MAIN(VS_OUT In) : SV_TARGET0
{
	float4 vColor = g_Texture.Sample(LinearSampler, In.vTex) * g_vColor;
	vColor.a *= saturate(g_fAlpha);
	return vColor;
}

technique11 DefaultTechnique
{
	pass DefaultPass
	{
		SetRasterizerState(RS_Cull_None);
		SetDepthStencilState(DSS_Z_Disable, 0);
		SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN();
	}
};