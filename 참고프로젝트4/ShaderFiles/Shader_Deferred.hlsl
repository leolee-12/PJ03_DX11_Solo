//#include "Shader_Defines.hlsli"
#include "Shader_LightUitility.hlsli"
#include "Shader_FXAA.hlsli"

cbuffer WVP : register(b1)
{
    matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
    matrix g_ProjMatrixInv, g_ViewMatrixInv;
    matrix g_ShadowMatrices[4];
};

float           g_fSplitDistances[4];
Texture2DArray<float>  g_CascadeDepthTexture;
bool            g_bCascadeShadow;

vector			g_vLightDir;
vector			g_vLightPos;
float			g_fLightRange;
float			g_fSpotPower;
float2			g_fLightThetaPi;
vector			g_vLightDiffuse;
vector			g_vLightAmbient;
vector			g_vLightSpecular;
vector			g_vLightEmissive = { 0.f, 0.f, 0.f, 0.f };
bool			g_bUseVolumetric;
matrix			g_LightViewMatrix, g_LightProjMatrix;

float           g_fLightConstDamping = 1.f;
float           g_fLightLinearDamping = 1.f;
float           g_fLightQuadDamping = 0.f;
float           g_fVolumeQuadDamping = 1.f;

Texture2D		g_DiffuseTexture;
vector			g_vMtrlAmbient = vector(1.f, 1.f, 1.f, 1.f);
vector			g_vMtrlSpecular = vector(1.f, 1.f, 1.f, 1.f);

vector			g_vCamPosition;

float			g_fSSRStep;
int				g_iSSRStepDistance;
float3			g_vHSV;

float           g_fMotionBlurAmount;
float           g_fMotionBlurWidth;
float2          g_vMotionBlurCenter;

float3          g_vRadialBlurWorldPosition;
float           g_fRadialBlurIntensity;

Texture2D		g_ShadeTexture;
Texture2D       g_AmbientTexture;
Texture2D		g_SpecularTexture;
Texture2D		g_NormalTexture;
Texture2D		g_DepthTexture;
Texture2D		g_LightDepthTexture;
Texture2D		g_ResultTexture;
Texture2D		g_ShaderTagTexture;
Texture2D       g_ShadowTexture;

Texture2D		g_VolumetricTexture;
Texture2D		g_BloomTexture;
Texture2D		g_LuminanceBloomTexture;
Texture2D		g_OriginalTexture;
Texture2D		g_OriginalRenderTexture;
Texture2D       g_ModelEffectTexture;

Texture2D		g_DistortionTexture;
Texture2D       g_RadialBlurTexture;
Texture2D       g_SolidEffectTexture;

Texture2D		g_EffectTexture;
Texture2D		g_EmissiveTexture;

Texture2D		g_PBRTexture;
Texture2D		g_HBAOTexture;
Texture2D		g_BRDFTexture;
Texture2D		g_IrradianceTexture;
Texture2D		g_PreFilteredTexture;

float			g_fTexW = 1280.0f;
float			g_fTexH = 720.0f;

Texture2D		g_VelocityTexture;

bool			g_bToneMapping = true;
bool			g_bHDRBloom = true;
bool			g_bPBR = true;
float			g_fExposure = 1.f;

float           g_fFogStart;
float           g_fFogDensity;
float4          g_vFogColor;
float           g_fFogFalloff;
float           g_fFogSkyStrength;
bool            g_bFogHeight;
bool            g_bFog;


bool g_bShaderHalf =  false ;
float g_fShaderHalf_X =  0.f ;

static const float fWeight[13] = {
	0.0561, 0.1353, 0.278, 0.4868, 0.7261, 0.9231, 1,
	0.9231, 0.7261, 0.4868, 0.278, 0.1353, 0.0561
};

static const float fTotal = 6.2108;
// static const float fTotal = 2.6054;


DepthStencilState DSS_DepthFalse_StencilEnable
{
    DepthEnable = false;
    DepthWriteMask = zero;

    StencilEnable = true;
    StencilReadMask = 0xff;
    StencilWriteMask = 0xff;

    FrontFaceStencilFunc = equal;
    FrontFaceStencilPass = keep;
    FrontFaceStencilFail = keep;
};



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

	/* In.vPosition * 월드 * 뷰 * 투영 */
	matrix		matWV, matWVP;

	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);

	Out.vPosition = mul(float4(In.vPosition, 1.f), matWVP);
	Out.vTexcoord = In.vTexcoord;

	return Out;
}

/* 통과된 정점을 대기 .*/

/* 투영변환 : /w */ /* -> -1, 1 ~ 1, -1 */
				/* 뷰포트변환-> 0, 0 ~ WINSX, WINSY */
				/* 래스터라이즈 : 정점정보에 기반하여 픽셀의 정보를 만든다. */


struct PS_IN
{
	float4		vPosition : SV_POSITION;
	float2		vTexcoord : TEXCOORD0;
};

struct PS_OUT
{
	float4		vColor : SV_TARGET0;
};


PS_OUT PS_MAIN_DEBUG(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	Out.vColor = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);

	return Out;
}


//---------------- 조명 ----------------//

struct PS_OUT_LIGHT
{
	float4		vShade : SV_TARGET0;
	float4		vSpecular : SV_TARGET1;
    float4		vAmbient : SV_TARGET2;
    float4		vVolumetric : SV_TARGET3;
    float4		vShadow : SV_TARGET4;
};


float4 VolumetricLighting_Spot(PS_IN In)
{
    float4 vDepthDesc = g_DepthTexture.Sample(PointSampler, In.vTexcoord);   
    float fViewZ = vDepthDesc.y * 1000.f;
    float4 viewSpacePosition, temp;
	
	/* 투영스페이스 상의 위치. */
	/* 로컬위치 * 월드행렬 * 뷰행렬* 투영행렬 / View.z */
	viewSpacePosition.x = In.vTexcoord.x * 2.f - 1.f;
	viewSpacePosition.y = In.vTexcoord.y * -2.f + 1.f;
	viewSpacePosition.z = vDepthDesc.x;
    viewSpacePosition.w = 1.f;
    temp = viewSpacePosition;
	
	/* 뷰스페이스 상의 위치를 구하자. */
	/* 로컬위치 * 월드행렬 * 뷰행렬 */
    
	viewSpacePosition = viewSpacePosition * fViewZ;
    viewSpacePosition = mul(viewSpacePosition, g_ProjMatrixInv);
    if (g_ShaderTagTexture.Sample(PointSampler, In.vTexcoord).a > 0.f)
    {
        temp = temp * fViewZ * 0.1f;
        temp = mul(temp, g_ProjMatrixInv);
    }
	else
        temp = viewSpacePosition;
	
    float3 V = float3(0.0f, 0.0f, 0.0f) - temp.xyz;
    float cameraDistance = length(V);
    V /= cameraDistance;
    const uint SAMPLE_COUNT = 16;
    float3 rayEnd = float3(0.0f, 0.0f, 0.0f);
    float stepSize = length(temp.xyz - rayEnd) / SAMPLE_COUNT;
    viewSpacePosition.xyz = temp.xyz + V * stepSize * BayerDither(In.vPosition.xy);
  	
	float marchedDistance = 0;
    float accumulation = 0;
	//[loop(SAMPLE_COUNT)]
    for (uint i = 0; i < SAMPLE_COUNT; ++i)
    {
        float3 L = (g_vLightPos).xyz - mul(viewSpacePosition, g_ViewMatrixInv).xyz;
        float distSquared = dot(L, L);
        float dist = sqrt(distSquared);
        L /= dist;		        
    
        float cosInnerCone = cos(g_fLightThetaPi.x);
        float cosOuterCone = cos(g_fLightThetaPi.y);
		
        float spotFactor = dot(normalize(L.xyz), -normalize(mul(g_vLightDir, g_ViewMatrix).xyz));
        float spotCutOff = cosOuterCone;
		
		//[branch]
        float spotAngle = acos(spotFactor);
        if (spotAngle < g_fLightThetaPi.y)
        {
            float attenuation = DoAttenuation(dist, g_fLightRange, g_fVolumeQuadDamping);
            float conAtt = saturate((spotFactor - cosOuterCone) / (cosInnerCone - cosOuterCone));
            conAtt *= conAtt;
            attenuation *= conAtt;
            accumulation += attenuation;
        }
        marchedDistance += stepSize;
        viewSpacePosition.xyz = viewSpacePosition.xyz + V * stepSize;			
    }
    accumulation /= SAMPLE_COUNT;
    float4 vResult = max(0, float4(accumulation * g_vLightDiffuse.rgb * g_fSpotPower, 1));
    if (vResult.x < 0.01f && vResult.y < 0.01f && vResult.z < 0.01f)
        vResult = float4(0.f, 0.f, 0.f, 0.f);
    return vResult;
}


