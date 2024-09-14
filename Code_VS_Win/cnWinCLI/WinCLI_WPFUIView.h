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

	virtual eUIEventSourceType cnLib_FUNC GetSourceType(void)noexcept(true)override;
	virtual iUIView* cnLib_FUNC GetSourceView(void)noexcept(true)override;
	virtual iUIKeyboard* cnLib_FUNC GetKeyboard(void)noexcept(true)override;
	virtual void cnLib_FUNC CancelEvent(void)noexcept(true)override;
	virtual bool cnLib_FUNC IsCancelled(void)noexcept(true)override;
	virtual bool cnLib_FUNC IsKeyDown(eKeyCode KeyCode)noexcept(true)override;
	virtual bool cnLib_FUNC IsKeyToggled(eKeyCode KeyCode)noexcept(true)override;

};
//---------------------------------------------------------------------------
class bcWPFMouseEvent : public iUIMouseEvent
{
public:
	virtual eUIEventSourceType cnLib_FUNC GetSourceType(void)noexcept(true)override;
	virtual iUIMouse* cnLib_FUNC GetMouse(void)noexcept(true)override;
	virtual void cnLib_FUNC CancelEvent(void)noexcept(true)override;
	virtual bool cnLib_FUNC IsCancelled(void)noexcept(true)override;
	virtual bool cnLib_FUNC GetPosition(iInterface *Relative,cUIPoint &Position)noexcept(true)override;
	virtual bool cnLib_FUNC IsButtonDown(eMouseButton Button)noexcept(true)override;

};
//---------------------------------------------------------------------------
class cWPFKeyEventByMessage : public bcWPFKeyEvent
{
public:
	cWPFKeyEventByMessage(iUIView *SourceView)noexcept(true);

	virtual iUIView* cnLib_FUNC GetSourceView(void)noexcept(true)override;


protected:
	iUIView *fSourceView;
};
//---------------------------------------------------------------------------
class cWPFMouseEventByMessage : public bcWPFMouseEvent
{
public:
	cWPFMouseEventByMessage(iUIView *SourceView)noexcept(true);

	virtual iUIView* cnLib_FUNC GetSourceView(void)noexcept(true)override;

protected:
	iUIView *fSourceView;
};
//---------------------------------------------------------------------------
class cWPFKeyEvent : public bcWPFKeyEvent
{
public:
	cWPFKeyEvent(iUIView *SourceView,mcWPFKeyEventArgs &EventArgs)noexcept(true);
	~cWPFKeyEvent()noexcept(true);


	virtual iUIView* cnLib_FUNC GetSourceView(void)noexcept(true)override;
	virtual void cnLib_FUNC CancelEvent(void)noexcept(true)override;
	virtual bool cnLib_FUNC IsCancelled(void)noexcept(true)override;

protected:
	iUIView *fSourceView;
	mcWPFKeyEventArgs &fEventArgs;
};
//---------------------------------------------------------------------------
class cWPFMouseEvent : public bcWPFMouseEvent
{
public:
	cWPFMouseEvent(iUIView *SourceView,mcWPFMouseEventArgs &EventArgs)noexcept(true);
	~cWPFMouseEvent()noexcept(true);

	virtual eUIEventSourceType cnLib_FUNC GetSourceType(void)noexcept(true)override;
	virtual iUIView* cnLib_FUNC GetSourceView(void)noexcept(true)override;
	virtual void cnLib_FUNC CancelEvent(void)noexcept(true)override;
	virtual bool cnLib_FUNC IsCancelled(void)noexcept(true)override;

protected:
	iUIView *fSourceView;
	mcWPFMouseEventArgs &fEventArgs;
};
//---------------------------------------------------------------------------
class cWPFTouchEvent : public iUITouchEvent
{
public:
	cWPFTouchEvent(iUIView *SourceView,mcWPFTouchEventArgs &EventArgs)noexcept(true);
	~cWPFTouchEvent()noexcept(true);

