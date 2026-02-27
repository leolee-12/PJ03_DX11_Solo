#pragma once

#include "Client_Defines.h"
#include "Level.h"

#include "Level_Test_Defines.h"

BEGIN(ENGINE)
class CGameObject;

END

BEGIN(Client)

class CLevel_BossStage final : public CLevel
{
	INFO_CLASS(CLevel_BossStage, CLevel)
private:
	CLevel_BossStage(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	virtual ~CLevel_BossStage() = default;

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

	shared_ptr<class CLight > m_pLight;

public:
	static CLevel_BossStage* Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	virtual void Free() override;
};

END