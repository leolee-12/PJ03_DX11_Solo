
#include "Shader_Defines.hlsli"

cbuffer WVP : register(b1)
{
    matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
};

Texture2D   g_OriginalRenderTexture;

Texture2D	g_ExtractBloomTexture;
Texture2D	g_ExtractLuminanceTexture;
Texture2D	g_ShaderTagTexture;
Texture2D g_DistortionTexture;
Texture2D g_ModelEffectTexture;

float g_PixelWidth;
float g_PixelHeight;

float g_BlurStrength;
float g_Brightness;

float2 DownSampleOffsets[25] =
{
	{-2.f,2.f},
	{-1.f,2.f},
	{ 0.f,2.f},
	{ 1.f,2.f},
	{ 2.f,2.f},

	{-2.f,1.f},
	{-1.f,1.f},
	{ 0.f,1.f},
	{ 1.f,1.f},
	{ 2.f,1.f},

	{-2.f,0.f},
	{-1.f,0.f},
	{ 0.f,0.f},
	{ 1.f,0.f},
	{ 2.f,0.f},

	{-2.f,-1.f},
	{-1.f,-1.f},
	{ 0.f,-1.f},
	{ 1.f,-1.f},
	{ 2.f,-1.f},

	{-2.f,-2.f},
	{-1.f,-2.f},
	{ 0.f,-2.f},
	{ 1.f,-2.f},
	{ 2.f,-2.f},
};

float PixelKernel[13] =
{
	-6.f,
	-5.f,
	-4.f,
	-3.f,
	-2.f,
	-1.f,
	 0.f,
	 1.f,
	 2.f,
	 3.f,
	 4.f,
	 5.f,
	 6.f,
};

static const float BlurWeights[13] =
{
	0.002216f,
	0.008764f,
	0.026995f,
	0.064759f,
	0.120985f,
	0.176033f,
	0.199471f,
	0.176033f,
	0.120985f,
	0.064759f,
	0.026995f,
	0.008764f,
	0.002216f,
};

static const float WidePixelKernel[55] =
{
-27,
-26,
-25,
-24,
-23,
-22,
-21,
-20,
-19,
-18,
-17,
-16,
-15,
-14,
-13,
-12,
-11,
-10,
-9,
-8,
-7,
-6,
-5,
-4,
-3,
-2,
-1,
0,
1,
2,
3,
4,
5,
6,
7,
8,
9,
10,
11,
12,
13,
14,
15,
16,
17,
18,
19,
20,
21,
22,
23,
24,
25,
26,
27,
};

static const float WideBlurWeights[55] =
{
	3.4266e-19,
	6.51055e-18,
	1.10693e-16,
	1.68409e-15,
	2.29275e-14,
	2.79314e-13,
	3.04491e-12,
	2.9703e-11,
	2.59282e-10,
	2.0253e-09,
	1.41563e-08,
	8.85434e-08,
	4.95573e-07,
	2.48201e-06,
	1.11236e-05,
	4.46101e-05,
	0.00016009,
	0.000514093,
	0.00147728,
	0.00379866,
	0.00874063,
	0.017997,
	0.033159,
	0.05467,
	0.0806569,
	0.106483,
	0.125794,
	0.132981,
	0.125794,
	0.106483,
	0.0806569,
	0.05467,
	0.033159,
	0.017997,
	0.00874063,
	0.00379866,
	0.00147728,
	0.000514093,
	0.00016009,
	4.46101e-05,
	1.11236e-05,
	2.48201e-06,
	4.95573e-07,
	8.85434e-08,
	1.41563e-08,
	2.0253e-09,
	2.59282e-10,
	2.9703e-11,
	3.04491e-12,
	2.79314e-13,
	2.29275e-14,
	1.68409e-15,
	1.10693e-16,
	6.51055e-18,
	3.4266e-19,
};

/*
5.35321e-05,
0.000244761,
0.000953635,
0.00316618,
0.00895781,
0.0215964,
0.0443683,
0.0776744,
0.115877,
0.147308,
0.159577,
0.147308,
0.115877,
0.0776744,
0.0443683,
0.0215964,
0.00895781,
0.00316618,
0.000953635,
0.000244761,
5.35321e-05,
9.97699e-06,
1.58452e-06,
2.14441e-07,*/



struct VS_IN
{
	float3		vPosition : POSITION;
	float2		vTexcoord : TEXCOORD0;
};

struct VS_OUT
{
	float4		vPosition : SV_POSITION;
	float2		vTexcoord : TEXCOORD0;
};

VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT		Out = (VS_OUT)0;

	matrix			matWV, matWVP;

	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);

	Out.vPosition = mul(vector(In.vPosition, 1.f), matWVP);
	Out.vTexcoord = In.vTexcoord;

	return Out;
}
struct PS_IN
{
	float4		vPosition : SV_POSITION;
	float2		vTexcoord : TEXCOORD0;

};

