#include "..\Public\VIBuffer_Instancing.h"
#include <random>

CVIBuffer_Instancing::CVIBuffer_Instancing(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CVIBuffer{ pDevice, pContext }
{
}

CVIBuffer_Instancing::CVIBuffer_Instancing(const CVIBuffer_Instancing& Prototype)
	: CVIBuffer{ Prototype }
	, m_pVBInstance{ Prototype.m_pVBInstance }
	, m_iNumInstance{ Prototype.m_iNumInstance }
	, m_iInstanceVertexStride{ Prototype.m_iInstanceVertexStride }
	, m_iNumIndexPerInstance{ Prototype.m_iNumIndexPerInstance }
	, m_pInstanceVertices{ Prototype.m_pInstanceVertices }
	, m_pSpeeds{ Prototype.m_pSpeeds }
	, m_vPivotPos{ Prototype.m_vPivotPos }
	, m_isLoop{ Prototype.m_isLoop }
	, m_fMaxTime{ Prototype.m_fMaxTime }
{
	Safe_AddRef(m_pVBInstance);
}

HRESULT CVIBuffer_Instancing::Initialize_Prototype(const VIBUFFER_INSTANCE_DESC* pInitialDesc)
{
	m_iNumInstance = pInitialDesc->iNumInstance;

	m_pSpeeds = new float[m_iNumInstance];

	m_vPivotPos = pInitialDesc->vPivot;

	m_isLoop = pInitialDesc->isLoop;

	m_fMaxTime = pInitialDesc->vLifeTime.y;

	/* 인게임내에서 스피드가 바뀌지 않도록 파티클 원형객체를 생성할때 각 입자별 스피드를 계산하여 저장해둔다. */
	for (size_t i = 0; i < m_iNumInstance; i++)
	{
		float speed = Get_Random(pInitialDesc->vSpeed.x, pInitialDesc->vSpeed.y);
		m_pSpeeds[i] = speed;
	}

	return S_OK;
}

HRESULT CVIBuffer_Instancing::Initialize(void* pArg)
{

	return S_OK;
}

HRESULT CVIBuffer_Instancing::Bind_Buffers()
{

	ID3D11Buffer* pVertexBufffers[] = {
		m_pVB,
		m_pVBInstance
	};

	_uint				iVertexStrides[] = {
		m_iVertexStride,
		m_iInstanceVertexStride
	};

	_uint				iOffsets[] = {
		0,
		0
	};

	m_pContext->IASetVertexBuffers(0, m_iNumVertexBuffers, pVertexBufffers, iVertexStrides, iOffsets);
	m_pContext->IASetIndexBuffer(m_pIB, m_eIndexFormat, 0);
	m_pContext->IASetPrimitiveTopology(m_ePrimitive_Topology);

	return S_OK;
}

HRESULT CVIBuffer_Instancing::Render()
{
	m_pContext->DrawIndexedInstanced(m_iNumIndexPerInstance, m_iNumInstance, 0, 0, 0);

	return S_OK;
}


HRESULT CVIBuffer_Instancing::Create_InstanceBuffer()
{
	ZeroMemory(&m_BufferDesc, sizeof(m_BufferDesc));

	m_BufferDesc.ByteWidth = m_iNumInstance * m_iInstanceVertexStride;
	m_BufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = m_iInstanceVertexStride;

	ZeroMemory(&m_InitialData, sizeof m_InitialData);
	m_InitialData.pSysMem = m_pInstanceVertices;

	if (FAILED(__super::Create_Buffer(&m_pVBInstance)))
		return E_FAIL;

	return S_OK;
}

_bool CVIBuffer_Instancing::Spread(_float fTimeDelta)
{
	D3D11_MAPPED_SUBRESOURCE		MappedSubResource{};

	m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &MappedSubResource);

	VTXINSTANCE* pMatrices = static_cast<VTXINSTANCE*>(MappedSubResource.pData);

	for (size_t i = 0; i < m_iNumInstance; i++)
	{
		// 라이프타임 업데이트
		pMatrices[i].vLifeTime.y += fTimeDelta;

		if (pMatrices[i].vLifeTime.y >= 0.f)
		{
			_vector		vMoveDir = XMVector3Normalize(XMLoadFloat4(&m_pInstanceVertices[i].vTranslation) - XMVectorSetW(XMLoadFloat3(&m_vPivotPos), 1.f));

			// Store moveDir in the instance buffer
			XMStoreFloat3(&pMatrices[i].vMoveDir, vMoveDir);

			XMStoreFloat4(&pMatrices[i].vTranslation,
				XMLoadFloat4(&pMatrices[i].vTranslation) + vMoveDir * m_pSpeeds[i] * fTimeDelta);
		}

		if (pMatrices[i].vLifeTime.y >= pMatrices[i].vLifeTime.x)
		{

			if (m_isLoop == true)
			{
				// 파티클을 초기 위치로 리셋
				pMatrices[i].vTranslation = m_pInstanceVertices[i].vTranslation;

				// lifeTime.y를 -lifeTime.x와 0 사이의 랜덤한 값으로 재설정
				pMatrices[i].vLifeTime.y = -RandomBetween(0.f, pMatrices[i].vLifeTime.x);
			}
			else
			{
				// 파티클 생명 주기가 끝났을 때 처리
				m_pContext->Unmap(m_pVBInstance, 0);
				return true;
			}
		}
	}

	m_pContext->Unmap(m_pVBInstance, 0);
	return false;
}


