#pragma once
#include "Tool_Defines.h"
#include "Base.h"

NS_BEGIN(Engine)
class CGameInstance;
NS_END

NS_BEGIN(Tool)

class CEditLoader final : public CBase
{
private:
	CEditLoader(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CEditLoader() = default;

public:
	HRESULT	Initialize(LEVEL eNextLevelID);
	HRESULT	Loading();
	_bool isFinished() const { return m_isFinished; }

#ifdef _DEBUG
public:
	void Show();
#endif

private:
	ID3D11Device*			m_pDevice = { nullptr };
	ID3D11DeviceContext*	m_pContext = { nullptr };
	CGameInstance*			m_pGameInstance = { nullptr };
	LEVEL					m_eNextLevelID = { LEVEL::END };

	HANDLE					m_hThread = {};
	CRITICAL_SECTION		m_CriticalSection = {};

	_tchar					m_szLoadingText[MAX_PATH] = {};
	std::atomic<_bool>		m_isFinished = {};

private:
	HRESULT Ready_Resources_For_EditLogo();
	HRESULT Ready_Resources_For_EditPlay();

public:
	static CEditLoader*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eNextLevelID);

protected:
	virtual void	Free() override;
};

NS_END