#pragma once
#include "Tool_Defines.h"
#include "Level.h"

NS_BEGIN(Tool)

class CLevel_EditPlay : public CLevel
{
private:
	CLevel_EditPlay(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CLevel_EditPlay() = default;

public:
	virtual			HRESULT Initialize() override;
	virtual void	Update(_float fTimeDelta) override;
	virtual HRESULT	Render() override;

public:
	static CLevel_EditPlay*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);

protected:
	virtual void		Free() override;
};

NS_END