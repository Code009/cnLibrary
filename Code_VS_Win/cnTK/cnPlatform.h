/*- Platform Declaration - Windows ----------------------------------------*/
/*         Developer : Code009                                             */
/*         Create on : 2014-02-24                                          */
/*-------------------------------------------------------------------------*/
#pragma once

/*-------------------------------------------------------------------------*/
#ifdef	__cplusplus

// C++ feature macro

//	_MSC_VER : 1920			Visual Studio 2019 RTW (16.0)	1920
#if _MSC_VER >= 1920

//	use C++20 Feature-test macros from Visual C++ 2019 (16.0)
//	Feature-test macros won't be disabled for limited language standard

#include <cnTK/CPPFeatureCheck.h>

// _MSC_VER >= 1920
#else
// _MSC_VER < 1920

//	_MSC_VER : 1200			Visual Studio 6.0				
//	_MSC_VER : 1300			Visual Studio .NET 2002	(7.0)
//	_MSC_VER : 1310			Visual Studio .NET 2003	(7.1)


//	_MSC_VER : 1400			Visual C++ 2005	(8.0)	Toolset v80
// c++11(no feature test macro) override
#if _MSC_VER >= 1400

#if _CPPUNWIND
#define	cnLibrary_CPPFEATURE_EXCEPTIONS		199711L
#endif	// _CPPUNWIND


// _MSC_VER >= 1400
#else 
// _MSC_VER < 1400

// not supported : c++11 override
#define	cnLibrary_CPPEXCLUDE_VIRTUAL_OVERRIDE

#endif	// _MSC_VER < 1400

//	_MSC_VER : 1500			Visual C++ 2008	(9.0)	Toolset v90
#if _MSC_VER >= 1500

#endif	// _MSC_VER >= 1500

//	_MSC_VER : 1600			Visual C++ 2010	(10.0)	Toolset v100
// c++11(no feature test macro) nullptr
// c++11 lambda (partial : callable object only)
#if _MSC_VER >= 1600

// c++11 rvalue reference
#define	cnLibrary_CPPFEATURE_RVALUE_REFERENCES	200610L		// __cpp_rvalue_references

// c++11 static_assert
#define	cnLibrary_CPPFEATURE_STATIC_ASSERT		200410L		// __cpp_static_assert

// c++11 auto, decltype, tailing return type
#define	cnLibrary_CPPFEATURE_DECLTYPE			200707L		// __cpp_decltype
// _MSC_VER >= 1600
#else
// _MSC_VER < 1600

// not supported : c++11 nullptr
#define	cnLibrary_CPPEXCLUDE_NULLPTR

#endif	// _MSC_VER < 1600


//	_MSC_VER : 1700			Visual C++ 2012	(11.0)	Toolset v110
// c++11(no feature test macro) scoped enumeration
// c++11 range based for
// c++11 lambda ( add support for stateless lambda )
#if _MSC_VER >= 1700

#define	cnLibrary_CPPFEATURE_LAMBDAS	200907L		// __cpp_lambdas

// _MSC_VER >= 1700
#else
// _MSC_VER < 1700

// not supported : c++11 scoped enumeration
#define	cnLibrary_CPPEXCLUDE_ENUMCLASS


#endif	// _MSC_VER < 1700


//	_MSC_VER : 1800			Visual C++ 2013	(12.0)	Toolset v120
// c++11 uniform initialization (bug)
// c++11 defaulted member function ( partial : move constructor and move operator not supported)
// c++11 Raw string literals
// c++11(no feature test macro) Explicit conversion operators
// c++11(no feature test macro) Default template arguments for function templates
// c++11(no feature test macro) class member deleted
#if _MSC_VER >= 1800

// c++11 Variadic templates
#define	cnLibrary_CPPFEATURE_VARIADIC_TEMPLATES		200704L		// __cpp_variadic_templates

// c++11 alias template
#define	cnLibrary_CPPFEATURE_ALIAS_TEMPLATES		200704L		// __cpp_alias_templates

// c++11 Delegating constructor
#define	cnLibrary_CPPFEATURE_DELEGATE_CONSTRUCTORS	200604L		// __cpp_delegating_constructors

// c++11 non-static class member initializers
#define	cnLibrary_CPPFEATURE_NSDMI					200809L		// __cpp_nsdmi

// _MSC_VER >= 1800
#else
// _MSC_VER < 1800

// not supported : c++11 Default template arguments for function templates
#define	cnLibrary_CPPEXCLUDE_FUNCTION_TEMPLATE_DEFALT_ARGUMENT

// not supported : c++11 class member deleted
#define	cnLibrary_CPPEXCLUDE_CLASS_MEMBER_DELETE

// not supported : c++11 Explicit conversion operators
#define	cnLibrary_CPPEXCLUDE_EXPLICIT_CONVERSION

