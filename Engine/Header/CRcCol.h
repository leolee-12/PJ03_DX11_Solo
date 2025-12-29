#pragma once
#include "CVIBuffer.h"
BEGIN(Engine)

class ENGINE_DLL CRcCol : public CVIBuffer
{
protected:
	explicit	CRcCol();
	explicit	CRcCol(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit	CRcCol(const CRcCol& rhs);
	virtual		~CRcCol();

public:
	HRESULT			Ready_Buffer() override;
	void			Render_Buffer() override;

public:
	static CRcCol*	Create(LPDIRECT3DDEVICE9 pGraphicDev);
	CComponent*		Clone() override;

private:
	void			Free() override;
};

END