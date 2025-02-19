#import <dispatch/dispatch.h>
#import <QuartzCore/QuartzCore.h>
//#include <GLKit/GLKit.h>

#include <cnSystem/cnUI.h>
#include <cnRTL/cnRTL.h>
#include <cnRTL/ocRTL.h>
#import "MacOS_UIApp.h"
#import "MacOS_UIView.h"

using namespace cnLibrary;
using namespace cnRTL;
using namespace cnMac;

#if	0
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
@interface cnLib_NSWindowDelegate()<NSWindowDelegate>
@end
typedef cnLib_NSWindowDelegate cnLib_UIAppDelegate;

@implementation cnLib_NSWindowDelegate


- (BOOL)windowShouldZoom:(NSWindow *)window toFrame:(NSRect)newFrame
{
	NSLog(@"windowShouldZoom");
	return TRUE;
}


- (void)windowWillMove:(NSNotification *)notification
{
	NSLog(@"windowWillMove");
}
- (void)windowDidMove:(NSNotification *)notification
{
	NSLog(@"windowDidMove");
}

- (void)windowDidResize:(NSNotification *)notification
{
	NSLog(@"windowDidResize");
}

- (void)windowWillClose:(NSNotification *)notification
{
	NSLog(@"windowWillClose");
}

- (void)windowDidMiniaturize:(NSNotification *)notification
{
	NSLog(@"windowDidMiniaturize");
}
- (void)windowWillMiniaturize:(NSNotification *)notification
{
	NSLog(@"windowWillMiniaturize");
}
- (void)windowDidDeminiaturize:(NSNotification *)notification
{
	NSLog(@"windowDidDeminiaturize");
}

#if	0
- (BOOL)windowShouldClose:(NSWindow *)sender;
- (nullable id)windowWillReturnFieldEditor:(NSWindow *)sender toObject:(nullable id)client;
- (NSSize)windowWillResize:(NSWindow *)sender toSize:(NSSize)frameSize;
- (NSRect)windowWillUseStandardFrame:(NSWindow *)window defaultFrame:(NSRect)newFrame;
- (nullable NSUndoManager *)windowWillReturnUndoManager:(NSWindow *)window;

/* Tells the delegate that the window is about to show a sheet, and gives the delegate a chance to customize the location of the sheet.
 */
- (NSRect)window:(NSWindow *)window willPositionSheet:(NSWindow *)sheet usingRect:(NSRect)rect;
/* If a window has a representedURL, the window will by default show a path popup menu for a command-click on a rectangle containing the window document icon button and the window title.  The window delegate may implement -window:shouldPopupDocumentPathMenu: to override NSWindow's default behavior for path popup menu.  A return of NO will prevent the menu from being shown.  A return of YES will cause the window to show the menu passed to this method, which by default will contain a menuItem for each path component of the representedURL.  If the representedURL has no path components, the menu will have no menu items.  Before returning YES, the window delegate may customize the menu by changing the menuItems.  menuItems may be added or deleted, and each menuItem title, action, or target may be modified.
*/
- (BOOL)window:(NSWindow *)window shouldPopUpDocumentPathMenu:(NSMenu *)menu NS_AVAILABLE_MAC(10_5);

/* The window delegate may implement -window:shouldDragDocumentWithEvent:from:withPasteboard: to override NSWindow document icon's default drag behavior.  The delegate can prohibit the drag by returning NO.  Before returning NO, the delegate may implement its own dragging behavior using -[NSWindow dragImage:at:offset:event:pasteboard:source:slideBack:].  Alternatively, the delegate can enable a drag by returning YES, for example to override NSWindow's default behavior of prohibiting the drag of an edited document.  Lastly, the delegate can customize the pasteboard contents before returning YES.
*/
- (BOOL)window:(NSWindow *)window shouldDragDocumentWithEvent:(NSEvent *)event from:(NSPoint)dragImageLocation withPasteboard:(NSPasteboard *)pasteboard NS_AVAILABLE_MAC(10_5);

