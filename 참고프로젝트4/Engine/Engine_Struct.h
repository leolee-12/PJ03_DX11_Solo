#pragma once

namespace Engine
{
	typedef struct tagGraphicDesc
	{
		enum WINMODE { WINMODE_FULL, WINMODE_WIN, WINMODE_END };

		WINMODE			eWinMode;
		HWND			hWnd;
		unsigned int	iBackBufferSizeX;
		unsigned int	iBackBufferSizeY;

	}GRAPHIC_DESC;

	typedef struct tagMaterialDesc
	{
		class CTexture_Data* pMtrlTextures[18];
	}MATERIAL_DESC;
	
	typedef struct tagInt32
	{
		tagInt32()
			:iX(0), iY(0), iZ(0) {}
		tagInt32(unsigned int ix, unsigned int iy, unsigned int iz)
			: iX(ix), iY(iy), iZ(iz) {}

		unsigned int iX, iY, iZ;

	}_uint3;

	typedef struct ENGINE_DLL tagVertex_Position
	{
		XMFLOAT3		vPosition;

		static const unsigned int					iNumElements = 1;
		static const D3D11_INPUT_ELEMENT_DESC		Elements[iNumElements];
	}VTXPOS;

	typedef struct ENGINE_DLL tagVertex_Point
	{
		XMFLOAT3		vPosition;
		XMFLOAT2		vPSize;

		static const unsigned int					iNumElements = 2;
		static const D3D11_INPUT_ELEMENT_DESC		Elements[iNumElements];
	}VTXPOINT;

	typedef struct ENGINE_DLL tagVertex_Position_Texcoord
	{
		XMFLOAT3		vPosition;
		XMFLOAT2		vTexcoord;

		static const unsigned int					iNumElements = 2;
		static const D3D11_INPUT_ELEMENT_DESC		Elements[iNumElements];
	}VTXPOSTEX;

	typedef struct ENGINE_DLL tagVertex_Cube
	{
		XMFLOAT3		vPosition;
		XMFLOAT3		vTexcoord;

		static const unsigned int					iNumElements = 2;
		static const D3D11_INPUT_ELEMENT_DESC		Elements[iNumElements];
	}VTXCUBE;

	typedef struct ENGINE_DLL tagVertex_Position_Normal_Texcoord
	{
		XMFLOAT3		vPosition;
		XMFLOAT3		vNormal;
		XMFLOAT2		vTexcoord;

		static const unsigned int					iNumElements = 3;
		static const D3D11_INPUT_ELEMENT_DESC		Elements[iNumElements];
	}VTXNORTEX;

	typedef struct ENGINE_DLL tagVertex_Mesh
	{
		XMFLOAT3		vPosition;
		XMFLOAT3		vNormal;
		XMFLOAT2		vTexcoord;
		XMFLOAT3		vTangent; 
		XMFLOAT3		vBinormal; // 종법선 벡터 : 접선과 법선의 외적된 결과, 노말의 수직인 벡터.	탄젠트 공간 y 축

		static const unsigned int					iNumElements = 5;
		static const D3D11_INPUT_ELEMENT_DESC		Elements[iNumElements];
	}VTXMESH;

	typedef struct ENGINE_DLL tagVertex_AnimMesh
	{
		XMFLOAT3		vPosition;
		XMFLOAT3		vNormal;
		XMFLOAT2		vTexcoord;
		XMFLOAT3		vTangent;
		XMUINT4			vBlendIndices;
		XMFLOAT4		vBlendWeights;

		static const unsigned int					iNumElements = 6;
		static const D3D11_INPUT_ELEMENT_DESC		Elements[iNumElements];
	}VTXMESHANIM;

	typedef struct ENGINE_DLL tagVertex_VTAnimMesh
	{
		XMFLOAT3		vPosition;
		XMFLOAT3		vNormal;
		XMFLOAT2		vTexcoord;
		XMFLOAT3		vTangent;
		XMUINT4			vBlendIndices;
		XMFLOAT4		vBlendWeights;
		unsigned int	iInstanceIndex;

		static const unsigned int					iNumElements = 7;
		static const D3D11_INPUT_ELEMENT_DESC		Elements[iNumElements];
	}VTX_VTF_MESHANIM;

