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
iPtr<iWindow> DNetCreateWPFWindow(HWND Parent,const wchar_t *WindowText,DWORD Style,DWORD ExStyle,LONG X,LONG Y,LONG Width,LONG Height,UINT ChildID);
//---------------------------------------------------------------------------
class cWPFUIWindow
	: public iUIWindow , public iWindowClient, public iWPFWindowClient
	, public iCLIObject, public cnRTL::bcVirtualLifeCycle
	, public mcWPFViewRoot
{
	friend mcWPFViewRoot;
public:
	cWPFUIWindow(cDNetUIThread *UIThread,mcConstructParameter &Parameter);
	~cWPFUIWindow();

	virtual void* cnLib_FUNC CastInterface(iTypeID IID)noexcept(true) override;

	virtual cGCRef& cnLib_FUNC GetObjecHandle(void)override;

	// iUIArea

	virtual iUIThread* cnLib_FUNC GetUIThread(void)override;
	virtual eUIState cnLib_FUNC GetUIState(void)override;
	virtual bool cnLib_FUNC InsertStateHandler(iUIStateHandler *Handler,sfInt16 Order)override;
	virtual bool cnLib_FUNC RemoveStateHandler(iUIStateHandler *Handler)override;
	virtual bool cnLib_FUNC GetVisible(void)override;
	virtual bool cnLib_FUNC SetVisible(bool Visible)override;
	virtual bool cnLib_FUNC IsEnabled(void)override;
	virtual bool cnLib_FUNC GetEnable(void)override;
	virtual bool cnLib_FUNC SetEnable(bool Enable)override;
	virtual cUIPoint cnLib_FUNC GetSize(void)override;
	virtual bool cnLib_FUNC SetSize(cUIPoint Size)override;
	virtual bool cnLib_FUNC TranslatePointTo(iInterface *Relative,cUIPoint &Position)override;
	virtual bool cnLib_FUNC MoveTo(iInterface *Relative,cUIPoint Position)override;
	virtual bool cnLib_FUNC ArrangeRectangle(iInterface *Relative,cUIPoint Position,cUIPoint Size)override;
	virtual Float32 cnLib_FUNC GetZPosition(void)override;
	virtual bool cnLib_FUNC SetZPosition(Float32 ZPosition)override;
	virtual Float32 cnLib_FUNC GetContentScale(void)override;

	// iUIWindow

	virtual iUIScreen* cnLib_FUNC GetScreen(void)override;
	virtual iUIWindow* cnLib_FUNC GetParent(void)override;
	virtual bool cnLib_FUNC InsertWindowHandler(iUIWindowHandler *Handler,sfInt16 Order)override;
	virtual bool cnLib_FUNC RemoveWindowHandler(iUIWindowHandler *Handler)override;

	virtual iUIView* cnLib_FUNC GetClient(void)override;
	virtual bool cnLib_FUNC SetClient(iUIView *View)override;

	// iWindowClient

	virtual void cnLib_FUNC SetBackgroundColor(COLORREF Color)override;

	bool CheckThread(void);

protected:
	void VirtualStarted(void);
	void VirtualStopped(void);

	iPtr<cDNetUIThread> fUIThread;

	// iWPFViewParent

	virtual eUIState WPFParentGetState(void)override;
	virtual Float32 WPFParentGetContentScale(void)override;
	virtual Float32 WPFParentGetLayoutScale(void)override;

	// iWPFWindowClient

	virtual iWindowClient* WPFWindowGetInterface(void)override;
	virtual void WPFWindowStateChanged(UIState NewState)override;
	virtual void WPFWindowDPIChanged(Float32 NewScale)override;

	eUIState fWindowState;
	Float32 fContentScale;

};
//---------------------------------------------------------------------------
class cWPFWindowAsWindowClient
	: public cWPFUIWindow
{
public:

	cWPFWindowAsWindowClient(cDNetUIThread *UIThread,mcWPFViewRoot::mcConstructParameter &Parameter);
	~cWPFWindowAsWindowClient();

	// iWindowClient

	virtual iWindow* cnLib_FUNC GetWindow(void)override;
	virtual bool cnLib_FUNC SetWindow(iWindow *Window)override;

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
	cWPFWindow(cDNetUIThread *UIThread);
	~cWPFWindow();

	HWND GetHandle(void);


	virtual void* cnLib_FUNC CastInterface(iTypeID IID)noexcept(true) override;

	// iDNetObject

	virtual cGCRef& cnLib_FUNC GetObjecHandle(void)override;

	// iWindow

	virtual iUIThread* cnLib_FUNC GetUIThread(void)override;
	virtual HWND cnLib_FUNC GetWindowHandle(void)override;
	virtual bool cnLib_FUNC InsertMessageHandler(iWindowMessageHandler *WindowHandler,sfInt8 Order)override;
	virtual bool cnLib_FUNC RemoveMessageHandler(iWindowMessageHandler *WindowHandler)override;
	virtual iWindowClient* GetClient(void)override;
	virtual iWindowFrame* GetFrame(void)override;
	virtual rPtr<iVariable> QueryAffixedVariable(const void *Token)override;

	cDNetUIThread* GetWPFUIThread(void);
	eUIState GetUIState(void)const;
	Float32 GetLayoutScale(void)const;
	Float32 GetContentScale(void)const;

protected:
	void VirtualStarted(void);
	void VirtualStopped(void);

	cnRTL::cAffixedVariableSet fAffixedVariableSet;

	bool CheckThread(void);
	iPtr<cDNetUIThread> fUIThread;

	cnRTL::cnWinRTL::cWindowUIState fWindowState;
	WORD fWindowDPI;

	cnRTL::cOrderedObjectSet<iWindowMessageHandler*,sfInt8> fHandlers;

	void WindowAttach(void);
	void WindowDetach(void);
private:
	

	virtual rPtr< iArrayReference<const uChar16> > cnLib_FUNC DependentCreateDescription(void)override;
	virtual void cnLib_FUNC DependentShutdownNotification(void)override;

	bool WindowMessage(LRESULT &MessageResult,HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam);
};
//---------------------------------------------------------------------------
class cWPFWindowClient
	: public cWPFUIWindow
{
public:
	cWPFWindowClient(cDNetUIThread *UIThread,mcWPFViewRoot::mcConstructParameter &Parameter);
	~cWPFWindowClient();

	// iWindowClient

	virtual iWindow* cnLib_FUNC GetWindow(void)override;
	virtual bool cnLib_FUNC SetWindow(iWindow *Window)override;

protected:

	iPtr<cWPFWindow> fWindowHost;

	bool WPFSetupWindow(iCLIObject *Object);
	bool WPFClearWindow(void);

	// iWPFViewParent

	virtual Float32 WPFParentGetLayoutScale(void)override;

	// iWPFWindowClient

	virtual void WPFWindowStateChanged(eUIState NewState)override;
	virtual void WPFWindowDPIChanged(Float32 NewScale)override;


	//void mWindowSetBGColor(uInt8 r,uInt8 g,uInt8 b);

	SIZE GetClientSize(void);
	
};
//---------------------------------------------------------------------------
}	// namespace cnWin
//---------------------------------------------------------------------------
}	// namespace cnLibrary
//---------------------------------------------------------------------------
#endif  /* __cplusplus */
/*-------------------------------------------------------------------------*/
