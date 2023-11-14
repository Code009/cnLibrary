/*- cnUI - UI Layout ------------------------------------------------------*/
/*         Developer : Code009                                             */
/*         Create on : 2021-04-08                                          */
/*-------------------------------------------------------------------------*/
#ifndef __cnLibrary_cnUI_UILayout_H__
#define __cnLibrary_cnUI_UILayout_H__

#include <cnUI/UICommon.h>

#ifdef __cplusplus
//---------------------------------------------------------------------------
namespace cnLibrary{
//---------------------------------------------------------------------------
namespace cnUI{
//---------------------------------------------------------------------------
//cnLib_INTENUM_BEGIN(ufInt8,Direction)
//{
//	Up				=0,
//	Down			=1,
//	Left			=2,
//	Right			=3,
//}cnLib_INTENUM_END(Direction);
//---------------------------------------------------------------------------
cnLib_INTENUM_BEGIN(ufInt8,Alignment)
{
	None		=0,
	Left		=1,
	Top			=2,
	TopLeft		=3,
	Right		=4,
	HCenter		=5,
	TopRight	=6,
	TopCenter	=7,
	Bottom		=8,
	BottomLeft	=9,
	VCenter		=10,
	CenterLeft	=11,
	BottomRight	=12,
	BottomCenter=13,
	CenterRight	=14,
	Center		=15,
}cnLib_INTENUM_END(Alignment);
//---------------------------------------------------------------------------
// RectAlignInRect
//	align a rect inside another rect
// Align		alignment
// DestPos		position of rectangle to align in
// DestSize		size of rectangle to align in
// AlignSize	size of rectangle to align
// DefaultPos	unaligned position
// return aligned position
cUIPoint RectAlignInRect(eAlignment Align,Float32 DestLeft,Float32 DestTop,Float32 DestRight,Float32 DestBottom,cUIPoint AlignSize,cUIPoint DefaultPos=UIPointZero);
// RectAlignInRect
//	align a rect inside another rect
// Align		alignment
// DestSize		size of rectangle to align in
// AlignSize	size of rectangle to align
// DefaultPos	position for unaligned rectangle
// return aligned position
cUIPoint RectAlignInRect(eAlignment Align,cUIPoint DestSize,cUIPoint AlignSize,cUIPoint DefaultPos=UIPointZero);

// AlignInnerRect
//	align a rect inside another rect
// Align		alignment
// DestSize		size of rectangle to align in
// AlignSize	size of rectangle to align
// DefaultPos	position for unaligned rectangle
// return aligned position
cUIPoint RectStretchAlignInRect(eAlignment Align,cUIPoint DestSize,cUIPoint &AlignSize);
//---------------------------------------------------------------------------
cnLib_INTENUM_BEGIN(ufInt8,AroundAlignment)
{
	Vertical		=0,

	Top				=0,
	VTopLeft		=1,
	VTopRight		=2,
	VTopCenter		=3,

	Bottom			=4,
	VBottomLeft		=5,
	VBottomRight	=6,
	VBottomCenter	=7,

	Horizontal		=8,

	Left			=8,
	HTopLeft		=9,
	HBottomLeft		=10,
	HCenterLeft		=11,

	Right			=12,
	HTopRight		=13,
	HBottomRight	=14,
	HCenterRight	=15,

	MaskAxis		=0x8,
	MaskDirection	=0xC,
	MaskSide		=0x3,

	SideNone		=0x0,
	SideLowwer		=0x1,
	SideUpper		=0x2,
	SideMiddle		=0x3,
}cnLib_INTENUM_END(AroundAlignment);
// RectAlignAroundRect
// Align		alignment
// DestPos		position of rectangle to align
// DestSize		size of rectangle to align
// return aligned position
cUIPoint RectAlignAroundRect(eAroundAlignment Align,cUIPoint DestPos,cUIPoint DestSize,cUIPoint SrcSize,cUIPoint DefaultPos={0,0});
cUIPoint RectAlignAroundRectLimited(eAroundAlignment Align,cUIPoint LimitSize,cUIPoint DestPos,cUIPoint DestSize,cUIPoint SrcSize,cUIPoint DefaultPos={0,0});

//---------------------------------------------------------------------------
void RectApplyMargin(cUIRect &Rect,const cUIRectangle &Margin);
cUIRectangle MergeMargin(cUIRectangle Margin1,const cUIRectangle &Margin2);
//---------------------------------------------------------------------------
cVector<Float32,4> RectToVector(Float32 Left,Float32 Top,Float32 Right,Float32 Bottom);
cVector<Float32,4> RectToVector(const cUIRectangle &Rect);
//---------------------------------------------------------------------------
struct cPixelLayout
{
	union{
		cVector<Float32,4> BoundsVector;
		cUIRectangle Bounds;
	};
	union{
		cVector<Float32,4> ClientMarginVector;
		cUIRectangle ClientMargin;
	};

