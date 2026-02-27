#pragma once

#include "Component.h"

BEGIN(Engine)

class CGameObject;

/// <summary>
/// 좌표기능이 빠진 컴포넌트 클래스
/// 로컬 좌표기능이 필요 없는 파생클래스로 사용합니다.
/// </summary>
class ENGINE_DLL CGameObjectComp abstract : public CComponent
{
	DERIVED_CLASS(CComponent, CGameObjectComp)
protected:
	explicit CGameObjectComp(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	explicit CGameObjectComp(const CGameObjectComp& rhs);
	virtual ~CGameObjectComp() = default;

public:
	virtual HRESULT	Initialize_Prototype();
	virtual HRESULT Initialize(void* Arg = nullptr) PURE;
	virtual void	Priority_Tick(const _float& fTimeDelta) PURE;
	virtual void	Tick(const _float& fTimeDelta) PURE;
	virtual void	Late_Tick(const _float& fTimeDelta) PURE;
	virtual HRESULT	Render() PURE;


public:
	virtual shared_ptr<CComponent> Clone(void* Arg = nullptr) PURE;

protected:
	virtual void Free() override;



#pragma region 기본 속성
public:
	GETSET_EX2(wstring, m_strPrototypeName, ProtoName, GET_C_REF, SET_C_REF)

protected:
	wstring	m_strPrototypeName = { TEXT("") };			// 프로토타입 저장시 사용

public:
	_float Get_Priority() const { return m_fPriority; }
	void Set_Priority(_float fPriority) { m_fPriority = fPriority; }

private:	// 기본 속성
	_float					m_fPriority = { 0.f };		// 우선도
#pragma endregion


#pragma region 계층구조
public:
	// 외부에서는 포인터 변경 불가를 조건으로 주소를 얻음
	GETSET_EX2(CGameObject*, m_pOwnerObject, OwnerObject, GET_REF_C, SET__C)

private:
	CGameObject* m_pOwnerObject = { nullptr };			// 소유하고 있는 게임오브젝트, 이는 반드시 누군가 소유해주어야 함.
#pragma endregion

};

END