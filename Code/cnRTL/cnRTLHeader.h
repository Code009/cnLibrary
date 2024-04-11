#ifndef __cnLibrary_cnRTL_cnRTLHeader_H__
#define __cnLibrary_cnRTL_cnRTLHeader_H__

#ifdef __cplusplus

#include <cnRTL/cnRTLCore.h>
#include <cnSystem/cnSystem.h>

/*-------------------------------------------------------------------------*/

#define	cnLibrary_AVAILABLE_RTL

#if cnLibrary_CPPFEATURELEVEL<99
#undef	cnLibrary_AVAILABLE_RTL
#pragma message("cnRTL needs cnTK full features")
#endif

#ifdef cnLibrary_CPPEXCLUDE_VIRTUAL_OVERRIDE
#undef	cnLibrary_AVAILABLE_RTL
#pragma message("cnRTL needs c++11 virtual override to compile")
#endif

#ifdef cnLibrary_CPPEXCLUDE_CLASS_MEMBER_DELETE
#undef	cnLibrary_AVAILABLE_RTL
#pragma message("cnRTL needs c++11 deleted class member to compile")
#endif

#ifdef cnLibrary_CPPEXCLUDE_CLASS_MEMBER_DEFAULT
#undef	cnLibrary_AVAILABLE_RTL
#pragma message("cnRTL needs c++11 defaulted class member to compile")
#endif // cnLibrary_CPPEXCLUDE_CLASS_MEMBER_DEFAULT


#if cnLibrary_CPPFEATURE_DECLTYPE < 200707L
#undef	cnLibrary_AVAILABLE_RTL
#pragma message("cnRTL needs c++11 decltype to compile")
#endif

#if cnLibrary_CPPFEATURE_ALIAS_TEMPLATES < 200704L
#undef	cnLibrary_AVAILABLE_RTL
#pragma message("cnRTL needs c++11 alias template to compile")
#endif

#if cnLibrary_CPPFEATURE_NSDMI < 200809L
#undef	cnLibrary_AVAILABLE_RTL
#pragma message("cnRTL needs c++11 Non-static data member initializers to compile")
#endif

#if cnLibrary_CPPFEATURE_INHERIT_CONSTRUCTORS < 200802L
#undef	cnLibrary_AVAILABLE_RTL
#pragma message("cnRTL needs c++11 Inheriting constructors to compile")
#endif

#if cnLibrary_CPPFEATURE_LAMBDAS < 200907L
#undef	cnLibrary_AVAILABLE_RTL
#pragma message("cnRTL needs c++11 lambdas to compile")
#endif // cnLibrary_CPPFEATURE_LAMBDAS < 200907L


#ifndef cnLibrary_AVAILABLE_RTL
#error	"cnRTL not compatible"
#endif // !cnLibrary_AVAILABLE_RTL

#undef	cnLibrary_AVAILABLE_RTL

//---------------------------------------------------------------------------
namespace cnLibrary{
//---------------------------------------------------------------------------
namespace cnRTL{
//---------------------------------------------------------------------------
// cnVar

using cnVar::Forward;
using cnVar::MoveCast;

using cnVar::cAnyVariable;

using cnVar::cAnyPtr;



using cnMemory::ArrayLength;

//---------------------------------------------------------------------------
}	// namespace cnRTL
//---------------------------------------------------------------------------
}   // namespace cnLibrary
//---------------------------------------------------------------------------
#endif  /* __cplusplus */
/*-------------------------------------------------------------------------*/
#endif