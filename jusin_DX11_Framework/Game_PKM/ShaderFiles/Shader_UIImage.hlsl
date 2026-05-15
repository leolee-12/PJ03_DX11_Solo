#include "Engine_Shader_Defines.hlsli"

float4x4 g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;

texture2D g_Texture;
texture2D g_TexDiff;

float4 g_vColor = float4(1.f, 1.f, 1.f, 1.f);
float g_fFadeTime = 0.f;

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

PS_OUT PS_SAMPLE(float2 vUV)
{
	PS_OUT Out;

	Out.vCol = g_Texture.Sample(MirrorSampler, vUV) * g_vColor;

	if (Out.vCol.a < 0.01f)
		discard;

	return Out;
}

PS_OUT PS_MAIN_SINGLE(PS_IN In)
{
	return PS_SAMPLE(In.vTex);
}

PS_OUT PS_MAIN_MIRROR_V(PS_IN In)
{
	return PS_SAMPLE(float2(In.vTex.x, In.vTex.y * 2.f));
}

PS_OUT PS_MAIN_MIRROR_H(PS_IN In)
{
	return PS_SAMPLE(float2(In.vTex.x * 2.f, In.vTex.y));
}

PS_OUT PS_MAIN_MIRROR_QUAD(PS_IN In)
{
	return PS_SAMPLE(In.vTex * 2.f);
}

PS_OUT PS_FADEBATTLE(PS_IN In)
{
	PS_OUT Out = (PS_OUT)0;

	float2 uv = In.vTex;

	float4 mask = g_Texture.Sample(MirrorSampler, uv);
	if (mask.a < 0.01f)
		discard;

	float2 diffUV = uv;
	diffUV.x -= g_fFadeTime * 0.35f;

	float4 diff = g_TexDiff.Sample(LinearSampler, diffUV);

	Out.vCol = diff * mask + g_vColor;

	if (Out.vCol.a < 0.01f)
		discard;

	return Out;
}

technique11 DefaultTechnique
{
pass Pass_Single        // 0: ´ÜÀÏ ÀÌ¹ÌÁö
		{
				SetRasterizerState(RS_Cull_None);
				SetDepthStencilState(DSS_Z_Disable, 0);
				SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

				VertexShader = compile vs_5_0 VS_MAIN();
				PixelShader = compile ps_5_0 PS_MAIN_SINGLE();
		}

		pass Pass_Mirror_V      // 1: »óÇÏ´ëÄª
		{
				SetRasterizerState(RS_Cull_None);
				SetDepthStencilState(DSS_Z_Disable, 0);
				SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

				VertexShader = compile vs_5_0 VS_MAIN();
				PixelShader = compile ps_5_0 PS_MAIN_MIRROR_V();
		}

		pass Pass_Mirror_H      // 2: ÁÂ¿ì´ëÄª
		{
				SetRasterizerState(RS_Cull_None);
				SetDepthStencilState(DSS_Z_Disable, 0);
				SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

				VertexShader = compile vs_5_0 VS_MAIN();
				PixelShader = compile ps_5_0 PS_MAIN_MIRROR_H();
		}

		pass Pass_Mirror_Quad   // 3: 4¹æÇâ ´ëÄª
		{
				SetRasterizerState(RS_Cull_None);
				SetDepthStencilState(DSS_Z_Disable, 0);
				SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

				VertexShader = compile vs_5_0 VS_MAIN();
				PixelShader = compile ps_5_0 PS_MAIN_MIRROR_QUAD();
		}

		pass Pass_FadeBattle	// 4
		{
				SetRasterizerState(RS_Cull_None);
				SetDepthStencilState(DSS_Z_Disable, 0);
				SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

				VertexShader = compile vs_5_0 VS_MAIN();
				PixelShader = compile ps_5_0 PS_FADEBATTLE();
		}
};