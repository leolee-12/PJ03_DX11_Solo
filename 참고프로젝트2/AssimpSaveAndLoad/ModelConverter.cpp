#include <string>
#include <filesystem>
#include "ModelConverter.h"

bool ModelConverter::LoadModel(const std::string& filePath, ModelType modelType) {
	unsigned int iFlag = aiProcess_Triangulate | aiProcess_GenNormals | aiProcess_ConvertToLeftHanded | aiProcessPreset_TargetRealtime_Fast;

	if (modelType == TYPE_NONANIM) {
		iFlag |= aiProcess_PreTransformVertices;
	}

	m_ai_Scene = importer.ReadFile(filePath, iFlag);

	if (!m_ai_Scene || m_ai_Scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !m_ai_Scene->mRootNode) {
		std::cerr << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
		return false;
	}

	return true;
}

bool ModelConverter::SaveModelToBinary(const std::string& filePath, const ModelHeader& header, const vector<MeshData>& meshes, const vector<MaterialData>& materials, const vector<AnimationData>& animations, const vector<BoneData>& bones, bool printVertices) const {
	std::ofstream outFile(filePath, std::ios::binary);
	if (!outFile) {
		std::cerr << "ERROR:: Could not open file for writing." << std::endl;
		return false;
	}

	// Save header
	outFile.write(reinterpret_cast<const char*>(&header), sizeof(header));
	std::cout << "Saving header: " << header.numMeshes << " meshes, " << header.numMaterials << " materials, " << header.numAnimations << " animations" << std::endl;

	// Save bones
	if (header.isAnim) {
		uint32_t numBones = bones.size();
		outFile.write(reinterpret_cast<const char*>(&numBones), sizeof(numBones));
		for (const auto& bone : bones) {
			SaveBoneToBinary(outFile, bone);
		}
	}

	// Save meshes
	int materialIndexCounter = 0;
	// Save meshes
	for (const auto& mesh : meshes) {
		// Save mesh name
		size_t nameLength = mesh.name.length();
		outFile.write(reinterpret_cast<const char*>(&nameLength), sizeof(nameLength));
		outFile.write(mesh.name.c_str(), nameLength);

		// Save vertex and index counts
		outFile.write(reinterpret_cast<const char*>(&mesh.vertexCount), sizeof(mesh.vertexCount));
		outFile.write(reinterpret_cast<const char*>(&mesh.indexCount), sizeof(mesh.indexCount));

		if (header.isAnim) {
			// Save animated vertices
			outFile.write(reinterpret_cast<const char*>(mesh.animVertices.data()), mesh.vertexCount * sizeof(VertexAnim));
		}
		else {
			// Save static vertices
			outFile.write(reinterpret_cast<const char*>(mesh.vertices.data()), mesh.vertexCount * sizeof(Vertex));
		}

		// Save indices
		outFile.write(reinterpret_cast<const char*>(mesh.indices.data()), mesh.indexCount * sizeof(unsigned int));

		// Save material index
		int materialIndexToSave = header.isAnim ? mesh.materialIndex : materialIndexCounter++;
		outFile.write(reinterpret_cast<const char*>(&materialIndexToSave), sizeof(materialIndexToSave));

		if (header.isAnim) {
			// Save bones associated with the mesh
			uint32_t numMeshBones = mesh.bones.size();
			outFile.write(reinterpret_cast<const char*>(&numMeshBones), sizeof(numMeshBones));
			for (const auto& bone : mesh.bones) {
				SaveBoneToBinary(outFile, bone);
			}

			// Save offset matrices
			uint32_t numOffsetMatrices = mesh.offsetMatrices.size();
			outFile.write(reinterpret_cast<const char*>(&numOffsetMatrices), sizeof(numOffsetMatrices));
			outFile.write(reinterpret_cast<const char*>(mesh.offsetMatrices.data()), numOffsetMatrices * sizeof(XMFLOAT4X4));
		}
	}

	int MeshCount = { 0 };
	// Save materials
	for (const auto& material : materials) {
		
		std::cout << "MeshIndex : " << MeshCount << std::endl;
		for (const auto& texturePath : material.texturePaths) {
			size_t pathLength = texturePath.length();
			outFile.write(reinterpret_cast<const char*>(&pathLength), sizeof(pathLength));
			outFile.write(texturePath.c_str(), pathLength);

			// Print the texture path
			std::cout << "Saving texture path: " << texturePath << std::endl;
		}
		MeshCount++;
	}

	// Save animations
	if (header.isAnim) {
		for (const auto& anim : animations) {
			// Save animation name
			size_t nameLength = anim.name.length();
			outFile.write(reinterpret_cast<const char*>(&nameLength), sizeof(nameLength));
			outFile.write(anim.name.c_str(), nameLength);

			// Save animation duration and ticks per second
			outFile.write(reinterpret_cast<const char*>(&anim.duration), sizeof(anim.duration));
			outFile.write(reinterpret_cast<const char*>(&anim.ticksPerSecond), sizeof(anim.ticksPerSecond));

			// Save animation channels
			uint32_t numChannels = anim.channels.size();
			outFile.write(reinterpret_cast<const char*>(&numChannels), sizeof(numChannels));
			for (const auto& channel : anim.channels) {
				// Save channel name
				size_t channelNameLength = channel.name.length();
				outFile.write(reinterpret_cast<const char*>(&channelNameLength), sizeof(channelNameLength));
				outFile.write(channel.name.c_str(), channelNameLength);

				// Save keyframes
				uint32_t numKeyframes = channel.keyframes.size();
				outFile.write(reinterpret_cast<const char*>(&numKeyframes), sizeof(numKeyframes));
				outFile.write(reinterpret_cast<const char*>(channel.keyframes.data()), numKeyframes * sizeof(Keyframe));
			}
		}
	}

	outFile.close();
	return true;
}

