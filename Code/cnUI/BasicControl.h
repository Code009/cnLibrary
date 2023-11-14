/*- cnUI - Basic Controls -------------------------------------------------*/
/*         Developer : Code009                                             */
/*         Create on : 2009-03-22                                          */
/*-------------------------------------------------------------------------*/
#ifndef __cnLibrary_cnUI_BasicControl_H__
#define __cnLibrary_cnUI_BasicControl_H__
/*-------------------------------------------------------------------------*/
#include <cnUI/ControlBase.h>

#ifdef __cplusplus
//---------------------------------------------------------------------------
namespace cnLibrary{
//---------------------------------------------------------------------------
namespace cnUI{
//---------------------------------------------------------------------------
class lFrame : public VisualControl
{
public:
	lFrame();
	~lFrame();

	iUIView* GetClientView(void);
	void SetClientView(iUIView *View);

	cUIRectangle ClientMargin;

protected:
	iPtr<iUIView> fClientView;

	virtual void ViewSetup(void)override;
	virtual void ViewClear(void)override;
	virtual void cnLib_FUNC UILayout(void)override;
};
//---------------------------------------------------------------------------
class cContent : public Control
{
public:
	cContent();
	~cContent();

	Float32 ContentZPosition=ZPosition_Content;
	void UpdateContentZPosition(void);

	void SetContent(rPtr<viControl> Content);
	template<class TVisual>
	void SetContent(TVisual &Visual){
		SetContent(rCreate< bwvControl<TVisual&> >(Visual));
	}

protected:
	rPtr<viControl> fContent;
	virtual void ControlContentSetDefault(void);
	virtual void ControlContentChanged(void);

	virtual void ViewSetup(void)override;
	virtual void ViewClear(void)override;
};
//---------------------------------------------------------------------------
class cVisual : public VisualControl
{ 
public:
	cVisual();
	~cVisual();

	void SetContent(rPtr<viControl> Content);
	template<class TVisual>
	void SetContent(TVisual &Visual){
		SetContent(rCreate< bwvControl<TVisual&> >(Visual));
	}

	virtual void UpdateControlZOffset(void)override;

protected:
	rPtr<viControl> fContent;
	virtual Float32 ControlContentZPosition(void);
	virtual void ControlContentSetDefault(void);

	virtual void ViewSetup(void)override;
	virtual void ViewClear(void)override;
};
//---------------------------------------------------------------------------
class cnLib_INTERFACE viTextControlData : public viData
{
public:
	cFunctionSet<void (void)> ControlTextNotifySet;
	virtual const uChar16* ControlTextString(void)=0;
	virtual uIntn ControlTextLength(void)=0;
	virtual const cUITextStyle& ControlTextStyle(void)=0;
	virtual cUIRectangle ControlTextMargin(void)=0;
	virtual eAlignment ControlTextAlignment(void)=0;
};
//---------------------------------------------------------------------------
class vLabel : public vSimpleViewPainter
{
public:
	vLabel(viTextControlData *Data=nullptr);
	~vLabel();

	static rPtr<viControl> Create(viTextControlData *Data);

	viTextControlData* GetData(void)const;
	void SetData(viTextControlData *Data);

protected:

	dPtr<viTextControlData> fData;

	virtual void cnLib_FUNC Paint(iUISimplePaintContext *Context)override;
private:
	void DataInsertCallback(void);
	void DataRemoveCallback(void);

	iPtr<iUITextGraph> fCacheTextGraph;
	cUIPoint fTextGraphSize;
	bool fUpdateText;

	void SetupTextCache(void);

	iFunctionToken *fTextControlNotifyToken;
	void Update(void);

};
//---------------------------------------------------------------------------
class cLabel : public cVisual, public viTextControlData
{ 
public:
	cLabel();
	~cLabel();

	cString<uChar16> Text;
	eAlignment TextAlign=Alignment::Center;

	void Update(void);
	cUITextStyle TextStyle;

	// viTextControlData

	virtual const uChar16* ControlTextString(void)override;
	virtual uIntn ControlTextLength(void)override;
	virtual const cUITextStyle& ControlTextStyle(void)override;
	virtual cUIRectangle ControlTextMargin(void)override;
	virtual eAlignment ControlTextAlignment(void)override;

protected:

