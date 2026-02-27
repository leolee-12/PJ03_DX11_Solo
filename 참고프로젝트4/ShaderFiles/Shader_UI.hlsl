#include "Shader_Defines.hlsli"

cbuffer WVP : register(b1)
{
    matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
};
Texture2D g_Texture;

Texture2D g_DiffuseTexture;
Texture2D g_MaskTexture;
Texture2D g_NoiseTexture;
Texture2D g_DepthTexture;
float4 g_vColor;

float g_UI_Alpha = { 100.f };
bool g_UI_DiscardStandard = { true };
float g_UI_AlphaDiscardValue = { 0.2f };

bool g_UI_UVCutting = { false };
bool g_UI_UVHPCutting = { false };
bool g_UI_WidthCutting;

float g_UI_UVCutRatio = { 1.f };
float g_UI_UVCutRatio2 = { 1.f }; //For HP

struct VS_IN
{
    float3 vPosition : POSITION;
    float2 vTexcoord : TEXCOORD0;
};


struct VS_OUT
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
};



VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT Out = (VS_OUT) 0;

    matrix matWV, matWVP;

    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);

    Out.vPosition = mul(float4(In.vPosition, 1.f), matWVP);
    Out.vTexcoord = In.vTexcoord;

    return Out;
}

struct PS_IN
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
};

struct PS_OUT
{
    float4 vColor : SV_TARGET0;
};

PS_OUT PS_MAIN_UI(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    Out.vColor = g_Texture.Sample(LinearSampler, In.vTexcoord);

    if (g_UI_Alpha < 1.f)
    {
        Out.vColor.a = min(Out.vColor.a += g_UI_Alpha, 1.f);
    }

    if (true == g_UI_DiscardStandard)
    {
        if (Out.vColor.a < g_UI_AlphaDiscardValue)
            discard;
    }
	
    return Out;
}

PS_OUT PS_MAIN_UVHPCut(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    float4 vNoColor = { 0.35f, 0.35f, 0.35f, 1.f };
    float4 vWarningColor = { 0.61f, 0.12f, 0.f, 1.f };
    float4 vDamageColor = { 0.678f, 0.f, 0.f, 1.f };
	
    if (true == g_UI_WidthCutting) //가로 자르기
    {
        if ((In.vTexcoord.x < g_UI_UVCutRatio) && (In.vTexcoord.x < g_UI_UVCutRatio2))
        {
            if (g_UI_UVCutRatio < 0.2f)
                Out.vColor = vWarningColor;
            else
                Out.vColor = g_Texture.Sample(LinearSampler, In.vTexcoord);
        }
        else if ((In.vTexcoord.x < g_UI_UVCutRatio) && (In.vTexcoord.x > g_UI_UVCutRatio2))
        {
            Out.vColor = vDamageColor;
        }
        else
        { //discard;
            Out.vColor = vNoColor;
        }
    }
    else if (false == g_UI_WidthCutting) //세로 자르기
    {
        if ((In.vTexcoord.y < g_UI_UVCutRatio) && (In.vTexcoord.y < g_UI_UVCutRatio2))
        {
            if (g_UI_UVCutRatio < 0.2f)
                Out.vColor = vWarningColor;
            else
                Out.vColor = g_Texture.Sample(LinearSampler, In.vTexcoord);
        }
        else if ((In.vTexcoord.y < g_UI_UVCutRatio) && (In.vTexcoord.y > g_UI_UVCutRatio2))
        {
            Out.vColor = vDamageColor;
        }
        else
        { //discard;
            Out.vColor = vNoColor;
        }
    }
    
    if (g_UI_Alpha < 1.f)
    {
        Out.vColor.a = min(Out.vColor.a += g_UI_Alpha, 1.f);
    }

    if (true == g_UI_DiscardStandard)
    {
        if (Out.vColor.a < g_UI_AlphaDiscardValue)
            discard;
    }
	
    return Out;
}

PS_OUT PS_MAIN_StdUVCut(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    if (true == g_UI_WidthCutting) //가로 자르기
    {
        if (In.vTexcoord.x < g_UI_UVCutRatio)
        {
            Out.vColor = g_Texture.Sample(LinearSampler, In.vTexcoord);
        }
        else
        {	//discard;
            Out.vColor.a = 0.f;
        }
    }
    else if (false == g_UI_WidthCutting) //세로 자르기
    {
        if (In.vTexcoord.y > (1.f - g_UI_UVCutRatio))
        {
            Out.vColor = g_Texture.Sample(LinearSampler, In.vTexcoord);
        }
        else
        { //discard;
            Out.vColor.a = 0.f;
        }
    }

    if (g_UI_Alpha < 1.f)
    {
        Out.vColor.a = min(Out.vColor.a += g_UI_Alpha, 1.f);
    }

    if (true == g_UI_DiscardStandard)
    {
        if (Out.vColor.a < g_UI_AlphaDiscardValue)
            discard;
    }   
        
    return
Out;
}

technique11 DefaultTechnique
{
    pass UI_DSS_None //0
    {
        SetRasterizerState(RS_Cull_None);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_AlphaBlend_Add, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_UI();
    }

    pass UI_DSS_Default //1
    {
        SetRasterizerState(RS_Cull_None);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend_Add, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_UI();
    }

    pass UI_UVHPCut //2
    {
        SetRasterizerState(RS_Cull_None);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_AlphaBlend_Add, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_UVHPCut();
    }
    pass UI_StdUVCut //3
    {
        SetRasterizerState(RS_Cull_None);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_AlphaBlend_Add, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_StdUVCut();
    }
}