	virtual eUIEventSourceType cnLib_FUNC GetSourceType(void)noexcept(true)override;
	virtual iUIView* cnLib_FUNC GetSourceView(void)noexcept(true)override;
	virtual void* cnLib_FUNC GetTouchID(void)noexcept(true)override;
	virtual void cnLib_FUNC CancelEvent(void)noexcept(true)override;
	virtual bool cnLib_FUNC IsCancelled(void)noexcept(true)override;
	virtual bool cnLib_FUNC GetPosition(iInterface *Relative,cUIPoint &Position)noexcept(true)override;

protected:
	mcWPFTouchEventArgs &fEventArgs;
	iUIView *fSourceView;
	void *fTouchID;
};
//---------------------------------------------------------------------------
class cWPFVisualAsWindow : public iUIWindow, public mcWPFVisual
{
public:
	cWPFVisualAsWindow(mcWPFVisual &&Visual)noexcept(true);
	~cWPFVisualAsWindow()noexcept(true);

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


private:
};
//---------------------------------------------------------------------------
class bcWPFViewContentDrawing
{
public:
	bcWPFViewContentDrawing()noexcept(true);
	~bcWPFViewContentDrawing()noexcept(true);

	cWPFUIView* GetView(void)noexcept(true);
	bool SetView(iUIView *View)noexcept(true);
	bool GetVisible(void)noexcept(true);
	bool SetVisible(bool Visible)noexcept(true);
	Float32 GetZPosition(void)noexcept(true);
	bool SetZPosition(Float32 ZPosition)noexcept(true);
	Float32 GetContentScale(void)noexcept(true);

	virtual cGCRef* GetDrawingPointer(void)noexcept(true)=0;

protected:
	cWPFUIView *fView;

	Float32 fDrawingWidth;
	Float32 fDrawingHeight;
	Float32 fLayoutScale;
	Float32 fZPosition;

	eUIState CheckViewContentState(void)noexcept(true);
	void UpdateDrawing(void)noexcept(true);

	virtual void ViewContentUpdateRect(void)noexcept(true)=0;
	virtual void ViewContentUpdateState(void)noexcept(true)=0;
	virtual void ViewContentUpdateWindow(iWindow *Window)noexcept(true)=0;
private:
	friend cWPFUIView;
	friend mcWPFView;

