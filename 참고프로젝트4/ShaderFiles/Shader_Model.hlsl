
#include "Shader_Defines.hlsli"

cbuffer WVP : register(b1)
{
    matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
    matrix g_ShadowVIewProjMatrices[4];
};

Texture2D		g_DiffuseTexture;
Texture2D		g_NormalTexture;
Texture2D		g_MRVTexture;
Texture2D		g_AOTexture;
Texture2D		g_EmissiveTexture;
Texture2D		g_AlphaTexture;

Texture2D		g_DepthTexture_Character;
float4			g_LightConeColor;

bool			g_bDark = false;     

float			g_fFar = 1000.f;


struct VS_IN
{
	float3		vPosition : POSITION;
	float3		vNormal : NORMAL;
	float2		vTexcoord : TEXCOORD0;
	float3		vTangent : TANGENT;
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

	matrix		matWV, matWVP;

	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);

	Out.vPosition = mul(float4(In.vPosition, 1.f), matWVP);
	Out.vNormal = normalize(mul(float4(In.vNormal, 0.f), g_WorldMatrix));
	Out.vTexcoord = In.vTexcoord;
	Out.vWorldPos = mul(float4(In.vPosition, 1.f), g_WorldMatrix);
	Out.vProjPos = Out.vPosition;
	Out.vTangent = normalize(mul(float4(In.vTangent, 0.f), g_WorldMatrix)); //X축 (World적용) 
	Out.vBinormal = normalize(vector(cross(Out.vNormal.xyz, Out.vTangent.xyz), 0.f)); //Y축 (World상의 x,z축으로 y축 구함)

	return Out;
}

VS_OUT_SHADOW VS_MAIN_SHADOW(VS_IN In)
{
    VS_OUT_SHADOW Out = (VS_OUT_SHADOW) 0;
	
    Out.vWorldPos = mul(vector(In.vPosition, 1.f), g_WorldMatrix);

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
    for (uint i = 0; i < 4; ++i)
    {
        GS_OUT_SHADOW_TEX_ARRAY output[3];
        for (uint r = 0; r < 3; ++r)
        {
            output[r].Position = mul(input[r].vWorldPos, g_ShadowVIewProjMatrices[i]);
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
	float4		vDiffuse : SV_TARGET0;
	float4		vNormal : SV_TARGET1;
	float4		vDepth : SV_TARGET2;
    float4		vPBR : SV_TARGET3;
    float4		vEmissive : SV_TARGET4;
    float4		vShaderTag : SV_TARGET5;
};


struct PS_IN_SHADOW
{
    float4 Position : SV_POSITION;
};

struct PS_OUT_SHADOW
{
    vector vLightDepth : SV_TARGET0;
};


struct PS_OUT_LIGHTMODEL
{
    float4 vDiffuse : SV_TARGET0;
    float4 vNormal : SV_TARGET1;
    float4 vDepth : SV_TARGET2;
    float4 vPBR : SV_TARGET3;
    float4 vEmissive : SV_TARGET4;
    float4 vShaderTag : SV_TARGET5;
};

struct PS_OUT_CHARACTER
{
    float4 vDiffuse : SV_TARGET0;
    float4 vNormal : SV_TARGET1;
    float4 vDepth : SV_TARGET2;
    float4 vPBR : SV_TARGET3;
    float4 vEmissive : SV_TARGET4;
    float4 vShaderTag : SV_TARGET5;
};

/* 픽셀셰이더 : 픽셀의 색!!!! 을 결정한다. */
PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

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

PS_OUT_CHARACTER PS_MAIN_CHARACTER(PS_IN In)
{
	PS_OUT_CHARACTER		Out = (PS_OUT_CHARACTER)0;

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
	
    Out.vNormal = vector(vNormal.xyz * 0.5f + 0.5f, 0.f);
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fFar, 0.0f, 0.0f);
    Out.vPBR = g_MRVTexture.Sample(LinearSampler, In.vTexcoord);
    Out.vPBR.z = g_AOTexture.Sample(LinearSampler, In.vTexcoord).x;
    Out.vEmissive = vEmissiveDesc;
   
    Out.vShaderTag = float4(1.f, 0.0f, 0.f, 0.f);
	
	return Out;
}


PS_OUT PS_MAIN_MAP(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	vector vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);
	vector vNormalDesc = g_NormalTexture.Sample(LinearSampler, In.vTexcoord);

	float3 vNormal = vNormalDesc.xyz * 2.f - 1.f; //0,0 ~ 1,1 -> -1,-1 ~ 1,1

	float3x3 WorldMatrix = float3x3(In.vTangent.xyz, In.vBinormal.xyz, In.vNormal.xyz); //각 월드 적용한 로컬스페이스로 월드Matrix 만듦

	vNormal = mul(vNormal, WorldMatrix); //Normal텍스처에서 구한 Normal에 위에서 구한 WorldMatrix 적용

	if (vMtrlDiffuse.a < 0.3f)
		discard;

	float2	vDepthTexcoord;
	vDepthTexcoord.x = (In.vProjPos.x / In.vProjPos.w) * 0.5f + 0.5f;
	vDepthTexcoord.y = (In.vProjPos.y / In.vProjPos.w) * -0.5f + 0.5f;
	float4	vDepthDesc = g_DepthTexture_Character.Sample(PointSampler, vDepthTexcoord);

	if (vDepthDesc.y != 1.f)
	{
		Out.vDiffuse = Color(78, 89, 119,255);
	}
	else
	{
		Out.vDiffuse = vMtrlDiffuse;
	}

	if (g_bDark)
		Out.vDiffuse.rgb *= 0.5f;

	Out.vNormal = vector(vNormal.xyz * 0.5f + 0.5f, 0.f);
	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fFar, 0.0f, 0.0f);
    Out.vPBR = g_MRVTexture.Sample(LinearSampler, In.vTexcoord);
   
	Out.vEmissive = g_EmissiveTexture.Sample(LinearSampler, In.vTexcoord);
    Out.vShaderTag = float4(0.f, 1.f, 0.f, 0.f);
	
	return Out;
}


