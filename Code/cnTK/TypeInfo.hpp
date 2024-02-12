/*- cnTK - Type Information -----------------------------------------------*/
/*         Developer : Code009                                             */
/*         Create on : 2023-02-25                                          */
/*-------------------------------------------------------------------------*/
#ifndef __cnLibrary_cnTK_TypeInfo_H__
#define	__cnLibrary_cnTK_TypeInfo_H__
/*-------------------------------------------------------------------------*/
#include <cnTK/Common.hpp>
#include <cnTK/TypeTraits.hpp>
#include <cnTK/Numerical.hpp>
/*-------------------------------------------------------------------------*/
#if	cnLibrary_CPPFEATURELEVEL >= 1
//---------------------------------------------------------------------------
#include <cnTK/TKMacrosDeclare.inc>
//---------------------------------------------------------------------------
namespace cnLibrary{
//---------------------------------------------------------------------------
namespace cnVar{
//---------------------------------------------------------------------------
struct cTypeIdentity
{
};
//---------------------------------------------------------------------------
template<class T>
struct cTypeIDDefinition
{
	static const cTypeIdentity Value;
};
//---------------------------------------------------------------------------
template<class T>
const cTypeIdentity cTypeIDDefinition<T>::Value;
//---------------------------------------------------------------------------
}	// namespace cnVar
//---------------------------------------------------------------------------
typedef const cnVar::cTypeIdentity* tTypeID;
//---------------------------------------------------------------------------
namespace cnVar{
//---------------------------------------------------------------------------

#if cnLibrary_CPPFEATURE_CONSTEXPR >= 200704L

template<class T>
struct TTypeID
{
	static constexpr tTypeID Value=&cTypeIDDefinition<T>::Value;
};

// cnLibrary_CPPFEATURE_CONSTEXPR >= 200704L
#else
// cnLibrary_CPPFEATURE_CONSTEXPR < 200704L
template<class T>
struct TTypeID
{
	static const tTypeID Value;
};
template<class T>
const tTypeID TTypeID<T>::Value=&cTypeIDDefinition<T>::Value;

#endif	// cnLibrary_CPPFEATURE_CONSTEXPR < 200704L


#if cnLibrary_CPPFEATURE_VARIABLE_TEMPLATES >= 201304L && cnLibrary_CPPFEATURE_INLINE_VARIABLES >= 201606L

template<class T>
inline cnLib_CONSTVAR tTypeID TypeID=&cTypeIDDefinition<T>::Value;

#endif	// cnLibrary_CPPFEATURE_VARIABLE_TEMPLATES >= 201304L && cnLibrary_CPPFEATURE_INLINE_VARIABLES >= 201606L

//---------------------------------------------------------------------------
}	// namespace cnVar
//---------------------------------------------------------------------------
}	// namespace cnLibrary
//---------------------------------------------------------------------------
namespace cnLib_THelper{
//---------------------------------------------------------------------------
namespace Var_TH{
//---------------------------------------------------------------------------

#if cnLibrary_CPPFEATURE_VARIADIC_TEMPLATES >= 200704L

template<class TRet,class...TArgs>
struct FunctionInfo
{
	typedef TRet tReturn;
	template<uIntn Index>
	struct Arguments : cnVar::TSelect<Index,TArgs...>{};
	static cnLib_CONSTVAR uIntn ArgumentCount=sizeof...(TArgs);
};
template<class TRet>
struct FunctionInfo<TRet>
{
	typedef TRet tReturn;
	static cnLib_CONSTVAR uIntn ArgumentCount=0;
};
template<class TClass,class TRet,class...TArgs>
struct MemberFunctionInfo : FunctionInfo<TRet,TArgs...>
{
	typedef TClass tClass;
};

// cnLibrary_CPPFEATURE_VARIADIC_TEMPLATES >= 200704L
#else
// cnLibrary_CPPFEATURE_VARIADIC_TEMPLATES < 200704L
	
template<class TRet,cnLib_VARIADIC_TEMPLATE_PARAMETER_OPT>
struct FunctionInfo
{
	typedef TRet tReturn;
	template<uIntn Index>
	struct Arguments : cnVar::TSelect<Index,cnLib_VARIADIC_TEMPLATE_EXPAND>{};
	static cnLib_CONSTVAR uIntn ArgumentCount=cnLib_VARIADIC_TEMPLATE_PARAMETER_COUNT;
};
template<class TRet>
struct FunctionInfo<TRet>
{
	typedef TRet tReturn;
	static cnLib_CONSTVAR uIntn ArgumentCount=0;
};
template<class TClass,class TRet,cnLib_VARIADIC_TEMPLATE_PARAMETER>
struct MemberFunctionInfo : FunctionInfo<TRet,cnLib_VARIADIC_TEMPLATE_EXPAND>
{
	typedef TClass tClass;
};

#endif // cnLibrary_CPPFEATURE_VARIADIC_TEMPLATES < 200704L

//---------------------------------------------------------------------------
}	// namespace Var_TH
//---------------------------------------------------------------------------
}	// namespace cnLib_THelper
//---------------------------------------------------------------------------
namespace cnLibrary{
//---------------------------------------------------------------------------
namespace cnVar{

#if 0

template<class TArray>
struct TArrayInfo;

template<class TElement>
struct TArrayInfo<TElement []>
{
	typedef TElement tElement;
	static cnLib_CONSTVAR uIntn Length=0;
};
template<class TElement,uIntn ArrayLength>
struct TArrayInfo<TElement [ArrayLength]>
{
	typedef TElement tElement;
	static cnLib_CONSTVAR uIntn Length=ArrayLength;
};


#if cnLibrary_CPPFEATURE_VARIADIC_TEMPLATES >= 200704L

template<class TFunctionPtr>
struct TFunctionInfo;

// function info

template<class TRet,class...TArgs>
struct TFunctionInfo<TRet (TArgs...)>
	: cnLib_THelper::Var_TH::FunctionInfo<TRet,TArgs...>
{
	static cnLib_CONSTVAR bool NoException=false;
};


template<class TRet,class...TArgs>
struct TFunctionInfo<TRet (*)(TArgs...)> 
	: cnLib_THelper::Var_TH::FunctionInfo<TRet,TArgs...>
{
	static cnLib_CONSTVAR bool NoException=false;
};

#if cnLibrary_CPPFEATURE_NOEXCEPT_FUNC_TYPE >= 201510L

template<class TRet,class...TArgs>
struct TFunctionInfo<TRet (TArgs...)noexcept>
	: cnLib_THelper::Var_TH::FunctionInfo<TRet,TArgs...>
{
	static cnLib_CONSTVAR bool NoException=true;
};


template<class TRet,class...TArgs>
struct TFunctionInfo<TRet (*)(TArgs...)noexcept> 
	: cnLib_THelper::Var_TH::FunctionInfo<TRet,TArgs...>
{
	static cnLib_CONSTVAR bool NoException=true;
};

#endif // cnLibrary_CPPFEATURE_NOEXCEPT_FUNC_TYPE >= 201510L


// member function info

template<class TClass,class TRet,class...TArgs>
struct TFunctionInfo<TRet (TClass::*)(TArgs...)>
	: cnLib_THelper::Var_TH::MemberFunctionInfo<TClass,TRet,TArgs...>
{
	typedef TClass tClassArgument;
};
template<class TClass,class TRet,class...TArgs>
struct TFunctionInfo<TRet (TClass::*)(TArgs...)const>
	: cnLib_THelper::Var_TH::MemberFunctionInfo<TClass,TRet,TArgs...>
{
	typedef const TClass tClassArgument;
};
template<class TClass,class TRet,class...TArgs>
struct TFunctionInfo<TRet (TClass::*)(TArgs...)volatile>
	: cnLib_THelper::Var_TH::MemberFunctionInfo<TClass,TRet,TArgs...>
{
	typedef volatile TClass tClassArgument;
};
template<class TClass,class TRet,class...TArgs>
struct TFunctionInfo<TRet (TClass::*)(TArgs...)const volatile>
	: cnLib_THelper::Var_TH::MemberFunctionInfo<TClass,TRet,TArgs...>
{
	typedef const volatile TClass tClassArgument;
};

#if cnLibrary_CPPFEATURE_NOEXCEPT_FUNC_TYPE >= 201510L

#endif // cnLibrary_CPPFEATURE_NOEXCEPT_FUNC_TYPE >= 201510L


#endif // cnLibrary_CPPFEATURE_VARIADIC_TEMPLATES >= 200704L

#endif // 0

//---------------------------------------------------------------------------
}	// namespace cnVar
//---------------------------------------------------------------------------
}	// namespace cnLibrary
//---------------------------------------------------------------------------
#if	cnLibrary_CPPFEATURELEVEL >= 3
//---------------------------------------------------------------------------
namespace cnLib_THelper{
//---------------------------------------------------------------------------
namespace Var_TH{
//---------------------------------------------------------------------------

// nop operator

struct NopTypeOperator_Construct
{
	static void Construct(void*)noexcept(true){}
};
struct NopTypeOperator_Destruct
{
	static void Destruct(void*)noexcept(true){}
};
struct NopTypeOperator_CopyConstruct
{
	static void CopyConstruct(void*,const void*)noexcept(true){}
};
struct NopTypeOperator_MoveConstruct
{
	static void MoveConstruct(void*,void*)noexcept(true){}
};
struct NopTypeOperator_CopyAssign
{
	static void CopyAssign(void*,const void*)noexcept(true){}
};
struct NopTypeOperator_MoveAssign
{
	static void MoveAssign(void*,void*)noexcept(true){}
};

// Object New

template<class T,bool ClassNew>
struct TypeOperator_New
{
	static void* New(void)noexcept(cnLib_NOEXCEPTEXPR(T::operator new(sizeof(T))))
	{	return T::operator new(sizeof(T));	}
};
template<class T>
struct TypeOperator_New<T,false>
{
	static void* New(void)noexcept(cnLib_NOEXCEPTEXPR(operator new(sizeof(T))))
	{	return operator new(sizeof(T));	}
};

// Object Delete

template<class T,bool ClassSizedDelete,bool ClassDelete>
struct TypeOperator_Delete
{
	static void Delete(void *p)noexcept(true){	return T::operator delete(p,sizeof(T));	}
};

template<class T,bool ClassDelete>
struct TypeOperator_Delete<T,false,ClassDelete>
{
	static void Delete(void *p)noexcept(true){	return T::operator delete(p);	}
};

template<class T>
struct TypeOperator_Delete<T,false,false>
{
#if cnLibrary_CPPFEATURE_SIZED_DEALLOCATION >= 201309L
	static void Delete(void *p)noexcept(true){	return ::operator delete(p,sizeof(T));	}
#else
	static void Delete(void *p)noexcept(true){	return ::operator delete(p);	}
#endif // cnLibrary_CPPFEATURE_SIZED_DEALLOCATION >= 201309L
};

// Object Construct

template<class T>
struct TypeOperator_Construct_Class
{
	static void Construct(void *p)
		noexcept(cnLib_NOEXCEPTEXPR(T(cnVar::DeclVar<T&>())))
	{	return cnVar::ManualConstruct(*static_cast<T*>(p));	}
};

template<class T,bool Available>
struct TypeOperator_Construct
	: cnVar::TSelect<cnVar::TIsTrivial<T>::Value
		,TypeOperator_Construct_Class<T>
		,NopTypeOperator_Construct
	>::Type{};

template<class T>	struct TypeOperator_Construct<T,false>{};

// Object Destruct

template<class T>
struct TypeOperator_Destruct_Class
{
	static void Destruct(void *p)
		noexcept(cnLib_NOEXCEPTEXPR(cnVar::DeclVar<T&>().~T()))
	{	return cnVar::ManualDestruct(*static_cast<T*>(p));	}
};

template<class T,bool Available>
struct TypeOperator_Destruct
	: cnVar::TSelect<cnVar::TIsTrivial<T>::Value
		,TypeOperator_Destruct_Class<T>
		,NopTypeOperator_Destruct
	>::Type{};

template<class T>	struct TypeOperator_Destruct<T,false>{};

// Object CopyConstruct

template<class T>
struct TypeOperator_CopyConstruct_Class
{
	static void CopyConstruct(void *d,const void *s)
		noexcept(cnLib_NOEXCEPTEXPR(T(cnVar::DeclVar<const T&>())))
	{
		cnVar::ManualConstruct(*static_cast<T*>(d),*static_cast<const T*>(s));
	}
};
template<uIntn Size>
struct TypeOperator_CopyConstruct_Trivial
{
	static void CopyConstruct(void *d,const void *s)noexcept(true){	cnMemory::TrivialCopy<Size>(d,s);	}
};

template<class T,bool Available>
struct TypeOperator_CopyConstruct
	: cnVar::TSelect<cnVar::TIsTrivial<T>::Value
		,TypeOperator_CopyConstruct_Class<T>
		,TypeOperator_CopyConstruct_Trivial<sizeof(T)>
	>::Type{};

template<class T>	struct TypeOperator_CopyConstruct<T,false>{};

// Object MoveConstruct

template<class T>
struct TypeOperator_MoveConstruct_Class
{
	static void MoveConstruct(void *d,void *s)
		noexcept(cnLib_NOEXCEPTEXPR(T(cnVar::DeclVar<T&&>())))
	{	cnVar::ManualConstruct(*static_cast<T*>(d),static_cast<T&&>(*static_cast<T*>(s)));	}
};
template<uIntn Size>
struct TypeOperator_MoveConstruct_Trivial
{
	static void MoveConstruct(void *d,void *s)noexcept(true){	cnMemory::TrivialCopy<Size>(d,s);	}
};

template<class T,bool Available>
struct TypeOperator_MoveConstruct
	: cnVar::TSelect<cnVar::TIsTrivial<T>::Value
		,TypeOperator_MoveConstruct_Class<T>
		,TypeOperator_MoveConstruct_Trivial<sizeof(T)>
	>::Type{};

template<class T>	struct TypeOperator_MoveConstruct<T,false>{};


// Object CopyAssign

template<class T>
struct TypeOperator_CopyAssign_Class
{
	static void CopyAssign(void *d,const void *s)
		noexcept(cnLib_NOEXCEPTEXPR(T(cnVar::DeclVar<T&>()=cnVar::DeclVar<const T&>())))
	{
		*static_cast<T*>(d)=*static_cast<const T*>(s);
	}
};
template<uIntn Size>
struct TypeOperator_CopyAssign_Trivial
{
	static void CopyAssign(void *d,const void *s)noexcept(true){	cnMemory::TrivialCopy<Size>(d,s);	}
};

template<class T,bool Available>
struct TypeOperator_CopyAssign
	: cnVar::TSelect<cnVar::TIsTrivial<T>::Value
		,TypeOperator_CopyAssign_Class<T>
		,TypeOperator_CopyAssign_Trivial<sizeof(T)>
	>::Type{};

template<class T>	struct TypeOperator_CopyAssign<T,false>{};

// Object MoveAssign

template<class T>
struct TypeOperator_MoveAssign_Class
{
	static void MoveAssign(void *d,void *s)
		noexcept(cnLib_NOEXCEPTEXPR(cnVar::DeclVar<T&>()=cnVar::DeclVar<T&&>()))
	{
		*static_cast<T*>(d)=static_cast<T&&>(*static_cast<T*>(s));
	}
};
template<uIntn Size>
struct TypeOperator_MoveAssign_Trivial
{
	static void MoveAssign(void *d,void *s)noexcept(true){	cnMemory::TrivialCopy<Size>(d,s);	}
};

template<class T,bool Available>
struct TypeOperator_MoveAssign
	: cnVar::TSelect<cnVar::TIsTrivial<T>::Value
		,TypeOperator_MoveAssign_Class<T>
		,TypeOperator_MoveAssign_Trivial<sizeof(T)>
	>::Type{};

template<class T>	struct TypeOperator_MoveAssign<T,false>{};

// Safe Object Operator

template<class T,bool Available>
struct SafeTypeOperator_Construct
	: TypeOperator_Construct<T,Available>{};

template<class T>
struct SafeTypeOperator_Construct<T,false>
	: NopTypeOperator_Construct{};

template<class T,bool Available>
struct SafeTypeOperator_Destruct
	: TypeOperator_Destruct<T,Available>{};

template<class T>
struct SafeTypeOperator_Destruct<T,false>
	: NopTypeOperator_Destruct{};


template<class T,bool Available>
struct SafeTypeOperator_CopyConstruct
	: TypeOperator_CopyConstruct<T,Available>{};

template<class T>
struct SafeTypeOperator_CopyConstruct<T,false>
	: NopTypeOperator_CopyConstruct{};


template<class T,bool Available>
struct SafeTypeOperator_MoveConstruct
	: TypeOperator_MoveConstruct<T,Available>{};

template<class T>
struct SafeTypeOperator_MoveConstruct<T,false>
	: NopTypeOperator_MoveConstruct{};

template<class T,bool Available>
struct SafeTypeOperator_CopyAssign
	: TypeOperator_CopyAssign<T,Available>{};

template<class T>
struct SafeTypeOperator_CopyAssign<T,false>
	: NopTypeOperator_CopyAssign{};


template<class T,bool Available>
struct SafeTypeOperator_MoveAssign
	: TypeOperator_MoveAssign<T,Available>{};

template<class T>
struct SafeTypeOperator_MoveAssign<T,false>
	: NopTypeOperator_MoveAssign{};


// Array New

template<class T,uIntn Length,bool ClassNew>
struct ArrayTypeOperator_New
{
	static void* New(void)noexcept(cnLib_NOEXCEPTEXPR(T::operator new[](sizeof(T)*Length)))
	{	return T::operator new[](sizeof(T)*Length);	}
};

template<class T,uIntn Length>
struct ArrayTypeOperator_New<T,Length,false>
{
	static void* New(void)noexcept(cnLib_NOEXCEPTEXPR(operator new[](sizeof(T)*Length)))
	{	return operator new[](sizeof(T)*Length);	}
};

// Array Delete

template<class T,uIntn Length,bool ClassSizedDelete,bool ClassDelete>
struct ArrayTypeOperator_Delete
{
	static void Delete(void *p)noexcept(true){	return T::operator delete[](p,sizeof(T)*Length);	}
};

template<class T,uIntn Length,bool ClassDelete>
struct ArrayTypeOperator_Delete<T,Length,false,ClassDelete>
{
	static void Delete(void *p)noexcept(true){	return T::operator delete[](p);	}
};

template<class T,uIntn Length>
struct ArrayTypeOperator_Delete<T,Length,false,false>
{
#if cnLibrary_CPPFEATURE_SIZED_DEALLOCATION >= 201309L
	static void Delete(void *p)noexcept(true){	return ::operator delete[](p,sizeof(T)*Length);	}
#else
	static void Delete(void *p)noexcept(true){	return ::operator delete[](p);	}
#endif // cnLibrary_CPPFEATURE_SIZED_DEALLOCATION >= 201309L
};

// Array Construct

template<class T,uIntn Length>
struct ArrayTypeOperator_Construct_Class
{
	static void Construct(void *p)
		noexcept(cnLib_NOEXCEPTEXPR(T(cnVar::DeclVar<T&>())))
	{
		for(uIntn i=0;i<Length;i++){
			cnVar::ManualConstruct(static_cast<T*>(p)[i]);
		}
	}
};

template<class T,uIntn Length,bool Available>
struct ArrayTypeOperator_Construct
	: cnVar::TSelect<cnVar::TIsTrivial<T>::Value
		,ArrayTypeOperator_Construct_Class<T,Length>
		,NopTypeOperator_Construct
	>::Type{};

template<class T,uIntn Length>	struct ArrayTypeOperator_Construct<T,Length,false>{};

// Array Destruct

template<class T,uIntn Length>
struct ArrayTypeOperator_Destruct_Class
{
	static void Destruct(void *p)
		noexcept(cnLib_NOEXCEPTEXPR(cnVar::DeclVar<T&>().~T()))
	{
		for(uIntn i=0;i<Length;i++){
			cnVar::ManualDestruct(static_cast<T*>(p)[i]);
		}
	}
};

template<class T,uIntn Length,bool Available>
struct ArrayTypeOperator_Destruct
	: cnVar::TSelect<cnVar::TIsTrivial<T>::Value
		,ArrayTypeOperator_Destruct_Class<T,Length>
		,NopTypeOperator_Destruct
	>::Type{};

template<class T,uIntn Length>	struct ArrayTypeOperator_Destruct<T,Length,false>{};

// Array CopyConstruct

template<class T,uIntn Length>
struct ArrayTypeOperator_CopyConstruct_Class
{
	static void CopyConstruct(void *d,const void *s)
		noexcept(cnLib_NOEXCEPTEXPR(T(cnVar::DeclVar<const T&>())))
	{
		for(uIntn i=0;i<Length;i++){
			cnVar::ManualConstruct(static_cast<T*>(d)[i],static_cast<const T*>(s)[i]);
		}
	}
};

template<class T,uIntn Length,bool Available>
struct ArrayTypeOperator_CopyConstruct
	: cnVar::TSelect<cnVar::TIsTrivial<T>::Value
		,ArrayTypeOperator_CopyConstruct_Class<T,Length>
		,TypeOperator_CopyConstruct_Trivial<sizeof(T)*Length>
	>::Type{};

template<class T,uIntn Length>	struct ArrayTypeOperator_CopyConstruct<T,Length,false>{};

// Array MoveConstruct

template<class T,uIntn Length>
struct ArrayTypeOperator_MoveConstruct_Class
{
	static void MoveConstruct(void *d,void *s)
		noexcept(cnLib_NOEXCEPTEXPR(T(cnVar::DeclVar<T&&>())))
	{
		for(uIntn i=0;i<Length;i++){
			cnVar::ManualConstruct(static_cast<T*>(d)[i],static_cast<T&&>(static_cast<T*>(s)[i]));
		}
	}
};

template<class T,uIntn Length,bool Available>
struct ArrayTypeOperator_MoveConstruct
	: cnVar::TSelect<cnVar::TIsTrivial<T>::Value
		,ArrayTypeOperator_MoveConstruct_Class<T,Length>
		,TypeOperator_MoveConstruct_Trivial<sizeof(T)*Length>
	>::Type{};

template<class T,uIntn Length>	struct ArrayTypeOperator_MoveConstruct<T,Length,false>{};


// Array CopyAssign

template<class T,uIntn Length>
struct ArrayTypeOperator_CopyAssign_Class
{
	static void CopyAssign(void *d,const void *s)
		noexcept(cnLib_NOEXCEPTEXPR(T(cnVar::DeclVar<T&>()=cnVar::DeclVar<const T&>())))
	{
		for(uIntn i=0;i<Length;i++){
			static_cast<T*>(d)[i]=static_cast<const T*>(s)[i];
		}
	}
};

template<class T,uIntn Length,bool Available>
struct ArrayTypeOperator_CopyAssign
	: cnVar::TSelect<cnVar::TIsTrivial<T>::Value
		,ArrayTypeOperator_CopyAssign_Class<T,Length>
		,TypeOperator_CopyAssign_Trivial<sizeof(T)*Length>
	>::Type{};

template<class T,uIntn Length>	struct ArrayTypeOperator_CopyAssign<T,Length,false>{};

// Array MoveAssign

template<class T,uIntn Length>
struct ArrayTypeOperator_MoveAssign_Class
{
	static void MoveAssign(void *d,void *s)
		noexcept(cnLib_NOEXCEPTEXPR(cnVar::DeclVar<T&>()=cnVar::DeclVar<T&&>()))
	{
		for(uIntn i=0;i<Length;i++){
			static_cast<T*>(d)[i]=static_cast<T&&>(static_cast<T*>(s)[i]);
		}
	}
};

template<class T,uIntn Length,bool Available>
struct ArrayTypeOperator_MoveAssign
	: cnVar::TSelect<cnVar::TIsTrivial<T>::Value
		,ArrayTypeOperator_MoveAssign_Class<T,Length>
		,TypeOperator_MoveAssign_Trivial<sizeof(T)*Length>
	>::Type{};

template<class T,uIntn Length>	struct ArrayTypeOperator_MoveAssign<T,Length,false>{};

// Safe Array Operator

template<class T,uIntn Length,bool Available>
struct ArraySafeTypeOperator_Construct
	: ArrayTypeOperator_Construct<T,Length,Available>{};

template<class T,uIntn Length>
struct ArraySafeTypeOperator_Construct<T,Length,false>
	: NopTypeOperator_Construct{};

template<class T,uIntn Length,bool Available>
struct ArraySafeTypeOperator_Destruct
	: ArrayTypeOperator_Destruct<T,Length,Available>{};

template<class T,uIntn Length>
struct ArraySafeTypeOperator_Destruct<T,Length,false>
	: NopTypeOperator_Destruct{};


template<class T,uIntn Length,bool Available>
struct ArraySafeTypeOperator_CopyConstruct
	: ArrayTypeOperator_CopyConstruct<T,Length,Available>{};

template<class T,uIntn Length>
struct ArraySafeTypeOperator_CopyConstruct<T,Length,false>
	: NopTypeOperator_CopyConstruct{};


template<class T,uIntn Length,bool Available>
struct ArraySafeTypeOperator_MoveConstruct
	: ArrayTypeOperator_MoveConstruct<T,Length,Available>{};

template<class T,uIntn Length>
struct ArraySafeTypeOperator_MoveConstruct<T,Length,false>
	: NopTypeOperator_MoveConstruct{};

template<class T,uIntn Length,bool Available>
struct ArraySafeTypeOperator_CopyAssign
	: ArrayTypeOperator_CopyAssign<T,Length,Available>{};

template<class T,uIntn Length>
struct ArraySafeTypeOperator_CopyAssign<T,Length,false>
	: NopTypeOperator_CopyAssign{};


template<class T,uIntn Length,bool Available>
struct ArraySafeTypeOperator_MoveAssign
	: ArrayTypeOperator_MoveAssign<T,Length,Available>{};

template<class T,uIntn Length>
struct ArraySafeTypeOperator_MoveAssign<T,Length,false>
	: NopTypeOperator_MoveAssign{};

//---------------------------------------------------------------------------
}	// namespace Var_TH
//---------------------------------------------------------------------------
}	// namespace cnLib_THelper
//---------------------------------------------------------------------------
namespace cnLibrary{
//---------------------------------------------------------------------------
namespace cnVar{
//---------------------------------------------------------------------------
struct NopTypeOperator
	: cnLib_THelper::Var_TH::NopTypeOperator_Construct
	, cnLib_THelper::Var_TH::NopTypeOperator_Destruct
	, cnLib_THelper::Var_TH::NopTypeOperator_CopyConstruct
	, cnLib_THelper::Var_TH::NopTypeOperator_MoveConstruct
	, cnLib_THelper::Var_TH::NopTypeOperator_CopyAssign
	, cnLib_THelper::Var_TH::NopTypeOperator_MoveAssign
{
	static void* New(void)noexcept(true){	return nullptr;	}
	static void Delete(void*)noexcept(true){}
};

template<class T>
struct TypeOperator
 	: cnLib_THelper::Var_TH::TypeOperator_New<T,cnVar::THasOperatorNew<T>::Value>
	, cnLib_THelper::Var_TH::TypeOperator_Delete<T,cnVar::THasOperatorDeleteSize<T>::Value,cnVar::THasOperatorDelete<T>::Value>
	, cnLib_THelper::Var_TH::TypeOperator_Construct<T,TIsDefaultConstructable<T>::Value>
	, cnLib_THelper::Var_TH::TypeOperator_Destruct<T,TIsDestructable<T>::Value>
	, cnLib_THelper::Var_TH::TypeOperator_CopyConstruct<T,TIsCopyConstructable<T>::Value>
	, cnLib_THelper::Var_TH::TypeOperator_MoveConstruct<T,TIsMoveConstructable<T>::Value>
	, cnLib_THelper::Var_TH::TypeOperator_CopyAssign<T,TIsCopyAssignable<T>::Value>
	, cnLib_THelper::Var_TH::TypeOperator_MoveAssign<T,TIsMoveAssignable<T>::Value>
{
};

// type operator - void

template<>
struct TypeOperator<void>{};
template<>
struct TypeOperator<const void>{};
template<>
struct TypeOperator<volatile void>{};
template<>
struct TypeOperator<const volatile void>{};

// type operator - array

template<class T,uIntn Length>
struct TypeOperator<T [Length]>
 	: cnLib_THelper::Var_TH::ArrayTypeOperator_New<T,Length,cnVar::THasOperatorNew<T>::Value>
	, cnLib_THelper::Var_TH::ArrayTypeOperator_Delete<T,Length,cnVar::THasOperatorDeleteSize<T>::Value,cnVar::THasOperatorDelete<T>::Value>
	, cnLib_THelper::Var_TH::ArrayTypeOperator_Construct<T,Length,TIsDefaultConstructable<T>::Value>
	, cnLib_THelper::Var_TH::ArrayTypeOperator_Destruct<T,Length,TIsDestructable<T>::Value>
	, cnLib_THelper::Var_TH::ArrayTypeOperator_CopyConstruct<T,Length,TIsCopyConstructable<T>::Value>
	, cnLib_THelper::Var_TH::ArrayTypeOperator_MoveConstruct<T,Length,TIsMoveConstructable<T>::Value>
	, cnLib_THelper::Var_TH::ArrayTypeOperator_CopyAssign<T,Length,TIsCopyAssignable<T>::Value>
	, cnLib_THelper::Var_TH::ArrayTypeOperator_MoveAssign<T,Length,TIsMoveAssignable<T>::Value>
{};

template<class T>
struct TypeOperator<T []>{};

// type operator - function

template<class TRet,class...TArgs>
struct TypeOperator<TRet (TArgs...)>{};

#if cnLibrary_CPPFEATURE_NOEXCEPT_FUNC_TYPE >= 201510L

template<class TRet,class...TArgs>
struct TypeOperator<TRet (TArgs...)noexcept>{};

#endif // cnLibrary_CPPFEATURE_NOEXCEPT_FUNC_TYPE >= 201510L

template<class T>
struct SafeTypeOperator
 	: cnLib_THelper::Var_TH::TypeOperator_New<T,cnVar::THasOperatorNew<T>::Value>
	, cnLib_THelper::Var_TH::TypeOperator_Delete<T,cnVar::THasOperatorDeleteSize<T>::Value,cnVar::THasOperatorDelete<T>::Value>
	, cnLib_THelper::Var_TH::SafeTypeOperator_Construct<T,TIsDefaultConstructable<T>::Value>
	, cnLib_THelper::Var_TH::SafeTypeOperator_Destruct<T,TIsDestructable<T>::Value>
	, cnLib_THelper::Var_TH::SafeTypeOperator_CopyConstruct<T,TIsCopyConstructable<T>::Value>
	, cnLib_THelper::Var_TH::SafeTypeOperator_MoveConstruct<T,TIsMoveConstructable<T>::Value>
	, cnLib_THelper::Var_TH::SafeTypeOperator_CopyAssign<T,TIsCopyAssignable<T>::Value>
	, cnLib_THelper::Var_TH::SafeTypeOperator_MoveAssign<T,TIsMoveAssignable<T>::Value>
{
};

// type operator - void

template<>
struct SafeTypeOperator<void>
	: NopTypeOperator{};
template<>
struct SafeTypeOperator<const void>
	: NopTypeOperator{};
template<>
struct SafeTypeOperator<volatile void>
	: NopTypeOperator{};
template<>
struct SafeTypeOperator<const volatile void>
	: NopTypeOperator{};

// type operator - array

template<class T,uIntn Length>
struct SafeTypeOperator<T [Length]>
 	: cnLib_THelper::Var_TH::ArrayTypeOperator_New<T,Length,cnVar::THasOperatorNew<T>::Value>
	, cnLib_THelper::Var_TH::ArrayTypeOperator_Delete<T,Length,cnVar::THasOperatorDeleteSize<T>::Value,cnVar::THasOperatorDelete<T>::Value>
	, cnLib_THelper::Var_TH::ArraySafeTypeOperator_Construct<T,Length,TIsDefaultConstructable<T>::Value>
	, cnLib_THelper::Var_TH::ArraySafeTypeOperator_Destruct<T,Length,TIsDestructable<T>::Value>
	, cnLib_THelper::Var_TH::ArraySafeTypeOperator_CopyConstruct<T,Length,TIsCopyConstructable<T>::Value>
	, cnLib_THelper::Var_TH::ArraySafeTypeOperator_MoveConstruct<T,Length,TIsMoveConstructable<T>::Value>
	, cnLib_THelper::Var_TH::ArraySafeTypeOperator_CopyAssign<T,Length,TIsCopyAssignable<T>::Value>
	, cnLib_THelper::Var_TH::ArraySafeTypeOperator_MoveAssign<T,Length,TIsMoveAssignable<T>::Value>
{};

template<class T>
struct SafeTypeOperator<T []>
	: NopTypeOperator{};

// type operator - function

template<class TRet,class...TArgs>
struct SafeTypeOperator<TRet (TArgs...)>
	: NopTypeOperator{};

#if cnLibrary_CPPFEATURE_NOEXCEPT_FUNC_TYPE >= 201510L

template<class TRet,class...TArgs>
struct SafeTypeOperator<TRet (TArgs...)noexcept>
	: NopTypeOperator{};

#endif // cnLibrary_CPPFEATURE_NOEXCEPT_FUNC_TYPE >= 201510L
//---------------------------------------------------------------------------
}	// namespace cnVar
//---------------------------------------------------------------------------
cnLib_ENUM_BEGIN(ufInt8, rtTypeClass)
{
	Data,
	Array,
	Pointer,
	Reference,
	Function,
	MemberPointer,
	MemberFunctionPointer
}cnLib_ENUM_END(rtTypeClass);
//---------------------------------------------------------------------------
namespace cnVar{
//---------------------------------------------------------------------------
struct cRuntimeTypeInfo;
//---------------------------------------------------------------------------
struct cRuntimeDataInfo
{
	const cRuntimeTypeInfo *TypeInfo;
	uIntn Length;
	struct cItem{
		tTypeID Type;
		const void *Info;
	};
	const cItem *Items;