- (NSSize)window:(NSWindow *)window willUseFullScreenContentSize:(NSSize)proposedSize NS_AVAILABLE_MAC(10_7);

- (NSApplicationPresentationOptions)window:(NSWindow *)window willUseFullScreenPresentationOptions:(NSApplicationPresentationOptions)proposedOptions NS_AVAILABLE_MAC(10_7);

/* The default animation between a window and its fullscreen representation is a crossfade.  With knowledge of the layout of a window before and after it enters fullscreen, an application can do a much better job on the animation.  The following API allows a window delegate to customize the animation by providing a custom window or windows containing layers or other effects.  In order to manage windows on spaces, we need the window delegate to provide a list of windows involved in the animation.  If an application does not do a custom animation, this method can be unimplemented or can return nil.  window:startCustomAnimationToEnterFullScreenWithDuration: will be called only if customWindowsToEnterFullScreenForWindow: returns non-nil.
 */
- (nullable NSArray<NSWindow *> *)customWindowsToEnterFullScreenForWindow:(NSWindow *)window NS_AVAILABLE_MAC(10_7);

/* The system has started its animation into fullscreen, including transitioning to a new space.  Start the window fullscreen animation immediately, and perform the animation with the given duration to  be in sync with the system animation.  This method is called only if customWindowToEnterFullScreenForWindow: returned non-nil.
*/
- (void)window:(NSWindow *)window startCustomAnimationToEnterFullScreenWithDuration:(NSTimeInterval)duration NS_AVAILABLE_MAC(10_7);

/* In some cases, the transition to enter fullscreen will fail, due to being in the midst of handling some other animation or user gesture.  We will attempt to minimize these cases, but believe there is a need for failure handling.  This method indicates that there was an error, and the application should clean up any work it may have done to prepare to enter fullscreen.  This message will be sent whether or not the delegate indicated a custom animation by returning non-nil from  customWindowsToEnterFullScreenForWindow:.
*/
- (void)windowDidFailToEnterFullScreen:(NSWindow *)window NS_AVAILABLE_MAC(10_7);

/* The window is about to exit fullscreen mode.  The following API allows a window delegate to customize the animation when the window is about to exit fullscreen.  In order to manage windows on spaces, we need the window delegate to provide a list of windows involved in the animation.  If an application does not do a custom animation, this method can be unimplemented or can return nil.  window:startCustomAnimationToExitFullScreenWithDuration: will be called only if customWindowsToExitFullScreenForWindow: returns non-nil.
*/
- (nullable NSArray<NSWindow *> *)customWindowsToExitFullScreenForWindow:(NSWindow *)window NS_AVAILABLE_MAC(10_7);

/* The system has started its animation out of fullscreen, including transitioning back to the desktop space.  Start the window animation immediately, and perform the animation with the given duration to  be in sync with the system animation.  This method is called only if customWindowsToExitFullScreenForWindow: returned non-nil.
*/
- (void)window:(NSWindow *)window startCustomAnimationToExitFullScreenWithDuration:(NSTimeInterval)duration NS_AVAILABLE_MAC(10_7);

/* customWindowsToEnterFullScreenForWindow:onScreen: will be called in place of customWindowsToEnterFullScreenForWindow: if both are implemented */
- (nullable NSArray<NSWindow *> *)customWindowsToEnterFullScreenForWindow:(NSWindow *)window onScreen:(NSScreen *)screen NS_AVAILABLE_MAC(10_9);

/* window:startCustomAnimationToEnterFullScreenOnScreen:withDuration: will be called in place of window:startCustomAnimationToEnterFullScreenWithDuration: if both are implemented */
- (void)window:(NSWindow *)window startCustomAnimationToEnterFullScreenOnScreen:(NSScreen *)screen withDuration:(NSTimeInterval)duration NS_AVAILABLE_MAC(10_9);