_bool CVIBuffer_Instancing::FocusPoint(_float fTimeDelta)
{
	D3D11_MAPPED_SUBRESOURCE MappedSubResource{};

	m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &MappedSubResource);

	VTXINSTANCE* pMatrices = static_cast<VTXINSTANCE*>(MappedSubResource.pData);

	for (size_t i = 0; i < m_iNumInstance; i++)
	{
		// 라이프타임 업데이트
		pMatrices[i].vLifeTime.y += fTimeDelta;

		if (pMatrices[i].vLifeTime.y >= 0.f)
		{
			_vector vMoveDir = XMVector3Normalize(XMVectorSetW(XMLoadFloat3(&m_vPivotPos), 1.f) - XMLoadFloat4(&m_pInstanceVertices[i].vTranslation));

			// 이동 방향을 인스턴스 버퍼에 저장
			XMStoreFloat3(&pMatrices[i].vMoveDir, vMoveDir);

			// 파티클 위치 업데이트
			XMStoreFloat4(&pMatrices[i].vTranslation,
				XMLoadFloat4(&pMatrices[i].vTranslation) + vMoveDir * m_pSpeeds[i] * fTimeDelta);
		}

		if (pMatrices[i].vLifeTime.y >= pMatrices[i].vLifeTime.x)
		{
			if (m_isLoop == true)
			{
				// 파티클을 초기 위치로 리셋
				pMatrices[i].vTranslation = m_pInstanceVertices[i].vTranslation;

				// lifeTime.y를 -lifeTime.x와 0 사이의 랜덤한 값으로 재설정
				pMatrices[i].vLifeTime.y = -RandomBetween(0.f, pMatrices[i].vLifeTime.x);
			}
			else
			{
				// 파티클 생명 주기가 끝났을 때 처리
				m_pContext->Unmap(m_pVBInstance, 0);
				return true;
			}
		}
	}

	m_pContext->Unmap(m_pVBInstance, 0);
	return false;
}

void CVIBuffer_Instancing::Particle_Initialize()
{
	D3D11_MAPPED_SUBRESOURCE		MappedSubResource{};

	m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &MappedSubResource);

	VTXINSTANCE* pMatrices = static_cast<VTXINSTANCE*>(MappedSubResource.pData);

	for (size_t i = 0; i < m_iNumInstance; i++)
	{
		// 각 인스턴스별로 모든 필드를 올바르게 복사합니다.
		pMatrices[i].vRight = m_pInstanceVertices[i].vRight;
		pMatrices[i].vUp = m_pInstanceVertices[i].vUp;
		pMatrices[i].vLook = m_pInstanceVertices[i].vLook;
		pMatrices[i].vMoveDir = m_pInstanceVertices[i].vMoveDir;
		pMatrices[i].vTranslation = m_pInstanceVertices[i].vTranslation;
		pMatrices[i].vLifeTime = m_pInstanceVertices[i].vLifeTime;
	}

	m_pContext->Unmap(m_pVBInstance, 0);
}



