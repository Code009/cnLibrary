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
cUIPoint RectAlignInRect(eAlignment Align,Float32 DestLeft,Float32 DestTop,Float32 DestRight,Float32 DestBottom,cUIPoint AlignSize,cUIPoint DefaultPos=UIPointZero)noexcept(true);
// RectAlignInRect
//	align a rect inside another rect
// Align		alignment
// DestSize		size of rectangle to align in
// AlignSize	size of rectangle to align
// DefaultPos	position for unaligned rectangle
// return aligned position
cUIPoint RectAlignInRect(eAlignment Align,cUIPoint DestSize,cUIPoint AlignSize,cUIPoint DefaultPos=UIPointZero)noexcept(true);

// AlignInnerRect
//	align a rect inside another rect
// Align		alignment
// DestSize		size of rectangle to align in
// AlignSize	size of rectangle to align
// DefaultPos	position for unaligned rectangle
// return aligned position
cUIPoint RectStretchAlignInRect(eAlignment Align,cUIPoint DestSize,cUIPoint &AlignSize)noexcept(true);
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
cUIPoint RectAlignAroundRect(eAroundAlignment Align,cUIPoint DestPos,cUIPoint DestSize,cUIPoint SrcSize,cUIPoint DefaultPos={0,0})noexcept(true);
cUIPoint RectAlignAroundRectLimited(eAroundAlignment Align,cUIPoint LimitSize,cUIPoint DestPos,cUIPoint DestSize,cUIPoint SrcSize,cUIPoint DefaultPos={0,0})noexcept(true);

//---------------------------------------------------------------------------
void RectApplyMargin(cUIRect &Rect,const cUIRectangle &Margin)noexcept(true);
cUIRectangle MergeMargin(cUIRectangle Margin1,const cUIRectangle &Margin2)noexcept(true);
//---------------------------------------------------------------------------
cVector<Float32,4> RectToVector(Float32 Left,Float32 Top,Float32 Right,Float32 Bottom)noexcept(true);
cVector<Float32,4> RectToVector(const cUIRectangle &Rect)noexcept(true);
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

	cPixelLayout()noexcept(true);
	cPixelLayout(const cPixelLayout &Src)noexcept(true);

	void ClearMargin(void)noexcept(true);
	void Reset(const cUIRect &LayoutRect)noexcept(true);
	void Reset(const cUIPoint &Size)noexcept(true);
	void Reset(const cUIPoint &Pos,const cUIPoint &Size)noexcept(true);
	void SetMargin(Float32 MarginLeft,Float32 MarginTop,Float32 MarginRight,Float32 MarginBottom)noexcept(true);

	void MergeMargin(const cVector<Float32,4> &MarginVector)noexcept(true);
	void MergeMargin(const cUIRectangle &Margin)noexcept(true);
	void MergeMargin(Float32 MarginLeft,Float32 MarginTop,Float32 MarginRight,Float32 MarginBottom)noexcept(true);

	void AddMargin(const cVector<Float32,4> &MarginVector)noexcept(true);
	void AddMargin(const cUIRectangle &Margin)noexcept(true);
	void AddMargin(Float32 MarginLeft,Float32 MarginTop,Float32 MarginRight,Float32 MarginBottom)noexcept(true);

	operator cUIRect ()const noexcept(true);

	Float32 GetClientWidth(Float32 MarginLeft=0,Float32 MarginRight=0)const noexcept(true);
	Float32 GetClientHeight(Float32 MarginTop=0,Float32 MarginBottom=0)const noexcept(true);

	cPixelLayout Union(const cPixelLayout &Src)const noexcept(true);
	cPixelLayout Padding(const cVector<Float32,4> &PaddingVector)noexcept(true);
	cPixelLayout Padding(Float32 pLeft,Float32 pTop,Float32 pRight,Float32 pBottom)const noexcept(true);
	cPixelLayout Client(void)const noexcept(true);
	cPixelLayout Client(Float32 MarginLeft,Float32 MarginTop,Float32 MarginRight,Float32 MarginBottom)const noexcept(true);
	cPixelLayout Client(const cUIRectangle &Margin)const noexcept(true);
	cPixelLayout Align(eAlignment Alignment,Float32 Width,Float32 Height)const noexcept(true);

	Float32 DivideWidth(ufInt32 Count,Float32 PartMargin=0,Float32 MarginLeft=0,Float32 MarginRight=0)const noexcept(true);
	Float32 DivideHeight(ufInt32 Count,Float32 PartMargin=0,Float32 MarginTop=0,Float32 MarginBottom=0)const noexcept(true);

	cPixelLayout LayoutLeft(Float32 Width,Float32 MarginLeft=0,Float32 MarginRight=0)noexcept(true);
	cPixelLayout LayoutRight(Float32 Width,Float32 MarginRight=0,Float32 MarginLeft=0)noexcept(true);
	cPixelLayout LayoutTop(Float32 Height,Float32 MarginTop=0,Float32 MarginBottom=0)noexcept(true);
	cPixelLayout LayoutBottom(Float32 Height,Float32 MarginBottom=0,Float32 MarginTop=0)noexcept(true);

	cPixelLayout PlaceLeft(Float32 Width,Float32 MarginLeft=0,Float32 MarginRight=0)noexcept(true);
	cPixelLayout PlaceRight(Float32 Width,Float32 MarginRight=0,Float32 MarginLeft=0)noexcept(true);
	cPixelLayout PlaceTop(Float32 Height,Float32 MarginTop=0,Float32 MarginBottom=0)noexcept(true);
	cPixelLayout PlaceBottom(Float32 Height,Float32 MarginBottom=0,Float32 MarginTop=0)noexcept(true);

	cPixelLayout SliceLeft(Float32 Width,Float32 MarginLeft=0,Float32 MarginRight=0)noexcept(true);
	cPixelLayout SliceRight(Float32 Width,Float32 MarginRight=0,Float32 MarginLeft=0)noexcept(true);
	cPixelLayout SliceTop(Float32 Height,Float32 MarginTop=0,Float32 MarginBottom=0)noexcept(true);
	cPixelLayout SliceBottom(Float32 Height,Float32 MarginBottom=0,Float32 MarginTop=0)noexcept(true);

};
//---------------------------------------------------------------------------
struct cLayout : cPixelLayout
{
	cVector<Float32,4> ParameterScaleVector;
	cLayout()noexcept(true);
	cLayout(const cLayout &Src)noexcept(true);

