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
	virtual void ScrollKitSetInfo(iControlScrollInfo *HorzInfo,iControlScrollInfo *VertInfo)noexcept(true)=0;
	//virtual bool ScrollKitSetBarVisible(bool HorizontalVisible,bool VerticalVisible,Float32 HorizontalViewportSize,Float32 VerticalViewportSize)=0;
};
//---------------------------------------------------------------------------
extern rPtr<kiScrollBar> (*gCreateDefaultScrollBarKit)(void)noexcept(true);
//---------------------------------------------------------------------------
template<class TClass,class TInterface=kiScrollBar>
class bwkScrollBar : public bwkControl<TClass,TInterface>
{
public:
	using bwkControl<TClass,TInterface>::bwkControl;

	virtual void ScrollKitSetInfo(iControlScrollInfo *HorzInfo,iControlScrollInfo *VertInfo)noexcept(true)override{
		return (*this)->ScrollKitSetInfo(HorzInfo,VertInfo);
	}
	//virtual bool ScrollKitSetBarVisible(bool HorizontalVisible,bool VerticalVisible,Float32 HorizontalViewportSize,Float32 VerticalViewportSize)noexcept(true)override{
	//	return (*this)->ScrollKitSetBarVisible(HorizontalVisible,VerticalVisible,HorizontalViewportSize,VerticalViewportSize);
	//}
};
//---------------------------------------------------------------------------
class cnLib_INTERFACE viScrollBarGripData : public viData
{
public:
	cFunctionSet<void (void)noexcept(true)> ScrollBarGripNotifySet;
	virtual eAlignment ScrollBarGripGetAlignment(void)noexcept(true)=0;
};
//---------------------------------------------------------------------------
extern rPtr<viControl> (*gCreateDefaultScrollBarGripVisual)(viScrollBarGripData *Data)noexcept(true);
//---------------------------------------------------------------------------
class kControlScrollBar : public LayoutControl , public viScrollBarGripData
{
public:
	kControlScrollBar()noexcept(true);
	~kControlScrollBar()noexcept(true);

	static rPtr<kiScrollBar> Create(void)noexcept(true);

	Float32 BarZPosition=ZPosition_ContentInset;

	class cScrollBarHorizontal : public cControlScrollBar
	{
	protected:
		virtual void ScrollInfoChanged(void)noexcept(true)override;
	}ScrollBarHorizontal;
	class cScrollBarVertical : public cControlScrollBar
	{
	protected:
		virtual void ScrollInfoChanged(void)noexcept(true)override;
	}ScrollBarVertical;
	VisualControl ScrollBarGrip;

	// kiControl

	void SetContentZPosition(Float32 ZPosition)noexcept(true);
	void SetContentVisible(bool Visible)noexcept(true);

	// kiScrollBar

	void ScrollKitSetInfo(iControlScrollInfo *HorzInfo,iControlScrollInfo *VertInfo)noexcept(true);
	bool ScrollKitSetBarVisible(bool HorizontalVisible,bool VerticalVisible,Float32 HorizontalViewportSize,Float32 VerticalViewportSize)noexcept(true);

	// viScrollBarGripData

	virtual eAlignment ScrollBarGripGetAlignment(void)noexcept(true)override;


	Float32 HorizontalBarSize=20;
	Float32 VerticalBarSize=20;
	bool VerticalBarAlignLeft=false;
	bool HorizontalBarAlignTop=false;
	void Update(void)noexcept(true);

protected:

	virtual void ViewSetup(void)noexcept(true)override;
	virtual void ViewClear(void)noexcept(true)override;

	virtual void cnLib_FUNC UILayout(void)noexcept(true)override;
	virtual cUIRectangle cnLib_FUNC UIMargin(const cUIRectangle &Margin)noexcept(true)override;

	bool fContentVisible;
	bool fHorzBarVisible=false;
	bool fVertBarVisible=false;

private:
	cUIRectangle fBarMargin;

	static bool CheckScrollBarVisible(cControlScrollBar &ScrollBar,bool &BarVisible,bool ContentVisible,Float32 BarSize)noexcept(true);
	void HorizontalScrollInfoChanged(void)noexcept(true);
	void VerticalScrollInfoChanged(void)noexcept(true);
};
//---------------------------------------------------------------------------
class cScrollControl : public VisualControl
{
public:
	cScrollControl()noexcept(true);
	~cScrollControl()noexcept(true);

	cCallbackSet ScrollOffsetChangeNotify;
	cCallbackSet ScrollRangeChangeNotify;