_bool CVIBuffer_Instancing::Spread_2D(_float fTimeDelta)
{
	D3D11_MAPPED_SUBRESOURCE		MappedSubResource{};

	m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &MappedSubResource);

	VTXINSTANCE* pMatrices = static_cast<VTXINSTANCE*>(MappedSubResource.pData);

	for (size_t i = 0; i < m_iNumInstance; i++)
	{
		// 현재 파티클의 위치를 개별적으로 로드
		float currentX = m_pInstanceVertices[i].vTranslation.x;
		float currentY = m_pInstanceVertices[i].vTranslation.y;

		// 피봇 위치도 개별적으로 로드
		float pivotX = m_vPivotPos.x;
		float pivotY = m_vPivotPos.y;

		// 현재 위치와 피봇 위치를 이용해 이동 방향 계산
		float dirX = currentX - pivotX;
		float dirY = currentY - pivotY;

		// 벡터 정규화
		float length = sqrt(dirX * dirX + dirY * dirY);
		if (length != 0.f)
		{
			dirX /= length;
			dirY /= length;
		}

		// 이동 속도 계산
		float moveX = dirX * m_pSpeeds[i] * fTimeDelta;
		float moveY = dirY * m_pSpeeds[i] * fTimeDelta;

		// 파티클의 현재 위치 업데이트
		pMatrices[i].vTranslation.x += moveX;
		pMatrices[i].vTranslation.y += moveY;

		// 생명 시간 업데이트
		pMatrices[i].vLifeTime.y += fTimeDelta;

		// 루핑 처리
		if (m_isLoop && pMatrices[i].vLifeTime.y >= pMatrices[i].vLifeTime.x)
		{
			// 초기 위치로 리셋
			pMatrices[i].vTranslation.x = m_pInstanceVertices[i].vTranslation.x;
			pMatrices[i].vTranslation.y = m_pInstanceVertices[i].vTranslation.y;
			pMatrices[i].vLifeTime.y = 0.f;
		}
		// 루프가 안돌지만 라이프타임 시간을 넘어섰을 때
		else if (!m_isLoop && pMatrices[i].vLifeTime.y >= pMatrices[i].vLifeTime.x)
		{
			m_pContext->Unmap(m_pVBInstance, 0);
			return true;
		}
	}

	m_pContext->Unmap(m_pVBInstance, 0);
	return false;
}

_bool CVIBuffer_Instancing::Spread_FireCracker_2D(_float fTimeDelta)
{
	//MaxTime을 넘어서면 모든 파티클 입자가 다 시간이 지났다고 판단
	if (m_fElapsedTime >= m_fMaxTime)
		return true;

	D3D11_MAPPED_SUBRESOURCE		MappedSubResource{};

	m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &MappedSubResource);

	VTXINSTANCE* pMatrices = static_cast<VTXINSTANCE*>(MappedSubResource.pData);

	// 조절 변수
	const float decelerationFactor = 0.8f; // 속도 감속 비율
	const float gravityStrength = -0.8f;  // 중력 가속도

	for (size_t i = 0; i < m_iNumInstance; i++)
	{
		if (!m_isLoop && pMatrices[i].vLifeTime.y >= pMatrices[i].vLifeTime.x)
			continue;

		// 생명 시간 비율 계산
		float ratio = pMatrices[i].vLifeTime.y / pMatrices[i].vLifeTime.x;

		// 현재 파티클의 위치를 개별적으로 로드
		float currentX = m_pInstanceVertices[i].vTranslation.x;
		float currentY = m_pInstanceVertices[i].vTranslation.y;

		// 피봇 위치도 개별적으로 로드
		float pivotX = m_vPivotPos.x;
		float pivotY = m_vPivotPos.y;

		// 현재 위치와 피봇 위치를 이용해 이동 방향 계산
		float dirX = currentX - pivotX;
		float dirY = currentY - pivotY;

		// 벡터 정규화
		float length = sqrt(dirX * dirX + dirY * dirY);
		if (length != 0.f)
		{
			dirX /= length;
			dirY /= length;
		}

		// 초기 속도 가져오기
		float initialSpeed = m_pSpeeds[i];

		float vx = 0.f;
		float vy = 0.f;

		if (ratio < 0.7f)
		{
			// 속도 감속 계산
			float speedDamping = 1.0f - (ratio / 0.7f) * decelerationFactor; // 감속 비율 적용

			// 현재 속도 계산
			vx = dirX * initialSpeed * speedDamping;
			vy = dirY * initialSpeed * speedDamping;
		}
		else
		{
			// 감속된 속도를 유지
			float speedDamping = 1.0f - decelerationFactor; // 감속 후 남은 속도

			// 수평 속도는 감속된 값을 유지
			vx = dirX * initialSpeed * speedDamping;

			// 중력 가속도 적용 (속도에 누적)
			vy = dirY * initialSpeed * speedDamping + gravityStrength * (pMatrices[i].vLifeTime.y - pMatrices[i].vLifeTime.x * 0.7f);
		}

		// 위치 업데이트
		pMatrices[i].vTranslation.x += vx * fTimeDelta;
		pMatrices[i].vTranslation.y += vy * fTimeDelta;

		// 생명 시간 업데이트
		pMatrices[i].vLifeTime.y += fTimeDelta;

		// 루핑 처리
		if (m_isLoop && pMatrices[i].vLifeTime.y >= pMatrices[i].vLifeTime.x)
		{
			// 초기 위치로 리셋
			pMatrices[i].vTranslation = m_pInstanceVertices[i].vTranslation;
			pMatrices[i].vLifeTime.y = 0.f;
		}
	}

	m_fElapsedTime += fTimeDelta;

	m_pContext->Unmap(m_pVBInstance, 0);
	return false;
}