struct PS_OUT
{
	vector		vBlurTexture : SV_TARGET0;
};

struct PS_OUT_EXTRACT
{
	vector vExtractBloom : SV_TARGET0;
};

PS_OUT PS_MAIN_BLURX(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	// Apply surrounding pixels  
	float4 color = 0;
	float2 samp = In.vTexcoord;

	//vector vExtractBloom = ;

	for (uint i = 0; i < 13; i++) {
		samp.x = In.vTexcoord.x + PixelKernel[i] * g_PixelWidth;
        float4 vSampColor = g_ExtractBloomTexture.Sample(LinearSampler, samp);
		color += g_ExtractBloomTexture.Sample(LinearSampler, samp) * BlurWeights[i] * g_BlurStrength;
	}
	
	
	Out.vBlurTexture = color;

	return Out;
}

PS_OUT PS_MAIN_BLURY(PS_IN In)
{
	PS_OUT Out = (PS_OUT)0;

	// Apply surrounding pixels  
	float4 color = 0;
	float2 samp = In.vTexcoord;

	//vector vExtractBloom = ;

	for (int i = 0; i < 13; i++)
	{
		samp.y = In.vTexcoord.y + PixelKernel[i] * g_PixelHeight;
		color += g_ExtractBloomTexture.Sample(LinearSampler, samp) * BlurWeights[i] * g_BlurStrength;
	}

	Out.vBlurTexture = color;

	return Out;
}

PS_OUT PS_MAIN_DOWNSAMPLING(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	float4 average = { 0.0f, 0.0f, 0.0f, 0.0f };
	float index = 0;

	for (int i = 0; i < 25; i++)
	{
		float4 SampColor = g_OriginalRenderTexture.Sample(LinearSampler, In.vTexcoord + float2(DownSampleOffsets[i].x * g_PixelWidth, DownSampleOffsets[i].y * g_PixelHeight));

		{
			average.rgb += SampColor.rgb;
		}
		if (SampColor.a > 0.0f)
			++index;
	}
	average *= (1.0f / 25.f);
	
	if (index == 0)
		discard;

	Out.vBlurTexture = float4(average.rgb,1.0f);

	return Out;
}

PS_OUT_EXTRACT PS_MAIN_EXTRACT_BRIGHTNESS(PS_IN In)
{
	PS_OUT_EXTRACT Out = (PS_OUT_EXTRACT)0;
	
    vector vOriginalColor = g_OriginalRenderTexture.Sample(LinearSampler, In.vTexcoord) + g_ModelEffectTexture.Sample(LinearSampler, In.vTexcoord);
    vector vShaderTag = g_ShaderTagTexture.Sample(PointSampler, In.vTexcoord);
	//if (vShaderTag[0] == 1.f)
		//discard;
	//Out.vColor = vOriginalColor;
	//Out.vColor.a = 1.f;

	float4 BrightColor = (float4)0.f;

	float brightness = dot(vOriginalColor.rgb, float3(0.2126f, 0.7152f, 0.0722f));
	if (brightness > 0.7f)
		BrightColor = float4(vOriginalColor.rgb * g_Brightness, 1.0f);
	else
	{
		discard;
	}
	Out.vExtractBloom = BrightColor;

	return Out;
}

PS_OUT PS_MAIN_BLURX_GLOW(PS_IN In)
{
	PS_OUT Out = (PS_OUT)0;

	float4 color = 0;
	float2 samp = In.vTexcoord;
	float4 SampleColor = 0;
	uint Index = 0;

	for (uint i = 0; i < 13; i++)
	{
		samp.x = In.vTexcoord.x + PixelKernel[i] * g_PixelWidth;
		SampleColor = g_ExtractBloomTexture.Sample(LinearSampler, samp);

		if (SampleColor.a > 0.03f)
		{
			color.rgb += SampleColor.rgb;
			++Index;
		}
		
		color.a += SampleColor.a * BlurWeights[i] * g_BlurStrength;
	}

	color.rgb /= Index;

	Out.vBlurTexture = color;

	if (Out.vBlurTexture.a < 0.01f)
	{
		discard;
	}

	return Out;
}

PS_OUT PS_MAIN_BLURY_GLOW(PS_IN In)
{
	PS_OUT Out = (PS_OUT)0;

	float4 color = 0;
	float2 samp = In.vTexcoord;
	float4 SampleColor = 0;
	uint Index = 0;

	for (uint i = 0; i < 13; i++)
	{
		samp.y = In.vTexcoord.y + PixelKernel[i] * g_PixelHeight;
		SampleColor = g_ExtractBloomTexture.Sample(LinearSampler, samp) ;

		if (SampleColor.a > 0.03f)
		{
			color.rgb += SampleColor.rgb;
			++Index;
		}

		color.a += SampleColor.a * BlurWeights[i] * g_BlurStrength;
	}

	color.rgb /= Index;

	Out.vBlurTexture = color;

	if (Out.vBlurTexture.a < 0.01f)
	{
		discard;
	}

	return Out;

}

