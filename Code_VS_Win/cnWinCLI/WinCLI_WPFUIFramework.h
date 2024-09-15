/*- cnWinCLI - WPF Framework ----------------------------------------------*/
/*         Developer : Code009                                             */
/*         Create on : 2024-09-02                                          */
/*-------------------------------------------------------------------------*/
#pragma once
/*-------------------------------------------------------------------------*/

#include <cnWinCLI\WinCLI_Common.h>
#include <cnWinCLI\WinCLIM_WPFUIFramework.h>

#ifdef __cplusplus
//---------------------------------------------------------------------------
namespace cnLibrary{
//---------------------------------------------------------------------------
namespace cnWin{
//---------------------------------------------------------------------------
struct cWPFUIViewTargetElementReference
{
#if _MANAGED
	DNet::IWPFUIViewTargetElement ^Target;
#endif // _MANAGED
};
iPtr<iUIView> MakeUIViewFromViewTarget(cWPFUIViewTargetElementReference &Reference)noexcept(true);
//---------------------------------------------------------------------------
struct cWPFUIViewRootTargetElementReference
{
#if _MANAGED
	DNet::IWPFUIViewRootTargetElement ^Target;
#endif // _MANAGED
};
iPtr<iUIWindow> MakeUIWindowFromRootViewTarget(cWPFUIViewRootTargetElementReference &Reference)noexcept(true);
//---------------------------------------------------------------------------
}	// namespace cnWin
//---------------------------------------------------------------------------
}	// namespace cnLibrary
//---------------------------------------------------------------------------
#endif  /* __cplusplus */