_bool CVIBuffer_Instancing::Half_Spread_X_2D(_float fTimeDelta)
{
	D3D11_MAPPED_SUBRESOURCE		MappedSubResource{};

	m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &MappedSubResource);

	VTXINSTANCE* pMatrices = static_cast<VTXINSTANCE*>(MappedSubResource.pData);

	for (size_t i = 0; i < m_iNumInstance; i++)
	{
		// 현재 파티클의 위치를 개별적으로 로드
		float currentX = m_pInstanceVertices[i].vTranslation.x;
		float currentY = m_pInstanceVertices[i].vTranslation.y;

		// 피봇 위치도 개별적으로 로드
		float pivotX = m_vPivotPos.x;
		float pivotY = m_vPivotPos.y;

		// 현재 위치와 피봇 위치를 이용해 이동 방향 계산
		float dirX = currentX - pivotX;
		float dirY = currentY - pivotY;

		// 아래로 향하는 경우 dirY를 양수로 변경하여 위로 향하도록 수정
		if (dirX < 0.f)
		{
			dirX = -dirX;
			dirY = -dirY;
		}


		// 벡터 정규화
		float length = sqrt(dirX * dirX + dirY * dirY);
		if (length != 0.f)
		{
			dirX /= length;
			dirY /= length;
		}

		// 이동 속도 계산
		float moveX = dirX * m_pSpeeds[i] * fTimeDelta;
		float moveY = dirY * m_pSpeeds[i] * fTimeDelta;

		// 파티클의 현재 위치 업데이트
		pMatrices[i].vTranslation.x += moveX;
		pMatrices[i].vTranslation.y += moveY;

		// 생명 시간 업데이트
		pMatrices[i].vLifeTime.y += fTimeDelta;

		// 루핑 처리
		if (m_isLoop && pMatrices[i].vLifeTime.y >= pMatrices[i].vLifeTime.x)
		{
			// 초기 위치로 리셋
			pMatrices[i].vTranslation.x = m_pInstanceVertices[i].vTranslation.x;
			pMatrices[i].vTranslation.y = m_pInstanceVertices[i].vTranslation.y;
			pMatrices[i].vLifeTime.y = 0.f;
		}
		// 루프가 안돌지만 라이프타임 시간을 넘어섰을 때
		else if (!m_isLoop && pMatrices[i].vLifeTime.y >= pMatrices[i].vLifeTime.x)
		{
			m_pContext->Unmap(m_pVBInstance, 0);
			return true;
		}
	}

	m_pContext->Unmap(m_pVBInstance, 0);
	return false;
}