	operator const cRuntimeTypeInfo* ()const{	return TypeInfo;	}
	
	template<class TType>
	operator TType ()const{	
		for(uIntn i=0;i<Length;i++){
			if(Items[i].Type==cnVar::TTypeID<TType>::Value){
				return static_cast<TType>(Items[i].Info);
			}
		}
		return nullptr;
	}
};
//---------------------------------------------------------------------------
struct cRuntimeTypeInfo_Data
{
	const cRuntimeDataInfo *DataInfo;
};
//---------------------------------------------------------------------------
struct cRuntimeTypeInfo_Array
{
	const cRuntimeTypeInfo *ElementType;
	uIntn Length;
};
//---------------------------------------------------------------------------
struct cRuntimeTypeInfo_Pointer
{
	const cRuntimeTypeInfo *TargetType;
};
//---------------------------------------------------------------------------
struct cRuntimeTypeInfo_Reference
{
	const cRuntimeTypeInfo *TargetType;
	bool IsRValue;
};
//---------------------------------------------------------------------------
struct cRuntimeTypeInfo_Function
{
	const cRuntimeTypeInfo *ReturnType;
	uIntn ParameterCount;
	const cRuntimeTypeInfo *const*ParameterType;
	bool NoException;
};
//---------------------------------------------------------------------------
struct cRuntimeTypeInfo_MemberPointer
{
	const cRuntimeTypeInfo *ClassType;
	const cRuntimeTypeInfo *MemberType;
};
//---------------------------------------------------------------------------
struct cRuntimeTypeInfo_MemberFunctionPointer
{
	const cRuntimeTypeInfo *ReturnType;
	uIntn ParameterCount;
	const cRuntimeTypeInfo *const*ParameterType;
	const cRuntimeTypeInfo *ClassType;
	bool Const		:1;
	bool Volatile	:1;
	bool Ref		:1;
	bool RValRef	:1;
	bool NoException;
};
//---------------------------------------------------------------------------
struct cRuntimeTypeOperator
{
	void (*Construct)(void *p);
	void (*Destruct)(void *p);
	void (*CopyConstruct)(void *d,const void *s);
	void (*MoveConstruct)(void *d,void *s);
	void (*Copy)(void *d,const void *s);
	void (*Move)(void *d,void *s);
	void* (*New)(void);
	void (*Delete)(void *p);
};
//---------------------------------------------------------------------------
struct cRuntimeTypeInfo
{
	const cTypeIdentity *ID;
	uInt32 Size;
	ertTypeClass TypeClass;
	bool IsConst					:1;
	bool IsVolatile					:1;
	bool IsDefaultConstructable		:1;
	bool IsDestructable				:1;
	bool IsCopyConstructable		:1;
	bool IsMoveConstructable		:1;
	bool IsCopyAssignable			:1;
	bool IsMoveAssignable			:1;
	bool IsDefaultConstructNoexcept	:1;
	bool IsDestructNoexcept			:1;
	bool IsCopyConstructNoexcept	:1;
	bool IsMoveConstructNoexcept	:1;
	bool IsCopyAssignNoexcept		:1;
	bool IsMoveAssignNoexcept		:1;
	const cRuntimeTypeOperator *Operators;

