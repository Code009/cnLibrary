/*--- Mac - UIView --------------------------------------------------------*/
/*         Developer : Code009                                             */
/*         Create on : 2012-09-09                                          */
/*-------------------------------------------------------------------------*/
#pragma once
/*-------------------------------------------------------------------------*/
#ifdef	__OBJC__
#import <UIKit/UIKit.h>

#include <cnSystem/iOS.h>
#include <cnMac/Mac_QuartzCore.h>
#include <cnRTL/ocRTL.h>
//---------------------------------------------------------------------------
#ifdef	__cplusplus

//---------------------------------------------------------------------------
namespace cnLibrary{


//---------------------------------------------------------------------------
class cOrderedMap
{
public:
	cOrderedMap();
	~cOrderedMap();

protected:

private:

	struct cInsetItem
	{
		cUIRect Rect;
	};
	//cnRTL::cOrderedReferenceSet<iUILayoutHandler*> fContentInsets;
	//cnRTL::cSeqMap<iUILayoutHandler*,cInsetItem> fContentInsetsData;

};

namespace cnMac{
//---------------------------------------------------------------------------
class cMacTouchEvent : public iUITouchEvent
{
public:

	virtual eUIEventSourceType cnLib_FUNC GetSourceType(void)override;
	virtual iUIView* cnLib_FUNC GetSourceView(void)override;
	virtual void cnLib_FUNC CancelEvent(void)override;
	virtual bool cnLib_FUNC IsCancelled(void)override;
	virtual void* cnLib_FUNC GetTouchID(void)override;
	virtual bool cnLib_FUNC GetPosition(iInterface *Relative,cUIPoint &Position)override;

	cnLib_UIView *SenderView;
	UITouch *Touch;
};
//---------------------------------------------------------------------------
class cMacUIView : public iUIView,public iUITouchControl, public iOCObject, public iUIViewCALayerHost,public iReference
{
public:
	cMacUIView(cnLib_UIView *HostView);
	~cMacUIView();
	cMacUIView(const cMacUIView &)=delete;

	virtual void* cnLib_FUNC CastInterface(iTypeID InterfaceID)override;
	virtual void cnLib_FUNC IncreaseReference(void)noexcept override;
	virtual void cnLib_FUNC DecreaseReference(void)noexcept override;

	cnLib_UIView* GetView(void)const;

	bool SetFrameRelativeTo(iUIArea *Relative,CGRect Frame);

	class iCalculateMarginFromParent
	{
	public:
		virtual cUIRectangle Calculate(void)=0;
	};
	void ResetCalculateMarginFromParent(void);
	void SetCalculateMarginFromParent(iCalculateMarginFromParent *Calculator);

// iUIViewCALayerHost

	virtual bool InsertViewCALayer(iUIViewCALayer *ViewLayer)override;
	virtual bool RemoveViewCALayer(iUIViewCALayer *ViewLayer)override;

// cnLib_UIView notify

	void onSetFrame(CGRect LastFrame);
	void onLayoutSubviews(void);
	void onSetContentScale(void);
	void onSetVisible(void);
	void onSetEnabled(void);
	void onWindowWillChange(UIWindow *NewWindow);
	void onWindowChange(void);
	//void onSendTouchExclusive(cMacUIView *Sender,iUITouchHandler *Exclude,iUITouchPoint *TouchPoint,cnRTL::UITouchAction Action);

	void onSendTouchBegin(iUITouchEvent *Event);
	void onSendTouchMove(iUITouchEvent *Event);
	void onSendTouchEnded(iUITouchEvent *Event);
	void onSendTouchLost(iUITouchEvent *Event);

	void onSendTouchFilterBegin(iUITouchEvent *Event);
	void onSendTouchFilterMove(iUITouchEvent *Event);
	void onSendTouchFilterEnded(iUITouchEvent *Event);
	void onSendTouchFilterLost(iUITouchEvent *Event);

// iOCObject methods

	virtual id GetOCObject(void)override;

// iUIArea methods

