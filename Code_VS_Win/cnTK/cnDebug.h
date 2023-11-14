/*- Debug -----------------------------------------------------------------*/
/*         Developer : Code009                                             */
/*         Create on : 2015-11-20                                          */
/*-------------------------------------------------------------------------*/
#pragma once

#ifdef	_DEBUG
#define	WIN32_LEAN_AND_MEAN
#include <Windows.h>

#define	cnLib_DEBUG
#endif

/*-------------------------------------------------------------------------*/
#include <cnTK\cnPlatform.h>
#ifdef __cplusplus

//---------------------------------------------------------------------------
#define cnLib_TO_STR(_arg_)  #_arg_
#define	cnLib_EXP_TO_STR(_arg_)	cnLib_TO_STR(_arg_)

#define cnLib_FILE_LINE	__FILE__ "(" cnLib_EXP_TO_STR(__LINE__) ")"


//---------------------------------------------------------------------------
#ifdef	cnLib_DEBUG
#define	cnLib_ASSERT(_e_)	(void)( !!(_e_) || ::MessageBoxW(nullptr,L"Assertion failed at : \r\n " cnLib_FILE_LINE "\r\n"  #_e_ "\r\n\r\nbreak execution?",L"assert",MB_ICONERROR|MB_YESNO)!=IDYES || (__debugbreak(),0) )
#else
#define	cnLib_ASSERT(_e_)	__assume(_e_)
#endif
//---------------------------------------------------------------------------
#ifdef	cnLib_DEBUG

#if _MSC_VER >= 1910 && _MSVC_LANG >= 201703L
#define	cnLib_SWITCH_UNEXPECTED	(void)( ::MessageBoxW(nullptr,L"Assertion failed at : \r\n " cnLib_FILE_LINE "\r\nUnexpected Switch Case\r\n\r\nbreak execution?",L"assert",MB_ICONERROR|MB_YESNO)!=IDYES || (__debugbreak(),0) );[[fallthrough]]
#else
#define	cnLib_SWITCH_UNEXPECTED	(void)( ::MessageBoxW(nullptr,L"Assertion failed at : \r\n " cnLib_FILE_LINE "\r\nUnexpected Switch Case\r\n\r\nbreak execution?",L"assert",MB_ICONERROR|MB_YESNO)!=IDYES || (__debugbreak(),0) )
#endif
#else
#if _MSC_VER >= 1910 && _MSVC_LANG >= 201703L
#define	cnLib_SWITCH_UNEXPECTED		__assume(false);[[fallthrough]]
#else
#define	cnLib_SWITCH_UNEXPECTED		__assume(false)
#endif
#endif
//---------------------------------------------------------------------------
namespace cnLibrary{

//---------------------------------------------------------------------------
#if cnLibrary_CPPFEATURE_STATIC_ASSERT >= 200410L

static_assert(sizeof(uIntn)==sizeof(void*),	"incorrect size of uIntn");
static_assert(sizeof(sIntn)==sizeof(void*),	"incorrect size of sIntn");

static_assert(sizeof(uInt64)==8,"incorrect size of uInt64");
static_assert(sizeof(sInt64)==8,"incorrect size of sInt64");
static_assert(sizeof(uInt32)==4,"incorrect size of uInt32");
static_assert(sizeof(sInt32)==4,"incorrect size of sInt32");
static_assert(sizeof(uInt16)==2,"incorrect size of uInt16");
static_assert(sizeof(sInt16)==2,"incorrect size of sInt16");
static_assert(sizeof(uInt8)==1,	"incorrect size of uInt8");
static_assert(sizeof(sInt8)==1,	"incorrect size of sInt8");

#endif // cnLibrary_CPPFEATURE_STATIC_ASSERT >= 200410L

//---------------------------------------------------------------------------
}   // namespace cnLibrary
//---------------------------------------------------------------------------
#endif  /* __cplusplus */
/*-------------------------------------------------------------------------*/
