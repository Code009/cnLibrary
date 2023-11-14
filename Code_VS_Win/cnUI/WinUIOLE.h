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
SIZE OLE_DPtoHIMETRIC(HDC DC,const SIZE &Size);
SIZE OLE_HIMETRICtoDP(HDC DC,const SIZE &Size);
RECT OLE_DPtoHIMETRIC(HDC DC,const RECT &Rect);
RECT OLE_HIMETRICtoDP(HDC DC,const RECT &Rect);
//---------------------------------------------------------------------------
class cOLEInPlaceUIForm : public Form
{
public:
	cOLEInPlaceUIForm();
	~cOLEInPlaceUIForm();

	HWND GetWindowHandle(void);
	bool GetToolRect(RECT &Rect);
	bool CheckToolSpace(const BORDERWIDTHS &Space);
	void RestoreToolSpace(void);
	bool SetToolSpace(const BORDERWIDTHS &Space);
	
	cLayout GetClientLayout(void);

	sfInt32n AllowSpaceLeft=0;
	sfInt32n AllowSpaceTop=0;
	sfInt32n AllowSpaceRight=0;
	sfInt32n AllowSpaceBottom=0;
protected:
	sfInt32n fSpaceLeft=0;
	sfInt32n fSpaceTop=0;
	sfInt32n fSpaceRight=0;
	sfInt32n fSpaceBottom=0;

	HWND fHostWindowHandle;

	virtual void cnLib_FUNC UIStarted(void)override;
	virtual void cnLib_FUNC UIStopped(void)override;
};
//---------------------------------------------------------------------------
class cOleInPlaceUIWindowComponent : public WindowMessageHandler, public IOleInPlaceUIWindow
{
public:
	cOleInPlaceUIWindowComponent();
	~cOleInPlaceUIWindowComponent();

	IUnknown* COMUnknown(void);
    bool COMQueryInterface(REFIID riid,_COM_Outptr_ void __RPC_FAR *__RPC_FAR *ppvObject);

	cOLEInPlaceUIForm *ToolForm=nullptr;


// IOleWindow

    virtual /* [input_sync] */ HRESULT STDMETHODCALLTYPE GetWindow(
        /* [out] */ __RPC__deref_out_opt HWND *phwnd)override;
        
    virtual HRESULT STDMETHODCALLTYPE ContextSensitiveHelp( 
        /* [in] */ BOOL fEnterMode)override;


// IOleInPlaceUIWindow

    virtual /* [input_sync] */ HRESULT STDMETHODCALLTYPE GetBorder(
        /* [out] */ __RPC__out LPRECT lprectBorder)override;
        
    virtual /* [input_sync] */ HRESULT STDMETHODCALLTYPE RequestBorderSpace(
        /* [unique][in] */ __RPC__in_opt LPCBORDERWIDTHS pborderwidths)override;
        
    virtual /* [input_sync] */ HRESULT STDMETHODCALLTYPE SetBorderSpace(
        /* [unique][in] */ __RPC__in_opt LPCBORDERWIDTHS pborderwidths)override;
        
    virtual HRESULT STDMETHODCALLTYPE SetActiveObject(
        /* [unique][in] */ __RPC__in_opt IOleInPlaceActiveObject *pActiveObject,
        /* [unique][string][in] */ __RPC__in_opt_string LPCOLESTR pszObjName)override;

protected:
};
//---------------------------------------------------------------------------
class cOleInPlaceFrameComponent : public WindowControl, public IOleInPlaceFrame
{
public:
	cOleInPlaceFrameComponent();
	~cOleInPlaceFrameComponent();

	IUnknown* COMUnknown(void);
    bool COMQueryInterface(REFIID riid,_COM_Outptr_ void __RPC_FAR *__RPC_FAR *ppvObject);

	cOLEInPlaceUIForm *ToolForm=nullptr;


// IOleWindow

    virtual /* [input_sync] */ HRESULT STDMETHODCALLTYPE GetWindow(
        /* [out] */ __RPC__deref_out_opt HWND *phwnd)override;
        
    virtual HRESULT STDMETHODCALLTYPE ContextSensitiveHelp( 
        /* [in] */ BOOL fEnterMode)override;


// IOleInPlaceUIWindow

    virtual /* [input_sync] */ HRESULT STDMETHODCALLTYPE GetBorder(
        /* [out] */ __RPC__out LPRECT lprectBorder)override;
        
    virtual /* [input_sync] */ HRESULT STDMETHODCALLTYPE RequestBorderSpace(
        /* [unique][in] */ __RPC__in_opt LPCBORDERWIDTHS pborderwidths)override;
        
