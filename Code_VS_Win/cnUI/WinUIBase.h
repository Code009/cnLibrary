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
iPtr<iWindowClient> CreateUIWindow(HWND Parent,const wchar_t *WindowText,DWORD Style=WS_OVERLAPPEDWINDOW,DWORD ExStyle=0);
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
class WindowComponent
{
public:
	WindowComponent();
	~WindowComponent();

	iWindow* GetWindow(void)const;
	void SetWindow(iWindow *Window);

protected:
	iPtr<iWindow> fWindow;

	virtual void WindowSetup(void){}
	virtual void WindowClear(void){}
};
//---------------------------------------------------------------------------
class WindowControl : public WindowComponent, public WindowMessageHandler
{
public:
	WindowControl();
	~WindowControl();

	uInt8 ComponentOrder=0;
	void UpdateComponentOrder(void);

protected:
	virtual void WindowSetup(void)override;
	virtual void WindowClear(void)override;
};
//---------------------------------------------------------------------------
class WindowClient : public WindowComponent
{
public:
	WindowClient();
	~WindowClient();

	iWindowClient* GetWindowClient(void)const;
	iUIWindow* GetUIWindow(void)const;

	iUIView* GetClientView(void)const;
	void SetClientView(iUIView *View);
protected:
	iPtr<iWindowClient> fWindowClient;

	virtual void WindowSetup(void);
	virtual void WindowClear(void);
};
//---------------------------------------------------------------------------
class WindowFrame : public WindowComponent
{
public:
	WindowFrame();
	~WindowFrame();

	iWindowFrame* GetWindowFrame(void)const;
	iUIWindow* GetUIWindow(void)const;

	iUIView* GetClientView(void)const;
	void SetClientView(iUIView *View);
protected:
	iPtr<iWindowFrame> fWindowFrame;

	virtual void WindowSetup(void)override;
	virtual void WindowClear(void)override;
};
//---------------------------------------------------------------------------
template<class T>
inline bool WindowControlSetPos(T &WindowControl,iUIArea *Relative,const cUIPoint &Pos)
{
	return ViewSetPos(WindowControl.GetUIWindow(),Relative,Pos);
}
//---------------------------------------------------------------------------
template<class T>
inline bool WindowControlSetSize(T &WindowControl,const cUIPoint &Size)
{
	return ViewSetSize(WindowControl.GetUIWindow(),Size);
}
//---------------------------------------------------------------------------
template<class T>
inline bool WindowControlSetRect(T &WindowControl,iUIArea *Relative,const cUIPoint &Pos,const cUIPoint &Size)
{
	return ViewSetRect(WindowControl.GetUIWindow(),Relative,Pos,Size);
}
//---------------------------------------------------------------------------
template<class T>
inline bool WindowControlSetRect(T &WindowControl,iUIArea *Relative,const cUIRect &Rect)
{
	return ViewSetRect(WindowControl.GetUIWindow(),Relative,Rect);
}
//---------------------------------------------------------------------------
class cPopupStaticWindow
{
public:
	cPopupStaticWindow();
	~cPopupStaticWindow();

	iWindow* GetWindow(void)const;
	HWND GetWindowHandle(void)const;

	iWindowClient* GetWindowClient(void)const;
	iUIWindow* GetUIWindow(void)const;
	iUIView* GetClientView(void)const;
	bool SetClientView(iUIView *View);

	cnRTL::cString<wchar_t> WindowTitle;
	DWORD WindowStyle=WS_OVERLAPPED|WS_DISABLED;
	DWORD WindowExStyle=WS_EX_TOOLWINDOW|WS_EX_NOACTIVATE;

	void SetupOwnerWindow(HWND OwnerWindow);
	void SetupOwner(iUIView *Owner);

	void MovePopup(iUIArea *Relative,const cUIPoint &WindowSize,const cUIPoint &AlignPos,const cUIPoint &AlignSize,eAroundAlignment Align);
	void MovePopup(iUIArea *Relative,const cUIPoint &Pos,const cUIPoint &Size);
	void ShowPopup(void);
	void HidePopup(void);

protected:
	iPtr<iWindowClient> fWindowClient;
	iPtr<iWindow> fWindow;
};
//---------------------------------------------------------------------------
class cPopupWindow : public WindowMessageHandler
{
public:
	cPopupWindow();
	~cPopupWindow();
	
	iWindow* GetWindow(void)const;
	HWND GetWindowHandle(void)const;

	iWindowClient* GetWindowClient(void)const;
	iUIWindow* GetUIWindow(void)const;
	iUIView* GetClientView(void)const;
	bool SetClientView(iUIView *View);

	cnRTL::cString<wchar_t> WindowTitle;
	DWORD WindowStyle=WS_OVERLAPPED;
	DWORD WindowExStyle=WS_EX_TOOLWINDOW|WS_EX_NOACTIVATE;

	void SetupOwnerWindow(HWND OwnerWindow);
	void SetupOwner(iUIView *Owner);

	void MovePopup(iUIArea *Relative,const cUIPoint &WindowSize,const cUIPoint &AlignPos,const cUIPoint &AlignSize,eAroundAlignment Align);
	void MovePopup(iUIArea *Relative,const cUIPoint &Pos,const cUIPoint &Size);
	void ShowPopup(void);
	void HidePopup(void);

	bool IsPopuped(void);

protected:
	iPtr<iWindow> fWindow;
	iPtr<iOwnerFocusWindowClient> fWindowClient;
	
	virtual bool cnLib_FUNC WindowMessage(LRESULT &Result,const cWindowMessageParam &MsgParam)override;
};
//---------------------------------------------------------------------------
class ModalDialog : protected WindowMessageHandler
{
public:
	ModalDialog();
	~ModalDialog();

	INT_PTR Modal(HWND Parent,const wchar_t *WindowText,LONG X=0,LONG Y=0,LONG Width=0,LONG Height=0,DWORD Style=WS_CAPTION|WS_SYSMENU|WS_DLGFRAME,DWORD ExStyle=WS_EX_DLGMODALFRAME);
	INT_PTR Modal(HWND Parent,HINSTANCE hInstance,LPCWSTR TemplateName);
	INT_PTR Modal(HWND Parent,HINSTANCE hInstance,LPCDLGTEMPLATEW DialogTemplate);

protected:
	iPtr<iWindowProvider> fDialogProvider;
	HWND fWindowHandle;

	virtual void cnLib_FUNC WindowAttached(void)override;
	virtual void cnLib_FUNC WindowDetached(void)override;
	virtual bool cnLib_FUNC WindowMessage(LRESULT &Result,const cWindowMessageParam &MsgParam)override;
	virtual void cnLib_FUNC WindowMessageProcessed(LRESULT MsgResult,const cWindowMessageParam &MsgParam)override;

	virtual void ModalDialogCreateClient(HWND Parent,int x,int y,int w,int h)=0;
	virtual void ModalDialogMoveClient(int x,int y,int w,int h)=0;
};
//---------------------------------------------------------------------------
INT_PTR ShowModalWindow(iWindowProvider *ClientProvider,HWND Parent,const wchar_t *Text,int x=0,int y=0,int w=0,int h=0,DWORD Style=WS_CAPTION|WS_SYSMENU,DWORD ExStyle=WS_EX_DLGMODALFRAME);
//---------------------------------------------------------------------------
}	// namespace cnUI
}	// namespace cnLibrary
//---------------------------------------------------------------------------
