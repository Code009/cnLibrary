#include <OpenGLES/ES2/gl.h>
#import "iOS_UIPaintTarget.h"

using namespace cnLibrary;
using namespace cnRTL;
using namespace cnMac;

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cOpenGLESContext::cOpenGLESContext(ocPtr<EAGLContext> Context)
	: fContext(ocDetach(Context))
{
}
//---------------------------------------------------------------------------
cOpenGLESContext::~cOpenGLESContext()
{
	[fContext release];
}
//---------------------------------------------------------------------------
id cOpenGLESContext::GetOCObject(void)
{
	return fContext;
}
//---------------------------------------------------------------------------
void cOpenGLESContext::Enter(void)
{
	[EAGLContext setCurrentContext:fContext];
}
//---------------------------------------------------------------------------
void cOpenGLESContext::Leave(void)
{
	[EAGLContext setCurrentContext:nil];
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void cOpenGLESRenderBuffer::GLSetup(void)
{
	glGenFramebuffers(1,&FrameBuffer);
	glGenRenderbuffers(2,RenderBuffer);
}
//---------------------------------------------------------------------------
void cOpenGLESRenderBuffer::GLClear(void)
{
	glDeleteRenderbuffers(2,RenderBuffer);
	glDeleteFramebuffers(1,&FrameBuffer);
}
//---------------------------------------------------------------------------
void cOpenGLESRenderBuffer::GLBind(void)
{
	glBindFramebuffer(GL_FRAMEBUFFER,FrameBuffer);
	glBindRenderbuffer(GL_RENDERBUFFER,RenderBuffer[0]);
}
//---------------------------------------------------------------------------
void cOpenGLESRenderBuffer::GLSetupBuffer(GLenum RenderFormat,sfInt32 Width,sfInt32 Height)
{
	glBindFramebuffer(GL_FRAMEBUFFER,FrameBuffer);
	// content buffer
	glBindRenderbuffer(GL_RENDERBUFFER,RenderBuffer[0]);
	glRenderbufferStorage(GL_RENDERBUFFER,RenderFormat,Width,Height);

	glFramebufferRenderbuffer(GL_FRAMEBUFFER,GL_COLOR_ATTACHMENT0,GL_RENDERBUFFER,RenderBuffer[0]);

	glBindRenderbuffer(GL_RENDERBUFFER,RenderBuffer[1]);
	glRenderbufferStorage(GL_RENDERBUFFER,GL_DEPTH_COMPONENT16,Width,Height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER,GL_DEPTH_ATTACHMENT,GL_RENDERBUFFER,RenderBuffer[1]);
}
//---------------------------------------------------------------------------
bool cOpenGLESRenderBuffer::GLSetupDrawableBuffer(EAGLContext *Context,id<EAGLDrawable> Drawable)
{
	glBindFramebuffer(GL_FRAMEBUFFER,FrameBuffer);
	// content buffer
	glBindRenderbuffer(GL_RENDERBUFFER, RenderBuffer[0]);

	if([Context renderbufferStorage:GL_RENDERBUFFER fromDrawable:Drawable]==false){
		//NSLog(@"Failed to renderbufferStorage\n");
		return false;
	}

	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, RenderBuffer[0]);

	GLint w,h;
	glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_WIDTH, &w);
	glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_HEIGHT, &h);

	glBindRenderbuffer(GL_RENDERBUFFER,RenderBuffer[1]);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, w, h);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, RenderBuffer[1]);

	glBindRenderbuffer(GL_RENDERBUFFER,RenderBuffer[0]);
	return true;
}
//---------------------------------------------------------------------------
void cOpenGLESRenderBuffer::GLSetupWithRenderBuffer(void)
{
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER,RenderBuffer[0]);

	GLint w,h;
	glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_WIDTH, &w);
	glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_HEIGHT, &h);

	glBindRenderbuffer(GL_RENDERBUFFER,RenderBuffer[1]);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, w, h);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER,RenderBuffer[1]);

	glBindRenderbuffer(GL_RENDERBUFFER,RenderBuffer[0]);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cOpenGLESViewLayer::cOpenGLESViewLayer()
{
	fLayer=[[CAEAGLLayer alloc]init];

	fPainterState=UIState::Null;
}
//---------------------------------------------------------------------------
cOpenGLESViewLayer::~cOpenGLESViewLayer()
{
	[fLayer removeFromSuperlayer];
	[fLayer release];
}
//---------------------------------------------------------------------------
const void* cOpenGLESViewLayer::CastInterface(iTypeID IID)const
{
	return ImpCastInterface<iOCObject>(this,IID);
}
//---------------------------------------------------------------------------
id cOpenGLESViewLayer::GetOCObject(void)
{
	return fLayer;
}
//---------------------------------------------------------------------------
CALayer* cOpenGLESViewLayer::GetLayer(void)
{
	return fLayer;
}
//---------------------------------------------------------------------------
iOpenGLContext* cOpenGLESViewLayer::GetGLESContext(void)
{
	return fGLESContext;
}
//---------------------------------------------------------------------------
void cOpenGLESViewLayer::SetGLESContext(iOpenGLContext *OpenGLContext)
{
	EAGLContext *NewContext;
	if(OpenGLContext!=nullptr){
		NewContext=ociSafeCast<EAGLContext>(OpenGLContext);
		if(NewContext==nil)
			return;
	}
	else{
		NewContext=nil;
	}
	bool Active=IsRenderActive();
	if(Active){
		if(fContext!=nullptr){
			[EAGLContext setCurrentContext:fContext];
			GLClear();
		}
	}
	fGLESContext=OpenGLContext;
	fContext=NewContext;
	if(Active){
		[EAGLContext setCurrentContext:fContext];
		if(fContext!=nullptr){
			GLSetup();
			[EAGLContext setCurrentContext:nil];
		}
	}
}
//---------------------------------------------------------------------------
bool cOpenGLESViewLayer::IsRenderActive(void)
{
	return fPainterState>UIState::Background;
}
//---------------------------------------------------------------------------
void cOpenGLESViewLayer::NotifyLayout(void)
{
	if(fPainter!=nullptr){
		fPainter->PaintRectChanged();
		fNeedDraw=true;
		fNeedSetupDrawableBuffer=true;
		UpdatePainterState();
		if(fNeedDraw){
			Render();
		}
	}
}
//---------------------------------------------------------------------------
void cOpenGLESViewLayer::SetLayerFrame(CGRect Frame)
{
	fLayer.frame=Frame;
	NotifyLayout();
}
//---------------------------------------------------------------------------
void cOpenGLESViewLayer::SetLayerScale(CGFloat Scale)
{
	if(fLayer.contentsScale==Scale)
		return;
	fLayer.contentsScale=Scale;
	NotifyLayout();
}
//---------------------------------------------------------------------------
void cOpenGLESViewLayer::OnViewStateChanged(void)
{
	UpdatePainterState();
}
//---------------------------------------------------------------------------
void cOpenGLESViewLayer::UpdatePainterState(void)
{
	auto PrevState=fPainterState;
	if(fPainter==nullptr || fHostView==nullptr || fContext==nullptr){
		fPainterState=UIState::Null;
	}
	else{
		fPainterState=fHostView->GetUIState();

		if(fPainterState>UIState::Background){
			auto Size=fLayer.bounds.size;
			if(Size.width<=0 || Size.height<=0){
				fPainterState=UIState::Background;
			}
		}
	}

	if(PrevState!=fPainterState){
		NotifyPainterState(PrevState,fPainterState);
	}
}
//---------------------------------------------------------------------------
void cOpenGLESViewLayer::UIStarted(void)
{
	fPainter->PaintStarted();
	if(fNeedNotifyRectChange){
		fPainter->PaintRectChanged();
		fNeedNotifyRectChange=false;
	}
}
//---------------------------------------------------------------------------
void cOpenGLESViewLayer::UIShow(void)
{
	if(fContext!=nil){
		[EAGLContext setCurrentContext:fContext];
		GLSetup();
	}

	fNeedDraw=true;
	fPainter->PaintShow();
}
//---------------------------------------------------------------------------
void cOpenGLESViewLayer::UIResume(void)
{
	fPainter->PaintResume();

	RenderProcess();
}
//---------------------------------------------------------------------------
void cOpenGLESViewLayer::UIPaused(void)
{
	fPainter->PaintPaused();
}
//---------------------------------------------------------------------------
void cOpenGLESViewLayer::UIHide(void)
{
	fPainter->PaintHide();

	if(fContext!=nil){
		[EAGLContext setCurrentContext:fContext];
		GLClear();
		[EAGLContext setCurrentContext:nil];
	}
}
//---------------------------------------------------------------------------
void cOpenGLESViewLayer::UIStopped(void)
{
	fPainter->PaintStopped();
}
//---------------------------------------------------------------------------
void cOpenGLESViewLayer::cStateNotifier::UIStarted(void){	This->UIStarted();	}
void cOpenGLESViewLayer::cStateNotifier::UIShow(void){		This->UIShow();	}
void cOpenGLESViewLayer::cStateNotifier::UIResume(void){	This->UIResume();	}
void cOpenGLESViewLayer::cStateNotifier::UIPaused(void){	This->UIPaused();	}
void cOpenGLESViewLayer::cStateNotifier::UIHide(void){		This->UIHide();	}
void cOpenGLESViewLayer::cStateNotifier::UIStopped(void){	This->UIStopped();	}
//---------------------------------------------------------------------------
void cOpenGLESViewLayer::NotifyPainterState(UIState PrevState,UIState NextState)
{
	if(fPainter==nullptr)
		return;

	cStateNotifier Notifier={this};
	UIStateNotifyChange(PrevState,NextState,Notifier);
}
//---------------------------------------------------------------------------
iUIView* cOpenGLESViewLayer::GetView(void)
{
	return fHostView;
}
//---------------------------------------------------------------------------
bool cOpenGLESViewLayer::SetView(iUIView *View)
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
bool cOpenGLESViewLayer::GetVisible(void)
{
	return !fLayer.hidden;
}
//---------------------------------------------------------------------------
bool cOpenGLESViewLayer::SetVisible(bool Visible)
{
	fLayer.hidden=!Visible;
	return true;
}
//---------------------------------------------------------------------------
sfInt16 cOpenGLESViewLayer::GetZIndex(void)
{
	return static_cast<sfInt16>(fLayer.zPosition);
}
//---------------------------------------------------------------------------
bool cOpenGLESViewLayer::SetZIndex(sfInt16 ZIndex)
{
	fLayer.zPosition=ZIndex;
	return true;
}
//---------------------------------------------------------------------------
void cOpenGLESViewLayer::SetPainter(iOpenGLViewPainter *Painter)
{
	if(fPainter!=nullptr){
		fPainterState=UIState::Null;
		NotifyPainterState(fPainterState,UIState::Null);
	}
	fPainter=Painter;
	if(fPainter!=nullptr){
		fNeedNotifyRectChange=true;
		UpdatePainterState();
	}
}
//---------------------------------------------------------------------------
cUIPoint cOpenGLESViewLayer::GetPaintSize(void)
{
	auto Size=fLayer.bounds.size;
	auto cs=fLayer.contentsScale;
	sfInt32 x=static_cast<sfInt32>(Size.width*cs);
	sfInt32 y=static_cast<sfInt32>(Size.height*cs);
	return {x,y};
}
//---------------------------------------------------------------------------
iOpenGLContext* cOpenGLESViewLayer::GetOpenGLContext(void)
{
	return fGLESContext;
}
//---------------------------------------------------------------------------
void cOpenGLESViewLayer::SetRender(void)
{
	QueryRender();
}
//---------------------------------------------------------------------------
void cOpenGLESViewLayer::GLSetup(void)
{
	fRenderBuffer.GLSetup();
	fNeedSetupDrawableBuffer=true;
}
//---------------------------------------------------------------------------
void cOpenGLESViewLayer::GLClear(void)
{
	fRenderBuffer.GLClear();
}
//---------------------------------------------------------------------------
void cOpenGLESViewLayer::QueryRender(void)
{
	if(fNeedDraw)
		return;
	fNeedDraw=true;

	if(IsRenderActive()){
		auto DispatchQueue=dispatch_get_main_queue();
		dispatch_async_f(DispatchQueue,this,DispatchFunc_Render);
	}
}
//---------------------------------------------------------------------------
void cOpenGLESViewLayer::DispatchFunc_Render(void *Parameter)
{
	auto This=static_cast<cOpenGLESViewLayer*>(Parameter);
	This->Render();
}
//---------------------------------------------------------------------------
void cOpenGLESViewLayer::Render(void)
{
	if(IsRenderActive()==false){
		return;
	}
	RenderProcess();
}
//---------------------------------------------------------------------------
void cOpenGLESViewLayer::RenderProcess(void)
{
	fNeedDraw=false;
	[EAGLContext setCurrentContext:fContext];

	if(fNeedSetupDrawableBuffer){
		fNeedSetupDrawableBuffer=false;
		fRenderBuffer.GLSetupDrawableBuffer(fContext,fLayer);
	}

	fPainter->GLPaint();

	[fContext presentRenderbuffer:GL_RENDERBUFFER];

	[EAGLContext setCurrentContext:nil];
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
EAGLSharegroup* cnMac::GetEAGLSharegroup(iOpenGLContext *Context)
{
	auto SharedEAGL=ociSafeCast<EAGLContext>(Context);
	if(SharedEAGL!=nullptr){
		return SharedEAGL.sharegroup;
	}
	auto ShareGroup=ociSafeCast<EAGLSharegroup>(Context);
	if(ShareGroup!=nullptr){
		return ShareGroup;
	}
	return nil;
}
//---------------------------------------------------------------------------



/*
class cOpenGLESMemoryPaintTarget : public iOpenGLESPaintTarget
{
protected:
	iOpenGLESPainter *fPainter=nullptr;
public:
	GLenum Format;
	GLsizei Width=0;
	GLsizei Height=0;

	cOpenGLESMemoryPaintTarget();
	~cOpenGLESMemoryPaintTarget();

// iOpenGLESPaintTarget

	virtual void cnLib_FUNC SetPainter(iOpenGLESPainter *Painter)override;
	virtual cUIPoint cnLib_FUNC GetPaintSize(void)override;

};
cOpenGLESMemoryPaintTarget::cOpenGLESMemoryPaintTarget()
{
}

cOpenGLESMemoryPaintTarget::~cOpenGLESMemoryPaintTarget()
{
}

void cOpenGLESMemoryPaintTarget::SetPainter(iOpenGLESPainter *Painter)
{
	if(fPainter!=nullptr){
		fPainter->PaintStopped();
	}
	fPainter=Painter;
	if(fPainter!=nullptr){
		fPainter->PaintStarted();

		fPainter->PaintRectChanged();
	}
}

cUIPoint cnLib_FUNC cOpenGLESMemoryPaintTarget::GetPaintSize(void)
{
	return {Width,Height};
}

bool cnLib_FUNC cOpenGLESMemoryPaintTarget::GLSetupRenderBuffer(GLenum Target)
{
	if(Width==0 || Height==0)
		return false;
	glRenderbufferStorage(Target, Format, Width, Height);
	return true;
}*/