bool ModelConverter::LoadModelFromBinary(const std::string& filePath, ModelHeader& header, vector<MeshData>& meshes, vector<MaterialData>& materials, vector<AnimationData>& animations, vector<BoneData>& bones) {
	std::ifstream inFile(filePath, std::ios::binary);
	if (!inFile) {
		std::cerr << "ERROR:: Could not open file for reading." << std::endl;
		return false;
	}

	// Load header
	inFile.read(reinterpret_cast<char*>(&header), sizeof(header));
	std::cout << "Reading header: " << header.numMeshes << " meshes, " << header.numMaterials << " materials, " << header.numAnimations << " animations" << std::endl;

	// Load bones
	if (header.isAnim) {
		uint32_t numBones;
		inFile.read(reinterpret_cast<char*>(&numBones), sizeof(numBones));
		bones.resize(numBones);
		for (auto& bone : bones) {
			LoadBoneFromBinary(inFile, bone);
		}
	}

	// Load meshes
	meshes.resize(header.numMeshes);
	for (auto& mesh : meshes) {
		// Load mesh name
		size_t nameLength;
		inFile.read(reinterpret_cast<char*>(&nameLength), sizeof(nameLength));
		mesh.name.resize(nameLength);
		inFile.read(&mesh.name[0], nameLength);

		// Load vertex and index counts
		inFile.read(reinterpret_cast<char*>(&mesh.vertexCount), sizeof(mesh.vertexCount));
		inFile.read(reinterpret_cast<char*>(&mesh.indexCount), sizeof(mesh.indexCount));

		if (header.isAnim) {
			// Load animated vertices
			mesh.animVertices.resize(mesh.vertexCount);
			inFile.read(reinterpret_cast<char*>(mesh.animVertices.data()), mesh.vertexCount * sizeof(VertexAnim));
		}
		else {
			// Load static vertices
			mesh.vertices.resize(mesh.vertexCount);
			inFile.read(reinterpret_cast<char*>(mesh.vertices.data()), mesh.vertexCount * sizeof(Vertex));
		}

		// Load indices
		mesh.indices.resize(mesh.indexCount);
		inFile.read(reinterpret_cast<char*>(mesh.indices.data()), mesh.indexCount * sizeof(uint32_t));
		inFile.read(reinterpret_cast<char*>(&mesh.materialIndex), sizeof(mesh.materialIndex));

		if (header.isAnim) {
			// Load bones associated with the mesh
			uint32_t numMeshBones;
			inFile.read(reinterpret_cast<char*>(&numMeshBones), sizeof(numMeshBones));
			mesh.bones.resize(numMeshBones);
			for (auto& bone : mesh.bones) {
				LoadBoneFromBinary(inFile, bone);
			}

			// Load offset matrices
			uint32_t numOffsetMatrices;
			inFile.read(reinterpret_cast<char*>(&numOffsetMatrices), sizeof(numOffsetMatrices));
			mesh.offsetMatrices.resize(numOffsetMatrices);
			inFile.read(reinterpret_cast<char*>(mesh.offsetMatrices.data()), numOffsetMatrices * sizeof(XMFLOAT4X4));
		}
	}
	
	// Load materials
	materials.resize(header.numMaterials);
	for (auto& material : materials) {
		for (auto& texturePath : material.texturePaths) {
			size_t pathLength;
			inFile.read(reinterpret_cast<char*>(&pathLength), sizeof(pathLength));
			texturePath.resize(pathLength);
			inFile.read(&texturePath[0], pathLength);
		}
	}

	// Load animations
	if (header.isAnim) {
		animations.resize(header.numAnimations);
		for (auto& anim : animations) {
			// Load animation name
			size_t nameLength;
			inFile.read(reinterpret_cast<char*>(&nameLength), sizeof(nameLength));
			anim.name.resize(nameLength);
			inFile.read(&anim.name[0], nameLength);

			// Load animation duration and ticks per second
			inFile.read(reinterpret_cast<char*>(&anim.duration), sizeof(anim.duration));
			inFile.read(reinterpret_cast<char*>(&anim.ticksPerSecond), sizeof(anim.ticksPerSecond));

			// Load animation channels
			uint32_t numChannels;
			inFile.read(reinterpret_cast<char*>(&numChannels), sizeof(numChannels));
			anim.channels.resize(numChannels);
			for (auto& channel : anim.channels) {
				// Load channel name
				size_t channelNameLength;
				inFile.read(reinterpret_cast<char*>(&channelNameLength), sizeof(channelNameLength));
				channel.name.resize(channelNameLength);
				inFile.read(&channel.name[0], channelNameLength);

				// Load keyframes
				uint32_t numKeyframes;
				inFile.read(reinterpret_cast<char*>(&numKeyframes), sizeof(numKeyframes));
				channel.keyframes.resize(numKeyframes);
				inFile.read(reinterpret_cast<char*>(channel.keyframes.data()), numKeyframes * sizeof(Keyframe));
			}
		}
	}

	inFile.close();
	return true;
}