float4 VolumetricLighting_Point(PS_IN In)
{
    float4 vDepthDesc = g_DepthTexture.Sample(PointSampler, In.vTexcoord);
    float fViewZ = vDepthDesc.y * 1000.f;
    float4 viewSpacePosition, temp;
	
	/* 투영스페이스 상의 위치. */
	/* 로컬위치 * 월드행렬 * 뷰행렬* 투영행렬 / View.z */
    viewSpacePosition.x = In.vTexcoord.x * 2.f - 1.f;
    viewSpacePosition.y = In.vTexcoord.y * -2.f + 1.f;
    viewSpacePosition.z = vDepthDesc.x;
    viewSpacePosition.w = 1.f;
    temp = viewSpacePosition;
	
	/* 뷰스페이스 상의 위치를 구하자. */
	/* 로컬위치 * 월드행렬 * 뷰행렬 */
    
    viewSpacePosition = viewSpacePosition * fViewZ;
    viewSpacePosition = mul(viewSpacePosition, g_ProjMatrixInv);
    if (g_ShaderTagTexture.Sample(PointSampler, In.vTexcoord).a > 0.f)
    {
        temp = temp * fViewZ * 0.1f;
        temp = mul(temp, g_ProjMatrixInv);
    }
    else
        temp = viewSpacePosition;
	
    float3 V = float3(0.0f, 0.0f, 0.0f) - temp.xyz;
    float cameraDistance = length(V);
    V /= cameraDistance;
    const uint SAMPLE_COUNT = 16;
    float3 rayEnd = float3(0.0f, 0.0f, 0.0f);
    float stepSize = length(temp.xyz - rayEnd) / SAMPLE_COUNT;
    viewSpacePosition.xyz = temp.xyz + V * stepSize * BayerDither(In.vPosition.xy);
  	
    float marchedDistance = 0;
    float accumulation = 0;
    
    for (uint i = 0; i < SAMPLE_COUNT; ++i)
    {
        float3 L = g_vLightPos.xyz - mul(viewSpacePosition, g_ViewMatrixInv).xyz;
        float distSquared = dot(L, L);
        float dist = sqrt(distSquared);
        L /= dist;
       
        float attenuation = DoAttenuation(dist, g_fLightRange, g_fVolumeQuadDamping);
        accumulation += attenuation;
        marchedDistance += stepSize;
        viewSpacePosition.xyz = viewSpacePosition.xyz + V * stepSize;
    }
    accumulation /= SAMPLE_COUNT;
    float4 vResult = max(0, float4(accumulation * g_vLightDiffuse.rgb * g_fSpotPower, 1));
    if (vResult.x < 0.01f && vResult.y < 0.01f && vResult.z < 0.01f)
        vResult = float4(0.f, 0.f, 0.f, 0.f);
    return vResult;
}

PS_OUT_LIGHT PS_MAIN_DIRECTIONAL(PS_IN In)
{
	PS_OUT_LIGHT		Out = (PS_OUT_LIGHT)0;

	vector		vNormalDesc = g_NormalTexture.Sample(PointSampler, In.vTexcoord);
    float4		vMRVDesc = g_PBRTexture.Sample(LinearSampler, In.vTexcoord);
    float4		vDiffuse = pow(g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord), 2.2f);
    float4		vShaderTag = g_ShaderTagTexture.Sample(PointSampler, In.vTexcoord);
	   
	/* 0, 1 -> -1, 1 */
	float4		vNormal = vector(vNormalDesc.xyz * 2.f - 1.f, 0.f);

	Out.vShade = g_vLightDiffuse * min((max(dot(normalize(g_vLightDir) * -1.f, vNormal), 0.f) + (g_vLightAmbient * g_vMtrlAmbient)), 1.f);

	//이미시브 추가
    Out.vShade += g_vLightEmissive;
    Out.vShade.a = min(Out.vShade.a, 1.f);
	
	vector		vDepthDesc = g_DepthTexture.Sample(PointSampler, In.vTexcoord);
	float		fViewZ = vDepthDesc.y * 1000.f;

	vector		vWorldPos;

	/* 투영스페이스 상의 위치. */
	/* 로컬위치 * 월드행렬 * 뷰행렬* 투영행렬 / View.z */
	vWorldPos.x = In.vTexcoord.x * 2.f - 1.f;
	vWorldPos.y = In.vTexcoord.y * -2.f + 1.f;
	vWorldPos.z = vDepthDesc.x;
	vWorldPos.w = 1.f;

	/* 뷰스페이스 상의 위치를 구하자. */
	/* 로컬위치 * 월드행렬 * 뷰행렬 */
	vWorldPos = vWorldPos * fViewZ;
	vWorldPos = mul(vWorldPos, g_ProjMatrixInv);
    float4 vViewPos = vWorldPos;
    
	/* 월드 상의 위치를 구하자. */
	/* 로컬위치 * 월드행렬 */
	vWorldPos = mul(vWorldPos, g_ViewMatrixInv);

    vector vLook = normalize(g_vCamPosition - vWorldPos);
	
    vLook.a = 0.f;
	
    float fOcclusion = vMRVDesc.z;
    float fRoughness = vMRVDesc.y;
    float fMetalness = vMRVDesc.x;
    
    if (g_bPBR && (fRoughness > 0.f || fMetalness > 0.f) && (g_bShaderHalf ? In.vTexcoord.x > g_fShaderHalf_X : true))
    {
        fRoughness = max(fRoughness, 0.001f);
        fMetalness = max(fMetalness, 0.001f);
        fOcclusion = max(fOcclusion, 0.1f);
        
        vector vHalfVec = normalize(vLook + normalize(g_vLightDir) * -1.f);

        float NdotL = saturate(dot(vNormal, normalize(g_vLightDir) * -1.f));
        float NdotH = saturate(dot(vNormal, vHalfVec));
        float NdotV = saturate(dot(vNormal, vLook));
        float HdotV = saturate(dot(vHalfVec, vLook));

        float3 vMetalic = lerp(float3(0.04f, 0.04f, 0.04f), vDiffuse.xyz, fMetalness);
		
		//  Cook-Torrance specular BRDF
        float NDF = trowbridgeReitzNDF(NdotH, fRoughness);
        float3 F = fresnel(vMetalic, HdotV, fRoughness);
        float G = schlickBeckmannGAF(NdotV, fRoughness) * schlickBeckmannGAF(NdotL, fRoughness);
		
		//  Energy Conservation
        vector kS = vector(F, 0.f);
        vector kD = vector(1.f, 1.f, 1.f, 0.f) - kS;
        kD *= 1.f - fMetalness;

        vector vNumerator = kS * NDF * G;
        float fDenominator = 4.f * NdotV * NdotL;
        vector vSpecular = vNumerator / max(fDenominator, 0.001f);

        vector vSpecularAcc = vSpecular * g_vLightDiffuse * fOcclusion;// * saturate(NdotL);
        vector vAmbientColor = vDiffuse * kD * g_vLightDiffuse * (NdotL);
               
        if (vShaderTag.a <= 0.f)
        {
			Out.vSpecular = vSpecularAcc;
			Out.vSpecular.a = 0.f;
        }
        
        Out.vAmbient = vAmbientColor;
        Out.vAmbient.a = 1.f;
    }
    else
    { //Out.vShade = vShadeDesc;

        vector vResult = g_vLightDiffuse * saturate(saturate(dot(normalize(g_vLightDir) * -1.f, vNormal)) + (g_vLightAmbient * g_vMtrlAmbient));

        if (vResult.r < 0.05f && vResult.g < 0.05f && vResult.b < 0.05f)
            discard;

        Out.vAmbient = vResult;
        Out.vAmbient.a = 1.f;

        vector vReflect = reflect(normalize(g_vLightDir), vNormal);

        if (vShaderTag.a <= 0.f)
        {
			Out.vSpecular = (g_vLightSpecular * g_vMtrlSpecular) * pow(max(dot(normalize(vLook) * -1.f, normalize(vReflect)), 0.f), 30.f);
			Out.vSpecular.a = 0.f;			
        }
    }
    
    float fShadowFactor = 1.0f;
    /*For.Cascade Shadow*/
    if (g_bCascadeShadow)
    {
         // 방향성 라이트이고 캐스케이드 그림자 매핑을 사용하는 경우 실행됩니다.

        float view_depth = vViewPos.z;

        // 총 4개의 캐스케이드에 대해 반복합니다.
        for (uint i = 0; i < 4; ++i)
        {
            matrix lightSpaceMatrix = g_ShadowMatrices[i]; // 해당 캐스케이드에 대한 라이트 공간 행렬을 가져옵니다.

                // 현재 픽셀의 뷰 공간에서의 깊이 값이 현재 캐스케이드의 분할 값보다 작은지 확인합니다.
            if (view_depth < g_fSplitDistances[i])
            {
                    // 현재 픽셀이 해당 캐스케이드에 속한다면 그림자 맵 좌표를 계산합니다.
                float4 shadowMapCoords = mul(vWorldPos, lightSpaceMatrix); // 뷰 공간 좌표를 라이트 공간 좌표로 변환합니다.
                //shadowMapCoords.w -= 0.1f;
                float3 UVD = 0.0f;
                UVD = shadowMapCoords.xyz / shadowMapCoords.w; // 원근 제거된 좌표를 계산합니다.
                UVD.xy = 0.5 * UVD.xy + 0.5; // [0,1] 범위로 매핑합니다.
                UVD.y = 1.0 - UVD.y; // y 좌표를 뒤집습니다 (DirectX에서 텍스처 좌표와 맞춤).

                    // PCF(픽셀 커널 필터링)를 사용하여 캐스케이드별 그림자 팩터를 계산합니다.
                fShadowFactor = CSMCalcShadowFactor_PCF3x3(g_ShadowSampler, g_CascadeDepthTexture, i, UVD, SHADOW_CASCADE_SIZE,
               1.0f);
                                
                break; // 해당 캐스케이드에서의 그림자 맵 좌표를 계산했으므로 반복문을 종료합니다.
            }
        }
        Out.vShadow = min(fShadowFactor + 0.7f, 1.f);
        if (vShaderTag.x > 0.f)
            Out.vShadow = 1.f;
        
       
            
        //Out.vAmbient = Out.vAmbient * SSCS(vViewPos.xyz,g_vLightDir,g_ProjMatrix,g_DepthTexture);
        
    }
    
	
	//vector		vReflect = reflect(normalize(g_vLightDir), vNormal);

	//Out.vSpecular = (g_vLightSpecular * g_vMtrlSpecular) * pow(max(dot(normalize(vLook) * -1.f, normalize(vReflect)), 0.f), 30.f);

	return Out;
}