    virtual /* [input_sync] */ HRESULT STDMETHODCALLTYPE SetBorderSpace(
        /* [unique][in] */ __RPC__in_opt LPCBORDERWIDTHS pborderwidths)override;
        
    virtual HRESULT STDMETHODCALLTYPE SetActiveObject(
        /* [unique][in] */ __RPC__in_opt IOleInPlaceActiveObject *pActiveObject,
        /* [unique][string][in] */ __RPC__in_opt_string LPCOLESTR pszObjName)override;

// IOleInPlaceFrame

	virtual HRESULT STDMETHODCALLTYPE InsertMenus(
        /* [in] */ __RPC__in HMENU hmenuShared,
        /* [out][in] */ __RPC__inout LPOLEMENUGROUPWIDTHS lpMenuWidths)override;
        
    virtual /* [input_sync] */ HRESULT STDMETHODCALLTYPE SetMenu(
        /* [in] */ __RPC__in HMENU hmenuShared,
        /* [in] */ __RPC__in HOLEMENU holemenu,
        /* [in] */ __RPC__in HWND hwndActiveObject)override;
        
    virtual HRESULT STDMETHODCALLTYPE RemoveMenus(
        /* [in] */ __RPC__in HMENU hmenuShared)override;
        
    virtual /* [input_sync] */ HRESULT STDMETHODCALLTYPE SetStatusText(
        /* [unique][in] */ __RPC__in_opt LPCOLESTR pszStatusText)override;
        
    virtual HRESULT STDMETHODCALLTYPE EnableModeless(
        /* [in] */ BOOL fEnable)override;
        
    virtual HRESULT STDMETHODCALLTYPE TranslateAccelerator(
        /* [in] */ __RPC__in LPMSG lpmsg,
        /* [in] */ WORD wID)override;

protected:
};
//---------------------------------------------------------------------------
class bCOMOleClientSite : public IOleClientSite
{
public:
	IUnknown* COMUnknown(void);
	bool COMQueryInterface(REFIID riid,_COM_Outptr_ void __RPC_FAR *__RPC_FAR *ppvObject);

// IOleClientSite

	// SaveObject
	// Saves the embedded object associated with the client site.
	// This function is synchronous; by the time it returns, the save will be completed.
	virtual HRESULT STDMETHODCALLTYPE SaveObject(void);

	// GetMoniker
	// Retrieves a moniker for the object's client site
	virtual HRESULT STDMETHODCALLTYPE GetMoniker(
		/* [in] */ DWORD dwAssign,
		/* [in] */ DWORD dwWhichMoniker,
		/* [out] */ __RPC__deref_out_opt IMoniker **ppmk)override;

	// GetContainer
	// Retrieves a pointer to the object's container.
	//	This method is optional but recommended for all containers that expect to support links to their embedded objects.
	//	Simple containers that do not support links to their embedded objects probably do not need to implement this method. Instead, they can return E_NOINTERFACE and set ppContainer to NULL.
	virtual HRESULT STDMETHODCALLTYPE GetContainer(
		/* [out] */ __RPC__deref_out_opt IOleContainer **ppContainer)override;

	// ShowObject
	// Asks a container to display its object to the user. This method ensures that the container itself is visible and not minimized.
	virtual HRESULT STDMETHODCALLTYPE ShowObject(void)override;

	// OnShowWindow
	// Notifies a container when an embedded object's window is about to become visible or invisible.
	// called when object created its own window
	virtual HRESULT STDMETHODCALLTYPE OnShowWindow( /* [in] */ BOOL fShow)override;

	// RequestNewObjectLayout
	// Asks a container to resize the display site for embedded objects.
	//	Responding to this method will be optional for containers.
	virtual HRESULT STDMETHODCALLTYPE RequestNewObjectLayout(void)override;

};
//---------------------------------------------------------------------------
class bCOMAdviseSink : public IAdviseSink
{
public:

	IUnknown* COMUnknown(void);
	bool COMQueryInterface(REFIID riid,_COM_Outptr_ void __RPC_FAR *__RPC_FAR *ppvObject);

// IAdviseSink

    virtual /* [local] */ void STDMETHODCALLTYPE OnDataChange( 
        /* [annotation][unique][in] */ 
        _In_  FORMATETC *pFormatetc,
        /* [annotation][unique][in] */ 
        _In_  STGMEDIUM *pStgmed)override;
        
    virtual /* [local] */ void STDMETHODCALLTYPE OnViewChange( 
        /* [in] */ DWORD dwAspect,
		/* [in] */ LONG lindex)override;
        
