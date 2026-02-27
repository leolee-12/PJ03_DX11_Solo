#pragma once

#include "Base.h"

/* 생성한 객체들을 보관한다. */
/* Prototype 방식으로 객체를 추가한다. */


BEGIN(Engine)

#pragma once
class CFile_Manager final : public CBase
{
private:
	CFile_Manager();
	virtual ~CFile_Manager() = default;
public:
	HRESULT Initialize();
public:
	HRESULT SaveObjects(const wstring& filename, void* pArg);
	void* LoadObjects(const wstring& filename);
	HRESULT ParseLine(const wstring& line, FILEDATA& obj);
	HRESULT Save_All_CameraPoints(const wstring& filename, void* pArg);
	HRESULT Load_All_CameraPoints(const std::wstring& filename, CameraSaveData* pArg);

	HRESULT Save_Effects(wstring& FilePath, void* pArg);
	void*	 Load_All_Effects();
	void*	Load_Effects(wstring& FilePath);
	void Read_LayerData(wifstream& file, EFFECT_LAYER_DATA& layerData);
	void Read_EffectData(wifstream& file, EFFECT_DATA& effectData);
	void Read_KeyFrameData(wifstream& file, EFFECT_KEYFRAME_DATA& keyFrameData);

private:
	vector<FILEDATA> m_vecpFileData;
	vector<EFFECT_LAYER_DATA>* m_pLoadedEffectData = { nullptr };
	vector<EFFECT_LAYER_DATA>* m_EffectsData = { nullptr };

public:
	static CFile_Manager* Create();
	virtual void Free() override;
};

END