PS_OUT_LIGHT PS_MAIN_POINT(PS_IN In)
{
    PS_OUT_LIGHT Out = (PS_OUT_LIGHT) 0;

    vector vNormalDesc = g_NormalTexture.Sample(PointSampler, In.vTexcoord);
    float4 vMRVDesc = g_PBRTexture.Sample(LinearSampler, In.vTexcoord);
    float4 vDiffuse = pow(g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord), 2.2f);
    float4 vShaderTag = g_ShaderTagTexture.Sample(PointSampler, In.vTexcoord);
    if (vShaderTag.a > 0.f)
        discard;
	/* 0, 1 -> -1, 1 */
    float4 vNormal = vector(vNormalDesc.xyz * 2.f - 1.f, 0.f);

    vector vDepthDesc = g_DepthTexture.Sample(PointSampler, In.vTexcoord);
    float fViewZ = vDepthDesc.y * 1000.f;

    vector vWorldPos;

	//-----------
    vWorldPos.x = In.vTexcoord.x * 2.f - 1.f;
    vWorldPos.y = In.vTexcoord.y * -2.f + 1.f;
    vWorldPos.z = vDepthDesc.x;
    vWorldPos.w = 1.f;

	//-----------
    vWorldPos = vWorldPos * fViewZ;
    vWorldPos = mul(vWorldPos, g_ProjMatrixInv);

	//-----------
    vWorldPos = mul(vWorldPos, g_ViewMatrixInv);
		
    float4 vLightDir = g_vLightPos - vWorldPos;
    float fDistance = length(vLightDir);

    float fAtt = max((g_fLightRange - fDistance) / 
                        (g_fLightConstDamping
                        + (g_fLightLinearDamping * g_fLightRange)
                        + (g_fLightQuadDamping * (g_fLightRange * g_fLightRange))), 0.f);
	
    float3 lightIntensity = g_vLightDiffuse * min(max(dot(normalize(vLightDir), vNormal), 0.f) + (g_vLightAmbient * g_vMtrlAmbient), 1.f);
    lightIntensity += g_vLightEmissive;
	
    
    float fRoughness = vMRVDesc.y;
    float fMetalness = vMRVDesc.x;
    float fOcclusion = vMRVDesc.z;

    if (g_bPBR && (fRoughness > 0.f || fMetalness > 0.f) && (g_bShaderHalf ? In.vTexcoord.x > g_fShaderHalf_X : true))
    {
        vector vLook = normalize(g_vCamPosition - vWorldPos);
        vLook.a = 0.f;
		
        fRoughness = max(fRoughness, 0.001f);
        fMetalness = max(fMetalness, 0.001f);
        fOcclusion = max(fOcclusion, 0.1f);
    
        vector vHalfVec = normalize(vLook + normalize(-vLightDir) * -1.f);

        float NdotL = saturate(dot(vNormal, normalize(-vLightDir) * -1.f));
        float NdotH = saturate(dot(vNormal, vHalfVec));
        float NdotV = saturate(dot(vNormal, vLook));
        float HdotV = saturate(dot(vHalfVec, vLook));
    
        float3 vMetalic = lerp(float3(0.04f, 0.04f, 0.04f), vDiffuse.xyz, fMetalness);

        float NDF = trowbridgeReitzNDF(NdotH, fRoughness);
        float3 F = fresnel(vMetalic, HdotV, fRoughness);
        float G = schlickBeckmannGAF(NdotV, fRoughness) * schlickBeckmannGAF(NdotL, fRoughness);

        vector kS = vector(F, 0.f);
        vector kD = vector(1.f, 1.f, 1.f, 0.f) - kS;
        kD *= 1.f - fMetalness;

        vector vNumerator = kS * NDF * G;
        float fDenominator = 4.f * NdotV * NdotL;
        vector vSpecular = vNumerator / max(fDenominator, 0.001f);

        vector vSpecularAcc = vSpecular * NdotL * g_vLightDiffuse * fAtt /** fOcclusion*/;
        vector vAmbientColor = kD * vDiffuse * fOcclusion * NdotL * g_vLightDiffuse * fAtt;
		
       	Out.vSpecular = vSpecularAcc * g_vLightSpecular;
        Out.vSpecular.a = 0.f;

        Out.vAmbient = vAmbientColor;
        Out.vAmbient.a = 1.f;
    }
    else
    { 
        vector vResult = g_vLightDiffuse * saturate(saturate(dot(normalize(g_vLightDir) * -1.f, vNormal)) + (g_vLightAmbient * g_vMtrlAmbient));

        if (vResult.r < 0.05f && vResult.g < 0.05f && vResult.b < 0.05f)
            discard;

        Out.vAmbient = vResult;
        Out.vAmbient.a = 1.f;

        vector vLook = g_vCamPosition - vWorldPos;
        vector vReflect = reflect(normalize(vLightDir), vNormal);
        float specularIntensity = pow(max(dot(normalize(vLook), normalize(vReflect)), 0.f), 30.f);
		
		
        Out.vSpecular = g_vLightSpecular * g_vMtrlSpecular * specularIntensity * fAtt;
    }
  
    Out.vShade = float4(lightIntensity, 1.0) * fAtt;
    
    if (vShaderTag.a > 0.f)
    {
        Out.vShade = float4(0.f, 0.f, 0.f, 0.f);
        Out.vAmbient = float4(0.f, 0.f, 0.f, 0.f);
        Out.vSpecular = float4(0.f, 0.f, 0.f, 0.f);
    }
    
    if (g_bUseVolumetric == true)
    Out.vVolumetric = VolumetricLighting_Point(In);
    
    return Out;
}

