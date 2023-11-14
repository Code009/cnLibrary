/*--- MacOS Interface - UI ------------------------------------------------*/
/*         Developer : Code009                                             */
/*         Create on : 2016-06-27                                          */
/*-------------------------------------------------------------------------*/
#pragma once
//---------------------------------------------------------------------------
//#include <CoreGraphics/CoreGraphics.h>
//---------------------------------------------------------------------------
#ifdef	__OBJC__
#import <Cocoa/Cocoa.h>

//---------------------------------------------------------------------------
@interface cnLib_NSView : NSView
@end
//---------------------------------------------------------------------------
#endif	// __OBJC__
//---------------------------------------------------------------------------
#ifdef	__cplusplus
//---------------------------------------------------------------------------
#include <cnSystem/cnSystem.h>
#include <cnSystem/OpenGL.h>
#include <cnSystem/cnMac_UI.h>
//---------------------------------------------------------------------------
namespace cnLibrary{
//---------------------------------------------------------------------------
class cnLib_INTERFACE iMacOSUIApplication : public iReference
{
public:
	virtual iUIThread* GetMainUIThread(void)=0;
	virtual void SetAppHandler(iUIStateHandler *Handler)=0;
	virtual eUIState GetAppState(void)=0;

	virtual void DidFinishLaunchingWithOptions(void)=0;
	virtual void WillResignActive(void)=0;
	virtual void DidHide(void)=0;
	virtual void WillUnhide(void)=0;
	virtual void DidBecomeActive(void)=0;
	virtual void WillTerminate(void)=0;
};
//---------------------------------------------------------------------------
namespace cnMacOS{
//---------------------------------------------------------------------------
//iPtr<iQuartzViewContent> CreateQuartzViewContent(void);
//---------------------------------------------------------------------------
}	// cnMacOS
//---------------------------------------------------------------------------
//	Objective C
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
namespace cnMacOS{
//---------------------------------------------------------------------------
iPtr<iUIWindow> CreateUIWindow(void);
//---------------------------------------------------------------------------
#ifdef	__OBJC__
//---------------------------------------------------------------------------
iPtr<iUIWindow> CreateUIWindow(NSWindow *Window);
iUIView* iUIViewFromView(cnLib_NSView *View);
//cnLib_UIView* GetWindowClientUIView(cnLib_UIViewController *ViewController);
//---------------------------------------------------------------------------
#endif	// __OBJC__
//---------------------------------------------------------------------------
iPtr<iOpenGLContext> CreateOpenGLContext(CGLPixelFormatObj PixelFormatObject,iOpenGLContext *SharedContext);
rPtr<iOpenGLViewContent> CreateOpenGLViewContent(iOpenGLContext *SharedContext);
rPtr<iMacOSUIApplication> CreateUIApplication(void);
//---------------------------------------------------------------------------
}	// cnMacOS
//---------------------------------------------------------------------------
}	// namespace cnLibrary
//---------------------------------------------------------------------------
#endif	// __cplusplus
