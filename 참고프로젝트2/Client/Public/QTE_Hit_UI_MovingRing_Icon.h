#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Rect;
END

BEGIN(Client)

class CQTE_Hit_UI_MovingRing_Icon final : public CGameObject
{
public:
	struct Hit_MovingRing_DESC
	{
		_float	fSizeX{}, fSizeY{}, fX{}, fY{};

		_float* pfElaspedTime{ nullptr };
		_float* pfTimer{ nullptr };
	};

private:
	CQTE_Hit_UI_MovingRing_Icon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CQTE_Hit_UI_MovingRing_Icon(const CQTE_Hit_UI_MovingRing_Icon& Prototype);
	virtual ~CQTE_Hit_UI_MovingRing_Icon() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Camera_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render(_float fTimeDelta) override;

public:
	_float					m_fSizeX{}, m_fSizeY{}, m_fX{}, m_fY{};
	_float4x4				m_ViewMatrix{}, m_ProjMatrix{};
	_int m_iTextureNumber = {};

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

private:
	CShader* m_pShaderCom = { nullptr };
	CTexture* m_pTextureCom = { nullptr };
	CVIBuffer_Rect* m_pVIBufferCom = { nullptr };

	_float* m_pfTimer{};
	_float* m_pfElaspedTime{};

public:
	static CQTE_Hit_UI_MovingRing_Icon* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END