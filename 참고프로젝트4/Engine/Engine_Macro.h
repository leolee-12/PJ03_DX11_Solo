#ifndef Engine_Macro_h__
#define Engine_Macro_h__
#include <type_traits>

#define			PI		3.141592f

#ifndef			MSG_BOX
#define			MSG_BOX(_message)		MessageBox(nullptr, TEXT(_message), L"System Message", MB_OK)
#endif

#define			BEGIN(NAMESPACE)		namespace NAMESPACE {
#define			END						}

#define			USING(NAMESPACE)	using namespace NAMESPACE;

#define D3DCOLOR_ARGB(a,r,g,b) \
    ((D3DCOLOR)((((a)&0xff)<<24)|(((b)&0xff)<<16)|(((g)&0xff)<<8)|((r)&0xff)))

#ifdef	ENGINE_EXPORTS
#define ENGINE_DLL		_declspec(dllexport)
#else
#define ENGINE_DLL		_declspec(dllimport)
#endif

#define NULL_CHECK( _ptr)	\
	{if( (_ptr) == 0){__debugbreak();return;}}

#define NULL_CHECK_RETURN( _ptr, _return)	\
	{if( (_ptr) == 0){__debugbreak();return _return;}}

#define NULL_CHECK_MSG( _ptr, _message )		\
	{if( (_ptr) == 0){MessageBox(nullptr, _message, L"System Message",MB_OK); __debugbreak();}}

#define NULL_CHECK_RETURN_MSG( _ptr, _return, _message )	\
	{if( (_ptr) == 0){MessageBox(nullptr, _message, L"System Message",MB_OK); __debugbreak();return _return;}}


#define FAILED_CHECK(_hr)	if( ((HRESULT)(_hr)) < 0 )	\
	{ MessageBoxW(nullptr, L"Failed", L"System Error",MB_OK); __debugbreak(); return E_FAIL;}

#define FAILED_CHECK_RETURN(_hr, _return)	if( ((HRESULT)(_hr)) < 0 )		\
	{ MessageBoxW(nullptr, L"Failed", L"System Error",MB_OK); __debugbreak(); return _return;}

#define FAILED_CHECK_MSG( _hr, _message)	if( ((HRESULT)(_hr)) < 0 )	\
	{ MessageBoxW(nullptr, _message, L"System Message",MB_OK); __debugbreak();return E_FAIL;}

#define FAILED_CHECK_RETURN_MSG( _hr, _return, _message)	if( ((HRESULT)(_hr)) < 0 )	\
	{ MessageBoxW(nullptr, _message, L"System Message",MB_OK); __debugbreak();return _return;}

#define chSTR2(x) #x
#define chSTR(x)  chSTR2(x)

#define TODO_MSG(desc) message(__FILE__ "(" chSTR(__LINE__) "): --------" #desc "--------")
#define chFixLater message(__FILE__ "(" chSTR(__LINE__) "): --------Fix this later--------")

#define FixLater \
    do { \
    __pragma(chFixLater) \
    __pragma (warning(push)) \
    __pragma (warning(disable:4127)) \
    } while(0) \
    __pragma (warning(pop))

#define CODE_MSG(desc) \
    do { \
    __pragma(TODO_MSG(desc)) \
    __pragma (warning(push)) \
    __pragma (warning(disable:4127)) \
    } while(0) \
    __pragma (warning(pop))


#define DELETER(CLASSNAME)	[](CLASSNAME* _ptr) \
	{											\
		if(is_base_of<CBase, CLASSNAME>::value)	\
			_ptr->Release_Shared();				\
		else                                    \
			delete _ptr;						\
	}								
												 

#define RETURN_EFAIL {assert(false); return E_FAIL;}

#define IMPLEMENT_CREATE(CLASSNAME)									    										\
	shared_ptr<CLASSNAME> CLASSNAME::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)				\
	{																											\
		shared_ptr<CLASSNAME> pInstance(new CLASSNAME(pDevice,pContext), DELETER(CLASSNAME));					\
		if (FAILED(pInstance->Initialize_Prototype()))															\
		{																										\
			assert(false);																						\
		}																										\
		return pInstance;																						\
	}

