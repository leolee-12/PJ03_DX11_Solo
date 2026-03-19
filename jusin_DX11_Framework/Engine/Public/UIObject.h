#pragma once
#include "GameObject.h"

NS_BEGIN(Engine)

class ENGINE_DLL CUIObject abstract : public CGameObject
{
public:
	typedef struct tagUIObjectDesc : public CGameObject::GAMEOBJECT_DESC
	{
		_float fCenterX, fCenterY;
		_float fSizeX, fSizeY;
	}UIOBJECT_DESC;

protected:
	CUIObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUIObject(const CUIObject& Prototype);
	virtual ~CUIObject() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Priority_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
	virtual HRESULT Render();

protected:
	_float m_fViewWidth{}, m_fViewHeight{};
	_float4x4 m_TransformMatrices[ETOUI(D3DTS::END)] = {};

protected:
	HRESULT Bind_ShaderResource(class CShader* pShader, const _char* pConstantName, D3DTS eType);

public:
	virtual CGameObject* Clone(void* pArg) = 0;

protected:
	virtual void Free();
};

NS_END