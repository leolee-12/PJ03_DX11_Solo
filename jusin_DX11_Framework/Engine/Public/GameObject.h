#pragma once
#include "Transform.h"
#include "Component.h"

NS_BEGIN(Engine)
class CGameInstance;

class ENGINE_DLL CGameObject abstract : public CBase
{
public:
	struct GAMEOBJECT_DESC : public CTransform::TRANSFORM_DESC
	{
		_float3 vSpawnPos = {};	// ¿ùµå ÁÂÇ¥
		_uint iFlag = { ObjFlag::ACTIVE };
	};

protected:
	CGameObject(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	CGameObject(const CGameObject& Prototype);
	virtual ~CGameObject() = default;

public:
	virtual _bool Is_UI() { return false; }
	virtual _string Get_TypeName() const { return "GameObject"; }
	virtual _float Get_RenderOrder() const { return m_pTransformCom->Get_Depth(); }
	virtual void On_ViewportResized(_float2 vNewViewport) {}
	const _wstring& Get_Name() const { return m_strName; }
	void Set_Name(const _wstring& wStr) { m_strName = wStr; }
	CTransform* Get_Transform() const { return m_pTransformCom; }

	_bool Is_Dead() const { return m_iFlag & ObjFlag::DEAD; }
	void Set_Dead() { m_iFlag |= ObjFlag::DEAD; }
	_uint Get_Flag() const { return m_iFlag; }
	void Set_Flag(_uint iFlag) { m_iFlag |= iFlag; }
	void Reset_Flag(_uint iFlag) { m_iFlag &= ~iFlag; }
	_bool Check_Flag(_uint iFlag) const { return (m_iFlag & iFlag) != 0; }

	virtual HRESULT	Initialize_Prototype();
	virtual HRESULT	Initialize(void* pArg);
	virtual void	Priority_Update(_float fTimeDelta);
	virtual void	Update(_float fTimeDelta);
	virtual void	Late_Update(_float fTimeDelta);
	virtual HRESULT	Render();
	virtual HRESULT	Render_Shadow() { return S_OK; };
	virtual HRESULT	Render_OutlineMask() { return S_OK; };

	CComponent* Find_Component(WNameID strComTag);

	template<typename T>
	T* Get_Component(WNameID strComTag)
	{
		return dynamic_cast<T*>(Find_Component(strComTag));
	}

protected:
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };
	CGameInstance* m_pGameInstance = { nullptr };

	WNameMap<CComponent*> m_Components;
	CTransform* m_pTransformCom = { nullptr };
	_wstring m_strName = {};
	_uint m_iFlag = {};

protected:
	HRESULT Add_Component(_uint iLevel, WNameID strProtoTag, WNameID strComTag, CComponent** ppOut, void* pArg = nullptr);

public:
	virtual CGameObject* Clone(void* pArg) PURE;

protected:
	virtual void Free() override;
};

NS_END