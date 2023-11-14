
//#include <mach/mach.h>
#import <QuartzCore/QuartzCore.h>
#import <QuartzCore/CALayer.h>

#import "iOS_UIView.h"
#import "iOS_UIApp.h"

using namespace cnLibrary;
using namespace cnRTL;
using namespace cnMac;


//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
@implementation cnLib_UIView
{
@private
	ocStaticIVar<cMacUIView> cnLib_IVar;
}

static cMacUIView& pMacUIView(cnLib_UIView *self)
{
	return reinterpret_cast<cMacUIView&>(self->cnLib_IVar);
}

+(instancetype)alloc
{
	auto p=[super alloc];

	cnVar::ManualConstruct(pMacUIView(p),p);
	return p;
}

-(void)dealloc
{
	cnVar::ManualDestruct(pMacUIView(self));
	[super dealloc];
}

#pragma mark property

-(void)setFrame:(CGRect)frame
{
	auto LastFrame=[super frame];
	[super setFrame:frame];

	auto &CPPView=pMacUIView(self);

	CPPView.onSetFrame(LastFrame);
}

-(void)layoutSubviews
{
	[super layoutSubviews];

	auto &CPPView=pMacUIView(self);
	CPPView.onLayoutSubviews();
}

-(void)setContentScaleFactor:(CGFloat)contentScaleFactor
{
	[super setContentScaleFactor:contentScaleFactor];

	auto &CPPView=pMacUIView(self);
	CPPView.onSetContentScale();

}

-(void)setUserInteractionEnabled:(BOOL)userInteractionEnabled
{
	[super setUserInteractionEnabled:userInteractionEnabled];

	auto &CPPView=pMacUIView(self);
	CPPView.onSetEnabled();
}
-(void)setHidden:(BOOL)hidden
{
	[super setHidden:hidden];

	auto &CPPView=pMacUIView(self);
	CPPView.onSetVisible();
}

-(void)willMoveToWindow:(UIWindow *)newWindow
{
	[super willMoveToWindow:newWindow];

	auto &CPPView=pMacUIView(self);
	CPPView.onWindowWillChange(newWindow);
}

-(void)didMoveToWindow
{
	[super didMoveToWindow];

	auto &CPPView=pMacUIView(self);
	CPPView.onWindowChange();

	auto window=self.window;
	if(window!=nil){
		self.contentScaleFactor=window.screen.scale;
	}
	
}

#pragma mark touches

template<class TEventCaller>
static void DispatchTouchFilter(cnLib_UIView *self,TEventCaller &EventCaller){
	auto SuperView=ocSafeCast<cnLib_UIView>(self.superview);
	if(SuperView!=nil){
		DispatchTouchFilter(SuperView,EventCaller);
	}

	EventCaller(self);
}


-(void)touchesBegan:(NSSet *)touches withEvent:(UIEvent *)event
{
	cMacTouchEvent Event;
	Event.SenderView=self;

	auto FilterCaller=[&Event](cnLib_UIView *View){
		auto &CPPView=pMacUIView(View);
		CPPView.onSendTouchFilterBegin(&Event);
	};
	for(UITouch *t in touches){
		Event.Touch=t;
		Event.TouchPoint=gMainUIThread.QueryTouchPoint(t);
		DispatchTouchFilter(self,FilterCaller);

		auto &CPPView=pMacUIView(self);
		CPPView.onSendTouchBegin(&Event);
	}
}

-(void)touchesMoved:(NSSet *)touches withEvent:(UIEvent *)event
{
	cMacTouchEvent Event;
	Event.SenderView=self;

	auto FilterCaller=[&Event](cnLib_UIView *View){
		auto &CPPView=pMacUIView(View);
		CPPView.onSendTouchFilterMove(&Event);
	};
	for(UITouch *t in touches){
		Event.Touch=t;
		Event.TouchPoint=gMainUIThread.QueryTouchPoint(t);
		DispatchTouchFilter(self,FilterCaller);

		auto &CPPView=pMacUIView(self);
		CPPView.onSendTouchMove(&Event);
	}
}

