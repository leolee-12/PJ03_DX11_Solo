#pragma once

#include "Base.h"

BEGIN(Renderer)
class CRenderInstance;
END

BEGIN(Engine)

class ENGINE_DLL CLevel abstract : public CBase
{
protected:
	CLevel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CLevel() = default;

public:
	virtual HRESULT Initialize();
	virtual void Update(_float fTimeDelta);
	virtual HRESULT Render(_float fTimeDelta);

public:
	HRESULT Clear_Resources();
 	_uint GetLevelIndex() { return m_iLevelIndex; };
	HRESULT ParseInitialize(const wstring& filePath);
	vector<EFFECT_LAYER_DATA>*  Set_Effect(wstring FilePath);

protected:
	ID3D11Device*				m_pDevice = { nullptr };
	ID3D11DeviceContext*		m_pContext = { nullptr };
	class CGameInstance*		m_pGameInstance = { nullptr };
	class Renderer::CRenderInstance*		m_pRenderInstance = { nullptr };

	_uint							m_iLevelIndex = { 0 };

public:
	virtual void Free() override;
};

END