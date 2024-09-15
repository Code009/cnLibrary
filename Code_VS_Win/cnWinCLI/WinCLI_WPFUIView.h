/*- cnWinCLI - WPF - View -------------------------------------------------*/
/*         Developer : Code009                                             */
/*         Create on : 2023-10-07                                          */
/*-------------------------------------------------------------------------*/
#pragma once
/*-------------------------------------------------------------------------*/

#include <cnWinCLI\WinCLI_WPFUICore.h>
#include <cnWinCLI\WinCLIM_WPFUIView.h>

#ifdef __cplusplus
//---------------------------------------------------------------------------
namespace cnLibrary{
//---------------------------------------------------------------------------
namespace cnWin{
//---------------------------------------------------------------------------
class cWPFUIView;
//---------------------------------------------------------------------------
iPtr<iUIView> WPFCreateUIView(void)noexcept(true);
//---------------------------------------------------------------------------
class cnLib_INTERFACE iWPFViewWindow
{
public:
	virtual iUIWindow* WPFWindowInterface(void)noexcept(true)=0;
	virtual iWindow* WPFWindowGetWindow(void)noexcept(true)=0;
	virtual bool WPFWindowIsActive(void)noexcept(true)=0;
};
//---------------------------------------------------------------------------
class cnLib_INTERFACE iWPFViewParent
{
public:
};
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
	cUIRectangle GetContentMargin(void)noexcept(true);
	void SetContentMargin(cUIRectangle Margin)noexcept(true);


	virtual const cGCHandle* GetDrawingPointer(void)noexcept(true)=0;

protected:
	cWPFUIView *fView;

	cUIRectangle fContentMargin;
	Float32 fViewWidth;
	Float32 fViewHeight;
	Float32 fLayoutScale;
	Float32 fZPosition;

	eUIState CheckViewContentState(void)noexcept(true);
	void UpdateDrawing(void)noexcept(true);

	virtual void ViewContentUpdateRect(void)noexcept(true)=0;
	virtual void ViewContentUpdateState(void)noexcept(true)=0;
	virtual void ViewContentUpdateWindow(iWindow *Window)noexcept(true)=0;
private:
	friend cWPFUIView;