	virtual void ControlContentSetDefault(void)override;
};
//---------------------------------------------------------------------------
cnLib_ENUM_BEGIN(ufInt8,ButtonState)
{
	Normal,
	Hot,
	Pressed,
	Disabled,
	Inactive,
}cnLib_ENUM_END(ButtonState);
//---------------------------------------------------------------------------
class cnLib_INTERFACE viButtonData : public viData
{
public:
	cFunctionSet<void (void)> ButtonNotifySet;
	virtual eButtonState ButtonState(void)=0;
	virtual ufInt8 CheckState(void)=0;
};
//---------------------------------------------------------------------------
class vButton : public ViewControl, public vSimpleViewPainter
{
public:
	vButton(viButtonData *Data=nullptr);
	~vButton();

	static rPtr<viControl> Create(viButtonData *Data);
	void SetView(iUIView *View);
	void SetContentZPosition(Float32 ZPosition);

	viButtonData* GetData(void)const;
	void SetData(viButtonData *Data);

protected:
	dPtr<viButtonData> fData;

	virtual void cnLib_FUNC Paint(iUISimplePaintContext *Context)override;
	void Update(void);
	
	virtual cUIRectangle cnLib_FUNC UIMargin(const cUIRectangle &Margin)override;

	static const cUIColor StateColorNormal;
	static const cUIColor StateColorHot;
	static const cUIColor StateColorPressed;
	static const cUIColor StateColorDisabled;
private:
	void DataInsertCallback(void);
	void DataRemoveCallback(void);

	iFunctionToken *fButtonNotifyToken;
};
//---------------------------------------------------------------------------
extern rPtr<viControl> (*gCreateDefaultButtonVisual)(viButtonData *Data);
//---------------------------------------------------------------------------
class bcButton : public Control, public viButtonData
{
public:
	bcButton();
	~bcButton();

	Float32 BackgroundZPosition=ZPosition_Background;
	void UpdateZPosition(void);

	void SetBackground(rPtr<viControl> Background);
	template<class TVisual>
	void SetBackground(TVisual &Visual){
		SetBackground(rCreate< bwvControl<TVisual&> >(Visual));
	}

	virtual eButtonState ButtonState(void)override;
	virtual ufInt8 CheckState(void)override;

protected:
	bool fHot		:1;
	bool fMouseDown	:1;
	bool fTouchDown	:1;
	eMouseButton fMouseDownBtn;

	eButtonState GetButtonState(void)const;

	rPtr<viControl> fBackground;


	void* fTouchDownPointID;

	virtual void ViewSetup(void)override;
	virtual void ViewClear(void)override;

	virtual void cnLib_FUNC MouseEnter(iUIMouseEvent *MouseEvent)override;
	virtual void cnLib_FUNC MouseLeave(iUIMouseEvent *MouseEvent)override;
	virtual void cnLib_FUNC MouseMove(iUIMouseEvent *MouseEvent)override;
	virtual void cnLib_FUNC MouseDown(iUIMouseEvent *MouseEvent,eMouseButton Button)override;
	virtual void cnLib_FUNC MouseUp(iUIMouseEvent *MouseEvent,eMouseButton Button)override;

	virtual void cnLib_FUNC TouchBegin(iUITouchEvent *TouchEvent)override;
	virtual void cnLib_FUNC TouchEnd(iUITouchEvent *TouchEvent)override;
	virtual void cnLib_FUNC TouchLost(iUITouchEvent *TouchEvent)override;
	virtual void cnLib_FUNC TouchMove(iUITouchEvent *TouchEvent)override;

	virtual void ButtonBackgroundSetDefault(void);
	virtual void ButtonBackgroundChanged(void);

	virtual bool ButtonMouseAllowButton(eMouseButton Button){	UnusedParameter(Button);	return true;	}
	virtual void ButtonClick(const cUIPoint &Pos){UnusedParameter(Pos);	}
	virtual void ButtonStateChanged(void);

};
//---------------------------------------------------------------------------
class cButton : public bcButton
{
public:
	cButton();
	~cButton();

	cFunction<void (void)> OnClick;

protected:
	virtual void ButtonClick(const cUIPoint &Pos)override;
};
//---------------------------------------------------------------------------
class bcTextButton : public bcButton, public viTextControlData
{
public:
	bcTextButton();
	~bcTextButton();

	cUITextStyle TextStyle;
	eAlignment TextAlign=Alignment::Center;
	sfInt8 ContentMarginLeft=0;
	sfInt8 ContentMarginTop=0;
	sfInt8 ContentMarginRight=0;
	sfInt8 ContentMarginBottom=0;

	Float32 ContentZPosition=ZPosition_Content;
	void UpdateZPosition(void);

	void SetContent(rPtr<viControl> Content);
	template<class TVisual>
	void SetContent(TVisual &Visual){
		SetContent(rCreate< bwvControl<TVisual&> >(Visual));
	}


