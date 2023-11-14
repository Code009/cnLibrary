#import <dispatch/dispatch.h>
#import <QuartzCore/QuartzCore.h>

#include <cnRTL/cnRTL.h>
#include <cnRTL/ocRTL.h>
#import "MacOS_UIView.h"
#import "MacOS_UIApp.h"

using namespace cnLibrary;
using namespace cnRTL;
using namespace cnMac;

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//@interface cnLib_NSView()<CALayerDelegate>
//@end

@implementation cnLib_NSView
{
@private
	ocStaticIVar<cMacNSView> cnLib_IVar;
}

static cMacNSView* CPPMacNSView(cnLib_NSView *self)
{
	return reinterpret_cast<cMacNSView*>(&self->cnLib_IVar);
}
static cMacNSView& pMacNSView(cnLib_NSView *self)
{
	return reinterpret_cast<cMacNSView&>(self->cnLib_IVar);
}

-(instancetype)initWithFrame:(NSRect)frameRect
{
	self=[super initWithFrame:frameRect];
	if(self!=nil){
		cnVar::ManualConstruct(pMacNSView(self),self);
	}
	return self;
}
-(instancetype)initWithCoder:(NSCoder *)coder
{
	self=[super initWithCoder:coder];
	if(self!=nil){
		cnVar::ManualConstruct(pMacNSView(self),self);
	}
	return self;
}

-(void)dealloc
{
	cnVar::ManualDestruct(pMacNSView(self));
	[super dealloc];
}

//-(void)drawRect:(NSRect)dirtyRect
// no content in View itself


#pragma mark property

-(void)setFrame:(NSRect)frame
{
	auto &MacView=pMacNSView(self);
	if(MacView.ocInitialized==false){
		return [super setFrame:frame];
	}
	auto LastFrame=[super frame];
	[super setFrame:frame];

	MacView.onSetFrame(LastFrame);
}

-(void)layout
{
	[super layout];

	auto &MacView=pMacNSView(self);
	MacView.onLayoutSubviews();
}

-(BOOL)isFlipped
{
	return YES;
}


/*

-(void)setUserInteractionEnabled:(BOOL)userInteractionEnabled
{
	[super setUserInteractionEnabled:userInteractionEnabled];

	auto MacUIView=pMacUIView(self);
	if(MacUIView!=nullptr){
		MacUIView->onSetEnabled();
	}
}
*/
-(void)setHidden:(BOOL)hidden
{
	[super setHidden:hidden];

	auto &MacView=pMacNSView(self);
	MacView.onSetVisible();
}

-(void)viewDidMoveToWindow
{
	[super viewDidMoveToWindow];

	auto &MacUIView=pMacNSView(self);
	MacUIView.onWindowChange();
}

#if	0
-(void)viewDidMoveToSuperview
{
	[super viewDidMoveToSuperview];
	auto &MacUIView=pMacNSView(self);
	MacUIView.onSuperViewChange();
}
#endif


#pragma mark first responder


-(BOOL)acceptsFirstResponder
{
	auto *CPP=CPPMacNSView(self);
	return CPP->CheckIsEnabled();
}

-(BOOL)becomeFirstResponder
{
	auto *CPP=CPPMacNSView(self);
	CPP->onBecomeFirstResponder();
	return YES;
}

-(BOOL)resignFirstResponder
{
	auto *CPP=CPPMacNSView(self);
	CPP->onResignFirstResponder();
	return YES;
}


#pragma mark keyboard

-(void)keyDown:(NSEvent *)event
{
	auto *CPP=CPPMacNSView(self);
	CPP->onKeyDown(event);
}
-(void)keyUp:(NSEvent *)event
{
	auto *CPP=CPPMacNSView(self);
	CPP->onKeyUp(event);
}


#pragma mark mouse

-(void)mouseDown:(NSEvent *)event
{
	auto *CPP=CPPMacNSView(self);
	CPP->onMouseDown(event,MouseButton::Left);
}

-(void)mouseUp:(NSEvent *)event
{
	auto *CPP=CPPMacNSView(self);
	CPP->onMouseUp(event,MouseButton::Left);
}

-(void)mouseDragged:(NSEvent *)event
{
	auto *CPP=CPPMacNSView(self);
	CPP->onMouseMove(event,MouseButton::Left);
}

-(void)rightMouseDown:(NSEvent *)event
{
	auto *CPP=CPPMacNSView(self);
	CPP->onMouseDown(event,MouseButton::Right);
}

-(void)rightMouseUp:(NSEvent *)event
{
	auto *CPP=CPPMacNSView(self);
	CPP->onMouseUp(event,MouseButton::Right);
}

-(void)rightMouseDragged:(NSEvent *)event
{
	auto *CPP=CPPMacNSView(self);
	CPP->onMouseMove(event,MouseButton::Right);
}

-(void)otherMouseDown:(NSEvent *)event
{
	auto *CPP=CPPMacNSView(self);
	CPP->onMouseDown(event,MouseButton::Middle);
}

-(void)otherMouseUp:(NSEvent *)event
{
	auto *CPP=CPPMacNSView(self);
	CPP->onMouseUp(event,MouseButton::Middle);
}

-(void)otherMouseDragged:(NSEvent *)event
{
	auto *CPP=CPPMacNSView(self);
	CPP->onMouseMove(event,MouseButton::Middle);
}

-(void)scrollWheel:(NSEvent *)event
{
	auto *CPP=CPPMacNSView(self);
	CPP->onMouseWheel(event);
}

#pragma mark tracking area mouse

-(void)mouseEntered:(NSEvent *)event
{
	return [super mouseEntered:event];
}

-(void)mouseExited:(NSEvent *)event
{
	return [super mouseExited:event];
}

-(void)mouseMoved:(NSEvent *)event
{
	return [super mouseMoved:event];
}


#pragma mark touches

