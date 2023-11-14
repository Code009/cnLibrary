/*- cnTK - Type Traits ----------------------------------------------------*/
/*         Developer : Code009                                             */
/*         Create on : 2018-08-14                                          */
/*-------------------------------------------------------------------------*/
#ifndef __cnLibrary_cnTK_TypeTraits_H__
#define	__cnLibrary_cnTK_TypeTraits_H__
/*-------------------------------------------------------------------------*/
#include <cnTK/Common.h>
/*-------------------------------------------------------------------------*/
#if	cnLibrary_CPPFEATURELEVEL >= 1
//---------------------------------------------------------------------------
#include <cnTK/TKMacrosDeclare.inc>

//---------------------------------------------------------------------------
namespace cnLibrary{
//---------------------------------------------------------------------------
namespace cnVar{
//---------------------------------------------------------------------------

template<class T>	struct TIsVoid						: TConstantValueFalse{};
template<>			struct TIsVoid<void>				: TConstantValueTrue{};
template<>			struct TIsVoid<const void>			: TConstantValueTrue{};
template<>			struct TIsVoid<volatile void>		: TConstantValueTrue{};
template<>			struct TIsVoid<const volatile void>	: TConstantValueTrue{};

template<class TDest,class TSrc> struct TCopyConst							: TTypeDef<TDest>{};
template<class TDest,class TSrc> struct TCopyConst<TDest,const TSrc>		: TTypeDef<const TDest>{};
template<class TDest,class TSrc> struct TCopyVolatile						: TTypeDef<TDest>{};
template<class TDest,class TSrc> struct TCopyVolatile<TDest,volatile TSrc>	: TTypeDef<volatile TDest>{};
template<class TDest,class TSrc> struct TCopyCV								: TCopyConst<typename TCopyVolatile<TDest,TSrc>::Type,TSrc>{};


template<class T> struct TRemoveConst			: TTypeDef<T>{};
template<class T> struct TRemoveConst<const T>	: TTypeDef<T>{};

template<class T> struct TRemoveVolatile			: TTypeDef<T>{};
template<class T> struct TRemoveVolatile<volatile T>: TTypeDef<T>{};

template<class T> struct TRemoveCV	: TRemoveConst<typename TRemoveVolatile<T>::Type>{};

template<class T> struct TRemovePointer			: TTypeDef<T>{};
template<class T> struct TRemovePointer<T *>	: TTypeDef<T>{};

template<class T> struct TRemoveCVPtr	: TRemoveCV<typename TRemovePointer<T>::Type>{};

template<class T> struct TRemoveReference		: TTypeDef<T>{};
template<class T> struct TRemoveReference<T &>	: TTypeDef<T>{};
#if cnLibrary_CPPFEATURE_RVALUE_REFERENCES >= 200610L
template<class T> struct TRemoveReference<T &&>	: TTypeDef<T>{};
#endif

template<class T> struct TRemoveCVRef	: TRemoveCV<typename TRemoveReference<T>::Type>{};



#if cnLibrary_CPPFEATURE_ALIAS_TEMPLATES >= 200704L


// type qualifier transform

template<class TDest,class TSrc> using CopyConst=		typename TCopyConst<TDest,TSrc>::Type;
template<class TDest,class TSrc> using CopyVolatile=	typename TCopyVolatile<TDest,TSrc>::Type;
template<class TDest,class TSrc> using CopyCV=			CopyVolatile<CopyConst<TDest,TSrc>,TSrc>;

// qualifier operation

template<class T> using RemoveConst=typename TRemoveConst<T>::Type;
template<class T> using RemoveVolatile=typename TRemoveVolatile<T>::Type;
template<class T> using RemoveCV=typename TRemoveCV<T>::Type;

template<class T> using RemovePointer=typename TRemovePointer< RemoveCV<T> >::Type;
template<class T> using RemoveCVPtr=typename TRemoveCVPtr<T>::Type;

template<class T> using RemoveReference=typename TRemoveReference<T>::Type;
template<class T> using RemoveCVRef=typename TRemoveCVRef<T>::Type;


#endif // cnLibrary_CPPFEATURE_ALIAS_TEMPLATES >= 200704L

//---------------------------------------------------------------------------

template<class T> struct TIsConst			: TConstantValueFalse{};
template<class T> struct TIsConst<const T>	: TConstantValueTrue{};

template<class T> struct TIsVolatile				: TConstantValueFalse{};
template<class T> struct TIsVolatile<volatile T>	: TConstantValueTrue{};

//---------------------------------------------------------------------------


#if cnLibrary_CPPFEATURE_VARIABLE_TEMPLATES >= 201304L

template<class T>
static cnLib_CONSTVAR bool IsConst=TIsConst<T>::Value;

template<class T>
static cnLib_CONSTVAR bool IsVolatile=TIsVolatile<T>::Value;

#endif // cnLibrary_CPPFEATURE_VARIABLE_TEMPLATES >= 201304L


template<class T>	struct TIsPointer 		: TConstantValueFalse{};
template<class T>	struct TIsPointer<T*>	: TConstantValueTrue{};



template<class T,class TTest>	struct TIsPointerOf 						: TConstantValueFalse{};
template<class T>				struct TIsPointerOf<T,T*>					: TConstantValueTrue{};
template<class T>				struct TIsPointerOf<T,const T*>				: TConstantValueTrue{};
template<class T>				struct TIsPointerOf<T,volatile T*>			: TConstantValueTrue{};
template<class T>				struct TIsPointerOf<T,volatile const T*>	: TConstantValueTrue{};


template<class T>	struct TIsReference 		: TConstantValueFalse{};
template<class T>	struct TIsReference<T&>		: TConstantValueTrue{};
#if cnLibrary_CPPFEATURE_RVALUE_REFERENCES >= 200610L
template<class T>	struct TIsReference<T&&>	: TConstantValueTrue{};
#endif

//---------------------------------------------------------------------------
}	// namespace cnVar
//---------------------------------------------------------------------------
}	// namespace cnLibrary
//---------------------------------------------------------------------------
namespace cnLib_THelper{
//---------------------------------------------------------------------------
namespace Var_TH{
//---------------------------------------------------------------------------


template<class T,class=void>
struct IsClass
	: cnVar::TConstantValueFalse{};

template<class T>
struct IsClass<T,typename cnVar::TSelect<0,void,int T::*>::Type>
	: cnVar::TConstantValueTrue{};

template<class T>
struct IsNumerical_Expression
{
	template<int=sizeof(cnVar::DeclVar<T>()*1)>
	struct Test
	{
		typedef void Type;
	};
	
