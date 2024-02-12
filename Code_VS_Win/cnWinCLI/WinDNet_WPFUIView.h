/*- cnWinWPF - View -------------------------------------------------------*/
/*         Developer : Code009                                             */
/*         Create on : 2023-10-07                                          */
/*-------------------------------------------------------------------------*/
#pragma once
/*-------------------------------------------------------------------------*/

#include <cnWinCLI\WinDNet_WPFUICore.h>
#include <cnWinCLI\WinDNetM_WPFUIView.h>

#ifdef __cplusplus
//---------------------------------------------------------------------------
namespace cnLibrary{
//---------------------------------------------------------------------------
namespace cnWin{
//---------------------------------------------------------------------------
class bcWPFKeyEvent : public iUIKeyEvent
{
public:

	virtual eUIEventSourceType cnLib_FUNC GetSourceType(void)override;
	virtual iUIView* cnLib_FUNC GetSourceView(void)override;
	virtual iUIKeyboard* cnLib_FUNC GetKeyboard(void)override;
	virtual void cnLib_FUNC CancelEvent(void)override;
	virtual bool cnLib_FUNC IsCancelled(void)override;
	virtual bool cnLib_FUNC IsKeyDown(eKeyCode KeyCode)override;
	virtual bool cnLib_FUNC IsKeyToggled(eKeyCode KeyCode)override;

};
//---------------------------------------------------------------------------
class bcWPFMouseEvent : public iUIMouseEvent
{
public:
	virtual eUIEventSourceType cnLib_FUNC GetSourceType(void)override;
	virtual iUIMouse* cnLib_FUNC GetMouse(void)override;
	virtual void cnLib_FUNC CancelEvent(void)override;
	virtual bool cnLib_FUNC IsCancelled(void)override;
	virtual bool cnLib_FUNC GetPosition(iInterface *Relative,cUIPoint &Position)override;
	virtual bool cnLib_FUNC IsButtonDown(eMouseButton Button)override;

};
//---------------------------------------------------------------------------
class cWPFKeyEventByMessage : public bcWPFKeyEvent
{
public:
	cWPFKeyEventByMessage(iUIView *SourceView);

	virtual iUIView* cnLib_FUNC GetSourceView(void)override;


protected:
	iUIView *fSourceView;
};
//---------------------------------------------------------------------------
class cWPFMouseEventByMessage : public bcWPFMouseEvent
{
public:
	cWPFMouseEventByMessage(iUIView *SourceView);

	virtual iUIView* cnLib_FUNC GetSourceView(void)override;

protected:
	iUIView *fSourceView;
};
//---------------------------------------------------------------------------
class cWPFKeyEvent : public bcWPFKeyEvent
{
public:
	cWPFKeyEvent(iUIView *SourceView,mcWPFKeyEventArgs &EventArgs);
	~cWPFKeyEvent();


	virtual iUIView* cnLib_FUNC GetSourceView(void)override;
	virtual void cnLib_FUNC CancelEvent(void)override;
	virtual bool cnLib_FUNC IsCancelled(void)override;

protected:
	iUIView *fSourceView;
	mcWPFKeyEventArgs &fEventArgs;
};
//---------------------------------------------------------------------------
class cWPFMouseEvent : public bcWPFMouseEvent
{
public:
	cWPFMouseEvent(iUIView *SourceView,mcWPFMouseEventArgs &EventArgs);
	~cWPFMouseEvent();

	virtual eUIEventSourceType cnLib_FUNC GetSourceType(void)override;
	virtual iUIView* cnLib_FUNC GetSourceView(void)override;
	virtual void cnLib_FUNC CancelEvent(void)override;
	virtual bool cnLib_FUNC IsCancelled(void)override;

protected:
	iUIView *fSourceView;
	mcWPFMouseEventArgs &fEventArgs;
};
//---------------------------------------------------------------------------
class cWPFTouchEvent : public iUITouchEvent
{
public:
	cWPFTouchEvent(iUIView *SourceView,mcWPFTouchEventArgs &EventArgs);
	~cWPFTouchEvent();