PS_OUT_LIGHT PS_MAIN_SPOT(PS_IN In)
{
    PS_OUT_LIGHT Out = (PS_OUT_LIGHT) 0;
    vector vNormalDesc = g_NormalTexture.Sample(PointSampler, In.vTexcoord);
    vector vNormal = vector(vNormalDesc.xyz * 2.f - 1.f, 0.f);
    float4 vMRVDesc = g_PBRTexture.Sample(LinearSampler, In.vTexcoord);
    float4 vDiffuse = pow(g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord), 2.2f);
    
    vector vDepthDesc = g_DepthTexture.Sample(PointSampler, In.vTexcoord);
    float fViewZ = vDepthDesc.y * 1000.f; //Far
    float4 vShaderTag = g_ShaderTagTexture.Sample(PointSampler, In.vTexcoord);
   
	//------------------
    vector vPosition;
	
    vPosition.x = In.vTexcoord.x * 2.f - 1.f;
    vPosition.y = In.vTexcoord.y * -2.f + 1.f;
    vPosition.z = vDepthDesc.x;
    vPosition.w = 1.f;
	
    vPosition = vPosition * fViewZ;
    vPosition = mul(vPosition, g_ProjMatrixInv);
    vPosition = mul(vPosition, g_ViewMatrixInv);

	//float4 vLightDir = normalize(vPosition - g_vLightPos);
    float4 vLightDir = normalize(vPosition - g_vLightPos);
    float fDistance = length(vPosition - g_vLightPos);
	//------------------
	
    float cosInnerCone = cos(g_fLightThetaPi.x);
    float cosOuterCone = cos(g_fLightThetaPi.y);
	
    float NowSpotCos = dot(normalize(vLightDir), normalize(g_vLightDir));
    float SpotFactor = saturate((NowSpotCos - cosOuterCone) / (cosInnerCone - cosOuterCone));
    	
	//float fAtt = g_fSpotPower - ((SpotFactor / fDistance) * (-g_fSpotPower));
    float fAtt = (SpotFactor / fDistance);
    //float fAtt = max((g_fLightRange - fDistance) / g_fLightRange, 0.f);
		
    
    fAtt = DoAttenuation(fDistance, g_fLightRange, g_fVolumeQuadDamping);
    float conAtt = saturate((SpotFactor - cosOuterCone) / (cosInnerCone - cosOuterCone));
    conAtt *= conAtt;
    fAtt *= conAtt;
        
    float4 lightIntensity = g_vLightDiffuse * saturate(max(dot(normalize(vLightDir) * -1.f, vNormal), 0.f) + (g_vLightAmbient * g_vMtrlAmbient));
    float4 emissive = g_vLightEmissive * SpotFactor;
	
    float fRoughness = vMRVDesc.y;
    float fMetalness = vMRVDesc.x;
    float fOcclusion = vMRVDesc.z;

    if (g_bPBR && (fRoughness > 0.f || fMetalness > 0.f) && (g_bShaderHalf ? In.vTexcoord.x > g_fShaderHalf_X : true))
    {
        vector vLook = normalize(g_vCamPosition - vPosition);
        vLook.a = 0.f;
		
        fRoughness = max(fRoughness, 0.001f);
        fMetalness = max(fMetalness, 0.001f);
        fOcclusion = max(fOcclusion, 0.1f);
    
        vector vHalfVec = normalize(vLook + normalize(-vLightDir) * -1.f);

        float NdotL = saturate(dot(vNormal, normalize(-vLightDir) * -1.f));
        float NdotH = saturate(dot(vNormal, vHalfVec));
        float NdotV = saturate(dot(vNormal, vLook));
        float HdotV = saturate(dot(vHalfVec, vLook));
    
        float3 vMetalic = lerp(float3(0.04f, 0.04f, 0.04f), vDiffuse.xyz, fMetalness);

        float NDF = trowbridgeReitzNDF(NdotH, fRoughness);
        float3 F = fresnel(vMetalic, HdotV, fRoughness);
        float G = schlickBeckmannGAF(NdotV, fRoughness) * schlickBeckmannGAF(NdotL, fRoughness);

        vector kS = vector(F, 0.f);
        vector kD = vector(1.f, 1.f, 1.f, 0.f) - kS;
        kD *= 1.f - fMetalness;

        vector vNumerator = kS * NDF * G;
        float fDenominator = 4.f * NdotV * NdotL;
        vector vSpecular = vNumerator / max(fDenominator, 0.001f);

        vector vSpecularAcc = vSpecular * NdotL * g_vLightDiffuse * fAtt /** fOcclusion*/;
        vector vAmbientColor = kD * vDiffuse * fOcclusion * NdotL * g_vLightDiffuse * fAtt;
		
        Out.vSpecular = vSpecularAcc * g_vLightSpecular;
        Out.vSpecular.a = 0.f;

        Out.vAmbient = vAmbientColor;
        Out.vAmbient.a = 1.f;
    }
    else
    {        	
        vector vReflect = reflect(normalize(vLightDir), vNormal);
        vector vLook = vPosition - g_vCamPosition;
    
        Out.vSpecular = (g_vLightSpecular) * (g_vMtrlSpecular) * pow(max(dot(normalize(vReflect) * -1.f, normalize(vLook)), 0.0f), 10.f) * fAtt;
        Out.vSpecular.a = min(Out.vSpecular.a, 1.f);
    }
  
    Out.vShade = (lightIntensity + emissive) * fAtt;
    Out.vShade.a = min(Out.vShade.a, 1.f);
	
    if (vShaderTag.a > 0.f)
    {
        Out.vShade = float4(0.f,0.f,0.f,0.f);
        Out.vAmbient = float4(0.f, 0.f, 0.f, 0.f);
        Out.vSpecular = float4(0.f, 0.f, 0.f, 0.f);
    }	
	
    if (g_bUseVolumetric == true)
    Out.vVolumetric = VolumetricLighting_Spot(In);

    return Out;
}