#endif	// _MSC_VER < 1800


//	_MSC_VER : 1900			Visual C++ 2015 (14.0)	Toolset v140
// c++11(no feature test macro) class member default implemetation
// c++11(no feature test macro) alignment (alignas,alignof)
// c++11(no feature test macro) noexcept
// c++11(no feature test macro) unstricted union
// c++11 thread local storage
// c++11 inline namespace
// c++11 Unicode string literals
// c++11 New character types (char16_t and char32_t)
// c++14 binary literals
// c++14 generic lambda
// c++14 lambda capture list
// c++14 return type deduction
// c++14 single quote (') as digit separator 
// c++17 New auto rules for direct-list-initialization 
// c++17 typename in a template template parameter
// c++17 Attributes for namespaces and enumerators
// c++17 u8 character literals
#if _MSC_VER >= 1900

// c++14 sized delete
#define	cnLibrary_CPPFEATURE_SIZED_DEALLOCATION		201309L		// __cpp_sized_deallocation

// c++11 User-defined literals
#define	cnLibrary_CPPFEATURE_USER_DEFINED_LITERALS	200809L		// __cpp_user_defined_literals

// c++11 constexpr
#define	cnLibrary_CPPFEATURE_CONSTEXPR				200704L		// __cpp_constexpr

// c++11 Uniform Initialization on template base class will cause compiler error
#define	cnLibrary_CPPFEATURE_INITIALIZER_LIST		200806L		// __cpp_initializer_lists

// c++11 ref-qualifiers
#define	cnLibrary_CPPFEATURE_REF_QUALIFIERS			200710L		// __cpp_ref_qualifiers

// c++11 Inheriting constructors
#define	cnLibrary_CPPFEATURE_INHERIT_CONSTRUCTORS	200802L		// __cpp_inheriting_constructors
#if _MSC_VER < 1910
#pragma warning(disable:4520)	// multiple default constructors specified
#endif

// c++11 New character types (char16_t and char32_t)
#define	cnLibrary_CPPFEATURE_UCHARS					200704L		// __cpp_unicode_characters

// c++11 attributes
#define	cnLibrary_CPPFEATURE_ATTRIBUTES				200809L		// __cpp_attributes

// _MSC_VER >= 1900
#else
// _MSC_VER < 1900

// not supported : c++11 alignment (alignas,alignof)
#define	cnLibrary_CPPEXCLUDE_ALIGNMENT

// not supported : c++11 default class member
#define	cnLibrary_CPPEXCLUDE_CLASS_MEMBER_DEFAULT

// not supported : c++11 noexcept
#define	cnLibrary_CPPEXCLUDE_NOEXCEPT

// not supported : c++11 unrestricted union
#define	cnLibrary_CPPEXCLUDE_UNRESTRICTED_UNION

#endif	// _MSC_VER < 1900

//	_MSC_FULL_VER : 190023506			Visual C++ 2015 (14.0)	Toolset v140 update 1

//	_MSC_FULL_VER : 190023918			Visual C++ 2015 (14.0)	Toolset v140 update 2
#if _MSC_FULL_VER >= 190023918

// c++14 template variable
#define	cnLibrary_CPPFEATURE_VARIABLE_TEMPLATES		201304L		// __cpp_variable_templates

#endif // _MSC_FULL_VER >= 190023918

//	_MSC_FULL_VER : 190024210			Visual C++ 2015 (14.0)	Toolset v140 update 3
// c++17 Nested namespace definition

//	_MSC_VER : 1910			Visual C++ 2017 (15.0)	Toolset v141
// c++17 static_assert without message
// c++17 Differing begin and end types in range-based for
// c++17 [[fallthrough]]
// c++11(no feature test macro) static initialization of global variable:
//		constexpr objects : supported
//		const objects : not supported, (support for some optimalize options)

#if _MSC_VER < 1910

// not supported : global variable constant initialization
#define	cnLibrary_CPPEXCLUDE_CONSTEXPR_STATIC_INITIALIZATION

// not supported : decltype expressions evaluation when SFINAE
#define	cnLibrary_CPPEXCLUDE_SFINAE_DECLTYPE_EXPRESSION

#include <type_traits>
#include <cnTK/STLTypeTraits.h>

#endif	// _MSC_VER < 1910

//	_MSC_VER : 1911			Visual C++ 2017 (15.3)	Toolset v141
// c++17 __has_include in preprocessor conditionals 
// c++17 Direct-list-initialization of enumerations 
// c++17 constexpr lambda expressions 
// c++17 constexpr if-statements
// c++17 [[nodiscard]] attribute 
// c++17 [[maybe_unused]] attribute
// c++17 Hexadecimal floating-point literals
// c++17 Using attribute namespaces without repetition 
// c++17 Structured Bindings
// c++17 Ignore unknown attributes
// c++17 init-statements for if and switch
#if _MSC_VER >= 1911

