#pragma once
#include "Transform.h"

NS_BEGIN(Engine)

class ENGINE_DLL CGameObject abstract : public CBase
{
public:
	typedef struct tagGameObjectDesc : public CTransform::TRANSFORM_DESC
	{
		_uint iFlag = { ObjFlag::ACTIVE };
	}GAMEOBJECT_DESC;

protected:
	CGameObject(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	CGameObject(const CGameObject& Prototype);
	virtual ~CGameObject() = default;

public:
	virtual _bool Is_UI() { return false; }
	virtual _string Get_TypeName() const { return "GameObject"; }
	const _wstring& Get_Name() const { return m_strName; }
	void Set_Name(const _wstring& wStr) { m_strName = wStr; }
	CTransform* Get_Transform() const { return m_pTransformCom; }

	_bool Is_Dead() const { return m_iFlag & ObjFlag::DEAD; }
	void Set_Dead() { m_iFlag |= ObjFlag::DEAD; }
	_uint Get_Flag() const { return m_iFlag; }
	void Set_Flag(_uint iFlag) { m_iFlag |= iFlag; }
	void Reset_Flag(_uint iFlag) { m_iFlag &= ~iFlag; }
	_bool Check_Flag(_uint iFlag) const { return (m_iFlag & iFlag) != 0; }

	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Priority_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
	virtual HRESULT Render();

protected:
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };
	class CGameInstance* m_pGameInstance = { nullptr };

	WNameMap<class CComponent*> m_Components;
	class CTransform* m_pTransformCom = { nullptr };
	_wstring m_strName = {};
	_uint m_iFlag = {};

protected:
	HRESULT Add_Component(_uint iLevel, WNameID strProtoTag, WNameID strComTag, CComponent** ppOut, void* pArg = nullptr);
	class CComponent* Find_Component(WNameID strComTag);

public:
	virtual CGameObject* Clone(void* pArg) PURE;

protected:
	virtual void Free() override;
};

NS_END