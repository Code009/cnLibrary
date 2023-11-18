/*- cnWinUI - Control for OLE InPlace Activation --------------------------*/
/*         Developer : Code009                                             */
/*         Create on : 2019-12-26                                          */
/*-------------------------------------------------------------------------*/
#pragma once
/*-------------------------------------------------------------------------*/
#include <cnUI\ControlBase.h>
#include <cnUI\WinUIBase.h>

//---------------------------------------------------------------------------
namespace cnLibrary{
//---------------------------------------------------------------------------
namespace cnUI{
//---------------------------------------------------------------------------
cnLib_ENUM_BEGIN(ufInt8,OLEInPlaceActiveState)
{
	Inactive,
	InPlaceActive,
	UIActive
}cnLib_ENUM_END(OLEInPlaceActiveState);
//---------------------------------------------------------------------------
class cOLEInPlaceFrameConfiguration
{
public:
	// InPlace DocWindow
	COMPtr<IOleInPlaceUIWindow> DocWindow;
	// InPlace FrameWindow
	COMPtr<IOleInPlaceFrame> Frame;
	
	virtual void FrameInfo(LPOLEINPLACEFRAMEINFO lpFrameInfo);
};
//---------------------------------------------------------------------------
class cOLEInPlaceFrameInfo : public cOLEInPlaceFrameConfiguration
{
public:
	bool IsMDIApp=false;
	HWND AcceleratorHWND=nullptr;
	HACCEL AcceleratorHandle=nullptr;
	UINT AcceleratorEntries=0;
	
	virtual void FrameInfo(LPOLEINPLACEFRAMEINFO lpFrameInfo)override;
};
//---------------------------------------------------------------------------
class cOLEInPlaceControl : public ViewControl, protected UIStateHandler
{
public:
	cOLEInPlaceControl();
	~cOLEInPlaceControl();

protected:
	eOLEInPlaceActiveState fActiveState;
public:
	bool ActivateWhenBackground	:1;
	bool ActivateWhenShow		:1;
	bool DeactivateWhenHide		:1;
	bool UIActivateOnEvent		:1;
	bool DisableUIActivation	:1;

	cOLEInPlaceFrameConfiguration *FrameConfiguration=nullptr;

	class iActivation
	{
	public:
		virtual bool OLEInPlaceActive(void)=0;
		virtual bool OLEUIActive(void)=0;
	};
	iActivation *OLEActivation=nullptr;

	IOleInPlaceObject* GetInPlaceObject(void);

	eOLEInPlaceActiveState GetActiveState(void)const;

	void UpdateObjectState(void);
	
	// action for OLE

	void SetActiveState(eOLEInPlaceActiveState State);
	void OLEDeactivate(void);
	void OLEUIDeactivate(void);

	// state for OLE

	HWND OLEGetWindowHandle(void);
	RECT OLEGetInPlaceRect(void);
	HRESULT OLEGetWindowContext(IOleInPlaceFrame **ppFrame,IOleInPlaceUIWindow **ppDoc,LPRECT lprcPosRect,LPRECT lprcClipRect,LPOLEINPLACEFRAMEINFO lpFrameInfo);

	// call from OLE

	void OLESetInPlaceObject(COMPtr<IOleInPlaceObject> InPlaceObject);
	bool OLECanInPlaceActivate(void);
	void OLEOnInPlaceActivate(void);
	void OLEOnUIActivate(void);
	void OLEOnUIDeactivate(void);
	void OLEOnInPlaceDeactivate(void);
	void OLEOnPosRectChange(LPCRECT lprcPosRect);
protected:
	
	COMPtr<IOleInPlaceObject> fInPlaceObject;

	void SetupInPlaceRect(void);

	virtual void ViewSetup(void)override;
	virtual void ViewClear(void)override;

	virtual void cnLib_FUNC RectangleChanged(bool Moved,bool Sized)override;