    virtual /* [local] */ void STDMETHODCALLTYPE OnRename( 
        /* [annotation][in] */ 
		_In_  IMoniker *pmk)override;
        
    virtual /* [local] */ void STDMETHODCALLTYPE OnSave( void)override;
        
    virtual /* [local] */ void STDMETHODCALLTYPE OnClose( void)override;

};
//---------------------------------------------------------------------------
class bCOMOleControlSite : public IOleControlSite
{
public:
	IUnknown* COMUnknown(void);
	bool COMQueryInterface(REFIID riid,_COM_Outptr_ void __RPC_FAR *__RPC_FAR *ppvObject);

	// IOleControlSite
 
	virtual HRESULT STDMETHODCALLTYPE OnControlInfoChanged(void)override;
        
    virtual HRESULT STDMETHODCALLTYPE LockInPlaceActive(
        /* [in] */ BOOL fLock)override;
        
    virtual HRESULT STDMETHODCALLTYPE GetExtendedControl(
        /* [out] */ __RPC__deref_out_opt IDispatch **ppDisp)override;
        
    virtual HRESULT STDMETHODCALLTYPE TransformCoords(
        /* [out][in] */ __RPC__inout POINTL *pPtlHimetric,
        /* [out][in] */ __RPC__inout POINTF *pPtfContainer,
        /* [in] */ DWORD dwFlags)override;
        
    virtual HRESULT STDMETHODCALLTYPE TranslateAccelerator(
        /* [in] */ __RPC__in MSG *pMsg,
        /* [in] */ DWORD grfModifiers)override;
        
    virtual HRESULT STDMETHODCALLTYPE OnFocus(
        /* [in] */ BOOL fGotFocus)override;
        
    virtual HRESULT STDMETHODCALLTYPE ShowPropertyFrame(void)override;
};
//---------------------------------------------------------------------------
class bCOMDispatch : public IDispatch
{
public:

	IUnknown* COMUnknown(void);
	bool COMQueryInterface(REFIID riid,_COM_Outptr_ void __RPC_FAR *__RPC_FAR *ppvObject);

// IDispatch

	virtual HRESULT STDMETHODCALLTYPE GetTypeInfoCount( 
		/* [out] */ __RPC__out UINT *pctinfo)override;
        
	virtual HRESULT STDMETHODCALLTYPE GetTypeInfo( 
		/* [in] */ UINT iTInfo,
		/* [in] */ LCID lcid,
		/* [out] */ __RPC__deref_out_opt ITypeInfo **ppTInfo)override;
        
	virtual HRESULT STDMETHODCALLTYPE GetIDsOfNames( 
		/* [in] */ __RPC__in REFIID riid,
		/* [size_is][in] */ __RPC__in_ecount_full(cNames) LPOLESTR *rgszNames,
		/* [range][in] */ __RPC__in_range(0,16384) UINT cNames,
		/* [in] */ LCID lcid,
		/* [size_is][out] */ __RPC__out_ecount_full(cNames) DISPID *rgDispId)override;
        
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
		_Out_opt_  UINT *puArgErr)override;

protected:
};
//---------------------------------------------------------------------------
class cOLEInPlaceSite : public IOleInPlaceSite
{
public:
	cOLEInPlaceSite();
	~cOLEInPlaceSite();

	cOLEInPlaceControl *InPlaceControl;

	IUnknown* COMUnknown(void);
	bool COMQueryInterface(REFIID riid,_COM_Outptr_ void __RPC_FAR *__RPC_FAR *ppvObject);

protected:

// IOleWindow

	virtual /* [input_sync] */ HRESULT STDMETHODCALLTYPE GetWindow(
		/* [out] */ __RPC__deref_out_opt HWND *phwnd)override;
        
	virtual HRESULT STDMETHODCALLTYPE ContextSensitiveHelp(
		/* [in] */ BOOL fEnterMode)override;

// IOleInPlaceSite

	virtual HRESULT STDMETHODCALLTYPE CanInPlaceActivate(void)override;
        
	virtual HRESULT STDMETHODCALLTYPE OnInPlaceActivate(void)override;
        
	virtual HRESULT STDMETHODCALLTYPE OnUIActivate(void)override;
        
	virtual HRESULT STDMETHODCALLTYPE GetWindowContext(
		/* [out] */ __RPC__deref_out_opt IOleInPlaceFrame **ppFrame,
		/* [out] */ __RPC__deref_out_opt IOleInPlaceUIWindow **ppDoc,
		/* [out] */ __RPC__out LPRECT lprcPosRect,
		/* [out] */ __RPC__out LPRECT lprcClipRect,
		/* [out][in] */ __RPC__inout LPOLEINPLACEFRAMEINFO lpFrameInfo) override;
        