	virtual eUIEventSourceType cnLib_FUNC GetSourceType(void)override;
	virtual iUIView* cnLib_FUNC GetSourceView(void)override;
	virtual void* cnLib_FUNC GetTouchID(void)override;
	virtual void cnLib_FUNC CancelEvent(void)override;
	virtual bool cnLib_FUNC IsCancelled(void)override;
	virtual bool cnLib_FUNC GetPosition(iInterface *Relative,cUIPoint &Position)override;

protected:
	mcWPFTouchEventArgs &fEventArgs;
	iUIView *fSourceView;
	void *fTouchID;
};
//---------------------------------------------------------------------------
class cWPFVisualAsWindow : public iUIWindow, public mcWPFVisual
{
public:
	cWPFVisualAsWindow(mcWPFVisual &&Visual);
	~cWPFVisualAsWindow();

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


private:
};
//---------------------------------------------------------------------------
class bcWPFViewContentDrawing
{
public:
	bcWPFViewContentDrawing();
	~bcWPFViewContentDrawing();

	cWPFUIView* GetView(void);
	bool SetView(iUIView *View);
	bool GetVisible(void);
	bool SetVisible(bool Visible);
	Float32 GetZPosition(void);
	bool SetZPosition(Float32 ZPosition);
	Float32 GetContentScale(void);

	virtual cGCRef* GetDrawingPointer(void)=0;

protected:
	cWPFUIView *fView;

	Float32 fDrawingWidth;
	Float32 fDrawingHeight;
	Float32 fLayoutScale;
	Float32 fZPosition;

	eUIState CheckViewContentState(void);
	void UpdateDrawing(void);

	virtual void ViewContentUpdateRect(void)=0;
	virtual void ViewContentUpdateState(void)=0;
	virtual void ViewContentUpdateWindow(iWindow *Window)=0;
private:
	friend cWPFUIView;
	friend mcWPFView;

