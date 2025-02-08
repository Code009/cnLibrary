/*- cnWinUI - OLE Support -------------------------------------------------*/
/*         Developer : Code009                                             */
/*         Create on : 2007-11-27                                          */
/*-------------------------------------------------------------------------*/
#pragma once
/*-------------------------------------------------------------------------*/

#include <cnSystem\cnWindows.h>
#include <Ole2.h>
#include <OleCtl.h>
//#include <DocObj.h>

#include <cnUI\WinUIBase.h>
#include <cnUI\WinUIOLEInPlace.h>

//---------------------------------------------------------------------------
namespace cnLibrary{
//---------------------------------------------------------------------------
namespace cnUI{
//---------------------------------------------------------------------------
SIZE OLE_DPtoHIMETRIC(HDC DC,const SIZE &Size)noexcept(true);
SIZE OLE_HIMETRICtoDP(HDC DC,const SIZE &Size)noexcept(true);
RECT OLE_DPtoHIMETRIC(HDC DC,const RECT &Rect)noexcept(true);
RECT OLE_HIMETRICtoDP(HDC DC,const RECT &Rect)noexcept(true);
//---------------------------------------------------------------------------
class cOLEInPlaceUIForm : public Form
{
public:
	cOLEInPlaceUIForm()noexcept(true);
	~cOLEInPlaceUIForm()noexcept(true);

	HWND GetWindowHandle(void)noexcept(true);
	bool GetToolRect(RECT &Rect)noexcept(true);
	bool CheckToolSpace(const BORDERWIDTHS &Space)noexcept(true);
	void RestoreToolSpace(void)noexcept(true);
	bool SetToolSpace(const BORDERWIDTHS &Space)noexcept(true);
	
	cLayout GetClientLayout(void)noexcept(true);

	sfInt32 AllowSpaceLeft=0;
	sfInt32 AllowSpaceTop=0;
	sfInt32 AllowSpaceRight=0;
	sfInt32 AllowSpaceBottom=0;
protected:
	sfInt32 fSpaceLeft=0;
	sfInt32 fSpaceTop=0;
	sfInt32 fSpaceRight=0;
	sfInt32 fSpaceBottom=0;

	HWND fHostWindowHandle;

	virtual void cnLib_FUNC UIStarted(void)noexcept(true)override;
	virtual void cnLib_FUNC UIStopped(void)noexcept(true)override;
};
//---------------------------------------------------------------------------
class cOleInPlaceUIWindowComponent : public WindowMessageHandler, public IOleInPlaceUIWindow
{
public:
	cOleInPlaceUIWindowComponent()noexcept(true);
	~cOleInPlaceUIWindowComponent()noexcept(true);

	typedef TCOMInterfacePack<IOleInPlaceUIWindow,IOleWindow> tCOMInterfacePack;

	cOLEInPlaceUIForm *ToolForm=nullptr;


// IOleWindow

    virtual /* [input_sync] */ HRESULT STDMETHODCALLTYPE GetWindow(
        /* [out] */ __RPC__deref_out_opt HWND *phwnd)noexcept(true)override;
        
    virtual HRESULT STDMETHODCALLTYPE ContextSensitiveHelp( 
        /* [in] */ BOOL fEnterMode)noexcept(true)override;


// IOleInPlaceUIWindow

    virtual /* [input_sync] */ HRESULT STDMETHODCALLTYPE GetBorder(
        /* [out] */ __RPC__out LPRECT lprectBorder)noexcept(true)override;
        
    virtual /* [input_sync] */ HRESULT STDMETHODCALLTYPE RequestBorderSpace(
        /* [unique][in] */ __RPC__in_opt LPCBORDERWIDTHS pborderwidths)noexcept(true)override;
        
    virtual /* [input_sync] */ HRESULT STDMETHODCALLTYPE SetBorderSpace(
        /* [unique][in] */ __RPC__in_opt LPCBORDERWIDTHS pborderwidths)noexcept(true)override;
        
    virtual HRESULT STDMETHODCALLTYPE SetActiveObject(
        /* [unique][in] */ __RPC__in_opt IOleInPlaceActiveObject *pActiveObject,
        /* [unique][string][in] */ __RPC__in_opt_string LPCOLESTR pszObjName)noexcept(true)override;

protected:
};
//---------------------------------------------------------------------------
class cOleInPlaceFrameComponent : public WindowControl, public IOleInPlaceFrame
{
public:
	cOleInPlaceFrameComponent()noexcept(true);
	~cOleInPlaceFrameComponent()noexcept(true);

