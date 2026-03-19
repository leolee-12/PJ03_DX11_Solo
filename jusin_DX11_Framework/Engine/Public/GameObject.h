#pragma once
#include "Transform.h"

NS_BEGIN(Engine)

class ENGINE_DLL CGameObject abstract : public CBase
{
public:
	typedef struct tagGameObjectDesc : public CTransform::TRANSFORM_DESC
	{
		_uint iFlag = {};
	}GAMEOBJECT_DESC;

protected:
	CGameObject(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	CGameObject(const CGameObject& Prototype);
	virtual ~CGameObject() = default;

public:
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

	map<const _wstring, class CComponent*> m_Components;
	class CTransform* m_pTransformCom = { nullptr };
	_uint m_iFlag = {};

protected:
	HRESULT Add_Component(_uint iLevel, const _wstring& strProtoTag, const _wstring& strComTag, CComponent** ppOut, void* pArg = nullptr);
	class CComponent* Find_Component(const _wstring& strComTag);

public:
	virtual CGameObject* Clone(void* pArg) PURE;

protected:
	virtual void Free() override;
};

NS_END