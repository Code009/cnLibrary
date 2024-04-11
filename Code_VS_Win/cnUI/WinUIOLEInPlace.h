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
	
	virtual void FrameInfo(LPOLEINPLACEFRAMEINFO lpFrameInfo)noexcept(true);
};
//---------------------------------------------------------------------------
class cOLEInPlaceFrameInfo : public cOLEInPlaceFrameConfiguration
{
public:
	bool IsMDIApp=false;
	HWND AcceleratorHWND=nullptr;
	HACCEL AcceleratorHandle=nullptr;
	UINT AcceleratorEntries=0;
	
	virtual void FrameInfo(LPOLEINPLACEFRAMEINFO lpFrameInfo)noexcept(true)override;
};
//---------------------------------------------------------------------------
class cOLEInPlaceControl : public ViewControl, protected UIStateHandler
{
public:
	cOLEInPlaceControl()noexcept(true);
	~cOLEInPlaceControl()noexcept(true);

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
		virtual bool OLEInPlaceActive(void)noexcept(true)=0;
		virtual bool OLEUIActive(void)noexcept(true)=0;
	};
	iActivation *OLEActivation=nullptr;

	IOleInPlaceObject* GetInPlaceObject(void)noexcept(true);

	eOLEInPlaceActiveState GetActiveState(void)const noexcept(true);

	void UpdateObjectState(void)noexcept(true);
	
	// action for OLE

	void SetActiveState(eOLEInPlaceActiveState State)noexcept(true);
	void OLEDeactivate(void)noexcept(true);
	void OLEUIDeactivate(void)noexcept(true);

	// state for OLE

	HWND OLEGetWindowHandle(void)noexcept(true);
	RECT OLEGetInPlaceRect(void)noexcept(true);
	HRESULT OLEGetWindowContext(IOleInPlaceFrame **ppFrame,IOleInPlaceUIWindow **ppDoc,LPRECT lprcPosRect,LPRECT lprcClipRect,LPOLEINPLACEFRAMEINFO lpFrameInfo)noexcept(true);

	// call from OLE

	void OLESetInPlaceObject(COMPtr<IOleInPlaceObject> InPlaceObject)noexcept(true);
	bool OLECanInPlaceActivate(void)noexcept(true);
	void OLEOnInPlaceActivate(void)noexcept(true);
	void OLEOnUIActivate(void)noexcept(true);
	void OLEOnUIDeactivate(void)noexcept(true);
	void OLEOnInPlaceDeactivate(void)noexcept(true);
	void OLEOnPosRectChange(LPCRECT lprcPosRect)noexcept(true);
protected:
	
	COMPtr<IOleInPlaceObject> fInPlaceObject;

	void SetupInPlaceRect(void)noexcept(true);

	virtual void ViewSetup(void)noexcept(true)override;
	virtual void ViewClear(void)noexcept(true)override;

	virtual void cnLib_FUNC RectangleChanged(bool Moved,bool Sized)noexcept(true)override;

	virtual void cnLib_FUNC UIStarted(void)noexcept(true)override;
	virtual void cnLib_FUNC UIShow(void)noexcept(true)override;
	virtual void cnLib_FUNC UIHide(void)noexcept(true)override;
	virtual void cnLib_FUNC UIStopped(void)noexcept(true)override;
	
	HWND fHostWindowHandle;

private:
	void AutoActive(void)noexcept(true);
	void IncOLEState(uIntn Count)noexcept(true);
	void DecOLEState(uIntn Count)noexcept(true);

};
//---------------------------------------------------------------------------
class cOLEInPlaceWindowlessControl : public cOLEInPlaceControl, protected iWindowMessageHandler
{
public:
	cOLEInPlaceWindowlessControl()noexcept(true);
	~cOLEInPlaceWindowlessControl()noexcept(true);

	// call from OLE

	void OLEOnInPlaceDeactivate(void)noexcept(true);

	void OLEOnInPlaceActivateEx(BOOL *pfNoRedraw,DWORD dwFlags)noexcept(true);
	void OLEOnInPlaceDeactivateEx(BOOL fNoRedraw)noexcept(true);
	bool OLERequestUIActivate(void)noexcept(true);

	// call from OLE windowless

	bool OLECanWindowlessActivate(void)noexcept(true);
	HRESULT OLEWindowlessGetCapture(void)noexcept(true);
	HRESULT OLEWindowlessSetCapture(BOOL fCapture)noexcept(true);
	HRESULT OLEWindowlessGetFocus(void)noexcept(true);
	HRESULT OLEWindowlessSetFocus(BOOL fFocus)noexcept(true);
	HRESULT OLEWindowlessGetDC(LPCRECT pRect,DWORD grfFlags,HDC *phDC)noexcept(true);
	HRESULT OLEWindowlessReleaseDC(HDC hDC)noexcept(true);
	HRESULT OLEWindowlessInvalidateRect(LPCRECT pRect,BOOL fErase)noexcept(true);
	HRESULT OLEWindowlessInvalidateRgn(HRGN hRGN,BOOL fErase)noexcept(true);
	HRESULT OLEWindowlessScrollRect(INT dx,INT dy,LPCRECT pRectScroll,LPCRECT pRectClip)noexcept(true);
	HRESULT OLEWindowlessAdjustRect(LPRECT prc)noexcept(true);
	HRESULT OLEWindowlessOnDefWindowMessage(UINT msg,WPARAM wParam,LPARAM lParam,LRESULT *plResult)noexcept(true);
protected:
	
	virtual void ViewSetup(void)noexcept(true)override;
	virtual void ViewClear(void)noexcept(true)override;
	
	// windowless

	COMPtr<IOleInPlaceObjectWindowless> fInPlaceObjectWindowless;
	COMPtr<IViewObject> fViewObjectWindowless;

	iWindowViewport *fWindowViewport;
	virtual void cnLib_FUNC WindowAttached(void)noexcept(true)override;
	virtual void cnLib_FUNC WindowDetached(void)noexcept(true)override;
	virtual bool cnLib_FUNC WindowMessage(LRESULT &MsgResult,const cWindowMessageParam &Message)noexcept(true)override;
	virtual void cnLib_FUNC WindowMessageProcessed(LRESULT MsgResult,const cWindowMessageParam &Message)noexcept(true)override;
	rPtr<iDCViewContent> fDCViewContent;

	class cOLEWindowlessPainter : public iDCPainter
	{
		cOLEInPlaceWindowlessControl *GetHost()noexcept(true);
		virtual void cnLib_FUNC PaintStarted(void)noexcept(true)override;
		virtual void cnLib_FUNC PaintShow(void)noexcept(true)override;
		virtual void cnLib_FUNC PaintResume(void)noexcept(true)override;
		virtual void cnLib_FUNC PaintPaused(void)noexcept(true)override;
		virtual void cnLib_FUNC PaintHide(void)noexcept(true)override;
		virtual void cnLib_FUNC PaintStopped(void)noexcept(true)override;
		virtual void cnLib_FUNC PaintRectChanged(void)noexcept(true)override;
		virtual void cnLib_FUNC PaintWindowChanged(void)noexcept(true)override{}
		virtual void cnLib_FUNC Paint(HDC DC,HRGN ClipRegion)noexcept(true)override;
	}fOLEWindowlessPainter;
	void WindowlessObjectDraw(HDC DC)noexcept(true);
};
//---------------------------------------------------------------------------
}	// namespace cnUI
//---------------------------------------------------------------------------
}	// namespace cnLibrary
//---------------------------------------------------------------------------
