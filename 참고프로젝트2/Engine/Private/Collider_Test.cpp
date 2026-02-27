#include "Collider_Test.h"
#include "GameInstance.h"
#include "DebugDraw.h"


#define COLOR_GREEN  { 0.f,1.f,0.f,1.f }
#define COLOR_BLUE  { 0.f,0.f,1.f,1.f }
#define COLOR_RED  { 1.f,0.f,0.f,1.f }

/*
CCollider_Test::CCollider_Test(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CVIBuffer{ pDevice,  pContext }
{
}

CCollider_Test::CCollider_Test(const CCollider_Test& rhs)
	:CVIBuffer{rhs}
{
}

HRESULT CCollider_Test::Initialize_Prototype()
{

	return S_OK;
}
*/

CCollider_Test::CCollider_Test(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CComponent{ pDevice,  pContext }
{
}

CCollider_Test::CCollider_Test(const CCollider_Test& Prototype)
	:CComponent{ Prototype }
#ifdef _DEBUG
	, m_pBatch{ Prototype.m_pBatch }
	, m_pEffect{ Prototype.m_pEffect }
	, m_pInputLayout{ Prototype.m_pInputLayout }
#endif
{

#ifdef _DEBUG
	Safe_AddRef(m_pInputLayout);
#endif
}

HRESULT CCollider_Test::Initialize_Prototype()
{

#ifdef _DEBUG
	m_pBatch = new PrimitiveBatch<VertexPositionColor>(m_pContext);
	m_pEffect = new BasicEffect(m_pDevice);

	m_pEffect->SetVertexColorEnabled(true);

	const void* pShaderByteCode = { nullptr };
	size_t		iShaderByteCodeLength = {};

	m_pEffect->GetVertexShaderBytecode(&pShaderByteCode, &iShaderByteCodeLength);

	if (FAILED(m_pDevice->CreateInputLayout(VertexPositionColor::InputElements, VertexPositionColor::InputElementCount, pShaderByteCode, iShaderByteCodeLength, &m_pInputLayout)))
		return E_FAIL;
#endif

	return S_OK;
}

HRESULT CCollider_Test::Initialize(void* pArg)
{
	if (nullptr == pArg)
		return E_FAIL;

	COLLIDER_DESC* pDesc = static_cast<COLLIDER_DESC*>(pArg);


	
	
	_float halfX = pDesc->fSizeX * 0.5f;
	_float halfY = pDesc->fSizeY * 0.5f;
	_float halfZ = pDesc->fSizeZ * 0.5f;


	_float OffsetX = pDesc->Offset.x;
	_float OffsetY = pDesc->Offset.y;
	_float OffsetZ = pDesc->Offset.z;

//	m_bObstacle = pDesc->bObstacle;

	m_pTargetTransform = pDesc->pTransform;


	//오프셋이 없는경우
	//m_LocalVertices[0] = { -halfX,	 halfY,		-halfZ };
	//m_LocalVertices[1] = { halfX,	 halfY,		-halfZ };
	//m_LocalVertices[2] = { halfX,	-halfY,		-halfZ };
	//m_LocalVertices[3] = { -halfX,	-halfY,		-halfZ };
	//m_LocalVertices[4] = { -halfX,	 halfY,		 halfZ };
	//m_LocalVertices[5] = { halfX,	 halfY,		 halfZ };
	//m_LocalVertices[6] = { halfX,	-halfY,		 halfZ };
	//m_LocalVertices[7] = { -halfX,	-halfY,		 halfZ };



	m_LocalVertices[0] = { -halfX + OffsetX,	 halfY + OffsetY,		-halfZ + OffsetZ };
	m_LocalVertices[1] = { halfX  + OffsetX,	 halfY + OffsetY,		-halfZ + OffsetZ };
	m_LocalVertices[2] = { halfX  + OffsetX,	-halfY + OffsetY,		-halfZ + OffsetZ };
	m_LocalVertices[3] = { -halfX + OffsetX,	-halfY + OffsetY,		-halfZ + OffsetZ };
	m_LocalVertices[4] = { -halfX + OffsetX,	 halfY + OffsetY,		 halfZ + OffsetZ };
	m_LocalVertices[5] = { halfX  + OffsetX,	 halfY + OffsetY,		 halfZ + OffsetZ };
	m_LocalVertices[6] = { halfX  + OffsetX,	-halfY + OffsetY,		 halfZ + OffsetZ };
	m_LocalVertices[7] = { -halfX + OffsetX,	-halfY + OffsetY,		 halfZ + OffsetZ };






	return S_OK;
}


void CCollider_Test::Update(_float3 Offset)
{
	_float4 WorldPos4;
	XMStoreFloat4(&WorldPos4, m_pTargetTransform->Get_State(CTransform::STATE_POSITION));

	_float3 WorldPos = { WorldPos4.x + Offset.x,  WorldPos4.y + Offset.y, WorldPos4.z + Offset .z};



	m_WorldVertices[3] = Add(m_LocalVertices[3], WorldPos);
	m_WorldVertices[5] = Add(m_LocalVertices[5], WorldPos);


	//Render용
#ifdef _DEBUG
	m_WorldVertices[0] = Add(m_LocalVertices[0], WorldPos);
	m_WorldVertices[1] = Add(m_LocalVertices[1], WorldPos);
	m_WorldVertices[2] = Add(m_LocalVertices[2], WorldPos);
	m_WorldVertices[4] = Add(m_LocalVertices[4], WorldPos);
	m_WorldVertices[6] = Add(m_LocalVertices[6], WorldPos);
	m_WorldVertices[7] = Add(m_LocalVertices[7], WorldPos);
#endif // _DEBUG



	int a = 0;
}


/*
void CCollider_Test::Update()
{
	
	_float4 WorldPos4;
	XMStoreFloat4(&WorldPos4, m_pTargetTransform->Get_State(CTransform::STATE_POSITION));

	_float3 WorldPos = { WorldPos4.x,  WorldPos4.y, WorldPos4.z };



	m_WorldVertices[3] = Add(m_LocalVertices[3], WorldPos);
	m_WorldVertices[5] = Add(m_LocalVertices[5], WorldPos);


	//Render용
#ifdef _DEBUG
	m_WorldVertices[0] = Add(m_LocalVertices[0] , WorldPos);
	m_WorldVertices[1] = Add(m_LocalVertices[1] , WorldPos);
	m_WorldVertices[2] = Add(m_LocalVertices[2] , WorldPos);
	m_WorldVertices[4] = Add(m_LocalVertices[4] , WorldPos);
	m_WorldVertices[6] = Add(m_LocalVertices[6] , WorldPos);
	m_WorldVertices[7] = Add(m_LocalVertices[7] , WorldPos);
#endif // _DEBUG



	int a = 0;
}

void CCollider_Test::Update(_float3 Offset)
{
	_float4 WorldPos4;
	XMStoreFloat4(&WorldPos4, m_pTargetTransform->Get_State(CTransform::STATE_POSITION));

	_float3 WorldPos = { WorldPos4.x + Offset.x,  WorldPos4.y + Offset.y, WorldPos4.z + Offset .z};



	m_WorldVertices[3] = Add(m_LocalVertices[3], WorldPos);
	m_WorldVertices[5] = Add(m_LocalVertices[5], WorldPos);


	//Render용
#ifdef _DEBUG
	m_WorldVertices[0] = Add(m_LocalVertices[0], WorldPos);
	m_WorldVertices[1] = Add(m_LocalVertices[1], WorldPos);
	m_WorldVertices[2] = Add(m_LocalVertices[2], WorldPos);
	m_WorldVertices[4] = Add(m_LocalVertices[4], WorldPos);
	m_WorldVertices[6] = Add(m_LocalVertices[6], WorldPos);
	m_WorldVertices[7] = Add(m_LocalVertices[7], WorldPos);
#endif // _DEBUG



	int a = 0;
}

*/

HRESULT CCollider_Test::Render()
{

#ifdef _DEBUG
	m_pEffect->SetWorld(XMMatrixIdentity());
	m_pEffect->SetView(m_pGameInstance->Get_Transform_Matrix(CPipeLine::D3DTS_VIEW));
	m_pEffect->SetProjection(m_pGameInstance->Get_Transform_Matrix(CPipeLine::D3DTS_PROJ));

	m_pContext->IASetInputLayout(m_pInputLayout);

	m_pEffect->Apply(m_pContext);

	m_pBatch->Begin();

	//m_pBounding->Render(m_pBatch, m_isColl == false ? XMVectorSet(0.f, 1.f, 0.f, 1.f) : XMVectorSet(1.f, 0.f, 0.f, 1.f));
	//DX::Draw(m_pBatch, *m_pDesc, DirectX::Colors::White);

	//생각해보니까  직육면체의 선분은 8개가 아니라 12개임   직육면체가 아닌 X모양이 그려지나 범위는 얼추 눈에 들어옴
	//for(int i=0; i<7; i++)
	//{
	//	VertexPositionColor v1(m_WorldVertices[i],  COLOR_GREEN);
	//	VertexPositionColor v2(m_WorldVertices[i + 1], COLOR_GREEN);
	//
	//	m_pBatch->DrawLine(v1,v2);
	//}
	//{
	//	VertexPositionColor v1(m_WorldVertices[7], COLOR_GREEN);
	//	VertexPositionColor v2(m_WorldVertices[0], COLOR_GREEN);
	//
	//	m_pBatch->DrawLine(v1, v2);
	//}

	{	
		VertexPositionColor v0(m_WorldVertices[0], COLOR_GREEN);
		VertexPositionColor v1(m_WorldVertices[1], COLOR_GREEN);
		VertexPositionColor v2(m_WorldVertices[2], COLOR_GREEN);
		VertexPositionColor v3(m_WorldVertices[3], COLOR_GREEN);
		VertexPositionColor v4(m_WorldVertices[4], COLOR_GREEN);
		VertexPositionColor v5(m_WorldVertices[5], COLOR_GREEN);
		VertexPositionColor v6(m_WorldVertices[6], COLOR_GREEN);
		VertexPositionColor v7(m_WorldVertices[7], COLOR_GREEN);

		//앞면
		m_pBatch->DrawLine(v0, v1);
		m_pBatch->DrawLine(v1, v2);
		m_pBatch->DrawLine(v2, v3);
		m_pBatch->DrawLine(v3, v0);

		//뒷면
		m_pBatch->DrawLine(v4, v5);
		m_pBatch->DrawLine(v5, v6);
		m_pBatch->DrawLine(v6, v7);
		m_pBatch->DrawLine(v7, v4);


		//앞면 뒷면 사이 연결점
		m_pBatch->DrawLine(v0, v4);
		m_pBatch->DrawLine(v3, v7);
		m_pBatch->DrawLine(v2, v6);
		m_pBatch->DrawLine(v1, v5);
	}

	m_pBatch->End();

#endif
	return S_OK;

}



HRESULT CCollider_Test::Render(_float4 fColor)
{

#ifdef _DEBUG
	m_pEffect->SetWorld(XMMatrixIdentity());
	m_pEffect->SetView(m_pGameInstance->Get_Transform_Matrix(CPipeLine::D3DTS_VIEW));
	m_pEffect->SetProjection(m_pGameInstance->Get_Transform_Matrix(CPipeLine::D3DTS_PROJ));

	m_pContext->IASetInputLayout(m_pInputLayout);

	m_pEffect->Apply(m_pContext);

	m_pBatch->Begin();

	//m_pBounding->Render(m_pBatch, m_isColl == false ? XMVectorSet(0.f, 1.f, 0.f, 1.f) : XMVectorSet(1.f, 0.f, 0.f, 1.f));
	//DX::Draw(m_pBatch, *m_pDesc, DirectX::Colors::White);

	//생각해보니까  직육면체의 선분은 8개가 아니라 12개임   직육면체가 아닌 X모양이 그려지나 범위는 얼추 눈에 들어옴
	//for(int i=0; i<7; i++)
	//{
	//	VertexPositionColor v1(m_WorldVertices[i],  COLOR_GREEN);
	//	VertexPositionColor v2(m_WorldVertices[i + 1], COLOR_GREEN);
	//
	//	m_pBatch->DrawLine(v1,v2);
	//}
	//{
	//	VertexPositionColor v1(m_WorldVertices[7], COLOR_GREEN);
	//	VertexPositionColor v2(m_WorldVertices[0], COLOR_GREEN);
	//
	//	m_pBatch->DrawLine(v1, v2);
	//}

	{
		VertexPositionColor v0(m_WorldVertices[0], fColor);
		VertexPositionColor v1(m_WorldVertices[1],fColor);
		VertexPositionColor v2(m_WorldVertices[2],fColor);
		VertexPositionColor v3(m_WorldVertices[3],fColor);
		VertexPositionColor v4(m_WorldVertices[4],fColor);
		VertexPositionColor v5(m_WorldVertices[5],fColor);
		VertexPositionColor v6(m_WorldVertices[6],fColor);
		VertexPositionColor v7(m_WorldVertices[7],fColor);

		//앞면
		m_pBatch->DrawLine(v0, v1);
		m_pBatch->DrawLine(v1, v2);
		m_pBatch->DrawLine(v2, v3);
		m_pBatch->DrawLine(v3, v0);

		//뒷면
		m_pBatch->DrawLine(v4, v5);
		m_pBatch->DrawLine(v5, v6);
		m_pBatch->DrawLine(v6, v7);
		m_pBatch->DrawLine(v7, v4);


		//앞면 뒷면 사이 연결점
		m_pBatch->DrawLine(v0, v4);
		m_pBatch->DrawLine(v3, v7);
		m_pBatch->DrawLine(v2, v6);
		m_pBatch->DrawLine(v1, v5);
	}

	m_pBatch->End();

#endif
	return S_OK;
}



bool CCollider_Test::isColliding(const CCollider_Test* other) //const
{
	

	return (m_WorldVertices[MIN].x <= other->m_WorldVertices[MAX].x && m_WorldVertices[MAX].x >= other->m_WorldVertices[MIN].x) &&
		//(m_WorldVertices[MIN].z <= other->m_WorldVertices[MAX].z && m_WorldVertices[MAX].z >= other->m_WorldVertices[MIN].z) &&
		(m_WorldVertices[MIN].y <= other->m_WorldVertices[MAX].y && m_WorldVertices[MAX].y >= other->m_WorldVertices[MIN].y);


}

bool CCollider_Test::isColliding( CComponent* other)
{
	CCollider_Test* Collider_other = static_cast<CCollider_Test*>(other);

	/*
	return (m_WorldVertices[MAX].vPosition.x <= Collider_other->m_WorldVertices[MAX].vPosition.x && m_WorldVertices[MAX].vPosition.x >= Collider_other->m_WorldVertices[MIN].vPosition.x) &&
		(m_WorldVertices[MIN].vPosition.y <= Collider_other->m_WorldVertices[MAX].vPosition.y && m_WorldVertices[MAX].vPosition.y >= Collider_other->m_WorldVertices[MIN].vPosition.y) &&
		(m_WorldVertices[MIN].vPosition.z <= Collider_other->m_WorldVertices[MAX].vPosition.z && m_WorldVertices[MAX].vPosition.z >= Collider_other->m_WorldVertices[MIN].vPosition.z);
*/
	return (m_WorldVertices[MIN].x <= Collider_other->m_WorldVertices[MAX].x && m_WorldVertices[MAX].x >= Collider_other->m_WorldVertices[MIN].x) &&
		(m_WorldVertices[MIN].y <= Collider_other->m_WorldVertices[MAX].y && m_WorldVertices[MAX].y >= Collider_other->m_WorldVertices[MIN].y);
		//&&(m_WorldVertices[MIN].z <= Collider_other->m_WorldVertices[MAX].z && m_WorldVertices[MAX].z >= Collider_other->m_WorldVertices[MIN].z);


}


bool CCollider_Test::isCollidingAndAddCheck(CComponent* other)
{
	if (isColliding(other))
	{
		static_cast<CCollider_Test*>(other)->SetAddCheck(true);
		return true;
	}
	
	return false;

}

bool CCollider_Test::GetnSwitch_AddCheck()
{
	if (m_bAddCollisionCheck)
	{
		m_bAddCollisionCheck = false;
		return true;
	}
		return false;
}


/*
_float3 CCollider_Test::CalculateOverlapLengths(const XMFLOAT3& min1, const XMFLOAT3& max1, const XMFLOAT3& min2, const XMFLOAT3& max2)
{
	XMFLOAT3 overlapMin, overlapMax, overlapLengths;


	overlapMin.x = max(min1.x, min2.x);
	overlapMin.y = max(min1.y, min2.y);
	overlapMin.z = max(min1.z, min2.z);

	overlapMax.x = min(max1.x, max2.x);
	overlapMax.y = min(max1.y, max2.y);
	overlapMax.z = min(max1.z, max2.z);

	overlapLengths.x = (overlapMin.x < overlapMax.x) ? (overlapMax.x - overlapMin.x) : 0.0f;
	overlapLengths.y = (overlapMin.y < overlapMax.y) ? (overlapMax.y - overlapMin.y) : 0.0f;
	overlapLengths.z = (overlapMin.z < overlapMax.z) ? (overlapMax.z - overlapMin.z) : 0.0f;

	return overlapLengths;
}

_float3 CCollider_Test::CalculateOverlapLengths(CCollider_Test* pTmpCollider, CCollider_Test* pDstCollider)
{
	_float3 Test = CalculateOverlapLengths(pTmpCollider->Get_Min(), pTmpCollider->Get_Max(), pDstCollider->Get_Min(), pDstCollider->Get_Max());

	return Test;
	//return CalculateOverlapLengths(pTmpCollider->Get_Min(), pTmpCollider->Get_Max(), pDstCollider->Get_Min(), pDstCollider->Get_Max());
}
*/


_float3 CCollider_Test::CalculateOverlapLengthsAIO(const XMFLOAT3& min1, const XMFLOAT3& max1, const XMFLOAT3& min2, const XMFLOAT3& max2)
{
	XMFLOAT3 overlapMin, overlapMax, overlapLengths;


	overlapMin.x = max(min1.x, min2.x);
	//overlapMin.y = max(min1.y, min2.y);
	overlapMin.z = max(min1.z, min2.z);

	overlapMax.x = min(max1.x, max2.x);
	//overlapMax.y = min(max1.y, max2.y);
	overlapMax.z = min(max1.z, max2.z);

	overlapLengths.x = (overlapMin.x < overlapMax.x) ? (overlapMax.x - overlapMin.x) : 0.0f;
	overlapLengths.y = 0.f;// (overlapMin.y < overlapMax.y) ? (overlapMax.y - overlapMin.y) : 0.0f;
	overlapLengths.z = (overlapMin.z < overlapMax.z) ? (overlapMax.z - overlapMin.z) : 0.0f;


	//길이 구하기 완료





	XMFLOAT3 direction = XMFLOAT3(0.0f, 0.0f, 0.0f);

	// X 축 충돌 판별
	if (max1.x > min2.x && min1.x < max2.x) {
		if (min1.x < min2.x) {
			direction.x = 1.0f; // 충돌 방향은 양수 방향 (m_WorldVertices2가 오른쪽에 있음)
		}
		else {
			direction.x = -1.0f; // 충돌 방향은 음수 방향 (m_WorldVertices2가 왼쪽에 있음)
		}
	}

	/*
	// Y 축 충돌 판별
	if (max1.y > min2.y && min1.y < max2.y) {
		if (min1.y < min2.y) {
			direction.y = 1.0f; // 충돌 방향은 양수 방향 (m_WorldVertices2가 위에 있음)
		}
		else {
			direction.y = -1.0f; // 충돌 방향은 음수 방향 (m_WorldVertices2가 아래에 있음)
		}
	}
	*/
	// Z 축 충돌 판별
	if (max1.z > min2.z && min1.z < max2.z) {
		if (min1.z < min2.z) {
			direction.z = 1.0f; // 충돌 방향은 양수 방향 (m_WorldVertices2가 앞에 있음)
		}
		else {
			direction.z = -1.0f; // 충돌 방향은 음수 방향 (m_WorldVertices2가 뒤에 있음)
		}
	}

	//방향 구하기 완료




	//총합
	overlapLengths.x = overlapLengths.x * direction.x;
	overlapLengths.y = 0.f;// overlapLengths.y* direction.y;
	overlapLengths.z = overlapLengths.z * direction.z;


	return overlapLengths;



}

_float3 CCollider_Test::CalculateOverlapLengthsAIO(CCollider_Test* pTmpCollider, CCollider_Test* pDstCollider)
{

	//_float3 Test =CalculateOverlapLengthsAIO(pTmpCollider->Get_Min(), pTmpCollider->Get_Max(), pDstCollider->Get_Min(), pDstCollider->Get_Max());
	//return Test;

	//return CalculateOverlapLengthsAIO(pTmpCollider->Get_Min(), pTmpCollider->Get_Max(), pDstCollider->Get_Min(), pDstCollider->Get_Max());
	return CalculateOverlapLengths2(pTmpCollider->Get_Min(), pTmpCollider->Get_Max(), pDstCollider->Get_Min(), pDstCollider->Get_Max());



}

_float3 CCollider_Test::CalculateOverlapLengths2(const XMFLOAT3& min1, const XMFLOAT3& max1, const XMFLOAT3& min2, const XMFLOAT3& max2)
{
	XMFLOAT3 overlapMin, overlapMax, overlapLengths;

	overlapMin.x = max(min1.x, min2.x);
	overlapMin.z = max(min1.z, min2.z);

	overlapMax.x = min(max1.x, max2.x);
	overlapMax.z = min(max1.z, max2.z);

	overlapLengths.x = (overlapMin.x < overlapMax.x) ? (overlapMax.x - overlapMin.x) : 0.0f;
	overlapLengths.y = 0.f;
	overlapLengths.z = (overlapMin.z < overlapMax.z) ? (overlapMax.z - overlapMin.z) : 0.0f;

	XMFLOAT3 direction = XMFLOAT3(0.0f, 0.0f, 0.0f);

	if (max1.x > min2.x && min1.x < max2.x) {
		if (min1.x < min2.x) {
			direction.x = -1.0f; // 밀어내는 방향은 음수 방향 (왼쪽)
		}
		else {
			direction.x = 1.0f; // 밀어내는 방향은 양수 방향 (오른쪽)
		}
	}

	if (max1.z > min2.z && min1.z < max2.z) {
		if (min1.z < min2.z) {
			direction.z = -1.0f; // 밀어내는 방향은 음수 방향 (뒤쪽)
		}
		else {
			direction.z = 1.0f; // 밀어내는 방향은 양수 방향 (앞쪽)
		}
	}

	// 축이 겹치는 양이 더 작은 쪽으로 밀어내기
	if (overlapLengths.x > 0 && overlapLengths.z > 0) {
		if (overlapLengths.x < overlapLengths.z) {
			overlapLengths.z = 0.0f; // x축으로 밀어내기
		}
		else {
			overlapLengths.x = 0.0f; // z축으로 밀어내기
		}
	}

	overlapLengths.x = overlapLengths.x * direction.x;
	overlapLengths.y = 0.f;
	overlapLengths.z = overlapLengths.z * direction.z;

	return overlapLengths;
}




CCollider_Test* CCollider_Test::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CCollider_Test* pInstance = new CCollider_Test(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CCollider_Test"));
		Safe_Release(pInstance);
	}

	return pInstance;

}

CComponent* CCollider_Test::Clone(void* pArg)
{
	CCollider_Test* pInstance = new CCollider_Test(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CCollider_Test"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCollider_Test::Free()
{
	__super::Free();

#ifdef _DEBUG
	if (false == m_isCloned)
	{
		Safe_Delete(m_pBatch);
		Safe_Delete(m_pEffect);
	}

	Safe_Release(m_pInputLayout);
#endif

}
