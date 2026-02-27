#pragma once


#include "Model.h"
#include "Shader.h"
#include "Texture.h"
#include "Collider.h"
#include "Transform.h"
#include "Navigation.h"
#include "VIBuffer_Rect.h"
#include "VIBuffer_Cube.h"
#include "VIBuffer_Terrain.h"
#include "VIBuffer_Rect_Instancing.h"
#include "VIBuffer_Point_Instancing.h"



/* 원형객첻르을 레벨별로 보관해준다. (CGameObject, CComponent) */
/* 지정한 원형을 복제하여 리턴해준다. */

NS_BEGIN(Engine)

class CPrototype_Manager final : public CBase
{
private:
	CPrototype_Manager();
	virtual ~CPrototype_Manager() = default;

public:
	HRESULT Initialize(_uint iNumLevels);
	HRESULT Add_Prototype(_uint iLevelID, const _wstring& strPrototypeTag, CBase* pPrototype);
	CBase* Clone_Prototype(PROTOTYPE eType, _uint iLevelID, const _wstring& strPrototypeTag, void* pArg = nullptr);
	void Clear(_uint iLevelID);
private:
	_uint							m_iNumLevels = {};
	map<const _wstring, CBase*>*	m_pPrototypes = { nullptr };
	typedef map<const _wstring, CBase*> PROTOTYPES;

private:
	CBase* Find_Prototype(_uint iLevelID, const _wstring& strPrototypeTag);

public:
	static CPrototype_Manager* Create(_uint iNumLevels);
	virtual void Free() override;
};

NS_END