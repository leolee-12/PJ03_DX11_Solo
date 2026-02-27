#include "Data.h"

class ModelConverter {
public:
	bool LoadModel(const std::string& filePath, ModelType modelType);
	bool SaveModelToBinary(const std::string& filePath, const ModelHeader& header, const vector<MeshData>& meshes, const vector<MaterialData>& materials, const vector<AnimationData>& animations, const vector<BoneData>& bones, bool printVertices) const;

	bool LoadModelFromBinary(const std::string& filePath, ModelHeader& header, vector<MeshData>& meshes, vector<MaterialData>& materials, vector<AnimationData>& animations, vector<BoneData>& bones);

	const aiScene* GetScene() const { return m_ai_Scene; }

	void ExtractBones(const aiNode* pNode, std::vector<BoneData>& bones) const;
	void SaveBoneToBinary(std::ofstream& outFile, const BoneData& bone) const;
	void LoadBoneFromBinary(std::ifstream& inFile, BoneData& bone);

	void FillModelData(const aiScene* scene, ModelHeader& header, vector<BoneData>& bones, vector<MeshData>& meshes, vector<MaterialData>& materials, vector<AnimationData>& animations, bool printVertices);

private:
	Assimp::Importer importer;
	const aiScene* m_ai_Scene = nullptr;
};


