#pragma once
#include <vector>
#include <string>
#include "assimp/Importer.hpp"
#include "assimp/postprocess.h"
#include "assimp/material.h"
#include "assimp/scene.h"
#include <iostream>
#include <fstream>
#include <d3d11.h>
#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>
#include <DirectXMath.h>
#include <d3dcompiler.h>
#include <DirectXCollision.h>
using namespace DirectX;
using namespace std;

enum ModelType {
    TYPE_NONANIM,
    TYPE_ANIM,
};

struct Vertex {
    XMFLOAT3 vPosition;
    XMFLOAT3 vNormal;
    XMFLOAT2 vTexcoord;
    XMFLOAT3 vTangent;
};

struct VertexAnim {
    XMFLOAT3 vPosition;
    XMFLOAT3 vNormal;
    XMFLOAT2 vTexcoord;
    XMFLOAT3 vTangent;
    XMUINT4 vBlendIndex;
    XMFLOAT4 vBlendWeight;
};

struct BoneWeight {
    unsigned int vertexID;
    float weight;
};

struct BoneData {
    string name;
    XMFLOAT4X4 transformationMatrix;
    vector<BoneData> children; // Add children to store hierarchical data
};

struct VertexWeight {
    unsigned int vertexID;
    float weight;
};

struct MeshData {
    string name;
    unsigned int vertexCount;
    unsigned int indexCount;
    vector<Vertex> vertices;
    vector<VertexAnim> animVertices;
    vector<unsigned int> indices;
    unsigned int materialIndex;
    vector<BoneData> bones; // 뼈 데이터를 저장할 벡터 추가
    vector<XMFLOAT4X4> offsetMatrices; // 오프셋 행렬을 저장할 벡터 추가
};


struct Keyframe {
    XMFLOAT3 scale;
    XMFLOAT4 rotation;
    XMFLOAT3 position;
    float time;
};


struct ChannelData {
    string name;
    vector<Keyframe> keyframes;
};

struct AnimationData {
    string name;
    float duration;
    float ticksPerSecond;
    vector<ChannelData> channels;
};

struct MaterialData {
    string texturePaths[AI_TEXTURE_TYPE_MAX];
};

struct ModelHeader {
    bool isAnim;
    unsigned int numMeshes;
    unsigned int numMaterials;
    unsigned int numAnimations;
};