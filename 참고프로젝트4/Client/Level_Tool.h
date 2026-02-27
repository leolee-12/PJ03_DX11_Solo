#pragma once

#include "Client_Defines.h"
#include "Level.h"

BEGIN(Client)

class CLevel_Tool final : public CLevel
{
private:
	CLevel_Tool(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	virtual ~CLevel_Tool() = default;

public:
	virtual HRESULT Initialize() override;
	virtual void Tick(_cref_time fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	virtual HRESULT Ready_Layer_Environment(const LAYERTYPE& strLayerTag);
	virtual HRESULT Ready_Layer_Object(const LAYERTYPE& strLayerTag);
	virtual HRESULT Ready_Layer_Player(const LAYERTYPE& strLayerTag);
	virtual HRESULT Ready_Layer_Monster(const LAYERTYPE& strLayerTag);
	virtual HRESULT Ready_Layer_NPC(const LAYERTYPE& strLayerTag);
	virtual HRESULT Ready_Layer_Effect(const LAYERTYPE& strLayerTag);
	virtual HRESULT Ready_Layer_Camera(const LAYERTYPE& strLayerTag);
	virtual HRESULT Ready_Layer_UI(const LAYERTYPE& strLayerTag);

	_bool bTest = {true};

public:
	static CLevel_Tool* Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	virtual void Free() override;
};

END