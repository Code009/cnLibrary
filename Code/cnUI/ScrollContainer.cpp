#include "ScrollContainer.h"

#include "Shape.h"

using namespace cnLibrary;
using namespace cnRTL;
using namespace cnUI;


rPtr<kiScrollBar> (*cnUI::gCreateDefaultScrollBarKit)(void)noexcept(true)=nullptr;

namespace cnLibrary{namespace cnUI{
static rPtr<viControl> CreateDefaultScrollBarGripVisual_SolidStaticColor(viScrollBarGripData*)noexcept(true){
	return vSolidStaticColor::Create(UIColorFromUInt32(0xFFCCCCCC));
}
}}

rPtr<viControl> (*cnUI::gCreateDefaultScrollBarGripVisual)(viScrollBarGripData *Data)noexcept(true)=CreateDefaultScrollBarGripVisual_SolidStaticColor;

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
kControlScrollBar::kControlScrollBar()noexcept(true)
	: fBarMargin{0,0,0,0}
{
	fContentVisible=true;
	if(gCreateDefaultScrollBarGripVisual!=nullptr){
		ScrollBarGrip.SetBackground(gCreateDefaultScrollBarGripVisual(this));
	}
}
//---------------------------------------------------------------------------
kControlScrollBar::~kControlScrollBar()noexcept(true)
{
	InvalidateData();
}
//---------------------------------------------------------------------------
rPtr<kiScrollBar> kControlScrollBar::Create(void)noexcept(true)
{
	return rCreate< bwkScrollBar<kControlScrollBar> >();
}
//---------------------------------------------------------------------------
void kControlScrollBar::ViewSetup(void)noexcept(true)
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
void kControlScrollBar::ViewClear(void)noexcept(true)
{
	ControlDestroyView(ScrollBarHorizontal,fView);
	ControlDestroyView(ScrollBarVertical,fView);
	ControlDestroyView(ScrollBarGrip,fView);

	LayoutControl::ViewClear();
}
//---------------------------------------------------------------------------
void kControlScrollBar::UILayout(void)noexcept(true)
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
cUIRectangle kControlScrollBar::UIMargin(const cUIRectangle &Margin)noexcept(true)
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
bool kControlScrollBar::CheckScrollBarVisible(cControlScrollBar &ScrollBar,bool &BarVisible,bool ContentVisible,Float32 BarSize)noexcept(true)
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
void kControlScrollBar::SetContentZPosition(Float32 ZPosition)noexcept(true)
{
	ScrollBarHorizontal.GetView()->SetZPosition(ZPosition);
	ScrollBarVertical.GetView()->SetZPosition(ZPosition);
	ScrollBarGrip.GetView()->SetZPosition(ZPosition);
}
//---------------------------------------------------------------------------
void kControlScrollBar::SetContentVisible(bool Visible)noexcept(true)
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
void kControlScrollBar::ScrollKitSetInfo(iControlScrollInfo *HorzInfo,iControlScrollInfo *VertInfo)noexcept(true)
{
	ScrollBarHorizontal.SetScrollInfo(HorzInfo);
	ScrollBarVertical.SetScrollInfo(VertInfo);
}
//---------------------------------------------------------------------------
bool kControlScrollBar::ScrollKitSetBarVisible(bool HorizontalVisible,bool VerticalVisible,Float32 HorizontalViewportSize,Float32 VerticalViewportSize)noexcept(true)
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
void kControlScrollBar::Update(void)noexcept(true)
{
	if(fView!=nullptr){
		fView->SetArrangement();
	}
	ScrollBarGripNotifySet();
}
//---------------------------------------------------------------------------
eAlignment kControlScrollBar::ScrollBarGripGetAlignment(void)noexcept(true)
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
void kControlScrollBar::cScrollBarHorizontal::ScrollInfoChanged(void)noexcept(true)
{
	cControlScrollBar::ScrollInfoChanged();

	auto Host=cnMemory::GetObjectFromMemberPointer(this,&kControlScrollBar::ScrollBarHorizontal);
	Host->HorizontalScrollInfoChanged();
}
//---------------------------------------------------------------------------
void kControlScrollBar::cScrollBarVertical::ScrollInfoChanged(void)noexcept(true)
{
	cControlScrollBar::ScrollInfoChanged();

	auto Host=cnMemory::GetObjectFromMemberPointer(this,&kControlScrollBar::ScrollBarVertical);
	Host->VerticalScrollInfoChanged();
}
//---------------------------------------------------------------------------
void kControlScrollBar::HorizontalScrollInfoChanged(void)noexcept(true)
{
	if(CheckScrollBarVisible(ScrollBarHorizontal,fHorzBarVisible,fContentVisible,HorizontalBarSize)){
		ControlSetVisible(ScrollBarGrip,fContentVisible&&fVertBarVisible&&fHorzBarVisible);
		if(fView!=nullptr)
			fView->SetArrangement();
	}
}
//---------------------------------------------------------------------------
void kControlScrollBar::VerticalScrollInfoChanged(void)noexcept(true)
{
	if(CheckScrollBarVisible(ScrollBarVertical,fVertBarVisible,fContentVisible,VerticalBarSize)){
		ControlSetVisible(ScrollBarGrip,fContentVisible&&fVertBarVisible&&fHorzBarVisible);
		if(fView!=nullptr)
			fView->SetArrangement();
	}
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cScrollControl::cScrollControl()noexcept(true)
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
	auto Dispatch=cnSystem::CurrentUIThread::GetDispatch(true);
	fUpdateProcedure=rCreate<cNotifyUpdateProcedure>();
	fUpdateProcedure->Owner=this;
	fUpdateProcedure->WorkProc=Dispatch->CreateWork(fUpdateProcedure,fUpdateProcedure);
}
//---------------------------------------------------------------------------
cScrollControl::~cScrollControl()noexcept(true)
{
	fUpdateProcedure->Owner=nullptr;
	SetView(nullptr);
}
//---------------------------------------------------------------------------
bool cScrollControl::ChangeOffset(bool Vertical,Float32 Offset)noexcept(true)
{
	if(CheckScrollOffset(Vertical,Offset)==false){
		return false;
	}
	fScrollOffset[Vertical]=Offset;
	return true;
}
//---------------------------------------------------------------------------
Float32 cScrollControl::GetScrollOffset(bool Vertical)const noexcept(true)
{
	return fScrollOffset[Vertical];
}
//---------------------------------------------------------------------------
bool cScrollControl::SetScrollOffset(bool Vertical,Float32 Offset)noexcept(true)
{
	if(ChangeOffset(Vertical,Offset)){
		UpdateScrollOffset();
		return true;
	}
	return false;
}
//---------------------------------------------------------------------------
Float32 cScrollControl::GetViewportSize(bool Vertical)const noexcept(true)
{
	return fScrollViewportSize[Vertical];
}
//---------------------------------------------------------------------------
cUIRectangle cScrollControl::GetViewportRectangle(void)const noexcept(true)
{
	cUIRectangle Rect;
	Rect.Left=fScrollOffset[0];
	Rect.Right=fScrollOffset[0]+fScrollViewportSize[0];
	Rect.Top=fScrollOffset[1];
	Rect.Bottom=fScrollOffset[1]+fScrollViewportSize[1];
	return Rect;
}
//---------------------------------------------------------------------------
cUIPoint cScrollControl::GetScrollOffset(void)const noexcept(true)
{
	cUIPoint Offset;
	Offset.x=fScrollOffset[false];
	Offset.y=fScrollOffset[true];
	return Offset;
}
//---------------------------------------------------------------------------
bool cScrollControl::SetScrollOffset(cUIPoint Offset)noexcept(true)
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
cUIPoint cScrollControl::GetViewportSize(void)const noexcept(true)
{
	cUIPoint VisibleSize;
	VisibleSize.x=fScrollViewportSize[0];
	VisibleSize.y=fScrollViewportSize[1];
	return VisibleSize;
}
//---------------------------------------------------------------------------
bool cScrollControl::IsRectangleInViewport(const cUIRectangle &Rect)const noexcept(true)
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
iControlScrollInfo::cScrollRange cScrollControl::GetScrollRange(bool Vertical)const noexcept(true)
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
Float32 cScrollControl::GetScrollStepSize(bool Vertical)const noexcept(true)
{
	return fScrollStepSize[Vertical];
}
//---------------------------------------------------------------------------
Float32 cScrollControl::GetScrollPageSize(bool Vertical)const noexcept(true)
{
	return fScrollPageSize[Vertical];
}
//---------------------------------------------------------------------------
cUIRectangle cScrollControl::GetVisibleRectangle(void)const noexcept(true)
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
bool cScrollControl::ScrollRangeToVisible(bool Vertical,Float32 Lowwer,Float32 Upper)noexcept(true)
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
bool cScrollControl::ScrollRangeToVisible(Float32 Left,Float32 Top,Float32 Right,Float32 Bottom)noexcept(true)
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
bool cScrollControl::ScrollRectToVisible(cUIPoint Pos,cUIPoint Size)noexcept(true)
{
	return ScrollRangeToVisible(Pos.x,Pos.y,Pos.x+Size.x,Pos.y+Size.y);
}
//---------------------------------------------------------------------------
void cScrollControl::SetNeedUpdateContentLayout(void)noexcept(true)
{
	if(fNeedUpdateContentLayout==false){
		fNeedUpdateContentLayout=true;
		fUpdateProcedure->WorkProc->Start();
	}
}
//---------------------------------------------------------------------------
void cScrollControl::UpdateScrollOffset(void)noexcept(true)
{
	SetNeedUpdateContentLayout();
	ScrollOffsetChangeNotify();
}
//---------------------------------------------------------------------------
bool cScrollControl::CheckScrollOffset(bool Vertical,Float32 &Offset)noexcept(true)
{
	bool Changed=false;
	Offset=cnMath::FloatRoundNearest(Offset);
	// limit offset range
	if(fScrollNoUpperLimit[Vertical]==false){
		Float32 UpperLimit=fScrollUpperLimit[Vertical];
		Float32 ViewportSize=fScrollViewportSize[Vertical];
		Float32 ScrollOffsetMax=UpperLimit-ViewportSize;
		if(Offset>ScrollOffsetMax){
			Offset=ScrollOffsetMax;
			Changed=true;
		}
	}
	if(fScrollNoLowwerLimit[Vertical]==false){
		Float32 LowwerLimit=fScrollLowwerLimit[Vertical];
		if(Offset<LowwerLimit)
			Offset=LowwerLimit;
		Changed=true;
	}
	return Changed;
}
//---------------------------------------------------------------------------
void cScrollControl::ScrollContentUpdateStepSizes(void)noexcept(true)
{
	fScrollStepSize[0]=cnMath::FloatRoundNearest(fScrollViewportSize[0]/16);
	fScrollStepSize[1]=cnMath::FloatRoundNearest(fScrollViewportSize[1]/16);
	fScrollPageSize[0]=cnMath::FloatRoundNearest(fScrollViewportSize[0]/2);
	fScrollPageSize[1]=cnMath::FloatRoundNearest(fScrollViewportSize[1]/2);
}
//---------------------------------------------------------------------------
void cScrollControl::ScrollContentUpdateContentLayout(void)noexcept(true)
{
}
//---------------------------------------------------------------------------
void cScrollControl::RectangleChanged(bool Moved,bool Sized)noexcept(true)
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
		SetNeedUpdateContentLayout();
	}
}
//---------------------------------------------------------------------------
void cScrollControl::ContentScaleChanged(void)noexcept(true)
{
	VisualControl::ContentScaleChanged();
}
//---------------------------------------------------------------------------
void cScrollControl::NotifyUpdateContent(void)noexcept(true)
{
	if(fNeedUpdateContentLayout){
		ScrollContentUpdateContentLayout();
		fNeedUpdateContentLayout=false;
	}
}
//---------------------------------------------------------------------------
void cScrollControl::cNotifyUpdateProcedure::Execute(void)noexcept(true)
{
	if(Owner!=nullptr){
		Owner->NotifyUpdateContent();
	}
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
lScrollFrame::lScrollFrame()noexcept(true)
	: fScrollControl(nullptr)
{
}
//---------------------------------------------------------------------------
lScrollFrame::~lScrollFrame()noexcept(true)
{
	SetScrollBarKit(nullptr);
	SetView(nullptr);
}
//---------------------------------------------------------------------------
void lScrollFrame::ViewSetup(void)noexcept(true)
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
void lScrollFrame::ViewClear(void)noexcept(true)
{
	if(fScrollBarKit!=nullptr){
		fScrollBarKit->SetView(nullptr);
	}

	LayoutControl::ViewClear();
}
//---------------------------------------------------------------------------
cScrollControl* lScrollFrame::GetScrollControl(void)const noexcept(true)
{
	return fScrollControl;
}
//---------------------------------------------------------------------------
void lScrollFrame::SetScrollControl(cScrollControl *Control)noexcept(true)
{
	if(fScrollControl!=nullptr){
		fScrollControl->ScrollOffsetChangeNotify.Remove(fScrollOffsetChangeNotifyToken);
		fScrollControl->ScrollRangeChangeNotify.Remove(fScrollRangeChangeNotifyToken);
	}
	fScrollControl=Control;
	if(fScrollControl!=nullptr){
		fScrollOffsetChangeNotifyToken=fScrollControl->ScrollOffsetChangeNotify.Insert([this]{
			ScrollOffsetChanged();
		});
		fScrollRangeChangeNotifyToken=fScrollControl->ScrollRangeChangeNotify.Insert([this]{
			ScrollRangeChanged();
		});
	
	}
}
//---------------------------------------------------------------------------
kiScrollBar* lScrollFrame::GetScrollBarKit(void)const noexcept(true)
{
	return fScrollBarKit;
}
//---------------------------------------------------------------------------
void lScrollFrame::SetScrollBarKit(rPtr<kiScrollBar> Kit)noexcept(true)
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
void lScrollFrame::UpdateScrollBarLayoutOrder(void)noexcept(true)
{
	if(fScrollBarKit!=nullptr){
		fScrollBarKit->SetLayoutOrder(ScrollBarKitLayoutOrder);
	}
}
//---------------------------------------------------------------------------
void lScrollFrame::ControlSetupDefaultScrollBarKit(void)noexcept(true)
{
	if(gCreateDefaultScrollBarKit!=nullptr){
		auto ScrollBarKit=gCreateDefaultScrollBarKit();
		SetScrollBarKit(ScrollBarKit);
	}
}
//---------------------------------------------------------------------------
cUIPoint lScrollFrame::GetScrollOffset(void)const noexcept(true)
{
	if(fScrollControl==nullptr)
		return UIPointZero;

	return fScrollControl->GetScrollOffset();
}
//---------------------------------------------------------------------------
bool lScrollFrame::SetScrollOffset(cUIPoint Offset)noexcept(true)
{
	if(fScrollControl==nullptr)
		return false;

	return fScrollControl->SetScrollOffset(Offset);
}
//---------------------------------------------------------------------------
Float32 lScrollFrame::GetScrollOffset(bool Vertical)const noexcept(true)
{
	if(fScrollControl==nullptr)
		return 0;
	return fScrollControl->GetScrollOffset(Vertical);
}
//---------------------------------------------------------------------------
bool lScrollFrame::SetScrollOffset(bool Vertical,Float32 Offset)noexcept(true)
{
	if(fScrollControl==nullptr)
		return false;
	return fScrollControl->SetScrollOffset(Vertical,Offset);
}
//---------------------------------------------------------------------------
iControlScrollInfo::cScrollRange lScrollFrame::GetScrollRange(bool Vertical)const noexcept(true)
{
	if(fScrollControl==nullptr){
		iControlScrollInfo::cScrollRange ScrollRange;
		ScrollRange.LowwerLimit=0;
		ScrollRange.UpperLimit=0;
		ScrollRange.NoLowwerLimit=false;
		ScrollRange.NoUpperLimit=false;
		ScrollRange.ViewportSize=0;
		return ScrollRange;
	}
	return fScrollControl->GetScrollRange(Vertical);
}
//---------------------------------------------------------------------------
Float32 lScrollFrame::GetScrollStepSize(bool Vertical)const noexcept(true)
{
	if(fScrollControl==nullptr)
		return 0;
	return fScrollControl->GetScrollStepSize(Vertical);
}
//---------------------------------------------------------------------------
Float32 lScrollFrame::GetScrollPageSize(bool Vertical)const noexcept(true)
{
	if(fScrollControl==nullptr)
		return 0;
	return fScrollControl->GetScrollPageSize(Vertical);
}
//---------------------------------------------------------------------------
bool lScrollFrame::ScrollContentRangeToVisible(bool Vertical,Float32 Lowwer,Float32 Upper)noexcept(true)
{
	if(fScrollControl==nullptr)
		return false;

	return fScrollControl->ScrollRangeToVisible(Vertical,Lowwer,Upper);
}
//---------------------------------------------------------------------------
bool lScrollFrame::ScrollContentRangeToVisible(Float32 Left,Float32 Top,Float32 Right,Float32 Bottom)noexcept(true)
{
	if(fScrollControl==nullptr)
		return false;

	return fScrollControl->ScrollRangeToVisible(Left,Top,Right,Bottom);
}
//---------------------------------------------------------------------------
bool lScrollFrame::ScrollContentRectToVisible(cUIPoint Pos,cUIPoint Size)noexcept(true)
{
	if(fScrollControl==nullptr)
		return false;

	return fScrollControl->ScrollRectToVisible(Pos,Size);
}
//---------------------------------------------------------------------------
void lScrollFrame::ScrollOffsetChanged(void)noexcept(true)
{
	ScrollInfoHorizontal.ScrollOffsetNotifySet();
	ScrollInfoVertical.ScrollOffsetNotifySet();
}
//---------------------------------------------------------------------------
void lScrollFrame::ScrollRangeChanged(void)noexcept(true)
{
	ScrollInfoHorizontal.ScrollInfoNotifySet();
	ScrollInfoVertical.ScrollInfoNotifySet();
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
lScrollFrame* lScrollFrame::cScrollInfoHorizontal::GetHost(void)noexcept(true)
{
	return cnMemory::GetObjectFromMemberPointer(this,&lScrollFrame::ScrollInfoHorizontal);
}
//---------------------------------------------------------------------------
const lScrollFrame* lScrollFrame::cScrollInfoHorizontal::GetHost(void)const noexcept(true)
{
	return cnMemory::GetObjectFromMemberPointer(this,&lScrollFrame::ScrollInfoHorizontal);
}
//---------------------------------------------------------------------------
Float32 lScrollFrame::cScrollInfoHorizontal::ScrollGetOffset(void)const noexcept(true)
{
	auto Host=GetHost();
	return Host->GetScrollOffset(false);
}
//---------------------------------------------------------------------------
bool lScrollFrame::cScrollInfoHorizontal::ScrollSetOffset(Float32 Offset)noexcept(true)
{
	auto Host=GetHost();
	return Host->SetScrollOffset(false,Offset);
}
//---------------------------------------------------------------------------
iControlScrollInfo::cScrollRange lScrollFrame::cScrollInfoHorizontal::ScrollGetScrollRange(void)const noexcept(true)
{
	auto Host=GetHost();
	return Host->GetScrollRange(false);
}
//---------------------------------------------------------------------------
Float32 lScrollFrame::cScrollInfoHorizontal::ScrollGetStepSize(void)const noexcept(true)
{
	auto Host=GetHost();
	return Host->GetScrollStepSize(false);
}
//---------------------------------------------------------------------------
Float32 lScrollFrame::cScrollInfoHorizontal::ScrollGetPageSize(void)const noexcept(true)
{
	auto Host=GetHost();
	return Host->GetScrollPageSize(false);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
lScrollFrame* lScrollFrame::cScrollInfoVertical::GetHost(void)noexcept(true)
{
	return cnMemory::GetObjectFromMemberPointer(this,&lScrollFrame::ScrollInfoVertical);
}
//---------------------------------------------------------------------------
const lScrollFrame* lScrollFrame::cScrollInfoVertical::GetHost(void)const noexcept(true)
{
	return cnMemory::GetObjectFromMemberPointer(this,&lScrollFrame::ScrollInfoVertical);
}
//---------------------------------------------------------------------------
Float32 lScrollFrame::cScrollInfoVertical::ScrollGetOffset(void)const noexcept(true)
{
	auto Host=GetHost();
	return Host->GetScrollOffset(true);
}
//---------------------------------------------------------------------------
bool lScrollFrame::cScrollInfoVertical::ScrollSetOffset(Float32 Offset)noexcept(true)
{
	auto Host=GetHost();
	return Host->SetScrollOffset(true,Offset);
}
//---------------------------------------------------------------------------
iControlScrollInfo::cScrollRange lScrollFrame::cScrollInfoVertical::ScrollGetScrollRange(void)const noexcept(true)
{
	auto Host=GetHost();
	return Host->GetScrollRange(true);
}
//---------------------------------------------------------------------------
Float32 lScrollFrame::cScrollInfoVertical::ScrollGetStepSize(void)const noexcept(true)
{
	auto Host=GetHost();
	return Host->GetScrollStepSize(true);
}
//---------------------------------------------------------------------------
Float32 lScrollFrame::cScrollInfoVertical::ScrollGetPageSize(void)const noexcept(true)
{
	auto Host=GetHost();
	return Host->GetScrollPageSize(true);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cScrollContainer::cScrollContainer()noexcept(true)
	: fScrollContent(nullptr)
{
}
//---------------------------------------------------------------------------
cScrollContainer::~cScrollContainer()noexcept(true)
{
	SetView(nullptr);
}
//---------------------------------------------------------------------------
void cScrollContainer::ViewSetup(void)noexcept(true)
{
	cScrollControl::ViewSetup();
	if(fScrollContent!=nullptr){
		fScrollContent->ScrollContentShow();
	}
}
//---------------------------------------------------------------------------
void cScrollContainer::ViewClear(void)noexcept(true)
{
	if(fScrollContent!=nullptr){
		fScrollContent->ScrollContentHide();
	}
	cScrollControl::ViewClear();
}
//---------------------------------------------------------------------------
void cScrollContainer::ScrollContentUpdateContentLayout(void)noexcept(true)
{
	cScrollControl::ScrollContentUpdateContentLayout();

	if(fScrollContent!=nullptr){
		fScrollContent->ScrollContentUpdate();
	}
}
//---------------------------------------------------------------------------
iUIView* cScrollContainer::GetScrollView(void)noexcept(true)
{
	return fView;	
}
//---------------------------------------------------------------------------
iScrollContent* cScrollContainer::GetContent(void)noexcept(true)
{
	return fScrollContent;
}
//---------------------------------------------------------------------------
void cScrollContainer::SetContent(iScrollContent *ScrollContent)noexcept(true)
{
	if(fScrollContent!=nullptr){
		if(fView!=nullptr){
			fScrollContent->ScrollContentHide();
		}
	}
	fScrollContent=ScrollContent;
	if(fScrollContent!=nullptr){
		if(fView!=nullptr){
			fScrollContent->ScrollContentShow();
		}
	}
}
//---------------------------------------------------------------------------
cUIPoint cScrollContainer::GetOffset(void)noexcept(true)
{
	return cScrollControl::GetScrollOffset();
}
//---------------------------------------------------------------------------
bool cScrollContainer::SetOffset(cUIPoint Offset)noexcept(true)
{
	return cScrollControl::SetScrollOffset(Offset);
}
//---------------------------------------------------------------------------
cUIRange cScrollContainer::GetScrollLimitX(bool &NoLowwerLimit,bool &NoUpperLimit)noexcept(true)
{
	cUIRange Range;
	Range.Begin=fScrollLowwerLimit[0];
	Range.End=fScrollUpperLimit[0];
	NoLowwerLimit=fScrollNoLowwerLimit[0];
	NoUpperLimit=fScrollNoUpperLimit[0];
	return Range;
}
//---------------------------------------------------------------------------
void cScrollContainer::SetScrollLimitX(cUIRange OffsetRange,bool NoLowwerLimit,bool NoUpperLimit)noexcept(true)
{
	fScrollLowwerLimit[0]=OffsetRange.Begin;
	fScrollUpperLimit[0]=OffsetRange.End;
	fScrollNoLowwerLimit[0]=NoLowwerLimit;
	fScrollNoUpperLimit[0]=NoUpperLimit;
}
//---------------------------------------------------------------------------
cUIRange cScrollContainer::GetScrollLimitY(bool &NoLimitLowwer,bool &NoLimitUpper)noexcept(true)
{
	cUIRange Range;
	Range.Begin=fScrollLowwerLimit[1];
	Range.End=fScrollUpperLimit[1];
	NoLimitLowwer=fScrollNoLowwerLimit[1];
	NoLimitUpper=fScrollNoUpperLimit[1];
	return Range;
}
//---------------------------------------------------------------------------
void cScrollContainer::SetScrollLimitY(cUIRange OffsetRange,bool NoLimitLowwer,bool NoLimitUpper)noexcept(true)
{
	fScrollLowwerLimit[1]=OffsetRange.Begin;
	fScrollUpperLimit[1]=OffsetRange.End;
	fScrollNoLowwerLimit[1]=NoLimitLowwer;
	fScrollNoUpperLimit[1]=NoLimitUpper;
}
//---------------------------------------------------------------------------
cUIPoint cScrollContainer::GetViewportSize(void)noexcept(true)
{
	cUIPoint RetSize;
	RetSize.x=fScrollViewportSize[0];
	RetSize.y=fScrollViewportSize[1];
	return RetSize;
}
//---------------------------------------------------------------------------
void cScrollContainer::NotifyUpdateScroll(void)noexcept(true)
{
	SetNeedUpdateContentLayout();
}
//---------------------------------------------------------------------------
bool cScrollContainer::IsRectangleInViewport(const cUIRectangle &Rect)noexcept(true)
{
	return cScrollControl::IsRectangleInViewport(Rect);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cScrollViewContainer::cScrollViewContainer()noexcept(true)
{
}
//---------------------------------------------------------------------------
cScrollViewContainer::~cScrollViewContainer()noexcept(true)
{
	SetView(nullptr);

	auto RemainingViewSet=cnVar::MoveCast(fContentViewSet);
	for(auto *Item : RemainingViewSet){
		Item->fOwner=nullptr;
	}
}
//---------------------------------------------------------------------------
void cScrollViewContainer::ViewSetup(void)noexcept(true)
{
	cScrollControl::ViewSetup();

	for(auto *Item : fContentViewSet){
		if(Item->View!=nullptr)
			fView->InsertView(Item->View);
	}
}
//---------------------------------------------------------------------------
void cScrollViewContainer::ViewClear(void)noexcept(true)
{
	for(auto *Item : fContentViewSet){
		if(Item->View!=nullptr)
			fView->RemoveView(Item->View);
	}
	fVisibleSet.Clear();

	cScrollControl::ViewClear();
}
//---------------------------------------------------------------------------
void cScrollViewContainer::ScrollContentUpdateContentLayout(void)noexcept(true)
{
	cScrollControl::ScrollContentUpdateContentLayout();
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
void cScrollViewContainer::UILayout(void)noexcept(true)
{
	cScrollControl::UILayout();

	for(auto Item : fVisibleSet){
		if(Item->View==nullptr)
			continue;

		auto ItemRect=ViewItemCalculateLayoutRect(Item);

		Item->View->ArrangeRectangle(fView,ItemRect.Pos,ItemRect.Size);
	}
}
//---------------------------------------------------------------------------
void cScrollViewContainer::ViewItemAttach(bcViewItem *Item)noexcept(true)
{
	if(fView!=nullptr){
		if(Item->View!=nullptr){
			fView->InsertView(Item->View);
		}
		ViewItemCheckVisible(Item);
	}
}
//---------------------------------------------------------------------------
void cScrollViewContainer::ViewItemDetach(bcViewItem *Item)noexcept(true)
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
const cUIRectangle& cScrollViewContainer::ViewItemGetRectangle(const bcViewItem *Item)noexcept(true)
{
	return Item->ContentRect;
}
//---------------------------------------------------------------------------
void cScrollViewContainer::ViewItemSetRectangle(bcViewItem *Item,cUIRectangle Rect)noexcept(true)
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
iViewReference* cScrollViewContainer::ViewItemGetViewReference(const bcViewItem *Item)const noexcept(true)
{
	return Item->ViewReference;
}
//---------------------------------------------------------------------------
bool cScrollViewContainer::ViewItemSetViewReference(bcViewItem *Item,iViewReference *ViewReference)noexcept(true)
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
cUIRect cScrollViewContainer::ViewItemCalculateLayoutRect(const bcViewItem *Item)const noexcept(true)
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
void cScrollViewContainer::ViewItemCheckVisible(bcViewItem *Item)noexcept(true)
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
void cScrollViewContainer::RecycleView(rPtr<iViewReference> ViewReference)noexcept(true)
{
	auto ImpType=ViewReference->GetImplementationType();
	fViewRecycleMap[ImpType].AppendMake(cnVar::MoveCast(ViewReference));
}
//---------------------------------------------------------------------------
rPtr<iViewReference> cScrollViewContainer::ReclaimView(tTypeID ImplementationType)noexcept(true)
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
void cScrollViewContainer::ContentViewAttach(cViewItem *Item)noexcept(true)
{
	fContentViewSet.Insert(Item);
	ViewItemAttach(Item);
}
//---------------------------------------------------------------------------
void cScrollViewContainer::ContentViewDetach(cViewItem *Item)noexcept(true)
{
	ViewItemDetach(Item);
	fContentViewSet.Remove(Item);
}
//---------------------------------------------------------------------------
bool cScrollViewContainer::bcViewItem::IsVisible(void)const noexcept(true)
{
	return Visible;
}
//---------------------------------------------------------------------------
iUIView *cScrollViewContainer::bcViewItem::GetView(void)const noexcept(true)
{
	return View;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cScrollViewContainer::cViewItem::cViewItem()noexcept(true)
	: fOwner(nullptr)
{
}
//---------------------------------------------------------------------------
cScrollViewContainer::cViewItem::~cViewItem()noexcept(true)
{
	if(fOwner!=nullptr){
		fOwner->ContentViewDetach(this);
	}
}
//---------------------------------------------------------------------------
bool cScrollViewContainer::cViewItem::Attach(cScrollViewContainer *Owner)noexcept(true)
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
bool cScrollViewContainer::cViewItem::Detach(cScrollViewContainer *Owner)noexcept(true)
{
	if(fOwner!=Owner)
		return false;

	fOwner->ContentViewDetach(this);
	fOwner=nullptr;
	return true;
}
//---------------------------------------------------------------------------
const cUIRectangle& cScrollViewContainer::cViewItem::GetRectangle(void)const noexcept(true)
{
	if(fOwner==nullptr)
		return UIRectangleZero;
	return fOwner->ViewItemGetRectangle(this);
}
//---------------------------------------------------------------------------
void cScrollViewContainer::cViewItem::SetRectangle(cUIRectangle Rect)noexcept(true)
{
	return fOwner->ViewItemSetRectangle(this,Rect);
}
//---------------------------------------------------------------------------
iViewReference* cScrollViewContainer::cViewItem::GetViewReference(void)const noexcept(true)
{
	if(fOwner==nullptr)
		return nullptr;
	return fOwner->ViewItemGetViewReference(this);
}
//---------------------------------------------------------------------------
bool cScrollViewContainer::cViewItem::SetViewReference(iViewReference *Reference)noexcept(true)
{
	if(fOwner==nullptr)
		return false;

	return fOwner->ViewItemSetViewReference(this,Reference);
}
//---------------------------------------------------------------------------
cUIRect cScrollViewContainer::cViewItem::CalculateLayoutRect(void)const noexcept(true)
{
	if(fOwner==nullptr)
		return UIRectZero;
	return fOwner->ViewItemCalculateLayoutRect(this);
}
//---------------------------------------------------------------------------
void cScrollViewContainer::cViewItem::VisibilityChanged(void)noexcept(true)
{
	OnVisibleChanged();
}
//---------------------------------------------------------------------------
