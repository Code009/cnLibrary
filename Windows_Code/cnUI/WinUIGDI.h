/*- cnWinUI - GDI ---------------------------------------------------------*/
/*         Developer : Code009                                             */
/*         Create on : 2021-04-10                                          */
/*-------------------------------------------------------------------------*/
#pragma once

#include <cnUI\WinUICommon.h>

//---------------------------------------------------------------------------
namespace cnLibrary{
//---------------------------------------------------------------------------
namespace cnUI{
//---------------------------------------------------------------------------
class vWinDCPainter : protected iDCPainter
{
public:
	vWinDCPainter()noexcept(true);
	~vWinDCPainter()noexcept(true);

	void SetView(iUIView *View)noexcept(true);
	void SetContentZPosition(Float32 ZPosition)noexcept(true);
	void SetContentVisible(bool Visible)noexcept(true);
	cUIRectangle GetFrameMargin(void)noexcept(true);
protected:
	rPtr<iDCViewContent> fDCViewContent;
	RECT fPaintRC;

	virtual void cnLib_FUNC PaintStarted(void)noexcept(true)override{}
	virtual void cnLib_FUNC PaintShow(void)noexcept(true)override{}
	virtual void cnLib_FUNC PaintResume(void)noexcept(true)override{}
	virtual void cnLib_FUNC PaintPaused(void)noexcept(true)override{}
	virtual void cnLib_FUNC PaintHide(void)noexcept(true)override{}
	virtual void cnLib_FUNC PaintStopped(void)noexcept(true)override{}
	virtual void cnLib_FUNC PaintRectChanged(void)noexcept(true)override;
	virtual void cnLib_FUNC PaintWindowChanged(void)noexcept(true)override{}
};
//---------------------------------------------------------------------------
template<int Width,COLORREF Color>
struct cSolidPenObject
{
	cSolidPenObject()noexcept(true){
		Pen=::CreatePen(PS_SOLID,Width,Color);
	}
	~cSolidPenObject()noexcept(true){
		::DeleteObject(Pen);
	}

	HPEN Pen;
};
//---------------------------------------------------------------------------
template<COLORREF Color>
struct cSolidBrushObject
{
	cSolidBrushObject()noexcept(true){
		Brush=::CreateSolidBrush(Color);
	}
	~cSolidBrushObject()noexcept(true){
		::DeleteObject(Brush);
	}

	HBRUSH Brush;
};
//---------------------------------------------------------------------------
}	// namespace cnUI
}	// namespace cnLibrary
//---------------------------------------------------------------------------
