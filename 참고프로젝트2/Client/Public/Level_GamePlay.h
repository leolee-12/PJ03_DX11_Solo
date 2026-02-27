#pragma once

#include "Client_Defines.h"
#include "Level.h"



BEGIN(Client)

class CLevel_GamePlay final : public CLevel
{
private:
	CLevel_GamePlay(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CLevel_GamePlay() = default;

public:
	virtual HRESULT Initialize() override;
	virtual void Update(_float fTimeDelta) override;
	virtual HRESULT Render(_float fTimeDelta) override;


	void Change_Level_ForCharacter();
private:
	/* 내 게임내에 배경을 구성하는 객체들을 추가한다. */
	HRESULT Ready_Layer_Camera(const _wstring& strLayerTag);
	HRESULT Ready_Lights();
	HRESULT Ready_Character();
	HRESULT Ready_UIObjects();
	HRESULT Ready_Sound();
	HRESULT Ready_Space();
	HRESULT Ready_Volcano();

	void			Create_Effect_Manager();
	void			Create_QTE_Manager();
	void			Create_Particle_Manager();
	void			Create_SubTitle_Manager();

	HRESULT Loading_For_Effect();



private:
	class CUI_Manager* m_pUI_Manager = { nullptr };
	class CEffect_Manager* m_pEffect_Manager = { nullptr };
	class CImgui_Manager* m_pIMGUI_Manager = { nullptr };
	class CQTE_Manager* m_pQTE_Manager = { nullptr };
	class CMap_Manager* m_pMap_Manager = { nullptr };
	class CParticle_Manager* m_pParticle_Manager = { nullptr };
	class CSubTitle_Manager* m_pSubTitle_Manager = { nullptr };

	_bool m_bLevelChange = false;
	_bool m_bLevelChanging = false;
	_float m_fAccLevelChangeTime = {};

public:
	static CLevel_GamePlay* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
};

END