	Float32 GetScrollOffset(bool Vertical)const noexcept(true);
	bool SetScrollOffset(bool Vertical,Float32 Offset)noexcept(true);
	cUIPoint GetScrollOffset(void)const noexcept(true);
	bool SetScrollOffset(cUIPoint Offset)noexcept(true);
	
	Float32 GetViewportSize(bool Vertical)const noexcept(true);
	cUIPoint GetViewportSize(void)const noexcept(true);
	cUIRectangle GetViewportRectangle(void)const noexcept(true);
	bool IsRectangleInViewport(const cUIRectangle &Rect)const noexcept(true);

	iControlScrollInfo::cScrollRange GetScrollRange(bool Vertical)const noexcept(true);
	Float32 GetScrollStepSize(bool Vertical)const noexcept(true);
	Float32 GetScrollPageSize(bool Vertical)const noexcept(true);

	cUIRectangle GetVisibleRectangle(void)const noexcept(true);
	bool ScrollRangeToVisible(bool Vertical,Float32 Lowwer,Float32 Upper)noexcept(true);
	bool ScrollRangeToVisible(Float32 Left,Float32 Top,Float32 Right,Float32 Bottom)noexcept(true);
	bool ScrollRectToVisible(cUIPoint Pos,cUIPoint Size)noexcept(true);

protected:

	virtual void cnLib_FUNC RectangleChanged(bool Moved,bool Sized)noexcept(true)override;
	virtual void cnLib_FUNC ContentScaleChanged(void)noexcept(true)override;

	Float32 fScrollOffset[2];
	Float32 fScrollViewportSize[2];
	Float32 fScrollStepSize[2];
	Float32 fScrollPageSize[2];

	Float32 fScrollLowwerLimit[2];
	Float32 fScrollUpperLimit[2];
	bool fScrollNoLowwerLimit[2];
	bool fScrollNoUpperLimit[2];

	void UpdateScrollOffset(void)noexcept(true);
	bool CheckScrollOffset(bool Vertical,Float32 &Offset)noexcept(true);

	virtual void ScrollContentUpdateContentLayout(void)noexcept(true);
	virtual void ScrollContentUpdateStepSizes(void)noexcept(true);

	void SetNeedUpdateContentLayout(void)noexcept(true);

	bool fUpdateStepSizesOnViewportSize;

private:
	bool fNeedUpdateContentLayout;
	bool ChangeOffset(bool Vertical,Float32 Offset)noexcept(true);

	class cNotifyUpdateProcedure : public iReference, public iProcedure
	{
	public:
		cScrollControl *Owner;

		rPtr<iAsyncProcedure> WorkProc;

		virtual void cnLib_FUNC Execute(void)noexcept(true)override;
	};
	rPtr<cNotifyUpdateProcedure> fUpdateProcedure;
	void NotifyUpdateContent(void)noexcept(true);
};
//---------------------------------------------------------------------------
class lScrollFrame : public LayoutControl
{
public:
	lScrollFrame()noexcept(true);
	~lScrollFrame()noexcept(true);

	cScrollControl* GetScrollControl(void)const noexcept(true);
	void SetScrollControl(cScrollControl *Control)noexcept(true);

	class cScrollInfoHorizontal : public iControlScrollInfo
	{
		lScrollFrame* GetHost(void)noexcept(true);
		const lScrollFrame* GetHost(void)const noexcept(true);
	public:
		virtual Float32 ScrollGetOffset(void)const noexcept(true)override;
		virtual bool ScrollSetOffset(Float32 Offset)noexcept(true)override;
		virtual cScrollRange ScrollGetScrollRange(void)const noexcept(true)override;
		virtual Float32 ScrollGetStepSize(void)const noexcept(true)override;
		virtual Float32 ScrollGetPageSize(void)const noexcept(true)override;
	}ScrollInfoHorizontal;
	class cScrollInfoVertical : public iControlScrollInfo
	{
		lScrollFrame* GetHost(void)noexcept(true);
		const lScrollFrame* GetHost(void)const noexcept(true);
	public:
		virtual Float32 ScrollGetOffset(void)const noexcept(true)override;
		virtual bool ScrollSetOffset(Float32 Offset)noexcept(true)override;
		virtual cScrollRange ScrollGetScrollRange(void)const noexcept(true)override;
		virtual Float32 ScrollGetStepSize(void)const noexcept(true)override;
		virtual Float32 ScrollGetPageSize(void)const noexcept(true)override;
	}ScrollInfoVertical;

