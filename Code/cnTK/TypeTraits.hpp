/*- cnTK - Type Traits ----------------------------------------------------*/
/*         Developer : Code009                                             */
/*         Create on : 2018-08-14                                          */
/*-------------------------------------------------------------------------*/
#ifndef __cnLibrary_cnTK_TypeTraits_HPP__
#define	__cnLibrary_cnTK_TypeTraits_HPP__
/*-------------------------------------------------------------------------*/
#include <cnTK/Common.hpp>
/*-------------------------------------------------------------------------*/
#if	cnLibrary_CPPFEATURELEVEL >= 1
//---------------------------------------------------------------------------
#include <cnTK/TKMacrosDeclare.inc>

//---------------------------------------------------------------------------
namespace cnLibrary{
//---------------------------------------------------------------------------
namespace cnVar{
//---------------------------------------------------------------------------

template<class T>	struct TIsCPPType : cnVar::TConstantValueFalse{};

template<>	struct TIsCPPType<long double>			: cnVar::TConstantValueTrue{};
template<>	struct TIsCPPType<double>				: cnVar::TConstantValueTrue{};
template<>	struct TIsCPPType<float>				: cnVar::TConstantValueTrue{};

template<>	struct TIsCPPType<unsigned long long>	: cnVar::TConstantValueTrue{};
template<>	struct TIsCPPType<signed long long>		: cnVar::TConstantValueTrue{};
template<>	struct TIsCPPType<unsigned long>		: cnVar::TConstantValueTrue{};
template<>	struct TIsCPPType<signed long>			: cnVar::TConstantValueTrue{};
template<>	struct TIsCPPType<unsigned int>			: cnVar::TConstantValueTrue{};
template<>	struct TIsCPPType<signed int>			: cnVar::TConstantValueTrue{};
template<>	struct TIsCPPType<unsigned short>		: cnVar::TConstantValueTrue{};
template<>	struct TIsCPPType<signed short>			: cnVar::TConstantValueTrue{};
template<>	struct TIsCPPType<unsigned char>		: cnVar::TConstantValueTrue{};
template<>	struct TIsCPPType<signed char>			: cnVar::TConstantValueTrue{};

template<>	struct TIsCPPType<wchar_t>	: cnVar::TConstantValueTrue{};

#if cnLibrary_CPPFEATURE_UCHARS >= 200704L

template<>	struct TIsCPPType<char16_t>	: cnVar::TConstantValueTrue{};
template<>	struct TIsCPPType<char32_t>	: cnVar::TConstantValueTrue{};

#endif // cnLibrary_CPPFEATURE_UCHARS >= 200704L

#if cnLibrary_CPPFEATURE_UCHAR8 >= 201811L

template<>	struct TIsCPPType<char8_t>	: cnVar::TConstantValueTrue{};

#endif // cnLibrary_CPPFEATURE_UCHAR8 >= 201811L


template<class T>
struct TIsSigned
	: TConstantValueBool< (static_cast<T>(-1)<0) >{};
template<>
struct TIsSigned<bool>
	: TConstantValueBool< false >{};


#if cnLibrary_CPPFEATURE_VARIABLE_TEMPLATES >= 201304L

template<class T>
static cnLib_CONSTVAR bool IsSigned=TIsSigned<T>::Value;

template<uIntn Size,bool Signed>
static cnLib_CONSTVAR bool IntegerTypeExists=TIntegerTypeExists<Size,Signed>::Value;

#endif // cnLibrary_CPPFEATURE_VARIABLE_TEMPLATES >= 201304L


#if cnLibrary_CPPFEATURE_VARIADIC_TEMPLATES >= 200704L

template<class T,class TCompare,class...VT>	struct TIsSame					: TConstantValueFalse{};
template<class T>							struct TIsSame<T,T>				: TConstantValueTrue{};
template<class T,class...TCompare>			struct TIsSame<T,T,TCompare...>	: TIsSame<T,TCompare...>{};

template<class T,class TCompare,class...VT>	struct TIsDifferent						: TConstantValueTrue{};
template<class T>							struct TIsDifferent<T,T>				: TConstantValueFalse{};
template<class T,class...TCompare>			struct TIsDifferent<T,T,TCompare...>	: TIsDifferent<T,TCompare...>{};

// cnLibrary_CPPFEATURE_VARIADIC_TEMPLATES >= 200704L
#else
// cnLibrary_CPPFEATURE_VARIADIC_TEMPLATES < 200704L

template<class T,class TCompare,cnLib_VARIADIC_TEMPLATE_PARAMETER_OPT>
struct TIsSame
	: TConstantValueFalse{};
template<class T>
struct TIsSame<T,T,cnLib_VARIADIC_TEMPLATE_ARGUMENT_SPECIALIZATION>
	: TConstantValueTrue{};
template<class T,cnLib_VARIADIC_TEMPLATE_PARAMETER>
struct TIsSame<T,T,cnLib_VARIADIC_TEMPLATE_EXPAND>
	: TIsSame<T,cnLib_VARIADIC_TEMPLATE_EXPAND>{};

template<class T,class TCompare,cnLib_VARIADIC_TEMPLATE_PARAMETER_OPT>
struct TIsDifferent
	: TConstantValueTrue{};
template<class T>
struct TIsDifferent<T,T,cnLib_VARIADIC_TEMPLATE_ARGUMENT_SPECIALIZATION>
	: TConstantValueFalse{};
template<class T,cnLib_VARIADIC_TEMPLATE_PARAMETER>
struct TIsDifferent<T,T,cnLib_VARIADIC_TEMPLATE_EXPAND>
	: TIsDifferent<T,cnLib_VARIADIC_TEMPLATE_EXPAND>{};

#endif // cnLibrary_CPPFEATURE_VARIADIC_TEMPLATES < 200704L


#if cnLibrary_CPPFEATURE_VARIADIC_TEMPLATES >= 200704L

template<class TFind,class...VT>	struct TIndexOf;

template<class TFind>							struct TIndexOf<TFind>					: TConstantValueUIntn<0>{};
template<class TFind,class...VT>				struct TIndexOf<TFind,TFind,VT...>		: TConstantValueUIntn<0>{};
template<class TFind,class TNoMatch,class...VT>	struct TIndexOf<TFind,TNoMatch,VT...>	: TConstantValueUIntn<1+TIndexOf<TFind,VT...>::Value>{};

// cnLibrary_CPPFEATURE_VARIADIC_TEMPLATES >= 200704L
#else
// cnLibrary_CPPFEATURE_VARIADIC_TEMPLATES < 200704L

template<class TFind,cnLib_VARIADIC_TEMPLATE_PARAMETER_OPT_P2>
struct TIndexOf;

template<class TFind>
struct TIndexOf<TFind,cnLib_VARIADIC_TEMPLATE_ARGUMENT_SPECIALIZATION_P2>
	: TConstantValueUIntn<0>{};

template<class TFind,cnLib_VARIADIC_TEMPLATE_PARAMETER_P1>
struct TIndexOf<TFind,TFind,cnLib_VARIADIC_TEMPLATE_EXPAND_P1>
	: TConstantValueUIntn<0>{};

template<class TFind,class TNoMatch,cnLib_VARIADIC_TEMPLATE_PARAMETER_P1>
struct TIndexOf
	: TConstantValueUIntn<
		1+TIndexOf<TFind,cnLib_VARIADIC_TEMPLATE_EXPAND_P1>::Value
	>
{};

#endif // cnLibrary_CPPFEATURE_VARIADIC_TEMPLATES < 200704L

#if cnLibrary_CPPFEATURE_VARIADIC_TEMPLATES >= 200704L && cnLibrary_CPPFEATURE_VARIABLE_TEMPLATES >= 201304L
// IsSame
//	test if all types in variadic list are the same
template<class T1,class T2,class...VTCompare>
static cnLib_CONSTVAR bool IsSame=TIsSame<T1,T2,VTCompare...>::Value;

// IndexOf
//	find index of given type in type list
template<class TFind>
struct IndexOf
{
	template<class...T>
	static cnLib_CONSTVAR uIntn In=TIndexOf<TFind,T...>::Value;
};

#endif // cnLibrary_CPPFEATURE_VARIADIC_TEMPLATES >= 200704L && cnLibrary_CPPFEATURE_VARIABLE_TEMPLATES >= 201304L
//---------------------------------------------------------------------------

#if cnLibrary_CPPFEATURE_VARIADIC_TEMPLATES >= 200704L


template<uIntn Index,class T,T...Values>			struct TSelectValue;

template<uIntn Index,class T>						struct TSelectValue<Index,T>{};
template<uIntn Index,class T,T Value,T...Values>	struct TSelectValue<Index,T,Value,Values...>	: TSelectValue<Index-1,T,Values...>{};
template<class T,T Value,T...Values>				struct TSelectValue<0,T,Value,Values...>		: TConstantValue<T,Value>{};


#if cnLibrary_CPPFEATURE_VARIABLE_TEMPLATES >= 201304L

// Select
//	select type from variadic list
template<uIntn Index,class T,T...Values>
static cnLib_CONSTVAR T SelectValue=TSelectValue<Index,T,Values...>::Value;

#endif	// cnLibrary_CPPFEATURE_VARIABLE_TEMPLATES >= 201304L

#endif // cnLibrary_CPPFEATURE_VARIADIC_TEMPLATES >= 200704L

//---------------------------------------------------------------------------

#if cnLibrary_CPPFEATURE_VARIADIC_TEMPLATES >= 200704L

template<class...VT>
struct TTypePack
{
	template<uIntn TypeIndex>	struct tTypeByIndex : TSelect<TypeIndex,VT...>{};
	static cnLib_CONSTVAR uIntn Count=sizeof...(VT);

