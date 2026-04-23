#include "Engine_Shader_Defines.hlsli"

float4x4 g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
texture2D g_Texture;

float4 g_vCamPos;

struct VS_IN
{
	float3 vPos : POSITION;
	row_major float4x4 TransformMatrix : WORLD;
	float2 vLifeTime : TEXCOORD0;
};

struct VS_OUT
{
	float4 vPos : POSITION;
	float2 vPSize : PSIZE;
	float2 vLifeTime : TEXCOORD0;
};

VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT Out;

	float4 vPos = mul(float4(In.vPos, 1.f), In.TransformMatrix);

	vPos = mul(vPos, g_WorldMatrix);
	
	Out.vPos = vPos;
	Out.vPSize = float2(length(In.TransformMatrix._11_12_13), length(In.TransformMatrix._21_22_23));
	Out.vLifeTime = In.vLifeTime;
	return Out;
}

struct GS_IN
{
	float4 vPos : POSITION;
	float2 vPSize : PSIZE;
	float2 vLifeTime : TEXCOORD0;
};

struct GS_OUT
{
	float4 vPos : SV_POSITION;
	float2 vTex : TEXCOORD0;
	float2 vLifeTime : TEXCOORD1;
};

[maxvertexcount(6)]
void GS_MAIN(point GS_IN In[1], inout TriangleStream<GS_OUT> OutStream)
{
	GS_OUT Out[4];

	float3 vLook = (g_vCamPos - In[0].vPos).xyz;
	float3 vRight = normalize(cross(float3(0.f, 1.f, 0.f), vLook)) * In[0].vPSize.x * 0.5f;
	float3 vUp = normalize(cross(vLook, vRight)) * In[0].vPSize.y * 0.5f;

	matrix matVP = mul(g_ViewMatrix, g_ProjMatrix);

	Out[0].vPos = mul(vector(In[0].vPos.xyz + vRight + vUp, 1.f), matVP);
	Out[0].vTex = float2(0.f, 0.f);
	Out[0].vLifeTime = In[0].vLifeTime;

	Out[1].vPos = mul(vector(In[0].vPos.xyz - vRight + vUp, 1.f), matVP);
	Out[1].vTex = float2(1.f, 0.f);
	Out[1].vLifeTime = In[0].vLifeTime;

	Out[2].vPos = mul(vector(In[0].vPos.xyz - vRight - vUp, 1.f), matVP);
	Out[2].vTex = float2(1.f, 1.f);
	Out[2].vLifeTime = In[0].vLifeTime;

	Out[3].vPos = mul(vector(In[0].vPos.xyz + vRight - vUp, 1.f), matVP);
	Out[3].vTex = float2(0.f, 1.f);
	Out[3].vLifeTime = In[0].vLifeTime;

	OutStream.Append(Out[0]);
	OutStream.Append(Out[1]);
	OutStream.Append(Out[2]);
	OutStream.RestartStrip();

	OutStream.Append(Out[0]);
	OutStream.Append(Out[2]);
	OutStream.Append(Out[3]);
	OutStream.RestartStrip();
}

struct PS_IN
{
	float4 vPos : SV_POSITION;
	float2 vTex : TEXCOORD0;
	float2 vLifeTime : TEXCOORD1;
};

struct PS_OUT
{
	float4 vCol : SV_TARGET0;
};

PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT Out;
	Out.vCol = g_Texture.Sample(LinearSampler, In.vTex);
	Out.vCol.rgb += 1.f - saturate(In.vLifeTime.x - In.vLifeTime.y);
	Out.vCol.a = Out.vCol.a * saturate(In.vLifeTime.x - In.vLifeTime.y);
	return Out;
}

technique11 DefaultTechnique
{
	pass DefaultPass
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = compile gs_5_0 GS_MAIN();
		PixelShader = compile ps_5_0 PS_MAIN();
	}
};