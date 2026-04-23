#include "UIAnimator.h"
#include "UIObject.h"

CUIAnimator::CUIAnimator(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent{ pDevice, pContext }
{
}

CUIAnimator::CUIAnimator(const CUIAnimator& Prototype)
	: CComponent{ Prototype }
{
}

HRESULT CUIAnimator::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUIAnimator::Initialize(void* pArg)
{
	if(nullptr != pArg)
	{
		auto pDesc = static_cast<UIANIMATOR_DESC*>(pArg);
		m_pOwner = pDesc->pOwner;
	}

	return S_OK;
}

void CUIAnimator::Tick(_float fTimeDelta)
{
	if (nullptr == m_pOwner)
		return;

	for (auto& entry : m_ActiveTweens)
	{
		if (entry.pTween) entry.pTween->Tick(fTimeDelta);
	}

	Purge_FinishedTweens();
}

_int CUIAnimator::Play_Tween(const CUITween::UITWEEN_DESC& tDesc)
{
	if (nullptr == m_pOwner)
		return 0;

	return Activate_Tween(tDesc);
}

void CUIAnimator::Stop_Tween(_int iHandle)
{
	_bool bStopped = false;

	for (auto& entry : m_ActiveTweens)
	{
		if (entry.iHandle == iHandle && entry.pTween)
		{
			entry.pTween->Stop();
			bStopped = true;
		}
	}

	if(bStopped)
		Purge_FinishedTweens();
}

void CUIAnimator::Stop_All()
{
	_bool bStopped = false;

	for (auto& entry : m_ActiveTweens)
	{
		if (entry.pTween)
		{
			entry.pTween->Stop();
			bStopped = true;
		}
	}

	if (bStopped)
		Purge_FinishedTweens();
}

HRESULT CUIAnimator::Register_Animation(const _wstring& strName, const vector<CUITween::UITWEEN_DESC>& vTracks)
{
	if (strName.empty() || vTracks.empty())
		return E_FAIL;

	m_NamedAnimations[strName] = vTracks;
	return S_OK;
}

void CUIAnimator::Play_Animation(const _wstring& strName)
{
	if (nullptr == m_pOwner)
		return;

	auto iter = m_NamedAnimations.find(strName);
	if (iter == m_NamedAnimations.end())
		return;

	for (const auto& tDesc : iter->second)
		Activate_Tween(tDesc, strName);
}

void CUIAnimator::Stop_Animation(const _wstring& strName)
{
	for (auto& entry : m_ActiveTweens)
	{
		if (entry.strSource == strName && entry.pTween)
			entry.pTween->Stop();
	}
}

_int CUIAnimator::Activate_Tween(const CUITween::UITWEEN_DESC& tDesc, const _wstring& strName)
{
	CUITween* pTween = CUITween::Create(m_pOwner, tDesc);
	if (nullptr == pTween)
		return 0;

	ACTIVE_TWEEN entry{};
	entry.iHandle = m_iNextHandle++;
	entry.strSource = strName;
	entry.pTween = pTween;  // Create 반환 시 refcount=1, 추가 AddRef 불필요
	m_ActiveTweens.push_back(entry);

	return entry.iHandle;
}

void CUIAnimator::Purge_FinishedTweens()
{
	auto itEnd = remove_if(m_ActiveTweens.begin(), m_ActiveTweens.end(),
		[](ACTIVE_TWEEN& entry) -> _bool
		{
			if (nullptr == entry.pTween)
				return true;

			if (entry.pTween->Is_Finished())
			{
				Safe_Release(entry.pTween);
				return true;
			}

			return false;
		});

	m_ActiveTweens.erase(itEnd, m_ActiveTweens.end());
}

CUIAnimator* CUIAnimator::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUIAnimator* pInstance = new CUIAnimator(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Create : CUIAnimator");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CUIAnimator::Clone(void* pArg)
{
	CUIAnimator* pInstance = new CUIAnimator(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CUIAnimator");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUIAnimator::Free()
{
	__super::Free();

	for (auto& entry : m_ActiveTweens)
		Safe_Release(entry.pTween);

	m_ActiveTweens.clear();
	m_NamedAnimations.clear();

}