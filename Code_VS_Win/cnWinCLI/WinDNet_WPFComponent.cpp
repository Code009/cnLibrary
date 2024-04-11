#include "WinDNet_WPFComponent.h"


using namespace cnLibrary;
using namespace cnRTL;
using namespace cnWinRTL;
using namespace cnWin;
//using namespace DNet;

//---------------------------------------------------------------------------
iPtr<iWindow> cnWin::DNetCreateWPFHwndSourceAsWindow(mcDNetUIThreadDispatcher *Dispatcher)noexcept
{
	return iCreate<cWPFHwndSourceAsWindow>(static_cast<cDNetUIThread*>(Dispatcher));
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cWPFHwndSourceAsWindow::cWPFHwndSourceAsWindow(cDNetUIThread *UIThread)noexcept
	: fUIThread(UIThread)
	//, fWindowDPI(96)
{
	auto Registration=cnSystem::GetSystemDependentRegistration();
	Registration->Register(this);
}
//---------------------------------------------------------------------------
cWPFHwndSourceAsWindow::~cWPFHwndSourceAsWindow()noexcept
{
	auto Registration=cnSystem::GetSystemDependentRegistration();
	Registration->Unregister(this);
}
//---------------------------------------------------------------------------
void cWPFHwndSourceAsWindow::VirtualStarted(void)noexcept
{
	//fUIThread->mAttachDispatcher(&fDispatcherFinishNotify);
}
//---------------------------------------------------------------------------
void cWPFHwndSourceAsWindow::VirtualStopped(void)noexcept
{
	//if(fUIThread->mDetachDispatcher(&fDispatcherFinishNotify)){
	//	// wait for notification
	//	return;
	//}
	// already shutdown
	VirtualDelete();
}
//---------------------------------------------------------------------------
rPtr< iArrayReference<const uChar16> > cWPFHwndSourceAsWindow::DependentCreateDescription(void)noexcept
{
	cString<uChar16> Desc=cnRTL::CreateStringFormat(u"cWPFWindow %p",this);
	return Desc.Token();
}
//---------------------------------------------------------------------------
void cWPFHwndSourceAsWindow::DependentShutdownNotification(void)noexcept
{
}
//---------------------------------------------------------------------------
void* cWPFHwndSourceAsWindow::CastInterface(iTypeID IID)noexcept
{
	return ImpCastInterface<iWindow>(this,IID);
}
//---------------------------------------------------------------------------
HWND cWPFHwndSourceAsWindow::GetHandle(void)noexcept
{
	return fWindowHandle;
}
//---------------------------------------------------------------------------
bool cWPFHwndSourceAsWindow::CheckThread(void)noexcept
{
	return fUIThread->IsCurrent();
}
//---------------------------------------------------------------------------
bool cWPFHwndSourceAsWindow::WindowMessage(LRESULT &MessageResult,HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)noexcept
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
void cWPFHwndSourceAsWindow::WindowAttach(void)noexcept
{
	fWindowState.WindowAttached(fWindowHandle);
	fWindowDPI=GetWindowDPI(fWindowHandle);
	fWindowLayoutScale=fWindowDPI/96.f;

	for(auto &Handler : fHandlers){
		Handler->WindowAttached();
	}
}
//---------------------------------------------------------------------------
void cWPFHwndSourceAsWindow::WindowDetach(void)noexcept
{
	fWindowState.WindowDetached();

	for(auto &Handler : fHandlers){
		Handler->WindowDetached();
	}
}
//---------------------------------------------------------------------------
cDNetUIThread* cWPFHwndSourceAsWindow::GetWPFUIThread(void)noexcept
{
	return fUIThread;
}
//---------------------------------------------------------------------------
iUIThread* cWPFHwndSourceAsWindow::GetUIThread(void)noexcept
{
	return fUIThread;
}
//---------------------------------------------------------------------------
eUIState cWPFHwndSourceAsWindow::GetUIState(void)const noexcept
{
	return fWindowState.GetUIState();
}
//---------------------------------------------------------------------------
Float32 cWPFHwndSourceAsWindow::GetLayoutScale(void)const noexcept
{
	return fWindowLayoutScale;
}
//---------------------------------------------------------------------------
Float32 cWPFHwndSourceAsWindow::GetContentScale(void)const noexcept
{
	return fWindowLayoutScale;
;
}
//---------------------------------------------------------------------------
HWND cWPFHwndSourceAsWindow::GetWindowHandle(void)noexcept
{
	return fWindowHandle;
}
//---------------------------------------------------------------------------
bool cWPFHwndSourceAsWindow::InsertMessageHandler(iWindowMessageHandler *Handler,sfInt8 Order)noexcept
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
bool cWPFHwndSourceAsWindow::RemoveMessageHandler(iWindowMessageHandler *Handler)noexcept
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
iWindowClient* cWPFHwndSourceAsWindow::GetClient(void)noexcept
{
	//if(fWindowClient!=nullptr)
	//	return fWindowClient->WPFWindowGetInterface();

	return nullptr;
}
//---------------------------------------------------------------------------
iWindowFrame* cWPFHwndSourceAsWindow::GetFrame(void)noexcept
{
	return nullptr;
}
//---------------------------------------------------------------------------
rPtr<iVariable> cWPFHwndSourceAsWindow::QueryAffixedVariable(const void *Token)noexcept
{
	return nullptr;
}
//---------------------------------------------------------------------------
