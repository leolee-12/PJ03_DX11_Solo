#pragma once

#include "Shader.h"

#include "VIBuffer_Cube.h"
#include "VIBuffer_Rect.h"
#include "VIBuffer_Terrain.h"
#include "VIBuffer_TerrainD.h"
#include "VIBuffer_Particle_Rect.h"
#include "VIBuffer_Particle_Point.h"
#include "VIBuffer_Trail.h"

#include "StateMachine.h"
#include "Navigation.h"
#include "Transform.h"
#include "RigidBody.h"
#include "Collider.h"
#include "Texture.h"
#include "Model.h"

#include "MaterialComponent.h"
#include "CommonModelComp.h"

#include "PhysX_Collider.h"
#include "PhysX_Controller.h"

/* 원형 컴포넌트들을 레벨별로 보관한다. */
/* 복제하고하는 원형을 찾아 복제하여 리턴한다. */

BEGIN(Engine)

class CComponent_Manager final : public CBase
{
	INFO_CLASS(CComponent_Manager,CBase)

private:
	CComponent_Manager();
	virtual ~CComponent_Manager() = default;

public:
	HRESULT Initialize(_uint iNumLevels);
	HRESULT Add_Prototype(_uint iLevelIndex, const wstring& strPrototypeTag, shared_ptr<class CComponent> pPrototype);
	shared_ptr<class CComponent> Clone_Component(_uint iLevelIndex, const wstring& strPrototypeTag, 
		shared_ptr<CGameObject> pOwner, void* pArg);

	void Clear(_uint iLevelIndex);

	map<const wstring, shared_ptr<class CComponent>>* Get_PrototypeList(_uint iLevelIndex) { return &m_pPrototypes[iLevelIndex]; }

private:
	_uint											m_iNumLevels = { 0 };

	map<const wstring, shared_ptr<class CComponent>>*			m_pPrototypes = { nullptr };
	typedef map<const wstring, shared_ptr<class CComponent>>	PROTOTYPES;

private:
	shared_ptr<class CComponent> Find_Prototype(_uint iLevelIndex, const wstring& strPrototypeTag);

public:
	static CComponent_Manager* Create(_uint iNumLevels);
	virtual void Free() override;
};

END