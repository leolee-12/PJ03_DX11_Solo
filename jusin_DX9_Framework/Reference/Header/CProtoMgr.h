#pragma once
#include "Engine_Define.h"

#include "CTriCol.h"
#include "CRcCol.h"
#include "CRcTex.h"
#include "CGridCol.h"
#include "CTerrainTex.h"
#include "CCubeTex.h"
#include "CTransform.h"
#include "CTexture.h"
#include "CCalculator.h"

BEGIN(Engine)

class ENGINE_DLL CProtoMgr : public CBase
{
	DECLARE_SINGLETON(CProtoMgr)

private:
	CProtoMgr();
	virtual ~CProtoMgr();

public:
	HRESULT			Ready_Prototype(const _tchar* pComponentTag, CComponent* pComponent);
	CComponent*		Clone_Prototype(const _tchar* pComponentTag);


private:
	CComponent* Find_Prototype(const _tchar* pComponentTag);

private:
	map<const _tchar*, CComponent*>		m_mapPrototype;

private:
	virtual void Free();

};

END
