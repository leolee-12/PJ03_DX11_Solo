#pragma once

#include "..\Public\File_Manager.h"
#include <sstream>
#include <stdexcept>
#include <fstream>
#include "Camera.h"

CFile_Manager::CFile_Manager()
{
}

HRESULT CFile_Manager::Initialize()
{
	return S_OK;
}

HRESULT CFile_Manager::SaveObjects(const wstring& filename, void* pArg)
{
	// 파일을 쓰기 모드로 열거나 없으면 새로 생성
	wofstream file(filename);
	if (!file.is_open()) {
		// 파일 생성에 실패한 경우 파일을 새로 생성하여 다시 시도
		file.open(filename, ios::out | ios::trunc);
		if (!file.is_open()) {
			// 파일 생성에 실패한 경우 오류 반환
			return E_FAIL;
		}
	}

	vector<FILEDATA>* pvecFileData = reinterpret_cast<vector<FILEDATA>*>(pArg);

	for (const auto& data : *pvecFileData) {
		file << L"LayerName: " << data.layerName << L"\n";
		file << L"PrototypeTag: " << data.prototypeTag << L"\n";
		file << L"LevelIndex: " << data.levelIndex << L"\n";
		file << L"Position: " << data.position.x << L" " << data.position.y << L" " << data.position.z << L"\n";
		file << L"RightRotation: " << data.rightRotation.x << L" " << data.rightRotation.y << L" " << data.rightRotation.z << L"\n";
		file << L"UpRotation: " << data.upRotation.x << L" " << data.upRotation.y << L" " << data.upRotation.z << L"\n";
		file << L"LookRotation: " << data.lookRotation.x << L" " << data.lookRotation.y << L" " << data.lookRotation.z << L"\n";
		file << L"Scale: " << data.scale.x << L" " << data.scale.y << L" " << data.scale.z << L"\n";
		file << L"IsParsing: " << data.isParsing << L"\n\n";
	}

	file.close();

	return S_OK;
}

void* CFile_Manager::LoadObjects(const wstring& filename)
{
	m_vecpFileData.clear();

	wifstream file(filename);
	if (!file.is_open()) {
		throw runtime_error("Cannot open file for reading.");
		return nullptr;
	}

	wstring line;
	FILEDATA data;

	// 라인을 순차적으로 읽는다.
	while (getline(file, line)) {
		// 빈 라인이 아니라면
		if (!line.empty()) {
			// 각 라인 파싱하고 파싱 실패하면 실패 리턴
			if (ParseLine(line, data) == E_FAIL) {
				file.close();
				return nullptr;
			}
		}
	}

	file.close();
	return &m_vecpFileData;
}

HRESULT CFile_Manager::ParseLine(const wstring& line, FILEDATA& obj) {
	wistringstream iss(line);
	wstring key;
	// ':' 콜론을 기준으로 키와 값으로 문자열 분리
	if (getline(iss, key, L':')) {
		wstring value;
		getline(iss, value);
		// 좌우 공백 제거 (C++11 이상)
		value.erase(0, value.find_first_not_of(L' ')); // 왼쪽 공백 제거
		value.erase(value.find_last_not_of(L' ') + 1); // 오른쪽 공백 제거

		if (key == L"IsParsing") {
			obj.isParsing = (value == L"true" || value == L"1");
			// 모든 값을 읽은 후 객체를 저장
			m_vecpFileData.push_back(obj);
			obj = FILEDATA(); // 객체 초기화
		}
		else if (key == L"LayerName") {
			obj.layerName = value;
		}
		else if (key == L"PrototypeTag") {
			obj.prototypeTag = value;
		}
		else if (key == L"LevelIndex") {
			obj.levelIndex = stoi(value);
		}
		else if (key == L"Position") {
			wistringstream vals(value);
			float a, b, c;
			vals >> a >> b >> c;
			obj.position.x = a;
			obj.position.y = b;
			obj.position.z = c;
		}
		else if (key == L"RightRotation") {
			wistringstream vals(value);
			float a, b, c;
			vals >> a >> b >> c;
			obj.rightRotation.x = a;
			obj.rightRotation.y = b;
			obj.rightRotation.z = c;
		}
		else if (key == L"UpRotation") {
			wistringstream vals(value);
			float a, b, c;
			vals >> a >> b >> c;
			obj.upRotation.x = a;
			obj.upRotation.y = b;
			obj.upRotation.z = c;
		}
		else if (key == L"LookRotation") {
			wistringstream vals(value);
			float a, b, c;
			vals >> a >> b >> c;
			obj.lookRotation.x = a;
			obj.lookRotation.y = b;
			obj.lookRotation.z = c;
		}
		else if (key == L"Scale") {
			wistringstream vals(value);
			float a, b, c;
			vals >> a >> b >> c;
			obj.scale.x = a;
			obj.scale.y = b;
			obj.scale.z = c;
		}

		return S_OK;
	}
	else
		return E_FAIL;
}

