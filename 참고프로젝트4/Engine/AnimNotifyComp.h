#pragma once

#include "Component.h"
#include "AnimNotify.h"
#include "Utility/DelegateTemplate.h"

BEGIN(Engine)

typedef FastDelegate0<shared_ptr<CAnimNotifyCollection>>		NotifyRegisterDelegate;

/// <summary>
/// 노티파이 컬렉션을 저장하고 그에 대한 처리를 진행하는 컴포넌트
/// </summary>
class ENGINE_DLL CAnimNotifyComp : public CComponent
{
	INFO_CLASS(CAnimNotifyComp, CComponent)
public:

protected:
	explicit CAnimNotifyComp(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	explicit CAnimNotifyComp(const CAnimNotifyComp& rhs);
	virtual ~CAnimNotifyComp() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize_Prototype(const wstring& strNotifyFilePath, NotifyRegisterDelegate Register);
	virtual HRESULT Initialize(void* pArg = nullptr) override;

public:
	static shared_ptr<CAnimNotifyComp> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	static shared_ptr<CAnimNotifyComp> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, 
		const wstring& strNotifyFilePath, NotifyRegisterDelegate Register);
	virtual shared_ptr<CComponent> Clone(void* Arg = nullptr);

protected:
	virtual void	Free();

public:
	using SNotifyCollections = _unmap<string, shared_ptr<CAnimNotifyCollection>>;

public:
	// 자동 등록용 콜백함수 등록기
	HRESULT Regist_AnimNotifyCreatorCallback(NotifyRegisterDelegate Callback);
	// 노티파이 내보내기
	HRESULT Output_AnimNotify(const wstring& strNotifyFilePath);
	// 노티파이 로드
	HRESULT Load_AnimNotify(const wstring& strNotifyFilePath);
	// 모델 등록
	HRESULT Regist_ModelComponent(weak_ptr<CCommonModelComp> pModelComp);
	// 노티파이에 함수 등록, 런타임에 등록해야함.
	HRESULT Regist_EventToNotify(const string& strAnimName, const string& strGroupName, 
		const string& strNotifyName, FastDelegate0<> Event);
	// 모든 노티파이를 비운다.
	void Clear_NotifyAll();

	// 현재 애니메이션의 노티파이
	virtual void Trigger_AnimNotifyByCurAnim(const _float& fTrackPos);
	// 현재 트랙의 트리거를 리셋 시킨다.
	virtual void ResetTrigger_AnimNotifyByCurAnim();
	void Set_CurrentAnimation(const string& strAnimName);

public:
	// 툴에서 쓰임
	void Reserve_Collection(const string& strCollectionName);
	shared_ptr<CAnimNotifyCollection> Get_CollectionByCurAnim();

protected:
	SNotifyCollections			m_NotifyCollections;		// 노티파이 모음집
	NotifyRegisterDelegate		m_RegisterCallback;

	string						m_strAnimName = "";			// 재생중인 애니메이션 이름
	weak_ptr<CCommonModelComp>	m_pModelComp;				// 모델에 대한 약참조

};

END