	typedef TCOMInterfacePack<IOleWindow,IOleInPlaceUIWindow,IOleInPlaceFrame> tCOMInterfacePack;

	cOLEInPlaceUIForm *ToolForm=nullptr;


// IOleWindow

    virtual /* [input_sync] */ HRESULT STDMETHODCALLTYPE GetWindow(
        /* [out] */ __RPC__deref_out_opt HWND *phwnd)noexcept(true)override;
        
    virtual HRESULT STDMETHODCALLTYPE ContextSensitiveHelp( 
        /* [in] */ BOOL fEnterMode)noexcept(true)override;


// IOleInPlaceUIWindow

    virtual /* [input_sync] */ HRESULT STDMETHODCALLTYPE GetBorder(
        /* [out] */ __RPC__out LPRECT lprectBorder)noexcept(true)override;
        
    virtual /* [input_sync] */ HRESULT STDMETHODCALLTYPE RequestBorderSpace(
        /* [unique][in] */ __RPC__in_opt LPCBORDERWIDTHS pborderwidths)noexcept(true)override;
        
    virtual /* [input_sync] */ HRESULT STDMETHODCALLTYPE SetBorderSpace(
        /* [unique][in] */ __RPC__in_opt LPCBORDERWIDTHS pborderwidths)noexcept(true)override;
        
    virtual HRESULT STDMETHODCALLTYPE SetActiveObject(
        /* [unique][in] */ __RPC__in_opt IOleInPlaceActiveObject *pActiveObject,
        /* [unique][string][in] */ __RPC__in_opt_string LPCOLESTR pszObjName)noexcept(true)override;

// IOleInPlaceFrame

	virtual HRESULT STDMETHODCALLTYPE InsertMenus(
        /* [in] */ __RPC__in HMENU hmenuShared,
        /* [out][in] */ __RPC__inout LPOLEMENUGROUPWIDTHS lpMenuWidths)noexcept(true)override;
        
    virtual /* [input_sync] */ HRESULT STDMETHODCALLTYPE SetMenu(
        /* [in] */ __RPC__in HMENU hmenuShared,
        /* [in] */ __RPC__in HOLEMENU holemenu,
        /* [in] */ __RPC__in HWND hwndActiveObject)noexcept(true)override;
        
    virtual HRESULT STDMETHODCALLTYPE RemoveMenus(
        /* [in] */ __RPC__in HMENU hmenuShared)noexcept(true)override;
        
    virtual /* [input_sync] */ HRESULT STDMETHODCALLTYPE SetStatusText(
        /* [unique][in] */ __RPC__in_opt LPCOLESTR pszStatusText)noexcept(true)override;
        
    virtual HRESULT STDMETHODCALLTYPE EnableModeless(
        /* [in] */ BOOL fEnable)noexcept(true)override;
        
    virtual HRESULT STDMETHODCALLTYPE TranslateAccelerator(
        /* [in] */ __RPC__in LPMSG lpmsg,
        /* [in] */ WORD wID)noexcept(true)override;

protected:
};
//---------------------------------------------------------------------------
class bCOMOleClientSite : public IOleClientSite
{
public:
	
	typedef TCOMInterfacePack<IOleClientSite> tCOMInterfacePack;

// IOleClientSite

	// SaveObject
	// Saves the embedded object associated with the client site.
	// This function is synchronous; by the time it returns, the save will be completed.
	virtual HRESULT STDMETHODCALLTYPE SaveObject(void)noexcept(true);

	// GetMoniker
	// Retrieves a moniker for the object's client site
	virtual HRESULT STDMETHODCALLTYPE GetMoniker(
		/* [in] */ DWORD dwAssign,
		/* [in] */ DWORD dwWhichMoniker,
		/* [out] */ __RPC__deref_out_opt IMoniker **ppmk)noexcept(true)override;

	// GetContainer
	// Retrieves a pointer to the object's container.
	//	This method is optional but recommended for all containers that expect to support links to their embedded objects.
	//	Simple containers that do not support links to their embedded objects probably do not need to implement this method. Instead, they can return E_NOINTERFACE and set ppContainer to NULL.
	virtual HRESULT STDMETHODCALLTYPE GetContainer(
		/* [out] */ __RPC__deref_out_opt IOleContainer **ppContainer)noexcept(true)override;

