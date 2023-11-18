/*- cnWinUI - Common ------------------------------------------------------*/
/*         Developer : Code009                                             */
/*         Create on : 2021-04-10                                          */
/*-------------------------------------------------------------------------*/
#pragma once
#include <cnSystem\cnWindows.h>
#include <cnUI\UICommon.h>
#include <cnUI\UILayout.h>
#include <cnRTL\WinCommon.h>
#include <cnRTL\WinCOM.h>
#include <cnRTL\WinGDI.h>

//---------------------------------------------------------------------------
namespace cnLibrary{
//---------------------------------------------------------------------------
namespace cnUI{
	using cnRTL::COMPtr;
	using cnRTL::COMInnerObject;
	using cnRTL::COMInnerPtr;

	using cnRTL::cnWinRTL::COLORREFFromUIColor;

HWND GetWindowHandleFromUIWindow(iUIArea *Area);
HWND GetWindowHandleFromUIView(iUIView *View);
//HWND GetComponentWindowHandle(iWindowComponent *WindowComponent);

void HWNDSetOwner(HWND WindowHandle,HWND OwnerHandle);
cUIPoint AlignWindowRect(iUIArea *Relative,const cUIPoint &WindowSize,const cUIPoint &AlignPos,const cUIPoint &AlignSize,eAroundAlignment Align);
void MoveWindowAlign(HWND WindowHandle,iUIArea *Relative,const cUIPoint &WindowSize,const cUIPoint &AlignPos,const cUIPoint &AlignSize,eAroundAlignment Align);

//---------------------------------------------------------------------------
iInterface* QueryNullMouseCursor(void);
iPtr<iInterface> QuerySystemMouseCursor(LPCWSTR ID);
//---------------------------------------------------------------------------
rPtr<iGDIFontHandle> QueryGDIFont(const cUITextStyle &TextStyle);
//---------------------------------------------------------------------------
class WindowMessageHandler : protected iWindowMessageHandler
{
protected:
	virtual void cnLib_FUNC WindowAttached(void)override;
	virtual void cnLib_FUNC WindowDetached(void)override;
	virtual bool cnLib_FUNC WindowMessage(LRESULT &Result,const cWindowMessageParam &MsgParam)override;
	virtual void cnLib_FUNC WindowMessageProcessed(LRESULT MsgResult,const cWindowMessageParam &MsgParam)override;
};
//---------------------------------------------------------------------------
}	// namespace cnUI
//---------------------------------------------------------------------------
}	// namespace cnLibrary
//---------------------------------------------------------------------------