/* In some cases, the transition to exit fullscreen will fail, due to being in the midst of handling some other animation or user gesture.  We will attempt to minimize these cases, but believe there is a need for failure handling.  This method indicates that there was an error, and the application should clean up any work it may have done to prepare to exit fullscreen.  This message will be sent whether or not the delegate indicated a custom animation by returning non-nil from  customWindowsToExitFullScreenForWindow:.
*/
- (void)windowDidFailToExitFullScreen:(NSWindow *)window NS_AVAILABLE_MAC(10_7);

/* Windows entering the version browser will be resized to the size returned by this method. If either dimension of the returned size is larger than the maxPreferredFrameSize, the window will also be scaled down to ensure it fits properly in the version browser. Returned sizes larger than maxAllowedSize will be constrained to that size. If this method is not implemented, the version browser will use -window:willUseStandardFrame: to determine the resulting window frame size.
*/
- (NSSize)window:(NSWindow *)window willResizeForVersionBrowserWithMaxPreferredSize:(NSSize)maxPreferredFrameSize maxAllowedSize:(NSSize)maxAllowedFrameSize NS_AVAILABLE_MAC(10_7);

/* Method called by -[NSWindow encodeRestorableStateWithCoder:] to give the delegate a chance to encode any additional state into the NSCoder.  This state is available in the NSCoder passed to restoreWindowWithIdentifier:state:handler: . See the header NSWindowRestoration.h for more information.
*/
- (void)window:(NSWindow *)window willEncodeRestorableState:(NSCoder *)state NS_AVAILABLE_MAC(10_7);

/* Method called by -[NSWindow restoreStateWithCoder:] to give the delegate a chance to restore its own state, which it may decode from the NSCoder. See the header NSWindowRestoration.h for more information.
*/
- (void)window:(NSWindow *)window didDecodeRestorableState:(NSCoder *)state NS_AVAILABLE_MAC(10_7);

