
#include <mach/mach.h>
#import <QuartzCore/QuartzCore.h>
#import <QuartzCore/CALayer.h>
//#include <GLKit/GLKit.h>

#include <cnSystem/cnUI.h>
#import "iOS_UIView.h"
#import "iOS_UIApp.h"

using namespace cnLibrary;
using namespace cnRTL;
using namespace cnMac;

//---------------------------------------------------------------------------
#pragma mark Window ViewController
//---------------------------------------------------------------------------
@implementation cnLib_UIViewController
{
@private
	ocStaticIVar<cUIViewControllerWindow> cnLib_IVar;
}

static cUIViewControllerWindow& pControllerWindow(cnLib_UIViewController *self)
{
	return reinterpret_cast<cUIViewControllerWindow&>(self->cnLib_IVar);
}

+(instancetype)alloc
{
	auto p=[super alloc];

	cnVar::ManualConstruct(pControllerWindow(p),p);
	return p;
}

-(void)dealloc
{
	cnVar::ManualDestruct(pControllerWindow(self));
	[super dealloc];
}

-(void)loadView
{
	auto &cw=pControllerWindow(self);
	auto *ClientView=cw.VCGetClientView();
	if(ClientView!=nil){
		self.view=ClientView;
		return;
	}

	return [super loadView];
}

cUIViewControllerWindow* cnMac::cnLib_UIViewController_GetInterface(cnLib_UIViewController *self)
{
	auto &cw=pControllerWindow(self);
	return &cw;
}

@end

