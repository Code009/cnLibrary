/*- cnUI - Control base declarations --------------------------------------*/
/*         Developer : Code009                                             */
/*         Create on : 2009-05-11                                          */
/*-------------------------------------------------------------------------*/
#ifndef __cnLibrary_cnUI_ControlBase_H__
#define __cnLibrary_cnUI_ControlBase_H__

#include <cnUI/UICommon.h>
#include <cnUI/UILayout.h>
#include <cnUI/UIInterface.h>

#ifdef __cplusplus
//---------------------------------------------------------------------------
namespace cnLibrary{
//---------------------------------------------------------------------------
namespace cnUI{
//---------------------------------------------------------------------------
static constexpr sfInt16 LayoutOrder_Background	=	-64;
//---------------------------------------------------------------------------
static constexpr Float32 ZPosition_Background		=	-64.f;
static constexpr Float32 ZPosition_Content			=	-32.f;
static constexpr Float32 ZPosition_ContentInset		=	32.f;
//---------------------------------------------------------------------------
class Form : protected UIStateHandler,protected UIViewHandler
{
public:
	Form();
	~Form();

	operator iUIView *()const;
	iUIView* GetView(void)const;
protected:
	iPtr<iUIView> fView;
};
//---------------------------------------------------------------------------
class ViewControl : protected UIViewHandler
{
public:
	ViewControl();
	~ViewControl();

	iUIView* GetView(void)const;
	void SetView(iUIView *View);
	iUIView* CreateView(void);

	
	sfInt16 LayoutOrder=0;
	virtual void UpdateLayoutOrder(void);
	void SetLayoutOrder(sfInt16 Order);

protected:
	iPtr<iUIView> fView;

	virtual void ViewSetup(void);
	virtual void ViewClear(void);
};
//---------------------------------------------------------------------------
template<class T>
inline iPtr<iUIView> ControlCreateView(T &Control)
{
	return Control.CreateView();
}
//---------------------------------------------------------------------------
template<class T>
inline iPtr<iUIView> ControlCreateView(T &Control,iUIView *ParentView)
{
	auto View=Control.CreateView();
	if(ParentView!=nullptr){
		ParentView->InsertView(View);
	}
	return View;
}
//---------------------------------------------------------------------------
template<class T>
inline void ControlDestroyView(T &Control)
{
	iUIView *View=Control.GetView();
	if(View!=nullptr){
		iUIView *ParentView=View->GetParent();
		if(ParentView!=nullptr){
			ParentView->RemoveView(View);
		}
		Control.SetView(nullptr);
	}
}
//---------------------------------------------------------------------------
template<class T>
inline void ControlDestroyView(T &Control,iUIView *ParentView)
{
	iUIView *View=Control.GetView();
	if(View!=nullptr){
		if(ParentView!=nullptr){
			ParentView->RemoveView(View);
		}
		Control.SetView(nullptr);
	}
}
//---------------------------------------------------------------------------
template<class T>
inline bool ControlSetParent(const T &Control,iUIView *ParentView)
{
	return ViewSetParent(Control.GetView(),ParentView);
}
//---------------------------------------------------------------------------
template<class T>
inline bool ControlGetPos(T &Control,iUIArea *Relative,cUIPoint &Pos)
{
	return ViewGetPos(Control.GetView(),Relative,Pos);
}
//---------------------------------------------------------------------------
template<class T>
inline bool ControlSetPos(T &Control,iUIArea *Relative,const cUIPoint &Pos)
{
	return ViewSetPos(Control.GetView(),Relative,Pos);
}
//---------------------------------------------------------------------------
template<class T>
inline cUIPoint ControlGetSize(T &Control)
{
	return ViewGetSize(Control.GetView());
}
//---------------------------------------------------------------------------
template<class T>
inline bool ControlSetSize(T &Control,const cUIPoint &Size)
{
	return ViewSetSize(Control.GetView(),Size);
}
//---------------------------------------------------------------------------
template<class T>
inline bool ControlGetRect(T &Control,iUIArea *Relative,cUIPoint &Pos,cUIPoint &Size)
{
	return ViewGetRect(Control.GetView(),Relative,Pos,Size);
}
//---------------------------------------------------------------------------
template<class T>
inline bool ControlSetRect(T &Control,iUIArea *Relative,const cUIPoint &Pos,const cUIPoint &Size)
{
	return ViewSetRect(Control.GetView(),Relative,Pos,Size);
}
//---------------------------------------------------------------------------
template<class T>
inline bool ControlGetRect(T &Control,iUIArea *Relative,cUIRect &Rect)
{
	return ViewGetRect(Control.GetView(),Relative,Rect);
}
//---------------------------------------------------------------------------
template<class T>
inline bool ControlSetRect(T &Control,iUIArea *Relative,const cUIRect &Rect)
{
	return ViewSetRect(Control.GetView(),Relative,Rect);
}
//---------------------------------------------------------------------------
template<class T>
inline bool ControlGetVisible(T &Control)
{
	return ViewGetVisible(Control.GetView());
}
//---------------------------------------------------------------------------
template<class T>
inline bool ControlSetVisible(T &Control,bool Visible)
{
	return ViewSetVisible(Control.GetView(),Visible);
}
//---------------------------------------------------------------------------
template<class T>
inline bool ControlGetEnable(T &Control)
{
	return ViewGetEnable(Control.GetView());
}
//---------------------------------------------------------------------------
template<class T>
inline bool ControlSetEnable(T &Control,bool Enable)
{
	return ViewSetEnable(Control.GetView(),Enable);
}
//---------------------------------------------------------------------------
template<class T>
inline sfInt16 ControlGetZIndex(T &Control)
{
	return ViewGetZIndex(Control.GetView());
}
//---------------------------------------------------------------------------
template<class T>
inline bool ControlSetZIndex(T &Control,sfInt16 ZIndex)
{
	return ViewSetZIndex(Control.GetView(),ZIndex);
}
//---------------------------------------------------------------------------
// Control
//	response user input in a view
class Control : public ViewControl, protected KeyHandler , protected MouseHandler, protected TouchHandler
{
public:
	Control();
	~Control();

protected:

