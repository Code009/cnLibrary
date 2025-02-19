#include "WinUIGDI.h"

using namespace cnLibrary;
using namespace cnUI;

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
vWinDCPainter::vWinDCPainter()noexcept(true)
	: fDCViewContent(cnWindows::CreateDCViewContent(this))
{
}
//---------------------------------------------------------------------------
vWinDCPainter::~vWinDCPainter()noexcept(true)
{
	fDCViewContent->SetView(nullptr);
}
//---------------------------------------------------------------------------
void vWinDCPainter::SetView(iUIView *View)noexcept(true)
{
	fDCViewContent->SetView(View);
}
//---------------------------------------------------------------------------
void vWinDCPainter::SetContentZPosition(Float32 ZPosition)noexcept(true)
{
	fDCViewContent->SetZPosition(ZPosition);
}
//---------------------------------------------------------------------------
void vWinDCPainter::SetContentVisible(bool Visible)noexcept(true)
{
	fDCViewContent->SetVisible(Visible);
}
//---------------------------------------------------------------------------
cUIRectangle vWinDCPainter::GetFrameMargin(void)noexcept(true)
{
	return UIRectangleZero;
}
//---------------------------------------------------------------------------
void vWinDCPainter::PaintRectChanged(void)noexcept(true)
{
	fPaintRC=fDCViewContent->GetPaintRect();
}
//---------------------------------------------------------------------------