void ModelConverter::ExtractBones(const aiNode* pNode, vector<BoneData>& bones) const {

	BoneData bone;
	//뼈 이름 저장
	bone.name = pNode->mName.C_Str();

	//뼈의 행렬
	aiMatrix4x4 aiTransform = pNode->mTransformation;
	aiTransform.Transpose(); // Transpose the matrix to match DirectX format
	memcpy(&bone.transformationMatrix, &aiTransform, sizeof(XMFLOAT4X4));

	//전체 뼈 벡터에 뼈 저장
	bones.push_back(bone);

	for (unsigned int i = 0; i < pNode->mNumChildren; ++i) {
		ExtractBones(pNode->mChildren[i], bones.back().children); // Pass children vector for recursive population
	}
}

void ModelConverter::SaveBoneToBinary(std::ofstream& outFile, const BoneData& bone) const {
	size_t nameLength = bone.name.length();
	outFile.write(reinterpret_cast<const char*>(&nameLength), sizeof(nameLength));
	outFile.write(bone.name.c_str(), nameLength);
	outFile.write(reinterpret_cast<const char*>(&bone.transformationMatrix), sizeof(bone.transformationMatrix));
	uint32_t numChildren = bone.children.size();
	outFile.write(reinterpret_cast<const char*>(&numChildren), sizeof(numChildren));
	for (const auto& child : bone.children) 
	{
		SaveBoneToBinary(outFile, child);
	}
}

void ModelConverter::LoadBoneFromBinary(std::ifstream& inFile, BoneData& bone) {
	size_t nameLength;
	inFile.read(reinterpret_cast<char*>(&nameLength), sizeof(nameLength));
	bone.name.resize(nameLength);
	inFile.read(&bone.name[0], nameLength);
	inFile.read(reinterpret_cast<char*>(&bone.transformationMatrix), sizeof(bone.transformationMatrix));
	uint32_t numChildren;
	inFile.read(reinterpret_cast<char*>(&numChildren), sizeof(numChildren));
	bone.children.resize(numChildren);
	for (auto& child : bone.children) {
		LoadBoneFromBinary(inFile, child);
	}
}

