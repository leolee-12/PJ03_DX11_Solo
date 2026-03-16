
float4x4 g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;

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

	//float4 vPos = mul(float4(In.vPos, 1.0f), g_WorldMatrix);
	//vPos = mul(vPos, g_ViewMatrix);
	//vPos = mul(vPos, g_ProjMatrix);
	
	//Out.Pos = vPos;
	Out.vPos = float4(In.vPos, 1.f);
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
	Out.vCol = In.vTex.y;
	return Out;
}

technique11 DefaultTechnique
{
	pass DefaultPass
	{
		VertexShader = compile vs_5_0 VS_MAIN();
		PixelShader = compile ps_5_0 PS_MAIN();
	}
};