	template<int=sizeof(cnVar::DeclVar<T>()&1)>
	struct Integer
	{
		typedef void Type;
	};

};


template<class T,class=void>
struct IsNumerical
	: cnVar::TConstantValueFalse{};

template<class T>
struct IsNumerical<T,typename IsNumerical_Expression<T>::template Test<>::Type>
	: cnVar::TConstantValueTrue{};



template<class T,class=void>
struct IsInteger
	: cnVar::TConstantValueFalse{};

template<class T>
struct IsInteger<T,typename IsNumerical_Expression<T>::template Integer<>::Type>
	: cnVar::TConstantValueTrue{};



template<class T,class TDef>
struct RequireDefined
	: cnVar::TSelect<cnVar::TStoreSizeOf<T>::Value&&0,cnVar::TTypeDef<TDef> >::Type{};


template<class T,class TCompare>
struct IsComparable_Expression
{
	template<int=sizeof((cnVar::DeclVar<T&>() < cnVar::DeclVar<TCompare&>()),0)>
	struct Less
	{
		typedef void Type;
	};
};

template<class T,class TCompare,class=typename RequireDefined<typename RequireDefined<T,TCompare>::Type,void>::Type>
struct IsComparable_Less
	: cnVar::TConstantValueFalse{};

template<class T,class TCompare>
struct IsComparable_Less<T,TCompare,typename IsComparable_Expression<T,TCompare>::template Less<>::Type>
	: cnVar::TConstantValueTrue{};


template<class TConvertFrom,class TConvertTo>
struct IsConvertible_Expression
{
	template<int=sizeof((static_cast<TConvertTo>(cnVar::DeclVar<TConvertFrom>())),0)>
	struct Test
	{
		typedef void Type;
	};
};


template<class TConvertFrom,class TConvertTo,class=typename RequireDefined<typename RequireDefined<TConvertFrom,TConvertTo>::Type,void>::Type>
struct IsConvertible
	: cnVar::TConstantValueFalse{};

template<class TConvertFrom,class TConvertTo>
struct IsConvertible<TConvertFrom,TConvertTo,typename IsConvertible_Expression<TConvertFrom,TConvertTo>::template Test<>::Type>
	: cnVar::TConstantValueTrue{};


template<class TConvertFrom,class TConvertTo>
struct IsReinterpretable_Distinct
	: cnVar::TConstantValueBool<sizeof(TConvertFrom)==sizeof(TConvertTo) && IsConvertible<TConvertFrom*,TConvertTo*>::Value>{};

template<class TConvertFrom>	struct IsReinterpretable_Distinct<TConvertFrom,void>: cnVar::TConstantValueTrue{};
template<class TConvertTo>		struct IsReinterpretable_Distinct<void,TConvertTo>	: cnVar::TConstantValueFalse{};
template<>						struct IsReinterpretable_Distinct<void,void>		: cnVar::TConstantValueTrue{};


template<class TConvertFrom,class TConvertTo>
struct IsReinterpretable : IsReinterpretable_Distinct<
	typename cnVar::TRemoveCV<TConvertFrom>::Type,
	typename cnVar::TRemoveCV<TConvertTo>::Type
>{};

template<class T>
struct IsInstantiable_Expression
{
	template<int=sizeof(cnVar::DeclVar<T>())>
	struct Test
	{
		typedef void Type;
	};
};


template<class T,class=typename RequireDefined<T,void>::Type>
struct IsInstantiable
	: cnVar::TConstantValueFalse{};

template<class T>
struct IsInstantiable<T,typename IsInstantiable_Expression<T>::template Test<>::Type>
	: cnVar::TConstantValueTrue{};


#if cnLibrary_CPPFEATURE_VARIADIC_TEMPLATES >= 200704L

template<class T,class...TArgs>
struct IsConstructableBy_Expression
{
	template<int=sizeof(T(cnVar::DeclVar<TArgs>()...))>
	struct Test
	{
		typedef void Type;
	};
};

template<class T,class TEnable,class...TArgs>
struct IsConstructableBy
	: cnVar::TConstantValueFalse{};

template<class T,class...TArgs>
struct IsConstructableBy<T,typename IsConstructableBy_Expression<T,TArgs...>::template Test<>::Type,TArgs...>
	: cnVar::TConstantValueTrue{};

// cnLibrary_CPPFEATURE_VARIADIC_TEMPLATES >= 200704L
#else
// cnLibrary_CPPFEATURE_VARIADIC_TEMPLATES < 200704L

template<class T,class TArg>
struct IsConstructableBy_Expression
{
	template<int=sizeof(T(cnVar::DeclVar<TArg>()))>
	struct Test
	{
		typedef void Type;
	};
};

template<class T,class TEnable,class TArg>
struct IsConstructableBy
	: cnVar::TConstantValueFalse{};

template<class T,class TArg>
struct IsConstructableBy<T,typename IsConstructableBy_Expression<T,TArg>::template Test<>::Type,TArg>
	: cnVar::TConstantValueTrue{};


#endif	// cnLibrary_CPPFEATURE_VARIADIC_TEMPLATES < 200704L

template<class T>
struct IsConstructable_Expression
{
	template<int=sizeof(T(),0)>
	struct Default
	{
		typedef void Type;
	};

