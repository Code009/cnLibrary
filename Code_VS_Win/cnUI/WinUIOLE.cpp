#include "WinUIOLE.h"

using namespace cnLibrary;
using namespace cnUI;

static constexpr int HIMETRIC_INCH=2540;	// HIMETRIC units per inch
//---------------------------------------------------------------------------
SIZE cnUI::OLE_DPtoHIMETRIC(HDC DC,const SIZE &Size)noexcept
{
	int cxPerInch, cyPerInch;
	cxPerInch=::GetDeviceCaps(DC,LOGPIXELSX);
	cyPerInch=::GetDeviceCaps(DC,LOGPIXELSY);

	SIZE RetSize;
	RetSize.cx=MulDiv(Size.cx, HIMETRIC_INCH, cxPerInch);
	RetSize.cy=MulDiv(Size.cy, HIMETRIC_INCH, cyPerInch);
	return RetSize;
}
SIZE cnUI::OLE_HIMETRICtoDP(HDC DC,const SIZE &Size)noexcept
{
	int cxPerInch, cyPerInch;
	cxPerInch=::GetDeviceCaps(DC,LOGPIXELSX);
	cyPerInch=::GetDeviceCaps(DC,LOGPIXELSY);
	
	SIZE RetSize;
	RetSize.cx=MulDiv(Size.cx, cxPerInch, HIMETRIC_INCH);
	RetSize.cy=MulDiv(Size.cy, cyPerInch, HIMETRIC_INCH);
	return RetSize;
}
//---------------------------------------------------------------------------
RECT cnUI::OLE_DPtoHIMETRIC(HDC DC,const RECT &Rect)noexcept
{
	int cxPerInch, cyPerInch;
	cxPerInch=::GetDeviceCaps(DC,LOGPIXELSX);
	cyPerInch=::GetDeviceCaps(DC,LOGPIXELSY);

	RECT RetRect;
	RetRect.left=MulDiv(Rect.left, HIMETRIC_INCH, cxPerInch);
	RetRect.right=MulDiv(Rect.right, HIMETRIC_INCH, cxPerInch);
	RetRect.top=MulDiv(Rect.top, HIMETRIC_INCH, cyPerInch);
	RetRect.bottom=MulDiv(Rect.bottom, HIMETRIC_INCH, cyPerInch);
	return RetRect;
}
RECT cnUI::OLE_HIMETRICtoDP(HDC DC,const RECT &Rect)noexcept
{
	int cxPerInch, cyPerInch;
	cxPerInch=::GetDeviceCaps(DC,LOGPIXELSX);
	cyPerInch=::GetDeviceCaps(DC,LOGPIXELSY);
	
	RECT RetRect;
	RetRect.left=MulDiv(Rect.left, cxPerInch, HIMETRIC_INCH);
	RetRect.right=MulDiv(Rect.right, cxPerInch, HIMETRIC_INCH);
	RetRect.top=MulDiv(Rect.top, cyPerInch, HIMETRIC_INCH);
	RetRect.bottom=MulDiv(Rect.bottom, cyPerInch, HIMETRIC_INCH);
	return RetRect;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cOLEInPlaceUIForm::cOLEInPlaceUIForm()noexcept
{
	fHostWindowHandle=nullptr;
}
//---------------------------------------------------------------------------
cOLEInPlaceUIForm::~cOLEInPlaceUIForm()noexcept
{
}
//---------------------------------------------------------------------------
void cOLEInPlaceUIForm::UIStarted(void)noexcept
{
	fHostWindowHandle=GetWindowHandleFromUIView(fView);
}
//---------------------------------------------------------------------------
void cOLEInPlaceUIForm::UIStopped(void)noexcept
{
	fHostWindowHandle=nullptr;
}
//---------------------------------------------------------------------------
HWND cOLEInPlaceUIForm::GetWindowHandle(void)noexcept
{
	return fHostWindowHandle;
}
//---------------------------------------------------------------------------
bool cOLEInPlaceUIForm::GetToolRect(RECT &Rect)noexcept
{
	if(fView==nullptr)
		return false;
	auto Window=fView->GetWindow();
	cUIPoint Pos;
	fView->GetPosition(Window,Pos);
	auto Size=fView->GetSize();
	Rect.left=Pos.x;
	Rect.top=Pos.y;
	Rect.right=Rect.left+Size.x;
	Rect.bottom=Rect.top+Size.y;
	return true;
}
//---------------------------------------------------------------------------
bool cOLEInPlaceUIForm::CheckToolSpace(const BORDERWIDTHS &Space)noexcept
{
	if(fView==nullptr)
		return false;
	if(Space.left>AllowSpaceLeft)
		return false;
	if(Space.right>AllowSpaceRight)
		return false;
	if(Space.top>AllowSpaceTop)
		return false;
	if(Space.bottom>AllowSpaceBottom)
		return false;
	return true;
}
//---------------------------------------------------------------------------
void cOLEInPlaceUIForm::RestoreToolSpace(void)noexcept
{
	fSpaceLeft=0;
	fSpaceTop=0;
	fSpaceRight=0;
	fSpaceBottom=0;
	if(fView!=nullptr){
		fView->SetArrangement();
	}

}
//---------------------------------------------------------------------------
bool cOLEInPlaceUIForm::SetToolSpace(const BORDERWIDTHS &Space)noexcept
{
	if(fView==nullptr)
		return false;
	if(Space.left>AllowSpaceLeft)
		return false;
	if(Space.right>AllowSpaceRight)
		return false;
	if(Space.top>AllowSpaceTop)
		return false;
	if(Space.bottom>AllowSpaceBottom)
		return false;
	fSpaceLeft=Space.left;
	fSpaceTop=Space.top;
	fSpaceRight=Space.right;
	fSpaceBottom=Space.bottom;
	if(fView!=nullptr){
		fView->SetArrangement();
	}
	return true;
}
//---------------------------------------------------------------------------
cLayout cOLEInPlaceUIForm::GetClientLayout(void)noexcept
{
	auto ViewSize=fView->GetSize();
	cUIPoint Size;
	Size.x=ViewSize.x-fSpaceRight-fSpaceLeft;
	Size.y=ViewSize.y-fSpaceBottom-fSpaceTop;
	cLayout Layout;
	Layout.Reset({static_cast<Float32>(fSpaceLeft),static_cast<Float32>(fSpaceTop)},Size);
	return Layout;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cOleInPlaceUIWindowComponent::cOleInPlaceUIWindowComponent()noexcept
{
}
//---------------------------------------------------------------------------
cOleInPlaceUIWindowComponent::~cOleInPlaceUIWindowComponent()noexcept
{
}
//---------------------------------------------------------------------------
IUnknown* cOleInPlaceUIWindowComponent::COMUnknown(void)noexcept
{
	return this;
}
//---------------------------------------------------------------------------
bool cOleInPlaceUIWindowComponent::COMQueryInterface(REFIID riid,_COM_Outptr_ void __RPC_FAR *__RPC_FAR *ppvObject)noexcept
{
	if(riid==IID_IOleWindow){
		*ppvObject=static_cast<IOleWindow*>(this);
		return true;
	}
	if(riid==IID_IOleInPlaceUIWindow){
		*ppvObject=static_cast<IOleInPlaceUIWindow*>(this);
		return true;
	}
	return false;
}
//---------------------------------------------------------------------------
HRESULT STDMETHODCALLTYPE cOleInPlaceUIWindowComponent::GetWindow( 
    /* [out] */ __RPC__deref_out_opt HWND *phwnd)noexcept
{
// Retrieves a handle to one of the windows participating in in-place activation (frame, document, parent, or in-place object window).
	if(ToolForm==nullptr){
		*phwnd=nullptr;
		return S_OK;
	}
	*phwnd=ToolForm->GetWindowHandle();
	return S_OK;
}
//---------------------------------------------------------------------------      
HRESULT STDMETHODCALLTYPE cOleInPlaceUIWindowComponent::ContextSensitiveHelp(
    /* [in] */ BOOL fEnterMode)noexcept
{fEnterMode;
// Determines whether context-sensitive help mode should be entered during an in-place activation session.
// optional
	return E_NOTIMPL;
}
//---------------------------------------------------------------------------
HRESULT STDMETHODCALLTYPE cOleInPlaceUIWindowComponent::GetBorder(
    /* [out] */ __RPC__out LPRECT lprectBorder)noexcept
{
// Retrieves the outer rectange for toolbars and controls while the object is active in place.
// optional : Necessary for containers with toolbar UI 
	if(ToolForm==nullptr){
		return INPLACE_E_NOTOOLSPACE;
	}
	if(ToolForm->GetToolRect(*lprectBorder)==false){
		return INPLACE_E_NOTOOLSPACE;
	}
	return S_OK;
}
//---------------------------------------------------------------------------
HRESULT STDMETHODCALLTYPE cOleInPlaceUIWindowComponent::RequestBorderSpace(
    /* [unique][in] */ __RPC__in_opt LPCBORDERWIDTHS pborderwidths)noexcept
{
// Determines whether there is space available for tools to be installed around the object's window frame while the object is active in place.
// optional : Necessary for containers with toolbar UI 
	if(ToolForm==nullptr){
		return INPLACE_E_NOTOOLSPACE;
	}
	if(ToolForm->CheckToolSpace(*pborderwidths)==false)
		return INPLACE_E_NOTOOLSPACE;
	return S_OK;
}
//---------------------------------------------------------------------------
HRESULT STDMETHODCALLTYPE cOleInPlaceUIWindowComponent::SetBorderSpace(
    /* [unique][in] */ __RPC__in_opt LPCBORDERWIDTHS pborderwidths)noexcept
{
// Allocates space for the border requested in the call to IOleInPlaceUIWindow::RequestBorderSpace.
// optional : Necessary for containers with toolbar UI 
	if(pborderwidths==nullptr){
		if(ToolForm!=nullptr)
			ToolForm->RestoreToolSpace();
		return S_OK;
	}
	
	if(ToolForm==nullptr){
		return OLE_E_INVALIDRECT;
	}
	if(ToolForm->SetToolSpace(*pborderwidths)==false)
		return OLE_E_INVALIDRECT;
	return S_OK;
}
//---------------------------------------------------------------------------
HRESULT STDMETHODCALLTYPE cOleInPlaceUIWindowComponent::SetActiveObject(
    /* [unique][in] */ __RPC__in_opt IOleInPlaceActiveObject *pActiveObject,
    /* [unique][string][in] */ __RPC__in_opt_string LPCOLESTR pszObjName)noexcept
{
// Provides a direct channel of communication between the object and each of the frame and document windows.
// optional
	return E_NOTIMPL;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cOleInPlaceFrameComponent::cOleInPlaceFrameComponent()noexcept
{
}
//---------------------------------------------------------------------------
cOleInPlaceFrameComponent::~cOleInPlaceFrameComponent()noexcept
{
}
//---------------------------------------------------------------------------
IUnknown* cOleInPlaceFrameComponent::COMUnknown(void)noexcept
{
	return this;
}
//---------------------------------------------------------------------------
bool cOleInPlaceFrameComponent::COMQueryInterface(REFIID riid,_COM_Outptr_ void __RPC_FAR *__RPC_FAR *ppvObject)noexcept
{
	if(riid==IID_IOleWindow){
		*ppvObject=static_cast<IOleWindow*>(this);
		return true;
	}
	if(riid==IID_IOleInPlaceUIWindow){
		*ppvObject=static_cast<IOleInPlaceUIWindow*>(this);
		return true;
	}
	if(riid==IID_IOleInPlaceFrame){
		*ppvObject=static_cast<IOleInPlaceFrame*>(this);
		return true;
	}
	return false;
}
//---------------------------------------------------------------------------
HRESULT STDMETHODCALLTYPE cOleInPlaceFrameComponent::GetWindow( 
    /* [out] */ __RPC__deref_out_opt HWND *phwnd)noexcept
{
// Retrieves a handle to one of the windows participating in in-place activation (frame, document, parent, or in-place object window).
	if(ToolForm==nullptr){
		*phwnd=fWindow->GetWindowHandle();
		return S_OK;
	}
	*phwnd=ToolForm->GetWindowHandle();
	return S_OK;
}
//---------------------------------------------------------------------------      
HRESULT STDMETHODCALLTYPE cOleInPlaceFrameComponent::ContextSensitiveHelp(
    /* [in] */ BOOL fEnterMode)noexcept
{
// Determines whether context-sensitive help mode should be entered during an in-place activation session.
// optional
	return E_NOTIMPL;
}
//---------------------------------------------------------------------------
HRESULT STDMETHODCALLTYPE cOleInPlaceFrameComponent::GetBorder(
    /* [out] */ __RPC__out LPRECT lprectBorder)noexcept
{
// Retrieves the outer rectange for toolbars and controls while the object is active in place.
// optional : Necessary for containers with toolbar UI 
	if(ToolForm==nullptr){
		return INPLACE_E_NOTOOLSPACE;
	}
	if(ToolForm->GetToolRect(*lprectBorder)==false){
		return INPLACE_E_NOTOOLSPACE;
	}
	return S_OK;
}
//---------------------------------------------------------------------------
HRESULT STDMETHODCALLTYPE cOleInPlaceFrameComponent::RequestBorderSpace(
    /* [unique][in] */ __RPC__in_opt LPCBORDERWIDTHS pborderwidths)noexcept
{
// Determines whether there is space available for tools to be installed around the object's window frame while the object is active in place.
// optional : Necessary for containers with toolbar UI 
	if(ToolForm==nullptr){
		return INPLACE_E_NOTOOLSPACE;
	}
	if(ToolForm->CheckToolSpace(*pborderwidths)==false)
		return INPLACE_E_NOTOOLSPACE;
	return S_OK;
}
//---------------------------------------------------------------------------
HRESULT STDMETHODCALLTYPE cOleInPlaceFrameComponent::SetBorderSpace(
    /* [unique][in] */ __RPC__in_opt LPCBORDERWIDTHS pborderwidths)noexcept
{
// Allocates space for the border requested in the call to IOleInPlaceUIWindow::RequestBorderSpace.
// optional : Necessary for containers with toolbar UI 
	if(pborderwidths==nullptr){
		if(ToolForm!=nullptr)
			ToolForm->RestoreToolSpace();
		return S_OK;
	}
	
	if(ToolForm==nullptr){
		return OLE_E_INVALIDRECT;
	}
	if(ToolForm->SetToolSpace(*pborderwidths)==false)
		return OLE_E_INVALIDRECT;
	return S_OK;
}
//---------------------------------------------------------------------------
HRESULT STDMETHODCALLTYPE cOleInPlaceFrameComponent::SetActiveObject(
    /* [unique][in] */ __RPC__in_opt IOleInPlaceActiveObject *pActiveObject,
    /* [unique][string][in] */ __RPC__in_opt_string LPCOLESTR pszObjName)noexcept
{
// Provides a direct channel of communication between the object and each of the frame and document windows.
// optional
	return E_NOTIMPL;
}
//---------------------------------------------------------------------------
HRESULT STDMETHODCALLTYPE cOleInPlaceFrameComponent::InsertMenus( 
    /* [in] */ __RPC__in HMENU hmenuShared,
    /* [out][in] */ __RPC__inout LPOLEMENUGROUPWIDTHS lpMenuWidths)noexcept
{
// Enables the container to insert menu groups into the composite menu to be used during the in-place session.
// optional : Necessary for containers with menu UI
	return E_NOTIMPL;
}
//---------------------------------------------------------------------------
HRESULT STDMETHODCALLTYPE cOleInPlaceFrameComponent::SetMenu( 
    /* [in] */ __RPC__in HMENU hmenuShared,
    /* [in] */ __RPC__in HOLEMENU holemenu,
    /* [in] */ __RPC__in HWND hwndActiveObject)noexcept
{
// Adds a composite menu to the window frame containing the object being activated in place.
// optional : Necessary for containers with menu UI 
	//return E_NOTIMPL;
	return S_OK;
}
//---------------------------------------------------------------------------
HRESULT STDMETHODCALLTYPE cOleInPlaceFrameComponent::RemoveMenus( 
    /* [in] */ __RPC__in HMENU hmenuShared)noexcept
{
// Removes a container's menu elements from the composite menu.
// optional : Necessary for containers with menu UI 
//	return E_NOTIMPL;
	return S_OK;
}
//---------------------------------------------------------------------------
HRESULT STDMETHODCALLTYPE cOleInPlaceFrameComponent::SetStatusText( 
    /* [unique][in] */ __RPC__in_opt LPCOLESTR pszStatusText)noexcept
{
// Sets and displays status text about the in-place object in the container's frame window status line.
// optional : Necessary only for containers that have a status line
	return S_OK;
}
//---------------------------------------------------------------------------
HRESULT STDMETHODCALLTYPE cOleInPlaceFrameComponent::EnableModeless( 
    /* [in] */ BOOL fEnable)noexcept
{
// Enables or disables a frame's modeless dialog boxes.
// optional
	return E_NOTIMPL;
}
//---------------------------------------------------------------------------
HRESULT STDMETHODCALLTYPE cOleInPlaceFrameComponent::TranslateAccelerator( 
    /* [in] */ __RPC__in LPMSG lpmsg,
    /* [in] */ WORD wID)noexcept
{
// Translates accelerator keystrokes intended for the container's frame while an object is active in place.
// optional
	return E_NOTIMPL;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
IUnknown* bCOMOleClientSite::COMUnknown(void)noexcept
{
	return this;
}
//---------------------------------------------------------------------------
bool bCOMOleClientSite::COMQueryInterface(REFIID riid,_COM_Outptr_ void __RPC_FAR *__RPC_FAR *ppvObject)noexcept
{
	if(riid==IID_IOleClientSite){
		*ppvObject=static_cast<IOleClientSite*>(this);
		return true;
	}
	return false;
}
//---------------------------------------------------------------------------
HRESULT STDMETHODCALLTYPE bCOMOleClientSite::SaveObject(void)noexcept
{
	return S_OK;
}
//---------------------------------------------------------------------------
HRESULT STDMETHODCALLTYPE bCOMOleClientSite::GetMoniker( 
    /* [in] */ DWORD dwAssign,
    /* [in] */ DWORD dwWhichMoniker,
    /* [out] */ __RPC__deref_out_opt IMoniker **ppmk)noexcept
{
	if(ppmk==nullptr){
		return E_POINTER;
	}
	*ppmk=nullptr;
	return E_FAIL;
}
//---------------------------------------------------------------------------
HRESULT STDMETHODCALLTYPE bCOMOleClientSite::GetContainer( 
    /* [out] */ __RPC__deref_out_opt IOleContainer **ppContainer)noexcept
{
	*ppContainer=nullptr;
	return E_NOINTERFACE;
}
//---------------------------------------------------------------------------
HRESULT STDMETHODCALLTYPE bCOMOleClientSite::ShowObject(void)noexcept
{
	return S_OK;
}
//---------------------------------------------------------------------------
HRESULT STDMETHODCALLTYPE bCOMOleClientSite::OnShowWindow( 
    /* [in] */ BOOL fShow)noexcept
{
	return S_OK;
}
//---------------------------------------------------------------------------
HRESULT STDMETHODCALLTYPE bCOMOleClientSite::RequestNewObjectLayout(void)noexcept
{
	return E_NOTIMPL;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
IUnknown* bCOMAdviseSink::COMUnknown(void)noexcept
{
	return this;
}
//---------------------------------------------------------------------------
bool bCOMAdviseSink::COMQueryInterface(REFIID riid,_COM_Outptr_ void __RPC_FAR *__RPC_FAR *ppvObject)noexcept
{
	if(riid==IID_IAdviseSink){
		*ppvObject=static_cast<IAdviseSink*>(this);
		return true;
	}
	return false;
}
//---------------------------------------------------------------------------
void STDMETHODCALLTYPE bCOMAdviseSink::OnDataChange( 
    /* [annotation][unique][in] */ 
    _In_  FORMATETC *pFormatetc,
    /* [annotation][unique][in] */ 
    _In_  STGMEDIUM *pStgmed)noexcept
{
}
//---------------------------------------------------------------------------
void STDMETHODCALLTYPE bCOMAdviseSink::OnViewChange( 
    /* [in] */ DWORD dwAspect,
	/* [in] */ LONG lindex)noexcept
{
}
//---------------------------------------------------------------------------
void STDMETHODCALLTYPE bCOMAdviseSink::OnRename( 
    /* [annotation][in] */ 
	_In_  IMoniker *pmk)noexcept
{
}
//---------------------------------------------------------------------------
void STDMETHODCALLTYPE bCOMAdviseSink::OnSave(void)noexcept
{
}
//---------------------------------------------------------------------------
void STDMETHODCALLTYPE bCOMAdviseSink::OnClose(void)noexcept
{
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
IUnknown* bCOMOleControlSite::COMUnknown(void)noexcept
{
	return this;
}
//---------------------------------------------------------------------------
bool bCOMOleControlSite::COMQueryInterface(REFIID riid,_COM_Outptr_ void __RPC_FAR *__RPC_FAR *ppvObject)noexcept
{
	if(riid==IID_IOleControlSite){
		*ppvObject=static_cast<IOleControlSite*>(this);
		return true;
	}
	return false;
}
//---------------------------------------------------------------------------
HRESULT STDMETHODCALLTYPE bCOMOleControlSite::OnControlInfoChanged(void)noexcept
{
	return S_OK;
}
//---------------------------------------------------------------------------
HRESULT STDMETHODCALLTYPE bCOMOleControlSite::LockInPlaceActive(
	/* [in] */ BOOL fLock)noexcept
{
	return E_NOTIMPL;
}
//---------------------------------------------------------------------------
HRESULT STDMETHODCALLTYPE bCOMOleControlSite::GetExtendedControl(
    /* [out] */ __RPC__deref_out_opt IDispatch **ppDisp)noexcept
{
	return E_NOTIMPL;
}
//---------------------------------------------------------------------------
HRESULT STDMETHODCALLTYPE bCOMOleControlSite::TransformCoords(
    /* [out][in] */ __RPC__inout POINTL *pPtlHimetric,
    /* [out][in] */ __RPC__inout POINTF *pPtfContainer,
    /* [in] */ DWORD dwFlags)noexcept
{
	HRESULT hr = S_OK;

	auto Device=cnWindows::QueryDCPaintDevice();
	auto ScreenIC=Device->GetScreenInfomationDC();
	HDC DC=ScreenIC->GetDC();
	SIZE sz;

	if (dwFlags & XFORMCOORDS_HIMETRICTOCONTAINER)
	{
		sz.cx = pPtlHimetric->x;
		sz.cy = pPtlHimetric->y;
		sz=OLE_HIMETRICtoDP(DC,sz);

		if (dwFlags & XFORMCOORDS_SIZE)
		{
			pPtfContainer->x = (float)abs(sz.cx);
			pPtfContainer->y = (float)abs(sz.cy);
		}
		else if (dwFlags & XFORMCOORDS_POSITION)
		{
			pPtfContainer->x = (float)sz.cx;
			pPtfContainer->y = (float)sz.cy;
		}
		else
		{
			hr = E_INVALIDARG;
		}
	}
	else if (dwFlags & XFORMCOORDS_CONTAINERTOHIMETRIC)
	{
		sz.cx = (int)(pPtfContainer->x);
		sz.cy = (int)(pPtfContainer->y);
		sz=OLE_DPtoHIMETRIC(DC,sz);

		if (dwFlags & XFORMCOORDS_SIZE)
		{
			pPtlHimetric->x = abs(sz.cx);
			pPtlHimetric->y = abs(sz.cy);
		}
		else if (dwFlags & XFORMCOORDS_POSITION)
		{
			pPtlHimetric->x = sz.cx;
			pPtlHimetric->y = sz.cy;
		}
		else
		{
			hr = E_INVALIDARG;
		}
	}
	else
	{
		hr = E_INVALIDARG;
	}
	return hr;
}
//---------------------------------------------------------------------------
HRESULT STDMETHODCALLTYPE bCOMOleControlSite::TranslateAccelerator(
    /* [in] */ __RPC__in MSG *pMsg,
    /* [in] */ DWORD grfModifiers)noexcept
{
	return E_NOTIMPL;
}
//---------------------------------------------------------------------------
HRESULT STDMETHODCALLTYPE bCOMOleControlSite::OnFocus(
    /* [in] */ BOOL fGotFocus)noexcept
{
	return S_OK;
}
//---------------------------------------------------------------------------
HRESULT STDMETHODCALLTYPE bCOMOleControlSite::ShowPropertyFrame(void)noexcept
{
	return E_NOTIMPL;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
IUnknown* bCOMDispatch::COMUnknown(void)noexcept
{
	return this;
}
//---------------------------------------------------------------------------
bool bCOMDispatch::COMQueryInterface(REFIID riid,_COM_Outptr_ void __RPC_FAR *__RPC_FAR *ppvObject)noexcept
{
	if(riid==IID_IDispatch){
		*ppvObject=static_cast<IDispatch*>(this);
		return true;
	}
	return false;
}
//---------------------------------------------------------------------------
HRESULT STDMETHODCALLTYPE bCOMDispatch::GetTypeInfoCount( 
    /* [out] */ __RPC__out UINT *pctinfo)noexcept
{
	*pctinfo=0;
	return S_OK;
}
//---------------------------------------------------------------------------
HRESULT STDMETHODCALLTYPE bCOMDispatch::GetTypeInfo( 
    /* [in] */ UINT iTInfo,
    /* [in] */ LCID lcid,
    /* [out] */ __RPC__deref_out_opt ITypeInfo **ppTInfo)noexcept
{
	return E_NOTIMPL;
}
//---------------------------------------------------------------------------
HRESULT STDMETHODCALLTYPE bCOMDispatch::GetIDsOfNames( 
    /* [in] */ __RPC__in REFIID riid,
    /* [size_is][in] */ __RPC__in_ecount_full(cNames) LPOLESTR *rgszNames,
    /* [range][in] */ __RPC__in_range(0,16384) UINT cNames,
    /* [in] */ LCID lcid,
    /* [size_is][out] */ __RPC__out_ecount_full(cNames) DISPID *rgDispId)noexcept
{
	return DISP_E_UNKNOWNNAME;
}
//---------------------------------------------------------------------------
HRESULT STDMETHODCALLTYPE bCOMDispatch::Invoke( 
    /* [annotation][in] */ 
    _In_  DISPID dispIdMember,
    /* [annotation][in] */ 
    _In_  REFIID riid,
    /* [annotation][in] */ 
    _In_  LCID lcid,
    /* [annotation][in] */ 
    _In_  WORD wFlags,
    /* [annotation][out][in] */ 
    _In_  DISPPARAMS *pDispParams,
    /* [annotation][out] */ 
    _Out_opt_  VARIANT *pVarResult,
    /* [annotation][out] */ 
    _Out_opt_  EXCEPINFO *pExcepInfo,
    /* [annotation][out] */ 
    _Out_opt_  UINT *puArgErr)noexcept
{
	return DISP_E_MEMBERNOTFOUND;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cOLEInPlaceSite::cOLEInPlaceSite()noexcept
{
}
//---------------------------------------------------------------------------
cOLEInPlaceSite::~cOLEInPlaceSite()noexcept
{
}
//---------------------------------------------------------------------------
IUnknown* cOLEInPlaceSite::COMUnknown(void)noexcept
{
	return this;
}
//---------------------------------------------------------------------------
bool cOLEInPlaceSite::COMQueryInterface(REFIID riid,_COM_Outptr_ void __RPC_FAR *__RPC_FAR *ppvObject)noexcept
{
	if(riid==IID_IOleWindow){
		*ppvObject=static_cast<IOleWindow*>(this);
		return true;
	}
	if(riid==IID_IOleInPlaceSite){
		*ppvObject=static_cast<IOleInPlaceSite*>(this);
		return true;
	}
	return false;
}
//---------------------------------------------------------------------------
HRESULT STDMETHODCALLTYPE cOLEInPlaceSite::GetWindow(
    /* [out] */ __RPC__deref_out_opt HWND *phwnd)noexcept
{
	if(phwnd==nullptr){
		return E_POINTER;
	}
	HWND WindowHandle;
	WindowHandle=InPlaceControl->OLEGetWindowHandle();
	*phwnd=WindowHandle;

	return WindowHandle!=nullptr ? S_OK : E_NOINTERFACE;
}
//---------------------------------------------------------------------------
HRESULT STDMETHODCALLTYPE cOLEInPlaceSite::ContextSensitiveHelp(
    /* [in] */ BOOL fEnterMode)noexcept
{
	return E_NOTIMPL;
}
//---------------------------------------------------------------------------
HRESULT STDMETHODCALLTYPE cOLEInPlaceSite::CanInPlaceActivate(void)noexcept
{
	if(InPlaceControl->OLECanInPlaceActivate()){
		return S_OK;
	}
	return S_FALSE;
}
//---------------------------------------------------------------------------
HRESULT STDMETHODCALLTYPE cOLEInPlaceSite::OnInPlaceActivate(void)noexcept
{
	InPlaceControl->OLEOnInPlaceActivate();
	return S_OK;
}
//---------------------------------------------------------------------------
HRESULT STDMETHODCALLTYPE cOLEInPlaceSite::OnUIActivate(void)noexcept
{
	InPlaceControl->OLEOnUIActivate();
	return S_OK;
}
//---------------------------------------------------------------------------
HRESULT STDMETHODCALLTYPE cOLEInPlaceSite::GetWindowContext(
    /* [out] */ __RPC__deref_out_opt IOleInPlaceFrame **ppFrame,
    /* [out] */ __RPC__deref_out_opt IOleInPlaceUIWindow **ppDoc,
    /* [out] */ __RPC__out LPRECT lprcPosRect,
    /* [out] */ __RPC__out LPRECT lprcClipRect,
    /* [out][in] */ __RPC__inout LPOLEINPLACEFRAMEINFO lpFrameInfo)noexcept
{
	return InPlaceControl->OLEGetWindowContext(ppFrame,ppDoc,lprcPosRect,lprcClipRect,lpFrameInfo);
}
//---------------------------------------------------------------------------
HRESULT STDMETHODCALLTYPE cOLEInPlaceSite::Scroll(
    /* [in] */ SIZE scrollExtant)noexcept
{
	return E_NOTIMPL;
}
//---------------------------------------------------------------------------
HRESULT STDMETHODCALLTYPE cOLEInPlaceSite::OnUIDeactivate(
    /* [in] */ BOOL fUndoable)noexcept
{fUndoable;
	InPlaceControl->OLEOnUIDeactivate();
	return S_OK;
}
//---------------------------------------------------------------------------
HRESULT STDMETHODCALLTYPE cOLEInPlaceSite::OnInPlaceDeactivate(void)noexcept
{
	InPlaceControl->OLEOnInPlaceDeactivate();
	return S_OK;
}
//---------------------------------------------------------------------------
HRESULT STDMETHODCALLTYPE cOLEInPlaceSite::DiscardUndoState(void)noexcept
{
	return S_OK;
}
//---------------------------------------------------------------------------
HRESULT STDMETHODCALLTYPE cOLEInPlaceSite::DeactivateAndUndo(void)noexcept
{
	return S_OK;
}
//---------------------------------------------------------------------------
HRESULT STDMETHODCALLTYPE cOLEInPlaceSite::OnPosRectChange(
    /* [in] */ __RPC__in LPCRECT lprcPosRect)noexcept
{
	InPlaceControl->OLEOnPosRectChange(lprcPosRect);
	return S_OK;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cOLEDocumentSite::cOLEDocumentSite(cOLEInPlaceControl *Control)noexcept
{
	fInPlaceSite.Outter=COMUnknown();
	fInPlaceSite.InPlaceControl=Control;
}
//---------------------------------------------------------------------------
cOLEDocumentSite::~cOLEDocumentSite()noexcept
{
}
//---------------------------------------------------------------------------
IUnknown* cOLEDocumentSite::COMUnknown(void)noexcept
{
	return static_cast<IOleClientSite*>(this);
}
//---------------------------------------------------------------------------
bool cOLEDocumentSite::COMQueryInterface(REFIID riid,_COM_Outptr_ void __RPC_FAR *__RPC_FAR *ppvObject)noexcept
{
	if(bCOMOleClientSite::COMQueryInterface(riid,ppvObject)){
		return true;
	}
	if(bCOMAdviseSink::COMQueryInterface(riid,ppvObject)){
		return true;
	}

	if(fInPlaceSite.COMQueryInterface(riid,ppvObject)){
		return true;
	}

	return false;
}
//---------------------------------------------------------------------------
IOleObject* cOLEDocumentSite::GetOleObject(void)noexcept
{
	return fOLEObject;
}
//---------------------------------------------------------------------------
void cOLEDocumentSite::SetOleObject(IOleObject *OLEObject)noexcept
{
	if(fOLEObject!=nullptr){
		fOLEObject->Close(OLECLOSE_NOSAVE);
		fOLEObject->SetClientSite(nullptr);
	}
	fOLEObject=OLEObject;
	if(fOLEObject!=nullptr){
		fOLEObject->SetClientSite(this);
	}
	fInPlaceSite.InPlaceControl->UpdateObjectState();
}
//---------------------------------------------------------------------------
HRESULT STDMETHODCALLTYPE cOLEDocumentSite::cInPlaceSite::CanInPlaceActivate(void)noexcept
{
	auto Host=cnMemory::GetObjectFromMemberPointer(static_cast<COMInnerObject<cInPlaceSite>*>(this),&cOLEDocumentSite::fInPlaceSite);
	COMPtr<IOleInPlaceObject> InPlaceObject;
	Host->fOLEObject->QueryInterface(COMRetPtrT(InPlaceObject));
	if(InPlaceObject!=nullptr){
		InPlaceControl->OLESetInPlaceObject(cnVar::MoveCast(InPlaceObject));
	}

	if(InPlaceControl->OLECanInPlaceActivate()==false){
		return S_FALSE;
	}

	return S_OK;
}
//---------------------------------------------------------------------------
bool cOLEDocumentSite::OLEOpen(LONG iVerb)noexcept
{
	HWND HostWindow=fInPlaceSite.InPlaceControl->OLEGetWindowHandle();
	RECT RectInHost=fInPlaceSite.InPlaceControl->OLEGetInPlaceRect();

	HRESULT hr=fOLEObject->DoVerb(iVerb,nullptr,this,0,HostWindow,&RectInHost);
	return SUCCEEDED(hr);
}
//---------------------------------------------------------------------------
void cOLEDocumentSite::OLEClose(DWORD dwSaveOption)noexcept
{
	fOLEObject->Close(dwSaveOption);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
bcOLEActiveXControlSite::bcOLEActiveXControlSite()noexcept
{
}
//---------------------------------------------------------------------------
bcOLEActiveXControlSite::~bcOLEActiveXControlSite()noexcept
{
}
//---------------------------------------------------------------------------
IUnknown* bcOLEActiveXControlSite::COMUnknown(void)noexcept
{
	return static_cast<IOleClientSite*>(this);
}
//---------------------------------------------------------------------------
bool bcOLEActiveXControlSite::COMQueryInterface(REFIID riid,_COM_Outptr_ void __RPC_FAR *__RPC_FAR *ppvObject)noexcept
{
	if(bCOMOleClientSite::COMQueryInterface(riid,ppvObject)){
		return true;
	}
	if(bCOMOleControlSite::COMQueryInterface(riid,ppvObject)){
		return true;
	}
	if(bCOMDispatch::COMQueryInterface(riid,ppvObject)){
		return true;
	}
	
	return false;
}
//---------------------------------------------------------------------------
IOleObject* bcOLEActiveXControlSite::GetOleObject(void)noexcept
{
	return fOLEObject;
}
//---------------------------------------------------------------------------
void bcOLEActiveXControlSite::SetOleObject(IOleObject *OLEObject)noexcept
{
	if(fOLEObject!=nullptr){
		fOLEObject->SetClientSite(nullptr);
	}
	fOLEObject=OLEObject;
	if(fOLEObject!=nullptr){
		fOLEObject->SetClientSite(this);
	}
}
//---------------------------------------------------------------------------
HRESULT STDMETHODCALLTYPE bcOLEActiveXControlSite::Invoke( 
    /* [annotation][in] */ 
    _In_  DISPID dispIdMember,
    /* [annotation][in] */ 
    _In_  REFIID riid,
    /* [annotation][in] */ 
    _In_  LCID lcid,
    /* [annotation][in] */ 
    _In_  WORD wFlags,
    /* [annotation][out][in] */ 
    _In_  DISPPARAMS *pDispParams,
    /* [annotation][out] */ 
    _Out_opt_  VARIANT *pVarResult,
    /* [annotation][out] */ 
    _Out_opt_  EXCEPINFO *pExcepInfo,
    /* [annotation][out] */ 
    _Out_opt_  UINT *puArgErr)noexcept
{
	switch(wFlags){
	case DISPATCH_METHOD:
	case DISPATCH_PROPERTYGET:
	
		switch(dispIdMember){
		case DISPID_AMBIENT_AUTOCLIP:	// 
			pVarResult->vt=VT_BOOL;
			pVarResult->boolVal=VARIANT_TRUE;	// true==auto clip area; false == no clip (control will create additional window!)
			return S_OK;
		//case DISPID_AMBIENT_SUPPORTSMNEMONICS:	// Describes whether the container processes mnemonics or not. A control can do whatever it wants with this information, such as not underline characters it would normally use as a mnemonic.
		case DISPID_AMBIENT_USERMODE:
			pVarResult->vt=VT_BOOL;
			pVarResult->boolVal=VARIANT_TRUE;	// true==Running mode; false == Design mode
			return S_OK;
		case DISPID_AMBIENT_MESSAGEREFLECT:			// Specifies whether the container would like to receive Windows messages such as WM_CTLCOLOR, WM_DRAWITEM, WM_PARENTNOTIFY, and so on as events.
			pVarResult->vt=VT_BOOL;
			pVarResult->boolVal=VARIANT_FALSE;
			return S_OK;
	
		case DISPID_AMBIENT_SHOWGRABHANDLES:
		//	Describes whether a control should show a hatch border or grab handles (in the hatch border) when in-place active.
		//	Controls must obey these properties, giving the container ultimate control over who actually draws these bits of user interface.
		//	A control container may want to draw its own instead of relying on each control, in which case these ambients will always be FALSE.
		case DISPID_AMBIENT_SHOWHATCHING:
		// see DISPID_AMBIENT_SHOWGRABHANDLES
		//	when the control's user interface is active. 
			pVarResult->vt=VT_BOOL;
			pVarResult->boolVal=VARIANT_FALSE;
			return S_OK;

		case DISPID_AMBIENT_UIDEAD:
		//	Describes whether the container is in a mode where controls should ignore user input.
		//	This applies irrespective of UserMode. A container might always set UIDead to TRUE in design mode, and may set it to TRUE when it has hit a breakpoint or such during run mode. A control must pay attention to this property.
			pVarResult->vt=VT_BOOL;
			pVarResult->boolVal=VARIANT_FALSE;
			return S_OK;

		case DISPID_AMBIENT_APPEARANCE:     // ambient appearance is 3D
			pVarResult->vt=VT_I2;
			pVarResult->iVal=1;
			return S_OK;
		case DISPID_AMBIENT_BACKCOLOR:
			pVarResult->vt=VT_COLOR;
			pVarResult->lVal=0xFF00;
			return S_OK;
		case DISPID_AMBIENT_FORECOLOR:
			pVarResult->vt=VT_COLOR;
			pVarResult->lVal=0xFF0000;
			return S_OK;

		//case DISPID_AMBIENT_FONT:
			//if (m_pOleFont == NULL)         // ambient font not initialized
			//	CreateOleFont(m_pWnd->GetFont());
			//
			//ASSERT(m_pOleFont != NULL);
			//if (m_pOleFont == NULL)         // failed to create font
			//	return FALSE;
			//pVarResult->vt=VT_FONT;
			//m_pOleFont->AddRef();
			//pVarResult->pdispVal=m_pOleFont;
			//return S_OK;

		case DISPID_AMBIENT_DISPLAYASDEFAULT:	//	The container will expose a TRUE for this property through whatever site contains what is marked as the default button when the button control should draw itself with a thicker default frame.
			pVarResult->vt=VT_BOOL;
			pVarResult->boolVal=VARIANT_FALSE;
			return S_OK;

		case DISPID_AMBIENT_LOCALEID:
			pVarResult->vt=VT_I4;
			pVarResult->lVal=GetThreadLocale();
			return S_OK;

		case DISPID_AMBIENT_DISPLAYNAME:
		{
			pVarResult->vt=VT_BSTR;
			pVarResult->bstrVal=SysAllocString(L"");
			return S_OK;
		}

		//case DISPID_AMBIENT_SCALEUNITS:
		//	{
		//		CString str;
		//		str.LoadString(AFX_IDS_OCC_SCALEUNITS_PIXELS);
		//		V_VT(pvarResult) = VT_BSTR;
		//		V_BSTR(pvarResult) = str.AllocSysString();
		//	}
		//	return S_OK;
		
		case DISPID_AMBIENT_SUPPORTSMNEMONICS:
			pVarResult->vt=VT_BOOL;
			pVarResult->boolVal=VARIANT_FALSE;
			return S_OK;
		}
	}
	return DISP_E_MEMBERNOTFOUND;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cOLEActiveXControlSite::cOLEActiveXControlSite(cOLEInPlaceControl *Control)noexcept
{
	fInPlaceSite.Outter=COMUnknown();
	fInPlaceSite.InPlaceControl=Control;
	Control->OLEActivation=this;
}
//---------------------------------------------------------------------------
cOLEActiveXControlSite::~cOLEActiveXControlSite()noexcept
{
}
//---------------------------------------------------------------------------
bool cOLEActiveXControlSite::COMQueryInterface(REFIID riid,_COM_Outptr_ void __RPC_FAR *__RPC_FAR *ppvObject)noexcept
{
	if(bcOLEActiveXControlSite::COMQueryInterface(riid,ppvObject)){
		return true;
	}

	if(fInPlaceSite.COMQueryInterface(riid,ppvObject)){
		return true;
	}
	
	return false;
}
//---------------------------------------------------------------------------
void cOLEActiveXControlSite::SetOleObject(IOleObject *OLEObject)noexcept
{
	fInPlaceSite.InPlaceControl->OLESetInPlaceObject(nullptr);
	bcOLEActiveXControlSite::SetOleObject(OLEObject);
	fInPlaceSite.InPlaceControl->UpdateObjectState();
}
//---------------------------------------------------------------------------
bool cOLEActiveXControlSite::OLEInPlaceActive(void)noexcept
{
	return OLEVerb(OLEIVERB_INPLACEACTIVATE);
}
//---------------------------------------------------------------------------
bool cOLEActiveXControlSite::OLEUIActive(void)noexcept
{
	return OLEVerb(OLEIVERB_UIACTIVATE);
}
//---------------------------------------------------------------------------
bool cOLEActiveXControlSite::OLEVerb(int iVerb)noexcept
{
	if(fOLEObject==nullptr)
		return false;
	COMPtr<IOleInPlaceObject> InPlaceObject;
	fOLEObject->QueryInterface(COMRetPtrT(InPlaceObject));
	if(InPlaceObject!=nullptr){
		fInPlaceSite.InPlaceControl->OLESetInPlaceObject(cnVar::MoveCast(InPlaceObject));
	}

	HWND HostWindow=fInPlaceSite.InPlaceControl->OLEGetWindowHandle();
	RECT RectInHost=fInPlaceSite.InPlaceControl->OLEGetInPlaceRect();

	HRESULT hr=fOLEObject->DoVerb(iVerb,nullptr,this,0,HostWindow,&RectInHost);

	return SUCCEEDED(hr);
}
//---------------------------------------------------------------------------
void cOLEActiveXControlSite::SetActiveState(eOLEInPlaceActiveState State)noexcept
{
	fInPlaceSite.InPlaceControl->SetActiveState(State);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cOLEInPlaceSiteWindowless::cOLEInPlaceSiteWindowless()noexcept
{
}
//---------------------------------------------------------------------------
cOLEInPlaceSiteWindowless::~cOLEInPlaceSiteWindowless()noexcept
{
}
//---------------------------------------------------------------------------
IUnknown* cOLEInPlaceSiteWindowless::COMUnknown(void)noexcept
{
	return this;
}
//---------------------------------------------------------------------------
bool cOLEInPlaceSiteWindowless::COMQueryInterface(REFIID riid,_COM_Outptr_ void __RPC_FAR *__RPC_FAR *ppvObject)noexcept
{
	if(riid==IID_IOleWindow){
		*ppvObject=static_cast<IOleWindow*>(this);
		return true;
	}
	if(riid==IID_IOleInPlaceSite){
		*ppvObject=static_cast<IOleInPlaceSite*>(this);
		return true;
	}
	if(riid==IID_IOleInPlaceSiteEx){
		*ppvObject=static_cast<IOleInPlaceSiteEx*>(this);
		return true;
	}
	if(riid==IID_IOleInPlaceSiteWindowless){
		*ppvObject=static_cast<IOleInPlaceSiteWindowless*>(this);
		return true;
	}
	return false;
}
//---------------------------------------------------------------------------
HRESULT STDMETHODCALLTYPE cOLEInPlaceSiteWindowless::GetWindow(
    /* [out] */ __RPC__deref_out_opt HWND *phwnd)noexcept
{
	if(phwnd==nullptr){
		return E_POINTER;
	}
	HWND WindowHandle;
	WindowHandle=InPlaceControl->OLEGetWindowHandle();
	*phwnd=WindowHandle;

	return WindowHandle!=nullptr ? S_OK : E_NOINTERFACE;
}
//---------------------------------------------------------------------------
HRESULT STDMETHODCALLTYPE cOLEInPlaceSiteWindowless::ContextSensitiveHelp(
    /* [in] */ BOOL fEnterMode)noexcept
{
	return E_NOTIMPL;
}
//---------------------------------------------------------------------------
HRESULT STDMETHODCALLTYPE cOLEInPlaceSiteWindowless::CanInPlaceActivate(void)noexcept
{
	if(InPlaceControl->OLECanInPlaceActivate()){
		return S_OK;
	}
	return S_FALSE;
}
//---------------------------------------------------------------------------
HRESULT STDMETHODCALLTYPE cOLEInPlaceSiteWindowless::OnInPlaceActivate(void)noexcept
{
	InPlaceControl->OLEOnInPlaceActivate();
	return S_OK;
}
//---------------------------------------------------------------------------
HRESULT STDMETHODCALLTYPE cOLEInPlaceSiteWindowless::OnUIActivate(void)noexcept
{
	InPlaceControl->OLEOnUIActivate();
	return S_OK;
}
//---------------------------------------------------------------------------
HRESULT STDMETHODCALLTYPE cOLEInPlaceSiteWindowless::GetWindowContext(
    /* [out] */ __RPC__deref_out_opt IOleInPlaceFrame **ppFrame,
    /* [out] */ __RPC__deref_out_opt IOleInPlaceUIWindow **ppDoc,
    /* [out] */ __RPC__out LPRECT lprcPosRect,
    /* [out] */ __RPC__out LPRECT lprcClipRect,
    /* [out][in] */ __RPC__inout LPOLEINPLACEFRAMEINFO lpFrameInfo)noexcept
{
	return InPlaceControl->OLEGetWindowContext(ppFrame,ppDoc,lprcPosRect,lprcClipRect,lpFrameInfo);
}
//---------------------------------------------------------------------------
HRESULT STDMETHODCALLTYPE cOLEInPlaceSiteWindowless::Scroll(
    /* [in] */ SIZE scrollExtant)noexcept
{
	return E_NOTIMPL;
}
//---------------------------------------------------------------------------
HRESULT STDMETHODCALLTYPE cOLEInPlaceSiteWindowless::OnUIDeactivate(
    /* [in] */ BOOL fUndoable)noexcept
{fUndoable;
	InPlaceControl->OLEOnUIDeactivate();
	return S_OK;
}
//---------------------------------------------------------------------------
HRESULT STDMETHODCALLTYPE cOLEInPlaceSiteWindowless::OnInPlaceDeactivate(void)noexcept
{
	InPlaceControl->OLEOnInPlaceDeactivate();
	return S_OK;
}
//---------------------------------------------------------------------------
HRESULT STDMETHODCALLTYPE cOLEInPlaceSiteWindowless::DiscardUndoState(void)noexcept
{
	return S_OK;
}
//---------------------------------------------------------------------------
HRESULT STDMETHODCALLTYPE cOLEInPlaceSiteWindowless::DeactivateAndUndo(void)noexcept
{
	return S_OK;
}
//---------------------------------------------------------------------------
HRESULT STDMETHODCALLTYPE cOLEInPlaceSiteWindowless::OnPosRectChange(
    /* [in] */ __RPC__in LPCRECT lprcPosRect)noexcept
{
	InPlaceControl->OLEOnPosRectChange(lprcPosRect);
	return S_OK;
}
//---------------------------------------------------------------------------
HRESULT STDMETHODCALLTYPE cOLEInPlaceSiteWindowless::OnInPlaceActivateEx(
    /* [out] */ __RPC__out BOOL *pfNoRedraw,
    /* [in] */ DWORD dwFlags)noexcept
{
	InPlaceControl->OLEOnInPlaceActivateEx(pfNoRedraw,dwFlags);
	return S_OK;
}
//---------------------------------------------------------------------------
HRESULT STDMETHODCALLTYPE cOLEInPlaceSiteWindowless::OnInPlaceDeactivateEx(
    /* [in] */ BOOL fNoRedraw)noexcept
{
	InPlaceControl->OLEOnInPlaceDeactivateEx(fNoRedraw);
	return S_OK;
}
//---------------------------------------------------------------------------
HRESULT STDMETHODCALLTYPE cOLEInPlaceSiteWindowless::RequestUIActivate(void)noexcept
{
	if(InPlaceControl->OLERequestUIActivate()==false){
		return S_FALSE;
	}
	return S_OK;
}
//---------------------------------------------------------------------------
HRESULT STDMETHODCALLTYPE cOLEInPlaceSiteWindowless::CanWindowlessActivate(void)noexcept
{
	if(InPlaceControl->OLECanWindowlessActivate()){
		return S_OK;
	}
	return S_FALSE;
}
//---------------------------------------------------------------------------
HRESULT STDMETHODCALLTYPE cOLEInPlaceSiteWindowless::GetCapture(void)noexcept
{
	return InPlaceControl->OLEWindowlessGetCapture();
}
//---------------------------------------------------------------------------
HRESULT STDMETHODCALLTYPE cOLEInPlaceSiteWindowless::SetCapture(
    /* [in] */ BOOL fCapture)noexcept
{
	return InPlaceControl->OLEWindowlessSetCapture(fCapture);
}
//---------------------------------------------------------------------------
HRESULT STDMETHODCALLTYPE cOLEInPlaceSiteWindowless::GetFocus(void)noexcept
{
	return InPlaceControl->OLEWindowlessGetFocus();
}
//---------------------------------------------------------------------------
HRESULT STDMETHODCALLTYPE cOLEInPlaceSiteWindowless::SetFocus( 
    /* [in] */ BOOL fFocus)noexcept
{
	return InPlaceControl->OLEWindowlessSetFocus(fFocus);
}
//---------------------------------------------------------------------------
HRESULT STDMETHODCALLTYPE cOLEInPlaceSiteWindowless::GetDC( 
    /* [unique][in] */ __RPC__in_opt LPCRECT pRect,
    /* [in] */ DWORD grfFlags,
    /* [out] */ __RPC__deref_out_opt HDC *phDC)noexcept
{
	return InPlaceControl->OLEWindowlessGetDC(pRect,grfFlags,phDC);
}
//---------------------------------------------------------------------------
HRESULT STDMETHODCALLTYPE cOLEInPlaceSiteWindowless::ReleaseDC( 
    /* [in] */ __RPC__in HDC hDC)noexcept
{
	return InPlaceControl->OLEWindowlessReleaseDC(hDC);
}
//---------------------------------------------------------------------------
HRESULT STDMETHODCALLTYPE cOLEInPlaceSiteWindowless::InvalidateRect( 
    /* [unique][in] */ __RPC__in_opt LPCRECT pRect,
    /* [in] */ BOOL fErase)noexcept
{
	return InPlaceControl->OLEWindowlessInvalidateRect(pRect,fErase);
}
//---------------------------------------------------------------------------
HRESULT STDMETHODCALLTYPE cOLEInPlaceSiteWindowless::InvalidateRgn( 
    /* [in] */ __RPC__in HRGN hRGN,
    /* [in] */ BOOL fErase)noexcept
{
	return InPlaceControl->OLEWindowlessInvalidateRgn(hRGN,fErase);
}
//---------------------------------------------------------------------------
HRESULT STDMETHODCALLTYPE cOLEInPlaceSiteWindowless::ScrollRect( 
    /* [in] */ INT dx,
    /* [in] */ INT dy,
    /* [in] */ __RPC__in LPCRECT pRectScroll,
    /* [in] */ __RPC__in LPCRECT pRectClip)noexcept
{
	return InPlaceControl->OLEWindowlessScrollRect(dx,dy,pRectScroll,pRectClip);
}
//---------------------------------------------------------------------------
HRESULT STDMETHODCALLTYPE cOLEInPlaceSiteWindowless::AdjustRect( 
    /* [out][in] */ __RPC__inout LPRECT prc)noexcept
{
	return InPlaceControl->OLEWindowlessAdjustRect(prc);
}
//---------------------------------------------------------------------------
HRESULT STDMETHODCALLTYPE cOLEInPlaceSiteWindowless::OnDefWindowMessage( 
    /* [annotation][in] */ 
    _In_  UINT msg,
    /* [annotation][in] */ 
    _In_  WPARAM wParam,
    /* [annotation][in] */ 
    _In_  LPARAM lParam,
    /* [out] */ __RPC__out LRESULT *plResult)noexcept
{
	return InPlaceControl->OLEWindowlessOnDefWindowMessage(msg,wParam,lParam,plResult);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cOLEActiveXControlSiteWindowless::cOLEActiveXControlSiteWindowless(cOLEInPlaceWindowlessControl *Control)noexcept
{
	fInPlaceSite.Outter=COMUnknown();
	fInPlaceSite.InPlaceControl=Control;
	Control->OLEActivation=this;
}
//---------------------------------------------------------------------------
cOLEActiveXControlSiteWindowless::~cOLEActiveXControlSiteWindowless()noexcept
{
}
//---------------------------------------------------------------------------
bool cOLEActiveXControlSiteWindowless::COMQueryInterface(REFIID riid,_COM_Outptr_ void __RPC_FAR *__RPC_FAR *ppvObject)noexcept
{
	if(bcOLEActiveXControlSite::COMQueryInterface(riid,ppvObject)){
		return true;
	}

	if(fInPlaceSite.COMQueryInterface(riid,ppvObject)){
		return true;
	}
	
	return false;
}
//---------------------------------------------------------------------------
bool cOLEActiveXControlSiteWindowless::OLEInPlaceActive(void)noexcept
{
	return OLEVerb(OLEIVERB_INPLACEACTIVATE);
}
//---------------------------------------------------------------------------
bool cOLEActiveXControlSiteWindowless::OLEUIActive(void)noexcept
{
	return OLEVerb(OLEIVERB_UIACTIVATE);
}
//---------------------------------------------------------------------------
bool cOLEActiveXControlSiteWindowless::OLEVerb(int iVerb)noexcept
{
	COMPtr<IOleInPlaceObject> InPlaceObject;
	fOLEObject->QueryInterface(COMRetPtrT(InPlaceObject));
	if(InPlaceObject!=nullptr){
		fInPlaceSite.InPlaceControl->OLESetInPlaceObject(cnVar::MoveCast(InPlaceObject));
	}

	HWND HostWindow=fInPlaceSite.InPlaceControl->OLEGetWindowHandle();
	RECT RectInHost=fInPlaceSite.InPlaceControl->OLEGetInPlaceRect();

	HRESULT hr=fOLEObject->DoVerb(iVerb,nullptr,this,0,HostWindow,&RectInHost);

	return SUCCEEDED(hr);
}
//---------------------------------------------------------------------------
void cOLEActiveXControlSiteWindowless::SetActiveState(eOLEInPlaceActiveState State)noexcept
{
	fInPlaceSite.InPlaceControl->SetActiveState(State);
}
//---------------------------------------------------------------------------
