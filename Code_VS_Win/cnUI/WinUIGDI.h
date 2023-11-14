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
	vWinDCPainter();
	~vWinDCPainter();

	void SetView(iUIView *View);
	void SetContentZPosition(Float32 ZPosition);
	void SetContentVisible(bool Visible);
	cUIRectangle GetFrameMargin(void);
protected:
	rPtr<iDCViewContent> fDCViewContent;
	RECT fPaintRC;

	virtual void cnLib_FUNC PaintStarted(void)override{}
	virtual void cnLib_FUNC PaintShow(void)override{}
	virtual void cnLib_FUNC PaintResume(void)override{}
	virtual void cnLib_FUNC PaintPaused(void)override{}
	virtual void cnLib_FUNC PaintHide(void)override{}
	virtual void cnLib_FUNC PaintStopped(void)override{}
	virtual void cnLib_FUNC PaintRectChanged(void)override;
	virtual void cnLib_FUNC PaintWindowChanged(void)override{}
};
//---------------------------------------------------------------------------
template<int Width,COLORREF Color>
struct cSolidPenObject
{
	cSolidPenObject(){
		Pen=::CreatePen(PS_SOLID,Width,Color);
	}
	~cSolidPenObject(){
		::DeleteObject(Pen);
	}

	HPEN Pen;
};
//---------------------------------------------------------------------------
template<COLORREF Color>
struct cSolidBrushObject
{
	cSolidBrushObject(){
		Brush=::CreateSolidBrush(Color);
	}
	~cSolidBrushObject(){
		::DeleteObject(Brush);
	}

	HBRUSH Brush;
};
//---------------------------------------------------------------------------
}	// namespace cnUI
}	// namespace cnLibrary
//---------------------------------------------------------------------------