	cUIPoint GetScrollOffset(void)const noexcept(true);
	bool SetScrollOffset(cUIPoint Offset)noexcept(true);

	Float32 GetScrollOffset(bool Vertical)const noexcept(true);
	bool SetScrollOffset(bool Vertical,Float32 Offset)noexcept(true);

	iControlScrollInfo::cScrollRange GetScrollRange(bool Vertical)const noexcept(true);
	Float32 GetScrollStepSize(bool Vertical)const noexcept(true);
	Float32 GetScrollPageSize(bool Vertical)const noexcept(true);

	bool ScrollContentRangeToVisible(bool Vertical,Float32 Lowwer,Float32 Upper)noexcept(true);
	bool ScrollContentRangeToVisible(Float32 Left,Float32 Top,Float32 Right,Float32 Bottom)noexcept(true);
	bool ScrollContentRectToVisible(cUIPoint Pos,cUIPoint Size)noexcept(true);

	
	kiScrollBar* GetScrollBarKit(void)const noexcept(true);
	void SetScrollBarKit(rPtr<kiScrollBar> Kit)noexcept(true);
	template<class TControl>
	void SetScrollBarKit(TControl &Kit)noexcept(true){
		SetScrollBarKit(rCreate< bwkScrollBar<TControl&> >(Kit));
	}

	sfInt16 ScrollBarKitLayoutOrder=0;
	void UpdateScrollBarLayoutOrder(void)noexcept(true);

protected:
	
	virtual void ViewSetup(void)noexcept(true)override;
	virtual void ViewClear(void)noexcept(true)override;


	cScrollControl *fScrollControl;
	
	rPtr<kiScrollBar> fScrollBarKit;
	virtual void ControlSetupDefaultScrollBarKit(void)noexcept(true);

	iFunctionToken *fScrollOffsetChangeNotifyToken;
	iFunctionToken *fScrollRangeChangeNotifyToken;

	void ScrollOffsetChanged(void)noexcept(true);
	void ScrollRangeChanged(void)noexcept(true);

};
//---------------------------------------------------------------------------
class cnLib_INTERFACE iScrollContent
{
public:
	virtual void ScrollContentShow(void)noexcept(true)=0;
	virtual void ScrollContentHide(void)noexcept(true)=0;
	virtual void ScrollContentUpdate(void)noexcept(true)=0;
};
//---------------------------------------------------------------------------
class cnLib_INTERFACE iScrollContentContainer
{
public:
	virtual iUIView* GetScrollView(void)noexcept(true)=0;
	virtual iScrollContent* GetContent(void)noexcept(true)=0;
	virtual void SetContent(iScrollContent *ScrollContent)noexcept(true)=0;

	virtual cUIPoint GetOffset(void)noexcept(true)=0;
	virtual bool SetOffset(cUIPoint Offset)noexcept(true)=0;
	virtual cUIRange GetScrollLimitX(bool &NoLowwerLimit,bool &NoUpperLimit)noexcept(true)=0;
	virtual void SetScrollLimitX(cUIRange OffsetRange,bool NoLowwerLimit,bool NoUpperLimit)noexcept(true)=0;
	virtual cUIRange GetScrollLimitY(bool &NoLowwerLimit,bool &NoUpperLimit)noexcept(true)=0;
	virtual void SetScrollLimitY(cUIRange OffsetRange,bool NoLowwerLimit,bool NoUpperLimit)noexcept(true)=0;
	virtual cUIPoint GetViewportSize(void)noexcept(true)=0;
	virtual void NotifyUpdateScroll(void)noexcept(true)=0;
	virtual bool IsRectangleInViewport(const cUIRectangle &Rect)noexcept(true)=0;

};
//---------------------------------------------------------------------------
class cScrollContainer : public cScrollControl, public iScrollContentContainer
{
public:
	cScrollContainer()noexcept(true);
	~cScrollContainer()noexcept(true);

	virtual iUIView* GetScrollView(void)noexcept(true)override final;
	virtual iScrollContent* GetContent(void)noexcept(true)override final;
	virtual void SetContent(iScrollContent *ScrollItem)noexcept(true)override final;

