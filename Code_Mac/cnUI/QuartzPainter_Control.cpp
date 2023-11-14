#include "UIControlQuartz.h"
#include <cnSystem/iOS.h>

using namespace cnLib;
using namespace cnRTL;
using namespace cnUI;
using namespace cnUI::Quartz;

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cQuartzPainter::cQuartzPainter()
{
	fPaintTarget=nullptr;
}
//---------------------------------------------------------------------------
cQuartzPainter::~cQuartzPainter()
{
}
//---------------------------------------------------------------------------
void cnLib_Func cQuartzPainter::SetPaintTarget(iQuartzPaintTarget *PaintTarget)
{
	fPaintTarget=PaintTarget;
}
//---------------------------------------------------------------------------
void cnLib_Func cQuartzPainter::PaintRectChanged(void)
{
	fPaintTarget->GetPaintRect(fDrawRect);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cButtonPainter::cButtonPainter(const cButton *Data)
{
	fData=Data;
}
//---------------------------------------------------------------------------
cButtonPainter::~cButtonPainter()
{
}
//---------------------------------------------------------------------------
void cnLib_Func cButtonPainter::DrawRect(CGContextRef Context,const CGRect &Rect)
{
	// button

	// text
	const uchar16 *Text=fData->Text;
	if(Text!=nullptr){
		CGRect textrc=fDrawRect;
		uintn BtnTextLength=cnString::GetLength(Text);
		textrc.size.width-=1;
		textrc.size.height-=1;
		if(fData->Down){
			textrc.origin.x+=1;
			textrc.origin.y+=1;
		}
	}
}
//---------------------------------------------------------------------------
void cnLib_Func cButtonPainter::UpdateData(void)
{
	if(fPaintTarget!=nullptr)
		fPaintTarget->InvalidateRect(&fDrawRect);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cCheckBoxPainter::cCheckBoxPainter(const cCheckBox *Data)
{
	fData=Data;
}
//---------------------------------------------------------------------------
cCheckBoxPainter::~cCheckBoxPainter()
{
}
//---------------------------------------------------------------------------
void cnLib_Func cCheckBoxPainter::DrawRect(CGContextRef Context,const CGRect &Rect)
{

	// text
	const uchar16 *Text=fData->Text;
	if(Text!=nullptr){
		uintn BtnTextLength=cnString::GetLength(Text);
	}
}
//---------------------------------------------------------------------------
void cnLib_Func cCheckBoxPainter::UpdateData(void)
{
	if(fPaintTarget!=nullptr)
		fPaintTarget->InvalidateRect(&fDrawRect);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cRadioBoxPainter::cRadioBoxPainter(const cRadioBox *Data)
{
	fData=Data;
}
//---------------------------------------------------------------------------
cRadioBoxPainter::~cRadioBoxPainter()
{
}
//---------------------------------------------------------------------------
void cnLib_Func cRadioBoxPainter::DrawRect(CGContextRef Context,const CGRect &Rect)
{

	// text
	const uchar16 *Text=fData->Text;
	if(Text!=nullptr){
		uintn BtnTextLength=cnString::GetLength(Text);
	}
}
//---------------------------------------------------------------------------
void cnLib_Func cRadioBoxPainter::UpdateData(void)
{
	if(fPaintTarget!=nullptr)
		fPaintTarget->InvalidateRect(&fDrawRect);
}
//---------------------------------------------------------------------------
