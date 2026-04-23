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
	return S_OK;
}

void CUIAnimator::Tick(_float fTimeDelta, CUIObject* pOwner)
{
	if (nullptr == pOwner)
		return;

	for (auto& entry : m_ActiveTweens)
	{
		if (entry.pTween) entry.pTween->Tick(fTimeDelta);
	}

	auto itEnd = remove_if(m_ActiveTweens.begin(), m_ActiveTweens.end(),
		[](ACTIVE_TWEEN& e) -> _bool
		{
			if (nullptr == e.pTween) return true;
			if (e.pTween->Is_Finished())
			{
				Safe_Release(e.pTween);
				return true;
			}
			return false;
		});

	m_ActiveTweens.erase(itEnd, m_ActiveTweens.end());
}

_int CUIAnimator::Play_Tween(const CUITween::UITWEEN_DESC& tDesc, CUIObject* pOwner)
{
	if (nullptr == pOwner)
		return 0;

	CUITween* pTween = CUITween::Create(pOwner, tDesc);
	if (nullptr == pTween)
		return 0;

	ACTIVE_TWEEN entry{};
	entry.iHandle = m_iNextHandle++;
	entry.strSource.clear();
	entry.pTween = pTween;  // Create 반환 시 refcount=1, 추가 AddRef 불필요
	m_ActiveTweens.push_back(entry);

	return entry.iHandle;
}

void CUIAnimator::Stop_Tween(_int iHandle)
{
	for (auto& entry : m_ActiveTweens)
	{
		if (entry.iHandle == iHandle && entry.pTween)
			entry.pTween->Stop();
	}
}

void CUIAnimator::Stop_All()
{
	for (auto& entry : m_ActiveTweens)
	{
		if (entry.pTween) entry.pTween->Stop();
	}
}

HRESULT CUIAnimator::Register_Animation(const _wstring& strName,
	const vector<CUITween::UITWEEN_DESC>& vTracks)
{
	if (strName.empty() || vTracks.empty())
		return E_FAIL;

	m_NamedAnimations[strName] = vTracks;
	return S_OK;
}

void CUIAnimator::Play_Animation(const _wstring& strName, CUIObject* pOwner)
{
	if (nullptr == pOwner)
		return;

	auto iter = m_NamedAnimations.find(strName);
	if (iter == m_NamedAnimations.end())
		return;

	for (const auto& tDesc : iter->second)
	{
		CUITween* pTween = CUITween::Create(pOwner, tDesc);
		if (nullptr == pTween) continue;

		ACTIVE_TWEEN entry{};
		entry.iHandle = m_iNextHandle++;
		entry.strSource = strName;
		entry.pTween = pTween;
		m_ActiveTweens.push_back(entry);
	}
}

void CUIAnimator::Stop_Animation(const _wstring& strName)
{
	for (auto& entry : m_ActiveTweens)
	{
		if (entry.strSource == strName && entry.pTween)
			entry.pTween->Stop();
	}
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