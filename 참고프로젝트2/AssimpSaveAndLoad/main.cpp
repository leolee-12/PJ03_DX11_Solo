#include "ModelConverter.h" 

#define DATAFILEPATH "../Client/Bin/ModelData/Char/Lobby_NPC/"

int main() {
	ModelConverter converter;
	
	vector<pair<string, ModelType>> modelPaths = {
		{"Models/NPC_Staff.fbx", TYPE_ANIM},
		{"Models/NPC_BUN.fbx", TYPE_ANIM},
	};

	bool printVertices = false; // Vertex 정보를 출력할지 여부를 결정하는 플래그

	for (const auto& modelInfo : modelPaths) {
		const auto& modelPath = modelInfo.first;
		const auto& modelType = modelInfo.second;

		std::string fileName = modelPath.substr(modelPath.find_last_of("/") + 1);
		std::string binFilePath = std::string(DATAFILEPATH) + fileName.substr(0, fileName.find_last_of(".")) + ".bin";

		if (!converter.LoadModel(modelPath, modelType)) {
			std::cerr << "Failed to load model: " << modelPath << std::endl;
			continue;
		}

		ModelHeader header;
		vector<MeshData> meshes;
		vector<MaterialData> materials;
		vector<AnimationData> animations;
		vector<BoneData> bones; // 뼈 데이터를 저장할 벡터
		converter.FillModelData(converter.GetScene(), header, bones, meshes, materials, animations, printVertices);

		if (!converter.SaveModelToBinary(binFilePath, header, meshes, materials, animations, bones, printVertices)) {
			std::cerr << "Failed to save model to binary: " << binFilePath << std::endl;
			continue;
		}

		ModelHeader loadedHeader;
		vector<MeshData> loadedMeshes;
		vector<MaterialData> loadedMaterials;
		vector<AnimationData> loadedAnimations;
		vector<BoneData> loadedBones; // 로드된 뼈 데이터를 저장할 벡터
		if (!converter.LoadModelFromBinary(binFilePath, loadedHeader, loadedMeshes, loadedMaterials, loadedAnimations, loadedBones)) {
			std::cerr << "Failed to load model from binary: " << binFilePath << std::endl;
			continue;
		}

		std::cout << "Model processed successfully: " << modelPath << std::endl;
	}

	return 0;
}
