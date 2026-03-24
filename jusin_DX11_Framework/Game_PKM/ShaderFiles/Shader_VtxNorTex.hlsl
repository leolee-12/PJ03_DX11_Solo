float4x4 g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;

vector g_vCamPos;

// 임시 광원
vector g_vLightDir = vector(1.f, -1.f, 1.f, 0.f);
vector g_vLightDiff = vector(1.f, 1.f, 1.f, 1.f);
vector g_vLightAmbt = vector(1.f, 1.f, 1.f, 1.f);
vector g_vLightSpec = vector(1.f, 1.f, 1.f, 1.f);

// 임시 재질
texture2D g_TexDiff;
vector g_vMtrlAmbt = vector(0.4f, 0.4f, 0.4f, 1.f);
vector g_vMtrlSpec = vector(1.f, 1.f, 1.f, 1.f);

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
	float4 vNorm : NORMAL;
	float2 vTex : TEXCOORD0;
	float4 vWorldPos : TEXCOORD1;
};

VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT Out;

	float4 vPos = mul(float4(In.vPos, 1.0f), g_WorldMatrix);
	vPos = mul(vPos, g_ViewMatrix);
	vPos = mul(vPos, g_ProjMatrix);
	
	Out.vPos = vPos;
	Out.vNorm = normalize(mul(float4(In.vNorm, 0.f), g_WorldMatrix));
	Out.vTex = In.vTex;
	Out.vWorldPos = mul(float4(In.vPos, 1.f), g_WorldMatrix);
	return Out;
}

struct PS_IN
{
	float4 vPos : SV_POSITION;
	float4 vNorm : NORMAL;
	float2 vTex : TEXCOORD0;
	float4 vWorldPos : TEXCOORD1;
};

struct PS_OUT
{
	float4 vCol : SV_TARGET0;
};

PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT Out;
	vector vMtrlDiff = g_TexDiff.Sample(DefaultSampler, In.vTex);

	if (vMtrlDiff.a < 0.1f)	// 일정 a값 미만은 버림 (알파테스트)
		discard;

	vector Normal = normalize(In.vNorm);
	vector Light = normalize(g_vLightDir);

	vector vLook = In.vWorldPos - g_vCamPos;
	vector vReflect = reflect(Light, Normal);
	float fSpec = pow(max(dot(normalize(vLook) * -1.f, normalize(vReflect)), 0.f), 50.f);

	// ---------- Phong 모델 ----------
	float fShade = max(dot(Light * -1.f, Normal), 0.f);
	vector Diff = (g_vLightDiff * vMtrlDiff) * fShade;
	vector Ambt = (g_vLightAmbt * g_vMtrlAmbt) * vMtrlDiff; // 실무적 보정 (Ambient에 텍스처 반영)
	vector Spec = (g_vLightSpec * g_vMtrlSpec) * fSpec;
	Out.vCol = saturate(Diff + Ambt + Spec);
	// --------------------------------

	// ---------- 학원 모델 ----------
	//vector vShade = max(dot(Light * -1.f, Normal), 0.f) + (g_vLightAmbt * g_vMtrlAmbt);
	//Out.vCol = g_vLightDiff * vMtrlDiff * saturate(vShade) + (g_vLightSpec * g_vMtrlSpec) * fSpec;
	// --------------------------------

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