// 첫 번째 스포트라이트
float4 CalculateSpotlight(float4 vPosition, float4 vNormal)
{
    float4 vLightDir1 = (vPosition - g_vLightPos) * g_fSpotPower;
    float fDistance1 = length(vLightDir1);
    
    float cosInnerCone1 = cos(g_fLightThetaPi.x);
    float cosOuterCone1 = cos(g_fLightThetaPi.y);
    float NowSpotCos1 = dot(normalize(vLightDir1), normalize(g_vLightDir));
    float SpotFactor1 = saturate((NowSpotCos1 - cosOuterCone1) / (cosInnerCone1 - cosOuterCone1));
    
    float fAtt1 = SpotFactor1 / fDistance1;
        
    float4 lightIntensity = g_vLightDiffuse * saturate(max(dot(normalize(vLightDir1) * -1.f, vNormal), 0.f) + (g_vLightAmbient * g_vMtrlAmbient));
    float4 emissive = g_vLightEmissive * SpotFactor1;
	
    float4 Shade1 = lightIntensity * fAtt1 + emissive;
    Shade1.a = min(Shade1.a, 1.f);	
	
    float4 vReflect1 = reflect(normalize(vLightDir1), vNormal);
    float4 vLook1 = vPosition - g_vCamPosition;
    float4 specular1 = (g_vLightSpecular) * (g_vMtrlSpecular) * pow(max(dot(normalize(vReflect1) * -1.f, normalize(vLook1)), 0.0f), 10.f) * fAtt1;
    
    return Shade1 + specular1;
}

// 두 번째 스포트라이트
float4 CalculateSpotlight2(float4 vPosition, float4 vNormal)
{
    float4 vLightDir2 = (vPosition - g_vLightPos) * g_fSpotPower;
    float fDistance2 = length(vLightDir2);
    
    float cosInnerCone2 = cos(g_fLightThetaPi.x);
    float cosOuterCone2 = cos(g_fLightThetaPi.y);
    float NowSpotCos2 = dot(normalize(vLightDir2), normalize(-g_vLightDir)); //대칭을 위한 반대!
    float SpotFactor2 = saturate((NowSpotCos2 - cosOuterCone2) / (cosInnerCone2 - cosOuterCone2));
    
    float fAtt2 = SpotFactor2 / fDistance2;
        
    float4 lightIntensity = g_vLightDiffuse * saturate(max(dot(normalize(vLightDir2) * -1.f, vNormal), 0.f) + (g_vLightAmbient * g_vMtrlAmbient));
    float4 emissive = g_vLightEmissive * SpotFactor2;
	
    float4 Shade2 = lightIntensity * fAtt2 + emissive;
    Shade2.a = min(Shade2.a, 1.f);
	
    float4 vReflect2 = reflect(normalize(vLightDir2), vNormal);
    float4 vLook2 = vPosition - g_vCamPosition;
    float4 specular2 = (g_vLightSpecular) * (g_vMtrlSpecular) * pow(max(dot(normalize(vReflect2) * -1.f, normalize(vLook2)), 0.0f), 10.f) * fAtt2;
    
    return Shade2 + specular2;
}

// 두 스포트라이트를 합치는 함수
float4 CombineSpotlights(float4 vPosition, float4 vNormal)
{
    float4 shade1 = CalculateSpotlight(vPosition, vNormal);
    float4 shade2 = CalculateSpotlight2(vPosition, vNormal);
    
    return (shade1 + shade2); // 합쳐지는 조명이 아니므로 평균 X
}

PS_OUT_LIGHT PS_MAIN_LightSymmetry(PS_IN In)
{
    PS_OUT_LIGHT Out = (PS_OUT_LIGHT) 0;

    vector vNormalDesc = g_NormalTexture.Sample(PointSampler, In.vTexcoord);
    vector vNormal = vector(vNormalDesc.xyz * 2.f - 1.f, 0.f);

    vector vDepthDesc = g_DepthTexture.Sample(PointSampler, In.vTexcoord);
    float4 vShaderTag = g_ShaderTagTexture.Sample(PointSampler, In.vTexcoord);
	
	
	float fViewZ = vDepthDesc.y * 1000.f; //Far
    
    vector vPosition;
    float4 vLightDir = vPosition - g_vLightPos;
	
    vPosition.x = In.vTexcoord.x * 2.f - 1.f;
    vPosition.y = In.vTexcoord.y * -2.f + 1.f;
    vPosition.z = vDepthDesc.x;
    vPosition.w = 1.f;
	
    vPosition = vPosition * fViewZ;
    vPosition = mul(vPosition, g_ProjMatrixInv);
    vPosition = mul(vPosition, g_ViewMatrixInv);
		
    Out.vShade = CombineSpotlights(vPosition, vNormal);
		
    return Out;
}

//---------------- 조명 끝 ----------------//

PS_OUT PS_MAIN_DEFERRED(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

    float4      vDiffuse = pow(g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord), 2.2f);
    float4		vNormalDesc = g_NormalTexture.Sample(LinearSampler, In.vTexcoord);
    float4		vShade = g_ShadeTexture.Sample(LinearSampler, In.vTexcoord);
    float4		vSpecular = g_SpecularTexture.Sample(LinearSampler, In.vTexcoord);
    float4		vHBAO = g_HBAOTexture.Sample(LinearSampler, In.vTexcoord);
    float4		vMRVDesc = g_PBRTexture.Sample(LinearSampler, In.vTexcoord);
    float4		vAmbient = g_AmbientTexture.Sample(LinearSampler, In.vTexcoord);
    float4      vShaderTag = g_ShaderTagTexture.Sample(LinearSampler, In.vTexcoord);
    float4      vShadow = g_ShadowTexture.Sample(LinearSampler, In.vTexcoord);
    
	vector vDepth = g_DepthTexture.Sample(PointSampler, In.vTexcoord);
	float fViewZ = vDepth.y * 1000.f;

	/*Shadow*/
	vector		vWorldPos;

	/* 투영스페이스 상의 위치. */
	/* 로컬위치 * 월드행렬 * 뷰행렬* 투영행렬 / View.z */
	vWorldPos.x = In.vTexcoord.x * 2.f - 1.f;
	vWorldPos.y = In.vTexcoord.y * -2.f + 1.f;
	vWorldPos.z = vDepth.x;
	vWorldPos.w = 1.f;

	/* 뷰스페이스 상의 위치를 구하자. */
	/* 로컬위치 * 월드행렬 * 뷰행렬 */
	vWorldPos = vWorldPos * fViewZ;
	vWorldPos = mul(vWorldPos, g_ProjMatrixInv);
    float4 vViewPos = vWorldPos;
	/* 월드 상의 위치를 구하자. */
	/* 로컬위치 * 월드행렬 */
	vWorldPos = mul(vWorldPos, g_ViewMatrixInv);

	//vWorldPos = mul(vWorldPos, g_LightViewMatrix);
	//vWorldPos = mul(vWorldPos, g_LightProjMatrix);

	float2		vUV = (float2)0.0f;

	vUV.x = (vWorldPos.x / vWorldPos.w) * 0.5f + 0.5f;
	vUV.y = (vWorldPos.y / vWorldPos.w) * -0.5f + 0.5f;

    //Out.vColor = vDiffuse * vShade + vSpecular;
	
    if (g_bPBR && (vMRVDesc.r > 0.f || vMRVDesc.g > 0.f) && (g_bShaderHalf ? In.vTexcoord.x > g_fShaderHalf_X : true))
    {
        vector vNormal = vector(vNormalDesc.xyz * 2.f - 1.f, 0.f);
        vNormal = normalize(vNormal);
        vector vLook = normalize(g_vCamPosition - vWorldPos);
    
        float NdotV = saturate(dot(vNormal, vLook));
    
        float fRoughness = max(0.01f, vMRVDesc.y);
        float fMetalness = max(0.01f, vMRVDesc.x);
        float fOcclusion = max(0.1f, vMRVDesc.z);
     
        vector vReflect = normalize(reflect(-normalize(vLook), vNormal));
        
        float4 brdfTerm = g_BRDFTexture.Sample(ClampSampler, float2(NdotV, 1.0 - fRoughness));
        float3 metalSpecularIBL = { 1.f, 1.f, 1.f }; // g_PreFilteredTexture.SampleLevel(ClampSampler, vReflect.xyz, (fRoughness * 10.f)).rgb;

        float3 dielectricColor = float3(0.04, 0.04, 0.04);
        float3 diffColor = vDiffuse.rgb * (1.0 - fMetalness);
        float3 specColor = lerp(dielectricColor.rgb, vDiffuse.rgb, fMetalness);
   
        vector diffuseIBL = { 0.5f, 0.5f, 0.5f, 0.5f }; //g_IrradianceTexture.Sample(ClampSampler, vNormal) / 3.14159;
        diffuseIBL.rgb = lerp(diffuseIBL.rgb * 0.3f, diffuseIBL.rgb, fOcclusion);
           
        float3 albedoByDiffuse = diffColor.rgb * diffuseIBL.rgb;

        float4 litColor;
        litColor.rgb = (albedoByDiffuse.rgb + (metalSpecularIBL * (specColor * brdfTerm.x + (brdfTerm.y)))) * fOcclusion;
 
        //litColor = pow(litColor, 2.2f);
      
        Out.vColor.rgb = vAmbient.rgb + vSpecular.rgb + (litColor.rgb * float3(1.f, 1.f, 1.f)) * vShade.rgb;
        Out.vColor.rgb *= vHBAO.rgb;
           
        Out.vColor.a = 1.f;
    }
    else
    {
        Out.vColor = vDiffuse * vShade + vSpecular;
        //Out.vColor = vDiffuse * vAmbient + vSpecular /** (1.f - bIsInShadow)*/;
        //Out.vColor.rgb *= vShade.rgb;
        if (g_bShaderHalf ? In.vTexcoord.x > g_fShaderHalf_X : true)
            Out.vColor.rgb *= vHBAO.rgb;
    }
	
    if (vShaderTag.x == 0.f)    
        Out.vColor.rgb *= vShadow.rgb;
    
	/* Fog */
    if (g_bFog && (g_bShaderHalf ? In.vTexcoord.x > g_fShaderHalf_X : true))
    {

        float fog;
        if(g_bFogHeight)
            fog = ExponentialFog(vWorldPos, vViewPos, g_vCamPosition, g_fFogStart, g_fFogDensity);
       else
            fog = ExponentialHeightFog(vWorldPos, vViewPos, g_vCamPosition, g_fFogStart, g_fFogDensity,g_fFogFalloff);
   
        //if (vShaderTag.a > 0.f)
        //    Out.vColor.rgb = lerp(Out.vColor.rgb, g_vFogColor.rgb, fog * g_fFogSkyStrength);
        //else
        //   
        if (vShaderTag.a > 0.f)
            fog *= g_fFogSkyStrength;

        //Out.vColor.rgb = lerp(Out.vColor.rgb, g_vFogColor.rgb, fog);
        float3 vFogColor  = g_vFogColor.rgb * fog;
        Out.vColor.rgb = vFogColor.rgb + Out.vColor.rgb - mul(vFogColor.rgb, Out.vColor.rgb);
        
    }
    
    float3 mapped = 1.f - exp(-Out.vColor.rgb * g_fExposure);
    Out.vColor.rgb = pow(mapped, 1.f / 2.2f);
	
	return Out;
}