	virtual HRESULT STDMETHODCALLTYPE Scroll(
		/* [in] */ SIZE scrollExtant)override;

	virtual HRESULT STDMETHODCALLTYPE OnUIDeactivate(
		/* [in] */ BOOL fUndoable)override;
        
	virtual HRESULT STDMETHODCALLTYPE OnInPlaceDeactivate(void)override;
        
	virtual HRESULT STDMETHODCALLTYPE DiscardUndoState(void) override;

	virtual HRESULT STDMETHODCALLTYPE DeactivateAndUndo(void)override;
        
	virtual HRESULT STDMETHODCALLTYPE OnPosRectChange(
		/* [in] */ __RPC__in LPCRECT lprcPosRect)override;
private:
};
//---------------------------------------------------------------------------
class cOLEDocumentSite : public bCOMOleClientSite, public bCOMAdviseSink
{
public:
	cOLEDocumentSite(cOLEInPlaceControl *Control);
	~cOLEDocumentSite();
	using bCOMOleClientSite::AddRef;
	using bCOMOleClientSite::Release;

	rPtr<iReference> Reference;

	IOleObject* GetOleObject(void);
	void SetOleObject(IOleObject *OLEObject);
	bool OLEOpen(LONG iVerb);
	void OLEClose(DWORD dwSaveOption=OLECLOSE_NOSAVE);

	// COM

	IUnknown* COMUnknown(void);
	bool COMQueryInterface(REFIID riid,_COM_Outptr_ void __RPC_FAR *__RPC_FAR *ppvObject);

protected:
	
	// InPlaceSite
	
	class cInPlaceSite : public cOLEInPlaceSite
	{
	public:
		virtual HRESULT STDMETHODCALLTYPE CanInPlaceActivate(void)override;
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

	bcOLEActiveXControlSite();
	~bcOLEActiveXControlSite();

	IOleObject* GetOleObject(void);
	void SetOleObject(IOleObject *OLEObject);

	// COM

	IUnknown* COMUnknown(void);
	bool COMQueryInterface(REFIID riid,_COM_Outptr_ void __RPC_FAR *__RPC_FAR *ppvObject);

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
		_Out_opt_  UINT *puArgErr)override;
};
//---------------------------------------------------------------------------
class cOLEActiveXControlSite : public bcOLEActiveXControlSite,private cOLEInPlaceControl::iActivation
{
public:
	cOLEActiveXControlSite(cOLEInPlaceControl *Control);
	~cOLEActiveXControlSite();

	bool COMQueryInterface(REFIID riid,_COM_Outptr_ void __RPC_FAR *__RPC_FAR *ppvObject);
	void SetOleObject(IOleObject *OLEObject);

	void SetActiveState(eOLEInPlaceActiveState State);

protected:

	// InPlaceSite

	COMInnerObject<cOLEInPlaceSite> fInPlaceSite;

private:

	bool OLEVerb(int iVerb);
	virtual bool OLEInPlaceActive(void)override;
	virtual bool OLEUIActive(void)override;
};
//---------------------------------------------------------------------------
class cOLEInPlaceSiteWindowless : public IOleInPlaceSiteWindowless
{
public:
	cOLEInPlaceSiteWindowless();
	~cOLEInPlaceSiteWindowless();

	cOLEInPlaceWindowlessControl *InPlaceControl;


	IUnknown* COMUnknown(void);
	bool COMQueryInterface(REFIID riid,_COM_Outptr_ void __RPC_FAR *__RPC_FAR *ppvObject);

protected:

// IOleWindow

	virtual /* [input_sync] */ HRESULT STDMETHODCALLTYPE GetWindow(
		/* [out] */ __RPC__deref_out_opt HWND *phwnd)override;
        
	virtual HRESULT STDMETHODCALLTYPE ContextSensitiveHelp(
		/* [in] */ BOOL fEnterMode)override;

// IOleInPlaceSite

	virtual HRESULT STDMETHODCALLTYPE CanInPlaceActivate(void)override;
        
	virtual HRESULT STDMETHODCALLTYPE OnInPlaceActivate(void)override;
        
	virtual HRESULT STDMETHODCALLTYPE OnUIActivate(void)override;
        
	virtual HRESULT STDMETHODCALLTYPE GetWindowContext(
		/* [out] */ __RPC__deref_out_opt IOleInPlaceFrame **ppFrame,
		/* [out] */ __RPC__deref_out_opt IOleInPlaceUIWindow **ppDoc,
		/* [out] */ __RPC__out LPRECT lprcPosRect,
		/* [out] */ __RPC__out LPRECT lprcClipRect,
		/* [out][in] */ __RPC__inout LPOLEINPLACEFRAMEINFO lpFrameInfo) override;
        
