#include "Shader_Defines.hlsli"

cbuffer WVP : register(b0)
{
    matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
};

Texture2D g_VertexTexture;
Texture2D g_DiffuseTexture;
Texture2D g_NoiseTexture;
Texture2D g_NormalTexture;
Texture2D g_DissolveTexture;


float g_fAccTime;
vector g_vColor;
float g_fUVStart;
bool g_bMaskInverse;
bool g_bMoveUV_Y;

float g_fDissolveAmount = 0.f;

matrix g_BoneMatrices[800];

struct VS_IN
{
    float3 vPosition : POSITION;
    float3 vNormal : NORMAL;
    float2 vTexcoord : TEXCOORD0;
    float3 vTangent : TANGENT;

    uint4 vBlendIndices : BLENDINDEX;
    float4 vBlendWeights : BLENDWEIGHT;
    float4 vInstanceIndex : TEXCOORD1;
};


struct VS_OUT
{
    float4 vPosition : SV_POSITION;
    float4 vNormal : NORMAL;
    float2 vTexcoord : TEXCOORD0;
    float4 vWorldPos : TEXCOORD1;
    float4 vProjPos : TEXCOORD2;
    float4 vTangent : TANGENT;
    float4 vBinormal : BINORMAL;
};



VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT Out = (VS_OUT) 0;


    float fWeightW = 1.f - (In.vBlendWeights.x + In.vBlendWeights.y + In.vBlendWeights.z);

    matrix BoneMatrix = g_BoneMatrices[In.vBlendIndices.x] * In.vBlendWeights.x +
		g_BoneMatrices[In.vBlendIndices.y] * In.vBlendWeights.y +
		g_BoneMatrices[In.vBlendIndices.z] * In.vBlendWeights.z +
		g_BoneMatrices[In.vBlendIndices.w] * fWeightW;

    vector vPosition = mul(vector(In.vPosition, 1.f), BoneMatrix);
    vector vNormal = mul(vector(In.vNormal, 0.f), BoneMatrix);
	

    matrix matWV, matWVP;

    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);

    Out.vPosition = mul(vPosition, matWVP);
    Out.vNormal = normalize(mul(vNormal, g_WorldMatrix));
    Out.vTexcoord = In.vTexcoord;
    Out.vWorldPos = mul(float4(In.vPosition, 1.f), g_WorldMatrix);
    Out.vProjPos = Out.vPosition;
    Out.vTangent = normalize(mul(float4(In.vTangent, 0.f), g_WorldMatrix)); //X축 (World적용) 
    Out.vBinormal = normalize(vector(cross(Out.vNormal.xyz, Out.vTangent.xyz), 0.f)); //Y축 (World상의 x,z축으로 y축 구함)

    return Out;
}

struct PS_IN
{
    float4 vPosition : SV_POSITION;
    float4 vNormal : NORMAL;
    float2 vTexcoord : TEXCOORD0;
    float4 vWorldPos : TEXCOORD1;
    float4 vProjPos : TEXCOORD2;
    float4 vTangent : TANGENT;
    float4 vBinormal : BINORMAL;

};

struct PS_OUT
{
    float4 vDiffuse : SV_TARGET0;
    float4 vNormal : SV_TARGET1;
    float4 vDepth : SV_TARGET2;
    float4 vORM : SV_TARGET3;
};

struct PS_OUT_PLAYER
{
    float4 vDiffuse : SV_TARGET0;
    float4 vNormal : SV_TARGET1;
    float4 vDepth : SV_TARGET2;
    float4 vORM : SV_TARGET3;
    float4 vDepth_Player : SV_TARGET4;
};

struct PS_OUT_EFFECT
{
    float4 vDiffuse : SV_TARGET0;
    float4 vNormal : SV_TARGET1;
    float4 vDepth : SV_TARGET2;
    float4 vORM : SV_TARGET3;
    float4 vDepth_Player : SV_TARGET4;
};
/* 픽셀셰이더 : 픽셀의 색!!!! 을 결정한다. */

