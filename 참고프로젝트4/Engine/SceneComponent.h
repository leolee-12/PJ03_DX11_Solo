#pragma once

#include "GameObjectComp.h"
#include "Transform.h"
#include "GameObject.h"

BEGIN(Engine)


/// <summary>
/// 내부적으로 트랜스폼 컴포넌트를 포함하는 클래스
/// 이 클래스를 상속받으면 트랜스폼 속성을 같이 가지게 된다.
/// </summary>
class ENGINE_DLL CSceneComponent abstract : public CGameObjectComp
{
	DERIVED_CLASS(CGameObjectComp, CSceneComponent)
protected:
	explicit CSceneComponent(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	explicit CSceneComponent(const CSceneComponent& rhs);
	virtual ~CSceneComponent() = default;

public:
	virtual HRESULT	Initialize_Prototype() override;
	virtual HRESULT Initialize(void* Arg = nullptr) override;
	virtual void	Priority_Tick(const _float& fTimeDelta);
	virtual void	Tick(const _float& fTimeDelta);
	virtual void	Late_Tick(const _float& fTimeDelta);
	virtual HRESULT	Render() PURE;

public:
	virtual shared_ptr<CComponent> Clone(void* Arg = nullptr) PURE;

protected:
	virtual void	Free() override;



#pragma region 씬 컴포넌트 계층
	// 씬 컴포넌트는 계층관계를 가진다. 자식을 여럿 가질 수 있으며, 그에 대한 기능을 제공한다.
public:
	GETSET_EX1(CSceneComponent*, m_pParentSceneComp, ParentSceneComp, GET)

	CSceneComponent*	Get_FirstChildSceneComp();
	CSceneComponent*	Get_ChildSceneComp(_uint iIndex);
	void				Add_ChildSceneComp(CSceneComponent* const pComp);
	_bool				Insert_ChildSceneComp(_uint iIndex, CSceneComponent* const pComp);

protected:
	CSceneComponent*			m_pParentSceneComp = { nullptr };	// 부모 씬 컴포넌트
	vector<CSceneComponent*>	m_vecChildSceneComp;				// 자식 씬 컴포넌트  
#pragma endregion



#pragma region 트랜스폼
public:
	_matrix Calculate_TransformMatrixFromParent();
	_float4x4 Calculate_TransformFloat4x4FromParent();
	// 자신은 뺴고 부모의 트랜스폼만 계산
	_matrix Calculate_TransformMatrixFromParentNoSelf();
	_float4x4 Calculate_TransformFloat4x4FromParentNoSelf();

public:
	CTransform& Transform() { return *m_pTransformComp; }

private:
	shared_ptr<CTransform>		m_pTransformComp = { nullptr };

#pragma endregion


};

END