﻿#include "WinDNetM_WPFUIViewWindow.h"

//#pragma comment(lib,"Imm32.lib")	// IME

using namespace cnLibrary;

using namespace cnRTL;
using namespace cnWinRTL;
using namespace cnWin;
using namespace DNet;


//---------------------------------------------------------------------------
iPtr<iWindowClient> cnWin::DNetCreateWindowClient(void)
{
	auto DispatchFrame=mcDNetUIThreadDispatcher::mCurrentUIDispatcher();
	if(DispatchFrame==nullptr)
		return nullptr;

	mcWPFViewRoot::mcConstructParameter Parameter={
		gcnew WPFViewRoot()
	};
	return DNetCreateWindowClient(DispatchFrame,Parameter);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
mcWPFViewRoot::mcWPFViewRoot(mcConstructParameter &Parameter)
	: fWPFRoot(cnVar::MoveCast(Parameter.WPFRoot))
{
	fDispatcherFinishNotify.NotifyProcedure=DispatcherFinishNotify;

	auto WPF=fWPFRoot.Get();
	WPF->CPPAttach(this);
}
//---------------------------------------------------------------------------
mcWPFViewRoot::~mcWPFViewRoot()
{
	auto WPF=fWPFRoot.Get();
	WPF->CPPDetach(this);
}
//---------------------------------------------------------------------------
System::Object^ mcWPFViewRoot::mGetWPFRootObject(void)
{
	return fWPFRoot;
}
//---------------------------------------------------------------------------
void mcWPFViewRoot::CleanupWPF(void)
{
	if(fClient!=nullptr){
		mResetClient();
	}

	auto WPFRoot=fWPFRoot.Get();
	WPFRoot->DispatcherFinishCleanup();
}
//---------------------------------------------------------------------------
void mcWPFViewRoot::DispatcherFinishNotify(mcDNetUIThreadDispatcher::cDispatcherFinishNotify *Notify,bool Shutdown)
{
	auto Host=cnMemory::GetObjectFromMemberPointer(Notify,&mcWPFViewRoot::fDispatcherFinishNotify);
	return Host->DispatcherFinishNotify(Shutdown);
}
//---------------------------------------------------------------------------
void mcWPFViewRoot::DispatcherFinishNotify(bool Shutdown)
{
	CleanupWPF();
	nDispatcherFinishNotify(Shutdown);
}
//---------------------------------------------------------------------------
bool mcWPFViewRoot::mSetClient(cGCRef &ObjectHandle)
{
	auto WPFViewClient=ObjectHandle.Cast<IWPFView>();
	if(WPFViewClient==nullptr)
		return false;

	auto WPFRoot=fWPFRoot.Get();
	if(WPFViewClient->ParentAcquire(WPFRoot)==false){
		return false;
	}

	fWPFClient.Set(WPFViewClient);
	fClient=WPFViewClient->ChildInterface;


	try{
		WPFRoot->ClientElement=WPFViewClient->Element;
		return true;
	}
	catch(System::Exception ^e){
		fWPFClient.Set(nullptr);
		fClient=nullptr;
		return false;
	}
}
//---------------------------------------------------------------------------
void mcWPFViewRoot::mResetClient(void)
{
	if(fClient==nullptr)
		return;

	auto WPFViewClient=fWPFClient.Get();
	fWPFClient.Set(nullptr);
	auto WPFRoot=fWPFRoot.Get();
	WPFRoot->ClientElement=nullptr;

	WPFViewClient->ParentRelease(WPFRoot);
	fClient=nullptr;
	fClientView=nullptr;
}
//---------------------------------------------------------------------------
void mcWPFViewRoot::WPFUIWindowRemoveSubview(IWPFView ^Subview)
{
	if(fClient!=Subview->ChildInterface)
		return;

	mResetClient();
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
WPFViewRoot::WPFViewRoot()
{
	VisualOpacity=1.0;

	fBGBrush=gcnew System::Windows::Media::SolidColorBrush(System::Windows::Media::Color::FromRgb(255,255,255));
	fBGBrush->Freeze();

	fBackgroundDrawing.Brush=fBGBrush;
}
//---------------------------------------------------------------------------
void WPFViewRoot::CPPAttach(mcWPFViewRoot *CPP)
{
	if(fCPP!=nullptr)
		return;
	fCPP=CPP;
}
//---------------------------------------------------------------------------
void WPFViewRoot::CPPDetach(mcWPFViewRoot *CPP)
{
	if(fCPP!=CPP)
		return;
	fCPP=nullptr;
}
//---------------------------------------------------------------------------
iWPFViewParent* WPFViewRoot::ParentInterface::get(void)
{
	return fCPP;
}
//---------------------------------------------------------------------------
void WPFViewRoot::ParentRemoveViewChild(IWPFView ^Subview)
{
	return fCPP->WPFUIWindowRemoveSubview(Subview);
}
//---------------------------------------------------------------------------
iUIWindow* WPFViewRoot::UIWindowInterface::get(void)
{
	return fCPP->nGetUIWindowInterface();
};
//---------------------------------------------------------------------------
void WPFViewRoot::RemoteChildAttach(System::Object^)
{
}
//---------------------------------------------------------------------------
void WPFViewRoot::RemoteChildDetach(System::Object^)
{
}
//---------------------------------------------------------------------------
System::Windows::Media::Visual^ WPFViewRoot::GetVisualChild(int index)
{
	if(index==0){
		return fClientElement;
	}
	return nullptr;
}
//---------------------------------------------------------------------------
int WPFViewRoot::VisualChildrenCount::get(void)
{
	return fClientElement!=nullptr;
}
//---------------------------------------------------------------------
void WPFViewRoot::OnRender(System::Windows::Media::DrawingContext^ drawingContext)
{
	drawingContext->DrawDrawing(%fBackgroundDrawing);
}
//---------------------------------------------------------------------------
System::Windows::Media::Color WPFViewRoot::BackgroundColor::get()
{
	return fBGBrush->Color;
}
//---------------------------------------------------------------------------
void WPFViewRoot::BackgroundColor::set(System::Windows::Media::Color c)
{
	auto NewBrush=gcnew System::Windows::Media::SolidColorBrush(c);

	NewBrush->Freeze();
	fBackgroundDrawing.Brush=NewBrush;

	delete fBGBrush;
	fBGBrush=NewBrush;
}
//---------------------------------------------------------------------------
void WPFViewRoot::ArrangeCore(System::Windows::Rect finalRect)
{
	__super::ArrangeCore(finalRect);
	fSize=finalRect.Size;

	System::Windows::Rect LayoutRect;
	LayoutRect.X=0;
	LayoutRect.Y=0;
	LayoutRect.Size=fSize;

	auto DrawingRect=gcnew System::Windows::Media::RectangleGeometry(LayoutRect);
	DrawingRect->Freeze();
	fBackgroundDrawing.Geometry=DrawingRect;

	// arrange

	if(fClientElement!=nullptr){
		//fClientElement->Measure(fSize);
		fClientElement->Arrange(LayoutRect);
	}
}
//---------------------------------------------------------------------------
System::Windows::UIElement^ WPFViewRoot::ClientElement::get(void)
{
	return fClientElement;
}
//---------------------------------------------------------------------------
void WPFViewRoot::ClientElement::set(System::Windows::UIElement ^Element)
{
	if(fClientElement!=nullptr){
		RemoveVisualChild(fClientElement);
	}
	fClientElement=Element;
	if(fClientElement!=nullptr){
		AddVisualChild(fClientElement);
	
		System::Windows::Rect LayoutRect;
		LayoutRect.X=0;
		LayoutRect.Y=0;
		LayoutRect.Size=fSize;
		fClientElement->Arrange(LayoutRect);
	}
}
//---------------------------------------------------------------------------
void WPFViewRoot::DispatcherFinishCleanup(void)
{
}
//---------------------------------------------------------------------------
void cnWin::mWPFViewRoot_SetBackgroundColor(cGCHandle &ViewRoot,uInt8 r,uInt8 g,uInt8 b)
{
	auto WPFRoot=ViewRoot.Cast<WPFViewRoot>();
	WPFRoot->BackgroundColor=System::Windows::Media::Color::FromRgb(r,g,b);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
rcWPFWindow::rcWPFWindow(mcWPFWindow *CPP)
{
	this->CPP=CPP;
	fHook=gcnew System::Windows::Interop::HwndSourceHook(this,&rcWPFWindow::WindowMessageHook);
}
//---------------------------------------------------------------------------
void rcWPFWindow::Create(System::Windows::Interop::HwndSourceParameters %Parameter)
{
	Destroy();

	fSource=gcnew System::Windows::Interop::HwndSource(Parameter);
	CPP->WPFSourceAttach(this);

	fSource->AddHook(fHook);
}
//---------------------------------------------------------------------------
void rcWPFWindow::Destroy(void)
{
	if(fSource!=nullptr){
		fSource->RemoveHook(fHook);
		fSource->RootVisual=nullptr;
		CPP->WPFSourceDetach();
		delete fSource;
		fSource=nullptr;
	}
}
//---------------------------------------------------------------------------
System::IntPtr rcWPFWindow::WindowMessageHook(System::IntPtr hwnd, int msg, System::IntPtr wParam, System::IntPtr lParam, bool %handled)
{
	if(msg==WM_DESTROY || msg==WM_NCDESTROY){
		fSource->RemoveHook(gcnew System::Windows::Interop::HwndSourceHook(this,&rcWPFWindow::WindowMessageHook));
		fSource->RootVisual=nullptr;
		CPP->WPFSourceDetach();
		fSource=nullptr;
		return System::IntPtr::IntPtr(0);
	}
	return CPP->WPFSourceMessage(reinterpret_cast<HWND>(hwnd.ToPointer()),static_cast<UINT>(msg),reinterpret_cast<WPARAM>(wParam.ToPointer()),reinterpret_cast<LPARAM>(lParam.ToPointer()),handled);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
mcWPFWindow::mcWPFWindow()
	: fWPFWindow(gcnew rcWPFWindow(this))
{
	fDispatcherFinishNotify.NotifyProcedure=DispatcherFinishNotify;
}
//---------------------------------------------------------------------------
mcWPFWindow::~mcWPFWindow()
{
}
//---------------------------------------------------------------------------
HWND mcWPFWindow::mGetHandle(void)
{
	return fWindowHandle;
}
//---------------------------------------------------------------------------
bool mcWPFWindow::Create(HWND ParentWindowHandle,const wchar_t *WindowText,DWORD Style,DWORD ExStyle,LONG X,LONG Y,LONG Width,LONG Height)
{
	auto WPFWnd=fWPFWindow.Get();

	System::Windows::Interop::HwndSourceParameters CreateParam(gcnew System::String(WindowText),Width,Height);
	CreateParam.ParentWindow=System::IntPtr(ParentWindowHandle);
	CreateParam.WindowStyle=Style;
	CreateParam.ExtendedWindowStyle=ExStyle;
	CreateParam.PositionX=X;
	CreateParam.PositionY=Y;
	
	WPFWnd->Create(CreateParam);

	return true;
}
//---------------------------------------------------------------------------
bool mcWPFWindow::ClientAttach(WPFViewRoot ^ViewRoot,iWPFWindowClient *WindowClient)
{
	auto WPFWnd=fWPFWindow.Get();
	auto Source=WPFWnd->Source;
	
	if(Source->Dispatcher->CheckAccess()==false){
		return false;
	}

	fWPFViewRoot.Set(ViewRoot);
	fWindowClient=WindowClient;
	auto Size=GetClientSize();
	fWindowClient->WPFWindowDPIChanged(fWindowLayoutScale);
	if(Source!=nullptr)
		Source->RootVisual=ViewRoot;

	return true;
}
//---------------------------------------------------------------------------
void mcWPFWindow::ClearClient(void)
{
	auto WPFWnd=fWPFWindow.Get();
	auto Source=WPFWnd->Source;

	fWPFViewRoot.Set(nullptr);

	if(Source!=nullptr)
		Source->RootVisual=nullptr;
}
//---------------------------------------------------------------------------
void mcWPFWindow::WPFSourceAttach(rcWPFWindow ^WPFWnd)
{
	HWND WindowHandle=static_cast<HWND>(WPFWnd->Source->Handle.ToPointer());

	fWindowHandle=WindowHandle;
	fWindowLayoutScale=static_cast<Float32>(WPFWnd->Source->CompositionTarget->TransformToDevice.M11);

	nWPFSourceAttach();
}
//---------------------------------------------------------------------------
void mcWPFWindow::WPFSourceDetach(void)
{
	nWPFSourceDetach();
	fWindowHandle=nullptr;
}
//---------------------------------------------------------------------------
System::IntPtr mcWPFWindow::WPFSourceMessage(HWND hwnd, int msg, WPARAM wParam, LPARAM lParam, bool %handled)
{
	switch(msg){
	case WM_WINDOWPOSCHANGED:
		{
			auto wpos=reinterpret_cast<const WINDOWPOS*>(lParam);
			if((wpos->flags&SWP_NOSIZE)==0){
				// sized
			}
		}
		break;
	}

	LRESULT Result=0;
	handled=nWPFMessage(Result,hwnd,msg,wParam,lParam);
	return System::IntPtr::IntPtr(Result);
}
//---------------------------------------------------------------------------
void mcWPFWindow::DispatcherFinishNotify(mcDNetUIThreadDispatcher::cDispatcherFinishNotify *Notify,bool Shutdown)
{
	auto Host=cnMemory::GetObjectFromMemberPointer(Notify,&mcWPFWindow::fDispatcherFinishNotify);
	return Host->DispatcherFinishNotify(Shutdown);
}
//---------------------------------------------------------------------------
void mcWPFWindow::DispatcherFinishNotify(bool Shutdown)
{
	CleanupWPF();
	nDispatcherFinishNotify(Shutdown);
}
//---------------------------------------------------------------------------
void mcWPFWindow::CleanupWPF(void)
{
	auto WPFWnd=fWPFWindow.Get();
	WPFWnd->Destroy();
}
//---------------------------------------------------------------------------
mcWPFWindow* mcWPFWindow::mWindowAttachClient(cGCHandle &WindowCLIHandle,mcWPFViewRoot *ViewRoot,iWPFWindowClient *WindowClient)
{
	auto WPFWnd=WindowCLIHandle.DynamicCast<rcWPFWindow>();
	if(WPFWnd==nullptr)
		return nullptr;

	auto CPP=WPFWnd->CPP;
	if(CPP->mWindowAttachClient(ViewRoot,WindowClient)==false)
		return nullptr;

	return CPP;
}
//---------------------------------------------------------------------------
bool mcWPFWindow::mWindowAttachClient(mcWPFViewRoot *ViewRoot,iWPFWindowClient *WindowClient)
{
	if(static_cast<System::Object^>(fWPFViewRoot)!=nullptr){
		return false;
	}

	auto Root=dynamic_cast<WPFViewRoot^>(ViewRoot->mGetWPFRootObject());
	if(Root==nullptr)
		return false;

	return ClientAttach(Root,WindowClient);
}
//---------------------------------------------------------------------------
bool mcWPFWindow::mWindowDetachClient(mcWPFViewRoot *ViewRoot)
{
	auto Root=dynamic_cast<WPFViewRoot^>(ViewRoot->mGetWPFRootObject());
	if(Root==nullptr)
		return false;

	if(Root!=fWPFViewRoot.Get())
		return false;	// incorrect client
	ClearClient();
	return true;
}
//---------------------------------------------------------------------------