	// viTextControlData
	
	virtual const uChar16* ControlTextString(void)override;
	virtual uIntn ControlTextLength(void)override;
	virtual const cUITextStyle& ControlTextStyle(void)override;
	virtual cUIRectangle ControlTextMargin(void)override;
	virtual eAlignment ControlTextAlignment(void)override;

protected:

	virtual void ViewSetup(void)override;
	virtual void ViewClear(void)override;

	rPtr<viControl> fContent;
	virtual void ButtonContentSetDefault(void);
};
//---------------------------------------------------------------------------
class cTextButton : public bcTextButton
{
public:
	cTextButton();
	~cTextButton();

	cString<uChar16> Text;
	ufInt8 Check=0;

	void Update(void);
	void UpdateState(void);

	// viButtonData

	virtual ufInt8 CheckState(void)override;

	// viTextControlData

	virtual const uChar16* ControlTextString(void)override;
	virtual uIntn ControlTextLength(void)override;
	
	cFunction<void (void)> OnClick;

protected:
	virtual bool ButtonMouseAllowButton(eMouseButton Button)override;
	virtual void ButtonClick(const cUIPoint &Pos)override;
};
//---------------------------------------------------------------------------
cnLib_ENUM_BEGIN(ufInt8,ScrollBarButton)
{
	None,
	Dec,
	DecBar,
	Track,
	IncBar,
	Inc,
}cnLib_ENUM_END(ScrollBarButton);
//---------------------------------------------------------------------------
class cnLib_INTERFACE viScrollBarData : public viData
{
public:
	cFunctionSet<void (void)> ScrollBarNotifySet;
	virtual eScrollBarButton ScrollBarActiveButton(void)=0;
	virtual eButtonState ScrollBarButtonState(void)=0;

	struct cScrollPosition
	{
		cUIRange View;
		cUIRange Total;
	};
	virtual cScrollPosition ScrollBarPosition(void)=0;
};
//---------------------------------------------------------------------------
class cnLib_INTERFACE viScrollBar : public viControl
{
public:
	virtual eScrollBarButton ScrollHitTest(Float32 x,Float32 y)=0;
	virtual Float32 ScrollOffsetToValue(Float32 OffsetX,Float32 OffsetY)=0;
};
//---------------------------------------------------------------------------
template<class TClass,class TInterface=viScrollBar>
class bwvScrollBar : public bwvControl<TClass,TInterface>
{
public:
	using bwvControl<TClass,TInterface>::bwvControl;

	virtual eScrollBarButton ScrollHitTest(Float32 x,Float32 y)override{
		return (*this)->ScrollHitTest(x,y);
	}
	virtual Float32 ScrollOffsetToValue(Float32 OffsetX,Float32 OffsetY)override{
		return (*this)->ScrollOffsetToValue(OffsetX,OffsetY);
	}
};
//---------------------------------------------------------------------------
extern rPtr<viScrollBar> (*gCreateDefaultScrollBarVisual)(viScrollBarData *Data);
//---------------------------------------------------------------------------
class bcScrollBar : public Control, public viScrollBarData
{
public:
	bcScrollBar();
	~bcScrollBar();

	Float32 ContentZPosition=ZPosition_Content;
	void UpdateZPosition(void);

	void SetContent(rPtr<viScrollBar> Content);
	template<class TVisual>
	void SetContent(TVisual &Visual){
		SetContent(rCreate< bwvScrollBar<TVisual&> >(Visual));
	}

	virtual eScrollBarButton ScrollBarActiveButton(void)override;
	virtual eButtonState ScrollBarButtonState(void)override;
	virtual cScrollPosition ScrollBarPosition(void)override;

protected:
	eScrollBarButton fActiveButton=ScrollBarButton::None;
	bool fHot			:1;
	bool fButtonDown	:1;

	rPtr<viScrollBar> fContent;
	virtual void ControlContentSetDefault(void);


	virtual void ViewSetup(void)override;
	virtual void ViewClear(void)override;

	virtual void cnLib_FUNC MouseEnter(iUIMouseEvent *MouseEvent)override;
	virtual void cnLib_FUNC MouseLeave(iUIMouseEvent *MouseEvent)override;
	virtual void cnLib_FUNC MouseMove(iUIMouseEvent *MouseEvent)override;
	virtual void cnLib_FUNC MouseDown(iUIMouseEvent *MouseEvent,eMouseButton Button)override;
	virtual void cnLib_FUNC MouseUp(iUIMouseEvent *MouseEvent,eMouseButton Button)override;
	virtual void cnLib_FUNC MouseWheel(iUIMouseEvent *MouseEvent,Float32 ScrollX,Float32 ScrollY)override;

