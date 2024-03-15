/*- cnTK - Common Template ------------------------------------------------*/
/*         Developer : Code009                                             */
/*         Create on : 2024-02-18                                          */
/*-------------------------------------------------------------------------*/
#ifndef __cnLibrary_cnRTL_CRTCoreTypeTraits_HPP__
#define	__cnLibrary_cnRTL_CRTCoreTypeTraits_HPP__
/*-------------------------------------------------------------------------*/

#include <cnRTL/CRTCoreCommon.h>

#if	__cplusplus
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
namespace cnLibrary{
//---------------------------------------------------------------------------
namespace cnVar{
//---------------------------------------------------------------------------

template<class TClass,class TBase>
struct TClassIsInheritFrom
	: cnVar::TConstantValueBool<std::is_base_of<TBase,TClass>::value>{};

template<class T>
struct TIsAbstract
	: cnVar::TConstantValueBool<!std::is_abstract<T>::value>{};

template<class T>
struct TIsInteger
	: TConstantValueBool<std::is_integral<T>::value>{};

template<class T>
struct TIsFloat
	: TConstantValueBool<std::is_floating_point<T>::value>{};


template<class T>
struct TIsTrivial
	: cnVar::TConstantValueBool<std::is_trivial<T>::value>{};

template<class TConvertFrom,class TConvertTo>
struct TIsConvertible
	: cnVar::TConstantValueBool<std::is_convertible<TConvertFrom,TConvertTo>::value>{};



#if cnLibrary_CPPFEATURE_VARIADIC_TEMPLATES >= 200704L

template<class TDest,class...TSrc>
struct TIsConstructibleBy
	: cnVar::TConstantValueBool<std::is_constructible<TDest,TSrc...>::value>{};

// cnLibrary_CPPFEATURE_VARIADIC_TEMPLATES >= 200704L
#else
// cnLibrary_CPPFEATURE_VARIADIC_TEMPLATES < 200704L

template<class TDest,class TSrc>
struct TIsConstructibleBy
	: cnVar::TConstantValueBool<std::is_constructible<TDest,TSrc>::value>{};

#endif	// cnLibrary_CPPFEATURE_VARIADIC_TEMPLATES < 200704L

template<class TDest,class TSrc>
struct TIsAssignableFrom
	: cnVar::TConstantValueBool<std::is_assignable<TDest,TSrc>::value>{};


template<class T>
struct TIsDestructible
	: cnVar::TConstantValueBool<std::is_destructible<T>::value>{};

template<class T>
struct TIsDefaultConstructible
	: cnVar::TConstantValueBool<std::is_default_constructible<T>::value>{};

template<class T>
struct TIsCopyConstructible
	: cnVar::TConstantValueBool<std::is_copy_constructible<T>::value>{};


template<class T>
struct TIsMoveConstructible
	: cnVar::TConstantValueBool<std::is_move_constructible<T>::value>{};


template<class T>
struct TIsCopyAssignable
	: cnVar::TConstantValueBool<std::is_copy_assignable<T>::value>{};


template<class T>
struct TIsMoveAssignable
	: cnVar::TConstantValueBool<std::is_move_assignable<T>::value>{};


//---------------------------------------------------------------------------
}	// namespace cnVar
//---------------------------------------------------------------------------
}	// namespace cnLibrary
//---------------------------------------------------------------------------
#endif	/* __cplusplus */
/*-------------------------------------------------------------------------*/
#endif