	bool ParameterNeedScale(void)const noexcept(true);
	bool ParameterNoScale(void)const noexcept(true);

	void Reset(const cUIRect &LayoutRect)noexcept(true);
	void Reset(const cUIPoint &Size)noexcept(true);
	void Reset(const cUIPoint &Pos,const cUIPoint &Size)noexcept(true);
	void Reset(Float32 ParameterScale,const cUIRect &LayoutRect)noexcept(true);
	void Reset(Float32 ParameterScale,const cUIPoint &Size)noexcept(true);
	void Reset(Float32 ParameterScale,const cUIPoint &Pos,const cUIPoint &Size)noexcept(true);
	void SetMargin(Float32 MarginLeft,Float32 MarginTop,Float32 MarginRight,Float32 MarginBottom)noexcept(true);

	void Reset(iUIView *View)noexcept(true);
	void ResetClient(iUIView *View)noexcept(true);

	void ParameterScale(Float32 Scale)noexcept(true);
	void SetParameterScale(Float32 Scale)noexcept(true);

	void MergeMargin(const cVector<Float32,4> &MarginVector)noexcept(true);
	void MergeMargin(const cUIRectangle &Margin)noexcept(true);
	void MergeMargin(Float32 MarginLeft,Float32 MarginTop,Float32 MarginRight,Float32 MarginBottom)noexcept(true);

	void AddMargin(const cVector<Float32,4> &MarginVector)noexcept(true);
	void AddMargin(const cUIRectangle &Margin)noexcept(true);
	void AddMargin(Float32 MarginLeft,Float32 MarginTop,Float32 MarginRight,Float32 MarginBottom)noexcept(true);


	//cUIRect UnScaleRect(void)const;

	Float32 GetClientWidth(Float32 MarginLeft=0,Float32 MarginRight=0)const noexcept(true);
	Float32 GetClientHeight(Float32 MarginTop=0,Float32 MarginBottom=0)const noexcept(true);

	cLayout Union(const cLayout &Src)const noexcept(true);
	cLayout Padding(Float32 pLeft,Float32 pTop,Float32 pRight,Float32 pBottom)const noexcept(true);
	cLayout Client(void)const noexcept(true);
	cLayout Client(Float32 MarginLeft,Float32 MarginTop,Float32 MarginRight,Float32 MarginBottom)const noexcept(true);
	cLayout Client(const cUIRectangle &Margin)const noexcept(true);
	cLayout Align(eAlignment Alignment,Float32 Width,Float32 Height)const noexcept(true);

	Float32 DivideWidth(ufInt32 Count,Float32 PartMargin=0,Float32 MarginLeft=0,Float32 MarginRight=0)const noexcept(true);
	Float32 DivideHeight(ufInt32 Count,Float32 PartMargin=0,Float32 MarginTop=0,Float32 MarginBottom=0)const noexcept(true);

	cLayout LayoutLeft(Float32 Width,Float32 MarginLeft=0,Float32 MarginRight=0)noexcept(true);
	cLayout LayoutRight(Float32 Width,Float32 MarginRight=0,Float32 MarginLeft=0)noexcept(true);
	cLayout LayoutTop(Float32 Height,Float32 MarginTop=0,Float32 MarginBottom=0)noexcept(true);
	cLayout LayoutBottom(Float32 Height,Float32 MarginBottom=0,Float32 MarginTop=0)noexcept(true);

	cLayout PlaceLeft(Float32 Width,Float32 MarginLeft=0,Float32 MarginRight=0)noexcept(true);
	cLayout PlaceRight(Float32 Width,Float32 MarginRight=0,Float32 MarginLeft=0)noexcept(true);
	cLayout PlaceTop(Float32 Height,Float32 MarginTop=0,Float32 MarginBottom=0)noexcept(true);
	cLayout PlaceBottom(Float32 Height,Float32 MarginBottom=0,Float32 MarginTop=0)noexcept(true);

	cLayout SliceLeft(Float32 Width,Float32 MarginLeft=0,Float32 MarginRight=0)noexcept(true);
	cLayout SliceRight(Float32 Width,Float32 MarginRight=0,Float32 MarginLeft=0)noexcept(true);
	cLayout SliceTop(Float32 Height,Float32 MarginTop=0,Float32 MarginBottom=0)noexcept(true);
	cLayout SliceBottom(Float32 Height,Float32 MarginBottom=0,Float32 MarginTop=0)noexcept(true);

};
//---------------------------------------------------------------------------
}//	namespace cnUI
//---------------------------------------------------------------------------
}	// namespace cnLibrary
//---------------------------------------------------------------------------
#endif  /* __cplusplus */
/*-------------------------------------------------------------------------*/
#endif
