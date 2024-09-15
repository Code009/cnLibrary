#include "WinCLIM_WPFUIWindow.h"

using namespace cnLibrary;
using namespace cnWin;
using namespace DNet;

//---------------------------------------------------------------------------
void cnWin::mMakeWPFViewRoot(cGCRef &Ref)noexcept
{
	Ref.Set(gcnew WPFViewRoot());
}
//---------------------------------------------------------------------------
riWPFUIViewRemoteRoot^ cnWin::WPFUIViewQueryRemoteRootFromRootVisual(System::Windows::Media::Visual ^RootVisual)noexcept
{
	auto RootTargetElement=dynamic_cast<IWPFUIViewRootTargetElement^>(RootVisual);
	if(RootTargetElement!=nullptr){
		return gcnew rcWPFUIViewRootTargetElementAsRemoteRoot(RootTargetElement);
	}
	return nullptr;

}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
rcNativeCaller_WPFUIWindow::rcNativeCaller_WPFUIWindow(void *CPP)
	: CPP(CPP)
{
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void WPFUIViewRootAdapter::CPPAttach(mbcWPFUIViewRoot *p)
{
	if(CPP!=nullptr)
		return;
	this->CPP=p;

	auto TargetElement=CPP->fTargetRootElement.Get();
	if(TargetElement!=nullptr){
		auto Element=dynamic_cast<System::Windows::UIElement^>(TargetElement);
		if(Element!=nullptr){
			NativeCaller=gcnew rcNativeCaller_WPFUIWindow(CPP);
			Element->Dispatcher->ShutdownStarted+=gcnew System::EventHandler(NativeCaller,&rcNativeCaller_WPFUIWindow::mbcWPFUIViewRoot_ShutdownStarted);
			Element->IsVisibleChanged+=gcnew System::Windows::DependencyPropertyChangedEventHandler(OnIsVisibleChanged);
			CPP->fWindowVisible=Element->IsVisible;
		}
		else{
			CPP->fWindowVisible=true;
			CPP->fWindowActive=true;
		}
	}
}
//---------------------------------------------------------------------------
void WPFUIViewRootAdapter::CPPDetach(mbcWPFUIViewRoot *p)
{
	if(CPP!=p)
		return;

	auto TargetElement=CPP->fTargetRootElement.Get();
	if(TargetElement!=nullptr){
		auto Element=dynamic_cast<System::Windows::UIElement^>(TargetElement);
		if(Element!=nullptr){
			Element->Dispatcher->ShutdownStarted-=gcnew System::EventHandler(NativeCaller,&rcNativeCaller_WPFUIWindow::mbcWPFUIViewRoot_ShutdownStarted);
			Element->IsVisibleChanged-=gcnew System::Windows::DependencyPropertyChangedEventHandler(OnIsVisibleChanged);
		}
	}
	CPP->fWindowVisible=false;
	CPP->fWindowActive=false;
	NativeCaller=nullptr;
	CPP=nullptr;
}
//---------------------------------------------------------------------------
void rcNativeCaller_WPFUIWindow::mbcWPFUIViewRoot_ShutdownStarted(System::Object^,System::EventArgs^)
{
	static_cast<mbcWPFUIViewRoot*>(CPP)->WPFDispatcherShutdownStarted();
}
//---------------------------------------------------------------------------
void WPFUIViewRootAdapter::RemoteChildAttach(IWPFUIViewTargetElement ^RemoteChild)
{
}
//---------------------------------------------------------------------------
void WPFUIViewRootAdapter::RemoteChildDetach(IWPFUIViewTargetElement ^RemoteChild)
{
}
//---------------------------------------------------------------------------
mbcWPFUIViewRoot* WPFUIViewRootAdapter::CPPFromSender(System::Object ^sender)
{
	auto TargetElement=dynamic_cast<IWPFUIViewRootTargetElement^>(sender);
	if(TargetElement!=nullptr){
		return TargetElement->Adapter.CPP;
	}
	return nullptr;
}
//---------------------------------------------------------------------------
void WPFUIViewRootAdapter::OnActiveChanged(void)
{
	if(CPP==nullptr)
		return;

	auto TargetElement=CPP->fTargetRootElement.Get();
	bool Active=TargetElement->IsActive;
	if(CPP->fWindowActive==Active)
		return;

	CPP->fWindowActive=Active;

	CPP->WPFWindowStateChanged();
}
//---------------------------------------------------------------------------
void WPFUIViewRootAdapter::OnIsVisibleChanged(System::Object ^sender,System::Windows::DependencyPropertyChangedEventArgs e)
{
	auto TargetElement=dynamic_cast<IWPFUIViewRootTargetElement^>(sender);
	if(TargetElement!=nullptr){
		auto CPPViewRoot=TargetElement->Adapter.CPP;
		if(CPPViewRoot!=nullptr){
			bool NewVisible=static_cast<System::Boolean>(e.NewValue);
			if(CPPViewRoot->fWindowVisible!=NewVisible){
				CPPViewRoot->fWindowVisible=NewVisible;
				CPPViewRoot->WPFWindowStateChanged();
			}
		}
	}
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
rcWPFUIViewRootTargetElementAsRemoteRoot::rcWPFUIViewRootTargetElementAsRemoteRoot(IWPFUIViewRootTargetElement ^Target)
	: fTarget(Target)
{
}
//---------------------------------------------------------------------------
void rcWPFUIViewRootTargetElementAsRemoteRoot::RemoteChildAttach(DNet::IWPFUIViewTargetElement ^RemoteChild)
{
	return fTarget->Adapter.RemoteChildAttach(RemoteChild);
}
//---------------------------------------------------------------------------
void rcWPFUIViewRootTargetElementAsRemoteRoot::RemoteChildDetach(DNet::IWPFUIViewTargetElement ^RemoteChild)
{
	return fTarget->Adapter.RemoteChildDetach(RemoteChild);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void mbcWPFUIViewRoot::mSetup(const cGCHandle &WPFUIViewRootTargetElement)noexcept
{
	auto TargetElement=WPFUIViewRootTargetElement.DynamicCast<IWPFUIViewRootTargetElement>();
	fTargetRootElement.Alloc(TargetElement);

	TargetElement->Adapter.CPPAttach(this);
}
//---------------------------------------------------------------------------
void mbcWPFUIViewRoot::mClear(void)noexcept
{
	auto TargetElement=fTargetRootElement.Get();
	TargetElement->Adapter.CPPDetach(this);

	fTargetRootElement.Free();
}
//---------------------------------------------------------------------------
#pragma unmanaged
const cGCHandle& mbcWPFUIViewRoot::GetTargetElementHandle(void)const noexcept
{
	return fTargetRootElement;
}
#pragma managed
//---------------------------------------------------------------------------
DNet::IWPFUIViewRootTargetElement^ mbcWPFUIViewRoot::GetTargetElement(void)const noexcept
{
	return fTargetRootElement.Get();
}
//---------------------------------------------------------------------------
void mbcWPFUIViewRoot::WPFDispatcherShutdownStarted(void)noexcept
{
	auto WPFRoot=fTargetRootElement.Get();
	if(WPFRoot==nullptr)
		return;

	WPFRoot->ClientElement=nullptr;

	WPFWindowShutdownStarted();
}
//---------------------------------------------------------------------------
bool mbcWPFUIViewRoot::mSetClient(const cGCHandle &ObjectHandle)noexcept
{
	auto ClientViewTargetElement=ObjectHandle.DynamicCast<System::Windows::UIElement>();
	if(ClientViewTargetElement==nullptr)
		return false;

	auto WPFRoot=fTargetRootElement.Get();

	try{
		WPFRoot->ClientElement=ClientViewTargetElement;
		return true;
	}
	catch(System::Exception^){
		return false;
	}
}
//---------------------------------------------------------------------------
void mbcWPFUIViewRoot::mResetClient(void)noexcept
{
	auto WPFRoot=fTargetRootElement.Get();
	WPFRoot->ClientElement=nullptr;
}
//---------------------------------------------------------------------------
bool mbcWPFUIViewRoot::mGetVisible(void)noexcept
{
	auto TargetElement=fTargetRootElement.Get();
	if(TargetElement!=nullptr){
		auto Element=dynamic_cast<System::Windows::UIElement^>(TargetElement);
		if(Element!=nullptr){
			return Element->Visibility==System::Windows::Visibility::Visible;
		}
	}
	return false;
}
//---------------------------------------------------------------------------
bool mbcWPFUIViewRoot::mSetVisible(bool Visible)noexcept
{
	auto TargetElement=fTargetRootElement.Get();
	if(TargetElement!=nullptr){
		auto Element=dynamic_cast<System::Windows::UIElement^>(TargetElement);
		if(Element!=nullptr){
			if(Visible){
				Element->Visibility=System::Windows::Visibility::Visible;
			}
			else{
				Element->Visibility=System::Windows::Visibility::Hidden;
			}
			return true;
		}
	}
	return false;
}
//---------------------------------------------------------------------------
bool mbcWPFUIViewRoot::mSetBackgroundColor(uInt8 r,uInt8 g,uInt8 b)noexcept
{
	auto WPFRoot=fTargetRootElement.Get();
	return WPFRoot->SetBackgroundColor(System::Windows::Media::Color::FromRgb(r,g,b));
}
//---------------------------------------------------------------------------
bool mbcWPFUIViewRoot::mSetWindowOwner(const cGCHandle &WPFRootRef)noexcept
{
	auto OwnerRoot=WPFRootRef.Cast<IWPFUIViewRootTargetElement>();
	if(OwnerRoot==nullptr)
		return false;

	auto WPFRoot=fTargetRootElement.Get();
	WPFRoot->OwnerTargetElement=OwnerRoot;
	return true;
}
//---------------------------------------------------------------------------
void mbcWPFUIViewRoot::mClearWindowOwner(void)noexcept
{
	auto WPFRoot=fTargetRootElement.Get();
	WPFRoot->OwnerTargetElement=nullptr;
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
		fClientElement->Arrange(LayoutRect);
	}
}
//---------------------------------------------------------------------------
WPFUIViewRootAdapter% WPFViewRoot::Adapter::get(void)
{
	return fViewRootAdapter;
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
bool WPFViewRoot::IsActive::get()
{
	return true;
}
//---------------------------------------------------------------------------
bool WPFViewRoot::SetBackgroundColor(System::Windows::Media::Color Color)
{
	BackgroundColor=Color;
	return true;
}
//---------------------------------------------------------------------------
IWPFUIViewRootTargetElement^ WPFViewRoot::OwnerTargetElement::get()
{
	return nullptr;
}
//---------------------------------------------------------------------------
void WPFViewRoot::OwnerTargetElement::set(IWPFUIViewRootTargetElement ^Owner)
{
}
//---------------------------------------------------------------------------
/*
bool WPFViewRoot::SetOwner(IWPFUIViewRootTargetElement ^Root)
{
	if(CPPWPFWindow==nullptr)
		return false;

	auto WPFWindow=static_cast<mbcWPFWindow*>(CPPWPFWindow);
	auto WndHandle=WPFWindow->mGetHandle();

	if(Root==nullptr){
		auto Style=::GetWindowLongW(WndHandle,GWL_STYLE);
		Style&=WS_POPUP;
		::SetWindowLongW(WndHandle,GWL_STYLE,Style);
		::SetParent(WndHandle,nullptr);
		return true;
	}

	auto OwnerRoot=dynamic_cast<WPFViewRoot^>(Root);
	if(OwnerRoot==nullptr)
		return false;
	if(OwnerRoot->CPPWPFWindow==nullptr)
		return false;
	auto OwnerWPFWindow=static_cast<mbcWPFWindow*>(OwnerRoot->CPPWPFWindow);

	auto OwnerWndHandle=OwnerWPFWindow->mGetHandle();
	auto Style=::GetWindowLongW(WndHandle,GWL_STYLE);
	Style|=WS_POPUP;
	::SetWindowLongW(WndHandle,GWL_STYLE,Style);

	::SetParent(WndHandle,OwnerWndHandle);
	return true;
}
*/
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
rcWPFHwndHandler::rcWPFHwndHandler(mbcWPFWindow *CPP)
{
	this->CPP=CPP;
}
//---------------------------------------------------------------------------
System::Windows::Interop::HwndSource^ rcWPFHwndHandler::Source::get(void)
{
	return fSource;
}
//---------------------------------------------------------------------------
void rcWPFHwndHandler::Source::set(System::Windows::Interop::HwndSource ^value)
{
	if(fSource!=nullptr){
		HwndSourceDetach();
	}
	fSource=value;
	if(fSource!=nullptr){
		HwndSourceAttach();
	}
}
//---------------------------------------------------------------------------
void rcWPFHwndHandler::HwndSourceAttach(void)
{
	void *WindowHandle=fSource->Handle.ToPointer();

	fHook=gcnew System::Windows::Interop::HwndSourceHook(this,&rcWPFHwndHandler::WindowMessageHook);
	fSource->AddHook(fHook);
	CPP->WPFSourceAttach(WindowHandle);
}
//---------------------------------------------------------------------------
void rcWPFHwndHandler::HwndSourceDetach(void)
{
	fSource->RemoveHook(fHook);
	fHook=nullptr;
	CPP->WPFSourceDetach();
}
//---------------------------------------------------------------------------
System::IntPtr rcWPFHwndHandler::WindowMessageHook(System::IntPtr hwnd, int msg, System::IntPtr wParam, System::IntPtr lParam, bool %handled)
{
	static constexpr int WM_DESTROY		=0x0002;
	static constexpr int WM_NCDESTROY	=0x0082;
	if(msg==WM_DESTROY || msg==WM_NCDESTROY){
		HwndSourceDetach();
		fSource=nullptr;
		return System::IntPtr::IntPtr(0);
	}
	uIntn Result=0;
	handled=CPP->WPFSourceMessage(Result,hwnd.ToPointer(),static_cast<uInt32>(msg),reinterpret_cast<uIntn>(wParam.ToPointer()),reinterpret_cast<uIntn>(lParam.ToPointer()));
	return System::IntPtr::IntPtr(reinterpret_cast<void*>(Result));
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void mbcWPFWindow::mSetup(void)noexcept
{
	fWPFHwndHandler.Alloc(gcnew rcWPFHwndHandler(this));
}
//---------------------------------------------------------------------------
void mbcWPFWindow::mClear(void)noexcept
{
	fWPFHwndHandler.Free();
}
//---------------------------------------------------------------------------
Float32 mbcWPFWindow::mQueryLayoutScale(void)const noexcept
{
	auto WPFHwndHandler=fWPFHwndHandler.Get();
	auto Source=WPFHwndHandler->Source;
	if(WPFHwndHandler->Source==nullptr){
		return 1.0f;
	}
	auto CompositionTarget=Source->CompositionTarget;
	if(CompositionTarget==nullptr)
		return 1.0f;
	return static_cast<Float32>(CompositionTarget->TransformToDevice.M11);
}
//---------------------------------------------------------------------------
bool mbcWPFWindow::mCreate(void *ParentWindowHandle,const wchar_t *WindowText,uInt32 Style,uInt32 ExStyle,sInt32 X,sInt32 Y,sInt32 Width,sInt32 Height)noexcept
{
	auto WPFHwndHandler=fWPFHwndHandler.Get();

	System::Windows::Interop::HwndSourceParameters CreateParam(gcnew System::String(WindowText),Width,Height);
	CreateParam.ParentWindow=System::IntPtr(ParentWindowHandle);
	CreateParam.WindowStyle=Style;
	CreateParam.ExtendedWindowStyle=ExStyle;
	CreateParam.PositionX=X;
	CreateParam.PositionY=Y;

	auto Source=gcnew System::Windows::Interop::HwndSource(CreateParam);

	WPFHwndHandler->Source=Source;
	return true;
}
//---------------------------------------------------------------------------
bool mbcWPFWindow::mDestroy(void)noexcept
{
	auto WPFHwndHandler=fWPFHwndHandler.Get();

	auto Source=WPFHwndHandler->Source;
	if(Source==nullptr)
		return false;
	WPFHwndHandler->Source=nullptr;
	delete Source;
	return true;
}
//---------------------------------------------------------------------------
bool mbcWPFWindow::mGetVisualRoot(cGCRef &VisualHandle)noexcept
{
	auto WPFHwndHandler=fWPFHwndHandler.Get();
	auto Source=WPFHwndHandler->Source;
	if(Source==nullptr)
		return false;
	VisualHandle.Set(Source->RootVisual);
	return true;
}
//---------------------------------------------------------------------------
bool mbcWPFWindow::mSetVisualRoot(const cGCHandle &VisualHandle)noexcept
{
	auto RootVisual=VisualHandle.DynamicCast<System::Windows::Media::Visual>();
	if(RootVisual==nullptr)
		return false;
	auto WPFHwndHandler=fWPFHwndHandler.Get();
	auto Source=WPFHwndHandler->Source;
	if(Source==nullptr)
		return false;
	Source->RootVisual=RootVisual;
	return true;
}
//---------------------------------------------------------------------------
bool mbcWPFWindow::mClearVisualRoot(void)noexcept
{
	auto WPFHwndHandler=fWPFHwndHandler.Get();
	auto Source=WPFHwndHandler->Source;
	if(Source==nullptr)
		return false;
	Source->RootVisual=nullptr;
	return true;
}
//---------------------------------------------------------------------------