-(void)touchesEnded:(NSSet *)touches withEvent:(UIEvent *)event
{
	cMacTouchEvent Event;
	Event.SenderView=self;

	auto FilterCaller=[&Event](cnLib_UIView *View){
		auto &CPPView=pMacUIView(View);
		CPPView.onSendTouchFilterEnded(&Event);
	};
	for(UITouch *t in touches){
		Event.Touch=t;
		auto tp=gMainUIThread.FetchTouchPoint(t);
		Event.TouchPoint=tp;
		DispatchTouchFilter(self,FilterCaller);

		auto &CPPView=pMacUIView(self);
		CPPView.onSendTouchEnded(&Event);
	}
}

-(void)touchesCancelled:(NSSet *)touches withEvent:(UIEvent *)event
{
	cMacTouchEvent Event;
	Event.SenderView=self;

	auto FilterCaller=[&Event](cnLib_UIView *View){
		auto &CPPView=pMacUIView(View);
		CPPView.onSendTouchFilterLost(&Event);
	};
	for(UITouch *t in touches){
		Event.Touch=t;
		auto tp=gMainUIThread.FetchTouchPoint(t);
		Event.TouchPoint=tp;
		DispatchTouchFilter(self,FilterCaller);

		auto &CPPView=pMacUIView(self);
		CPPView.onSendTouchLost(&Event);
	}
}
//---------------------------------------------------------------------------

#pragma mark interface

cMacUIView* cnMac::cnLib_UIView_GetUIView(cnLib_UIView *self)
{
	return &pMacUIView(self);
}

