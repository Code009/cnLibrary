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
iPtr<iWindowClient> cnWin::WPFCreateWindowClient(void)noexcept
{
	auto UIThread=cWPFUIThread::CurrentUIThread();
	if(UIThread==nullptr)
		return nullptr;

	cGCRef WPFViewRootHandle;
	mMakeWPFViewRoot(WPFViewRootHandle);
	return iCreate<cWPFWindowClient>(UIThread,WPFViewRootHandle);
}
//---------------------------------------------------------------------------
rPtr<iPopupWindowControl> cnWin::WPFCreatePopupWindowControl(void)noexcept
{
	auto UIThread=cWPFUIThread::CurrentUIThread();
	if(UIThread==nullptr)
		return nullptr;

	cGCRef WPFViewRootHandle;
	mMakeWPFViewRoot(WPFViewRootHandle);
	return iCreate<cWPFPopupWindowControl>(UIThread,WPFViewRootHandle);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cWPFWindow::cWPFWindow(cWPFUIThread *UIThread)noexcept
	: fUIThread(UIThread)
	, fWindowDPI(96)
{
	mbcWPFWindow::mSetup();
	gWPFModule.ObjectRegistration.Acquire(this);
}
//---------------------------------------------------------------------------
cWPFWindow::~cWPFWindow()noexcept
{
	gWPFModule.ObjectRegistration.Release(this);
	mbcWPFWindow::mClear();
}
//---------------------------------------------------------------------------
void cWPFWindow::VirtualStarted(void)noexcept
{
}
//---------------------------------------------------------------------------
void cWPFWindow::VirtualStopped(void)noexcept
{
	if(fUIThread->IsCurrent()){
		Cleanup();
	}
	else{
		fUIThread->mDispatchExecute(true,&fExitParentProcedure);
	}
}
//---------------------------------------------------------------------------
void cWPFWindow::cExitParentProcedure::Execute(void)noexcept
{
	auto Host=cnMemory::GetObjectFromMemberPointer(this,&cWPFWindow::fExitParentProcedure);
	return Host->Cleanup();
}
//---------------------------------------------------------------------------
void cWPFWindow::Cleanup(void)noexcept
{
	mDestroy();

	VirtualDelete();
}
//---------------------------------------------------------------------------
void* cWPFWindow::CastInterface(iTypeID IID)noexcept
{
	return cnVar::ImplementCastInterface<iWindow>(this,IID);
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
bool cWPFWindow::SubclassAttach(HWND,bool)noexcept
{
	return false;
}
//---------------------------------------------------------------------------
bool cWPFWindow::SubclassDetach(void)noexcept
{
	return false;
}
//---------------------------------------------------------------------------
bool cWPFWindow::WindowCreate(HWND Parent,const wchar_t *WindowText,DWORD Style,DWORD ExStyle,LONG X,LONG Y,LONG Width,LONG Height,UINT ChildID)noexcept
{
	if(ChildID!=0)
		return false;
	return mCreate(Parent,WindowText,Style,ExStyle,X,Y,Width,Height);
}
//---------------------------------------------------------------------------
bool cWPFWindow::WindowDestroy(void)noexcept
{
	return mDestroy();
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
void cWPFWindow::WPFSourceAttach(void *WindowHandle)noexcept
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
void cWPFWindow::WPFSourceDetach(void)noexcept
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
bool cWPFWindow::WPFSourceMessage(uIntn &Result,void *hwnd, uInt32 msg, uIntn wParam, uIntn lParam)noexcept
{
	return WindowMessage(reinterpret_cast<LRESULT&>(Result),static_cast<HWND>(hwnd),msg,wParam,lParam);
}
//---------------------------------------------------------------------------
cWPFUIThread* cWPFWindow::GetWPFUIThread(void)noexcept
{
	return fUIThread;
}
//---------------------------------------------------------------------------
iUIThread* cWPFWindow::GetUIThread(void)noexcept
{
	return fUIThread;
}
//---------------------------------------------------------------------------
WORD cWPFWindow::GetWindowDPI(void)const noexcept
{
	return fWindowDPI;
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
	if(fWPFClient!=nullptr)
		return fWPFClient->WPFWindowGetInterface();

	return nullptr;
}
//---------------------------------------------------------------------------
bool cWPFWindow::SetClient(iWindowClient *Client)noexcept
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
void cWPFWindow::WindowClientExit(iWindowClient *Client)noexcept
{
	if(fWPFClient==nullptr){
		return;
	}
	auto WPFClient=iCast<iWPFWindowClient>(Client);
	if(fWPFClient!=WPFClient)
		return;

	fWPFClient->WPFClientClearWindow(this);
	mClearVisualRoot();
}
//---------------------------------------------------------------------------
iWindowFrame* cWPFWindow::GetFrame(void)noexcept
{
	return nullptr;
}
//---------------------------------------------------------------------------
bool cWPFWindow::SetFrame(iWindowFrame*)noexcept
{
	return false;
}
//---------------------------------------------------------------------------
rPtr<iVariable> cWPFWindow::QueryAffixedVariable(const void *Token)noexcept
{
	return fAffixedVariableSet.QueryAffixedVariable(Token);
}
//---------------------------------------------------------------------------
bool cWPFWindow::GetMouseAutoCapture(void)noexcept
{
	return true;
}
//---------------------------------------------------------------------------
bool cWPFWindow::SetMouseAutoCapture(bool Enable)noexcept
{
	return false;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cWPFWindowClient::cWPFWindowClient(cWPFUIThread *UIThread,const cGCHandle &WPFUIViewRootTargetElement)noexcept
	: fUIThread(UIThread)
	, fContentScale(1.f)
{
	gWPFModule.ObjectRegistration.Acquire(this);
	mbcWPFUIViewRoot::mSetup(WPFUIViewRootTargetElement);
}
//---------------------------------------------------------------------------
cWPFWindowClient::~cWPFWindowClient()noexcept
{
	mbcWPFUIViewRoot::mClear();
	gWPFModule.ObjectRegistration.Release(this);
}
//---------------------------------------------------------------------------
void cWPFWindowClient::VirtualStarted(void)noexcept
{
}
//---------------------------------------------------------------------------
void cWPFWindowClient::VirtualStopped(void)noexcept
{
	if(fUIThread->IsCurrent()){
		Cleanup();
	}
	else{
		fUIThread->mDispatchExecute(true,&fExitParentProcedure);
	}
}
//---------------------------------------------------------------------------
void cWPFWindowClient::cExitParentProcedure::Execute(void)noexcept
{
	auto Host=cnMemory::GetObjectFromMemberPointer(this,&cWPFWindowClient::fExitParentProcedure);
	return Host->Cleanup();
}
//---------------------------------------------------------------------------
void cWPFWindowClient::Cleanup(void)noexcept
{
	if(fWindowHost!=nullptr){
		fWindowHost->WindowClientExit(this);
	}

	VirtualDelete();
}
//---------------------------------------------------------------------------
void* cWPFWindowClient::CastInterface(iTypeID IID)noexcept
{
	return cnVar::FindInterface<iUIWindow,iCLIObject,iWindowClient,iWPFWindowClient>(this,IID);
}
//---------------------------------------------------------------------------
bool cWPFWindowClient::CheckThread(void)noexcept
{
	return fUIThread->IsCurrent();
}
//---------------------------------------------------------------------------
iUIThread* cWPFWindowClient::GetUIThread(void)noexcept
{
	return fUIThread;
}
//---------------------------------------------------------------------------
const cGCHandle& cWPFWindowClient::GetObjecHandle(void)noexcept
{
	return fTargetRootElement;
}
//---------------------------------------------------------------------------
eUIState cWPFWindowClient::GetUIState(void)noexcept
{
	return fWindowState;
}
//---------------------------------------------------------------------------
bool cWPFWindowClient::InsertStateHandler(iUIStateHandler *Handler,sfInt16 Order)noexcept
{
	return false;
}
//---------------------------------------------------------------------------
bool cWPFWindowClient::RemoveStateHandler(iUIStateHandler *Handler)noexcept
{
	return false;
}
//---------------------------------------------------------------------------
bool cWPFWindowClient::GetVisible(void)noexcept
{
	return mGetVisible();
}
//---------------------------------------------------------------------------
bool cWPFWindowClient::SetVisible(bool Visible)noexcept
{
	return mSetVisible(Visible);
}
//---------------------------------------------------------------------------
bool cWPFWindowClient::IsEnabled(void)noexcept
{
	return false;
}
//---------------------------------------------------------------------------
bool cWPFWindowClient::GetEnable(void)noexcept
{
	return false;
}
//---------------------------------------------------------------------------
bool cWPFWindowClient::SetEnable(bool Enable)noexcept
{
	return false;
}
//---------------------------------------------------------------------------
bool cWPFWindowClient::TranslatePointTo(iInterface *Relative,cUIPoint &Position)noexcept
{
	return false;
}
//---------------------------------------------------------------------------
bool cWPFWindowClient::MoveTo(iInterface *Relative,cUIPoint Position)noexcept
{
	return false;
}
//---------------------------------------------------------------------------
cUIPoint cWPFWindowClient::GetSize(void)noexcept
{
	return {0,0};
}
//---------------------------------------------------------------------------
bool cWPFWindowClient::SetSize(cUIPoint Size)noexcept
{
	return false;
}
//---------------------------------------------------------------------------
bool cWPFWindowClient::ArrangeRectangle(iInterface *Relative,cUIPoint Position,cUIPoint Size)noexcept
{
	return false;
}
//---------------------------------------------------------------------------
Float32 cWPFWindowClient::GetZPosition(void)noexcept
{
	return 0;
}
//---------------------------------------------------------------------------
bool cWPFWindowClient::SetZPosition(Float32)noexcept
{
	return false;
}
//---------------------------------------------------------------------------
Float32 cWPFWindowClient::GetContentScale(void)noexcept
{
	return fContentScale;
}
//---------------------------------------------------------------------------
iUIScreen* cWPFWindowClient::GetScreen(void)noexcept
{
	return nullptr;
}
//---------------------------------------------------------------------------
iUIWindow* cWPFWindowClient::GetParent(void)noexcept
{
	return nullptr;
}
//---------------------------------------------------------------------------
bool cWPFWindowClient::InsertWindowHandler(iUIWindowHandler *Handler,sfInt16 Order)noexcept
{
	return false;
}
//---------------------------------------------------------------------------
bool cWPFWindowClient::RemoveWindowHandler(iUIWindowHandler *Handler)noexcept
{
	return false;
}
//---------------------------------------------------------------------------
iUIView* cWPFWindowClient::GetClient(void)noexcept
{
	return fClient;
}
//---------------------------------------------------------------------------
bool cWPFWindowClient::SetClient(iUIView *View)noexcept
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
iWindow* cWPFWindowClient::GetWindow(void)noexcept
{
	return fWindowHost;
}
//---------------------------------------------------------------------------
void cWPFWindowClient::SetBackgroundColor(COLORREF Color)noexcept
{
	if(CheckThread()==false)
		return;
	uInt8 r=Color&0xFF;
	uInt8 g=(Color>>8)&0xFF;
	uInt8 b=(Color>>16)&0xFF;
	mSetBackgroundColor(r,g,b);
}
//---------------------------------------------------------------------------
iWindowClient* cWPFWindowClient::WPFWindowGetInterface(void)noexcept
{
	return this;
}
//---------------------------------------------------------------------------
void cWPFWindowClient::WPFWindowDPIChanged(WORD NewDPI)noexcept
{
	fContentScale=NewDPI/96.f;
	if(fClient!=nullptr){
		fClient->WPFChildTreeNotifyScale(fLayoutScale,fContentScale);
	}
}
//---------------------------------------------------------------------------
iUIWindow* cWPFWindowClient::WPFWindowInterface(void)noexcept
{
	return this;
}
//---------------------------------------------------------------------------
iWindow* cWPFWindowClient::WPFWindowGetWindow(void)noexcept
{
	if(fWindowHost==nullptr)
		return nullptr;
	return fWindowHost;
}
//---------------------------------------------------------------------------
bool cWPFWindowClient::WPFWindowIsActive(void)noexcept
{
	return fWindowActive;
}
//---------------------------------------------------------------------------
eUIState cWPFWindowClient::EvaluateUIState(void)const noexcept
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
void cWPFWindowClient::UpdateUIState(void)noexcept
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
void cWPFWindowClient::UpdateContentScale(Float32 NewLayoutScale,Float32 NewContentScale)noexcept
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
void cWPFWindowClient::WPFWindowStateChanged(void)noexcept
{
	UpdateUIState();
}
//---------------------------------------------------------------------------
void cWPFWindowClient::WPFWindowShutdownStarted(void)noexcept
{
}
//---------------------------------------------------------------------------
bool cWPFWindowClient::WPFClientSetupWindow(cWPFWindow *Window)noexcept
{
	if(fWindowHost!=nullptr)
		return false;

	fWindowHost=Window;
	UpdateContentScale(Window->mQueryLayoutScale(),Window->GetWindowDPI()/96.f);

	UpdateUIState();

	return true;
}
//---------------------------------------------------------------------------
void cWPFWindowClient::WPFClientClearWindow(cWPFWindow *Window)noexcept
{
	if(fWindowHost!=Window)
		return;

	fWindowHost=nullptr;
}
//---------------------------------------------------------------------------
mbcWPFUIViewRoot* cWPFWindowClient::WPFClientGetViewRoot(void)noexcept
{
	return this;
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
//---------------------------------------------------------------------------
cWPFPopupWindowControl::cWPFPopupWindowControl(cWPFUIThread *UIThread,const cGCHandle &WPFUIViewRootTargetElement)noexcept
	: cWPFWindowClient(UIThread,WPFUIViewRootTargetElement)
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
	return mSetWindowOwner(WPFRootHandle);
}
//---------------------------------------------------------------------------
void cWPFPopupWindowControl::ClearOwner(void)noexcept
{
	return mClearWindowOwner();
}
//---------------------------------------------------------------------------
void cWPFPopupWindowControl::ShowPopup(void)noexcept
{
	mSetVisible(true);
}
//---------------------------------------------------------------------------
void cWPFPopupWindowControl::HidePopup(void)noexcept
{
	mSetVisible(false);
}
//---------------------------------------------------------------------------