HRESULT CFile_Manager::Save_All_CameraPoints(const wstring& filename, void* pArg)
{
	if (nullptr == pArg)
		return E_FAIL;

	wofstream file(filename);
	if (!file.is_open())
	{
		// 파일 생성 실패 처리
		return E_FAIL;
	}

	CameraSaveData* saveData = static_cast<CameraSaveData*>(pArg);

	// 각 모델 순회
	for (const auto& model : saveData->models)
	{
		file << L"[VirtualCamera]\n";
		auto modelIt = modelIDToString.find(model.modelID);
		if (modelIt != modelIDToString.end())
		{
			file << L"ModelID: " << modelIt->second << L"\n\n";
		}
		else
		{
			file << L"ModelID: " << L"Unknown" << L"\n\n";
		}

		// 각 스킬 순회
		for (const auto& skill : model.skills)
		{
			file << L"[Skill]\n";
			file << L"SkillName: " << skill.skillName.c_str() << L"\n\n";

			// 각 애니메이션 순회
			for (const auto& anim : skill.animations)
			{
				file << L"[Animation]\n";
				file << L"AnimationName: " << anim.animationName.c_str() << L"\n";
				file << L"PointCount: " << anim.points.size() << L"\n\n";

				// 각 포인트 순회
				for (const auto& point : anim.points)
				{
					file << L"[CameraPoint]\n";
					file << L"Position: " << point.position.x << L" " << point.position.y << L" " << point.position.z << L"\n";
					file << L"Rotation: " << point.rotation.x << L" " << point.rotation.y << L" " << point.rotation.z << L" " << point.rotation.w << L"\n";
					file << L"Duration: " << point.duration << L"\n";
					file << L"InterpolationType: " << point.interpolationType << L"\n";
					file << L"Damping: " << point.damping << L"\n";
					file << L"HasWorldFloat4x4: " << (point.hasWorldFloat4x4 ? 1 : 0) << L"\n\n";
				}
			}
		}
	}

	file.close();
	return S_OK;
}