PS_OUT PS_Blur_X_With_OriginTexture(PS_IN In)
{
	PS_OUT Out = (PS_OUT)0;

	float4 color = 0;
	float2 samp = In.vTexcoord;
	float4 SampleColor = 0;
	uint Index = 0;

	for (uint i = 0; i < 55; i++)
	{
		samp.x = In.vTexcoord.x + WidePixelKernel[i] * g_PixelWidth;
        SampleColor = g_ExtractBloomTexture.Sample(ClampSampler, samp);

		if (SampleColor.a > 0.03f)
		{
            color.rgb += SampleColor.rgb;
			++Index;
		}

        color.a += SampleColor.a * WideBlurWeights[i] * g_BlurStrength;
    }

	color.rgb /= Index;

	Out.vBlurTexture = color;

	clip(Out.vBlurTexture.a - 0.01f);

	return Out;
}

PS_OUT PS_Blur_Y_With_OriginTexture(PS_IN In)
{
	PS_OUT Out = (PS_OUT)0;

	float4 color = 0;
	float2 samp = In.vTexcoord;
	float4 SampleColor = 0;
	uint Index = 0;

	for (uint i = 0; i < 55; i++)
	{
		samp.y = In.vTexcoord.y + WidePixelKernel[i] * g_PixelHeight;
        SampleColor = g_ExtractBloomTexture.Sample(ClampSampler, samp);

		if (SampleColor.a > 0.03f)
		{
            color.rgb += SampleColor.rgb;
            ++Index;
        }

        color.a += SampleColor.a * WideBlurWeights[i] * g_BlurStrength;
    }

	color.rgb /= Index;

	Out.vBlurTexture = color;

	clip(Out.vBlurTexture.a - 0.01f);

	return Out;
}


technique11 DefaultTechnique
{
	pass BlurX //0
	{
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		SetDepthStencilState(DSS_DepthEnable_WriteZero, 0);
		SetRasterizerState(RS_Default);

		VertexShader = compile vs_5_0 VS_MAIN();
		HullShader = NULL;
		DomainShader = NULL;
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_BLURX();
	}

	pass BlurY //1
	{
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		SetDepthStencilState(DSS_DepthEnable_WriteZero, 0);
		SetRasterizerState(RS_Default);

		VertexShader = compile vs_5_0 VS_MAIN();
		HullShader = NULL;
		DomainShader = NULL;
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_BLURY();
	}

	pass Extract_Brightness //2
	{
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		SetDepthStencilState(DSS_DepthEnable_WriteZero, 0);
		SetRasterizerState(RS_Default);

		VertexShader = compile vs_5_0 VS_MAIN();
		HullShader = NULL;
		DomainShader = NULL;
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_EXTRACT_BRIGHTNESS();
	}

	pass BlurX_Glow //3
	{
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		SetDepthStencilState(DSS_DepthEnable_WriteZero, 0);
		SetRasterizerState(RS_Default);

		VertexShader = compile vs_5_0 VS_MAIN();
		HullShader = NULL;
		DomainShader = NULL;
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_BLURX_GLOW();
	}

	pass BlurY_Glow //4
	{
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		SetDepthStencilState(DSS_DepthEnable_WriteZero, 0);
		SetRasterizerState(RS_Default);

		VertexShader = compile vs_5_0 VS_MAIN();
		HullShader = NULL;
		DomainShader = NULL;
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_BLURY_GLOW();
	}

	pass BlurX_With_Origin //5
	{
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		SetDepthStencilState(DSS_DepthEnable_WriteZero, 0);
		SetRasterizerState(RS_Default);

		VertexShader = compile vs_5_0 VS_MAIN();
		HullShader = NULL;
		DomainShader = NULL;
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_Blur_X_With_OriginTexture();
	}

	pass BlurY_With_Origin //6
	{
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		SetDepthStencilState(DSS_DepthEnable_WriteZero, 0);
		SetRasterizerState(RS_Default);

		VertexShader = compile vs_5_0 VS_MAIN();
		HullShader = NULL;
		DomainShader = NULL;
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_Blur_Y_With_OriginTexture();
	}

		pass DownSampling //7
	{
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		SetDepthStencilState(DSS_DepthEnable_WriteZero, 0);
		SetRasterizerState(RS_Default);

		VertexShader = compile vs_5_0 VS_MAIN();
		HullShader = NULL;
		DomainShader = NULL;
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_DOWNSAMPLING();
	}
}