	virtual cUIPoint GetOffset(void)noexcept(true)override final;
	virtual bool SetOffset(cUIPoint Offset)noexcept(true)override final;
	virtual cUIRange GetScrollLimitX(bool &NoLowwerLimit,bool &NoUpperLimit)noexcept(true)override final;
	virtual void SetScrollLimitX(cUIRange OffsetRange,bool NoLowwerLimit,bool NoUpperLimit)noexcept(true)override final;
	virtual cUIRange GetScrollLimitY(bool &NoLowwerLimit,bool &NoUpperLimit)noexcept(true)override final;
	virtual void SetScrollLimitY(cUIRange OffsetRange,bool NoLowwerLimit,bool NoUpperLimit)noexcept(true)override final;
	virtual cUIPoint GetViewportSize(void)noexcept(true)override final;
	virtual void NotifyUpdateScroll(void)noexcept(true)override final;
	virtual bool IsRectangleInViewport(const cUIRectangle &Rect)noexcept(true)override final;

protected:
	virtual void ViewSetup(void)noexcept(true)override;
	virtual void ViewClear(void)noexcept(true)override;

	virtual void ScrollContentUpdateContentLayout(void)noexcept(true)override;

	iScrollContent *fScrollContent;

};
//---------------------------------------------------------------------------
class cScrollViewContainer : public cScrollControl
{
public:
	cScrollViewContainer()noexcept(true);
	~cScrollViewContainer()noexcept(true);

	class bcViewItem
	{
	public:
		iUIView *GetView(void)const noexcept(true);
		bool IsVisible(void)const noexcept(true);
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
		cViewItem()noexcept(true);
		~cViewItem()noexcept(true);
	
		bool Attach(cScrollViewContainer *Owner)noexcept(true);
		bool Detach(cScrollViewContainer *Owner)noexcept(true);
		
		iViewReference* GetViewReference(void)const noexcept(true);
		bool SetViewReference(iViewReference *ViewReference)noexcept(true);

		const cUIRectangle& GetRectangle(void)const noexcept(true);
		void SetRectangle(cUIRectangle Rect)noexcept(true);
		cUIRect CalculateLayoutRect(void)const noexcept(true);

		cFunctionSet<void (void)noexcept(true)> OnVisibleChanged;
	protected:
		friend cScrollViewContainer;
		virtual void VisibilityChanged(void)noexcept(true);
		cScrollViewContainer *fOwner;
	};
	
	rPtr<iViewReference> ReclaimView(tTypeID ImplementationType)noexcept(true);

	template<class T>
	rPtr< cControlViewReference<T> > ReclaimControl(void)noexcept(true){
		auto ViewReference=ReclaimView(cnVar::TTypeID< cControlViewReference<T> >::Value);
		return rPtr< cControlViewReference<T> >::TakeFromManual(static_cast<cControlViewReference<T>*>(ViewReference.ExtractToManual())); 
	}

protected:
	
	virtual void ViewSetup(void)noexcept(true)override;
	virtual void ViewClear(void)noexcept(true)override;

	virtual void cnLib_FUNC UILayout(void)noexcept(true)override;

	virtual void ScrollContentUpdateContentLayout(void)noexcept(true)override;

	void ViewItemAttach(bcViewItem *Item)noexcept(true);
	void ViewItemDetach(bcViewItem *Item)noexcept(true);

	const cUIRectangle& ViewItemGetRectangle(const bcViewItem *Item)noexcept(true);
	void ViewItemSetRectangle(bcViewItem *Item,cUIRectangle Rect)noexcept(true);
	cUIRect ViewItemCalculateLayoutRect(const bcViewItem *Item)const noexcept(true);
	iViewReference* ViewItemGetViewReference(const bcViewItem *Item)const noexcept(true);
	bool ViewItemSetViewReference(bcViewItem *Item,iViewReference *ViewReference)noexcept(true);
	void ViewItemCheckVisible(bcViewItem *Item)noexcept(true);

private:
	cnRTL::cSeqSet<bcViewItem*> fVisibleSet;
	cnRTL::cSeqMap<tTypeID, cnRTL::cSeqList< rPtr<iViewReference> > > fViewRecycleMap;

	void RecycleView(rPtr<iViewReference> ViewReference)noexcept(true);

	cnRTL::cSeqSet<cViewItem*> fContentViewSet;
	void ContentViewAttach(cViewItem *Item)noexcept(true);
	void ContentViewDetach(cViewItem *Item)noexcept(true);
	
};
typedef cScrollViewContainer::cViewItem cScrollViewItem;
//---------------------------------------------------------------------------
}	// namespace cnUI
}	// namespace cnLibrary
//---------------------------------------------------------------------------
#endif  /* __cplusplus */
/*-------------------------------------------------------------------------*/
#endif
