#pragma once

#include "Client_Defines.h"
#include "GameObject.h"
#include "Effect_Layer.h"
#include "Base.h"
BEGIN(Engine)
class CGameInstance;
class CGameObject;
class CModel;
END

BEGIN(Renderer)
class CRenderInstance;
END

BEGIN(Client)

class CMap_Manager : public CBase
{
	DECLARE_SINGLETON(CMap_Manager)
public:
	typedef struct : public CGameObject::GAMEOBJECT_DESC
	{
		_wstring m_PrototypeKey;
	}Map_Object_Key;
	enum MAP_TYPE { MAP_SPACE, MAP_DEST_SPACE,MAP_EF_SPACE, MAP_VOLCANO, MAP_DEST_VOLCANO, MAP_END };
	enum East_Finish_Type { EAST_LASER, EAST_SPHERE, EAST_END };
private:
	CMap_Manager();
	virtual ~CMap_Manager() = default;

public:
	HRESULT Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	void Camera_Update(_float fTimeDelta);
	void Update(_float fTimeDelta);
	void Late_Update(_float fTimeDelta);
	void Render(_float fTimeDelta);
public:
	void Map_Change(MAP_TYPE eMapType);
	_float2 Active_DestructiveFinish(_bool isRight);
	_float2 Active_EastFinish();
	void PlayerCall_EastFinish(East_Finish_Type eEastEffectType, _float fWhiteSpeed);
	void IsDone_Active();
	
	void Push_MapObject(MAP_TYPE eMapType, _wstring& strKey,class CGameObject* pGameObject);
	void All_Black(_bool isTrue);
	MAP_TYPE m_eCurMap = { MAP_SPACE };
	
	_float4x4 m_RockMatrix[3]{};
private: /* East_Finish_Type Variable */
	_bool			m_isEastFinish = { false };
	/* 어떤식으로 트리거를 호출해서 동작할지 모름 그때에 맞춰서 넣어주자 */
	_bool			m_isWhiteDoneCheck = { false };
	East_Finish_Type m_eEastEffectType = { EAST_END };
	_float			m_fEastAccTime = { 0.f };
	_bool			m_isEastFinishStart = { false };
	CEffect_Layer* m_pEastEffect_Layer = { nullptr };
private: /* Destructive Variable */
	_bool			m_isRight = { false };
	_bool			m_isDestructive_View = { false };
	_bool			m_isDestructive_Active = { false };
	_float			m_AccTime = { 0.f };
	_float			m_MapViewTime = { 0.f };

	_float4x4 Result4x4 = {};

	LIGHT_DESC* m_pMapLightDesc = { nullptr };
	LIGHT_DESC m_PreLightDesc = {};
private: /* MapObject Variable */
	map<const wstring, class CGameObject*>			m_SpaceModels;
	map<const wstring, class CGameObject*>			m_Destructive_SpaceModels;
	map<const wstring, class CGameObject*>			m_VolcanoModels;
	map<const wstring, class CGameObject*>			m_Destructive_VolcanoModels;
	
private: /* Default Variable */
	CGameInstance*			m_pGameInstance = { nullptr };
	CRenderInstance*		m_pRenderInstance = { nullptr };

	ID3D11Device*			m_pDevice = { nullptr };
	ID3D11DeviceContext*	m_pContext = { nullptr };
public:
	HRESULT Ready_Components();
	virtual void Free() override;
};

END