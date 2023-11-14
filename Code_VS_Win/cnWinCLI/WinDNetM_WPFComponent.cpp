#include "WinDNetM_WPFComponent.h"


using namespace cnLibrary;
using namespace cnRTL;
using namespace cnWinRTL;
using namespace cnWin;
using namespace DNet;
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
rcWPFHwndSource::rcWPFHwndSource(mcWPFHwndSource *CPP)
	: fCPP(CPP)
{
}
//---------------------------------------------------------------------------
System::Windows::Interop::HwndSource^ rcWPFHwndSource::Source::get(void)
{
	return fSource;
}
//---------------------------------------------------------------------------
void rcWPFHwndSource::Source::set(System::Windows::Interop::HwndSource ^Source)
{
	if(Source->CheckAccess()==false)
		return;

	//Source->Dispatcher->HasShutdownFinished
	if(fSource!=nullptr){
		//fSource->Dispatcher->HasShutdownFinished-=;
		fSource->RemoveHook(fHook);
		fCPP->WPFSourceDetach();
	}
	fSource=Source;
	if(fSource!=nullptr){
		if(fHook==nullptr){
			fHook=gcnew System::Windows::Interop::HwndSourceHook(this,&rcWPFHwndSource::WindowMessageHook);
		}
		fSource->AddHook(fHook);
		//fSource->Dispatcher->HasShutdownFinished+=;
		fCPP->WPFSourceAttach(this);
	}
	else{
		fHook=nullptr;
	}
}
//---------------------------------------------------------------------------
System::IntPtr rcWPFHwndSource::WindowMessageHook(System::IntPtr hwnd, int msg, System::IntPtr wParam, System::IntPtr lParam, bool %handled)
{
	if(msg==WM_DESTROY || msg==WM_NCDESTROY){
		fSource->RemoveHook(fHook);
		fCPP->WPFSourceDetach();
		fHook=nullptr;
		fSource=nullptr;
		return System::IntPtr::IntPtr(0);
	}
	return fCPP->WPFSourceMessage(reinterpret_cast<HWND>(hwnd.ToPointer()),static_cast<UINT>(msg),reinterpret_cast<WPARAM>(wParam.ToPointer()),reinterpret_cast<LPARAM>(lParam.ToPointer()),handled);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
mcWPFHwndSource::mcWPFHwndSource()
	: fWPFHwnd(gcnew rcWPFHwndSource(this))
{
}
//---------------------------------------------------------------------------
mcWPFHwndSource::~mcWPFHwndSource()
{
}
//---------------------------------------------------------------------------
void mcWPFHwndSource::WPFSourceAttach(rcWPFHwndSource ^WPFHwnd)
{
	HWND WindowHandle=static_cast<HWND>(WPFHwnd->Source->Handle.ToPointer());

	fWindowHandle=WindowHandle;
	fWindowLayoutScale=static_cast<Float32>(WPFHwnd->Source->CompositionTarget->TransformToDevice.M11);

	WindowAttach();
}
//---------------------------------------------------------------------------
void mcWPFHwndSource::WPFSourceDetach(void)
{
	WindowDetach();
	fWindowHandle=nullptr;
}
//---------------------------------------------------------------------------
System::IntPtr mcWPFHwndSource::WPFSourceMessage(HWND hwnd, int msg, WPARAM wParam, LPARAM lParam, bool %handled)
{
	LRESULT Result=0;
	handled=WindowMessage(Result,hwnd,msg,wParam,lParam);
	return System::IntPtr::IntPtr(Result);
}
//---------------------------------------------------------------------------
void mcWPFHwndSource::WPFDispatcherFinishNotify(bool Shutdown)
{
	WindowShutdownFinished(Shutdown);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
mcWPFHwndSourceAsWindow::mcWPFHwndSourceAsWindow()
{
}
//---------------------------------------------------------------------------
mcWPFHwndSourceAsWindow::~mcWPFHwndSourceAsWindow()
{
}
//---------------------------------------------------------------------------
