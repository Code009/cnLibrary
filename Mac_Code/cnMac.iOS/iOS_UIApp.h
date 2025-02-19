/*--- Mac - UI ------------------------------------------------------------*/
/*         Developer : Code009                                             */
/*         Create on : 2012-09-09                                          */
/*-------------------------------------------------------------------------*/
#pragma once
/*-------------------------------------------------------------------------*/

#ifdef	__OBJC__
#import <UIKit/UIKit.h>

#include <cnSystem/iOS_UI.h>
//---------------------------------------------------------------------------
#ifdef	__cplusplus

#include <cnMac/Mac_UIThread.h>
#include <cnRTL/ocRTL.h>
#include <cnMac.iOS/iOS_UIView.h>
//---------------------------------------------------------------------------
namespace cnLibrary{
namespace cnMac{
//---------------------------------------------------------------------------
class cUIViewControllerWindow : public iUIWindow
{
public:
	cUIViewControllerWindow(cnLib_UIViewController *HostViewController);
	~cUIViewControllerWindow();

	cnLib_UIView* GetClientUIView(void);
	void SetClientUIView(cnLib_UIView *View);

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

// iUIWindow methods

	virtual iUIWindow* cnLib_FUNC GetParent(void)override;
	virtual iUIArea* cnLib_FUNC GetClient(void)override;
	virtual bool cnLib_FUNC SetClient(iUIArea *Area)override;


	void GetFrameMargin(sfInt32n &Left,sfInt32n &Top,sfInt32n &Right,sfInt32n &Bottom);

	cnLib_UIView* VCGetClientView(void);

protected:
	cnLib_UIViewController *fVC;
	cnLib_UIView *fClientView=nil;

	bool CheckUIThread(void);

	void SetArrangement(void);
	void ArrangeLayout(void);


	class API_AVAILABLE(ios(11.0)) cCalculateMarginFromParent_SafeArea : public cMacUIView::iCalculateMarginFromParent
	{
		virtual cRectangle Calculate(void)override;
	};
	cRectangle CalculateMarginFromParent_SafeArea(void) API_AVAILABLE(ios(11.0));

	class cCalculateMarginFromParent_LayoutGuide : public cMacUIView::iCalculateMarginFromParent
	{
		virtual cRectangle Calculate(void)override;
	};
	cRectangle CalculateMarginFromParent_LayoutGuide(void);

	cnVar::cPolymorphicObject<cMacUIView::iCalculateMarginFromParent,
		cCalculateMarginFromParent_SafeArea,
		cCalculateMarginFromParent_LayoutGuide
	> fCalculator;
	static cUIViewControllerWindow* CalculatorGetHost(cMacUIView::iCalculateMarginFromParent *Interface);
};
//---------------------------------------------------------------------------
cUIViewControllerWindow* cnLib_UIViewController_GetInterface(cnLib_UIViewController *self);
//---------------------------------------------------------------------------
#if	0
class cUITouchDispatch
{
private:
	class cUITouchPoint : public iUITouchPoint
	{
	private:
		cnRTL::ocPtr<UITouch> fTouch;
	public:
		cUITouchPoint();
		cUITouchPoint(UITouch *Touch);
		~cUITouchPoint();

		bool Exclusived=false;
		virtual bool cnLib_FUNC GetPosition(cUIPoint &Position,iUIArea *Relative)override;
		virtual bool cnLib_FUNC GetPreviousPosition(cUIPoint &Position,iUIArea *Relative)override;
	};
	cnRTL::cSeqMap<UITouch*,iPtr<cUITouchPoint> > fTouchMap;

	cnLib_UIView *fCurrentView;
	iUITouchHandler *fExclusiveHandler=nullptr;
public:
	cUITouchDispatch();
	~cUITouchDispatch();

	bool TouchExclusive(iUITouchHandler *Handler);

	void Dispatch(cnLib_UIView *View,UITouch *Touch,cnRTL::UITouchAction Action);
};
#endif
//---------------------------------------------------------------------------
class ciOSMainUIThread : public cUIThread
{
public:
	ciOSMainUIThread();
	~ciOSMainUIThread();
	//cUITouchDispatch TouchDispatch;

	UIState GetAppState(void)const;

	cnRTL::cFunctionSet<void (void)> OnUIAppStateChanged;

	iUITouchPoint* QueryTouchPoint(UITouch *Touch);
	iPtr<iUITouchPoint> FetchTouchPoint(UITouch *Touch);

	void DidFinishLaunchingWithOptions(void);
	void WillResignActive(void);
	void DidEnterBackground(void);
	void WillEnterForeground(void);
	void DidBecomeActive(void);
	void WillTerminate(void);

private:
	class cUITouchPoint : public iUITouchPoint
	{
	private:
		cnRTL::ocPtr<UITouch> fTouch;
	public:
		cUITouchPoint(UITouch *Touch):fTouch(Touch){}
		~cUITouchPoint(){}
	};
	cnRTL::cSeqMap<ocPtr<UITouch>,iPtr<cUITouchPoint> > fTouchMap;
};
extern ciOSMainUIThread gMainUIThread;
//---------------------------------------------------------------------------
class ciOSUIApplication : public iiOSUIApplication
{
public:
	ciOSUIApplication();
	~ciOSUIApplication();

	virtual iUIThread* GetMainUIThread(void)override;
	virtual void SetAppHandler(iUIStateHandler *Handler)override;
	virtual eUIState GetAppState(void)override;

	virtual void DidFinishLaunchingWithOptions(void)override;
	virtual void WillResignActive(void)override;
	virtual void DidEnterBackground(void)override;
	virtual void WillEnterForeground(void)override;
	virtual void DidBecomeActive(void)override;
	virtual void WillTerminate(void)override;
private:
	iUIStateHandler *fStateHandler;
};
//---------------------------------------------------------------------------
}	// namespace cnMac
}	// namespace cnLibrary
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
#endif	// __cplusplus
//---------------------------------------------------------------------------
#endif	// __OBJC__
/*-------------------------------------------------------------------------*/
