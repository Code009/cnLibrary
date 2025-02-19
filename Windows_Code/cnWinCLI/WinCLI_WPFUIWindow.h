/*- cnWinCLI - WPF Window -------------------------------------------------*/
/*         Developer : Code009                                             */
/*         Create on : 2023-10-07                                          */
/*-------------------------------------------------------------------------*/
#pragma once
/*-------------------------------------------------------------------------*/

#include <cnWinCLI\WinCLI_Common.h>
#include <cnWinCLI\WinCLI_WPFUIView.h>
#include <cnWinCLI\WinCLIM_WPFUIWindow.h>

#ifdef __cplusplus
//---------------------------------------------------------------------------
namespace cnLibrary{
//---------------------------------------------------------------------------
namespace cnWin{
//---------------------------------------------------------------------------
iPtr<iWindowClient> WPFCreateWindowClient(void)noexcept(true);
rPtr<iPopupWindowControl> WPFCreatePopupWindowControl(void)noexcept(true);
//---------------------------------------------------------------------------
class cWPFWindow;
//---------------------------------------------------------------------------
class iWPFWindowClient : public iInterface
{
public:
	struct tInterfaceID{	static iTypeID Value;	};
	virtual void* cnLib_FUNC CastInterface(iTypeID ID)noexcept(true) override{		return cnClass::ImplementCastInterface(this,ID);	}