_bool CVIBuffer_Instancing::Half_Spread_Y_2D(_float fTimeDelta)
{
	//MaxTime을 넘어서면 모든 파티클 입자가 다 시간이 지났다고 판단
	if (m_fElapsedTime >= m_fMaxTime)
		return true;

	D3D11_MAPPED_SUBRESOURCE		MappedSubResource{};

	m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &MappedSubResource);

	VTXINSTANCE* pMatrices = static_cast<VTXINSTANCE*>(MappedSubResource.pData);

	for (size_t i = 0; i < m_iNumInstance; i++)
	{
		// 루프가 안돌지만 라이프타임 시간을 넘어섰을 때
		if (!m_isLoop && pMatrices[i].vLifeTime.y >= pMatrices[i].vLifeTime.x)
			continue;

		// 현재 파티클의 위치를 개별적으로 로드
		float currentX = m_pInstanceVertices[i].vTranslation.x;
		float currentY = m_pInstanceVertices[i].vTranslation.y;

		// 피봇 위치도 개별적으로 로드
		float pivotX = m_vPivotPos.x;
		float pivotY = m_vPivotPos.y;

		// 현재 위치와 피봇 위치를 이용해 이동 방향 계산
		float dirX = currentX - pivotX;
		float dirY = currentY - pivotY;

		// 아래로 향하는 경우 dirY를 양수로 변경하여 위로 향하도록 수정
		if (dirY < 0.f)
		{
			dirX = -dirX;
			dirY = -dirY;
		}


		// 벡터 정규화
		float length = sqrt(dirX * dirX + dirY * dirY);
		if (length != 0.f)
		{
			dirX /= length;
			dirY /= length;
		}

		// 이동 속도 계산
		float moveX = dirX * m_pSpeeds[i] * fTimeDelta;
		float moveY = dirY * m_pSpeeds[i] * fTimeDelta;

		// 파티클의 현재 위치 업데이트
		pMatrices[i].vTranslation.x += moveX;
		pMatrices[i].vTranslation.y += moveY;

		// 생명 시간 업데이트
		pMatrices[i].vLifeTime.y += fTimeDelta;

		// 루핑 처리
		if (m_isLoop && pMatrices[i].vLifeTime.y >= pMatrices[i].vLifeTime.x)
		{
			// 초기 위치로 리셋
			pMatrices[i].vTranslation.x = m_pInstanceVertices[i].vTranslation.x;
			pMatrices[i].vTranslation.y = m_pInstanceVertices[i].vTranslation.y;
			pMatrices[i].vLifeTime.y = 0.f;
		}
	}

	m_fElapsedTime += fTimeDelta;

	m_pContext->Unmap(m_pVBInstance, 0);
	return false;
}

_bool CVIBuffer_Instancing::Spiral_Spread_2D(_float fTimeDelta)
{
	//MaxTime을 넘어서면 모든 파티클 입자가 다 시간이 지났다고 판단
	if (m_fElapsedTime >= m_fMaxTime)
		return true;

	D3D11_MAPPED_SUBRESOURCE MappedSubResource{};

	m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &MappedSubResource);

	VTXINSTANCE* pMatrices = static_cast<VTXINSTANCE*>(MappedSubResource.pData);

	const float spiralSpeed = XM_PI; // 회전 속도 (라디안 단위)

	for (size_t i = 0; i < m_iNumInstance; i++)
	{
		// 루프가 안돌지만 라이프타임 시간을 넘어섰을 때
		if (!m_isLoop && pMatrices[i].vLifeTime.y >= pMatrices[i].vLifeTime.x)
			continue;

		float currentX = m_pInstanceVertices[i].vTranslation.x;
		float currentY = m_pInstanceVertices[i].vTranslation.y;

		float pivotX = m_vPivotPos.x;
		float pivotY = m_vPivotPos.y;

		float dirX = currentX - pivotX;
		float dirY = currentY - pivotY;

		// 각도 계산
		float angle = atan2(dirY, dirX);
		angle += spiralSpeed * fTimeDelta; // 회전

		// 새로운 방향 벡터 계산
		dirX = cosf(angle);
		dirY = sinf(angle);

		// 이동 속도 계산
		float moveX = dirX * m_pSpeeds[i] * fTimeDelta;
		float moveY = dirY * m_pSpeeds[i] * fTimeDelta;

		// 파티클의 현재 위치 업데이트
		pMatrices[i].vTranslation.x += moveX;
		pMatrices[i].vTranslation.y += moveY;

		// 생명 시간 업데이트
		pMatrices[i].vLifeTime.y += fTimeDelta;

		// 루핑 처리
		if (m_isLoop && pMatrices[i].vLifeTime.y >= pMatrices[i].vLifeTime.x)
		{
			pMatrices[i].vTranslation.x = m_pInstanceVertices[i].vTranslation.x;
			pMatrices[i].vTranslation.y = m_pInstanceVertices[i].vTranslation.y;
			pMatrices[i].vLifeTime.y = 0.f;
		}
	}

	m_fElapsedTime += fTimeDelta;

	m_pContext->Unmap(m_pVBInstance, 0);
	return false;
}