	void ClearMargin(void);
	void Reset(const cUIRect &LayoutRect);
	void Reset(const cUIPoint &Size);
	void Reset(const cUIPoint &Pos,const cUIPoint &Size);
	void SetMargin(Float32 MarginLeft,Float32 MarginTop,Float32 MarginRight,Float32 MarginBottom);

	void MergeMargin(const cVector<Float32,4> &MarginVector);
	void MergeMargin(const cUIRectangle &Margin);
	void MergeMargin(Float32 MarginLeft,Float32 MarginTop,Float32 MarginRight,Float32 MarginBottom);

	void AddMargin(const cVector<Float32,4> &MarginVector);
	void AddMargin(const cUIRectangle &Margin);
	void AddMargin(Float32 MarginLeft,Float32 MarginTop,Float32 MarginRight,Float32 MarginBottom);

	operator cUIRect ()const;

	Float32 GetClientWidth(Float32 MarginLeft=0,Float32 MarginRight=0)const;
	Float32 GetClientHeight(Float32 MarginTop=0,Float32 MarginBottom=0)const;

	cPixelLayout Union(const cPixelLayout &Src)const;
	cPixelLayout Padding(const cVector<Float32,4> &PaddingVector);
	cPixelLayout Padding(Float32 pLeft,Float32 pTop,Float32 pRight,Float32 pBottom)const;
	cPixelLayout Client(void)const;
	cPixelLayout Client(Float32 MarginLeft,Float32 MarginTop,Float32 MarginRight,Float32 MarginBottom)const;
	cPixelLayout Client(const cUIRectangle &Margin)const;
	cPixelLayout Align(eAlignment Alignment,Float32 Width,Float32 Height)const;

	Float32 DivideWidth(ufInt32n Count,Float32 PartMargin=0,Float32 MarginLeft=0,Float32 MarginRight=0)const;
	Float32 DivideHeight(ufInt32n Count,Float32 PartMargin=0,Float32 MarginTop=0,Float32 MarginBottom=0)const;

	cPixelLayout LayoutLeft(Float32 Width,Float32 MarginLeft=0,Float32 MarginRight=0);
	cPixelLayout LayoutRight(Float32 Width,Float32 MarginRight=0,Float32 MarginLeft=0);
	cPixelLayout LayoutTop(Float32 Height,Float32 MarginTop=0,Float32 MarginBottom=0);
	cPixelLayout LayoutBottom(Float32 Height,Float32 MarginBottom=0,Float32 MarginTop=0);

	cPixelLayout PlaceLeft(Float32 Width,Float32 MarginLeft=0,Float32 MarginRight=0);
	cPixelLayout PlaceRight(Float32 Width,Float32 MarginRight=0,Float32 MarginLeft=0);
	cPixelLayout PlaceTop(Float32 Height,Float32 MarginTop=0,Float32 MarginBottom=0);
	cPixelLayout PlaceBottom(Float32 Height,Float32 MarginBottom=0,Float32 MarginTop=0);

	cPixelLayout SliceLeft(Float32 Width,Float32 MarginLeft=0,Float32 MarginRight=0);
	cPixelLayout SliceRight(Float32 Width,Float32 MarginRight=0,Float32 MarginLeft=0);
	cPixelLayout SliceTop(Float32 Height,Float32 MarginTop=0,Float32 MarginBottom=0);
	cPixelLayout SliceBottom(Float32 Height,Float32 MarginBottom=0,Float32 MarginTop=0);

};
//---------------------------------------------------------------------------
struct cLayout : cPixelLayout
{
	cVector<Float32,4> ParameterScaleVector;