	void WPFSetSize(Float32 LayoutScale,Float32 Width,Float32 Height);
};
//---------------------------------------------------------------------------
class cWPFUIView
	: public iUIView, public iUIKeyControl, public iUIMouseControl, public iUITouchControl
	, public iCLIObject , public mcWPFView
	, public cnRTL::bcVirtualLifeCycle
{
	friend mcWPFView;
public:
	cnLib_INTERFACE_ID(cWPFUIView)
	
	cWPFUIView(cDNetUIThread *UIThread,mcConstructParameter &Parameter);
	~cWPFUIView();

	virtual void* cnLib_FUNC CastInterface(iTypeID ID)noexcept(true) override;

	// iCLIObject

	virtual cGCRef& cnLib_FUNC GetObjecHandle(void)override;

	// iUIArea

	virtual iUIThread* cnLib_FUNC GetUIThread(void)override;
	virtual Float32 cnLib_FUNC GetContentScale(void)override;
	virtual cUIPoint cnLib_FUNC GetSize(void)override;
	virtual bool cnLib_FUNC SetSize(cUIPoint Size)override;
	virtual bool cnLib_FUNC TranslatePointTo(iInterface *Relative,cUIPoint &Position)override;
	virtual bool cnLib_FUNC MoveTo(iInterface *Relative,cUIPoint Position)override;
	virtual bool cnLib_FUNC ArrangeRectangle(iInterface *Relative,cUIPoint Position,cUIPoint Size)override;
	virtual Float32 cnLib_FUNC GetZPosition(void)override;
	virtual bool cnLib_FUNC SetZPosition(Float32 ZPosition)override;

	// iUIView

	virtual eUIState cnLib_FUNC GetUIState(void)override;
	virtual bool cnLib_FUNC InsertStateHandler(iUIStateHandler *Handler,sfInt16 Order)override;
	virtual bool cnLib_FUNC RemoveStateHandler(iUIStateHandler *Handler)override;
	virtual bool cnLib_FUNC GetVisible(void)override;
	virtual bool cnLib_FUNC SetVisible(bool Visible)override;
	virtual bool cnLib_FUNC IsEnabled(void)override;
	virtual bool cnLib_FUNC GetEnable(void)override;
	virtual bool cnLib_FUNC SetEnable(bool Enable)override;

	virtual bool cnLib_FUNC InsertViewHandler(iUIViewHandler *Handler,sfInt16 Order)override;
	virtual bool cnLib_FUNC RemoveViewHandler(iUIViewHandler *Handler)override;

	virtual void cnLib_FUNC ArrangeLayout(void)override;
	virtual void cnLib_FUNC SetArrangement(void)override;
	virtual void cnLib_FUNC SetIncludeParentFrameMargin(bool Include)override;
	virtual cUIRectangle cnLib_FUNC GetFrameMargin(void)override;

	virtual iUIWindow* cnLib_FUNC GetWindow(void)override;
	virtual iUIView* cnLib_FUNC GetParent(void)override;
	virtual bool cnLib_FUNC InsertView(iUIView *View)override;
	virtual bool cnLib_FUNC RemoveView(iUIView *View)override;


	// iUIKeyControl

	virtual iUIView* cnLib_FUNC GetFocus(void)override;
	virtual void cnLib_FUNC SetFocus(bool ActivateWindow)override;

	virtual bool cnLib_FUNC InsertKeyHandler(iUIKeyHandler *Handler,sfInt16 Order)override;
	virtual bool cnLib_FUNC RemoveKeyHandler(iUIKeyHandler *Handler)override;

	virtual bool cnLib_FUNC InsertKeyFilter(iUIKeyHandler *Filter,sfInt16 Order)override;
	virtual bool cnLib_FUNC RemoveKeyFilter(iUIKeyHandler *Filter)override;

	virtual bool cnLib_FUNC KeyAcquireExclusive(iUIKeyHandler *Handler)override;
	virtual bool cnLib_FUNC KeyReleaseExclusive(iUIKeyHandler *Handler)override;

	virtual bool cnLib_FUNC InsertTextInputHandler(iUITextInputHandler *Handler,sfInt16 Order)override;
	virtual bool cnLib_FUNC RemoveTextInputHandler(iUITextInputHandler *Handler)override;
	virtual bool cnLib_FUNC SetCompositionViewInfo(iUITextCompositionViewInfo *CompositionViewInfo)override;


	// iUIMouseControl

	virtual void cnLib_FUNC MouseUpdateCursor(void)override;

	virtual bool cnLib_FUNC InsertMouseHandler(iUIMouseHandler *Handler,sfInt16 Order)override;
	virtual bool cnLib_FUNC RemoveMouseHandler(iUIMouseHandler *Handler)override;

	virtual bool cnLib_FUNC InsertMouseFilter(iUIMouseHandler *Filter,sfInt16 Order)override;
	virtual bool cnLib_FUNC RemoveMouseFilter(iUIMouseHandler *Filter)override;

	virtual bool cnLib_FUNC MouseSetCaptrue(void)override;
	virtual void cnLib_FUNC MouseReleaseCaptrue(void)override;
	virtual bool cnLib_FUNC MouseIsCaptured(void)override;
	virtual bool cnLib_FUNC MouseAcquireExclusive(iUIMouseHandler *Handler)override;
	virtual bool cnLib_FUNC MouseReleaseExclusive(iUIMouseHandler *Handler)override;

	// iUITouchControl

	virtual bool cnLib_FUNC InsertTouchHandler(iUITouchHandler *Handler,sfInt16 Order)override;
	virtual bool cnLib_FUNC RemoveTouchHandler(iUITouchHandler *Handler)override;

	virtual bool cnLib_FUNC InsertTouchFilter(iUITouchHandler *Filter,sfInt16 Order)override;
	virtual bool cnLib_FUNC RemoveTouchFilter(iUITouchHandler *Filter)override;

	virtual bool cnLib_FUNC TouchAcquireExclusive(iUITouchHandler *Handler)override;
	virtual bool cnLib_FUNC TouchReleaseExclusive(iUITouchHandler *Handler)override;

	// iWPFViewChild

	virtual cGCRef& WPFChildGetElementHandle(void)override;

	virtual Float32 WPFChildGetLayoutScale(void)override;
	virtual Float32 WPFChildGetZPosition(void)override;
	virtual void WPFChildTreeNotifyWindow(iUIWindow *Window)override;
	virtual void WPFChildTreeNotifyState(void)override;
	virtual void WPFChildTreeNotifyScale(void)override;


	// iWPFViewParent

	virtual eUIState WPFParentGetState(void)override;
	virtual Float32 WPFParentGetContentScale(void)override;
	virtual Float32 WPFParentGetLayoutScale(void)override;


	// functions for bcWPFViewContentDrawing

	bool InsertViewContent(bcWPFViewContentDrawing *Content);
	bool RemoveViewContent(bcWPFViewContentDrawing *Content);
	void UpdateViewContentZPosition(void);
	void UpdateViewContent(void);
	Float32 GetViewContentContentScale(void);

protected:
	void VirtualStarted(void);
	void VirtualStopped(void);

	iPtr<cDNetUIThread> fUIThread;

	bool CheckThread(void);

	cnRTL::cUIViewEventNotification fViewNotification;

	cnRTL::cUIKeyEventNotification fKeyNotification;
	cnRTL::cUIMouseEventNotification fMouseNotification;
	cnRTL::cUITouchEventNotification fTouchNotification;
	
	
	// state


	eUIState fViewState;

	eUIState EvaluateViewState(void);
	bool UpdateViewState(void);
	void NotifyWindowChanged(void);
	void UpdateWindow(iUIWindow *Window);

	// layout

	bool fNeedLayout;


	// input - key
	
	bool fKeyFocusInRangeNotified=false;
	bool fKeyFocusedNotifyValue=false;

	// input - mouse

	bool fMouseEnterNotified=false;
	bool fMouseIsOverNotifyValue=false;

	// contents

	bool fContentOrderCacheUpdate=false;
	cnRTL::cSeqSet<bcWPFViewContentDrawing*> fContents;
	cnRTL::cSeqList<const cGCHandle*> fContentOrderCache;


	void SetupContentOrderCache(void);
	void ClearContentOrderCache(void);
	void ApplyContentChanged(void);


private:
	// functions for mcWPFView

	void WPFNotifyFocusEnterRange(iUIKeyEvent *KeyEvent);
	void WPFNotifyFocusLeaveRange(iUIKeyEvent *KeyEvent);
	void WPFNotifyFocusEnter(iUIKeyEvent *KeyEvent);
	void WPFNotifyFocusLeave(iUIKeyEvent *KeyEvent);
	void WPFNotifyMouseIsDirectlyEnter(iUIMouseEvent *MouseEvent);
	void WPFNotifyMouseIsDirectlyLeave(iUIMouseEvent *MouseEvent);
	void WPFNotifyMouseEnterRange(iUIMouseEvent *MouseEvent);
	void WPFNotifyMouseLeave(iUIMouseEvent *MouseEvent);

	// notification as wpf parent

	void ParentNotifyKeyboardFocusWithin(iWPFViewChild *Subview,iUIKeyEvent *KeyEvent);
	void ParentNotifyKeyboardFocuseEnter(iUIKeyEvent *KeyEvent);
	void ParentNotifyKeyboardFocuseLeave(iUIKeyEvent *KeyEvent);
	void ParentNotifyKeyboardKeyInput(iUIKeyEvent *KeyEvent,eKeyCode Key);
	void ParentNotifyMouseEnter(iWPFViewChild *Subview,iUIMouseEvent *MouseEvent);
	void ParentNotifyMouseIsDirectlyEnter(iUIMouseEvent *MouseEvent);
	void ParentNotifyMouseIsDirectlyLeave(iUIMouseEvent *MouseEvent);
};
//---------------------------------------------------------------------------
}	// namespace cnWin
//---------------------------------------------------------------------------
}	// namespace cnLibrary
//---------------------------------------------------------------------------
#endif  /* __cplusplus */
/*-------------------------------------------------------------------------*/
