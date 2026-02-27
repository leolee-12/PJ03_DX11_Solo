#pragma once

#include "Client_Defines.h"
#include "Level.h"

BEGIN(ENGINE)
class CGameObject;

END

BEGIN(Client)

class CLevel_Stage2 final : public CLevel
{
	INFO_CLASS(CLevel_Stage2, CLevel)
private:
	CLevel_Stage2(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	virtual ~CLevel_Stage2() = default;

public:
	virtual HRESULT Initialize() override;
	virtual void Tick(_cref_time fTimeDelta) override;
	virtual void Late_Tick(_cref_time fTimeDelta) override;
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
	virtual HRESULT Ready_Layer_Collider(const LAYERTYPE& strLayerTag);
	virtual HRESULT Ready_Layer_Trigger(const LAYERTYPE& strLayerTag);

	virtual HRESULT Ready_Light();
	virtual void Ready_Event();

	shared_ptr<class CLight > m_pLight;

private:
	vector<_float4> m_vecLaserPos;
	_float	m_fTimeAcc = { 0.f };
	
	_bool	m_bFirstLaserOn = { false };
	_bool	m_bSecLaserOn = { false };

	_bool	m_bOnce = { true };
	_bool	m_bAddTime = { false };

	FTimeChecker	m_TimeChecker1;
	FTimeChecker	m_TimeChecker2;
	FTimeChecker	m_TimeChecker3;
	FTimeChecker	m_TimeChecker4;
	FTimeChecker	m_TimeChecker5;
	FTimeChecker	m_TimeChecker6;


public:
	static CLevel_Stage2* Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	virtual void Free() override;
};

END