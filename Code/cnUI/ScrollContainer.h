/*- cnUI - Scroll Container -----------------------------------------------*/
/*         Developer : Code009                                             */
/*         Create on : 2019-07-27                                          */
/*-------------------------------------------------------------------------*/
#ifndef __cnLibrary_cnUI_ScrollContainer_H__
#define __cnLibrary_cnUI_ScrollContainer_H__
/*-------------------------------------------------------------------------*/
#include <cnUI/ControlBase.h>
#include <cnUI/BasicControl.h>

#ifdef __cplusplus
//---------------------------------------------------------------------------
namespace cnLibrary{
//---------------------------------------------------------------------------
namespace cnUI{
//---------------------------------------------------------------------------
class cnLib_INTERFACE kiScrollBar : public kiControl
{
public:
	virtual void ScrollKitSetInfo(iControlScrollInfo *HorzInfo,iControlScrollInfo *VertInfo)=0;
	//virtual bool ScrollKitSetBarVisible(bool HorizontalVisible,bool VerticalVisible,Float32 HorizontalViewportSize,Float32 VerticalViewportSize)=0;
};
//---------------------------------------------------------------------------
extern rPtr<kiScrollBar> (*gCreateDefaultScrollBarKit)(void);
//---------------------------------------------------------------------------
template<class TClass,class TInterface=kiScrollBar>
class bwkScrollBar : public bwkControl<TClass,TInterface>
{
public:
	using bwkControl<TClass,TInterface>::bwkControl;

	virtual void ScrollKitSetInfo(iControlScrollInfo *HorzInfo,iControlScrollInfo *VertInfo)override{
		return (*this)->ScrollKitSetInfo(HorzInfo,VertInfo);
	}
	//virtual bool ScrollKitSetBarVisible(bool HorizontalVisible,bool VerticalVisible,Float32 HorizontalViewportSize,Float32 VerticalViewportSize)override{
	//	return (*this)->ScrollKitSetBarVisible(HorizontalVisible,VerticalVisible,HorizontalViewportSize,VerticalViewportSize);
	//}
};
//---------------------------------------------------------------------------
class cnLib_INTERFACE viScrollBarGripData : public viData
{
public:
	cFunctionSet<void (void)> ScrollBarGripNotifySet;
	virtual eAlignment ScrollBarGripGetAlignment(void)=0;
};
//---------------------------------------------------------------------------
extern rPtr<viControl> (*gCreateDefaultScrollBarGripVisual)(viScrollBarGripData *Data);
//---------------------------------------------------------------------------
class kControlScrollBar : public LayoutControl , public viScrollBarGripData
{
public:
	kControlScrollBar();
	~kControlScrollBar();

	static rPtr<kiScrollBar> Create(void);

	Float32 BarZPosition=ZPosition_ContentInset;

	class cScrollBarHorizontal : public cControlScrollBar
	{
	protected:
		virtual void ScrollInfoChanged(void)override;
	}ScrollBarHorizontal;
	class cScrollBarVertical : public cControlScrollBar
	{
	protected:
		virtual void ScrollInfoChanged(void)override;
	}ScrollBarVertical;
	VisualControl ScrollBarGrip;

	// kiControl

	void SetContentZPosition(Float32 ZPosition);
	void SetContentVisible(bool Visible);

	// kiScrollBar

	void ScrollKitSetInfo(iControlScrollInfo *HorzInfo,iControlScrollInfo *VertInfo);
	bool ScrollKitSetBarVisible(bool HorizontalVisible,bool VerticalVisible,Float32 HorizontalViewportSize,Float32 VerticalViewportSize);

	// viScrollBarGripData

	virtual eAlignment ScrollBarGripGetAlignment(void)override;


	Float32 HorizontalBarSize=20;
	Float32 VerticalBarSize=20;
	bool VerticalBarAlignLeft=false;
	bool HorizontalBarAlignTop=false;
	void Update(void);

protected:

	virtual void ViewSetup(void)override;
	virtual void ViewClear(void)override;

	virtual void cnLib_FUNC UILayout(void)override;
	virtual cUIRectangle cnLib_FUNC UIMargin(const cUIRectangle &Margin)override;

	bool fContentVisible;
	bool fHorzBarVisible=false;
	bool fVertBarVisible=false;

private:
	cUIRectangle fBarMargin;

