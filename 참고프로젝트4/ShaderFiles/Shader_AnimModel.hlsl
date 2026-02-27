#include "Shader_Defines.hlsli"

cbuffer WVP : register(b1)
{
    matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
    matrix g_ShadowVIewProjMatrices[4];
};

Texture2D g_NoiseTexture;
Texture2D g_NormalTexture;
Texture2D g_DissolveTexture;

Texture2D g_DiffuseTexture;
Texture2D g_AOTexture;
Texture2D g_EmissiveTexture;
Texture2D g_AlphaTexture;
Texture2D g_MRVTexture;
Texture2D g_MTexture;

float			g_fBlendTexture_Value = 0.01f;
float			g_fAccTime;
vector			g_vColor;
float			g_fUVStart;
bool			g_bMaskInverse;
bool			g_bMoveUV_Y;

float			g_fDissolveAmount = 0.f;
float			g_fDissolveGradiationDistance;
float3			g_vDissolveGradiationStartColor;
float3			g_vDissolveGradiationEndColor;

matrix			g_BoneMatrices[800];

struct VS_IN
{
	float3		vPosition : POSITION;
	float3		vNormal : NORMAL;
	float2		vTexcoord : TEXCOORD0;
	float3		vTangent : TANGENT;

	uint4		vBlendIndices : BLENDINDEX;
	float4		vBlendWeights : BLENDWEIGHT;
};


struct VS_OUT
{
	float4		vPosition : SV_POSITION;
	float4		vNormal : NORMAL;
	float2		vTexcoord : TEXCOORD0;
	float4		vWorldPos : TEXCOORD1;
	float4		vProjPos : TEXCOORD2;
	float4		vTangent : TANGENT;
	float4		vBinormal : BINORMAL;
};

struct VS_OUT_SHADOW
{
    float4 vWorldPos : POSITION;
};


VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT		Out = (VS_OUT)0;


	float		fWeightW = 1.f - (In.vBlendWeights.x + In.vBlendWeights.y + In.vBlendWeights.z);

	matrix		BoneMatrix = g_BoneMatrices[In.vBlendIndices.x] * In.vBlendWeights.x +
		g_BoneMatrices[In.vBlendIndices.y] * In.vBlendWeights.y +
		g_BoneMatrices[In.vBlendIndices.z] * In.vBlendWeights.z +
		g_BoneMatrices[In.vBlendIndices.w] * fWeightW;

	vector		vPosition = mul(vector(In.vPosition, 1.f), BoneMatrix);
	vector		vNormal = mul(vector(In.vNormal, 0.f), BoneMatrix);
	

	matrix		matWV, matWVP;

	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);

	Out.vPosition = mul(vPosition, matWVP);
	Out.vNormal = normalize(mul(vNormal, g_WorldMatrix));
	Out.vTexcoord = In.vTexcoord;
	Out.vWorldPos = mul(vPosition, g_WorldMatrix);
	Out.vProjPos = Out.vPosition;
	Out.vTangent = normalize(mul(float4(In.vTangent, 0.f), g_WorldMatrix)); //X축 (World적용) 
	Out.vBinormal = normalize(vector(cross(Out.vNormal.xyz, Out.vTangent.xyz), 0.f)); //Y축 (World상의 x,z축으로 y축 구함)

	return Out;
}

VS_OUT_SHADOW VS_MAIN_SHADOW(VS_IN In)
{
    VS_OUT_SHADOW Out = (VS_OUT_SHADOW) 0;
	
    float fWeightW = 1.f - (In.vBlendWeights.x + In.vBlendWeights.y + In.vBlendWeights.z);

    matrix BoneMatrix = g_BoneMatrices[In.vBlendIndices.x] * In.vBlendWeights.x +
		g_BoneMatrices[In.vBlendIndices.y] * In.vBlendWeights.y +
		g_BoneMatrices[In.vBlendIndices.z] * In.vBlendWeights.z +
		g_BoneMatrices[In.vBlendIndices.w] * fWeightW;

    vector vPosition = mul(vector(In.vPosition, 1.f), BoneMatrix);
    Out.vWorldPos = mul(vPosition, g_WorldMatrix);

    return Out;
}

