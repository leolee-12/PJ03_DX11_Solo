float4x4 g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
float4x4 g_WITMatrix;
vector g_vCamPos;

// 광원
vector g_vLightDir;
vector g_vLightDiff;
vector g_vLightAmbt;
vector g_vLightSpec;

// 재질
texture2D g_TexDiff;
vector g_vMtrlAmbt = vector(0.4f, 0.4f, 0.4f, 1.f);
vector g_vMtrlSpec = vector(1.f, 1.f, 1.f, 1.f);

sampler DefaultSampler = sampler_state
{	// D3D11_SAMPLER_DESC 참고
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = WRAP;
	AddressV = WRAP;
};

RasterizerState RS_CullBack
{
	FillMode = Solid;
	CullMode = Back;
	FrontCounterClockwise = false;
};

RasterizerState RS_CullNone
{
	FillMode = Solid;
	CullMode = None;
	FrontCounterClockwise = false;
};

RasterizerState RS_CullBack_CCW
{
	FillMode = Solid;
	CullMode = Back;
	FrontCounterClockwise = true;
};

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
};

VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT Out;

	float4 vPos = mul(float4(In.vPos, 1.0f), g_WorldMatrix);
	vPos = mul(vPos, g_ViewMatrix);
	vPos = mul(vPos, g_ProjMatrix);
	
	Out.vPos = vPos;
	Out.vNorm = normalize(mul(float4(In.vNorm, 0.f), g_WITMatrix));
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

PS_OUT PS_MAIN(PS_IN In)	// Phong Model
{
	PS_OUT Out;
	vector vMtrlDiff = g_TexDiff.Sample(DefaultSampler, In.vTex);

	//if (vMtrlDiff.a < 0.1f)	// 일정 a값 미만은 버림 (알파테스트)
	//	discard;

	vector Normal = normalize(In.vNorm);
	vector Light = normalize(g_vLightDir);

	vector vView = normalize(g_vCamPos - In.vWorldPos);
	vector vReflect = reflect(Light, Normal);
	float fSpec = pow(max(dot(vView, normalize(vReflect)), 0.f), 50.f);

	// ---------- Phong 모델 ----------
	float fShade = max(dot(Light * -1.f, Normal), 0.f);
	vector Diff = (g_vLightDiff * vMtrlDiff) * fShade;
	vector Ambt = (g_vLightAmbt * g_vMtrlAmbt) * vMtrlDiff;
	vector Spec = (g_vLightSpec * g_vMtrlSpec) * fSpec;
	Out.vCol = saturate(Diff + Ambt + Spec);
	// 정확한 Phong 모델은 Diff + Ambt + Spec이지만, Ambt에 vMtrlDiff를 곱해주면 좀 더 자연스러운 결과가 나옴 (Ambt도 텍스처 색상값에 비례하도록)
	// --------------------------------

	// ---------- 학원 모델 -----------
	//vector vShade = max(dot(Light * -1.f, Normal), 0.f) + (g_vLightAmbt * g_vMtrlAmbt);
	//Out.vCol = g_vLightDiff * vMtrlDiff * saturate(vShade) + (g_vLightSpec * g_vMtrlSpec) * fSpec;
	// --------------------------------

	return Out;
}

PS_OUT PS_MAIN_BLINNPHONG(PS_IN In)	// Blinn-Phong Model
{
	PS_OUT Out;

	vector vMtrlDiff = g_TexDiff.Sample(DefaultSampler, In.vTex);

	//if (vMtrlDiff.a < 0.1f)	// 일정 a값 미만은 버림 (알파테스트)
	//	discard;

	vector Normal = normalize(In.vNorm);
	vector Light = normalize(g_vLightDir);
	
	vector vView = normalize(g_vCamPos - In.vWorldPos);
	vector vHalf = normalize((-Light) + vView);
	float fSpec = pow(max(dot(Normal, vHalf), 0.f), 150.f);

	// ---------- Blinn-Phong 모델 ----------
	float fShade = max(dot(Light * -1.f, Normal), 0.f);
	vector Diff = (g_vLightDiff * vMtrlDiff) * fShade;
	vector Ambt = (g_vLightAmbt * g_vMtrlAmbt) * vMtrlDiff; // Ambt*TexDiff 보정
	vector Spec = (g_vLightSpec * g_vMtrlSpec) * fSpec;
	Out.vCol = saturate(Diff + Ambt + Spec);
	// --------------------------------

	// ---------- 학원 모델 -----------
	//vector vShade = max(dot(Light * -1.f, Normal), 0.f) + (g_vLightAmbt * g_vMtrlAmbt);
	//Out.vCol = g_vLightDiff * vMtrlDiff * saturate(vShade) + (g_vLightSpec * g_vMtrlSpec) * fSpec;
	// --------------------------------

	return Out;
}

technique11 DefaultTechnique
{
	pass DefaultPass	// 0. Phong Model
	{
		VertexShader = compile vs_5_0 VS_MAIN();
		PixelShader = compile ps_5_0 PS_MAIN();
	}
	pass BlinnPhongPass	// 1. Blinn-Phong Model
	{
		VertexShader = compile vs_5_0 VS_MAIN();
		PixelShader = compile ps_5_0 PS_MAIN_BLINNPHONG();
	}
	pass Pass2	// 후면컬링
	{
		SetRasterizerState(RS_CullBack);

		VertexShader = compile vs_5_0 VS_MAIN();
		PixelShader = compile ps_5_0 PS_MAIN();
	}

	pass Pass3 // 컬링X
	{
		SetRasterizerState(RS_CullNone);

		VertexShader = compile vs_5_0 VS_MAIN();
		PixelShader = compile ps_5_0 PS_MAIN();
	}

	pass Pass4 // 전면컬링
{
	SetRasterizerState(RS_CullBack_CCW);

	VertexShader = compile vs_5_0 VS_MAIN();
	PixelShader = compile ps_5_0 PS_MAIN();
}
};