HRESULT CFile_Manager::Load_All_CameraPoints(const std::wstring& filename, CameraSaveData* pArg)
{
	if (nullptr == pArg)
		return E_FAIL;

	std::wifstream file(filename);
	if (!file.is_open())
	{
		// 파일 열기 실패 처리
		return E_FAIL;
	}

	std::wstring line;
	CameraSaveData::ModelData currentModel;
	CameraSaveData::ModelData::SkillData currentSkill;
	CameraSaveData::ModelData::SkillData::AnimationData currentAnim;

	// 변수 초기화
	currentModel.modelID = -1;

	while (std::getline(file, line))
	{
		if (line.empty())
			continue;

		if (line == L"[VirtualCamera]")
		{
			// Save current Animation and Skill before moving to new Model
			if (!currentAnim.animationName.empty())
			{
				currentSkill.animations.push_back(currentAnim);
				currentAnim = CameraSaveData::ModelData::SkillData::AnimationData();
			}
			if (!currentSkill.skillName.empty())
			{
				currentModel.skills.push_back(currentSkill);
				currentSkill = CameraSaveData::ModelData::SkillData();
			}
			if (currentModel.modelID != -1)
			{
				pArg->models.push_back(currentModel);
			}

			currentModel = CameraSaveData::ModelData();
			currentModel.modelID = -1; // 모델 ID 초기화

			// ModelID 읽기
			std::getline(file, line);
			if (line.find(L"ModelID: ") != std::wstring::npos)
			{
				std::wstring modelStr = line.substr(9);
				// 선행 공백 제거
				size_t first = modelStr.find_first_not_of(L" \t");
				if (first != std::wstring::npos)
					modelStr = modelStr.substr(first);

				// WStringToString 함수 호출 제거
				auto it = stringToModelID.find(modelStr);
				if (it != stringToModelID.end())
				{
					currentModel.modelID = it->second;
				}
				else
				{
					currentModel.modelID = -1; // 알 수 없는 모델
				}
			}

			// 다음 라인 (빈 줄) 건너뛰기
			std::getline(file, line);
		}

		else if (line == L"[Skill]")
		{
			// Save current Animation before moving to new Skill
			if (!currentAnim.animationName.empty())
			{
				currentSkill.animations.push_back(currentAnim);
				currentAnim = CameraSaveData::ModelData::SkillData::AnimationData();
			}
			// Save current Skill before starting new one
			if (!currentSkill.skillName.empty())
			{
				currentModel.skills.push_back(currentSkill);
				currentSkill = CameraSaveData::ModelData::SkillData();
			}

			// SkillName 읽기
			std::getline(file, line);
			if (line.find(L"SkillName: ") != std::wstring::npos)
			{
				std::wstring skillNameStr = line.substr(11);
				// 선행 공백 제거
				size_t first = skillNameStr.find_first_not_of(L" \t");
				if (first != std::wstring::npos)
					skillNameStr = skillNameStr.substr(first);

				currentSkill.skillName = WStringToString(skillNameStr);
			}

			// 다음 라인 (빈 줄) 건너뛰기
			std::getline(file, line);
		}
		else if (line == L"[Animation]")
		{
			// Save current Animation before starting new one
			if (!currentAnim.animationName.empty())
			{
				currentSkill.animations.push_back(currentAnim);
				currentAnim = CameraSaveData::ModelData::SkillData::AnimationData();
			}

			// AnimationName 읽기
			std::getline(file, line);
			if (line.find(L"AnimationName: ") != std::wstring::npos)
			{
				std::wstring animNameStr = line.substr(15);
				// 선행 공백 제거
				size_t first = animNameStr.find_first_not_of(L" \t");
				if (first != std::wstring::npos)
					animNameStr = animNameStr.substr(first);

				currentAnim.animationName = WStringToString(animNameStr);
			}

			// PointCount 읽기
			std::getline(file, line);
			if (line.find(L"PointCount: ") != std::wstring::npos)
			{
				// 현재는 PointCount를 사용하지 않으므로 주석 처리
			}

			// 다음 라인 (빈 줄) 건너뛰기
			std::getline(file, line);
		}
		else if (line == L"[CameraPoint]")
		{
			CameraPoint point;

			// Position 읽기
			std::getline(file, line);
			if (line.find(L"Position: ") != std::wstring::npos)
			{
				std::wstring posStr = line.substr(10);
				std::wistringstream iss(posStr);
				iss >> point.position.x >> point.position.y >> point.position.z;
			}

			// Rotation 읽기
			std::getline(file, line);
			if (line.find(L"Rotation: ") != std::wstring::npos)
			{
				std::wstring rotStr = line.substr(10);
				std::wistringstream iss(rotStr);
				iss >> point.rotation.x >> point.rotation.y >> point.rotation.z >> point.rotation.w;
			}

			// Duration 읽기
			std::getline(file, line);
			if (line.find(L"Duration: ") != std::wstring::npos)
			{
				std::wstring durStr = line.substr(10);
				point.duration = std::stof(durStr);
			}

			// InterpolationType 읽기
			std::getline(file, line);
			if (line.find(L"InterpolationType: ") != std::wstring::npos)
			{
				std::wstring interpStr = line.substr(18);
				point.interpolationType = std::stoi(interpStr);
			}

			// Damping 읽기
			std::getline(file, line);
			if (line.find(L"Damping: ") != std::wstring::npos)
			{
				std::wstring dampingStr = line.substr(9);
				point.damping = std::stof(dampingStr);
			}

			std::getline(file, line);
			if (line.find(L"HasWorldFloat4x4: ") != std::wstring::npos)
			{
				std::wstring hasWStr = line.substr(17);
				hasWStr = Trim(hasWStr); // Trim whitespace

				try
				{
					point.hasWorldFloat4x4 = (std::stoi(hasWStr) != 0);
				}
				catch (const std::exception& e)
				{
					point.hasWorldFloat4x4 = false;
				}
			}

			// 다음 라인 (빈 줄) 건너뛰기
			std::getline(file, line);

			// 포인트 추가
			currentAnim.points.push_back(point);
		}
		else
		{
			// 기타 섹션 또는 무시할 내용
		}
	}

	// Save any remaining data at the end of the file
	if (!currentAnim.animationName.empty())
	{
		currentSkill.animations.push_back(currentAnim);
	}
	if (!currentSkill.skillName.empty())
	{
		currentModel.skills.push_back(currentSkill);
	}
	if (currentModel.modelID != -1)
	{
		pArg->models.push_back(currentModel);
	}

	file.close();
	return S_OK;
}

