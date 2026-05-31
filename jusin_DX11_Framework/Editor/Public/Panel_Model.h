#pragma once
#include "Panel_Base.h"

NS_BEGIN(Editor)

class CPanel_Model final : public CPanel_Base
{
private:
	CPanel_Model();
	virtual ~CPanel_Model() = default;

public:
	HRESULT Initialize() override;
	void Update(_float fTimeDelta) override;
	HRESULT Render() override;

private:
	_char m_szFbxPath[MAX_PATH] = {};
	_char m_szOutputDir[MAX_PATH] = {};
	MODEL m_eType = { MODEL::NONANIM };
	_float m_fScale = { 1.f };
	_float m_fRotationX = { 0.f };
	_float m_fRotationY = { 180.f };
	_float m_fRotationZ = { 0.f };

	_char m_szTexDir[MAX_PATH] = {};
	_char m_szMappingJsonPath[MAX_PATH] = {};
	_char m_szPrevFbxPath[MAX_PATH] = {};   // 이전 FBX 경로 추적용
	_bool m_bMappingGenerated = { false };

public:
	static CPanel_Model* Create();

private:
	virtual void Free() override;
};

NS_END