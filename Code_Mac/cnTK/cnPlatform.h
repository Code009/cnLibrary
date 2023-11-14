/*- Platform Declaration - Mac --------------------------------------------*/
/*            Program: Code009                                             */
/*      Creation Date: 2014-02-24                                          */
/*-------------------------------------------------------------------------*/
#pragma once
#include <TargetConditionals.h>
/*-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*/
#ifdef	__cplusplus

#if	!__has_feature(cxx_alignas) || !__has_feature(cxx_alignof)
// not supported : c++11 alignment (alignas,alignof)
#define	cnLibrary_CPPEXCLUDE_ALIGNMENT
#endif

#if	!__has_feature(cxx_default_function_template_args)
// not supported : c++11 Default template arguments for function templates
#define	cnLibrary_CPPEXCLUDE_FUNCTION_TEMPLATE_DEFALT_ARGUMENT
#endif

#if	!__has_feature(cxx_defaulted_functions)
// not supported : c++11 class member default implemetation
#define	cnLibrary_CPPEXCLUDE_CLASS_MEMBER_DEFAULT
#endif

#if	!__has_feature(cxx_deleted_functions)
// not supported : c++11 class member deleted
#define	cnLibrary_CPPEXCLUDE_CLASS_MEMBER_DELETE
#endif

#if !__has_feature(cxx_explicit_conversions)
// not supported : c++11 Explicit conversion operators
#define	cnLibrary_CPPEXCLUDE_EXPLICIT_CONVERSION
#endif

#if	!__has_feature(cxx_nullptr)
// not supported : c++11 nullptr
#define	cnLibrary_CPPEXCLUDE_NULLPTR
#endif

#if	!__has_feature(cxx_override_control)
// not supported : c++11 override
#define	cnLibrary_CPPEXCLUDE_VIRTUAL_OVERRIDE
#endif

#if	!__has_feature(cxx_unrestricted_unions)
// not supported : c++11 Unrestricted unions
#define	cnLibrary_CPPEXCLUDE_UNRESTRICTED_UNION
#endif

#if	!__has_feature(cxx_strong_enums)
// not supported : c++11 scoped enumeration
#define	cnLibrary_CPPEXCLUDE_ENUMCLASS
#endif

#ifndef	__cpp_static_assert				// static_assert	200410L	(C++11)
#define	cnLibrary_CPPEXCLUDE_STATIC_ASSERT
#endif

#ifndef	__cpp_decltype					// decltype	200707L	(C++11)
#define	cnLibrary_CPPEXCLUDE_AUTOTYPE
#endif

#ifndef	__cpp_rvalue_references			//Rvalue reference	200610L	(C++11)
#define	cnLibrary_CPPEXCLUDE_MOVE_SEMANTICS
#endif

#ifndef	__cpp_nsdmi						// Non-static data member initializers	200809L	(C++11)
#define	cnLibrary_CPPEXCLUDE_CLASS_NSDMI
#endif

#ifndef	__cpp_initializer_lists			// List-initialization and std::initializer_list	200806L	(C++11)
#define	cnLibrary_CPPEXCLUDE_UNIFORM_INITIALIZATION
#endif

#ifndef	__cpp_variadic_templates		// Variadic templates	200704L	(C++11)
#define	cnLibrary_CPPEXCLUDE_VARIADIC_TEMPLATE
#endif

#ifndef	__cpp_alias_templates			// Alias templates	200704L	(C++11)
#define	cnLibrary_CPPEXCLUDE_ALIAS_TEMPLATE
#endif

#ifndef	__cpp_delegating_constructors	// Delegating constructors	200604L	(C++11)
#define	cnLibrary_CPPEXCLUDE_DELEGATE_CONSTRUCTOR
#endif

#ifndef	__cpp_inheriting_constructors	// Inheriting constructors	200802L	(C++11)
#define	cnLibrary_CPPEXCLUDE_INHERIT_CONSTRUCTOR
#endif

#ifndef	__cpp_constexpr					// constexpr		200704L	(C++11)
#define	cnLibrary_CPPEXCLUDE_CONSTEXPR
#endif

#ifndef	__cpp_user_defined_literals		// User-defined literals	200809L	(C++11)
#define	cnLibrary_CPPEXCLUDE_USER_DEFINED_LITERALS
#endif