	static bool CheckScrollBarVisible(cControlScrollBar &ScrollBar,bool &BarVisible,bool ContentVisible,Float32 BarSize);
	void HorizontalScrollInfoChanged(void);
	void VerticalScrollInfoChanged(void);
};

//---------------------------------------------------------------------------
class cScrollContent : public VisualControl
{
public:
	cScrollContent();
	~cScrollContent();

	cCallbackSet ScrollOffsetChangeNotify;
	cCallbackSet ScrollRangeChangeNotify;

	Float32 GetScrollOffset(bool Vertical)const;
	bool SetScrollOffset(bool Vertical,Float32 Offset);
	cUIPoint GetScrollOffset(void)const;
	bool SetScrollOffset(cUIPoint Offset);
	
	Float32 GetViewportSize(bool Vertical)const;
	cUIPoint GetViewportSize(void)const;
	cUIRectangle GetViewportRectangle(void)const;
	bool IsRectangleInViewport(const cUIRectangle &Rect)const;

	iControlScrollInfo::cScrollRange GetScrollRange(bool Vertical)const;
	Float32 GetScrollStepSize(bool Vertical)const;
	Float32 GetScrollPageSize(bool Vertical)const;

	cUIRectangle GetVisibleRectangle(void)const;
	bool ScrollRangeToVisible(bool Vertical,Float32 Lowwer,Float32 Upper);
	bool ScrollRangeToVisible(Float32 Left,Float32 Top,Float32 Right,Float32 Bottom);
	bool ScrollRectToVisible(cUIPoint Pos,cUIPoint Size);

protected:

	virtual void cnLib_FUNC RectangleChanged(bool Moved,bool Sized)override;
	virtual void cnLib_FUNC ContentScaleChanged(void)override;
	virtual void cnLib_FUNC UILayout(void)override;

	Float32 fScrollOffset[2];
	Float32 fScrollViewportSize[2];
	
	Float32 fScrollLowwerLimit[2];
	Float32 fScrollUpperLimit[2];
	Float32 fScrollStepSize[2];
	Float32 fScrollPageSize[2];
	bool fScrollNoLowwerLimit[2];
	bool fScrollNoUpperLimit[2];

	void UpdateScrollOffset(void);
	void UpdateScrollLimits(void);

	virtual void ScrollContentProcessOffset(bool Vertical,Float32 &Offset);
	virtual void ScrollContentUpdateContentLayout(void);
	virtual void ScrollContentUpdateStepSizes(void);

	void SetNeedUpdateContentLayout(void);

	bool fUpdateStepSizesOnViewportSize;

private:
	bool fNeedUpdateContentLayout;
	bool ChangeOffset(bool Vertical,Float32 Offset);
};
//---------------------------------------------------------------------------
class lScrollFrame : public LayoutControl
{
public:
	lScrollFrame();
	~lScrollFrame();

	cScrollContent* GetScrollContent(void)const;
	void SetScrollContent(cScrollContent *Content);

	class cScrollInfoHorizontal : public iControlScrollInfo
	{
		lScrollFrame* GetHost(void);
		const lScrollFrame* GetHost(void)const;
	public:
		virtual Float32 ScrollGetOffset(void)const override;
		virtual bool ScrollSetOffset(Float32 Offset)override;
		virtual cScrollRange ScrollGetScrollRange(void)const override;
		virtual Float32 ScrollGetStepSize(void)const override;
		virtual Float32 ScrollGetPageSize(void)const override;
	}ScrollInfoHorizontal;
	class cScrollInfoVertical : public iControlScrollInfo
	{
		lScrollFrame* GetHost(void);
		const lScrollFrame* GetHost(void)const;
	public:
		virtual Float32 ScrollGetOffset(void)const override;
		virtual bool ScrollSetOffset(Float32 Offset)override;
		virtual cScrollRange ScrollGetScrollRange(void)const override;
		virtual Float32 ScrollGetStepSize(void)const override;
		virtual Float32 ScrollGetPageSize(void)const override;
	}ScrollInfoVertical;

	cUIPoint GetScrollOffset(void)const;
	bool SetScrollOffset(cUIPoint Offset);

	Float32 GetScrollOffset(bool Vertical)const;
	bool SetScrollOffset(bool Vertical,Float32 Offset);

	iControlScrollInfo::cScrollRange GetScrollRange(bool Vertical)const;
	Float32 GetScrollStepSize(bool Vertical)const;
	Float32 GetScrollPageSize(bool Vertical)const;