#define IMPLEMENT_CREATE_EX1(CLASSNAME,TYPE1,MVALUE1)									    					\
	shared_ptr<CLASSNAME> CLASSNAME::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, TYPE1 MVALUE1)\
	{																											\
		shared_ptr<CLASSNAME> pInstance(new CLASSNAME(pDevice,pContext), DELETER(CLASSNAME));					\
		if (FAILED(pInstance->Initialize_Prototype(MVALUE1)))													\
		{																										\
			assert(false);																						\
		}																										\
		return pInstance;																						\
	}

#define IMPLEMENT_CREATE_EX2(CLASSNAME,TYPE1,MVALUE1,TYPE2,MVALUE2)									    					\
	shared_ptr<CLASSNAME> CLASSNAME::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, TYPE1 MVALUE1,  TYPE2 MVALUE2)\
	{																											\
		shared_ptr<CLASSNAME> pInstance(new CLASSNAME(pDevice,pContext), DELETER(CLASSNAME));					\
		if (FAILED(pInstance->Initialize_Prototype(MVALUE1,MVALUE2)))											\
		{																										\
			assert(false);																						\
		}																										\
		return pInstance;																						\
	}

#define IMPLEMENT_CREATE_EX3(CLASSNAME,TYPE1,MVALUE1,TYPE2,MVALUE2,TYPE3,MVALUE3)									    					\
	shared_ptr<CLASSNAME> CLASSNAME::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, TYPE1 MVALUE1,  TYPE2 MVALUE2,  TYPE3 MVALUE3)\
	{																											\
		shared_ptr<CLASSNAME> pInstance(new CLASSNAME(pDevice,pContext), DELETER(CLASSNAME));					\
		if (FAILED(pInstance->Initialize_Prototype(MVALUE1,MVALUE2,MVALUE3)))											\
		{																										\
			assert(false);																						\
		}																										\
		return pInstance;																						\
	}

#define IMPLEMENT_CREATE_EX4(CLASSNAME,TYPE1,MVALUE1,TYPE2,MVALUE2,TYPE3,MVALUE3,TYPE4,MVALUE4)									    					\
	shared_ptr<CLASSNAME> CLASSNAME::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, TYPE1 MVALUE1,  TYPE2 MVALUE2,  TYPE3 MVALUE3,  TYPE4 MVALUE4)\
	{																											\
		shared_ptr<CLASSNAME> pInstance(new CLASSNAME(pDevice,pContext), DELETER(CLASSNAME));					\
		if (FAILED(pInstance->Initialize_Prototype(MVALUE1,MVALUE2,MVALUE3,MVALUE4)))											\
		{																										\
			assert(false);																						\
		}																										\
		return pInstance;																						\
	}


#define IMPLEMENT_CLONE(CLASSNAME,BASENAME)															\
	shared_ptr<BASENAME> CLASSNAME::Clone(void* pArg)												\
	{																								\
		shared_ptr<CLASSNAME> pInstance(new CLASSNAME(*this), DELETER(CLASSNAME));					\
		if(is_base_of<CGameObject, CLASSNAME>::value)												\
		{																							\
			CGameObject::GAMEOBJECT_DESC* pDesc = (CGameObject::GAMEOBJECT_DESC*)pArg;				\
			if(pDesc != nullptr && pDesc->bIsCloneInPool)											\
				return pInstance;																    \
		}																						    \
		if (FAILED(pInstance->Initialize(pArg)))													\
		{																							\
			assert(false);																			\
		}																							\
		return pInstance;																			\
	}


#define GET_SINGLE(CLASSNAME)		CLASSNAME::GetInstance()

#define NO_COPY(CLASSNAME)								\
		private:										\
		CLASSNAME(const CLASSNAME&);					\
		CLASSNAME& operator = (const CLASSNAME&);		

#define DECLARE_SINGLETON(CLASSNAME)					\
		NO_COPY(CLASSNAME)								\
		private:										\
		static CLASSNAME*	m_pInstance;				\
		public:											\
		static CLASSNAME*	GetInstance( void );		\
		static unsigned long DestroyInstance( void );			