// c++17 if constexpr
#define	cnLibrary_CPPFEATURE_IF_CONSTEXPR		201606L		// __cpp_if_constexpr

// _MSC_VER >= 1911
#else
// _MSC_VER < 1911
#pragma warning(disable:4510)	// constructor could not be generated
#pragma warning(disable:4610)	// struct '' can never be instantiated - user defined constructor required
#pragma warning(disable:4127)	// conditional expression is constant (consider using 'if constexpr' statement instead)

#endif	// _MSC_VER < 1911

//	_MSC_VER : 1912			Visual C++ 2017 (15.5)	Toolset v141
// c++17 Allow constant evaluation for all non-type template arguments 
// c++17 Fold Expressions
// c++17 Make exception specifications be part of the type system
// 
#if _MSC_VER >= 1912

// c++17 Dynamic memory allocation for over-aligned data
#define	cnLibrary_CPPFEATURE_ALIGNED_NEW		201606L		// __cpp_aligned_new

// c++17 Inline variables
#define	cnLibrary_CPPFEATURE_INLINE_VARIABLES	201606L		// __cpp_inline_variables

#define	cnLibrary_CPPFEATURE_NOEXCEPT_FUNC_TYPE	201510L		// __cpp_noexcept_function_type

#endif	// _MSC_VER >= 1912

//	_MSC_VER : 1913			Visual C++ 2017 (15.6)	Toolset v141
// c++17 Guaranteed copy elision
#if _MSC_VER >= 1913

#endif	// _MSC_VER >= 1913

//	_MSC_VER : 1914			Visual C++ 2017 (15.7)	Toolset v141
// c++11 fully support exp SFINAE
// c++17 Class template argument deduction
// c++17 Replacement of class objects containing reference members
// c++17 Pack expansions in using-declarations
#if _MSC_VER >= 1914

// c++17 Aggregate initialization of classes with base classes 
#define	cnLibrary_CPPFEATURE_AGGREGATE_BASE			201603L		// __cpp_aggregate_bases

// c++17 Non-type template parameters with auto type 
#define	cnLibrary_CPPFEATURE_TEMPLATE_NONTYPE_AUTO	201606L		// __cpp_nontype_template_parameter_auto

#endif	// _MSC_VER >= 1914

//	_MSC_VER : 1915			Visual C++ 2017 (15.8)	Toolset v141
#if _MSC_VER >= 1915

#endif	// _MSC_VER >= 1915

//	_MSC_VER : 1916			Visual C++ 2017 (15.9)	Toolset v141
#if _MSC_VER >= 1916

#endif	// _MSC_VER >= 1916


// not supported : size expressions evaluation when SFINAE
#define	cnLibrary_CPPEXCLUDE_SFINAE_SIZEOF_EXPRESSION

#endif	// _MSC_VER < 1920

#ifdef _MSVC_LANG
	
	// limited language version
	//	Feature-test macros won't be disabled for limited language standard

// c++ 20 : 202002L
#if _MSVC_LANG < 202002L

#undef	cnLibrary_CPPFEATURE_COROUTINE
#undef	cnLibrary_CPPFEATURE_CONSTINIT
#undef	cnLibrary_CPPFEATURE_CONCEPTS
#undef	cnLibrary_CPPFEATURE_THREE_WAY_COMPARISION

#endif	// _MSVC_LANG < 202002L

// c++ 17 : 201703L
#if _MSVC_LANG < 201703L

#pragma warning(disable:4127)	// conditional expression is constant (consider using 'if constexpr' statement instead)


// disabled : c++17 constexpr if
#undef	cnLibrary_CPPFEATURE_IF_CONSTEXPR

// disabled : c++17 Dynamic memory allocation for over-aligned data
#undef	cnLibrary_CPPFEATURE_ALIGNED_NEW

// disabled : c++17 Inline variables
#undef	cnLibrary_CPPFEATURE_INLINE_VARIABLES

#undef	cnLibrary_CPPFEATURE_NOEXCEPT_FUNC_TYPE

#endif	// _MSVC_LANG < 201703L

// c++ 14 : 201402L
#if _MSVC_LANG < 201402L

// disabled : c++14 sized delete
#undef	cnLibrary_CPPFEATURE_SIZED_DEALLOCATION

// disabled : c++14 template variable
#undef	cnLibrary_CPPFEATURE_VARIABLE_TEMPLATES

#endif // _MSVC_LANG < 201402L

#endif	// _MSVC_LANG

#if _MANAGED

#define	cnLibrary_CPPEXCLUDE_ALIGNMENT
//#define	cnLibrary_CPPEXCLUDE_NOEXCEPT

