#pragma once

#include "Client_Defines.h"
#include "Level.h"

BEGIN(Client)

class CLevel_Chara_Select final : public CLevel
{
private:
	CLevel_Chara_Select(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CLevel_Chara_Select() = default;

public:
	virtual HRESULT Initialize() override;
	virtual void Update(_float fTimeDelta) override;
	virtual HRESULT Render(_float fTimeDelta) override;

private:
	/* 내 게임내에 배경을 구성하는 객체들을 추가한다. */
	HRESULT Ready_Layer_BackGround(const _wstring& strLayerTag);
	HRESULT Ready_Camera(const _wstring& strLayerTag);
	HRESULT Ready_Character(const _wstring& strLayerTag);
	HRESULT Ready_Sound();
	HRESULT Ready_Lights();

public:
	static CLevel_Chara_Select* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
};

END