_bool CVIBuffer_Instancing::Circular_Spread_2D(_float fTimeDelta)
{
	//MaxTime을 넘어서면 모든 파티클 입자가 다 시간이 지났다고 판단
	if (m_fElapsedTime >= m_fMaxTime)
		return true;

	D3D11_MAPPED_SUBRESOURCE MappedSubResource{};

	m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &MappedSubResource);

	VTXINSTANCE* pMatrices = static_cast<VTXINSTANCE*>(MappedSubResource.pData);

	for (size_t i = 0; i < m_iNumInstance; i++)
	{
		// 루프가 안돌지만 라이프타임 시간을 넘어섰을 때
		if (!m_isLoop && pMatrices[i].vLifeTime.y >= pMatrices[i].vLifeTime.x)
			continue;

		float currentX = m_pInstanceVertices[i].vTranslation.x;
		float currentY = m_pInstanceVertices[i].vTranslation.y;

		float pivotX = m_vPivotPos.x;
		float pivotY = m_vPivotPos.y;

		float dirX = currentX - pivotX;
		float dirY = currentY - pivotY;

		// 각도 랜덤화 (0도 ~ 360도)
		float angle = Get_Random(0.f, 360.f) * (XM_PI / 180.f);
		dirX = cosf(angle);
		dirY = sinf(angle);

		// 벡터 정규화
		float length = sqrt(dirX * dirX + dirY * dirY);
		if (length != 0.f)
		{
			dirX /= length;
			dirY /= length;
		}

		// 이동 속도 계산
		float moveX = dirX * m_pSpeeds[i] * fTimeDelta;
		float moveY = dirY * m_pSpeeds[i] * fTimeDelta;

		// 파티클의 현재 위치 업데이트
		pMatrices[i].vTranslation.x += moveX;
		pMatrices[i].vTranslation.y += moveY;

		// 생명 시간 업데이트
		pMatrices[i].vLifeTime.y += fTimeDelta;

		// 루핑 처리
		if (m_isLoop && pMatrices[i].vLifeTime.y >= pMatrices[i].vLifeTime.x)
		{
			pMatrices[i].vTranslation.x = m_pInstanceVertices[i].vTranslation.x;
			pMatrices[i].vTranslation.y = m_pInstanceVertices[i].vTranslation.y;
			pMatrices[i].vLifeTime.y = 0.f;
		}
	}

	m_fElapsedTime += fTimeDelta;

	m_pContext->Unmap(m_pVBInstance, 0);
	return false;
}

_bool CVIBuffer_Instancing::Elliptical_Spread_2D(_float fTimeDelta)
{
	//MaxTime을 넘어서면 모든 파티클 입자가 다 시간이 지났다고 판단
	if (m_fElapsedTime >= m_fMaxTime)
		return true;

	D3D11_MAPPED_SUBRESOURCE MappedSubResource{};

	m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &MappedSubResource);

	VTXINSTANCE* pMatrices = static_cast<VTXINSTANCE*>(MappedSubResource.pData);

	for (size_t i = 0; i < m_iNumInstance; i++)
	{
		// 루프가 안돌지만 라이프타임 시간을 넘어섰을 때
		if (!m_isLoop && pMatrices[i].vLifeTime.y >= pMatrices[i].vLifeTime.x)
			continue;

		float currentX = m_pInstanceVertices[i].vTranslation.x;
		float currentY = m_pInstanceVertices[i].vTranslation.y;

		float pivotX = m_vPivotPos.x;
		float pivotY = m_vPivotPos.y;

		float dirX = currentX - pivotX;
		float dirY = currentY - pivotY;

		// 타원형 스프레드를 위해 Y축 속도 감소
		float ellipseFactor = 0.5f; // Y축 속도 비율
		dirY *= ellipseFactor;

		// 벡터 정규화
		float length = sqrt(dirX * dirX + dirY * dirY);
		if (length != 0.f)
		{
			dirX /= length;
			dirY /= length;
		}

		// 이동 속도 계산
		float moveX = dirX * m_pSpeeds[i] * fTimeDelta;
		float moveY = dirY * m_pSpeeds[i] * fTimeDelta;

		// 파티클의 현재 위치 업데이트
		pMatrices[i].vTranslation.x += moveX;
		pMatrices[i].vTranslation.y += moveY;

		// 생명 시간 업데이트
		pMatrices[i].vLifeTime.y += fTimeDelta;

		// 루핑 처리
		if (m_isLoop && pMatrices[i].vLifeTime.y >= pMatrices[i].vLifeTime.x)
		{
			pMatrices[i].vTranslation.x = m_pInstanceVertices[i].vTranslation.x;
			pMatrices[i].vTranslation.y = m_pInstanceVertices[i].vTranslation.y;
			pMatrices[i].vLifeTime.y = 0.f;
		}
	}

	m_fElapsedTime += fTimeDelta;
	m_pContext->Unmap(m_pVBInstance, 0);
	return false;
}