#define IMPLEMENT_SINGLETON(CLASSNAME)							\
		CLASSNAME*	CLASSNAME::m_pInstance = nullptr;			\
		CLASSNAME*	CLASSNAME::GetInstance( void )	{			\
			if(NULL == m_pInstance) {							\
				m_pInstance = new CLASSNAME;					\
			}													\
			return m_pInstance;									\
		}														\
		unsigned long CLASSNAME::DestroyInstance( void ) {		\
			unsigned long dwRefCnt = 0;							\
			if(nullptr != m_pInstance)							\
			{													\
				dwRefCnt = m_pInstance->Release();				\
				if(0 == dwRefCnt)								\
					m_pInstance = nullptr;						\
			}													\
			return dwRefCnt;									\
		}

#define INFO_CLASS(DERIVEDCLASS,BASECLASS)						\
		public:													\
			using DERIVED	= DERIVEDCLASS;						\
			using BASE		= BASECLASS;							

#define			THIS_CLASS(CLASSNAME)		using ThisClass = CLASSNAME;
#define			BASE_CLASS(CLASSNAME)		using Base = CLASSNAME;
#define			DERIVED_CLASS(BASENAME, THISCLASS)	\
					THIS_CLASS(THISCLASS)			\
					BASE_CLASS(BASENAME)
#define			DERIVED_CLASS_SINGLETON(BASENAME, THISCLASS) \
					DERIVED_CLASS(BASENAME, THISCLASS)	\
					DECLARE_SINGLETON(THISCLASS)

// getset 매크로
// 일반 Set 함수
#define SET(TYPE, MVALUE, NAME)										\
			void Set_##NAME(TYPE value) { MVALUE = value; }

// 입력 값이 const Type
#define SET_C(TYPE, MVALUE, NAME)									\
			void Set_##NAME(const TYPE value) { MVALUE = value; }
// 입력 값이 Type const (포인터 용)
#define SET__C(TYPE, MVALUE, NAME)									\
			void Set_##NAME(TYPE const value) { MVALUE = value; }
// 입력 값이 const Type const (포인터 용)
#define SET_C_C(TYPE, MVALUE, NAME)									\
			void Set_##NAME(const TYPE const value) { MVALUE = value; }

// 입력 값이 Type*
#define SET_PTR(TYPE, MVALUE, NAME)									\
			void Set_##NAME(TYPE* value) { MVALUE = *value; }
// 입력 값이 cosnt Type*
#define SET_C_PTR(TYPE, MVALUE, NAME)								\
			void Set_##NAME(const TYPE* value) { MVALUE = *value; }
// 입력 값이 Type const*
#define SET_PTR_C(TYPE, MVALUE, NAME)								\
			void Set_##NAME(TYPE const* value) { MVALUE = *value; }
// 입력 값이 const Type const*
#define SET_C_PTR_C(TYPE, MVALUE, NAME)								\
			void Set_##NAME(const TYPE const* value) { MVALUE = *value; }

// 입력 값이 Type&
#define SET_REF(TYPE, MVALUE, NAME)									\
			void Set_##NAME(TYPE& value) { MVALUE = value; }
// 입력 값이 const Type&
#define SET_C_REF(TYPE, MVALUE, NAME)								\
			void Set_##NAME(const TYPE& value) { MVALUE = value; }
// 입력 값이 Type const&
#define SET_REF_C(TYPE, MVALUE, NAME)								\
			void Set_##NAME(TYPE& const value) { MVALUE = value; }
// 입력 값이 const Type const&
#define SET_C_REF_C(TYPE, MVALUE, NAME)								\
			void Set_##NAME(const TYPE const& value) { MVALUE = value; }

// 입력 값이 Type&&
#define SET_RREF(TYPE, MVALUE, NAME)								\
			void Set_##NAME(TYPE&& value) { MVALUE = value; }
// 입력 값이 const Type&&
#define SET_C_RREF(TYPE, MVALUE, NAME)								\
			void Set_##NAME(const TYPE&& value) { MVALUE = value; }


// 반환값이 Type
#define GET(TYPE, MVALUE, NAME)										\
			TYPE Get_##NAME() { return MVALUE; }	

