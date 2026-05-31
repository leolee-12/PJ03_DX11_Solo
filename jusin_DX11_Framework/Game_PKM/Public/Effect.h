#pragma once
#include "Game_PKM_Defines.h"
#include "GameObject.h"
#include "Effect_Defines.h"

NS_BEGIN(Game_PKM)
class CParticleEmitter;
class CEffect_Mesh;
class CTrail;

class CEffect final : public CGameObject
{
public:
	struct EFFECT_DESC final : public CGameObject::GAMEOBJECT_DESC
	{
		const EFFECT_DEFINITION* pDefinition = nullptr;	// weak (Manager 보유)
		_uint   iSpawnLevel = 0;						// emitter들이 spawn될 level index
		WNameID strLayerTag = { INVALID_TAG };			// emitter들이 들어갈 Layer

		struct ATTACH_INFO
		{
			enum class KIND { NONE, BONE, MATRIX } eKind = KIND::NONE;
			class CGameObject*	pOwner = nullptr;			// BONE 모드에서 owner Is_Dead 감지용 (MATRIX / NONE은 nullable)
			string				strBoneName = "";			// BONE 모드에서 사용
			const _float4x4*	pSourceMatrix = nullptr;	// MATRIX 모드에서 사용
			_float4x4			mLocalOffset = {};			// identity default 권장
		};

		ATTACH_INFO tAttach = {};
	};

private:
	CEffect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CEffect(const CEffect& Prototype);
	virtual ~CEffect() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void    Priority_Update(_float fTimeDelta) override;
	virtual void    Update(_float fTimeDelta) override;
	virtual void    Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	void  Stop();         // 추가 spawn 멈춤 (자연 소멸 대기)
	void  Destroy();      // 즉시 제거

private:
	EFFECT_DESC m_tDesc = {};
	const EFFECT_DEFINITION*	m_pDefinition = nullptr;
	vector<CParticleEmitter*>	m_Emitters;		// borrowed (Layer가 ref 보유)
	vector<CEffect_Mesh*>		m_MeshEmitters;	// borrowed
	vector<CTrail*>				m_Trails;		// borrowed
	EFFECT_DESC::ATTACH_INFO	m_tAttach = {};
	const _float4x4*			m_pAttachMatrix = nullptr;

private:
	void Resolve_Attach_Once();

public:
	static CEffect* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;

private:
	virtual void Free() override;
};

NS_END