	// ShowObject
	// Asks a container to display its object to the user. This method ensures that the container itself is visible and not minimized.
	virtual HRESULT STDMETHODCALLTYPE ShowObject(void)noexcept(true)override;

	// OnShowWindow
	// Notifies a container when an embedded object's window is about to become visible or invisible.
	// called when object created its own window
	virtual HRESULT STDMETHODCALLTYPE OnShowWindow( /* [in] */ BOOL fShow)noexcept(true)override;

	// RequestNewObjectLayout
	// Asks a container to resize the display site for embedded objects.
	//	Responding to this method will be optional for containers.
	virtual HRESULT STDMETHODCALLTYPE RequestNewObjectLayout(void)noexcept(true)override;

};
//---------------------------------------------------------------------------
class bCOMAdviseSink : public IAdviseSink
{
public:

	typedef TCOMInterfacePack<IAdviseSink> tCOMInterfacePack;

// IAdviseSink

    virtual /* [local] */ void STDMETHODCALLTYPE OnDataChange( 
        /* [annotation][unique][in] */ 
        _In_  FORMATETC *pFormatetc,
        /* [annotation][unique][in] */ 
        _In_  STGMEDIUM *pStgmed)noexcept(true)override;
        
    virtual /* [local] */ void STDMETHODCALLTYPE OnViewChange( 
        /* [in] */ DWORD dwAspect,
		/* [in] */ LONG lindex)noexcept(true)override;
        
    virtual /* [local] */ void STDMETHODCALLTYPE OnRename( 
        /* [annotation][in] */ 
		_In_  IMoniker *pmk)noexcept(true)override;
        
    virtual /* [local] */ void STDMETHODCALLTYPE OnSave( void)noexcept(true)override;
        
    virtual /* [local] */ void STDMETHODCALLTYPE OnClose( void)noexcept(true)override;

};
//---------------------------------------------------------------------------
class bCOMOleControlSite : public IOleControlSite
{
public:
	
	typedef TCOMInterfacePack<IOleControlSite> tCOMInterfacePack;

	// IOleControlSite
 
	virtual HRESULT STDMETHODCALLTYPE OnControlInfoChanged(void)noexcept(true)override;
        
    virtual HRESULT STDMETHODCALLTYPE LockInPlaceActive(
        /* [in] */ BOOL fLock)noexcept(true)override;
        
    virtual HRESULT STDMETHODCALLTYPE GetExtendedControl(
        /* [out] */ __RPC__deref_out_opt IDispatch **ppDisp)noexcept(true)override;
        
    virtual HRESULT STDMETHODCALLTYPE TransformCoords(
        /* [out][in] */ __RPC__inout POINTL *pPtlHimetric,
        /* [out][in] */ __RPC__inout POINTF *pPtfContainer,
        /* [in] */ DWORD dwFlags)noexcept(true)override;
        
    virtual HRESULT STDMETHODCALLTYPE TranslateAccelerator(
        /* [in] */ __RPC__in MSG *pMsg,
        /* [in] */ DWORD grfModifiers)noexcept(true)override;
        
    virtual HRESULT STDMETHODCALLTYPE OnFocus(
        /* [in] */ BOOL fGotFocus)noexcept(true)override;
        
    virtual HRESULT STDMETHODCALLTYPE ShowPropertyFrame(void)noexcept(true)override;
};
//---------------------------------------------------------------------------
class bCOMDispatch : public IDispatch
{
public:

	typedef TCOMInterfacePack<IDispatch> tCOMInterfacePack;

// IDispatch

	virtual HRESULT STDMETHODCALLTYPE GetTypeInfoCount( 
		/* [out] */ __RPC__out UINT *pctinfo)noexcept(true)override;
        
	virtual HRESULT STDMETHODCALLTYPE GetTypeInfo( 
		/* [in] */ UINT iTInfo,
		/* [in] */ LCID lcid,
		/* [out] */ __RPC__deref_out_opt ITypeInfo **ppTInfo)noexcept(true)override;
        
	virtual HRESULT STDMETHODCALLTYPE GetIDsOfNames( 
		/* [in] */ __RPC__in REFIID riid,
		/* [size_is][in] */ __RPC__in_ecount_full(cNames) LPOLESTR *rgszNames,
		/* [range][in] */ __RPC__in_range(0,16384) UINT cNames,
		/* [in] */ LCID lcid,
		/* [size_is][out] */ __RPC__out_ecount_full(cNames) DISPID *rgDispId)noexcept(true)override;
        
