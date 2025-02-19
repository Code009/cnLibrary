/*--- iOS Interface - UI --------------------------------------------------*/
/*         Developer : Code009                                             */
/*         Create on : 2015-01-27                                          */
/*-------------------------------------------------------------------------*/
#pragma once

#include <CoreGraphics/CoreGraphics.h>

#ifdef	__OBJC__
#import <UIKit/UIKit.h>
#import <OpenGLES/EAGL.h>

//---------------------------------------------------------------------------
@interface cnLib_UIView : UIView
@end
//---------------------------------------------------------------------------
@interface cnLib_UIViewController : UIViewController
@end
//---------------------------------------------------------------------------
#endif	// __OBJC__
//---------------------------------------------------------------------------
#ifdef	__cplusplus
//---------------------------------------------------------------------------
#include <cnSystem/OpenGL.h>
//---------------------------------------------------------------------------
namespace cnLibrary{
//---------------------------------------------------------------------------
class cnLib_INTERFACE iiOSUIApplication : public iReference
{
public:
	virtual iUIThread* GetMainUIThread(void)=0;
	virtual void SetAppHandler(iUIStateHandler *Handler)=0;
	virtual eUIState GetAppState(void)=0;

	virtual void DidFinishLaunchingWithOptions(void)=0;
	virtual void WillResignActive(void)=0;
	virtual void DidEnterBackground(void)=0;
	virtual void WillEnterForeground(void)=0;
	virtual void DidBecomeActive(void)=0;
	virtual void WillTerminate(void)=0;
};
//---------------------------------------------------------------------------
namespace cniOS{
//---------------------------------------------------------------------------
rPtr<iOpenGLViewContent> CreateOpenGLESViewContent(iOpenGLContext *Context);
//---------------------------------------------------------------------------
#ifdef	__OBJC__
//---------------------------------------------------------------------------
iPtr<iOpenGLContext> CreateOpenGLESContext(EAGLRenderingAPI API,iOpenGLContext *SharedContext);
//---------------------------------------------------------------------------
iUIView* iUIViewFromView(cnLib_UIView *View);
iUIWindow* iUIWindowFromViewController(cnLib_UIViewController *ViewController);
cnLib_UIView* GetWindowClientUIView(cnLib_UIViewController *ViewController);
rPtr<iiOSUIApplication> CreateUIApplication(void);
//---------------------------------------------------------------------------
#endif	// __OBJC__
//---------------------------------------------------------------------------
}	// cniOS
//---------------------------------------------------------------------------
}	// namespace cnLibrary
//---------------------------------------------------------------------------
#endif	// __cplusplus
