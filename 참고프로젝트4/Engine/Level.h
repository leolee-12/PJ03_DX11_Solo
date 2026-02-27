#pragma once

#include "Base.h"

BEGIN(Engine)

class ENGINE_DLL CLevel abstract : public CBase
{
	INFO_CLASS(CLevel,CBase)

protected:
	CLevel(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	virtual ~CLevel() = default;

public:
	virtual HRESULT Initialize() ;
	virtual void Tick(_cref_time fTimeDelta);
	virtual void Late_Tick(_cref_time fTimeDelta);
	virtual HRESULT Render();

protected:
	ComPtr<ID3D11Device>			m_pDevice = { nullptr };
	ComPtr<ID3D11DeviceContext>	m_pContext = { nullptr };

protected:
	virtual HRESULT Ready_Light();
	virtual HRESULT Ready_Layer_Environment(const LAYERTYPE & strLayerTag);
	virtual HRESULT Ready_Layer_Object(const LAYERTYPE & strLayerTag);
	virtual HRESULT Ready_Layer_Player(const LAYERTYPE & strLayerTag);
	virtual HRESULT Ready_Layer_Monster(const LAYERTYPE& strLayerTag);
	virtual HRESULT Ready_Layer_NPC(const LAYERTYPE& strLayerTag);
	virtual HRESULT Ready_Layer_Effect(const LAYERTYPE & strLayerTag);
	virtual HRESULT Ready_Layer_Camera(const LAYERTYPE & strLayerTag);
	virtual HRESULT Ready_Layer_UI(const LAYERTYPE & strLayerTag);
	virtual HRESULT Ready_Layer_Trigger(const LAYERTYPE& strLayerTag);
	virtual HRESULT Ready_Layer_Collider(const LAYERTYPE& strLayerTag);

	virtual void Ready_Event();

protected:
	_bool	m_bNextLevel = { false };

protected:
	class CGameInstance*	m_pGameInstance = { nullptr };

public:
	virtual void Free() override;
};

END