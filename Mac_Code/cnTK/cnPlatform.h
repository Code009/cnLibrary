/*- Platform Declaration - Mac --------------------------------------------*/
/*            Program: Code009                                             */
/*      Creation Date: 2014-02-24                                          */
/*-------------------------------------------------------------------------*/
#pragma once
#include <TargetConditionals.h>
#include <stddef.h>
#include <assert.h>
/*-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*/
#ifdef	__cplusplus

#include <cnTK/CPPFeatureCheck.h>

#if	!__has_feature(cxx_nullptr)
// not supported : c++11 nullptr
#define	cnLibrary_CPPEXCLUDE_NULLPTR
#endif

#if	!__has_feature(cxx_override_control)
// not supported : c++11 override
#define	cnLibrary_CPPEXCLUDE_VIRTUAL_OVERRIDE
#endif

#if	!__has_feature(cxx_strong_enums)
// not supported : c++11 scoped enumeration
#define	cnLibrary_CPPEXCLUDE_ENUMCLASS
#endif

#if	!__has_feature(cxx_default_function_template_args)
// not supported : c++11 Default template arguments for function templates
#define	cnLibrary_CPPEXCLUDE_FUNCTION_TEMPLATE_DEFALT_ARGUMENT
#endif

#if	!__has_feature(cxx_deleted_functions)
// not supported : c++11 class member deleted
#define	cnLibrary_CPPEXCLUDE_CLASS_MEMBER_DELETE
#endif

#if !__has_feature(cxx_explicit_conversions)
// not supported : c++11 Explicit conversion operators
#define	cnLibrary_CPPEXCLUDE_EXPLICIT_CONVERSION
#endif

#if	!__has_feature(cxx_alignas) || !__has_feature(cxx_alignof)
// not supported : c++11 alignment (alignas,alignof)
#define	cnLibrary_CPPEXCLUDE_ALIGNMENT
#endif

#if	!__has_feature(cxx_defaulted_functions)
// not supported : c++11 class member default implemetation
#define	cnLibrary_CPPEXCLUDE_CLASS_MEMBER_DEFAULT
#endif

#if	!__has_feature(cxx_unrestricted_unions)
// not supported : c++11 Unrestricted unions
#define	cnLibrary_CPPEXCLUDE_UNRESTRICTED_UNION
#endif

//cnLibrary_CPPEXCLUDE_NOEXCEPT
//cnLibrary_CPPEXCLUDE_CONSTEXPR_STATIC_INITALIZATION


#if	!cnLibrary_CPPFEATURE_COROUTINE

#if __has_builtin(__builtin_coro_resume)
#define cnLibrary_CPPEXCLUDE_COROUTINES	201902L
#endif

#endif


//#ifndef	__cpp_impl_coroutine			// Coroutines (compiler support)	201902L	(C++20)
//static_assert(false,"not supported : Coroutines");
//#define cnLibrary_CPPEXCLUDE_COROUTINES
//#endif

//---------------------------------------------------------------------------
namespace cnLibrary{
//---------------------------------------------------------------------------

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

typedef size_t	tSize;

#ifndef cnLibrary_CPPEXCLUDE_NULLPTR
typedef std::nullptr_t tNullptr;
#endif


// integer

typedef unsigned char		uInt8;
typedef signed char			sInt8;
typedef unsigned short		uInt16;
typedef signed short		sInt16;
typedef unsigned int		uInt32;
typedef signed int			sInt32;
typedef unsigned long long	uInt64;
typedef signed long long	sInt64;

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

#if cnLibrary_CPPFEATURE_UCHAR8 >= 201811L
typedef	char8_t				uChar8;
#else	// cnLibrary_CPPFEATURE_UCHAR8 < 201811L
typedef	char				uChar8;
#endif	// cnLibrary_CPPFEATURE_UCHAR8

#if cnLibrary_CPPFEATURE_UCHARS >= 200704L

typedef char16_t			uChar16;
typedef char32_t			uChar32;

// cnLibrary_CPPFEATURE_UCHARS >= 200704L
#else
// cnLibrary_CPPFEATURE_UCHARS < 200704L

#error	"char16_t and char32_t not supported"

#endif	// cnLibrary_CPPFEATURE_UCHARS < 200704L


// float
typedef float			Float32;
typedef double			Float64;
typedef long double		Float128;


static constexpr uIntn ByteBitCount=8;
//---------------------------------------------------------------------------
}	// namespace cnLibrary
//---------------------------------------------------------------------------
#endif	// __cplsuplus
/*-------------------------------------------------------------------------*/
// <=> not ready
#undef	cnLibrary_CPPFEATURE_THREE_WAY_COMPARISION

#define	cnLib_FUNC
#define	cnLib_INTERFACE
#define	cnLib_DEPRECATED	[[deprecated]]

#define	cnLib_CPPATTRIBUTE(__attribute__)	[[__attribute__]]

#define	cnLib_SWITCH_FALLTHROUGH	[[fallthrough]]

#define cnLib_MEMBER_OFFSET(_type_,_member_)	offsetof(_type_,_member_)

/*-------------------------------------------------------------------------*/


#ifdef	DEBUG
#define	cnLib_DEBUG
#endif

#define	cnLib_MEXP(...)	__VA_ARGS__
/*-------------------------------------------------------------------------*/
#define cnLib_TO_STR(_arg_)  #_arg_
#define	cnLib_EXP_TO_STR(_arg_)	cnLib_TO_STR(_arg_)

#define cnLib_FILE_LINE	__FILE__ "(" cnLib_EXP_TO_STR(__LINE__) ")"

/*-------------------------------------------------------------------------*/
// assert
#ifdef	cnLib_DEBUG
#define	cnLib_ASSERT(_x_)	assert(_x_)
#else
#define	cnLib_ASSERT(_x_)	((void)(_x_))
#endif
/*-------------------------------------------------------------------------*/


//#define	cnLib_LOCAL	//__attribute__((visibility("hidden")))