	virtual /* [local] */ HRESULT STDMETHODCALLTYPE Invoke( 
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
		_Out_opt_  UINT *puArgErr)noexcept(true)override;

protected:
};
//---------------------------------------------------------------------------
class cOLEInPlaceSite : public IOleInPlaceSite
{
public:
	cOLEInPlaceSite()noexcept(true);
	~cOLEInPlaceSite()noexcept(true);

	cOLEInPlaceControl *InPlaceControl;

	typedef TCOMInterfacePack<IOleWindow,IOleInPlaceSite> tCOMInterfacePack;

protected:

// IOleWindow

	virtual /* [input_sync] */ HRESULT STDMETHODCALLTYPE GetWindow(
		/* [out] */ __RPC__deref_out_opt HWND *phwnd)noexcept(true)override;
        
	virtual HRESULT STDMETHODCALLTYPE ContextSensitiveHelp(
		/* [in] */ BOOL fEnterMode)noexcept(true)override;

// IOleInPlaceSite

	virtual HRESULT STDMETHODCALLTYPE CanInPlaceActivate(void)noexcept(true)override;
        
	virtual HRESULT STDMETHODCALLTYPE OnInPlaceActivate(void)noexcept(true)override;
        
	virtual HRESULT STDMETHODCALLTYPE OnUIActivate(void)noexcept(true)override;
        
	virtual HRESULT STDMETHODCALLTYPE GetWindowContext(
		/* [out] */ __RPC__deref_out_opt IOleInPlaceFrame **ppFrame,
		/* [out] */ __RPC__deref_out_opt IOleInPlaceUIWindow **ppDoc,
		/* [out] */ __RPC__out LPRECT lprcPosRect,
		/* [out] */ __RPC__out LPRECT lprcClipRect,
		/* [out][in] */ __RPC__inout LPOLEINPLACEFRAMEINFO lpFrameInfo)noexcept(true)override;
        
	virtual HRESULT STDMETHODCALLTYPE Scroll(
		/* [in] */ SIZE scrollExtant)noexcept(true)override;

	virtual HRESULT STDMETHODCALLTYPE OnUIDeactivate(
		/* [in] */ BOOL fUndoable)noexcept(true)override;
        
	virtual HRESULT STDMETHODCALLTYPE OnInPlaceDeactivate(void)noexcept(true)override;
        
	virtual HRESULT STDMETHODCALLTYPE DiscardUndoState(void)noexcept(true)override;

	virtual HRESULT STDMETHODCALLTYPE DeactivateAndUndo(void)noexcept(true)override;
        
	virtual HRESULT STDMETHODCALLTYPE OnPosRectChange(
		/* [in] */ __RPC__in LPCRECT lprcPosRect)noexcept(true)override;
private:
};
//---------------------------------------------------------------------------
class cOLEDocumentSite : public bCOMOleClientSite, public bCOMAdviseSink
{
public:
	cOLEDocumentSite(cOLEInPlaceControl *Control)noexcept(true);
	~cOLEDocumentSite()noexcept(true);
	using bCOMOleClientSite::AddRef;
	using bCOMOleClientSite::Release;

	rPtr<iReference> Reference;

	IOleObject* GetOleObject(void)noexcept(true);
	void SetOleObject(IOleObject *OLEObject)noexcept(true);
	bool OLEOpen(LONG iVerb)noexcept(true);
	void OLEClose(DWORD dwSaveOption=OLECLOSE_NOSAVE)noexcept(true);

	// COM

	typedef TCOMInterfacePack<bCOMOleClientSite::tCOMInterfacePack,bCOMAdviseSink::tCOMInterfacePack> tCOMInterfacePack;

	bool COMQueryInterface(REFIID riid,_COM_Outptr_ void __RPC_FAR *__RPC_FAR *ppvObject)noexcept(true);

protected:
	
	// InPlaceSite
	
	class cInPlaceSite : public cOLEInPlaceSite
	{
	public:
		virtual HRESULT STDMETHODCALLTYPE CanInPlaceActivate(void)noexcept(true)override;
	};
	COMInnerObject<cInPlaceSite> fInPlaceSite;
	
	// OLE Object