	template<int=sizeof(T(cnVar::DeclVar<const T&>()))>
	struct Copy
	{
		typedef void Type;
	};

#if cnLibrary_CPPFEATURE_RVALUE_REFERENCES >= 200610L
	template<int=sizeof(T(cnVar::DeclVar<T&&>()))>
	struct Move
	{
		typedef void Type;
	};
#endif	// cnLibrary_CPPFEATURE_RVALUE_REFERENCES >= 200610L

};

template<class T,class=void>
struct IsDefaultConstructable
	: cnVar::TConstantValueFalse{};

template<class T>
struct IsDefaultConstructable<T,typename IsConstructable_Expression<T>::template Default<>::Type>
	: cnVar::TConstantValueTrue{};

template<class T,class=void>
struct IsCopyConstructable
	: cnVar::TConstantValueFalse{};

template<class T>
struct IsCopyConstructable<T,typename IsConstructable_Expression<T>::template Copy<>::Type>
	: cnVar::TConstantValueTrue{};


template<class T,class=void>
struct IsMoveConstructable
	: cnVar::TConstantValueFalse{};

#if cnLibrary_CPPFEATURE_RVALUE_REFERENCES >= 200610L
template<class T>
struct IsMoveConstructable<T,typename IsConstructable_Expression<T>::template Move<>::Type>
	: cnVar::TConstantValueTrue{};
#endif	// cnLibrary_CPPFEATURE_RVALUE_REFERENCES >= 200610L


template<class T>
struct IsDestructable_Expression
{
	template<int=sizeof((cnVar::DeclVar<T&>().~T()),0)>
	struct Test
	{
		typedef void Type;
	};

};

template<class T,class=void>
struct IsDestructable
	: cnVar::TConstantValueFalse{};

template<class T>
struct IsDestructable<T,typename IsDestructable_Expression<T>::template Test<>::Type>
	: cnVar::TConstantValueTrue{};

template<>
struct IsDestructable<void,void>
	: cnVar::TConstantValueFalse{};

template<class T>
struct IsAssignable_Expression
{
	template<class TSrc>
	struct By
	{
		template<int=sizeof((cnVar::DeclVar<T&>()=cnVar::DeclVar<TSrc>()),0)>
		struct Test
		{
			typedef void Type;
		};
	};

	template<int=sizeof((cnVar::DeclVar<T&>()=cnVar::DeclVar<const T&>()),0)>
	struct Copy
	{
		typedef void Type;
	};

#if cnLibrary_CPPFEATURE_RVALUE_REFERENCES >= 200610L

	template<int=sizeof((cnVar::DeclVar<T&>()=cnVar::DeclVar<T&&>()),0)>
	struct Move
	{
		typedef void Type;
	};

#endif	// cnLibrary_CPPFEATURE_RVALUE_REFERENCES >= 200610L

};

template<class TDest,class TSrc,class=void>
struct IsAssignable
	: cnVar::TConstantValueFalse{};

template<class TDest,class TSrc>
struct IsAssignable<TDest,TSrc,typename IsAssignable_Expression<TDest>::template By<TSrc>::template Test<>::Type>
	: cnVar::TConstantValueTrue{};


template<class T,class=void>
struct IsCopyAssignable
	: cnVar::TConstantValueFalse{};

template<class T>
struct IsCopyAssignable<T,typename IsAssignable_Expression<T>::template Copy<>::Type>
	: cnVar::TConstantValueTrue{};


template<class T,class=void>
struct IsMoveAssignable
	: cnVar::TConstantValueFalse{};

#if cnLibrary_CPPFEATURE_RVALUE_REFERENCES >= 200610L
template<class T>
struct IsMoveAssignable<T,typename IsAssignable_Expression<T>::template Move<>::Type>
	: cnVar::TConstantValueTrue{};
#endif	// cnLibrary_CPPFEATURE_RVALUE_REFERENCES >= 200610L


template<class T>
struct CanAllocation_Expression
{
	template<int=sizeof(new T)>
	struct New
	{
		typedef void Type;
	};

	template<int=sizeof((delete cnVar::DeclVar<T*>()),0)>
	struct Delete
	{
		typedef void Type;
	};

};


template<class T,class=void>
struct CanNew
	: cnVar::TConstantValueFalse{};

template<class T>
struct CanNew<T,typename CanAllocation_Expression<T>::template New<>::Type>
	: cnVar::TConstantValueTrue{};


template<class T,class=void>
struct CanDelete
	: cnVar::TConstantValueFalse{};

template<class T>
struct CanDelete<T,typename CanAllocation_Expression<T>::template Delete<>::Type>
	: cnVar::TConstantValueTrue{};




template<class TT>	union IsTrivial_TU{
	int v;
	TT t; 
	~IsTrivial_TU()noexcept(true){}
};

template<class T,class=void>
struct IsTrivial_Union
	: cnVar::TConstantValueFalse{};

template<class T>
struct IsTrivial_Union<T,typename IsConstructable_Expression< IsTrivial_TU<T> >::template Default<>::Type>
	: cnVar::TConstantValueTrue{};


template<class T>
struct IsTrivial_NotVoid
	: cnVar::TSelect<cnLib_THelper::Var_TH::IsInstantiable<T>::Value
		, cnVar::TConstantValueFalse
		, IsTrivial_Union<T>
	>::Type{};

template<class T>
struct IsTrivial
	: cnVar::TSelect<cnVar::TIsVoid<T>::Value
		, IsTrivial_NotVoid<T>
		, cnVar::TConstantValueTrue
	>::Type{};

template<class T>
struct IsTrivial<T&>
	: cnVar::TConstantValueTrue{};

#if cnLibrary_CPPFEATURE_RVALUE_REFERENCES >= 200610L
template<class T>
struct IsTrivial<T&&>
	: cnVar::TConstantValueTrue{};
#endif	// cnLibrary_CPPFEATURE_RVALUE_REFERENCES >= 200610L

//---------------------------------------------------------------------------
}	// namespace Var_TH
//---------------------------------------------------------------------------
}	// namespace cnLib_THelper
//---------------------------------------------------------------------------
namespace cnLibrary{
//---------------------------------------------------------------------------
namespace cnVar{
//---------------------------------------------------------------------------

// TIsClass
//	check if a type is use defined type. class or struct or union
template<class T>
struct TIsClass
	: TConstantValueBool<cnLib_THelper::Var_TH::IsClass<T>::Value>{};

template<class T>
struct TIsInteger
	: TConstantValueBool<!cnLib_THelper::Var_TH::IsClass<T>::Value && cnLib_THelper::Var_TH::IsInteger<T>::Value>{};

template<class T>
struct TIsFloat
	: TConstantValueBool<!cnLib_THelper::Var_TH::IsClass<T>::Value && cnLib_THelper::Var_TH::IsNumerical<T>::Value && !cnLib_THelper::Var_TH::IsInteger<T>::Value>{};


template<class T>
struct TIsComparable
{
	