	bool ParameterNeedScale(void)const;
	bool ParameterNoScale(void)const;

	void Reset(const cUIRect &LayoutRect);
	void Reset(const cUIPoint &Size);
	void Reset(const cUIPoint &Pos,const cUIPoint &Size);
	void Reset(Float32 ParameterScale,const cUIRect &LayoutRect);
	void Reset(Float32 ParameterScale,const cUIPoint &Size);
	void Reset(Float32 ParameterScale,const cUIPoint &Pos,const cUIPoint &Size);
	void SetMargin(Float32 MarginLeft,Float32 MarginTop,Float32 MarginRight,Float32 MarginBottom);

	void Reset(iUIView *View);
	void ResetClient(iUIView *View);

	void ParameterScale(Float32 Scale);
	void SetParameterScale(Float32 Scale);

	void MergeMargin(const cVector<Float32,4> &MarginVector);
	void MergeMargin(const cUIRectangle &Margin);
	void MergeMargin(Float32 MarginLeft,Float32 MarginTop,Float32 MarginRight,Float32 MarginBottom);

	void AddMargin(const cVector<Float32,4> &MarginVector);
	void AddMargin(const cUIRectangle &Margin);
	void AddMargin(Float32 MarginLeft,Float32 MarginTop,Float32 MarginRight,Float32 MarginBottom);


	//cUIRect UnScaleRect(void)const;

	Float32 GetClientWidth(Float32 MarginLeft=0,Float32 MarginRight=0)const;
	Float32 GetClientHeight(Float32 MarginTop=0,Float32 MarginBottom=0)const;

	cLayout Union(const cLayout &Src)const;
	cLayout Padding(Float32 pLeft,Float32 pTop,Float32 pRight,Float32 pBottom)const;
	cLayout Client(void)const;
	cLayout Client(Float32 MarginLeft,Float32 MarginTop,Float32 MarginRight,Float32 MarginBottom)const;
	cLayout Client(const cUIRectangle &Margin)const;
	cLayout Align(eAlignment Alignment,Float32 Width,Float32 Height)const;

	Float32 DivideWidth(ufInt32n Count,Float32 PartMargin=0,Float32 MarginLeft=0,Float32 MarginRight=0)const;
	Float32 DivideHeight(ufInt32n Count,Float32 PartMargin=0,Float32 MarginTop=0,Float32 MarginBottom=0)const;

	cLayout LayoutLeft(Float32 Width,Float32 MarginLeft=0,Float32 MarginRight=0);
	cLayout LayoutRight(Float32 Width,Float32 MarginRight=0,Float32 MarginLeft=0);
	cLayout LayoutTop(Float32 Height,Float32 MarginTop=0,Float32 MarginBottom=0);
	cLayout LayoutBottom(Float32 Height,Float32 MarginBottom=0,Float32 MarginTop=0);

	cLayout PlaceLeft(Float32 Width,Float32 MarginLeft=0,Float32 MarginRight=0);
	cLayout PlaceRight(Float32 Width,Float32 MarginRight=0,Float32 MarginLeft=0);
	cLayout PlaceTop(Float32 Height,Float32 MarginTop=0,Float32 MarginBottom=0);
	cLayout PlaceBottom(Float32 Height,Float32 MarginBottom=0,Float32 MarginTop=0);

	cLayout SliceLeft(Float32 Width,Float32 MarginLeft=0,Float32 MarginRight=0);
	cLayout SliceRight(Float32 Width,Float32 MarginRight=0,Float32 MarginLeft=0);
	cLayout SliceTop(Float32 Height,Float32 MarginTop=0,Float32 MarginBottom=0);
	cLayout SliceBottom(Float32 Height,Float32 MarginBottom=0,Float32 MarginTop=0);

};
//---------------------------------------------------------------------------
}//	namespace cnUI
//---------------------------------------------------------------------------
}	// namespace cnLibrary
//---------------------------------------------------------------------------
#endif  /* __cplusplus */
/*-------------------------------------------------------------------------*/
#endif
