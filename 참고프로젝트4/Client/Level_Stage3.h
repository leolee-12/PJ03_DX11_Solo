#pragma once

#include "Client_Defines.h"
#include "Level.h"

BEGIN(ENGINE)
class CGameObject;

END

BEGIN(Client)

class CLevel_Stage3 final : public CLevel
{
	INFO_CLASS(CLevel_Stage3, CLevel)
private:
	CLevel_Stage3(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	virtual ~CLevel_Stage3() = default;

public:
	virtual HRESULT Initialize() override;
	virtual void Tick(_cref_time fTimeDelta) override;
	virtual void Late_Tick(_cref_time fTimeDelta) override;
	virtual HRESULT Render() override;
	
private:
	_bool	m_bOnce = { true };
	_float	m_fTimeAcc = { 0.f };
	_bool	m_bAddTime = { false };

private:
	virtual HRESULT Ready_Layer_Environment(const LAYERTYPE& strLayerTag);
	virtual HRESULT Ready_Layer_Object(const LAYERTYPE& strLayerTag);
	virtual HRESULT Ready_Layer_Player(const LAYERTYPE& strLayerTag);
	virtual HRESULT Ready_Layer_Monster(const LAYERTYPE& strLayerTag);
	virtual HRESULT Ready_Layer_NPC(const LAYERTYPE& strLayerTag);
	virtual HRESULT Ready_Layer_Effect(const LAYERTYPE& strLayerTag);
	virtual HRESULT Ready_Layer_Camera(const LAYERTYPE& strLayerTag);
	virtual HRESULT Ready_Layer_UI(const LAYERTYPE& strLayerTag);
	virtual HRESULT Ready_Layer_Collider(const LAYERTYPE& strLayerTag);
	virtual HRESULT Ready_Layer_Trigger(const LAYERTYPE& strLayerTag);

	virtual HRESULT Ready_Light();
	virtual void Ready_Event();

	shared_ptr<class CLight > m_pLight;

public:
	static CLevel_Stage3* Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	virtual void Free() override;
};

END