	template<class TExtend>	struct tExtend : TTypeDef< TTypePack<VT...,TExtend> >{};
	template<class TForType>	struct Declarator;
};

// cnLibrary_CPPFEATURE_VARIADIC_TEMPLATES >= 200704L
#else
// cnLibrary_CPPFEATURE_VARIADIC_TEMPLATES < 200704L

template<cnLib_VARIADIC_TEMPLATE_PARAMETER_OPT>
struct TTypePack
{
	template<uIntn TypeIndex>	struct tTypeByIndex : TSelect<TypeIndex,cnLib_VARIADIC_TEMPLATE_EXPAND>{};
	static cnLib_CONSTVAR uIntn Count=cnLib_VARIADIC_TEMPLATE_PARAMETER_COUNT;

	template<class TForType>	struct Declarator;
};

#endif	// cnLibrary_CPPFEATURE_VARIADIC_TEMPLATES < 200704L

//---------------------------------------------------------------------------
template<class T>
struct TTypeComponent
{
};

// storage type

template<class T>
struct TTypeComponent<const T>{
	typedef T tDistinct;
};

template<class T>
struct TTypeComponent<volatile T>{
	typedef T tDistinct;
};


template<class T>
struct TTypeComponent<const volatile T>{
	typedef T tDistinct;
};

// pointer

template<class T>
struct TTypeComponent<T*>
{
	typedef T tPointed;
};

// reference

template<class T>
struct TTypeComponent<T&>
{
	typedef T tReferenced;
};

template<class T>
struct TTypeComponent<T&&>
{
	typedef T tReferenced;
};

// array

template<class T,uIntn ArrayLength>
struct TTypeComponent<T[ArrayLength]>
{
	typedef T tElement;
	static cnLib_CONSTVAR uIntn Length=ArrayLength;
};

template<class T>
struct TTypeComponent<T[]>
{
	typedef T tElement;
	static cnLib_CONSTVAR uIntn Length=0;
};

// function

#if cnLibrary_CPPFEATURE_VARIADIC_TEMPLATES >= 200704L

template<class TRet,class...TArgs>
struct TTypeComponent<TRet (TArgs...)>
{
	typedef TRet tReturn;
	TTypePack<TArgs...> tArguments;
	static cnLib_CONSTVAR uIntn ArgumentCount=sizeof...(TArgs);
	static cnLib_CONSTVAR bool NoException=false;
};

template<class TRet,class...TArgs>
struct TTypeComponent<TRet (*)(TArgs...)>
	: TTypeComponent<TRet (TArgs...)>
{
};

#if cnLibrary_CPPFEATURE_NOEXCEPT_FUNC_TYPE >= 201510L

template<class TRet,class...TArgs>
struct TTypeComponent<TRet (TArgs...)noexcept>
	: TTypeComponent<TRet (TArgs...)>
{
	static cnLib_CONSTVAR bool NoException=true;
};


template<class TRet,class...TArgs>
struct TTypeComponent<TRet (*)(TArgs...)noexcept>
	: TTypeComponent<TRet (TArgs...)noexcept>
{
};

#endif // cnLibrary_CPPFEATURE_NOEXCEPT_FUNC_TYPE >= 201510L

// cnLibrary_CPPFEATURE_VARIADIC_TEMPLATES >= 200704L
#else
// cnLibrary_CPPFEATURE_VARIADIC_TEMPLATES < 200704L


template<class TRet,cnLib_VARIADIC_TEMPLATE_PARAMETER_OPT>
struct TTypeComponent<TRet (cnLib_VARIADIC_TEMPLATE_EXPAND)>
{
	typedef TRet tReturn;
	TTypePack<TArgs...> tArguments;
	static cnLib_CONSTVAR uIntn ArgumentCount=sizeof...(TArgs);
	static cnLib_CONSTVAR bool NoException=false;
};

template<class TRet,cnLib_VARIADIC_TEMPLATE_PARAMETER_OPT>
struct TTypeComponent<TRet (*)(cnLib_VARIADIC_TEMPLATE_EXPAND)>
	: TTypeComponent<TRet (cnLib_VARIADIC_TEMPLATE_EXPAND)>
{
};

#if cnLibrary_CPPFEATURE_NOEXCEPT_FUNC_TYPE >= 201510L

template<class TRet,cnLib_VARIADIC_TEMPLATE_PARAMETER_OPT>
struct TTypeComponent<TRet (cnLib_VARIADIC_TEMPLATE_EXPAND)noexcept>
	: TTypeComponent<TRet (cnLib_VARIADIC_TEMPLATE_EXPAND)>
{
	static cnLib_CONSTVAR bool NoException=true;
};


template<class TRet,cnLib_VARIADIC_TEMPLATE_PARAMETER_OPT>
struct TTypeComponent<TRet (*)(cnLib_VARIADIC_TEMPLATE_EXPAND)noexcept>
	: TTypeComponent<TRet (cnLib_VARIADIC_TEMPLATE_EXPAND)noexcept>
{
};

#endif // cnLibrary_CPPFEATURE_NOEXCEPT_FUNC_TYPE >= 201510L

#endif	// cnLibrary_CPPFEATURE_VARIADIC_TEMPLATES < 200704L

// Member Pointer

template<class TClass,class TMember>
struct TTypeComponent<TMember TClass::*>
{
	typedef TClass tClass;
	typedef TMember tMember;
};

// Member Function Pointer

#if cnLibrary_CPPFEATURE_VARIADIC_TEMPLATES >= 200704L

template<class TClass,class TRet,class...TArgs>
struct TTypeComponent<TRet (TClass::*)(TArgs...)>
	: TTypeComponent<TRet (*)(TArgs...)>
{
	typedef TClass tClass;
	typedef TClass tClassArgument;
};

template<class TClass,class TRet,class...TArgs>
struct TTypeComponent<TRet (TClass::*)(TArgs...)const>
	: TTypeComponent<TRet (*)(TArgs...)>
{
	typedef TClass tClass;
	typedef const TClass tClassArgument;
};

template<class TClass,class TRet,class...TArgs>
struct TTypeComponent<TRet (TClass::*)(TArgs...)volatile>
	: TTypeComponent<TRet (*)(TArgs...)>
{
	typedef TClass tClass;
	typedef volatile TClass tClassArgument;
};

template<class TClass,class TRet,class...TArgs>
struct TTypeComponent<TRet (TClass::*)(TArgs...)const volatile>
	: TTypeComponent<TRet (*)(TArgs...)>
{
	typedef TClass tClass;
	typedef const volatile TClass tClassArgument;
};

#if cnLibrary_CPPFEATURE_NOEXCEPT_FUNC_TYPE >= 201510L


template<class TClass,class TRet,class...TArgs>
struct TTypeComponent<TRet (TClass::*)(TArgs...)noexcept>
	: TTypeComponent<TRet (*)(TArgs...)noexcept>
{
	typedef TClass tClass;
	typedef TClass tClassArgument;
};

template<class TClass,class TRet,class...TArgs>
struct TTypeComponent<TRet (TClass::*)(TArgs...)const noexcept>
	: TTypeComponent<TRet (*)(TArgs...)noexcept>
{
	typedef TClass tClass;
	typedef const TClass tClassArgument;
};

template<class TClass,class TRet,class...TArgs>
struct TTypeComponent<TRet (TClass::*)(TArgs...)volatile noexcept>
	: TTypeComponent<TRet (*)(TArgs...)noexcept>
{
	typedef TClass tClass;
	typedef volatile TClass tClassArgument;
};

template<class TClass,class TRet,class...TArgs>
struct TTypeComponent<TRet (TClass::*)(TArgs...)const volatile noexcept>
	: TTypeComponent<TRet (*)(TArgs...)noexcept>
{
	typedef TClass tClass;
	typedef const volatile TClass tClassArgument;
};

#endif // cnLibrary_CPPFEATURE_NOEXCEPT_FUNC_TYPE >= 201510L

// cnLibrary_CPPFEATURE_VARIADIC_TEMPLATES >= 200704L
#else
// cnLibrary_CPPFEATURE_VARIADIC_TEMPLATES < 200704L


template<class TClass,class TRet,cnLib_VARIADIC_TEMPLATE_PARAMETER_OPT>
struct TTypeComponent<TRet (TClass::*)(cnLib_VARIADIC_TEMPLATE_EXPAND)>
	: TTypeComponent<TRet (*)(cnLib_VARIADIC_TEMPLATE_EXPAND)>
{
	typedef TClass tClass;
	typedef TClass tClassArgument;
};

template<class TClass,class TRet,cnLib_VARIADIC_TEMPLATE_PARAMETER_OPT>
struct TTypeComponent<TRet (TClass::*)(cnLib_VARIADIC_TEMPLATE_EXPAND)const>
	: TTypeComponent<TRet (*)(cnLib_VARIADIC_TEMPLATE_EXPAND)>
{
	typedef TClass tClass;
	typedef const TClass tClassArgument;
};

template<class TClass,class TRet,cnLib_VARIADIC_TEMPLATE_PARAMETER_OPT>
struct TTypeComponent<TRet (TClass::*)(cnLib_VARIADIC_TEMPLATE_EXPAND)volatile>
	: TTypeComponent<TRet (*)(cnLib_VARIADIC_TEMPLATE_EXPAND)>
{
	typedef TClass tClass;
	typedef volatile TClass tClassArgument;
};

template<class TClass,class TRet,cnLib_VARIADIC_TEMPLATE_PARAMETER_OPT>
struct TTypeComponent<TRet (TClass::*)(cnLib_VARIADIC_TEMPLATE_EXPAND)const volatile>
	: TTypeComponent<TRet (*)(cnLib_VARIADIC_TEMPLATE_EXPAND)>
{
	typedef TClass tClass;
	typedef const volatile TClass tClassArgument;
};

#if cnLibrary_CPPFEATURE_NOEXCEPT_FUNC_TYPE >= 201510L


template<class TClass,class TRet,cnLib_VARIADIC_TEMPLATE_PARAMETER_OPT>
struct TTypeComponent<TRet (TClass::*)(cnLib_VARIADIC_TEMPLATE_EXPAND)noexcept>
	: TTypeComponent<TRet (*)(cnLib_VARIADIC_TEMPLATE_EXPAND)noexcept>
{
	typedef TClass tClass;
	typedef TClass tClassArgument;
};

template<class TClass,class TRet,cnLib_VARIADIC_TEMPLATE_PARAMETER_OPT>
struct TTypeComponent<TRet (TClass::*)(cnLib_VARIADIC_TEMPLATE_EXPAND)const noexcept>
	: TTypeComponent<TRet (*)(cnLib_VARIADIC_TEMPLATE_EXPAND)noexcept>
{
	typedef TClass tClass;
	typedef const TClass tClassArgument;
};

template<class TClass,class TRet,cnLib_VARIADIC_TEMPLATE_PARAMETER_OPT>
struct TTypeComponent<TRet (TClass::*)(cnLib_VARIADIC_TEMPLATE_EXPAND)volatile noexcept>
	: TTypeComponent<TRet (*)(cnLib_VARIADIC_TEMPLATE_EXPAND)noexcept>
{
	typedef TClass tClass;
	typedef volatile TClass tClassArgument;
};

template<class TClass,class TRet,cnLib_VARIADIC_TEMPLATE_PARAMETER_OPT>
struct TTypeComponent<TRet (TClass::*)(cnLib_VARIADIC_TEMPLATE_EXPAND)const volatile noexcept>
	: TTypeComponent<TRet (*)(cnLib_VARIADIC_TEMPLATE_EXPAND)noexcept>
{
	typedef TClass tClass;
	typedef const volatile TClass tClassArgument;
};

#endif // cnLibrary_CPPFEATURE_NOEXCEPT_FUNC_TYPE >= 201510L

#endif	// cnLibrary_CPPFEATURE_VARIADIC_TEMPLATES < 200704L
//---------------------------------------------------------------------------
}	// namespace cnVar
//---------------------------------------------------------------------------
}	// namespace cnLibrary
//---------------------------------------------------------------------------
namespace cnLib_THelper{
namespace Var_TH{

#if !defined(cnLibrary_CPPEXCLUDE_SFINAE_DECLTYPE_EXPRESSION) && cnLibrary_CPPFEATURE_DECLTYPE >= 200707L

template<class TEnable,class T>	struct IsTypeDefined : cnVar::TConstantValueFalse{};
template<class T>
struct IsTypeDefined<decltype(UnusedParameter(cnVar::DeclVal<T>())),T>
	: cnVar::TConstantValueTrue{};

template<>	struct IsTypeDefined<void,void> : cnVar::TConstantValueTrue{};


// !defined(cnLibrary_CPPEXCLUDE_SFINAE_DECLTYPE_EXPRESSION) && cnLibrary_CPPFEATURE_DECLTYPE >= 200707L
#else
// defined(cnLibrary_CPPEXCLUDE_SFINAE_DECLTYPE_EXPRESSION) || cnLibrary_CPPFEATURE_DECLTYPE < 200707L

#ifndef cnLibrary_CPPEXCLUDE_SFINAE_SIZEOF_EXPRESSION

template<class TEnable,class T>	struct IsTypeDefined : cnVar::TConstantValueFalse{};
template<class T>
struct IsTypeDefined<typename cnVar::TTypeConditional<void,sizeof(T)>::Type,T>
	: cnVar::TConstantValueTrue{};

template<>	struct IsTypeDefined<void,void> : cnVar::TConstantValueTrue{};
#endif	// !cnLibrary_CPPEXCLUDE_SFINAE_SIZEOF_EXPRESSION

#endif // defined(cnLibrary_CPPEXCLUDE_SFINAE_DECLTYPE_EXPRESSION) || cnLibrary_CPPFEATURE_DECLTYPE < 200707L


template<class TEnable,class T>
struct IsClass
	: cnVar::TConstantValueFalse{};

template<class T>
struct IsClass<typename cnVar::TSelect<0,void,int T::*>::Type,T>
	: cnVar::TConstantValueTrue{};

}	// namespace Var_TH
}	// namespace cnLib_THelper
//---------------------------------------------------------------------------
namespace cnLibrary{
//---------------------------------------------------------------------------
namespace cnVar{
//---------------------------------------------------------------------------

#if ( !defined(cnLibrary_CPPEXCLUDE_SFINAE_DECLTYPE_EXPRESSION) && cnLibrary_CPPFEATURE_DECLTYPE >= 200707L) || ! !defined(cnLibrary_CPPEXCLUDE_SFINAE_SIZEOF_EXPRESSION)

template<class T>	struct TIsTypeDefined					: cnLib_THelper::Var_TH::IsTypeDefined<void,T>{};
template<class T>	struct TIsTypeDefined<const T>			: cnLib_THelper::Var_TH::IsTypeDefined<void,T>{};
template<class T>	struct TIsTypeDefined<volatile T>		: cnLib_THelper::Var_TH::IsTypeDefined<void,T>{};
template<class T>	struct TIsTypeDefined<const volatile T>	: cnLib_THelper::Var_TH::IsTypeDefined<void,T>{};

#if cnLibrary_CPPFEATURE_VARIABLE_TEMPLATES >= 201304L

// TIsTypeDefined
//	 test if type is defined, not incompleted type
template<class T>
static cnLib_CONSTVAR bool IsTypeDefined=TIsTypeDefined<T>::Value;

#endif // cnLibrary_CPPFEATURE_VARIABLE_TEMPLATES >= 201304L

#endif // ( !defined(cnLibrary_CPPEXCLUDE_SFINAE_DECLTYPE_EXPRESSION) && cnLibrary_CPPFEATURE_DECLTYPE >= 200707L) || ! !defined(cnLibrary_CPPEXCLUDE_SFINAE_SIZEOF_EXPRESSION)


// TIsClass
//	check if a type is use defined type. class or struct or union
template<class T>
struct TIsClass
	: TConstantValueBool<cnLib_THelper::Var_TH::IsClass<void,T>::Value>{};


#if cnLibrary_CPPFEATURE_VARIABLE_TEMPLATES >= 201304L

// IsClass
//	 test if type is user defined class
template<class T>
static cnLib_CONSTVAR bool IsClass=TIsClass<T>::Value;

#endif // cnLibrary_CPPFEATURE_VARIABLE_TEMPLATES >= 201304L

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
#ifndef cnLibrary_CPPDEFINED_TYPE_TRAITS

//---------------------------------------------------------------------------
namespace cnLib_THelper{
//---------------------------------------------------------------------------
namespace Var_TH{
//---------------------------------------------------------------------------

#if !defined(cnLibrary_CPPEXCLUDE_SFINAE_DECLTYPE_EXPRESSION) && cnLibrary_CPPFEATURE_DECLTYPE >= 200707L

// use decltype expression to implement type traits

template<class TEnable,class TClass,class TBase>
struct ClassIsInheritFrom
	: cnVar::TConstantValueFalse{};

template<class TClass,class TBase>
struct ClassIsInheritFrom<decltype(static_cast<void>(static_cast<TBase*>(cnVar::DeclVal<TClass*>()))),TClass,TBase>
	: cnVar::TConstantValueTrue{};



template<class TEnable,class T>
struct IsAbstract
	: cnVar::TConstantValueTrue{};

template<class T>
struct IsAbstract<decltype(UnusedParameter<T>(cnVar::DeclVal<T>())),T>
	: cnVar::TConstantValueFalse{};



template<class TEnable,class T>
struct IsNumerical
	: cnVar::TConstantValueFalse{};

template<class T>
struct IsNumerical<decltype(static_cast<void>(cnVar::DeclVal<T>()*1)),T>
	: cnVar::TConstantValueTrue{};


template<class TEnable,class T>
struct IsInteger
	: cnVar::TConstantValueFalse{};

template<class T>
struct IsInteger<decltype(static_cast<void>(cnVar::DeclVal<T>()&1)),T>
	: cnVar::TConstantValueTrue{};


template<class TT>	union IsTrivial_TU{
	int v;
	TT t; 
	~IsTrivial_TU()noexcept(true){}
};

template<class TEnable,class T>
struct IsTrivial
	: cnVar::TConstantValueFalse{};

template<class T>
struct IsTrivial<decltype(UnusedParameter(IsTrivial_TU<T>())),T>
	: cnVar::TConstantValueTrue{};


template<class TEnable,class TConvertFrom,class TConvertTo>
struct IsConvertible
	: cnVar::TConstantValueFalse{};

template<class TConvertFrom,class TConvertTo>
struct IsConvertible<decltype(static_cast<void>(static_cast<TConvertTo>(cnVar::DeclVal<TConvertFrom>())))
	,TConvertFrom,TConvertTo> : cnVar::TConstantValueTrue{};



#if cnLibrary_CPPFEATURE_VARIADIC_TEMPLATES >= 200704L

template<class TEnable,class T,class...TArgs>
struct IsConstructibleBy
	: cnVar::TConstantValueFalse{};


template<class T,class TArg0,class TArg1,class...TArgs>
struct IsConstructibleBy<decltype(T(cnVar::DeclVal<TArg0>(),cnVar::DeclVal<TArg1>(),cnVar::DeclVal<TArgs>()...))
	,T,TArg0,TArg1,TArgs...>	: cnVar::TConstantValueTrue{};

// cnLibrary_CPPFEATURE_VARIADIC_TEMPLATES >= 200704L
#else
// cnLibrary_CPPFEATURE_VARIADIC_TEMPLATES < 200704L

template<class TEnable,class T,class TArg>
struct IsConstructibleBy
	: cnVar::TConstantValueFalse{};


#endif	// cnLibrary_CPPFEATURE_VARIADIC_TEMPLATES < 200704L

template<class T,class TArg>
struct IsConstructibleBy<decltype(UnusedParameter<T>(cnVar::DeclVal<TArg>()))
	,T,TArg> : cnVar::TConstantValueTrue{};


template<class TEnable,class TDest,class TSrc>
struct IsAssignableFrom
	: cnVar::TConstantValueFalse{};

template<class TDest,class TSrc>
struct IsAssignableFrom<decltype(static_cast<void>(cnVar::DeclVal<TDest&>()=cnVar::DeclVal<TSrc>())),TDest,TSrc>
	: cnVar::TConstantValueTrue{};

template<class TEnable,class T>
struct IsDestructible
	: cnVar::TConstantValueFalse{};

template<class T>
struct IsDestructible<decltype(static_cast<void>(cnVar::DeclVal<T&>().~T())),T>
	: cnVar::TConstantValueTrue{};


template<class TEnable,class T>
struct IsDefaultConstructible
	: cnVar::TConstantValueFalse{};

template<class T>
struct IsDefaultConstructible<decltype(static_cast<void>(T())),T>
	: cnVar::TConstantValueTrue{};


template<class TEnable,class T>
struct IsCopyConstructible
	: cnVar::TConstantValueFalse{};

template<class T>
struct IsCopyConstructible<decltype(static_cast<void>(T(cnVar::DeclVal<const T&>()))),T>
	: cnVar::TConstantValueTrue{};


template<class TEnable,class T>
struct IsMoveConstructible
	: cnVar::TConstantValueFalse{};

#if cnLibrary_CPPFEATURE_RVALUE_REFERENCES >= 200610L
template<class T>
struct IsMoveConstructible<decltype(static_cast<void>(T(cnVar::DeclVal<T&&>()))),T>
	: cnVar::TConstantValueTrue{};
#endif	// cnLibrary_CPPFEATURE_RVALUE_REFERENCES >= 200610L


template<class TEnable,class T>
struct IsCopyAssignable
	: cnVar::TConstantValueFalse{};

template<class T>
struct IsCopyAssignable<decltype(static_cast<void>(cnVar::DeclVal<T&>()=cnVar::DeclVal<const T&>())),T>
	: cnVar::TConstantValueTrue{};


template<class TEnable,class T>
struct IsMoveAssignable
	: cnVar::TConstantValueFalse{};

#if cnLibrary_CPPFEATURE_RVALUE_REFERENCES >= 200610L
template<class T>
struct IsMoveAssignable<decltype(static_cast<void>(cnVar::DeclVal<T&>()=cnVar::DeclVal<T&&>())),T>
	: cnVar::TConstantValueTrue{};
#endif	// cnLibrary_CPPFEATURE_RVALUE_REFERENCES >= 200610L



// !defined(cnLibrary_CPPEXCLUDE_SFINAE_DECLTYPE_EXPRESSION) && cnLibrary_CPPFEATURE_DECLTYPE >= 200707L
#else
// defined(cnLibrary_CPPEXCLUDE_SFINAE_DECLTYPE_EXPRESSION) || cnLibrary_CPPFEATURE_DECLTYPE < 200707L

#ifndef cnLibrary_CPPEXCLUDE_SFINAE_SIZEOF_EXPRESSION

// use sizeof expression to implement type traits

template<class TEnable,class TClass,class TBase>
struct ClassIsInheritFrom
	: cnVar::TConstantValueFalse{};

template<class TClass,class TBase>
struct ClassIsInheritFrom<typename cnVar::TTypeConditional<void,sizeof(cnVar::DeclVal<TBase*&>()=cnVar::DeclVal<TClass*>())>::Type,TClass,TBase>
	: cnVar::TConstantValueTrue{};


template<class TEnable,class T>
struct IsAbstract
	: cnVar::TConstantValueTrue{};

template<class T>
struct IsAbstract<typename cnVar::TTypeConditional<void,sizeof(cnVar::DeclVal<T>())>::Type,T>
	: cnVar::TConstantValueFalse{};



template<class TEnable,class T>
struct IsNumerical
	: cnVar::TConstantValueFalse{};

template<class T>
struct IsNumerical<typename cnVar::TTypeConditional<void,sizeof(cnVar::DeclVal<T>()*1)>::Type,T>
	: cnVar::TConstantValueTrue{};


template<class TEnable,class T>
struct IsInteger
	: cnVar::TConstantValueFalse{};

template<class T>
struct IsInteger<typename cnVar::TTypeConditional<void,sizeof(cnVar::DeclVal<T>()&1)>::Type,T>
	: cnVar::TConstantValueTrue{};


template<class TT>	union IsTrivial_TU{
	int v;
	TT t; 
	~IsTrivial_TU()noexcept(true){}
};

template<class TEnable,class T>
struct IsTrivial
	: cnVar::TConstantValueFalse{};

template<class T>
struct IsTrivial<typename cnVar::TSelect<!sizeof(cnVar::DeclVal<IsTrivial_TU<T>&>()=IsTrivial_TU<T>()),void>::Type,T>
	: cnVar::TConstantValueTrue{};



template<class TEnable,class TConvertFrom,class TConvertTo>
struct IsConvertible
	: cnVar::TConstantValueFalse{};

template<class TConvertFrom,class TConvertTo>
struct IsConvertible<typename cnVar::TTypeConditional<void,sizeof(static_cast<TConvertTo>(cnVar::DeclVal<TConvertFrom>()))>::Type
	,TConvertFrom,TConvertTo> : cnVar::TConstantValueTrue{};


#if cnLibrary_CPPFEATURE_VARIADIC_TEMPLATES >= 200704L

template<class TEnable,class T,class...TArgs>
struct IsConstructibleBy
	: cnVar::TConstantValueFalse{};

template<class T,class TArg0,class TArg1,class...TArgs>
struct IsConstructibleBy<typename cnVar::TTypeConditional<void,sizeof(T(cnVar::DeclVal<TArg0>(),cnVar::DeclVal<TArg1>(),cnVar::DeclVal<TArgs>()...))>::Type
	,T,TArg0,TArg1,TArgs...>	: cnVar::TConstantValueTrue{};

// cnLibrary_CPPFEATURE_VARIADIC_TEMPLATES >= 200704L
#else
// cnLibrary_CPPFEATURE_VARIADIC_TEMPLATES < 200704L

template<class TEnable,class T,class TArg>
struct IsConstructibleBy
	: cnVar::TConstantValueFalse{};


#endif	// cnLibrary_CPPFEATURE_VARIADIC_TEMPLATES < 200704L

template<class T,class TArg>
struct IsConstructibleBy<typename cnVar::TTypeConditional<void,sizeof(static_cast<T>(cnVar::DeclVal<TArg>()))>::Type
	,T,TArg> : cnVar::TConstantValueTrue{};

template<class TEnable,class TDest,class TSrc>
struct IsAssignableFrom
	: cnVar::TConstantValueFalse{};

template<class TDest,class TSrc>
struct IsAssignableFrom<typename cnVar::TTypeConditional<void,sizeof(static_cast<void>(cnVar::DeclVal<TDest&>()=cnVar::DeclVal<TSrc>()),0)>::Type,TDest,TSrc>
	: cnVar::TConstantValueTrue{};



template<class TEnable,class T>
struct IsDestructible
	: cnVar::TConstantValueFalse{};

template<class T>
struct IsDestructible<typename cnVar::TTypeConditional<void,sizeof(cnVar::DeclVal<T&>().~T(),0)>::Type,T>
	: cnVar::TConstantValueTrue{};


template<class TEnable,class T>
struct IsDefaultConstructible
	: cnVar::TConstantValueFalse{};

template<class T>
struct IsDefaultConstructible<typename cnVar::TTypeConditional<void,sizeof(T())>::Type,T>
	: cnVar::TConstantValueTrue{};


template<class TEnable,class T>
struct IsCopyConstructible
	: cnVar::TConstantValueFalse{};

template<class T>
struct IsCopyConstructible<typename cnVar::TTypeConditional<void,sizeof(T(cnVar::DeclVal<const T&>()))>::Type,T>
	: cnVar::TConstantValueTrue{};


template<class TEnable,class T>
struct IsMoveConstructible
	: cnVar::TConstantValueFalse{};

#if cnLibrary_CPPFEATURE_RVALUE_REFERENCES >= 200610L
template<class T>
struct IsMoveConstructible<typename cnVar::TTypeConditional<void,sizeof(T(cnVar::DeclVal<T&&>()))>::Type,T>
	: cnVar::TConstantValueTrue{};
#endif	// cnLibrary_CPPFEATURE_RVALUE_REFERENCES >= 200610L


template<class TEnable,class T>
struct IsCopyAssignable
	: cnVar::TConstantValueFalse{};

template<class T>
struct IsCopyAssignable<typename cnVar::TTypeConditional<void,sizeof(static_cast<void>(cnVar::DeclVal<T&>()=cnVar::DeclVal<const T&>()),0)>::Type,T>
	: cnVar::TConstantValueTrue{};


template<class TEnable,class T>
struct IsMoveAssignable
	: cnVar::TConstantValueFalse{};

#if cnLibrary_CPPFEATURE_RVALUE_REFERENCES >= 200610L
template<class T>
struct IsMoveAssignable<typename cnVar::TTypeConditional<void,sizeof(static_cast<void>(cnVar::DeclVal<T&>()=cnVar::DeclVal<T&&>()),0)>::Type,T>
	: cnVar::TConstantValueTrue{};
#endif	// cnLibrary_CPPFEATURE_RVALUE_REFERENCES >= 200610L




#endif	// !cnLibrary_CPPEXCLUDE_SFINAE_SIZEOF_EXPRESSION

#endif // defined(cnLibrary_CPPEXCLUDE_SFINAE_DECLTYPE_EXPRESSION) || cnLibrary_CPPFEATURE_DECLTYPE < 200707L

//---------------------------------------------------------------------------
}	// namespace Var_TH
//---------------------------------------------------------------------------
}	// namespace cnLib_THelper
//---------------------------------------------------------------------------
namespace cnLibrary{
//---------------------------------------------------------------------------
namespace cnVar{
//---------------------------------------------------------------------------

#if ( !defined(cnLibrary_CPPEXCLUDE_SFINAE_DECLTYPE_EXPRESSION) && cnLibrary_CPPFEATURE_DECLTYPE >= 200707L) || ! !defined(cnLibrary_CPPEXCLUDE_SFINAE_SIZEOF_EXPRESSION)


template<class TClass,class TBase>
struct TClassIsInheritFrom
	: cnLib_THelper::Var_TH::ClassIsInheritFrom<typename TTypeRequireDefined<void,TClass>::Type,TClass,TBase>{};


template<class T>
struct TIsAbstract
	: cnLib_THelper::Var_TH::IsAbstract<typename TTypeRequireDefined<void,T>::Type,T>{};

template<class T>
struct TIsInteger
	: TConstantValueBool<!cnLib_THelper::Var_TH::IsClass<typename TTypeRequireDefined<void,T>::Type,T>::Value && cnLib_THelper::Var_TH::IsInteger<void,T>::Value>{};

template<class T>
struct TIsFloat
	: TConstantValueBool<!cnLib_THelper::Var_TH::IsClass<typename TTypeRequireDefined<void,T>::Type,T>::Value && cnLib_THelper::Var_TH::IsNumerical<void,T>::Value && !cnLib_THelper::Var_TH::IsInteger<void,T>::Value>{};


template<class T>
struct TIsTrivial
	: cnVar::TSelect<TIsAbstract<T>::Value
		, cnLib_THelper::Var_TH::IsTrivial<typename TTypeRequireDefined<void,T>::Type,T>
		, cnVar::TConstantValueFalse
	>::Type{};

template<>	struct TIsTrivial<void>					: cnVar::TConstantValueTrue{};
template<>	struct TIsTrivial<const void>			: cnVar::TConstantValueTrue{};
template<>	struct TIsTrivial<volatile void>		: cnVar::TConstantValueTrue{};
template<>	struct TIsTrivial<const volatile void>	: cnVar::TConstantValueTrue{};

template<class T>	struct TIsTrivial<T&>	: cnVar::TConstantValueTrue{};

#if cnLibrary_CPPFEATURE_RVALUE_REFERENCES >= 200610L
template<class T>	struct TIsTrivial<T&&>	: cnVar::TConstantValueTrue{};
#endif	// cnLibrary_CPPFEATURE_RVALUE_REFERENCES >= 200610L


template<class TConvertFrom,class TConvertTo>
struct TIsConvertible
	: cnLib_THelper::Var_TH::IsConvertible<typename TTypeRequireDefined<void,typename TTypeRequireDefined<TConvertFrom,TConvertTo>::Type>::Type
	,TConvertFrom,TConvertTo>{};



#if cnLibrary_CPPFEATURE_VARIADIC_TEMPLATES >= 200704L

template<class TDest,class...TSrc>
struct TIsConstructibleBy
	: cnLib_THelper::Var_TH::IsConstructibleBy<void,TDest,TSrc...>{};

// cnLibrary_CPPFEATURE_VARIADIC_TEMPLATES >= 200704L
#else
// cnLibrary_CPPFEATURE_VARIADIC_TEMPLATES < 200704L

template<class TDest,class TSrc>
struct TIsConstructibleBy
	: cnLib_THelper::Var_TH::IsConstructibleBy<void,TDest,TSrc>{};

#endif	// cnLibrary_CPPFEATURE_VARIADIC_TEMPLATES < 200704L

template<class TDest,class TSrc>
struct TIsAssignableFrom
	: cnLib_THelper::Var_TH::IsAssignableFrom<void,TDest,TSrc>{};


template<class T>
struct TIsDestructible
	: cnLib_THelper::Var_TH::IsDestructible<void,T>{};

template<class T>
struct TIsDefaultConstructible
	: cnLib_THelper::Var_TH::IsDefaultConstructible<void,T>{};

template<class T>
struct TIsCopyConstructible
	: cnLib_THelper::Var_TH::IsCopyConstructible<void,T>{};


template<class T>
struct TIsMoveConstructible
	: cnLib_THelper::Var_TH::IsMoveConstructible<void,T>{};




template<class T>
struct TIsCopyAssignable
	: cnLib_THelper::Var_TH::IsCopyAssignable<void,T>{};


template<class T>
struct TIsMoveAssignable
	: cnLib_THelper::Var_TH::IsMoveAssignable<void,T>{};


#endif	// ( !defined(cnLibrary_CPPEXCLUDE_SFINAE_DECLTYPE_EXPRESSION) && cnLibrary_CPPFEATURE_DECLTYPE >= 200707L) || ! !defined(cnLibrary_CPPEXCLUDE_SFINAE_SIZEOF_EXPRESSION)

//---------------------------------------------------------------------------
}	// namespace cnVar
//---------------------------------------------------------------------------
}	// namespace cnLibrary
//---------------------------------------------------------------------------
#endif // !cnLibrary_CPPDEFINED_TYPE_TRAITS
//---------------------------------------------------------------------------
namespace cnLibrary{
//---------------------------------------------------------------------------
namespace cnVar{

#if cnLibrary_CPPFEATURE_VARIABLE_TEMPLATES >= 201304L

template<class TClass,class TBase>
static cnLib_CONSTVAR bool ClassIsInheritFrom=TClassIsInheritFrom<TClass,TBase>::Value;

// IsInstantiable
//	 test if type is abstract
template<class T>
static cnLib_CONSTVAR bool IsAbstract=TIsAbstract<T>::Value;

// IsInteger
//	 test if type is trivial integer type
template<class T>
static cnLib_CONSTVAR bool IsInteger=TIsInteger<T>::Value;

// IsFloat
//	 test if type is trivial float type
template<class T>
static cnLib_CONSTVAR bool IsFloat=TIsFloat<T>::Value;

// IsTrivial
//	 test if type is trivial
template<class T>
static cnLib_CONSTVAR bool IsTrivial=TIsTrivial<T>::Value;

// IsConvertible
template<class TConvertFrom,class TConvertTo>
static cnLib_CONSTVAR bool IsConvertible=TIsConvertible<TConvertFrom,TConvertTo>::Value;



#if cnLibrary_CPPFEATURE_VARIADIC_TEMPLATES >= 200704L

template<class TDest,class...TSrc>
static cnLib_CONSTVAR bool IsConstructibleBy=TIsConstructibleBy<TDest,TSrc...>::Value;

#else

template<class TDest,class TSrc>
static cnLib_CONSTVAR bool IsConstuctibleBy=TIsConstuctibleBy<TDest,TSrc>::Value;

#endif	// cnLibrary_CPPFEATURE_VARIADIC_TEMPLATES >= 200704L

template<class TDest,class TSrc>
static cnLib_CONSTVAR bool IsAssignableFrom=TIsAssignableFrom<TDest,TSrc>::Value;


template<class T>
static cnLib_CONSTVAR bool IsDestructible=TIsDestructible<T>::Value;

template<class T>
static cnLib_CONSTVAR bool IsDefaultConstructible=TIsDefaultConstructible<T>::Value;

template<class T>
static cnLib_CONSTVAR bool IsCopyConstructible=TIsCopyConstructible<T>::Value;

template<class T>
static cnLib_CONSTVAR bool IsMoveConstructible=TIsMoveConstructible<T>::Value;


template<class T>
static cnLib_CONSTVAR bool IsCopyAssignable=TIsCopyAssignable<T>::Value;

template<class T>
static cnLib_CONSTVAR bool IsMoveAssignable=TIsMoveAssignable<T>::Value;



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

template<class TConvertFrom,class TConvertTo>
struct IsReinterpretable
	: cnVar::TConstantValueBool<sizeof(TConvertFrom)==sizeof(TConvertTo) && cnVar::TIsConvertible<typename cnVar::TRemoveReference<TConvertFrom>::Type*,typename cnVar::TRemoveReference<TConvertTo>::Type*>::Value>{};

template<class TConvertFrom,class TConvertTo>
struct IsReinterpretable<TConvertFrom*,TConvertTo*>
	: IsReinterpretable<TConvertFrom,TConvertTo>{};

template<class TConvertTo>		struct IsReinterpretable<void,TConvertTo>	: cnVar::TConstantValueFalse{};
template<class TConvertFrom>	struct IsReinterpretable<TConvertFrom,void>	: cnVar::TConstantValueTrue{};
template<>						struct IsReinterpretable<void,void>			: cnVar::TConstantValueTrue{};



template<class TEnable,class T>
struct DefaultConstructIsNoexcept
	: cnVar::TConstantValueFalse{};

template<class TEnable,class T>
struct DestructIsNoexcept
	: cnVar::TConstantValueFalse{};

template<class TEnable,class T>
struct CopyConstructIsNoexcept
	: cnVar::TConstantValueFalse{};

template<class TEnable,class T>
struct CopyAssignIsNoexcept
	: cnVar::TConstantValueFalse{};

template<class TEnable,class T>
struct MoveConstructIsNoexcept
	: cnVar::TConstantValueFalse{};

template<class TEnable,class T>
struct MoveAssignIsNoexcept
	: cnVar::TConstantValueFalse{};

template<>
struct DefaultConstructIsNoexcept<void,void>
	: cnVar::TConstantValueFalse{};

template<>
struct DestructIsNoexcept<void,void>
	: cnVar::TConstantValueFalse{};

template<>
struct CopyConstructIsNoexcept<void,void>
	: cnVar::TConstantValueFalse{};

template<>
struct CopyAssignIsNoexcept<void,void>
	: cnVar::TConstantValueFalse{};

template<>
struct MoveConstructIsNoexcept<void,void>
	: cnVar::TConstantValueFalse{};

template<>
struct MoveAssignIsNoexcept<void,void>
	: cnVar::TConstantValueFalse{};


#ifndef cnLibrary_CPPEXCLUDE_NOEXCEPT

template<class T>
struct DefaultConstructIsNoexcept<typename cnVar::TTypeConditional<void,cnVar::TIsDefaultConstructible<T>::Value>::Type,T>
	: cnVar::TConstantValueBool<noexcept(typename cnVar::TRemoveVolatile<T>::Type())>{};

template<class T>
struct DestructIsNoexcept<typename cnVar::TTypeConditional<void,cnVar::TIsDestructible<T>::Value>::Type,T>
	: cnVar::TConstantValueBool<noexcept(cnVar::DeclVal<T&>().~T())>{};
template<class T>
struct CopyConstructIsNoexcept<typename cnVar::TTypeConditional<void,cnVar::TIsCopyConstructible<T>::Value>::Type,T>
	: cnVar::TConstantValueBool<noexcept(typename cnVar::TRemoveVolatile<T>::Type(cnVar::DeclVal<const T&>()))>{};
template<class T>
struct CopyAssignIsNoexcept<typename cnVar::TTypeConditional<void,cnVar::TIsCopyAssignable<T>::Value>::Type,T>
	: cnVar::TConstantValueBool<noexcept(cnVar::DeclVal<T&>()=cnVar::DeclVal<const T&>())>{};

#if cnLibrary_CPPFEATURE_RVALUE_REFERENCES >= 200610L
template<class T>
struct MoveConstructIsNoexcept<typename cnVar::TTypeConditional<void,cnVar::TIsMoveConstructible<T>::Value>::Type,T>
	: cnVar::TConstantValueBool<noexcept(typename cnVar::TRemoveVolatile<T>::Type(cnVar::DeclVal<T&&>()))>{};
template<class T>
struct MoveAssignIsNoexcept<typename cnVar::TTypeConditional<void,cnVar::TIsMoveAssignable<T>::Value>::Type,T>
	: cnVar::TConstantValueBool<noexcept(cnVar::DeclVal<T&>()=cnVar::DeclVal<T&&>())>{};
#endif	// cnLibrary_CPPFEATURE_RVALUE_REFERENCES

#endif	// !cnLibrary_CPPEXCLUDE_NOEXCEPT



//---------------------------------------------------------------------------
}	// namespace Var_TH
//---------------------------------------------------------------------------
}	// namespace cnLib_THelper
//---------------------------------------------------------------------------
namespace cnLibrary{
//---------------------------------------------------------------------------
namespace cnVar{
//---------------------------------------------------------------------------


template<class TConvertFrom,class TConvertTo>
struct TIsReinterpretable
	: cnLib_THelper::Var_TH::IsReinterpretable<typename TRemoveCV<typename TTypeRequireDefined<TConvertFrom>::Type>::Type
		,typename TRemoveCV<typename TTypeRequireDefined<TConvertTo>::Type>::Type>{};



template<class T>
struct TIsDefaultConstructNoexcept
	: cnLib_THelper::Var_TH::DefaultConstructIsNoexcept<void,T>{};

template<class T>
struct TIsDestructNoexcept
	: cnLib_THelper::Var_TH::DestructIsNoexcept<void,T>{};

template<class T>
struct TIsCopyConstructNoexcept
	: cnLib_THelper::Var_TH::CopyConstructIsNoexcept<void,T>{};

template<class T>
struct TIsCopyAssignNoexcept
	: cnLib_THelper::Var_TH::CopyAssignIsNoexcept<void,T>{};

template<class T>
struct TIsMoveConstructNoexcept
	: cnLib_THelper::Var_TH::MoveConstructIsNoexcept<void,T>{};
template<class T>
struct TIsMoveAssignNoexcept
	: cnLib_THelper::Var_TH::MoveAssignIsNoexcept<void,T>{};


#if cnLibrary_CPPFEATURE_VARIABLE_TEMPLATES >= 201304L

// IsReinterpretable
template<class TConvertFrom,class TConvertTo>
static cnLib_CONSTVAR bool IsReinterpretable=TIsReinterpretable<TConvertFrom,TConvertTo>::Value;


template<class T>
static cnLib_CONSTVAR bool IsDefaultConstructNoexcept=TIsDefaultConstructNoexcept<T>::Value;

template<class T>
static cnLib_CONSTVAR bool IsDestructNoexcept=TIsDestructNoexcept<T>::Value;

template<class T>
static cnLib_CONSTVAR bool IsCopyConstructNoexcept=TIsCopyConstructNoexcept<T>::Value;

template<class T>
static cnLib_CONSTVAR bool IsCopyAssignNoexcept=TIsCopyAssignNoexcept<T>::Value;

template<class T>
static cnLib_CONSTVAR bool IsMoveConstructNoexcept=TIsMoveConstructNoexcept<T>::Value;

template<class T>
static cnLib_CONSTVAR bool IsMoveAssignNoexcept=TIsMoveAssignNoexcept<T>::Value;

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
		noexcept(noexcept(Dest=static_cast<TDest>(Src)))
	{	Dest=static_cast<TDest>(Src);	return true;	}