#ifndef	__cpp_ref_qualifiers			// ref-qualifiers	200710L	(C++11)
#define	cnLibrary_CPPEXCLUDE_CLASS_REF_QUALIFIER
#endif

#ifndef	__cpp_sized_deallocation		// Sized deallocation	201309L	(C++14)
#define	cnLibrary_CPPEXCLUDE_SIZED_DEALLOCATION
#endif

#ifndef	__cpp_variable_templates		// Variable templates	201304L	(C++14)
#define	cnLibrary_CPPEXCLUDE_TEMPLATE_VARIABLE
#endif

#ifndef	__cpp_inline_variables			// Inline variables	201606L	(C++17)
#define	cnLibrary_CPPEXCLUDE_INLINE_VARIABLE
#endif

#ifndef	__cpp_aligned_new				// Dynamic memory allocation for over-aligned data	201606L	(C++17)
#define	cnLibrary_CPPEXCLUDE_ALIGNED_ALLOCATION
#endif

#ifndef	__cpp_if_constexpr				//constexpr if	201606L	(C++17)
#define	cnLibrary_CPPEXCLUDE_CONSTEXPR_IF
#endif


#ifndef __cpp_coroutines
#define cnLibrary_CPPEXCLUDE_COROUTINES
#endif

#if !__has_builtin(__builtin_coro_resume)
#define cnLibrary_CPPEXCLUDE_COROUTINES
#endif
//#ifndef	__cpp_impl_coroutine			// Coroutines (compiler support)	201902L	(C++20)
//static_assert(false,"not supported : Coroutines");
//#define cnLibrary_CPPEXCLUDE_COROUTINES
//#endif

//__cpp_impl_three_way_comparison	Three-way comparison (compiler support)	201907L	(C++20)

//---------------------------------------------------------------------------
namespace cnLibrary{
//---------------------------------------------------------------------------

// integer

typedef unsigned char		uInt8;
typedef signed char			sInt8;
typedef unsigned short		uInt16;
typedef signed short		sInt16;
typedef unsigned int		uInt32;
typedef signed int			sInt32;
typedef unsigned long long	uInt64;
typedef signed long long	sInt64;
// word integer

#if	TARGET_CPU_X86

typedef unsigned int		uIntn;
typedef signed int			sIntn;
typedef void				uInt128;
typedef void				sInt128;

#endif	// TARGET_CPU_X86

#if TARGET_CPU_X86_64

typedef unsigned long long	uIntn;
typedef signed long long	sIntn;
typedef __uint128_t			uInt128;
typedef __int128_t			sInt128;

#endif // TARGET_CPU_X86_64

#if	TARGET_CPU_ARM	// arm 32

typedef unsigned int		uIntn;
typedef signed int			sIntn;
typedef void				uInt128;
typedef void				sInt128;

#endif	// TARGET_CPU_ARM

#if TARGET_CPU_ARM64

typedef unsigned long long	uIntn;
typedef signed long long	sIntn;
typedef __uint128_t			uInt128;
typedef __int128_t			sInt128;

#endif	// TARGET_CPU_ARM64

// fastest integer for size

typedef unsigned char			ufInt8;
typedef signed char				sfInt8;
typedef unsigned short			ufInt16;
typedef signed short			sfInt16;
typedef unsigned int			ufInt32;
typedef signed int				sfInt32;
typedef unsigned long long		ufInt64;
typedef signed long long		sfInt64;
typedef uInt128					ufInt128;
typedef sInt128					sfInt128;

// character

typedef char		uChar8;

#ifdef	__cpp_unicode_characters
typedef char16_t	uChar16;
typedef char32_t	uChar32;
#else
#error	"char16_t and char32_t not supported"
#endif

// float
typedef float		Float32;
typedef double		Float64;


static constexpr uIntn byteBitCount=8;
//---------------------------------------------------------------------------
}	// namespace cnLibrary
//---------------------------------------------------------------------------
#endif	// __cplsuplus
//---------------------------------------------------------------------------

#define	cnLib_INTERFACE
#define	cnLib_FUNC
#define	cnLib_DEPRECATED	[[deprecated]]

#define	cnLib_SWITCH_FALLTHROUGH	[[fallthrough]]
#define	cnLib_NOEXCEPT		noexcept


//---------------------------------------------------------------------------


