/*- cnWinCLI - Managed - WPF Framework ------------------------------------*/
/*         Developer : Code009                                             */
/*         Create on : 2020-04-15                                          */
/*-------------------------------------------------------------------------*/
#pragma once
/*-------------------------------------------------------------------------*/
#if _MANAGED
#using <PresentationCore.dll>
#using <PresentationFramework.dll>
#using <System.Xaml.dll>
#endif	/* _MANAGED */

#include <cnWinCLI\WinCLIM_WPFUIView.h>
#include <cnWinCLI\WinCLIM_WPFUIWindow.h>

#ifdef __cplusplus
//---------------------------------------------------------------------------
namespace cnLibrary{
//---------------------------------------------------------------------------
#if _MANAGED
//---------------------------------------------------------------------------
namespace DNet{
//---------------------------------------------------------------------------
public ref class WPFWindow : System::Windows::Window, IWPFUIViewRootTargetElement
{
protected:

	// IWPFUIViewRootTargetElement

	property WPFUIViewRootAdapter %Adapter{
		virtual WPFUIViewRootAdapter% get(void)=IWPFUIViewRootTargetElement::Adapter::get;
	}

	property System::Windows::UIElement ^ClientElement{
		virtual System::Windows::UIElement^ get(void)=IWPFUIViewRootTargetElement::ClientElement::get;
		virtual void set(System::Windows::UIElement^)=IWPFUIViewRootTargetElement::ClientElement::set;
	}

	property IWPFUIViewRootTargetElement^ OwnerTargetElement{
		virtual IWPFUIViewRootTargetElement^ get()=IWPFUIViewRootTargetElement::OwnerTargetElement::get;
		virtual void set(IWPFUIViewRootTargetElement ^Owner)=IWPFUIViewRootTargetElement::OwnerTargetElement::set;
	}

	property bool IsActive{
		virtual bool get()=IWPFUIViewRootTargetElement::IsActive::get;
	}

	virtual void OnActivated(System::EventArgs ^e)override;

	virtual bool SetBackgroundColor(System::Windows::Media::Color Color)=IWPFUIViewRootTargetElement::SetBackgroundColor;

	WPFUIViewRootAdapter fViewRootAdapter;

};
//---------------------------------------------------------------------------
public ref class WPFFrameworkElement : System::Windows::FrameworkElement, IWPFUIViewTargetElement
{
public:
	property WPFUIViewAdapter% Adapter{
		virtual WPFUIViewAdapter% get(void)=IWPFUIViewTargetElement::Adapter::get;
	};

	property System::Windows::DependencyObject ^ViewParent{
		virtual System::Windows::DependencyObject ^get(void)=IWPFUIViewTargetElement::ViewParent::get;
	}

	virtual bool ViewAddVisualChild(System::Windows::UIElement ^Child)=IWPFUIViewTargetElement::ViewAddVisualChild;
	virtual bool ViewRemoveVisualChild(System::Windows::UIElement ^Child)=IWPFUIViewTargetElement::ViewRemoveVisualChild;

public protected:
	virtual void OnVisualParentChanged(System::Windows::DependencyObject ^oldParent)override;
protected:

	WPFUIViewAdapter fViewAdapter;

	// view children

	virtual System::Windows::Media::Visual^ GetVisualChild(int index)override;
	virtual property int VisualChildrenCount{
		virtual int get(void)override;
	};

	// layout
	
	//virtual void OnDpiChanged (System::Windows::DpiScale ^oldDpi,System::Windows::DpiScale ^newDpi)override;
	virtual System::Windows::Size MeasureOverride(System::Windows::Size availableSize)override;
	virtual System::Windows::Size ArrangeOverride(System::Windows::Size finalSize)override;
	virtual void OnRender(System::Windows::Media::DrawingContext^ drawingContext)override;
	virtual System::Windows::Media::GeometryHitTestResult^ HitTestCore(System::Windows::Media::GeometryHitTestParameters ^ hitTestParameters)override;
	virtual System::Windows::Media::HitTestResult^ HitTestCore(System::Windows::Media::PointHitTestParameters ^ hitTestParameters)override;


};
//---------------------------------------------------------------------------
}	// namespace DNet
//---------------------------------------------------------------------------
#endif // _MANAGED
//---------------------------------------------------------------------------
}	// namespace cnLibrary
//---------------------------------------------------------------------------
#endif  /* __cplusplus */
/*-------------------------------------------------------------------------*/