	typedef struct ENGINE_DLL tagInstance_Mesh
	{
		XMFLOAT4		vRight, vUp, vLook, vPosition;

	}VTXINSTANCEMESH;

	typedef struct tagVertex_Instance
	{
		XMFLOAT4		vRight, vUp, vLook, vPosition;
		XMFLOAT4		vColor;
		XMFLOAT4		vDir; // 인스턴스마다 방향벡터에 따른 Right, Up, Look을 재배치하기 위해서
		XMFLOAT2		vTextureUV; // 인스턴스마다 다르게 스프라이트를 돌려주가 위해서
		unsigned int	iMaskIndex; // 인스턴스마다 마스크 텍스쳐를 다르게 사용하기 위해서

	}VTXINSTANCE;

	typedef struct ENGINE_DLL tagVertex_Particle_Rect
	{
		static const unsigned int					iNumElements = 10;
		static const D3D11_INPUT_ELEMENT_DESC		Elements[iNumElements];
	}VTX_PARTICLE_RECT;

	typedef struct ENGINE_DLL tagVertex_Particle_Point
	{
		static const unsigned int					iNumElements = 10;
		static const D3D11_INPUT_ELEMENT_DESC		Elements[iNumElements];
	}VTX_PARTICLE_POINT;

	typedef struct ENGINE_DLL tagInstance_MeshDesc
	{
		static const unsigned int					iNumElements = 9;
		static const D3D11_INPUT_ELEMENT_DESC		Elements[iNumElements];
	}VTX_MESH_INSTANCE;


	typedef struct tagKeyFrame 
	{
		XMFLOAT3		vScale, vPosition;
		XMFLOAT4		vRotation;
		float			fTrackPosition;
	}KEYFRAME;

	typedef struct tagCollisionInfo
	{
		XMFLOAT3		vLine;
		XMFLOAT3		vNormal;
	}COLLISIONINFO;

	typedef struct tagCameraShakeDesc
	{
		bool		bShaking;		// 카메라 쉐이킹 여부
		float		fAccTime;		// 누적시간

		float		fOrigin_Amplitude; //첫 진폭
		float		fAmplitude;	// 현재 진폭
		float		fDuration;	// 흔드는 시간
		float		fSpeed;		// 흔드는 속도

		//XMFLOAT3	vOriginPos;	// 원래 있던 위치

	}CAMERASHAKE_DESC;

	typedef struct tagLightDesc
	{																	  //그림자용 광원
		enum TYPE { TYPE_DIRECTIONAL, TYPE_POINT, TYPE_SPOT, TYPE_SYMMETRY, TYPE_SHADOW, TYPE_END };
		TYPE			eType;
		XMFLOAT4		vDirection;
		XMFLOAT4		vPosition;

		string			strLightName = "";

		float			fRange;
		float			fSpotPower; 
		XMFLOAT2		fThetaPi; //For Spot

		// 감쇄값 : (Range * Quad^2 )+ (Range * Linear) + Const
		float			fConstDamping = { FLT_MIN };	// 고정 감쇄
		float			fLinearDamping = { 1.f };		// 선형 감쇄
		float			fQuadDamping = { 0.f };			// 제곱 감쇄

		XMFLOAT4		vDiffuse;
		XMFLOAT4		vAmbient;
		XMFLOAT4		vSpecular;
		XMFLOAT4		vEmissive;

		bool			bUseVolumetric = { false };
		// 볼류메트릭의 감쇄 설정이 달라 감쇄값을 분리, 값이 커지면 빛이 멀수록 빠르게 약해짐 (1이 아무수치 적용 안되었을 때)
		float			fVolumeQuadDamping = { 1.f };
	}LIGHT_DESC;


	typedef struct tagShadowLightDesc
	{
		XMFLOAT4	vEye;
		XMFLOAT4	vAt;
		float		fFovy = XMConvertToRadians(60.0f);
		float		fAspect = 1280.f / 720.f;
		float		fNear = 0.1f;
		float		fFar = 300.0f;
	}SHADOWLIGHT_DESC;

	//유진 사용 - UI/카메라 모션에 공동 사용
	typedef struct	tagMotion_Set_Data {
		int				iMotionPointMax;
		int				iCurrentMotionPoint;
		float			fDuration;
		float			fTrackPosition;
		bool			bisLoop;
	}MOTION_SETTING_DESC;

}