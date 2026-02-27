#include "Shader_Defines.hlsli"

static const float2x2 BayerMatrix2 =
{
    1.0 / 5.0, 3.0 / 5.0,
	4.0 / 5.0, 2.0 / 5.0
};

static const float3x3 BayerMatrix3 =
{
    3.0 / 10.0, 7.0 / 10.0, 4.0 / 10.0,
	6.0 / 10.0, 1.0 / 10.0, 9.0 / 10.0,
	2.0 / 10.0, 8.0 / 10.0, 5.0 / 10.0
};

static const float4x4 BayerMatrix4 =
{
    1.0 / 17.0, 9.0 / 17.0, 3.0 / 17.0, 11.0 / 17.0,
	13.0 / 17.0, 5.0 / 17.0, 15.0 / 17.0, 7.0 / 17.0,
	4.0 / 17.0, 12.0 / 17.0, 2.0 / 17.0, 10.0 / 17.0,
	16.0 / 17.0, 8.0 / 17.0, 14.0 / 17.0, 6.0 / 17.0
};

static const float BayerMatrix8[8][8] =
{
    { 1.0 / 65.0, 49.0 / 65.0, 13.0 / 65.0, 61.0 / 65.0, 4.0 / 65.0, 52.0 / 65.0, 16.0 / 65.0, 64.0 / 65.0 },
    { 33.0 / 65.0, 17.0 / 65.0, 45.0 / 65.0, 29.0 / 65.0, 36.0 / 65.0, 20.0 / 65.0, 48.0 / 65.0, 32.0 / 65.0 },
    { 9.0 / 65.0, 57.0 / 65.0, 5.0 / 65.0, 53.0 / 65.0, 12.0 / 65.0, 60.0 / 65.0, 8.0 / 65.0, 56.0 / 65.0 },
    { 41.0 / 65.0, 25.0 / 65.0, 37.0 / 65.0, 21.0 / 65.0, 44.0 / 65.0, 28.0 / 65.0, 40.0 / 65.0, 24.0 / 65.0 },
    { 3.0 / 65.0, 51.0 / 65.0, 15.0 / 65.0, 63.0 / 65.0, 2.0 / 65.0, 50.0 / 65.0, 14.0 / 65.0, 62.0 / 65.0 },
    { 35.0 / 65.0, 19.0 / 65.0, 47.0 / 65.0, 31.0 / 65.0, 34.0 / 65.0, 18.0 / 65.0, 46.0 / 65.0, 30.0 / 65.0 },
    { 11.0 / 65.0, 59.0 / 65.0, 7.0 / 65.0, 55.0 / 65.0, 10.0 / 65.0, 58.0 / 65.0, 6.0 / 65.0, 54.0 / 65.0 },
    { 43.0 / 65.0, 27.0 / 65.0, 39.0 / 65.0, 23.0 / 65.0, 42.0 / 65.0, 26.0 / 65.0, 38.0 / 65.0, 22.0 / 65.0 }
};

float BayerDither2(in float2 pixel)
{
    return BayerMatrix2[pixel.x % 2][pixel.y % 2];
}

float BayerDither3(in float2 pixel)
{
    return BayerMatrix3[pixel.x % 3][pixel.y % 3];
}

float BayerDither4(in float2 pixel)
{
    return BayerMatrix4[pixel.x % 4][pixel.y % 4];
}

float BayerDither8(in float2 pixel)
{
    return BayerMatrix8[pixel.x % 8][pixel.y % 8];
}

float BayerDither(in float2 pixel)
{
    return BayerDither8(pixel);
}


float2 Dither(float2 coord, float seed, float2 size)
{
    float noiseX = ((frac(1.0 - (coord.x + seed * 1.0) * (size.x / 2.0)) * 0.25) + (frac((coord.y + seed * 2.0) * (size.y / 2.0)) * 0.75)) * 2.0 - 1.0;
    float noiseY = ((frac(1.0 - (coord.x + seed * 3.0) * (size.x / 2.0)) * 0.75) + (frac((coord.y + seed * 4.0) * (size.y / 2.0)) * 0.25)) * 2.0 - 1.0;
    return float2(noiseX, noiseY);
}

