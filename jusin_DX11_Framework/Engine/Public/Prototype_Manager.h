#pragma once
#include "Model.h"
#include "Shader.h"
#include "Texture.h"
#include "Collider.h"
#include "Navigation.h"
#include "UIAnimator.h"
#include "VIBuffer_Rect.h"
#include "VIBuffer_Cube.h"
#include "VIBuffer_Terrain.h"
#include "VIBuffer_Rect_Instance.h"
#include "VIBuffer_Point_Instance.h"

/* -------------------------------------------------- */
// 프로토타입 매니저
// - 원본 객체(CGameObject, CComponent)를 보관
// - 선택된 원본 객체를 복제하여 리턴
/* -------------------------------------------------- */

NS_BEGIN(Engine)

class CPrototype_Manager final : public CBase
{
private:
	CPrototype_Manager();
	virtual ~CPrototype_Manager() = default;

public:
	HRESULT		Initialize(_uint iNumLevels);
	HRESULT		Add_Prototype(_uint iLevelIndex, WNameID strProtoTag, CBase* pPrototype);
	CBase*		Clone_Prototype(PROTOTYPE eType, _uint iLevelIndex, WNameID strProtoTag, void* pArg);
	void		Clear(_uint iLevelIndex);

private:
	typedef WNameMap<class CBase*, ALWAYS_HASHMAP>	PROTOTYPES;
	PROTOTYPES*	m_pPrototypes = { nullptr };
	_uint		m_iNumLevels = {};

	mutex m_Mutex;

private:
	CBase*	Find_Prototype(_uint iLevelIndex, WNameID strProtoTag);
	CBase* Find_Prototype_NoLock(_uint iLevelIndex, WNameID strProtoTag);

public:
	static CPrototype_Manager*	Create(_uint iNumLevels);

protected:
	virtual void	Free() override;
};

NS_END