	void ScrollBarNotify(void);

	virtual void ScrollBarMove(Float32 Offset);
	enum eScrollBarMoveDistance{
		sbMovePage,
		sbMoveStep,
	};
	virtual void ScrollBarMoveAction(bool Negative,eScrollBarMoveDistance Distance);
private:

	class cMousePressTimerProc : public iProcedure
	{
		virtual void cnLib_FUNC Execute(void)override;
	}fMousePressTimerProc;
	rPtr<iAsyncTimer> fMousePressTimer;

	iPtr<iUIMouse> fActiveMouse;
	cUIPoint fTrackPos;
	void MousePress(void);
};
//---------------------------------------------------------------------------
class cScrollBar : public bcScrollBar
{
public:
	cScrollBar();
	~cScrollBar();

	Float32 ScrollMin=0;
	Float32 ScrollMax=0;
	Float32 ScrollCurrent=0;
	Float32 ScrollViewSize=1;
	Float32 ScrollPageSize=1;
	Float32 ScrollStepSize=1;

	void Update(void);

	virtual cScrollPosition ScrollBarPosition(void)override;

	cFunction<void (void)> OnScroll;

protected:
	virtual void ScrollBarMove(Float32 Offset)override;
	virtual void ScrollBarMoveAction(bool Negative,eScrollBarMoveDistance Distance)override;
};
//---------------------------------------------------------------------------
class cnLib_INTERFACE iScrollOffset
{
public:
	cFunctionSet<void (void)> ScrollOffsetNotifySet;

	virtual Float32 ScrollGetOffset(void)const=0;
	virtual bool ScrollSetOffset(Float32 Offset)=0;
};
//---------------------------------------------------------------------------
class cnLib_INTERFACE iControlScrollInfo : public iScrollOffset
{
public:
	cFunctionSet<void (void)> ScrollInfoNotifySet;

	struct cScrollRange
	{
		Float32 ViewportSize;
		Float32 LowwerLimit;
		Float32 UpperLimit;
		bool NoLowwerLimit;
		bool NoUpperLimit;
	};

	virtual cScrollRange ScrollGetScrollRange(void)const=0;
	virtual Float32 ScrollGetStepSize(void)const=0;
	virtual Float32 ScrollGetPageSize(void)const=0;
};
//---------------------------------------------------------------------------
class cControlScrollBar : public bcScrollBar
{
public:
	cControlScrollBar();
	~cControlScrollBar();

	iControlScrollInfo* GetScrollInfo(void)const;
	void SetScrollInfo(iControlScrollInfo *ScrollInfo);

	virtual cScrollPosition ScrollBarPosition(void)override;

	bool NeedScrollBar(void)const;

protected:
	iControlScrollInfo *fScrollInfo=nullptr;

	virtual void ScrollBarMove(Float32 Offset)override;
	virtual void ScrollBarMoveAction(bool Negative,eScrollBarMoveDistance Distance)override;

	virtual void ScrollInfoChanged(void);

private:
	iFunctionToken *fScrollInfoNotifyToken;
	iFunctionToken *fScrollOffsetNotifyToken;
};
//---------------------------------------------------------------------------
class cnLib_INTERFACE viTabData : public viData
{
public:
	cFunctionSet<void (void)> TabNotifySet;
	virtual sfInt16 TabActiveIndex(void)=0;
	virtual sfInt16 TabHotIndex(void)=0;
	
	virtual uIntn TabCount(void)=0;
	struct cTabItem{
		const uChar16 *Text;
		uIntn TextLength;
		cUITextStyle TextStyle;
		sfInt16 TabSize;
		sfInt16 TextMargin;
	};
	virtual cTabItem TabGet(uIntn Index)=0;
};
//---------------------------------------------------------------------------
extern rPtr<viControl> (*gCreateDefaultTabVisual)(viTabData *Data);
//---------------------------------------------------------------------------
class bcTab : public cContent, public viTabData
{
public:
	bcTab();
	~bcTab();

	virtual sfInt16 TabActiveIndex(void)override;
	virtual sfInt16 TabHotIndex(void)override;

	virtual uIntn TabCount(void)override;
	virtual cTabItem TabGet(uIntn Index)override;

protected:
	void TabNotify(void);

	virtual void ControlContentSetDefault(void)override;