	template<class TCompare>
	struct Less
		: TConstantValueBool<cnLib_THelper::Var_TH::IsComparable_Less<T,TCompare>::Value>{};
};

template<class TConvertFrom,class TConvertTo>
struct TIsConvertible
	: TConstantValueBool<cnLib_THelper::Var_TH::IsConvertible<TConvertFrom,TConvertTo>::Value>{};


template<class TConvertFrom,class TConvertTo>
struct TIsReinterpretable
	: TConstantValueBool<cnLib_THelper::Var_TH::IsReinterpretable<TConvertFrom,TConvertTo>::Value>{};

template<class T>
struct TIsInstantiable
	: TConstantValueBool<cnLib_THelper::Var_TH::IsInstantiable<T>::Value>{};

template<class T>
struct TIsTrivial
	: TConstantValueBool<cnLib_THelper::Var_TH::IsTrivial<T>::Value>{};

#if cnLibrary_CPPFEATURE_VARIADIC_TEMPLATES >= 200704L

template<class TDest,class...TSrc>
struct TIsConstuctableBy
	: TConstantValueBool<cnLib_THelper::Var_TH::IsConstructableBy<TDest,void,TSrc...>::Value>{};

	// cnLibrary_CPPFEATURE_VARIADIC_TEMPLATES >= 200704L
#else
	// cnLibrary_CPPFEATURE_VARIADIC_TEMPLATES < 200704L

template<class TDest,class TSrc>
struct TIsConstuctableBy
	: TConstantValueBool<cnLib_THelper::Var_TH::IsConstructableBy<TDest,void,TSrc>::Value>{};

#endif	// cnLibrary_CPPFEATURE_VARIADIC_TEMPLATES < 200704L

template<class T>
struct TIsDefaultConstructable
	: TConstantValueBool<cnLib_THelper::Var_TH::IsDefaultConstructable<T>::Value>{};

template<class T>
struct TIsCopyConstructable
	: TConstantValueBool<cnLib_THelper::Var_TH::IsCopyConstructable<T>::Value>{};


template<class T>
struct TIsMoveConstructable
	: TConstantValueBool<cnLib_THelper::Var_TH::IsMoveConstructable<T>::Value>{};


template<class T>
struct TIsDestructable
	: TConstantValueBool<cnLib_THelper::Var_TH::IsDestructable<T>::Value>{};


template<class TDest,class TSrc>
struct TIsAssignable
	: TConstantValueBool<cnLib_THelper::Var_TH::IsAssignable<TDest,TSrc>::Value>{};

template<class T>
struct TIsCopyAssignable
	: TConstantValueBool<cnLib_THelper::Var_TH::IsCopyAssignable<T>::Value>{};


template<class T>
struct TIsMoveAssignable
	: TConstantValueBool<cnLib_THelper::Var_TH::IsMoveAssignable<T>::Value>{};

template<class T>
struct TCanNew
	: TConstantValueBool<cnLib_THelper::Var_TH::CanNew<T>::Value>{};




template<class T>
struct TCanDelete
	: TConstantValueBool<cnLib_THelper::Var_TH::CanDelete<T>::Value>{};


#if cnLibrary_CPPFEATURE_VARIABLE_TEMPLATES >= 201304L


// IsClass
//	 test if type is user defined class
template<class T>
static cnLib_CONSTVAR bool IsClass=TIsClass<T>::Value;

// IsInteger
//	 test if type is trivial integer type
template<class T>
static cnLib_CONSTVAR bool IsInteger=TIsInteger<T>::Value;

// IsFloat
//	 test if type is trivial float type
template<class T>
static cnLib_CONSTVAR bool IsFloat=TIsFloat<T>::Value;

// IsConvertible
template<class TConvertFrom,class TConvertTo>
static cnLib_CONSTVAR bool IsConvertible=TIsConvertible<TConvertFrom,TConvertTo>::Value;

// IsInstantiable
//	 test if type is instantiable
template<class T>
static cnLib_CONSTVAR bool IsInstantiable=TIsInstantiable<T>::Value;
// IsTrivial
//	 test if type is trivial
template<class T>
static cnLib_CONSTVAR bool IsTrivial=TIsTrivial<T>::Value;


template<class T>
static cnLib_CONSTVAR bool IsDefaultConstructable=TIsDefaultConstructable<T>::Value;

#if cnLibrary_CPPFEATURE_VARIADIC_TEMPLATES >= 200704L

template<class TDest,class...TSrc>
static cnLib_CONSTVAR bool IsConstuctableBy=TIsConstuctableBy<TDest,TSrc...>::Value;

#endif	// cnLibrary_CPPFEATURE_VARIADIC_TEMPLATES >= 200704L


template<class T>
static cnLib_CONSTVAR bool IsCopyConstructable=TIsCopyConstructable<T>::Value;

template<class T>
static cnLib_CONSTVAR bool IsMoveConstructable=TIsMoveConstructable<T>::Value;

template<class T>
static cnLib_CONSTVAR bool IsDestructable=TIsDestructable<T>::Value;

template<class TDest,class...TSrc>
static cnLib_CONSTVAR bool IsAssignable=TIsAssignable<TDest,TSrc...>::Value;


template<class T>
static cnLib_CONSTVAR bool IsCopyAssignable=TIsCopyAssignable<T>::Value;

template<class T>
static cnLib_CONSTVAR bool IsMoveAssignable=TIsMoveAssignable<T>::Value;


template<class T>
static cnLib_CONSTVAR bool CanNew=TCanNew<T>::Value;

template<class T>
static cnLib_CONSTVAR bool CanDelete=TCanDelete<T>::Value;

#endif // cnLibrary_CPPFEATURE_VARIABLE_TEMPLATES >= 201304L


#if cnLibrary_CPPFEATURE_CONSTEVAL >= 201811L

template<class TConvertTo,class TConvertFrom>
consteval bool IsConvertibleTo(TConvertFrom){
	return TIsConvertible<TConvertFrom,TConvertTo>::Value;
}

#endif // cnLibrary_CPPFEATURE_CONSTEVAL > 201811L


//---------------------------------------------------------------------------
}	// namespace cnVar
//---------------------------------------------------------------------------
}	// namespace cnLibrary
//---------------------------------------------------------------------------
namespace cnLib_THelper{
//---------------------------------------------------------------------------
namespace Var_TH{
//---------------------------------------------------------------------------



template<class TClass,class TBase>
struct ClassInheritFrom_Expression
{
	template<int=sizeof(static_cast<TBase*>(cnVar::DeclVar<TClass*>()))>
	struct Test
	{
		typedef void Type;
	};

};


template<class TClass,class TBase,class=typename RequireDefined<TClass,void>::Type>
struct ClassInheritFrom
	: cnVar::TConstantValueFalse{};

template<class TClass,class TBase>
struct ClassInheritFrom<TClass,TBase,typename ClassInheritFrom_Expression<TClass,TBase>::template Test<>::Type>
	: cnVar::TConstantValueTrue{};


//---------------------------------------------------------------------------


template<class TClass>
struct ClassAllocation_Expression
{
	template<int=sizeof(TClass::operator new(1))>
	struct New
	{
		typedef void Type;
	};
	
