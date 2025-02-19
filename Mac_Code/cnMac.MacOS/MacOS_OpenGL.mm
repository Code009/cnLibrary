#import "MacOS_OpenGL.h"

using namespace cnLibrary;
using namespace cnRTL;
using namespace cnMac;


cnLib_INTERFACE_LOCALID_DEFINE(cOpenGLContext);

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cOpenGLContext::cOpenGLContext(CGLContextObj Context)
	: fContext(Context)
{
}
//---------------------------------------------------------------------------
cOpenGLContext::~cOpenGLContext()
{
	CGLDestroyContext(fContext);
}
//---------------------------------------------------------------------------
CGLContextObj cOpenGLContext::GetCGLContext(void)
{
	return fContext;
}
//---------------------------------------------------------------------------
void cOpenGLContext::Enter(void)
{
	CGLSetCurrentContext(fContext);
}
//---------------------------------------------------------------------------
void cOpenGLContext::Leave(void)
{
	CGLSetCurrentContext(nullptr);
}
//---------------------------------------------------------------------------


@implementation cnLib_OpenGLLayer
{
@private
	alignas(cOpenGLViewLayer) ocStaticIVar<cOpenGLViewLayer> cnLib_IVar;
}

static cOpenGLViewLayer* CPPOpenGLLayer(cnLib_OpenGLLayer *self){
	return reinterpret_cast<cOpenGLViewLayer*>(&self->cnLib_IVar);
}
cOpenGLViewLayer* cnMac::ViewContentFromOpenGLViewLayer(cnLib_OpenGLLayer *Layer)
{
	return CPPOpenGLLayer(Layer);
}

-(instancetype)init
{
	self=[super init];
	if(self){
		cnVar::ManualConstruct(*CPPOpenGLLayer(self),self);
	}
	return self;
}

-(void)dealloc
{
	cnVar::ManualDestruct(*CPPOpenGLLayer(self));
	[super dealloc];
}

-(id<CAAction>)actionForKey:(NSString *)event
{
	return [NSNull null];
}


-(CGLPixelFormatObj)copyCGLPixelFormatForDisplayMask:(uint32_t)mask
{
	auto CPP=CPPOpenGLLayer(self);
	return CPP->LayerAcquirePixelFormat(mask);
}
-(void)releaseCGLPixelFormat:(CGLPixelFormatObj)pf
{
	auto CPP=CPPOpenGLLayer(self);
	return CPP->LayerReleasePixelFormat(pf);
}


-(CGLContextObj)copyCGLContextForPixelFormat:(CGLPixelFormatObj)pf
{
	auto CPP=CPPOpenGLLayer(self);
	return CPP->LayerAcquireContext(pf);
}

-(void)releaseCGLContext:(CGLContextObj)ctx
{
	auto CPP=CPPOpenGLLayer(self);
	return CPP->LayerReleaseContext(ctx);
}

-(void)drawInCGLContext:(CGLContextObj)ctx pixelFormat:(CGLPixelFormatObj)pf forLayerTime:(CFTimeInterval)t displayTime:(const CVTimeStamp *)ts
{
	auto CPP=CPPOpenGLLayer(self);

	CPP->LayerGLOnDraw();

}
@end