	virtual void cnLib_FUNC MouseLeave(iUIMouseEvent *MouseEvent)override;
	virtual void cnLib_FUNC MouseMove(iUIMouseEvent *MouseEvent)override;
	virtual void cnLib_FUNC MouseDown(iUIMouseEvent *MouseEvent,eMouseButton Button)override;
	virtual void cnLib_FUNC MouseUp(iUIMouseEvent *MouseEvent,eMouseButton Button)override;


	sfInt16 TabHitTest(Float32 x,Float32 y);

	virtual void TabClick(sfInt16 Index);

private:
	sfInt16 fHotIndex;
	bool fMouseBtnLeftDown=false;
};
//---------------------------------------------------------------------------
class cTab : public bcTab
{
public:
	cTab();
	~cTab();

	virtual sfInt16 TabActiveIndex(void)override;
	virtual uIntn TabCount(void)override;
	virtual cTabItem TabGet(uIntn Index)override;

	struct cItem{
		cnRTL::cString<uChar16> Text;
		sfInt16 ItemWidth;
	};
	cnRTL::cSeqList<cItem> TabList;


	sfInt16 GetActiveIndex(void)const;
	void SetActiveIndex(sfInt16 Index);
	

	sfInt16 TabTextMargin=0;
	cUITextStyle TextStyle;
	void Update(void);

	cFunction<void (void)> OnTabChanged;
	cFunction<void (sfInt16 TabIndex)> OnClickTab;

protected:
	virtual void TabClick(sfInt16 Index)override;

	sfInt16 fActiveIndex=-1;
};
//---------------------------------------------------------------------------
extern rPtr<viControl> (*gCreateDefaultSplitterBarVisual)(void);
//---------------------------------------------------------------------------
class cSplitterBar : public cContent
{
public:
	cSplitterBar();
	~cSplitterBar();

	eMouseButton MouseActionButton=MouseButton::Left;

	cFunction<void (Float32 Delta)> OnDrag;

	iPtr<iInterface> MouseCursorH;
	iPtr<iInterface> MouseCursorV;

protected:

	bool fBtnDown=false;
	bool fHorization;
	cUIPoint fMoveRefPoint;

	virtual void ControlContentSetDefault(void)override;

	virtual void cnLib_FUNC RectangleChanged(bool Moved,bool Sized)override;

	virtual void cnLib_FUNC MouseLeave(iUIMouseEvent *MouseEvent)override;
	virtual iInterface* cnLib_FUNC MouseGetCursor(iUIMouseEvent *MouseEvent)override;
	virtual void cnLib_FUNC MouseMove(iUIMouseEvent *MouseEvent)override;
	virtual void cnLib_FUNC MouseDown(iUIMouseEvent *MouseEvent,eMouseButton Button)override;
	virtual void cnLib_FUNC MouseUp(iUIMouseEvent *MouseEvent,eMouseButton Button)override;

	void SizeChanged(void);
};
//---------------------------------------------------------------------------
class lEdgeInsets : public LayoutControl
{
public:
	lEdgeInsets();
	~lEdgeInsets();

	iControlComponent* GetInset(eDirection Direction)const;
	void SetInset(rPtr<iControlComponent> Inset,eDirection Direction);
	template<class TControl>
	void SetInset(TControl &Inset,eDirection Direction){
		SetInset(rCreate< bwControlComponent<TControl&> >(Inset),Direction);
	}

	cUIRectangle InsetSizes;

protected:

	virtual void ViewSetup(void)override;
	virtual void ViewClear(void)override;

	virtual cUIRectangle cnLib_FUNC UIMargin(const cUIRectangle &Margin)override;
	virtual void cnLib_FUNC UILayout(void)override;

	struct cInsetItem
	{
		iPtr<iUIView> View;
		rPtr<iControlComponent> Content;
	}fInsets[4];


	cUIRectangle fInsetPos;
};
//---------------------------------------------------------------------------
class cTitledFrame : public VisualControl, public viTextControlData
{
public:
	cTitledFrame();
	~cTitledFrame();

	cUIRectangle TextMargin;
	cString<uChar16> Text;
	cUITextStyle TextStyle;
	uInt32 BackgroundColor=0;

	void Update(void);

	virtual const uChar16* ControlTextString(void)override;
	virtual uIntn ControlTextLength(void)override;
	virtual const cUITextStyle& ControlTextStyle(void)override;
	virtual cUIRectangle ControlTextMargin(void)override;
	virtual eAlignment ControlTextAlignment(void)override;

protected:
	//virtual void ControlBackgroundSetDefault(void)override;
};
//---------------------------------------------------------------------------
}	// namespace cnUI
}	// namespace cnLibrary
//---------------------------------------------------------------------------
#endif  /* __cplusplus */
/*-------------------------------------------------------------------------*/
#endif
