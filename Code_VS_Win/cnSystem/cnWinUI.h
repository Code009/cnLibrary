/*- Windows - User Interface ----------------------------------------------*/
/*         Developer : Code009                                             */
/*         Create on : 2015-01-25                                          */
/*-------------------------------------------------------------------------*/
#pragma once
/*-------------------------------------------------------------------------*/
#pragma warning(push)
#pragma warning(disable:4458)	// declaration of '' hides class member
#pragma warning(disable:4459)	// declaration of '' hides global declaration
//#include <dxgi.h>
//#include <d3d9.h>
#pragma warning(pop)
#include <wincodec.h>
/*-------------------------------------------------------------------------*/
#include <cnSystem\cnThread.h>
#include <cnSystem\cnUI.h>
#include <cnSystem\cnUIGraph.h>
#include <cnSystem\cnConsole.h>

#ifdef __cplusplus
//---------------------------------------------------------------------------
#ifdef	cnLibrary_CPPEXCLUDE_VIRTUAL_OVERRIDE
#define	override
#endif
//---------------------------------------------------------------------------
#if cnLibrary_CPPFEATURE_CONSTEXPR >= 200704L
#define	cnLib_CONSTVAR	constexpr
// cnLibrary_CPPFEATURE_CONSTEXPR >= 200704L
#else
// cnLibrary_CPPFEATURE_CONSTEXPR < 200704L
#define	cnLib_CONSTVAR	const
#endif // cnLibrary_CPPFEATURE_CONSTEXPR < 200704L
//---------------------------------------------------------------------------
namespace cnLibrary{
//---------------------------------------------------------------------------
class cnLib_INTERFACE iWindowsConsoleHost : public iReference
{
public:
	virtual iConsoleOutput* cnLib_FUNC GetOutput(void)noexcept(true)=0;
	virtual iConsoleInput* cnLib_FUNC GetInput(void)noexcept(true)=0;
};
//---------------------------------------------------------------------------
namespace cnWindows{
//---------------------------------------------------------------------------
rPtr<iWindowsConsoleHost> CreateWindowsConsoleHost(iDispatch *Dispatch)noexcept(true);
//---------------------------------------------------------------------------
}   // namespace cnWindows
//---------------------------------------------------------------------------
class iWindow;
//---------------------------------------------------------------------------
struct cWindowMessageParam
{
	UINT Code;
	WPARAM wParam;
	LPARAM lParam;
};
//---------------------------------------------------------------------------
class cnLib_INTERFACE iWindowMessageHandler
{
public:
	virtual void cnLib_FUNC WindowAttached(void)noexcept(true)=0;
	virtual void cnLib_FUNC WindowDetached(void)noexcept(true)=0;
	virtual bool cnLib_FUNC WindowMessage(LRESULT &Result,const cWindowMessageParam &MsgParam)noexcept(true)=0;
	virtual void cnLib_FUNC WindowMessageProcessed(LRESULT Result,const cWindowMessageParam &MsgParam)noexcept(true)=0;
};
//---------------------------------------------------------------------------
class iWindowViewport : public iInterface
{
public:
	struct tInterfaceID{	static iTypeID Value;	};
	virtual void* cnLib_FUNC CastInterface(iTypeID ID)noexcept(true) override{		return cnVar::ImplementCastInterface(this,ID);	}

	virtual bool cnLib_FUNC GetMouseAutoCapture(void)noexcept(true)=0;
	virtual bool cnLib_FUNC SetMouseAutoCapture(bool Enable)noexcept(true)=0;

	//virtual bool cnLib_FUNC InsertWindowViewportHandler(iWindowMessageHandler *Handler)=0;
	//virtual bool cnLib_FUNC RemoveWindowViewportHandler(iWindowMessageHandler *Handler)=0;
	//virtual bool cnLib_FUNC IsMouseCaptured(iWindowMessageHandler *Handler)=0;
	//virtual bool cnLib_FUNC MouseCaptureEx(iWindowMessageHandler *Handler)=0;
};
//---------------------------------------------------------------------------
class cnLib_INTERFACE iWindowHandleProperty : public iInterface
{
public:
	struct tInterfaceID{	static iTypeID Value;	};
	virtual void* cnLib_FUNC CastInterface(iTypeID ID)noexcept(true) override{		return cnVar::ImplementCastInterface(this,ID);	}