#ifdef cnLibrary_CPPFEATURE_EXCEPTIONS
#undef	cnLibrary_CPPFEATURE_EXCEPTIONS
#endif // cnLibrary_CPPFEATURE_EXCEPTIONS
//#ifdef cnLibrary_CPPFEATURE_NOEXCEPT_FUNC_TYPE
//#undef	cnLibrary_CPPFEATURE_NOEXCEPT_FUNC_TYPE
//#endif // cnLibrary_CPPFEATURE_NOEXCEPT_FUNC_TYPE


#endif // _MANAGED


#if _MSC_VER >= 1900 && _MSC_VER < 1920
// other flag

#if _RESUMABLE_FUNCTIONS_SUPPORTED

// c++20 coroutine ( experimental )
#define	cnLibrary_CPPFEATURE_COROUTINE		201902L		// __cpp_impl_coroutine

#endif // _RESUMABLE_FUNCTIONS_SUPPORTED

#endif

//---------------------------------------------------------------------------
namespace cnLibrary{
//---------------------------------------------------------------------------

// integer
//_M_X64 _M_IX86
#ifdef _M_AMD64

typedef unsigned __int64	uIntn;
typedef __int64				sIntn;

#else

typedef unsigned long		uIntn;
typedef long				sIntn;

#endif

static const uIntn ByteBitCount=8;

typedef size_t	tSize;

#ifndef cnLibrary_CPPEXCLUDE_NULLPTR
typedef std::nullptr_t tNullptr;
#endif


// fixed size integer

typedef unsigned long long		uInt64;
typedef signed long long		sInt64;
typedef unsigned long			uInt32;
typedef signed long				sInt32;
typedef unsigned short			uInt16;
typedef signed short			sInt16;
typedef unsigned char			uInt8;
typedef signed char				sInt8;

// fastest integer for size

typedef unsigned long long		ufInt64;
typedef signed long long		sfInt64;
typedef unsigned long			ufInt32;
typedef signed long				sfInt32;
typedef unsigned long			ufInt16;
typedef signed long				sfInt16;
typedef unsigned char			ufInt8;
typedef signed char				sfInt8;

	// 128bit integer not supported

union __declspec(align(16)) uInt128{
    unsigned __int64    u64[2];
};
union __declspec(align(16)) sInt128{
    __int64             i64[2];
    unsigned __int64    u64[2];
};
typedef uInt128	ufInt128;
typedef sInt128	sfInt128;



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

typedef wchar_t			uChar16;
typedef uInt32			uChar32;

#endif	// cnLibrary_CPPFEATURE_UCHARS < 200704L

// float
typedef float		Float32;
typedef double		Float64;
typedef void		Float128;

//---------------------------------------------------------------------------
}	// namespace cnLibrary
//---------------------------------------------------------------------------

#endif	// __cplusplus

// <=> not ready
#undef	cnLibrary_CPPFEATURE_THREE_WAY_COMPARISION

/*- attribute -------------------------------------------------------------*/

#define	cnLib_FUNC			__cdecl
#define	cnLib_INTERFACE		__declspec(novtable)
#define	cnLib_DEPRECATED	__declspec(deprecated)


#if cnLibrary_CPPFEATURE_ATTRIBUTES >= 200809L

#define	cnLib_CPPATTRIBUTE(__attribute__)	[[__attribute__]]

#else

#define	cnLib_CPPATTRIBUTE(__attribute__)

#endif // cnLibrary_CPPFEATURE_ATTRIBUTES >= 200809L

#if _MSC_VER >= 1910 && _MSVC_LANG >= 201703L

#define	cnLib_SWITCH_FALLTHROUGH	cnLib_CPPATTRIBUTE(fallthrough)

#else

#define	cnLib_SWITCH_FALLTHROUGH

#endif

#define	cnLib_MEMBER_OFFSET(_type_,_member_)	( (&static_cast<_type_*>(nullptr)->_member_-static_cast<decltype(&static_cast<_type_*>(nullptr)->_member_)>(nullptr)) * sizeof(static_cast<_type_*>(nullptr)->_member_) )


#ifdef	_DEBUG
#define	cnLib_DEBUG
#endif

#define	cnLib_MEXP(...)	__VA_ARGS__

//---------------------------------------------------------------------------
#define cnLib_TO_STR(_arg_)  #_arg_
#define cnLib_MACRO_TO_STR(...)  cnLib_TO_STR(__VA_ARGS__)

#define cnLib_FILE_LINE	__FILE__ "(" cnLib_MACRO_TO_STR(__LINE__) ")"

#define	cnLib_ASSUME(_e_)	__assume(_e_)

/*-------------------------------------------------------------------------*/
