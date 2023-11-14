/*--- Mac - UIPaintTarget -------------------------------------------------*/
/*         Developer : Code009                                             */
/*         Create on : 2012-05-25                                          */
/*-------------------------------------------------------------------------*/
#pragma once

/*-------------------------------------------------------------------------*/

#ifdef	__OBJC__
#import <UIKit/UIKit.h>
#import <QuartzCore/QuartzCore.h>
#import <CoreText/CoreText.h>
//---------------------------------------------------------------------------
#include <cnSystem/iOS.h>
#include <cnSystem/OpenGL.h>
#include <cnMac/Mac_QuartzCore.h>
#include <cnRTL/ocRTL.h>

//---------------------------------------------------------------------------
#ifdef	__cplusplus
//---------------------------------------------------------------------------
namespace cnLibrary{
namespace cnMac{
//---------------------------------------------------------------------------
#pragma mark OpenGLES Context
//---------------------------------------------------------------------------
class cOpenGLESContext : public iOpenGLContext, public iOCObject
{
public:
	cOpenGLESContext(ocPtr<EAGLContext> Context);
	~cOpenGLESContext();

	using iOCObject::CastInterface;

	// iOCObject

	virtual id GetOCObject(void)override;

	// iOpenGLContext

	virtual void Enter(void)override;
	virtual void Leave(void)override;

protected:
	EAGLContext *fContext;
};
//---------------------------------------------------------------------------
#pragma mark OpenGLES - Render Buffer
//---------------------------------------------------------------------------
struct cOpenGLESRenderBuffer
{
	GLuint FrameBuffer;
	GLuint RenderBuffer[2];

	void GLSetup(void);
	void GLClear(void);

	void GLBind(void);
	void GLSetupBuffer(GLenum RenderFormat,sfInt32 Width,sfInt32 Height);
	bool GLSetupDrawableBuffer(EAGLContext *Context,id<EAGLDrawable> Drawable);
	void GLSetupWithRenderBuffer(void);

};
//---------------------------------------------------------------------------
#pragma mark OpenGLES - UIViewCALayer
//---------------------------------------------------------------------------
class cOpenGLESViewLayer : public iOpenGLViewContent, public iUIViewCALayer
{
public:
	cOpenGLESViewLayer();
	~cOpenGLESViewLayer();

	virtual const void* cnLib_FUNC CastInterface(iTypeID IID)const override;

	CALayer* GetLayer(void);

	iOpenGLContext* GetGLESContext(void);
	void SetGLESContext(iOpenGLContext *OpenGLContext);

// iOCObject methods

	virtual id cnLib_FUNC GetOCObject(void)override;
	
// iUIViewCALayer

	virtual void SetLayerFrame(CGRect Frame)override;
	virtual void SetLayerScale(CGFloat Scale)override;
	virtual void OnViewStateChanged(void)override;

// iUIViewContent

	virtual iUIView* cnLib_FUNC GetView(void)override;
	virtual bool cnLib_FUNC SetView(iUIView *View)override;
	virtual bool cnLib_FUNC GetVisible(void)override;
	virtual bool cnLib_FUNC SetVisible(bool Visible)override;
	virtual sfInt16 cnLib_FUNC GetZIndex(void)override;
	virtual bool cnLib_FUNC SetZIndex(sfInt16 ZIndex)override;

// iOpenGLViewContent

	virtual void cnLib_FUNC SetPainter(iOpenGLViewPainter *Painter)override;
	virtual cUIPoint cnLib_FUNC GetPaintSize(void)override;
	virtual iOpenGLContext* cnLib_FUNC GetOpenGLContext(void)override;
	virtual void cnLib_FUNC SetRender(void)override;

protected:
	CAEAGLLayer *fLayer;
	iPtr<iUIView> fHostView=nullptr;

	iOpenGLViewPainter *fPainter;

	iPtr<iOpenGLContext> fGLESContext;
	EAGLContext *fContext;
	cOpenGLESRenderBuffer fRenderBuffer;
private:

	UIState fPainterState;
	void UpdatePainterState(void);

	
	void UIStarted(void);
	void UIShow(void);
	void UIResume(void);
	void UIPaused(void);
	void UIHide(void);
	void UIStopped(void);

	struct cStateNotifier
	{
		cOpenGLESViewLayer *This;
		void UIStarted(void);
		void UIShow(void);
		void UIResume(void);
		void UIPaused(void);
		void UIHide(void);
		void UIStopped(void);
	};

	void NotifyPainterState(UIState PrevState,UIState NextState);

	bool fNeedNotifyRectChange=false;
	bool fNeedDraw=false;
	bool fNeedSetupDrawableBuffer=false;

	void NotifyLayout(void);

	bool IsRenderActive(void);
	void GLSetup(void);
	void GLClear(void);
	void QueryRender(void);
	void Render(void);
	void RenderProcess(void);

	static void DispatchFunc_Render(void *_Nullable);
};
//---------------------------------------------------------------------------
EAGLSharegroup* GetEAGLSharegroup(iOpenGLContext *Context);
rPtr<iOpenGLViewContent> CreateOpenGLViewContent(iOpenGLContext *SharedContext);
//---------------------------------------------------------------------------
}	// namespace cnMac
}	// namespace cnLibrary
//---------------------------------------------------------------------------
#endif	// __cplusplus
//---------------------------------------------------------------------------
#endif	// __OBJC__
/*-------------------------------------------------------------------------*/