	COMPtr<IOleObject> fOLEObject;

};
//---------------------------------------------------------------------------
class bcOLEActiveXControlSite : public bCOMOleClientSite, public bCOMOleControlSite, public bCOMDispatch
{
public:
	using bCOMOleClientSite::AddRef;
	using bCOMOleClientSite::Release;

	bcOLEActiveXControlSite()noexcept(true);
	~bcOLEActiveXControlSite()noexcept(true);

	IOleObject* GetOleObject(void)noexcept(true);
	void SetOleObject(IOleObject *OLEObject)noexcept(true);

	// COM
	
	typedef TCOMInterfacePack<bCOMOleClientSite::tCOMInterfacePack
		,bCOMOleControlSite::tCOMInterfacePack,bCOMDispatch::tCOMInterfacePack
	> tCOMInterfacePack;

protected:
	// OLE Object

	COMPtr<IOleObject> fOLEObject;

	// IDispatch

	virtual /* [local] */ HRESULT STDMETHODCALLTYPE Invoke( 
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
		_Out_opt_  UINT *puArgErr)noexcept(true)override;
};
//---------------------------------------------------------------------------
class cOLEActiveXControlSite : public bcOLEActiveXControlSite,private cOLEInPlaceControl::iActivation
{
public:
	cOLEActiveXControlSite(cOLEInPlaceControl *Control)noexcept(true);
	~cOLEActiveXControlSite()noexcept(true);

	bool COMQueryInterface(REFIID riid,_COM_Outptr_ void __RPC_FAR *__RPC_FAR *ppvObject)noexcept(true);
	void SetOleObject(IOleObject *OLEObject)noexcept(true);

	void SetActiveState(eOLEInPlaceActiveState State)noexcept(true);

protected:

	// InPlaceSite

	COMInnerObject<cOLEInPlaceSite> fInPlaceSite;

private:

	bool OLEVerb(int iVerb)noexcept(true);
	virtual bool OLEInPlaceActive(void)noexcept(true)override;
	virtual bool OLEUIActive(void)noexcept(true)override;
};
//---------------------------------------------------------------------------
class cOLEInPlaceSiteWindowless : public IOleInPlaceSiteWindowless
{
public:
	cOLEInPlaceSiteWindowless()noexcept(true);
	~cOLEInPlaceSiteWindowless()noexcept(true);

	cOLEInPlaceWindowlessControl *InPlaceControl;


	typedef TCOMInterfacePack<IOleWindow,IOleInPlaceSite,IOleInPlaceSiteEx,IOleInPlaceSiteWindowless> tCOMInterfacePack;

protected:

// IOleWindow

	virtual /* [input_sync] */ HRESULT STDMETHODCALLTYPE GetWindow(
		/* [out] */ __RPC__deref_out_opt HWND *phwnd)noexcept(true)override;
        
	virtual HRESULT STDMETHODCALLTYPE ContextSensitiveHelp(
		/* [in] */ BOOL fEnterMode)noexcept(true)override;

// IOleInPlaceSite

	virtual HRESULT STDMETHODCALLTYPE CanInPlaceActivate(void)noexcept(true)override;
        
	virtual HRESULT STDMETHODCALLTYPE OnInPlaceActivate(void)noexcept(true)override;
        
	virtual HRESULT STDMETHODCALLTYPE OnUIActivate(void)noexcept(true)override;
        
	virtual HRESULT STDMETHODCALLTYPE GetWindowContext(
		/* [out] */ __RPC__deref_out_opt IOleInPlaceFrame **ppFrame,
		/* [out] */ __RPC__deref_out_opt IOleInPlaceUIWindow **ppDoc,
		/* [out] */ __RPC__out LPRECT lprcPosRect,
		/* [out] */ __RPC__out LPRECT lprcClipRect,
		/* [out][in] */ __RPC__inout LPOLEINPLACEFRAMEINFO lpFrameInfo)noexcept(true)override;
        
	virtual HRESULT STDMETHODCALLTYPE Scroll(
		/* [in] */ SIZE scrollExtant)noexcept(true)override;

	virtual HRESULT STDMETHODCALLTYPE OnUIDeactivate(
		/* [in] */ BOOL fUndoable)noexcept(true)override;
        
	virtual HRESULT STDMETHODCALLTYPE OnInPlaceDeactivate(void)noexcept(true)override;
        
	virtual HRESULT STDMETHODCALLTYPE DiscardUndoState(void)noexcept(true)override;