PS_OUT PS_MAIN_BLENDBLOOM(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;
	
    vector vBloom = g_BloomTexture.Sample(ClampSampler, In.vTexcoord);
    vector vLuminanceBloom = g_LuminanceBloomTexture.Sample(ClampSampler, In.vTexcoord);
    vector vOriginal = g_OriginalTexture.Sample(ClampSampler, In.vTexcoord) + g_ModelEffectTexture.Sample(ClampSampler, In.vTexcoord);
    vector vSolid = g_SolidEffectTexture.Sample(ClampSampler, In.vTexcoord); // 추가
    
	//Out.vColor.a = vOriginal.a +(1 - vOriginal.a) * vBloom.a;
	//Out.vColor.rgb = vOriginal.rgb + vBloom.rgb;
    //Out.vColor = vBloom * vBloom.a + vOriginal * (1.f - vBloom.a);
	
	if (g_bHDRBloom)
	{
		if (vLuminanceBloom.r > 0.f || vLuminanceBloom.g > 0.f || vLuminanceBloom.b > 0.f)
		{
            vBloom = vLuminanceBloom * vLuminanceBloom.a + vBloom * (1.f - vLuminanceBloom.a);
    
			//Out.vColor.rgb += vLuminanceBloom.rgb;
			//Out.vColor.a = Out.vColor.a + (1 - Out.vColor.a) * vLuminanceBloom.a;
		}
	
		//if (Out.vColor.a <= 0.f)
		//{
		//	if (g_bHDRBloom && vLuminanceBloom.r > 0.f)
		//		Out.vColor.a = vLuminanceBloom.a;
		//	else
		//		discard;
		//}
	}
    //Out.vColor = vSolid; // 추가
    Out.vColor = vOriginal + vBloom;
    //Out.vColor = saturate(Out.vColor.a - vSolid.a);
    //float fAlpha = Out.vColor.a;
    
    //Out.vColor = lerp(Out.vColor, vSolid, fAlpha);
    //Out.vColor.rgb += vBloom.rgb * vBloom.a;
    //Out.vColor = vSolid;
    //Out.vColor += vOriginal + vBloom;
	//Out.vColor = pow(Out.vColor, 1.f / 2.2f);

	return Out;
}

struct PS_OUT_RESULT
{
    float4 vColor : SV_TARGET0;
    float4 vCopyForMotionBlur : SV_TARGET1;
};


PS_OUT_RESULT PS_MAIN_RESULT(PS_IN In)
{
    PS_OUT_RESULT Out = (PS_OUT_RESULT) 0;

    float4 vDistortionDesc = g_DistortionTexture.Sample(ClampSampler, In.vTexcoord);
    
    float fDistortionX = (vDistortionDesc.r - 0.5f) * vDistortionDesc.a * 0.1f; // 추가
    float fDistortionY = (vDistortionDesc.r) * vDistortionDesc.a * 0.1f; // 추가
    float fRadialBlur = g_RadialBlurTexture.Sample(ClampSampler, In.vTexcoord).r * 0.1f;
	
    
    float4 vEffect = g_EffectTexture.Sample(ClampSampler, In.vTexcoord + float2(fDistortionX, fDistortionY));
    float4 vEmissive = g_EmissiveTexture.Sample(ClampSampler, In.vTexcoord + float2(fDistortionX, fDistortionY));
    float4 vShaderTag = g_ShaderTagTexture.Sample(ClampSampler, In.vTexcoord + float2(fDistortionX, fDistortionY));
    float4 vVolumetric = g_VolumetricTexture.Sample(ClampSampler, In.vTexcoord + float2(fDistortionX, fDistortionY));
   
    //vector vSolid = g_SolidEffectTexture.Sample(ClampSampler, In.vTexcoord); // 추가
       
    if ((g_bShaderHalf ? In.vTexcoord.x > g_fShaderHalf_X : true))
    {
        Out.vColor = pow(g_DiffuseTexture.Sample(ClampSampler, In.vTexcoord + float2(fDistortionX, fDistortionY)), 2.2f);
   
	    if(g_bToneMapping && vShaderTag.a <= 0.f)
            Out.vColor.rgb = CommerceToneMapping(Out.vColor.rgb);

	    Out.vColor = pow(Out.vColor, 1 / 2.2f);

	    Out.vColor.rgb = AdjustHSV(Out.vColor.rgb, g_vHSV.x, g_vHSV.y, g_vHSV.z);
	
	    Out.vColor = saturate(Out.vColor);

        Out.vColor += vEffect + vEmissive + vVolumetric;

        Out.vCopyForMotionBlur = Out.vColor;            
    }
    else
        Out.vColor = (g_DiffuseTexture.Sample(ClampSampler, In.vTexcoord + float2(fDistortionX, fDistortionY)));
   
    
	return Out;
}