float2 ModDither(float2 u)
{
    float noiseX = fmod(u.x + u.y + fmod(208. + u.x * 3.58, 13. + fmod(u.y * 22.9, 9.)), 7.) * .143;
    float noiseY = fmod(u.y + u.x + fmod(203. + u.y * 3.18, 12. + fmod(u.x * 27.4, 8.)), 6.) * .139;
    return float2(noiseX, noiseY) * 2.0 - 1.0;
}

float DoAttenuation(float distance, float range, float quad_damping)
{
    float att = saturate(1.0f - ((distance * distance) / (range * range)));
    return (att * att) / (quad_damping * quad_damping);
}




/******************************/
/*For. CascadeShadow */


#define SHADOW_CASCADE_SIZE  2048
#define CASCADE_COUNT  4


SamplerComparisonState g_ShadowSampler
{
    Filter = COMPARISON_MIN_MAG_LINEAR_MIP_POINT;
    AddressU = MIRROR;
    AddressV = MIRROR;
    ComparisonFunc = LESS;
};


//https://panoskarabelas.com/posts/screen_space_shadows/
static const uint SSCS_MAX_STEPS = 16; // Max ray steps, affects quality and performance.

static float ConvertZToLinearDepth(float depth)
{
    float cameraNear =0.1f;
    float cameraFar = 1000.f;
    return (cameraNear * cameraFar) / (cameraFar - depth * (cameraFar - cameraNear));
}

inline bool IsSaturated(float value)
{
    return value == saturate(value);
}
inline bool IsSaturated(float2 value)
{
    return IsSaturated(value.x) && IsSaturated(value.y);
}

float SSCS(float3 viewSpacePosition, float4 lightdir, float4x4 projection,Texture2D DepthTx)
{
    float3 rayPos = viewSpacePosition;
    float2 rayUV = 0.0f;

    float4 projectedRay = mul(float4(rayPos, 1.0f), projection);
    projectedRay.xy /= projectedRay.w;
    rayUV = projectedRay.xy * float2(0.5f, -0.5f) + 0.5f;

    float depth = DepthTx.Sample(ClampSampler, rayUV);
    float linearDepth = ConvertZToLinearDepth(depth);
    const float SSCS_STEP_LENGTH = 200.0f / (float) SSCS_MAX_STEPS;

    if (linearDepth > 200.0f)
        return 1.0f;

    float3 rayDir = normalize(-lightdir.xyz);
    float3 rayStep = rayDir * SSCS_STEP_LENGTH;
    
    float occlusion = 0.0f;
    [unroll(SSCS_MAX_STEPS)]
    for (uint i = 0; i < SSCS_MAX_STEPS; i++)
    {
        rayPos += rayStep;
        projectedRay = mul(float4(rayPos, 1.0), projection);
        projectedRay.xy /= projectedRay.w;
        rayUV = projectedRay.xy * float2(0.5f, -0.5f) + 0.5f;

        [branch]
        if (IsSaturated(rayUV))
        {
            depth = DepthTx.Sample(ClampSampler, rayUV);
            linearDepth = ConvertZToLinearDepth(depth);
            float depthDelta = projectedRay.z - linearDepth;

            if (depthDelta > 0 && (depthDelta < 0.5f))
            {
                occlusion = 1.0f;
                float2 fade = max(12 * abs(rayUV - 0.5) - 5, 0);
                occlusion *= saturate(1 - dot(fade, fade));
                break;
            }
        }
    }

    return 1.0f - occlusion;
}


float CalcShadowFactor_Basic(SamplerComparisonState shadowSampler, Texture2D<float> shadowMap, float3 uvd)
{
    if (uvd.z > 1.0f)
        return 1.0;
    return shadowMap.SampleCmpLevelZero(shadowSampler,
		uvd.xy, uvd.z).r;
}