HRESULT CFile_Manager::Save_Effects(wstring& FilePath, void* pArg)
{
	wofstream file(FilePath);
	if (!file.is_open()) {
		return E_FAIL;
	}

	vector<EFFECT_LAYER_DATA>* EffectDataList = reinterpret_cast<vector<EFFECT_LAYER_DATA>*>(pArg);

	// 레이어 데이터를 파일에 저장
	for (const auto& layerData : *EffectDataList) {
		file << L"[Layer]" << L"\n";
		file << L"LayerName: " << layerData.layerName << L"\n";
		file << L"Duration: " << layerData.duration << L"\n";
		file << L"TickPerSecond: " << layerData.tickPerSecond << L"\n";
		file << L"KeyFramesCount: " << layerData.keyFramesCount << L"\n";
		file << L"NumEffec: " << layerData.iNumEffect << L"\n";
		file << L"Position: " << layerData.vPosition.x << L" " << layerData.vPosition.y << L" " << layerData.vPosition.z << L"\n";
		file << L"Scale: " << layerData.vScaled.x << L" " << layerData.vScaled.y << L" " << layerData.vScaled.z << L"\n";
		file << L"Rotation: " << layerData.vRotation.x << L" " << layerData.vRotation.y << L" " << layerData.vRotation.z << L"\n";
		file << L"IsFollowing: " << (layerData.bIsFollowing ? L"true" : L"false") << L"\n\n";

		// 이펙트 데이터를 파일에 저장
		for (const auto& effectData : layerData.effects) {
			file << L"[Effect]" << L"\n";
			file << L"EffectName: " << effectData.effectName << L"\n";
			file << L"ModelName: " << effectData.modelName << L"\n";
			file << L"MaskTextureName: " << effectData.maskTextureName << L"\n";
			file << L"DiffuseTextureName: " << effectData.diffuseTextureName << L"\n";
			file << L"EffectType: " << effectData.effectType << L"\n";
			file << L"RenderIndex: " << effectData.renderIndex << L"\n";
			file << L"PassIndex: " << effectData.passIndex << L"\n";
			file << L"UniqueIndex: " << effectData.uniqueIndex << L"\n";
			file << L"Position: " << effectData.position.x << L" " << effectData.position.y << L" " << effectData.position.z << L"\n";
			file << L"Scale: " << effectData.scale.x << L" " << effectData.scale.y << L" " << effectData.scale.z << L"\n";
			file << L"Rotation: " << effectData.rotation.x << L" " << effectData.rotation.y << L" " << effectData.rotation.z << L"\n";
			file << L"IsLoop: " << (effectData.isLoop ? L"true" : L"false") << L"\n";
			file << L"Color: " << effectData.vColor.x << L" " << effectData.vColor.y << L" " << effectData.vColor.z << L" " << effectData.vColor.w << L"\n";
			file << L"GlowColor: " << effectData.vGlowColor.x << L" " << effectData.vGlowColor.y << L" " << effectData.vGlowColor.z << L" " << effectData.vGlowColor.w << L"\n";
			file << L"GlowFactor: " << effectData.fGlowFactor << L"\n";
			file << L"DerredPassIndex: " << effectData.iDerredPassIndex << L"\n";
			file << L"NumKeyFrame: " << effectData.iNumKeyFrame << L"\n\n";

			// 키프레임 데이터를 파일에 저장
			for (const auto& keyFrameData : effectData.keyframes) {
				file << L"[KeyFrame]" << L"\n";
				file << L"KeyFrameNumber: " << keyFrameData.keyFrameNumber << L"\n";
				file << L"Position: " << keyFrameData.position.x << L" " << keyFrameData.position.y << L" " << keyFrameData.position.z << L"\n";
				file << L"Scale: " << keyFrameData.scale.x << L" " << keyFrameData.scale.y << L" " << keyFrameData.scale.z << L"\n";
				file << L"Rotation: " << keyFrameData.rotation.x << L" " << keyFrameData.rotation.y << L" " << keyFrameData.rotation.z << L"\n";
				file << L"CurTime: " << keyFrameData.curTime << L"\n";
				file << L"Duration: " << keyFrameData.duration << L"\n";
				file << L"IsNotPlaying: " << (keyFrameData.bIsNotPlaying ? L"true" : L"false") << L"\n\n";
			}
		}
	}

	file.close();
	return S_OK;
}