struct GS_INPUT_SHADOW
{
	float4 vWorldPos : POSITION;
};

struct GS_OUT_SHADOW_TEX_ARRAY
{
    float4 Position : SV_POSITION;
    uint RTIndex : SV_RenderTargetArrayIndex;
};

[maxvertexcount(12)]
void GS_MAIN_SHADOW(triangle GS_INPUT_SHADOW input[3], inout TriangleStream<GS_OUT_SHADOW_TEX_ARRAY> TriStream)
{    
    for (uint i = 0; i < 4; ++i )
    {
        GS_OUT_SHADOW_TEX_ARRAY output[3];
        for (uint r = 0; r < 3; ++r)
        {
            output[r].Position = mul(input[r].vWorldPos,g_ShadowVIewProjMatrices[i]);
            output[r].RTIndex = i;
            TriStream.Append(output[r]);			
        }
        TriStream.RestartStrip();
    }
}

struct PS_IN
{
	float4		vPosition : SV_POSITION;
	float4		vNormal : NORMAL;
	float2		vTexcoord : TEXCOORD0;
	float4		vWorldPos : TEXCOORD1;
	float4		vProjPos : TEXCOORD2;
	float4		vTangent : TANGENT;
	float4		vBinormal : BINORMAL;

};

struct PS_OUT
{
    float4 vDiffuse : SV_TARGET0;
    float4 vNormal : SV_TARGET1;
    float4 vDepth : SV_TARGET2;
    float4 vPBR : SV_TARGET3;
    float4 vEmissive : SV_TARGET4;
    float4 vShaderTag : SV_TARGET5;
    float4 vModelEffect : SV_TARGET6;
};

struct PS_OUT_PLAYER
{
    float4 vDiffuse : SV_TARGET0;
    float4 vNormal : SV_TARGET1;
    float4 vDepth : SV_TARGET2;
    float4 vPBR : SV_TARGET3;
    float4 vEmissive : SV_TARGET4;
    float4 vShaderTag : SV_TARGET5;
};

struct PS_OUT_EFFECT
{
    float4 vDiffuse : SV_TARGET0;
    float4 vNormal : SV_TARGET1;
    float4 vDepth : SV_TARGET2;
    float4 vPBR : SV_TARGET3;
    float4 vEmissive : SV_TARGET4;
    float4 vShaderTag : SV_TARGET5;
};

struct PS_IN_SHADOW
{	
    float4 Position : SV_POSITION;
};


/* 픽셀셰이더 : 픽셀의 색!!!! 을 결정한다. */

PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	vector vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);
	vector vNormalDesc = g_NormalTexture.Sample(LinearSampler, In.vTexcoord);
    float4 vEmissiveDesc = g_EmissiveTexture.Sample(LinearSampler, In.vTexcoord);
    float4 vAlphaDesc = g_AlphaTexture.Sample(LinearSampler, In.vTexcoord);
    float4 vMRVDesc = g_MRVTexture.Sample(LinearSampler, In.vTexcoord);
    float4 vMDesc = g_MTexture.Sample(LinearSampler, In.vTexcoord);
	float3 vNormal = vNormalDesc.xyz * 2.f - 1.f; //0,0 ~ 1,1 -> -1,-1 ~ 1,1
	float3x3 WorldMatrix = float3x3(In.vTangent.xyz, In.vBinormal.xyz, In.vNormal.xyz); //각 월드 적용한 로컬스페이스로 월드Matrix 만듦
	vNormal = mul(vNormal, WorldMatrix); //Normal텍스처에서 구한 Normal에 위에서 구한 WorldMatrix 적용
	   
    Out.vDiffuse = vMtrlDiffuse + vEmissiveDesc;
    Out.vDiffuse.a *= vAlphaDesc.r;
    
    if (Out.vDiffuse.a < 0.3f)
        discard;
	
	//픽셀유형중에 UNORM 붙은것 → 0~1 사이의 정규화된 수여야 한다.
	/* -1 ~ 1 -> 0 ~ 1 */
	Out.vNormal = vector(vNormal.xyz * 0.5f + 0.5f, 0.f);
	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 1000.0f, 0.0f, 0.0f);
    Out.vPBR = vMRVDesc;
    Out.vPBR.z = g_AOTexture.Sample(LinearSampler, In.vTexcoord).x;
    Out.vEmissive = vEmissiveDesc;
   
    Out.vShaderTag = float4(1.f, 0.0f, 0.f, 0.f);
	return Out;
}



