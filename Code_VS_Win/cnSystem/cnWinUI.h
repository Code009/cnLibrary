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
class iWindow;
//---------------------------------------------------------------------------
class cnLib_INTERFACE iWindowsConsoleHost : public iReference
{
public:
	virtual iConsoleOutput* cnLib_FUNC GetOutput(void)=0;
	virtual iConsoleInput* cnLib_FUNC GetInput(void)=0;
	virtual bool cnLib_FUNC ConsoleMain(void)=0;
};
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
	virtual void cnLib_FUNC WindowAttached(void)=0;
	virtual void cnLib_FUNC WindowDetached(void)=0;
	virtual bool cnLib_FUNC WindowMessage(LRESULT &Result,const cWindowMessageParam &MsgParam)=0;
	virtual void cnLib_FUNC WindowMessageProcessed(LRESULT Result,const cWindowMessageParam &MsgParam)=0;
};
//---------------------------------------------------------------------------
class iWindowViewport : public iInterface
{
public:
	struct tInterfaceID{	static iTypeID Value;	};
	virtual void* cnLib_FUNC CastInterface(iTypeID ID)noexcept(true) override{		return cnVar::ImplementCastInterface(this,ID);	}

	virtual bool cnLib_FUNC GetMouseAutoCapture(void)=0;
	virtual bool cnLib_FUNC SetMouseAutoCapture(bool Enable)=0;

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

	virtual HWND cnLib_FUNC GetWindowHandle(void)=0;
};
//---------------------------------------------------------------------------
class cnLib_INTERFACE iWindowClient : public iInterface
{
public:
	struct tInterfaceID{	static iTypeID Value;	};
	virtual void* cnLib_FUNC CastInterface(iTypeID ID)noexcept(true) override{		return cnVar::ImplementCastInterface(this,ID);	}

	virtual iWindow* cnLib_FUNC GetWindow(void)=0;
	virtual bool cnLib_FUNC SetWindow(iWindow *Window)=0;
	// Client
	virtual void cnLib_FUNC SetBackgroundColor(COLORREF Color)=0;

};
//---------------------------------------------------------------------------
class cnLib_INTERFACE iWindowFrame : public iInterface
{
public:
	struct tInterfaceID{	static iTypeID Value;	};
	virtual void* cnLib_FUNC CastInterface(iTypeID ID)noexcept(true) override{		return cnVar::ImplementCastInterface(this,ID);	}

	virtual iWindow* cnLib_FUNC GetWindow(void)=0;
	virtual bool cnLib_FUNC SetWindow(iWindow *Window)=0;

	virtual bool cnLib_FUNC SetBorderDefault(void)=0;
	virtual RECT cnLib_FUNC GetBorder(void)=0;
	virtual bool cnLib_FUNC SetBorder(const RECT &BorderSize)=0;
	//virtual void cnLib_FUNC SetBorderBackgroundColor(COLORREF Color)=0;
};
//---------------------------------------------------------------------------
class cnLib_INTERFACE iWindow : public iInterface
{
public:
	struct tInterfaceID{	static iTypeID Value;	};
	virtual void* cnLib_FUNC CastInterface(iTypeID ID)noexcept(true) override{		return cnVar::ImplementCastInterface(this,ID);	}

	virtual iUIThread* cnLib_FUNC GetUIThread(void)=0;
	virtual HWND cnLib_FUNC GetWindowHandle(void)=0;

	virtual bool cnLib_FUNC InsertMessageHandler(iWindowMessageHandler *MessageHandler,sfInt8 Order=0)=0;
	virtual bool cnLib_FUNC RemoveMessageHandler(iWindowMessageHandler *MessageHandler)=0;

	virtual iWindowClient* GetClient(void)=0;
	virtual iWindowFrame* GetFrame(void)=0;

	virtual rPtr<iVariable> QueryAffixedVariable(const void *Token)=0;

