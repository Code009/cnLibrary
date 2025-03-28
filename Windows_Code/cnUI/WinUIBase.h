/*- cnWinUI - Base --------------------------------------------------------*/
/*         Developer : Code009                                             */
/*         Create on : 2019-05-13                                          */
/*-------------------------------------------------------------------------*/
#pragma once

#include <cnUI\WinUICommon.h>

//---------------------------------------------------------------------------
namespace cnLibrary{
//---------------------------------------------------------------------------
namespace cnUI{
//---------------------------------------------------------------------------
iPtr<iWindowClient> CreateUIWindow(HWND Parent,const wchar_t *WindowText,DWORD Style=WS_OVERLAPPEDWINDOW,DWORD ExStyle=0)noexcept(true);
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
class WindowComponent
{
public:
	WindowComponent()noexcept(true);
	~WindowComponent()noexcept(true);

	iWindow* GetWindow(void)const noexcept(true);
	void SetWindow(iWindow *Window)noexcept(true);

protected:
	iPtr<iWindow> fWindow;

	virtual void WindowSetup(void)noexcept(true){}
	virtual void WindowClear(void)noexcept(true){}
};
//---------------------------------------------------------------------------
class WindowControl : public WindowComponent, public WindowMessageHandler
{
public:
	WindowControl()noexcept(true);
	~WindowControl()noexcept(true);

	uInt8 ComponentOrder=0;
	void UpdateComponentOrder(void)noexcept(true);

protected:
	virtual void WindowSetup(void)noexcept(true)override;
	virtual void WindowClear(void)noexcept(true)override;
};
//---------------------------------------------------------------------------
template<class T>
inline bool WindowControlSetPos(T &WindowControl,iUIArea *Relative,const cUIPoint &Pos)noexcept(true)
{
	return ViewSetPos(WindowControl.GetUIWindow(),Relative,Pos);
}
//---------------------------------------------------------------------------
template<class T>
inline bool WindowControlSetSize(T &WindowControl,const cUIPoint &Size)noexcept(true)
{
	return ViewSetSize(WindowControl.GetUIWindow(),Size);
}
//---------------------------------------------------------------------------
template<class T>
inline bool WindowControlSetRect(T &WindowControl,iUIArea *Relative,const cUIPoint &Pos,const cUIPoint &Size)noexcept(true)
{
	return ViewSetRect(WindowControl.GetUIWindow(),Relative,Pos,Size);
}
//---------------------------------------------------------------------------
template<class T>
inline bool WindowControlSetRect(T &WindowControl,iUIArea *Relative,const cUIRect &Rect)noexcept(true)
{
	return ViewSetRect(WindowControl.GetUIWindow(),Relative,Rect);
}
//---------------------------------------------------------------------------
class cPopupStaticWindow
{
public:
	cPopupStaticWindow()noexcept(true);
	~cPopupStaticWindow()noexcept(true);

	iWindow* GetWindow(void)const noexcept(true);
	HWND GetWindowHandle(void)const noexcept(true);

	iWindowClient* GetWindowClient(void)const noexcept(true);
	iUIWindow* GetUIWindow(void)const noexcept(true);
	iUIView* GetClientView(void)const noexcept(true);
	bool SetClientView(iUIView *View)noexcept(true);

	cnRTL::cString<wchar_t> WindowTitle;
	DWORD WindowStyle=WS_OVERLAPPED|WS_DISABLED;
	DWORD WindowExStyle=WS_EX_TOOLWINDOW|WS_EX_NOACTIVATE;

	void SetupOwnerWindow(HWND OwnerWindow)noexcept(true);
	void SetupOwner(iUIView *Owner)noexcept(true);