PS_OUT_PLAYER PS_MAIN_CHARACTER(PS_IN In)
{
	PS_OUT_PLAYER		Out = (PS_OUT_PLAYER)0;

    vector vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);
    vector vNormalDesc = g_NormalTexture.Sample(LinearSampler, In.vTexcoord);
    float4 vEmissiveDesc = g_EmissiveTexture.Sample(LinearSampler, In.vTexcoord);
    float4 vAlphaDesc = g_AlphaTexture.Sample(LinearSampler, In.vTexcoord);
    float4 vMRVDesc = g_MRVTexture.Sample(LinearSampler, In.vTexcoord);
    float4 vMDesc = g_MTexture.Sample(LinearSampler, In.vTexcoord);
    float3 vNormal = vNormalDesc.xyz * 2.f - 1.f; //0,0 ~ 1,1 -> -1,-1 ~ 1,1
    float3x3 WorldMatrix = float3x3(In.vTangent.xyz, In.vBinormal.xyz, In.vNormal.xyz); //각 월드 적용한 로컬스페이스로 월드Matrix 만듦
    vNormal = mul(vNormal, WorldMatrix); //Normal텍스처에서 구한 Normal에 위에서 구한 WorldMatrix 적용
	   
    Out.vDiffuse = vMtrlDiffuse + vEmissiveDesc;
    Out.vDiffuse.a *= vAlphaDesc.r;
    
    if (Out.vDiffuse.a < 0.3f)
        discard;
	
	//픽셀유형중에 UNORM 붙은것 → 0~1 사이의 정규화된 수여야 한다.
	/* -1 ~ 1 -> 0 ~ 1 */
    Out.vNormal = vector(vNormal.xyz * 0.5f + 0.5f, 0.f);
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 1000.0f, 0.0f, 0.0f);
    Out.vPBR = vMRVDesc;
    Out.vPBR.z = g_AOTexture.Sample(LinearSampler, In.vTexcoord).x;
    Out.vEmissive = vEmissiveDesc;
   
    Out.vShaderTag = float4(1.f, 0.0f, 0.f, 0.f);
	
	return Out;
}


PS_OUT_PLAYER PS_MAIN_MASK_CLAMP(PS_IN In)
{
	PS_OUT_PLAYER		Out = (PS_OUT_PLAYER)0;

	//In.vTexcoord.x = clamp(In.vTexcoord.x,(g_fUVStart - 0.1f) - g_fAccTime, g_fUVStart - g_fAccTime);
	if(!g_bMoveUV_Y)
		In.vTexcoord.x = (g_fUVStart - 0.4f) - g_fAccTime + In.vTexcoord.x * 0.4f;
	else
		In.vTexcoord.y = (g_fUVStart - 0.4f) - g_fAccTime + In.vTexcoord.y * 0.4f;

	vector vNoiseDesc = g_NoiseTexture.Sample(ClampSampler, In.vTexcoord).r;
	vector vMtrlDiffuse = g_DiffuseTexture.Sample(ClampSampler, In.vTexcoord );
	vector vNormalDesc = g_NormalTexture.Sample(ClampSampler, In.vTexcoord);


	float3 vNormal = vNormalDesc.xyz * 2.f - 1.f; //0,0 ~ 1,1 -> -1,-1 ~ 1,1
	float3x3 WorldMatrix = float3x3(In.vTangent.xyz, In.vBinormal.xyz, In.vNormal.xyz); //각 월드 적용한 로컬스페이스로 월드Matrix 만듦
	vNormal = mul(vNormal, WorldMatrix); //Normal텍스처에서 구한 Normal에 위에서 구한 WorldMatrix 적용

	float fMtrlAlpha = 0.f;
	if (g_bMaskInverse)
		fMtrlAlpha = vMtrlDiffuse.r;
	else
		fMtrlAlpha = 1.f - vMtrlDiffuse.r;

	Out.vDiffuse.a = fMtrlAlpha;
	Out.vDiffuse.a = 1.f - Out.vDiffuse.a;
	Out.vDiffuse.a *= g_vColor.a;
	Out.vDiffuse.rgb = g_vColor.rgb + (vMtrlDiffuse.rgb);
	
	if (Out.vDiffuse.a <= 0.f)
		discard;

	//Out.vDiffuse = vMtrlDiffuse;
	//픽셀유형중에 UNORM 붙은것 → 0~1 사이의 정규화된 수여야 한다.
	/* -1 ~ 1 -> 0 ~ 1 */
	Out.vNormal = vector(vNormal.xyz * 0.5f + 0.5f, 0.f);
	//Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 1000.0f, 0.0f, 0.0f);
	//Out.vDepth_Player = Out.vDepth;

	return Out;
}