	// SetUserInputProvider
	//	specify user input provider for the ui object
	// [in]Provider		provider, if the parameter is null, the ui object uses its default provider
	// return true if provider is accepted
	//virtual bool cnLib_FUNC SetUserInputProvider(iInterface *Provider)=0;

};
//---------------------------------------------------------------------------
class cnLib_INTERFACE iWindowProvider : public iInterface
{
public:
	virtual bool cnLib_FUNC SubclassAttach(HWND Window,bool LocalClass)=0;
	virtual bool cnLib_FUNC SubclassDetach(void)=0;
	virtual bool cnLib_FUNC Create(HWND Parent,const wchar_t *WindowText,DWORD Style=WS_OVERLAPPEDWINDOW,DWORD ExStyle=0,LONG X=CW_USEDEFAULT,LONG Y=CW_USEDEFAULT,LONG Width=CW_USEDEFAULT,LONG Height=CW_USEDEFAULT,UINT ChildID=0)=0;
	virtual INT_PTR cnLib_FUNC ModalDialog(HWND Parent,const wchar_t *WindowText,LONG X=0,LONG Y=0,LONG Width=0,LONG Height=0,DWORD Style=WS_CAPTION|WS_SYSMENU|WS_DLGFRAME,DWORD ExStyle=WS_EX_DLGMODALFRAME)=0;
	virtual INT_PTR cnLib_FUNC ModalDialogTemplate(HWND Parent,HINSTANCE hInstance,LPCWSTR TemplateName)=0;
	virtual INT_PTR cnLib_FUNC ModalDialogTemplateIndirect(HWND Parent,HINSTANCE hInstance,LPCDLGTEMPLATEW DialogTemplate)=0;
};
//- Window ------------------------------------------------------------------
class cnLib_INTERFACE iOwnerFocusWindowClient : public iWindowClient
{
public:
	virtual void cnLib_FUNC UpdateOwner(void)=0;
};
//---------------------------------------------------------------------------
class cnLib_INTERFACE iWindowsUIApplication : public iReference
{
public:
	virtual iUIThread* cnLib_FUNC GetMainUIThread(void)=0;
	virtual void cnLib_FUNC UIMain(iWindow *MainWindow,int CmdShow)=0;
};
//---------------------------------------------------------------------------
class cnLib_INTERFACE iMouseCursorSetter : public iInterface
{
public:
	struct tInterfaceID{	static iTypeID Value;	};
	virtual void* cnLib_FUNC CastInterface(iTypeID ID)noexcept(true) override{		return cnVar::ImplementCastInterface(this,ID);	}

	virtual void cnLib_FUNC SetCursor(void)=0;
};
//---------------------------------------------------------------------------
// GDI 
//---------------------------------------------------------------------------
class cnLib_INTERFACE iGDIFontHandle : public iReference
{
public:
	virtual HFONT cnLib_FUNC GetFontHandle(void)=0;
};
//---------------------------------------------------------------------------
class cnLib_INTERFACE iGDIBitmap : public iInterface
{
public:
	struct tInterfaceID{	static iTypeID Value;	};
	virtual void* cnLib_FUNC CastInterface(iTypeID ID)noexcept(true) override{		return cnVar::ImplementCastInterface(this,ID);	}
	
	virtual HBITMAP cnLib_FUNC GetBitmap(void)=0;
	virtual bool cnLib_FUNC HasAlpha(void)=0;
};
//---------------------------------------------------------------------------
class cnLib_INTERFACE iGDIDC : public iReference
{
public:
	virtual HDC cnLib_FUNC GetDC(void)=0;
};
//---------------------------------------------------------------------------
class cnLib_INTERFACE iDCPaintDevice : public iReference
{
public:
	virtual iGDIDC* cnLib_FUNC GetScreenInfomationDC(void)=0;

	virtual rPtr<iGDIFontHandle> cnLib_FUNC QueryFont(const cUITextStyle &TextStyle)=0;
	virtual rPtr<iGDIDC> cnLib_FUNC QueryMemoryDC(void)=0;

