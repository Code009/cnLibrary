#include "WinUIGDI.h"

using namespace cnLibrary;
using namespace cnUI;

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
vWinDCPainter::vWinDCPainter()
	: fDCViewContent(cnWindows::CreateDCViewContent(this))
{
}
//---------------------------------------------------------------------------
vWinDCPainter::~vWinDCPainter()
{
	fDCViewContent->SetView(nullptr);
}
//---------------------------------------------------------------------------
void vWinDCPainter::SetView(iUIView *View)
{
	fDCViewContent->SetView(View);
}
//---------------------------------------------------------------------------
void vWinDCPainter::SetContentZPosition(Float32 ZPosition)
{
	fDCViewContent->SetZPosition(ZPosition);
}
//---------------------------------------------------------------------------
void vWinDCPainter::SetContentVisible(bool Visible)
{
	fDCViewContent->SetVisible(Visible);
}
//---------------------------------------------------------------------------
cUIRectangle vWinDCPainter::GetFrameMargin(void)
{
	return UIRectangleZero;
}
//---------------------------------------------------------------------------
void vWinDCPainter::PaintRectChanged(void)
{
	fPaintRC=fDCViewContent->GetPaintRect();
}
//---------------------------------------------------------------------------