	template<class TDest,class TSrc>
	static TDest CastRet(const TSrc &Src)
		noexcept(noexcept(TDest(static_cast<TDest>(Src))))
	{	return static_cast<TDest>(Src);	}
	template<class TDest,class TSrc>
	static TDest CastRetDef(const TSrc &Src,const TDest &)
		noexcept(noexcept(TDest(static_cast<TDest>(Src))))
	{	return static_cast<TDest>(Src);	}
};
template<>
struct TryCast<false>
{
	template<class TDest,class TSrc>
	static bool Call(TDest&,const TSrc&)noexcept(true){	return false;	}


	template<class TDest,class TSrc>
	static TDest CastRet(const TSrc &)
		noexcept(noexcept(TDest()))
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

// forward argument
template<class T>
inline T&& Forward(typename TRemoveReference<T>::Type &Arg)noexcept(true){
	return static_cast<T&&>(Arg);
}
template<class T>
inline T&& Forward(typename TRemoveReference<T>::Type &&Arg)noexcept(true){
	return static_cast<T&&>(Arg);
}
// explicit move
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
inline bool TryCastTo(TDest &Dest,const TSrc &Src)noexcept(noexcept((cnLib_THelper::Var_TH::TryCast<TIsConvertible<TSrc,TDest>::Value>::Call(Dest,Src))))
{	return cnLib_THelper::Var_TH::TryCast<TIsConvertible<const TSrc,TDest>::Value>::Call(Dest,Src);	}


template<class TDest,class TSrc>
inline TDest TryCast(const TSrc &Src)noexcept(noexcept((cnLib_THelper::Var_TH::TryCast<TIsConvertible<TSrc,TDest>::Value>::template CastRet<TDest,TSrc>(Src))))
{	return cnLib_THelper::Var_TH::TryCast<TIsConvertible<const TSrc,TDest>::Value>::template CastRet<TDest,TSrc>(Src);	}

template<class TDest,class TSrc>
inline TDest TryCast(const TSrc &Src,typename TTypeDef<TDest>::Type const &Default)noexcept(noexcept((cnLib_THelper::Var_TH::TryCast<TIsConvertible<TSrc,TDest>::Value>::template CastRetDef<TDest,TSrc>(Src,Default))))
{	return cnLib_THelper::Var_TH::TryCast<TIsConvertible<const TSrc,TDest>::Value>::template CastRetDef<TDest,TSrc>(Src,Default);	}



//---------------------------------------------------------------------------

#if cnLibrary_CPPFEATURE_VARIADIC_TEMPLATES >= 200704L


template<bool...Values>				struct TBooleanValuesAnd				: cnVar::TConstantValueFalse{};
template<bool v>					struct TBooleanValuesAnd<v>				: cnVar::TConstantValueBool<v>{};
template<bool v0,bool v1,bool...vv>	struct TBooleanValuesAnd<v0,v1,vv...>	: cnVar::TBooleanValuesAnd<v1,vv...>{};
template<bool v1,bool...vv>			struct TBooleanValuesAnd<false,v1,vv...>: cnVar::TConstantValueFalse{};

template<bool...Values>					struct TBooleanValuesOr					: cnVar::TConstantValueFalse{};
template<bool v>						struct TBooleanValuesOr<v>				: cnVar::TConstantValueBool<v>{};
template<bool v0,bool v1,bool...vv>		struct TBooleanValuesOr<v0,v1,vv...>	: cnVar::TConstantValueTrue{};
template<bool v1,bool...vv>				struct TBooleanValuesOr<false,v1,vv...>	: cnVar::TBooleanValuesOr<v1,vv...>{};


template<class TTypePack,uIntn Count,class TAppend>
struct TMakeTypePackRepeat
	: TMakeTypePackRepeat<typename TTypePack::template tExtend<TAppend>::Type,Count-1,TAppend>{};

template<class TValueSequence,class TAppend>
struct TMakeTypePackRepeat<TValueSequence,0,TAppend>
	: TTypeDef<TValueSequence>{};


template<class T,T...Values>
struct TValueSequence
{
	typedef T tValue;
	template<uIntn Index>	struct tAt : TSelectValue<Index,T,Values...>{};
	template<T Value>	struct tExtend : TTypeDef< TValueSequence<T,Values...,Value> >{};