	template<int=sizeof(TClass::operator delete(0),0)>
	struct Delete
	{
		typedef void Type;
	};
	
	template<int=sizeof(TClass::operator delete(0,1),0)>
	struct DeleteSize
	{
		typedef void Type;
	};
	
	template<int=sizeof(TClass::operator new[](1))>
	struct NewArray
	{
		typedef void Type;
	};
	
	template<int=sizeof(TClass::operator delete[](0),0)>
	struct DeleteArray
	{
		typedef void Type;
	};
	
	template<int=sizeof(TClass::operator delete[](0,1),0)>
	struct DeleteSizeArray
	{
		typedef void Type;
	};
};


template<class TClass,class=typename RequireDefined<TClass,void>::Type>
struct HasOperatorNew
	: cnVar::TConstantValueFalse{};

template<class TClass>
struct HasOperatorNew<TClass,typename ClassAllocation_Expression<TClass>::template New<>::Type>
	: cnVar::TConstantValueTrue{};



template<class TClass,class=typename RequireDefined<TClass,void>::Type>
struct HasOperatorDelete
	: cnVar::TConstantValueFalse{};

template<class TClass>
struct HasOperatorDelete<TClass,typename ClassAllocation_Expression<TClass>::template Delete<>::Type>
	: cnVar::TConstantValueTrue{};



template<class TClass,class=typename RequireDefined<TClass,void>::Type>
struct HasOperatorDeleteSize
	: cnVar::TConstantValueFalse{};

template<class TClass>
struct HasOperatorDeleteSize<TClass,typename ClassAllocation_Expression<TClass>::template DeleteSize<>::Type>
	: cnVar::TConstantValueTrue{};


template<class TClass,class=typename RequireDefined<TClass,void>::Type>
struct HasOperatorNewArray
	: cnVar::TConstantValueFalse{};

template<class TClass>
struct HasOperatorNewArray<TClass,typename ClassAllocation_Expression<TClass>::template NewArray<>::Type>
	: cnVar::TConstantValueTrue{};


template<class TClass,class=typename RequireDefined<TClass,void>::Type>
struct HasOperatorDeleteArray
	: cnVar::TConstantValueFalse{};

template<class TClass>
struct HasOperatorDeleteArray<TClass,typename ClassAllocation_Expression<TClass>::template DeleteArray<>::Type>
	: cnVar::TConstantValueTrue{};


template<class TClass,class=typename RequireDefined<TClass,void>::Type>
struct HasOperatorDeleteSizeArray
	: cnVar::TConstantValueFalse{};

template<class TClass>
struct HasOperatorDeleteSizeArray<TClass,typename ClassAllocation_Expression<TClass>::template DeleteSizeArray<>::Type>
	: cnVar::TConstantValueTrue{};


struct SpecialMemberIsNoexceptFalse
{
	template<class T>	struct DefaultConstruct : cnVar::TConstantValueFalse{};
	template<class T>	struct Destruct : cnVar::TConstantValueFalse{};
	template<class T>	struct CopyConstruct : cnVar::TConstantValueFalse{};
	template<class T>	struct CopyAssign : cnVar::TConstantValueFalse{};
	template<class T>	struct MoveConstruct : cnVar::TConstantValueFalse{};
	template<class T>	struct MoveAssign : cnVar::TConstantValueFalse{};

};

#ifndef cnLibrary_CPPEXCLUDE_NOEXCEPT

template<bool Value>
struct SpecialMemberIsNoexcept
{
	template<class T>	struct DefaultConstruct : cnVar::TConstantValueBool<noexcept(typename cnVar::TRemoveVolatile<T>::Type())>{};
	template<class T>	struct Destruct : cnVar::TConstantValueBool<noexcept(cnVar::DeclVar<T&>().~T())>{};
	template<class T>	struct CopyConstruct : cnVar::TConstantValueBool<noexcept(typename cnVar::TRemoveVolatile<T>::Type(cnVar::DeclVar<const T&>()))>{};
	template<class T>	struct CopyAssign : cnVar::TConstantValueBool<noexcept(cnVar::DeclVar<T&>()=cnVar::DeclVar<const T&>())>{};
#if cnLibrary_CPPFEATURE_RVALUE_REFERENCES >= 200610L
	template<class T>	struct MoveConstruct : cnVar::TConstantValueBool<noexcept(typename cnVar::TRemoveVolatile<T>::Type(cnVar::DeclVar<T&&>()))>{};
	template<class T>	struct MoveAssign : cnVar::TConstantValueBool<noexcept(cnVar::DeclVar<T&>()=cnVar::DeclVar<T&&>())>{};
#else	// cnLibrary_CPPFEATURE_RVALUE_REFERENCES < 200610L
	template<class T>	struct MoveConstruct : cnVar::TConstantValueFalse{};
	template<class T>	struct MoveAssign : cnVar::TConstantValueFalse{};
#endif	// cnLibrary_CPPFEATURE_RVALUE_REFERENCES

};
template<>
struct SpecialMemberIsNoexcept<false>
	: SpecialMemberIsNoexceptFalse{};

// !cnLibrary_CPPEXCLUDE_NOEXCEPT

#else
// cnLibrary_CPPEXCLUDE_NOEXCEPT
template<bool Value>
struct SpecialMemberIsNoexcept
	: SpecialMemberIsNoexceptFalse{};
#endif

//---------------------------------------------------------------------------
}	// namespace Var_TH
//---------------------------------------------------------------------------
}	// namespace cnLib_THelper
//---------------------------------------------------------------------------
namespace cnLibrary{
//---------------------------------------------------------------------------
namespace cnVar{


template<class TClass,class TBase>
struct TClassInheritFrom
	: TConstantValueBool<cnLib_THelper::Var_TH::ClassInheritFrom<TClass,TBase>::Value>{};


template<class T>
struct THasOperatorNew
	: TConstantValueBool<cnLib_THelper::Var_TH::HasOperatorNew<T>::Value>{};


template<class T>
struct THasOperatorDelete
	: TConstantValueBool<cnLib_THelper::Var_TH::HasOperatorDelete<T>::Value>{};


template<class T>
struct THasOperatorDeleteSize
	: TConstantValueBool<cnLib_THelper::Var_TH::HasOperatorDeleteSize<T>::Value>{};


template<class T>
struct THasOperatorNewArray
	: TConstantValueBool<cnLib_THelper::Var_TH::HasOperatorNewArray<T>::Value>{};


template<class T>
struct THasOperatorDeleteArray
	: TConstantValueBool<cnLib_THelper::Var_TH::HasOperatorDeleteArray<T>::Value>{};


template<class T>
struct THasOperatorDeleteSizeArray
	: TConstantValueBool<cnLib_THelper::Var_TH::HasOperatorDeleteSizeArray<T>::Value>{};


template<class T>
struct TIsDefaultConstructNoexcept
	: cnLib_THelper::Var_TH::SpecialMemberIsNoexcept<TIsDefaultConstructable<T>::Value>::template DefaultConstruct<T>{};

template<class T>
struct TIsDestructNoexcept
	: cnLib_THelper::Var_TH::SpecialMemberIsNoexcept<TIsDestructable<T>::Value>::template Destruct<T>{};

template<class T>
struct TIsCopyConstructNoexcept
	: cnLib_THelper::Var_TH::SpecialMemberIsNoexcept<TIsCopyConstructable<T>::Value>::template CopyConstruct<T>{};

template<class T>
struct TIsCopyAssignNoexcept
	: cnLib_THelper::Var_TH::SpecialMemberIsNoexcept<TIsCopyAssignable<T>::Value>::template CopyAssign<T>{};

template<class T>
struct TIsMoveConstructNoexcept
	: cnLib_THelper::Var_TH::SpecialMemberIsNoexcept<TIsMoveConstructable<T>::Value>::template MoveConstruct<T>{};
template<class T>
struct TIsMoveAssignNoexcept
	: cnLib_THelper::Var_TH::SpecialMemberIsNoexcept<TIsMoveAssignable<T>::Value>::template MoveAssign<T>{};


#if cnLibrary_CPPFEATURE_VARIABLE_TEMPLATES >= 201304L
template<class T>
static cnLib_CONSTVAR bool HasOperatorNew=THasOperatorNew<T>::Value;

template<class T>
static cnLib_CONSTVAR bool HasOperatorDelete=THasOperatorDelete<T>::Value;

template<class T>
static cnLib_CONSTVAR bool HasOperatorDeleteSize=THasOperatorDeleteSize<T>::Value;

#endif	// cnLibrary_CPPFEATURE_VARIABLE_TEMPLATES >= 201304L


//---------------------------------------------------------------------------
}	// namespace cnVar
//---------------------------------------------------------------------------
}	// namespace cnLibrary
//---------------------------------------------------------------------------
// Cast
//---------------------------------------------------------------------------
namespace cnLib_THelper{
//---------------------------------------------------------------------------
namespace Var_TH{
//---------------------------------------------------------------------------
template<bool>
struct TryCast
{
	template<class TDest,class TSrc>
	static bool Call(TDest &Dest,const TSrc &Src)
		noexcept(cnLib_NOEXCEPTEXPR(Dest=static_cast<TDest>(Src)))
	{	Dest=static_cast<TDest>(Src);	return true;	}

