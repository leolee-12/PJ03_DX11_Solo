// 두 트랜스폼 행렬을 받아 

cbuffer CB : register(b0)
{
    float g_fBlend; // 블렌딩 가중치
    float3 g_fPadding;
};

// 뼈 행렬 구조체
// 64 BYTE
struct BonesBuffer
{
    float3 vPosition;
    float4 vRotation;
    float3 vScale;
};


// 블렌딩할 애니메이션 두개를 받는다.
StructuredBuffer<BonesBuffer> InputBlend0 : register(t0);
StructuredBuffer<BonesBuffer> InputBlend1 : register(t1);
// 결과 애니메이션
RWStructuredBuffer<BonesBuffer> Output : register(u0);


float4 slerp(float4 q1, float4 q2, float t)
{
    // 쿼터니언 각도 구하기
    float dotProduct = dot(q1, q2);

    // 큐터니언 사이의 각도가 180도 이상이면, 하나의 큐터니언을 반전
    if (dotProduct < 0.0f)
    {
        q2 = -q2;
        dotProduct = -dotProduct;
    }

    // t로 보간
    float scale0 = 1.0f - t;
    float scale1 = t;

    // 쿼터니언 보간
    if ((1.0f - dotProduct) > 0.001f)
    {
        float angle = acos(dotProduct);
        float invSinTheta = 1.0f / sin(angle);
        scale0 = sin((1.0f - t) * angle) * invSinTheta;
        scale1 = sin(t * angle) * invSinTheta;
    }

    // 보간된 큐터니언을 반환합니다.
    return scale0 * q1 + scale1 * q2;
}


[numthreads(1024, 1, 1)]
void main(uint3 Gid : SV_GroupID, uint3 DTid : SV_DispatchThreadID,
	uint3 GTid : SV_GroupThreadID, uint GI : SV_GroupIndex)
{
    Output[GI].vPosition = lerp(InputBlend0[GI].vPosition, InputBlend1[GI].vPosition, g_fBlend);
    GroupMemoryBarrierWithGroupSync();
    Output[GI].vRotation = slerp(InputBlend0[GI].vRotation, InputBlend1[GI].vRotation, g_fBlend);
    GroupMemoryBarrierWithGroupSync();
    Output[GI].vScale = lerp(InputBlend0[GI].vScale, InputBlend1[GI].vScale, g_fBlend);
    GroupMemoryBarrierWithGroupSync();
}