	virtual iUIThread* cnLib_FUNC GetUIThread(void)override;
	virtual Float32 cnLib_FUNC GetContentScale(void)override;
	virtual cUIPoint cnLib_FUNC GetSize(void)override;
	virtual bool cnLib_FUNC SetSize(cUIPoint Size)override;
	virtual bool cnLib_FUNC TranslatePointTo(iInterface *Relative,cUIPoint &Position)override;
	virtual bool cnLib_FUNC MoveTo(iInterface *Relative,cUIPoint Position)override;
	virtual bool cnLib_FUNC ArrangeRectangle(iInterface *Relative,cUIPoint Position,cUIPoint Size)override;
	virtual Float32 cnLib_FUNC GetZPosition(void)override;
	virtual bool cnLib_FUNC SetZPosition(Float32 Position)override;

// iUIView methods

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

	virtual void cnLib_FUNC SetArrangement(void)override;
	virtual void cnLib_FUNC ArrangeLayout(void)override;
	virtual void cnLib_FUNC SetIncludeParentFrameMargin(bool Include)override;
	virtual cRectangle cnLib_FUNC GetFrameMargin(void)override;

	virtual iUIArea* cnLib_FUNC GetWindow(void)override;
	virtual iUIView* cnLib_FUNC GetParent(void)override;
	virtual bool cnLib_FUNC InsertView(iUIView *View)override;
	virtual bool cnLib_FUNC RemoveView(iUIView *View)override;

// Touch

	virtual bool cnLib_FUNC InsertTouchHandler(iUITouchHandler *Handler,sfInt16 Order)override;
	virtual bool cnLib_FUNC RemoveTouchHandler(iUITouchHandler *Handler)override;

	virtual bool cnLib_FUNC InsertTouchFilter(iUITouchHandler *Filter,sfInt16 Order)override;
	virtual bool cnLib_FUNC RemoveTouchFilter(iUITouchHandler *Filter)override;

	virtual bool cnLib_FUNC TouchAcquireExclusive(iUITouchHandler *Handler)override;
	virtual bool cnLib_FUNC TouchReleaseExclusive(iUITouchHandler *Handler)override;


protected:
	cnLib_UIView *fView;

	iCalculateMarginFromParent *fCalculateMarginFromParent;

	class cCalculateMarginFromParent_Default : public iCalculateMarginFromParent
	{
		virtual cRectangle Calculate(void)override;
	}fCalculateMarginFromParent_Default;
	cRectangle CalculateMarginFromParent_Default(void);

	cnRTL::cSeqSet<iUIStateHandler*> fStateHandlers;
	cnRTL::cSeqSet<iUILayoutHandler*> fLayoutHandlers;
	cnRTL::cSeqSet<iUITouchHandler*> fTouchHandlers;
	cnRTL::cSeqSet<iUITouchHandler*> fTouchFilters;

	// Layers

	CALayer *fViewLayerHost;
	cnRTL::cSeqSet<iUIViewCALayer*> fViewLayers;

	//

	UIState fUIState;
	bool fIncludeParentMargin;
	sfInt16 fZIndex;

	void UpdateUIState(void);

	bool CheckUIFunction(void);

	class cAppStateChangeFunc : public iProcedure
	{
		virtual void cnLib_FUNC Execute(void)override;
	}fAppStateChangeFunc;
	void AppStateChanged(void);

	// Layout

	cRectangle fMargin;

	static UIView* FindNextSiblingForZIndex(UIView *SubView,UIView *Parent,sfInt16 ZIndex);

	// Key Focus
	bool IsFocused(void);
	bool SetFocus(bool Set);

};
cMacUIView* cnLib_UIView_GetUIView(cnLib_UIView *self);
//---------------------------------------------------------------------------
}	// namespace cnMac
}	// namespace cnLibrary
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
#endif	// __cplusplus
//---------------------------------------------------------------------------
#endif	// __OBJC__
/*-------------------------------------------------------------------------*/
