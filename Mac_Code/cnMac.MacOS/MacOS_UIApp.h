/*--- Mac - UIApp ---------------------------------------------------------*/
/*         Developer : Code009                                             */
/*         Create on : 2012-09-09                                          */
/*-------------------------------------------------------------------------*/
#pragma once
/*-------------------------------------------------------------------------*/
#include <cnSystem/cnUI.h>

#ifdef	__OBJC__
#include <cnSystem/MacOS.h>
#include <cnRTL/ocRTL.h>
#include <cnMac/Mac_UIThread.h>

@interface cnLib_MainUIThread : NSObject
@end

//---------------------------------------------------------------------------
#ifdef	__cplusplus
//---------------------------------------------------------------------------
namespace cnLibrary{
namespace cnMac{
//---------------------------------------------------------------------------
class cUIWindow : public iUIWindow
{
public:
	cUIWindow(NSWindow *Window);
	~cUIWindow();

// iUIArea methods

	virtual iUIThread* cnLib_FUNC GetUIThread(void)override;
	virtual uIntn cnLib_FUNC FindRelativeBase(cUIPoint &Offset,iUIArea*const *Relatives,uIntn Count)override;
	virtual bool cnLib_FUNC GetPosition(iUIArea *Relative,cUIPoint &Position)override;
	virtual bool cnLib_FUNC SetPosition(iUIArea *Relative,cUIPoint Position)override;
	virtual cUIPoint cnLib_FUNC GetSize(void)override;
	virtual bool cnLib_FUNC SetSize(cUIPoint Size)override;
	virtual bool cnLib_FUNC SetRectangle(iUIArea *Relative,cUIPoint Position,cUIPoint Size)override;
	virtual sfInt16 cnLib_FUNC GetZIndex(void)override;
	virtual bool cnLib_FUNC SetZIndex(sfInt16 Index)override;
	virtual Float32 cnLib_FUNC GetContentScale(void)override;

// iUIWindow methods

	virtual iUIWindow* cnLib_FUNC GetParent(void)override;
	virtual iUIArea* cnLib_FUNC GetClient(void)override;
	virtual bool cnLib_FUNC SetClient(iUIArea *Area)override;

protected:
	NSWindow *fWindow;

	bool CheckUIThread(void);

	cnRTL::cSeqSet<iUILayoutHandler*> fLayoutHandlers;
};
//---------------------------------------------------------------------------
class cMacOSMainUIThread : public cUIThread
{
public:
	cMacOSMainUIThread();
	~cMacOSMainUIThread();

	UIState GetAppState(void)const;

	void DidFinishLaunching(void);
	void WillResignActive(void);
	void DidHide(void);
	void WillUnhide(void);
	void DidBecomeActive(void);
	void WillTerminate(void);

	cnRTL::cFunctionSet<void (void)> OnUIAppStateChanged;
private:
	void AppDidLaunched(void);
	void AppWillUnhide(void);
	void AppDidBecomeActive(void);
	void AppWillResignActive(void);
	void AppDidHide(void);
	void AppWillTerminate(void);

	eUIState fAppState;
	NSApplication *fApp;
};
extern cMacOSMainUIThread gMainUIThread;
//---------------------------------------------------------------------------
class cMacOSUIApplication : public iMacOSUIApplication
{
public:
	cMacOSUIApplication();
	~cMacOSUIApplication();

	virtual iUIThread* GetMainUIThread(void)override;
	virtual void SetAppHandler(iUIStateHandler *Handler)override;
	virtual eUIState GetAppState(void)override;

	virtual void DidFinishLaunchingWithOptions(void)override;
	virtual void WillResignActive(void)override;
	virtual void DidHide(void)override;
	virtual void WillUnhide(void)override;
	virtual void DidBecomeActive(void)override;
	virtual void WillTerminate(void)override;

	cnRTL::cFunctionSet<void (void)> OnUIAppStateChanged;
private:
	iUIStateHandler *fStateHandler;
};
//---------------------------------------------------------------------------
}	// namespace cnMac
}	// namespace cnLibrary
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
#endif	// __cplusplus
//---------------------------------------------------------------------------
#endif	// __OBJC__
/*-------------------------------------------------------------------------*/
