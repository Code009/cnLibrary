
#include "WinDNet_WPFUIViewWindow.h"

#pragma comment(lib,"d3d9.lib")

using namespace cnLibrary;
using namespace cnRTL;
using namespace cnWinRTL;
using namespace cnWin;

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
iPtr<iWindow> cnWin::DNetCreateWPFWindow(HWND Parent,const wchar_t *WindowText,DWORD Style,DWORD ExStyle,LONG X,LONG Y,LONG Width,LONG Height,UINT ChildID)
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
iPtr<iUIWindow> cnWin::DNetCreateWPFWindowAsWindowClient(mcDNetUIThreadDispatcher *Dispatcher,mcWPFViewRoot::mcConstructParameter &Parameter)
{
	return iCreate<cWPFWindowAsWindowClient>(static_cast<cDNetUIThread*>(Dispatcher),Parameter);
}
//---------------------------------------------------------------------------
iPtr<iWindowClient> cnWin::DNetCreateWindowClient(mcDNetUIThreadDispatcher *Dispatcher,mcWPFViewRoot::mcConstructParameter &Parameter)
{
	return iCreate<cWPFWindowClient>(static_cast<cDNetUIThread*>(Dispatcher),Parameter);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void mcWPFViewRoot::nDispatcherFinishNotify(bool Shutdown)
{
	if(Shutdown==false){
		// called by VirtualStopped
		auto UIWindow=static_cast<cWPFUIWindow*>(this);
		UIWindow->VirtualDelete();
	}
}
//---------------------------------------------------------------------------
iUIWindow* mcWPFViewRoot::nGetUIWindowInterface(void)
{
	return static_cast<cWPFUIWindow*>(this);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cWPFUIWindow::cWPFUIWindow(cDNetUIThread *UIThread,mcConstructParameter &Parameter)
	: fUIThread(UIThread)
	, mcWPFViewRoot(Parameter)
{
	cnLib_ASSERT(fUIThread->mIsCurrent());
}
//---------------------------------------------------------------------------
cWPFUIWindow::~cWPFUIWindow()
{
}
//---------------------------------------------------------------------------
void cWPFUIWindow::VirtualStarted(void)
{
	fUIThread->mAttachDispatcher(&fDispatcherFinishNotify);
}
//---------------------------------------------------------------------------
void cWPFUIWindow::VirtualStopped(void)
{
	if(fUIThread->mDetachDispatcher(&fDispatcherFinishNotify)){
		// wait for notification
		return;
	}

	// already shutdown
	VirtualDelete();
}
//---------------------------------------------------------------------------
void* cWPFUIWindow::CastInterface(iTypeID IID)noexcept(true)
{
	return ImpCastInterface<iUIWindow,iCLIObject,iWindowClient>(this,IID);
}
//---------------------------------------------------------------------------
bool cWPFUIWindow::CheckThread(void)
{
	return fUIThread->IsCurrent();
}
//---------------------------------------------------------------------------
iUIThread* cWPFUIWindow::GetUIThread(void)
{
	return fUIThread;
}
//---------------------------------------------------------------------------
cGCRef& cWPFUIWindow::GetObjecHandle(void)
{
	return fWPFRoot;
}
//---------------------------------------------------------------------------
eUIState cWPFUIWindow::GetUIState(void)
{
	return fWindowState;
}
//---------------------------------------------------------------------------
bool cWPFUIWindow::InsertStateHandler(iUIStateHandler *Handler,sfInt16 Order)
{
	return false;
}
//---------------------------------------------------------------------------
bool cWPFUIWindow::RemoveStateHandler(iUIStateHandler *Handler)
{
	return false;
}
//---------------------------------------------------------------------------
bool cWPFUIWindow::GetVisible(void)
{
	return false;
}
//---------------------------------------------------------------------------
bool cWPFUIWindow::SetVisible(bool Visible)
{
	return false;
}
//---------------------------------------------------------------------------
bool cWPFUIWindow::IsEnabled(void)
{
	return false;
}
//---------------------------------------------------------------------------
bool cWPFUIWindow::GetEnable(void)
{
	return false;
}
//---------------------------------------------------------------------------
bool cWPFUIWindow::SetEnable(bool Enable)
{
	return false;
}
//---------------------------------------------------------------------------
bool cWPFUIWindow::TranslatePointTo(iInterface *Relative,cUIPoint &Position)
{
	return false;
}
//---------------------------------------------------------------------------
bool cWPFUIWindow::MoveTo(iInterface *Relative,cUIPoint Position)
{
	return false;
}
//---------------------------------------------------------------------------
cUIPoint cWPFUIWindow::GetSize(void)
{
	return {0,0};
}
//---------------------------------------------------------------------------
bool cWPFUIWindow::SetSize(cUIPoint Size)
{
	return false;
}
//---------------------------------------------------------------------------
bool cWPFUIWindow::ArrangeRectangle(iInterface *Relative,cUIPoint Position,cUIPoint Size)
{
	return false;
}
//---------------------------------------------------------------------------
Float32 cWPFUIWindow::GetZPosition(void)
{
	return 0;
}
//---------------------------------------------------------------------------
bool cWPFUIWindow::SetZPosition(Float32)
{
	return false;
}
//---------------------------------------------------------------------------
Float32 cWPFUIWindow::GetContentScale(void)
{
	return 1.f;
}
//---------------------------------------------------------------------------
iUIScreen* cWPFUIWindow::GetScreen(void)
{
	return nullptr;
}
//---------------------------------------------------------------------------
iUIWindow* cWPFUIWindow::GetParent(void)
{
	return nullptr;
}
//---------------------------------------------------------------------------
bool cWPFUIWindow::InsertWindowHandler(iUIWindowHandler *Handler,sfInt16 Order)
{
	return false;
}
//---------------------------------------------------------------------------
bool cWPFUIWindow::RemoveWindowHandler(iUIWindowHandler *Handler)
{
	return false;
}
//---------------------------------------------------------------------------
iUIView* cWPFUIWindow::GetClient(void)
{
	return fClientView;
}
//---------------------------------------------------------------------------
bool cWPFUIWindow::SetClient(iUIView *View)
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
void cWPFUIWindow::SetBackgroundColor(COLORREF Color)
{
	if(CheckThread()==false)
		return;
	uInt8 r=Color&0xFF;
	uInt8 g=(Color>>8)&0xFF;
	uInt8 b=(Color>>16)&0xFF;
	mWPFViewRoot_SetBackgroundColor(fWPFRoot,r,g,b);
}
//---------------------------------------------------------------------------
iWindowClient* cWPFUIWindow::WPFWindowGetInterface(void)
{
	return this;
}
//---------------------------------------------------------------------------
void cWPFUIWindow::WPFWindowStateChanged(UIState State)
{
	if(fWindowState==State)
		return;

	fWindowState=State;
	if(fClient!=nullptr){
		fClient->WPFChildTreeNotifyState();
	}
}
//---------------------------------------------------------------------------
void cWPFUIWindow::WPFWindowDPIChanged(Float32 NewScale)
{
	fContentScale=NewScale;
	if(fClient!=nullptr){
		fClient->WPFChildTreeNotifyScale();
	}
}
//---------------------------------------------------------------------------
eUIState cWPFUIWindow::WPFParentGetState(void)
{
	return fWindowState;
}
//---------------------------------------------------------------------------
Float32 cWPFUIWindow::WPFParentGetContentScale(void)
{
	return fContentScale;
}
//---------------------------------------------------------------------------
Float32 cWPFUIWindow::WPFParentGetLayoutScale(void)
{
	return 1.f;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cWPFWindowAsWindowClient::cWPFWindowAsWindowClient(cDNetUIThread *UIThread,mcWPFViewRoot::mcConstructParameter &Parameter)
	: cWPFUIWindow(UIThread,Parameter)
{
}
//---------------------------------------------------------------------------
cWPFWindowAsWindowClient::~cWPFWindowAsWindowClient()
{
}
//---------------------------------------------------------------------------
iWindow* cWPFWindowAsWindowClient::GetWindow(void)
{
	return nullptr;
	//return fWindowHost;
}
//---------------------------------------------------------------------------
bool cWPFWindowAsWindowClient::SetWindow(iWindow *)
{
	return false;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void mcWPFWindow::nDispatcherFinishNotify(bool Shutdown)
{
	if(Shutdown==false){
		// called by VirtualStopped
		auto Window=static_cast<cWPFWindow*>(this);
		Window->VirtualDelete();
	}
}
//---------------------------------------------------------------------------
SIZE mcWPFWindow::GetClientSize(void)
{
	RECT rc;
	if(::GetClientRect(fWindowHandle,&rc)){
		return {rc.right-rc.left,rc.bottom-rc.top};
	}
	return {0,0};
}
//---------------------------------------------------------------------------
void mcWPFWindow::nWPFSourceAttach(void)
{
	auto Window=static_cast<cWPFWindow*>(this);
	Window->WindowAttach();
}
//---------------------------------------------------------------------------
void mcWPFWindow::nWPFSourceDetach(void)
{
	auto Window=static_cast<cWPFWindow*>(this);
	Window->WindowDetach();
}
//---------------------------------------------------------------------------
bool mcWPFWindow::nWPFMessage(LRESULT &Result,HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	auto Window=static_cast<cWPFWindow*>(this);

	return Window->WindowMessage(Result,hwnd,msg,wParam,lParam);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cWPFWindow::cWPFWindow(cDNetUIThread *UIThread)
	: fUIThread(UIThread)
	//, fWindowDPI(96)
{
	auto Registration=cnSystem::GetSystemDependentRegistration();
	Registration->Register(this);
}
//---------------------------------------------------------------------------
cWPFWindow::~cWPFWindow()
{
	auto Registration=cnSystem::GetSystemDependentRegistration();
	Registration->Unregister(this);
}
//---------------------------------------------------------------------------
void cWPFWindow::VirtualStarted(void)
{
	fUIThread->mAttachDispatcher(&fDispatcherFinishNotify);
}
//---------------------------------------------------------------------------
void cWPFWindow::VirtualStopped(void)
{
	if(fUIThread->mDetachDispatcher(&fDispatcherFinishNotify)){
		// wait for notification
		return;
	}
	// already shutdown
	VirtualDelete();
}
//---------------------------------------------------------------------------
rPtr< iArrayReference<const uChar16> > cWPFWindow::DependentCreateDescription(void)
{
	cString<uChar16> Desc=cnRTL::CreateStringFormat(u"cWPFWindow %p",this);
	return Desc.Token();
}
//---------------------------------------------------------------------------
void cWPFWindow::DependentShutdownNotification(void)
{
}
//---------------------------------------------------------------------------
void* cWPFWindow::CastInterface(iTypeID IID)noexcept(true)
{
	return ImpCastInterface<iCLIObject,iWindow>(this,IID);
}
//---------------------------------------------------------------------------
HWND cWPFWindow::GetHandle(void)
{
	return fWindowHandle;
}
//---------------------------------------------------------------------------
bool cWPFWindow::CheckThread(void)
{
	return fUIThread->IsCurrent();
}
//---------------------------------------------------------------------------
cGCRef& cWPFWindow::GetObjecHandle(void)
{
	return fWPFWindow;
}
//---------------------------------------------------------------------------
bool cWPFWindow::WindowMessage(LRESULT &MessageResult,HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
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
void cWPFWindow::WindowAttach(void)
{
	fWindowState.WindowAttached(fWindowHandle);
	fWindowDPI=GetWindowDPI(fWindowHandle);
	fWindowLayoutScale=fWindowDPI/96.f;

	for(auto &Handler : fHandlers){
		Handler->WindowAttached();
	}
}
//---------------------------------------------------------------------------
void cWPFWindow::WindowDetach(void)
{
	fWindowState.WindowDetached();

	for(auto &Handler : fHandlers){
		Handler->WindowDetached();
	}
}
//---------------------------------------------------------------------------
cDNetUIThread* cWPFWindow::GetWPFUIThread(void)
{
	return fUIThread;
}
//---------------------------------------------------------------------------
iUIThread* cWPFWindow::GetUIThread(void)
{
	return fUIThread;
}
//---------------------------------------------------------------------------
eUIState cWPFWindow::GetUIState(void)const
{
	return fWindowState.GetUIState();
}
//---------------------------------------------------------------------------
Float32 cWPFWindow::GetLayoutScale(void)const
{
	return fWindowLayoutScale;
}
//---------------------------------------------------------------------------
Float32 cWPFWindow::GetContentScale(void)const
{
	return fWindowLayoutScale;
;
}
//---------------------------------------------------------------------------
HWND cWPFWindow::GetWindowHandle(void)
{
	return fWindowHandle;
}
//---------------------------------------------------------------------------
bool cWPFWindow::InsertMessageHandler(iWindowMessageHandler *Handler,sfInt8 Order)
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
bool cWPFWindow::RemoveMessageHandler(iWindowMessageHandler *Handler)
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
iWindowClient* cWPFWindow::GetClient(void)
{
	if(fWindowClient!=nullptr)
		return fWindowClient->WPFWindowGetInterface();

	return nullptr;
}
//---------------------------------------------------------------------------
iWindowFrame* cWPFWindow::GetFrame(void)
{
	return nullptr;
}
//---------------------------------------------------------------------------
rPtr<iVariable> cWPFWindow::QueryAffixedVariable(const void *Token)
{
	return fAffixedVariableSet.QueryAffixedVariable(Token);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cWPFWindowClient::cWPFWindowClient(cDNetUIThread *UIThread,mcWPFViewRoot::mcConstructParameter &Parameter)
	: cWPFUIWindow(UIThread,Parameter)
{
}
//---------------------------------------------------------------------------
cWPFWindowClient::~cWPFWindowClient()
{
}
//---------------------------------------------------------------------------
iWindow* cWPFWindowClient::GetWindow(void)
{
	return fWindowHost;
}
//---------------------------------------------------------------------------
bool cWPFWindowClient::SetWindow(iWindow *Window)
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
bool cWPFWindowClient::WPFSetupWindow(iCLIObject *Object)
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
bool cWPFWindowClient::WPFClearWindow(void)
{
	if(fWindowHost==nullptr)
		return true;

	return fWindowHost->mWindowDetachClient(this);
}
//---------------------------------------------------------------------------
void cWPFWindowClient::WPFWindowStateChanged(eUIState NewState)
{
	cWPFUIWindow::WPFWindowStateChanged(NewState);

	if(fClient!=nullptr){
		fClient->WPFChildTreeNotifyState();
	}
}
//---------------------------------------------------------------------------
void cWPFWindowClient::WPFWindowDPIChanged(Float32 NewScale)
{
	cWPFUIWindow::WPFWindowDPIChanged(NewScale);

	if(fClient!=nullptr){
		fClient->WPFChildTreeNotifyScale();
	}
}
//---------------------------------------------------------------------------
SIZE cWPFWindowClient::GetClientSize(void)
{
	auto WindowHandle=fWindowHost->GetHandle();
	
	RECT rc;
	if(::GetClientRect(WindowHandle,&rc)){
		return {rc.right-rc.left,rc.bottom-rc.top};
	}
	return {0,0};
}
//---------------------------------------------------------------------------
Float32 cWPFWindowClient::WPFParentGetLayoutScale(void)
{
	if(fWindowHost==nullptr)
		return 1.f;
	return fWindowHost->GetLayoutScale();
}
//---------------------------------------------------------------------------