	bool ScrollContentRangeToVisible(bool Vertical,Float32 Lowwer,Float32 Upper);
	bool ScrollContentRangeToVisible(Float32 Left,Float32 Top,Float32 Right,Float32 Bottom);
	bool ScrollContentRectToVisible(cUIPoint Pos,cUIPoint Size);

	
	kiScrollBar* GetScrollBarKit(void)const;
	void SetScrollBarKit(rPtr<kiScrollBar> Kit);
	template<class TControl>
	void SetScrollBarKit(TControl &Kit){
		SetScrollBarKit(rCreate< bwkScrollBar<TControl&> >(Kit));
	}

	sfInt16 ScrollBarKitLayoutOrder=0;
	void UpdateScrollBarLayoutOrder(void);

protected:
	
	virtual void ViewSetup(void)override;
	virtual void ViewClear(void)override;


	cScrollContent *fScrollContent;
	
	rPtr<kiScrollBar> fScrollBarKit;
	virtual void ControlSetupDefaultScrollBarKit(void);

	iFunctionToken *fScrollOffsetChangeNotifyToken;
	iFunctionToken *fScrollRangeChangeNotifyToken;

	void ScrollOffsetChanged(void);
	void ScrollRangeChanged(void);

};
//---------------------------------------------------------------------------
class cScrollViewContainer : public cScrollContent
{
public:
	cScrollViewContainer();
	~cScrollViewContainer();

	class bcViewItem
	{
	public:
		iUIView *GetView(void)const;
		bool IsVisible(void)const;
	private:
		friend cScrollViewContainer;
		rPtr<iViewReference> ViewReference;
		iUIView *View=nullptr;
		cUIRectangle ContentRect;
		bool Visible=false;
	};

	class cViewItem : public bcViewItem
	{
	public:
		cViewItem();
		~cViewItem();
	
		bool Attach(cScrollViewContainer *Owner);
		bool Detach(cScrollViewContainer *Owner);
		
		iViewReference* GetViewReference(void)const;
		bool SetViewReference(iViewReference *ViewReference);

		const cUIRectangle& GetRectangle(void)const;
		void SetRectangle(cUIRectangle Rect);
		cUIRect CalculateLayoutRect(void)const;

		cFunctionSet<void (void)> OnVisibleChanged;
	protected:
		friend cScrollViewContainer;
		virtual void VisibilityChanged(void);
		cScrollViewContainer *fOwner;
	};
	
	rPtr<iViewReference> ReclaimView(tTypeID ImplementationType);

	template<class T>
	rPtr< cControlViewReference<T> > ReclaimControl(void){
		auto ViewReference=ReclaimView(cnVar::TTypeID< cControlViewReference<T> >::Value);
		return rPtr< cControlViewReference<T> >::TakeFromManual(static_cast<cControlViewReference<T>*>(ViewReference.ExtractToManual())); 
	}

protected:
	
	virtual void ViewSetup(void)override;
	virtual void ViewClear(void)override;

	virtual void cnLib_FUNC UILayout(void)override;

	virtual void ScrollContentUpdateContentLayout(void)override;

	void ViewItemAttach(bcViewItem *Item);
	void ViewItemDetach(bcViewItem *Item);

	const cUIRectangle& ViewItemGetRectangle(const bcViewItem *Item);
	void ViewItemSetRectangle(bcViewItem *Item,cUIRectangle Rect);
	cUIRect ViewItemCalculateLayoutRect(const bcViewItem *Item)const;
	iViewReference* ViewItemGetViewReference(const bcViewItem *Item)const;
	bool ViewItemSetViewReference(bcViewItem *Item,iViewReference *ViewReference);
	void ViewItemCheckVisible(bcViewItem *Item);

private:
	cnRTL::cSeqSet<bcViewItem*> fVisibleSet;
	cnRTL::cSeqMap<tTypeID, cnRTL::cSeqList< rPtr<iViewReference> > > fViewRecycleMap;

	void RecycleView(rPtr<iViewReference> ViewReference);

	cnRTL::cSeqSet<cViewItem*> fContentViewSet;
	void ContentViewAttach(cViewItem *Item);
	void ContentViewDetach(cViewItem *Item);
	
};
typedef cScrollViewContainer::cViewItem cScrollViewItem;
//---------------------------------------------------------------------------
}	// namespace cnUI
}	// namespace cnLibrary
//---------------------------------------------------------------------------
#endif  /* __cplusplus */
/*-------------------------------------------------------------------------*/
#endif