_bool CVIBuffer_Instancing::Random_Wiggle_Spread_2D(_float fTimeDelta)
{
	//MaxTime을 넘어서면 모든 파티클 입자가 다 시간이 지났다고 판단
	if (m_fElapsedTime >= m_fMaxTime)
		return true;

	D3D11_MAPPED_SUBRESOURCE MappedSubResource{};

	m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &MappedSubResource);

	VTXINSTANCE* pMatrices = static_cast<VTXINSTANCE*>(MappedSubResource.pData);

	const float scaleFactor = 50.f; // 진동 강도 스케일 팩터

	for (size_t i = 0; i < m_iNumInstance; i++)
	{
		// 루프가 안돌지만 라이프타임 시간을 넘어섰을 때
		if (!m_isLoop && pMatrices[i].vLifeTime.y >= pMatrices[i].vLifeTime.x)
			continue;

		float currentX = m_pInstanceVertices[i].vTranslation.x;
		float currentY = m_pInstanceVertices[i].vTranslation.y;

		float pivotX = m_vPivotPos.x;
		float pivotY = m_vPivotPos.y;

		float dirX = currentX - pivotX;
		float dirY = currentY - pivotY;

		// 벡터 정규화
		float length = sqrt(dirX * dirX + dirY * dirY);
		if (length != 0.f)
		{
			dirX /= length;
			dirY /= length;
		}

		// 진동 강도를 속도와 연동
		float wiggleIntensity = m_pSpeeds[i] * scaleFactor;

		// 랜덤 진동 추가
		float wiggleX = ((rand() % 100) / 100.f - 0.5f) * wiggleIntensity;
		float wiggleY = ((rand() % 100) / 100.f - 0.5f) * wiggleIntensity;

		// 이동 속도 계산
		float moveX = dirX * m_pSpeeds[i] * fTimeDelta + wiggleX * fTimeDelta;
		float moveY = dirY * m_pSpeeds[i] * fTimeDelta + wiggleY * fTimeDelta;

		// 파티클의 현재 위치 업데이트
		pMatrices[i].vTranslation.x += moveX;
		pMatrices[i].vTranslation.y += moveY;

		// 생명 시간 업데이트
		pMatrices[i].vLifeTime.y += fTimeDelta;

		// 루핑 처리
		if (m_isLoop && pMatrices[i].vLifeTime.y >= pMatrices[i].vLifeTime.x)
		{
			pMatrices[i].vTranslation.x = m_pInstanceVertices[i].vTranslation.x;
			pMatrices[i].vTranslation.y = m_pInstanceVertices[i].vTranslation.y;
			pMatrices[i].vLifeTime.y = 0.f;
		}
	}

	m_fElapsedTime += fTimeDelta;
	m_pContext->Unmap(m_pVBInstance, 0);
	return false;
}