	void WPFSetSize(Float32 LayoutScale,Float32 Width,Float32 Height)noexcept(true);
};
//---------------------------------------------------------------------------
class cWPFUIView
	: public iUIView, public iUIKeyControl, public iUIMouseControl, public iUITouchControl
	, public iCLIObject , public mcWPFView
	, public cnRTL::bcVirtualLifeCycle
{
	friend mcWPFView;
public:

	cWPFUIView(cDNetUIThread *UIThread,mcConstructParameter &Parameter)noexcept(true);
	~cWPFUIView()noexcept(true);

	struct tInterfaceID{	static iTypeID Value;	};
	virtual void* cnLib_FUNC CastInterface(iTypeID ID)noexcept(true) override;

	// iCLIObject

	virtual cGCRef& cnLib_FUNC GetObjecHandle(void)noexcept(true)override;

	// iUIArea

	virtual iUIThread* cnLib_FUNC GetUIThread(void)noexcept(true)override;
	virtual Float32 cnLib_FUNC GetContentScale(void)noexcept(true)override;
	virtual cUIPoint cnLib_FUNC GetSize(void)noexcept(true)override;
	virtual bool cnLib_FUNC SetSize(cUIPoint Size)noexcept(true)override;
	virtual bool cnLib_FUNC TranslatePointTo(iInterface *Relative,cUIPoint &Position)noexcept(true)override;
	virtual bool cnLib_FUNC MoveTo(iInterface *Relative,cUIPoint Position)noexcept(true)override;
	virtual bool cnLib_FUNC ArrangeRectangle(iInterface *Relative,cUIPoint Position,cUIPoint Size)noexcept(true)override;
	virtual Float32 cnLib_FUNC GetZPosition(void)noexcept(true)override;
	virtual bool cnLib_FUNC SetZPosition(Float32 ZPosition)noexcept(true)override;

	// iUIView

	virtual eUIState cnLib_FUNC GetUIState(void)noexcept(true)override;
	virtual bool cnLib_FUNC InsertStateHandler(iUIStateHandler *Handler,sfInt16 Order)noexcept(true)override;
	virtual bool cnLib_FUNC RemoveStateHandler(iUIStateHandler *Handler)noexcept(true)override;
	virtual bool cnLib_FUNC GetVisible(void)noexcept(true)override;
	virtual bool cnLib_FUNC SetVisible(bool Visible)noexcept(true)override;
	virtual bool cnLib_FUNC IsEnabled(void)noexcept(true)override;
	virtual bool cnLib_FUNC GetEnable(void)noexcept(true)override;
	virtual bool cnLib_FUNC SetEnable(bool Enable)noexcept(true)override;

	virtual bool cnLib_FUNC InsertViewHandler(iUIViewHandler *Handler,sfInt16 Order)noexcept(true)override;
	virtual bool cnLib_FUNC RemoveViewHandler(iUIViewHandler *Handler)noexcept(true)override;

	virtual void cnLib_FUNC ArrangeLayout(void)noexcept(true)override;
	virtual void cnLib_FUNC SetArrangement(void)noexcept(true)override;
	virtual void cnLib_FUNC SetIncludeParentFrameMargin(bool Include)noexcept(true)override;
	virtual cUIRectangle cnLib_FUNC GetFrameMargin(void)noexcept(true)override;

	virtual iUIWindow* cnLib_FUNC GetWindow(void)noexcept(true)override;
	virtual iUIView* cnLib_FUNC GetParent(void)noexcept(true)override;
	virtual bool cnLib_FUNC InsertView(iUIView *View)noexcept(true)override;
	virtual bool cnLib_FUNC RemoveView(iUIView *View)noexcept(true)override;


	// iUIKeyControl

	virtual iUIView* cnLib_FUNC GetFocus(void)noexcept(true)override;
	virtual void cnLib_FUNC SetFocus(bool ActivateWindow)noexcept(true)override;

	virtual bool cnLib_FUNC InsertKeyHandler(iUIKeyHandler *Handler,sfInt16 Order)noexcept(true)override;
	virtual bool cnLib_FUNC RemoveKeyHandler(iUIKeyHandler *Handler)noexcept(true)override;

	virtual bool cnLib_FUNC InsertKeyFilter(iUIKeyHandler *Filter,sfInt16 Order)noexcept(true)override;
	virtual bool cnLib_FUNC RemoveKeyFilter(iUIKeyHandler *Filter)noexcept(true)override;

	virtual bool cnLib_FUNC KeyAcquireExclusive(iUIKeyHandler *Handler)noexcept(true)override;
	virtual bool cnLib_FUNC KeyReleaseExclusive(iUIKeyHandler *Handler)noexcept(true)override;

	virtual bool cnLib_FUNC InsertTextInputHandler(iUITextInputHandler *Handler,sfInt16 Order)noexcept(true)override;
	virtual bool cnLib_FUNC RemoveTextInputHandler(iUITextInputHandler *Handler)noexcept(true)override;
	virtual bool cnLib_FUNC SetCompositionViewInfo(iUITextCompositionViewInfo *CompositionViewInfo)noexcept(true)override;


	// iUIMouseControl

	virtual void cnLib_FUNC MouseUpdateCursor(void)noexcept(true)override;

	virtual bool cnLib_FUNC InsertMouseHandler(iUIMouseHandler *Handler,sfInt16 Order)noexcept(true)override;
	virtual bool cnLib_FUNC RemoveMouseHandler(iUIMouseHandler *Handler)noexcept(true)override;

	virtual bool cnLib_FUNC InsertMouseFilter(iUIMouseHandler *Filter,sfInt16 Order)noexcept(true)override;
	virtual bool cnLib_FUNC RemoveMouseFilter(iUIMouseHandler *Filter)noexcept(true)override;

	virtual bool cnLib_FUNC MouseSetCaptrue(void)noexcept(true)override;
	virtual void cnLib_FUNC MouseReleaseCaptrue(void)noexcept(true)override;
	virtual bool cnLib_FUNC MouseIsCaptured(void)noexcept(true)override;
	virtual bool cnLib_FUNC MouseAcquireExclusive(iUIMouseHandler *Handler)noexcept(true)override;
	virtual bool cnLib_FUNC MouseReleaseExclusive(iUIMouseHandler *Handler)noexcept(true)override;

	// iUITouchControl

	virtual bool cnLib_FUNC InsertTouchHandler(iUITouchHandler *Handler,sfInt16 Order)noexcept(true)override;
	virtual bool cnLib_FUNC RemoveTouchHandler(iUITouchHandler *Handler)noexcept(true)override;

	virtual bool cnLib_FUNC InsertTouchFilter(iUITouchHandler *Filter,sfInt16 Order)noexcept(true)override;
	virtual bool cnLib_FUNC RemoveTouchFilter(iUITouchHandler *Filter)noexcept(true)override;

	virtual bool cnLib_FUNC TouchAcquireExclusive(iUITouchHandler *Handler)noexcept(true)override;
	virtual bool cnLib_FUNC TouchReleaseExclusive(iUITouchHandler *Handler)noexcept(true)override;

	// iWPFViewChild

	virtual cGCRef& WPFChildGetElementHandle(void)noexcept(true)override;

	virtual Float32 WPFChildGetLayoutScale(void)noexcept(true)override;
	virtual Float32 WPFChildGetZPosition(void)noexcept(true)override;
	virtual void WPFChildTreeNotifyWindow(iUIWindow *Window)noexcept(true)override;
	virtual void WPFChildTreeNotifyState(void)noexcept(true)override;
	virtual void WPFChildTreeNotifyScale(void)noexcept(true)override;


	// iWPFViewParent

	virtual eUIState WPFParentGetState(void)noexcept(true)override;
	virtual Float32 WPFParentGetContentScale(void)noexcept(true)override;
	virtual Float32 WPFParentGetLayoutScale(void)noexcept(true)override;


	// functions for bcWPFViewContentDrawing

	bool InsertViewContent(bcWPFViewContentDrawing *Content)noexcept(true);
	bool RemoveViewContent(bcWPFViewContentDrawing *Content)noexcept(true);
	void UpdateViewContentZPosition(void)noexcept(true);
	void UpdateViewContent(void)noexcept(true);
	Float32 GetViewContentContentScale(void)noexcept(true);

protected:
	void VirtualStarted(void)noexcept(true);
	void VirtualStopped(void)noexcept(true);

	iPtr<cDNetUIThread> fUIThread;

	bool CheckThread(void)noexcept(true);

	cnRTL::cUIViewEventNotification fViewNotification;

	cnRTL::cUIKeyEventNotification fKeyNotification;
	cnRTL::cUIMouseEventNotification fMouseNotification;
	cnRTL::cUITouchEventNotification fTouchNotification;
	
	
	// state


	eUIState fViewState;

	eUIState EvaluateViewState(void)noexcept(true);
	bool UpdateViewState(void)noexcept(true);
	void NotifyWindowChanged(void)noexcept(true);
	void UpdateWindow(iUIWindow *Window)noexcept(true);

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


	void SetupContentOrderCache(void)noexcept(true);
	void ClearContentOrderCache(void)noexcept(true);
	void ApplyContentChanged(void)noexcept(true);


private:
	// functions for mcWPFView

	void WPFNotifyFocusEnterRange(iUIKeyEvent *KeyEvent)noexcept(true);
	void WPFNotifyFocusLeaveRange(iUIKeyEvent *KeyEvent)noexcept(true);
	void WPFNotifyFocusEnter(iUIKeyEvent *KeyEvent)noexcept(true);
	void WPFNotifyFocusLeave(iUIKeyEvent *KeyEvent)noexcept(true);
	void WPFNotifyMouseIsDirectlyEnter(iUIMouseEvent *MouseEvent)noexcept(true);
	void WPFNotifyMouseIsDirectlyLeave(iUIMouseEvent *MouseEvent)noexcept(true);
	void WPFNotifyMouseEnterRange(iUIMouseEvent *MouseEvent)noexcept(true);
	void WPFNotifyMouseLeave(iUIMouseEvent *MouseEvent)noexcept(true);

	// notification as wpf parent

	void ParentNotifyKeyboardFocusWithin(iWPFViewChild *Subview,iUIKeyEvent *KeyEvent)noexcept(true);
	void ParentNotifyKeyboardFocuseEnter(iUIKeyEvent *KeyEvent)noexcept(true);
	void ParentNotifyKeyboardFocuseLeave(iUIKeyEvent *KeyEvent)noexcept(true);
	void ParentNotifyKeyboardKeyInput(iUIKeyEvent *KeyEvent,eKeyCode Key)noexcept(true);
	void ParentNotifyMouseEnter(iWPFViewChild *Subview,iUIMouseEvent *MouseEvent)noexcept(true);
	void ParentNotifyMouseIsDirectlyEnter(iUIMouseEvent *MouseEvent)noexcept(true);
	void ParentNotifyMouseIsDirectlyLeave(iUIMouseEvent *MouseEvent)noexcept(true);
};
//---------------------------------------------------------------------------
}	// namespace cnWin
//---------------------------------------------------------------------------
}	// namespace cnLibrary
//---------------------------------------------------------------------------
#endif  /* __cplusplus */
/*-------------------------------------------------------------------------*/