/* Notifications
*/
- (void)windowDidExpose:(NSNotification *)notification;
- (void)windowDidBecomeKey:(NSNotification *)notification;
- (void)windowDidResignKey:(NSNotification *)notification;
- (void)windowDidBecomeMain:(NSNotification *)notification;
- (void)windowDidResignMain:(NSNotification *)notification;
- (void)windowDidUpdate:(NSNotification *)notification;
- (void)windowDidChangeScreen:(NSNotification *)notification;
- (void)windowDidChangeScreenProfile:(NSNotification *)notification;
- (void)windowDidChangeBackingProperties:(NSNotification *)notification NS_AVAILABLE_MAC(10_7); // added in 10.7.3
- (void)windowWillBeginSheet:(NSNotification *)notification;
- (void)windowDidEndSheet:(NSNotification *)notification;
- (void)windowWillStartLiveResize:(NSNotification *)notification NS_AVAILABLE_MAC(10_6);
- (void)windowDidEndLiveResize:(NSNotification *)notification NS_AVAILABLE_MAC(10_6);
- (void)windowWillEnterFullScreen:(NSNotification *)notification NS_AVAILABLE_MAC(10_7);
- (void)windowDidEnterFullScreen:(NSNotification *)notification NS_AVAILABLE_MAC(10_7);
- (void)windowWillExitFullScreen:(NSNotification *)notification NS_AVAILABLE_MAC(10_7);
- (void)windowDidExitFullScreen:(NSNotification *)notification NS_AVAILABLE_MAC(10_7);
- (void)windowWillEnterVersionBrowser:(NSNotification *)notification NS_AVAILABLE_MAC(10_7);
- (void)windowDidEnterVersionBrowser:(NSNotification *)notification NS_AVAILABLE_MAC(10_7);
- (void)windowWillExitVersionBrowser:(NSNotification *)notification NS_AVAILABLE_MAC(10_7);
- (void)windowDidExitVersionBrowser:(NSNotification *)notification NS_AVAILABLE_MAC(10_7);
- (void)windowDidChangeOcclusionState:(NSNotification *)notification NS_AVAILABLE_MAC(10_9);
#endif
@end
#endif
//---------------------------------------------------------------------------
#pragma mark Window
//---------------------------------------------------------------------------
cUIWindow::cUIWindow(NSWindow *Window)
	: fWindow(Window)
{
	[fWindow retain];
}
//---------------------------------------------------------------------------
cUIWindow::~cUIWindow()
{
	[fWindow release];
}
//---------------------------------------------------------------------------
bool cUIWindow::CheckUIThread(void)
{
	return gMainUIThread.IsCurrentThread();
}
//---------------------------------------------------------------------------
iUIThread* cUIWindow::GetUIThread(void)
{
	return &gMainUIThread;
}
//---------------------------------------------------------------------------
iUIWindow* cUIWindow::GetParent(void)
{
	return nullptr;
}
//---------------------------------------------------------------------------
iUIArea* cUIWindow::GetClient(void)
{
	id ocview=fWindow.contentView;
	auto ClientView=ocSafeCast<cnLib_NSView>(ocview);
	if(ClientView==nullptr)
		return nullptr;
	return cnLib_NSView_GetUIView(ClientView);
}
//---------------------------------------------------------------------------
bool cUIWindow::SetClient(iUIArea *View)
{
	auto ClientView=ociSafeCast<NSView>(View);
	if(ClientView==nullptr)
		return false;
	fWindow.contentView=ClientView;
	return true;
}
//---------------------------------------------------------------------------
uIntn cUIWindow::FindRelativeBase(cUIPoint &Offset,iUIArea*const *Relatives,uIntn Count)
{
	Offset={0,0};
	if(CheckUIThread()==false)
		return uintMax;

	for(uIntn i=0;i<Count;i++){
		auto *Layout=Relatives[i];
		if(Layout==this){
			return i;
		}
	}
/*	auto rootvc=fWindow.rootViewController;
	if(rootvc==nil)
		return uintMax;
	for(uintn i=0;i<Count;i++){
		auto OCView=iCast<iOCUIView>(Layouts[i]);
		if(OCView!=nullptr){
			CGPoint p={0,0};
			p=[rootvc.view convertPoint:p toView:OCView->GetUIView()];
			auto scale=rootvc.view.layer.contentScale;
			Offset.x=p.x*scale;
			Offset.y=p.y*scale;
			return i;
		}
	}
*/
	return uintMax;
}
//---------------------------------------------------------------------------
bool cUIWindow::GetPosition(iUIArea *Relative,cUIPoint &Position)
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
/*
	auto rootvc=fWindow.rootViewController;
	if(rootvc==nil)
		return false;

	auto OCView=iCast<iOCUIView>(Relative);
	if(OCView!=nullptr){
		CGPoint p={0,0};
		p=[rootvc.view convertPoint:p toView:OCView->GetUIView()];
		auto scale=rootvc.view.layer.contentScale;
		Position.x=p.x*scale;
		Position.y=p.y*scale;
		return true;
	}
	*/
	return false;
}
//---------------------------------------------------------------------------
bool cUIWindow::SetPosition(iUIArea *Relative,cUIPoint Position)
{
	return false;
}
//---------------------------------------------------------------------------
cUIPoint cUIWindow::GetSize(void)
{
	if(CheckUIThread()==false)
		return {0,0};

	auto FrameSize=fWindow.frame.size;
	cUIPoint Size;
	Size.x=FrameSize.width;
	Size.y=FrameSize.height;
	return Size;
}
//---------------------------------------------------------------------------
bool cUIWindow::SetSize(cUIPoint Size)
{
	return false;
}
//---------------------------------------------------------------------------
bool cUIWindow::SetRectangle(iUIArea *Relative,cUIPoint Position,cUIPoint Size)
{
	return false;
}
//---------------------------------------------------------------------------
sfInt16 cUIWindow::GetZIndex(void)
{
	return 0;
}
//---------------------------------------------------------------------------
bool cUIWindow::SetZIndex(sfInt16 Index)
{
	return false;
}
//---------------------------------------------------------------------------
Float32 cUIWindow::GetContentScale(void)
{
	return fWindow.backingScaleFactor;
}
//---------------------------------------------------------------------------
/*
void cUIWindow::UpdateUIState(void)
{
	auto PrevUIState=fUIState;

	auto WindowVisible=!fWindow.isVisible;
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
		if(WindowVisible==false){
			fUIState=UIState::Background;
		}
		else{
			fUIState=UIState::Inactive;
		}
		break;
	case UIState::Active:
		if(WindowVisible==false){
			fUIState=UIState::Background;
		}
		else{
			fUIState=UIState::Active;
		}
		break;
	}

	cnRTL::UIStateNotifyChange(PrevUIState,fUIState,fLayoutHandlers.GetList(),fLayoutHandlers.GetCount());
}
*/