	virtual HWND cnLib_FUNC GetWindowHandle(void)noexcept(true)=0;
};
//---------------------------------------------------------------------------
class cnLib_INTERFACE iWindowClient : public iInterface
{
public:
	struct tInterfaceID{	static iTypeID Value;	};
	virtual void* cnLib_FUNC CastInterface(iTypeID ID)noexcept(true) override{		return cnVar::ImplementCastInterface(this,ID);	}

	virtual iWindow* cnLib_FUNC GetWindow(void)noexcept(true)=0;
	virtual bool cnLib_FUNC SetWindow(iWindow *Window)noexcept(true)=0;
	// Client
	virtual void cnLib_FUNC SetBackgroundColor(COLORREF Color)noexcept(true)=0;

};
//---------------------------------------------------------------------------
class cnLib_INTERFACE iWindowFrame : public iInterface
{
public:
	struct tInterfaceID{	static iTypeID Value;	};
	virtual void* cnLib_FUNC CastInterface(iTypeID ID)noexcept(true) override{		return cnVar::ImplementCastInterface(this,ID);	}

	virtual iWindow* cnLib_FUNC GetWindow(void)noexcept(true)=0;
	virtual bool cnLib_FUNC SetWindow(iWindow *Window)noexcept(true)=0;

	virtual bool cnLib_FUNC SetBorderDefault(void)noexcept(true)=0;
	virtual RECT cnLib_FUNC GetBorder(void)noexcept(true)=0;
	virtual bool cnLib_FUNC SetBorder(const RECT &BorderSize)noexcept(true)=0;
	//virtual void cnLib_FUNC SetBorderBackgroundColor(COLORREF Color)noexcept(true)=0;
};
//---------------------------------------------------------------------------
class cnLib_INTERFACE iWindow : public iInterface
{
public:
	struct tInterfaceID{	static iTypeID Value;	};
	virtual void* cnLib_FUNC CastInterface(iTypeID ID)noexcept(true) override{		return cnVar::ImplementCastInterface(this,ID);	}

	virtual iUIThread* cnLib_FUNC GetUIThread(void)noexcept(true)=0;
	virtual HWND cnLib_FUNC GetWindowHandle(void)noexcept(true)=0;

	virtual bool cnLib_FUNC InsertMessageHandler(iWindowMessageHandler *MessageHandler,sfInt8 Order=0)noexcept(true)=0;
	virtual bool cnLib_FUNC RemoveMessageHandler(iWindowMessageHandler *MessageHandler)noexcept(true)=0;

	virtual iWindowClient* GetClient(void)noexcept(true)=0;
	virtual iWindowFrame* GetFrame(void)noexcept(true)=0;

	virtual rPtr<iVariable> QueryAffixedVariable(const void *Token)noexcept(true)=0;

	// SetUserInputProvider
	//	specify user input provider for the ui object
	// [in]Provider		provider, if the parameter is null, the ui object uses its default provider
	// return true if provider is accepted
	//virtual bool cnLib_FUNC SetUserInputProvider(iInterface *Provider)noexcept(true)=0;

};
//---------------------------------------------------------------------------
class cnLib_INTERFACE iWindowProvider : public iInterface
{
public:
	virtual bool cnLib_FUNC SubclassAttach(HWND Window,bool LocalClass)noexcept(true)=0;
	virtual bool cnLib_FUNC SubclassDetach(void)noexcept(true)=0;
	virtual bool cnLib_FUNC Create(HWND Parent,const wchar_t *WindowText,DWORD Style=WS_OVERLAPPEDWINDOW,DWORD ExStyle=0,LONG X=CW_USEDEFAULT,LONG Y=CW_USEDEFAULT,LONG Width=CW_USEDEFAULT,LONG Height=CW_USEDEFAULT,UINT ChildID=0)noexcept(true)=0;
	virtual INT_PTR cnLib_FUNC ModalDialog(HWND Parent,const wchar_t *WindowText,LONG X=0,LONG Y=0,LONG Width=0,LONG Height=0,DWORD Style=WS_CAPTION|WS_SYSMENU|WS_DLGFRAME,DWORD ExStyle=WS_EX_DLGMODALFRAME)noexcept(true)=0;
	virtual INT_PTR cnLib_FUNC ModalDialogTemplate(HWND Parent,HINSTANCE hInstance,LPCWSTR TemplateName)noexcept(true)=0;
	virtual INT_PTR cnLib_FUNC ModalDialogTemplateIndirect(HWND Parent,HINSTANCE hInstance,LPCDLGTEMPLATEW DialogTemplate)noexcept(true)=0;
};
//- Window ------------------------------------------------------------------
class cnLib_INTERFACE iOwnerFocusWindowClient : public iWindowClient
{
public:
	virtual void cnLib_FUNC UpdateOwner(void)noexcept(true)=0;
};
//---------------------------------------------------------------------------
class cnLib_INTERFACE iWindowsUISessionHandler
{
public:
	virtual void cnLib_FUNC UISessionStart(void)noexcept(true)=0;
	virtual void cnLib_FUNC UISessionExit(void)noexcept(true)=0;
};
//---------------------------------------------------------------------------
class cnLib_INTERFACE iWindowsUIApplication : public iReference
{
public:

