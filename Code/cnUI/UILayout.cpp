#include "UILayout.h"

//---------------------------------------------------------------------------
using namespace cnLibrary;
using namespace cnUI;
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cUIPoint cnUI::RectAlignInRect(eAlignment Align,Float32 DestLeft,Float32 DestTop,Float32 DestRight,Float32 DestBottom,cUIPoint AlignSize,cUIPoint DefaultPos)
{
	cUIPoint AlignPos;
	switch(Align&(Alignment::Left|Alignment::Right)){
	default:
	case Alignment::None:
		AlignPos.x=DefaultPos.x;
		break;
	case Alignment::Left:
		AlignPos.x=DestLeft;
		break;
	case Alignment::Right:
		AlignPos.x=DestRight-AlignSize.x;
		break;
	case Alignment::HCenter:
		AlignPos.x=(DestLeft+DestRight-AlignSize.x)/2;
		break;
	}
	switch(Align&(Alignment::Top|Alignment::Bottom)){
	default:
	case Alignment::None:
		AlignPos.y=DefaultPos.y;
		break;
	case Alignment::Top:
		AlignPos.y=DestTop;
		break;
	case Alignment::Bottom:
		AlignPos.y=DestBottom-AlignSize.y;
		break;
	case Alignment::VCenter:
		AlignPos.y=(DestTop+DestBottom-AlignSize.y)/2;
		break;
	}
	return AlignPos;
}
//---------------------------------------------------------------------------
cUIPoint cnUI::RectAlignInRect(eAlignment Align,cUIPoint DestSize,cUIPoint AlignSize,cUIPoint AlignPos)
{
	switch(Align&(Alignment::Left|Alignment::Right)){
	default:
	case Alignment::None:
		break;
	case Alignment::Left:
		AlignPos.x=0;
		break;
	case Alignment::Right:
		AlignPos.x=DestSize.x-AlignSize.x;
		break;
	case Alignment::HCenter:
		AlignPos.x=(DestSize.x-AlignSize.x)/2;
		break;
	}
	switch(Align&(Alignment::Top|Alignment::Bottom)){
	default:
	case Alignment::None:
		break;
	case Alignment::Top:
		AlignPos.y=0;
		break;
	case Alignment::Bottom:
		AlignPos.y=DestSize.y-AlignSize.y;
		break;
	case Alignment::VCenter:
		AlignPos.y=(DestSize.y-AlignSize.y)/2;
		break;
	}
	return AlignPos;
}
//---------------------------------------------------------------------------
cUIPoint cnUI::RectStretchAlignInRect(eAlignment Align,cUIPoint DestSize,cUIPoint &AlignSize)
{
	cUIPoint Pos;
	switch(Align&(Alignment::Left|Alignment::Right)){
	default:
	case Alignment::None:
		Pos.x=0;
		AlignSize.x=DestSize.x;
		break;
	case Alignment::Left:
		Pos.x=0;
		break;
	case Alignment::Right:
		Pos.x=DestSize.x-AlignSize.x;
		break;
	case Alignment::HCenter:
		Pos.x=(DestSize.x-AlignSize.x)/2;
		break;
	}
	switch(Align&(Alignment::Top|Alignment::Bottom)){
	default:
	case Alignment::None:
		Pos.y=0;
		AlignSize.y=DestSize.y;
		break;
	case Alignment::Top:
		Pos.y=0;
		break;
	case Alignment::Bottom:
		Pos.y=DestSize.y-AlignSize.y;
		break;
	case Alignment::VCenter:
		Pos.y=(DestSize.y-AlignSize.y)/2;
		break;
	}
	return Pos;
}
//---------------------------------------------------------------------------
cUIPoint cnUI::RectAlignAroundRect(eAroundAlignment Align,cUIPoint DestPos,cUIPoint DestSize,cUIPoint SrcSize,cUIPoint DefaultPos)
{
	cUIPoint Pos;
	switch(Align&AroundAlignment::MaskDirection){
	default:
	case AroundAlignment::Top:
		Pos.y=DestPos.y-SrcSize.y;
		break;
	case AroundAlignment::Bottom:
		Pos.y=DestPos.y+DestSize.y;
		break;
	case AroundAlignment::Left:
		Pos.x=DestPos.x-SrcSize.x;
		break;
	case AroundAlignment::Right:
		Pos.x=DestPos.x+DestSize.x;
		break;
	}

	if((Align&AroundAlignment::MaskAxis)==AroundAlignment::Horizontal){
		switch(Align&AroundAlignment::MaskSide){
		default:	// SideNone
			Pos.y=DefaultPos.y;
			break;
		case AroundAlignment::SideLowwer:
			Pos.y=DestPos.y+DestSize.y-SrcSize.y;
			break;
		case AroundAlignment::SideUpper:
			Pos.y=DestPos.y;
			break;
		case AroundAlignment::SideMiddle:
			Pos.y=DestPos.y+(DestSize.y-SrcSize.y)/2;
			break;
		}
	}
	else{
		switch(Align&AroundAlignment::MaskSide){
		default:	// SideNone
			Pos.x=DefaultPos.x;
			break;
		case AroundAlignment::SideLowwer:
			Pos.x=DestPos.x+DestSize.x-SrcSize.x;
			break;
		case AroundAlignment::SideUpper:
			Pos.x=DestPos.x;
			break;
		case AroundAlignment::SideMiddle:
			Pos.x=DestPos.x+(DestSize.x-SrcSize.x)/2;
			break;
		}
	}
	return Pos;
}
//---------------------------------------------------------------------------
cUIPoint cnUI::RectAlignAroundRectLimited(eAroundAlignment Align,cUIPoint LimitSize,cUIPoint DestPos,cUIPoint DestSize,cUIPoint SrcSize,cUIPoint DefaultPos)
{
	cUIPoint Pos;

	Float32 LimitSizeRight=LimitSize.x-DestPos.x-DestSize.x;
	Float32 LimitSizeBottom=LimitSize.y-DestPos.y-DestSize.y;
	bool DirectionPositive;
	switch(Align&AroundAlignment::MaskDirection){
	default:
	case AroundAlignment::Top:
		DirectionPositive=false;
		if(DestPos.y<SrcSize.y){
			// not enough size for top
			if(LimitSizeBottom>SrcSize.y){
				// enough size for bottom, switch to bottom
				DirectionPositive=true;
			}
		}
		break;
	case AroundAlignment::Bottom:
		DirectionPositive=true;
		if(LimitSizeBottom<SrcSize.y){
			// not enough size for bottom
			if(DestPos.y>SrcSize.y){
				// enough size for top, switch to top
				DirectionPositive=false;
			}
		}
		break;
	case AroundAlignment::Left:
		DirectionPositive=false;
		if(DestPos.x<SrcSize.x){
			// not enough size for left
			if(LimitSizeRight>SrcSize.x){
				// enough size for right, switch to right
				DirectionPositive=true;
			}
		}
		break;
	case AroundAlignment::Right:
		DirectionPositive=true;
		if(LimitSizeRight<SrcSize.x){
			// not enough size for bottom
			if(DestPos.x>SrcSize.x){
				// enough size for left, switch to left
				DirectionPositive=false;
			}
		}
		break;
	}

	if((Align&AroundAlignment::MaskAxis)==AroundAlignment::Horizontal){
		if(DirectionPositive){
			// Right
			Pos.x=DestPos.x+DestSize.x;
		}
		else{
			// Left
			Pos.x=DestPos.x-SrcSize.x;
		}
		Float32 LimitSizeBottomSide=LimitSize.y-DestPos.y;
		Float32 PosAlignTop=DestPos.y+DestSize.y-SrcSize.y;
		Float32 PosAlignBottom=DestPos.y;
		bool FixTop;
		bool FixBottom;
		switch(Align&AroundAlignment::MaskSide){
		default:	// SideNone
			Pos.y=DefaultPos.y;
			FixTop=false;
			FixBottom=false;
			break;
		case AroundAlignment::SideLowwer:
			// Top
			Pos.y=PosAlignTop;
			FixTop=false;
			FixBottom=true;
			break;
		case AroundAlignment::SideUpper:
			// Bottom
			Pos.y=PosAlignBottom;
			FixTop=true;
			FixBottom=false;
			break;
		case AroundAlignment::SideMiddle:
			Pos.y=DestPos.y+(DestSize.y-SrcSize.y)/2;
			FixBottom=true;
			FixTop=true;
			break;
		}
		if(FixBottom && Pos.y<0){
			// not enough room for top
			if(LimitSizeBottomSide>SrcSize.y){
				// switch to bottom
				Pos.y=PosAlignBottom;
			}
		}
		else if(FixTop && Pos.y+SrcSize.y>LimitSize.y){
			// not enough room for bottom
			if(PosAlignTop>0){
				// switch to top
				Pos.y=PosAlignTop;
			}
		}
	}
	else{
		if(DirectionPositive){
			// Bottom
			Pos.y=DestPos.y+DestSize.y;
		}
		else{
			// Top
			Pos.y=DestPos.y-SrcSize.y;
		}
		Float32 LimitSizeRightSide=LimitSize.x-DestPos.x;
		Float32 PosAlignLeft=DestPos.x+DestSize.x-SrcSize.x;
		Float32 PosAlignRight=DestPos.x;
		bool FixLeft;
		bool FixRight;
		switch(Align&AroundAlignment::MaskSide){
		default:	// SideNone
			Pos.x=DefaultPos.x;
			FixLeft=false;
			FixRight=false;
			break;
		case AroundAlignment::SideLowwer:
			// Left
			Pos.x=PosAlignLeft;
			FixLeft=false;
			FixRight=true;
			break;
		case AroundAlignment::SideUpper:
			// Right
			Pos.x=PosAlignRight;
			FixLeft=true;
			FixRight=false;
			break;
		case AroundAlignment::SideMiddle:
			Pos.x=DestPos.x+(DestSize.x-SrcSize.x)/2;
			FixRight=true;
			FixLeft=true;
			break;
		}
		if(FixRight && Pos.x<0){
			// not enough room for left
			if(LimitSizeRightSide>SrcSize.x){
				// switch to right
				Pos.x=PosAlignRight;
			}
		}
		else if(FixLeft && Pos.x+SrcSize.x>LimitSize.x){
			// not enough room for right
			if(PosAlignLeft>0){
				// switch to left
				Pos.x=PosAlignLeft;
			}
		}
	}
	return Pos;
}
//---------------------------------------------------------------------------
void cnUI::RectApplyMargin(cUIRect &Rect,const cUIRectangle &Margin)
{
	Rect.Pos.x+=Margin.Left;
	Rect.Pos.y+=Margin.Top;

	Rect.Size.x-=Margin.Left+Margin.Right;
	Rect.Size.y-=Margin.Top+Margin.Bottom;
}
//---------------------------------------------------------------------------
cUIRectangle cnUI::MergeMargin(cUIRectangle Margin1,const cUIRectangle &Margin2)
{
	Margin1.Left-=Margin2.Left;
	Margin1.Top-=Margin2.Top;
	Margin1.Right-=Margin2.Right;
	Margin1.Bottom-=Margin2.Bottom;


	if(Margin1.Left<0)
		Margin1.Left=0;
	if(Margin1.Top<0)
		Margin1.Top=0;
	if(Margin1.Right<0)
		Margin1.Right=0;
	if(Margin1.Bottom<0)
		Margin1.Bottom=0;
	return Margin1;
}
//---------------------------------------------------------------------------
cVector<Float32,4> cnUI::RectToVector(Float32 Left,Float32 Top,Float32 Right,Float32 Bottom)
{
	cVector<Float32,4> v;
	v[0]=Left;
	v[1]=Top;
	v[2]=Right;
	v[3]=Bottom;
	return v;
}
//---------------------------------------------------------------------------
cVector<Float32,4> cnUI::RectToVector(const cUIRectangle &Rect)
{
	cVector<Float32,4> v;
	v[0]=Rect.Left;
	v[1]=Rect.Top;
	v[2]=Rect.Right;
	v[3]=Rect.Bottom;
	return v;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void cPixelLayout::ClearMargin(void)
{
	ClientMarginVector.Zero();
}
//---------------------------------------------------------------------------
void cPixelLayout::Reset(const cUIRect &Rect)
{
	return Reset(Rect.Pos,Rect.Size);
}
//---------------------------------------------------------------------------
void cPixelLayout::Reset(const cUIPoint &Size)
{
	ClearMargin();

	Bounds.Left=0;
	Bounds.Top=0;
	Bounds.Right=Size.x;
	Bounds.Bottom=Size.y;
}
//---------------------------------------------------------------------------
void cPixelLayout::Reset(const cUIPoint &Pos,const cUIPoint &Size)
{
	ClearMargin();

	Bounds.Left=Pos.x;
	Bounds.Top=Pos.y;
	Bounds.Right=Pos.x+Size.x;
	Bounds.Bottom=Pos.y+Size.y;
}
//---------------------------------------------------------------------------
void cPixelLayout::SetMargin(Float32 MarginLeft,Float32 MarginTop,Float32 MarginRight,Float32 MarginBottom)
{
	ClientMargin.Left=MarginLeft;
	ClientMargin.Top=MarginTop;
	ClientMargin.Right=MarginRight;
	ClientMargin.Bottom=MarginBottom;
}
//---------------------------------------------------------------------------
cPixelLayout::operator cUIRect ()const
{
	cUIRect r;
	r.Pos.x=Bounds.Left;
	r.Pos.y=Bounds.Top;
	r.Size.x=Bounds.Right-Bounds.Left;
	r.Size.y=Bounds.Bottom-Bounds.Top;
	return r;
}
//---------------------------------------------------------------------------
void cPixelLayout::MergeMargin(const cVector<Float32,4> &MarginVector)
{
	ClientMarginVector-=MarginVector;

	if(ClientMargin.Left<0)
		ClientMargin.Left=0;
	if(ClientMargin.Top<0)
		ClientMargin.Top=0;
	if(ClientMargin.Right<0)
		ClientMargin.Right=0;
	if(ClientMargin.Bottom<0)
		ClientMargin.Bottom=0;
}
//---------------------------------------------------------------------------
void cPixelLayout::MergeMargin(const cUIRectangle &Margin)
{
	return MergeMargin(RectToVector(Margin));
}
//---------------------------------------------------------------------------
void cPixelLayout::MergeMargin(Float32 MarginLeft,Float32 MarginTop,Float32 MarginRight,Float32 MarginBottom)
{
	return MergeMargin(RectToVector(MarginLeft,MarginTop,MarginRight,MarginBottom));
}
//---------------------------------------------------------------------------
void cPixelLayout::AddMargin(const cVector<Float32,4> &MarginVector)
{
	ClientMarginVector+=MarginVector;
}
//---------------------------------------------------------------------------
void cPixelLayout::AddMargin(const cUIRectangle &Margin)
{
	cVector<Float32,4> MarginVector=RectToVector(Margin);
	ClientMarginVector+=MarginVector;
}
//---------------------------------------------------------------------------
void cPixelLayout::AddMargin(Float32 MarginLeft,Float32 MarginTop,Float32 MarginRight,Float32 MarginBottom)
{
	cVector<Float32,4> MarginVector=RectToVector(MarginLeft,MarginTop,MarginRight,MarginBottom);
	ClientMarginVector+=MarginVector;
}
//---------------------------------------------------------------------------
Float32 cPixelLayout::GetClientWidth(Float32 MarginLeft,Float32 MarginRight)const
{
	if(MarginLeft<ClientMargin.Left){
		MarginLeft=ClientMargin.Left;
	}
	if(MarginRight<ClientMargin.Right){
		MarginRight=ClientMargin.Right;
	}
	Float32 ClientLeft=Bounds.Left+MarginLeft;
	Float32 ClientRight=Bounds.Right-MarginRight;
	return ClientRight-ClientLeft;
}
//---------------------------------------------------------------------------
Float32 cPixelLayout::GetClientHeight(Float32 MarginTop,Float32 MarginBottom)const
{
	if(MarginTop<ClientMargin.Top){
		MarginTop=ClientMargin.Top;
	}
	if(MarginBottom<ClientMargin.Bottom){
		MarginBottom=ClientMargin.Bottom;
	}
	Float32 ClientTop=Bounds.Top+MarginTop;
	Float32 ClientBottom=Bounds.Bottom-MarginBottom;
	return ClientBottom-ClientTop;
}
//---------------------------------------------------------------------------
cPixelLayout cPixelLayout::Union(const cPixelLayout &Src)const
{
	cPixelLayout r;
	if(Bounds.Left<Src.Bounds.Left){
		r.Bounds.Left=Bounds.Left;
		r.ClientMargin.Left=ClientMargin.Left;
	}
	else{
		r.Bounds.Left=Src.Bounds.Left;
		r.ClientMargin.Left=Src.ClientMargin.Left;
	}
	if(Bounds.Top<Src.Bounds.Top){
		r.Bounds.Top=Bounds.Top;
		r.ClientMargin.Top=ClientMargin.Top;
	}
	else{
		r.Bounds.Top=Src.Bounds.Top;
		r.ClientMargin.Top=Src.ClientMargin.Top;
	}
	if(Bounds.Right>Src.Bounds.Right){
		r.Bounds.Right=Bounds.Right;
		r.ClientMargin.Right=ClientMargin.Right;
	}
	else{
		r.Bounds.Right=Src.Bounds.Right;
		r.ClientMargin.Right=Src.ClientMargin.Right;
	}
	if(Bounds.Bottom>Src.Bounds.Bottom){
		r.Bounds.Bottom=Bounds.Bottom;
		r.ClientMargin.Bottom=ClientMargin.Bottom;
	}
	else{
		r.Bounds.Bottom=Src.Bounds.Bottom;
		r.ClientMargin.Bottom=Src.ClientMargin.Bottom;
	}
	return r;
}
//---------------------------------------------------------------------------
cPixelLayout cPixelLayout::Padding(Float32 pLeft,Float32 pTop,Float32 pRight,Float32 pBottom)const
{
	cPixelLayout r;
	r.ClearMargin();
	r.Bounds.Left=Bounds.Left+pLeft;
	r.Bounds.Top=Bounds.Top+pTop;
	r.Bounds.Right=Bounds.Right-pRight;
	r.Bounds.Bottom=Bounds.Bottom-pBottom;
	return r;
}
//---------------------------------------------------------------------------
cPixelLayout cPixelLayout::Client(void)const
{
	cPixelLayout r;
	r.ClearMargin();
	r.Bounds.Left=Bounds.Left+ClientMargin.Left;
	r.Bounds.Top=Bounds.Top+ClientMargin.Top;
	r.Bounds.Right=Bounds.Right-ClientMargin.Right;
	r.Bounds.Bottom=Bounds.Bottom-ClientMargin.Bottom;
	if(r.Bounds.Right<r.Bounds.Left)
		r.Bounds.Right=r.Bounds.Left;
	if(r.Bounds.Bottom<r.Bounds.Top)
		r.Bounds.Bottom=r.Bounds.Top;
	return r;
}
//---------------------------------------------------------------------------
cPixelLayout cPixelLayout::Client(Float32 MarginLeft,Float32 MarginTop,Float32 MarginRight,Float32 MarginBottom)const
{
	if(MarginLeft<ClientMargin.Left){
		MarginLeft=ClientMargin.Left;
	}
	if(MarginTop<ClientMargin.Top){
		MarginTop=ClientMargin.Top;
	}
	if(MarginRight<ClientMargin.Right){
		MarginRight=ClientMargin.Right;
	}
	if(MarginBottom<ClientMargin.Bottom){
		MarginBottom=ClientMargin.Bottom;
	}
	cPixelLayout r;
	r.ClearMargin();
	r.Bounds.Left=Bounds.Left+MarginLeft;
	r.Bounds.Top=Bounds.Top+MarginTop;
	r.Bounds.Right=Bounds.Right-MarginRight;
	r.Bounds.Bottom=Bounds.Bottom-MarginBottom;
	if(r.Bounds.Right<r.Bounds.Left)
		r.Bounds.Right=r.Bounds.Left;
	if(r.Bounds.Bottom<r.Bounds.Top)
		r.Bounds.Bottom=r.Bounds.Top;
	return r;
}
//---------------------------------------------------------------------------
cPixelLayout cPixelLayout::Client(const cUIRectangle &Margin)const
{
	return Client(Margin.Left,Margin.Top,Margin.Right,Margin.Bottom);
}
//---------------------------------------------------------------------------
cPixelLayout cPixelLayout::Align(eAlignment Alignment,Float32 Width,Float32 Height)const
{
	cPixelLayout r;
	r.ClearMargin();

	r.Bounds.Left=Bounds.Left+ClientMargin.Left;
	r.Bounds.Right=Bounds.Right-ClientMargin.Right;
	r.Bounds.Top=Bounds.Top+ClientMargin.Top;
	r.Bounds.Bottom=Bounds.Bottom-ClientMargin.Bottom;

	switch(Alignment&(Alignment::Left|Alignment::Right)){
	case Alignment::None:
		break;
	case Alignment::Left:
		r.Bounds.Right=r.Bounds.Left+Width;
		break;
	case Alignment::Right:
		r.Bounds.Left=r.Bounds.Right-Width;
		break;
	default:
	case Alignment::HCenter:
		r.Bounds.Left=(r.Bounds.Left+r.Bounds.Right-Width)/2;
		r.Bounds.Right=r.Bounds.Left+Width;
		break;
	}
	switch(Alignment&(Alignment::Top|Alignment::Bottom)){
	case Alignment::None:
		break;
	case Alignment::Top:
		r.Bounds.Bottom=r.Bounds.Top+Height;
		break;
	case Alignment::Bottom:
		r.Bounds.Top=r.Bounds.Bottom-Height;
		break;
	default:
	case Alignment::VCenter:
		r.Bounds.Top=(r.Bounds.Top+r.Bounds.Bottom-Height)/2;
		r.Bounds.Bottom=r.Bounds.Top+Height;
		break;
	}
	return r;
}
//---------------------------------------------------------------------------
Float32 cPixelLayout::DivideWidth(ufInt32n Count,Float32 PartMargin,Float32 MarginLeft,Float32 MarginRight)const
{
	Float32 DivideWidth=GetClientWidth(MarginLeft,MarginRight);
	DivideWidth=(DivideWidth-PartMargin*(Count-1))/Count;
	return DivideWidth;
}
//---------------------------------------------------------------------------
Float32 cPixelLayout::DivideHeight(ufInt32n Count,Float32 PartMargin,Float32 MarginTop,Float32 MarginBottom)const
{
	Float32 DivideHeight=GetClientHeight(MarginTop,MarginBottom);
	DivideHeight=(DivideHeight-PartMargin*(Count-1))/Count;
	return DivideHeight;
}
//---------------------------------------------------------------------------
cPixelLayout cPixelLayout::LayoutLeft(Float32 Width,Float32 MarginLeft,Float32 MarginRight)
{
	cPixelLayout r;
	if(MarginLeft<ClientMargin.Left){
		MarginLeft=ClientMargin.Left;
	}
	r.ClientMargin.Left=MarginLeft;
	r.ClientMargin.Right=MarginRight;
	r.ClientMargin.Top=ClientMargin.Top;
	r.ClientMargin.Bottom=ClientMargin.Bottom;

	r.Bounds.Top=Bounds.Top;
	r.Bounds.Bottom=Bounds.Bottom;
	r.Bounds.Left=Bounds.Left;

	Bounds.Left=Bounds.Left+MarginLeft+Width;
	r.Bounds.Right=Bounds.Left+MarginRight;
	
	ClientMargin.Left=MarginRight;

	if(Bounds.Right<r.Bounds.Right){
		Bounds.Right=r.Bounds.Right;
	}
	return r;
}
//---------------------------------------------------------------------------
cPixelLayout cPixelLayout::LayoutRight(Float32 Width,Float32 MarginRight,Float32 MarginLeft)
{
	cPixelLayout r;
	if(MarginRight<ClientMargin.Right){
		MarginRight=ClientMargin.Right;
	}
	r.ClientMargin.Left=MarginLeft;
	r.ClientMargin.Right=MarginRight;
	r.ClientMargin.Top=ClientMargin.Top;
	r.ClientMargin.Bottom=ClientMargin.Bottom;
	r.Bounds.Top=Bounds.Top;
	r.Bounds.Bottom=Bounds.Bottom;
	r.Bounds.Right=Bounds.Right;

	Bounds.Right=Bounds.Right-MarginRight-Width;
	r.Bounds.Left=Bounds.Right-MarginLeft;

	ClientMargin.Right=MarginLeft;
	if(Bounds.Left>r.Bounds.Left){
		Bounds.Left=r.Bounds.Left;
	}
	return r;
}
//---------------------------------------------------------------------------
cPixelLayout cPixelLayout::LayoutTop(Float32 Height,Float32 MarginTop,Float32 MarginBottom)
{
	cPixelLayout r;
	if(MarginTop<ClientMargin.Top){
		MarginTop=ClientMargin.Top;
	}
	r.ClientMargin.Top=MarginTop;
	r.ClientMargin.Bottom=MarginBottom;
	r.ClientMargin.Left=ClientMargin.Left;
	r.ClientMargin.Right=ClientMargin.Right;
	r.Bounds.Left=Bounds.Left;
	r.Bounds.Right=Bounds.Right;
	r.Bounds.Top=Bounds.Top;
	
	Bounds.Top=Bounds.Top+MarginTop+Height;
	r.Bounds.Bottom=Bounds.Top+MarginBottom;

	ClientMargin.Top=MarginBottom;
	if(Bounds.Bottom<r.Bounds.Bottom){
		Bounds.Bottom=r.Bounds.Bottom;
	}
	return r;
}
//---------------------------------------------------------------------------
cPixelLayout cPixelLayout::LayoutBottom(Float32 Height,Float32 MarginBottom,Float32 MarginTop)
{
	cPixelLayout r;
	if(MarginBottom<ClientMargin.Bottom){
		MarginBottom=ClientMargin.Bottom;
	}
	r.ClientMargin.Top=MarginTop;
	r.ClientMargin.Bottom=MarginBottom;
	r.ClientMargin.Left=ClientMargin.Left;
	r.ClientMargin.Right=ClientMargin.Right;
	r.Bounds.Left=Bounds.Left;
	r.Bounds.Right=Bounds.Right;
	r.Bounds.Bottom=Bounds.Bottom;

	Bounds.Bottom=Bounds.Bottom-MarginBottom-Height;
	r.Bounds.Top=Bounds.Bottom-MarginTop;

	ClientMargin.Bottom=MarginTop;
	if(Bounds.Top>r.Bounds.Top){
		Bounds.Top=r.Bounds.Top;
	}
	return r;
}
//---------------------------------------------------------------------------
cPixelLayout cPixelLayout::PlaceLeft(Float32 Width,Float32 MarginLeft,Float32 MarginRight)
{
	cPixelLayout r;
	r.ClearMargin();

	if(MarginLeft<ClientMargin.Left){
		MarginLeft=ClientMargin.Left;
	}
	r.Bounds.Left=Bounds.Left+MarginLeft;
	r.Bounds.Right=r.Bounds.Left+Width;
	r.Bounds.Top=Bounds.Top+ClientMargin.Top;
	r.Bounds.Bottom=Bounds.Bottom-ClientMargin.Bottom;
	
	ClientMargin.Left=MarginRight;
	if(Bounds.Right<r.Bounds.Right){
		Bounds.Right=r.Bounds.Right;
	}
	Bounds.Left=r.Bounds.Right;
	return r;
}
//---------------------------------------------------------------------------
cPixelLayout cPixelLayout::PlaceRight(Float32 Width,Float32 MarginRight,Float32 MarginLeft)
{
	cPixelLayout r;
	r.ClearMargin();

	if(MarginRight<ClientMargin.Right){
		MarginRight=ClientMargin.Right;
	}
	r.Bounds.Right=Bounds.Right-MarginRight;
	r.Bounds.Left=r.Bounds.Right-Width;
	r.Bounds.Top=Bounds.Top+ClientMargin.Top;
	r.Bounds.Bottom=Bounds.Bottom-ClientMargin.Bottom;

	ClientMargin.Right=MarginLeft;
	if(Bounds.Left>r.Bounds.Left){
		Bounds.Left=r.Bounds.Left;
	}
	Bounds.Right=r.Bounds.Left;
	return r;
}
//---------------------------------------------------------------------------
cPixelLayout cPixelLayout::PlaceTop(Float32 Height,Float32 MarginTop,Float32 MarginBottom)
{
	cPixelLayout r;
	r.ClearMargin();

	if(MarginTop<ClientMargin.Top){
		MarginTop=ClientMargin.Top;
	}
	r.Bounds.Top=Bounds.Top+MarginTop;
	r.Bounds.Bottom=r.Bounds.Top+Height;
	r.Bounds.Left=Bounds.Left+ClientMargin.Left;
	r.Bounds.Right=Bounds.Right-ClientMargin.Right;

	ClientMargin.Top=MarginBottom;
	if(Bounds.Bottom<r.Bounds.Bottom){
		Bounds.Bottom=r.Bounds.Bottom;
	}
	Bounds.Top=r.Bounds.Bottom;
	return r;
}
//---------------------------------------------------------------------------
cPixelLayout cPixelLayout::PlaceBottom(Float32 Height,Float32 MarginBottom,Float32 MarginTop)
{
	cPixelLayout r;
	r.ClearMargin();

	if(MarginBottom<ClientMargin.Bottom){
		MarginBottom=ClientMargin.Bottom;
	}
	r.Bounds.Bottom=Bounds.Bottom-MarginBottom;
	r.Bounds.Top=r.Bounds.Bottom-Height;
	r.Bounds.Left=Bounds.Left+ClientMargin.Left;
	r.Bounds.Right=Bounds.Right-ClientMargin.Right;

	ClientMargin.Bottom=MarginTop;
	if(Bounds.Top>r.Bounds.Top){
		Bounds.Top=r.Bounds.Top;
	}
	Bounds.Bottom=r.Bounds.Top;
	return r;
}
//---------------------------------------------------------------------------
cPixelLayout cPixelLayout::SliceLeft(Float32 Width,Float32 MarginLeft,Float32 MarginRight)
{
	cPixelLayout r;
	r.ClearMargin();

	if(MarginLeft<ClientMargin.Left){
		MarginLeft=ClientMargin.Left;
	}
	r.Bounds.Left=Bounds.Left+MarginLeft;
	r.Bounds.Right=r.Bounds.Left+Width;
	r.Bounds.Top=Bounds.Top+ClientMargin.Top;
	r.Bounds.Bottom=Bounds.Bottom-ClientMargin.Bottom;
	
	ClientMargin.Left=MarginRight;
	if(Bounds.Right<r.Bounds.Right){
		r.Bounds.Right=Bounds.Right;
	}
	Bounds.Left=r.Bounds.Right;
	return r;
}
//---------------------------------------------------------------------------
cPixelLayout cPixelLayout::SliceRight(Float32 Width,Float32 MarginRight,Float32 MarginLeft)
{
	cPixelLayout r;
	r.ClearMargin();

	if(MarginRight<ClientMargin.Right){
		MarginRight=ClientMargin.Right;
	}
	r.Bounds.Right=Bounds.Right-MarginRight;
	r.Bounds.Left=r.Bounds.Right-Width;
	r.Bounds.Top=Bounds.Top+ClientMargin.Top;
	r.Bounds.Bottom=Bounds.Bottom-ClientMargin.Bottom;

	ClientMargin.Right=MarginLeft;
	if(Bounds.Left>r.Bounds.Left){
		r.Bounds.Left=Bounds.Left;
	}
	Bounds.Right=r.Bounds.Left;
	return r;
}
//---------------------------------------------------------------------------
cPixelLayout cPixelLayout::SliceTop(Float32 Height,Float32 MarginTop,Float32 MarginBottom)
{
	cPixelLayout r;
	r.ClearMargin();

	if(MarginTop<ClientMargin.Top){
		MarginTop=ClientMargin.Top;
	}
	r.Bounds.Top=Bounds.Top+MarginTop;
	r.Bounds.Bottom=r.Bounds.Top+Height;
	r.Bounds.Left=Bounds.Left+ClientMargin.Left;
	r.Bounds.Right=Bounds.Right-ClientMargin.Right;

	ClientMargin.Top=MarginBottom;
	if(Bounds.Bottom<r.Bounds.Bottom){
		r.Bounds.Bottom=Bounds.Bottom;
	}
	Bounds.Top=r.Bounds.Bottom;
	return r;
}
//---------------------------------------------------------------------------
cPixelLayout cPixelLayout::SliceBottom(Float32 Height,Float32 MarginBottom,Float32 MarginTop)
{
	cPixelLayout r;
	r.ClearMargin();

	if(MarginBottom<ClientMargin.Bottom){
		MarginBottom=ClientMargin.Bottom;
	}
	r.Bounds.Bottom=Bounds.Bottom-MarginBottom;
	r.Bounds.Top=r.Bounds.Bottom-Height;
	r.Bounds.Left=Bounds.Left+ClientMargin.Left;
	r.Bounds.Right=Bounds.Right-ClientMargin.Right;

	ClientMargin.Bottom=MarginTop;
	if(Bounds.Top>r.Bounds.Top){
		r.Bounds.Top=Bounds.Top;
	}
	Bounds.Bottom=r.Bounds.Top;
	return r;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void cLayout::Reset(const cUIRect &LayoutRect)
{
	ParameterScaleVector.Fill(1);
	cPixelLayout::Reset(LayoutRect);
}
//---------------------------------------------------------------------------
void cLayout::Reset(const cUIPoint &Size)
{
	ParameterScaleVector.Fill(1);
	cPixelLayout::Reset(Size);
}
//---------------------------------------------------------------------------
void cLayout::Reset(const cUIPoint &Pos,const cUIPoint &Size)
{
	ParameterScaleVector.Fill(1);
	cPixelLayout::Reset(Pos,Size);
}
//---------------------------------------------------------------------------
void cLayout::Reset(Float32 ParameterScale,const cUIRect &LayoutRect)
{
	return Reset(ParameterScale,LayoutRect.Pos,LayoutRect.Size);
}
//---------------------------------------------------------------------------
void cLayout::Reset(Float32 ParameterScale,const cUIPoint &Size)
{
	ClearMargin();
	ParameterScaleVector.Fill(ParameterScale);

	BoundsVector[0]=0;
	BoundsVector[1]=0;
	BoundsVector[2]=Size.x;
	BoundsVector[3]=Size.y;

	BoundsVector*=ParameterScaleVector;
}
//---------------------------------------------------------------------------
void cLayout::Reset(Float32 ParameterScale,const cUIPoint &Pos,const cUIPoint &Size)
{
	ClearMargin();
	ParameterScaleVector.Fill(ParameterScale);

	BoundsVector[0]=Pos.x;
	BoundsVector[1]=Pos.y;
	BoundsVector[2]=Pos.x+Size.x;
	BoundsVector[3]=Pos.y+Size.y;

	BoundsVector*=ParameterScaleVector;
}
//---------------------------------------------------------------------------
void cLayout::SetMargin(Float32 MarginLeft,Float32 MarginTop,Float32 MarginRight,Float32 MarginBottom)
{
	
	ClientMarginVector[0]=MarginLeft;
	ClientMarginVector[1]=MarginTop;
	ClientMarginVector[2]=MarginRight;
	ClientMarginVector[3]=MarginBottom;
	if(ParameterNeedScale()){
		ClientMarginVector*=ParameterScaleVector;
	}
}
//---------------------------------------------------------------------------
void cLayout::Reset(iUIView *View)
{
	Float32 ContentScale=View->GetContentScale();
	auto ViewSize=View->GetSize();
	ClientMargin=View->GetFrameMargin();
	ParameterScaleVector.Fill(ContentScale);

	BoundsVector[0]=0;
	BoundsVector[1]=0;
	BoundsVector[2]=ViewSize.x;
	BoundsVector[3]=ViewSize.y;
}
//---------------------------------------------------------------------------
void cLayout::ResetClient(iUIView *View)
{
	Float32 ContentScale=View->GetContentScale();
	auto ViewSize=View->GetSize();
	auto Margin=View->GetFrameMargin();
	ClearMargin();

	ParameterScaleVector.Fill(ContentScale);
	Bounds.Left=Margin.Left;
	Bounds.Top=Margin.Top;
	Bounds.Right=ViewSize.x-Margin.Right;
	Bounds.Bottom=ViewSize.y-Margin.Bottom;
}
//---------------------------------------------------------------------------
bool cLayout::ParameterNeedScale(void)const
{
	return ParameterScaleVector[0]!=1.f;
}
//---------------------------------------------------------------------------
bool cLayout::ParameterNoScale(void)const
{
	return ParameterScaleVector[0]==1.f;
}
//---------------------------------------------------------------------------
void cLayout::MergeMargin(const cVector<Float32,4> &MarginVector)
{
	if(ParameterNoScale()){
		return cPixelLayout::MergeMargin(MarginVector);
	}
	cVector<Float32,4> sMarginVector=MarginVector*ParameterScaleVector;
	return cPixelLayout::MergeMargin(sMarginVector);
}
//---------------------------------------------------------------------------
void cLayout::MergeMargin(const cUIRectangle &Margin)
{
	cVector<Float32,4> sMarginVector=RectToVector(Margin);
	if(ParameterNeedScale()){
		sMarginVector*=ParameterScaleVector;
	}
	return cPixelLayout::MergeMargin(sMarginVector);
}
//---------------------------------------------------------------------------
void cLayout::MergeMargin(Float32 MarginLeft,Float32 MarginTop,Float32 MarginRight,Float32 MarginBottom)
{
	cVector<Float32,4> sMarginVector=RectToVector(MarginLeft,MarginTop,MarginRight,MarginBottom);
	if(ParameterNeedScale()){
		sMarginVector*=ParameterScaleVector;
	}
	return cPixelLayout::MergeMargin(sMarginVector);
}
//---------------------------------------------------------------------------
void cLayout::AddMargin(const cVector<Float32,4> &MarginVector)
{
	if(ParameterNoScale()){
		return cPixelLayout::AddMargin(MarginVector);
	}
	cVector<Float32,4> sMarginVector=MarginVector*ParameterScaleVector;
	return cPixelLayout::AddMargin(sMarginVector);
}
//---------------------------------------------------------------------------
void cLayout::AddMargin(const cUIRectangle &Margin)
{
	cVector<Float32,4> sMarginVector=RectToVector(Margin);
	if(ParameterNeedScale()){
		sMarginVector*=ParameterScaleVector;
	}
	return cPixelLayout::AddMargin(sMarginVector);
}
//---------------------------------------------------------------------------
void cLayout::AddMargin(Float32 MarginLeft,Float32 MarginTop,Float32 MarginRight,Float32 MarginBottom)
{
	cVector<Float32,4> sMarginVector=RectToVector(MarginLeft,MarginTop,MarginRight,MarginBottom);
	if(ParameterNeedScale()){
		sMarginVector*=ParameterScaleVector;
	}
	return cPixelLayout::AddMargin(sMarginVector);
}
//---------------------------------------------------------------------------
void cLayout::ParameterScale(Float32 Scale)
{
	ParameterScaleVector*=cnRTL::VectorFillValue(Scale);
}
//---------------------------------------------------------------------------
void cLayout::SetParameterScale(Float32 Scale)
{
	ParameterScaleVector=cnRTL::VectorFillValue(Scale);
}
//---------------------------------------------------------------------------
Float32 cLayout::GetClientWidth(Float32 MarginLeft,Float32 MarginRight)const
{
	if(ParameterNoScale()){
		return cPixelLayout::GetClientWidth(MarginLeft,MarginRight);
	}

	Float32 Scale=ParameterScaleVector[0];
	Float32 w=cPixelLayout::GetClientWidth(MarginLeft*Scale,MarginRight*Scale);
	return w/Scale;
}
//---------------------------------------------------------------------------
Float32 cLayout::GetClientHeight(Float32 MarginTop,Float32 MarginBottom)const
{
	if(ParameterNoScale()){
		return cPixelLayout::GetClientHeight(MarginTop,MarginBottom);
	}

	Float32 Scale=ParameterScaleVector[0];
	Float32 w=cPixelLayout::GetClientHeight(MarginTop*Scale,MarginBottom*Scale);
	return w/Scale;
}
//---------------------------------------------------------------------------
cLayout cLayout::Union(const cLayout &Src)const
{
	cLayout r;
	if(Src.ParameterScaleVector[0]==ParameterScaleVector[0]){
		// no scale
		static_cast<cPixelLayout&>(r)=cPixelLayout::Union(Src);
	}
	else{
		Float32 Scale=ParameterScaleVector[0]/Src.ParameterScaleVector[0];
		cVector<Float32,4> vScale;
		vScale.Fill(Scale);
		cPixelLayout PixelSrc;
		PixelSrc.BoundsVector*=vScale;
		PixelSrc.ClientMarginVector*=vScale;
		static_cast<cPixelLayout&>(r)=cPixelLayout::Union(PixelSrc);
	}
	r.ParameterScaleVector=ParameterScaleVector;
	return r;
}
//---------------------------------------------------------------------------
cLayout cLayout::Padding(Float32 pLeft,Float32 pTop,Float32 pRight,Float32 pBottom)const
{
	cLayout r;
	if(ParameterNoScale()){
		static_cast<cPixelLayout&>(r)=cPixelLayout::Padding(pLeft,pTop,pRight,pBottom);
	}
	else{
		Float32 s=ParameterScaleVector[0];
		static_cast<cPixelLayout&>(r)=cPixelLayout::Padding(pLeft*s,pTop*s,pRight*s,pBottom*s);
	}
	r.ParameterScaleVector=ParameterScaleVector;
	return r;
}
//---------------------------------------------------------------------------
cLayout cLayout::Client(void)const
{
	cLayout r;
	static_cast<cPixelLayout&>(r)=cPixelLayout::Client();
	r.ParameterScaleVector=ParameterScaleVector;
	return r;
}
//---------------------------------------------------------------------------
cLayout cLayout::Client(Float32 MarginLeft,Float32 MarginTop,Float32 MarginRight,Float32 MarginBottom)const
{
	cLayout r;
	if(ParameterNoScale()){
		static_cast<cPixelLayout&>(r)=cPixelLayout::Client(MarginLeft,MarginTop,MarginRight,MarginBottom);
	}
	else{
		Float32 s=ParameterScaleVector[0];
		static_cast<cPixelLayout&>(r)=cPixelLayout::Client(MarginLeft*s,MarginTop*s,MarginRight*s,MarginBottom*s);
	}
	r.ParameterScaleVector=ParameterScaleVector;
	return r;
}
//---------------------------------------------------------------------------
cLayout cLayout::Client(const cUIRectangle &Margin)const
{
	return Client(Margin.Left,Margin.Top,Margin.Right,Margin.Bottom);
}
//---------------------------------------------------------------------------
cLayout cLayout::Align(eAlignment Alignment,Float32 Width,Float32 Height)const
{
	cLayout r;
	if(ParameterNoScale()){
		static_cast<cPixelLayout&>(r)=cPixelLayout::Align(Alignment,Width,Height);
	}
	else{
		Float32 s=ParameterScaleVector[0];
		static_cast<cPixelLayout&>(r)=cPixelLayout::Align(Alignment,Width*s,Height*s);
	}
	r.ParameterScaleVector=ParameterScaleVector;
	return r;
}
//---------------------------------------------------------------------------
Float32 cLayout::DivideWidth(ufInt32n Count,Float32 PartMargin,Float32 MarginLeft,Float32 MarginRight)const
{
	if(ParameterNoScale()){
		return cPixelLayout::DivideWidth(Count,PartMargin,MarginLeft,MarginRight);
	}

	Float32 Scale=ParameterScaleVector[0];
	Float32 w=cPixelLayout::DivideWidth(Count,PartMargin*Scale,MarginLeft*Scale,MarginRight*Scale);
	return w/Scale;
}
//---------------------------------------------------------------------------
Float32 cLayout::DivideHeight(ufInt32n Count,Float32 PartMargin,Float32 MarginTop,Float32 MarginBottom)const
{
	if(ParameterNoScale()){
		return cPixelLayout::DivideHeight(Count,PartMargin,MarginTop,MarginBottom);
	}

	Float32 Scale=ParameterScaleVector[0];
	Float32 h=cPixelLayout::DivideHeight(Count,PartMargin*Scale,MarginTop*Scale,MarginBottom*Scale);
	return h/Scale;
}
//---------------------------------------------------------------------------
cLayout cLayout::LayoutLeft(Float32 Width,Float32 MarginLeft,Float32 MarginRight)
{
	cLayout r;
	if(ParameterNoScale()){
		static_cast<cPixelLayout&>(r)=cPixelLayout::LayoutLeft(Width,MarginLeft,MarginRight);
	}
	else{
		Float32 s=ParameterScaleVector[0];
		static_cast<cPixelLayout&>(r)=cPixelLayout::LayoutLeft(Width*s,MarginLeft*s,MarginRight*s);
	}
	r.ParameterScaleVector=ParameterScaleVector;
	return r;
}
//---------------------------------------------------------------------------
cLayout cLayout::LayoutRight(Float32 Width,Float32 MarginRight,Float32 MarginLeft)
{
	cLayout r;
	if(ParameterNoScale()){
		static_cast<cPixelLayout&>(r)=cPixelLayout::LayoutRight(Width,MarginRight,MarginLeft);
	}
	else{
		Float32 s=ParameterScaleVector[0];
		static_cast<cPixelLayout&>(r)=cPixelLayout::LayoutRight(Width*s,MarginRight*s,MarginLeft*s);
	}
	r.ParameterScaleVector=ParameterScaleVector;
	return r;
}
//---------------------------------------------------------------------------
cLayout cLayout::LayoutTop(Float32 Height,Float32 MarginTop,Float32 MarginBottom)
{
	cLayout r;
	if(ParameterNoScale()){
		static_cast<cPixelLayout&>(r)=cPixelLayout::LayoutTop(Height,MarginTop,MarginBottom);
	}
	else{
		Float32 s=ParameterScaleVector[0];
		static_cast<cPixelLayout&>(r)=cPixelLayout::LayoutTop(Height*s,MarginTop*s,MarginBottom*s);
	}
	r.ParameterScaleVector=ParameterScaleVector;
	return r;
}
//---------------------------------------------------------------------------
cLayout cLayout::LayoutBottom(Float32 Height,Float32 MarginBottom,Float32 MarginTop)
{
	cLayout r;
	if(ParameterNoScale()){
		static_cast<cPixelLayout&>(r)=cPixelLayout::LayoutBottom(Height,MarginBottom,MarginTop);
	}
	else{
		Float32 s=ParameterScaleVector[0];
		static_cast<cPixelLayout&>(r)=cPixelLayout::LayoutBottom(Height*s,MarginBottom*s,MarginTop*s);
	}
	r.ParameterScaleVector=ParameterScaleVector;
	return r;
}
//---------------------------------------------------------------------------
cLayout cLayout::PlaceLeft(Float32 Width,Float32 MarginLeft,Float32 MarginRight)
{
	cLayout r;
	if(ParameterNoScale()){
		static_cast<cPixelLayout&>(r)=cPixelLayout::PlaceLeft(Width,MarginLeft,MarginRight);
	}
	else{
		Float32 s=ParameterScaleVector[0];
		static_cast<cPixelLayout&>(r)=cPixelLayout::PlaceLeft(Width*s,MarginLeft*s,MarginRight*s);
	}
	r.ParameterScaleVector=ParameterScaleVector;
	return r;
}
//---------------------------------------------------------------------------
cLayout cLayout::PlaceRight(Float32 Width,Float32 MarginRight,Float32 MarginLeft)
{
	cLayout r;
	if(ParameterNoScale()){
		static_cast<cPixelLayout&>(r)=cPixelLayout::PlaceRight(Width,MarginRight,MarginLeft);
	}
	else{
		Float32 s=ParameterScaleVector[0];
		static_cast<cPixelLayout&>(r)=cPixelLayout::PlaceRight(Width*s,MarginRight*s,MarginLeft*s);
	}
	r.ParameterScaleVector=ParameterScaleVector;
	return r;
}
//---------------------------------------------------------------------------
cLayout cLayout::PlaceTop(Float32 Height,Float32 MarginTop,Float32 MarginBottom)
{
	cLayout r;
	if(ParameterNoScale()){
		static_cast<cPixelLayout&>(r)=cPixelLayout::PlaceTop(Height,MarginTop,MarginBottom);
	}
	else{
		Float32 s=ParameterScaleVector[0];
		static_cast<cPixelLayout&>(r)=cPixelLayout::PlaceTop(Height*s,MarginTop*s,MarginBottom*s);
	}
	r.ParameterScaleVector=ParameterScaleVector;
	return r;
}
//---------------------------------------------------------------------------
cLayout cLayout::PlaceBottom(Float32 Height,Float32 MarginBottom,Float32 MarginTop)
{
	cLayout r;
	if(ParameterNoScale()){
		static_cast<cPixelLayout&>(r)=cPixelLayout::PlaceBottom(Height,MarginBottom,MarginTop);
	}
	else{
		Float32 s=ParameterScaleVector[0];
		static_cast<cPixelLayout&>(r)=cPixelLayout::PlaceBottom(Height*s,MarginBottom*s,MarginTop*s);
	}
	r.ParameterScaleVector=ParameterScaleVector;
	return r;
}
//---------------------------------------------------------------------------
cLayout cLayout::SliceLeft(Float32 Width,Float32 MarginLeft,Float32 MarginRight)
{
	cLayout r;
	if(ParameterNoScale()){
		static_cast<cPixelLayout&>(r)=cPixelLayout::SliceLeft(Width,MarginLeft,MarginRight);
	}
	else{
		Float32 s=ParameterScaleVector[0];
		static_cast<cPixelLayout&>(r)=cPixelLayout::SliceLeft(Width*s,MarginLeft*s,MarginRight*s);
	}
	r.ParameterScaleVector=ParameterScaleVector;
	return r;
}
//---------------------------------------------------------------------------
cLayout cLayout::SliceRight(Float32 Width,Float32 MarginRight,Float32 MarginLeft)
{
	cLayout r;
	if(ParameterNoScale()){
		static_cast<cPixelLayout&>(r)=cPixelLayout::SliceRight(Width,MarginRight,MarginLeft);
	}
	else{
		Float32 s=ParameterScaleVector[0];
		static_cast<cPixelLayout&>(r)=cPixelLayout::SliceRight(Width*s,MarginRight*s,MarginLeft*s);
	}
	r.ParameterScaleVector=ParameterScaleVector;
	return r;
}
//---------------------------------------------------------------------------
cLayout cLayout::SliceTop(Float32 Height,Float32 MarginTop,Float32 MarginBottom)
{
	cLayout r;
	if(ParameterNoScale()){
		static_cast<cPixelLayout&>(r)=cPixelLayout::SliceTop(Height,MarginTop,MarginBottom);
	}
	else{
		Float32 s=ParameterScaleVector[0];
		static_cast<cPixelLayout&>(r)=cPixelLayout::SliceTop(Height*s,MarginTop*s,MarginBottom*s);
	}
	r.ParameterScaleVector=ParameterScaleVector;
	return r;
}
//---------------------------------------------------------------------------
cLayout cLayout::SliceBottom(Float32 Height,Float32 MarginBottom,Float32 MarginTop)
{
	cLayout r;
	if(ParameterNoScale()){
		static_cast<cPixelLayout&>(r)=cPixelLayout::SliceBottom(Height,MarginBottom,MarginTop);
	}
	else{
		Float32 s=ParameterScaleVector[0];
		static_cast<cPixelLayout&>(r)=cPixelLayout::SliceBottom(Height*s,MarginBottom*s,MarginTop*s);
	}
	r.ParameterScaleVector=ParameterScaleVector;
	return r;
}
//---------------------------------------------------------------------------
