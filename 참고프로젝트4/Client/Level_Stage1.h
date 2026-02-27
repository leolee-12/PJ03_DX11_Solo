#pragma once

#include "Client_Defines.h"
#include "Level.h"

#include "Level_Test_Defines.h"

BEGIN(ENGINE)
class CGameObject;

END

BEGIN(Client)

class CLevel_Stage1 final : public CLevel
{
	INFO_CLASS(CLevel_Stage1, CLevel)
private:
	CLevel_Stage1(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	virtual ~CLevel_Stage1() = default;

public:
	virtual HRESULT Initialize() override;
	virtual void Tick(_cref_time fTimeDelta) override;
	virtual void Late_Tick(_cref_time fTimeDelta) override;
	virtual HRESULT Render() override;
	
private:
	//_bool	m_bOne = { true };

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

private:
	_bool	m_bColliderDead = { false };

	shared_ptr<class CLight > m_pLight;

public:
	static CLevel_Stage1* Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	virtual void Free() override;
};

END