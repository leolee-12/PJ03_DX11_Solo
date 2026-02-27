#include "Shader_Defines.hlsli"

cbuffer WVP : register(b1)
{
    matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
};

Texture2D g_DiffuseTexture;
Texture2D g_NormalTexture;
Texture2D g_MRVTexture;
Texture2D g_AOTexture;
Texture2D g_EmissiveTexture;
Texture2D g_AlphaTexture;

Texture2D g_DepthTexture_Character;
float4 g_LightConeColor;

bool g_bDark = false;

float g_fFar = 1000.f;


struct VS_IN
{
    float3 vPosition : POSITION;
    float3 vNormal : NORMAL;
    float2 vTexcoord : TEXCOORD0;
    float3 vTangent : TANGENT;
    float3 vBinormal : BINORMAL;
    row_major float4x4 TransformMatrix : WORLD;
};

struct VS_OUT
{
    float4 vPosition : SV_POSITION;
    float4 vNormal : NORMAL;
    float2 vTexcoord : TEXCOORD0;
    float3 vRight : TEXCOORD1;
    float3 vUp : TEXCOORD2;
    float4 vProjPos : TEXCOORD3;
    float4 vTangent : TANGENT;
    float4 vBinormal : BINORMAL;
};


VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT Out = (VS_OUT) 0;

    vector vPosition = mul(float4(In.vPosition, 1.f), In.TransformMatrix);

    float3 vRight = float3(In.TransformMatrix._11, In.TransformMatrix._12, In.TransformMatrix._13);
    float3 vUp = float3(In.TransformMatrix._21, In.TransformMatrix._22, In.TransformMatrix._23);

    matrix matWV, matWVP;

    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);

    Out.vPosition = mul(vPosition, matWVP);
    Out.vNormal = normalize(mul(float4(In.vNormal, 0.f), g_WorldMatrix));
    Out.vTexcoord = In.vTexcoord;
    Out.vRight = normalize(mul(vRight, (float3x3) g_WorldMatrix));
    Out.vUp = normalize(mul(vUp, (float3x3) g_WorldMatrix));
    Out.vProjPos = Out.vPosition;
    Out.vTangent = normalize(mul(float4(In.vTangent, 0.f), g_WorldMatrix));
    Out.vBinormal = normalize(vector(cross(Out.vNormal.xyz, Out.vTangent.xyz), 0.f));

    return Out;
}

struct PS_IN
{
    float4 vPosition : SV_POSITION;
    float4 vNormal : NORMAL;
    float2 vTexcoord : TEXCOORD0;
    float3 vRight : TEXCOORD1;
    float3 vUp : TEXCOORD2;
    float4 vProjPos : TEXCOORD3;
    float4 vTangent : TANGENT;
    float4 vBinormal : BINORMAL;
};

struct PS_OUT
{
    float4 vDiffuse : SV_TARGET0;
    float4 vNormal : SV_TARGET1;
    float4 vDepth : SV_TARGET2;
    float4 vPBR : SV_TARGET3;
    float4 vEmissive : SV_TARGET4;
    float4 vShaderTag : SV_TARGET5;
};


PS_OUT PS_MAIN(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    vector vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);
    vector vNormalDesc = g_NormalTexture.Sample(LinearSampler, In.vTexcoord);
    float4 vEmissiveDesc = g_EmissiveTexture.Sample(LinearSampler, In.vTexcoord);
    float4 vAlphaDesc = g_AlphaTexture.Sample(LinearSampler, In.vTexcoord);
    float3 vNormal = vNormalDesc.xyz * 2.f - 1.f; //0,0 ~ 1,1 -> -1,-1 ~ 1,1
    float3x3 WorldMatrix = float3x3(In.vTangent.xyz, In.vBinormal.xyz, In.vNormal.xyz); //각 월드 적용한 로컬스페이스로 월드Matrix 만듦

    vNormal = mul(vNormal, WorldMatrix); //Normal텍스처에서 구한 Normal에 위에서 구한 WorldMatrix 적용


	//vMtrlDiffuse *= g_AOTexture.Sample(LinearSampler, In.vTexcoord).x;

    Out.vDiffuse = vMtrlDiffuse + vEmissiveDesc;
    Out.vDiffuse.a *= vAlphaDesc.r;

    if (Out.vDiffuse.a < 0.3f)
        discard;

	//Out.vDiffuse = vector(1.f, 1.f, 1.f, 1.f);

    Out.vNormal = vector(vNormal.xyz * 0.5f + 0.5f, 0.f);
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fFar, 0.0f, 0.0f);
    Out.vPBR = g_MRVTexture.Sample(LinearSampler, In.vTexcoord);
    Out.vPBR.z = g_AOTexture.Sample(LinearSampler, In.vTexcoord).x;
    Out.vEmissive = vEmissiveDesc;

    Out.vShaderTag = float4(0.f, 1.0f, 0.f, 0.f);
    return Out;
}

technique11 DefaultTechnique
{
    pass Model_Instance
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
}

