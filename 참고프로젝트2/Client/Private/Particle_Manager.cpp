#include "stdafx.h"
#include "Particle_Manager.h"
#include "GameInstance.h"
#include "Particle.h"

IMPLEMENT_SINGLETON(CParticle_Manager)

CParticle_Manager::CParticle_Manager()
	: m_pGameInstance{ CGameInstance::Get_Instance() }
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT CParticle_Manager::Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CParticle* pParticle = nullptr;
	CParticle::PARTICLE_DESC Desc{};

	// 각 파티클 타입별로 최소 5개의 파티클 객체를 미리 생성하여 풀에 추가
	for (int i = 0; i < 3; ++i)
	{
		//프리저 1필
		Desc.fXScale = 7.f;
		Desc.fYScale = 0.5f;
		Desc.fGlowFactor = 15.f;
		Desc.iPassIndex = 2;
		Desc.vColor = _float4(0.4f, 0.f, 1.f, 1.0f);

		pParticle = static_cast<CParticle*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Particle_Focus"), &Desc));
		pParticle->Set_Particle_Active(false);
		m_ParticlePools[FREIZA_ULTIMATE_1_PARTICLE].push_back(pParticle);

		//프리저 3필
		Desc.fXScale = 7.f;
		Desc.fYScale = 0.5f;
		Desc.fGlowFactor = 15.f;
		Desc.iPassIndex = 2;
		Desc.vColor = _float4(0.4f, 0.f, 1.f, 1.0f);

		pParticle = static_cast<CParticle*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Particle_Spread"), &Desc));
		pParticle->Set_Particle_Active(false);
		m_ParticlePools[FREIZA_ULTIMATE_3_PARTICLE].push_back(pParticle);

		//프리저 1필 히트
		Desc.fXScale = 7.f;
		Desc.fYScale = 0.5f;
		Desc.fGlowFactor = 15.f;
		Desc.iPassIndex = 2;
		Desc.vColor = _float4(0.4f, 0.f, 1.f, 1.0f);

		pParticle = static_cast<CParticle*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Particle_Frieza_1_Ultimate_Hit"), &Desc));
		pParticle->Set_Particle_Active(false);
		m_ParticlePools[FREIZA_ULTIMATE_1_HIT_PARTICLE].push_back(pParticle);

		//프리저 3필 히트
		Desc.fXScale = 7.f;
		Desc.fYScale = 0.5f;
		Desc.fGlowFactor = 15.f;
		Desc.iPassIndex = 2;
		Desc.vColor = _float4(0.4f, 0.f, 1.f, 1.0f);

		pParticle = static_cast<CParticle*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Particle_Frieza_3_Ultimate_Hit"), &Desc));
		pParticle->Set_Particle_Active(false);
		m_ParticlePools[FREIZA_ULTIMATE_3_HIT_PARTICLE].push_back(pParticle);

		//21 3필
		Desc.fXScale = 1.f;
		Desc.fYScale = 0.07f;
		Desc.fGlowFactor = 15.f;
		Desc.iPassIndex = 2;
		Desc.vColor = _float4(1.f, 0.f, 0.8f, 1.0f);

		pParticle = static_cast<CParticle*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Particle_21_3_Ultimate"), &Desc));
		pParticle->Set_Particle_Active(false);
		m_ParticlePools[ULTIMATE_3_21_PARTICLE].push_back(pParticle);
	}

	

	for (size_t i = 0; i < 10; i++)
	{
		//공통 Hit
		Desc.fXScale = 3.f;
		Desc.fYScale = 0.5f;
		Desc.fGlowFactor = 20.f;
		Desc.iPassIndex = 2;
		Desc.vColor = _float4(1.f, 1.f, 0.f, 1.0f);

		pParticle = static_cast<CParticle*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Particle_Common_Hit"), &Desc));
		pParticle->Set_Particle_Active(false);
		m_ParticlePools[COMMON_HIT_PARTICLE].push_back(pParticle);
	}
	return S_OK;
}

void CParticle_Manager::Camera_Update(_float fTimeDelta)
{
}


void CParticle_Manager::Update(_float fTimeDelta)
{
	for (auto& poolPair : m_ParticlePools)
	{
		auto& pool = poolPair.second;
		for (auto& particle : pool)
		{
			if (particle->IsActive())
			{
				particle->Update(fTimeDelta);
			}
		}
	}
}

