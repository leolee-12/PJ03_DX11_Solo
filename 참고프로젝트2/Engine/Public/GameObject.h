#pragma once

#include "Transform.h"


/* 클라이언트 개발자가 제작한 객체들의 부모 클래스가 된다. */

BEGIN(Renderer)
class CRenderInstance;
END

BEGIN(Engine)

class ENGINE_DLL CGameObject abstract : public CBase
{
public:
	struct GAMEOBJECT_DESC : public CTransform::TRANSFORM_DESC
	{
		_bool isParsing = { false };
		wstring prototypeTag;
		wstring layerName;
		_uint levelIndex;
		_float3 position;
		_float3 rightRotation;
		_float3 upRotation;
		_float3 lookRotation;
		_float3 scale;
	};

protected:
	CGameObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGameObject(const CGameObject& Prototype);
	virtual ~CGameObject() = default;

public:
	class CComponent* Get_Component(const _wstring& strComponentTag) {
		auto	iter = m_Components.find(strComponentTag);
		if (iter == m_Components.end())
			return nullptr;
		return iter->second;
	}
	/* 항상 쓰지 않았던 m_iGameObjectData 를 렌더타겟 식별번호로서 쓸 수 있게 겟셋을 만듦*/
	void Set_GameObjectData(_int iData) {
		m_iGameObjectData = iData;
	}
	void Set_ObjectRenderData(_int iData) {
		m_iObjectRenderData = iData;
	}
	_int Get_GameObjectData() { return m_iGameObjectData; }
	_int* Get_GameObjectData_Ptr() { return &m_iGameObjectData; }
	_int Get_ObjectRenderData() { return m_iObjectRenderData; }
	
public:
	static const _wstring		m_strTransformTag;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Player_Update(_float fTimeDelta);
	virtual void Camera_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
	virtual HRESULT Priority_Render(_float fTimeDelta);
	virtual HRESULT Render(_float fTimeDelta);
	virtual HRESULT Shadow_Render(_float fTimeDelta);
public:
	virtual void OnCollisionEnter(class CCollider* other, _float fTimeDelta);
	virtual void OnCollisionStay(class CCollider* other, _float fTimeDelta);
	virtual void OnCollisionExit(class CCollider* other);

public:
	HRESULT Add_Component(_uint iPrototypeLevelIndex, const _wstring& strPrototypeTag, const _wstring& strComponentTag, class CComponent** ppOut, void* pArg = nullptr);
	void Clear_Collider_Component();
	//활성화 비활성화 관련
	void SetActive(_bool active) { m_bIsActive = active; }
	_bool IsActive() const { return m_bIsActive; }

	//IMGUI 관련
	void SetImguiSelect(_bool _bIsImguiClick) { m_bIsImguiClick = _bIsImguiClick; };
	void Destory();

	_bool						m_bIsActive = { true };

protected:	
	ID3D11Device*				m_pDevice = { nullptr };
	ID3D11DeviceContext*		m_pContext = { nullptr };
	class CGameInstance*		m_pGameInstance = { nullptr };
	class Renderer::CRenderInstance*		m_pRenderInstance = { nullptr };

	class CTransform*			m_pTransformCom = { nullptr };

	//활성화 비활성화 관련

public:
	//IMGUI 관련
	_bool						m_bIsImguiClick = { false };
	//파싱 관련
	_bool						m_bIsPasingObject = { false };
	_int						m_iGameObjectData = { 0 };
	_int						m_iObjectRenderData = { 2 };


	map<const _wstring, CComponent*>		m_Components;
	RENDER_OBJECT m_RendererDesc;


public:
	virtual CGameObject* Clone(void* pArg = nullptr) = 0;
	virtual void Free() override;
};

END