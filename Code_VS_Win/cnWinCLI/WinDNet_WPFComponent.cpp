#include "WinDNet_WPFComponent.h"


using namespace cnLibrary;
using namespace cnRTL;
using namespace cnWinRTL;
using namespace cnWin;
//using namespace DNet;

//---------------------------------------------------------------------------
iPtr<iWindow> cnWin::DNetCreateWPFHwndSourceAsWindow(mcDNetUIThreadDispatcher *Dispatcher)
{
	return iCreate<cWPFHwndSourceAsWindow>(static_cast<cDNetUIThread*>(Dispatcher));
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cWPFHwndSourceAsWindow::cWPFHwndSourceAsWindow(cDNetUIThread *UIThread)
	: fUIThread(UIThread)
	//, fWindowDPI(96)
{
	auto Registration=cnSystem::GetSystemDependentRegistration();
	Registration->Register(this);
}
//---------------------------------------------------------------------------
cWPFHwndSourceAsWindow::~cWPFHwndSourceAsWindow()
{
	auto Registration=cnSystem::GetSystemDependentRegistration();
	Registration->Unregister(this);
}
//---------------------------------------------------------------------------
void cWPFHwndSourceAsWindow::VirtualStarted(void)noexcept(true)
{
	//fUIThread->mAttachDispatcher(&fDispatcherFinishNotify);
}
//---------------------------------------------------------------------------
void cWPFHwndSourceAsWindow::VirtualStopped(void)noexcept(true)
{
	//if(fUIThread->mDetachDispatcher(&fDispatcherFinishNotify)){
	//	// wait for notification
	//	return;
	//}
	// already shutdown
	VirtualDelete();
}
//---------------------------------------------------------------------------
rPtr< iArrayReference<const uChar16> > cWPFHwndSourceAsWindow::DependentCreateDescription(void)
{
	cString<uChar16> Desc=cnRTL::CreateStringFormat(u"cWPFWindow %p",this);
	return Desc.Token();
}
//---------------------------------------------------------------------------
void cWPFHwndSourceAsWindow::DependentShutdownNotification(void)
{
}
//---------------------------------------------------------------------------
void* cWPFHwndSourceAsWindow::CastInterface(iTypeID IID)noexcept(true)
{
	return ImpCastInterface<iWindow>(this,IID);
}
//---------------------------------------------------------------------------
HWND cWPFHwndSourceAsWindow::GetHandle(void)
{
	return fWindowHandle;
}
//---------------------------------------------------------------------------
bool cWPFHwndSourceAsWindow::CheckThread(void)
{
	return fUIThread->IsCurrent();
}
//---------------------------------------------------------------------------
bool cWPFHwndSourceAsWindow::WindowMessage(LRESULT &MessageResult,HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
	switch(msg){
	case WM_DPICHANGED:
	{
		WORD NewDPI=LOWORD(wParam);
		if(NewDPI!=fWindowDPI){
			// change dpi
			fWindowDPI=NewDPI;
			fWindowLayoutScale=fWindowDPI/96.f;
			//if(fWindowClient!=nullptr){
			//	fWindowClient->WPFWindowDPIChanged(GetContentScale());
			//}
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
		//if(fWindowClient!=nullptr){
		//	auto NewState=fWindowState.GetUIState();
		//	fWindowClient->WPFWindowStateChanged(NewState);
		//}
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
void cWPFHwndSourceAsWindow::WindowAttach(void)
{
	fWindowState.WindowAttached(fWindowHandle);
	fWindowDPI=GetWindowDPI(fWindowHandle);
	fWindowLayoutScale=fWindowDPI/96.f;

	for(auto &Handler : fHandlers){
		Handler->WindowAttached();
	}
}
//---------------------------------------------------------------------------
void cWPFHwndSourceAsWindow::WindowDetach(void)
{
	fWindowState.WindowDetached();

	for(auto &Handler : fHandlers){
		Handler->WindowDetached();
	}
}
//---------------------------------------------------------------------------
cDNetUIThread* cWPFHwndSourceAsWindow::GetWPFUIThread(void)
{
	return fUIThread;
}
//---------------------------------------------------------------------------
iUIThread* cWPFHwndSourceAsWindow::GetUIThread(void)
{
	return fUIThread;
}
//---------------------------------------------------------------------------
eUIState cWPFHwndSourceAsWindow::GetUIState(void)const
{
	return fWindowState.GetUIState();
}
//---------------------------------------------------------------------------
Float32 cWPFHwndSourceAsWindow::GetLayoutScale(void)const
{
	return fWindowLayoutScale;
}
//---------------------------------------------------------------------------
Float32 cWPFHwndSourceAsWindow::GetContentScale(void)const
{
	return fWindowLayoutScale;
;
}
//---------------------------------------------------------------------------
HWND cWPFHwndSourceAsWindow::GetWindowHandle(void)
{
	return fWindowHandle;
}
//---------------------------------------------------------------------------
bool cWPFHwndSourceAsWindow::InsertMessageHandler(iWindowMessageHandler *Handler,sfInt8 Order)
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
bool cWPFHwndSourceAsWindow::RemoveMessageHandler(iWindowMessageHandler *Handler)
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
iWindowClient* cWPFHwndSourceAsWindow::GetClient(void)
{
	//if(fWindowClient!=nullptr)
	//	return fWindowClient->WPFWindowGetInterface();

	return nullptr;
}
//---------------------------------------------------------------------------
iWindowFrame* cWPFHwndSourceAsWindow::GetFrame(void)
{
	return nullptr;
}
//---------------------------------------------------------------------------
rPtr<iVariable> cWPFHwndSourceAsWindow::QueryAffixedVariable(const void *Token)
{
	return nullptr;
}
//---------------------------------------------------------------------------
