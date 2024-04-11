#include "WinUIGDI.h"

using namespace cnLibrary;
using namespace cnUI;

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
vWinDCPainter::vWinDCPainter()noexcept
	: fDCViewContent(cnWindows::CreateDCViewContent(this))
{
}
//---------------------------------------------------------------------------
vWinDCPainter::~vWinDCPainter()noexcept
{
	fDCViewContent->SetView(nullptr);
}
//---------------------------------------------------------------------------
void vWinDCPainter::SetView(iUIView *View)noexcept
{
	fDCViewContent->SetView(View);
}
//---------------------------------------------------------------------------
void vWinDCPainter::SetContentZPosition(Float32 ZPosition)noexcept
{
	fDCViewContent->SetZPosition(ZPosition);
}
//---------------------------------------------------------------------------
void vWinDCPainter::SetContentVisible(bool Visible)noexcept
{
	fDCViewContent->SetVisible(Visible);
}
//---------------------------------------------------------------------------
cUIRectangle vWinDCPainter::GetFrameMargin(void)noexcept
{
	return UIRectangleZero;
}
//---------------------------------------------------------------------------
void vWinDCPainter::PaintRectChanged(void)noexcept
{
	fPaintRC=fDCViewContent->GetPaintRect();
}
//---------------------------------------------------------------------------
