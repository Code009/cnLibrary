#include "WinCLI_WPFUIWindow.h"

#pragma comment(lib,"d3d9.lib")

using namespace cnLibrary;
using namespace cnRTL;
using namespace cnWinRTL;
using namespace cnWin;

cnLib_INTERFACE_LOCALID_DEFINE(cWPFWindow);
cnLib_INTERFACE_LOCALID_DEFINE(iWPFWindowClient);

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
iPtr<iWindowClient> cnWin::WPFCreateWindowClient(void)noexcept(true)
{
	auto UIThread=cWPFUIThread::CurrentUIThread();
	if(UIThread==nullptr)
		return nullptr;

	cGCRef WPFViewRootHandle;
	mMakeWPFViewRoot(WPFViewRootHandle);
	return iCreate<cWPFWindowClient>(UIThread,WPFViewRootHandle);
}
//---------------------------------------------------------------------------
rPtr<iPopupWindowControl> cnWin::WPFCreatePopupWindowControl(void)noexcept(true)
{
	auto UIThread=cWPFUIThread::CurrentUIThread();
	if(UIThread==nullptr)
		return nullptr;

	cGCRef WPFViewRootHandle;
	mMakeWPFViewRoot(WPFViewRootHandle);
	return rCreate<cWPFPopupWindowControl>(UIThread,WPFViewRootHandle);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cWPFWindow::cWPFWindow(cWPFUIThread *UIThread)noexcept(true)
	: fUIThread(UIThread)
	, fWindowDPI(96)
{
	mbcWPFWindow::mSetup();
	gWPFModule.ObjectRegistration.Acquire(this);
}
//---------------------------------------------------------------------------
cWPFWindow::~cWPFWindow()noexcept(true)
{
	gWPFModule.ObjectRegistration.Release(this);
	mbcWPFWindow::mClear();
}
//---------------------------------------------------------------------------
void cWPFWindow::VirtualStarted(void)noexcept(true)
{
}
//---------------------------------------------------------------------------
void cWPFWindow::VirtualStopped(void)noexcept(true)
{
	if(fUIThread->IsCurrent()){
		Cleanup();
	}
	else{
		fUIThread->mDispatchExecute(true,&fExitParentProcedure);
	}
}
//---------------------------------------------------------------------------
void cWPFWindow::cExitParentProcedure::Execute(void)noexcept(true)
{
	auto Host=cnMemory::GetObjectFromMemberPointer(this,&cWPFWindow::fExitParentProcedure);
	return Host->Cleanup();
}
//---------------------------------------------------------------------------
void cWPFWindow::Cleanup(void)noexcept(true)
{
	mDestroy();

	Dispose();
}
//---------------------------------------------------------------------------
void* cWPFWindow::CastInterface(iTypeID IID)noexcept(true)
{
	return cnClass::ImplementCastInterface<iWindow>(this,IID);
}
//---------------------------------------------------------------------------
HWND cWPFWindow::GetHandle(void)noexcept(true)
{
	return fWindowHandle;
}
//---------------------------------------------------------------------------
bool cWPFWindow::CheckThread(void)noexcept(true)
{
	return fUIThread->IsCurrent();
}
//---------------------------------------------------------------------------
bool cWPFWindow::SubclassAttach(HWND,bool)noexcept(true)
{
	return false;
}
//---------------------------------------------------------------------------
bool cWPFWindow::SubclassDetach(void)noexcept(true)
{
	return false;
}
//---------------------------------------------------------------------------
bool cWPFWindow::WindowCreate(HWND Parent,const wchar_t *WindowText,DWORD Style,DWORD ExStyle,LONG X,LONG Y,LONG Width,LONG Height,UINT ChildID)noexcept(true)
{
	if(ChildID!=0)
		return false;
	return mCreate(Parent,WindowText,Style,ExStyle,X,Y,Width,Height);
}
//---------------------------------------------------------------------------
bool cWPFWindow::WindowDestroy(void)noexcept(true)
{
	return mDestroy();
}
//---------------------------------------------------------------------------
bool cWPFWindow::WindowMessage(LRESULT &MessageResult,HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)noexcept(true)
{
	switch(msg){
	case WM_DPICHANGED:
	{
		WORD NewDPI=LOWORD(wParam);
		if(NewDPI!=fWindowDPI){
			// change dpi
			fWindowDPI=NewDPI;
			if(fWPFClient!=nullptr){
				fWPFClient->WPFWindowDPIChanged(fWindowDPI);
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
void cWPFWindow::WPFSourceAttach(void *WindowHandle)noexcept(true)
{
	fWindowHandle=static_cast<HWND>(WindowHandle);
	fWindowDPI=::GetWindowDPI(fWindowHandle);

	for(auto &Handler : fHandlers){
		Handler->WindowAttached();
	}
	if(fWPFClient!=nullptr){
		auto ViewRoot=fWPFClient->WPFClientGetViewRoot();
		mSetVisualRoot(ViewRoot->GetTargetElementHandle());
	}
}
//---------------------------------------------------------------------------
void cWPFWindow::WPFSourceDetach(void)noexcept(true)
{
	if(fWPFClient!=nullptr){
		mClearVisualRoot();
	}

	for(auto &Handler : fHandlers){
		Handler->WindowDetached();
	}
	fWindowHandle=nullptr;
}
//---------------------------------------------------------------------------
bool cWPFWindow::WPFSourceMessage(uIntn &Result,void *hwnd, uInt32 msg, uIntn wParam, uIntn lParam)noexcept(true)
{
	return WindowMessage(reinterpret_cast<LRESULT&>(Result),static_cast<HWND>(hwnd),msg,wParam,lParam);
}
//---------------------------------------------------------------------------
cWPFUIThread* cWPFWindow::GetWPFUIThread(void)noexcept(true)
{
	return fUIThread;
}
//---------------------------------------------------------------------------
iUIThread* cWPFWindow::GetUIThread(void)noexcept(true)
{
	return fUIThread;
}
//---------------------------------------------------------------------------
WORD cWPFWindow::GetWindowDPI(void)const noexcept(true)
{
	return fWindowDPI;
}
//---------------------------------------------------------------------------
HWND cWPFWindow::GetWindowHandle(void)noexcept(true)
{
	return fWindowHandle;
}
//---------------------------------------------------------------------------
bool cWPFWindow::InsertMessageHandler(iWindowMessageHandler *Handler,sfInt8 Order)noexcept(true)
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
bool cWPFWindow::RemoveMessageHandler(iWindowMessageHandler *Handler)noexcept(true)
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
iWindowClient* cWPFWindow::GetClient(void)noexcept(true)
{
	if(fWPFClient!=nullptr)
		return fWPFClient->WPFWindowGetInterface();

	return nullptr;
}
//---------------------------------------------------------------------------
bool cWPFWindow::SetClient(iWindowClient *Client)noexcept(true)
{
	if(fUIThread->IsCurrent()==false)
		return false;

	auto WPFClient=iCast<iWPFWindowClient>(Client);
	if(WPFClient==nullptr)
		return false;

	if(WPFClient->WPFClientSetupWindow(this)==false)
		return false;

	if(fWPFClient!=nullptr){
		fWPFClient->WPFClientClearWindow(this);
		mClearVisualRoot();
	}
	fWPFClient=WPFClient;
	mSetVisualRoot(fWPFClient->WPFClientGetViewRoot()->GetTargetElementHandle());
	return true;
}
//---------------------------------------------------------------------------
void cWPFWindow::WindowClientExit(iWindowClient *Client)noexcept(true)
{
	if(fWPFClient==nullptr){
		return;
	}
	auto WPFClient=iCast<iWPFWindowClient>(Client);
	if(fWPFClient!=WPFClient)
		return;

	mClearVisualRoot();

	// may be destroyed by last reference in the client
	fWPFClient->WPFClientClearWindow(this);
}
//---------------------------------------------------------------------------
iWindowFrame* cWPFWindow::GetFrame(void)noexcept(true)
{
	return nullptr;
}
//---------------------------------------------------------------------------
bool cWPFWindow::SetFrame(iWindowFrame*)noexcept(true)
{
	return false;
}
//---------------------------------------------------------------------------
rPtr<iVariable> cWPFWindow::QueryAffixedVariable(const void *Token)noexcept(true)
{
	return fAffixedVariableSet.QueryAffixedVariable(Token);
}
//---------------------------------------------------------------------------
bool cWPFWindow::GetMouseAutoCapture(void)noexcept(true)
{
	return true;
}
//---------------------------------------------------------------------------
bool cWPFWindow::SetMouseAutoCapture(bool Enable)noexcept(true)
{
	return false;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cWPFWindowClient::cWPFWindowClient(cWPFUIThread *UIThread,const cGCHandle &WPFUIViewRootTargetElement)noexcept(true)
	: fUIThread(UIThread)
	, fContentScale(1.f)
{
	gWPFModule.ObjectRegistration.Acquire(this);
	mbcWPFUIViewRoot::mSetup(WPFUIViewRootTargetElement);
}
//---------------------------------------------------------------------------
cWPFWindowClient::~cWPFWindowClient()noexcept(true)
{
	mbcWPFUIViewRoot::mClear();
	gWPFModule.ObjectRegistration.Release(this);
}
//---------------------------------------------------------------------------
void cWPFWindowClient::VirtualStarted(void)noexcept(true)
{
}
//---------------------------------------------------------------------------
void cWPFWindowClient::VirtualStopped(void)noexcept(true)
{
	if(fUIThread->IsCurrent()){
		Cleanup();
	}
	else{
		fUIThread->mDispatchExecute(true,&fExitParentProcedure);
	}
}
//---------------------------------------------------------------------------
void cWPFWindowClient::cExitParentProcedure::Execute(void)noexcept(true)
{
	auto Host=cnMemory::GetObjectFromMemberPointer(this,&cWPFWindowClient::fExitParentProcedure);
	return Host->Cleanup();
}
//---------------------------------------------------------------------------
void cWPFWindowClient::Cleanup(void)noexcept(true)
{
	if(fWindowHost!=nullptr){
		fWindowHost->WindowClientExit(this);
	}

	Dispose();
}
//---------------------------------------------------------------------------
void* cWPFWindowClient::CastInterface(iTypeID IID)noexcept(true)
{
	return cnClass::FindInterface<iUIWindow,iCLIObject,iWindowClient,iWPFWindowClient>(this,IID);
}
//---------------------------------------------------------------------------
bool cWPFWindowClient::CheckThread(void)noexcept(true)
{
	return fUIThread->IsCurrent();
}
//---------------------------------------------------------------------------
iUIThread* cWPFWindowClient::GetUIThread(void)noexcept(true)
{
	return fUIThread;
}
//---------------------------------------------------------------------------
const cGCHandle& cWPFWindowClient::GetObjecHandle(void)noexcept(true)
{
	return fTargetRootElement;
}
//---------------------------------------------------------------------------
eUIState cWPFWindowClient::GetUIState(void)noexcept(true)
{
	return fWindowState;
}
//---------------------------------------------------------------------------
bool cWPFWindowClient::InsertStateHandler(iUIStateHandler *Handler,sfInt16 Order)noexcept(true)
{
	return false;
}
//---------------------------------------------------------------------------
bool cWPFWindowClient::RemoveStateHandler(iUIStateHandler *Handler)noexcept(true)
{
	return false;
}
//---------------------------------------------------------------------------
bool cWPFWindowClient::GetVisible(void)noexcept(true)
{
	return mGetVisible();
}
//---------------------------------------------------------------------------
bool cWPFWindowClient::SetVisible(bool Visible)noexcept(true)
{
	return mSetVisible(Visible);
}
//---------------------------------------------------------------------------
bool cWPFWindowClient::IsEnabled(void)noexcept(true)
{
	return false;
}
//---------------------------------------------------------------------------
bool cWPFWindowClient::GetEnable(void)noexcept(true)
{
	return false;
}
//---------------------------------------------------------------------------
bool cWPFWindowClient::SetEnable(bool Enable)noexcept(true)
{
	return false;
}
//---------------------------------------------------------------------------
bool cWPFWindowClient::TranslatePointTo(iInterface *Relative,cUIPoint &Position)noexcept(true)
{
	return false;
}
//---------------------------------------------------------------------------
bool cWPFWindowClient::MoveTo(iInterface *Relative,cUIPoint Position)noexcept(true)
{
	return false;
}
//---------------------------------------------------------------------------
cUIPoint cWPFWindowClient::GetSize(void)noexcept(true)
{
	return {0,0};
}
//---------------------------------------------------------------------------
bool cWPFWindowClient::SetSize(cUIPoint Size)noexcept(true)
{
	return false;
}
//---------------------------------------------------------------------------
bool cWPFWindowClient::ArrangeRectangle(iInterface *Relative,cUIPoint Position,cUIPoint Size)noexcept(true)
{
	return false;
}
//---------------------------------------------------------------------------
Float32 cWPFWindowClient::GetZPosition(void)noexcept(true)
{
	return 0;
}
//---------------------------------------------------------------------------
bool cWPFWindowClient::SetZPosition(Float32)noexcept(true)
{
	return false;
}
//---------------------------------------------------------------------------
Float32 cWPFWindowClient::GetContentScale(void)noexcept(true)
{
	return fContentScale;
}
//---------------------------------------------------------------------------
iUIScreen* cWPFWindowClient::GetScreen(void)noexcept(true)
{
	return nullptr;
}
//---------------------------------------------------------------------------
iUIWindow* cWPFWindowClient::GetParent(void)noexcept(true)
{
	return nullptr;
}
//---------------------------------------------------------------------------
bool cWPFWindowClient::InsertWindowHandler(iUIWindowHandler *Handler,sfInt16 Order)noexcept(true)
{
	return false;
}
//---------------------------------------------------------------------------
bool cWPFWindowClient::RemoveWindowHandler(iUIWindowHandler *Handler)noexcept(true)
{
	return false;
}
//---------------------------------------------------------------------------
iUIView* cWPFWindowClient::GetClient(void)noexcept(true)
{
	return fClient;
}
//---------------------------------------------------------------------------
bool cWPFWindowClient::SetClient(iUIView *View)noexcept(true)
{
	if(CheckThread()==false)
		return false;

	if(View==nullptr){
		if(fClient!=nullptr){
			fClient->WPFChildParentRelease(this);
			fClient=nullptr;
		}
		mResetClient();
		return true;
	}
	auto ClientView=iCast<cWPFUIView>(View);
	if(ClientView==nullptr)
		return false;

	fClient=ClientView;
	if(fClient->WPFChildParentAcquire(this)==false){
		return false;
	}
	fClient->WPFChildTreeNotifyScale(fLayoutScale,fContentScale);
	if(mSetClient(fClient->WPFGetTargetElementHandle())){

		fClient->WPFChildTreeNotifyWindow(this);
		return true;
	}
	else{
		fClient->WPFChildParentRelease(this);
		return false;
	}
}
//---------------------------------------------------------------------------
iWindow* cWPFWindowClient::GetWindow(void)noexcept(true)
{
	return fWindowHost;
}
//---------------------------------------------------------------------------
void cWPFWindowClient::SetBackgroundColor(COLORREF Color)noexcept(true)
{
	if(CheckThread()==false)
		return;
	uInt8 r=Color&0xFF;
	uInt8 g=(Color>>8)&0xFF;
	uInt8 b=(Color>>16)&0xFF;
	mSetBackgroundColor(r,g,b);
}
//---------------------------------------------------------------------------
iWindowClient* cWPFWindowClient::WPFWindowGetInterface(void)noexcept(true)
{
	return this;
}
//---------------------------------------------------------------------------
void cWPFWindowClient::WPFWindowDPIChanged(WORD NewDPI)noexcept(true)
{
	fContentScale=NewDPI/96.f;
	if(fClient!=nullptr){
		fClient->WPFChildTreeNotifyScale(fLayoutScale,fContentScale);
	}
}
//---------------------------------------------------------------------------
iUIWindow* cWPFWindowClient::WPFWindowInterface(void)noexcept(true)
{
	return this;
}
//---------------------------------------------------------------------------
iWindow* cWPFWindowClient::WPFWindowGetWindow(void)noexcept(true)
{
	if(fWindowHost==nullptr)
		return nullptr;
	return fWindowHost;
}
//---------------------------------------------------------------------------
bool cWPFWindowClient::WPFWindowIsActive(void)noexcept(true)
{
	return fWindowActive;
}
//---------------------------------------------------------------------------
eUIState cWPFWindowClient::EvaluateUIState(void)const noexcept(true)
{
	if(fWindowHost==nullptr || fWindowHost->GetWindowHandle()==nullptr){
		return UIState::Null;
	}
	if(fWindowVisible==false){
		return UIState::Background;
	}
	if(fWindowActive==false){
		return UIState::Inactive;
	}
	return UIState::Active;
}
//---------------------------------------------------------------------------
void cWPFWindowClient::UpdateUIState(void)noexcept(true)
{
	eUIState NewState=EvaluateUIState();

	if(fWindowState==NewState)
		return;
	fWindowState=NewState;
	if(fClient!=nullptr){
		fClient->WPFChildTreeNotifyState();
	}
}
//---------------------------------------------------------------------------
void cWPFWindowClient::UpdateContentScale(Float32 NewLayoutScale,Float32 NewContentScale)noexcept(true)
{
	if(fContentScale==NewContentScale && fLayoutScale==NewLayoutScale)
		return;
	
	fLayoutScale=NewLayoutScale;
	fContentScale=NewContentScale;
	if(fClient!=nullptr){
		fClient->WPFChildTreeNotifyScale(NewLayoutScale,NewContentScale);
	}
}
//---------------------------------------------------------------------------
void cWPFWindowClient::WPFWindowStateChanged(void)noexcept(true)
{
	UpdateUIState();
}
//---------------------------------------------------------------------------
void cWPFWindowClient::WPFWindowShutdownStarted(void)noexcept(true)
{
}
//---------------------------------------------------------------------------
bool cWPFWindowClient::WPFClientSetupWindow(cWPFWindow *Window)noexcept(true)
{
	if(fWindowHost!=nullptr)
		return false;

	fWindowHost=Window;
	UpdateContentScale(Window->mQueryLayoutScale(),Window->GetWindowDPI()/96.f);

	UpdateUIState();

	return true;
}
//---------------------------------------------------------------------------
void cWPFWindowClient::WPFClientClearWindow(cWPFWindow *Window)noexcept(true)
{
	if(fWindowHost!=Window)
		return;

	fWindowHost=nullptr;
}
//---------------------------------------------------------------------------
mbcWPFUIViewRoot* cWPFWindowClient::WPFClientGetViewRoot(void)noexcept(true)
{
	return this;
}
//---------------------------------------------------------------------------
SIZE cWPFWindowClient::GetClientSize(void)noexcept(true)
{
	auto WindowHandle=fWindowHost->GetHandle();

	RECT rc;
	if(::GetClientRect(WindowHandle,&rc)){
		return {rc.right-rc.left,rc.bottom-rc.top};
	}
	return {0,0};
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cWPFPopupWindowControl::cWPFPopupWindowControl(cWPFUIThread *UIThread,const cGCHandle &WPFUIViewRootTargetElement)noexcept(true)
	: cWPFWindowClient(UIThread,WPFUIViewRootTargetElement)
{
}
//---------------------------------------------------------------------------
cWPFPopupWindowControl::~cWPFPopupWindowControl()noexcept(true)
{
}
//---------------------------------------------------------------------------
void cWPFPopupWindowControl::SetCallback(iPopupWindowControlCallback *Callback)noexcept(true)
{
	fCallback=Callback;
}
//---------------------------------------------------------------------------
iUIWindow* cWPFPopupWindowControl::GetUIWindow(void)noexcept(true)
{
	return this;
}
//---------------------------------------------------------------------------
bool cWPFPopupWindowControl::SetupOwner(iUIView *View)noexcept(true)
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
	return mSetWindowOwner(WPFRootHandle);
}
//---------------------------------------------------------------------------
void cWPFPopupWindowControl::ClearOwner(void)noexcept(true)
{
	return mClearWindowOwner();
}
//---------------------------------------------------------------------------
void cWPFPopupWindowControl::ShowPopup(void)noexcept(true)
{
	mSetVisible(true);
}
//---------------------------------------------------------------------------
void cWPFPopupWindowControl::HidePopup(void)noexcept(true)
{
	mSetVisible(false);
}
//---------------------------------------------------------------------------
