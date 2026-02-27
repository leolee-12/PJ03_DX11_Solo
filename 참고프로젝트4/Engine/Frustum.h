#pragma once

#include "Base.h"

#define FRUSTUM_DEBUG 0

BEGIN(Engine)

/// <summary>
/// ÀýµÎÃ¼
/// </summary>
class CFrustum : public CBase
{
	DERIVED_CLASS(CBase, CFrustum)

private:
	CFrustum();
	virtual ~CFrustum() = default;

public:
	HRESULT Initialize();
	void	Tick();

	

public:
	static CFrustum* Create();
	virtual void Free() override;

public:
	void	Transform_ToLocalSpace(_fmatrix WorldMatrix);

private:
	class CGameInstance* m_pGI = { nullptr };

public:
	const BoundingFrustum* Get_WorldFrustumPtr() const { return &m_WorldFrustum; }

private:
	BoundingFrustum m_OriginFrustum;
	BoundingFrustum m_ViewFrustum;
	BoundingFrustum m_WorldFrustum;

#ifdef _DEBUG
private:
	HRESULT Render_Debug();

private:
	PrimitiveBatch<VertexPositionColor>* m_pBatch = { nullptr };
	BasicEffect* m_pEffect = { nullptr };
	ComPtr<ID3D11InputLayout>					m_pInputLayout = { nullptr };
#endif // _DEBUG

};

END