PS_OUT PS_MAIN(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    vector vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);
    vector vNormalDesc = g_NormalTexture.Sample(LinearSampler, In.vTexcoord);
	
    float3 vNormal = vNormalDesc.xyz * 2.f - 1.f; //0,0 ~ 1,1 -> -1,-1 ~ 1,1
    float3x3 WorldMatrix = float3x3(In.vTangent.xyz, In.vBinormal.xyz, In.vNormal.xyz); //각 월드 적용한 로컬스페이스로 월드Matrix 만듦
    vNormal = mul(vNormal, WorldMatrix); //Normal텍스처에서 구한 Normal에 위에서 구한 WorldMatrix 적용

    if (vMtrlDiffuse.a < 0.3f)
        discard;

	//픽셀유형중에 UNORM 붙은것 → 0~1 사이의 정규화된 수여야 한다.
	/* -1 ~ 1 -> 0 ~ 1 */
    Out.vNormal = vector(vNormal.xyz * 0.5f + 0.5f, 0.f);
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 1000.0f, 0.0f, 0.0f);
    Out.vDiffuse = vMtrlDiffuse;

	//float DissolveDesc = g_DissolveTexture.Sample(LinearSampler, In.vTexcoord).r;
	//
	//if (g_fDissolveAmount > 0.1f)
	//	Out.vDiffuse.r += 0.8f;
	//
	//clip(Out.vDiffuse.a - 0.1f);
	//clip(DissolveDesc - g_fDissolveAmount);
	//Out.vDiffuse.rgb += float3(1.f, 0.1f, 0.1f) * step(DissolveDesc - g_fDissolveAmount, 0.02f);
	//Out.vORM = 0;

    return Out;
}



PS_OUT_PLAYER PS_MAIN_CHARACTER(PS_IN In)
{
    PS_OUT_PLAYER Out = (PS_OUT_PLAYER) 0;

    vector vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);
    vector vNormalDesc = g_NormalTexture.Sample(LinearSampler, In.vTexcoord);

    float3 vNormal = vNormalDesc.xyz * 2.f - 1.f; //0,0 ~ 1,1 -> -1,-1 ~ 1,1
    float3x3 WorldMatrix = float3x3(In.vTangent.xyz, In.vBinormal.xyz, In.vNormal.xyz); //각 월드 적용한 로컬스페이스로 월드Matrix 만듦
    vNormal = mul(vNormal, WorldMatrix); //Normal텍스처에서 구한 Normal에 위에서 구한 WorldMatrix 적용

    if (vMtrlDiffuse.a < 0.3f)
        discard;

    Out.vDiffuse = vMtrlDiffuse;
	//픽셀유형중에 UNORM 붙은것 → 0~1 사이의 정규화된 수여야 한다.
	/* -1 ~ 1 -> 0 ~ 1 */
    Out.vNormal = vector(vNormal.xyz * 0.5f + 0.5f, 0.f);
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 1000.0f, 0.0f, 0.0f);
    Out.vDepth_Player = Out.vDepth;

    Out.vDiffuse.rgb = AdjustHSV(Out.vDiffuse.rgb, 1.f, 1.3f, 1.15f);
	
    float DissolveDesc = g_DissolveTexture.Sample(LinearSampler, In.vTexcoord).r;
	
    if (g_fDissolveAmount > 0.1f)
        Out.vDiffuse.r += 0.8f;

    clip(Out.vDiffuse.a - 0.1f);
    clip(DissolveDesc - g_fDissolveAmount);
    Out.vDiffuse.rgb += float3(1.f, 0.1f, 0.1f) * step(DissolveDesc - g_fDissolveAmount, 0.02f);
    Out.vORM = 0;

    return Out;
}


PS_OUT_PLAYER PS_MAIN_MASK_CLAMP(PS_IN In)
{
    PS_OUT_PLAYER Out = (PS_OUT_PLAYER) 0;

	//In.vTexcoord.x = clamp(In.vTexcoord.x,(g_fUVStart - 0.1f) - g_fAccTime, g_fUVStart - g_fAccTime);
    if (!g_bMoveUV_Y)
        In.vTexcoord.x = (g_fUVStart - 0.4f) - g_fAccTime + In.vTexcoord.x * 0.4f;
    else
        In.vTexcoord.y = (g_fUVStart - 0.4f) - g_fAccTime + In.vTexcoord.y * 0.4f;

    vector vNoiseDesc = g_NoiseTexture.Sample(ClampSampler, In.vTexcoord).r;
    vector vMtrlDiffuse = g_DiffuseTexture.Sample(ClampSampler, In.vTexcoord);
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
    Out.vORM = 0;

    return Out;
}

PS_OUT_PLAYER PS_MAIN_MASK_NONCLAMP(PS_IN In)
{
    PS_OUT_PLAYER Out = (PS_OUT_PLAYER) 0;

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
    Out.vORM = 0;

    return Out;
}


struct PS_OUT_SHADOW
{
    vector vLightDepth : SV_TARGET0;
};

PS_OUT_SHADOW PS_MAIN_SHADOW(PS_IN In)
{
    PS_OUT_SHADOW Out = (PS_OUT_SHADOW) 0;

    Out.vLightDepth = In.vProjPos.w / 800.0f; //임시 far값 300.f

    return Out;
}

technique11 DefaultTechnique
{
    pass Model
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

    pass Model_Player
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

    pass Model_Mask_Clamp
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

    pass Model_Mask_NonClamp
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

    pass Shadow
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_SHADOW();
    }
}
