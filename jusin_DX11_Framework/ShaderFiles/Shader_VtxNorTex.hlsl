
float4x4 g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
texture2D g_Texture;

sampler DefaultSampler = sampler_state
{	// D3D11_SAMPLER_DESC 참고
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = WRAP;
	AddressV = WRAP;
};

struct VS_IN
{
	float3 vPos : POSITION;
	float3 vNorm : NORMAL;
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
	Out.vCol = g_Texture.Sample(DefaultSampler, In.vTex);

	if (Out.vCol.a < 0.1f)	// 일정 a값 미만은 버림 (알파테스트)
		discard;

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