	virtual iPtr<iGDIBitmap> cnLib_FUNC CreateBitmapCopyFromSource(iBitmapDataSource *Source)=0;
};
//---------------------------------------------------------------------------
class cnLib_INTERFACE iDCPainter : public iUIPainter
{
public:
	// PaintRectChanged
	//	called when rectangle of the content changed.
	//	will not called when paint state is idle
	virtual void cnLib_FUNC PaintRectChanged(void)=0;
	// PaintWindowChanged
	//	called when rectangle of the content changed
	//	will not called when paint state is idle
	virtual void cnLib_FUNC PaintWindowChanged(void)=0;
	// Paint
	//	paint content rectangle to DC
	//	will not called when paint state is background or lowwer
	virtual void cnLib_FUNC Paint(HDC DC,HRGN ClipRegion)=0;

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
	virtual RECT cnLib_FUNC GetPaintRect(void)=0;
	// GetDC
	virtual HDC	cnLib_FUNC GetDC(void)=0;
	// ReleaseDC
	virtual void cnLib_FUNC ReleaseDC(HDC DC)=0;

	virtual HDC	cnLib_FUNC GetIC(void)=0;
	virtual void cnLib_FUNC ReleaseIC(HDC DC)=0;

	// InvalidateRect
	virtual void cnLib_FUNC InvalidateRect(const RECT *Rect)=0;

	virtual eUIState cnLib_FUNC GetPaintState(void)=0;
	virtual iWindow* cnLib_FUNC GetWindow(void)=0;

};
//---------------------------------------------------------------------------
// DXGI
//---------------------------------------------------------------------------
class cnLib_INTERFACE iDXGIPainter : public iUIPainter
{
public:
	virtual void cnLib_FUNC PaintSizeChanged(void)=0;
	virtual void cnLib_FUNC RenderBufferChanged(void)=0;
};
//---------------------------------------------------------------------------
class cnLib_INTERFACE iDXGIViewContent : public iUIViewContent
{
public:
	virtual eUIState cnLib_FUNC GetPaintState(void)=0;
	virtual cUIPoint cnLib_FUNC GetPaintSize(void)=0;
	virtual bool GetRenderBufferSharedHandle(HANDLE &SharedHandle)=0;
	virtual void UpdateRenderBuffer(void)=0;
};
//---------------------------------------------------------------------------
namespace cnWindows{
//---------------------------------------------------------------------------
iWindow* cnLib_FUNC GetWindowFromHandle(HWND WindowHandle);
//---------------------------------------------------------------------------
iPtr<iWindow> cnLib_FUNC CreateHWND(HWND Parent,const wchar_t *WindowText,DWORD Style=WS_OVERLAPPEDWINDOW,DWORD ExStyle=0,LONG X=CW_USEDEFAULT,LONG Y=CW_USEDEFAULT,LONG Width=CW_USEDEFAULT,LONG Height=CW_USEDEFAULT,UINT ChildID=0);
rPtr<iWindowsUIApplication> cnLib_FUNC CreateWindowsUIApplication(void);
iPtr<iUIThread> cnLib_FUNC CreateUIThreadOnCurrentThread(void);
iPtr<iUIThread> cnLib_FUNC StartUIThread(void);
//---------------------------------------------------------------------------
iPtr<iWindowProvider> cnLib_FUNC CreateWindowProvider(void);
iPtr<iWindowClient> cnLib_FUNC CreateWindowClient(void);
iPtr<iWindowFrame> cnLib_FUNC CreateWindowFrame(void);
iPtr<iOwnerFocusWindowClient> cnLib_FUNC CreateOwnerFocusWindowClient(void);
//---------------------------------------------------------------------------
rPtr<iDCPaintDevice> cnLib_FUNC QueryDCPaintDevice(void);
rPtr<iDCViewContent> cnLib_FUNC CreateDCViewContent(iDCPainter *Painter,uIntn DCViewContentOptions=0);

//---------------------------------------------------------------------------

rPtr<iDXGIViewContent> cnLib_FUNC CreateDXGIViewContent(iDXGIPainter *Painter);
//---------------------------------------------------------------------------
}	// namespace cnWindows
//---------------------------------------------------------------------------
}	// namespace cnLibrary
//---------------------------------------------------------------------------
#undef cnLib_CONSTVAR
//---------------------------------------------------------------------------
#endif  /* __cplusplus */
/*-------------------------------------------------------------------------*/

