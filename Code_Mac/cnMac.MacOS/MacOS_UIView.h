/*--- Mac - NSView --------------------------------------------------------*/
/*         Developer : Code009                                             */
/*         Create on : 2012-09-09                                          */
/*-------------------------------------------------------------------------*/
#pragma once
/*-------------------------------------------------------------------------*/
#include <cnSystem/cnUI.h>

#ifdef	__OBJC__

#include <cnSystem/MacOS.h>
#include <cnMac/Mac_QuartzCore.h>
#include <cnRTL/ocRTL.h>

//---------------------------------------------------------------------------
#ifdef	__cplusplus
//---------------------------------------------------------------------------
namespace cnLibrary{
namespace cnMac{
//---------------------------------------------------------------------------
class cMacNSView : public iUIView
	, public iOCObject
	, public iUIKeyControl
	, public iUIMouseControl
	, public iUITouchControl
	, public iUIViewCALayerHost
	, public iReference
{
public:
	cMacNSView(cnLib_NSView *HostView);
	~cMacNSView();

	cMacNSView(const cMacNSView&)=delete;

	virtual const void* cnLib_FUNC CastInterface(iTypeID InterfaceID)const override;

	bool CheckIsEnabled(void);

// cnLib_NSView notify
	void onSetFrame(CGRect LastFrame);
	void onLayoutSubviews(void);
	void onSetVisible(void);
	void onSetEnabled(void);
	//void onWindowWillChange(UIWindow *NewWindow);
	void onWindowChange(void);
	//void onSuperViewChange(void);

	void onBecomeFirstResponder(void);
	void onResignFirstResponder(void);

	void onKeyDown(NSEvent *Event);
	void onKeyUp(NSEvent *Event);

	void onMouseMove(NSEvent *Event,eMouseButton Button);
	void onMouseDown(NSEvent *Event,eMouseButton Button);
	void onMouseUp(NSEvent *Event,eMouseButton Button);
	void onMouseWheel(NSEvent *Event);

	void onTouchBegin(NSEvent *Event);
	void onTouchEnd(NSEvent *Event);
	void onTouchMove(NSEvent *Event);
	void onTouchCancelled(NSEvent *Event);

	bool GetViewRelativePosition(iUIArea *Relative,cUIPoint &Position,NSPoint ViewPos);

// iUIViewCALayerHost

	virtual bool InsertViewCALayer(iUIViewCALayer *ViewLayer)override;
	virtual bool RemoveViewCALayer(iUIViewCALayer *ViewLayer)override;

// iOCObject methods

	virtual id GetOCObject(void)override;

// iUIArea methods

	virtual iUIThread* cnLib_FUNC GetUIThread(void)override;
	virtual uIntn cnLib_FUNC FindRelativeBase(cUIPoint &Offset,iUIArea*const *Relatives,uIntn Count)override;
	virtual bool cnLib_FUNC GetPosition(iUIArea *Relative,cUIPoint &Position)override;
	virtual bool cnLib_FUNC SetPosition(iUIArea *Relative,cUIPoint Position)override;
	virtual cUIPoint cnLib_FUNC GetSize(void)override;
	virtual bool cnLib_FUNC SetSize(cUIPoint Size)override;
	virtual bool cnLib_FUNC SetRectangle(iUIArea *Relative,cUIPoint Position,cUIPoint Size)override;
	virtual sfInt16 cnLib_FUNC GetZIndex(void)override;
	virtual bool cnLib_FUNC SetZIndex(sfInt16 Index)override;
	virtual Float32 cnLib_FUNC GetContentScale(void)override;

// iUIView methods

	virtual eUIState cnLib_FUNC GetUIState(void)override;
	virtual bool cnLib_FUNC InsertStateHandler(iUIStateHandler *Handler,sfInt16 Order)override;
	virtual bool cnLib_FUNC RemoveStateHandler(iUIStateHandler *Handler)override;
	virtual bool cnLib_FUNC GetVisible(void)override;
	virtual bool cnLib_FUNC SetVisible(bool Visible)override;
	virtual bool cnLib_FUNC IsEnabled(void)override;
	virtual bool cnLib_FUNC GetEnable(void)override;
	virtual bool cnLib_FUNC SetEnable(bool Enable)override;
	virtual bool cnLib_FUNC InsertLayoutHandler(iUILayoutHandler *Handler,sfInt16 Order)override;
	virtual bool cnLib_FUNC RemoveLayoutHandler(iUILayoutHandler *Handler)override;
	virtual void cnLib_FUNC SetArrangement(void)override;
	virtual void cnLib_FUNC ArrangeLayout(void)override;
	virtual void cnLib_FUNC SetIncludeParentFrameMargin(bool Include)override;
	virtual cRectangle cnLib_FUNC GetFrameMargin(void)override;

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
	virtual void cnLib_FUNC KeyReleaseExclusive(iUIKeyHandler *Handler)override;

	virtual bool cnLib_FUNC InsertTextInputHandler(iUITextInputHandler *Handler,sfInt16 Order)override;
	virtual bool cnLib_FUNC RemoveTextInputHandler(iUITextInputHandler *Handler)override;
	virtual bool cnLib_FUNC SetCompositionViewInfo(iUITextCompositionViewInfo *CompositionViewInfo)override;

// iUIMouseControl

	virtual void cnLib_FUNC MouseUpdateCursor(void)override;

	virtual bool cnLib_FUNC InsertMouseHandler(iUIMouseHandler *Handler,sfInt16 Order)override;
	virtual bool cnLib_FUNC RemoveMouseHandler(iUIMouseHandler *Handler)override;

	virtual bool cnLib_FUNC InsertMouseFilter(iUIMouseHandler *Filter,sfInt16 Order)override;
	virtual bool cnLib_FUNC RemoveMouseFilter(iUIMouseHandler *Filter)override;

	virtual sfInt16n cnLib_FUNC GetWheelScale(void)override;
	virtual bool cnLib_FUNC MouseSetCaptrue(void)override;
	virtual void cnLib_FUNC MouseReleaseCaptrue(void)override;
	virtual bool cnLib_FUNC MouseIsCaptured(void)override;
	virtual bool cnLib_FUNC MouseAcquireExclusive(iUIMouseHandler *Handler)override;
	virtual void cnLib_FUNC MouseReleaseExclusive(iUIMouseHandler *Handler)override;

// iUITouchControl

	virtual bool cnLib_FUNC InsertTouchHandler(iUITouchHandler *Handler,sfInt16 Order)override;
	virtual bool cnLib_FUNC RemoveTouchHandler(iUITouchHandler *Handler)override;

	virtual bool cnLib_FUNC InsertTouchFilter(iUITouchHandler *Filter,sfInt16 Order)override;
	virtual bool cnLib_FUNC RemoveTouchFilter(iUITouchHandler *Filter)override;

	virtual bool cnLib_FUNC TouchAcquireExclusive(iUITouchHandler *Handler)override;
	virtual void cnLib_FUNC TouchReleaseExclusive(iUITouchHandler *Handler)override;

protected:
	cnLib_NSView *fView=nil;

	bool CheckUIFunction(void);
	bool IsViewVisible(void)const;

	cnRTL::cSeqSet<iUIStateHandler*> fStateHandlers;
	cnRTL::cSeqSet<iUILayoutHandler*> fLayoutHandlers;
	cnRTL::cSeqSet<iUIKeyHandler*> fKeyHandlers;
	cnRTL::cSeqSet<iUIKeyHandler*> fKeyFilters;
	cnRTL::cSeqSet<iUIMouseHandler*> fMouseHandlers;
	cnRTL::cSeqSet<iUIMouseHandler*> fMouseFilters;
	cnRTL::cSeqSet<iUITouchHandler*> fTouchHandlers;
	cnRTL::cSeqSet<iUITouchHandler*> fTouchFilters;

	CALayer *fViewLayerHost;
	cnRTL::cSeqSet<iUIViewCALayer*> fViewLayers;

	void ViewLayersSetFrame(CGRect Frame);
	void ViewLayersSetScale(CGFloat Scale);

public:
	bool ocInitialized;
protected:
	UIState fUIState;
	bool fUIEnabled;
	bool fIsFirstResponder;
	bool fIncludeParentMargin=false;

	sfInt16 fZIndex=0;

	void UpdateUIState(void);

	template<class TCaller>
	void NotifyLayoutHandler(TCaller &&Caller);
	void UpdatePainterRect(void);

	cRectangle fMargin;

	cRectangle CalculateMarginFromParent_Default(void);
	static NSView* FindNextSiblingForZIndex(NSView *SubView,NSView *Parent,sfInt16 ZIndex);


	virtual void cnLib_FUNC IncReference(void)override;
	virtual void cnLib_FUNC DecReference(void)override;

	cnRTL::iFunctionToken *fAppStateChangeToken;
	void AppStateChanged(void);

	class cUIKeyEvent : public iUIKeyEvent
	{
	public:
		cMacNSView *Owner;
		virtual iUIView* cnLib_FUNC GetSourceView(void)override;
		virtual iUIKeyboard* cnLib_FUNC GetKeyboard(void)override;
		virtual void cnLib_FUNC CancelEvent(void)override;
		virtual bool cnLib_FUNC IsKeyDown(eKeyCode KeyCode)override;
		virtual bool cnLib_FUNC IsKeyToggled(eKeyCode KeyCode)override;
	};

	template<class TFunc>
	void ForEachMouseHandlers(TFunc &&Operator);
	
	class cUIMouseEvent : public iUIMouseEvent
	{
	public:
		cMacNSView *Owner;
		NSEvent *Event;
		virtual eUIEventSourceType cnLib_FUNC GetSourceType(void)override;
		virtual iUIView* cnLib_FUNC GetSourceView(void)override;
		virtual iUIMouse* cnLib_FUNC GetMouse(void)override;
		virtual void cnLib_FUNC CancelEvent(void)override;
		virtual bool cnLib_FUNC GetPosition(iUIArea *Relative,cUIPoint &Position)override;
		virtual bool cnLib_FUNC IsButtonDown(eMouseButton Button)override;
	};
	iUIMouseHandler *fMouseExclusiveHandler;

	class cTouchPoint : public iUITouchPoint
	{
	public:
		cTouchPoint();
		~cTouchPoint();

		ocPtr<NSTouch> Touch;
	};
	template<class TFunc>
	void ForEachTouchHandlers(TFunc &&Operator);

	class cTouchPadEvent : public iUITouchEvent
	{
	public:
		cMacNSView *Owner;
		NSEvent *Event;
		cTouchPoint *TouchPoint;
		virtual iUIView* cnLib_FUNC GetSourceView(void)override;
		virtual iUITouchPoint* cnLib_FUNC GetTouchPoint(void)override;
		virtual bool cnLib_FUNC GetPosition(iUIArea *Relative,cUIPoint &Position)override;
	protected:
		NSPoint fEventMousePos;
		bool fEventMousePosValid=false;

	};
	cnRTL::cSeqMap<id,iPtr<cTouchPoint> > fTouchPadMap;
	NSPoint fTouchPadRelativeDevicePos;
	cTouchPoint* TouchPadInsert(NSTouch *Touch);
	iPtr<cTouchPoint> TouchPadRemove(NSTouch *Touch);
};
//---------------------------------------------------------------------------
cMacNSView* cnLib_NSView_GetUIView(cnLib_NSView *View);
//---------------------------------------------------------------------------
}	// namespace cnMac
}	// namespace cnLibrary
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
#endif	// __cplusplus
//---------------------------------------------------------------------------
#endif	// __OBJC__
/*-------------------------------------------------------------------------*/
