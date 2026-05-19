#include "Engine_Shader_Defines.hlsli"

float4x4 g_WorldMatrix;       // emitter root world
float4x4 g_ViewMatrix;
float4x4 g_ProjMatrix;
float4x4 g_ViewInvMatrix;     // 카메라 basis (world space) 추출용

float3   g_vFixedAxis = float3(0.f, 1.f, 0.f);
uint     g_iBillboardMode = 0;   // 0=VIEW_ALIGNED, 1=AXIS_LOCKED, 2=FIXED_NORMAL

texture2D g_Texture;

/* 공용 hlsli에 없는 state - 셰이더 파일 내부 local 정의.
   Engine_Shader_Defines.hlsli는 비수정(공용 자산). */
DepthStencilState DSS_DepthReadNoWrite
{
	DepthEnable = true;
	DepthWriteMask = ZERO;
	DepthFunc = LESS_EQUAL;
};

BlendState BS_Additive
{
	BlendEnable[0] = true;
	SrcBlend = ONE;
	DestBlend = ONE;
	BlendOp = ADD;
	SrcBlendAlpha = ONE;
	DestBlendAlpha = ONE;
	BlendOpAlpha = ADD;
	RenderTargetWriteMask[0] = 0x0F;
};

struct VS_IN
{
	float3 vPos : POSITION;        // quad corner (-0.5 ~ +0.5)
	float2 vTex : TEXCOORD0;

	float4 vCenterSize : TEXCOORD1;       // .xyz=vCenter (emitter local), .w=fSize
	float4 vRotPad : TEXCOORD2;       // .x=fRotation, .yzw=pad
	float4 vColor : TEXCOORD3;
	float4 vAgeLifePad : TEXCOORD4;       // .xy=(age,life), .zw=pad
	float4 vAtlasUV : TEXCOORD5;       // (offsetU, offsetV, scaleU, scaleV)
};

struct VS_OUT
{
	float4 vPos : SV_POSITION;
	float2 vTex : TEXCOORD0;
	float4 vColor : TEXCOORD1;
	float2 vAgeLife : TEXCOORD2;
};

VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT Out;

	/* 1) emitter local center -> world center */
	float3 vCenter_W = mul(float4(In.vCenterSize.xyz, 1.f), g_WorldMatrix).xyz;
	float  fSize = In.vCenterSize.w;
	float  fRotation = In.vRotPad.x;

	/* 2) 카메라 basis (월드 공간) - view 의 inverse 상단 3x3 */
	float3 vCamRight_W = g_ViewInvMatrix._11_12_13;
	float3 vCamUp_W = g_ViewInvMatrix._21_22_23;
	float3 vCamPos_W = g_ViewInvMatrix._41_42_43;

	float3 vRight, vUp;

	if (g_iBillboardMode == 0)        // VIEW_ALIGNED
	{
		vRight = vCamRight_W;
		vUp = vCamUp_W;
	}
	else if (g_iBillboardMode == 1)   // AXIS_LOCKED
	{
		vUp = normalize(g_vFixedAxis);
		float3 vToCam = normalize(vCamPos_W - vCenter_W);
		vRight = normalize(cross(vUp, vToCam));
	}
	else                              // FIXED_NORMAL - 월드 평면 고정 (XY 평면)
	{
		vRight = float3(1.f, 0.f, 0.f);
		vUp = float3(0.f, 1.f, 0.f);
	}

	/* 3) 빌보드 평면 안에서 fRotation 회전 */
	float s = sin(fRotation);
	float c = cos(fRotation);
	float3 vR = c * vRight + s * vUp;
	float3 vU = -s * vRight + c * vUp;

	/* 4) corner offset 적용 - In.vPos = (-0.5~+0.5, -0.5~+0.5, 0) */
	float3 vWorldPos = vCenter_W
		+ vR * (In.vPos.x * fSize)
		+ vU * (In.vPos.y * fSize);

	Out.vPos = mul(mul(float4(vWorldPos, 1.f), g_ViewMatrix), g_ProjMatrix);

	/* 5) atlas UV - M4 시점은 vAtlasUV=(0,0,1,1) 로 보내므로 결과적으로 vTex 그대로 */
	Out.vTex = In.vTex * In.vAtlasUV.zw + In.vAtlasUV.xy;
	Out.vColor = In.vColor;
	Out.vAgeLife = In.vAgeLifePad.xy;
	return Out;
}

float4 PS_MAIN(VS_OUT In) : SV_TARGET0
{
	float4 vTex = g_Texture.Sample(LinearSampler, In.vTex);
/* M6: instance vColor 가 이미 커브 평가된 색·알파를 담는다. PS는 텍스처×색만 처리. */
return vTex * In.vColor;
}

technique11 DefaultTechnique
{
	pass Alpha                  // pass 0
	{
		SetRasterizerState(RS_Cull_None);
		SetDepthStencilState(DSS_DepthReadNoWrite, 0);
		SetBlendState(BS_AlphaBlend, float4(0,0,0,0), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN();
	}
	pass Additive               // pass 1
	{
		SetRasterizerState(RS_Cull_None);
		SetDepthStencilState(DSS_DepthReadNoWrite, 0);
		SetBlendState(BS_Additive, float4(0,0,0,0), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN();
	}
};