	virtual void ViewSetup(void)override;
	virtual void ViewClear(void)override;
};
//---------------------------------------------------------------------------
class LayoutControl : public ViewControl
{
public:
	LayoutControl();
	~LayoutControl();

	Float32 ControlZOffset=0;
	virtual void UpdateControlZOffset(void);

	void SetControlZOffset(Float32 ZOffset);

};
//---------------------------------------------------------------------------
class VisualControl : public LayoutControl
{
public:
	VisualControl();
	~VisualControl();

	virtual void UpdateControlZOffset(void)override;

	void SetContentVisible(bool Visible);

	void SetBackground(rPtr<viControl> Background);
	template<class TVisual>
	void SetBackground(TVisual &Visual){
		SetBackground(rCreate< bwvControl<TVisual&> >(Visual));
	}
protected:

	virtual void ViewSetup(void)override;
	virtual void ViewClear(void)override;

	virtual void ControlBackgroundSetDefault(void);
	virtual Float32 ControlBackgroundZPosition(void);
	virtual void ControlBackgroundChanged(void);

	rPtr<viControl> fBackground;
};
//---------------------------------------------------------------------------
class vSimpleViewPainter : protected iUISimplePainter
{
public:
	vSimpleViewPainter();
	~vSimpleViewPainter();

	void SetView(iUIView *View);
	void SetContentZPosition(Float32 ZPosition);
	void SetContentVisible(bool Visible);
protected:
	rPtr<iUISimpleViewContent> fViewContent;
	cUIPoint fPaintSize;

	virtual void cnLib_FUNC PaintStarted(void)override;
	virtual void cnLib_FUNC PaintShow(void)override;
	virtual void cnLib_FUNC PaintResume(void)override;
	virtual void cnLib_FUNC PaintPaused(void)override;
	virtual void cnLib_FUNC PaintHide(void)override;
	virtual void cnLib_FUNC PaintStopped(void)override;
	virtual void cnLib_FUNC PaintRectChanged(void)override;
};
//---------------------------------------------------------------------------
class KeyAction : public ViewControl, protected KeyHandler
{
public:
	KeyAction();
	~KeyAction();

	bool KeyFilter=false;
protected:
	bool fKeyRegisterAsFilter;
	virtual void ViewSetup(void)override;
	virtual void ViewClear(void)override;
};
//---------------------------------------------------------------------------
class MouseAction : public ViewControl, protected MouseHandler
{
public:
	MouseAction();
	~MouseAction();

	bool MouseFilter=false;
protected:
	bool fMouseRegisterAsFilter;
	virtual void ViewSetup(void)override;
	virtual void ViewClear(void)override;
};
//---------------------------------------------------------------------------
class TouchAction : public ViewControl, protected TouchHandler
{
public:
	TouchAction();
	~TouchAction();

	bool TouchFilter=false;
protected:
	bool fTouchRegisterAsFilter;
	virtual void ViewSetup(void)override;
	virtual void ViewClear(void)override;
};
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
extern void (*gApplyDefaultTextStyle)(cUITextStyle &TextStyle);
void ApplyDefaultTextStyle(cUITextStyle &TextStyle);
//---------------------------------------------------------------------------
class cnLib_INTERFACE viControlStateData : public viData
{
public:
	cFunctionSet<void (void)> ControlStateNotifySet;
	virtual bool ControlHot(void)=0;
	virtual bool ControlFocused(void)=0;
	virtual bool ControlDisabled(void)=0;
};
//---------------------------------------------------------------------------
}//	namespace cnUI
//---------------------------------------------------------------------------
}	// namespace cnLibrary
//---------------------------------------------------------------------------
#endif  /* __cplusplus */
/*-------------------------------------------------------------------------*/
#endif