-(void)touchesBeganWithEvent:(NSEvent *)event
{
	auto *CPP=CPPMacNSView(self);
	CPP->onTouchBegin(event);
}

-(void)touchesEndedWithEvent:(NSEvent *)event
{
	auto *CPP=CPPMacNSView(self);
	CPP->onTouchEnd(event);
}

-(void)touchesMovedWithEvent:(NSEvent *)event
{
	auto *CPP=CPPMacNSView(self);
	CPP->onTouchMove(event);
}

-(void)touchesCancelledWithEvent:(NSEvent *)event
{
	auto *CPP=CPPMacNSView(self);
	CPP->onTouchCancelled(event);
}

#pragma mark interface

cMacNSView* cnMac::cnLib_NSView_GetUIView(cnLib_NSView *View)
{
	return CPPMacNSView(View);
}

@end	// cnLib_UIView

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cMacNSView::cTouchPoint::cTouchPoint()
{
}
//---------------------------------------------------------------------------
cMacNSView::cTouchPoint::~cTouchPoint()
{
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cMacNSView::cMacNSView(cnLib_NSView *HostView)
{
	ocInitialized=true;
	fUIEnabled=true;
	fIsFirstResponder=false;
	fView=HostView;
	fView.wantsLayer=YES;

	fViewLayerHost=[[CALayer alloc]init];
	[fView.layer addSublayer:fViewLayerHost];

	UpdateUIState();

	fAppStateChangeToken=gMainUIThread.OnUIAppStateChanged.Insert([this]{
		AppStateChanged();
	});
}
//---------------------------------------------------------------------------
cMacNSView::~cMacNSView()
{
//	cnLib_ASSERT(fViewLayerHost==nil);
	[fViewLayerHost removeFromSuperlayer];
	[fViewLayerHost release];


	gMainUIThread.OnUIAppStateChanged.Remove(fAppStateChangeToken);

	auto HandlerCount=fLayoutHandlers.GetCount();
	if(HandlerCount!=0 && fUIState!=UIState::Null){
		cUIStateHandlerSetNotifier Notifier(fStateHandlers);
		UIStateNotifyChange(fUIState,UIState::Null,Notifier);
	}
}
//---------------------------------------------------------------------------
void cnLib_FUNC cMacNSView::IncReference(void)
{
	[fView retain];
}
void cnLib_FUNC cMacNSView::DecReference(void)
{
	[fView release];
}
//---------------------------------------------------------------------------
id cnLib_FUNC cMacNSView::GetOCObject(void)
{
	return fView;
}
//---------------------------------------------------------------------------
const void* cnLib_FUNC cMacNSView::CastInterface(iTypeID InterfaceID)const
{
	return ImpCastInterface<
			iReference,iUIView,iOCObject,
			iUIKeyControl,iUIMouseControl,iUITouchControl,
			iUIViewCALayerHost
		>(this,InterfaceID);
}
//---------------------------------------------------------------------------
template<class TCaller>
void cMacNSView::NotifyLayoutHandler(TCaller &&Caller)
{
	for(auto &p : fLayoutHandlers){
		Caller(p);
	}
}
//---------------------------------------------------------------------------
void cMacNSView::UpdatePainterRect(void)
{
}
//---------------------------------------------------------------------------
bool cMacNSView::InsertViewCALayer(iUIViewCALayer *ViewLayer)
{
	auto *Layer=ocSafeCast<CALayer>(ViewLayer->GetOCObject());
	if(Layer==nil)
		return false;
	if(fViewLayers.Insert(ViewLayer)==false)
		return false;
		/*
	switch(fViewLayers.GetCount()){
	case 1:
		// one layer directly inserted
		fViewLayerHost=Layer;
		[fView.layer insertSublayer:Layer atIndex:0];
		break;
	case 2:
		{
			// move layers to one host

			auto LastLayer=fViewLayerHost;
			[LastLayer removeFromSuperlayer];
			// make layer
			fViewLayerHost=[[CALayer alloc]init];
			[fViewLayerHost addSublayer:LastLayer];

			[fView.layer insertSublayer:fViewLayerHost atIndex:0];
		}
		//[[fallthrough]];
	*/
	//default:
		// add to layer host
		[fViewLayerHost addSublayer:Layer];
	//	break;
	//}

	ViewLayer->SetLayerScale(fView.layer.contentsScale);

	return true;
}
//---------------------------------------------------------------------------
bool cMacNSView::RemoveViewCALayer(iUIViewCALayer *ViewLayer)
{
	if(fViewLayers.Remove(ViewLayer)==false)
		return false;
	CALayer *Layer=ViewLayer->GetOCObject();
	[Layer removeFromSuperlayer];
	/*switch(fViewLayers.GetCount()){
	case 0:
		fViewLayerHost=nil;
		break;
	case 1:
		{
			// fallback to one layer only
			auto LastViewLayer=fViewLayers.GetList()[0];
			CALayer *LastLayer=LastViewLayer->GetOCObject();
			[LastLayer removeFromSuperlayer];

			[fViewLayerHost removeFromSuperlayer];
			[fViewLayerHost release];

			fViewLayerHost=LastLayer;
			[fView.layer insertSublayer:fViewLayerHost atIndex:0];
		}
		break;
	default:
		break;
	}*/
	return true;
}
//---------------------------------------------------------------------------
void cMacNSView::onSetFrame(CGRect LastFrame)
{
	auto NewFrame=fView.frame;
	bool Moved=!CGPointEqualToPoint(LastFrame.origin,NewFrame.origin);
	bool Sized=!CGSizeEqualToSize(LastFrame.size,NewFrame.size);
	if(Moved || Sized){
		NewFrame.origin={0,0};
		fViewLayerHost.frame=NewFrame;
		NotifyLayoutHandler([=](iUILayoutHandler *Handler){
			Handler->RectangleChanged(Moved,Sized);
		});
	}
}
//---------------------------------------------------------------------------
void cMacNSView::ViewLayersSetFrame(CGRect Frame)
{
	for(auto *ViewLayer : fViewLayers){
		ViewLayer->SetLayerFrame(Frame);
	}
}
//---------------------------------------------------------------------------
bool cMacNSView::CheckIsEnabled(void)
{
	return fUIEnabled;
}
//---------------------------------------------------------------------------
cnLibrary::cRectangle cMacNSView::CalculateMarginFromParent_Default(void)
{
	auto ParentView=ocSafeCast<cnLib_NSView>(fView.superview);
	if(ParentView==nullptr){
		return RectangleZero;
	}
	// calculate margin from parent
	auto &CPPParentView=pMacNSView(ParentView);
	cRectangle Margin=CPPParentView.fMargin;


	sfInt32 ParentWidth,ParentHeight;
	cRectangle ViewRect;
	{
		auto ParentSize=ParentView.bounds.size;
		auto ParentScale=ParentView.layer.contentsScale;
		ParentWidth=ParentSize.width*ParentScale;
		ParentHeight=ParentSize.height*ParentScale;

		auto ViewFrame=fView.frame;
		auto Scale=fView.layer.contentsScale;
		ViewRect.Left=static_cast<sfInt32>(ViewFrame.origin.x*Scale);
		ViewRect.Top=static_cast<sfInt32>(ViewFrame.origin.y*Scale);
		ViewRect.Right=static_cast<sfInt32>(ViewFrame.size.width*Scale);
		ViewRect.Bottom=static_cast<sfInt32>(ViewFrame.size.height*Scale);
		ViewRect.Right+=ViewRect.Left;
		ViewRect.Bottom+=ViewRect.Top;
	}

	Margin.Left-=ViewRect.Left;
	Margin.Top-=ViewRect.Top;
	Margin.Right=ViewRect.Right-(ParentWidth-Margin.Right);
	Margin.Bottom=ViewRect.Bottom-(ParentHeight-Margin.Bottom);

	if(Margin.Left<0)
		Margin.Left=0;
	if(Margin.Top<0)
		Margin.Top=0;
	if(Margin.Right<0)
		Margin.Right=0;
	if(Margin.Bottom<0)
		Margin.Bottom=0;
	return Margin;
}
//---------------------------------------------------------------------------
void cMacNSView::onLayoutSubviews(void)
{
	// calculate margin
	if(fIncludeParentMargin){
		fMargin=CalculateMarginFromParent_Default();
	}
	else{
		fMargin=RectangleZero;
	}
	for(auto *Handler : fLayoutHandlers){
		auto InsetMargin=Handler->UIMargin(fMargin);
		fMargin.Left+=InsetMargin.Left;
		fMargin.Top+=InsetMargin.Top;
		fMargin.Right+=InsetMargin.Right;
		fMargin.Bottom+=InsetMargin.Bottom;
	}

	NotifyLayoutHandler([=](iUILayoutHandler *Handler){
		Handler->UILayout();
	});


	auto NewBounds=fView.bounds;
	ViewLayersSetFrame(NewBounds);

	if(fViewLayerHost!=nil){
		//if(fViewLayers.GetCount()>1){
		//	fViewLayerHost.frame=NewBounds;
		//}
		//[fView.layer insertSublayer:fViewLayerHost atIndex:0];
	}

}
//---------------------------------------------------------------------------
void cMacNSView::onSetVisible(void)
{
	NotifyLayoutHandler([=](iUILayoutHandler *Handler){
		Handler->VisibleChanged();
	});
	UpdateUIState();
}
//---------------------------------------------------------------------------
void cMacNSView::onSetEnabled(void)
{
	NotifyLayoutHandler([=](iUILayoutHandler *Handler){
		Handler->EnableChanged();
	});
}
//---------------------------------------------------------------------------
bool cMacNSView::IsViewVisible(void)const
{
	bool Visible=!fView.hidden;
	if(Visible==false){
		return false;
	}
	if(fView.window==nil){
		return false;
	}
	return true;
}
//---------------------------------------------------------------------------
#if	0
void cMacNSView::onSuperViewChange(void)
{
}
#endif
//---------------------------------------------------------------------------
void cMacNSView::ViewLayersSetScale(CGFloat Scale)
{
	for(auto *ViewLayer : fViewLayers){
		ViewLayer->SetLayerScale(Scale);
	}
}
//---------------------------------------------------------------------------
void cMacNSView::onWindowChange(void)
{
	auto window=fView.window;
	if(window!=nil){
		auto NewScale=window.screen.backingScaleFactor;
		if(NewScale!=fView.layer.contentsScale){
			fView.layer.contentsScale=NewScale;
			NotifyLayoutHandler([=](iUILayoutHandler *Handler){
				Handler->RectangleChanged(false,true);
			});
			UpdatePainterRect();

			ViewLayersSetScale(NewScale);
		}
	}
	UpdateUIState();

}
//---------------------------------------------------------------------------
void cMacNSView::onBecomeFirstResponder(void)
{
	fIsFirstResponder=true;
	for(auto *Handler : fKeyHandlers){
		Handler->KeyFocusEnter();
	}
}
//---------------------------------------------------------------------------
void cMacNSView::onResignFirstResponder(void)
{
	fIsFirstResponder=false;
	for(auto *Handler : fKeyHandlers){
		Handler->KeyFocusLeave();
	}
}
//---------------------------------------------------------------------------
iUIView* cMacNSView::cUIKeyEvent::GetSourceView(void)
{
	return Owner;
}
iUIKeyboard* cMacNSView::cUIKeyEvent::GetKeyboard(void)
{
	return nullptr;
}
void cMacNSView::cUIKeyEvent::CancelEvent(void)
{
}
bool cMacNSView::cUIKeyEvent::IsKeyDown(eKeyCode KeyCode)
{
	return false;
}
bool cMacNSView::cUIKeyEvent::IsKeyToggled(eKeyCode KeyCode)
{
	return false;
}
//---------------------------------------------------------------------------
void cMacNSView::onKeyDown(NSEvent *Event)
{
	cUIKeyEvent KeyEvent;
	if(Event.ARepeat==NO){
		for(auto *Handler : fKeyHandlers){
			Handler->KeyDown(&KeyEvent,static_cast<eKeyCode>(Event.keyCode));
		}
	}
	for(auto *Handler : fKeyHandlers){
		Handler->KeyInput(&KeyEvent,static_cast<eKeyCode>(Event.keyCode));
	}
}
//---------------------------------------------------------------------------
void cMacNSView::onKeyUp(NSEvent *Event)
{
	cUIKeyEvent KeyEvent;
	for(auto *Handler : fKeyHandlers){
		Handler->KeyUp(&KeyEvent,static_cast<eKeyCode>(Event.keyCode));
	}
}
//---------------------------------------------------------------------------
iUIView* cMacNSView::cUIMouseEvent::GetSourceView(void)
{
	return Owner;
}

iUIMouse* cMacNSView::cUIMouseEvent::GetMouse(void)
{
	return nullptr;
}
void cMacNSView::cUIMouseEvent::CancelEvent(void)
{
}

bool cMacNSView::cUIMouseEvent::GetPosition(iUIArea *Relative,cUIPoint &Position)
{
	auto WindowLocation=[Event locationInWindow];
	auto ViewPos=[Owner->fView convertPoint:WindowLocation fromView:nil];

	return Owner->GetViewRelativePosition(Relative,Position,ViewPos);
}

eUIEventSourceType cMacNSView::cUIMouseEvent::GetSourceType(void)
{
	switch(Event.subtype){
	default:
		return UIEventSourceType::Unknow;
	case NSEventSubtypeMouseEvent:
		return UIEventSourceType::Mouse;

    case NSEventSubtypeTouch:
    	return UIEventSourceType::Touch;
	}
}
bool cMacNSView::cUIMouseEvent::IsButtonDown(eMouseButton Button)
{
	return false;
}

//---------------------------------------------------------------------------
template<class TFunc>
void cMacNSView::ForEachMouseHandlers(TFunc &&Operator)
{
	if(fMouseExclusiveHandler!=nullptr){
		Operator(fMouseExclusiveHandler);
		return;
	}
	for(auto *Handler : fMouseHandlers){
		Operator(Handler);
	}
}
//---------------------------------------------------------------------------
void cMacNSView::onMouseMove(NSEvent *Event,eMouseButton Button)
{
	cUIMouseEvent MouseEvent;
	MouseEvent.Owner=this;
	MouseEvent.Event=Event;
	ForEachMouseHandlers([&MouseEvent](iUIMouseHandler *Handler){
		Handler->MouseMove(&MouseEvent);
	});
}
//---------------------------------------------------------------------------
void cMacNSView::onMouseDown(NSEvent *Event,eMouseButton Button)
{
	cUIMouseEvent MouseEvent;
	MouseEvent.Owner=this;
	MouseEvent.Event=Event;
	ForEachMouseHandlers([&MouseEvent,Button](iUIMouseHandler *Handler){
		Handler->MouseDown(&MouseEvent,Button);
	});
}
//---------------------------------------------------------------------------
void cMacNSView::onMouseUp(NSEvent *Event,eMouseButton Button)
{
	cUIMouseEvent MouseEvent;
	MouseEvent.Owner=this;
	MouseEvent.Event=Event;
	ForEachMouseHandlers([&MouseEvent,Button](iUIMouseHandler *Handler){
		Handler->MouseUp(&MouseEvent,Button);
	});

	fMouseExclusiveHandler=nullptr;
}
//---------------------------------------------------------------------------
void cMacNSView::onMouseWheel(NSEvent *Event)
{
	cUIMouseEvent MouseEvent;
	MouseEvent.Owner=this;
	MouseEvent.Event=Event;
	sfInt16n ScrollX=Event.scrollingDeltaX;
	sfInt16n ScrollY=Event.scrollingDeltaY;
	ForEachMouseHandlers([&MouseEvent,ScrollX,ScrollY](iUIMouseHandler *Handler){
		Handler->MouseWheel(&MouseEvent,ScrollX,ScrollY);
	});
}
//---------------------------------------------------------------------------
iUIView* cMacNSView::cTouchPadEvent::GetSourceView(void)
{
	return Owner;
}

iUITouchPoint* cMacNSView::cTouchPadEvent::GetTouchPoint(void)
{
	return TouchPoint;
}

bool cMacNSView::cTouchPadEvent::GetPosition(iUIArea *Relative,cUIPoint &Position)
{
	if(fEventMousePosValid==false){
		fEventMousePosValid=true;
		auto WindowLocation=[Event locationInWindow];
		fEventMousePos=[Owner->fView convertPoint:WindowLocation fromView:nil];
	}

	if(Owner->GetViewRelativePosition(Relative,Position,fEventMousePos)==false)
		return false;

	NSTouch *Touch=TouchPoint->Touch;

	auto DevicePos=Touch.normalizedPosition;
	DevicePos.x-=Owner->fTouchPadRelativeDevicePos.x;
	DevicePos.y-=Owner->fTouchPadRelativeDevicePos.y;
	auto DeviceSize=Touch.deviceSize;
	DevicePos.x*=DeviceSize.width;
	DevicePos.y*=DeviceSize.height;

	auto Scale=Owner->fView.layer.contentsScale;
	Position.x+=DevicePos.x*Scale;
	Position.y-=DevicePos.y*Scale;
	return true;
}
//---------------------------------------------------------------------------
template<class TFunc>
void cMacNSView::ForEachTouchHandlers(TFunc &&Operator)
{
	for(auto *Handler : fTouchHandlers){
		Operator(Handler);
	}
}
//---------------------------------------------------------------------------
cMacNSView::cTouchPoint* cMacNSView::TouchPadInsert(NSTouch *Touch)
{
	if(fTouchPadMap.GetCount()==0){
		fTouchPadRelativeDevicePos=Touch.normalizedPosition;
	}
	auto TouchID=Touch.identity;
	auto &TouchPoint=fTouchPadMap[TouchID];
	if(TouchPoint==nullptr){
		TouchPoint=iCreate<cTouchPoint>();
	}
	else{
		if(fTouchPadMap.GetCount()==1){
			fTouchPadRelativeDevicePos=Touch.normalizedPosition;
		}
	}
	TouchPoint->Touch=Touch;
	return TouchPoint;
}
//---------------------------------------------------------------------------
iPtr<cMacNSView::cTouchPoint> cMacNSView::TouchPadRemove(NSTouch *Touch)
{
	auto TouchPair=fTouchPadMap.GetPair(Touch.identity);
	if(TouchPair==nullptr){
		return nullptr;
	}
	auto TouchPoint=cnVar::MoveCast(TouchPair->Value);
	fTouchPadMap.RemovePair(TouchPair);
	TouchPoint->Touch=Touch;
	return TouchPoint;
}
//---------------------------------------------------------------------------
void cMacNSView::onTouchBegin(NSEvent *Event)
{
	cTouchPadEvent TouchPadEvent;
	TouchPadEvent.Owner=this;
	TouchPadEvent.Event=Event;

	auto Touches=[Event touchesMatchingPhase:NSTouchPhaseBegan inView:fView];
	for(NSTouch *Touch in Touches){
		auto TouchPoint=TouchPadInsert(Touch);
		TouchPadEvent.TouchPoint=TouchPoint;
		ForEachTouchHandlers([&TouchPadEvent](iUITouchHandler *Handler){
			Handler->TouchBegin(&TouchPadEvent);
		});
	}
}
//---------------------------------------------------------------------------
void cMacNSView::onTouchEnd(NSEvent *Event)
{
	cTouchPadEvent TouchPadEvent;
	TouchPadEvent.Owner=this;
	TouchPadEvent.Event=Event;

	auto Touches=[Event touchesMatchingPhase:NSTouchPhaseEnded inView:fView];
	for(NSTouch *Touch in Touches){
		auto TouchPoint=TouchPadRemove(Touch);
		if(TouchPoint!=nullptr){
			TouchPadEvent.TouchPoint=TouchPoint;
			ForEachTouchHandlers([&TouchPadEvent](iUITouchHandler *Handler){
				Handler->TouchEnd(&TouchPadEvent);
			});
		}
	}
}
//---------------------------------------------------------------------------
void cMacNSView::onTouchMove(NSEvent *Event)
{
	cTouchPadEvent TouchPadEvent;
	TouchPadEvent.Owner=this;
	TouchPadEvent.Event=Event;

	auto Touches=[Event touchesMatchingPhase:NSTouchPhaseMoved inView:fView];
	for(NSTouch *Touch in Touches){
		auto TouchPoint=TouchPadInsert(Touch);
		TouchPadEvent.TouchPoint=TouchPoint;
		ForEachTouchHandlers([&TouchPadEvent](iUITouchHandler *Handler){
			Handler->TouchMove(&TouchPadEvent);
		});
	}
}
//---------------------------------------------------------------------------
void cMacNSView::onTouchCancelled(NSEvent *Event)
{
	cTouchPadEvent TouchPadEvent;
	TouchPadEvent.Owner=this;
	TouchPadEvent.Event=Event;

	auto Touches=[Event touchesMatchingPhase:NSTouchPhaseCancelled inView:fView];
	for(NSTouch *Touch in Touches){
		auto TouchPoint=TouchPadRemove(Touch);
		if(TouchPoint!=nullptr){
			TouchPadEvent.TouchPoint=TouchPoint;
			ForEachTouchHandlers([&TouchPadEvent](iUITouchHandler *Handler){
				Handler->TouchLost(&TouchPadEvent);
			});
		}
	}
}
//---------------------------------------------------------------------------
bool cMacNSView::CheckUIFunction(void)
{
	return gMainUIThread.IsCurrentThread();
}
//---------------------------------------------------------------------------
iUIThread* cMacNSView::GetUIThread(void)
{
	return &gMainUIThread;
}
//---------------------------------------------------------------------------
UIState cMacNSView::GetUIState(void)
{
	return UIState::Active;
}
//---------------------------------------------------------------------------
bool cMacNSView::InsertStateHandler(iUIStateHandler *Handler,sfInt16 Order)
{
	if(CheckUIFunction()==false)
		return false;
	if(fStateHandlers.Insert(Handler)==false){
		return true;
	}

	cnRTL::UIStateNotifyChange(UIState::Null,fUIState,*Handler);
	return true;
}
//---------------------------------------------------------------------------
bool cMacNSView::RemoveStateHandler(iUIStateHandler *Handler)
{
	if(CheckUIFunction()==false)
		return false;
	if(fStateHandlers.Remove(Handler)==false){
		return true;
	}
	UIStateNotifyChange(fUIState,UIState::Null,*Handler);
	return true;
}
//---------------------------------------------------------------------------
void cMacNSView::UpdateUIState(void)
{
	auto PrevUIState=fUIState;

	if(fView.window==nil){
		fUIState=UIState::Null;
	}
	else{
		auto ViewVisible=!fView.hidden;
		if(ViewVisible){
			ViewVisible=fView.window.isVisible;
		}
		auto AppState=gMainUIThread.GetAppState();
		switch(AppState){
		default:
		case UIState::Null:
			fUIState=UIState::Null;
			break;
		case UIState::Background:
			fUIState=UIState::Background;
			break;
		case UIState::Inactive:
			if(ViewVisible==false){
				fUIState=UIState::Background;
			}
			else{
				fUIState=UIState::Inactive;
			}
			break;
		case UIState::Active:
			if(ViewVisible==false){
				fUIState=UIState::Background;
			}
			else{
				fUIState=UIState::Active;
			}
			break;
		}
	}

	cUIStateHandlerSetNotifier Notifier(fStateHandlers);
	UIStateNotifyChange(PrevUIState,fUIState,Notifier);
}
//---------------------------------------------------------------------------
uIntn cMacNSView::FindRelativeBase(cUIPoint &Offset,iUIArea*const *Relatives,uIntn Count)
{
	Offset={0,0};
	if(CheckUIFunction()==false)
		return IndexNotFound;

	for(uIntn i=0;i<Count;i++){
		auto *Layout=Relatives[i];
		if(Layout==this){
			return i;
		}
	}
	for(uIntn i=0;i<Count;i++){
		auto OCView=ociSafeCast<NSView>(Relatives[i]);
		if(OCView!=nullptr){
			CGPoint p={0,0};
			p=[fView convertPoint:p toView:OCView];
			auto scale=fView.layer.contentsScale;
			Offset.x=p.x*scale;
			Offset.y=p.y*scale;
			return i;
		}
	}
	return IndexNotFound;
}
//---------------------------------------------------------------------------
bool cMacNSView::GetViewRelativePosition(iUIArea *Relative,cUIPoint &Position,NSPoint ViewPos)
{
	if(Relative==this){
		auto scale=fView.layer.contentsScale;
		Position.x=ViewPos.x*scale;
		Position.y=ViewPos.y*scale;
		return true;
	}
	auto ViewWindow=fView.window;
	if(ViewWindow==nil){
		return false;
	}
	if(Relative==nullptr){
		// to screen
		auto WindowPos=[fView convertPoint:ViewPos toView:nil];
		auto ScreenPos=[ViewWindow convertRectToScreen:NSRect{WindowPos,{0,0}}].origin;
		auto scale=ViewWindow.screen.backingScaleFactor;
		Position.x=ScreenPos.x*scale;
		Position.y=ScreenPos.y*scale;
		return true;
	}
	auto RelativeView=ociSafeCast<NSView>(Relative);
	if(RelativeView!=nullptr){
		if(RelativeView.window==fView.window){
			// in the same window
			auto RelativeViewPos=[fView convertPoint:ViewPos toView:RelativeView];
			auto scale=RelativeView.layer.contentsScale;
			Position.x=RelativeViewPos.x*scale;
			Position.y=RelativeViewPos.y*scale;
			return true;
		}
		// different window
		// convert through screen
		auto RelativeViewWindow=RelativeView.window;
		if(RelativeViewWindow==nil)
			return false;
		if(RelativeViewWindow.screen!=ViewWindow.screen)
			return false;

		auto ViewWindowPos=[fView convertPoint:ViewPos toView:nil];
		auto ViewScreenPos=[ViewWindow convertRectToScreen:NSRect{ViewWindowPos,{0,0}}].origin;

		auto RelativeWindowPos=[RelativeViewWindow convertRectFromScreen:NSRect{ViewScreenPos,{0,0}}].origin;
		auto RelativeViewPos=[RelativeView convertPoint:RelativeWindowPos fromView:nil];

		auto scale=RelativeView.layer.contentsScale;
		Position.x=RelativeViewPos.x*scale;
		Position.y=RelativeViewPos.x*scale;
		return true;
	}

	auto ViewWindowPos=[fView convertPoint:ViewPos toView:nil];
	auto ViewScreenPos=[ViewWindow convertRectToScreen:NSRect{ViewWindowPos,{0,0}}].origin;
	auto scale=ViewWindow.screen.backingScaleFactor;
	ViewScreenPos.x*=scale;
	ViewScreenPos.y*=scale;

	// convert through screen
	cUIPoint ScreenPos;
	if(Relative->GetPosition(nullptr,ScreenPos)==false)
		return false;

	Position.x=ViewScreenPos.x-ScreenPos.x;
	Position.y=ViewScreenPos.y-ScreenPos.y;
	return true;
}
//---------------------------------------------------------------------------
bool cMacNSView::GetPosition(iUIArea *Relative,cUIPoint &Position)
{
	if(CheckUIFunction()==false)
		return false;

	return GetViewRelativePosition(Relative,Position,{0,0});
}
//---------------------------------------------------------------------------
bool cMacNSView::SetPosition(iUIArea *Relative,cUIPoint Position)
{
	if(CheckUIFunction()==false)
		return false;
	auto Frame=fView.frame;
	auto scale=1/fView.layer.contentsScale;
	if(Relative==this){
		Frame.origin.x+=Position.x*scale;
		Frame.origin.y+=Position.y*scale;
		fView.frame=Frame;
		return true;
	}
	Frame.origin.x=Position.x*scale;
	Frame.origin.y=Position.y*scale;
	if(Relative==nullptr){
		Frame.origin=[fView convertPoint:Frame.origin fromView:nil];
		fView.frame=Frame;
		return true;
	}
	auto View=ociSafeCast<NSView>(Relative);
	if(View!=nullptr){
		auto RelativeView=View;
		if(RelativeView==fView.superview){
			fView.frame=Frame;
			return true;
		}
		Frame.origin=[fView convertPoint:Frame.origin fromView:RelativeView];
		fView.frame=Frame;
		return true;
	}
	return false;
}
//---------------------------------------------------------------------------
cUIPoint cMacNSView::GetSize(void)
{
	if(CheckUIFunction()==false)
		return {0,0};
	auto scale=fView.layer.contentsScale;
	auto FrameSize=fView.bounds.size;
	cUIPoint Size;
	Size.x=FrameSize.width*scale;
	Size.y=FrameSize.height*scale;
	return Size;
}
//---------------------------------------------------------------------------
bool cMacNSView::SetSize(cUIPoint Size)
{
	if(CheckUIFunction()==false)
		return false;
	auto scale=fView.layer.contentsScale;
	auto Frame=fView.frame;
	Frame.size.width=Size.x/scale;
	Frame.size.height=Size.y/scale;
	fView.frame=Frame;
	return true;
}
//---------------------------------------------------------------------------
bool cMacNSView::SetRectangle(iUIArea *Relative,cUIPoint Position,cUIPoint Size)
{
	if(CheckUIFunction()==false)
		return false;
	NSRect Frame;
	auto scale=1/fView.layer.contentsScale;
	Frame.size.width=Size.x*scale;
	Frame.size.height=Size.y*scale;
	if(Relative==this){
		Frame.origin.x+=Position.x*scale;
		Frame.origin.y+=Position.y*scale;
		fView.frame=Frame;
		return true;
	}
	Frame.origin.x=Position.x*scale;
	Frame.origin.y=Position.y*scale;
	if(Relative==nullptr){
		Frame.origin=[fView convertPoint:Frame.origin fromView:nil];
		fView.frame=Frame;
		return true;
	}
	auto View=ociSafeCast<NSView>(Relative);
	if(View!=nullptr){
		auto RelativeView=View;
		if(RelativeView==fView.superview){
			fView.frame=Frame;
			return true;
		}
		Frame.origin=[fView convertPoint:Frame.origin fromView:RelativeView];
		fView.frame=Frame;
		return true;
	}
	return false;
}
//---------------------------------------------------------------------------
iUIWindow* cMacNSView::GetWindow(void)
{
	if(CheckUIFunction()==false)
		return nullptr;
	return nullptr;
}
//---------------------------------------------------------------------------
iUIView* cMacNSView::GetParent(void)
{
	if(CheckUIFunction()==false)
		return nullptr;
	auto SuperView=ocSafeCast<cnLib_NSView>(fView.superview);
	if(SuperView==nil)
		return nullptr;
	return cnLib_NSView_GetUIView(SuperView);
}
//---------------------------------------------------------------------------
bool cMacNSView::InsertView(iUIView *View)
{
	if(CheckUIFunction()==false)
		return false;
	auto SubView=ociSafeCast<cnLib_NSView>(View);
	if(SubView==nil)
		return false;
	auto &CPPSubView=pMacNSView(SubView);
	auto Sibling=FindNextSiblingForZIndex(SubView,fView,CPPSubView.fZIndex);
	[fView addSubview:SubView];
	if(Sibling==nullptr){
		[fView addSubview:SubView];
	}
	else{
		[fView addSubview:SubView positioned:NSWindowBelow relativeTo:Sibling];
	}

	return true;
}
//---------------------------------------------------------------------------
bool cMacNSView::RemoveView(iUIView *View)
{
	if(CheckUIFunction()==false)
		return false;
	auto SubView=ociSafeCast<cnLib_NSView>(View);
	if(SubView==nil)
		return false;
	if(SubView.superview!=fView)
		return false;
	[SubView removeFromSuperview];
	return true;
}
//---------------------------------------------------------------------------
bool cMacNSView::GetVisible(void)
{
	return !fView.hidden;
}
//---------------------------------------------------------------------------
bool cMacNSView::SetVisible(bool Visible)
{
	if(CheckUIFunction()==false)
		return false;
	fView.hidden=!Visible;
	return true;
}
//---------------------------------------------------------------------------
bool cMacNSView::IsEnabled(void)
{
	if(CheckUIFunction()==false)
		return false;

	//return fView.userInteractionEnabled;
	return true;
}
//---------------------------------------------------------------------------
bool cMacNSView::GetEnable(void)
{
	if(CheckUIFunction()==false)
		return false;
	//return fView.userInteractionEnabled;
	return true;
}
//---------------------------------------------------------------------------
bool cMacNSView::SetEnable(bool Enable)
{
	if(CheckUIFunction()==false)
		return false;
//	fView.userInteractionEnabled=Enable;
	return true;
}
//---------------------------------------------------------------------------
void cMacNSView::SetArrangement(void)
{
	[fView setNeedsLayout:YES];
}
//---------------------------------------------------------------------------
void cMacNSView::ArrangeLayout(void)
{
	[fView layoutSubtreeIfNeeded];
}
//---------------------------------------------------------------------------
void cMacNSView::SetIncludeParentFrameMargin(bool Include)
{
	if(fIncludeParentMargin==Include)
		return;

	fIncludeParentMargin=Include;
	auto ParentMargin=CalculateMarginFromParent_Default();
	if(ParentMargin.Left!=0 || ParentMargin.Right!=0 ||
		 ParentMargin.Top==0 || ParentMargin.Bottom==0)
	{
		[fView setNeedsLayout:YES];
	}
}
//---------------------------------------------------------------------------
cnLibrary::cRectangle cMacNSView::GetFrameMargin(void)
{
	return fMargin;
}
//---------------------------------------------------------------------------
bool cMacNSView::InsertLayoutHandler(iUILayoutHandler *Handler,sfInt16 Order)
{
	if(CheckUIFunction()==false)
		return false;
	fLayoutHandlers.Insert(Handler);
	return true;
}
//---------------------------------------------------------------------------
bool cMacNSView::RemoveLayoutHandler(iUILayoutHandler *Handler)
{
	if(CheckUIFunction()==false)
		return false;
	fLayoutHandlers.Remove(Handler);
	return true;
}
//---------------------------------------------------------------------------
sfInt16 cMacNSView::GetZIndex(void)
{
	if(CheckUIFunction()==false)
		return 0;
	return fZIndex;
}
//---------------------------------------------------------------------------
bool cMacNSView::SetZIndex(sfInt16 Index)
{
	if(CheckUIFunction()==false)
		return false;
	fZIndex=Index;
	auto SuperView=fView.superview;
	if(SuperView!=nil){
		auto Sibling=FindNextSiblingForZIndex(fView,SuperView,fZIndex);
		if(Sibling==nullptr){
			[SuperView addSubview:fView];
		}
		else{
			[SuperView addSubview:fView positioned:NSWindowBelow relativeTo:Sibling];
		}
	}
	return true;
}
//---------------------------------------------------------------------------
NSView* cMacNSView::FindNextSiblingForZIndex(NSView *SubView,NSView *Parent,sfInt16 ZIndex)
{
	for(NSView *sv in Parent.subviews){
		if(sv==SubView)
			continue;
		auto SubView=ocSafeCast<cnLib_NSView>(sv);
		if(SubView==nil)
			continue;

		auto &CPPSibling=pMacNSView(SubView);
		if(CPPSibling.fZIndex>ZIndex){
			return sv;
		}
	}
	return nil;
}
//---------------------------------------------------------------------------
Float32 cMacNSView::GetContentScale(void)
{
	return fView.layer.contentsScale;
}
//---------------------------------------------------------------------------
bool cMacNSView::InsertKeyHandler(iUIKeyHandler *Handler,sfInt16 Order)
{
	if(CheckUIFunction()==false)
		return false;
	return fKeyHandlers.Insert(Handler);
}
//---------------------------------------------------------------------------
bool cMacNSView::RemoveKeyHandler(iUIKeyHandler *Handler)
{
	if(CheckUIFunction()==false)
		return false;
	return fKeyHandlers.Remove(Handler);
}
//---------------------------------------------------------------------------
bool cMacNSView::InsertKeyFilter(iUIKeyHandler *Filter,sfInt16 Order)
{
	return false;
}
//---------------------------------------------------------------------------
bool cMacNSView::RemoveKeyFilter(iUIKeyHandler *Filter)
{
	return false;
}
//---------------------------------------------------------------------------
bool cMacNSView::KeyAcquireExclusive(iUIKeyHandler *Handler)
{
	return false;
}
//---------------------------------------------------------------------------
void cMacNSView::KeyReleaseExclusive(iUIKeyHandler *Handler)
{
}
//---------------------------------------------------------------------------
bool cMacNSView::InsertTextInputHandler(iUITextInputHandler *Handler,sfInt16 Order)
{
	return false;
}
//---------------------------------------------------------------------------
bool cMacNSView::RemoveTextInputHandler(iUITextInputHandler *Handler)
{
	return false;
}
//---------------------------------------------------------------------------
bool cMacNSView::SetCompositionViewInfo(iUITextCompositionViewInfo *CompositionViewInfo)
{
	return false;
}
//---------------------------------------------------------------------------
iUIView* cMacNSView::GetFocus(void)
{
	if(CheckUIFunction()==false)
		return nullptr;
	auto View=ocSafeCast<cnLib_NSView>(fView.window.firstResponder);
	if(View==nullptr)
		return nullptr;
	return CPPMacNSView(fView);
}
//---------------------------------------------------------------------------
void cMacNSView::SetFocus(bool ActivateWindow)
{
	if(CheckUIFunction()==false)
		return;

	[fView.window makeFirstResponder:fView];
}
//---------------------------------------------------------------------------
void cMacNSView::MouseUpdateCursor(void)
{
}
//---------------------------------------------------------------------------
bool cMacNSView::InsertMouseHandler(iUIMouseHandler *Handler,sfInt16 Order)
{
	if(CheckUIFunction()==false)
		return false;
	return fMouseHandlers.Insert(Handler);
}
//---------------------------------------------------------------------------
bool cMacNSView::RemoveMouseHandler(iUIMouseHandler *Handler)
{
	if(CheckUIFunction()==false)
		return false;
	return fMouseHandlers.Remove(Handler);
}
//---------------------------------------------------------------------------
bool cMacNSView::InsertMouseFilter(iUIMouseHandler *Filter,sfInt16 Order)
{
	return false;
}
//---------------------------------------------------------------------------
bool cMacNSView::RemoveMouseFilter(iUIMouseHandler *Filter)
{
	return false;
}
//---------------------------------------------------------------------------
sfInt16n cMacNSView::GetWheelScale(void)
{
	return 1;
}
//---------------------------------------------------------------------------
bool cMacNSView::MouseSetCaptrue(void)
{
	return false;
}
//---------------------------------------------------------------------------
void cMacNSView::MouseReleaseCaptrue(void)
{
}
//---------------------------------------------------------------------------
bool cMacNSView::MouseIsCaptured(void)
{
	return false;
}
//---------------------------------------------------------------------------
bool cMacNSView::MouseAcquireExclusive(iUIMouseHandler *Handler)
{
	if(fIsFirstResponder==false)
		return false;

	fMouseExclusiveHandler=Handler;
	return true;
}
//---------------------------------------------------------------------------
void cMacNSView::MouseReleaseExclusive(iUIMouseHandler *Handler)
{
	if(fIsFirstResponder==false)
		return;

	fMouseExclusiveHandler=nullptr;
}
//---------------------------------------------------------------------------
bool cMacNSView::InsertTouchHandler(iUITouchHandler *Handler,sfInt16 Order)
{
	if(CheckUIFunction()==false)
		return false;

	fTouchHandlers.Insert(Handler);
	return true;
}
bool cMacNSView::RemoveTouchHandler(iUITouchHandler *Handler)
{
	if(CheckUIFunction()==false)
		return false;

	fTouchHandlers.Remove(Handler);
	return true;
}
bool cMacNSView::InsertTouchFilter(iUITouchHandler *Filter,sfInt16 Order)
{
	return false;
}
bool cMacNSView::RemoveTouchFilter(iUITouchHandler *Filter)
{
	return false;
}
bool cMacNSView::TouchAcquireExclusive(iUITouchHandler *Handler)
{
	return false;
}
void cMacNSView::TouchReleaseExclusive(iUITouchHandler *Handler)
{
}
//---------------------------------------------------------------------------
void cMacNSView::AppStateChanged(void)
{
	UpdateUIState();
}
//---------------------------------------------------------------------------