	void MovePopup(iUIArea *Relative,const cUIPoint &WindowSize,const cUIPoint &AlignPos,const cUIPoint &AlignSize,eAroundAlignment Align)noexcept(true);
	void MovePopup(iUIArea *Relative,const cUIPoint &Pos,const cUIPoint &Size)noexcept(true);
	void ShowPopup(void)noexcept(true);
	void HidePopup(void)noexcept(true);

protected:
	iPtr<iWindowClient> fWindowClient;
	iPtr<iWindow> fWindow;
};
//---------------------------------------------------------------------------
class cPopupWindow : public WindowMessageHandler
{
public:
	cPopupWindow()noexcept(true);
	~cPopupWindow()noexcept(true);
	
	iWindow* GetWindow(void)const noexcept(true);
	HWND GetWindowHandle(void)const noexcept(true);

	iWindowClient* GetWindowClient(void)const noexcept(true);
	iUIWindow* GetUIWindow(void)const noexcept(true);
	iUIView* GetClientView(void)const noexcept(true);
	bool SetClientView(iUIView *View)noexcept(true);

	cnRTL::cString<wchar_t> WindowTitle;
	DWORD WindowStyle=WS_OVERLAPPED;
	DWORD WindowExStyle=WS_EX_TOOLWINDOW|WS_EX_NOACTIVATE;

	void SetupOwnerWindow(HWND OwnerWindow)noexcept(true);
	void SetupOwner(iUIView *Owner)noexcept(true);

	void MovePopup(iUIArea *Relative,const cUIPoint &WindowSize,const cUIPoint &AlignPos,const cUIPoint &AlignSize,eAroundAlignment Align)noexcept(true);
	void MovePopup(iUIArea *Relative,const cUIPoint &Pos,const cUIPoint &Size)noexcept(true);
	void ShowPopup(void)noexcept(true);
	void HidePopup(void)noexcept(true);

	bool IsPopuped(void)noexcept(true);

protected:
	iPtr<iWindow> fWindow;
	iPtr<iOwnerFocusWindowClient> fWindowClient;
	
	virtual bool cnLib_FUNC WindowMessage(LRESULT &Result,const cWindowMessageParam &MsgParam)noexcept(true)override;
};
//---------------------------------------------------------------------------
class ModalDialog : protected WindowMessageHandler
{
public:
	ModalDialog()noexcept(true);
	~ModalDialog()noexcept(true);

	INT_PTR Modal(HWND Parent,const wchar_t *WindowText,LONG X=0,LONG Y=0,LONG Width=0,LONG Height=0,DWORD Style=WS_CAPTION|WS_SYSMENU|WS_DLGFRAME,DWORD ExStyle=WS_EX_DLGMODALFRAME)noexcept(true);
	INT_PTR Modal(HWND Parent,HINSTANCE hInstance,LPCWSTR TemplateName)noexcept(true);
	INT_PTR Modal(HWND Parent,HINSTANCE hInstance,LPCDLGTEMPLATEW DialogTemplate)noexcept(true);

protected:
	iPtr<iModalDialogProvider> fDialogProvider;
	HWND fWindowHandle;

	virtual void cnLib_FUNC WindowAttached(void)noexcept(true)override;
	virtual void cnLib_FUNC WindowDetached(void)noexcept(true)override;
	virtual bool cnLib_FUNC WindowMessage(LRESULT &Result,const cWindowMessageParam &MsgParam)noexcept(true)override;
	virtual void cnLib_FUNC WindowMessageProcessed(LRESULT MsgResult,const cWindowMessageParam &MsgParam)noexcept(true)override;

	virtual void ModalDialogCreateClient(HWND Parent,int x,int y,int w,int h)noexcept(true)=0;
	virtual void ModalDialogMoveClient(int x,int y,int w,int h)noexcept(true)=0;
};
//---------------------------------------------------------------------------
INT_PTR ShowModalWindow(iWindowProvider *ClientProvider,HWND Parent,const wchar_t *Text,int x=0,int y=0,int w=0,int h=0,DWORD Style=WS_CAPTION|WS_SYSMENU,DWORD ExStyle=WS_EX_DLGMODALFRAME)noexcept(true);
//---------------------------------------------------------------------------
}	// namespace cnUI
}	// namespace cnLibrary
//---------------------------------------------------------------------------