void CParticle_Manager::Late_Update(_float fTimeDelta)
{
	for (auto& poolPair : m_ParticlePools)
	{
		auto& pool = poolPair.second;
		for (auto& particle : pool)
		{
			if (particle->IsActive())
			{
				particle->Late_Update(fTimeDelta);
			}
		}
	}
}

HRESULT CParticle_Manager::Render(_float fTimeDelta)
{
	return S_OK;
}

HRESULT CParticle_Manager::Play(PARTICLE_ID eID, const _float3& vPosition)
{
	auto& pool = m_ParticlePools[eID];
	CParticle* pParticle = nullptr;

	//풀의 사이즈가 0이 아닌상황에
	if (pool.size() != 0)
	{
		// 비활성화된 파티클을 풀에서 검색
		for (auto& particle : pool)
		{
			if (!particle->IsActive())
			{
				pParticle = particle;
				break;
			}
		}
	}

	if (pParticle == nullptr)
	{
		CParticle::PARTICLE_DESC Desc{};

		// 비활성화된 파티클이 없으면 새로 생성
		switch (eID)
		{
		case FREIZA_ULTIMATE_3_PARTICLE:
			//프리저 3필
			Desc.fXScale = 7.f;
			Desc.fYScale = 0.5f;
			Desc.fGlowFactor = 15.f;
			Desc.iPassIndex = 2;
			Desc.vColor = _float4(0.4f, 0.f, 1.f, 1.0f);

			pParticle = static_cast<CParticle*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Particle_Spread"), &Desc));
			pool.push_back(pParticle);
			break;

		case FREIZA_ULTIMATE_1_PARTICLE:
			//프리저 1필
			Desc.fXScale = 7.f;
			Desc.fYScale = 0.5f;
			Desc.fGlowFactor = 15.f;
			Desc.iPassIndex = 2;
			Desc.vColor = _float4(0.4f, 0.f, 1.f, 1.0f);

			pParticle = static_cast<CParticle*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Particle_Focus"), &Desc));
			pool.push_back(pParticle);
			break;

		case FREIZA_ULTIMATE_1_HIT_PARTICLE:
			//프리저 1필 히트
			Desc.fXScale = 7.f;
			Desc.fYScale = 0.5f;
			Desc.fGlowFactor = 15.f;
			Desc.iPassIndex = 2;
			Desc.vColor = _float4(0.4f, 0.f, 1.f, 1.0f);

			pParticle = static_cast<CParticle*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Particle_Frieza_1_Ultimate_Hit"), &Desc));
			pool.push_back(pParticle);
			break;

		case FREIZA_ULTIMATE_3_HIT_PARTICLE:
			//프리저 3필 히트
			Desc.fXScale = 7.f;
			Desc.fYScale = 0.5f;
			Desc.fGlowFactor = 15.f;
			Desc.iPassIndex = 2;
			Desc.vColor = _float4(0.4f, 0.f, 1.f, 1.0f);

			pParticle = static_cast<CParticle*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Particle_Frieza_3_Ultimate_Hit"), &Desc));
			pool.push_back(pParticle);
			break;

		case ULTIMATE_3_21_PARTICLE:
			//21 3필
			Desc.fXScale = 7.f;
			Desc.fYScale = 0.5f;
			Desc.fGlowFactor = 15.f;
			Desc.iPassIndex = 2;
			Desc.vColor = _float4(0.4f, 0.f, 1.f, 1.0f);

			pParticle = static_cast<CParticle*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Particle_21_3_Ultimate"), &Desc));
			pool.push_back(pParticle);
			break;

		case COMMON_HIT_PARTICLE:
			//공통 Hit
			Desc.fXScale = 3.f;
			Desc.fYScale = 0.5f;
			Desc.fGlowFactor = 20.f;
			Desc.iPassIndex = 2;
			Desc.vColor = _float4(1.f, 1.f, 0.f, 1.0f);

			pParticle = static_cast<CParticle*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Particle_Common_Hit"), &Desc));
			pool.push_back(pParticle);
			break;
		}
	}

	//파티클 활성화 및 초기화
	pParticle->Set_Particle_Active(true); // 초기에는 비활성화 상태
	pParticle->Set_Position(vPosition);

	// 추가적인 초기화가 필요하면 여기에 구현

	return S_OK;
}

