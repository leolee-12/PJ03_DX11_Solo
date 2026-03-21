#pragma once
#include "Shader.h"
#include "Texture.h"
#include "VIBuffer_Rect.h"
#include "VIBuffer_Terrain.h"

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
	HRESULT		Add_Prototype(_uint iLevelIndex, const _wstring& strPrototypeTag, CBase* pPrototype);
	CBase*		Clone_Prototype(PROTOTYPE eType, _uint iLevelIndex, const _wstring& strPrototypeTag, void* pArg);
	void		Clear(_uint iLevelIndex);

private:
	typedef map<_wstring, class CBase*>	PROTOTYPES;
	PROTOTYPES*	m_pPrototypes = { nullptr };
	_uint		m_iNumLevels = {};

private:
	CBase*	Find_Prototype(_uint iLevelIndex, const _wstring& strPrototypeTag);

public:
	static CPrototype_Manager*	Create(_uint iNumLevels);

protected:
	virtual void	Free() override;
};

NS_END