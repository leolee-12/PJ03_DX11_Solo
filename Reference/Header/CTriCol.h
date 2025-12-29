#pragma once
#include "CVIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CTriCol : public CVIBuffer
{
protected:
	explicit	CTriCol();
	explicit	CTriCol(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit	CTriCol(const CTriCol& rhs);
	virtual		~CTriCol();

public:
	HRESULT				Ready_Buffer() override;
	void				Render_Buffer() override;

public:
	static CTriCol* Create(LPDIRECT3DDEVICE9 pGraphicDev);
	CComponent* Clone() override;

private:
	void				Free() override;
};

END