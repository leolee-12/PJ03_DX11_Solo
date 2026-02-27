#pragma once

#include "Base.h"
#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CGameInstance;
class CGameObject;
END

BEGIN(Client)

class CSubTitle_Manager : public CBase
{
	DECLARE_SINGLETON(CSubTitle_Manager)

public:
	enum SUBTITLE_ID
	{
		GOKU_VS_FRIEZA_SUBTITLE_0,
		GOKU_VS_FRIEZA_SUBTITLE_1,
		GOKU_VS_FRIEZA_SUBTITLE_2,
		GOKU_VS_FRIEZA_SUBTITLE_3,
		GOKU_VS_FRIEZA_SUBTITLE_4,
		HUMAN_21_SUBTITLE_0,
		FRIEZA_SUBTITLE_0,
		GOKU_SUBTITLE_0,
		HIT_WIN_SUBTITLE_0,
		SUBTITLE_END
	};

private:
	CSubTitle_Manager();
	virtual ~CSubTitle_Manager() = default;

public:
	virtual HRESULT Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Camera_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
	virtual HRESULT Render(_float fTimeDelta);

	HRESULT Play(SUBTITLE_ID eID, _float duration);
	void Stop(SUBTITLE_ID eID);

	CGameInstance* m_pGameInstance = { nullptr };

	unordered_map<SUBTITLE_ID, _wstring> m_mapSubTitleText;


	class CSubTitle* m_pSubTitle = nullptr;

public:
	virtual void Free() override;
};

END