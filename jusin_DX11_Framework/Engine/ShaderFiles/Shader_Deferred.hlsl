#include "Engine_Shader_Defines.hlsli"

float4x4 g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
float4x4 g_ViewInvMatrix, g_ProjInvMatrix;
float4x4 g_SLViewMatrix, g_SLProjMatrix;

texture2D g_Texture;
texture2D g_TexNorm;
texture2D g_TexDepth;
texture2D g_TexDiff;
texture2D g_TexSpec;
texture2D g_TexShade;
texture2D g_TexLightDepth;

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

float g_fShadowFarZ;



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

	// 투영
	vector vWorldPos;
	vWorldPos.x = In.vTex.x * 2.f - 1.f;
	vWorldPos.y = In.vTex.y * -2.f + 1.f;
	vWorldPos.z = vDepthDesc.x;
	vWorldPos.w = 1.f;

	// 뷰
	vWorldPos *= fViewZ;
	vWorldPos = mul(vWorldPos, g_ProjInvMatrix);

	// 월드
	vWorldPos = mul(vWorldPos, g_ViewInvMatrix);

	vector vReflect = reflect(normalize(g_vLightDir), normalize(vNormal));
	vector vLook = vWorldPos - g_vCamPos;
	Out.vSpec = (g_vLightSpec * g_vMtrlSpec) * pow(saturate(dot(normalize(vReflect) * -1.f, normalize(vLook))), 50.f);
	float fNdotL = dot(normalize(g_vLightDir) * -1.f, normalize(vNormal));
	float fHalfLambert = fNdotL * 0.5f + 0.5f;
	//fHalfLambert *= fHalfLambert; // Valve Half-Lambert: 라이트 면 또렷, 반대 면 부드럽게
	Out.vShade = g_vLightDiff * fHalfLambert + g_vLightAmbt * g_vMtrlAmbt;

		//vector vMtrlSpec = g_TexSpec.Sample(LinearSampler, In.vTex);

	//vector Light = normalize(g_vLightDir);
	//vector vView = normalize(g_vCamPos - In.vWorldPos);
	//vector vHalf = normalize((-Light) + vView);
	//float fSpec = pow(max(dot(Normal, vHalf), 0.f), 150.f);
	//float fShade = max(dot(Light * -1.f, Normal), 0.f);
	//
	//vector Diff = (g_vLightDiff * vMtrlDiff) * fShade;
	//vector Ambt = g_vLightAmbt * vMtrlDiff;
	//vector Spec = (g_vLightSpec * vMtrlSpec) * fSpec;
	//Out.vCol = saturate(Diff + Ambt + Spec);
	//return Out;

	return Out;
}

PS_OUT_LIGHT PS_MAIN_POINT(PS_IN In)
{
	PS_OUT_LIGHT Out = (PS_OUT_LIGHT)0;

	vector vNormalDesc = g_TexNorm.Sample(LinearSampler, In.vTex);
	vector vDepthDesc = g_TexDepth.Sample(LinearSampler, In.vTex);
	float4 vNormal = vector(vNormalDesc.xyz * 2.f - 1.f, 0.f);
	float fViewZ = vDepthDesc.y * g_fFarZ;
	
	// 투영
	vector vWorldPos;
	vWorldPos.x = In.vTex.x * 2.f - 1.f;
	vWorldPos.y = -(In.vTex.y * 2.f - 1.f);
	vWorldPos.z = vDepthDesc.x;
	vWorldPos.w = 1.f;

	// 뷰
	vWorldPos *= fViewZ;
	vWorldPos = mul(vWorldPos, g_ProjInvMatrix);

	// 월드
	vWorldPos = mul(vWorldPos, g_ViewInvMatrix);

	vector vLightDir = vWorldPos - g_vLightPos;
	float fAtt = saturate((g_fLightRange - length(vLightDir)) / g_fLightRange);

	vector vReflect = reflect(normalize(vLightDir), normalize(vNormal));
	vector vLook = vWorldPos - g_vCamPos;
	Out.vSpec = (g_vLightSpec * g_vMtrlSpec) * pow(saturate(dot(normalize(vReflect) * -1.f, normalize(vLook))), 50.f) * fAtt;

	float fNdotL = dot(normalize(vLightDir) * -1.f, normalize(vNormal));
	float fHalfLambert = fNdotL * 0.5f + 0.5f;
	fHalfLambert *= fHalfLambert;
	Out.vShade = (g_vLightDiff * fHalfLambert + g_vLightAmbt * g_vMtrlAmbt) * fAtt;

	return Out;
}