	virtual iUIThread* cnLib_FUNC GetMainUIThread(void)noexcept(true)=0;
	virtual bool cnLib_FUNC InsertHandler(iWindowsUISessionHandler *SessionHandler)noexcept(true)=0;
	virtual bool cnLib_FUNC RemoveHandler(iWindowsUISessionHandler *SessionHandler)noexcept(true)=0;
	virtual void cnLib_FUNC UIMain(void)noexcept(true)=0;
	virtual void cnLib_FUNC CloseUISession(void)noexcept(true)=0;
};
//---------------------------------------------------------------------------
class cnLib_INTERFACE iMouseCursorSetter : public iInterface
{
public:
	struct tInterfaceID{	static iTypeID Value;	};
	virtual void* cnLib_FUNC CastInterface(iTypeID ID)noexcept(true) override{		return cnVar::ImplementCastInterface(this,ID);	}

	virtual void cnLib_FUNC SetCursor(void)noexcept(true)=0;
};
//---------------------------------------------------------------------------
// GDI 
//---------------------------------------------------------------------------
class cnLib_INTERFACE iGDIFontHandle : public iReference
{
public:
	virtual HFONT cnLib_FUNC GetFontHandle(void)noexcept(true)=0;
};
//---------------------------------------------------------------------------
class cnLib_INTERFACE iGDIBitmap : public iInterface
{
public:
	struct tInterfaceID{	static iTypeID Value;	};
	virtual void* cnLib_FUNC CastInterface(iTypeID ID)noexcept(true) override{		return cnVar::ImplementCastInterface(this,ID);	}
	
	virtual HBITMAP cnLib_FUNC GetBitmap(void)noexcept(true)=0;
	virtual bool cnLib_FUNC HasAlpha(void)noexcept(true)=0;
};
//---------------------------------------------------------------------------
class cnLib_INTERFACE iGDIDC : public iReference
{
public:
	virtual HDC cnLib_FUNC GetDC(void)noexcept(true)=0;
};
//---------------------------------------------------------------------------
class cnLib_INTERFACE iDCPaintDevice : public iReference
{
public:
	virtual iGDIDC* cnLib_FUNC GetScreenInfomationDC(void)noexcept(true)=0;

	virtual rPtr<iGDIFontHandle> cnLib_FUNC QueryFont(const cUITextStyle &TextStyle)noexcept(true)=0;
	virtual rPtr<iGDIDC> cnLib_FUNC QueryMemoryDC(void)noexcept(true)=0;

	virtual iPtr<iGDIBitmap> cnLib_FUNC CreateBitmapCopyFromSource(iBitmapDataSource *Source)noexcept(true)=0;
};
//---------------------------------------------------------------------------
class cnLib_INTERFACE iDCPainter : public iUIPainter
{
public:
	// PaintRectChanged
	//	called when rectangle of the content changed.
	//	will not called when paint state is idle
	virtual void cnLib_FUNC PaintRectChanged(void)noexcept(true)=0;
	// PaintWindowChanged
	//	called when rectangle of the content changed
	//	will not called when paint state is idle
	virtual void cnLib_FUNC PaintWindowChanged(void)noexcept(true)=0;
	// Paint
	//	paint content rectangle to DC
	//	will not called when paint state is background or lowwer
	virtual void cnLib_FUNC Paint(HDC DC,HRGN ClipRegion)noexcept(true)=0;

};
//---------------------------------------------------------------------------
static cnLib_CONSTVAR uIntn DCViewContentOptions_BackgroundRending		=0x01;
static cnLib_CONSTVAR uIntn DCViewContentOptions_OutputAlpha			=0x02;
//---------------------------------------------------------------------------
class cnLib_INTERFACE iDCViewContent : public iUIViewContent
{
public:
	// GetPaintRect
	//	get paint rectangle in DC
	// return paint rectangle
	virtual RECT cnLib_FUNC GetPaintRect(void)noexcept(true)=0;
	// GetDC
	virtual HDC	cnLib_FUNC GetDC(void)noexcept(true)=0;
	// ReleaseDC
	virtual void cnLib_FUNC ReleaseDC(HDC DC)noexcept(true)=0;