	virtual void cnLib_FUNC UIStarted(void)override;
	virtual void cnLib_FUNC UIShow(void)override;
	virtual void cnLib_FUNC UIHide(void)override;
	virtual void cnLib_FUNC UIStopped(void)override;
	
	HWND fHostWindowHandle;

private:
	void AutoActive(void);
	void IncOLEState(uIntn Count);
	void DecOLEState(uIntn Count);

};
//---------------------------------------------------------------------------
class cOLEInPlaceWindowlessControl : public cOLEInPlaceControl, protected iWindowMessageHandler
{
public:
	cOLEInPlaceWindowlessControl();
	~cOLEInPlaceWindowlessControl();

	// call from OLE

	void OLEOnInPlaceDeactivate(void);

	void OLEOnInPlaceActivateEx(BOOL *pfNoRedraw,DWORD dwFlags);
	void OLEOnInPlaceDeactivateEx(BOOL fNoRedraw);
	bool OLERequestUIActivate(void);

	// call from OLE windowless

	bool OLECanWindowlessActivate(void);
	HRESULT OLEWindowlessGetCapture(void);
	HRESULT OLEWindowlessSetCapture(BOOL fCapture);
	HRESULT OLEWindowlessGetFocus(void);
	HRESULT OLEWindowlessSetFocus(BOOL fFocus);
	HRESULT OLEWindowlessGetDC(LPCRECT pRect,DWORD grfFlags,HDC *phDC);
	HRESULT OLEWindowlessReleaseDC(HDC hDC);
	HRESULT OLEWindowlessInvalidateRect(LPCRECT pRect,BOOL fErase);
	HRESULT OLEWindowlessInvalidateRgn(HRGN hRGN,BOOL fErase);
	HRESULT OLEWindowlessScrollRect(INT dx,INT dy,LPCRECT pRectScroll,LPCRECT pRectClip);
	HRESULT OLEWindowlessAdjustRect(LPRECT prc);
	HRESULT OLEWindowlessOnDefWindowMessage(UINT msg,WPARAM wParam,LPARAM lParam,LRESULT *plResult);
protected:
	
	virtual void ViewSetup(void)override;
	virtual void ViewClear(void)override;
	
	// windowless

	COMPtr<IOleInPlaceObjectWindowless> fInPlaceObjectWindowless;
	COMPtr<IViewObject> fViewObjectWindowless;

	iWindowViewport *fWindowViewport;
	virtual void cnLib_FUNC WindowAttached(void)override;
	virtual void cnLib_FUNC WindowDetached(void)override;
	virtual bool cnLib_FUNC WindowMessage(LRESULT &MsgResult,const cWindowMessageParam &Message)override;
	virtual void cnLib_FUNC WindowMessageProcessed(LRESULT MsgResult,const cWindowMessageParam &Message)override;
	rPtr<iDCViewContent> fDCViewContent;

	class cOLEWindowlessPainter : public iDCPainter
	{
		cOLEInPlaceWindowlessControl *GetHost();
		virtual void cnLib_FUNC PaintStarted(void)override;
		virtual void cnLib_FUNC PaintShow(void)override;
		virtual void cnLib_FUNC PaintResume(void)override;
		virtual void cnLib_FUNC PaintPaused(void)override;
		virtual void cnLib_FUNC PaintHide(void)override;
		virtual void cnLib_FUNC PaintStopped(void)override;
		virtual void cnLib_FUNC PaintRectChanged(void)override;
		virtual void cnLib_FUNC PaintWindowChanged(void)override{}
		virtual void cnLib_FUNC Paint(HDC DC,HRGN ClipRegion)override;
	}fOLEWindowlessPainter;
	void WindowlessObjectDraw(HDC DC);
};
//---------------------------------------------------------------------------
}	// namespace cnUI
//---------------------------------------------------------------------------
}	// namespace cnLibrary
//---------------------------------------------------------------------------