static cnLib_UIViewController* cnLib_UIView_GetWindowViewController(UIView *self)
{
	return ocSafeCast<cnLib_UIViewController>(self.window.rootViewController);
}
//---------------------------------------------------------------------------
@end	// cnLib_UIView
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
iUIView* cMacTouchEvent::GetSourceView(void)
{
	return &pMacUIView(SenderView);
}
//---------------------------------------------------------------------------
iUITouchPoint* cMacTouchEvent::GetTouchPoint(void)
{
	return TouchPoint;
}
//---------------------------------------------------------------------------
bool cMacTouchEvent::GetPosition(iUIArea *Relative,cUIPoint &Position)
{
	auto View=ociSafeCast<UIView>(Relative);
	if(View==nullptr)
		return false;
	float scale=View.contentScaleFactor;
	auto pos=[Touch locationInView:View];
	Position.x=pos.x*scale;
	Position.y=pos.y*scale;
	return true;

}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cMacUIView::cMacUIView(cnLib_UIView *HostView)
{
	fView=HostView;
	fViewLayerHost=nil;

	fUIState=UIState::Null;
	fIncludeParentMargin=false;

	fMargin=RectangleZero;

	fView.multipleTouchEnabled=TRUE;

	fCalculateMarginFromParent=&fCalculateMarginFromParent_Default;

	// notify handlers
	for(auto *Handler : fLayoutHandlers){
		Handler->RectangleChanged(true,true);
	}

	gMainUIThread.OnUIAppStateChanged.Insert(&fAppStateChangeFunc);
}
//---------------------------------------------------------------------------
cMacUIView::~cMacUIView()
{
	cnLib_ASSERT(fViewLayerHost==nil);

	gMainUIThread.OnUIAppStateChanged.Remove(&fAppStateChangeFunc);
}
//---------------------------------------------------------------------------
void cnLib_FUNC cMacUIView::IncReference(void)
{
	[fView retain];
}
//---------------------------------------------------------------------------
void cnLib_FUNC cMacUIView::DecReference(void)
{
	[fView release];
}
//---------------------------------------------------------------------------
cnLib_UIView* cMacUIView::GetView(void)const
{
	return fView;
}
//---------------------------------------------------------------------------
const void* cnLib_FUNC cMacUIView::CastInterface(iTypeID InterfaceID)const
{
	return ImpCastInterface<iReference,iUIView,iUITouchControl,iOCObject,iUIViewCALayerHost>(this,InterfaceID);
}
//---------------------------------------------------------------------------
iUIThread* cMacUIView::GetUIThread(void)
{
	return &gMainUIThread;
}
//---------------------------------------------------------------------------
bool cMacUIView::InsertViewCALayer(iUIViewCALayer *ViewLayer)
{
	auto *Layer=ocSafeCast<CALayer>(ViewLayer->GetOCObject());
	if(Layer==nil){
		// invalid layer
		return false;
	}
	if(fViewLayers.Insert(ViewLayer)==false){
		return false;
	}
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
	default:
		// add to layer host
		[fViewLayerHost addSublayer:Layer];
		break;
	}
	ViewLayer->SetLayerScale(fView.contentScaleFactor);
	return true;
}
//---------------------------------------------------------------------------
bool cMacUIView::RemoveViewCALayer(iUIViewCALayer *ViewLayer)
{
	if(fViewLayers.Remove(ViewLayer)==false)
		return false;
	CALayer *Layer=ViewLayer->GetOCObject();
	[Layer removeFromSuperlayer];
	switch(fViewLayers.GetCount()){
	case 0:
		fViewLayerHost=nil;
		break;
	case 1:
		{
			// fallback to one layer only
			auto LastViewLayer=fViewLayers.GetItems()[0];
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
	}
	return true;
}
//---------------------------------------------------------------------------
void cMacUIView::onSetFrame(CGRect LastFrame)
{
	if(fView==nil)
		return;	// not initialized
	auto NewFrame=fView.frame;

	bool Moved=!CGPointEqualToPoint(LastFrame.origin,NewFrame.origin);
	bool Sized=!CGSizeEqualToSize(LastFrame.size,NewFrame.size);

	if(Moved || Sized){
		for(auto *Handler : fLayoutHandlers){
			Handler->RectangleChanged(Moved,Sized);
		}
	}
}
//---------------------------------------------------------------------------
void cMacUIView::ResetCalculateMarginFromParent(void)
{
	fCalculateMarginFromParent=&fCalculateMarginFromParent_Default;
}
//---------------------------------------------------------------------------
void cMacUIView::SetCalculateMarginFromParent(iCalculateMarginFromParent *Calculator)
{
	fCalculateMarginFromParent=Calculator;
}
//---------------------------------------------------------------------------
cRectangle cMacUIView::cCalculateMarginFromParent_Default::Calculate(void)
{
	auto Host=cnMemory::GetObjectFromMemberPointer(this,&cMacUIView::fCalculateMarginFromParent_Default);
	return Host->CalculateMarginFromParent_Default();
}
//---------------------------------------------------------------------------
cRectangle cMacUIView::CalculateMarginFromParent_Default(void)
{
	auto ParentView=ocSafeCast<cnLib_UIView>(fView.superview);
	if(ParentView==nullptr){
		return RectangleZero;
	}
	// calculate margin from parent
	auto &CPPParentView=pMacUIView(ParentView);
	cRectangle Margin=CPPParentView.fMargin;


	sfInt32 ParentWidth,ParentHeight;
	cRectangle ViewRect;
	{
		auto ParentSize=ParentView.bounds.size;
		auto ParentScale=ParentView.contentScaleFactor;
		ParentWidth=ParentSize.width*ParentScale;
		ParentHeight=ParentSize.height*ParentScale;

		auto ViewFrame=fView.frame;
		auto Scale=fView.contentScaleFactor;
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
void cMacUIView::onLayoutSubviews(void)
{
	// calculate margin
	if(fIncludeParentMargin){
		fMargin=fCalculateMarginFromParent->Calculate();
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

	// layout

	for(auto *Handler : fLayoutHandlers){
		Handler->UILayout();
	}


	// layout layers
	auto NewBounds=fView.bounds;

	auto ViewLayers=fViewLayers.GetItems();
	for(uIntn i=0,c=fViewLayers.GetCount();i<c;i++){
		ViewLayers[i]->SetLayerFrame(NewBounds);
	}

	if(fViewLayerHost!=nil){
		if(fViewLayers.GetCount()>1){
			fViewLayerHost.frame=NewBounds;
		}
		[fView.layer insertSublayer:fViewLayerHost atIndex:0];
	}
}
//---------------------------------------------------------------------------
void cMacUIView::onSetVisible(void)
{
	for(auto *Handler : fLayoutHandlers){
		Handler->VisibleChanged();
	}

	UpdateUIState();
}
//---------------------------------------------------------------------------
void cMacUIView::onSetEnabled(void)
{
	for(auto *Handler : fLayoutHandlers){
		Handler->EnableChanged();
	}
}
//---------------------------------------------------------------------------
void cMacUIView::onSetContentScale(void)
{
	for(auto *Handler : fLayoutHandlers){
		Handler->RectangleChanged(false,true);
	}

	auto ViewLayers=fViewLayers.GetItems();
	auto ContentScale=fView.contentScaleFactor;
	for(uIntn i=0,c=fViewLayers.GetCount();i<c;i++){
		ViewLayers[i]->SetLayerScale(ContentScale);
	}
}
//---------------------------------------------------------------------------
void cMacUIView::cAppStateChangeFunc::Execute(void)
{
	auto Host=cnMemory::GetObjectFromMemberPointer(this,&cMacUIView::fAppStateChangeFunc);
	return Host->AppStateChanged();
}
//---------------------------------------------------------------------------
void cMacUIView::AppStateChanged(void)
{
	UpdateUIState();
}
//---------------------------------------------------------------------------
void cMacUIView::UpdateUIState(void)
{
	auto PrevState=fUIState;

	// get new state

	fUIState=gMainUIThread.GetAppState();
	if(fUIState>UIState::Background){
		// check if view visible in layout tree
		if(fView.window==nil){
			fUIState=UIState::Background;
		}
	}

	if(fUIState==PrevState)
		return;

	cUIStateHandlerSetNotifier Notifier(fStateHandlers);
	UIStateNotifyChange(PrevState,fUIState,Notifier);

	for(auto *ViewLayer : fViewLayers){
		ViewLayer->OnViewStateChanged();
	}
}
//---------------------------------------------------------------------------
void cMacUIView::onWindowWillChange(UIWindow *NewWindow)
{
	UpdateUIState();
}
//---------------------------------------------------------------------------
void cMacUIView::onWindowChange(void)
{
	UpdateUIState();
}
//---------------------------------------------------------------------------
//void cMacUIView::onSendTouchExclusive(cMacUIView *Sender,iUITouchHandler *Exclude,iUITouchPoint *TouchPoint,UITouchAction Action)
//{
//	if(Exclude==nullptr)
//		return onSendTouch(Sender,TouchPoint,Action);
//
//	for(auto &p : fTouchHandlers){
//		if(p==Exclude)
//			continue;
//		CallTouchHandler(p,Sender,Action,TouchPoint);
//	}
//}
//---------------------------------------------------------------------------
//void cMacUIView::onSendTouch(cMacUIView *Sender,iUITouchPoint *TouchPoint,UITouchAction Action)
//{
//	for(auto &p : fTouchHandlers){
//		CallTouchHandler(p,Sender,Action,TouchPoint);
//	}
//}
//---------------------------------------------------------------------------
void cMacUIView::onSendTouchBegin(iUITouchEvent *Event)
{
	for(auto &p : fTouchHandlers){
		p->TouchBegin(Event);
	}
}
//---------------------------------------------------------------------------
void cMacUIView::onSendTouchMove(iUITouchEvent *Event)
{
	for(auto &p : fTouchHandlers){
		p->TouchMove(Event);
	}
}
//---------------------------------------------------------------------------
void cMacUIView::onSendTouchEnded(iUITouchEvent *Event)
{
	for(auto &p : fTouchHandlers){
		p->TouchEnd(Event);
	}
}
//---------------------------------------------------------------------------
void cMacUIView::onSendTouchLost(iUITouchEvent *Event)
{
	for(auto &p : fTouchHandlers){
		p->TouchLost(Event);
	}
}
//---------------------------------------------------------------------------
void cMacUIView::onSendTouchFilterBegin(iUITouchEvent *Event)
{
	for(auto &p : fTouchFilters){
		p->TouchBegin(Event);
	}
}
//---------------------------------------------------------------------------
void cMacUIView::onSendTouchFilterMove(iUITouchEvent *Event)
{
	for(auto &p : fTouchFilters){
		p->TouchMove(Event);
	}
}
//---------------------------------------------------------------------------
void cMacUIView::onSendTouchFilterEnded(iUITouchEvent *Event)
{
	for(auto &p : fTouchFilters){
		p->TouchEnd(Event);
	}
}
//---------------------------------------------------------------------------
void cMacUIView::onSendTouchFilterLost(iUITouchEvent *Event)
{
	for(auto &p : fTouchFilters){
		p->TouchLost(Event);
	}
}
//---------------------------------------------------------------------------
bool cMacUIView::CheckUIFunction(void)
{
	return [NSThread isMainThread];
}
//---------------------------------------------------------------------------
id cMacUIView::GetOCObject(void)
{
	return fView;
}
//---------------------------------------------------------------------------
UIState cnLib_FUNC cMacUIView::GetUIState(void)
{
	return fUIState;
}
//---------------------------------------------------------------------------
bool cnLib_FUNC cMacUIView::InsertStateHandler(iUIStateHandler *Handler,sfInt16 Order)
{
	if(CheckUIFunction()==false)
		return false;
	if(fStateHandlers.Insert(Handler)==false){
		return false;
	}

	UIStateNotifyChange(UIState::Null,fUIState,*Handler);

	return true;
}
//---------------------------------------------------------------------------
bool cnLib_FUNC cMacUIView::RemoveStateHandler(iUIStateHandler *Handler)
{
	if(CheckUIFunction()==false)
		return false;
	UIStateNotifyChange(fUIState,UIState::Null,*Handler);
	return fStateHandlers.Remove(Handler);
}
//---------------------------------------------------------------------------
uIntn cMacUIView::FindRelativeBase(cUIPoint &Offset,iUIArea*const *Relatives,uIntn Count)
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
		auto OCView=ociSafeCast<UIView>(Relatives[i]);
		if(OCView!=nullptr){
			CGPoint p={0,0};
			p=[fView convertPoint:p toView:OCView];
			auto scale=fView.contentScaleFactor;
			Offset.x=p.x*scale;
			Offset.y=p.y*scale;
			return i;
		}
	}
	return IndexNotFound;
}
//---------------------------------------------------------------------------
bool cMacUIView::GetPosition(iUIArea *Relative,cUIPoint &Position)
{
	if(CheckUIFunction()==false)
		return false;
	if(Relative==this){
		Position={0,0};
		return true;
	}
	if(Relative==nullptr){
		Position={0,0};
		return true;
	}
	auto scale=fView.contentScaleFactor;
	auto OCView=ociSafeCast<UIView>(Relative);
	if(OCView!=nullptr){
		auto RelativeView=OCView;
		CGPoint p;
		if(RelativeView==fView.superview){
			p=fView.frame.origin;
		}
		else{
			p=[fView convertPoint:{0,0} toView:RelativeView];
		}
		Position.x=p.x*scale;
		Position.y=p.y*scale;
		return true;
	}
	return false;
}
//---------------------------------------------------------------------------
bool cMacUIView::SetPosition(iUIArea *Relative,cUIPoint Position)
{
	if(CheckUIFunction()==false)
		return false;
	auto Frame=fView.frame;
	auto scale=1/fView.contentScaleFactor;
	Frame.origin.x=Position.x*scale;
	Frame.origin.y=Position.y*scale;
	return SetFrameRelativeTo(Relative,Frame);
}
//---------------------------------------------------------------------------
cUIPoint cMacUIView::GetSize(void)
{
	if(CheckUIFunction()==false)
		return {0,0};
	auto scale=fView.contentScaleFactor;
	auto FrameSize=fView.bounds.size;
	cUIPoint Size;
	Size.x=FrameSize.width*scale;
	Size.y=FrameSize.height*scale;
	return Size;
}
//---------------------------------------------------------------------------
bool cMacUIView::SetSize(cUIPoint Size)
{
	if(CheckUIFunction()==false)
		return false;
	auto scale=fView.contentScaleFactor;
	auto Frame=fView.frame;
	Frame.size.width=Size.x/scale;
	Frame.size.height=Size.y/scale;
	fView.frame=Frame;
	return true;
}
//---------------------------------------------------------------------------
bool cMacUIView::SetRectangle(iUIArea *Relative,cUIPoint Position,cUIPoint Size)
{
	if(CheckUIFunction()==false)
		return false;
	auto Frame=fView.frame;
	auto scale=1/fView.contentScaleFactor;
	Frame.origin.x=Position.x*scale;
	Frame.origin.y=Position.y*scale;
	Frame.size.width=Size.x*scale;
	Frame.size.height=Size.y*scale;
	return SetFrameRelativeTo(Relative,Frame);
}
//---------------------------------------------------------------------------
bool cMacUIView::SetFrameRelativeTo(iUIArea *Relative,CGRect Frame)
{
	if(Relative==this){
		auto CurFrame=fView.frame;
		Frame.origin.x+=CurFrame.origin.x;
		Frame.origin.y+=CurFrame.origin.y;
		fView.frame=Frame;
		return true;
	}
	if(Relative==nullptr){
		Frame.origin=[fView convertPoint:Frame.origin fromView:nil];
		fView.frame=Frame;
		return true;
	}
	auto RelativeOCView=ociSafeCast<UIView>(Relative);
	if(RelativeOCView!=nullptr){
		if(RelativeOCView!=fView.superview){
			Frame.origin=[fView convertPoint:Frame.origin fromView:RelativeOCView];
		}
		fView.frame=Frame;
		return true;
	}
	return false;
}
//---------------------------------------------------------------------------
iUIWindow* cMacUIView::GetWindow(void)
{
	if(CheckUIFunction()==false)
		return nullptr;
	auto WindowObject=cnLib_UIView_GetWindowViewController(fView);
	if(WindowObject==nullptr)
		return nullptr;
	return cnLib_UIViewController_GetInterface(WindowObject);
}
//---------------------------------------------------------------------------
iUIView* cMacUIView::GetParent(void)
{
	if(CheckUIFunction()==false)
		return nullptr;
	auto SuperView=ocSafeCast<cnLib_UIView>(fView.superview);
	if(SuperView==nil)
		return nullptr;
	return cnLib_UIView_GetUIView(SuperView);
}
//---------------------------------------------------------------------------
bool cMacUIView::InsertView(iUIView *View)
{
	if(CheckUIFunction()==false)
		return false;
	auto *ChildView=ociSafeCast<cnLib_UIView>(View);
	if(ChildView==nullptr)
		return false;
	auto &CPPChildView=pMacUIView(ChildView);
	auto Sibling=FindNextSiblingForZIndex(ChildView,fView,CPPChildView.fZIndex);
	if(Sibling==nullptr){
		[fView addSubview:ChildView];
	}
	else{
		[fView insertSubview:ChildView belowSubview:Sibling];
	}
	return true;
}
//---------------------------------------------------------------------------
bool cMacUIView::RemoveView(iUIView *View)
{
	if(CheckUIFunction()==false)
		return false;
	auto *ChildView=ociSafeCast<UIView>(View);
	if(ChildView==nullptr || ChildView.superview!=fView)
		return false;
	[ChildView removeFromSuperview];
	return true;
}
//---------------------------------------------------------------------------
bool cMacUIView::GetVisible(void)
{
	if(CheckUIFunction()==false)
		return false;
	return !fView.hidden;
}
//---------------------------------------------------------------------------
bool cMacUIView::SetVisible(bool Visible)
{
	if(CheckUIFunction()==false)
		return false;
	fView.hidden=!Visible;
	return true;
}
//---------------------------------------------------------------------------
bool cMacUIView::IsEnabled(void)
{
	if(CheckUIFunction()==false)
		return false;
	return fView.userInteractionEnabled;
}
//---------------------------------------------------------------------------
bool cMacUIView::GetEnable(void)
{
	if(CheckUIFunction()==false)
		return false;
	return fView.userInteractionEnabled;
}
//---------------------------------------------------------------------------
bool cMacUIView::SetEnable(bool Enable)
{
	if(CheckUIFunction()==false)
		return false;
	fView.userInteractionEnabled=Enable;
	return true;
}
//---------------------------------------------------------------------------
sfInt16 cMacUIView::GetZIndex(void)
{
	return fZIndex;
}
//---------------------------------------------------------------------------
bool cMacUIView::SetZIndex(sfInt16 Index)
{
	if(fZIndex==Index)
		return true;

	fZIndex=Index;

	auto SuperView=fView.superview;
	if(SuperView!=nil){
		auto Sibling=FindNextSiblingForZIndex(fView,SuperView,fZIndex);
		if(Sibling==nullptr){
			[SuperView bringSubviewToFront:fView];
		}
		else{
			[SuperView insertSubview:fView belowSubview:Sibling];
		}
	}
	return true;
}
//---------------------------------------------------------------------------
UIView* cMacUIView::FindNextSiblingForZIndex(UIView *SubView,UIView *Parent,sfInt16 ZIndex)
{
	for(UIView *sv in Parent.subviews){
		if(sv==SubView)
			continue;
		auto SubView=ocSafeCast<cnLib_UIView>(sv);
		if(SubView==nil)
			continue;

		auto &CPPSibling=pMacUIView(SubView);
		if(CPPSibling.fZIndex>ZIndex){
			return sv;
		}
	}
	return nil;
}
//---------------------------------------------------------------------------
Float32 cMacUIView::GetContentScale(void)
{
	return fView.contentScaleFactor;
}
//---------------------------------------------------------------------------
void cMacUIView::SetArrangement(void)
{
	[fView setNeedsLayout];
}
//---------------------------------------------------------------------------
void cMacUIView::ArrangeLayout(void)
{
	[fView layoutIfNeeded];
}
//---------------------------------------------------------------------------
void cMacUIView::SetIncludeParentFrameMargin(bool Include)
{
	if(fIncludeParentMargin==Include)
		return;

	fIncludeParentMargin=Include;
	auto ParentMargin=fCalculateMarginFromParent->Calculate();
	if(ParentMargin.Left!=0 || ParentMargin.Right!=0 ||
		 ParentMargin.Top==0 || ParentMargin.Bottom==0)
	{
		[fView setNeedsLayout];
	}
}
//---------------------------------------------------------------------------
cRectangle cMacUIView::GetFrameMargin(void)
{
	return fMargin;
}
//---------------------------------------------------------------------------
bool cMacUIView::InsertLayoutHandler(iUILayoutHandler *Handler,sfInt16 Order)
{
	if(CheckUIFunction()==false)
		return false;
	if(fLayoutHandlers.Insert(Handler)==false){
		return false;
	}

	if(fView!=nullptr){
		Handler->RectangleChanged(true,true);
		[fView setNeedsLayout];
	}
	return true;
}
//---------------------------------------------------------------------------
bool cMacUIView::RemoveLayoutHandler(iUILayoutHandler *Handler)
{
	if(CheckUIFunction()==false)
		return false;
	if(fLayoutHandlers.Remove(Handler)==false){
		return false;
	}
	[fView setNeedsLayout];
	return true;

}
//---------------------------------------------------------------------------
bool cMacUIView::IsFocused(void)
{
	if(CheckUIFunction()==false)
		return false;
	return fView.isFirstResponder;
}
//---------------------------------------------------------------------------
bool cMacUIView::SetFocus(bool Set)
{
	if(CheckUIFunction()==false)
		return false;
	if(Set)
		return [fView becomeFirstResponder];
	else
		return [fView resignFirstResponder];
}
//---------------------------------------------------------------------------
bool cMacUIView::InsertTouchHandler(iUITouchHandler *Handler,sfInt16 Order)
{
	if(CheckUIFunction()==false)
		return false;
	return fTouchHandlers.Insert(Handler);
}
//---------------------------------------------------------------------------
bool cMacUIView::RemoveTouchHandler(iUITouchHandler *Handler)
{
	if(CheckUIFunction()==false)
		return false;
	return fTouchHandlers.Remove(Handler);
}
//---------------------------------------------------------------------------
bool cMacUIView::InsertTouchFilter(iUITouchHandler *Filter,sfInt16 Order)
{
	return false;
}
//---------------------------------------------------------------------------
bool cMacUIView::RemoveTouchFilter(iUITouchHandler *Filter)
{
	return false;
}
//---------------------------------------------------------------------------
bool cMacUIView::TouchAcquireExclusive(iUITouchHandler *Handler)
{
	if(CheckUIFunction()==false)
		return false;
	//return gMainUIThread.TouchDispatch.TouchExclusive(Handler);
	return false;
}
//---------------------------------------------------------------------------
void cMacUIView::TouchReleaseExclusive(iUITouchHandler *Handler)
{
	if(CheckUIFunction()==false)
		return;
	//return gMainUIThread.TouchDispatch.TouchExclusive(Handler);
}
//---------------------------------------------------------------------------
