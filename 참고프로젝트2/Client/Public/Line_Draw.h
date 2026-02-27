#pragma once

#include "GameObject.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Line_Rect;
END


BEGIN(Client)
class CLine_Draw : public CGameObject
{

private:
	CLine_Draw(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CLine_Draw(const CLine_Draw& Prototype);
	virtual ~CLine_Draw() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Camera_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render(_float fTimeDelta) override;

	void TextRender(const vector<CameraPoint>& points, _float fTimeDelta);

public:
	void Set_LinePoints(const vector<CameraPoint>& points, const _float3& vStart, const _float3& vEnd);
	CShader* m_pShaderCom = { nullptr };
	CTexture* m_pTextureCom = { nullptr };
	CVIBuffer_Line_Rect* m_pVIBufferCom = { nullptr };

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

	_float3 m_vStartPoint = {};
	_float3 m_vEndPoint = {};
	_float m_fThickness = 0.01f;

public:
	static CLine_Draw* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END