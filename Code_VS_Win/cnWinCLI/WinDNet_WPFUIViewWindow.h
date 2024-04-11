/*- cnWinWPF - View Content -----------------------------------------------*/
/*         Developer : Code009                                             */
/*         Create on : 2023-10-07                                          */
/*-------------------------------------------------------------------------*/
#pragma once
/*-------------------------------------------------------------------------*/

#include <cnWinCLI\WinDNet_Common.h>
#include <cnWinCLI\WinDNet_WPFUIView.h>
#include <cnWinCLI\WinDNetM_WPFUIViewWindow.h>

#ifdef __cplusplus
//---------------------------------------------------------------------------
namespace cnLibrary{
//---------------------------------------------------------------------------
namespace cnWin{
//---------------------------------------------------------------------------
iPtr<iWindow> DNetCreateWPFWindow(HWND Parent,const wchar_t *WindowText,DWORD Style,DWORD ExStyle,LONG X,LONG Y,LONG Width,LONG Height,UINT ChildID)noexcept(true);
//---------------------------------------------------------------------------
class cWPFUIWindow
	: public iUIWindow , public iWindowClient, public iWPFWindowClient
	, public iCLIObject, public cnRTL::bcVirtualLifeCycle
	, public mcWPFViewRoot
{
	friend mcWPFViewRoot;
public:
	cWPFUIWindow(cDNetUIThread *UIThread,mcConstructParameter &Parameter)noexcept(true);
	~cWPFUIWindow()noexcept(true);

	virtual void* cnLib_FUNC CastInterface(iTypeID IID)noexcept(true) override;

	virtual cGCRef& cnLib_FUNC GetObjecHandle(void)noexcept(true)override;

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

	virtual void cnLib_FUNC SetBackgroundColor(COLORREF Color)noexcept(true)override;

	bool CheckThread(void)noexcept(true);

protected:
	void VirtualStarted(void)noexcept(true);
	void VirtualStopped(void)noexcept(true);

	iPtr<cDNetUIThread> fUIThread;

	// iWPFViewParent

	virtual eUIState WPFParentGetState(void)noexcept(true)override;
	virtual Float32 WPFParentGetContentScale(void)noexcept(true)override;
	virtual Float32 WPFParentGetLayoutScale(void)noexcept(true)override;

	// iWPFWindowClient

	virtual iWindowClient* WPFWindowGetInterface(void)noexcept(true)override;
	virtual void WPFWindowStateChanged(UIState NewState)noexcept(true)override;
	virtual void WPFWindowDPIChanged(Float32 NewScale)noexcept(true)override;

	eUIState fWindowState;
	Float32 fContentScale;

};
//---------------------------------------------------------------------------
class cWPFWindowAsWindowClient
	: public cWPFUIWindow
{
public:

	cWPFWindowAsWindowClient(cDNetUIThread *UIThread,mcWPFViewRoot::mcConstructParameter &Parameter)noexcept(true);
	~cWPFWindowAsWindowClient()noexcept(true);

	// iWindowClient

	virtual iWindow* cnLib_FUNC GetWindow(void)noexcept(true)override;
	virtual bool cnLib_FUNC SetWindow(iWindow *Window)noexcept(true)override;

protected:
	
};
//---------------------------------------------------------------------------
class cWPFWindow
	: public iCLIObject, public iWindow
	, private iDependentInfo, public cnRTL::bcVirtualLifeCycle
	, public mcWPFWindow
{
	friend mcWPFWindow;
public:
	cWPFWindow(cDNetUIThread *UIThread)noexcept(true);
	~cWPFWindow()noexcept(true);

	HWND GetHandle(void)noexcept(true);


	virtual void* cnLib_FUNC CastInterface(iTypeID IID)noexcept(true) override;

	// iDNetObject

	virtual cGCRef& cnLib_FUNC GetObjecHandle(void)noexcept(true)override;

	// iWindow

	virtual iUIThread* cnLib_FUNC GetUIThread(void)noexcept(true)override;
	virtual HWND cnLib_FUNC GetWindowHandle(void)noexcept(true)override;
	virtual bool cnLib_FUNC InsertMessageHandler(iWindowMessageHandler *WindowHandler,sfInt8 Order)noexcept(true)override;
	virtual bool cnLib_FUNC RemoveMessageHandler(iWindowMessageHandler *WindowHandler)noexcept(true)override;
	virtual iWindowClient* GetClient(void)noexcept(true)override;
	virtual iWindowFrame* GetFrame(void)noexcept(true)override;
	virtual rPtr<iVariable> QueryAffixedVariable(const void *Token)noexcept(true)override;

	cDNetUIThread* GetWPFUIThread(void)noexcept(true);
	eUIState GetUIState(void)const noexcept(true);
	Float32 GetLayoutScale(void)const noexcept(true);
	Float32 GetContentScale(void)const noexcept(true);

protected:
	void VirtualStarted(void)noexcept(true);
	void VirtualStopped(void)noexcept(true);

	cnRTL::cAffixedVariableSet fAffixedVariableSet;

	bool CheckThread(void)noexcept(true);
	iPtr<cDNetUIThread> fUIThread;

	cnRTL::cnWinRTL::cWindowUIState fWindowState;
	WORD fWindowDPI;

	cnRTL::cOrderedObjectSet<iWindowMessageHandler*,sfInt8> fHandlers;

	void WindowAttach(void)noexcept(true);
	void WindowDetach(void)noexcept(true);
private:
	

	virtual rPtr< iArrayReference<const uChar16> > cnLib_FUNC DependentCreateDescription(void)noexcept(true)override;
	virtual void cnLib_FUNC DependentShutdownNotification(void)noexcept(true)override;

	bool WindowMessage(LRESULT &MessageResult,HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)noexcept(true);
};
//---------------------------------------------------------------------------
class cWPFWindowClient
	: public cWPFUIWindow
{
public:
	cWPFWindowClient(cDNetUIThread *UIThread,mcWPFViewRoot::mcConstructParameter &Parameter)noexcept(true);
	~cWPFWindowClient()noexcept(true);

	// iWindowClient

	virtual iWindow* cnLib_FUNC GetWindow(void)noexcept(true)override;
	virtual bool cnLib_FUNC SetWindow(iWindow *Window)noexcept(true)override;

protected:

	iPtr<cWPFWindow> fWindowHost;

	bool WPFSetupWindow(iCLIObject *Object)noexcept(true);
	bool WPFClearWindow(void)noexcept(true);

	// iWPFViewParent

	virtual Float32 WPFParentGetLayoutScale(void)noexcept(true)override;

	// iWPFWindowClient

	virtual void WPFWindowStateChanged(eUIState NewState)noexcept(true)override;
	virtual void WPFWindowDPIChanged(Float32 NewScale)noexcept(true)override;


	//void mWindowSetBGColor(uInt8 r,uInt8 g,uInt8 b);

	SIZE GetClientSize(void)noexcept(true);
	
};
//---------------------------------------------------------------------------
}	// namespace cnWin
//---------------------------------------------------------------------------
}	// namespace cnLibrary
//---------------------------------------------------------------------------
#endif  /* __cplusplus */
/*-------------------------------------------------------------------------*/