HRESULT CParticle_Manager::Play(PARTICLE_ID eID, const _vector& vPosition)
{
	auto& pool = m_ParticlePools[eID];
	CParticle* pParticle = nullptr;

	//풀의 사이즈가 0이 아닌상황에
	if (pool.size() != 0)
	{
		// 비활성화된 파티클을 풀에서 검색
		for (auto& particle : pool)
		{
			if (!particle->IsActive())
			{
				pParticle = particle;
				break;
			}
		}
	}

	if (pParticle == nullptr)
	{
		CParticle::PARTICLE_DESC Desc{};

		// 비활성화된 파티클이 없으면 새로 생성
		switch (eID)
		{
		case FREIZA_ULTIMATE_3_PARTICLE:
			//프리저 3필
			Desc.fXScale = 7.f;
			Desc.fYScale = 0.5f;
			Desc.fGlowFactor = 15.f;
			Desc.iPassIndex = 2;
			Desc.vColor = _float4(0.4f, 0.f, 1.f, 1.0f);

			pParticle = static_cast<CParticle*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Particle_Spread"), &Desc));
			pool.push_back(pParticle);
			break;

		case FREIZA_ULTIMATE_1_PARTICLE:
			//프리저 1필
			Desc.fXScale = 7.f;
			Desc.fYScale = 0.5f;
			Desc.fGlowFactor = 15.f;
			Desc.iPassIndex = 2;
			Desc.vColor = _float4(0.4f, 0.f, 1.f, 1.0f);

			pParticle = static_cast<CParticle*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Particle_Focus"), &Desc));
			pool.push_back(pParticle);
			break;

		case FREIZA_ULTIMATE_1_HIT_PARTICLE:
			//프리저 1필 히트
			Desc.fXScale = 7.f;
			Desc.fYScale = 0.5f;
			Desc.fGlowFactor = 15.f;
			Desc.iPassIndex = 2;
			Desc.vColor = _float4(0.4f, 0.f, 1.f, 1.0f);

			pParticle = static_cast<CParticle*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Particle_Frieza_1_Ultimate_Hit"), &Desc));
			pool.push_back(pParticle);
			break;

		case FREIZA_ULTIMATE_3_HIT_PARTICLE:
			//프리저 3필 히트
			Desc.fXScale = 7.f;
			Desc.fYScale = 0.5f;
			Desc.fGlowFactor = 15.f;
			Desc.iPassIndex = 2;
			Desc.vColor = _float4(0.4f, 0.f, 1.f, 1.0f);

			pParticle = static_cast<CParticle*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Particle_Frieza_3_Ultimate_Hit"), &Desc));
			pool.push_back(pParticle);
			break;

		case ULTIMATE_3_21_PARTICLE:
			//21 3필
			Desc.fXScale = 7.f;
			Desc.fYScale = 0.5f;
			Desc.fGlowFactor = 15.f;
			Desc.iPassIndex = 2;
			Desc.vColor = _float4(0.4f, 0.f, 1.f, 1.0f);

			pParticle = static_cast<CParticle*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Particle_21_3_Ultimate"), &Desc));
			pool.push_back(pParticle);
			break;

		case COMMON_HIT_PARTICLE:
			//공통 Hit
			Desc.fXScale = 3.f;
			Desc.fYScale = 0.5f;
			Desc.fGlowFactor = 20.f;
			Desc.iPassIndex = 2;
			Desc.vColor = _float4(1.f, 1.f, 0.f, 1.0f);

			pParticle = static_cast<CParticle*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Particle_Common_Hit"), &Desc));
			pool.push_back(pParticle);
			break;
		}
	}

	//파티클 활성화 및 초기화
	pParticle->Set_Particle_Active(true); // 초기에는 비활성화 상태
	pParticle->Set_Position(vPosition);

	// 추가적인 초기화가 필요하면 여기에 구현

	return S_OK;
}

void CParticle_Manager::Stop(PARTICLE_ID eID)
{
	auto& pool = m_ParticlePools[eID];

	for (auto& iter : pool)
	{
		if (iter->IsActive())
			iter->Set_Particle_Active(false);
	}
}


void CParticle_Manager::Free()
{
	Safe_Release(m_pGameInstance);

	for (auto& poolPair : m_ParticlePools)
	{
		auto& pool = poolPair.second;
		for (auto& particle : pool)
		{
			Safe_Release(particle);
		}
		pool.clear();
	}

	__super::Free();
}