	template<class TDest,class TSrc>
	static TDest CastRet(const TSrc &Src)
		noexcept(cnLib_NOEXCEPTEXPR(TDest(static_cast<TDest>(Src))))
	{	return static_cast<TDest>(Src);	}
	template<class TDest,class TSrc>
	static TDest CastRetDef(const TSrc &Src,const TDest &)
		noexcept(cnLib_NOEXCEPTEXPR(TDest(static_cast<TDest>(Src))))
	{	return static_cast<TDest>(Src);	}
};
template<>
struct TryCast<false>
{
	template<class TDest,class TSrc>
	static bool Call(TDest&,const TSrc&)noexcept(true){	return false;	}


	template<class TDest,class TSrc>
	static TDest CastRet(const TSrc &)
		noexcept(cnLib_NOEXCEPTEXPR(TDest()))
	{	return TDest();	}
	template<class TDest,class TSrc>
	static TDest CastRetDef(const TSrc &,const TDest &Def)noexcept(true)
	{	return Def;	}
};

//---------------------------------------------------------------------------
}	// namespace Var_TH
//---------------------------------------------------------------------------
}	// namespace cnLib_THelper
//---------------------------------------------------------------------------
namespace cnLibrary{
//---------------------------------------------------------------------------
namespace cnVar{


#if cnLibrary_CPPFEATURE_RVALUE_REFERENCES >= 200610L

//- forward argument --------------------------------------------------------
template<class T>
inline T&& Forward(typename TRemoveReference<T>::Type &Arg)noexcept(true){
	return static_cast<T&&>(Arg);
}
template<class T>
inline T&& Forward(typename TRemoveReference<T>::Type &&Arg)noexcept(true){
	return static_cast<T&&>(Arg);
}
//- explicit move -----------------------------------------------------------
template<class T>
inline typename TRemoveReference<T>::Type && MoveCast(T&& Var)noexcept(true)
{	return static_cast<typename TRemoveReference<T>::Type&&>(Var);	}

// cnLibrary_CPPFEATURE_RVALUE_REFERENCES >= 200610L
#else
// cnLibrary_CPPFEATURE_RVALUE_REFERENCES < 200610L

template<class T>
inline T& Forward(T &Arg)noexcept(true)
{	return Arg;	}
template<class T>
inline T& MoveCast(T &Var)noexcept(true)
{	return Var;	}

#endif // cnLibrary_CPPFEATURE_RVALUE_REFERENCES < 200610L

//---------------------------------------------------------------------------

//---------------------------------------------------------------------------

template<class TDest,class TSrc>
inline bool TryCastTo(TDest &Dest,const TSrc &Src)noexcept(cnLib_NOEXCEPTEXPR((cnLib_THelper::Var_TH::TryCast<TIsConvertible<TSrc,TDest>::Value>::Call(Dest,Src))))
{	return cnLib_THelper::Var_TH::TryCast<TIsConvertible<TSrc,TDest>::Value>::Call(Dest,Src);	}


template<class TDest,class TSrc>
inline TDest TryCast(const TSrc &Src)noexcept(cnLib_NOEXCEPTEXPR((cnLib_THelper::Var_TH::TryCast<TIsConvertible<TSrc,TDest>::Value>::template CastRet<TDest,TSrc>(Src))))
{	return cnLib_THelper::Var_TH::TryCast<TIsConvertible<TSrc,TDest>::Value>::template CastRet<TDest,TSrc>(Src);	}

template<class TDest,class TSrc>
inline TDest TryCast(const TSrc &Src,typename TTypeDef<TDest>::Type const &Default)noexcept(cnLib_NOEXCEPTEXPR((cnLib_THelper::Var_TH::TryCast<TIsConvertible<TSrc,TDest>::Value>::template CastRetDef<TDest,TSrc>(Src,Default))))
{	return cnLib_THelper::Var_TH::TryCast<TIsConvertible<TSrc,TDest>::Value>::template CastRetDef<TDest,TSrc>(Src,Default);	}

//---------------------------------------------------------------------------
}	// namespace cnVar
//---------------------------------------------------------------------------
}	// namespace cnLibrary
//---------------------------------------------------------------------------
namespace cnLib_THelper{
//---------------------------------------------------------------------------
namespace Var_TH{
//---------------------------------------------------------------------------
namespace IntegerConversion{
//---------------------------------------------------------------------------

cnVar::TIntegerOfSize<sizeof(long double),true>::Type		TestMatch(long double);
cnVar::TIntegerOfSize<sizeof(double),true>::Type			TestMatch(double);
cnVar::TIntegerOfSize<sizeof(float),true>::Type			TestMatch(float);

unsigned long long	TestMatch(unsigned long long);
signed long long	TestMatch(signed long long);
unsigned long		TestMatch(unsigned long);
signed long			TestMatch(signed long);
unsigned int		TestMatch(unsigned int);
signed int			TestMatch(signed int);
unsigned short		TestMatch(unsigned short);
signed short		TestMatch(signed short);
unsigned char		TestMatch(unsigned char);
signed char			TestMatch(signed char);

uInt8	TestMatch(uInt8);
sInt8	TestMatch(sInt8);
uInt16	TestMatch(uInt16);
sInt16	TestMatch(sInt16);
uInt32	TestMatch(uInt32);
sInt32	TestMatch(sInt32);
uInt64	TestMatch(uInt64);
sInt64	TestMatch(sInt64);

sInt32	TestMatch(Float32);
sInt64	TestMatch(Float64);


template<class T>
struct MatchType_Expression
{
	template<int=sizeof(TestMatch(cnVar::DeclVar<T>()))>
	struct Test
	{
		typedef void Type;
	};

};


template<class T,class TCondition=void>
struct MatchTypeDef
	: cnVar::TTypeDef<void>{};

template<class T>
struct MatchTypeDef<T,typename MatchType_Expression<T>::template Test<>::Type>
	: cnVar::TIntegerOfSize<sizeof(TestMatch(cnVar::DeclVar<T>())),cnVar::TIsSigned<typename cnVar::TRemoveReference<T>::Type>::Value>{};


template<class T,class TMatch>
struct Decl
{
	static cnLib_CONSTVAR ufInt8 MatchSize=sizeof(TMatch);
	static cnLib_CONSTVAR bool IsSigned=cnVar::TIsSigned<TMatch>::Value;
	static cnLib_CONSTVAR bool IsConvertible=true;
	static cnLib_CONSTVAR bool IsMatch=false;

