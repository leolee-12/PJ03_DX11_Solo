#pragma once	//	(P)
#include "CVIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CCubeTex : public CVIBuffer
{
protected:
	explicit		CCubeTex();
	explicit		CCubeTex(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit		CCubeTex(const CCubeTex& rhs);
	virtual			~CCubeTex();

public:
	HRESULT			Ready_Buffer()	override;
	void			Render_Buffer()	override;

public:
	static CCubeTex*	Create(LPDIRECT3DDEVICE9 pGraphicDev);
	CComponent*			Clone()	override;

private:
	void				Free()	override;
};

END
