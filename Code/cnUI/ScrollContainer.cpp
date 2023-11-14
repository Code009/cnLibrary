#include "ScrollContainer.h"

#include "Shape.h"

using namespace cnLibrary;
using namespace cnRTL;
using namespace cnUI;


rPtr<kiScrollBar> (*cnUI::gCreateDefaultScrollBarKit)(void)=nullptr;

namespace cnLibrary{namespace cnUI{
static rPtr<viControl> CreateDefaultScrollBarGripVisual_SolidStaticColor(viScrollBarGripData*){
	return vSolidStaticColor::Create(UIColorFromUInt32(0xFFCCCCCC));
}
}}

rPtr<viControl> (*cnUI::gCreateDefaultScrollBarGripVisual)(viScrollBarGripData *Data)=CreateDefaultScrollBarGripVisual_SolidStaticColor;

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
kControlScrollBar::kControlScrollBar()
	: fBarMargin{0,0,0,0}
{
	fContentVisible=true;
	if(gCreateDefaultScrollBarGripVisual!=nullptr){
		ScrollBarGrip.SetBackground(gCreateDefaultScrollBarGripVisual(this));
	}
}
//---------------------------------------------------------------------------
kControlScrollBar::~kControlScrollBar()
{
	InvalidateData();
}
//---------------------------------------------------------------------------
rPtr<kiScrollBar> kControlScrollBar::Create(void)
{
	return rCreate< bwkScrollBar<kControlScrollBar> >();
}
//---------------------------------------------------------------------------
void kControlScrollBar::ViewSetup(void)
{
	LayoutControl::ViewSetup();

	{
		auto View=ControlCreateView(ScrollBarHorizontal,fView);
		View->SetZPosition(BarZPosition);
	}
	{
		auto View=ControlCreateView(ScrollBarVertical,fView);
		View->SetZPosition(BarZPosition);
	}
	{
		auto View=ControlCreateView(ScrollBarGrip,fView);
		View->SetZPosition(BarZPosition);
	}
	fVertBarVisible=true;
	fHorzBarVisible=true;

	if(CheckScrollBarVisible(ScrollBarHorizontal,fHorzBarVisible,fContentVisible,HorizontalBarSize) ||  CheckScrollBarVisible(ScrollBarVertical,fVertBarVisible,fContentVisible,VerticalBarSize) ){
		ControlSetVisible(ScrollBarGrip,fContentVisible&&fVertBarVisible&&fHorzBarVisible);
		fView->SetArrangement();
	}
}
//---------------------------------------------------------------------------
void kControlScrollBar::ViewClear(void)
{
	ControlDestroyView(ScrollBarHorizontal,fView);
	ControlDestroyView(ScrollBarVertical,fView);
	ControlDestroyView(ScrollBarGrip,fView);

	LayoutControl::ViewClear();
}
//---------------------------------------------------------------------------
void kControlScrollBar::UILayout(void)
{
	auto ViewSize=fView->GetSize();

	//Float32 fHoriBarPos=fBarMargin.Top;
	//Float32 fVertBarPos=fBarMargin.Right;

	cUIRect Rect;
	Rect.Pos.x=fBarMargin.Left;
	Rect.Pos.y=fBarMargin.Top;
	Rect.Size.x=ViewSize.x-fBarMargin.Left-fBarMargin.Right;
	Rect.Size.y=ViewSize.y-fBarMargin.Top-fBarMargin.Bottom;

	cUIPoint Pos;
	cUIPoint Size;
	if(fHorzBarVisible){
		if(fVertBarVisible){

			Float32 ScrollHoriSize=Rect.Size.x-VerticalBarSize;
			Float32 ScrollVertSize=Rect.Size.y-HorizontalBarSize;

			// vert scroll bar
			Size.y=ScrollVertSize;
			Size.x=VerticalBarSize;
			if(HorizontalBarAlignTop){
				Pos.y=Rect.Pos.y+HorizontalBarSize;
			}
			else{
				Pos.y=Rect.Pos.y;
			}
			if(VerticalBarAlignLeft){
				Pos.x=Rect.Pos.x;
			}
			else{
				Pos.x=Rect.Pos.x+ScrollHoriSize;
			}
			ControlSetRect(ScrollBarVertical,fView,Pos,Size);

			// horz scroll bar
			Size.x=ScrollHoriSize;
			Size.y=HorizontalBarSize;
			if(VerticalBarAlignLeft){
				Pos.x=Rect.Pos.x+VerticalBarSize;
			}
			else{
				Pos.x=Rect.Pos.x;
			}
			if(HorizontalBarAlignTop){
				Pos.y=Rect.Pos.y;
			}
			else{
				Pos.y=Rect.Pos.y+ScrollVertSize;
			}
			ControlSetRect(ScrollBarHorizontal,fView,Pos,Size);

			// scroll bar grip
			Size.x=VerticalBarSize;
			Size.y=HorizontalBarSize;
			if(VerticalBarAlignLeft){
				Pos.x=Rect.Pos.x;
			}
			else{
				Pos.x=Rect.Pos.x+ScrollHoriSize;
			}
			if(HorizontalBarAlignTop){
				Pos.y=Rect.Pos.y;
			}
			else{
				Pos.y=Rect.Pos.y+ScrollVertSize;
			}
			ControlSetRect(ScrollBarGrip,fView,Pos,Size);
		}
		else{
			// horz scroll bar only
			Pos.x=Rect.Pos.x;
			Size.x=Rect.Size.x;
			Size.y=HorizontalBarSize;
			if(HorizontalBarAlignTop){
				Pos.y=Rect.Pos.y;
			}
			else{
				Pos.y=Rect.Pos.y+Rect.Size.y-HorizontalBarSize;
			}
			ControlSetRect(ScrollBarHorizontal,fView,Pos,Size);
		}
	}
	else{
		if(fVertBarVisible){
			// vert scroll bar only
			Pos.y=Rect.Pos.y;
			Size.y=Rect.Size.y;
			Size.x=VerticalBarSize;
			if(VerticalBarAlignLeft){
				Pos.x=Rect.Pos.x;
			}
			else{
				Pos.x=Rect.Pos.x+Rect.Size.x-VerticalBarSize;
			}
			ControlSetRect(ScrollBarVertical,fView,Pos,Size);
		}
	}
}
//---------------------------------------------------------------------------
cUIRectangle kControlScrollBar::UIMargin(const cUIRectangle &Margin)
{
	bool HorzScrollBarVisible=ControlGetVisible(ScrollBarHorizontal);
	bool VertScrollBarVisible=ControlGetVisible(ScrollBarVertical);

	fBarMargin=Margin;

	cUIRectangle InsetMargin=UIRectangleZero;
	if(VertScrollBarVisible){
		if(VerticalBarAlignLeft){
			InsetMargin.Left=VerticalBarSize;
		}
		else{
			InsetMargin.Right=VerticalBarSize;
		}
	}
	if(HorzScrollBarVisible){
		if(HorizontalBarAlignTop){
			InsetMargin.Top=HorizontalBarSize;
		}
		else{
			InsetMargin.Bottom=HorizontalBarSize;
		}
	}
	return InsetMargin;
}
//---------------------------------------------------------------------------
bool kControlScrollBar::CheckScrollBarVisible(cControlScrollBar &ScrollBar,bool &BarVisible,bool ContentVisible,Float32 BarSize)
{
	auto ScrollInfo=ScrollBar.GetScrollInfo();
	if(ScrollInfo==nullptr)
		return false;
	bool Visible=ScrollBar.NeedScrollBar();

	auto ScrollRange=ScrollInfo->ScrollGetScrollRange();

	bool PrevVisible=BarVisible;

	if(ScrollRange.ViewportSize<BarSize*2){
		Visible=false;
	}

	if(PrevVisible==Visible){
		return false;
	}
	BarVisible=Visible;
	ControlSetVisible(ScrollBar,ContentVisible&&BarVisible);
	return true;
}
//---------------------------------------------------------------------------
void kControlScrollBar::SetContentZPosition(Float32 ZPosition)
{
	ScrollBarHorizontal.GetView()->SetZPosition(ZPosition);
	ScrollBarVertical.GetView()->SetZPosition(ZPosition);
	ScrollBarGrip.GetView()->SetZPosition(ZPosition);
}
//---------------------------------------------------------------------------
void kControlScrollBar::SetContentVisible(bool Visible)
{
	if(fContentVisible==Visible)
		return;
	fContentVisible=Visible;
	if(fContentVisible){
		ScrollBarHorizontal.GetView()->SetVisible(fHorzBarVisible);
		ScrollBarVertical.GetView()->SetVisible(fVertBarVisible);
		ScrollBarGrip.GetView()->SetVisible(fHorzBarVisible&&fVertBarVisible);
	}
	else{
		ScrollBarHorizontal.GetView()->SetVisible(false);
		ScrollBarVertical.GetView()->SetVisible(false);
		ScrollBarGrip.GetView()->SetVisible(false);
	}
}
//---------------------------------------------------------------------------
void kControlScrollBar::ScrollKitSetInfo(iControlScrollInfo *HorzInfo,iControlScrollInfo *VertInfo)
{
	ScrollBarHorizontal.SetScrollInfo(HorzInfo);
	ScrollBarVertical.SetScrollInfo(VertInfo);
}
//---------------------------------------------------------------------------
bool kControlScrollBar::ScrollKitSetBarVisible(bool HorizontalVisible,bool VerticalVisible,Float32 HorizontalViewportSize,Float32 VerticalViewportSize)
{
	bool HPrevVisible=fHorzBarVisible;
	bool VPrevVisible=fVertBarVisible;

	if(HorizontalViewportSize<VerticalBarSize*2){
		VerticalVisible=false;
	}
	if(VerticalViewportSize<HorizontalBarSize*2){
		HorizontalVisible=false;
	}

	bool VisibleChanged=false;
	if(HPrevVisible!=HorizontalVisible){
		VisibleChanged=true;
		if(fContentVisible){
			ControlSetVisible(ScrollBarHorizontal,HorizontalVisible);
		}
	}
	if(VPrevVisible!=VerticalVisible){
		VisibleChanged=true;
		if(fContentVisible){
			ControlSetVisible(ScrollBarVertical,VerticalVisible);
		}
	}

	if(VisibleChanged){
		if(fContentVisible){
			if(HorizontalVisible && VerticalVisible){
				ControlSetVisible(ScrollBarGrip,true);
			}
			else{
				ControlSetVisible(ScrollBarGrip,false);
			}
		}
	}

	return VisibleChanged;
}
//---------------------------------------------------------------------------
void kControlScrollBar::Update(void)
{
	if(fView!=nullptr){
		fView->SetArrangement();
	}
	ScrollBarGripNotifySet();
}
//---------------------------------------------------------------------------
eAlignment kControlScrollBar::ScrollBarGripGetAlignment(void)
{
	if(VerticalBarAlignLeft){
		if(HorizontalBarAlignTop){
			return Alignment::TopLeft;
		}
		else{
			return Alignment::BottomLeft;
		}
	}
	else{
		if(HorizontalBarAlignTop){
			return Alignment::TopRight;
		}
		else{
			return Alignment::BottomRight;
		}
	}
}
//---------------------------------------------------------------------------
void kControlScrollBar::cScrollBarHorizontal::ScrollInfoChanged(void)
{
	cControlScrollBar::ScrollInfoChanged();

	auto Host=cnMemory::GetObjectFromMemberPointer(this,&kControlScrollBar::ScrollBarHorizontal);
	Host->HorizontalScrollInfoChanged();
}
//---------------------------------------------------------------------------
void kControlScrollBar::cScrollBarVertical::ScrollInfoChanged(void)
{
	cControlScrollBar::ScrollInfoChanged();

	auto Host=cnMemory::GetObjectFromMemberPointer(this,&kControlScrollBar::ScrollBarVertical);
	Host->VerticalScrollInfoChanged();
}
//---------------------------------------------------------------------------
void kControlScrollBar::HorizontalScrollInfoChanged(void)
{
	if(CheckScrollBarVisible(ScrollBarHorizontal,fHorzBarVisible,fContentVisible,HorizontalBarSize)){
		ControlSetVisible(ScrollBarGrip,fContentVisible&&fVertBarVisible&&fHorzBarVisible);
		if(fView!=nullptr)
			fView->SetArrangement();
	}
}
//---------------------------------------------------------------------------
void kControlScrollBar::VerticalScrollInfoChanged(void)
{
	if(CheckScrollBarVisible(ScrollBarVertical,fVertBarVisible,fContentVisible,VerticalBarSize)){
		ControlSetVisible(ScrollBarGrip,fContentVisible&&fVertBarVisible&&fHorzBarVisible);
		if(fView!=nullptr)
			fView->SetArrangement();
	}
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cScrollContent::cScrollContent()
	: fScrollOffset{0,0}
	, fScrollViewportSize{0,0}
	, fScrollLowwerLimit{0,0}
	, fScrollUpperLimit{0,0}
	, fScrollNoLowwerLimit{false,false}
	, fScrollNoUpperLimit{false,false}
	, fScrollStepSize{0,0}
	, fScrollPageSize{0,0}
	, fNeedUpdateContentLayout(false)
	, fUpdateStepSizesOnViewportSize(true)
{
}
//---------------------------------------------------------------------------
cScrollContent::~cScrollContent()
{
	SetView(nullptr);
}
//---------------------------------------------------------------------------
bool cScrollContent::ChangeOffset(bool Vertical,Float32 Offset)
{
	ScrollContentProcessOffset(Vertical,Offset);
	if(Offset==fScrollOffset[Vertical]){
		return false;
	}
	fScrollOffset[Vertical]=Offset;
	return true;
}
//---------------------------------------------------------------------------
Float32 cScrollContent::GetScrollOffset(bool Vertical)const
{
	return fScrollOffset[Vertical];
}
//---------------------------------------------------------------------------
bool cScrollContent::SetScrollOffset(bool Vertical,Float32 Offset)
{
	if(ChangeOffset(Vertical,Offset)){
		UpdateScrollOffset();
		return true;
	}
	return false;
}
//---------------------------------------------------------------------------
Float32 cScrollContent::GetViewportSize(bool Vertical)const
{
	return fScrollViewportSize[Vertical];
}
//---------------------------------------------------------------------------
cUIRectangle cScrollContent::GetViewportRectangle(void)const
{
	cUIRectangle Rect;
	Rect.Left=fScrollOffset[0];
	Rect.Right=fScrollOffset[0]+fScrollViewportSize[0];
	Rect.Top=fScrollOffset[1];
	Rect.Bottom=fScrollOffset[1]+fScrollViewportSize[1];
	return Rect;
}
//---------------------------------------------------------------------------
cUIPoint cScrollContent::GetScrollOffset(void)const
{
	cUIPoint Offset;
	Offset.x=fScrollOffset[false];
	Offset.y=fScrollOffset[true];
	return Offset;
}
//---------------------------------------------------------------------------
bool cScrollContent::SetScrollOffset(cUIPoint Offset)
{
	bool Changed=false;
	if(ChangeOffset(false,Offset.x)){
		Changed=true;
	}
	if(ChangeOffset(true,Offset.y)){
		Changed=true;
	}
	if(Changed){
		UpdateScrollOffset();
	}
	return Changed;
}
//---------------------------------------------------------------------------
cUIPoint cScrollContent::GetViewportSize(void)const
{
	cUIPoint VisibleSize;
	VisibleSize.x=fScrollViewportSize[0];
	VisibleSize.y=fScrollViewportSize[1];
	return VisibleSize;
}
//---------------------------------------------------------------------------
bool cScrollContent::IsRectangleInViewport(const cUIRectangle &Rect)const
{
	if(Rect.Right<=fScrollOffset[0]){
		return false;
	}
	if(Rect.Bottom<=fScrollOffset[1]){
		return false;
	}
	if(Rect.Left>fScrollOffset[0]+fScrollViewportSize[0]){
		return false;
	}
	if(Rect.Top>fScrollOffset[1]+fScrollViewportSize[1]){
		return false;
	}
	return true;
}
//---------------------------------------------------------------------------
iControlScrollInfo::cScrollRange cScrollContent::GetScrollRange(bool Vertical)const
{
	iControlScrollInfo::cScrollRange ScrollRange;
	ScrollRange.LowwerLimit=fScrollLowwerLimit[Vertical];
	ScrollRange.UpperLimit=fScrollUpperLimit[Vertical];
	ScrollRange.NoLowwerLimit=fScrollNoLowwerLimit[Vertical];
	ScrollRange.NoUpperLimit=fScrollNoUpperLimit[Vertical];
	ScrollRange.ViewportSize=fScrollViewportSize[Vertical];
	return ScrollRange;
}
//---------------------------------------------------------------------------
Float32 cScrollContent::GetScrollStepSize(bool Vertical)const
{
	return fScrollStepSize[Vertical];
}
//---------------------------------------------------------------------------
Float32 cScrollContent::GetScrollPageSize(bool Vertical)const
{
	return fScrollPageSize[Vertical];
}
//---------------------------------------------------------------------------
cUIRectangle cScrollContent::GetVisibleRectangle(void)const
{
	cUIRectangle Rect;
	Rect.Left=fScrollOffset[0];
	Rect.Top=fScrollOffset[1];
	Rect.Right=fScrollOffset[0]+fScrollViewportSize[0];
	Rect.Bottom=fScrollOffset[1]+fScrollViewportSize[1];

	if(fView!=nullptr){
		auto FrameMargin=fView->GetFrameMargin();
		Rect.Left+=FrameMargin.Left;
		Rect.Top+=FrameMargin.Top;
		Rect.Right-=FrameMargin.Right;
		Rect.Bottom-=FrameMargin.Bottom;
	}
	return Rect;
}
//---------------------------------------------------------------------------
bool cScrollContent::ScrollRangeToVisible(bool Vertical,Float32 Lowwer,Float32 Upper)
{
	Float32 VisibleLowwer=fScrollOffset[Vertical];
	Float32 VisibleUpper=fScrollOffset[Vertical]+fScrollViewportSize[Vertical];
	if(fView!=nullptr){
		auto FrameMargin=fView->GetFrameMargin();
		VisibleLowwer+=Vertical?FrameMargin.Top:FrameMargin.Left;
		VisibleUpper-=Vertical?FrameMargin.Bottom:FrameMargin.Right;
	}
	if(Upper>=VisibleLowwer && Lowwer<=VisibleUpper){
		return true;
	}
	Float32 DestCenter=(Lowwer+Upper)/2;
	Float32 NewOffset=DestCenter-(VisibleUpper-VisibleLowwer)/2;
	return SetScrollOffset(NewOffset,Vertical);
}
//---------------------------------------------------------------------------
bool cScrollContent::ScrollRangeToVisible(Float32 Left,Float32 Top,Float32 Right,Float32 Bottom)
{
	bool Changed=false;

	auto VisibleRect=GetVisibleRectangle();

	if(Right<VisibleRect.Left || Left>VisibleRect.Right){
		Float32 DestHCenter=(Left+Right)/2;
		Float32 NewOffset=DestHCenter-(VisibleRect.Right-VisibleRect.Left)/2;
		if(ChangeOffset(0,NewOffset)){
			Changed=true;
		}
	}
	if(Bottom<VisibleRect.Top || Top>VisibleRect.Bottom){
		Float32 DestVCenter=(Top+Bottom)/2;
		Float32 NewOffset=DestVCenter-(VisibleRect.Bottom-VisibleRect.Top)/2;
		if(ChangeOffset(1,NewOffset)){
			Changed=true;
		}
	}
	if(Changed){
		UpdateScrollOffset();
	}
	return Changed;
}
//---------------------------------------------------------------------------
bool cScrollContent::ScrollRectToVisible(cUIPoint Pos,cUIPoint Size)
{
	return ScrollRangeToVisible(Pos.x,Pos.y,Pos.x+Size.x,Pos.y+Size.y);
}
//---------------------------------------------------------------------------
void cScrollContent::SetNeedUpdateContentLayout(void)
{
	if(fNeedUpdateContentLayout==false){
		fNeedUpdateContentLayout=true;
		if(fView!=nullptr){
			fView->SetArrangement();
		}
	}
}
//---------------------------------------------------------------------------
void cScrollContent::UpdateScrollOffset(void)
{
	SetNeedUpdateContentLayout();
	ScrollOffsetChangeNotify();
}
//---------------------------------------------------------------------------
void cScrollContent::UpdateScrollLimits(void)
{
	ScrollRangeChangeNotify();
}
//---------------------------------------------------------------------------
void cScrollContent::ScrollContentProcessOffset(bool Vertical,Float32 &Offset)
{
	Offset=cnMath::FloatRoundNearest(Offset);
	// limit offset range
	if(fScrollNoUpperLimit[Vertical]==false){
		Float32 UpperLimit=fScrollUpperLimit[Vertical];
		Float32 ViewportSize=fScrollViewportSize[Vertical];
		Float32 ScrollOffsetMax=UpperLimit-ViewportSize;
		if(Offset>ScrollOffsetMax){
			Offset=ScrollOffsetMax;
		}
	}
	if(fScrollNoLowwerLimit[Vertical]==false){
		Float32 LowwerLimit=fScrollLowwerLimit[Vertical];
		if(Offset<LowwerLimit)
			Offset=LowwerLimit;
	}
}
//---------------------------------------------------------------------------
void cScrollContent::ScrollContentUpdateStepSizes(void)
{
	fScrollStepSize[0]=cnMath::FloatRoundNearest(fScrollViewportSize[0]/16);
	fScrollStepSize[1]=cnMath::FloatRoundNearest(fScrollViewportSize[1]/16);
	fScrollPageSize[0]=cnMath::FloatRoundNearest(fScrollViewportSize[0]/2);
	fScrollPageSize[1]=cnMath::FloatRoundNearest(fScrollViewportSize[1]/2);
}
//---------------------------------------------------------------------------
void cScrollContent::ScrollContentUpdateContentLayout(void)
{
}
//---------------------------------------------------------------------------
void cScrollContent::RectangleChanged(bool Moved,bool Sized)
{
	VisualControl::RectangleChanged(Moved,Sized);
	if(Sized){
		cUIPoint ViewSize=fView->GetSize();
		fScrollViewportSize[0]=ViewSize.x;
		fScrollViewportSize[1]=ViewSize.y;
	
		if(fUpdateStepSizesOnViewportSize){
			ScrollContentUpdateStepSizes();
		}

		ScrollRangeChangeNotify();
		fNeedUpdateContentLayout=true;
	}
}
//---------------------------------------------------------------------------
void cScrollContent::ContentScaleChanged(void)
{
	VisualControl::ContentScaleChanged();
}
//---------------------------------------------------------------------------
void cScrollContent::UILayout(void)
{
	LayoutControl::UILayout();

	if(fNeedUpdateContentLayout){
		ScrollContentUpdateContentLayout();
		fNeedUpdateContentLayout=false;
	}

}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
lScrollFrame::lScrollFrame()
	: fScrollContent(nullptr)
{
}
//---------------------------------------------------------------------------
lScrollFrame::~lScrollFrame()
{
	SetScrollBarKit(nullptr);
	SetView(nullptr);
}
//---------------------------------------------------------------------------
void lScrollFrame::ViewSetup(void)
{
	LayoutControl::ViewSetup();

	if(fScrollBarKit!=nullptr){
		fScrollBarKit->SetView(fView);
	}
	else{
		ControlSetupDefaultScrollBarKit();
	}
}
//---------------------------------------------------------------------------
void lScrollFrame::ViewClear(void)
{
	if(fScrollBarKit!=nullptr){
		fScrollBarKit->SetView(nullptr);
	}

	LayoutControl::ViewClear();
}
//---------------------------------------------------------------------------
cScrollContent* lScrollFrame::GetScrollContent(void)const
{
	return fScrollContent;
}
//---------------------------------------------------------------------------
void lScrollFrame::SetScrollContent(cScrollContent *Content)
{
	if(fScrollContent!=nullptr){
		fScrollContent->ScrollOffsetChangeNotify.Remove(fScrollOffsetChangeNotifyToken);
		fScrollContent->ScrollRangeChangeNotify.Remove(fScrollRangeChangeNotifyToken);
	}
	fScrollContent=Content;
	if(fScrollContent!=nullptr){
		fScrollOffsetChangeNotifyToken=fScrollContent->ScrollOffsetChangeNotify.Insert([this]{
			ScrollOffsetChanged();
		});
		fScrollRangeChangeNotifyToken=fScrollContent->ScrollRangeChangeNotify.Insert([this]{
			ScrollRangeChanged();
		});
	
	}
}
//---------------------------------------------------------------------------
kiScrollBar* lScrollFrame::GetScrollBarKit(void)const
{
	return fScrollBarKit;
}
//---------------------------------------------------------------------------
void lScrollFrame::SetScrollBarKit(rPtr<kiScrollBar> Kit)
{
	if(fScrollBarKit!=nullptr){
		fScrollBarKit->ScrollKitSetInfo(nullptr,nullptr);
		fScrollBarKit->SetView(nullptr);
	}
	fScrollBarKit=cnVar::MoveCast(Kit);
	if(fScrollBarKit!=nullptr){
		fScrollBarKit->ScrollKitSetInfo(&ScrollInfoHorizontal,&ScrollInfoVertical);
		fScrollBarKit->SetView(fView);
		fScrollBarKit->SetLayoutOrder(ScrollBarKitLayoutOrder);
	}
}
//---------------------------------------------------------------------------
void lScrollFrame::UpdateScrollBarLayoutOrder(void)
{
	if(fScrollBarKit!=nullptr){
		fScrollBarKit->SetLayoutOrder(ScrollBarKitLayoutOrder);
	}
}
//---------------------------------------------------------------------------
void lScrollFrame::ControlSetupDefaultScrollBarKit(void)
{
	if(gCreateDefaultScrollBarKit!=nullptr){
		auto ScrollBarKit=gCreateDefaultScrollBarKit();
		SetScrollBarKit(ScrollBarKit);
	}
}
//---------------------------------------------------------------------------
cUIPoint lScrollFrame::GetScrollOffset(void)const
{
	if(fScrollContent==nullptr)
		return UIPointZero;

	return fScrollContent->GetScrollOffset();
}
//---------------------------------------------------------------------------
bool lScrollFrame::SetScrollOffset(cUIPoint Offset)
{
	if(fScrollContent==nullptr)
		return false;

	return fScrollContent->SetScrollOffset(Offset);
}
//---------------------------------------------------------------------------
Float32 lScrollFrame::GetScrollOffset(bool Vertical)const
{
	if(fScrollContent==nullptr)
		return 0;
	return fScrollContent->GetScrollOffset(Vertical);
}
//---------------------------------------------------------------------------
bool lScrollFrame::SetScrollOffset(bool Vertical,Float32 Offset)
{
	if(fScrollContent==nullptr)
		return false;
	return fScrollContent->SetScrollOffset(Vertical,Offset);
}
//---------------------------------------------------------------------------
iControlScrollInfo::cScrollRange lScrollFrame::GetScrollRange(bool Vertical)const
{
	if(fScrollContent==nullptr){
		iControlScrollInfo::cScrollRange ScrollRange;
		ScrollRange.LowwerLimit=0;
		ScrollRange.UpperLimit=0;
		ScrollRange.NoLowwerLimit=false;
		ScrollRange.NoUpperLimit=false;
		ScrollRange.ViewportSize=0;
		return ScrollRange;
	}
	return fScrollContent->GetScrollRange(Vertical);
}
//---------------------------------------------------------------------------
Float32 lScrollFrame::GetScrollStepSize(bool Vertical)const
{
	if(fScrollContent==nullptr)
		return 0;
	return fScrollContent->GetScrollStepSize(Vertical);
}
//---------------------------------------------------------------------------
Float32 lScrollFrame::GetScrollPageSize(bool Vertical)const
{
	if(fScrollContent==nullptr)
		return 0;
	return fScrollContent->GetScrollPageSize(Vertical);
}
//---------------------------------------------------------------------------
bool lScrollFrame::ScrollContentRangeToVisible(bool Vertical,Float32 Lowwer,Float32 Upper)
{
	if(fScrollContent==nullptr)
		return false;

	return fScrollContent->ScrollRangeToVisible(Vertical,Lowwer,Upper);
}
//---------------------------------------------------------------------------
bool lScrollFrame::ScrollContentRangeToVisible(Float32 Left,Float32 Top,Float32 Right,Float32 Bottom)
{
	if(fScrollContent==nullptr)
		return false;

	return fScrollContent->ScrollRangeToVisible(Left,Top,Right,Bottom);
}
//---------------------------------------------------------------------------
bool lScrollFrame::ScrollContentRectToVisible(cUIPoint Pos,cUIPoint Size)
{
	if(fScrollContent==nullptr)
		return false;

	return fScrollContent->ScrollRectToVisible(Pos,Size);
}
//---------------------------------------------------------------------------
void lScrollFrame::ScrollOffsetChanged(void)
{
	ScrollInfoHorizontal.ScrollOffsetNotifySet();
	ScrollInfoVertical.ScrollOffsetNotifySet();
}
//---------------------------------------------------------------------------
void lScrollFrame::ScrollRangeChanged(void)
{
	ScrollInfoHorizontal.ScrollInfoNotifySet();
	ScrollInfoVertical.ScrollInfoNotifySet();
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
lScrollFrame* lScrollFrame::cScrollInfoHorizontal::GetHost(void)
{
	return cnMemory::GetObjectFromMemberPointer(this,&lScrollFrame::ScrollInfoHorizontal);
}
//---------------------------------------------------------------------------
const lScrollFrame* lScrollFrame::cScrollInfoHorizontal::GetHost(void)const
{
	return cnMemory::GetObjectFromMemberPointer(this,&lScrollFrame::ScrollInfoHorizontal);
}
//---------------------------------------------------------------------------
Float32 lScrollFrame::cScrollInfoHorizontal::ScrollGetOffset(void)const
{
	auto Host=GetHost();
	return Host->GetScrollOffset(false);
}
//---------------------------------------------------------------------------
bool lScrollFrame::cScrollInfoHorizontal::ScrollSetOffset(Float32 Offset)
{
	auto Host=GetHost();
	return Host->SetScrollOffset(false,Offset);
}
//---------------------------------------------------------------------------
iControlScrollInfo::cScrollRange lScrollFrame::cScrollInfoHorizontal::ScrollGetScrollRange(void)const
{
	auto Host=GetHost();
	return Host->GetScrollRange(false);
}
//---------------------------------------------------------------------------
Float32 lScrollFrame::cScrollInfoHorizontal::ScrollGetStepSize(void)const
{
	auto Host=GetHost();
	return Host->GetScrollStepSize(false);
}
//---------------------------------------------------------------------------
Float32 lScrollFrame::cScrollInfoHorizontal::ScrollGetPageSize(void)const
{
	auto Host=GetHost();
	return Host->GetScrollPageSize(false);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
lScrollFrame* lScrollFrame::cScrollInfoVertical::GetHost(void)
{
	return cnMemory::GetObjectFromMemberPointer(this,&lScrollFrame::ScrollInfoVertical);
}
//---------------------------------------------------------------------------
const lScrollFrame* lScrollFrame::cScrollInfoVertical::GetHost(void)const
{
	return cnMemory::GetObjectFromMemberPointer(this,&lScrollFrame::ScrollInfoVertical);
}
//---------------------------------------------------------------------------
Float32 lScrollFrame::cScrollInfoVertical::ScrollGetOffset(void)const
{
	auto Host=GetHost();
	return Host->GetScrollOffset(true);
}
//---------------------------------------------------------------------------
bool lScrollFrame::cScrollInfoVertical::ScrollSetOffset(Float32 Offset)
{
	auto Host=GetHost();
	return Host->SetScrollOffset(true,Offset);
}
//---------------------------------------------------------------------------
iControlScrollInfo::cScrollRange lScrollFrame::cScrollInfoVertical::ScrollGetScrollRange(void)const
{
	auto Host=GetHost();
	return Host->GetScrollRange(true);
}
//---------------------------------------------------------------------------
Float32 lScrollFrame::cScrollInfoVertical::ScrollGetStepSize(void)const
{
	auto Host=GetHost();
	return Host->GetScrollStepSize(true);
}
//---------------------------------------------------------------------------
Float32 lScrollFrame::cScrollInfoVertical::ScrollGetPageSize(void)const
{
	auto Host=GetHost();
	return Host->GetScrollPageSize(true);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cScrollViewContainer::cScrollViewContainer()
{
}
//---------------------------------------------------------------------------
cScrollViewContainer::~cScrollViewContainer()
{
	SetView(nullptr);

	auto RemainingViewSet=cnVar::MoveCast(fContentViewSet);
	for(auto *Item : RemainingViewSet){
		Item->fOwner=nullptr;
	}
}
//---------------------------------------------------------------------------
void cScrollViewContainer::ViewSetup(void)
{
	cScrollContent::ViewSetup();

	for(auto *Item : fContentViewSet){
		if(Item->View!=nullptr)
			fView->InsertView(Item->View);
	}
}
//---------------------------------------------------------------------------
void cScrollViewContainer::ViewClear(void)
{
	for(auto *Item : fContentViewSet){
		if(Item->View!=nullptr)
			fView->RemoveView(Item->View);
	}
	fVisibleSet.Clear();

	cScrollContent::ViewClear();
}
//---------------------------------------------------------------------------
void cScrollViewContainer::ScrollContentUpdateContentLayout(void)
{
	cScrollContent::ScrollContentUpdateContentLayout();
	cnLib_ASSERT(fView!=nullptr);

	auto PrevVisibleSet=cnVar::MoveCast(fVisibleSet);
	for(auto Item : PrevVisibleSet){
		bool Visible=IsRectangleInViewport(Item->ContentRect);
		if(Visible){
			fVisibleSet.Insert(Item);
		}
		else{
			if(Item->View!=nullptr){
				Item->View->SetVisible(false);
			}
			Item->Visible=false;
		}
	}

	for(auto Item : fContentViewSet){
		ViewItemCheckVisible(Item);
	}
}
//---------------------------------------------------------------------------
void cScrollViewContainer::UILayout(void)
{
	cScrollContent::UILayout();

	for(auto Item : fVisibleSet){
		if(Item->View==nullptr)
			continue;

		auto ItemRect=ViewItemCalculateLayoutRect(Item);

		Item->View->ArrangeRectangle(fView,ItemRect.Pos,ItemRect.Size);
	}
}
//---------------------------------------------------------------------------
void cScrollViewContainer::ViewItemAttach(bcViewItem *Item)
{
	if(fView!=nullptr){
		if(Item->View!=nullptr){
			fView->InsertView(Item->View);
		}
		ViewItemCheckVisible(Item);
	}
}
//---------------------------------------------------------------------------
void cScrollViewContainer::ViewItemDetach(bcViewItem *Item)
{
	if(Item->View!=nullptr){
		if(fView!=nullptr){
			fView->RemoveView(Item->View);
		}
		RecycleView(cnVar::MoveCast(Item->ViewReference));
		Item->View=nullptr;
	}
	fVisibleSet.Remove(Item);
}
//---------------------------------------------------------------------------
const cUIRectangle& cScrollViewContainer::ViewItemGetRectangle(const bcViewItem *Item)
{
	return Item->ContentRect;
}
//---------------------------------------------------------------------------
void cScrollViewContainer::ViewItemSetRectangle(bcViewItem *Item,cUIRectangle Rect)
{
	Item->ContentRect=Rect;

	bool Visible=IsRectangleInViewport(Item->ContentRect);
	if(Item->Visible==Visible)
		return;
	Item->Visible=Visible;
	if(Item->View!=nullptr){
		Item->View->SetVisible(Visible);
	}
	if(Visible){
		fVisibleSet.Insert(Item);
	}
	else{
		fVisibleSet.Remove(Item);
	}
}
//---------------------------------------------------------------------------
iViewReference* cScrollViewContainer::ViewItemGetViewReference(const bcViewItem *Item)const
{
	return Item->ViewReference;
}
//---------------------------------------------------------------------------
bool cScrollViewContainer::ViewItemSetViewReference(bcViewItem *Item,iViewReference *ViewReference)
{
	if(Item->View!=nullptr){
		if(fView!=nullptr){
			fView->RemoveView(Item->View);
		}
		RecycleView(cnVar::MoveCast(Item->ViewReference));
	}

	if(ViewReference!=nullptr){
		Item->View=ViewReference->GetView();
		if(Item->View==nullptr){
			Item->ViewReference=nullptr;
			Item->View=nullptr;
			return false;
		}
		if(fView->InsertView(Item->View)==false){
			Item->View=nullptr;
			return false;
		}
		Item->ViewReference=ViewReference;

		Item->View->SetVisible(Item->Visible);
	}
	else{
		Item->ViewReference=nullptr;
		Item->View=nullptr;
	}
	return true;
}
//---------------------------------------------------------------------------
cUIRect cScrollViewContainer::ViewItemCalculateLayoutRect(const bcViewItem *Item)const
{
	cUIRect ItemRect;

	ItemRect.Pos.x=Item->ContentRect.Left;
	ItemRect.Pos.y=Item->ContentRect.Top;
	ItemRect.Size.x=Item->ContentRect.Right-Item->ContentRect.Left;
	ItemRect.Size.y=Item->ContentRect.Bottom-Item->ContentRect.Top;

	ItemRect.Pos.x-=fScrollOffset[0];
	ItemRect.Pos.y-=fScrollOffset[1];

	return ItemRect;
}
//---------------------------------------------------------------------------
void cScrollViewContainer::ViewItemCheckVisible(bcViewItem *Item)
{
	if(Item->Visible)
		return;
	bool Visible=IsRectangleInViewport(Item->ContentRect);
	if(Visible==false)
		return;
	Item->Visible=true;
	if(Item->View!=nullptr){
		Item->View->SetVisible(true);
	}
	fVisibleSet.Insert(Item);
}
//---------------------------------------------------------------------------
void cScrollViewContainer::RecycleView(rPtr<iViewReference> ViewReference)
{
	auto ImpType=ViewReference->GetImplementationType();
	fViewRecycleMap[ImpType].AppendMake(cnVar::MoveCast(ViewReference));
}
//---------------------------------------------------------------------------
rPtr<iViewReference> cScrollViewContainer::ReclaimView(tTypeID ImplementationType)
{
	auto TypePair=fViewRecycleMap.GetPair(ImplementationType);
	if(TypePair==nullptr)
		return nullptr;

	auto &RecViews=TypePair->Value;
	uIntn ViewCount=RecViews.GetCount();
	if(ViewCount==0){
		fViewRecycleMap.RemovePair(TypePair);
		return nullptr;
	}

	auto ViewReference=cnVar::MoveCast(RecViews[ViewCount-1]);
	if(ViewCount==1){
		fViewRecycleMap.RemovePair(TypePair);
	}
	else{
		TypePair->Value.RemoveTail(1);
	}
	return ViewReference;
}
//---------------------------------------------------------------------------
void cScrollViewContainer::ContentViewAttach(cViewItem *Item)
{
	fContentViewSet.Insert(Item);
	ViewItemAttach(Item);
}
//---------------------------------------------------------------------------
void cScrollViewContainer::ContentViewDetach(cViewItem *Item)
{
	ViewItemDetach(Item);
	fContentViewSet.Remove(Item);
}
//---------------------------------------------------------------------------
bool cScrollViewContainer::bcViewItem::IsVisible(void)const
{
	return Visible;
}
//---------------------------------------------------------------------------
iUIView *cScrollViewContainer::bcViewItem::GetView(void)const
{
	return View;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cScrollViewContainer::cViewItem::cViewItem()
	: fOwner(nullptr)
{
}
//---------------------------------------------------------------------------
cScrollViewContainer::cViewItem::~cViewItem()
{
	if(fOwner!=nullptr){
		fOwner->ContentViewDetach(this);
	}
}
//---------------------------------------------------------------------------
bool cScrollViewContainer::cViewItem::Attach(cScrollViewContainer *Owner)
{
	if(fOwner!=nullptr)
		return false;
	if(Owner==nullptr)
		return false;

	fOwner=Owner;
	fOwner->ContentViewAttach(this);
	return true;
}
//---------------------------------------------------------------------------
bool cScrollViewContainer::cViewItem::Detach(cScrollViewContainer *Owner)
{
	if(fOwner!=Owner)
		return false;

	fOwner->ContentViewDetach(this);
	fOwner=nullptr;
	return true;
}
//---------------------------------------------------------------------------
const cUIRectangle& cScrollViewContainer::cViewItem::GetRectangle(void)const
{
	if(fOwner==nullptr)
		return UIRectangleZero;
	return fOwner->ViewItemGetRectangle(this);
}
//---------------------------------------------------------------------------
void cScrollViewContainer::cViewItem::SetRectangle(cUIRectangle Rect)
{
	return fOwner->ViewItemSetRectangle(this,Rect);
}
//---------------------------------------------------------------------------
iViewReference* cScrollViewContainer::cViewItem::GetViewReference(void)const
{
	if(fOwner==nullptr)
		return nullptr;
	return fOwner->ViewItemGetViewReference(this);
}
//---------------------------------------------------------------------------
bool cScrollViewContainer::cViewItem::SetViewReference(iViewReference *Reference)
{
	if(fOwner==nullptr)
		return false;

	return fOwner->ViewItemSetViewReference(this,Reference);
}
//---------------------------------------------------------------------------
cUIRect cScrollViewContainer::cViewItem::CalculateLayoutRect(void)const
{
	if(fOwner==nullptr)
		return UIRectZero;
	return fOwner->ViewItemCalculateLayoutRect(this);
}
//---------------------------------------------------------------------------
void cScrollViewContainer::cViewItem::VisibilityChanged(void)
{
	OnVisibleChanged();
}
//---------------------------------------------------------------------------