float CalcShadowFactor_PCF3x3(SamplerComparisonState shadowSampler, Texture2D<float> shadowMap,
							  float3 uvd, int smSize, float softness)
{
    if (uvd.z > 1.0f)
        return 1.0;

    float depth = uvd.z;
    const float dx = 1.0f / smSize;
    float percentLit = 0.0f;

    float2 offsets[9] =
    {
        float2(-dx, -dx), float2(0.0f, -dx), float2(dx, -dx),
		float2(-dx, 0.0f), float2(0.0f, 0.0f), float2(dx, 0.0f),
		float2(-dx, +dx), float2(0.0f, +dx), float2(dx, +dx)
    };

	[unroll]
    for (int i = 0; i < 9; ++i)
    {
        offsets[i] = offsets[i] * float2(softness, softness);
        percentLit += shadowMap.SampleCmpLevelZero(shadowSampler,
			uvd.xy + offsets[i], depth).r;
    }
    return percentLit /= 9.0f;
}

float CSMCalcShadowFactor_Basic(SamplerComparisonState shadowSampler, Texture2DArray<float> shadowMap,
								uint index, float3 uvd, int smSize, float softness)
{
    return shadowMap.SampleCmpLevelZero(shadowSampler,
		float3(uvd.xy, index), uvd.z).r;
}

float CSMCalcShadowFactor_PCF3x3(SamplerComparisonState shadowSampler, Texture2DArray<float> shadowMap, uint arrayIndex, float3 uvd, int smSize, float softness)
{
    const float dx = 1.0f / smSize;
    float percentLit = 0.0f;

    const float2 offsets[9] =
    {
        float2(-dx, -dx), float2(0.0f, -dx), float2(dx, -dx),
		float2(-dx, 0.0f), float2(0.0f, 0.0f), float2(dx, 0.0f),
		float2(-dx, +dx), float2(0.0f, +dx), float2(dx, +dx)
    };

	[unroll]
    for (int i = 0; i < 9; ++i)
    {
        percentLit += shadowMap.SampleCmpLevelZero(shadowSampler,
			float3(uvd.xy + offsets[i], arrayIndex), uvd.z).r;
    }
    return percentLit /= 9.0f;
}


float CalcShadowFactor_PCF5x5(SamplerComparisonState shadowSampler, Texture2D<float> shadowMap,
							  float3 shadowPosH, int smSize, float softness)
{
    if (shadowPosH.z > 1.0f)
        return 1.0;
	
    float depth = shadowPosH.z;
    const float dx = 1.0f / smSize;

    float percentLit = 0.0f;
    float2 offsets[25] =
    {
        float2(-2 * dx, -2 * dx), float2(-1 * dx, -2 * dx), float2(0 * dx, -2 * dx), float2(1 * dx, -2 * dx), float2(2 * dx, -2 * dx),
		float2(-2 * dx, -1 * dx), float2(-1 * dx, -1 * dx), float2(0 * dx, -1 * dx), float2(1 * dx, -1 * dx), float2(2 * dx, -1 * dx),
		float2(-2 * dx, 0 * dx), float2(-1 * dx, 0 * dx), float2(0 * dx, 0 * dx), float2(1 * dx, 0 * dx), float2(2 * dx, 0 * dx),
		float2(-2 * dx, 1 * dx), float2(-1 * dx, 1 * dx), float2(0 * dx, 1 * dx), float2(1 * dx, 1 * dx), float2(2 * dx, 1 * dx),
		float2(-2 * dx, 2 * dx), float2(-1 * dx, 2 * dx), float2(0 * dx, 2 * dx), float2(1 * dx, 2 * dx), float2(2 * dx, 2 * dx),
    };

	[unroll]
    for (int i = 0; i < 25; ++i)
    {
        offsets[i] = offsets[i] * float2(softness, softness);
        percentLit += shadowMap.SampleCmpLevelZero(shadowSampler,
			shadowPosH.xy + offsets[i], depth).r;
    }
    return percentLit /= 25.0f;
}

float CalcShadowFactor_PCF7x7(SamplerComparisonState shadowSampler, Texture2D<float> shadowMap,
							  float3 shadowPosH, int smSize, float softness)
{
    if (shadowPosH.z > 1.0f)
        return 1.0;
   
    float depth = shadowPosH.z;
    const float dx = 1.0f / smSize;
    float percentLit = 0.0f;

	[unroll]
    for (int i = -3; i <= 3; ++i)
    {
        for (int j = -3; j <= 3; ++j)
        {
            percentLit += shadowMap.SampleCmpLevelZero(shadowSampler,
				shadowPosH.xy + float2(i * dx * softness, j * dx * softness), depth).r;
        }
    }
    return percentLit /= 49;
}

