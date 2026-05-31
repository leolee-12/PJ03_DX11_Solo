#pragma once
#include "Base.h"

/* 클라이언트 개발자가 제작할 모든 레벨클래스의 부모가 되는 클래스 */

NS_BEGIN(Engine)

class ENGINE_DLL CLevel abstract : public CBase
{
protected:
	CLevel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CLevel() = default;

public:
	virtual HRESULT Initialize();
	virtual void Update(_float fTimeDelta);
	virtual HRESULT Render();

	virtual void OnPause() {};	// 본 레벨이 스택 상에서 paused 상태로 진입할 때 호출(위에 새 레벨이 push)
	virtual void OnResume() {};	// 본 레벨이 스택 상에서 다시 top이 될 때 호출 (위의 레벨이 pop)

protected:
	ID3D11Device*			m_pDevice = { nullptr };
	ID3D11DeviceContext*	m_pContext = { nullptr };
	class CGameInstance*	m_pGameInstance = { nullptr };

protected:
	virtual void	Free() override;
};

NS_END