PS_OUT_BACKBUFFER PS_MAIN_COMBINED(PS_IN In)
{
	PS_OUT_BACKBUFFER Out;

	// Diffuse + Shade -> 최종 색상 결정
	vector vDiffuse = g_TexDiff.Sample(LinearSampler, In.vTex);
	if (0.5f > vDiffuse.a)
		discard;

	vector vShade = g_TexShade.Sample(LinearSampler, In.vTex);
	vector vSpec = g_TexSpec.Sample(LinearSampler, In.vTex);
	Out.vBackBuffer = vDiffuse * vShade + vSpec;

	// 그림자 반영
	vector vDepthDesc = g_TexDepth.Sample(LinearSampler, In.vTex);
	float fViewZ = vDepthDesc.y * g_fFarZ;

	vector vWorldPos;

	// - 투영 스페이스 위치
	vWorldPos.x = In.vTex.x * 2.f - 1.f;
	vWorldPos.y = In.vTex.y * -2.f + 1.f;
	vWorldPos.z = vDepthDesc.x;
	vWorldPos.w = 1.f;

	// - 뷰 스페이스 위치
	vWorldPos *= fViewZ;
	vWorldPos = mul(vWorldPos, g_ProjInvMatrix);
	
	// - 월드 스페이스 위치 -> 광원 시점 Clip 스페이스
	vWorldPos = mul(vWorldPos, g_ViewInvMatrix);
	vWorldPos = mul(vWorldPos, g_SLViewMatrix);
	vWorldPos = mul(vWorldPos, g_SLProjMatrix);

	// 직교투영: w=1, NDC z 그대로 사용
	float fLightNDCZ = vWorldPos.z / vWorldPos.w;

	float2 vTexcoord;
	vTexcoord.x = (vWorldPos.x / vWorldPos.w) * 0.5f + 0.5f;	/* -1 ~ 1 => 0 ~ 1 */
	vTexcoord.y = (vWorldPos.y / vWorldPos.w) * -0.5f + 0.5f;   /* 1 ~ -1 => 0 ~ 1 */

	// 그림자 맵 텍셀 크기
	float fW, fH;
	g_TexLightDepth.GetDimensions(fW, fH);
	float2 vTexelSize = float2(1.f / fW, 1.f / fH);
	
	// 3x3 PCF (각 탭은 하드웨어 2x2 PCF -> 실효 4x4)
	float fLitFactor = 0.f;
	[unroll]
		for (int y = -1; y <= 1; ++y)
		{
			[unroll]
			for (int x = -1; x <= 1; ++x)
			{
				float2 vOffset = float2(x, y) * vTexelSize;
				fLitFactor += g_TexLightDepth.SampleCmpLevelZero(
					ShadowCompareSampler, vTexcoord + vOffset, fLightNDCZ - 0.0005f).r;
			}
		}
	fLitFactor /= 9.f;
	
	// Contrast remap: 깊은 그림자/완전 빛은 클램프, 경계만 S-커브로 부드럽게
	fLitFactor = smoothstep(0.f, 1.f, fLitFactor);
	Out.vBackBuffer = Out.vBackBuffer * lerp(0.65f, 1.0f, fLitFactor);
	
	// --- 톤매핑 (휘도 기반 확장 Reinhard - 색 비율 유지, 채도 보존) ---
	float fExposure = 2.6f;   // 2.0 -> 2.2 (전체 밝기 보강)
	float fWhitePoint = 4.0f;
	float3 vHDR = Out.vBackBuffer.rgb * fExposure;
	float fL = dot(vHDR, float3(0.2126f, 0.7152f, 0.0722f));
	float fLmapped = (fL * (1.0f + fL / (fWhitePoint * fWhitePoint))) / (1.0f + fL);
	float3 vMapped = vHDR * (fLmapped / max(fL, 1e-4f));

	// 휘도 기반 톤매핑이 이미 색을 보존하므로 추가 채도 부스트 불필요
	vMapped = saturate(vMapped);

	Out.vBackBuffer = float4(vMapped, Out.vBackBuffer.a);

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