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
	using cnRTL::COMInnerMember;
	using cnRTL::COMInnerObject;
	using cnRTL::COMInnerPtr;

	using cnRTL::cnWinRTL::COLORREFFromUIColor;

HWND GetWindowHandleFromUIWindow(iUIArea *Area)noexcept(true);
HWND GetWindowHandleFromUIView(iUIView *View)noexcept(true);
//HWND GetComponentWindowHandle(iWindowComponent *WindowComponent)noexcept(true);

void HWNDSetOwner(HWND WindowHandle,HWND OwnerHandle)noexcept(true);
cUIPoint AlignWindowRect(iUIArea *Relative,const cUIPoint &WindowSize,const cUIPoint &AlignPos,const cUIPoint &AlignSize,eAroundAlignment Align)noexcept(true);
void MoveWindowAlign(HWND WindowHandle,iUIArea *Relative,const cUIPoint &WindowSize,const cUIPoint &AlignPos,const cUIPoint &AlignSize,eAroundAlignment Align)noexcept(true);

//---------------------------------------------------------------------------
iInterface* QueryNullMouseCursor(void)noexcept(true);
iPtr<iInterface> QuerySystemMouseCursor(LPCWSTR ID)noexcept(true);
//---------------------------------------------------------------------------
rPtr<iGDIFontHandle> QueryGDIFont(const cUITextStyle &TextStyle)noexcept(true);
//---------------------------------------------------------------------------
class WindowMessageHandler : public iWindowMessageHandler
{
public:
	virtual void cnLib_FUNC WindowAttached(void)noexcept(true)override;
	virtual void cnLib_FUNC WindowDetached(void)noexcept(true)override;
	virtual bool cnLib_FUNC WindowMessage(LRESULT &Result,const cWindowMessageParam &MsgParam)noexcept(true)override;
	virtual void cnLib_FUNC WindowMessageProcessed(LRESULT MsgResult,const cWindowMessageParam &MsgParam)noexcept(true)override;
};
//---------------------------------------------------------------------------
}	// namespace cnUI
//---------------------------------------------------------------------------
}	// namespace cnLibrary
//---------------------------------------------------------------------------