PS_OUT_LIGHTMODEL PS_LIGHTMODEL(PS_IN In)
{
    PS_OUT_LIGHTMODEL Out = (PS_OUT_LIGHTMODEL) 0;

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
	
    Out.vNormal = vector(vNormal.xyz * 0.5f + 0.5f, 0.f);
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fFar, 0.0f, 0.0f);
    Out.vPBR = g_MRVTexture.Sample(LinearSampler, In.vTexcoord);
    Out.vPBR.z = g_AOTexture.Sample(LinearSampler, In.vTexcoord).x;
    Out.vEmissive = vEmissiveDesc;
   
    Out.vShaderTag = float4(0.f, 1.0f, 0.f, 0.f);
	
    return Out;
}

PS_OUT PS_LIGHTCONE(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    vector vMtrlDiffuse = g_LightConeColor; //
    vector vNormalDesc = g_NormalTexture.Sample(LinearSampler, In.vTexcoord);
    float4 vEmissiveDesc = { 0.f, 0.f, 0.f, 1.f };
    float3 vNormal = vNormalDesc.xyz * 2.f - 1.f; //0,0 ~ 1,1 -> -1,-1 ~ 1,1

    float3x3 WorldMatrix = float3x3(In.vTangent.xyz, In.vBinormal.xyz, In.vNormal.xyz); //각 월드 적용한 로컬스페이스로 월드Matrix 만듦

    vNormal = mul(vNormal, WorldMatrix); //Normal텍스처에서 구한 Normal에 위에서 구한 WorldMatrix 적용
		
    if (vMtrlDiffuse.a < 0.005f)
        discard;
	
    //vMtrlDiffuse *= g_AOTexture.Sample(LinearSampler, In.vTexcoord).x;
    //vMtrlDiffuse += vEmissiveDesc;
	
    Out.vDiffuse = vMtrlDiffuse;
    Out.vNormal = vector(vNormal.xyz * 0.5f + 0.5f, 0.f);
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fFar, 0.0f, 0.0f);
    Out.vPBR = g_MRVTexture.Sample(LinearSampler, In.vTexcoord);
    Out.vEmissive = vEmissiveDesc;
	
    if (vEmissiveDesc.r > 0.05f || vEmissiveDesc.g > 0.05f || vEmissiveDesc.b > 0.05f)
        Out.vShaderTag = float4(0.f, 0.0f, 0.f, 1.f);
    else
        Out.vShaderTag = float4(0.f, 1.0f, 0.f, 0.f);
	
    return Out;
}


PS_OUT_SHADOW PS_MAIN_SHADOW(PS_IN_SHADOW In)
{
    PS_OUT_SHADOW Out = (PS_OUT_SHADOW) 0;
    Out.vLightDepth = float4(1.f, 0.f, 0.f, 1.f);
    return Out;

}



technique11 DefaultTechnique
{
	pass Model  //0
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

    pass Model_Character //1
    {
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_Default, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_CHARACTER();
	}

    pass Model_Map //2
    {
		SetRasterizerState(RS_Cull_None);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_Default, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_MAP();
	}

    pass Shadow //3
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 4);
        SetBlendState(BS_Default, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN_SHADOW();
        GeometryShader = compile gs_5_0 GS_MAIN_SHADOW();
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_SHADOW();
    }

    pass LightModel //4
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_LIGHTMODEL();
    }

    pass LightCone //5
    {
        SetRasterizerState(RS_Cull_None);
        SetDepthStencilState(DSS_DepthEnable_WriteZero, 0);
        SetBlendState(BS_AlphaBlend_Add, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_LIGHTCONE();
    }
}