#pragma mark Window
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cUIViewControllerWindow::cUIViewControllerWindow(cnLib_UIViewController *HostViewController)
{
	fVC=HostViewController;
	if(@available(iOS 11, *)){
		fCalculator.ConstructAs<cCalculateMarginFromParent_SafeArea>();
	}
	else{
		fCalculator.ConstructAs<cCalculateMarginFromParent_LayoutGuide>();
	}
}
//---------------------------------------------------------------------------
cUIViewControllerWindow::~cUIViewControllerWindow()
{
	if(@available(iOS 11, *)){
		fCalculator.DestructAs<cCalculateMarginFromParent_SafeArea>();
	}
	else{
		fCalculator.DestructAs<cCalculateMarginFromParent_LayoutGuide>();
	}
}
//---------------------------------------------------------------------------
bool cUIViewControllerWindow::CheckUIThread(void)
{
	return [NSThread isMainThread];
}
//---------------------------------------------------------------------------
iUIThread* cUIViewControllerWindow::GetUIThread(void)
{
	return &gMainUIThread;
}
//---------------------------------------------------------------------------
iUIWindow* cUIViewControllerWindow::GetParent(void)
{
	return nullptr;
}
//---------------------------------------------------------------------------
cnLib_UIView* cUIViewControllerWindow::VCGetClientView(void)
{
	return fClientView;
}
//---------------------------------------------------------------------------
cnLib_UIView* cUIViewControllerWindow::GetClientUIView(void)
{
	return fClientView;
}
//---------------------------------------------------------------------------
cUIViewControllerWindow* cUIViewControllerWindow::CalculatorGetHost(cMacUIView::iCalculateMarginFromParent *Interface)
{
	return cnMemory::GetObjectFromMemberPointer(reinterpret_cast<cnVar::cPolymorphicObject<cMacUIView::iCalculateMarginFromParent,
		cCalculateMarginFromParent_SafeArea,
		cCalculateMarginFromParent_LayoutGuide
	>*>(Interface),&cUIViewControllerWindow::fCalculator);
}
//---------------------------------------------------------------------------
cRectangle cUIViewControllerWindow::cCalculateMarginFromParent_SafeArea::Calculate(void)
{
	auto Host=CalculatorGetHost(this);
	return Host->CalculateMarginFromParent_SafeArea();
}
//---------------------------------------------------------------------------
cRectangle cUIViewControllerWindow::CalculateMarginFromParent_SafeArea(void)
{
	Float32 Scale=fClientView.contentScaleFactor;
	auto Insets=fClientView.safeAreaInsets;
	Insets.top*=Scale;
	Insets.bottom*=Scale;
	Insets.left*=Scale;
	Insets.right*=Scale;

	cRectangle Margin;
	Margin.Left=static_cast<sfInt32>(Insets.left);
	Margin.Top=static_cast<sfInt32>(Insets.top);
	Margin.Right=static_cast<sfInt32>(Insets.right);
	Margin.Bottom=static_cast<sfInt32>(Insets.bottom);
	return Margin;
}
//---------------------------------------------------------------------------
cRectangle cUIViewControllerWindow::cCalculateMarginFromParent_LayoutGuide::Calculate(void)
{
	auto Host=CalculatorGetHost(this);
	return Host->CalculateMarginFromParent_LayoutGuide();
}
//---------------------------------------------------------------------------
cRectangle cUIViewControllerWindow::CalculateMarginFromParent_LayoutGuide(void)
{
	cRectangle Margin;
	Margin.Left=0;
	Margin.Right=0;
	Float32 MarginTop=fVC.topLayoutGuide.length;
	Float32 MarginBottom=fVC.bottomLayoutGuide.length;

	Float32 Scale=fClientView.contentScaleFactor;
	MarginTop*=Scale;
	MarginBottom*=Scale;

	Margin.Top=static_cast<sfInt32n>(MarginTop);
	Margin.Bottom=static_cast<sfInt32n>(MarginBottom);
	return Margin;
}
//---------------------------------------------------------------------------
void cUIViewControllerWindow::SetClientUIView(cnLib_UIView *View)
{
	if(fClientView!=nil){
		auto MacClientView=cnLib_UIView_GetUIView(fClientView);
		MacClientView->ResetCalculateMarginFromParent();
	}
	fClientView=View;
	if(fClientView!=nil){
		auto MacClientView=cnLib_UIView_GetUIView(fClientView);
		MacClientView->SetCalculateMarginFromParent(&fCalculator);
	}

	if(fVC.isViewLoaded){
		// view was loaded
		auto OldView=fVC.view;
		auto *Window=OldView.window;

		// remove old view
		Window.rootViewController=nil;


		fVC.view=nil;

		// reload ViewController
		Window.rootViewController=fVC;
	}
}
//---------------------------------------------------------------------------
iUIArea* cUIViewControllerWindow::GetClient(void)
{
	if(fClientView==nil)
		return nullptr;
	return cniOS::iUIViewFromView(fClientView);
}
//---------------------------------------------------------------------------
bool cUIViewControllerWindow::SetClient(iUIArea *Area)
{
	auto ocview=ociSafeCast<cnLib_UIView>(Area);
	if(ocview==nullptr)
		return false;
	SetClientUIView(ocview);
	return true;
}
//---------------------------------------------------------------------------
uIntn cUIViewControllerWindow::FindRelativeBase(cUIPoint &Offset,iUIArea*const*Relatives,uIntn Count)
{
	Offset={0,0};
	if(CheckUIThread()==false)
		return IndexNotFound;

	for(uIntn i=0;i<Count;i++){
		auto *Layout=Relatives[i];
		if(Layout==this){
			return i;
		}
	}
	for(uIntn i=0;i<Count;i++){
		auto OCView=ociSafeCast<cnLib_UIView>(Relatives[i]);
		if(OCView!=nullptr){
			CGPoint p={0,0};
			p=[fVC.view convertPoint:p toView:OCView];
			auto scale=fVC.view.contentScaleFactor;
			Offset.x=p.x*scale;
			Offset.y=p.y*scale;
			return i;
		}
	}

	return IndexNotFound;
}
//---------------------------------------------------------------------------
bool cUIViewControllerWindow::GetPosition(iUIArea *Relative,cUIPoint &Position)
{
	if(CheckUIThread()==false)
		return false;
	if(Relative==this){
		Position={0,0};
		return true;
	}
	if(Relative==nullptr){
		Position={0,0};
		return true;
	}

	auto OCView=ociSafeCast<UIView>(Relative);
	if(OCView!=nullptr){
		CGPoint p={0,0};
		p=[fVC.view convertPoint:p toView:OCView];
		auto scale=fVC.view.contentScaleFactor;
		Position.x=p.x*scale;
		Position.y=p.y*scale;
		return true;
	}
	return false;
}
//---------------------------------------------------------------------------
bool cUIViewControllerWindow::SetPosition(iUIArea *Relative,cUIPoint Position)
{
	return false;
}
//---------------------------------------------------------------------------
cUIPoint cUIViewControllerWindow::GetSize(void)
{
	if(CheckUIThread()==false)
		return {0,0};

	auto FrameSize=fClientView.bounds.size;
	cUIPoint Size;
	Size.x=FrameSize.width;
	Size.y=FrameSize.height;
	return Size;
}
//---------------------------------------------------------------------------
bool cUIViewControllerWindow::SetSize(cUIPoint Size)
{
	return false;
}
//---------------------------------------------------------------------------
bool cUIViewControllerWindow::SetRectangle(iUIArea *Relative,cUIPoint Position,cUIPoint Size)
{
	return false;
}
//---------------------------------------------------------------------------
sfInt16 cUIViewControllerWindow::GetZIndex(void)
{
	return 0;
}
//---------------------------------------------------------------------------
bool cUIViewControllerWindow::SetZIndex(sfInt16 Index)
{
	return false;
}
//---------------------------------------------------------------------------
Float32 cUIViewControllerWindow::GetContentScale(void)
{
	if(fClientView==nil)
		return 1.f;
	return fClientView.contentScaleFactor;
}
//---------------------------------------------------------------------------
void cUIViewControllerWindow::GetFrameMargin(sfInt32n &Left,sfInt32n &Top,sfInt32n &Right,sfInt32n &Bottom)
{
	if(@available(iOS 11, *)){
		if(fClientView!=nullptr){
			Float32 Scale=fClientView.contentScaleFactor;
			auto Insets=fClientView.safeAreaInsets;
			Insets.top*=Scale;
			Insets.bottom*=Scale;
			Insets.left*=Scale;
			Insets.right*=Scale;
			Left=static_cast<sfInt32n>(Insets.left);
			Top=static_cast<sfInt32n>(Insets.top);
			Right=static_cast<sfInt32n>(Insets.right);
			Bottom=static_cast<sfInt32n>(Insets.bottom);
		}
		else{
			Left=0;
			Top=0;
			Right=0;
			Bottom=0;
		}
	}
	else if(@available(iOS 7, *)){
		Left=0;
		Right=0;
		Float32 MarginTop=fVC.topLayoutGuide.length;
		Float32 MarginBottom=fVC.bottomLayoutGuide.length;
		if(fClientView!=nullptr){
			Float32 Scale=fClientView.contentScaleFactor;
			MarginTop*=Scale;
			MarginBottom*=Scale;
		}
		Top=static_cast<sfInt32n>(MarginTop);
		Bottom=static_cast<sfInt32n>(MarginBottom);

	}
	else{
		Left=0;
		Top=0;
		Right=0;
		Bottom=0;
	}
}
//---------------------------------------------------------------------------
#if	0
//---------------------------------------------------------------------------
cUITouchDispatch::cUITouchPoint::cUITouchPoint()
	:fTouch(nullptr)
{
}
//---------------------------------------------------------------------------
cUITouchDispatch::cUITouchPoint::cUITouchPoint(UITouch *Touch)
	:fTouch(Touch)
{
}
//---------------------------------------------------------------------------
cUITouchDispatch::cUITouchPoint::~cUITouchPoint()
{
}
//---------------------------------------------------------------------------
bool cUITouchDispatch::cUITouchPoint::GetPosition(cUIPoint &Position,iUIArea *Relative)
{
	auto View=ociSafeCast<UIView>(Relative);
	if(View==nullptr)
		return false;
	float scale=View.contentScaleFactor;
	auto pos=[fTouch locationInView:View];
	Position.x=pos.x*scale;
	Position.y=pos.y*scale;
	return true;
}
//---------------------------------------------------------------------------
bool cUITouchDispatch::cUITouchPoint::GetPreviousPosition(cUIPoint &Position,iUIArea *Relative)
{
	auto View=ociSafeCast<UIView>(Relative);
	if(View==nullptr)
		return false;
	float scale=View.contentScaleFactor;
	auto pos=[fTouch previousLocationInView:View];
	Position.x=pos.x*scale;
	Position.y=pos.y*scale;
	return true;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cUITouchDispatch::cUITouchDispatch()
{
}
//---------------------------------------------------------------------------
cUITouchDispatch::~cUITouchDispatch()
{
}
//---------------------------------------------------------------------------
bool cUITouchDispatch::TouchExclusive(iUITouchHandler *Handler)
{
	if(fExclusiveHandler!=nullptr)
		return false;
	fExclusiveHandler=Handler;
	return true;
}
//---------------------------------------------------------------------------
void cUITouchDispatch::Dispatch(cnLib_UIView *View,UITouch *Touch,UITouchAction Action)
{
	fCurrentView=View;
	iPtr<cUITouchPoint> tp;
	switch(Action){
	default:
		{
			auto *p=fTouchMap.Get(Touch);
			if(p==nullptr)
				return;
			tp=*p;
		}
		break;
	case UITouchAction::Begin:
		tp=iCreate<cUITouchPoint>(Touch);
		fTouchMap[Touch]=tp;
		break;
	case UITouchAction::End:
	case UITouchAction::Lost:
		{
			auto Pair=fTouchMap.GetPair(Touch);
			if(Pair==nullptr){
				return;
			}
			tp=cnVar::MoveCast(Pair->Value);
			fTouchMap.RemovePair(Pair);
		}

		break;
	}

	auto Sender=cnLib_UIView_GetUIView(View);
	if(fExclusiveHandler!=nullptr){
		if(tp->Exclusived==false){
			// lost in other handler
			auto View=fCurrentView;
			do{
				auto MacView=cnLib_UIView_GetUIView(View);
				MacView->onSendTouchExclusive(Sender,fExclusiveHandler,tp,UITouchAction::Lost);
				View=ocSafeCast<cnLib_UIView>(View.superview);
			}while(View!=nullptr);
			tp->Exclusived=true;
		}
		CallTouchHandler(fExclusiveHandler,Sender,Action,tp);

		if(fTouchMap.GetCount()==0){
			fExclusiveHandler=nullptr;
		}
	}
	else{
		if(tp->Exclusived){
			tp->Exclusived=false;
			// begin in other handler
			auto View=fCurrentView;
			do{
				auto MacView=cnLib_UIView_GetUIView(View);
				MacView->onSendTouchExclusive(Sender,fExclusiveHandler,tp,UITouchAction::Begin);
				View=ocSafeCast<cnLib_UIView>(View.superview);
			}while(View!=nullptr);
		}
		do{
			auto MacView=cnLib_UIView_GetUIView(View);
			MacView->onSendTouch(Sender,tp,Action);
			View=ocSafeCast<cnLib_UIView>(View.superview);
		}while(View!=nullptr);
	}
}
#endif
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
ciOSMainUIThread::ciOSMainUIThread()
{
}
//---------------------------------------------------------------------------
ciOSMainUIThread::~ciOSMainUIThread()
{
}
//---------------------------------------------------------------------------
UIState ciOSMainUIThread::GetAppState(void)const
{
	auto App=[UIApplication sharedApplication];
	auto State=App.applicationState;
	switch(State){
	case UIApplicationStateBackground:
		return UIState::Background;
	case UIApplicationStateInactive:
		return UIState::Inactive;
	case UIApplicationStateActive:
		return UIState::Active;
	}
	return UIState::Null;
}
//---------------------------------------------------------------------------
void ciOSMainUIThread::DidFinishLaunchingWithOptions(void)
{
	OnUIAppStateChanged();
}
void ciOSMainUIThread::WillResignActive(void)
{
	OnUIAppStateChanged();
}
void ciOSMainUIThread::DidEnterBackground(void)
{
	OnUIAppStateChanged();
}
void ciOSMainUIThread::WillEnterForeground(void)
{
	OnUIAppStateChanged();
}
void ciOSMainUIThread::DidBecomeActive(void)
{
	OnUIAppStateChanged();
}
void ciOSMainUIThread::WillTerminate(void)
{
	OnUIAppStateChanged();
}
//---------------------------------------------------------------------------
iUITouchPoint* ciOSMainUIThread::QueryTouchPoint(UITouch *Touch)
{
	auto &TouchPoint=fTouchMap[Touch];
	if(TouchPoint==nullptr){
		TouchPoint=iCreate<cUITouchPoint>(Touch);
	}
	return TouchPoint;
}
//---------------------------------------------------------------------------
iPtr<iUITouchPoint> ciOSMainUIThread::FetchTouchPoint(UITouch *Touch)
{
	auto TouchPoint=fTouchMap.GetPair(Touch);
	if(TouchPoint==nullptr){
		return iCreate<cUITouchPoint>(Touch);
	}
	auto t=cnVar::MoveCast(TouchPoint->Value);
	fTouchMap.RemovePair(TouchPoint);
	return t;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
ciOSUIApplication::ciOSUIApplication()
{
	fStateHandler=nullptr;
}
//---------------------------------------------------------------------------
ciOSUIApplication::~ciOSUIApplication()
{
}
//---------------------------------------------------------------------------
iUIThread* ciOSUIApplication::GetMainUIThread(void)
{
	return &gMainUIThread;
}
//---------------------------------------------------------------------------
void ciOSUIApplication::SetAppHandler(iUIStateHandler *Handler)
{
	fStateHandler=Handler;
}
//---------------------------------------------------------------------------
eUIState ciOSUIApplication::GetAppState(void)
{
	return gMainUIThread.GetAppState();
}
//---------------------------------------------------------------------------
void ciOSUIApplication::DidFinishLaunchingWithOptions(void)
{
	gMainUIThread.DidFinishLaunchingWithOptions();

	if(fStateHandler!=nullptr){
		fStateHandler->UIStarted();
	}
	if(fStateHandler!=nullptr){
		fStateHandler->UIShow();
	}
}
//---------------------------------------------------------------------------
void ciOSUIApplication::WillResignActive(void)
{
	gMainUIThread.WillResignActive();

	if(fStateHandler!=nullptr){
		fStateHandler->UIPaused();
	}
}
//---------------------------------------------------------------------------
void ciOSUIApplication::DidEnterBackground(void)
{
	gMainUIThread.DidEnterBackground();

	if(fStateHandler!=nullptr){
		fStateHandler->UIHide();
	}
}
//---------------------------------------------------------------------------
void ciOSUIApplication::WillEnterForeground(void)
{
	gMainUIThread.WillEnterForeground();

	if(fStateHandler!=nullptr){
		fStateHandler->UIShow();
	}
}
//---------------------------------------------------------------------------
void ciOSUIApplication::DidBecomeActive(void)
{
	gMainUIThread.DidBecomeActive();

	if(fStateHandler!=nullptr){
		fStateHandler->UIResume();
	}
}
//---------------------------------------------------------------------------
void ciOSUIApplication::WillTerminate(void)
{
	gMainUIThread.WillTerminate();

	if(fStateHandler!=nullptr){
		fStateHandler->UIStopped();
	}
}
//---------------------------------------------------------------------------