	template<class TInt>
	static TInt Cast(T Value)noexcept(cnLib_NOEXCEPTEXPR(static_cast<TInt>(static_cast<TMatch>(Value))))
	{	return static_cast<TInt>(static_cast<TMatch>(Value));	}

	template<class TInt>
	static TInt rtCast(void *Value)noexcept(cnLib_NOEXCEPTEXPR(static_cast<TInt>(static_cast<TMatch>(*static_cast<T*>(Value)))))
	{	return static_cast<TInt>(static_cast<TMatch>(*static_cast<T*>(Value)));	}
};

template<class T>
struct Decl<T,T>
{
	static cnLib_CONSTVAR ufInt8 MatchSize=sizeof(T);
	static cnLib_CONSTVAR bool IsSigned=cnVar::TIsSigned<T>::Value;
	static cnLib_CONSTVAR bool IsConvertible=true;
	static cnLib_CONSTVAR bool IsMatch=true;

	template<class TInt>
	static TInt Cast(T Value)noexcept(cnLib_NOEXCEPTEXPR(static_cast<TInt>(Value)))
	{	return static_cast<TInt>(Value);	}

	template<class TInt>
	static TInt rtCast(void *Value)noexcept(cnLib_NOEXCEPTEXPR(static_cast<TInt>(*static_cast<T*>(Value))))
	{	return static_cast<TInt>(*static_cast<T*>(Value));	}
};



template<class T,class TDistinct>
struct DeclUnconvertible
{
	static cnLib_CONSTVAR ufInt8 MatchSize=0;
	static cnLib_CONSTVAR bool IsSigned=false;
	static cnLib_CONSTVAR bool IsConvertible=false;
	static cnLib_CONSTVAR bool IsMatch=false;

	template<class TInt>
	static TInt Cast(T)noexcept(true){
		return 0;
	}

	template<class TInt>
	static TInt rtCast(void*)noexcept(true){
		return 0;
	}
};

template<class T,class TDistinct>
struct DeclUnconvertible<T,TDistinct*>
{
	static cnLib_CONSTVAR ufInt8 MatchSize=sizeof(void*);
	static cnLib_CONSTVAR bool IsSigned=false;
	static cnLib_CONSTVAR bool IsConvertible=true;
	static cnLib_CONSTVAR bool IsMatch=false;

	template<class TInt>
	static TInt Cast(T Value)noexcept(cnLib_NOEXCEPTEXPR(static_cast<TInt>(reinterpret_cast<uIntn>(Value))))
	{	return static_cast<TInt>(reinterpret_cast<uIntn>(Value));	}