// 반환값이 const Type
#define GET_C(TYPE, MVALUE, NAME)									\
			const TYPE Get_##NAME() const { return MVALUE; }	
// 반환값이 Type const
#define GET__C(TYPE, MVALUE, NAME)									\
			TYPE const Get_##NAME() const { return MVALUE; }	
// 반환값이 const Type const
#define GET_C_C(TYPE, MVALUE, NAME)									\
			const TYPE const Get_##NAME() const { return MVALUE; }	

// 반환값이 Type&
#define GET_REF(TYPE, MVALUE, NAME)									\
			TYPE& Get_##NAME() { return MVALUE; }	
// 반환값이 const Type&
#define GET_C_REF(TYPE, MVALUE, NAME)								\
			const TYPE& Get_##NAME() const { return MVALUE; }	
// 반환값이 Type const&
#define GET_REF_C(TYPE, MVALUE, NAME)								\
			TYPE const& Get_##NAME() const { return MVALUE; }	
// 반환값이 const Type const&
#define GET_C_REF_C(TYPE, MVALUE, NAME)								\
			const TYPE const& Get_##NAME() const { return MVALUE; }

// 반환값이 Type* 
#define GET_PTR(TYPE, MVALUE, NAME)									\
			TYPE* Get_##NAME() { return &MVALUE; }
// 반환값이 const Type*
#define GET_C_PTR(TYPE, MVALUE, NAME)								\
			const TYPE* Get_##NAME() const { return &MVALUE; }
// 반환값이 Type const*
#define GET_PTR_C(TYPE, MVALUE, NAME)								\
			TYPE const* Get_##NAME() const { return &MVALUE; }
// 반환값이 const Type const*
#define GET_C_PTR_C(TYPE, MVALUE, NAME)								\
			const TYPE const* Get_##NAME() const { return &MVALUE; }



// 복수 GET 매크로
#pragma region GET SET 함수 복수로드 매크로
#define GETSET_EX1(TYPE, MVALUE, NAME, ARG1)		\
			ARG1##(TYPE, MVALUE, NAME)

#define GETSET_EX2(TYPE, MVALUE, NAME, ARG1, ARG2)			  \
			ARG1##(TYPE, MVALUE, NAME)	\
			ARG2##(TYPE, MVALUE, NAME)


#define GETSET_EX3(TYPE, MVALUE, NAME, ARG1, ARG2, ARG3)			  \
			ARG1##(TYPE, MVALUE, NAME)	\
			ARG2##(TYPE, MVALUE, NAME)	\
			ARG3##(TYPE, MVALUE, NAME)

#define GETSET_EX4(TYPE, MVALUE, NAME, ARG1, ARG2, ARG3, ARG4)				  \
			ARG1##(TYPE, MVALUE, NAME)	\
			ARG2##(TYPE, MVALUE, NAME)	\
			ARG3##(TYPE, MVALUE, NAME)	\
			ARG4##(TYPE, MVALUE, NAME)

#define GETSET_EX5(TYPE, MVALUE, NAME, ARG1, ARG2, ARG3, ARG4, ARG5)				  \
			ARG1##(TYPE, MVALUE, NAME)	\
			ARG2##(TYPE, MVALUE, NAME)	\
			ARG3##(TYPE, MVALUE, NAME)	\
			ARG4##(TYPE, MVALUE, NAME)	\
			ARG5##(TYPE, MVALUE, NAME)

#define GETSET_EX6(TYPE, MVALUE, NAME, ARG1, ARG2, ARG3, ARG4, ARG5, ARG6)					  \
			ARG1##(TYPE, MVALUE, NAME)	\
			ARG2##(TYPE, MVALUE, NAME)	\
			ARG3##(TYPE, MVALUE, NAME)	\
			ARG4##(TYPE, MVALUE, NAME)	\
			ARG5##(TYPE, MVALUE, NAME)	\
			ARG6##(TYPE, MVALUE, NAME)

#define GETSET_EX7(TYPE, MVALUE, NAME, ARG1, ARG2, ARG3, ARG4, ARG5, ARG6, ARG7)					 \
			ARG1##(TYPE, MVALUE, NAME)	\
			ARG2##(TYPE, MVALUE, NAME)	\
			ARG3##(TYPE, MVALUE, NAME)	\
			ARG4##(TYPE, MVALUE, NAME)	\
			ARG5##(TYPE, MVALUE, NAME)	\
			ARG6##(TYPE, MVALUE, NAME)	\
			ARG7##(TYPE, MVALUE, NAME)