//---------------------------------------------------------------------------
cOpenGLViewLayer::cOpenGLViewLayer(cnLib_OpenGLLayer *Layer)
{
	fLayer=Layer;

	fPainterState=UIState::Null;
}
//---------------------------------------------------------------------------
cOpenGLViewLayer::~cOpenGLViewLayer()
{
}
//---------------------------------------------------------------------------
const void* cOpenGLViewLayer::CastInterface(iTypeID IID)const
{
	return ImpCastInterface<iOCObject,iUIViewCALayer,iReference>(this,IID);
}
//---------------------------------------------------------------------------
void cOpenGLViewLayer::IncReference(void)
{
	[fLayer retain];
}
//---------------------------------------------------------------------------
void cOpenGLViewLayer::DecReference(void)
{
	[fLayer release];
}
//---------------------------------------------------------------------------
id cOpenGLViewLayer::GetOCObject(void)
{
	return fLayer;
}
//---------------------------------------------------------------------------
CALayer* cOpenGLViewLayer::GetLayer(void)
{
	return fLayer;
}
//---------------------------------------------------------------------------
void cOpenGLViewLayer::NotifyLayout(void)
{
	if(fPainter!=nullptr){
		fPainter->PaintRectChanged();
		[fLayer setNeedsDisplay];
	}
}
//---------------------------------------------------------------------------
void cOpenGLViewLayer::SetLayerFrame(CGRect Frame)
{
	fLayer.frame=Frame;
	NotifyLayout();
}
//---------------------------------------------------------------------------
void cOpenGLViewLayer::SetLayerScale(CGFloat Scale)
{
	if(fLayer.contentsScale==Scale)
		return;
	fLayer.contentsScale=Scale;
	NotifyLayout();
}
//---------------------------------------------------------------------------
void cOpenGLViewLayer::OnViewStateChanged(void)
{
	UpdatePainterState();
}
//---------------------------------------------------------------------------
void cOpenGLViewLayer::UpdatePainterState(void)
{
	if(fPainter==nullptr){
		fPainterState=UIState::Null;
		return;
	}
	eUIState NewState;
	if(fHostView==nullptr){
		NewState=UIState::Null;
	}
	else if(fContext==nullptr){
		NewState=UIState::Null;
	}
	else{
		NewState=fHostView->GetUIState();
	}

	if(fPainterState==NewState){
		return;
	}
	auto PrevState=fPainterState;
	fPainterState=NewState;
	NotifyPainterState(PrevState,fPainterState);
}
//---------------------------------------------------------------------------
void cOpenGLViewLayer::cPainterNotifier::UIStarted(void){
	This->fPainter->PaintStarted();
	if(This->fNeedNotifyRectChange){
		This->fPainter->PaintRectChanged();
		This->fNeedNotifyRectChange=false;
	}
}
void cOpenGLViewLayer::cPainterNotifier::UIShow(void){		This->fPainter->PaintShow();	}
void cOpenGLViewLayer::cPainterNotifier::UIResume(void){	This->fPainter->PaintResume();	}
void cOpenGLViewLayer::cPainterNotifier::UIPaused(void){	This->fPainter->PaintPaused();	}
void cOpenGLViewLayer::cPainterNotifier::UIHide(void){		This->fPainter->PaintHide();	}
void cOpenGLViewLayer::cPainterNotifier::UIStopped(void){	This->fPainter->PaintStopped();	}
//---------------------------------------------------------------------------
void cOpenGLViewLayer::NotifyPainterState(eUIState PrevState,eUIState NextState)
{
	if(fPainter==nullptr)
		return;
	cPainterNotifier Notifier={this};
	UIStateNotifyChange(PrevState,NextState,Notifier);
}
//---------------------------------------------------------------------------
iUIView* cOpenGLViewLayer::GetView(void)
{
	return fHostView;
}
//---------------------------------------------------------------------------
bool cOpenGLViewLayer::SetView(iUIView *View)
{
	if(fHostView!=nullptr){
		// notify painter stopped
		if(fPainterState!=UIState::Null){
			auto PrevState=fPainterState;
			fPainterState=UIState::Null;
			NotifyPainterState(PrevState,UIState::Null);
		}
		// remove from previous host
		auto LayerHost=iCast<iUIViewCALayerHost>(fHostView);
		LayerHost->RemoveViewCALayer(this);
	}
	cnLib_ASSERT(fPainterState==UIState::Null);
	fHostView=nullptr;
	if(View==nullptr){
		return true;
	}
	auto LayerHost=iCast<iUIViewCALayerHost>(View);
	if(LayerHost==nullptr){
		return false;
	}
	if(LayerHost->InsertViewCALayer(this)==false){
		return false;
	}
	fHostView=View;
	if(fPainter!=nullptr){
		fNeedNotifyRectChange=true;
		// recalculate new state
		UpdatePainterState();
	}
	return true;
}
//---------------------------------------------------------------------------
bool cOpenGLViewLayer::GetVisible(void)
{
	return !fLayer.hidden;
}
//---------------------------------------------------------------------------
bool cOpenGLViewLayer::SetVisible(bool Visible)
{
	fLayer.hidden=!Visible;
	return true;
}
//---------------------------------------------------------------------------
sfInt16 cOpenGLViewLayer::GetZIndex(void)
{
	return static_cast<sfInt16>(fLayer.zPosition);
}
//---------------------------------------------------------------------------
bool cOpenGLViewLayer::SetZIndex(sfInt16 ZIndex)
{
	fLayer.zPosition=ZIndex;
	return true;
}
//---------------------------------------------------------------------------
void cOpenGLViewLayer::SetPainter(iOpenGLViewPainter *Painter)
{
	if(fPainter!=nullptr){
		NotifyPainterState(fPainterState,UIState::Null);
	}
	fPainter=Painter;
	if(fPainter!=nullptr){
		fNeedNotifyRectChange=true;
		NotifyPainterState(UIState::Null,fPainterState);
	}
}
//---------------------------------------------------------------------------
cUIPoint cOpenGLViewLayer::GetPaintSize(void)
{
	auto Size=fLayer.bounds.size;
	auto cs=fLayer.contentsScale;
	sfInt32n x=Size.width*cs;
	sfInt32n y=Size.height*cs;
	return {x,y};
}
//---------------------------------------------------------------------------
iOpenGLContext* cOpenGLViewLayer::GetOpenGLContext(void)
{
	return fContext;
}
//---------------------------------------------------------------------------
void cOpenGLViewLayer::SetRender(void)
{
	[fLayer setNeedsDisplay];
}
//---------------------------------------------------------------------------
CGLPixelFormatObj cOpenGLViewLayer::LayerAcquirePixelFormat(uint32_t mask)
{
	CGLPixelFormatAttribute Attributes[]={
		kCGLPFADisplayMask,static_cast<CGLPixelFormatAttribute>(mask),
		kCGLPFANoRecovery,
		kCGLPFAAccelerated,
		static_cast<CGLPixelFormatAttribute>(0)
	};
	CGLPixelFormatObj PixelFormat;
	GLint PixelFormatCount=0;
	CGLChoosePixelFormat(Attributes,&PixelFormat,&PixelFormatCount);

	return PixelFormat;
}
//---------------------------------------------------------------------------
void cOpenGLViewLayer::LayerReleasePixelFormat(CGLPixelFormatObj pf)
{
	CGLDestroyPixelFormat(pf);
}
//---------------------------------------------------------------------------
CGLContextObj cOpenGLViewLayer::LayerAcquireContext(CGLPixelFormatObj pf)
{
	CGLContextObj Shared=nullptr;
	if(SharedContext!=nullptr){
		Shared=SharedContext->GetCGLContext();
	}
	CGLContextObj NewContext;
	CGLCreateContext(pf,Shared,&NewContext);
	fContext=iCreate<cOpenGLContext>(NewContext);

	UpdatePainterState();
	return NewContext;
}
//---------------------------------------------------------------------------
void cOpenGLViewLayer::LayerReleaseContext(CGLContextObj ctx)
{
	if(fContext->GetCGLContext()==ctx){
		fContext=nullptr;
		UpdatePainterState();
	}
}
//---------------------------------------------------------------------------
void cOpenGLViewLayer::LayerGLOnDraw(void)
{
	if(fPainterState>UIState::Background){
		fPainter->GLPaint();
	}
}
//---------------------------------------------------------------------------
iPtr<iOpenGLContext> cnMac::CreateOpenGLContext(CGLPixelFormatObj PixelFormatObject,iOpenGLContext *SharedContext)
{
	CGLContextObj SharedObject;
	auto OpenGLContext=iCast<cOpenGLContext>(SharedContext);
	if(OpenGLContext!=nullptr){
		SharedObject=OpenGLContext->GetCGLContext();
	}
	else{
		SharedObject=nullptr;
	}


	if(PixelFormatObject==nullptr){
		if(SharedObject==nullptr){
			return nullptr;
		}
		PixelFormatObject=CGLGetPixelFormat(SharedObject);
	}

	CGLContextObj NewContext;
	CGLCreateContext(PixelFormatObject,SharedObject,&NewContext);
	if(NewContext==nullptr)
		return nullptr;

	return iCreate<cOpenGLContext>(NewContext);
}
//---------------------------------------------------------------------------
