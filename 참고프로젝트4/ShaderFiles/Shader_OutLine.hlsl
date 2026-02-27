#include "ShadeR_Defines.hlsli"

cbuffer WVP : register(b1)
{
    matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
}

Texture2D g_DepthTexture;
Texture2D g_NormalTexture;
Texture2D g_OutLineTexture;
Texture2D g_OriginalRenderTexture;

Texture2D g_ShaderFlagTexture;
Texture2D g_IntensityFlagTexture;

float g_Divider = 1.f;

float g_ViewPortWidth = 1280.f;
float g_ViewPortHeight = 720.f;


// Laplacian Filter
float mask[9] =
{
    -1.f, -1.f, -1.f,
    -1.f,  8.f, -1.f,
    -1.f, -1.f, -1.f
};

// 근처 픽셀
float coord[3] =
{ -1.f, 0.f, 1.f };

float Blur_mask[9] =
{
    1.f, 1.f, 1.f,
    1.f, 1.f, 1.f,
    1.f, 1.f, 1.f
};



struct VS_IN
{
    float3		vPosition : POSITION;
    float2		vTexUV : TEXCOORD0;
};

struct VS_OUT
{
    float4		vPosition : SV_POSITION;
    float2		vTexUV : TEXCOORD0;
};

VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT		Out = (VS_OUT)0;

    matrix			matWV, matWVP;

    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);

    Out.vPosition = mul(vector(In.vPosition, 1.f), matWVP);
    Out.vTexUV = In.vTexUV;

    return Out;
}

// w나누기연산을 수행하낟. (In 투영스페이스)
// 뷰포트 변환. (In 뷰포트(윈도우좌표))

// 래스터라이즈(픽셀정볼르 생성한다. )


struct PS_IN
{
    float4		vPosition : SV_POSITION;
    float2		vTexUV : TEXCOORD0;
};

struct PS_OUT
{
    vector		vColor : SV_TARGET0;
};

PS_OUT PS_MAIN(PS_IN In)
{
    PS_OUT		Out = (PS_OUT)0;

    //vector vShaderFlag = g_ShaderFlagTexture.Sample(LinearSampler, In.vTexUV);

   // if (0.1f > vShaderFlag.r)
       // discard;

    float4 Color = 0;
    float4 Ret;

    for (uint i = 0; i < 9; i++)
    {
        Color += mask[i] *
            g_DepthTexture.Sample(LinearSampler,
                In.vTexUV + float2(coord[i % (uint) 3] / (g_ViewPortWidth), coord[i / (uint) 3] / (g_ViewPortHeight)));

        Color += mask[i] *
            g_NormalTexture.Sample(LinearSampler,
                In.vTexUV + float2(coord[i % (uint) 3] / (g_ViewPortWidth), coord[i / (uint) 3] / (g_ViewPortHeight))) * 0.1f;

    }
    //?

    float gray = 1.f - (Color.r * 0.3f + Color.g * 0.59f + Color.b * 0.11f);
    Ret = float4(gray, gray, gray, 1.f) / 1.f;

    Out.vColor = vector(1.f, 1.f, 1.f, 1.f);

    Out.vColor = smoothstep(0.8f, 1.f, Ret);

    return Out;
}



DepthStencilState	DSS_None_ZTest_And_Write
{
    DepthEnable = false;
    DepthWriteMask = zero;
};


technique11 DefaultTechnique
{
    pass Default
    {
        SetBlendState(BS_AlphaBlend_Add, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
        SetDepthStencilState(DSS_None_ZTest_And_Write, 0);
        SetRasterizerState(RS_Default);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN();
    }

}