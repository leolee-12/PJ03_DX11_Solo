#pragma once
#include <vector>
#include <string>
#include <list>
#include <unordered_map>
#include <fstream>


    struct Save_Key
    {
        _wstring key;
        _int iD;
        _float2 vTexCoord;
    };
    struct MoveTex_Node_Save
    {
        _int MoveTex_node_id;
        _float2 fDirection;
        _float fSpeed;
    };
    struct Node_Position
    {
        _int nodeID;          // 노드 ID
        ImVec2 nodePosition;  // 노드의 위치
    };
    struct Sprite_Node_Save
    {
        _int Sprite_node_id;
        _bool isLoop;
        _float2 fSpriteSizeNumber;
        _float fSpeed;
    };
    struct Link_Save
    {
        _int srcNodeID;   // 연결된 소스 노드 ID
        _int destNodeID;  // 연결된 목적지 노드 ID
    };
	typedef struct
	{
		vector<Save_Key> keys;                        // 텍스처 키 정보
		vector<Node_Position> nodePositions;          // 각 노드의 위치
		vector<MoveTex_Node_Save> moveTexNodes;       // 무브 텍스 노드 정보
		vector<Sprite_Node_Save> spriteNodes;         // 스프라이트 노드 정보
		vector<Link_Save> links;                      // 노드 연결 정보
        vector<_float2> vTexCoord;
	}Shader_Tab_Save;

