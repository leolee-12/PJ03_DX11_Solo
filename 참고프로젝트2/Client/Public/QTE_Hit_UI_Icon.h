#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Rect;
END

BEGIN(Client)

class CQTE_Hit_UI_Icon final : public CGameObject
{
public:

	enum KEY_ID
	{
		HIT_KEY_LIGHT,
		HIT_KEY_MEDIUM,
		HIT_KEY_HEAVY,
		HIT_KEY_ULTIMATE,
		HIT_KEY_END
	};

	//QTE_Hit에서 입력처리를 해서 결정
	enum RESULT_ID
	{
		HIT_RESULT_NOT_YET_DECIDED,
		HIT_RESULT_GOOD,
		HIT_RESULT_EXCELLENT,
		HIT_RESULT_PERFECT,
		HIT_RESULT_FAILED
	};

	struct QTE_Hit_UI_ICON_DESC
	{
		_float	fSizeX{}, fSizeY{}, fX{}, fY{}, fAlpha{}, fTimer;
		_int iTextureNumber{};
		KEY_ID key{};
		CGameObject* Hit_Situation = { nullptr };
	};

private:
	CQTE_Hit_UI_Icon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CQTE_Hit_UI_Icon(const CQTE_Hit_UI_Icon& Prototype);
	virtual ~CQTE_Hit_UI_Icon() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Camera_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render(_float fTimeDelta) override;


	CQTE_Hit_UI_Icon::RESULT_ID Send_Input(KEY_ID _Input, _bool isFinal);
public:
	_float					m_fSizeX{}, m_fSizeY{}, m_fX{}, m_fY{};

	KEY_ID m_Key = HIT_KEY_END;
	RESULT_ID m_currentResult_ID = HIT_RESULT_NOT_YET_DECIDED;

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();
	void Finalize_Icon(_bool isFinal = false);
	void Calculate_Result();

private:
	CShader* m_pShaderCom = { nullptr };
	CTexture* m_pTexture_Key_InputButtonCom = { nullptr };
	CVIBuffer_Rect* m_pVIBufferCom = { nullptr };

	_float					 m_fAlpha{}, m_fTimer{}, m_fElaspedTime{};
	_float4x4				m_ViewMatrix{}, m_ProjMatrix{};
	_int m_iTextureNumber = {};

	class CQTE_Hit_Situation* m_pHit_Situation = { nullptr };
	class CQTE_Hit_UI_MovingRing_Icon* m_pHit_MovingRing_Icon = { nullptr };

public:
	static CQTE_Hit_UI_Icon* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END