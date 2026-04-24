#pragma once
#include "Component.h"
#include "UITween.h"

NS_BEGIN(Engine)

class ENGINE_DLL CUIAnimator final : public CComponent
{
public:
	struct UIANIMATOR_DESC
	{
		class CUIObject* pOwner = { nullptr };
	};

protected:
	CUIAnimator(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUIAnimator(const CUIAnimator& Prototype);
	virtual ~CUIAnimator() = default;

public:
	_bool Has_Animation(const _wstring& strName) const;
	const unordered_map<_wstring, vector<CUITween::UITWEEN_DESC>>&
		Get_Animations() const { return m_NamedAnimations; }
	_bool Remove_Animation(const _wstring& strName);
	_bool Rename_Animation(const _wstring& strOld, const _wstring& strNew);
	void  Clear_Animations();

	_bool Is_Playing() const { return !m_ActiveTweens.empty(); }

	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;

	void Tick(_float fTimeDelta);
	_int Play_Tween(const CUITween::UITWEEN_DESC& tDesc);
	void Stop_Tween(_int iHandle);
	void Stop_All();

	HRESULT Register_Animation(const _wstring& strName,
	const vector<CUITween::UITWEEN_DESC>& vTracks);
	void Play_Animation(const _wstring& strName);
	void Stop_Animation(const _wstring& strName);

private:
	struct ACTIVE_TWEEN
	{
		_int iHandle{ 0 };
		_wstring strSource{};
		CUITween* pTween{ nullptr };
	};

	class CUIObject* m_pOwner = { nullptr };
	vector<ACTIVE_TWEEN> m_ActiveTweens;
	unordered_map<_wstring, vector<CUITween::UITWEEN_DESC>> m_NamedAnimations;
	_int m_iNextHandle = { 1 };

private:
	_int Activate_Tween(const CUITween::UITWEEN_DESC& tDesc, const _wstring& strName = L"Default_Tween_Name");
	void Purge_FinishedTweens();
	_bool Is_Animation_Active(const _wstring& strName) const;

public:
	static CUIAnimator* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CComponent* Clone(void* pArg) override;

protected:
	virtual void Free() override;
};

NS_END