
#include "WinDNet_WPFUIViewWindow.h"

#pragma comment(lib,"d3d9.lib")

using namespace cnLibrary;
using namespace cnRTL;
using namespace cnWinRTL;
using namespace cnWin;

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
iPtr<iWindow> cnWin::DNetCreateWPFWindow(HWND Parent,const wchar_t *WindowText,DWORD Style,DWORD ExStyle,LONG X,LONG Y,LONG Width,LONG Height,UINT ChildID)noexcept
{ChildID;
	auto UIThread=cDNetUIThread::CurrentUIThread();
	if(UIThread==nullptr)
		return nullptr;
	auto Window=iCreate<cWPFWindow>(UIThread);
	Style&=~WS_CHILD;
	if(Window->Create(Parent,WindowText,Style,ExStyle,X,Y,Width,Height)==false){
		return nullptr;
	}

	return Window;
}
//---------------------------------------------------------------------------
iPtr<iUIWindow> cnWin::DNetCreateWPFWindowAsWindowClient(mcDNetUIThreadDispatcher *Dispatcher,mcWPFViewRoot::mcConstructParameter &Parameter)noexcept
{
	return iCreate<cWPFWindowAsWindowClient>(static_cast<cDNetUIThread*>(Dispatcher),Parameter);
}
//---------------------------------------------------------------------------
iPtr<iWindowClient> cnWin::DNetCreateWindowClient(mcDNetUIThreadDispatcher *Dispatcher,mcWPFViewRoot::mcConstructParameter &Parameter)noexcept
{
	return iCreate<cWPFWindowClient>(static_cast<cDNetUIThread*>(Dispatcher),Parameter);
}
//---------------------------------------------------------------------------
rPtr<iPopupWindowControl> cnWin::DNetCreatePopupWindowControl(mcDNetUIThreadDispatcher *Dispatcher,mcWPFViewRoot::mcConstructParameter &Parameter)noexcept
{
	return iCreate<cWPFPopupWindowControl>(static_cast<cDNetUIThread*>(Dispatcher),Parameter);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void mcWPFViewRoot::nDispatcherFinishNotify(bool Shutdown)noexcept
{
	if(Shutdown==false){
		// called by VirtualStopped
		auto UIWindow=static_cast<cWPFUIWindow*>(this);
		UIWindow->VirtualDelete();
	}
}
//---------------------------------------------------------------------------
iUIWindow* mcWPFViewRoot::nGetUIWindowInterface(void)noexcept
{
	return static_cast<cWPFUIWindow*>(this);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cWPFUIWindow::cWPFUIWindow(cDNetUIThread *UIThread,mcConstructParameter &Parameter)noexcept
	: fUIThread(UIThread)
	, mcWPFViewRoot(Parameter)
{
	cnLib_ASSERT(fUIThread->mIsCurrent());
}
//---------------------------------------------------------------------------
cWPFUIWindow::~cWPFUIWindow()noexcept
{
}
//---------------------------------------------------------------------------
void cWPFUIWindow::VirtualStarted(void)noexcept
{
	fUIThread->mAttachDispatcher(&fDispatcherFinishNotify);
}
//---------------------------------------------------------------------------
void cWPFUIWindow::VirtualStopped(void)noexcept
{
	if(fUIThread->mDetachDispatcher(&fDispatcherFinishNotify)){
		// wait for notification
		return;
	}

	// already shutdown
	VirtualDelete();
}
//---------------------------------------------------------------------------
void* cWPFUIWindow::CastInterface(iTypeID IID)noexcept
{
	return ImpCastInterface<iUIWindow,iCLIObject,iWindowClient>(this,IID);
}
//---------------------------------------------------------------------------
bool cWPFUIWindow::CheckThread(void)noexcept
{
	return fUIThread->IsCurrent();
}
//---------------------------------------------------------------------------
iUIThread* cWPFUIWindow::GetUIThread(void)noexcept
{
	return fUIThread;
}
//---------------------------------------------------------------------------
cGCRef& cWPFUIWindow::GetObjecHandle(void)noexcept
{
	return fWPFRoot;
}
//---------------------------------------------------------------------------
eUIState cWPFUIWindow::GetUIState(void)noexcept
{
	return fWindowState;
}
//---------------------------------------------------------------------------
bool cWPFUIWindow::InsertStateHandler(iUIStateHandler *Handler,sfInt16 Order)noexcept
{
	return false;
}
//---------------------------------------------------------------------------
bool cWPFUIWindow::RemoveStateHandler(iUIStateHandler *Handler)noexcept
{
	return false;
}
//---------------------------------------------------------------------------
bool cWPFUIWindow::GetVisible(void)noexcept
{
	return GetWindowVisible();
}
//---------------------------------------------------------------------------
bool cWPFUIWindow::SetVisible(bool Visible)noexcept
{
	return SetWindowVisible(Visible);
}
//---------------------------------------------------------------------------
bool cWPFUIWindow::IsEnabled(void)noexcept
{
	return false;
}
//---------------------------------------------------------------------------
bool cWPFUIWindow::GetEnable(void)noexcept
{
	return false;
}
//---------------------------------------------------------------------------
bool cWPFUIWindow::SetEnable(bool Enable)noexcept
{
	return false;
}
//---------------------------------------------------------------------------
bool cWPFUIWindow::TranslatePointTo(iInterface *Relative,cUIPoint &Position)noexcept
{
	return false;
}
//---------------------------------------------------------------------------
bool cWPFUIWindow::MoveTo(iInterface *Relative,cUIPoint Position)noexcept
{
	return false;
}
//---------------------------------------------------------------------------
cUIPoint cWPFUIWindow::GetSize(void)noexcept
{
	return {0,0};
}
//---------------------------------------------------------------------------
bool cWPFUIWindow::SetSize(cUIPoint Size)noexcept
{
	return false;
}
//---------------------------------------------------------------------------
bool cWPFUIWindow::ArrangeRectangle(iInterface *Relative,cUIPoint Position,cUIPoint Size)noexcept
{
	return false;
}
//---------------------------------------------------------------------------
Float32 cWPFUIWindow::GetZPosition(void)noexcept
{
	return 0;
}
//---------------------------------------------------------------------------
bool cWPFUIWindow::SetZPosition(Float32)noexcept
{
	return false;
}
//---------------------------------------------------------------------------
Float32 cWPFUIWindow::GetContentScale(void)noexcept
{
	return 1.f;
}
//---------------------------------------------------------------------------
iUIScreen* cWPFUIWindow::GetScreen(void)noexcept
{
	return nullptr;
}
//---------------------------------------------------------------------------
iUIWindow* cWPFUIWindow::GetParent(void)noexcept
{
	return nullptr;
}
//---------------------------------------------------------------------------
bool cWPFUIWindow::InsertWindowHandler(iUIWindowHandler *Handler,sfInt16 Order)noexcept
{
	return false;
}
//---------------------------------------------------------------------------
bool cWPFUIWindow::RemoveWindowHandler(iUIWindowHandler *Handler)noexcept
{
	return false;
}
//---------------------------------------------------------------------------
iUIView* cWPFUIWindow::GetClient(void)noexcept
{
	return fClientView;
}
//---------------------------------------------------------------------------
bool cWPFUIWindow::SetClient(iUIView *View)noexcept
{
	if(CheckThread()==false)
		return false;

	if(View==nullptr){
		mResetClient();
		return true;
	}
	auto CLIObject=iCast<iCLIObject>(View);
	if(CLIObject==nullptr)
		return false;

	auto &ObjectHandle=CLIObject->GetObjecHandle();
	if(mSetClient(ObjectHandle)){
		fClient->WPFChildTreeNotifyWindow(this);
		fClient->WPFChildTreeNotifyScale();
		fClientView=View;
		return true;
	}
	else{
		return false;
	}
}
//---------------------------------------------------------------------------
void cWPFUIWindow::SetBackgroundColor(COLORREF Color)noexcept
{
	if(CheckThread()==false)
		return;
	uInt8 r=Color&0xFF;
	uInt8 g=(Color>>8)&0xFF;
	uInt8 b=(Color>>16)&0xFF;
	mWPFViewRoot_SetBackgroundColor(fWPFRoot,r,g,b);
}
//---------------------------------------------------------------------------
iWindowClient* cWPFUIWindow::WPFWindowGetInterface(void)noexcept
{
	return this;
}
//---------------------------------------------------------------------------
void cWPFUIWindow::WPFWindowStateChanged(UIState State)noexcept
{
	if(fWindowState==State)
		return;

	fWindowState=State;
	if(fClient!=nullptr){
		fClient->WPFChildTreeNotifyState();
	}
}
//---------------------------------------------------------------------------
void cWPFUIWindow::WPFWindowDPIChanged(Float32 NewScale)noexcept
{
	fContentScale=NewScale;
	if(fClient!=nullptr){
		fClient->WPFChildTreeNotifyScale();
	}
}
//---------------------------------------------------------------------------
eUIState cWPFUIWindow::WPFParentGetState(void)noexcept
{
	return fWindowState;
}
//---------------------------------------------------------------------------
Float32 cWPFUIWindow::WPFParentGetContentScale(void)noexcept
{
	return fContentScale;
}
//---------------------------------------------------------------------------
Float32 cWPFUIWindow::WPFParentGetLayoutScale(void)noexcept
{
	return 1.f;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cWPFWindowAsWindowClient::cWPFWindowAsWindowClient(cDNetUIThread *UIThread,mcWPFViewRoot::mcConstructParameter &Parameter)noexcept
	: cWPFUIWindow(UIThread,Parameter)
{
}
//---------------------------------------------------------------------------
cWPFWindowAsWindowClient::~cWPFWindowAsWindowClient()noexcept
{
}
//---------------------------------------------------------------------------
iWindow* cWPFWindowAsWindowClient::GetWindow(void)noexcept
{
	return nullptr;
	//return fWindowHost;
}
//---------------------------------------------------------------------------
bool cWPFWindowAsWindowClient::SetWindow(iWindow *)noexcept
{
	return false;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void mcWPFWindow::nDispatcherFinishNotify(bool Shutdown)noexcept
{
	if(Shutdown==false){
		// called by VirtualStopped
		auto Window=static_cast<cWPFWindow*>(this);
		Window->VirtualDelete();
	}
}
//---------------------------------------------------------------------------
SIZE mcWPFWindow::GetClientSize(void)noexcept
{
	RECT rc;
	if(::GetClientRect(fWindowHandle,&rc)){
		return {rc.right-rc.left,rc.bottom-rc.top};
	}
	return {0,0};
}
//---------------------------------------------------------------------------
void mcWPFWindow::nWPFSourceAttach(void)noexcept
{
	auto Window=static_cast<cWPFWindow*>(this);
	Window->WindowAttach();
}
//---------------------------------------------------------------------------
void mcWPFWindow::nWPFSourceDetach(void)noexcept
{
	auto Window=static_cast<cWPFWindow*>(this);
	Window->WindowDetach();
}
//---------------------------------------------------------------------------
bool mcWPFWindow::nWPFMessage(LRESULT &Result,HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)noexcept
{
	auto Window=static_cast<cWPFWindow*>(this);

	return Window->WindowMessage(Result,hwnd,msg,wParam,lParam);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cWPFWindow::cWPFWindow(cDNetUIThread *UIThread)noexcept
	: fUIThread(UIThread)
	//, fWindowDPI(96)
{
	cnSystem::SystemDependentRegistration->Register(this);
}
//---------------------------------------------------------------------------
cWPFWindow::~cWPFWindow()noexcept
{
	cnSystem::SystemDependentRegistration->Unregister(this);
}
//---------------------------------------------------------------------------
void cWPFWindow::VirtualStarted(void)noexcept
{
	fUIThread->mAttachDispatcher(&fDispatcherFinishNotify);
}
//---------------------------------------------------------------------------
void cWPFWindow::VirtualStopped(void)noexcept
{
	if(fUIThread->mDetachDispatcher(&fDispatcherFinishNotify)){
		// wait for notification
		return;
	}
	// already shutdown
	VirtualDelete();
}
//---------------------------------------------------------------------------
rPtr<iStringReference> cWPFWindow::DependentCreateDescription(void)noexcept
{
	cString<uChar16> Desc=cnRTL::CreateStringFormat(u"cWPFWindow %p",this);
	return Desc.Token();
}
//---------------------------------------------------------------------------
void cWPFWindow::DependentShutdownNotification(void)noexcept
{
}
//---------------------------------------------------------------------------
void* cWPFWindow::CastInterface(iTypeID IID)noexcept
{
	return ImpCastInterface<iCLIObject,iWindow>(this,IID);
}
//---------------------------------------------------------------------------
HWND cWPFWindow::GetHandle(void)noexcept
{
	return fWindowHandle;
}
//---------------------------------------------------------------------------
bool cWPFWindow::CheckThread(void)noexcept
{
	return fUIThread->IsCurrent();
}
//---------------------------------------------------------------------------
cGCRef& cWPFWindow::GetObjecHandle(void)noexcept
{
	return fWPFWindow;
}
//---------------------------------------------------------------------------
bool cWPFWindow::WindowMessage(LRESULT &MessageResult,HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)noexcept
{
	switch(msg){
	case WM_DPICHANGED:
	{
		WORD NewDPI=LOWORD(wParam);
		if(NewDPI!=fWindowDPI){
			// change dpi
			fWindowDPI=NewDPI;
			fWindowLayoutScale=fWindowDPI/96.f;
			if(fWindowClient!=nullptr){
				fWindowClient->WPFWindowDPIChanged(GetContentScale());
			}
		}
		// resize window
		LPRECT lprNewRect = reinterpret_cast<LPRECT>(lParam);
		SetWindowPos(hwnd, nullptr, lprNewRect->left, lprNewRect->top, lprNewRect->right - lprNewRect->left, lprNewRect->bottom - lprNewRect->top, 
			SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_NOACTIVATE);
	}
		break;
	}

	cWindowMessageParam MessageParam;
	MessageParam.Code=msg;
	MessageParam.wParam=wParam;
	MessageParam.lParam=lParam;

	if(fWindowState.WindowMessageCheckState(MessageParam)){
		// ui state changed
		if(fWindowClient!=nullptr){
			auto NewState=fWindowState.GetUIState();
			fWindowClient->WPFWindowStateChanged(NewState);
		}
	}

	bool Processed=false;
	for(auto &Handler : fHandlers){
		if(Handler->WindowMessage(MessageResult,MessageParam)){
			Processed=true;
			break;
		}
	}

	for(auto &Handler : fHandlers){
		Handler->WindowMessageProcessed(MessageResult,MessageParam);
	}

	return Processed;
}
//---------------------------------------------------------------------------
void cWPFWindow::WindowAttach(void)noexcept
{
	fWindowState.WindowAttached(fWindowHandle);
	fWindowDPI=GetWindowDPI(fWindowHandle);
	fWindowLayoutScale=fWindowDPI/96.f;

	for(auto &Handler : fHandlers){
		Handler->WindowAttached();
	}
}
//---------------------------------------------------------------------------
void cWPFWindow::WindowDetach(void)noexcept
{
	fWindowState.WindowDetached();

	for(auto &Handler : fHandlers){
		Handler->WindowDetached();
	}
}
//---------------------------------------------------------------------------
cDNetUIThread* cWPFWindow::GetWPFUIThread(void)noexcept
{
	return fUIThread;
}
//---------------------------------------------------------------------------
iUIThread* cWPFWindow::GetUIThread(void)noexcept
{
	return fUIThread;
}
//---------------------------------------------------------------------------
eUIState cWPFWindow::GetUIState(void)const noexcept
{
	return fWindowState.GetUIState();
}
//---------------------------------------------------------------------------
Float32 cWPFWindow::GetLayoutScale(void)const noexcept
{
	return fWindowLayoutScale;
}
//---------------------------------------------------------------------------
Float32 cWPFWindow::GetContentScale(void)const noexcept
{
	return fWindowLayoutScale;
;
}
//---------------------------------------------------------------------------
HWND cWPFWindow::GetWindowHandle(void)noexcept
{
	return fWindowHandle;
}
//---------------------------------------------------------------------------
bool cWPFWindow::InsertMessageHandler(iWindowMessageHandler *Handler,sfInt8 Order)noexcept
{
	if(CheckThread()==false)
		return false;

	if(fHandlers.Insert(Handler,Order)){
		if(fWindowHandle!=nullptr){
			Handler->WindowAttached();
		}
	}
	return false;
}
//---------------------------------------------------------------------------
bool cWPFWindow::RemoveMessageHandler(iWindowMessageHandler *Handler)noexcept
{
	if(CheckThread()==false)
		return false;
	if(fHandlers.Remove(Handler)){
		if(fWindowHandle!=nullptr){
			Handler->WindowDetached();
		}
	}
	return false;
}
//---------------------------------------------------------------------------
iWindowClient* cWPFWindow::GetClient(void)noexcept
{
	if(fWindowClient!=nullptr)
		return fWindowClient->WPFWindowGetInterface();

	return nullptr;
}
//---------------------------------------------------------------------------
iWindowFrame* cWPFWindow::GetFrame(void)noexcept
{
	return nullptr;
}
//---------------------------------------------------------------------------
rPtr<iVariable> cWPFWindow::QueryAffixedVariable(const void *Token)noexcept
{
	return fAffixedVariableSet.QueryAffixedVariable(Token);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cWPFWindowClient::cWPFWindowClient(cDNetUIThread *UIThread,mcWPFViewRoot::mcConstructParameter &Parameter)noexcept
	: cWPFUIWindow(UIThread,Parameter)
{
}
//---------------------------------------------------------------------------
cWPFWindowClient::~cWPFWindowClient()noexcept
{
}
//---------------------------------------------------------------------------
iWindow* cWPFWindowClient::GetWindow(void)noexcept
{
	return fWindowHost;
}
//---------------------------------------------------------------------------
bool cWPFWindowClient::SetWindow(iWindow *Window)noexcept
{
	if(fUIThread->IsCurrent()==false)
		return false;

	if(Window==nullptr){
		return WPFClearWindow();
	}
	auto DNetObject=iCast<iCLIObject>(Window);
	if(DNetObject==nullptr)
		return false;
	
	if(WPFClearWindow()==false)
		return false;

	return WPFSetupWindow(DNetObject);
}
//---------------------------------------------------------------------------
bool cWPFWindowClient::WPFSetupWindow(iCLIObject *Object)noexcept
{
	fWindowHost=static_cast<cWPFWindow*>(mcWPFWindow::mWindowAttachClient(Object->GetObjecHandle(),this,this));
	if(fWindowHost==nullptr){
		return false;
	}

	if(fClient!=nullptr){
		fClient->WPFChildTreeNotifyState();
	}

	return true;
}
//---------------------------------------------------------------------------
bool cWPFWindowClient::WPFClearWindow(void)noexcept
{
	if(fWindowHost==nullptr)
		return true;

	return fWindowHost->mWindowDetachClient(this);
}
//---------------------------------------------------------------------------
void cWPFWindowClient::WPFWindowStateChanged(eUIState NewState)noexcept
{
	cWPFUIWindow::WPFWindowStateChanged(NewState);

	if(fClient!=nullptr){
		fClient->WPFChildTreeNotifyState();
	}
}
//---------------------------------------------------------------------------
void cWPFWindowClient::WPFWindowDPIChanged(Float32 NewScale)noexcept
{
	cWPFUIWindow::WPFWindowDPIChanged(NewScale);

	if(fClient!=nullptr){
		fClient->WPFChildTreeNotifyScale();
	}
}
//---------------------------------------------------------------------------
SIZE cWPFWindowClient::GetClientSize(void)noexcept
{
	auto WindowHandle=fWindowHost->GetHandle();
	
	RECT rc;
	if(::GetClientRect(WindowHandle,&rc)){
		return {rc.right-rc.left,rc.bottom-rc.top};
	}
	return {0,0};
}
//---------------------------------------------------------------------------
Float32 cWPFWindowClient::WPFParentGetLayoutScale(void)noexcept
{
	if(fWindowHost==nullptr)
		return 1.f;
	return fWindowHost->GetLayoutScale();
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cWPFPopupWindowControl::cWPFPopupWindowControl(cDNetUIThread *UIThread,mcWPFViewRoot::mcConstructParameter &Parameter)noexcept
	: cWPFWindowClient(UIThread,Parameter)
{
}
//---------------------------------------------------------------------------
cWPFPopupWindowControl::~cWPFPopupWindowControl()noexcept
{
}
//---------------------------------------------------------------------------
void cWPFPopupWindowControl::SetCallback(iPopupWindowControlCallback *Callback)noexcept
{
	fCallback=Callback;
}
//---------------------------------------------------------------------------
iUIWindow* cWPFPopupWindowControl::GetUIWindow(void)noexcept
{
	return this;
}
//---------------------------------------------------------------------------
bool cWPFPopupWindowControl::SetupOwner(iUIView *View)noexcept
{
	if(View==nullptr){
		ClearOwner();
		return true;
	}
	auto Window=View->GetWindow();
	if(Window==nullptr){
		ClearOwner();
		return true;
	}
	auto CLIWindow=iCast<iCLIObject>(Window);
	if(CLIWindow==nullptr){
		return false;
	}

	auto &WPFRootHandle=CLIWindow->GetObjecHandle();
	return SetWindowOwner(WPFRootHandle);
}
//---------------------------------------------------------------------------
void cWPFPopupWindowControl::ClearOwner(void)noexcept
{
	return ClearWindowOwner();
}
//---------------------------------------------------------------------------
void cWPFPopupWindowControl::ShowPopup(void)noexcept
{
	SetWindowVisible(true);
}
//---------------------------------------------------------------------------
void cWPFPopupWindowControl::HidePopup(void)noexcept
{
	SetWindowVisible(false);
}
//---------------------------------------------------------------------------