@implementation cnLib_MainUIThread

+(void)didFinishLaunching
{
	gMainUIThread.DidFinishLaunching();
}
+(void)willResignActive
{
	gMainUIThread.WillResignActive();
}
+(void)didHide
{
	gMainUIThread.DidHide();
}
+(void)willUnhide
{
	gMainUIThread.WillUnhide();
}
+(void)didBecomeActive
{
	gMainUIThread.DidBecomeActive();
}
+(void)willTerminate
{
	gMainUIThread.WillTerminate();
}

@end

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cMacOSMainUIThread::cMacOSMainUIThread()
{
	fApp=[NSApplication sharedApplication];
	fAppState=UIState::Null;
	auto nc=[NSNotificationCenter defaultCenter];
	auto ao=[cnLib_MainUIThread class];
	[nc addObserver:ao selector:@selector(didFinishLaunching)	name:NSApplicationDidFinishLaunchingNotification object:nil];
	[nc addObserver:ao selector:@selector(willUnhide)			name:NSApplicationWillUnhideNotification object:nil];
	[nc addObserver:ao selector:@selector(didBecomeActive)		name:NSApplicationDidBecomeActiveNotification object:nil];
	[nc addObserver:ao selector:@selector(willResignActive)		name:NSApplicationWillResignActiveNotification object:nil];
	[nc addObserver:ao selector:@selector(didHide)				name:NSApplicationDidHideNotification object:nil];
	[nc addObserver:ao selector:@selector(willTerminate)		name:NSApplicationWillTerminateNotification object:nil];
}
//---------------------------------------------------------------------------
cMacOSMainUIThread::~cMacOSMainUIThread()
{
	[[NSNotificationCenter defaultCenter] removeObserver:[cnLib_MainUIThread class]];
}
//---------------------------------------------------------------------------
UIState cMacOSMainUIThread::GetAppState(void)const
{
	return fAppState;
}
//---------------------------------------------------------------------------
void cMacOSMainUIThread::DidFinishLaunching(void)
{
	AppDidLaunched();
	AppWillUnhide();
}
//---------------------------------------------------------------------------
void cMacOSMainUIThread::WillUnhide(void)
{
	bool AppActive=fApp.active;
	switch(fAppState){
	case UIState::Null:
		break;
	case UIState::Background:
		AppWillUnhide();
		// fall through
	case UIState::Inactive:
		if(AppActive){
			AppDidBecomeActive();
		}
		break;
	case UIState::Active:
		break;
	}
}
void cMacOSMainUIThread::DidBecomeActive(void)
{
	if(fApp.isHidden==false){
		switch(fAppState){
		case UIState::Null:
			break;
		case UIState::Background:
			AppWillUnhide();
			// fall through
		case UIState::Inactive:
			AppDidBecomeActive();
			break;
		case UIState::Active:
			break;
		}
	}
}
void cMacOSMainUIThread::WillResignActive(void)
{
	if(fApp.isHidden==false){
		if(fAppState==UIState::Active){
			AppWillResignActive();
		}
	}
}
void cMacOSMainUIThread::DidHide(void)
{
	switch(fAppState){
	default:
		break;
	case UIState::Active:
		AppWillResignActive();
		cnLib_SWITCH_FALLTHROUGH;
	case UIState::Inactive:
		AppDidHide();
		break;
	}
}
void cMacOSMainUIThread::WillTerminate(void)
{
	switch(fAppState){
	default:
		break;
	case UIState::Active:
		AppWillResignActive();
		// fall through
	case UIState::Inactive:
		AppDidHide();
		break;
	}
	AppWillTerminate();
}
//---------------------------------------------------------------------------
void cMacOSMainUIThread::AppDidLaunched(void)
{
	//fAppState=UIState::Background;
	OnUIAppStateChanged();
}
void cMacOSMainUIThread::AppWillUnhide(void)
{
	fAppState=UIState::Inactive;
	OnUIAppStateChanged();
}
void cMacOSMainUIThread::AppDidBecomeActive(void)
{
	fAppState=UIState::Active;
	OnUIAppStateChanged();
}
void cMacOSMainUIThread::AppWillResignActive(void)
{
	fAppState=UIState::Inactive;
	OnUIAppStateChanged();
}
void cMacOSMainUIThread::AppDidHide(void)
{
	fAppState=UIState::Background;
	OnUIAppStateChanged();
}
void cMacOSMainUIThread::AppWillTerminate(void)
{
	fAppState=UIState::Null;
	OnUIAppStateChanged();
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cMacOSUIApplication::cMacOSUIApplication()
{
	fStateHandler=nullptr;
}
//---------------------------------------------------------------------------
cMacOSUIApplication::~cMacOSUIApplication()
{
}
//---------------------------------------------------------------------------
iUIThread* cMacOSUIApplication::GetMainUIThread(void)
{
	return &gMainUIThread;
}
//---------------------------------------------------------------------------
void cMacOSUIApplication::SetAppHandler(iUIStateHandler *Handler)
{
	fStateHandler=Handler;
}
//---------------------------------------------------------------------------
eUIState cMacOSUIApplication::GetAppState(void)
{
	return gMainUIThread.GetAppState();
}
//---------------------------------------------------------------------------
void cMacOSUIApplication::DidFinishLaunchingWithOptions(void)
{
	if(fStateHandler!=nullptr){
		fStateHandler->UIStarted();
	}
	if(fStateHandler!=nullptr){
		fStateHandler->UIShow();
	}
}
//---------------------------------------------------------------------------
void cMacOSUIApplication::WillResignActive(void)
{
//	gMainUIThread.WillResignActive();

	if(fStateHandler!=nullptr){
		fStateHandler->UIPaused();
	}
}
//---------------------------------------------------------------------------
void cMacOSUIApplication::DidHide(void)
{
//	gMainUIThread.DidEnterBackground();

	if(fStateHandler!=nullptr){
		fStateHandler->UIHide();
	}
}
//---------------------------------------------------------------------------
void cMacOSUIApplication::WillUnhide(void)
{
//	gMainUIThread.WillUnhide();

	if(fStateHandler!=nullptr){
		fStateHandler->UIShow();
	}
}
//---------------------------------------------------------------------------
void cMacOSUIApplication::DidBecomeActive(void)
{
//	gMainUIThread.DidBecomeActive();

	if(fStateHandler!=nullptr){
		fStateHandler->UIResume();
	}
}
//---------------------------------------------------------------------------
void cMacOSUIApplication::WillTerminate(void)
{
//	gMainUIThread.WillTerminate();

	if(fStateHandler!=nullptr){
		fStateHandler->UIStopped();
	}
}
//---------------------------------------------------------------------------
