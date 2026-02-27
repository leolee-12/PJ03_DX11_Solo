// 행렬을 합치는 계산 셰이더

cbuffer CB : register(b0)
{
    uint g_iCount;
};

// 뼈 행렬 구조체
// 64 BYTE
struct BonesBuffer
{
    float4x4 Transform;
};

struct BoneParent
{
    int iParentID;
};


groupshared int iCurIndex = 0;

// 블렌딩할 애니메이션 두개를 받는다.
StructuredBuffer<BonesBuffer> InputTransform : register(t0);
StructuredBuffer<BoneParent> InputHierarchy : register(t1); // 'InputHierarchy'로 수정
// 결과 애니메이션
RWStructuredBuffer<BonesBuffer> Output : register(u0);

[numthreads(1024, 1, 1)]
void main(uint3 Gid : SV_GroupID, uint3 DTid : SV_DispatchThreadID,
	uint3 GTid : SV_GroupThreadID, uint GI : SV_GroupIndex)
{
    // 스레드 그룹 내에서 각 스레드가 자신의 인덱스를 계산하도록 함
    uint iIndex = Gid.x * 64 + GTid.x;

    // 현재 스레드의 부모 ID 가져오기
    int iParentID = InputHierarchy[iIndex].iParentID;

    // 부모가 계산되었는지 확인하고 계산되지 않았다면 대기
    while (iParentID >= iCurIndex)
    {
        // 부모 ID 다시 확인
        iParentID = InputHierarchy[iIndex].iParentID;
    }
    // 스레드가 대기하는 동안 다른 스레드에게 제어를 양보
    GroupMemoryBarrierWithGroupSync();

    // 현재 스레드의 변환 행렬 가져오기
    float4x4 currentTransform = InputTransform[iIndex].Transform;

    // 부모의 변환 행렬과 현재 스레드의 변환 행렬을 곱함
    float4x4 resultTransform = mul(currentTransform, Output[iParentID].Transform);

    // 결과 행렬을 출력 버퍼에 저장
    Output[iIndex].Transform = resultTransform;
}
