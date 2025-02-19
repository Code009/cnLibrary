#include "WinUIOLE.h"

using namespace cnLibrary;
using namespace cnUI;

static constexpr int HIMETRIC_INCH=2540;	// HIMETRIC units per inch
//---------------------------------------------------------------------------
SIZE cnUI::OLE_DPtoHIMETRIC(HDC DC,const SIZE &Size)noexcept(true)
{
	int cxPerInch, cyPerInch;
	cxPerInch=::GetDeviceCaps(DC,LOGPIXELSX);
	cyPerInch=::GetDeviceCaps(DC,LOGPIXELSY);

	SIZE RetSize;
	RetSize.cx=MulDiv(Size.cx, HIMETRIC_INCH, cxPerInch);
	RetSize.cy=MulDiv(Size.cy, HIMETRIC_INCH, cyPerInch);
	return RetSize;
}
SIZE cnUI::OLE_HIMETRICtoDP(HDC DC,const SIZE &Size)noexcept(true)
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
RECT cnUI::OLE_DPtoHIMETRIC(HDC DC,const RECT &Rect)noexcept(true)
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
RECT cnUI::OLE_HIMETRICtoDP(HDC DC,const RECT &Rect)noexcept(true)
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
cOLEInPlaceUIForm::cOLEInPlaceUIForm()noexcept(true)
{
	fHostWindowHandle=nullptr;
}
//---------------------------------------------------------------------------
cOLEInPlaceUIForm::~cOLEInPlaceUIForm()noexcept(true)
{
}
//---------------------------------------------------------------------------
void cOLEInPlaceUIForm::UIStarted(void)noexcept(true)
{
	fHostWindowHandle=GetWindowHandleFromUIView(fView);
}
//---------------------------------------------------------------------------
void cOLEInPlaceUIForm::UIStopped(void)noexcept(true)
{
	fHostWindowHandle=nullptr;
}
//---------------------------------------------------------------------------
HWND cOLEInPlaceUIForm::GetWindowHandle(void)noexcept(true)
{
	return fHostWindowHandle;
}
//---------------------------------------------------------------------------
bool cOLEInPlaceUIForm::GetToolRect(RECT &Rect)noexcept(true)
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
bool cOLEInPlaceUIForm::CheckToolSpace(const BORDERWIDTHS &Space)noexcept(true)
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
void cOLEInPlaceUIForm::RestoreToolSpace(void)noexcept(true)
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
bool cOLEInPlaceUIForm::SetToolSpace(const BORDERWIDTHS &Space)noexcept(true)
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
cLayout cOLEInPlaceUIForm::GetClientLayout(void)noexcept(true)
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
cOleInPlaceUIWindowComponent::cOleInPlaceUIWindowComponent()noexcept(true)
{
}
//---------------------------------------------------------------------------
cOleInPlaceUIWindowComponent::~cOleInPlaceUIWindowComponent()noexcept(true)
{
}
//---------------------------------------------------------------------------
HRESULT STDMETHODCALLTYPE cOleInPlaceUIWindowComponent::GetWindow( 
    /* [out] */ __RPC__deref_out_opt HWND *phwnd)noexcept(true)
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
    /* [in] */ BOOL fEnterMode)noexcept(true)
{fEnterMode;
// Determines whether context-sensitive help mode should be entered during an in-place activation session.
// optional
	return E_NOTIMPL;
}
//---------------------------------------------------------------------------
HRESULT STDMETHODCALLTYPE cOleInPlaceUIWindowComponent::GetBorder(
    /* [out] */ __RPC__out LPRECT lprectBorder)noexcept(true)
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
    /* [unique][in] */ __RPC__in_opt LPCBORDERWIDTHS pborderwidths)noexcept(true)
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
    /* [unique][in] */ __RPC__in_opt LPCBORDERWIDTHS pborderwidths)noexcept(true)
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
    /* [unique][string][in] */ __RPC__in_opt_string LPCOLESTR pszObjName)noexcept(true)
{
// Provides a direct channel of communication between the object and each of the frame and document windows.
// optional
	return E_NOTIMPL;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cOleInPlaceFrameComponent::cOleInPlaceFrameComponent()noexcept(true)
{
}
//---------------------------------------------------------------------------
cOleInPlaceFrameComponent::~cOleInPlaceFrameComponent()noexcept(true)
{
}
//---------------------------------------------------------------------------
HRESULT STDMETHODCALLTYPE cOleInPlaceFrameComponent::GetWindow( 
    /* [out] */ __RPC__deref_out_opt HWND *phwnd)noexcept(true)
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
    /* [in] */ BOOL fEnterMode)noexcept(true)
{
// Determines whether context-sensitive help mode should be entered during an in-place activation session.
// optional
	return E_NOTIMPL;
}
//---------------------------------------------------------------------------
HRESULT STDMETHODCALLTYPE cOleInPlaceFrameComponent::GetBorder(
    /* [out] */ __RPC__out LPRECT lprectBorder)noexcept(true)
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
    /* [unique][in] */ __RPC__in_opt LPCBORDERWIDTHS pborderwidths)noexcept(true)
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
    /* [unique][in] */ __RPC__in_opt LPCBORDERWIDTHS pborderwidths)noexcept(true)
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
    /* [unique][string][in] */ __RPC__in_opt_string LPCOLESTR pszObjName)noexcept(true)
{
// Provides a direct channel of communication between the object and each of the frame and document windows.
// optional
	return E_NOTIMPL;
}
//---------------------------------------------------------------------------
HRESULT STDMETHODCALLTYPE cOleInPlaceFrameComponent::InsertMenus( 
    /* [in] */ __RPC__in HMENU hmenuShared,
    /* [out][in] */ __RPC__inout LPOLEMENUGROUPWIDTHS lpMenuWidths)noexcept(true)
{
// Enables the container to insert menu groups into the composite menu to be used during the in-place session.
// optional : Necessary for containers with menu UI
	return E_NOTIMPL;
}
//---------------------------------------------------------------------------
HRESULT STDMETHODCALLTYPE cOleInPlaceFrameComponent::SetMenu( 
    /* [in] */ __RPC__in HMENU hmenuShared,
    /* [in] */ __RPC__in HOLEMENU holemenu,
    /* [in] */ __RPC__in HWND hwndActiveObject)noexcept(true)
{
// Adds a composite menu to the window frame containing the object being activated in place.
// optional : Necessary for containers with menu UI 
	//return E_NOTIMPL;
	return S_OK;
}
//---------------------------------------------------------------------------
HRESULT STDMETHODCALLTYPE cOleInPlaceFrameComponent::RemoveMenus( 
    /* [in] */ __RPC__in HMENU hmenuShared)noexcept(true)
{
// Removes a container's menu elements from the composite menu.
// optional : Necessary for containers with menu UI 
//	return E_NOTIMPL;
	return S_OK;
}
//---------------------------------------------------------------------------
HRESULT STDMETHODCALLTYPE cOleInPlaceFrameComponent::SetStatusText( 
    /* [unique][in] */ __RPC__in_opt LPCOLESTR pszStatusText)noexcept(true)
{
// Sets and displays status text about the in-place object in the container's frame window status line.
// optional : Necessary only for containers that have a status line
	return S_OK;
}
//---------------------------------------------------------------------------
HRESULT STDMETHODCALLTYPE cOleInPlaceFrameComponent::EnableModeless( 
    /* [in] */ BOOL fEnable)noexcept(true)
{
// Enables or disables a frame's modeless dialog boxes.
// optional
	return E_NOTIMPL;
}
//---------------------------------------------------------------------------
HRESULT STDMETHODCALLTYPE cOleInPlaceFrameComponent::TranslateAccelerator( 
    /* [in] */ __RPC__in LPMSG lpmsg,
    /* [in] */ WORD wID)noexcept(true)
{
// Translates accelerator keystrokes intended for the container's frame while an object is active in place.
// optional
	return E_NOTIMPL;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
HRESULT STDMETHODCALLTYPE bCOMOleClientSite::SaveObject(void)noexcept(true)
{
	return S_OK;
}
//---------------------------------------------------------------------------
HRESULT STDMETHODCALLTYPE bCOMOleClientSite::GetMoniker( 
    /* [in] */ DWORD dwAssign,
    /* [in] */ DWORD dwWhichMoniker,
    /* [out] */ __RPC__deref_out_opt IMoniker **ppmk)noexcept(true)
{
	if(ppmk==nullptr){
		return E_POINTER;
	}
	*ppmk=nullptr;
	return E_FAIL;
}
//---------------------------------------------------------------------------
HRESULT STDMETHODCALLTYPE bCOMOleClientSite::GetContainer( 
    /* [out] */ __RPC__deref_out_opt IOleContainer **ppContainer)noexcept(true)
{
	*ppContainer=nullptr;
	return E_NOINTERFACE;
}
//---------------------------------------------------------------------------
HRESULT STDMETHODCALLTYPE bCOMOleClientSite::ShowObject(void)noexcept(true)
{
	return S_OK;
}
//---------------------------------------------------------------------------
HRESULT STDMETHODCALLTYPE bCOMOleClientSite::OnShowWindow( 
    /* [in] */ BOOL fShow)noexcept(true)
{
	return S_OK;
}
//---------------------------------------------------------------------------
HRESULT STDMETHODCALLTYPE bCOMOleClientSite::RequestNewObjectLayout(void)noexcept(true)
{
	return E_NOTIMPL;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void STDMETHODCALLTYPE bCOMAdviseSink::OnDataChange( 
    /* [annotation][unique][in] */ 
    _In_  FORMATETC *pFormatetc,
    /* [annotation][unique][in] */ 
    _In_  STGMEDIUM *pStgmed)noexcept(true)
{
}
//---------------------------------------------------------------------------
void STDMETHODCALLTYPE bCOMAdviseSink::OnViewChange( 
    /* [in] */ DWORD dwAspect,
	/* [in] */ LONG lindex)noexcept(true)
{
}
//---------------------------------------------------------------------------
void STDMETHODCALLTYPE bCOMAdviseSink::OnRename( 
    /* [annotation][in] */ 
	_In_  IMoniker *pmk)noexcept(true)
{
}
//---------------------------------------------------------------------------
void STDMETHODCALLTYPE bCOMAdviseSink::OnSave(void)noexcept(true)
{
}
//---------------------------------------------------------------------------
void STDMETHODCALLTYPE bCOMAdviseSink::OnClose(void)noexcept(true)
{
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
HRESULT STDMETHODCALLTYPE bCOMOleControlSite::OnControlInfoChanged(void)noexcept(true)
{
	return S_OK;
}
//---------------------------------------------------------------------------
HRESULT STDMETHODCALLTYPE bCOMOleControlSite::LockInPlaceActive(
	/* [in] */ BOOL fLock)noexcept(true)
{
	return E_NOTIMPL;
}
//---------------------------------------------------------------------------
HRESULT STDMETHODCALLTYPE bCOMOleControlSite::GetExtendedControl(
    /* [out] */ __RPC__deref_out_opt IDispatch **ppDisp)noexcept(true)
{
	return E_NOTIMPL;
}
//---------------------------------------------------------------------------
HRESULT STDMETHODCALLTYPE bCOMOleControlSite::TransformCoords(
    /* [out][in] */ __RPC__inout POINTL *pPtlHimetric,
    /* [out][in] */ __RPC__inout POINTF *pPtfContainer,
    /* [in] */ DWORD dwFlags)noexcept(true)
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
    /* [in] */ DWORD grfModifiers)noexcept(true)
{
	return E_NOTIMPL;
}
//---------------------------------------------------------------------------
HRESULT STDMETHODCALLTYPE bCOMOleControlSite::OnFocus(
    /* [in] */ BOOL fGotFocus)noexcept(true)
{
	return S_OK;
}
//---------------------------------------------------------------------------
HRESULT STDMETHODCALLTYPE bCOMOleControlSite::ShowPropertyFrame(void)noexcept(true)
{
	return E_NOTIMPL;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
HRESULT STDMETHODCALLTYPE bCOMDispatch::GetTypeInfoCount( 
    /* [out] */ __RPC__out UINT *pctinfo)noexcept(true)
{
	*pctinfo=0;
	return S_OK;
}
//---------------------------------------------------------------------------
HRESULT STDMETHODCALLTYPE bCOMDispatch::GetTypeInfo( 
    /* [in] */ UINT iTInfo,
    /* [in] */ LCID lcid,
    /* [out] */ __RPC__deref_out_opt ITypeInfo **ppTInfo)noexcept(true)
{
	return E_NOTIMPL;
}
//---------------------------------------------------------------------------
HRESULT STDMETHODCALLTYPE bCOMDispatch::GetIDsOfNames( 
    /* [in] */ __RPC__in REFIID riid,
    /* [size_is][in] */ __RPC__in_ecount_full(cNames) LPOLESTR *rgszNames,
    /* [range][in] */ __RPC__in_range(0,16384) UINT cNames,
    /* [in] */ LCID lcid,
    /* [size_is][out] */ __RPC__out_ecount_full(cNames) DISPID *rgDispId)noexcept(true)
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
    _Out_opt_  UINT *puArgErr)noexcept(true)
{
	return DISP_E_MEMBERNOTFOUND;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cOLEInPlaceSite::cOLEInPlaceSite()noexcept(true)
{
}
//---------------------------------------------------------------------------
cOLEInPlaceSite::~cOLEInPlaceSite()noexcept(true)
{
}
//---------------------------------------------------------------------------
HRESULT STDMETHODCALLTYPE cOLEInPlaceSite::GetWindow(
    /* [out] */ __RPC__deref_out_opt HWND *phwnd)noexcept(true)
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
    /* [in] */ BOOL fEnterMode)noexcept(true)
{
	return E_NOTIMPL;
}
//---------------------------------------------------------------------------
HRESULT STDMETHODCALLTYPE cOLEInPlaceSite::CanInPlaceActivate(void)noexcept(true)
{
	if(InPlaceControl->OLECanInPlaceActivate()){
		return S_OK;
	}
	return S_FALSE;
}
//---------------------------------------------------------------------------
HRESULT STDMETHODCALLTYPE cOLEInPlaceSite::OnInPlaceActivate(void)noexcept(true)
{
	InPlaceControl->OLEOnInPlaceActivate();
	return S_OK;
}
//---------------------------------------------------------------------------
HRESULT STDMETHODCALLTYPE cOLEInPlaceSite::OnUIActivate(void)noexcept(true)
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
    /* [out][in] */ __RPC__inout LPOLEINPLACEFRAMEINFO lpFrameInfo)noexcept(true)
{
	return InPlaceControl->OLEGetWindowContext(ppFrame,ppDoc,lprcPosRect,lprcClipRect,lpFrameInfo);
}
//---------------------------------------------------------------------------
HRESULT STDMETHODCALLTYPE cOLEInPlaceSite::Scroll(
    /* [in] */ SIZE scrollExtant)noexcept(true)
{
	return E_NOTIMPL;
}
//---------------------------------------------------------------------------
HRESULT STDMETHODCALLTYPE cOLEInPlaceSite::OnUIDeactivate(
    /* [in] */ BOOL fUndoable)noexcept(true)
{fUndoable;
	InPlaceControl->OLEOnUIDeactivate();
	return S_OK;
}
//---------------------------------------------------------------------------
HRESULT STDMETHODCALLTYPE cOLEInPlaceSite::OnInPlaceDeactivate(void)noexcept(true)
{
	InPlaceControl->OLEOnInPlaceDeactivate();
	return S_OK;
}
//---------------------------------------------------------------------------
HRESULT STDMETHODCALLTYPE cOLEInPlaceSite::DiscardUndoState(void)noexcept(true)
{
	return S_OK;
}
//---------------------------------------------------------------------------
HRESULT STDMETHODCALLTYPE cOLEInPlaceSite::DeactivateAndUndo(void)noexcept(true)
{
	return S_OK;
}
//---------------------------------------------------------------------------
HRESULT STDMETHODCALLTYPE cOLEInPlaceSite::OnPosRectChange(
    /* [in] */ __RPC__in LPCRECT lprcPosRect)noexcept(true)
{
	InPlaceControl->OLEOnPosRectChange(lprcPosRect);
	return S_OK;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cOLEDocumentSite::cOLEDocumentSite(cOLEInPlaceControl *Control)noexcept(true)
{
	fInPlaceSite.Outter=static_cast<IOleClientSite*>(this);
	fInPlaceSite.InPlaceControl=Control;
}
//---------------------------------------------------------------------------
cOLEDocumentSite::~cOLEDocumentSite()noexcept(true)
{
}
//---------------------------------------------------------------------------
bool cOLEDocumentSite::COMQueryInterface(REFIID riid,_COM_Outptr_ void __RPC_FAR *__RPC_FAR *ppvObject)noexcept(true)
{
	if(fInPlaceSite.COMInnerQueryInterface(riid,ppvObject))
		return true;

	return false;
}
//---------------------------------------------------------------------------
IOleObject* cOLEDocumentSite::GetOleObject(void)noexcept(true)
{
	return fOLEObject;
}
//---------------------------------------------------------------------------
void cOLEDocumentSite::SetOleObject(IOleObject *OLEObject)noexcept(true)
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
HRESULT STDMETHODCALLTYPE cOLEDocumentSite::cInPlaceSite::CanInPlaceActivate(void)noexcept(true)
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
bool cOLEDocumentSite::OLEOpen(LONG iVerb)noexcept(true)
{
	HWND HostWindow=fInPlaceSite.InPlaceControl->OLEGetWindowHandle();
	RECT RectInHost=fInPlaceSite.InPlaceControl->OLEGetInPlaceRect();

	HRESULT hr=fOLEObject->DoVerb(iVerb,nullptr,this,0,HostWindow,&RectInHost);
	return SUCCEEDED(hr);
}
//---------------------------------------------------------------------------
void cOLEDocumentSite::OLEClose(DWORD dwSaveOption)noexcept(true)
{
	fOLEObject->Close(dwSaveOption);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
bcOLEActiveXControlSite::bcOLEActiveXControlSite()noexcept(true)
{
}
//---------------------------------------------------------------------------
bcOLEActiveXControlSite::~bcOLEActiveXControlSite()noexcept(true)
{
}
//---------------------------------------------------------------------------
IOleObject* bcOLEActiveXControlSite::GetOleObject(void)noexcept(true)
{
	return fOLEObject;
}
//---------------------------------------------------------------------------
void bcOLEActiveXControlSite::SetOleObject(IOleObject *OLEObject)noexcept(true)
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
    _Out_opt_  UINT *puArgErr)noexcept(true)
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
cOLEActiveXControlSite::cOLEActiveXControlSite(cOLEInPlaceControl *Control)noexcept(true)
{
	fInPlaceSite.Outter=static_cast<IOleClientSite*>(this);
	fInPlaceSite.InPlaceControl=Control;
	Control->OLEActivation=this;
}
//---------------------------------------------------------------------------
cOLEActiveXControlSite::~cOLEActiveXControlSite()noexcept(true)
{
}
//---------------------------------------------------------------------------
bool cOLEActiveXControlSite::COMQueryInterface(REFIID riid,_COM_Outptr_ void __RPC_FAR *__RPC_FAR *ppvObject)noexcept(true)
{
	if(fInPlaceSite.COMInnerQueryInterface(riid,ppvObject))
		return true;

	return false;
}
//---------------------------------------------------------------------------
void cOLEActiveXControlSite::SetOleObject(IOleObject *OLEObject)noexcept(true)
{
	fInPlaceSite.InPlaceControl->OLESetInPlaceObject(nullptr);
	bcOLEActiveXControlSite::SetOleObject(OLEObject);
	fInPlaceSite.InPlaceControl->UpdateObjectState();
}
//---------------------------------------------------------------------------
bool cOLEActiveXControlSite::OLEInPlaceActive(void)noexcept(true)
{
	return OLEVerb(OLEIVERB_INPLACEACTIVATE);
}
//---------------------------------------------------------------------------
bool cOLEActiveXControlSite::OLEUIActive(void)noexcept(true)
{
	return OLEVerb(OLEIVERB_UIACTIVATE);
}
//---------------------------------------------------------------------------
bool cOLEActiveXControlSite::OLEVerb(int iVerb)noexcept(true)
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
void cOLEActiveXControlSite::SetActiveState(eOLEInPlaceActiveState State)noexcept(true)
{
	fInPlaceSite.InPlaceControl->SetActiveState(State);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cOLEInPlaceSiteWindowless::cOLEInPlaceSiteWindowless()noexcept(true)
{
}
//---------------------------------------------------------------------------
cOLEInPlaceSiteWindowless::~cOLEInPlaceSiteWindowless()noexcept(true)
{
}
//---------------------------------------------------------------------------
HRESULT STDMETHODCALLTYPE cOLEInPlaceSiteWindowless::GetWindow(
    /* [out] */ __RPC__deref_out_opt HWND *phwnd)noexcept(true)
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
    /* [in] */ BOOL fEnterMode)noexcept(true)
{
	return E_NOTIMPL;
}
//---------------------------------------------------------------------------
HRESULT STDMETHODCALLTYPE cOLEInPlaceSiteWindowless::CanInPlaceActivate(void)noexcept(true)
{
	if(InPlaceControl->OLECanInPlaceActivate()){
		return S_OK;
	}
	return S_FALSE;
}
//---------------------------------------------------------------------------
HRESULT STDMETHODCALLTYPE cOLEInPlaceSiteWindowless::OnInPlaceActivate(void)noexcept(true)
{
	InPlaceControl->OLEOnInPlaceActivate();
	return S_OK;
}
//---------------------------------------------------------------------------
HRESULT STDMETHODCALLTYPE cOLEInPlaceSiteWindowless::OnUIActivate(void)noexcept(true)
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
    /* [out][in] */ __RPC__inout LPOLEINPLACEFRAMEINFO lpFrameInfo)noexcept(true)
{
	return InPlaceControl->OLEGetWindowContext(ppFrame,ppDoc,lprcPosRect,lprcClipRect,lpFrameInfo);
}
//---------------------------------------------------------------------------
HRESULT STDMETHODCALLTYPE cOLEInPlaceSiteWindowless::Scroll(
    /* [in] */ SIZE scrollExtant)noexcept(true)
{
	return E_NOTIMPL;
}
//---------------------------------------------------------------------------
HRESULT STDMETHODCALLTYPE cOLEInPlaceSiteWindowless::OnUIDeactivate(
    /* [in] */ BOOL fUndoable)noexcept(true)
{fUndoable;
	InPlaceControl->OLEOnUIDeactivate();
	return S_OK;
}
//---------------------------------------------------------------------------
HRESULT STDMETHODCALLTYPE cOLEInPlaceSiteWindowless::OnInPlaceDeactivate(void)noexcept(true)
{
	InPlaceControl->OLEOnInPlaceDeactivate();
	return S_OK;
}
//---------------------------------------------------------------------------
HRESULT STDMETHODCALLTYPE cOLEInPlaceSiteWindowless::DiscardUndoState(void)noexcept(true)
{
	return S_OK;
}
//---------------------------------------------------------------------------
HRESULT STDMETHODCALLTYPE cOLEInPlaceSiteWindowless::DeactivateAndUndo(void)noexcept(true)
{
	return S_OK;
}
//---------------------------------------------------------------------------
HRESULT STDMETHODCALLTYPE cOLEInPlaceSiteWindowless::OnPosRectChange(
    /* [in] */ __RPC__in LPCRECT lprcPosRect)noexcept(true)
{
	InPlaceControl->OLEOnPosRectChange(lprcPosRect);
	return S_OK;
}
//---------------------------------------------------------------------------
HRESULT STDMETHODCALLTYPE cOLEInPlaceSiteWindowless::OnInPlaceActivateEx(
    /* [out] */ __RPC__out BOOL *pfNoRedraw,
    /* [in] */ DWORD dwFlags)noexcept(true)
{
	InPlaceControl->OLEOnInPlaceActivateEx(pfNoRedraw,dwFlags);
	return S_OK;
}
//---------------------------------------------------------------------------
HRESULT STDMETHODCALLTYPE cOLEInPlaceSiteWindowless::OnInPlaceDeactivateEx(
    /* [in] */ BOOL fNoRedraw)noexcept(true)
{
	InPlaceControl->OLEOnInPlaceDeactivateEx(fNoRedraw);
	return S_OK;
}
//---------------------------------------------------------------------------
HRESULT STDMETHODCALLTYPE cOLEInPlaceSiteWindowless::RequestUIActivate(void)noexcept(true)
{
	if(InPlaceControl->OLERequestUIActivate()==false){
		return S_FALSE;
	}
	return S_OK;
}
//---------------------------------------------------------------------------
HRESULT STDMETHODCALLTYPE cOLEInPlaceSiteWindowless::CanWindowlessActivate(void)noexcept(true)
{
	if(InPlaceControl->OLECanWindowlessActivate()){
		return S_OK;
	}
	return S_FALSE;
}
//---------------------------------------------------------------------------
HRESULT STDMETHODCALLTYPE cOLEInPlaceSiteWindowless::GetCapture(void)noexcept(true)
{
	return InPlaceControl->OLEWindowlessGetCapture();
}
//---------------------------------------------------------------------------
HRESULT STDMETHODCALLTYPE cOLEInPlaceSiteWindowless::SetCapture(
    /* [in] */ BOOL fCapture)noexcept(true)
{
	return InPlaceControl->OLEWindowlessSetCapture(fCapture);
}
//---------------------------------------------------------------------------
HRESULT STDMETHODCALLTYPE cOLEInPlaceSiteWindowless::GetFocus(void)noexcept(true)
{
	return InPlaceControl->OLEWindowlessGetFocus();
}
//---------------------------------------------------------------------------
HRESULT STDMETHODCALLTYPE cOLEInPlaceSiteWindowless::SetFocus( 
    /* [in] */ BOOL fFocus)noexcept(true)
{
	return InPlaceControl->OLEWindowlessSetFocus(fFocus);
}
//---------------------------------------------------------------------------
HRESULT STDMETHODCALLTYPE cOLEInPlaceSiteWindowless::GetDC( 
    /* [unique][in] */ __RPC__in_opt LPCRECT pRect,
    /* [in] */ DWORD grfFlags,
    /* [out] */ __RPC__deref_out_opt HDC *phDC)noexcept(true)
{
	return InPlaceControl->OLEWindowlessGetDC(pRect,grfFlags,phDC);
}
//---------------------------------------------------------------------------
HRESULT STDMETHODCALLTYPE cOLEInPlaceSiteWindowless::ReleaseDC( 
    /* [in] */ __RPC__in HDC hDC)noexcept(true)
{
	return InPlaceControl->OLEWindowlessReleaseDC(hDC);
}
//---------------------------------------------------------------------------
HRESULT STDMETHODCALLTYPE cOLEInPlaceSiteWindowless::InvalidateRect( 
    /* [unique][in] */ __RPC__in_opt LPCRECT pRect,
    /* [in] */ BOOL fErase)noexcept(true)
{
	return InPlaceControl->OLEWindowlessInvalidateRect(pRect,fErase);
}
//---------------------------------------------------------------------------
HRESULT STDMETHODCALLTYPE cOLEInPlaceSiteWindowless::InvalidateRgn( 
    /* [in] */ __RPC__in HRGN hRGN,
    /* [in] */ BOOL fErase)noexcept(true)
{
	return InPlaceControl->OLEWindowlessInvalidateRgn(hRGN,fErase);
}
//---------------------------------------------------------------------------
HRESULT STDMETHODCALLTYPE cOLEInPlaceSiteWindowless::ScrollRect( 
    /* [in] */ INT dx,
    /* [in] */ INT dy,
    /* [in] */ __RPC__in LPCRECT pRectScroll,
    /* [in] */ __RPC__in LPCRECT pRectClip)noexcept(true)
{
	return InPlaceControl->OLEWindowlessScrollRect(dx,dy,pRectScroll,pRectClip);
}
//---------------------------------------------------------------------------
HRESULT STDMETHODCALLTYPE cOLEInPlaceSiteWindowless::AdjustRect( 
    /* [out][in] */ __RPC__inout LPRECT prc)noexcept(true)
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
    /* [out] */ __RPC__out LRESULT *plResult)noexcept(true)
{
	return InPlaceControl->OLEWindowlessOnDefWindowMessage(msg,wParam,lParam,plResult);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cOLEActiveXControlSiteWindowless::cOLEActiveXControlSiteWindowless(cOLEInPlaceWindowlessControl *Control)noexcept(true)
{
	fInPlaceSite.Outter=static_cast<IOleClientSite*>(this);
	fInPlaceSite.InPlaceControl=Control;
	Control->OLEActivation=this;
}
//---------------------------------------------------------------------------
cOLEActiveXControlSiteWindowless::~cOLEActiveXControlSiteWindowless()noexcept(true)
{
}
//---------------------------------------------------------------------------
bool cOLEActiveXControlSiteWindowless::COMQueryInterface(REFIID riid,_COM_Outptr_ void __RPC_FAR *__RPC_FAR *ppvObject)noexcept(true)
{
	if(fInPlaceSite.COMInnerQueryInterface(riid,ppvObject)){
		return true;
	}
	
	return false;
}
//---------------------------------------------------------------------------
bool cOLEActiveXControlSiteWindowless::OLEInPlaceActive(void)noexcept(true)
{
	return OLEVerb(OLEIVERB_INPLACEACTIVATE);
}
//---------------------------------------------------------------------------
bool cOLEActiveXControlSiteWindowless::OLEUIActive(void)noexcept(true)
{
	return OLEVerb(OLEIVERB_UIACTIVATE);
}
//---------------------------------------------------------------------------
bool cOLEActiveXControlSiteWindowless::OLEVerb(int iVerb)noexcept(true)
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
void cOLEActiveXControlSiteWindowless::SetActiveState(eOLEInPlaceActiveState State)noexcept(true)
{
	fInPlaceSite.InPlaceControl->SetActiveState(State);
}
//---------------------------------------------------------------------------