PS_OUT_PLAYER PS_MAIN_MASK_NONCLAMP(PS_IN In)
{
	PS_OUT_PLAYER		Out = (PS_OUT_PLAYER)0;

	//In.vTexcoord.x = clamp(In.vTexcoord.x,(g_fUVStart - 0.1f) - g_fAccTime, g_fUVStart - g_fAccTime);
	vector vMtrlDiffuse = g_DiffuseTexture.Sample(ClampSampler, In.vTexcoord);
	vector vNormalDesc = g_NormalTexture.Sample(ClampSampler, In.vTexcoord);
	//vector vMtrlNoise = g_NoiseTexture.Sample(PointSampler, In.vTexcoord);

	float3 vNormal = vNormalDesc.xyz * 2.f - 1.f; //0,0 ~ 1,1 -> -1,-1 ~ 1,1
	float3x3 WorldMatrix = float3x3(In.vTangent.xyz, In.vBinormal.xyz, In.vNormal.xyz); //각 월드 적용한 로컬스페이스로 월드Matrix 만듦
	vNormal = mul(vNormal, WorldMatrix); //Normal텍스처에서 구한 Normal에 위에서 구한 WorldMatrix 적용

	float fMtrlAlpha = 0.f;
	if (g_bMaskInverse)
		fMtrlAlpha = vMtrlDiffuse.r;
	else
		fMtrlAlpha = 1.f - vMtrlDiffuse.r;

	Out.vDiffuse.a = fMtrlAlpha * g_vColor.a;
	Out.vDiffuse.rgb = g_vColor.rgb + vMtrlDiffuse.rgb;

	if (Out.vDiffuse.a <= 0.f)
		discard;

	//Out.vDiffuse = vMtrlDiffuse;
	//픽셀유형중에 UNORM 붙은것 → 0~1 사이의 정규화된 수여야 한다.
	/* -1 ~ 1 -> 0 ~ 1 */
	Out.vNormal = vector(vNormal.xyz * 0.5f + 0.5f, 0.f);
	//Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 1000.0f, 0.0f, 0.0f);
	//Out.vDepth_Player = Out.vDepth;

	return Out;
}