	template<class TInt>
	static TInt rtCast(void *Value)noexcept(cnLib_NOEXCEPTEXPR(static_cast<TInt>(*static_cast<const uIntn*>(Value))))
	{	return static_cast<TInt>(*static_cast<const uIntn*>(Value));	}
};



template<class T,bool Convertible>
struct DeclNoMatch
	: Decl<T,sInt64>{};

template<class T>
struct DeclNoMatch<T,false>
	: DeclUnconvertible<T,typename cnVar::TRemoveCV<T>::Type>{};

template<class T>
struct Decl<T,void>	// fail to find exact match
	: DeclNoMatch<T,cnVar::TIsConvertible<T&,sInt64>::Value>{};

//---------------------------------------------------------------------------
}	// namespace IntegerConversion
//---------------------------------------------------------------------------
namespace FloatConversion{
//---------------------------------------------------------------------------

template<uIntn Size>	struct FloatForIntegerSize{};

template<>	struct FloatForIntegerSize<1>	: cnVar::TTypeDef<Float32>{};
template<>	struct FloatForIntegerSize<2>	: cnVar::TTypeDef<Float32>{};
template<>	struct FloatForIntegerSize<4>	: cnVar::TTypeDef<Float64>{};
template<>	struct FloatForIntegerSize<8>	: cnVar::TTypeDef<Float64>{};
template<>	struct FloatForIntegerSize<16>	: cnVar::TTypeDef<Float64>{};

long double		TestMatch(long double);
double			TestMatch(double);
float			TestMatch(float);

FloatForIntegerSize<sizeof(unsigned long long)>::Type	TestMatch(unsigned long long);
FloatForIntegerSize<sizeof(signed long long)>::Type		TestMatch(signed long long);
FloatForIntegerSize<sizeof(unsigned long)>::Type		TestMatch(unsigned long);
FloatForIntegerSize<sizeof(signed long)>::Type			TestMatch(signed long);
FloatForIntegerSize<sizeof(unsigned int)>::Type			TestMatch(unsigned int);
FloatForIntegerSize<sizeof(signed int)>::Type			TestMatch(signed int);
FloatForIntegerSize<sizeof(unsigned short)>::Type		TestMatch(unsigned short);
FloatForIntegerSize<sizeof(signed short)>::Type			TestMatch(signed short);
FloatForIntegerSize<sizeof(unsigned char)>::Type		TestMatch(unsigned char);
FloatForIntegerSize<sizeof(signed char)>::Type			TestMatch(signed char);

FloatForIntegerSize<sizeof(uInt8)>::Type	TestMatch(uInt8);
FloatForIntegerSize<sizeof(sInt8)>::Type	TestMatch(sInt8);
FloatForIntegerSize<sizeof(uInt16)>::Type	TestMatch(uInt16);
FloatForIntegerSize<sizeof(sInt16)>::Type	TestMatch(sInt16);
FloatForIntegerSize<sizeof(uInt32)>::Type	TestMatch(uInt32);
FloatForIntegerSize<sizeof(sInt32)>::Type	TestMatch(sInt32);
FloatForIntegerSize<sizeof(uInt64)>::Type	TestMatch(uInt64);
FloatForIntegerSize<sizeof(sInt64)>::Type	TestMatch(sInt64);

Float32	TestMatch(Float32);
Float64	TestMatch(Float64);


template<class T>
struct MatchType_Expression
{
	template<int=sizeof(TestMatch(cnVar::DeclVar<T>()))>
	struct Test
	{
		typedef void Type;
	};

};


template<class T,class TCondition=void>
struct MatchTypeDef
	: cnVar::TTypeDef<void>{};

template<class T>
struct MatchTypeDef<T,typename MatchType_Expression<T>::template Test<>::Type>
	: cnVar::TFloatOfSize<sizeof(TestMatch(cnVar::DeclVar<T>()))>{};

template<class T,class TMatch>
struct Decl
{
	typedef TMatch tMatch;
	static cnLib_CONSTVAR ufInt8 MatchSize=sizeof(TMatch);
	static cnLib_CONSTVAR bool IsConvertible=true;
	static cnLib_CONSTVAR bool IsMatch=false;

	template<class TFloat>
	static TFloat Cast(T Value)noexcept(cnLib_NOEXCEPTEXPR(static_cast<TFloat>(static_cast<TMatch>(Value))))
	{	return static_cast<TFloat>(static_cast<TMatch>(Value));	}

	template<class TFloat>
	static TFloat rtCast(void *Value)noexcept(cnLib_NOEXCEPTEXPR(static_cast<TFloat>(static_cast<TMatch>(*static_cast<T*>(Value)))))
	{	return static_cast<TFloat>(static_cast<TMatch>(*static_cast<T*>(Value)));	}
};


template<class T>
struct Decl<T,T>
{
	typedef T tMatch;
	static cnLib_CONSTVAR ufInt8 MatchSize=sizeof(T);
	static cnLib_CONSTVAR bool IsConvertible=true;
	static cnLib_CONSTVAR bool IsMatch=true;

	template<class TFloat>
	static TFloat Cast(T Value)noexcept(cnLib_NOEXCEPTEXPR(static_cast<TFloat>(Value)))
	{	return static_cast<TFloat>(Value);	}

	template<class TFloat>
	static TFloat rtCast(void *Value)noexcept(cnLib_NOEXCEPTEXPR(static_cast<TFloat>(*static_cast<T*>(Value))))
	{	return static_cast<TFloat>(*static_cast<T*>(Value));	}
};




template<class T,class TDistinct>
struct DeclUnconvertible
{
	static cnLib_CONSTVAR ufInt8 MatchSize=0;
	static cnLib_CONSTVAR bool IsConvertible=false;
	static cnLib_CONSTVAR bool IsMatch=false;

	template<class TFloat>
	static TFloat Cast(T)noexcept(true){
		return 0.f;
	}

	template<class TFloat>
	static TFloat rtCast(void*)noexcept(true){
		return 0.f;
	}
};

template<class T,bool Convertible>
struct DeclNoMatch
	: Decl<T,Float64>{};

template<class T>
struct DeclNoMatch<T,false>
	: DeclUnconvertible<T,typename cnVar::TRemoveCV<T>::Type>{};

template<class T>
struct Decl<T,void>	// fail to find exact match
	: DeclNoMatch<T,cnVar::TIsConvertible<T&,sInt64>::Value>{};

//---------------------------------------------------------------------------
}	// namespace FloatConversion
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
struct TIntegerConversion
	: cnLib_THelper::Var_TH::IntegerConversion::Decl<T
		,typename cnLib_THelper::Var_TH::IntegerConversion::MatchTypeDef<T>::Type
	>{};


template<class T>
struct TFloatConversion
	: cnLib_THelper::Var_TH::FloatConversion::Decl<T
		,typename cnLib_THelper::Var_TH::FloatConversion::MatchTypeDef<T>::Type
	>{};

//---------------------------------------------------------------------------
}	// namespace cnVar
//---------------------------------------------------------------------------
}	// namespace cnLibrary
//---------------------------------------------------------------------------
#include <cnTK/TKMacrosCleanup.inc>
//---------------------------------------------------------------------------
#endif	/* cnLibrary_CPPFEATURELEVEL >= 1 */
/*-------------------------------------------------------------------------*/
#endif
