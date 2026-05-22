#include "Engine_Shader_Defines.hlsli"

float4x4 g_WorldMatrix;
float4x4 g_ViewMatrix;
float4x4 g_ProjMatrix;

float3   g_vScale = float3(1.f, 1.f, 1.f);
float4   g_vColor = float4(1.f, 1.f, 1.f, 1.f);

texture2D g_Texture;


DepthStencilState DSS_DepthReadNoWrite
{
    DepthEnable = true;
    DepthWriteMask = ZERO;
    DepthFunc = LESS_EQUAL;
};

DepthStencilState DSS_DepthOff
{
    DepthEnable = false;
    DepthWriteMask = ZERO;
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
    float3 vPos : POSITION;
    float3 vNorm : NORMAL;
    float2 vTex : TEXCOORD0;
    float3 vTangent : TANGENT;
    float3 vBinormal : BINORMAL;
};

struct VS_OUT
{
    float4 vPos : SV_POSITION;
    float2 vTex : TEXCOORD0;
};


VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT Out;

    float3 vScaled = In.vPos * g_vScale;
    float4 vWorld = mul(float4(vScaled, 1.f), g_WorldMatrix);
    float4 vView = mul(vWorld, g_ViewMatrix);
    Out.vPos = mul(vView, g_ProjMatrix);

    Out.vTex = In.vTex;
    return Out;
}


float4 PS_MAIN(VS_OUT In) : SV_TARGET0
{
    float4 vTex = g_Texture.Sample(LinearSampler, In.vTex);

/* Particle3D와 동일 단일 채널 마스크 자동 인식: g=0, b=0, a=1 텍스처는 R 마스크로 처리 */
const bool bSingleChannelMask =
    (vTex.g <= 0.0001f && vTex.b <= 0.0001f && vTex.a >= 0.999f);

if (bSingleChannelMask)
{
    const float fMask = vTex.r;
    return float4(g_vColor.rgb * fMask, g_vColor.a * fMask);
}

return vTex * g_vColor;
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
    pass Alpha_DepthOff         // pass 2
    {
        SetRasterizerState(RS_Cull_None);
        SetDepthStencilState(DSS_DepthOff, 0);
        SetBlendState(BS_AlphaBlend, float4(0,0,0,0), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN();
    }
    pass Additive_DepthOff      // pass 3
    {
        SetRasterizerState(RS_Cull_None);
        SetDepthStencilState(DSS_DepthOff, 0);
        SetBlendState(BS_Additive, float4(0,0,0,0), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN();
    }
};