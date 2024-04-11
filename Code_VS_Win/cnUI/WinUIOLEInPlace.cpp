#include "WinUIOLEInPlace.h"

using namespace cnLibrary;
using namespace cnUI;

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void cOLEInPlaceFrameConfiguration::FrameInfo(LPOLEINPLACEFRAMEINFO lpFrameInfo)noexcept
{
	lpFrameInfo->fMDIApp=false;
	lpFrameInfo->cAccelEntries=0;
	lpFrameInfo->haccel=nullptr;
	lpFrameInfo->hwndFrame=nullptr;
}
//---------------------------------------------------------------------------
void cOLEInPlaceFrameInfo::FrameInfo(LPOLEINPLACEFRAMEINFO lpFrameInfo)noexcept
{
	lpFrameInfo->fMDIApp=IsMDIApp;
	lpFrameInfo->cAccelEntries=AcceleratorEntries;
	lpFrameInfo->haccel=AcceleratorHandle;
	lpFrameInfo->hwndFrame=AcceleratorHWND;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cOLEInPlaceControl::cOLEInPlaceControl()noexcept
{
	fActiveState=OLEInPlaceActiveState::Inactive;
	ActivateWhenBackground=false;
	ActivateWhenShow=false;
	DeactivateWhenHide=false;
	UIActivateOnEvent=false;
	DisableUIActivation=false;
}
//---------------------------------------------------------------------------
cOLEInPlaceControl::~cOLEInPlaceControl()noexcept
{
	SetView(nullptr);
}
//---------------------------------------------------------------------------
RECT cOLEInPlaceControl::OLEGetInPlaceRect(void)noexcept
{
	if(fView==nullptr)
		return {0,0,0,0};

	// get rect relative to host window
	cUIPoint p;
	fView->GetPosition(fView->GetWindow(),p);
	auto s=fView->GetSize();
	RECT RetRC;
	RetRC.left=p.x;
	RetRC.top=p.y;
	RetRC.right=p.x+s.x;
	RetRC.bottom=p.y+s.y;
	return RetRC;
}
//---------------------------------------------------------------------------
void cOLEInPlaceControl::ViewSetup(void)noexcept
{
	ViewControl::ViewSetup();
	fView->InsertStateHandler(this);
}
//---------------------------------------------------------------------------
void cOLEInPlaceControl::ViewClear(void)noexcept
{
	fView->RemoveStateHandler(this);
	ViewControl::ViewClear();
}
//---------------------------------------------------------------------------
IOleInPlaceObject* cOLEInPlaceControl::GetInPlaceObject(void)noexcept
{
	return fInPlaceObject;
}
//---------------------------------------------------------------------------
void cOLEInPlaceControl::OLESetInPlaceObject(COMPtr<IOleInPlaceObject> InPlaceObject)noexcept
{
	fInPlaceObject=cnVar::MoveCast(InPlaceObject);
	fActiveState=OLEInPlaceActiveState::Inactive;
}
//---------------------------------------------------------------------------
eOLEInPlaceActiveState cOLEInPlaceControl::GetActiveState(void)const noexcept
{
	return fActiveState;
}
//---------------------------------------------------------------------------
void cOLEInPlaceControl::SetActiveState(eOLEInPlaceActiveState State)noexcept
{
	sIntn Offset=static_cast<sIntn>(State)-static_cast<sIntn>(fActiveState);
	if(Offset==0)
		return;
	if(Offset>0){
		IncOLEState(Offset);
	}
	else{
		DecOLEState(-Offset);
	}
}
//---------------------------------------------------------------------------
void cOLEInPlaceControl::IncOLEState(uIntn Count)noexcept
{
	cnLib_ASSERT(Count!=0);
	switch(fActiveState){
	case OLEInPlaceActiveState::Inactive:
		if(--Count==0){
			if(OLEActivation==nullptr)
				return;
			OLEActivation->OLEInPlaceActive();
			return;
		}
	case OLEInPlaceActiveState::InPlaceActive:
		if(DisableUIActivation || OLEActivation==nullptr)
			return;
		OLEActivation->OLEUIActive();
		return;
	}
}
//---------------------------------------------------------------------------
void cOLEInPlaceControl::DecOLEState(uIntn Count)noexcept
{
	cnLib_ASSERT(Count!=0);
	switch(fActiveState){
	case OLEInPlaceActiveState::UIActive:
		if(--Count==0){
			fInPlaceObject->UIDeactivate();
			// let fInPlaceSite notify the state change for fOleState
			return;
		}
	case OLEInPlaceActiveState::InPlaceActive:
		// set fOleState in case no notification from InPlaceObject
		fActiveState=OLEInPlaceActiveState::Inactive;
		fInPlaceObject->InPlaceDeactivate();
		if(--Count==0){
			return;
		}
	}
}
//---------------------------------------------------------------------------
void cOLEInPlaceControl::UIStarted(void)noexcept
{
	fHostWindowHandle=GetWindowHandleFromUIView(fView);

	if(ActivateWhenBackground && OLEActivation!=nullptr){
		AutoActive();
	}
}
//---------------------------------------------------------------------------
void cOLEInPlaceControl::UIShow(void)noexcept
{
	if(ActivateWhenShow && OLEActivation!=nullptr){
		AutoActive();
	}
}
//---------------------------------------------------------------------------
void cOLEInPlaceControl::UIHide(void)noexcept
{
	if(DeactivateWhenHide){
		OLEDeactivate();
	}
}
//---------------------------------------------------------------------------
void cOLEInPlaceControl::UIStopped(void)noexcept
{
	OLEDeactivate();
	fHostWindowHandle=nullptr;
}
//---------------------------------------------------------------------------
void cOLEInPlaceControl::UpdateObjectState(void)noexcept
{
	if(fView==nullptr)
		return;
	auto State=fView->GetUIState();
	switch(State){
	case UIState::Background:
		if(ActivateWhenBackground && OLEActivation!=nullptr){
			AutoActive();
		}
		break;
	case UIState::Active:
	case UIState::Inactive:
		if(ActivateWhenShow && OLEActivation!=nullptr){
			AutoActive();
		}
		break;
	}
}
//---------------------------------------------------------------------------
void cOLEInPlaceControl::RectangleChanged(bool,bool)noexcept
{
	SetupInPlaceRect();
}
//---------------------------------------------------------------------------
void cOLEInPlaceControl::SetupInPlaceRect(void)noexcept
{
	if(fInPlaceObject==nullptr){
		return;
	}
	if(fActiveState>OLEInPlaceActiveState::Inactive){
		auto ClientRC=OLEGetInPlaceRect();
		fInPlaceObject->SetObjectRects(&ClientRC,&ClientRC);
	}
}
//---------------------------------------------------------------------------
void cOLEInPlaceControl::AutoActive(void)noexcept
{
	if(UIActivateOnEvent && !DisableUIActivation){
		if(fActiveState<OLEInPlaceActiveState::UIActive){
			OLEActivation->OLEUIActive();
		}
	}
	else{
		if(fActiveState<OLEInPlaceActiveState::InPlaceActive){
			OLEActivation->OLEInPlaceActive();
		}
	}
}
//---------------------------------------------------------------------------
HWND cOLEInPlaceControl::OLEGetWindowHandle(void)noexcept
{
	return fHostWindowHandle;
}
//---------------------------------------------------------------------------
HRESULT cOLEInPlaceControl::OLEGetWindowContext(IOleInPlaceFrame **ppFrame,IOleInPlaceUIWindow **ppDoc,LPRECT lprcPosRect,LPRECT lprcClipRect,LPOLEINPLACEFRAMEINFO lpFrameInfo)noexcept
{
	*ppDoc=nullptr;
	*ppFrame=nullptr;
	if(lpFrameInfo->cb<sizeof(OLEINPLACEFRAMEINFO)){
		return E_FAIL;
	}

	if(FrameConfiguration==nullptr || FrameConfiguration->Frame==nullptr){
		*ppFrame=nullptr;
		*ppDoc=nullptr;
		return S_FALSE;
	}

	FrameConfiguration->Frame->AddRef();
	*ppFrame=FrameConfiguration->Frame;

	if(FrameConfiguration->DocWindow!=nullptr){
		FrameConfiguration->DocWindow->AddRef();
		*ppDoc=FrameConfiguration->DocWindow;
	}
	else{
		*ppDoc=nullptr;
	}

	FrameConfiguration->FrameInfo(lpFrameInfo);

	RECT RectPixel=OLEGetInPlaceRect();
	*lprcPosRect=RectPixel;
	*lprcClipRect=RectPixel;
	return S_OK;
}
//---------------------------------------------------------------------------
void cOLEInPlaceControl::OLEDeactivate(void)noexcept
{
	if(fInPlaceObject==nullptr){
		return;
	}
	switch(fActiveState){
	case OLEInPlaceActiveState::UIActive:
		fInPlaceObject->UIDeactivate();
		cnLib_SWITCH_FALLTHROUGH;
	case OLEInPlaceActiveState::InPlaceActive:
		fInPlaceObject->InPlaceDeactivate();
		break;
	}
}
//---------------------------------------------------------------------------
void cOLEInPlaceControl::OLEUIDeactivate(void)noexcept
{
	if(fInPlaceObject==nullptr){
		return;
	}

	if(fActiveState==OLEInPlaceActiveState::UIActive){
		fInPlaceObject->UIDeactivate();
	}
}
//---------------------------------------------------------------------------
bool cOLEInPlaceControl::OLECanInPlaceActivate(void)noexcept
{
	if(fInPlaceObject==nullptr)
		return false;
	if(fView==nullptr)
		return false;
	auto ViewState=fView->GetUIState();
	if(ViewState<=UIState::Null){
		return false;
	}

	return true;
}
//---------------------------------------------------------------------------
void cOLEInPlaceControl::OLEOnInPlaceActivate(void)noexcept
{
	fActiveState=OLEInPlaceActiveState::InPlaceActive;
}
//---------------------------------------------------------------------------
void cOLEInPlaceControl::OLEOnUIActivate(void)noexcept
{
	fActiveState=OLEInPlaceActiveState::UIActive;
}
//---------------------------------------------------------------------------
void cOLEInPlaceControl::OLEOnUIDeactivate(void)noexcept
{
	fActiveState=OLEInPlaceActiveState::InPlaceActive;
}
//---------------------------------------------------------------------------
void cOLEInPlaceControl::OLEOnInPlaceDeactivate(void)noexcept
{
	fActiveState=OLEInPlaceActiveState::Inactive;
}
//---------------------------------------------------------------------------
void cOLEInPlaceControl::OLEOnPosRectChange(LPCRECT lprcPosRect)noexcept
{
	cUIPoint Pos;
	Pos.x=lprcPosRect->left;
	Pos.y=lprcPosRect->top;
	cUIPoint Size;
	Size.x=lprcPosRect->right-lprcPosRect->left;
	Size.y=lprcPosRect->bottom-lprcPosRect->top;
	fView->ArrangeRectangle(fView->GetWindow(),Pos,Size);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cOLEInPlaceWindowlessControl::cOLEInPlaceWindowlessControl()noexcept
{
	fDCViewContent=cnWindows::CreateDCViewContent(&fOLEWindowlessPainter);
	fDCViewContent->SetVisible(false);
}
//---------------------------------------------------------------------------
cOLEInPlaceWindowlessControl::~cOLEInPlaceWindowlessControl()noexcept
{
	SetView(nullptr);
}
//---------------------------------------------------------------------------
void cOLEInPlaceWindowlessControl::ViewSetup(void)noexcept
{
	cOLEInPlaceControl::ViewSetup();

	//fWindowViewport=iCast<iWindowViewport>(fView);
	//if(fWindowViewport!=nullptr){
	//	fWindowViewport->InsertWindowViewportHandler(this);
	//}
	fDCViewContent->SetView(fView);
}
//---------------------------------------------------------------------------
void cOLEInPlaceWindowlessControl::ViewClear(void)noexcept
{
	fDCViewContent->SetView(nullptr);
	//if(fWindowViewport!=nullptr){
	//	fWindowViewport->RemoveWindowViewportHandler(this);
	//	fWindowViewport=nullptr;
	//}

	cOLEInPlaceControl::ViewClear();
}
//---------------------------------------------------------------------------
void cOLEInPlaceWindowlessControl::OLEOnInPlaceDeactivate(void)noexcept
{
	fDCViewContent->SetVisible(false);
	fInPlaceObjectWindowless=nullptr;
	fViewObjectWindowless=nullptr;
	fActiveState=OLEInPlaceActiveState::Inactive;
}
//---------------------------------------------------------------------------
void cOLEInPlaceWindowlessControl::OLEOnInPlaceActivateEx(BOOL *pfNoRedraw,DWORD dwFlags)noexcept
{
	if(dwFlags&ACTIVATE_WINDOWLESS){
		if(fInPlaceObject!=nullptr){
			fInPlaceObject->QueryInterface(COMRetPtrT(fInPlaceObjectWindowless));
			fInPlaceObject->QueryInterface(COMRetPtrT(fViewObjectWindowless));
			if(fViewObjectWindowless!=nullptr){
				fDCViewContent->SetVisible(true);
			}
		}
	}
	fActiveState=OLEInPlaceActiveState::InPlaceActive;
}
//---------------------------------------------------------------------------
void cOLEInPlaceWindowlessControl::OLEOnInPlaceDeactivateEx(BOOL fNoRedraw)noexcept
{fNoRedraw;
	fDCViewContent->SetVisible(false);
	fInPlaceObjectWindowless=nullptr;
	fViewObjectWindowless=nullptr;
	fActiveState=OLEInPlaceActiveState::Inactive;
}
//---------------------------------------------------------------------------
bool cOLEInPlaceWindowlessControl::OLERequestUIActivate(void)noexcept
{
	if(DisableUIActivation){
		return false;
	}
	return true;
}
//---------------------------------------------------------------------------
bool cOLEInPlaceWindowlessControl::OLECanWindowlessActivate(void)noexcept
{
	return fWindowViewport!=nullptr;
}
//---------------------------------------------------------------------------
HRESULT cOLEInPlaceWindowlessControl::OLEWindowlessGetCapture(void)noexcept
{
	auto MouseControl=iCast<iUIMouseControl>(fView);
	if(MouseControl!=nullptr){
		if(MouseControl->MouseIsCaptured())
			return S_OK;
	}
	return S_FALSE;
}
//---------------------------------------------------------------------------
HRESULT cOLEInPlaceWindowlessControl::OLEWindowlessSetCapture(BOOL fCapture)noexcept
{
	auto MouseControl=iCast<iUIMouseControl>(fView);
	if(fCapture){
		if(MouseControl!=nullptr){
			if(MouseControl->MouseSetCaptrue())
				return S_OK;
		}
		return S_FALSE;
	}
	else{
		if(MouseControl!=nullptr){
			MouseControl->MouseReleaseCaptrue();
		}
		return S_OK;
	}
}
//---------------------------------------------------------------------------
HRESULT cOLEInPlaceWindowlessControl::OLEWindowlessGetFocus(void)noexcept
{
	auto KeyControl=iCast<iUIKeyControl>(fView);
	if(KeyControl!=nullptr){
		bool Focused=fView==KeyControl->GetFocus();
		if(Focused)
			return S_OK;
	}
	return S_FALSE;
}
//---------------------------------------------------------------------------
HRESULT cOLEInPlaceWindowlessControl::OLEWindowlessSetFocus(BOOL fFocus)noexcept
{
	auto KeyControl=iCast<iUIKeyControl>(fView);
	if(KeyControl!=nullptr){
		if(fFocus){
			KeyControl->SetFocus(true);
		}
		else{
		}
		return S_OK;
	}
	return E_FAIL;
}
//---------------------------------------------------------------------------
HRESULT cOLEInPlaceWindowlessControl::OLEWindowlessGetDC(LPCRECT pRect,DWORD grfFlags,HDC *phDC)noexcept
{
/*	if(grfFlags&OLEDC_NODRAW){
	}
	if(grfFlags&OLEDC_PAINTBKGND){
	}
	if(grfFlags&OLEDC_OFFSCREEN){
	}*/
	*phDC=fDCViewContent->GetDC();
	return S_OK;
}
//---------------------------------------------------------------------------
HRESULT cOLEInPlaceWindowlessControl::OLEWindowlessReleaseDC(HDC hDC)noexcept
{
	fDCViewContent->ReleaseDC(hDC);
	return S_OK;
}
//---------------------------------------------------------------------------
HRESULT cOLEInPlaceWindowlessControl::OLEWindowlessInvalidateRect(LPCRECT pRect,BOOL fErase)noexcept
{fErase;
	fDCViewContent->InvalidateRect(pRect);
	return S_OK;
}
//---------------------------------------------------------------------------
HRESULT cOLEInPlaceWindowlessControl::OLEWindowlessInvalidateRgn(HRGN hRGN,BOOL fErase)noexcept
{hRGN,fErase;
	// no region function, just invalidate all
	auto crc=OLEGetInPlaceRect();
	fDCViewContent->InvalidateRect(&crc);
	return S_OK;
}
//---------------------------------------------------------------------------
HRESULT cOLEInPlaceWindowlessControl::OLEWindowlessScrollRect(INT dx,INT dy,LPCRECT pRectScroll,LPCRECT pRectClip)noexcept
{
	::ScrollWindow(fHostWindowHandle,dx,dy,pRectScroll,pRectClip);
	return S_OK;
}
//---------------------------------------------------------------------------
HRESULT cOLEInPlaceWindowlessControl::OLEWindowlessAdjustRect( 
    /* [out][in] */ __RPC__inout LPRECT prc)noexcept
{
	return S_OK;
}
//---------------------------------------------------------------------------
HRESULT cOLEInPlaceWindowlessControl::OLEWindowlessOnDefWindowMessage( 
    /* [annotation][in] */ 
    _In_  UINT msg,
    /* [annotation][in] */ 
    _In_  WPARAM wParam,
    /* [annotation][in] */ 
    _In_  LPARAM lParam,
    /* [out] */ __RPC__out LRESULT *plResult)noexcept
{
	*plResult=::DefWindowProcW(fHostWindowHandle,msg,wParam,lParam);
	return S_OK;
}
//---------------------------------------------------------------------------
void cOLEInPlaceWindowlessControl::WindowAttached(void)noexcept
{
}
void cOLEInPlaceWindowlessControl::WindowDetached(void)noexcept
{
}
//---------------------------------------------------------------------------
struct cOleInPlaceObjectWindowlessMessageInfo
{
	UINT Code;
};
static const cOleInPlaceObjectWindowlessMessageInfo gOleInPlaceObjectWindowlessMessage[]=
{
	// Code				
	{WM_SETFOCUS,			},	// 7
	{WM_KILLFOCUS,			},	// 8
	{WM_CANCELMODE,			},	// 1F
	{WM_SETCURSOR,			},	// 20
	
	{WM_HELP,				},	// 53

	{WM_KEYDOWN,			},	// 100
	{WM_KEYUP,				},	// 101
	{WM_CHAR,				},	// 102
	{WM_DEADCHAR,			},	// 103
	{WM_SYSKEYDOWN,			},	// 104
	{WM_SYSKEYUP,			},	// 105
	{WM_SYSDEADCHAR,		},	// 107

	{WM_MOUSEMOVE,			},	// 200
	{WM_LBUTTONDOWN,		},	// 201
	{WM_LBUTTONUP,			},	// 202
	{WM_LBUTTONDBLCLK,		},	// 203
	{WM_RBUTTONDOWN,		},	// 204
	{WM_RBUTTONUP,			},	// 205
	{WM_RBUTTONDBLCLK,		},	// 206
	{WM_MBUTTONDOWN,		},	// 207
	{WM_MBUTTONUP,			},	// 208
	{WM_MBUTTONDBLCLK,		},	// 209
	
							
	{WM_IME_SETCONTEXT,		},	// 281
	{WM_IME_NOTIFY,			},	// 282
	{WM_IME_CONTROL,		},	// 283
	{WM_IME_COMPOSITIONFULL,},	// 284
	{WM_IME_SELECT,			},	// 285
	{WM_IME_CHAR,			},	// 286
	{WM_IME_REQUEST,		},	// 288

	{WM_IME_KEYDOWN,		},	// 290
	{WM_IME_KEYUP,			},	// 291
};
//---------------------------------------------------------------------------
bool cOLEInPlaceWindowlessControl::WindowMessage(LRESULT &MsgResult,const cWindowMessageParam &Message)noexcept
{MsgResult,Message;
	return false;
}
//---------------------------------------------------------------------------
void cOLEInPlaceWindowlessControl::WindowMessageProcessed(LRESULT,const cWindowMessageParam &Message)noexcept
{
	if(fInPlaceObjectWindowless){
		uIntn MessageIndex;
		if(cnMemory::ViewBinarySearch(MessageIndex,gOleInPlaceObjectWindowlessMessage,cnMemory::ArrayLength(gOleInPlaceObjectWindowlessMessage),
			[Code=Message.Code](const cOleInPlaceObjectWindowlessMessageInfo &MessageInfo){
				sIntn d=MessageInfo.Code-Code;
				if(d<0)
					d=-1;
				if(d>0)
					d=1;
				return static_cast<sInt8>(d);
			}
		)){
			// pass to object
			LRESULT Result;
			HRESULT hr=fInPlaceObjectWindowless->OnWindowMessage(Message.Code,Message.wParam,Message.lParam,&Result);
			if(hr==S_OK){
				// processed by object
			}
			else if(hr==S_FALSE){
				// should pass to default
			}
		}
	}
}
//---------------------------------------------------------------------------
cOLEInPlaceWindowlessControl *cOLEInPlaceWindowlessControl::cOLEWindowlessPainter::GetHost()noexcept
{
	return cnMemory::GetObjectFromMemberPointer(this,&cOLEInPlaceWindowlessControl::fOLEWindowlessPainter);
}
//---------------------------------------------------------------------------
void cOLEInPlaceWindowlessControl::cOLEWindowlessPainter::PaintStarted(void)noexcept{}
void cOLEInPlaceWindowlessControl::cOLEWindowlessPainter::PaintShow(void)noexcept{}
void cOLEInPlaceWindowlessControl::cOLEWindowlessPainter::PaintResume(void)noexcept{}
void cOLEInPlaceWindowlessControl::cOLEWindowlessPainter::PaintPaused(void)noexcept{}
void cOLEInPlaceWindowlessControl::cOLEWindowlessPainter::PaintHide(void)noexcept{}
void cOLEInPlaceWindowlessControl::cOLEWindowlessPainter::PaintStopped(void)noexcept{}
void cOLEInPlaceWindowlessControl::cOLEWindowlessPainter::PaintRectChanged(void)noexcept{}
//---------------------------------------------------------------------------
void cOLEInPlaceWindowlessControl::cOLEWindowlessPainter::Paint(HDC DC,HRGN)noexcept
{
	return GetHost()->WindowlessObjectDraw(DC);
}
//---------------------------------------------------------------------------
void cOLEInPlaceWindowlessControl::WindowlessObjectDraw(HDC DC)noexcept
{
	if(fViewObjectWindowless==nullptr)
		return;
	auto crc=OLEGetInPlaceRect();
	RECTL crcl={crc.left,crc.top,crc.right,crc.bottom};
	fViewObjectWindowless->Draw(DVASPECT_CONTENT,0,nullptr,nullptr,nullptr,DC,&crcl,nullptr,nullptr,0);
}
//---------------------------------------------------------------------------