	virtual HRESULT STDMETHODCALLTYPE Scroll(
		/* [in] */ SIZE scrollExtant)override;

	virtual HRESULT STDMETHODCALLTYPE OnUIDeactivate(
		/* [in] */ BOOL fUndoable)override;
        
	virtual HRESULT STDMETHODCALLTYPE OnInPlaceDeactivate(void)override;
        
	virtual HRESULT STDMETHODCALLTYPE DiscardUndoState(void) override;

	virtual HRESULT STDMETHODCALLTYPE DeactivateAndUndo(void)override;
        
	virtual HRESULT STDMETHODCALLTYPE OnPosRectChange(
		/* [in] */ __RPC__in LPCRECT lprcPosRect)override;

// IOleInPlaceSiteEx
	virtual HRESULT STDMETHODCALLTYPE OnInPlaceActivateEx(
		/* [out] */ __RPC__out BOOL *pfNoRedraw,
		/* [in] */ DWORD dwFlags)override;
        
	virtual HRESULT STDMETHODCALLTYPE OnInPlaceDeactivateEx(
		/* [in] */ BOOL fNoRedraw)override;
        
	virtual HRESULT STDMETHODCALLTYPE RequestUIActivate(void)override;

// IOleInPlaceSiteWindowless

	virtual HRESULT STDMETHODCALLTYPE CanWindowlessActivate(void)override;
	virtual HRESULT STDMETHODCALLTYPE GetCapture(void)override;
	virtual HRESULT STDMETHODCALLTYPE SetCapture(
		/* [in] */ BOOL fCapture)override;
	virtual HRESULT STDMETHODCALLTYPE GetFocus(void)override;
	virtual HRESULT STDMETHODCALLTYPE SetFocus(
		/* [in] */ BOOL fFocus)override;
	virtual HRESULT STDMETHODCALLTYPE GetDC(
		/* [unique][in] */ __RPC__in_opt LPCRECT pRect,
		/* [in] */ DWORD grfFlags,
		/* [out] */ __RPC__deref_out_opt HDC *phDC)override;
	virtual HRESULT STDMETHODCALLTYPE ReleaseDC(
		/* [in] */ __RPC__in HDC hDC)override;
	virtual HRESULT STDMETHODCALLTYPE InvalidateRect(
		/* [unique][in] */ __RPC__in_opt LPCRECT pRect,
		/* [in] */ BOOL fErase)override;
	virtual HRESULT STDMETHODCALLTYPE InvalidateRgn(
		/* [in] */ __RPC__in HRGN hRGN,
		/* [in] */ BOOL fErase)override;
	virtual HRESULT STDMETHODCALLTYPE ScrollRect(
		/* [in] */ INT dx,
		/* [in] */ INT dy,
		/* [in] */ __RPC__in LPCRECT pRectScroll,
		/* [in] */ __RPC__in LPCRECT pRectClip)override;
	virtual HRESULT STDMETHODCALLTYPE AdjustRect(
		/* [out][in] */ __RPC__inout LPRECT prc)override;
	virtual HRESULT STDMETHODCALLTYPE OnDefWindowMessage( 
		/* [annotation][in] */ 
		_In_  UINT msg,
		/* [annotation][in] */ 
		_In_  WPARAM wParam,
		/* [annotation][in] */ 
		_In_  LPARAM lParam,
		/* [out] */ __RPC__out LRESULT *plResult)override;
};
//---------------------------------------------------------------------------
class cOLEActiveXControlSiteWindowless : public bcOLEActiveXControlSite,private cOLEInPlaceControl::iActivation
{
public:
	cOLEActiveXControlSiteWindowless(cOLEInPlaceWindowlessControl *Control);
	~cOLEActiveXControlSiteWindowless();

	bool COMQueryInterface(REFIID riid,_COM_Outptr_ void __RPC_FAR *__RPC_FAR *ppvObject);

	void SetActiveState(eOLEInPlaceActiveState State);

protected:

	// InPlaceSite

	COMInnerObject<cOLEInPlaceSiteWindowless> fInPlaceSite;

private:

	bool OLEVerb(int iVerb);
	virtual bool OLEInPlaceActive(void)override;
	virtual bool OLEUIActive(void)override;
};
//---------------------------------------------------------------------------
}	// namespace cnUI
//---------------------------------------------------------------------------
}	// namespace cnLibrary
//---------------------------------------------------------------------------