#define GETSET_EX8(TYPE, MVALUE, NAME, ARG1, ARG2, ARG3, ARG4, ARG5, ARG6, ARG7, ARG8)					 \
			ARG1##(TYPE, MVALUE, NAME)	\
			ARG2##(TYPE, MVALUE, NAME)	\
			ARG3##(TYPE, MVALUE, NAME)	\
			ARG4##(TYPE, MVALUE, NAME)	\
			ARG5##(TYPE, MVALUE, NAME)	\
			ARG6##(TYPE, MVALUE, NAME)	\
			ARG7##(TYPE, MVALUE, NAME)	\
			ARG8##(TYPE, MVALUE, NAME)

#define GETSET_EX9(TYPE, MVALUE, NAME, ARG1, ARG2, ARG3, ARG4, ARG5, ARG6, ARG7, ARG8, ARG9)			   \
			ARG1##(TYPE, MVALUE, NAME)	\
			ARG2##(TYPE, MVALUE, NAME)	\
			ARG3##(TYPE, MVALUE, NAME)	\
			ARG4##(TYPE, MVALUE, NAME)	\
			ARG5##(TYPE, MVALUE, NAME)	\
			ARG6##(TYPE, MVALUE, NAME)	\
			ARG7##(TYPE, MVALUE, NAME)	\
			ARG8##(TYPE, MVALUE, NAME)	\
			ARG9##(TYPE, MVALUE, NAME)

#define GETSET_EX10(TYPE, MVALUE, NAME, ARG1, ARG2, ARG3, ARG4, ARG5, ARG6, ARG7, ARG8, ARG9, ARG10)		\
			ARG1##(TYPE, MVALUE, NAME)	\
			ARG2##(TYPE, MVALUE, NAME)	\
			ARG3##(TYPE, MVALUE, NAME)	\
			ARG4##(TYPE, MVALUE, NAME)	\
			ARG5##(TYPE, MVALUE, NAME)	\
			ARG6##(TYPE, MVALUE, NAME)	\
			ARG7##(TYPE, MVALUE, NAME)	\
			ARG8##(TYPE, MVALUE, NAME)	\
			ARG9##(TYPE, MVALUE, NAME)	\
			ARG10##(TYPE, MVALUE, NAME)

////////////////////////////////////////////////////////////////////////////////////
//Custom
////////////////////////////////////////////////////////////////////////////////////

//Status용 Set
#define SET_S(TYPE1,TYPE2, MVALUE, NAME)										\
			void Set_##NAME(TYPE1 type ,TYPE2 value) { MVALUE[type] = value; }

//Status용 Get
#define GET_S(TYPE1,TYPE2, MVALUE, NAME)										\
			TYPE2 Get_##NAME(TYPE1 type) { return MVALUE[type]; }	

//Status용 Add
#define ADD_S(TYPE1,TYPE2, MVALUE, NAME)										\
			void Add_##NAME(TYPE1 type ,TYPE2 value) { MVALUE[type] += value; }

// Add
#define ADD(TYPE, MVALUE, NAME)										\
			void Add_##NAME(TYPE value) { MVALUE += value; }

//Status용 GETSET
#define GETSET_EX2_S(TYPE1,TYPE2, MVALUE, NAME, ARG1, ARG2)			  \
			ARG1##(TYPE1,TYPE2, MVALUE, NAME)	\
			ARG2##(TYPE1,TYPE2, MVALUE, NAME)

//Status용 GETSETADD
#define GETSETADD_EX3_S(TYPE1,TYPE2, MVALUE, NAME, ARG1, ARG2, ARG3)			  \
			ARG1##(TYPE1,TYPE2, MVALUE, NAME)	\
			ARG2##(TYPE1,TYPE2, MVALUE, NAME)	\
			ARG3##(TYPE1,TYPE2, MVALUE, NAME)



#endif // Engine_Macro_h__
