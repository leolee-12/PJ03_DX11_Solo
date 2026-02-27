#pragma once

#include "Base.h"
#include "Renderer_Defines.h"

BEGIN(Engine)
class CShader;
class CVIBuffer_Rect;
END

BEGIN(Renderer)

class CLight final : public CBase
{
private:
	CLight();
	virtual ~CLight() = default;

public:
	LIGHT_DESC* Get_LightDesc()  {
		return &m_LightDesc;
	}

public:
	HRESULT Initialize(const LIGHT_DESC& LightDesc, _float4 vChaseColor = { 1.f,1.f,1.f,1.f }, _bool* pisChaseLight = { nullptr });
	HRESULT Render(class CShader* pShader, class CVIBuffer_Rect* pVIBuffer, _int iPassIndex, LIGHT_DESC* pLightDesc = nullptr);
	HRESULT Render_Map(class CShader* pShader, class CVIBuffer_Rect* pVIBuffer);
	HRESULT Render_Player(class CShader* pShader, class CVIBuffer_Rect* pVIBuffer);
	HRESULT Render_Effect(class CShader* pShader, class CVIBuffer_Rect* pVIBuffer, LIGHT_DESC* pLightDesc);
private:
	LIGHT_DESC				m_LightDesc{};
	LIGHT_DESC				m_ChaseLightDesc{};
	_bool* m_pIsChaseLight = { nullptr };
public:
	static CLight* Create(const LIGHT_DESC& LightDesc, _float4 vChaseColor = { 1.f,1.f,1.f,1.f }, _bool* pisChaseLight = { nullptr });
	virtual void Free() override;
};

END

/*

이펙트 라이트가 들어오면 그 위에 덮어그리는게 맞음
플레이어의 라이트는 먼저 그리는 대신
라이프타임을 넣은 이펙트빛을 생성해서 그 위에 한번더 계산해줘야함


*/