void* CFile_Manager::Load_All_Effects()
{
	const wstring directoryPath = L"../Bin/Effects/Effect/*.txt";
	WIN32_FIND_DATAW findData;
	HANDLE hFind = FindFirstFileW(directoryPath.c_str(), &findData);

	m_EffectsData = new vector<EFFECT_LAYER_DATA>;

	if (hFind == INVALID_HANDLE_VALUE)
		return nullptr;
	
	do 
	{
		wstring filePath = L"../Bin/Effects/Effect/" + wstring(findData.cFileName);

		vector<EFFECT_LAYER_DATA>* pLoadedData = static_cast<vector<EFFECT_LAYER_DATA>*>(Load_Effects(filePath));

		if (pLoadedData != nullptr) 
		{
			m_EffectsData->insert(m_EffectsData->end(), pLoadedData->begin(), pLoadedData->end());
			delete pLoadedData;
		}

	} while (FindNextFileW(hFind, &findData) != 0); // 다음 파일로 이동

	FindClose(hFind); // 핸들 닫기

	return m_EffectsData;
}

void* CFile_Manager::Load_Effects(wstring& FilePath)
{
	m_pLoadedEffectData = new vector<EFFECT_LAYER_DATA>;

	wifstream file(FilePath);
	if (!file.is_open()) {
		return nullptr;
	}

	wstring line;
	EFFECT_LAYER_DATA layerData;

	while (getline(file, line)) {
		if (line.find(L"[Layer]") != wstring::npos) {
			layerData = EFFECT_LAYER_DATA();
			// 레이어 정보를 읽어오기
			Read_LayerData(file, layerData);
			m_pLoadedEffectData->push_back(layerData);
		}
	}

	file.close();
	return m_pLoadedEffectData;
}

void CFile_Manager::Read_LayerData(wifstream& file, EFFECT_LAYER_DATA& layerData)
{
	wstring line;

	// 레이어 기본 정보 읽기
	while (getline(file, line) && !line.empty()) {
		wistringstream iss(line);
		wstring key, value;

		getline(iss, key, L':');
		getline(iss, value);
		value.erase(0, value.find_first_not_of(L' '));

		if (key == L"LayerName") layerData.layerName = value;
		else if (key == L"Duration") layerData.duration = stof(value);
		else if (key == L"TickPerSecond") layerData.tickPerSecond = stof(value);
		else if (key == L"KeyFramesCount") layerData.keyFramesCount = stoi(value);
		else if (key == L"NumEffec") layerData.iNumEffect = stoi(value);
		else if (key == L"Position") {
			wistringstream vecStream(value);
			vecStream >> layerData.vPosition.x >> layerData.vPosition.y >> layerData.vPosition.z;
		}
		else if (key == L"Scale") {
			wistringstream vecStream(value);
			vecStream >> layerData.vScaled.x >> layerData.vScaled.y >> layerData.vScaled.z;
		}
		else if (key == L"Rotation") {
			wistringstream vecStream(value);
			vecStream >> layerData.vRotation.x >> layerData.vRotation.y >> layerData.vRotation.z;
		}
		else if (key == L"IsFollowing") {
			layerData.bIsFollowing = (value == L"true");
		}
	}

	// 각 이펙트 데이터 읽기
	for (int i = 0; i < layerData.iNumEffect; ++i) {
		EFFECT_DATA effectData;
		Read_EffectData(file, effectData);
		layerData.effects.push_back(effectData);
	}
}