	union{
		const void *Abstract;
		const cRuntimeTypeInfo_Data *Data;
		const cRuntimeTypeInfo_Array *Array;
		const cRuntimeTypeInfo_Pointer *Pointer;
		const cRuntimeTypeInfo_Reference *Reference;
		const cRuntimeTypeInfo_MemberPointer *MemberPointer;
		const cRuntimeTypeInfo_Function *Function;
		const cRuntimeTypeInfo_MemberFunctionPointer *MemberFunctionPointer;
	};

};
//---------------------------------------------------------------------------
template<class T>
struct cRuntimeTypeInfoDefinition
{
	static const cRuntimeTypeInfo Value;
};
//---------------------------------------------------------------------------
template<class T>
struct TRuntimeDataInfo
{
	static const cRuntimeDataInfo Value;
};
//---------------------------------------------------------------------------
}	// namespace cnVar
//---------------------------------------------------------------------------
typedef const cnVar::cRuntimeTypeInfo *rtTypeInfo;
typedef const cnVar::cRuntimeDataInfo *rtDataInfo;
//---------------------------------------------------------------------------
}	// namespace cnLibrary
//---------------------------------------------------------------------------
namespace cnLib_THelper{
//---------------------------------------------------------------------------
namespace Var_TH{
//---------------------------------------------------------------------------
namespace RuntimeInfo{
//---------------------------------------------------------------------------

// operators

template<class T>
struct RuntimeTypeOperatorDef
{
	static const cnVar::cRuntimeTypeOperator Value;
};

template<class T>
const cnVar::cRuntimeTypeOperator RuntimeTypeOperatorDef<T>::Value={
	cnVar::SafeTypeOperator<T>::Construct,
	cnVar::SafeTypeOperator<T>::Destruct,
	cnVar::SafeTypeOperator<T>::CopyConstruct,
	cnVar::SafeTypeOperator<T>::MoveConstruct,
	cnVar::SafeTypeOperator<T>::CopyAssign,
	cnVar::SafeTypeOperator<T>::MoveAssign,
	cnVar::SafeTypeOperator<T>::New,
	cnVar::SafeTypeOperator<T>::Delete,
};

// runtime info

template<class T>
struct RuntimeTypeInfoDef;

// runtime info - distinct - data

template<class T>
struct RuntimeTypeInfoDistinctDef
{
	static cnLib_CONSTVAR ertTypeClass TypeClass=rtTypeClass::Data;
	static const cnVar::cRuntimeTypeInfo_Data TypeClassInfo;
};


template<class T>
const cnVar::cRuntimeTypeInfo_Data RuntimeTypeInfoDistinctDef<T>::TypeClassInfo={
	&cnVar::TRuntimeDataInfo<T>::Value
};

// runtime info - distinct - array

template<class T,uIntn ArrayLength>
struct RuntimeTypeInfoDistinctDef<T [ArrayLength]>
{
	static cnLib_CONSTVAR ertTypeClass TypeClass=rtTypeClass::Array;
	static const cnVar::cRuntimeTypeInfo_Array TypeClassInfo;
};

template<class T,uIntn ArrayLength>
const cnVar::cRuntimeTypeInfo_Array RuntimeTypeInfoDistinctDef<T [ArrayLength]>::TypeClassInfo={
	&RuntimeTypeInfoDef<T>::TypeInfo,
	ArrayLength,
};

template<class T>
struct RuntimeTypeInfoDistinctDef<T []>
{
	static cnLib_CONSTVAR ertTypeClass TypeClass=rtTypeClass::Array;
	static const cnVar::cRuntimeTypeInfo_Array TypeClassInfo;
};

template<class T>
const cnVar::cRuntimeTypeInfo_Array RuntimeTypeInfoDistinctDef<T []>::TypeClassInfo={
	&RuntimeTypeInfoDef<T>::TypeInfo,
	0,
};

// runtime info - distinct - pointer

template<class T>
struct RuntimeTypeInfoDistinctDef<T*>
{
	static cnLib_CONSTVAR ertTypeClass TypeClass=rtTypeClass::Pointer;
	static const cnVar::cRuntimeTypeInfo_Pointer TypeClassInfo;
};

template<class T>
const cnVar::cRuntimeTypeInfo_Pointer RuntimeTypeInfoDistinctDef<T*>::TypeClassInfo={
	&RuntimeTypeInfoDef<T>::TypeInfo,
};

// runtime info - distinct - lvalue reference

template<class T>
struct RuntimeTypeInfoDistinctDef<T&>
{
	static cnLib_CONSTVAR ertTypeClass TypeClass=rtTypeClass::Reference;
	static const cnVar::cRuntimeTypeInfo_Reference TypeClassInfo;
};

template<class T>
const cnVar::cRuntimeTypeInfo_Reference RuntimeTypeInfoDistinctDef<T&>::TypeClassInfo={
	&RuntimeTypeInfoDef<T>::TypeInfo,
	false,
};

// runtime info - distinct - rvalue reference


template<class T>
struct RuntimeTypeInfoDistinctDef<T&&>
{
	static cnLib_CONSTVAR ertTypeClass TypeClass=rtTypeClass::Reference;
	static const cnVar::cRuntimeTypeInfo_Reference TypeClassInfo;
};


template<class T>
const cnVar::cRuntimeTypeInfo_Reference RuntimeTypeInfoDistinctDef<T&&>::TypeClassInfo={
	&RuntimeTypeInfoDef<T>::TypeInfo,
	true,
};


// runtime info - distinct - function - Parameter list


template<class...TArgs>
struct cRuntimeTypeInfoDistinctFunctionParameterList
{
	static const rtTypeInfo Value[cnMath::TMax<uIntn,1,sizeof...(TArgs)>::Value];
};

template<class...TArgs>
const rtTypeInfo cRuntimeTypeInfoDistinctFunctionParameterList<TArgs...>::Value[cnMath::TMax<uIntn,1,sizeof...(TArgs)>::Value]={
	&RuntimeTypeInfoDef<TArgs>::TypeInfo...
};

// runtime info - distinct - function

template<class TRet,class...TArgs>
struct RuntimeTypeInfoDistinctDef<TRet (TArgs...)>
{
	static cnLib_CONSTVAR ertTypeClass TypeClass=rtTypeClass::Function;
	static const cnVar::cRuntimeTypeInfo_Function TypeClassInfo;
};

template<class TRet,class...TArgs>
const cnVar::cRuntimeTypeInfo_Function RuntimeTypeInfoDistinctDef<TRet (TArgs...)>::TypeClassInfo={
	&RuntimeTypeInfoDef<TRet>::TypeInfo,	// Return Type
	sizeof...(TArgs),								// ParameterCount
	cRuntimeTypeInfoDistinctFunctionParameterList<TArgs...>::Value,	// Parameter Type
	false,		// NoException
};

#if cnLibrary_CPPFEATURE_NOEXCEPT_FUNC_TYPE >= 201510L

// runtime info - distinct - function - noexcept

template<class TRet,class...TArgs>
struct RuntimeTypeInfoDistinctDef<TRet (TArgs...)noexcept>
{
	static cnLib_CONSTVAR ertTypeClass TypeClass=rtTypeClass::Function;
	static const cnVar::cRuntimeTypeInfo_Function TypeClassInfo;
};

template<class TRet,class...TArgs>
const cnVar::cRuntimeTypeInfo_Function RuntimeTypeInfoDistinctDef<TRet (TArgs...)noexcept>::TypeClassInfo={
	&RuntimeTypeInfoDef<TRet>::TypeInfo,	// Return Type
	sizeof...(TArgs),								// ParameterCount
	cRuntimeTypeInfoDistinctFunctionParameterList<TArgs...>::Value,	// Parameter Type
	true,		// NoException
};

#endif // cnLibrary_CPPFEATURE_NOEXCEPT_FUNC_TYPE >= 201510L


// runtime info - distinct - member pointer

template<class TClass,class TMember>
struct RuntimeTypeInfoDistinctDef<TMember TClass::*>
{
	static cnLib_CONSTVAR ertTypeClass TypeClass=rtTypeClass::MemberPointer;
	static const cnVar::cRuntimeTypeInfo_MemberPointer TypeClassInfo;
};

template<class TClass,class TMember>
const cnVar::cRuntimeTypeInfo_MemberPointer RuntimeTypeInfoDistinctDef<TMember TClass::*>::TypeClassInfo={
	&RuntimeTypeInfoDef<TClass>::TypeInfo,
	&RuntimeTypeInfoDef<TMember>::TypeInfo,
};

// runtime info - distinct - member function pointer

template<class TClass,class TRet,class...TArgs>
struct RuntimeTypeInfoDistinctDef<TRet (TClass::*)(TArgs...)>
{
	static cnLib_CONSTVAR ertTypeClass TypeClass=rtTypeClass::MemberFunctionPointer;
	static const cnVar::cRuntimeTypeInfo_MemberFunctionPointer TypeClassInfo;
};

template<class TClass,class TRet,class...TArgs>
const cnVar::cRuntimeTypeInfo_MemberFunctionPointer RuntimeTypeInfoDistinctDef<TRet (TClass::*)(TArgs...)>::TypeClassInfo={
	&RuntimeTypeInfoDef<TRet>::TypeInfo,	// Return Type
	sizeof...(TArgs),								// ParameterCount
	cRuntimeTypeInfoDistinctFunctionParameterList<TArgs...>::Value,	// Parameter Type
	&RuntimeTypeInfoDef<TClass>::TypeInfo,
	false,	// Const
	false,	// Volatile
	false,	// Reference
	false,	// RVReference
	false,	// Noexcept
};

// runtime info - distinct - member function pointer - const

template<class TClass,class TRet,class...TArgs>
struct RuntimeTypeInfoDistinctDef<TRet (TClass::*)(TArgs...)const>
{
	static cnLib_CONSTVAR ertTypeClass TypeClass=rtTypeClass::MemberFunctionPointer;
	static const cnVar::cRuntimeTypeInfo_MemberFunctionPointer TypeClassInfo;
};


template<class TClass,class TRet,class...TArgs>
const cnVar::cRuntimeTypeInfo_MemberFunctionPointer RuntimeTypeInfoDistinctDef<TRet (TClass::*)(TArgs...)const>::TypeClassInfo={
	&RuntimeTypeInfoDef<TRet>::TypeInfo,	// Return Type
	sizeof...(TArgs),								// ParameterCount
	cRuntimeTypeInfoDistinctFunctionParameterList<TArgs...>::Value,	// Parameter Type
	&RuntimeTypeInfoDef<TClass>::TypeInfo,
	true,	// Const
	false,	// Volatile
	false,	// Reference
	false,	// RVReference
	false,	// Noexcept
};

// runtime info - distinct - member function pointer - volatile

template<class TClass,class TRet,class...TArgs>
struct RuntimeTypeInfoDistinctDef<TRet (TClass::*)(TArgs...)volatile>
{
	static cnLib_CONSTVAR ertTypeClass TypeClass=rtTypeClass::MemberFunctionPointer;
	static const rtTypeInfo MemberFunctionParameters[];
	static const cnVar::cRuntimeTypeInfo_MemberFunctionPointer TypeClassInfo;
};


template<class TClass,class TRet,class...TArgs>
const cnVar::cRuntimeTypeInfo_MemberFunctionPointer RuntimeTypeInfoDistinctDef<TRet (TClass::*)(TArgs...)volatile>::TypeClassInfo={
	&RuntimeTypeInfoDef<TRet>::TypeInfo,	// Return Type
	sizeof...(TArgs),								// ParameterCount
	cRuntimeTypeInfoDistinctFunctionParameterList<TArgs...>::Value,	// Parameter Type
	&RuntimeTypeInfoDef<TClass>::TypeInfo,
	false,	// Const
	true,	// Volatile
	false,	// Reference
	false,	// RVReference
	false,	// Noexcept
};
// runtime info - distinct - member function pointer - const volatile

template<class TClass,class TRet,class...TArgs>
struct RuntimeTypeInfoDistinctDef<TRet (TClass::*)(TArgs...)const volatile>
{
	static cnLib_CONSTVAR ertTypeClass TypeClass=rtTypeClass::MemberFunctionPointer;
	static const cnVar::cRuntimeTypeInfo_MemberFunctionPointer TypeClassInfo;
};


template<class TClass,class TRet,class...TArgs>
const cnVar::cRuntimeTypeInfo_MemberFunctionPointer RuntimeTypeInfoDistinctDef<TRet (TClass::*)(TArgs...)const volatile>::TypeClassInfo={
	&RuntimeTypeInfoDef<TRet>::TypeInfo,	// Return Type
	sizeof...(TArgs),								// ParameterCount
	cRuntimeTypeInfoDistinctFunctionParameterList<TArgs...>::Value,	// Parameter Type
	&RuntimeTypeInfoDef<TClass>::TypeInfo,
	true,	// Const
	true,	// Volatile
	false,	// Reference
	false,	// RVReference
	false,	// Noexcept
};

#if cnLibrary_CPPFEATURE_NOEXCEPT_FUNC_TYPE >= 201510L

// runtime info - distinct - member function pointer - noexcept

template<class TClass,class TRet,class...TArgs>
struct RuntimeTypeInfoDistinctDef<TRet (TClass::*)(TArgs...)noexcept>
{
	static cnLib_CONSTVAR ertTypeClass TypeClass=rtTypeClass::MemberFunctionPointer;
	static const cnVar::cRuntimeTypeInfo_MemberFunctionPointer TypeClassInfo;
};

template<class TClass,class TRet,class...TArgs>
const cnVar::cRuntimeTypeInfo_MemberFunctionPointer RuntimeTypeInfoDistinctDef<TRet (TClass::*)(TArgs...)noexcept>::TypeClassInfo={
	&RuntimeTypeInfoDef<TRet>::TypeInfo,	// Return Type
	sizeof...(TArgs),								// ParameterCount
	cRuntimeTypeInfoDistinctFunctionParameterList<TArgs...>::Value,	// Parameter Type
	&RuntimeTypeInfoDef<TClass>::TypeInfo,
	false,	// Const
	false,	// Volatile
	false,	// Reference
	false,	// RVReference
	true,	// Noexcept
};

// runtime info - distinct - member function pointer - const - noexcept

template<class TClass,class TRet,class...TArgs>
struct RuntimeTypeInfoDistinctDef<TRet (TClass::*)(TArgs...)const noexcept>
{
	static cnLib_CONSTVAR ertTypeClass TypeClass=rtTypeClass::MemberFunctionPointer;
	static const cnVar::cRuntimeTypeInfo_MemberFunctionPointer TypeClassInfo;
};


template<class TClass,class TRet,class...TArgs>
const cnVar::cRuntimeTypeInfo_MemberFunctionPointer RuntimeTypeInfoDistinctDef<TRet (TClass::*)(TArgs...)const noexcept>::TypeClassInfo={
	&RuntimeTypeInfoDef<TRet>::TypeInfo,	// Return Type
	sizeof...(TArgs),								// ParameterCount
	cRuntimeTypeInfoDistinctFunctionParameterList<TArgs...>::Value,	// Parameter Type
	&RuntimeTypeInfoDef<TClass>::TypeInfo,
	true,	// Const
	false,	// Volatile
	false,	// Reference
	false,	// RVReference
	true,	// Noexcept
};

// runtime info - distinct - member function pointer - volatile - noexcept

template<class TClass,class TRet,class...TArgs>
struct RuntimeTypeInfoDistinctDef<TRet (TClass::*)(TArgs...)volatile noexcept>
{
	static cnLib_CONSTVAR ertTypeClass TypeClass=rtTypeClass::MemberFunctionPointer;
	static const rtTypeInfo MemberFunctionParameters[];
	static const cnVar::cRuntimeTypeInfo_MemberFunctionPointer TypeClassInfo;
};


template<class TClass,class TRet,class...TArgs>
const cnVar::cRuntimeTypeInfo_MemberFunctionPointer RuntimeTypeInfoDistinctDef<TRet (TClass::*)(TArgs...)volatile noexcept>::TypeClassInfo={
	&RuntimeTypeInfoDef<TRet>::TypeInfo,	// Return Type
	sizeof...(TArgs),								// ParameterCount
	cRuntimeTypeInfoDistinctFunctionParameterList<TArgs...>::Value,	// Parameter Type
	&RuntimeTypeInfoDef<TClass>::TypeInfo,
	false,	// Const
	true,	// Volatile
	false,	// Reference
	false,	// RVReference
	true,	// Noexcept
};
// runtime info - distinct - member function pointer - const volatile - noexcept

template<class TClass,class TRet,class...TArgs>
struct RuntimeTypeInfoDistinctDef<TRet (TClass::*)(TArgs...)const volatile noexcept>
{
	static cnLib_CONSTVAR ertTypeClass TypeClass=rtTypeClass::MemberFunctionPointer;
	static const cnVar::cRuntimeTypeInfo_MemberFunctionPointer TypeClassInfo;
};


template<class TClass,class TRet,class...TArgs>
const cnVar::cRuntimeTypeInfo_MemberFunctionPointer RuntimeTypeInfoDistinctDef<TRet (TClass::*)(TArgs...)const volatile noexcept>::TypeClassInfo={
	&RuntimeTypeInfoDef<TRet>::TypeInfo,	// Return Type
	sizeof...(TArgs),								// ParameterCount
	cRuntimeTypeInfoDistinctFunctionParameterList<TArgs...>::Value,	// Parameter Type
	&RuntimeTypeInfoDef<TClass>::TypeInfo,
	true,	// Const
	true,	// Volatile
	false,	// Reference
	false,	// RVReference
	true,	// Noexcept
};

#endif	// cnLibrary_CPPFEATURE_NOEXCEPT_FUNC_TYPE >= 201510L

#if cnLibrary_CPPFEATURE_REF_QUALIFIERS >= 200710L

// runtime info - distinct - member function pointer - &

template<class TClass,class TRet,class...TArgs>
struct RuntimeTypeInfoDistinctDef<TRet (TClass::*)(TArgs...)&>
{
	static cnLib_CONSTVAR ertTypeClass TypeClass=rtTypeClass::MemberFunctionPointer;
	static const cnVar::cRuntimeTypeInfo_MemberFunctionPointer TypeClassInfo;
};

template<class TClass,class TRet,class...TArgs>
const cnVar::cRuntimeTypeInfo_MemberFunctionPointer RuntimeTypeInfoDistinctDef<TRet (TClass::*)(TArgs...)&>::TypeClassInfo={
	&RuntimeTypeInfoDef<TRet>::TypeInfo,	// Return Type
	sizeof...(TArgs),								// ParameterCount
	cRuntimeTypeInfoDistinctFunctionParameterList<TArgs...>::Value,	// Parameter Type
	&RuntimeTypeInfoDef<TClass>::TypeInfo,
	false,	// Const
	false,	// Volatile
	true,	// Reference
	false,	// RVReference
	false,	// Noexcept
};

// runtime info - distinct - member function pointer - const&

template<class TClass,class TRet,class...TArgs>
struct RuntimeTypeInfoDistinctDef<TRet (TClass::*)(TArgs...)const&>
{
	static cnLib_CONSTVAR ertTypeClass TypeClass=rtTypeClass::MemberFunctionPointer;
	static const cnVar::cRuntimeTypeInfo_MemberFunctionPointer TypeClassInfo;
};


template<class TClass,class TRet,class...TArgs>
const cnVar::cRuntimeTypeInfo_MemberFunctionPointer RuntimeTypeInfoDistinctDef<TRet (TClass::*)(TArgs...)const&>::TypeClassInfo={
	&RuntimeTypeInfoDef<TRet>::TypeInfo,	// Return Type
	sizeof...(TArgs),								// ParameterCount
	cRuntimeTypeInfoDistinctFunctionParameterList<TArgs...>::Value,	// Parameter Type
	&RuntimeTypeInfoDef<TClass>::TypeInfo,
	true,	// Const
	false,	// Volatile
	true,	// Reference
	false,	// RVReference
	false,	// Noexcept
};

// runtime info - distinct - member function pointer - volatile&

template<class TClass,class TRet,class...TArgs>
struct RuntimeTypeInfoDistinctDef<TRet (TClass::*)(TArgs...)volatile&>
{
	static cnLib_CONSTVAR ertTypeClass TypeClass=rtTypeClass::MemberFunctionPointer;
	static const cnVar::cRuntimeTypeInfo_MemberFunctionPointer TypeClassInfo;
};


template<class TClass,class TRet,class...TArgs>
const cnVar::cRuntimeTypeInfo_MemberFunctionPointer RuntimeTypeInfoDistinctDef<TRet (TClass::*)(TArgs...)volatile&>::TypeClassInfo={
	&RuntimeTypeInfoDef<TRet>::TypeInfo,	// Return Type
	sizeof...(TArgs),								// ParameterCount
	cRuntimeTypeInfoDistinctFunctionParameterList<TArgs...>::Value,	// Parameter Type
	&RuntimeTypeInfoDef<TClass>::TypeInfo,
	false,	// Const
	true,	// Volatile
	true,	// Reference
	false,	// RVReference
	false,	// Noexcept
};
// runtime info - distinct - member function pointer - const volatile&

template<class TClass,class TRet,class...TArgs>
struct RuntimeTypeInfoDistinctDef<TRet (TClass::*)(TArgs...)const volatile&>
{
	static cnLib_CONSTVAR ertTypeClass TypeClass=rtTypeClass::MemberFunctionPointer;
	static const cnVar::cRuntimeTypeInfo_MemberFunctionPointer TypeClassInfo;
};


template<class TClass,class TRet,class...TArgs>
const cnVar::cRuntimeTypeInfo_MemberFunctionPointer RuntimeTypeInfoDistinctDef<TRet (TClass::*)(TArgs...)const volatile&>::TypeClassInfo={
	&RuntimeTypeInfoDef<TRet>::TypeInfo,	// Return Type
	sizeof...(TArgs),								// ParameterCount
	cRuntimeTypeInfoDistinctFunctionParameterList<TArgs...>::Value,	// Parameter Type
	&RuntimeTypeInfoDef<TClass>::TypeInfo,
	true,	// Const
	true,	// Volatile
	true,	// Reference
	false,	// RVReference
	false,	// Noexcept
};


#if cnLibrary_CPPFEATURE_NOEXCEPT_FUNC_TYPE >= 201510L

// runtime info - distinct - member function pointer - & - noexcept

template<class TClass,class TRet,class...TArgs>
struct RuntimeTypeInfoDistinctDef<TRet (TClass::*)(TArgs...)&noexcept>
{
	static cnLib_CONSTVAR ertTypeClass TypeClass=rtTypeClass::MemberFunctionPointer;
	static const cnVar::cRuntimeTypeInfo_MemberFunctionPointer TypeClassInfo;
};

template<class TClass,class TRet,class...TArgs>
const cnVar::cRuntimeTypeInfo_MemberFunctionPointer RuntimeTypeInfoDistinctDef<TRet (TClass::*)(TArgs...)&noexcept>::TypeClassInfo={
	&RuntimeTypeInfoDef<TRet>::TypeInfo,	// Return Type
	sizeof...(TArgs),								// ParameterCount
	cRuntimeTypeInfoDistinctFunctionParameterList<TArgs...>::Value,	// Parameter Type
	&RuntimeTypeInfoDef<TClass>::TypeInfo,
	false,	// Const
	false,	// Volatile
	true,	// Reference
	false,	// RVReference
	true,	// Noexcept
};

// runtime info - distinct - member function pointer - const& - noexcept

template<class TClass,class TRet,class...TArgs>
struct RuntimeTypeInfoDistinctDef<TRet (TClass::*)(TArgs...)const& noexcept>
{
	static cnLib_CONSTVAR ertTypeClass TypeClass=rtTypeClass::MemberFunctionPointer;
	static const cnVar::cRuntimeTypeInfo_MemberFunctionPointer TypeClassInfo;
};


template<class TClass,class TRet,class...TArgs>
const cnVar::cRuntimeTypeInfo_MemberFunctionPointer RuntimeTypeInfoDistinctDef<TRet (TClass::*)(TArgs...)const& noexcept>::TypeClassInfo={
	&RuntimeTypeInfoDef<TRet>::TypeInfo,	// Return Type
	sizeof...(TArgs),								// ParameterCount
	cRuntimeTypeInfoDistinctFunctionParameterList<TArgs...>::Value,	// Parameter Type
	&RuntimeTypeInfoDef<TClass>::TypeInfo,
	true,	// Const
	false,	// Volatile
	true,	// Reference
	false,	// RVReference
	true,	// Noexcept
};

// runtime info - distinct - member function pointer - volatile& - noexcept

template<class TClass,class TRet,class...TArgs>
struct RuntimeTypeInfoDistinctDef<TRet (TClass::*)(TArgs...)volatile& noexcept>
{
	static cnLib_CONSTVAR ertTypeClass TypeClass=rtTypeClass::MemberFunctionPointer;
	static const rtTypeInfo MemberFunctionParameters[];
	static const cnVar::cRuntimeTypeInfo_MemberFunctionPointer TypeClassInfo;
};


template<class TClass,class TRet,class...TArgs>
const cnVar::cRuntimeTypeInfo_MemberFunctionPointer RuntimeTypeInfoDistinctDef<TRet (TClass::*)(TArgs...)volatile& noexcept>::TypeClassInfo={
	&RuntimeTypeInfoDef<TRet>::TypeInfo,	// Return Type
	sizeof...(TArgs),								// ParameterCount
	cRuntimeTypeInfoDistinctFunctionParameterList<TArgs...>::Value,	// Parameter Type
	&RuntimeTypeInfoDef<TClass>::TypeInfo,
	false,	// Const
	true,	// Volatile
	true,	// Reference
	false,	// RVReference
	true,	// Noexcept
};
// runtime info - distinct - member function pointer - const volatile& - noexcept

template<class TClass,class TRet,class...TArgs>
struct RuntimeTypeInfoDistinctDef<TRet (TClass::*)(TArgs...)const volatile& noexcept>
{
	static cnLib_CONSTVAR ertTypeClass TypeClass=rtTypeClass::MemberFunctionPointer;
	static const cnVar::cRuntimeTypeInfo_MemberFunctionPointer TypeClassInfo;
};


template<class TClass,class TRet,class...TArgs>
const cnVar::cRuntimeTypeInfo_MemberFunctionPointer RuntimeTypeInfoDistinctDef<TRet (TClass::*)(TArgs...)const volatile& noexcept>::TypeClassInfo={
	&RuntimeTypeInfoDef<TRet>::TypeInfo,	// Return Type
	sizeof...(TArgs),								// ParameterCount
	cRuntimeTypeInfoDistinctFunctionParameterList<TArgs...>::Value,	// Parameter Type
	&RuntimeTypeInfoDef<TClass>::TypeInfo,
	true,	// Const
	true,	// Volatile
	true,	// Reference
	false,	// RVReference
	true,	// Noexcept
};

#endif	// cnLibrary_CPPFEATURE_NOEXCEPT_FUNC_TYPE >= 201510L

// runtime info - distinct - member function pointer - &&

template<class TClass,class TRet,class...TArgs>
struct RuntimeTypeInfoDistinctDef<TRet (TClass::*)(TArgs...)&&>
{
	static cnLib_CONSTVAR ertTypeClass TypeClass=rtTypeClass::MemberFunctionPointer;
	static const cnVar::cRuntimeTypeInfo_MemberFunctionPointer TypeClassInfo;
};

template<class TClass,class TRet,class...TArgs>
const cnVar::cRuntimeTypeInfo_MemberFunctionPointer RuntimeTypeInfoDistinctDef<TRet (TClass::*)(TArgs...)&&>::TypeClassInfo={
	&RuntimeTypeInfoDef<TRet>::TypeInfo,	// Return Type
	sizeof...(TArgs),								// ParameterCount
	cRuntimeTypeInfoDistinctFunctionParameterList<TArgs...>::Value,	// Parameter Type
	&RuntimeTypeInfoDef<TClass>::TypeInfo,
	false,	// Const
	false,	// Volatile
	true,	// Reference
	true,	// RVReference
	false,	// Noexcept
};

// runtime info - distinct - member function pointer - const&&

template<class TClass,class TRet,class...TArgs>
struct RuntimeTypeInfoDistinctDef<TRet (TClass::*)(TArgs...)const&&>
{
	static cnLib_CONSTVAR ertTypeClass TypeClass=rtTypeClass::MemberFunctionPointer;
	static const cnVar::cRuntimeTypeInfo_MemberFunctionPointer TypeClassInfo;
};


template<class TClass,class TRet,class...TArgs>
const cnVar::cRuntimeTypeInfo_MemberFunctionPointer RuntimeTypeInfoDistinctDef<TRet (TClass::*)(TArgs...)const&&>::TypeClassInfo={
	&RuntimeTypeInfoDef<TRet>::TypeInfo,	// Return Type
	sizeof...(TArgs),								// ParameterCount
	cRuntimeTypeInfoDistinctFunctionParameterList<TArgs...>::Value,	// Parameter Type
	&RuntimeTypeInfoDef<TClass>::TypeInfo,
	true,	// Const
	false,	// Volatile
	true,	// Reference
	true,	// RVReference
	false,	// Noexcept
};

// runtime info - distinct - member function pointer - volatile&&

template<class TClass,class TRet,class...TArgs>
struct RuntimeTypeInfoDistinctDef<TRet (TClass::*)(TArgs...)volatile&&>
{
	static cnLib_CONSTVAR ertTypeClass TypeClass=rtTypeClass::MemberFunctionPointer;
	static const cnVar::cRuntimeTypeInfo_MemberFunctionPointer TypeClassInfo;
};


template<class TClass,class TRet,class...TArgs>
const cnVar::cRuntimeTypeInfo_MemberFunctionPointer RuntimeTypeInfoDistinctDef<TRet (TClass::*)(TArgs...)volatile&&>::TypeClassInfo={
	&RuntimeTypeInfoDef<TRet>::TypeInfo,	// Return Type
	sizeof...(TArgs),								// ParameterCount
	cRuntimeTypeInfoDistinctFunctionParameterList<TArgs...>::Value,	// Parameter Type
	&RuntimeTypeInfoDef<TClass>::TypeInfo,
	false,	// Const
	true,	// Volatile
	true,	// Reference
	true,	// RVReference
	false,	// Noexcept
};
// runtime info - distinct - member function pointer - const volatile&&

template<class TClass,class TRet,class...TArgs>
struct RuntimeTypeInfoDistinctDef<TRet (TClass::*)(TArgs...)const volatile&&>
{
	static cnLib_CONSTVAR ertTypeClass TypeClass=rtTypeClass::MemberFunctionPointer;
	static const cnVar::cRuntimeTypeInfo_MemberFunctionPointer TypeClassInfo;
};


template<class TClass,class TRet,class...TArgs>
const cnVar::cRuntimeTypeInfo_MemberFunctionPointer RuntimeTypeInfoDistinctDef<TRet (TClass::*)(TArgs...)const volatile&&>::TypeClassInfo={
	&RuntimeTypeInfoDef<TRet>::TypeInfo,	// Return Type
	sizeof...(TArgs),								// ParameterCount
	cRuntimeTypeInfoDistinctFunctionParameterList<TArgs...>::Value,	// Parameter Type
	&RuntimeTypeInfoDef<TClass>::TypeInfo,
	true,	// Const
	true,	// Volatile
	true,	// Reference
	true,	// RVReference
	false,	// Noexcept
};


#if cnLibrary_CPPFEATURE_NOEXCEPT_FUNC_TYPE >= 201510L

// runtime info - distinct - member function pointer - && - noexcept

template<class TClass,class TRet,class...TArgs>
struct RuntimeTypeInfoDistinctDef<TRet (TClass::*)(TArgs...)&&noexcept>
{
	static cnLib_CONSTVAR ertTypeClass TypeClass=rtTypeClass::MemberFunctionPointer;
	static const cnVar::cRuntimeTypeInfo_MemberFunctionPointer TypeClassInfo;
};

template<class TClass,class TRet,class...TArgs>
const cnVar::cRuntimeTypeInfo_MemberFunctionPointer RuntimeTypeInfoDistinctDef<TRet (TClass::*)(TArgs...)&&noexcept>::TypeClassInfo={
	&RuntimeTypeInfoDef<TRet>::TypeInfo,	// Return Type
	sizeof...(TArgs),								// ParameterCount
	cRuntimeTypeInfoDistinctFunctionParameterList<TArgs...>::Value,	// Parameter Type
	&RuntimeTypeInfoDef<TClass>::TypeInfo,
	false,	// Const
	false,	// Volatile
	true,	// Reference
	true,	// RVReference
	true,	// Noexcept
};

// runtime info - distinct - member function pointer - const&& - noexcept

template<class TClass,class TRet,class...TArgs>
struct RuntimeTypeInfoDistinctDef<TRet (TClass::*)(TArgs...)const&& noexcept>
{
	static cnLib_CONSTVAR ertTypeClass TypeClass=rtTypeClass::MemberFunctionPointer;
	static const cnVar::cRuntimeTypeInfo_MemberFunctionPointer TypeClassInfo;
};


template<class TClass,class TRet,class...TArgs>
const cnVar::cRuntimeTypeInfo_MemberFunctionPointer RuntimeTypeInfoDistinctDef<TRet (TClass::*)(TArgs...)const&& noexcept>::TypeClassInfo={
	&RuntimeTypeInfoDef<TRet>::TypeInfo,	// Return Type
	sizeof...(TArgs),								// ParameterCount
	cRuntimeTypeInfoDistinctFunctionParameterList<TArgs...>::Value,	// Parameter Type
	&RuntimeTypeInfoDef<TClass>::TypeInfo,
	true,	// Const
	false,	// Volatile
	true,	// Reference
	true,	// RVReference
	true,	// Noexcept
};

// runtime info - distinct - member function pointer - volatile&& - noexcept

template<class TClass,class TRet,class...TArgs>
struct RuntimeTypeInfoDistinctDef<TRet (TClass::*)(TArgs...)volatile&& noexcept>
{
	static cnLib_CONSTVAR ertTypeClass TypeClass=rtTypeClass::MemberFunctionPointer;
	static const rtTypeInfo MemberFunctionParameters[];
	static const cnVar::cRuntimeTypeInfo_MemberFunctionPointer TypeClassInfo;
};


template<class TClass,class TRet,class...TArgs>
const cnVar::cRuntimeTypeInfo_MemberFunctionPointer RuntimeTypeInfoDistinctDef<TRet (TClass::*)(TArgs...)volatile&& noexcept>::TypeClassInfo={
	&RuntimeTypeInfoDef<TRet>::TypeInfo,	// Return Type
	sizeof...(TArgs),								// ParameterCount
	cRuntimeTypeInfoDistinctFunctionParameterList<TArgs...>::Value,	// Parameter Type
	&RuntimeTypeInfoDef<TClass>::TypeInfo,
	false,	// Const
	true,	// Volatile
	true,	// Reference
	true,	// RVReference
	true,	// Noexcept
};
// runtime info - distinct - member function pointer - const volatile&& - noexcept

template<class TClass,class TRet,class...TArgs>
struct RuntimeTypeInfoDistinctDef<TRet (TClass::*)(TArgs...)const volatile&& noexcept>
{
	static cnLib_CONSTVAR ertTypeClass TypeClass=rtTypeClass::MemberFunctionPointer;
	static const cnVar::cRuntimeTypeInfo_MemberFunctionPointer TypeClassInfo;
};


template<class TClass,class TRet,class...TArgs>
const cnVar::cRuntimeTypeInfo_MemberFunctionPointer RuntimeTypeInfoDistinctDef<TRet (TClass::*)(TArgs...)const volatile&& noexcept>::TypeClassInfo={
	&RuntimeTypeInfoDef<TRet>::TypeInfo,	// Return Type
	sizeof...(TArgs),								// ParameterCount
	cRuntimeTypeInfoDistinctFunctionParameterList<TArgs...>::Value,	// Parameter Type
	&RuntimeTypeInfoDef<TClass>::TypeInfo,
	true,	// Const
	true,	// Volatile
	true,	// Reference
	true,	// RVReference
	true,	// Noexcept
};

#endif	// cnLibrary_CPPFEATURE_NOEXCEPT_FUNC_TYPE >= 201510L


#endif // cnLibrary_CPPFEATURE_REF_QUALIFIERS >= 200710L


}	// namespace RuntimeInfo
//---------------------------------------------------------------------------
}	// namespace Var_TH
//---------------------------------------------------------------------------
}	// namespace cnLib_THelper
//---------------------------------------------------------------------------
namespace cnLibrary{
//---------------------------------------------------------------------------
namespace cnVar{
//---------------------------------------------------------------------------

// runtime info


template<class T>
const cnVar::cRuntimeTypeInfo cRuntimeTypeInfoDefinition<T>::Value={
	&cTypeIDDefinition<T>::Value,
	TStoreSizeOf<T>::Value,
	cnLib_THelper::Var_TH::RuntimeInfo::RuntimeTypeInfoDistinctDef<T>::TypeClass,
	false,	// const
	false,	// volatile
	TIsDefaultConstructable<T>::Value,
	TIsDestructable<T>::Value,
	TIsCopyConstructable<T>::Value,
	TIsMoveConstructable<T>::Value,
	TIsCopyAssignable<T>::Value,
	TIsMoveAssignable<T>::Value,
	TIsDefaultConstructNoexcept<T>::Value,
	TIsDestructNoexcept<T>::Value,
	TIsCopyConstructNoexcept<T>::Value,
	TIsMoveConstructNoexcept<T>::Value,
	TIsCopyAssignNoexcept<T>::Value,
	TIsMoveAssignNoexcept<T>::Value,
	&cnLib_THelper::Var_TH::RuntimeInfo::RuntimeTypeOperatorDef<T>::Value,
	&cnLib_THelper::Var_TH::RuntimeInfo::RuntimeTypeInfoDistinctDef<T>::TypeClassInfo
};

// runtime info - const

template<class T>
struct cRuntimeTypeInfoDefinition<T const>
{
	static const cRuntimeTypeInfo Value;
};


template<class T>
const cnVar::cRuntimeTypeInfo cRuntimeTypeInfoDefinition<T const>::Value={
	&cTypeIDDefinition<T const>::ID,
	cnLib_THelper::Var_TH::RuntimeInfo::RuntimeTypeInfoDistinctDef<T>::TypeClass,
	true,	// const
	false,	// volatile
	false,
	false,
	TIsCopyConstructable<T const>::Value,
	TIsMoveConstructable<T const>::Value,
	false,
	false,
	false,
	false,
	TIsCopyConstructNoexcept<T const>::Value,
	TIsMoveConstructNoexcept<T const>::Value,
	false,
	false,
	&cnLib_THelper::Var_TH::RuntimeInfo::RuntimeTypeOperatorDef<T const>::Value,
	&cnLib_THelper::Var_TH::RuntimeInfo::RuntimeTypeInfoDistinctDef<T>::TypeClassInfo
};


// runtime info - volatile

template<class T>
struct cRuntimeTypeInfoDefinition<T volatile>
{
	static const cRuntimeTypeInfo Value;
};


template<class T>
const cnVar::cRuntimeTypeInfo cRuntimeTypeInfoDefinition<T volatile>::Value={
	&cTypeIDDefinition<T volatile>::ID,
	cnLib_THelper::Var_TH::RuntimeInfo::RuntimeTypeInfoDistinctDef<T>::TypeClass,
	false,	// const
	false,	// volatile
	TIsDefaultConstructable<T volatile>::Value,
	TIsDestructable<T volatile>::Value,
	TIsCopyConstructable<T volatile>::Value,
	TIsMoveConstructable<T volatile>::Value,
	TIsCopyAssignable<T volatile>::Value,
	TIsMoveAssignable<T volatile>::Value,
	TIsDefaultConstructNoexcept<T volatile>::Value,
	TIsDestructNoexcept<T volatile>::Value,
	TIsCopyConstructNoexcept<T volatile>::Value,
	TIsMoveConstructNoexcept<T volatile>::Value,
	TIsCopyAssignNoexcept<T volatile>::Value,
	TIsMoveAssignNoexcept<T volatile>::Value,
	&cnLib_THelper::Var_TH::RuntimeInfo::RuntimeTypeOperatorDef<T volatile>::Value,
	&cnLib_THelper::Var_TH::RuntimeInfo::RuntimeTypeInfoDistinctDef<T>::TypeClassInfo
};

// runtime info - const volatile

template<class T>
struct cRuntimeTypeInfoDefinition<T const volatile>
{
	static const cRuntimeTypeInfo Value;
};


template<class T>
const cnVar::cRuntimeTypeInfo cRuntimeTypeInfoDefinition<T const volatile>::Value={
	&cTypeIDDefinition<T const volatile>::ID,
	cnLib_THelper::Var_TH::RuntimeInfo::RuntimeTypeInfoDistinctDef<T>::TypeClass,
	true,	// const
	false,	// volatile
	false,
	false,
	TIsCopyConstructable<T const volatile>::Value,
	TIsMoveConstructable<T const volatile>::Value,
	false,
	false,
	false,
	false,
	TIsCopyConstructNoexcept<T const volatile>::Value,
	TIsMoveConstructNoexcept<T const volatile>::Value,
	false,
	false,
	&cnLib_THelper::Var_TH::RuntimeInfo::RuntimeTypeOperatorDef<T const volatile>::Value,
	&cnLib_THelper::Var_TH::RuntimeInfo::RuntimeTypeInfoDistinctDef<T>::TypeClassInfo
};

template<class T>
struct TRuntimeTypeInfo :	TConstantValue<rtTypeInfo, &cRuntimeTypeInfoDefinition<T>::Value>{};

//---------------------------------------------------------------------------

#if cnLibrary_CPPFEATURE_VARIABLE_TEMPLATES >= 201304L && cnLibrary_CPPFEATURE_INLINE_VARIABLES >= 201606L

template<class T>
inline cnLib_CONSTVAR rtTypeInfo RuntimeTypeInfo=&cRuntimeTypeInfoDefinition<T>::Value;

#endif	// cnLibrary_CPPFEATURE_VARIABLE_TEMPLATES >= 201304L && cnLibrary_CPPFEATURE_INLINE_VARIABLES >= 201606L

//---------------------------------------------------------------------------
}	// namespace cnVar
//---------------------------------------------------------------------------
}	// namespace cnLibrary
//---------------------------------------------------------------------------
namespace cnLib_THelper{
//---------------------------------------------------------------------------
namespace Var_TH{
//---------------------------------------------------------------------------
namespace RuntimeInfo{
//---------------------------------------------------------------------------



template<class T,class TTypePack>
struct cRuntimeDataInfoItemsDefinition;

template<class T,class...VT>
struct cRuntimeDataInfoItemsDefinition< T,cnVar::TTypePack<VT...> >
{
	static cnLib_CONSTVAR uIntn ItemCount=sizeof...(VT);
	static const cnVar::cRuntimeDataInfo::cItem Items[sizeof...(VT)];
};

template<class T,class...VT>
const cnVar::cRuntimeDataInfo::cItem cRuntimeDataInfoItemsDefinition< T,cnVar::TTypePack<VT...> >::Items[sizeof...(VT)]={
	{cnVar::TTypeID<typename cnVar::TRemoveCV<decltype(VT::Value)>::Type>::Value,VT::Value}...
};

struct cRuntimeDataInfoItemsDefinitionEmpty
{
	static cnLib_CONSTVAR uIntn ItemCount=0;
	static cnLib_CONSTVAR cnVar::cRuntimeDataInfo::cItem *Items=0;
};

template<class T,class TTypePack>
struct DataInfoDefTypesSelect
	: cnVar::TTypeDef< cRuntimeDataInfoItemsDefinition<T,TTypePack> >{};

template<class T>
struct DataInfoDefTypesSelect< T,cnVar::TTypePack<> >
	: cnVar::TTypeDef<cRuntimeDataInfoItemsDefinitionEmpty>{};


template<class T,class=void>
struct DataInfoDefSelect
	: cnVar::TTypeDef<cRuntimeDataInfoItemsDefinitionEmpty>{};

template<class T>
struct DataInfoDefSelect<T,typename cnVar::TSelect<0,void,typename T::tRuntimeDataInfoDecl::Type>::Type>
	: DataInfoDefTypesSelect<T,typename T::tRuntimeDataInfoDecl::Type>{};

//---------------------------------------------------------------------------
}	// namespace RuntimeInfo
//---------------------------------------------------------------------------
}	// namespace Var_TH
//---------------------------------------------------------------------------
}	// namespace cnLib_THelper
//---------------------------------------------------------------------------
namespace cnLibrary{
//---------------------------------------------------------------------------
namespace cnVar{
//---------------------------------------------------------------------------
template<class T>
const cRuntimeDataInfo TRuntimeDataInfo<T>::Value=
{
	TRuntimeTypeInfo<T>::Value,
	cnLib_THelper::Var_TH::RuntimeInfo::DataInfoDefSelect<T>::Type::ItemCount,
	cnLib_THelper::Var_TH::RuntimeInfo::DataInfoDefSelect<T>::Type::Items,
};


template<class...VT>
struct TRuntimeInfoDecl
	: TTypeDef< TTypePack<VT...> >{};

template<class...VTParent,class...VT>
struct TRuntimeInfoDecl<TRuntimeInfoDecl<VTParent...>,VT...>
	: TTypeDef< TTypePack<VTParent...,VT...> >{};

//---------------------------------------------------------------------------

struct cMetaClassField
{
	const uChar16 *Name;
	rtDataInfo DataInfo;
	uIntn Offset;
};

struct cMetaClassFunction
{
	const uChar16 *Name;
	const cRuntimeTypeInfo *Type;
};

struct cMetaClass
{
	ufInt16 FieldCount;
	ufInt16 FunctionCount;
	const cMetaClassField *Fields;
	const cMetaClassFunction *Functions;
};
//---------------------------------------------------------------------------
}	// namespace cnVar
//---------------------------------------------------------------------------
typedef const cnVar::cMetaClass* tMetaClass;
//---------------------------------------------------------------------------
namespace cnVar{
//---------------------------------------------------------------------------

struct cIntegerConversionInfo
{
	ufInt8 MatchSize;
	bool IsSigned			:1;
	bool IsConvertible		:1;
	uInt8 (*ConvertU8)(void *Data);
	uInt16 (*ConvertU16)(void *Data);
	uInt32 (*ConvertU32)(void *Data);
	uInt64 (*ConvertU64)(void *Data);
	sInt8 (*ConvertS8)(void *Data);
	sInt16 (*ConvertS16)(void *Data);
	sInt32 (*ConvertS32)(void *Data);
	sInt64 (*ConvertS64)(void *Data);
};

struct cFloatConversionInfo
{
	ufInt8 FloatSize;
	bool IsConvertible		:1;
	Float32 (*Convert32)(void *Data);
	Float64 (*Convert64)(void *Data);
};


template<class T>
struct TIntegerConversionInfoDef
{
	static const cIntegerConversionInfo ConversionInfo;
};


template<class T>
const cIntegerConversionInfo TIntegerConversionInfoDef<T>::ConversionInfo={
	TIntegerConversion<T>::MatchSize,			// MatchSize
	TIntegerConversion<T>::IsSigned,			// IsSigned
	TIntegerConversion<T>::IsConvertible,		// IsConvertible

	TIntegerConversion<T>::template rtCast<uInt8>,
	TIntegerConversion<T>::template rtCast<uInt16>,
	TIntegerConversion<T>::template rtCast<uInt32>,
	TIntegerConversion<T>::template rtCast<uInt64>,
	TIntegerConversion<T>::template rtCast<sInt8>,
	TIntegerConversion<T>::template rtCast<sInt16>,
	TIntegerConversion<T>::template rtCast<sInt32>,
	TIntegerConversion<T>::template rtCast<sInt64>,
};

template<class T>
struct TFloatConversionInfoDef
{
	static const cFloatConversionInfo ConversionInfo;
};


template<class T>
const cFloatConversionInfo TFloatConversionInfoDef<T>::ConversionInfo={
	TFloatConversion<T>::FloatSize,				// FloatSize
	TFloatConversion<T>::IsConvertible,			// IsConvertible

	TFloatConversion<T>::template rtCast<Float32>,
	TFloatConversion<T>::template rtCast<Float32>,
};


template<class T>	struct TIntegerConversionRuntimeInfo :	TConstantValue<const cIntegerConversionInfo*,&TIntegerConversionInfoDef<T>::ConversionInfo>{};
template<class T>	struct TFloatConversionRuntimeInfo :	TConstantValue<const cFloatConversionInfo*,&TFloatConversionInfoDef<T>::ConversionInfo>{};

//---------------------------------------------------------------------------
}	// namespace cnVar
//---------------------------------------------------------------------------
}	// namespace cnLibrary
//---------------------------------------------------------------------------
#endif	// cnLibrary_CPPFEATURELEVEL >= 3
//---------------------------------------------------------------------------
#include <cnTK/TKMacrosCleanup.inc>
//---------------------------------------------------------------------------
#endif	/* cnLibrary_CPPFEATURELEVEL >= 1 */
/*-------------------------------------------------------------------------*/
#endif