static const float2 PoissonSamples[64] =
{
    float2(-0.5119625f, -0.4827938f),
	float2(-0.2171264f, -0.4768726f),
	float2(-0.7552931f, -0.2426507f),
	float2(-0.7136765f, -0.4496614f),
	float2(-0.5938849f, -0.6895654f),
	float2(-0.3148003f, -0.7047654f),
	float2(-0.42215f, -0.2024607f),
	float2(-0.9466816f, -0.2014508f),
	float2(-0.8409063f, -0.03465778f),
	float2(-0.6517572f, -0.07476326f),
	float2(-0.1041822f, -0.02521214f),
	float2(-0.3042712f, -0.02195431f),
	float2(-0.5082307f, 0.1079806f),
	float2(-0.08429877f, -0.2316298f),
	float2(-0.9879128f, 0.1113683f),
	float2(-0.3859636f, 0.3363545f),
	float2(-0.1925334f, 0.1787288f),
	float2(0.003256182f, 0.138135f),
	float2(-0.8706837f, 0.3010679f),
	float2(-0.6982038f, 0.1904326f),
	float2(0.1975043f, 0.2221317f),
	float2(0.1507788f, 0.4204168f),
	float2(0.3514056f, 0.09865579f),
	float2(0.1558783f, -0.08460935f),
	float2(-0.0684978f, 0.4461993f),
	float2(0.3780522f, 0.3478679f),
	float2(0.3956799f, -0.1469177f),
	float2(0.5838975f, 0.1054943f),
	float2(0.6155105f, 0.3245716f),
	float2(0.3928624f, -0.4417621f),
	float2(0.1749884f, -0.4202175f),
	float2(0.6813727f, -0.2424808f),
	float2(-0.6707711f, 0.4912741f),
	float2(0.0005130528f, -0.8058334f),
	float2(0.02703013f, -0.6010728f),
	float2(-0.1658188f, -0.9695674f),
	float2(0.4060591f, -0.7100726f),
	float2(0.7713396f, -0.4713659f),
	float2(0.573212f, -0.51544f),
	float2(-0.3448896f, -0.9046497f),
	float2(0.1268544f, -0.9874692f),
	float2(0.7418533f, -0.6667366f),
	float2(0.3492522f, 0.5924662f),
	float2(0.5679897f, 0.5343465f),
	float2(0.5663417f, 0.7708698f),
	float2(0.7375497f, 0.6691415f),
	float2(0.2271994f, -0.6163502f),
	float2(0.2312844f, 0.8725659f),
	float2(0.4216993f, 0.9002838f),
	float2(0.4262091f, -0.9013284f),
	float2(0.2001408f, -0.808381f),
	float2(0.149394f, 0.6650763f),
	float2(-0.09640376f, 0.9843736f),
	float2(0.7682328f, -0.07273844f),
	float2(0.04146584f, 0.8313184f),
	float2(0.9705266f, -0.1143304f),
	float2(0.9670017f, 0.1293385f),
	float2(0.9015037f, -0.3306949f),
	float2(-0.5085648f, 0.7534177f),
	float2(0.9055501f, 0.3758393f),
	float2(0.7599946f, 0.1809109f),
	float2(-0.2483695f, 0.7942952f),
	float2(-0.4241052f, 0.5581087f),
	float2(-0.1020106f, 0.6724468f),
};

float CalcShadowFactor_Poisson(SamplerComparisonState shadowSampler, Texture2D<float> shadowMap,
							   float4 shadowPosH, int smSize, float softness)
{
    if (shadowPosH.z > 1.0f)
        return 1.0;

    float depth = shadowPosH.z;
    const float dx = 1.0f / smSize;
    float percentLit = 0.0f;

	[unroll]
    for (int i = 0; i < 64; ++i)
    {
        percentLit += shadowMap.SampleCmpLevelZero(shadowSampler,
			shadowPosH.xy + PoissonSamples[i] * float2(dx * softness, dx * softness), depth).r;
    }
    return percentLit /= 64;
}