	virtual HDC	cnLib_FUNC GetIC(void)noexcept(true)=0;
	virtual void cnLib_FUNC ReleaseIC(HDC DC)noexcept(true)=0;

	// InvalidateRect
	virtual void cnLib_FUNC InvalidateRect(const RECT *Rect)noexcept(true)=0;

	virtual eUIState cnLib_FUNC GetPaintState(void)noexcept(true)=0;
	virtual iWindow* cnLib_FUNC GetWindow(void)noexcept(true)=0;

};
//---------------------------------------------------------------------------
// DXGI
//---------------------------------------------------------------------------
class cnLib_INTERFACE iDXGIPainter : public iUIPainter
{
public:
	virtual void cnLib_FUNC PaintSizeChanged(void)noexcept(true)=0;
	virtual void cnLib_FUNC RenderBufferChanged(void)noexcept(true)=0;
};
//---------------------------------------------------------------------------
class cnLib_INTERFACE iDXGIViewContent : public iUIViewContent
{
public:
	virtual eUIState cnLib_FUNC GetPaintState(void)noexcept(true)=0;
	virtual cUIPoint cnLib_FUNC GetPaintSize(void)noexcept(true)=0;
	virtual bool GetRenderBufferSharedHandle(HANDLE &SharedHandle)noexcept(true)=0;
	virtual void UpdateRenderBuffer(void)noexcept(true)=0;
};
//---------------------------------------------------------------------------
namespace cnWindows{
//---------------------------------------------------------------------------
iWindow* cnLib_FUNC GetWindowFromHandle(HWND WindowHandle)noexcept(true);
//---------------------------------------------------------------------------
iPtr<iWindow> cnLib_FUNC CreateWindowHandle(HWND Parent,const wchar_t *WindowText,DWORD Style=WS_OVERLAPPEDWINDOW,DWORD ExStyle=0,LONG X=CW_USEDEFAULT,LONG Y=CW_USEDEFAULT,LONG Width=CW_USEDEFAULT,LONG Height=CW_USEDEFAULT,UINT ChildID=0)noexcept(true);
rPtr<iWindowsUIApplication> cnLib_FUNC CreateWindowsUIApplication(void)noexcept(true);
iPtr<iUIThread> cnLib_FUNC CreateUIThreadOnCurrentThread(void)noexcept(true);
iPtr<iUIThread> cnLib_FUNC StartUIThread(void)noexcept(true);
//---------------------------------------------------------------------------
iPtr<iWindowProvider> cnLib_FUNC CreateWindowProvider(void)noexcept(true);
iPtr<iWindowClient> cnLib_FUNC CreateWindowClient(void)noexcept(true);
iPtr<iWindowFrame> cnLib_FUNC CreateWindowFrame(void)noexcept(true);
iPtr<iOwnerFocusWindowClient> cnLib_FUNC CreateOwnerFocusWindowClient(void)noexcept(true);
//---------------------------------------------------------------------------
rPtr<iDCPaintDevice> cnLib_FUNC QueryDCPaintDevice(void)noexcept(true);
rPtr<iDCViewContent> cnLib_FUNC CreateDCViewContent(iDCPainter *Painter,uIntn DCViewContentOptions=0)noexcept(true);

//---------------------------------------------------------------------------

rPtr<iDXGIViewContent> cnLib_FUNC CreateDXGIViewContent(iDXGIPainter *Painter)noexcept(true);
//---------------------------------------------------------------------------
}	// namespace cnWindows
//---------------------------------------------------------------------------
}	// namespace cnLibrary
//---------------------------------------------------------------------------
#undef cnLib_CONSTVAR
//---------------------------------------------------------------------------
#ifdef	cnLibrary_CPPEXCLUDE_VIRTUAL_OVERRIDE
#undef	override
#endif
//---------------------------------------------------------------------------
#endif  /* __cplusplus */
/*-------------------------------------------------------------------------*/

