#pragma once

#include "Base.h"
#include "Utility/DelegateTemplate.h"
#include "Utility/RapidJson_Utility.h"

BEGIN(Engine)

typedef FastDelegate0<void> NotifyEventDelegate0;

struct FNotifyType
{
	NotifyEventDelegate0	Event;
	string					strName;
};

class CAnimNotifyCollection;

/// <summary>
/// 노티파이의 기본이 되는 클래스
/// 애니메이션의 키프레임을 지날때 이벤트를 발생시킨다.
/// 이벤트는 노티파이를 사용하는 모듈에서 작동시킨다.(애니메이션 컴포넌트)
/// 해당 모듈에서 조건에 따라 발동시키며, 이는 컴포넌트의 정의에 따라 바뀐다.
/// 
/// 노티파이는 애니메이션들과 별도의 트랙을 가지며
/// 트랙이 있다는 가정하에 애니메이션 컴포넌트에서 현재 트랙과 함께 체크한다.
/// 트랙이 존재하면 
/// </summary>
class ENGINE_DLL CAnimNotify abstract : public CBase
{
	INFO_CLASS(CAnimNotify, CBase)

public:
	explicit CAnimNotify();
	explicit CAnimNotify(const CAnimNotify& rhs);
	virtual ~CAnimNotify() = default;

public:
	virtual HRESULT Initialize() PURE;
	virtual HRESULT Initialize(_float fTrackPos, const string& strName);

	// 트리거를 작동하기 전상태로 되돌린다.
	virtual void OnReset();

	// 작동시 발생시킬 기능
	virtual _bool OnTrigger();

public:
	virtual void Free() override;
	virtual shared_ptr<CAnimNotify> Clone() = 0;

public:
	virtual FRapidJson Bake_Notify();

protected:
	class CGameInstance* m_pGI = { nullptr };

public:
	void			Set_NotifyTrackPos(const _float fTrackPos);
	const _float&	Get_NotifyTrackPos() const { return m_fTrackPos; }
	void			Set_NotifyCollection(CAnimNotifyCollection* pNotifyCollection) { m_pCollection = pNotifyCollection; }
	const string&	Get_Name() const { return m_strName; }
	void			Set_Name(const string& strName) { m_strName = strName; }
	HRESULT			Set_Event(NotifyEventDelegate0 Event);

protected:
	FNotifyType*			m_pNotifyData = { nullptr };
	_float					m_fTrackPos = { 0.f };
	CAnimNotifyCollection*	m_pCollection = { nullptr };
	string					m_strName = { "" };				// 노티파이에 할당된 이름

private:
	_bool			m_bIsTriggered = { false };
	

};


/// <summary>
/// 컬렉션은 애님노티파이를 더 쉽게 관리하기 위해 다른 모듈에서 별도로 정의하지 않고도
/// 사용할 수 있도록 캡슐화를 지원하는 클래스입니다.
/// 하나의 컬렉션당 한 애니메이션 세트에 대한 노티파이를 저장합니다.
/// 용도별로 컬렉션을 만들어 사용합니다.
/// ex) 이펙트, 사운드, 이벤트
/// </summary>
class ENGINE_DLL CAnimNotifyCollection abstract : public CBase
{
	INFO_CLASS(CAnimNotifyCollection, CBase)

protected:
	explicit CAnimNotifyCollection();
	explicit CAnimNotifyCollection(const CAnimNotifyCollection& rhs);
	virtual ~CAnimNotifyCollection() = default;

public:
	// 모든 저장된 노티파이를 리셋시킴
	void OnReset();

	// 현재 진행중인 트랙포즈와 저장된 PrevTrackPos를 통해 트리거를 작동
	void OnTrigger(_cref_time m_fTrackPos);

public:
	virtual void Free() override;
	virtual shared_ptr<CAnimNotifyCollection> Clone() = 0;

public:
	using SNotifyContainer = _unmap<string, vector<shared_ptr<CAnimNotify>>>;

public:
	void Regist_ModelComponent(weak_ptr<CCommonModelComp> pModelCom);
	void Reserve_Collection(const string& strNotifyTag);
	void Add_Notify(const string& strNotifyTag, shared_ptr<CAnimNotify>& pNotify);
	SNotifyContainer& Get_NotifyList() { return m_Notifies; }
	void Sort_Notifies();
	void Clear_Collections();
	void Clear_OnlyNotifies();
	vector<shared_ptr<CAnimNotify>>* Get_NotifyGroupPtr(const string& strGroupTag);

	HRESULT Regist_EventToNotify(const string& strGroupName, const string& strNotifyName, FastDelegate0<> Event);
	weak_ptr<CCommonModelComp>& Get_ModelComp() { return m_pModelCom; }
	void Set_AnimName(const string& strAnimName) { m_strAnimName = strAnimName; }
	const string& Get_AnimName() const { return m_strAnimName; }

public:
	FRapidJson Bake_Collection();
	virtual HRESULT Input_Json(FRapidJson& InputData) = 0;

protected:
	_float							m_fPrevTrackPos = { 0.f };			// 트리거를 작동시키기위해 필요한 변수
	SNotifyContainer				m_Notifies;							// 노티파이 관리 컨테이너
	weak_ptr<CCommonModelComp>		m_pModelCom;						// 노티파이들이 이 모델 컴포넌트를 참조한다.
	string							m_strAnimName = { "" };				// 트랙의 이름, 즉 애니메이션 이름

};

END