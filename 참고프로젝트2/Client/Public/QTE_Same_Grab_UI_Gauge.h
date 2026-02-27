#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Rect;
END

BEGIN(Client)

class CQTE_Same_Grab_UI_Gauge final : public CGameObject
{
public:
	struct QTE_UI_Gauge_DESC
	{
		_float	fSizeX{}, fSizeY{}, fX{}, fY{}, playTime{};
	};

private:
	CQTE_Same_Grab_UI_Gauge(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CQTE_Same_Grab_UI_Gauge(const CQTE_Same_Grab_UI_Gauge& Prototype);
	virtual ~CQTE_Same_Grab_UI_Gauge() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Camera_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render(_float fTimeDelta) override;

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

private:
	CShader* m_pShaderCom = { nullptr };
	//게이지바와 게이지
	CTexture* m_pTextureCom[2] = { nullptr,nullptr };
	CVIBuffer_Rect* m_pVIBufferCom = { nullptr };

	_float					m_fSizeX{}, m_fSizeY{}, m_fX{}, m_fY{}, m_fAlpha{};
	_float4x4				m_ViewMatrix{}, m_ProjMatrix{};

	_float m_fElapsedTime = {};
	_float m_fPlayTime{};

public:
	static CQTE_Same_Grab_UI_Gauge* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END