	void WPFArrange(Float32 LayoutScale,Float32 Width,Float32 Height)noexcept(true);
};
//---------------------------------------------------------------------------
class cWPFUIView
	: public iUIView, public iUIKeyControl, public iUIMouseControl, public iUITouchControl
	, public mbcWPFUIView, public iCLIObject, protected iWPFViewParent
	, public cnRTL::bcVirtualLifeCycle
{
public:
	cWPFUIView(cWPFUIThread *UIThread,const cGCHandle &WPFUIViewTargetElement)noexcept(true);
	~cWPFUIView()noexcept(true);

	struct tInterfaceID{	static iTypeID Value;	};
	virtual void* cnLib_FUNC CastInterface(iTypeID ID)noexcept(true) override;

	const cGCHandle& WPFGetTargetElementHandle(void)noexcept(true);

	// iCLIObject

	virtual const cGCHandle& cnLib_FUNC GetObjecHandle(void)noexcept(true)override;

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

	// iWPFViewParent

	// functions for bcWPFViewContentDrawing

	bool InsertViewContent(bcWPFViewContentDrawing *Content)noexcept(true);
	bool RemoveViewContent(bcWPFViewContentDrawing *Content)noexcept(true);
	void UpdateViewContentZPosition(void)noexcept(true);
	void UpdateViewContent(void)noexcept(true);
	Float32 GetViewContentContentScale(void)noexcept(true);

	// functions for Subview

	bool WPFChildParentAcquire(iWPFViewParent *Parent)noexcept(true);
	void WPFChildParentRelease(iWPFViewParent *Parent)noexcept(true);

	Float32 WPFChildGetLayoutScale(void)noexcept(true);
	Float32 WPFChildGetZPosition(void)noexcept(true);
	void WPFChildTreeNotifyWindow(iWPFViewWindow *WPFWindow)noexcept(true);
	void WPFChildTreeNotifyState(void)noexcept(true);
	void WPFChildTreeNotifyScale(Float32 LayoutScale,Float32 ContentScale)noexcept(true);

protected:
	void VirtualStarted(void)noexcept(true);
	void VirtualStopped(void)noexcept(true);

	iPtr<cWPFUIThread> fUIThread;

	class cExternalWPFParent : public iWPFViewParent
	{
	public:
	}fExternalWPFParent;

	//virtual iWPFViewParent* WPFUIViewGetWPFViewParent(void)noexcept(true)override;

	virtual cChildrenInfo* WPFSetupChildrenInfo(void* &Handle)noexcept(true)override;
	virtual void WPFClearChildrenInfo(void *Handle)noexcept(true)override;
	virtual bool WPFAttachExtParent(void)noexcept(true)override;
	virtual void WPFDetachExtParent(void)noexcept(true)override;
	virtual void WPFNotifyVisible(bool Visible)noexcept(true)override;
	virtual void WPFNotifyArrange(bool Moved,bool Sized,cUIRect Rect)noexcept(true)override;

	virtual void WPFOnIsKeyboardFocusedChanged(bool Focused)noexcept(true)override;
	virtual void WPFOnIsKeyboardFocusWithinChanged(bool Focused)noexcept(true)override;
	virtual void WPFOnKeyDownFilter(mcWPFKeyEventArgs &KeyEventArgs,eKeyCode KeyCode,bool Repeat)noexcept(true)override;
	virtual void WPFOnKeyDown(mcWPFKeyEventArgs &KeyEventArgs,eKeyCode KeyCode,bool Repeat)noexcept(true)override;
	virtual void WPFOnKeyUp(mcWPFKeyEventArgs &KeyEventArgs,eKeyCode KeyCode)noexcept(true)override;
	virtual void WPFOnKeyUpFilter(mcWPFKeyEventArgs &KeyEventArgs,eKeyCode KeyCode)noexcept(true)override;


	// mouse events from IWPFView

	virtual void WPFOnIsMouseDirectlyOverChanged(bool InRange)noexcept(true)override;
	virtual void WPFOnMouseEnter(mcWPFMouseEventArgs &MouseEventArgs)noexcept(true)override;
	virtual void WPFOnMouseLeave(mcWPFMouseEventArgs &MouseEventArgs)noexcept(true)override;
	virtual void WPFOnMouseMove(mcWPFMouseEventArgs &MouseEventArgs)noexcept(true)override;
	virtual void WPFOnMouseMoveFilter(mcWPFMouseEventArgs &MouseEventArgs)noexcept(true)override;
	virtual void WPFOnMouseDown(mcWPFMouseEventArgs &MouseEventArgs,eMouseButton Button)noexcept(true)override;
	virtual void WPFOnMouseDownFilter(mcWPFMouseEventArgs &MouseEventArgs,eMouseButton Button)noexcept(true)override;
	virtual void WPFOnMouseUp(mcWPFMouseEventArgs &MouseEventArgs,eMouseButton Button)noexcept(true)override;
	virtual void WPFOnMouseUpFilter(mcWPFMouseEventArgs &MouseEventArgs,eMouseButton Button)noexcept(true)override;
	virtual void WPFOnMouseWheel(mcWPFMouseEventArgs &MouseEventArgs,Float32 ScrollX,Float32 ScrollY)noexcept(true)override;
	virtual void WPFOnMouseWheelFilter(mcWPFMouseEventArgs &MouseEventArgs,Float32 ScrollX,Float32 ScrollY)noexcept(true)override;

	// touch events from IWPFView

	virtual void WPFOnTouchDown(mcWPFTouchEventArgs &TouchEventArgs)noexcept(true)override;
	virtual void WPFOnTouchDownFilter(mcWPFTouchEventArgs &TouchEventArgs)noexcept(true)override;
	virtual void WPFOnTouchUp(mcWPFTouchEventArgs &TouchEventArgs)noexcept(true)override;
	virtual void WPFOnTouchUpFilter(mcWPFTouchEventArgs &TouchEventArgs)noexcept(true)override;
	virtual void WPFOnTouchMove(mcWPFTouchEventArgs &TouchEventArgs)noexcept(true)override;
	virtual void WPFOnTouchMoveFilter(mcWPFTouchEventArgs &TouchEventArgs)noexcept(true)override;

	iWPFViewWindow *fWPFWindow=nullptr;
	iWPFViewParent *fWPFParent=nullptr;
	cWPFUIView *fParent=nullptr;
	Float32 fZPosition;
	Float32 fContentScale;

	bool CheckThread(void)noexcept(true);

	cnRTL::cUIViewEventNotification fViewNotification;

	cnRTL::cUIKeyEventNotification fKeyNotification;
	cnRTL::cUIMouseEventNotification fMouseNotification;
	cnRTL::cUITouchEventNotification fTouchNotification;
	
	
	// state

	bool fStateVisible=false;
	bool fLayoutInProgress=false;

	eUIState fViewState;

	eUIState EvaluateViewState(void)noexcept(true);
	bool UpdateViewState(void)noexcept(true);
	void NotifyWindowChanged(void)noexcept(true);
	void UpdateWindow(iWPFViewWindow *WPFWindow)noexcept(true);

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

	// subviews

	struct cSubviewSetItem
	{
		Float32 ZPosition;
		cWPFUIView *Child;
	};
	cnRTL::cLinkMap<cWPFUIView*,cSubviewSetItem> fSubviewMap;
	typedef cnRTL::cLinkMap<cWPFUIView*,cSubviewSetItem>::tContent tSubviewNode;

	struct cSubviewInfo : cChildrenInfo
	{
		cnRTL::cSeqList<mbcWPFUIView*> ChildList;
	};

	// content

	struct cSubviewItemZOrderOperator
	{
		static sfInt8 Compare(const tSubviewNode *Item,const tSubviewNode *Value)noexcept(true);
	};
	cnRTL::cSeqSet<tSubviewNode*,cSubviewItemZOrderOperator> fSubviewZOrderSet;


private:
	void UpdateSubviewZPosition(cWPFUIView *Subview)noexcept(true);
	bool RemoveSubview(cWPFUIView *Subview)noexcept(true);
	void Cleanup(void)noexcept(true);

	class cExitParentProcedure : public iProcedure
	{
		virtual void cnLib_FUNC Execute(void)noexcept(true);
	}fExitParentProcedure;

	// functions for WPFChild

	bool WPFChildParentAcquire(cWPFUIView *Parent)noexcept(true);
	void WPFChildParentRelease(cWPFUIView *Parent)noexcept(true);

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

	void ParentNotifyKeyboardFocusWithin(cWPFUIView *Subview,iUIKeyEvent *KeyEvent)noexcept(true);
	void ParentNotifyKeyboardFocuseEnter(iUIKeyEvent *KeyEvent)noexcept(true);
	void ParentNotifyKeyboardFocuseLeave(iUIKeyEvent *KeyEvent)noexcept(true);
	void ParentNotifyKeyboardKeyInput(iUIKeyEvent *KeyEvent,eKeyCode Key)noexcept(true);
	void ParentNotifyMouseEnter(cWPFUIView *Subview,iUIMouseEvent *MouseEvent)noexcept(true);
	void ParentNotifyMouseIsDirectlyEnter(iUIMouseEvent *MouseEvent)noexcept(true);
	void ParentNotifyMouseIsDirectlyLeave(iUIMouseEvent *MouseEvent)noexcept(true);
};
//---------------------------------------------------------------------------
}	// namespace cnWin
//---------------------------------------------------------------------------
}	// namespace cnLibrary
//---------------------------------------------------------------------------
#endif  /* __cplusplus */