void CFile_Manager::Read_EffectData(wifstream& file, EFFECT_DATA& effectData)
{
	wstring line;

	// 이펙트 기본 정보 읽기
	while (getline(file, line) && !line.empty()) {
		wistringstream iss(line);
		wstring key, value;

		getline(iss, key, L':');
		getline(iss, value);
		value.erase(0, value.find_first_not_of(L' '));

		if (key == L"EffectName") effectData.effectName = value;
		else if (key == L"ModelName") effectData.modelName = value;
		else if (key == L"MaskTextureName") effectData.maskTextureName = value;
		else if (key == L"DiffuseTextureName") effectData.diffuseTextureName = value;
		else if (key == L"EffectType") effectData.effectType = stoi(value);
		else if (key == L"RenderIndex") effectData.renderIndex = stoi(value);
		else if (key == L"PassIndex") effectData.passIndex = stoi(value);
		else if (key == L"UniqueIndex") effectData.uniqueIndex = stoi(value);
		else if (key == L"Position") {
			wistringstream pos(value);
			pos >> effectData.position.x >> effectData.position.y >> effectData.position.z;
		}
		else if (key == L"Scale") {
			wistringstream scale(value);
			scale >> effectData.scale.x >> effectData.scale.y >> effectData.scale.z;
		}
		else if (key == L"Rotation") {
			wistringstream rot(value);
			rot >> effectData.rotation.x >> effectData.rotation.y >> effectData.rotation.z;
		}
		else if (key == L"IsNotPlaying") effectData.isNotPlaying = (value == L"true");
		else if (key == L"IsLoop") effectData.isLoop = (value == L"true");
		else if (key == L"Color") {
			wistringstream colorStream(value);
			colorStream >> effectData.vColor.x >> effectData.vColor.y >> effectData.vColor.z >> effectData.vColor.w;
		}
		else if (key == L"GlowColor") {
			wistringstream glowColorStream(value);
			glowColorStream >> effectData.vGlowColor.x >> effectData.vGlowColor.y >> effectData.vGlowColor.z >> effectData.vGlowColor.w;
		}
		else if (key == L"GlowFactor") effectData.fGlowFactor = stof(value);
		else if (key == L"DerredPassIndex") effectData.iDerredPassIndex = stoi(value);
		else if (key == L"NumKeyFrame") effectData.iNumKeyFrame = stoi(value);
	}

	for (int i = 0; i < effectData.iNumKeyFrame; ++i) {
		EFFECT_KEYFRAME_DATA keyFrameData;
		Read_KeyFrameData(file, keyFrameData);
		effectData.keyframes.push_back(keyFrameData);
	}
}


void CFile_Manager::Read_KeyFrameData(wifstream& file, EFFECT_KEYFRAME_DATA& keyFrameData)
{
	wstring line;

	while (getline(file, line) && !line.empty()) {
		wistringstream iss(line);
		wstring key, value;

		getline(iss, key, L':');
		getline(iss, value);
		value.erase(0, value.find_first_not_of(L' '));

		if (key == L"KeyFrameNumber") keyFrameData.keyFrameNumber = stoi(value);
		else if (key == L"Position") {
			wistringstream pos(value);
			pos >> keyFrameData.position.x >> keyFrameData.position.y >> keyFrameData.position.z;
		}
		else if (key == L"Scale") {
			wistringstream scale(value);
			scale >> keyFrameData.scale.x >> keyFrameData.scale.y >> keyFrameData.scale.z;
		}
		else if (key == L"Rotation") {
			wistringstream rot(value);
			rot >> keyFrameData.rotation.x >> keyFrameData.rotation.y >> keyFrameData.rotation.z;
		}
		else if (key == L"CurTime") keyFrameData.curTime = stof(value);
		else if (key == L"Duration") keyFrameData.duration = stof(value);
		else if (key == L"IsNotPlaying") {
			if (value == L"true") keyFrameData.bIsNotPlaying = true;
			else if (value == L"false") keyFrameData.bIsNotPlaying = false;
		}
	}
}

CFile_Manager * CFile_Manager::Create()
{
	CFile_Manager* pInstance = new CFile_Manager();

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Created : CFileManager"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CFile_Manager::Free()
{
	__super::Free();

	if (m_pLoadedEffectData != nullptr)
	{
		Safe_Delete(m_pLoadedEffectData);
	}

	if (m_EffectsData != nullptr)
	{
		Safe_Delete(m_EffectsData);
	}
}