	virtual HRESULT STDMETHODCALLTYPE DeactivateAndUndo(void)noexcept(true)override;
        
	virtual HRESULT STDMETHODCALLTYPE OnPosRectChange(
		/* [in] */ __RPC__in LPCRECT lprcPosRect)noexcept(true)override;

// IOleInPlaceSiteEx
	virtual HRESULT STDMETHODCALLTYPE OnInPlaceActivateEx(
		/* [out] */ __RPC__out BOOL *pfNoRedraw,
		/* [in] */ DWORD dwFlags)noexcept(true)override;
        
	virtual HRESULT STDMETHODCALLTYPE OnInPlaceDeactivateEx(
		/* [in] */ BOOL fNoRedraw)noexcept(true)override;
        
	virtual HRESULT STDMETHODCALLTYPE RequestUIActivate(void)noexcept(true)override;

// IOleInPlaceSiteWindowless

	virtual HRESULT STDMETHODCALLTYPE CanWindowlessActivate(void)noexcept(true)override;
	virtual HRESULT STDMETHODCALLTYPE GetCapture(void)noexcept(true)override;
	virtual HRESULT STDMETHODCALLTYPE SetCapture(
		/* [in] */ BOOL fCapture)noexcept(true)override;
	virtual HRESULT STDMETHODCALLTYPE GetFocus(void)noexcept(true)override;
	virtual HRESULT STDMETHODCALLTYPE SetFocus(
		/* [in] */ BOOL fFocus)noexcept(true)override;
	virtual HRESULT STDMETHODCALLTYPE GetDC(
		/* [unique][in] */ __RPC__in_opt LPCRECT pRect,
		/* [in] */ DWORD grfFlags,
		/* [out] */ __RPC__deref_out_opt HDC *phDC)noexcept(true)override;
	virtual HRESULT STDMETHODCALLTYPE ReleaseDC(
		/* [in] */ __RPC__in HDC hDC)noexcept(true)override;
	virtual HRESULT STDMETHODCALLTYPE InvalidateRect(
		/* [unique][in] */ __RPC__in_opt LPCRECT pRect,
		/* [in] */ BOOL fErase)noexcept(true)override;
	virtual HRESULT STDMETHODCALLTYPE InvalidateRgn(
		/* [in] */ __RPC__in HRGN hRGN,
		/* [in] */ BOOL fErase)noexcept(true)override;
	virtual HRESULT STDMETHODCALLTYPE ScrollRect(
		/* [in] */ INT dx,
		/* [in] */ INT dy,
		/* [in] */ __RPC__in LPCRECT pRectScroll,
		/* [in] */ __RPC__in LPCRECT pRectClip)noexcept(true)override;
	virtual HRESULT STDMETHODCALLTYPE AdjustRect(
		/* [out][in] */ __RPC__inout LPRECT prc)noexcept(true)override;
	virtual HRESULT STDMETHODCALLTYPE OnDefWindowMessage( 
		/* [annotation][in] */ 
		_In_  UINT msg,
		/* [annotation][in] */ 
		_In_  WPARAM wParam,
		/* [annotation][in] */ 
		_In_  LPARAM lParam,
		/* [out] */ __RPC__out LRESULT *plResult)noexcept(true)override;
};
//---------------------------------------------------------------------------
class cOLEActiveXControlSiteWindowless : public bcOLEActiveXControlSite,private cOLEInPlaceControl::iActivation
{
public:
	cOLEActiveXControlSiteWindowless(cOLEInPlaceWindowlessControl *Control)noexcept(true);
	~cOLEActiveXControlSiteWindowless()noexcept(true);

	bool COMQueryInterface(REFIID riid,_COM_Outptr_ void __RPC_FAR *__RPC_FAR *ppvObject)noexcept(true);

	void SetActiveState(eOLEInPlaceActiveState State)noexcept(true);

protected:

	// InPlaceSite

	COMInnerObject<cOLEInPlaceSiteWindowless> fInPlaceSite;

private:

	bool OLEVerb(int iVerb)noexcept(true);
	virtual bool OLEInPlaceActive(void)noexcept(true)override;
	virtual bool OLEUIActive(void)noexcept(true)override;
};
//---------------------------------------------------------------------------
}	// namespace cnUI
//---------------------------------------------------------------------------
}	// namespace cnLibrary
//---------------------------------------------------------------------------
