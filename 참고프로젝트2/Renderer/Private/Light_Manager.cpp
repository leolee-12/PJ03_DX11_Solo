#include "..\Public\Light_Manager.h"
#include "Light.h"

CLight_Manager::CLight_Manager(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: m_pDevice { pDevice }
	, m_pContext { pContext }
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

LIGHT_DESC * CLight_Manager::Get_LightDesc(LIGHT_TYPE eLightType, _uint iLightIndex, string strName)
{
	switch (eLightType)
	{
	case LIGHT_BACKGROUND:
	{
		auto	iter = m_Lights.begin();

		for (size_t i = 0; i < iLightIndex; i++)
			++iter;

		return (*iter)->Get_LightDesc();
	}
	case LIGHT_PLAYER:
	{
		auto iter = m_PlayerLights.find(strName);

		if (iter == m_PlayerLights.end()) 
			return nullptr;
		

		return iter->second->Get_LightDesc();
	}
	case LIGHT_EFFECT:
	{
		auto iter = m_EffectLights.find(strName);

		if (iter == m_EffectLights.end())
			return nullptr;

		return iter->second->Get_LightDesc();
	}
	}
}

void CLight_Manager::Remove_LightDesc(string strName)
{
	m_EffectLights.erase(strName);
}

_int CLight_Manager::Check_EffectLights()
{
	return m_EffectLights.size();
}

HRESULT CLight_Manager::Initialize()
{

	return S_OK;
}

HRESULT CLight_Manager::Add_Light(const LIGHT_DESC & LightDesc)
{
	CLight*			pLight = CLight::Create(LightDesc);
	if (nullptr == pLight)
		return E_FAIL;

	m_Lights.emplace_back(pLight);
	
	return S_OK;
}

void CLight_Manager::BGLight_Pop_Front()
{
	if (NULL != m_Lights.size())
	{
		Safe_Release(m_Lights.front());
		m_Lights.pop_front();
	}
}

HRESULT CLight_Manager::Add_Player_Light(string strKey, const LIGHT_DESC& LightDesc, _float4 vChaseColor, _bool* pisChaseLight)
{
	CLight* pLight = CLight::Create(LightDesc, vChaseColor, pisChaseLight);
	if (nullptr == pLight)
		return E_FAIL;

	m_PlayerLights[strKey] = pLight;
	return S_OK;
}

HRESULT CLight_Manager::Add_Effect_Light(string strKey, const LIGHT_DESC& LightDesc)
{
	CLight* pLight = CLight::Create(LightDesc);
	if (nullptr == pLight)
		return E_FAIL;

	m_EffectLights[strKey] = pLight;
	return S_OK;
}

HRESULT CLight_Manager::Render_Lights(LIGHT_TYPE eLightType, CShader * pShader, CVIBuffer_Rect * pVIBuffer, string strName, _float fTimeDelta)
{
	switch (eLightType)
	{
	case LIGHT_BACKGROUND:
		for (auto& pLight : m_Lights)
			pLight->Render(pShader, pVIBuffer, 1);
		break;
	case LIGHT_PLAYER:
		if (m_PlayerLights.find(strName) == m_PlayerLights.end())
			return S_OK;
		m_PlayerLights[strName]->Render(pShader, pVIBuffer, 5);

		break;
	case LIGHT_EFFECT:
		_int iFrameRenderCount = { 0 };
		for (auto it = m_EffectLights.begin(); it != m_EffectLights.end(); )
		{
			LIGHT_DESC* pLightDesc = it->second->Get_LightDesc();
			pLightDesc->fAccTime += fTimeDelta;

			if (pLightDesc->fAccTime > pLightDesc->fLifeTime)
				it = m_EffectLights.erase(it);
			else
			{
				/* È½¼ö Á¦ÇÑ */
				if(iFrameRenderCount < 2)
					it->second->Render(pShader, pVIBuffer, 6, m_PlayerLights[strName]->Get_LightDesc());
				++it;
				++iFrameRenderCount;
			}
		}
		break;
	}

	return S_OK;
}

CLight_Manager * CLight_Manager::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CLight_Manager*		pInstance = new CLight_Manager(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Created : CLight_Manager"));
		Safe_Release(pInstance);
	}

	return pInstance;
}
void CLight_Manager::Clear_Light()
{
	for (auto& pLight : m_Lights)
		Safe_Release(pLight);

	m_Lights.clear();

	for (auto& pPlayerLight : m_PlayerLights)
		Safe_Release(pPlayerLight.second);

	m_PlayerLights.clear();

	for (auto& pEffectLight : m_EffectLights)
		Safe_Release(pEffectLight.second);

	m_EffectLights.clear();
}
void CLight_Manager::Free()
{
	__super::Free();

	for (auto& pLight : m_Lights)
		Safe_Release(pLight);

	m_Lights.clear();

	for (auto& pPlayerLight : m_PlayerLights)
		Safe_Release(pPlayerLight.second);

	m_PlayerLights.clear();

	for (auto& pEffectLight : m_EffectLights)
		Safe_Release(pEffectLight.second);

	m_EffectLights.clear();

	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}