_bool CVIBuffer_Instancing::Random_Wiggle_Half_Spread_2D(_float fTimeDelta)
{
	D3D11_MAPPED_SUBRESOURCE MappedSubResource{};

	m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &MappedSubResource);

	VTXINSTANCE* pMatrices = static_cast<VTXINSTANCE*>(MappedSubResource.pData);

	const float scaleFactor = 50.f; // 진동 강도 스케일 팩터

	for (size_t i = 0; i < m_iNumInstance; i++)
	{
		float currentX = m_pInstanceVertices[i].vTranslation.x;
		float currentY = m_pInstanceVertices[i].vTranslation.y;

		float pivotX = m_vPivotPos.x;
		float pivotY = m_vPivotPos.y;

		float dirX = currentX - pivotX;
		float dirY = currentY - pivotY;

		// 벡터 정규화
		float length = sqrt(dirX * dirX + dirY * dirY);
		if (length != 0.f)
		{
			dirX /= length;
			dirY /= length;
		}

		// 진동 강도를 속도와 연동
		float wiggleIntensity = m_pSpeeds[i] * scaleFactor;

		// 랜덤 진동 추가
		float wiggleX = ((rand() % 100) / 100.f - 0.5f) * wiggleIntensity;
		float wiggleY = ((rand() % 100) / 100.f - 0.5f) * wiggleIntensity;

		// 이동 속도 계산
		float moveX = dirX * m_pSpeeds[i] * fTimeDelta + wiggleX * fTimeDelta;
		float moveY = dirY * m_pSpeeds[i] * fTimeDelta + wiggleY * fTimeDelta;

		if (moveX < 0.f)
			moveX = -moveX;

		// 파티클의 현재 위치 업데이트
		pMatrices[i].vTranslation.x += moveX;
		pMatrices[i].vTranslation.y += moveY;

		// 생명 시간 업데이트
		pMatrices[i].vLifeTime.y += fTimeDelta;

		// 루핑 처리
		if (m_isLoop && pMatrices[i].vLifeTime.y >= pMatrices[i].vLifeTime.x)
		{
			pMatrices[i].vTranslation.x = m_pInstanceVertices[i].vTranslation.x;
			pMatrices[i].vTranslation.y = m_pInstanceVertices[i].vTranslation.y;
			pMatrices[i].vLifeTime.y = 0.f;
		}
		else if (!m_isLoop && pMatrices[i].vLifeTime.y >= pMatrices[i].vLifeTime.x)
		{
			m_pContext->Unmap(m_pVBInstance, 0);
			return true;
		}
	}

	m_pContext->Unmap(m_pVBInstance, 0);
	return false;
}

void CVIBuffer_Instancing::Drop(_float fTimeDelta)
{
	D3D11_MAPPED_SUBRESOURCE		MappedSubResource{};

	m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &MappedSubResource);

	VTXINSTANCE* pMatrices = static_cast<VTXINSTANCE*>(MappedSubResource.pData);

	for (size_t i = 0; i < m_iNumInstance; i++)
	{
		// _vector		vMoveDir = XMVectorSet(Get_Random(0.f, 2.f) - 1.f, -1.f, Get_Random(0.f, 2.f) - 1.f, 0.f);
		_vector		vMoveDir = XMVectorSet(0.0f, -1.f, 0.0f, 0.f);

		XMStoreFloat4(&pMatrices[i].vTranslation,
			XMLoadFloat4(&pMatrices[i].vTranslation) + vMoveDir * m_pSpeeds[i] * fTimeDelta);

		pMatrices[i].vLifeTime.y += fTimeDelta;
		if (m_isLoop == true && pMatrices[i].vLifeTime.y >= pMatrices[i].vLifeTime.x)
		{
			pMatrices[i].vTranslation = m_pInstanceVertices[i].vTranslation;
			pMatrices[i].vLifeTime.y = 0.f;
		}
	}

	m_pContext->Unmap(m_pVBInstance, 0);

}

void CVIBuffer_Instancing::MoveDir(_vector vDir, _float fTimeDelta)
{
	D3D11_MAPPED_SUBRESOURCE        MappedSubResource{};

	m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &MappedSubResource);

	VTXINSTANCE* pMatrices = static_cast<VTXINSTANCE*>(MappedSubResource.pData);

	for (size_t i = 0; i < m_iNumInstance; i++)
	{
		_vector        vMoveDir = vDir;

		XMStoreFloat4(&pMatrices[i].vTranslation,
			XMLoadFloat4(&pMatrices[i].vTranslation) + vMoveDir * m_pSpeeds[i] * fTimeDelta);

		pMatrices[i].vLifeTime.y += fTimeDelta;
		if (m_isLoop == true && pMatrices[i].vLifeTime.y >= pMatrices[i].vLifeTime.x)
		{
			pMatrices[i].vTranslation = m_pInstanceVertices[i].vTranslation;
			pMatrices[i].vLifeTime.y = 0.f;
		}
	}

	m_pContext->Unmap(m_pVBInstance, 0);
}

_float CVIBuffer_Instancing::Get_RandomNormalize()
{
	return (_float)rand() / RAND_MAX;
}

_float CVIBuffer_Instancing::Get_Random(_float fMin, _float fMax)
{
	return fMin + (fMax - fMin) * Get_RandomNormalize();
}

void CVIBuffer_Instancing::Free()
{
	__super::Free();

	Safe_Release(m_pVBInstance);

	if (false == m_isCloned)
	{
		Safe_Delete_Array(m_pInstanceVertices);
		Safe_Delete_Array(m_pSpeeds);
	}
}