std::string ExtractFileName(const std::string& filePath) {
	size_t pos = filePath.find_last_of("\\/");  // 마지막 경로 구분자 위치 찾기
	if (pos != std::string::npos) {
		return filePath.substr(pos + 1);  // 파일명만 추출
	}
	return filePath;  // 경로 구분자가 없으면 전체 문자열 반환
}

// FillModelData 함수 수정
void ModelConverter::FillModelData(const aiScene* scene, ModelHeader& header, vector<BoneData>& bones, vector<MeshData>& meshes, vector<MaterialData>& materials, vector<AnimationData>& animations, bool printVertices) {
	header.numMeshes = scene->mNumMeshes;
	header.numMaterials = scene->mNumMaterials;
	header.isAnim = scene->HasAnimations();
	header.numAnimations = header.isAnim ? scene->mNumAnimations : 0;

	// 뼈 데이터를 먼저 채웁니다.
	if (header.isAnim) {
		ExtractBones(scene->mRootNode, bones);
	}

	// 메쉬 데이터를 채웁니다.
	meshes.resize(scene->mNumMeshes);
	for (unsigned int i = 0; i < scene->mNumMeshes; ++i) {
		const aiMesh* aiMesh = scene->mMeshes[i];
		MeshData& mesh = meshes[i];
		
		mesh.name = aiMesh->mName.C_Str(); // 메쉬 이름 저장
		mesh.vertexCount = aiMesh->mNumVertices; // 메쉬의 버텍스 숫자
		mesh.indexCount = aiMesh->mNumFaces * 3; // 메쉬의 인덱스 숫자

		if (header.isAnim) {
			// 애님 버텍스 데이터 채우기
			mesh.animVertices.resize(mesh.vertexCount);
			for (unsigned int j = 0; j < aiMesh->mNumVertices; ++j) {
				VertexAnim& vertex = mesh.animVertices[j];
				memcpy(&vertex.vPosition, &aiMesh->mVertices[j], sizeof(XMFLOAT3));
				memcpy(&vertex.vNormal, &aiMesh->mNormals[j], sizeof(XMFLOAT3));
				memcpy(&vertex.vTexcoord, &aiMesh->mTextureCoords[0][j], sizeof(XMFLOAT2));
				memcpy(&vertex.vTangent, &aiMesh->mTangents[j], sizeof(XMFLOAT3));
				// Blend indices and weights 초기화
				vertex.vBlendIndex = XMUINT4(0, 0, 0, 0);
				vertex.vBlendWeight = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
			}

			//뼈 개수
			for (unsigned int j = 0; j < aiMesh->mNumBones; ++j) {
				//메시에 영향을 끼치는 뼈중 하나
				const aiBone* aiBone = aiMesh->mBones[j];
				BoneData bone;

				bone.name = aiBone->mName.C_Str();
				aiMatrix4x4 aiTransform = aiBone->mOffsetMatrix;
				aiTransform.Transpose();
				memcpy(&bone.transformationMatrix, &aiTransform, sizeof(XMFLOAT4X4));

				//이 뼈가 영향을 끼치는 정점의 수
				for (unsigned int k = 0; k < aiBone->mNumWeights; ++k) {

					aiVertexWeight AIWeight = aiBone->mWeights[k];

					if (0.f == mesh.animVertices[AIWeight.mVertexId].vBlendWeight.x)
					{
						mesh.animVertices[AIWeight.mVertexId].vBlendIndex.x = j;
						mesh.animVertices[AIWeight.mVertexId].vBlendWeight.x = AIWeight.mWeight;
					}

					else if (0.f == mesh.animVertices[AIWeight.mVertexId].vBlendWeight.y)
					{
						mesh.animVertices[AIWeight.mVertexId].vBlendIndex.y = j;
						mesh.animVertices[AIWeight.mVertexId].vBlendWeight.y = AIWeight.mWeight;
					}

					else if (0.f == mesh.animVertices[AIWeight.mVertexId].vBlendWeight.z)
					{
						mesh.animVertices[AIWeight.mVertexId].vBlendIndex.z = j;
						mesh.animVertices[AIWeight.mVertexId].vBlendWeight.z = AIWeight.mWeight;
					}

					else
					{
						mesh.animVertices[AIWeight.mVertexId].vBlendIndex.w = j;
						mesh.animVertices[AIWeight.mVertexId].vBlendWeight.w = AIWeight.mWeight;
					}
					
				}
				mesh.bones.push_back(bone);
			}


			// 오프셋 행렬 채우기
			mesh.offsetMatrices.resize(aiMesh->mNumBones);
			for (unsigned int j = 0; j < aiMesh->mNumBones; ++j) {
				aiMatrix4x4 aiOffsetMatrix = aiMesh->mBones[j]->mOffsetMatrix;
				aiOffsetMatrix.Transpose();
				XMFLOAT4X4 offsetMatrix;
				memcpy(&offsetMatrix, &aiOffsetMatrix, sizeof(XMFLOAT4X4));
				mesh.offsetMatrices[j] = offsetMatrix;
			}
		}
		else {
			mesh.vertices.resize(mesh.vertexCount);
			for (unsigned int j = 0; j < aiMesh->mNumVertices; ++j) {
				Vertex& vertex = mesh.vertices[j];
				vertex.vPosition = { aiMesh->mVertices[j].x, aiMesh->mVertices[j].y, aiMesh->mVertices[j].z };
				vertex.vNormal = { aiMesh->mNormals[j].x, aiMesh->mNormals[j].y, aiMesh->mNormals[j].z };
				vertex.vTexcoord = { aiMesh->mTextureCoords[0][j].x, aiMesh->mTextureCoords[0][j].y };
				vertex.vTangent = { aiMesh->mTangents[j].x, aiMesh->mTangents[j].y, aiMesh->mTangents[j].z };
			}
		}

		mesh.indices.reserve(mesh.indexCount);
		for (unsigned int j = 0; j < aiMesh->mNumFaces; ++j) {
			const aiFace& face = aiMesh->mFaces[j];
			mesh.indices.insert(mesh.indices.end(), face.mIndices, face.mIndices + face.mNumIndices);
		}

		mesh.materialIndex = aiMesh->mMaterialIndex;
	}

	// 재료 데이터를 채웁니다.
	materials.resize(scene->mNumMaterials);
	for (unsigned int i = 0; i < scene->mNumMaterials; ++i) {
		const aiMaterial* aiMaterial = scene->mMaterials[i];
		MaterialData& material = materials[i];

		for (unsigned int j = 0; j < AI_TEXTURE_TYPE_MAX; ++j) {
			aiString str;

			if (aiMaterial->GetTexture(static_cast<aiTextureType>(j), 0, &str) == AI_SUCCESS) {
				std::string texturePath = str.C_Str();
				std::string fileName = ExtractFileName(texturePath);
				material.texturePaths[j] = fileName;
				//material.texturePaths[j + 1] = fileName + "1";
			}
		}
	}

	// 애니메이션 데이터를 채웁니다.
	 // 애니메이션 데이터를 채웁니다.
	if (header.isAnim) {
		animations.resize(scene->mNumAnimations);
		for (unsigned int i = 0; i < scene->mNumAnimations; ++i) {
			const aiAnimation* aiAnim = scene->mAnimations[i];
			AnimationData& anim = animations[i];

			anim.name = aiAnim->mName.C_Str();
			anim.duration = aiAnim->mDuration;
			anim.ticksPerSecond = aiAnim->mTicksPerSecond ? aiAnim->mTicksPerSecond : 25.0;

			anim.channels.resize(aiAnim->mNumChannels);
			for (unsigned int j = 0; j < aiAnim->mNumChannels; ++j) {
				const aiNodeAnim* aiChannel = aiAnim->mChannels[j];
				ChannelData& channel = anim.channels[j];

				channel.name = aiChannel->mNodeName.C_Str();
				channel.keyframes.resize(aiChannel->mNumPositionKeys);
				for (unsigned int k = 0; k < aiChannel->mNumPositionKeys; ++k) {
					Keyframe& keyframe = channel.keyframes[k];
					keyframe.position = { aiChannel->mPositionKeys[k].mValue.x, aiChannel->mPositionKeys[k].mValue.y, aiChannel->mPositionKeys[k].mValue.z };
					keyframe.scale = { aiChannel->mScalingKeys[k].mValue.x, aiChannel->mScalingKeys[k].mValue.y, aiChannel->mScalingKeys[k].mValue.z };
					keyframe.rotation = { aiChannel->mRotationKeys[k].mValue.x, aiChannel->mRotationKeys[k].mValue.y, aiChannel->mRotationKeys[k].mValue.z, aiChannel->mRotationKeys[k].mValue.w };
					keyframe.time = aiChannel->mPositionKeys[k].mTime;
				}
			}
		}
	}

}