PS_OUT PS_MAIN_RESULT_MOTIONBLUR(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	vector vColor = vector(0.f, 0.f, 0.f, 0.f);
    
    float2 iResolution = float2(1.f, 1.f);
    float2 uv = In.vTexcoord.xy / iResolution.xy * float2(1.0, 1.0);
    
    float2 dir = (In.vTexcoord.xy - g_vMotionBlurCenter.xy) / iResolution.xy * float2(1.0, 1.0);
    
    for (int i = 0; i < g_fMotionBlurAmount; i += 2) //operating at 2 samples for better performance
    {
        vColor += g_DiffuseTexture.Sample(ClampSampler, uv + float(i) / float(g_fMotionBlurAmount) * dir * g_fMotionBlurWidth);
        vColor += g_DiffuseTexture.Sample(ClampSampler, uv + float(i + 1) / float(g_fMotionBlurAmount) * dir * g_fMotionBlurWidth);
    }
    
    vColor = vColor / float(g_fMotionBlurAmount);
    
    
	Out.vColor = vColor;
    
    
	return Out;
}

PS_OUT PS_MAIN_RESULT_RADIALBLUR(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    vector vColor = vector(0.f, 0.f, 0.f, 0.f);

    float fBlurStart = 1.f;
    //float2 center = float2(0.5f, 0.5f); //중심점<-마우스의 위치를 받아오면 마우스를 중심으로 블러됨

    //In.vTexcoord.xy -= center;
    
    matrix matVP = mul(g_ViewMatrix, g_ProjMatrix);
    float4 vBlurCenter = mul(float4(g_vRadialBlurWorldPosition, 1.f), matVP);
    vBlurCenter /= vBlurCenter.w;

    vBlurCenter.x = vBlurCenter.x * 0.5f + 0.5f;
    vBlurCenter.y = vBlurCenter.y * -0.5f + 0.5f;
	
    float2 center = float2(vBlurCenter.x, vBlurCenter.y); //중심점<-마우스의 위치를 받아오면 마우스를 중심으로 블러됨

    //float g_fBlurWidth = 0.5f;
    float g_fBlurWidth = g_fRadialBlurIntensity;
    float fPrecompute = g_fBlurWidth * (1.0f / 19.f);

    for (uint i = 0; i < 10; ++i)
    {
        float scale = fBlurStart + (float(i) * fPrecompute);
        float2 uv = In.vTexcoord.xy * scale + center;

        vColor += g_DiffuseTexture.Sample(PointSampler, uv);
    }
    vColor /= 20.f;
    Out.vColor = vColor;

    Out.vColor.rgb = AdjustHSV(Out.vColor.rgb, g_vHSV.x, g_vHSV.y, g_vHSV.z);

    return Out;
} // 추가

//
//float4 Blur_X(float2 vTexCoord)
//{
//	float4		vOut = (float4)0;
//
//	float2		vUV = (float2)0;
//
//	for (int i = -6; i < 7; ++i)
//	{
//		vUV = vTexCoord + float2(1.f / g_fTexW * i, 0);
//		vOut += fWeight[6 + i] * g_EffectTexture.Sample(ClampSampler, vUV);
//	}
//
//	vOut /= fTotal;
//
//	return vOut;
//}
//
//float4 Blur_Y(float2 vTexCoord)
//{
//	float4		vOut = (float4)0;
//
//	float2		vUV = (float2)0;
//
//	for (int i = -6; i < 7; ++i)
//	{
//		vUV = vTexCoord + float2(0, 1.f / (g_fTexH / 2.f) * i);
//		vOut += fWeight[6 + i] * g_EffectTexture.Sample(ClampSampler, vUV);
//	}
//
//	vOut /= fTotal;
//
//	return vOut;
//}
//
//PS_OUT PS_MAIN_BLUR_X(PS_IN In)
//{
//	PS_OUT		Out = (PS_OUT)0;
//
//	Out.vColor = Blur_X(In.vTexcoord);
//
//	return Out;
//}
//
//PS_OUT PS_MAIN_BLUR_Y(PS_IN In)
//{
//	PS_OUT		Out = (PS_OUT)0;
//
//	Out.vColor = Blur_Y(In.vTexcoord);
//
//	return Out;
//}


PS_OUT PS_MAIN_RIMLIGHT(PS_IN In)
{
	PS_OUT Out = (PS_OUT)0;
	vector vDepthDesc = g_DepthTexture.Sample(PointSampler, In.vTexcoord);
	vector vNormal = g_NormalTexture.Sample(PointSampler, In.vTexcoord) * 2.f - 1.f;
	vNormal = normalize(vNormal);

	if (vDepthDesc.x == 1.f && vDepthDesc.y == 1.f && vDepthDesc.z == 1.f && vDepthDesc.w == 1.f)
		discard;

	float fViewZ = vDepthDesc.y * 1000.f;
	/* 0 -> -1, 1 -> 1*/

	vector vWorldPos;

	vWorldPos.x = In.vTexcoord.x * 2.f - 1.f;
	vWorldPos.y = In.vTexcoord.y * -2.f + 1.f;
	vWorldPos.z = vDepthDesc.x;
	vWorldPos.w = 1.0f;

	vWorldPos *= fViewZ;

	vWorldPos = mul(vWorldPos, g_ProjMatrixInv);

	vWorldPos = mul(vWorldPos, g_ViewMatrixInv);

	vector vView = normalize(g_vCamPosition - vWorldPos);
	half fRim = 1.f - saturate(dot(vNormal, vView));

	if (fRim >= 0.99f)
		discard;

	vector vRimColor = { 0.f, 1.f, 0.408f, 1.f };
	vRimColor.xyz = lerp(vRimColor.xyz * 0.03f, vRimColor.xyz, fRim * fRim);

	half fRimPower = vRimColor.a;

	Out.vColor = vector(vRimColor.xyz, 1.f) * fRim * fRimPower;

	return Out;
}


static const int SSR_MAX_STEPS = 16;
static const int SSR_BINARY_SEARCH_STEPS = 16;

float4 SSRBinarySearch(float3 dir, inout float3 hitCoord)
{
    float depth;
    for (int i = 0; i < SSR_BINARY_SEARCH_STEPS; i++)
    {
        float4 projectedCoord = mul(float4(hitCoord, 1.0f), g_ProjMatrix);
        projectedCoord.xy /= projectedCoord.w;
        projectedCoord.xy = projectedCoord.xy * float2(0.5f, -0.5f) + float2(0.5f, 0.5f);

        float3 viewSpacePosition = GetViewSpacePosition(g_DepthTexture, projectedCoord.xy, g_ProjMatrixInv);
        float depthDifference = hitCoord.z - viewSpacePosition.z;

        if (depthDifference <= 0.0f)
            hitCoord += dir;

        dir *= 0.5f;
        hitCoord -= dir;
    }

    float4 projectedCoord = mul(float4(hitCoord, 1.0f), g_ProjMatrix);
    projectedCoord.xy /= projectedCoord.w;
    projectedCoord.xy = projectedCoord.xy * float2(0.5f, -0.5f) + float2(0.5f, 0.5f);
    
    float3 viewSpacePosition = GetViewSpacePosition(g_DepthTexture, projectedCoord.xy, g_ProjMatrixInv);
    float depthDifference = hitCoord.z - viewSpacePosition.z;

    return float4(projectedCoord.xy, depth, abs(depthDifference) < 2.0f ? 1.0f : 0.0f);
}

