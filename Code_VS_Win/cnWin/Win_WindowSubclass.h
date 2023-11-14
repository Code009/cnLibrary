/*- cnWin - Window Subclass -----------------------------------------------*/
/*         Developer : Code009                                             */
/*         Create on : 2018-06-28                                          */
/*-------------------------------------------------------------------------*/
#pragma once
/*-------------------------------------------------------------------------*/
#include <cnWin\Win_Common.h>
#include <cnWin\Win_Window.h>

#ifdef __cplusplus
//---------------------------------------------------------------------------
namespace cnLibrary{
//---------------------------------------------------------------------------
namespace cnWin{
//---------------------------------------------------------------------------
#if _WIN32_WINNT >= _WIN32_WINNT_WINXP
//---------------------------------------------------------------------------
#if cnWin_WINDOWSUBCLASS_VERSION < _WIN32_WINNT_WINXP
#undef	cnWin_WINDOWSUBCLASS_VERSION
#undef	cnWin_WINDOWSUBCLASS_NAMESPACE
#define	cnWin_WINDOWSUBCLASS_VERSION		_WIN32_WINNT_WINXP
#define	cnWin_WINDOWSUBCLASS_NAMESPACE		cnRTL::cnWinRTL::NTXPWindowSubclass
#endif // cnWin_WINDOWSUBCLASS_VERSION < _WIN32_WINNT_VISTA
//---------------------------------------------------------------------------
#endif	// _WIN32_WINNT >= _WIN32_WINNT_WINXP
//---------------------------------------------------------------------------
}	// namespace cnWin
//---------------------------------------------------------------------------
}	// namespace cnLibrary
//---------------------------------------------------------------------------
#endif  /* __cplusplus */
/*-------------------------------------------------------------------------*/
