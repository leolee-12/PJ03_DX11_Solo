#pragma once
#include "Editor_Defines.h"
#include "Level.h"

NS_BEGIN(Editor)

class CLevel_EditLogo : public CLevel
{
private:
	CLevel_EditLogo(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CLevel_EditLogo() = default;

public:
	virtual			HRESULT Initialize() override;
	virtual void	Update(_float fTimeDelta) override;
	virtual HRESULT	Render() override;

	HRESULT			Ready_Layer_BackGround(const _wstring& strLayerTag);

public:
	static CLevel_EditLogo*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);

protected:
	virtual void			Free() override;
};

NS_END