	template<class TForType>	struct Declarator;
};


template<class TValueSequence,uIntn Count,typename TValueSequence::tValue Index=0,typename TValueSequence::tValue Increment=1>
struct TMakeIndexSequence
	: TMakeIndexSequence<typename TValueSequence::template tExtend<Index>::Type,Count-1,Index+Increment,Increment>{};

template<class TValueSequence,typename TValueSequence::tValue Index,typename TValueSequence::tValue Increment>
struct TMakeIndexSequence<TValueSequence,0,Index,Increment>
	: TTypeDef<TValueSequence>{};


template<class TValueSequence,typename TValueSequence::tValue...AddValues>
struct TMakeAccumulateSequence;

template<class TValueSequence,typename TValueSequence::tValue Value,typename TValueSequence::tValue...AddValues>
struct TMakeAccumulateSequence<TValueSequence,Value,AddValues...>
	: TMakeAccumulateSequence<typename TValueSequence::template tExtend<Value>::Type,AddValues...>{};

template<class TValueSequence>
struct TMakeAccumulateSequence<TValueSequence>
	: TTypeDef<TValueSequence>{};


template<class...TConstantArraies>
struct TMergeConstantArrayDef;

template<template<class T,T...> class TConstantClass,class T,T...Elements1,T...Elements2,class...TConstantArraies>
struct TMergeConstantArrayDef<TConstantClass<T,Elements1...>,TConstantClass<T,Elements2...>,TConstantArraies...>
	: TMergeConstantArrayDef<TConstantClass<T,Elements1...,Elements2...>,TConstantArraies...>
{
};

template<template<class T,T...> class TConstantClass,class T,T...Elements>
struct TMergeConstantArrayDef< TConstantClass<T,Elements...> >
	: TTypeDef< TConstantClass<T,Elements...> >{};

#endif // cnLibrary_CPPFEATURE_VARIADIC_TEMPLATES >= 200704L

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
