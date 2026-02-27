#pragma once

#include "Client_Defines.h"
#include "Level.h"

BEGIN(Client)

class CLevel_Logo final : public CLevel
{
	INFO_CLASS(CLevel_Logo, CLevel)

private:
	CLevel_Logo(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	virtual ~CLevel_Logo() = default;

public:
	virtual HRESULT Initialize() override;
	virtual void Tick(_cref_time fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	virtual HRESULT Ready_Layer_Environment(const LAYERTYPE& strLayerTag)override;
	virtual HRESULT Ready_Layer_Object(const LAYERTYPE& strLayerTag);
	virtual HRESULT Ready_Layer_Character(const LAYERTYPE& strLayerTag);
	virtual HRESULT Ready_Layer_Effect(const LAYERTYPE& strLayerTag);
	virtual HRESULT Ready_Layer_Camera(const LAYERTYPE& strLayerTag);
	virtual HRESULT Ready_Layer_UI(const LAYERTYPE& strLayerTag);

private:
	shared_ptr<CGameObject> m_pLogoUI = { nullptr };

public:
	static CLevel_Logo* Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	virtual void Free() override;
};

END