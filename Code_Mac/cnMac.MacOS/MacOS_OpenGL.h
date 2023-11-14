/*--- Mac - OpenGL --------------------------------------------------------*/
/*         Developer : Code009                                             */
/*         Create on : 2020-10-19                                          */
/*-------------------------------------------------------------------------*/
#pragma once

/*-------------------------------------------------------------------------*/
#include <cnSystem/cnUI.h>

#ifdef	__OBJC__
#include <cnSystem/MacOS.h>
#include <cnSystem/OpenGL.h>
#include <cnRTL/ocRTL.h>
#include <cnMac/Mac_UIThread.h>
#include <cnMac/Mac_QuartzCore.h>

@interface cnLib_OpenGLLayer : CAOpenGLLayer

@end

//---------------------------------------------------------------------------
#ifdef	__cplusplus
//---------------------------------------------------------------------------
namespace cnLibrary{
namespace cnMac{
//---------------------------------------------------------------------------
#pragma mark OpenGL Context
//---------------------------------------------------------------------------
class cOpenGLContext : public iOpenGLContext
{
public:
	cnLib_INTERFACE_DEFINE(cOpenGLContext,iOpenGLContext);

	cOpenGLContext(CGLContextObj Context);
	~cOpenGLContext();

	CGLContextObj GetCGLContext(void);

	// iOpenGLContext

	virtual void Enter(void)override;
	virtual void Leave(void)override;
protected:
	CGLContextObj fContext;
};
//---------------------------------------------------------------------------
#pragma mark OpenGL - UIViewCALayer
//---------------------------------------------------------------------------
class cOpenGLViewLayer : public iOpenGLViewContent, public iUIViewCALayer
{
public:
	cOpenGLViewLayer(cnLib_OpenGLLayer *Layer);
	~cOpenGLViewLayer();

	iPtr<cOpenGLContext> SharedContext;

	virtual const void* cnLib_FUNC CastInterface(iTypeID IID)const override;
	virtual void cnLib_FUNC IncReference(void)override;
	virtual void cnLib_FUNC DecReference(void)override;

	CALayer* GetLayer(void);

	CGLPixelFormatObj LayerAcquirePixelFormat(uint32_t mask);
	void LayerReleasePixelFormat(CGLPixelFormatObj pf);
	CGLContextObj LayerAcquireContext(CGLPixelFormatObj pf);
	void LayerReleaseContext(CGLContextObj ctx);
	void LayerGLOnDraw(void);

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
	CAOpenGLLayer *fLayer;
	iPtr<iUIView> fHostView=nullptr;

	iPtr<cOpenGLContext> fContext;

	iOpenGLViewPainter *fPainter;

private:

	eUIState fPainterState;
	void UpdatePainterState(void);
	struct cPainterNotifier{
		cOpenGLViewLayer *This;
		void UIStarted(void);
		void UIShow(void);
		void UIResume(void);
		void UIPaused(void);
		void UIHide(void);
		void UIStopped(void);
	};
	void NotifyPainterState(eUIState PrevState,eUIState NextState);

	bool fNeedNotifyRectChange=false;

	void NotifyLayout(void);
};
//---------------------------------------------------------------------------
iPtr<iOpenGLContext> CreateOpenGLContext(CGLPixelFormatObj PixelFormatObject,iOpenGLContext *SharedContext);
cOpenGLViewLayer* ViewContentFromOpenGLViewLayer(cnLib_OpenGLLayer *Layer);
//---------------------------------------------------------------------------
}	// namespace cnMac
}	// namespace cnLibrary
//---------------------------------------------------------------------------
#endif	// __cplusplus
//---------------------------------------------------------------------------
#endif	// __OBJC__
/*-------------------------------------------------------------------------*/
