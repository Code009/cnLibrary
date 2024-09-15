#include "WinCLIM_WPFUIFramework.h"


using namespace cnLibrary;
using namespace cnWin;
using namespace DNet;

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
WPFUIViewRootAdapter% WPFWindow::Adapter::get(void)
{
	return fViewRootAdapter;
}
//---------------------------------------------------------------------------
System::Windows::UIElement^ WPFWindow::ClientElement::get(void)
{
	return dynamic_cast<UIElement^>(Content);
}
//---------------------------------------------------------------------------
void WPFWindow::ClientElement::set(System::Windows::UIElement ^value)
{
	Content=value;
}
//---------------------------------------------------------------------------
IWPFUIViewRootTargetElement^ WPFWindow::OwnerTargetElement::get(void)
{
	return dynamic_cast<IWPFUIViewRootTargetElement^>(Owner);
}
//---------------------------------------------------------------------------
void WPFWindow::OwnerTargetElement::set(IWPFUIViewRootTargetElement ^Value)
{
	auto OwnerWnd=dynamic_cast<WPFWindow^>(Value);
	if(OwnerWnd==nullptr)
		return;

	Owner=OwnerWnd;
}
//---------------------------------------------------------------------------
bool WPFWindow::IsActive::get()
{
	return static_cast<System::Windows::Window^>(this)->IsActive;
}
//---------------------------------------------------------------------------
void WPFWindow::OnActivated(System::EventArgs^)
{
	fViewRootAdapter.OnActiveChanged();
}
//---------------------------------------------------------------------------
bool WPFWindow::SetBackgroundColor(System::Windows::Media::Color)
{
	return false;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
WPFUIViewAdapter% WPFFrameworkElement::Adapter::get(void)
{
	return fViewAdapter;
}
//---------------------------------------------------------------------------
System::Windows::DependencyObject ^WPFFrameworkElement::ViewParent::get(void)
{
	return __super::VisualParent;
}
//---------------------------------------------------------------------------
bool WPFFrameworkElement::ViewAddVisualChild(System::Windows::UIElement ^Child)
{
	try{
		this->AddVisualChild(Child);
		return true;
	}
	catch(System::Exception ^){
		return false;
	}
}
//---------------------------------------------------------------------------
bool WPFFrameworkElement::ViewRemoveVisualChild(System::Windows::UIElement ^Child)
{
	try{
		this->RemoveVisualChild(Child);
		return true;
	}
	catch(System::Exception ^){
		return false;
	}
}
//---------------------------------------------------------------------------
System::Windows::Media::Visual^ WPFFrameworkElement::GetVisualChild(int index)
{
	return fViewAdapter.GetVisualChild(index);
}
//---------------------------------------------------------------------------
int WPFFrameworkElement::VisualChildrenCount::get(void)
{
	return fViewAdapter.VisualChildrenCount;
}
//---------------------------------------------------------------------------
void WPFFrameworkElement::OnVisualParentChanged(System::Windows::DependencyObject ^oldParent)
{
	__super::OnVisualParentChanged(oldParent);
	fViewAdapter.OnVisualParentChanged(VisualParent);
}
//---------------------------------------------------------------------------
System::Windows::Size WPFFrameworkElement::MeasureOverride(System::Windows::Size availableSize)
{
	return fViewAdapter.MeasureOverride(availableSize);
}
//---------------------------------------------------------------------------
System::Windows::Size WPFFrameworkElement::ArrangeOverride(System::Windows::Size finalSize)
{
	return fViewAdapter.ArrangeOverride(finalSize);
}
//---------------------------------------------------------------------------
void WPFFrameworkElement::OnRender(System::Windows::Media::DrawingContext^ drawingContext)
{
	return fViewAdapter.OnRender(drawingContext);
}
//---------------------------------------------------------------------------
System::Windows::Media::GeometryHitTestResult^ WPFFrameworkElement::HitTestCore(System::Windows::Media::GeometryHitTestParameters ^ hitTestParameters)
{
	return fViewAdapter.HitTestCore(hitTestParameters);
}
//---------------------------------------------------------------------------
System::Windows::Media::HitTestResult^ WPFFrameworkElement::HitTestCore(System::Windows::Media::PointHitTestParameters ^ hitTestParameters)
{
	return fViewAdapter.HitTestCore(hitTestParameters);
}
//---------------------------------------------------------------------------