	virtual bool WPFClientSetupWindow(cWPFWindow *Window)noexcept(true)=0;
	virtual void WPFClientClearWindow(cWPFWindow *Window)noexcept(true)=0;
	virtual mbcWPFUIViewRoot* WPFClientGetViewRoot(void)noexcept(true)=0;
	virtual iWindowClient* WPFWindowGetInterface(void)noexcept(true)=0;
	virtual void WPFWindowDPIChanged(WORD NewDPI)noexcept(true)=0;
};
//---------------------------------------------------------------------------
class cWPFWindow
	: public iWindow, public iWindowProvider
	, public cnRTL::bcVirtualLifeCycle
	, protected mbcWPFWindow
{
public:
	cWPFWindow(cWPFUIThread *UIThread)noexcept(true);
	~cWPFWindow()noexcept(true);

	HWND GetHandle(void)noexcept(true);

	using mbcWPFWindow::mCreate;
	using mbcWPFWindow::mDestroy;
	using mbcWPFWindow::mQueryLayoutScale;


	struct tInterfaceID{	static iTypeID Value;	};
	virtual void* cnLib_FUNC CastInterface(iTypeID IID)noexcept(true)override;

	// iWindow

	virtual iUIThread* cnLib_FUNC GetUIThread(void)noexcept(true)override;
	virtual HWND cnLib_FUNC GetWindowHandle(void)noexcept(true)override;
	virtual bool cnLib_FUNC InsertMessageHandler(iWindowMessageHandler *WindowHandler,sfInt8 Order)noexcept(true)override;
	virtual bool cnLib_FUNC RemoveMessageHandler(iWindowMessageHandler *WindowHandler)noexcept(true)override;
	virtual iWindowClient* GetClient(void)noexcept(true)override;
	virtual bool SetClient(iWindowClient *Client)noexcept(true)override;
	virtual iWindowFrame* GetFrame(void)noexcept(true)override;
	virtual bool SetFrame(iWindowFrame *Client)noexcept(true)override;
	virtual rPtr<iVariable> QueryAffixedVariable(const void *Token)noexcept(true)override;
	virtual bool cnLib_FUNC GetMouseAutoCapture(void)noexcept(true)override;
	virtual bool cnLib_FUNC SetMouseAutoCapture(bool Enable)noexcept(true)override;

	// iWindowProvider

	virtual bool cnLib_FUNC SubclassAttach(HWND Window,bool LocalClass)noexcept(true)override;
	virtual bool cnLib_FUNC SubclassDetach(void)noexcept(true)override;
	virtual bool cnLib_FUNC WindowCreate(HWND Parent,const wchar_t *WindowText,DWORD Style=WS_OVERLAPPEDWINDOW,DWORD ExStyle=0,LONG X=CW_USEDEFAULT,LONG Y=CW_USEDEFAULT,LONG Width=CW_USEDEFAULT,LONG Height=CW_USEDEFAULT,UINT ChildID=0)noexcept(true)override;
	virtual bool cnLib_FUNC WindowDestroy(void)noexcept(true)override;


	cWPFUIThread* GetWPFUIThread(void)noexcept(true);
	WORD GetWindowDPI(void)const noexcept(true);


	void WindowClientExit(iWindowClient *Client)noexcept(true);
protected:
	void VirtualStarted(void)noexcept(true);
	void VirtualStopped(void)noexcept(true);

	cnRTL::cAffixedVariableSet fAffixedVariableSet;

	bool CheckThread(void)noexcept(true);
	iPtr<cWPFUIThread> fUIThread;

	HWND fWindowHandle;
	WORD fWindowDPI;

	iWPFWindowClient *fWPFClient=nullptr;
	cnRTL::cOrderedObjectSet<iWindowMessageHandler*,sfInt8> fHandlers;

	virtual void WPFSourceAttach(void *WindowHandle)noexcept(true)override;
	virtual void WPFSourceDetach(void)noexcept(true)override;
	virtual bool WPFSourceMessage(uIntn &Result,void *hwnd, uInt32 msg, uIntn wParam, uIntn lParam)noexcept(true)override;
private:
	bool WindowMessage(LRESULT &MessageResult,HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)noexcept(true);

	void Cleanup(void)noexcept(true);

	class cExitParentProcedure : public iProcedure
	{
		virtual void cnLib_FUNC Execute(void)noexcept(true);
	}fExitParentProcedure;
};
//---------------------------------------------------------------------------
class cWPFWindowClient
	: public iWindowClient, public iWPFWindowClient
	, public iUIWindow, public iWPFViewParent, public iWPFViewWindow
	, public iCLIObject, public cnRTL::bcVirtualLifeCycle
	, public mbcWPFUIViewRoot
{
public:
	cWPFWindowClient(cWPFUIThread *UIThread,const cGCHandle &WPFUIViewRootTargetElement)noexcept(true);
	~cWPFWindowClient()noexcept(true);

	virtual void* cnLib_FUNC CastInterface(iTypeID IID)noexcept(true) override;

	virtual const cGCHandle& cnLib_FUNC GetObjecHandle(void)noexcept(true)override;

	// iUIArea

	virtual iUIThread* cnLib_FUNC GetUIThread(void)noexcept(true)override;
	virtual eUIState cnLib_FUNC GetUIState(void)noexcept(true)override;
	virtual bool cnLib_FUNC InsertStateHandler(iUIStateHandler *Handler,sfInt16 Order)noexcept(true)override;
	virtual bool cnLib_FUNC RemoveStateHandler(iUIStateHandler *Handler)noexcept(true)override;
	virtual bool cnLib_FUNC GetVisible(void)noexcept(true)override;
	virtual bool cnLib_FUNC SetVisible(bool Visible)noexcept(true)override;
	virtual bool cnLib_FUNC IsEnabled(void)noexcept(true)override;
	virtual bool cnLib_FUNC GetEnable(void)noexcept(true)override;
	virtual bool cnLib_FUNC SetEnable(bool Enable)noexcept(true)override;
	virtual cUIPoint cnLib_FUNC GetSize(void)noexcept(true)override;
	virtual bool cnLib_FUNC SetSize(cUIPoint Size)noexcept(true)override;
	virtual bool cnLib_FUNC TranslatePointTo(iInterface *Relative,cUIPoint &Position)noexcept(true)override;
	virtual bool cnLib_FUNC MoveTo(iInterface *Relative,cUIPoint Position)noexcept(true)override;
	virtual bool cnLib_FUNC ArrangeRectangle(iInterface *Relative,cUIPoint Position,cUIPoint Size)noexcept(true)override;
	virtual Float32 cnLib_FUNC GetZPosition(void)noexcept(true)override;
	virtual bool cnLib_FUNC SetZPosition(Float32 ZPosition)noexcept(true)override;
	virtual Float32 cnLib_FUNC GetContentScale(void)noexcept(true)override;

	// iUIWindow

	virtual iUIScreen* cnLib_FUNC GetScreen(void)noexcept(true)override;
	virtual iUIWindow* cnLib_FUNC GetParent(void)noexcept(true)override;
	virtual bool cnLib_FUNC InsertWindowHandler(iUIWindowHandler *Handler,sfInt16 Order)noexcept(true)override;
	virtual bool cnLib_FUNC RemoveWindowHandler(iUIWindowHandler *Handler)noexcept(true)override;

	virtual iUIView* cnLib_FUNC GetClient(void)noexcept(true)override;
	virtual bool cnLib_FUNC SetClient(iUIView *View)noexcept(true)override;

	// iWindowClient

	virtual iWindow* cnLib_FUNC GetWindow(void)noexcept(true)override;
	virtual void cnLib_FUNC SetBackgroundColor(COLORREF Color)noexcept(true)override;

	bool CheckThread(void)noexcept(true);

protected:
	void VirtualStarted(void)noexcept(true);
	void VirtualStopped(void)noexcept(true);

	iPtr<cWPFUIThread> fUIThread;
	iPtr<cWPFWindow> fWindowHost;
	cWPFUIView *fClient=nullptr;

	virtual void WPFWindowStateChanged(void)noexcept(true)override;
	virtual void WPFWindowShutdownStarted(void)noexcept(true)override;

	// iWPFViewParent

	// iWPFViewWindow

	virtual iUIWindow* WPFWindowInterface(void)noexcept(true)override;
	virtual iWindow* WPFWindowGetWindow(void)noexcept(true)override;
	virtual bool WPFWindowIsActive(void)noexcept(true)override;

	// iWPFWindowClient

	virtual bool WPFClientSetupWindow(cWPFWindow *Window)noexcept(true)override;
	virtual void WPFClientClearWindow(cWPFWindow *Window)noexcept(true)override;
	virtual mbcWPFUIViewRoot* WPFClientGetViewRoot(void)noexcept(true)override;
	virtual iWindowClient* WPFWindowGetInterface(void)noexcept(true)override;
	virtual void WPFWindowDPIChanged(WORD NewDPI)noexcept(true)override;

	eUIState fWindowState;
	Float32 fLayoutScale;
	Float32 fContentScale;

	SIZE GetClientSize(void)noexcept(true);

private:
	eUIState EvaluateUIState(void)const noexcept(true);
	void UpdateUIState(void)noexcept(true);

	void UpdateContentScale(Float32 NewLayoutScale,Float32 NewContentScale)noexcept(true);

	void Cleanup(void)noexcept(true);

	class cExitParentProcedure : public iProcedure
	{
		virtual void cnLib_FUNC Execute(void)noexcept(true);
	}fExitParentProcedure;
};
//---------------------------------------------------------------------------
class cWPFPopupWindowControl : public iPopupWindowControl, public cWPFWindowClient
{
public:
	cWPFPopupWindowControl(cWPFUIThread *UIThread,const cGCHandle &WPFUIViewRootTargetElement)noexcept(true);
	~cWPFPopupWindowControl()noexcept(true);

	virtual void cnLib_FUNC SetCallback(iPopupWindowControlCallback *Callback)noexcept(true)override;
	virtual iUIWindow* cnLib_FUNC GetUIWindow(void)noexcept(true)override;
	virtual bool cnLib_FUNC SetupOwner(iUIView *View)noexcept(true)override;
	virtual void cnLib_FUNC ShowPopup(void)noexcept(true)override;
	virtual void cnLib_FUNC HidePopup(void)noexcept(true)override;
protected:
	iPopupWindowControlCallback *fCallback=nullptr;

	void ClearOwner(void)noexcept(true);
};
//---------------------------------------------------------------------------
}	// namespace cnWin
//---------------------------------------------------------------------------
}	// namespace cnLibrary
//---------------------------------------------------------------------------
#endif  /* __cplusplus */