float4 SSRRayMarch(float3 dir, inout float3 hitCoord)
{
    float depth;
    for (int i = 0; i < SSR_MAX_STEPS; i++)
    {
        hitCoord += dir;
        float4 projectedCoord = mul(float4(hitCoord, 1.0f), g_ProjMatrix);
        projectedCoord.xy /= projectedCoord.w;
        projectedCoord.xy = projectedCoord.xy * float2(0.5f, -0.5f) + float2(0.5f, 0.5f);
        
        float3 viewSpacePosition = GetViewSpacePosition(g_DepthTexture, projectedCoord.xy, g_ProjMatrixInv);
        float depthDifference = hitCoord.z - viewSpacePosition.z;

		[branch]
        if (depthDifference > 0.0f)
        {
            return SSRBinarySearch(dir, hitCoord);
        }

        dir *= g_fSSRStep;
    }
    return float4(0.0f, 0.0f, 0.0f, 0.0f);
}


bool IsInsideScreen(float2 vCoord)
{
    return !(vCoord.x < 0 || vCoord.x > 1 || vCoord.y < 0 || vCoord.y > 1);
}

PS_OUT PS_MAIN_SSR(PS_IN In)
{
	PS_OUT Out = (PS_OUT)0;

	vector vDepth = g_DepthTexture.Sample(PointSampler, In.vTexcoord);

	float fViewZ = vDepth.y * 1000.f;

	vector vWorldPos;

	vWorldPos.x = In.vTexcoord.x * 2.f - 1.f;
	vWorldPos.y = In.vTexcoord.y * -2.f + 1.f;
	vWorldPos.z = vDepth.x;
	vWorldPos.w = 1.0f;

	vWorldPos *= fViewZ;

	vWorldPos = mul(vWorldPos, g_ProjMatrixInv);

	vWorldPos = mul(vWorldPos, g_ViewMatrixInv);

    float4 normalMetallic = g_NormalTexture.Sample(BorderSampler, In.vTexcoord);
    float4 sceneColor = g_ResultTexture.SampleLevel(ClampSampler, In.vTexcoord, 0);
    
    float metallic = 1.f;
   
    if (metallic < 0.01f)
    {        
       Out.vColor = sceneColor;
       return Out;
    }
    
    float3 normal = normalMetallic.rgb;
    normal = 2 * normal - 1.0;
    normal = normalize(normal);

    float3 viewSpacePosition = GetViewSpacePosition(g_DepthTexture, In.vTexcoord, g_ProjMatrixInv);
    float3 reflectDirection = normalize(reflect(viewSpacePosition, normal));

    float3 hitPosition = viewSpacePosition;
    float4 coords = SSRRayMarch(reflectDirection, hitPosition);

    float2 coordsEdgeFactor = float2(1, 1) - pow(saturate(abs(coords.xy - float2(0.5f, 0.5f)) * 2), 8);
    float screenEdgeFactor = saturate(min(coordsEdgeFactor.x, coordsEdgeFactor.y));
    float reflectionIntensity = saturate(screenEdgeFactor * saturate(reflectDirection.z) * (coords.w));

    float3 reflectionColor = reflectionIntensity * g_ResultTexture.SampleLevel(ClampSampler, coords.xy, 0).rgb;
    Out.vColor = sceneColor + metallic * max(0, float4(reflectionColor, 1.0f));
   
    //vector vViewDir = normalize(vWorldPos - g_vCamPosition);
	//vViewDir.w = 0.f;

	//vector vRayOrigin = vWorldPos;
	//vector vRayDir = normalize(reflect(vViewDir, vNormal));
	//vRayDir.w = 0.f;

	//float fStep = g_fSSRStep;

	//matrix matVP = mul(g_ViewMatrix, g_ProjMatrix);

	//float fPixelDepth = 0.f;
	//int iStepDistance = 0;
	//float2 vRayPixelPos = (float2)0;

	//for (iStepDistance = 1; iStepDistance < 50; ++iStepDistance)
	//{
	//	vector vDirStep = vRayDir * fStep * iStepDistance;
	//	vDirStep.w = 0.f;
	//	vector vRayWorldPos = vRayOrigin + vDirStep;

	//	vector vRayProjPos = mul(vRayWorldPos, matVP);
	//	vRayProjPos.x = vRayProjPos.x / vRayProjPos.w;
	//	vRayProjPos.y = vRayProjPos.y / vRayProjPos.w;

	//	vRayPixelPos = float2(vRayProjPos.x * 0.5f + 0.5f, vRayProjPos.y * -0.5f + 0.5f);

	//	clip(vRayPixelPos);
	//	clip(1.f - vRayPixelPos);

	//	vector vPixelDepth = g_DepthTexture.Sample(PointSampler, vRayPixelPos);

	//	fPixelDepth = vPixelDepth.x;
	//	fPixelDepth *= vPixelDepth.y * 1000.f;

	//	float fDiff = vRayProjPos.z - fPixelDepth;

	//	if (fDiff > 0.f && fDiff < 0.01f)
	//		break;
	//}

	//clip(49.5f - iStepDistance);

	//Out.vColor = g_ResultTexture.Sample(PointSampler, vRayPixelPos) * (1.f - iStepDistance / 50.f);

	return Out;
}


PS_OUT PS_MAIN_COPY_ORIGIN(PS_IN In)
{
	PS_OUT Out = (PS_OUT)0;
    Out.vColor = g_OriginalRenderTexture.Sample(LinearSampler, In.vTexcoord);
	Out.vColor.a = 1.f;
	return Out;
}


PS_OUT PS_FXAA(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    Out.vColor = float4(FxaaPixelShader(g_OriginalRenderTexture, In.vTexcoord), 1.0f);
    
    return Out;
}


technique11 DefaultTechnique
{
	pass Debug //0
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_None, 0);
		SetBlendState(BS_Default, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_DEBUG();
	}

	pass Light_Directional //1
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_None, 0);
		SetBlendState(BS_Blend_Add, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_DIRECTIONAL();
	}

	pass Light_Point //2
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_None, 0);
		SetBlendState(BS_Blend_Add, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_POINT();
	}

	pass Deferred //3
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_None, 0);
		SetBlendState(BS_Default, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_DEFERRED();
	}


	pass Blur_X //4
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_None, 0);
		SetBlendState(BS_Default, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_DEFERRED();
	}
	pass Blur_Y //5
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_None, 0);
		SetBlendState(BS_Default, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_DEFERRED();
	}

	pass RESULT //6
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_None, 0);
		SetBlendState(BS_Default, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_RESULT();
	}

	pass RESULT_MOTIONBLUR //7
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_None, 0);
		SetBlendState(BS_Default, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_RESULT_MOTIONBLUR();
	}

	pass RIMLIGHT //8
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_None, 0);
		SetBlendState(BS_AlphaBlend_Add, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_RIMLIGHT();
	}

	pass SSR //9
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_DepthFalse_StencilEnable, 2);
		SetBlendState(BS_AlphaBlend_Add, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_SSR();
	}

	pass BLENDBLOOM //10
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_None, 0);
		SetBlendState(BS_AlphaBlend_Add, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_BLENDBLOOM();
	}

	pass Copy_OriginalRenderTexture //11
	{
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		SetDepthStencilState(DSS_None, 0);
		SetRasterizerState(RS_Default);
		VertexShader = compile vs_5_0 VS_MAIN();
		HullShader = NULL;
		DomainShader = NULL;
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_COPY_ORIGIN();
	}

    pass Light_Spot //12
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Blend_Add, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_SPOT();
    }

    pass Light_Symmetry //13
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Blend_Add, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_LightSymmetry();
    }


    pass FXAA //14
    {
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
        SetDepthStencilState(DSS_None, 0);
        SetRasterizerState(RS_Default);
        VertexShader = compile vs_5_0 VS_MAIN();
        HullShader = NULL;
        DomainShader = NULL;
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_FXAA();
    }

    pass RESULT_RADIALBLUR //15
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Default, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_RESULT_RADIALBLUR();
    } // 추가
}
