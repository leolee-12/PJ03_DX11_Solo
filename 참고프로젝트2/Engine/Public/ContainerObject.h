#pragma once

#include "GameObject.h"

BEGIN(Engine)

class ENGINE_DLL CContainerObject abstract : public CGameObject
{
public:
	typedef struct : public CGameObject::GAMEOBJECT_DESC
	{
		_uint	iNumParts;
	}CONTAINEROBJECT_DESC;

protected:
	CContainerObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CContainerObject(const CContainerObject& Prototype);
	virtual~CContainerObject() = default;

public:
	class CComponent* Get_Component(_uint iPartID, const _wstring& strComponentTag);
	vector<class CPartObject*>& GetParts() { return m_Parts; };
	class CPartObject* GetPart(_uint index) { return m_Parts[index]; };

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Camera_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
	virtual HRESULT Render(_float fTimeDelta);

protected:
	/* 이 컨테이너 객체를 구성하기위한 부품객체들을 모아놓는다. */
	_uint								m_iNumParts = {};
	vector<class CPartObject*>			m_Parts;

protected:
	HRESULT Add_PartObject(_uint iPartIndex, const _wstring& strPrototypeTag, void* pArg);

public:
	virtual CGameObject* Clone(void* pArg) = 0;
	virtual void Free() override;
};

END