PS_OUT PS_MAIN_DISSOLVE(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    vector vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);
    vector vNormalDesc = g_NormalTexture.Sample(LinearSampler, In.vTexcoord);
    float4 vEmissiveDesc = g_EmissiveTexture.Sample(LinearSampler, In.vTexcoord);
    float4 vAlphaDesc = g_AlphaTexture.Sample(LinearSampler, In.vTexcoord);
    float4 vMRVDesc = g_MRVTexture.Sample(LinearSampler, In.vTexcoord);
    float4 vMDesc = g_MTexture.Sample(LinearSampler, In.vTexcoord);
    float3 vNormal = vNormalDesc.xyz * 2.f - 1.f; //0,0 ~ 1,1 -> -1,-1 ~ 1,1
    float3x3 WorldMatrix = float3x3(In.vTangent.xyz, In.vBinormal.xyz, In.vNormal.xyz); //각 월드 적용한 로컬스페이스로 월드Matrix 만듦
    vNormal = mul(vNormal, WorldMatrix); //Normal텍스처에서 구한 Normal에 위에서 구한 WorldMatrix 적용
  
    vector vMtrlDissolve = g_DissolveTexture.Sample(LinearSampler, In.vTexcoord);

    float fDissolveDesc = vMtrlDissolve.r;

    clip(fDissolveDesc - g_fDissolveAmount);
	
    if (g_fDissolveAmount + g_fDissolveGradiationDistance >= fDissolveDesc)
    {
        float fLerpRatio = (fDissolveDesc - g_fDissolveAmount) / g_fDissolveGradiationDistance;
        Out.vDiffuse = vector(lerp(g_vDissolveGradiationStartColor, g_vDissolveGradiationEndColor, fLerpRatio), 1.f)
						+ vEmissiveDesc;
    }
    else
    {
        Out.vDiffuse = vMtrlDiffuse + vEmissiveDesc;
    }
	
    Out.vDiffuse.a *= vAlphaDesc.r;
    
    if (Out.vDiffuse.a < 0.3f)
        discard;
	
	//픽셀유형중에 UNORM 붙은것 → 0~1 사이의 정규화된 수여야 한다.
	/* -1 ~ 1 -> 0 ~ 1 */
    Out.vModelEffect = Out.vDiffuse;
    Out.vDiffuse = 0.f;

    Out.vNormal = vector(vNormal.xyz * 0.5f + 0.5f, 0.f);
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 1000.0f, 0.0f, 0.0f);
    Out.vPBR = vMRVDesc;
    Out.vPBR.z = g_AOTexture.Sample(LinearSampler, In.vTexcoord).x;
    Out.vEmissive = vEmissiveDesc;
    Out.vShaderTag = float4(1.f, 0.0f, 0.f, 0.f);
    return Out;
}

struct PS_OUT_SHADOW
{
	vector		vLightDepth : SV_TARGET0;
};

//PS_OUT_SHADOW PS_MAIN_SHADOW(PS_IN In)
//{
//	PS_OUT_SHADOW		Out = (PS_OUT_SHADOW)0;

//	Out.vLightDepth = In.vProjPos.w / 800.0f; //임시 far값 300.f

//	return Out;
//}

PS_OUT_SHADOW PS_MAIN_SHADOW(PS_IN_SHADOW In)
{
    PS_OUT_SHADOW Out = (PS_OUT_SHADOW) 0;
    Out.vLightDepth = float4(1.f, 0.f, 0.f, 1.f);
    return Out;

}

technique11 DefaultTechnique
{
	pass Model //0
	{
		SetRasterizerState(RS_Cull_None);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_Default, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN();
	}

	pass Model_Player //1
	{
		SetRasterizerState(RS_Cull_None);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_Default, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_CHARACTER();
	}

	pass Model_Mask_Clamp//2
	{
		SetRasterizerState(RS_Cull_None);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_AlphaBlend_Add, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_MASK_CLAMP();
	}

	pass Model_Mask_NonClamp//3
	{
		SetRasterizerState(RS_Cull_None);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_AlphaBlend_Add, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_MASK_NONCLAMP();
	}

	pass Shadow//4
	{
        SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Default, 4);
		SetBlendState(BS_Default, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xffffffff);

		//VertexShader = compile vs_5_0 VS_MAIN();
		//GeometryShader = NULL;
		//HullShader = NULL;
		//DomainShader = NULL;
		//PixelShader = compile ps_5_0 PS_MAIN_SHADOW();
        VertexShader = compile vs_5_0 VS_MAIN_SHADOW();
        GeometryShader = compile gs_5_0 GS_MAIN_SHADOW();
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_SHADOW();
	}

    pass Dissolve //5
    {
        SetRasterizerState(RS_Cull_None);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_DISSOLVE();
    }

}
