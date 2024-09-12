#include "WinUIControl.h"

using namespace cnLibrary;
using namespace cnRTL;
using namespace cnUI;


//---------------------------------------------------------------------------
void cnUI::SetUIDefaultVisualToWindowsStandardStyle(void)noexcept
{
	gCreateDefaultTextCaretVisual=vWinTextCaret::Create;
	gCreateDefaultScrollBarKit=kWinControlScrollBar::Create;
	gCreateDefaultButtonVisual=vWinPushButton::Create;
	gCreateDefaultTreeBranchVisual=vWinTreeBranchGraph::Create;
	gCreateDefaultTreeExpandButtonVisual=vWinTreeExpandButton::Create;
	gCreateDefaultScrollBarVisual=vWinScrollBar::Create;
	gCreateDefaultScrollBarGripVisual=vWinScrollBarGrip::Create;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
vWinRectangle::vWinRectangle()noexcept
{
}
//---------------------------------------------------------------------------
vWinRectangle::~vWinRectangle()noexcept
{
	ClearObject();
}
//---------------------------------------------------------------------------
rPtr<viControl> vWinRectangle::Create(uInt32 BorderColor,uInt32 BackgroundColor)noexcept
{
	auto Content=rCreate< bwvControl<vWinRectangle> >();
	Content->BorderColor=BorderColor;
	Content->BackgroundColor=BackgroundColor;
	return Content;
}
//---------------------------------------------------------------------------
void vWinRectangle::SetView(iUIView *View)noexcept
{
	ViewControl::SetView(View);
	vWinDCPainter::SetView(View);
}
//---------------------------------------------------------------------------
void vWinRectangle::Update(void)noexcept
{
	ClearObject();
	fDCViewContent->InvalidateRect(&fPaintRC);
}
//---------------------------------------------------------------------------
void vWinRectangle::ClearObject(void)noexcept
{
	if(fObjectValid){
		if(fBGBrush!=nullptr){
			::DeleteObject(fBGBrush);
		}
		if(fBorderPen!=nullptr){
			::DeleteObject(fBorderPen);
		}
		fObjectValid=false;
	}
}
//---------------------------------------------------------------------------
cUIRectangle vWinRectangle::UIMargin(const cUIRectangle &Margin)noexcept
{
	cUIRectangle FrameMargin={1,1,1,1};
	return MergeMargin(FrameMargin,Margin);
}
//---------------------------------------------------------------------------
void vWinRectangle::Paint(HDC DC,HRGN)noexcept
{
	if(fObjectValid==false){
		fObjectValid=true;
		if(BackgroundColor&0xFF000000){
			fBGBrush=::CreateSolidBrush(BackgroundColor&0xFFFFFF);
		}
		if(BorderColor&0xFF000000){
			fBorderPen=::CreatePen(PS_SOLID,1,BorderColor&0xFFFFFF);
		}
	}
	
	HGDIOBJ Brush=fBGBrush;
	if(Brush==nullptr){
		Brush=::GetStockObject(NULL_BRUSH);
	}
	HGDIOBJ Pen=fBorderPen;
	if(Pen==nullptr){
		Pen=::GetStockObject(NULL_PEN);
	}
	Brush=::SelectObject(DC,Brush);
	Pen=::SelectObject(DC,Pen);

	::Rectangle(DC,fPaintRC.left,fPaintRC.top,fPaintRC.right,fPaintRC.bottom);
	::SelectObject(DC,Pen);
	::SelectObject(DC,Brush);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
vWinBorder::vWinBorder()noexcept
{
}
//---------------------------------------------------------------------------
vWinBorder::~vWinBorder()noexcept
{
}
//---------------------------------------------------------------------------
rPtr<viControl> vWinBorder::Create(UINT edge,UINT grfFlags,uInt32 BkColor)noexcept
{
	auto Content=rCreate< bwvControl<vWinBorder> >();
	Content->BackgroundColor=BkColor;
	Content->EdgeFlag=edge;
	Content->BorderFlag=grfFlags;
	return Content;
}
//---------------------------------------------------------------------------
void vWinBorder::SetView(iUIView *View)noexcept
{
	ViewControl::SetView(View);
	vWinDCPainter::SetView(View);
}
//---------------------------------------------------------------------------
cUIRectangle vWinBorder::UIMargin(const cUIRectangle &Margin)noexcept
{
	cUIRectangle FrameMargin={2,2,2,2};
	return MergeMargin(FrameMargin,Margin);
}
//---------------------------------------------------------------------------
void vWinBorder::Update(void)noexcept
{
	fDCViewContent->InvalidateRect(&fPaintRC);
}
//---------------------------------------------------------------------------
void vWinBorder::Paint(HDC DC,HRGN)noexcept
{
	bool DrawBG=(BackgroundColor&0xFF000000)!=0;
	if(DrawBG){
		::SetBkColor(DC,BackgroundColor&0xFFFFFF);
		::ExtTextOutW(DC,0,0,ETO_OPAQUE,&fPaintRC,nullptr,0,nullptr);
	}
	::DrawEdge(DC,&fPaintRC,EdgeFlag,BorderFlag);
}
//---------------------------------------------------------------------------
#if 0
//---------------------------------------------------------------------------
vWinGroupBox::vWinGroupBox(viTextControlData *Data,viSolidColorData *BackgroundColorData)
	: fData(Data)
	, fBackgroundColorData(BackgroundColorData)
{
	TextDataInsertCallback();
	ColorDataInsertCallback();
}
//---------------------------------------------------------------------------
vWinGroupBox::~vWinGroupBox()
{
	TextDataRemoveCallback();
	ColorDataRemoveCallback();
}
//---------------------------------------------------------------------------
rPtr<viControl> vWinGroupBox::Create(viTextControlData *Data,viSolidColorData *BackgroundColorData,UINT edge,UINT grfFlags)
{
	auto Content=rCreate< bwvControl<vWinGroupBox> >(Data,BackgroundColorData);
	Content->EdgeFlag=edge;
	Content->BorderFlag=grfFlags;
	return Content;
}
//---------------------------------------------------------------------------
viTextControlData* vWinGroupBox::GetTextData(void)const
{
	return fData;
}
//---------------------------------------------------------------------------
void vWinGroupBox::SetTextData(viTextControlData *Data)
{
	if(fData==Data)
		return;

	ColorDataRemoveCallback();
	fData=Data;
	TextDataInsertCallback();

	Update();
}
//---------------------------------------------------------------------------
viSolidColorData* vWinGroupBox::GetColorData(void)const
{
	return fBackgroundColorData;
}
//---------------------------------------------------------------------------
void vWinGroupBox::SetColorData(viSolidColorData *BackgroundColorData)
{
	if(fBackgroundColorData==BackgroundColorData)
		return;

	ColorDataRemoveCallback();
	fBackgroundColorData=BackgroundColorData;
	ColorDataInsertCallback();

	Update();
}
//---------------------------------------------------------------------------
void vWinGroupBox::SetData(viTextControlData *Data,viSolidColorData *BackgroundColorData)
{
	if(fData==Data)
		return SetColorData(BackgroundColorData);
	if(fBackgroundColorData==BackgroundColorData)
		return SetTextData(Data);

	ColorDataRemoveCallback();
	ColorDataRemoveCallback();
	fData=Data;
	fBackgroundColorData=BackgroundColorData;
	TextDataInsertCallback();
	ColorDataInsertCallback();

	Update();
}
//---------------------------------------------------------------------------
void vWinGroupBox::Update(void)
{
	fFont=nullptr;
	fDCViewContent->InvalidateRect(&fPaintRC);
}
//---------------------------------------------------------------------------
void vWinGroupBox::SetView(iUIView *View)
{
	LayoutControl::SetView(View);
	vWinDCPainter::SetView(View);
}
//---------------------------------------------------------------------------
void vWinGroupBox::SetContentZIndex(sfInt16 ZIndex)
{
	LayoutControl::SetContentZIndex(ZIndex);
	vWinDCPainter::SetContentZIndex(ZIndex);
}
//---------------------------------------------------------------------------
cUIRectangle vWinGroupBox::UIMargin(const cUIRectangle &Margin)
{
	if(fData==nullptr){
		return RectangleZero;
	}
	auto &TextStyle=fData->ControlTextStyle();
	cUIRectangle FrameMargin=CalculateBoxMargin(TextStyle.FontSize);
	return MergeMargin(FrameMargin,Margin);
}
//---------------------------------------------------------------------------
cUIRectangle vWinGroupBox::CalculateBoxMargin(sfInt32n TextHeight)
{
	cUIRectangle Margin;
	Margin.Left=5;
	Margin.Top=5;
	Margin.Right=5;
	Margin.Bottom=5;

	if(Margin.Top<TextHeight){
		Margin.Top=TextHeight;
	}
	return Margin;
}
//---------------------------------------------------------------------------
void vWinGroupBox::SetupFont(const cUITextStyle &TextStyle)
{
	if(fFont!=nullptr)
		return;

	auto PaintDevice=cnWindows::QueryDCPaintDevice();
	fFont=PaintDevice->QueryFont(TextStyle);
}
//---------------------------------------------------------------------------
void vWinGroupBox::Paint(HDC DC)
{
	if(fBackgroundColorData!=nullptr){
		uInt32 BackgroundColor=fBackgroundColorData->ColorValue();
		bool DrawBG=(BackgroundColor&0xFF000000)!=0;
		if(DrawBG){
			::SetBkColor(DC,BackgroundColor&0xFFFFFF);
			::ExtTextOutW(DC,0,0,ETO_OPAQUE,&fPaintRC,nullptr,0,nullptr);
		}
	}

	if(fData==nullptr)
		return;

	auto &TextStyle=fData->ControlTextStyle();

	SetupFont(TextStyle);

	auto Margin=CalculateBoxMargin(TextStyle.FontSize);
	RECT BorderRC;
	BorderRC.left=fPaintRC.left+Margin.Left/2;
	BorderRC.top=fPaintRC.top+Margin.Top/2;
	BorderRC.right=fPaintRC.right-Margin.Right/2;
	BorderRC.bottom=fPaintRC.bottom-Margin.Bottom/2;
	
	::DrawEdge(DC,&BorderRC,EdgeFlag,BorderFlag);

	COLORREF BGColor=::GetSysColor(COLOR_BTNFACE);
	auto Text=fData->ControlTextString();
	uIntn TextLength=fData->ControlTextLength();
	if(TextLength!=0){
		SIZE TextSize;
		::GetTextExtentPoint32W(DC,utow(Text),static_cast<int>(TextLength),&TextSize);

		HGDIOBJ OldFont=nullptr;
		if(fFont!=nullptr){
			OldFont=::SelectObject(DC,fFont->GetFontHandle());
		}

		RECT TextRC;
		TextRC.left=fPaintRC.left+20;
		TextRC.top=fPaintRC.top;
		TextRC.right=TextRC.left+TextSize.cx+10;
		TextRC.bottom=TextRC.top+Margin.Top+2;
		int TextX=TextRC.left+5;
		int TextY=(TextRC.top+TextRC.bottom-TextSize.cy)/2;
		::SetTextColor(DC,TextStyle.TextColor&0xFFFFFF);
		if((TextStyle.BackgroundColor&0xFF000000)!=0){
			// background
			::SetBkColor(DC,BGColor);
			::ExtTextOutW(DC,TextX,TextY,ETO_OPAQUE,&TextRC,nullptr,0,nullptr);
			// text
			::SetBkColor(DC,TextStyle.BackgroundColor&0xFFFFFF);
			::SetBkMode(DC,OPAQUE);
			::ExtTextOutW(DC,TextX,TextY,ETO_CLIPPED,&TextRC,utow(Text),static_cast<UINT>(TextLength),nullptr);
		}
		else{
			::SetBkColor(DC,BGColor);
			::SetBkMode(DC,TRANSPARENT);
			::ExtTextOutW(DC,TextX,TextY,ETO_OPAQUE|ETO_CLIPPED,&TextRC,utow(Text),static_cast<UINT>(TextLength),nullptr);
		}
		if(OldFont!=nullptr){
			::SelectObject(DC,OldFont);
		}
	}

}
//---------------------------------------------------------------------------
void vWinGroupBox::TextDataInsertCallback(void)
{
	if(fData!=nullptr){
		fData->ControlTextNotifySet.Insert(&fTextControlNotifyProcedure);
	}
}
//---------------------------------------------------------------------------
void vWinGroupBox::TextDataRemoveCallback(void)
{
	if(fData!=nullptr){
		fData->ControlTextNotifySet.Remove(&fTextControlNotifyProcedure);
	}
}
//---------------------------------------------------------------------------
void vWinGroupBox::ColorDataInsertCallback(void)
{
	if(fBackgroundColorData!=nullptr){
		fBackgroundColorData->SolidColorNotifySet.Insert(&fSolidColorNotifyProcedure);
	}
}
//---------------------------------------------------------------------------
void vWinGroupBox::ColorDataRemoveCallback(void)
{
	if(fBackgroundColorData!=nullptr){
		fBackgroundColorData->SolidColorNotifySet.Remove(&fSolidColorNotifyProcedure);
	}
}
//---------------------------------------------------------------------------
void vWinGroupBox::cSolidColorNotifyProcedure::Execute(void)
{
	auto Host=cnMemory::GetObjectFromMemberPointer(this,&vWinGroupBox::fSolidColorNotifyProcedure);
	Host->Update();
}
//---------------------------------------------------------------------------
void vWinGroupBox::cTextControlNotifyProcedure::Execute(void)
{
	auto Host=cnMemory::GetObjectFromMemberPointer(this,&vWinGroupBox::fTextControlNotifyProcedure);
	Host->Update();
}
#endif // 0
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cWinGroupBoxPainter_Normal::cWinGroupBoxPainter_Normal()noexcept
{
}
//---------------------------------------------------------------------------
cWinGroupBoxPainter_Normal::~cWinGroupBoxPainter_Normal()noexcept
{
}
//---------------------------------------------------------------------------
cUIRectangle cWinGroupBoxPainter_Normal::GetContentMarginRect(void)noexcept
{
	return {2,2,2,2};
}
//---------------------------------------------------------------------------
void cWinGroupBoxPainter_Normal::PaintBox(HDC DC,const RECT &Rect)noexcept
{
	cUIRectangle Margin={0,0,0,0};
	RECT BorderRC;
	BorderRC.left=static_cast<int>(Rect.left+Margin.Left/2);
	BorderRC.top=static_cast<int>(Rect.top+Margin.Top/2);
	BorderRC.right=static_cast<int>(Rect.right-Margin.Right/2);
	BorderRC.bottom=static_cast<int>(Rect.bottom-Margin.Bottom/2);
	
	::DrawEdge(DC,&BorderRC,EDGE_ETCHED,BF_RECT);

}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
vWinGroupBox::vWinGroupBox(viTextControlData *TextData)noexcept
	: fTextData(TextData)
{
	TextDataInsertCallback();
}
//---------------------------------------------------------------------------
vWinGroupBox::~vWinGroupBox()noexcept
{
	TextDataRemoveCallback();
}
//---------------------------------------------------------------------------
rPtr<viControl> vWinGroupBox::Create(viTextControlData *Data)noexcept
{
	return rCreate< bwvControl<vWinGroupBox> >(Data);
}
//---------------------------------------------------------------------------
viTextControlData* vWinGroupBox::GetTextData(void)const noexcept
{
	return fTextData;
}
//---------------------------------------------------------------------------
void vWinGroupBox::SetTextData(viTextControlData *Data)noexcept
{
	if(fTextData==Data)
		return;

	TextDataRemoveCallback();
	fTextData=Data;
	TextDataInsertCallback();

	Update();
}
//---------------------------------------------------------------------------
void vWinGroupBox::Update(void)noexcept
{
	fFont=nullptr;
	fDCViewContent->InvalidateRect(&fPaintRC);
}
//---------------------------------------------------------------------------
void vWinGroupBox::SetView(iUIView *View)noexcept
{
	ViewControl::SetView(View);
	vWinDCPainter::SetView(View);
}
//---------------------------------------------------------------------------
cUIRectangle vWinGroupBox::CalculateBoxMargin(Float32 TextHeight)noexcept
{
	auto Margin=fPainter->GetContentMarginRect();

	if(Margin.Top<TextHeight){
		Margin.Top=TextHeight;
	}
	return Margin;
}
//---------------------------------------------------------------------------
void vWinGroupBox::SetupFont(const cUITextStyle &TextStyle)noexcept
{
	if(fFont!=nullptr)
		return;

	auto PaintDevice=cnWindows::QueryDCPaintDevice();
	fFont=PaintDevice->QueryFont(TextStyle);
}
//---------------------------------------------------------------------------
void vWinGroupBox::ThemeSetup(HWND WindowHandle)noexcept
{
	HTHEME Theme=nullptr;
	if(WindowHandle!=nullptr && ::IsAppThemed()){
		Theme=cWinGroupBoxPainter_Theme::OpenTheme(WindowHandle);
	}
	if(Theme!=nullptr){
		fPainter=new cWinGroupBoxPainter_Theme(Theme);
	}
	else{
		fPainter=new cWinGroupBoxPainter_Normal;
	}
}
//---------------------------------------------------------------------------
void vWinGroupBox::ThemeClear(void)noexcept
{
	delete fPainter;
	fPainter=nullptr;
}
//---------------------------------------------------------------------------
void vWinGroupBox::Paint(HDC DC,HRGN ClipRegion)noexcept
{
	if(fTextData==nullptr)
		return;

	auto &TextStyle=fTextData->ControlTextStyle();

	SetupFont(TextStyle);

	auto Margin=CalculateBoxMargin(TextStyle.FontSize);
	RECT BorderRC;
	BorderRC.left=fPaintRC.left+static_cast<sfInt32>(Margin.Left)/2;
	BorderRC.top=fPaintRC.top+static_cast<sfInt32>(Margin.Top)/2;
	BorderRC.right=fPaintRC.right-static_cast<sfInt32>(Margin.Right)/2;
	BorderRC.bottom=fPaintRC.bottom-static_cast<sfInt32>(Margin.Bottom)/2;
	
	auto Text=fTextData->ControlTextString();
	uIntn TextLength=fTextData->ControlTextLength();
	if(TextLength!=0){
		SIZE TextSize;
		::GetTextExtentPoint32W(DC,utow(Text),static_cast<int>(TextLength),&TextSize);

		HGDIOBJ OldFont=nullptr;
		if(fFont!=nullptr){
			OldFont=::SelectObject(DC,fFont->GetFontHandle());
		}

		RECT TextRC;
		TextRC.left=fPaintRC.left+20;
		TextRC.top=fPaintRC.top;
		TextRC.right=TextRC.left+TextSize.cx+10;
		TextRC.bottom=TextRC.top+static_cast<sfInt32>(Margin.Top)+2;
		int TextX=TextRC.left+5;
		int TextY=(TextRC.top+TextRC.bottom-TextSize.cy)/2;
		::SetTextColor(DC,COLORREFFromUIColor(TextStyle.TextColor));

		if(TextStyle.BackgroundColor.Alpha!=0){

			fPainter->PaintBox(DC,BorderRC);

			::SetBkMode(DC,OPAQUE);
			// text color
			::SetBkColor(DC,COLORREFFromUIColor(TextStyle.BackgroundColor));
			::ExtTextOutW(DC,TextX,TextY,ETO_OPAQUE|ETO_CLIPPED,&TextRC,utow(Text),static_cast<UINT>(TextLength),nullptr);
		}
		else{
			::SetBkMode(DC,TRANSPARENT);
			::ExtTextOutW(DC,TextX,TextY,ETO_CLIPPED,&TextRC,utow(Text),static_cast<UINT>(TextLength),nullptr);


			// clip text rect
			::ExcludeClipRect(DC,TextRC.left,TextRC.top,TextRC.right,TextRC.bottom);

			fPainter->PaintBox(DC,BorderRC);

			// restore clipping
			::SelectClipRgn(DC,ClipRegion);
		}


		if(OldFont!=nullptr){
			::SelectObject(DC,OldFont);
		}

	}
	else{
		fPainter->PaintBox(DC,BorderRC);
	}


}
//---------------------------------------------------------------------------
cUIRectangle vWinGroupBox::UIMargin(const cUIRectangle &Margin)noexcept
{
	if(fTextData==nullptr){
		return UIRectangleZero;
	}
	auto &TextStyle=fTextData->ControlTextStyle();
	cUIRectangle FrameMargin=CalculateBoxMargin(TextStyle.FontSize);
	return MergeMargin(FrameMargin,Margin);
}
//---------------------------------------------------------------------------
void vWinGroupBox::TextDataInsertCallback(void)noexcept
{
	if(fTextData!=nullptr){
		fTextControlNotifyToken=fTextData->ControlTextNotifySet.Insert([this]{
			Update();
		});
	}
}
//---------------------------------------------------------------------------
void vWinGroupBox::TextDataRemoveCallback(void)noexcept
{
	if(fTextData!=nullptr){
		fTextData->ControlTextNotifySet.Remove(fTextControlNotifyToken);
	}
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cWinGroupBox::cWinGroupBox()noexcept
{
}
//---------------------------------------------------------------------------
cWinGroupBox::~cWinGroupBox()noexcept
{
}
//---------------------------------------------------------------------------
void cWinGroupBox::ControlBackgroundSetDefault(void)noexcept
{
	SetBackground(vWinGroupBox::Create(this));
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cWinNCFramePainter_Normal::cWinNCFramePainter_Normal()noexcept
{
}
//---------------------------------------------------------------------------
cWinNCFramePainter_Normal::~cWinNCFramePainter_Normal()noexcept
{
}
//---------------------------------------------------------------------------
void cWinNCFramePainter_Normal::Paint(HDC DC,const RECT &Rect,const RECT &Border,Float32 CaptionHeight,bool Active)noexcept
{
	::SetBkColor(DC,::GetSysColor(COLOR_BTNFACE));
	::ExtTextOutW(DC,0,0,ETO_OPAQUE,&Rect,nullptr,0,nullptr);
	::DrawEdge(DC,(LPRECT)&Rect,EDGE_RAISED,BF_RECT);

	// caption

	TRIVERTEX v[2];
	v[0].x=Rect.left+Border.left;
	v[0].y=Rect.top+Border.top;
	v[1].x=Rect.right-Border.right;
	v[1].y=v[0].x+static_cast<sfInt32>(CaptionHeight);

	COLORREF ColorLeft;
	COLORREF ColorRight;
	if(Active){
		ColorLeft=::GetSysColor(COLOR_ACTIVECAPTION);
		ColorRight=::GetSysColor(COLOR_GRADIENTACTIVECAPTION);
	}
	else{
		ColorLeft=::GetSysColor(COLOR_INACTIVECAPTION);
		ColorRight=::GetSysColor(COLOR_GRADIENTINACTIVECAPTION);
	}
	v[0].Red=static_cast<COLOR16>((ColorLeft&0xFF)<<8);
	v[0].Green=static_cast<COLOR16>(ColorLeft&0xFF00);
	v[0].Blue=static_cast<COLOR16>((ColorLeft&0xFF0000)>>8);
	v[0].Alpha=0xFFFF;
	v[1].Red=static_cast<COLOR16>((ColorRight&0xFF)<<8);
	v[1].Green=static_cast<COLOR16>(ColorRight&0xFF00);
	v[1].Blue=static_cast<COLOR16>((ColorRight&0xFF0000)>>8);
	v[1].Alpha=0xFFFF;
	GRADIENT_RECT gr;
	gr.UpperLeft=0;
	gr.LowerRight=1;

	::GradientFill(DC,v,2,&gr,1,GRADIENT_FILL_RECT_H);
}
//---------------------------------------------------------------------------
void cWinNCFramePainter_Normal::PaintFrame(HDC DC,const RECT &Rect,const RECT &Border,Float32 CaptionHeight,bool Active)noexcept
{
	return Paint(DC,Rect,Border,CaptionHeight,Active);
}
//---------------------------------------------------------------------------
void cWinNCFramePainter_Normal::PaintSmallFrame(HDC DC,const RECT &Rect,const RECT &Border,Float32 CaptionHeight,bool Active)noexcept
{
	return Paint(DC,Rect,Border,CaptionHeight,Active);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
vWinNCFrame::vWinNCFrame(viWinNCFrameData *Data)noexcept
	: fData(Data)
{
	DataInsertCallback();
}
//---------------------------------------------------------------------------
vWinNCFrame::~vWinNCFrame()noexcept
{
	DataRemoveCallback();
}
//---------------------------------------------------------------------------
rPtr<viControl> vWinNCFrame::Create(viWinNCFrameData *Data)noexcept
{
	return rCreate< bwvControl<vWinNCFrame> >(Data);
}
//---------------------------------------------------------------------------
viWinNCFrameData* vWinNCFrame::GetData(void)const noexcept
{
	return fData;
}
//---------------------------------------------------------------------------
void vWinNCFrame::SetData(viWinNCFrameData *Data)noexcept
{
	if(fData==Data)
		return;
	
	DataRemoveCallback();
	fData=Data;
	DataInsertCallback();

	Update();
}
//---------------------------------------------------------------------------
void vWinNCFrame::Update(void)noexcept
{
	fDCViewContent->InvalidateRect(&fPaintRC);
}
//---------------------------------------------------------------------------
void vWinNCFrame::DataInsertCallback(void)noexcept
{
	if(fData!=nullptr){
		fFrameNotifyToken=fData->FrameNotifySet.Insert([this]{
			Update();
		});
	}
}
//---------------------------------------------------------------------------
void vWinNCFrame::DataRemoveCallback(void)noexcept
{
	if(fData!=nullptr){
		fData->FrameNotifySet.Remove(fFrameNotifyToken);
	}
}
//---------------------------------------------------------------------------
void vWinNCFrame::ThemeSetup(HWND WindowHandle)noexcept
{
	HTHEME Theme=nullptr;
	if(::IsAppThemed()){
		Theme=cWinNCFramePainter_Theme::OpenTheme(WindowHandle);
	}
	if(Theme!=nullptr){
		fPainter=new cWinNCFramePainter_Theme(Theme);
	}
	else{
		fPainter=new cWinNCFramePainter_Normal;
	}
}
//---------------------------------------------------------------------------
void vWinNCFrame::ThemeClear(void)noexcept
{
	delete fPainter;
	fPainter=nullptr;
}
//---------------------------------------------------------------------------
void vWinNCFrame::Paint(HDC DC,HRGN)noexcept
{
	bool SmallCaption;
	bool Active;
	RECT BorderSize;
	Float32 CaptionHeight;
	if(fData!=nullptr){
		SmallCaption=fData->FrameIsSmallCaption();
		Active=fData->FrameIsActivate();
		BorderSize=fData->FrameGetBorderSize();
		CaptionHeight=fData->FrameGetCaptionHeight();
	}
	else{
		SmallCaption=false;
		Active=false;
		BorderSize.left=3;
		BorderSize.top=3;
		BorderSize.right=3;
		BorderSize.bottom=3;
		CaptionHeight=30;
	}
	if(SmallCaption){
		fPainter->PaintSmallFrame(DC,fPaintRC,BorderSize,CaptionHeight,Active);
	}
	else{
		fPainter->PaintFrame(DC,fPaintRC,BorderSize,CaptionHeight,Active);
	}
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cWinNCFrame::cWinNCFrame()noexcept
{
	SetDefaultBorderSize();
}
//---------------------------------------------------------------------------
cWinNCFrame::~cWinNCFrame()noexcept
{
	SetView(nullptr);
	InvalidateData();
}
//---------------------------------------------------------------------------
void cWinNCFrame::SetDefaultBorderSize(void)noexcept
{
	RECT rc={0,0,0,0};
	AdjustWindowRect(&rc,WS_OVERLAPPEDWINDOW,false);

	SizeLeft=static_cast<Float32>(-rc.left);
	SizeRight=static_cast<Float32>(rc.right);
	SizeBottom=static_cast<Float32>(rc.bottom);
	SizeTop=static_cast<Float32>(rc.bottom);
	CaptionHeight=static_cast<Float32>(rc.bottom-rc.top);
}
//---------------------------------------------------------------------------
static RECT GetWindowNCSize(HWND WindowHandle)noexcept
{
	RECT Rect;
	Rect.left=0;
	Rect.top=0;
	Rect.right=0x8000;
	Rect.bottom=0x8000;

	::SendMessageW(WindowHandle,WM_NCCALCSIZE,FALSE,reinterpret_cast<LPARAM>(&Rect));
	Rect.right=0x8000-Rect.right;
	Rect.bottom=0x8000-Rect.bottom;
	return Rect;
}
//---------------------------------------------------------------------------
void cWinNCFrame::SetBorderSizeForWindowHandle(HWND WindowHandle)noexcept
{
	DWORD ExStyle=::GetWindowLongW(WindowHandle,GWL_EXSTYLE);
	SmallCaption=0!=(ExStyle&WS_EX_TOOLWINDOW);
	auto BorderSize=GetWindowNCSize(WindowHandle);
	
	SizeLeft=static_cast<Float32>(BorderSize.left);
	SizeRight=static_cast<Float32>(BorderSize.right);
	SizeBottom=static_cast<Float32>(BorderSize.bottom);
	SizeTop=static_cast<Float32>(BorderSize.bottom);
	CaptionHeight=static_cast<Float32>(BorderSize.bottom-BorderSize.top);

	Update();
}
//---------------------------------------------------------------------------
void cWinNCFrame::SetBorderSizeForWindowFrame(iWindowFrame *Frame)noexcept
{
	HWND WindowHandle=Frame->GetWindow()->GetWindowHandle();
	if(WindowHandle==nullptr){
		return SetDefaultBorderSize();
	}
	DWORD ExStyle=::GetWindowLongW(WindowHandle,GWL_EXSTYLE);
	SmallCaption=0!=(ExStyle&WS_EX_TOOLWINDOW);
	auto BorderSize=Frame->GetBorder();
	
	SizeLeft=static_cast<Float32>(BorderSize.left);
	SizeRight=static_cast<Float32>(BorderSize.right);
	SizeBottom=static_cast<Float32>(BorderSize.bottom);
	SizeTop=static_cast<Float32>(BorderSize.bottom);
	CaptionHeight=static_cast<Float32>(BorderSize.top-BorderSize.bottom);

	Update();
}
//---------------------------------------------------------------------------
void cWinNCFrame::Update(void)noexcept
{
	FrameNotifySet();
}
//---------------------------------------------------------------------------
void cWinNCFrame::ViewSetup(void)noexcept
{
	VisualControl::ViewSetup();

	ViewInsertMouseHandler(fView,this);

	//auto WindowViewport=iCast<iWindowViewport>(fView);
	//if(WindowViewport!=nullptr){
		//WindowViewport->SetMouseAutoCapture(false);
	//}
}
//---------------------------------------------------------------------------
void cWinNCFrame::ViewClear(void)noexcept
{
	//auto WindowViewport=iCast<iWindowViewport>(fView);
	//if(WindowViewport!=nullptr){
		//WindowViewport->SetMouseAutoCapture(true);
	//}

	ViewRemoveMouseHandler(fView,this);

	VisualControl::ViewClear();
}
//---------------------------------------------------------------------------
void cWinNCFrame::ControlBackgroundSetDefault(void)noexcept
{
	SetBackground(rCreate< bwvControl<vWinNCFrame> >(this));
}
//---------------------------------------------------------------------------
bool cWinNCFrame::FrameIsSmallCaption(void)noexcept
{
	return SmallCaption;
}
//---------------------------------------------------------------------------
bool cWinNCFrame::FrameIsActivate(void)noexcept
{
	return fActivate;
}
//---------------------------------------------------------------------------
RECT cWinNCFrame::FrameGetBorderSize(void)noexcept
{
	RECT Border;
	Border.left=static_cast<sfInt32>(SizeLeft);
	Border.top=static_cast<sfInt32>(SizeTop);
	Border.right=static_cast<sfInt32>(SizeRight);
	Border.bottom=static_cast<sfInt32>(SizeBottom);
	return Border;
}
//---------------------------------------------------------------------------
Float32 cWinNCFrame::FrameGetCaptionHeight(void)noexcept
{
	return CaptionHeight;
}
//---------------------------------------------------------------------------
DWORD cWinNCFrame::HitTest(const cUIPoint &ViewPos)noexcept
{
	auto ViewSize=fView->GetSize();
	if(ViewPos.x<SizeLeft){
		if(ViewPos.y<SizeCorner){
			return HTTOPLEFT;
		}
		if(ViewPos.y>ViewSize.y-SizeCorner){
			return HTBOTTOMLEFT;
		}
		return HTLEFT;
	}
	else if(ViewPos.x>ViewSize.x-SizeRight){
		if(ViewPos.y<SizeCorner){
			return HTTOPRIGHT;
		}
		if(ViewPos.y>ViewSize.y-SizeCorner){
			return HTBOTTOMRIGHT;
		}
		return HTRIGHT;
	}
	else{
		if(ViewPos.y<SizeTop){
			if(ViewPos.x<SizeCorner){
				return HTTOPLEFT;
			}
			if(ViewPos.x>ViewSize.x-SizeCorner){
				return HTTOPRIGHT;
			}
			return HTTOP;
		}
		else if(ViewPos.y<SizeTop+CaptionHeight){
			if(ViewPos.x>SizeLeft && ViewPos.x<ViewSize.x-SizeRight){
				return HTCAPTION;
			}
			return HTNOWHERE;
		}
		else if(ViewPos.y>ViewSize.y-SizeBottom){
			if(ViewPos.x<SizeCorner){
				return HTBOTTOMLEFT;
			}
			if(ViewPos.x>ViewSize.x-SizeCorner){
				return HTBOTTOMRIGHT;
			}
			return HTBOTTOM;
		}
	}
	return HTNOWHERE;
}
//---------------------------------------------------------------------------
iInterface* cWinNCFrame::MouseGetCursor(iUIMouseEvent *MouseEvent)noexcept
{
	cUIPoint pt;
	MouseEvent->GetPosition(fView,pt);
	auto HTCode=HitTest(pt);
	switch(HTCode){
	case HTCAPTION:
		return cnSystem::GetSysMouseCursor(SysMouseCursor::Normal);
	case HTTOP:
		return cnSystem::GetSysMouseCursor(SysMouseCursor::SizeV);
	case HTTOPLEFT:
		return cnSystem::GetSysMouseCursor(SysMouseCursor::SizeBS);
	case HTTOPRIGHT:
		return cnSystem::GetSysMouseCursor(SysMouseCursor::SizeFS);
	case HTLEFT:
		return cnSystem::GetSysMouseCursor(SysMouseCursor::SizeH);
	case HTRIGHT:
		return cnSystem::GetSysMouseCursor(SysMouseCursor::SizeH);
	case HTBOTTOM:
		return cnSystem::GetSysMouseCursor(SysMouseCursor::SizeV);
	case HTBOTTOMLEFT:
		return cnSystem::GetSysMouseCursor(SysMouseCursor::SizeFS);
	case HTBOTTOMRIGHT:
		return cnSystem::GetSysMouseCursor(SysMouseCursor::SizeBS);
	}
	return nullptr;
}
//---------------------------------------------------------------------------
void cWinNCFrame::MouseDown(iUIMouseEvent *MouseEvent,eMouseButton Button)noexcept
{
	if(Button==MouseButton::Left){
		auto WindowHandle=GetWindowHandleFromUIView(fView);
		cUIPoint pt;
		MouseEvent->GetPosition(nullptr,pt);
		RECT wrc;
		::GetWindowRect(WindowHandle,&wrc);
		cUIPoint ViewPos;
		ViewPos.x=pt.x-wrc.left;
		ViewPos.y=pt.y-wrc.top;
		auto HTCode=HitTest(ViewPos);
		POINTS pts;
		pts.x=static_cast<SHORT>(pt.x);
		pts.y=static_cast<SHORT>(pt.y);
		::DefWindowProcW(WindowHandle,WM_NCLBUTTONDOWN,HTCode,reinterpret_cast<LPARAM&>(pts));
	}
}
//---------------------------------------------------------------------------
void cWinNCFrame::WindowAttached(void)noexcept
{
}
//---------------------------------------------------------------------------
void cWinNCFrame::WindowDetached(void)noexcept
{
}
//---------------------------------------------------------------------------
bool cWinNCFrame::WindowMessage(LRESULT &MsgResult,const cWindowMessageParam &MsgParam)noexcept
{MsgResult,MsgParam;
	return false;
}
//---------------------------------------------------------------------------
void cWinNCFrame::WindowMessageProcessed(LRESULT,const cWindowMessageParam &Message)noexcept
{
	//auto WindowHandle=GetViewWindowHandle(fView);
	switch(Message.Code){
	case WM_ACTIVATE:
		switch(Message.wParam){
		case WA_ACTIVE:
		case WA_CLICKACTIVE:
			fActivate=true;
			break;
		default:
		case WA_INACTIVE:
			fActivate=false;
			break;
		}
		break;
	case WM_WINDOWPOSCHANGED:
		break;
	}
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
bvWinButton::bvWinButton(viButtonData *Data)noexcept
	: fButtonData(Data)
{
	ButtonDataInsertCallback();
}
//---------------------------------------------------------------------------
bvWinButton::~bvWinButton()noexcept
{
	ButtonDataRemoveCallback();
}
//---------------------------------------------------------------------------
viButtonData* bvWinButton::GetButtonData(void)const noexcept
{
	return fButtonData;
}
//---------------------------------------------------------------------------
void bvWinButton::SetButtonData(viButtonData *Data)noexcept
{
	if(fButtonData==Data)
		return;

	ButtonDataRemoveCallback();
	fButtonData=Data;
	ButtonDataInsertCallback();

	Update();
}
//---------------------------------------------------------------------------
void bvWinButton::ButtonDataInsertCallback(void)noexcept
{
	if(fButtonData!=nullptr){
		fButtonDataNotifyToken=fButtonData->ButtonNotifySet.Insert([this]{
			Update();
		});
	}
}
//---------------------------------------------------------------------------
void bvWinButton::ButtonDataRemoveCallback(void)noexcept
{
	if(fButtonData!=nullptr){
		fButtonData->ButtonNotifySet.Remove(fButtonDataNotifyToken);
	}
}
//---------------------------------------------------------------------------
void bvWinButton::Update(void)noexcept
{
	fDCViewContent->InvalidateRect(&fPaintRC);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
bvWinThemeButton::bvWinThemeButton(viButtonData *Data)noexcept
	: fButtonData(Data)
{
	ButtonDataInsertCallback();
}
//---------------------------------------------------------------------------
bvWinThemeButton::~bvWinThemeButton()noexcept
{
	ButtonDataRemoveCallback();
}
//---------------------------------------------------------------------------
viButtonData* bvWinThemeButton::GetButtonData(void)const noexcept
{
	return fButtonData;
}
//---------------------------------------------------------------------------
void bvWinThemeButton::SetButtonData(viButtonData *Data)noexcept
{
	if(fButtonData==Data)
		return;

	ButtonDataRemoveCallback();
	fButtonData=Data;
	ButtonDataInsertCallback();

	Update();
}
//---------------------------------------------------------------------------
void bvWinThemeButton::ButtonDataInsertCallback(void)noexcept
{
	if(fButtonData!=nullptr){
		fButtonDataNotifyToken=fButtonData->ButtonNotifySet.Insert([this]{
			Update();
		});
	}
}
//---------------------------------------------------------------------------
void bvWinThemeButton::ButtonDataRemoveCallback(void)noexcept
{
	if(fButtonData!=nullptr){
		fButtonData->ButtonNotifySet.Remove(fButtonDataNotifyToken);
	}
}
//---------------------------------------------------------------------------
void bvWinThemeButton::Update(void)noexcept
{
	fDCViewContent->InvalidateRect(&fPaintRC);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cWinNCButtonPainter_Normal::cWinNCButtonPainter_Normal()noexcept
{
}
cWinNCButtonPainter_Normal::~cWinNCButtonPainter_Normal()noexcept
{
}
//---------------------------------------------------------------------------
UINT cWinNCButtonPainter_Normal::StateIDFromState(eButtonState State)noexcept
{
	UINT StateFlag=0;
	switch(State){
	default:
	case ButtonState::Inactive:
	case ButtonState::Normal:
		StateFlag=0;
		break;
	case ButtonState::Hot:
		StateFlag=DFCS_HOT;
		break;
	case ButtonState::Pressed:
		StateFlag=DFCS_PUSHED;
		break;
	case ButtonState::Disabled:
		StateFlag=DFCS_INACTIVE;
		break;
	}
	return StateFlag;
}
//---------------------------------------------------------------------------
void cWinNCButtonPainter_Normal::PaintCloseButton(HDC DC,const RECT &Rect,eButtonState State)noexcept
{
	auto StateFlag=StateIDFromState(State);
	::DrawFrameControl(DC,(LPRECT)&Rect,DFC_CAPTION,DFCS_CAPTIONCLOSE|StateFlag);
}
//---------------------------------------------------------------------------
void cWinNCButtonPainter_Normal::PaintMinButton(HDC DC,const RECT &Rect,eButtonState State)noexcept
{
	auto StateFlag=StateIDFromState(State);
	::DrawFrameControl(DC,(LPRECT)&Rect,DFC_CAPTION,DFCS_CAPTIONMIN|StateFlag);
}
//---------------------------------------------------------------------------
void cWinNCButtonPainter_Normal::PaintMaxButton(HDC DC,const RECT &Rect,eButtonState State)noexcept
{
	auto StateFlag=StateIDFromState(State);
	::DrawFrameControl(DC,(LPRECT)&Rect,DFC_CAPTION,DFCS_CAPTIONMAX|StateFlag);
}
//---------------------------------------------------------------------------
void cWinNCButtonPainter_Normal::PaintRestoreButton(HDC DC,const RECT &Rect,eButtonState State)noexcept
{
	auto StateFlag=StateIDFromState(State);
	::DrawFrameControl(DC,(LPRECT)&Rect,DFC_CAPTION,DFCS_CAPTIONRESTORE|StateFlag);
}
//---------------------------------------------------------------------------
void cWinNCButtonPainter_Normal::PaintSmallCloseButton(HDC DC,const RECT &Rect,eButtonState State)noexcept
{
	auto StateFlag=StateIDFromState(State);
	::DrawFrameControl(DC,(LPRECT)&Rect,DFC_CAPTION,DFCS_CAPTIONCLOSE|StateFlag);
}
//---------------------------------------------------------------------------
void cWinNCButtonPainter_Normal::PaintHelpButton(HDC DC,const RECT &Rect,eButtonState State)noexcept
{
	auto StateFlag=StateIDFromState(State);
	::DrawFrameControl(DC,(LPRECT)&Rect,DFC_CAPTION,DFCS_CAPTIONHELP|StateFlag);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
vWinNCButton::vWinNCButton(viButtonData *Data)noexcept
	: bvWinThemeButton(Data)
{
}
//---------------------------------------------------------------------------
vWinNCButton::~vWinNCButton()noexcept
{
}
//---------------------------------------------------------------------------
rPtr<viControl> vWinNCButton::Create(viButtonData *Data)noexcept
{
	return rCreate< bwvControl<vWinNCButton> >(Data);
}
//---------------------------------------------------------------------------
void vWinNCButton::ThemeSetup(HWND WindowHandle)noexcept
{
	HTHEME Theme=nullptr;
	if(::IsAppThemed()){
		Theme=cWinNCButtonPainter_Theme::OpenTheme(WindowHandle);
	}
	if(Theme!=nullptr){
		fPainter=new cWinNCButtonPainter_Theme(Theme);
	}
	else{
		fPainter=new cWinNCButtonPainter_Normal;
	}
}
//---------------------------------------------------------------------------
void vWinNCButton::ThemeClear(void)noexcept
{
	delete fPainter;
	fPainter=nullptr;
}
//---------------------------------------------------------------------------
void vWinNCButton::Paint(HDC DC,HRGN)noexcept
{
	eButtonState State;
	if(fButtonData!=nullptr){
		State=fButtonData->ButtonState();
	}
	else{
		State=ButtonState::Disabled;
	}
	switch(ButtonType){
	case NCButtonType::Close:
		fPainter->PaintCloseButton(DC,fPaintRC,State);
		break;
	case NCButtonType::Min:
		fPainter->PaintMinButton(DC,fPaintRC,State);
		break;
	case NCButtonType::Max:
		fPainter->PaintMaxButton(DC,fPaintRC,State);
		break;
	case NCButtonType::Restore:
		fPainter->PaintRestoreButton(DC,fPaintRC,State);
		break;
	case NCButtonType::SmallClose:
		fPainter->PaintSmallCloseButton(DC,fPaintRC,State);
		break;
	case NCButtonType::Help:
		fPainter->PaintHelpButton(DC,fPaintRC,State);
		break;
	}
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
uIntn cnUI::WinNCGetButtonList(HWND WindowHandle,uIntn ListCount,eNCButtonType *ListBuffer)noexcept
{
	DWORD Style=::GetWindowLongW(WindowHandle,GWL_STYLE);
	DWORD ExStyle=::GetWindowLongW(WindowHandle,GWL_EXSTYLE);

	uIntn ButtonIndex=0;

	//if(ExStyle&WS_EX_TOOLWINDOW){
	//	if(ExStyle&WS_EX_CONTEXTHELP){
	//		ListBuffer[ButtonIndex]=NCButtonType::Help;
	//		ButtonIndex++;
	//		if(ButtonIndex>=ListCount)
	//			return ListCount;
	//	}
	//	if(Style&WS_MINIMIZEBOX){
	//		ListBuffer[ButtonIndex]=NCButtonType::Min;
	//		ButtonIndex++;
	//		if(ButtonIndex>=ListCount)
	//			return ListCount;
	//	}
	//	if(Style&WS_MAXIMIZE){
	//		ListBuffer[ButtonIndex]=NCButtonType::Restore;
	//		ButtonIndex++;
	//		if(ButtonIndex>=ListCount)
	//			return ListCount;
	//	}
	//
	//	if(Style&WS_MAXIMIZE){
	//		ListBuffer[ButtonIndex]=NCButtonType::Restore;
	//		ButtonIndex++;
	//		if(ButtonIndex>=ListCount)
	//			return ListCount;
	//	}
	//	if(Style&WS_SYSMENU){
	//		ListBuffer[ButtonIndex]=NCButtonType::SmallClose;
	//		ButtonIndex++;
	//		if(ButtonIndex>=ListCount)
	//			return ListCount;
	//	}
	//}
	//else{
		if(ExStyle&WS_EX_CONTEXTHELP){
			ListBuffer[ButtonIndex]=NCButtonType::Help;
			ButtonIndex++;
			if(ButtonIndex>=ListCount)
				return ListCount;
		}
		if(Style&WS_MINIMIZE){
			ListBuffer[ButtonIndex]=NCButtonType::Restore;
			ButtonIndex++;
			if(ButtonIndex>=ListCount)
				return ListCount;
		}
		else if(Style&WS_MINIMIZEBOX){
			ListBuffer[ButtonIndex]=NCButtonType::Min;
			ButtonIndex++;
			if(ButtonIndex>=ListCount)
				return ListCount;
		}
		if(Style&WS_MAXIMIZE){
			ListBuffer[ButtonIndex]=NCButtonType::Restore;
			ButtonIndex++;
			if(ButtonIndex>=ListCount)
				return ListCount;
		}
		else if(Style&WS_MAXIMIZEBOX){
			ListBuffer[ButtonIndex]=NCButtonType::Max;
			ButtonIndex++;
			if(ButtonIndex>=ListCount)
				return ListCount;
		}
		if(Style&WS_SYSMENU){
			ListBuffer[ButtonIndex]=NCButtonType::Close;
			ButtonIndex++;
			if(ButtonIndex>=ListCount)
				return ListCount;
		}
	//}
	return ButtonIndex;
}
//---------------------------------------------------------------------------
uIntn cnUI::WinNCGetButtonList(iUIView *View,uIntn ListCount,eNCButtonType *ListBuffer)noexcept
{
	auto WindowHandle=GetWindowHandleFromUIView(View);
	return WinNCGetButtonList(WindowHandle,ListCount,ListBuffer);
}
//---------------------------------------------------------------------------
void cnUI::WinNCCommand(iUIView *View,const cUIPoint *Pos,eNCButtonType ButtonType)noexcept
{
	HWND WindowHandle=GetWindowHandleFromUIView(View);
	if(WindowHandle==nullptr)
		return;
	WPARAM CommandWParam;
	switch(ButtonType){
	default:
		return;
	case NCButtonType::Close:
		CommandWParam=SC_CLOSE;
		break;
	case NCButtonType::Min:
		CommandWParam=SC_MINIMIZE;
		break;
	case NCButtonType::Max:
		CommandWParam=SC_MAXIMIZE;
		break;
	case NCButtonType::Restore:
		CommandWParam=SC_RESTORE;
		break;
	case NCButtonType::SmallClose:
		CommandWParam=SC_CLOSE;
		break;
	case NCButtonType::Help:
		CommandWParam=SC_CONTEXTHELP;
		break;
	}
	LPARAM lParam;
	if(Pos!=nullptr){
		cUIPoint ScreenPoint;
		View->GetPosition(nullptr,ScreenPoint);

		ScreenPoint.x+=Pos->x;
		ScreenPoint.y+=Pos->y;
		POINTS pts;
		pts.x=static_cast<SHORT>(ScreenPoint.x);
		pts.y=static_cast<SHORT>(ScreenPoint.y);
		lParam=reinterpret_cast<LPARAM&>(pts);
	}
	else{
		lParam=1;
	}
	::PostMessageW(WindowHandle,WM_SYSCOMMAND,CommandWParam,lParam);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cWinNCButton::cWinNCButton()noexcept
	: fNCButtonContent(this)
{
	fNCButtonContent.ButtonType=NCButtonType::Help;
}
//---------------------------------------------------------------------------
cWinNCButton::~cWinNCButton()noexcept
{
	SetView(nullptr);
}
//---------------------------------------------------------------------------
void cWinNCButton::SetButtonType(eNCButtonType ButtonType)noexcept
{
	fNCButtonContent.ButtonType=ButtonType;
	fNCButtonContent.Update();
}
//---------------------------------------------------------------------------
eButtonState cWinNCButton::ButtonState(void)noexcept
{
	auto State=GetButtonState();
	if(State==ButtonState::Normal){
		auto ViewState=fView->GetUIState();
		if(ViewState<UIState::Active){
			State=ButtonState::Inactive;
		}
	}
	return State;
}
//---------------------------------------------------------------------------
void cWinNCButton::ViewSetup(void)noexcept
{
	bcButton::ViewSetup();

	fView->InsertStateHandler(this);

	fNCButtonContent.SetView(fView);
}
//---------------------------------------------------------------------------
void cWinNCButton::ViewClear(void)noexcept
{
	fNCButtonContent.SetView(nullptr);

	fView->RemoveStateHandler(this);
	bcButton::ViewClear();
}
//---------------------------------------------------------------------------
void cWinNCButton::UIResume(void)noexcept
{
	// update for activation changed
	fNCButtonContent.Update();
}
//---------------------------------------------------------------------------
void cWinNCButton::UIPaused(void)noexcept
{
	// update for activation changed
	fNCButtonContent.Update();
}
//---------------------------------------------------------------------------
void cWinNCButton::ButtonClick(const cUIPoint &Pos)noexcept
{
	// perform standard non-client button commands
	WinNCCommand(fView,&Pos,fNCButtonContent.ButtonType);
}
//---------------------------------------------------------------------------
void cWinNCButton::ButtonBackgroundSetDefault(void)noexcept
{
	// no default background
}
//---------------------------------------------------------------------------
bool cWinNCButton::ButtonMouseAllowButton(eMouseButton Button)noexcept
{
	return Button==MouseButton::Left;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
rPtr<iGDIFontHandle> cnUI::vSetupTextControl(SIZE &TextSize,const wchar_t *Text,uIntn TextLength,const cUITextStyle &TextStyle)noexcept
{
	auto Font=QueryGDIFont(TextStyle);

	auto Device=cnWindows::QueryDCPaintDevice();
	auto ScreenIC=Device->GetScreenInfomationDC();
	auto DC=ScreenIC->GetDC();

	auto OldFont=::SelectObject(DC,Font->GetFontHandle());

	if(Text==nullptr || TextLength==0 || ::GetTextExtentExPointW(DC,Text,static_cast<int>(TextLength),0,nullptr,nullptr,&TextSize)==false){
		TextSize.cx=0;
		TextSize.cy=0;
	}


	::SelectObject(DC,OldFont);
	
	return Font;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cWinPushButtonPainter_Normal::cWinPushButtonPainter_Normal()noexcept
{
}
cWinPushButtonPainter_Normal::~cWinPushButtonPainter_Normal()noexcept
{
}
//---------------------------------------------------------------------------
cUIRectangle cWinPushButtonPainter_Normal::GetContentMarginRect(void)noexcept
{
	return {2,2,2,2};
}
//---------------------------------------------------------------------------
void cWinPushButtonPainter_Normal::PaintButton(HDC DC,const RECT &Rect,eButtonState State)noexcept
{
	UINT StateFlag=0;

	switch(State){
	default:
	case ButtonState::Inactive:
	case ButtonState::Normal:
		StateFlag=0;
		break;
	case ButtonState::Hot:
		StateFlag=DFCS_HOT;
		break;
	case ButtonState::Pressed:
		StateFlag=DFCS_PUSHED;
		break;
	case ButtonState::Disabled:
		StateFlag=DFCS_INACTIVE;
		break;
	}
	::DrawFrameControl(DC,(LPRECT)&Rect,DFC_BUTTON,DFCS_BUTTONPUSH|StateFlag);
}
//---------------------------------------------------------------------------
void cWinPushButtonPainter_Normal::PaintText(HDC DC,const RECT &TextRect,const wchar_t *Text,uIntn TextLength,HFONT TextFont,eButtonState State)noexcept
{
	RECT DrawRect;
	if(State==ButtonState::Pressed){
		DrawRect.left=TextRect.left+2;
		DrawRect.top=TextRect.top+2;
		DrawRect.right=TextRect.right;
		DrawRect.bottom=TextRect.bottom;
	}
	else{
		DrawRect.left=TextRect.left;
		DrawRect.top=TextRect.top;
		DrawRect.right=TextRect.right-2;
		DrawRect.bottom=TextRect.bottom-2;
	}
	HGDIOBJ OldFont=nullptr;
	if(TextFont!=nullptr){
		OldFont=::SelectObject(DC,TextFont);
	}

	::DrawTextW(DC,Text,static_cast<int>(TextLength),&DrawRect,DT_SINGLELINE|DT_CENTER|DT_VCENTER);
	
	if(OldFont!=nullptr){
		::SelectObject(DC,OldFont);
	}
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
vWinPushButton::vWinPushButton(viButtonData *Data)noexcept
	: bvWinThemeButton(Data)
{
}
//---------------------------------------------------------------------------
vWinPushButton::~vWinPushButton()noexcept
{
}
//---------------------------------------------------------------------------
rPtr<viControl> vWinPushButton::Create(viButtonData *Data)noexcept
{
	return rCreate< bwvControl<vWinPushButton> >(Data);
}
//---------------------------------------------------------------------------
void vWinPushButton::SetView(iUIView *View)noexcept
{
	ViewControl::SetView(View);
	vWinDCThemePainter::SetView(View);
}
//---------------------------------------------------------------------------
void vWinPushButton::ThemeSetup(HWND WindowHandle)noexcept
{
	HTHEME Theme=nullptr;
	if(WindowHandle!=nullptr && ::IsAppThemed()){
		Theme=cWinPushButtonPainter_Theme::OpenTheme(WindowHandle);
	}
	if(Theme!=nullptr){
		fPainter=new cWinPushButtonPainter_Theme(Theme);
	}
	else{
		fPainter=new cWinPushButtonPainter_Normal;
	}
}
//---------------------------------------------------------------------------
void vWinPushButton::ThemeClear(void)noexcept
{
	delete fPainter;
	fPainter=nullptr;
}
//---------------------------------------------------------------------------
void vWinPushButton::Paint(HDC DC,HRGN)noexcept
{
	eButtonState State;
	if(fButtonData!=nullptr){
		State=fButtonData->ButtonState();
	}
	else{
		State=ButtonState::Disabled;
	}
	fPainter->PaintButton(DC,fPaintRC,State);
}
//---------------------------------------------------------------------------
cUIRectangle vWinPushButton::UIMargin(const cUIRectangle &Margin)noexcept
{
	if(fPainter==nullptr)
		return UIRectangleZero;

	auto FrameMargin=fPainter->GetContentMarginRect();
	return MergeMargin(FrameMargin,Margin);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void cWinTextButton::ButtonBackgroundSetDefault(void)noexcept
{
	SetBackground(vWinPushButton::Create(this));
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void cWinImageTextButton::ButtonBackgroundSetDefault(void)noexcept
{
	SetBackground(vWinPushButton::Create(this));
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
bvWinThemeStateButton::bvWinThemeStateButton(viButtonData *ButtonData,viImageTextControlData *TextData)noexcept
	: bvWinThemeButton(ButtonData)
	, fTextData(TextData)
{
	ImageTextDataInsertCallback();
}
//---------------------------------------------------------------------------
bvWinThemeStateButton::~bvWinThemeStateButton()noexcept
{
	ImageTextDataRemoveCallback();
}
//---------------------------------------------------------------------------
viImageTextControlData* bvWinThemeStateButton::GetImageTextData(void)const noexcept
{
	return fTextData;
}
//---------------------------------------------------------------------------
void bvWinThemeStateButton::SetImageTextData(viImageTextControlData *Data)noexcept
{
	if(fTextData==Data)
		return;

	ImageTextDataRemoveCallback();
	fTextData=Data;
	ImageTextDataInsertCallback();

	Update();
}
//---------------------------------------------------------------------------
void bvWinThemeStateButton::ImageTextDataInsertCallback(void)noexcept
{
	if(fTextData!=nullptr){
		fImageTextNotifyToken=fTextData->ControlImageTextNotifySet.Insert([this]{
			Update();
		});
	}
}
//---------------------------------------------------------------------------
void bvWinThemeStateButton::ImageTextDataRemoveCallback(void)noexcept
{
	if(fTextData!=nullptr){
		fTextData->ControlImageTextNotifySet.Remove(fImageTextNotifyToken);
	}
}
//---------------------------------------------------------------------------
void bvWinThemeStateButton::Update(void)noexcept
{
	fFont=nullptr;
	fDCViewContent->InvalidateRect(&fPaintRC);
}
//---------------------------------------------------------------------------
void bvWinThemeStateButton::UpdateState(void)noexcept
{
	fDCViewContent->InvalidateRect(&fPaintRC);
}
//---------------------------------------------------------------------------
void bvWinThemeStateButton::SetupText(void)noexcept
{
	if(fFont!=nullptr)
		return;
	if(fTextData==nullptr)
		return;
	auto &TextStyle=fTextData->ControlTextStyle();
	auto String=fTextData->ControlTextString();
	auto Length=fTextData->ControlTextLength();
	fFont=vSetupTextControl(fTextSize,cnRTL::utow(String),Length,TextStyle);
}
//---------------------------------------------------------------------------
void bvWinThemeStateButton::SetupButtonRects(RECT &BoxRect,RECT &TextRect,const cUIPoint &BoxSize)noexcept
{
	cUIPoint TextSize;
	TextSize.x=static_cast<Float32>(fTextSize.cx);
	TextSize.y=static_cast<Float32>(fTextSize.cy);
	cLayoutImageTextOptions Option=fTextData->ControlImageTextOption();

	cUIPoint TextPos;
	cUIPoint BoxPos;
	cUIPoint PaintSize;
	PaintSize.x=static_cast<Float32>(fPaintRC.right-fPaintRC.left);
	PaintSize.y=static_cast<Float32>(fPaintRC.bottom-fPaintRC.top);
	LayoutImageText(TextPos,BoxPos,TextSize,BoxSize,PaintSize,Option);

	// state box
	BoxRect.left=fPaintRC.left+static_cast<sfInt32>(BoxPos.x);
	BoxRect.top=fPaintRC.top+static_cast<sfInt32>(BoxPos.y);
	BoxRect.right=BoxRect.left+static_cast<sfInt32>(BoxSize.x);
	BoxRect.bottom=BoxRect.top+static_cast<sfInt32>(BoxSize.y);

	// text
	TextRect.left=fPaintRC.left+static_cast<sfInt32>(TextPos.x);
	TextRect.top=fPaintRC.top+static_cast<sfInt32>(TextPos.y);
	TextRect.right=TextRect.left+static_cast<sfInt32>(TextSize.x);
	TextRect.bottom=TextRect.top+static_cast<sfInt32>(TextSize.y);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cWinStateButton::cWinStateButton()noexcept
{
}
//---------------------------------------------------------------------------
cWinStateButton::~cWinStateButton()noexcept
{
}
//---------------------------------------------------------------------------
void cWinStateButton::ButtonBackgroundSetDefault(void)noexcept
{
	// no button background
}
//---------------------------------------------------------------------------
bool cWinStateButton::ButtonMouseAllowButton(eMouseButton Button)noexcept
{
	return Button==MouseButton::Left;
}
//---------------------------------------------------------------------------
void cWinStateButton::ButtonClick(const cUIPoint &)noexcept
{
	if(OnClick!=nullptr){
		OnClick();
	}
}
//---------------------------------------------------------------------------
ufInt8 cWinStateButton::CheckState(void)noexcept
{
	return Check;
}
//---------------------------------------------------------------------------
const uChar16* cWinStateButton::ControlTextString(void)noexcept
{
	return Text;
}
//---------------------------------------------------------------------------
uIntn cWinStateButton::ControlTextLength(void)noexcept
{
	return Text->Length;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cWinCheckButtonPainter_Normal::cWinCheckButtonPainter_Normal()noexcept
{
}
cWinCheckButtonPainter_Normal::~cWinCheckButtonPainter_Normal()
{
}
//---------------------------------------------------------------------------
cUIPoint cWinCheckButtonPainter_Normal::GetBoxSize(HDC)noexcept
{
	return {14,14};
}
//---------------------------------------------------------------------------
void cWinCheckButtonPainter_Normal::PaintButton(HDC DC,const RECT &Rect,eButtonState State,ufInt8 CheckState)noexcept
{
	static constexpr int CheckStateMap[3]={
		DFCS_BUTTONCHECK,				// Unchecked
		DFCS_BUTTONCHECK|DFCS_CHECKED,	// Checked
		DFCS_BUTTON3STATE|DFCS_CHECKED,	// Mixed
	};

	UINT StateFlag=DFCS_BUTTONCHECK;
	if(CheckState<3){
		StateFlag=CheckStateMap[CheckState];
	}

	switch(State){
	default:
	case ButtonState::Inactive:
	case ButtonState::Normal:
		break;
	case ButtonState::Hot:
		StateFlag|=DFCS_HOT;
		break;
	case ButtonState::Pressed:
		StateFlag|=DFCS_PUSHED;
		break;
	case ButtonState::Disabled:
		StateFlag|=DFCS_INACTIVE;
		break;
	}
	::DrawFrameControl(DC,(LPRECT)&Rect,DFC_BUTTON,StateFlag);
}
//---------------------------------------------------------------------------
void cWinCheckButtonPainter_Normal::PaintText(HDC DC,const RECT &TextRect,const wchar_t *Text,uIntn TextLength,HFONT TextFont,eButtonState State,uInt8 CheckState)noexcept
{State,CheckState;
	HGDIOBJ OldFont=nullptr;
	if(TextFont!=nullptr){
		OldFont=::SelectObject(DC,TextFont);
	}

	::ExtTextOutW(DC,TextRect.left,TextRect.top,ETO_CLIPPED,&TextRect,Text,static_cast<UINT>(TextLength),nullptr);
	
	if(OldFont!=nullptr){
		::SelectObject(DC,OldFont);
	}
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
vWinCheckBox::vWinCheckBox(viButtonData *ButtonData,viImageTextControlData *TextData)noexcept
	: bvWinThemeStateButton(ButtonData,TextData)
{
}
//---------------------------------------------------------------------------
vWinCheckBox::~vWinCheckBox()noexcept
{
}
//---------------------------------------------------------------------------
rPtr<viControl> vWinCheckBox::Create(viButtonData *ButtonData,viImageTextControlData *TextData)noexcept
{
	return rCreate< bwvControl<vWinCheckBox> >(ButtonData,TextData);
}
//---------------------------------------------------------------------------
void vWinCheckBox::ThemeSetup(HWND WindowHandle)noexcept
{
	HTHEME Theme=nullptr;
	if(::IsAppThemed()){
		Theme=cWinCheckButtonPainter_Theme::OpenTheme(WindowHandle);
	}
	if(Theme!=nullptr){
		fPainter=new cWinCheckButtonPainter_Theme(Theme);
	}
	else{
		fPainter=new cWinCheckButtonPainter_Normal;
	}
}
//---------------------------------------------------------------------------
void vWinCheckBox::ThemeClear(void)noexcept
{
	delete fPainter;
	fPainter=nullptr;
}
//---------------------------------------------------------------------------
void vWinCheckBox::Paint(HDC DC,HRGN)noexcept
{
	if(fTextData==nullptr)
		return;

	SetupText();


	RECT BoxRect;
	RECT TextRect;
	SetupButtonRects(BoxRect,TextRect,fPainter->GetBoxSize(DC));

	eButtonState BtnState;
	ufInt8 CheckState;
	if(fButtonData!=nullptr){
		BtnState=fButtonData->ButtonState();
		CheckState=fButtonData->CheckState();
	}
	else{
		BtnState=ButtonState::Disabled;
		CheckState=0;
	}
	fPainter->PaintButton(DC,BoxRect,BtnState,CheckState);

	auto Text=fTextData->ControlTextString();
	auto TextLength=fTextData->ControlTextLength();
	auto &TextStyle=fTextData->ControlTextStyle();
	if(Text!=nullptr && TextLength!=0){
		if(TextStyle.BackgroundColor.Alpha!=0){
			::SetBkMode(DC,OPAQUE);
			::SetBkColor(DC,COLORREFFromUIColor(TextStyle.BackgroundColor));
		}
		else{
			::SetBkMode(DC,TRANSPARENT);
		}
		::SetTextColor(DC,COLORREFFromUIColor(TextStyle.TextColor));
		fPainter->PaintText(DC,TextRect,utow(Text),TextLength,fFont->GetFontHandle(),BtnState,CheckState);
	}
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cWinCheckButton::cWinCheckButton()noexcept
{
	TextAlign=Alignment::CenterLeft;
}
//---------------------------------------------------------------------------
void cWinCheckButton::ControlContentSetDefault(void)noexcept
{
	SetContent(vWinCheckBox::Create(this,this));
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cWinRadioButtonPainter_Normal::cWinRadioButtonPainter_Normal()noexcept
{
}
cWinRadioButtonPainter_Normal::~cWinRadioButtonPainter_Normal()noexcept
{
}
//---------------------------------------------------------------------------
cUIPoint cWinRadioButtonPainter_Normal::GetBoxSize(HDC)noexcept
{
	return {14,14};
}
//---------------------------------------------------------------------------
void cWinRadioButtonPainter_Normal::PaintButton(HDC DC,const RECT &Rect,eButtonState State,ufInt8 CheckState)noexcept
{
	UINT StateFlag=0;
	if(CheckState){
		StateFlag=DFCS_CHECKED;
	}

	switch(State){
	default:
	case ButtonState::Inactive:
	case ButtonState::Normal:
		break;
	case ButtonState::Hot:
		StateFlag|=DFCS_HOT;
		break;
	case ButtonState::Pressed:
		StateFlag|=DFCS_PUSHED;
		break;
	case ButtonState::Disabled:
		StateFlag|=DFCS_INACTIVE;
		break;
	}
	
	::DrawFrameControl(DC,(LPRECT)&Rect,DFC_BUTTON,DFCS_BUTTONRADIO|StateFlag);
}
//---------------------------------------------------------------------------
void cWinRadioButtonPainter_Normal::PaintText(HDC DC,const RECT &TextRect,const wchar_t *Text,uIntn TextLength,HFONT TextFont,eButtonState State,uInt8 CheckState)noexcept
{State,CheckState;
	HGDIOBJ OldFont=nullptr;
	if(TextFont!=nullptr){
		OldFont=::SelectObject(DC,TextFont);
	}

	::ExtTextOutW(DC,TextRect.left,TextRect.top,ETO_CLIPPED,&TextRect,Text,static_cast<UINT>(TextLength),nullptr);
	
	if(OldFont!=nullptr){
		::SelectObject(DC,OldFont);
	}
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
vWinRadioButton::vWinRadioButton(viButtonData *ButtonData,viImageTextControlData *TextData)noexcept
	: bvWinThemeStateButton(ButtonData,TextData)
{
}
//---------------------------------------------------------------------------
vWinRadioButton::~vWinRadioButton()
{
}
//---------------------------------------------------------------------------
rPtr<viControl> vWinRadioButton::Create(viButtonData *ButtonData,viImageTextControlData *TextData)noexcept
{
	return rCreate< bwvControl<vWinRadioButton> >(ButtonData,TextData);
}
//---------------------------------------------------------------------------
void vWinRadioButton::ThemeSetup(HWND WindowHandle)noexcept
{
	HTHEME Theme=nullptr;
	if(::IsAppThemed()){
		Theme=cWinRadioButtonPainter_Theme::OpenTheme(WindowHandle);
	}
	if(Theme!=nullptr){
		fPainter=new cWinRadioButtonPainter_Theme(Theme);
	}
	else{
		fPainter=new cWinRadioButtonPainter_Normal;
	}
}
//---------------------------------------------------------------------------
void vWinRadioButton::ThemeClear(void)noexcept
{
	delete fPainter;
	fPainter=nullptr;
}
//---------------------------------------------------------------------------
void vWinRadioButton::Paint(HDC DC,HRGN)noexcept
{
	if(fTextData==nullptr)
		return;

	SetupText();
	
	RECT BoxRect;
	RECT TextRect;
	SetupButtonRects(BoxRect,TextRect,fPainter->GetBoxSize(DC));

	eButtonState BtnState;
	ufInt8 CheckState;
	if(fButtonData!=nullptr){
		BtnState=fButtonData->ButtonState();
		CheckState=fButtonData->CheckState();
	}
	else{
		BtnState=ButtonState::Disabled;
		CheckState=0;
	}
	fPainter->PaintButton(DC,BoxRect,BtnState,CheckState);

	auto Text=fTextData->ControlTextString();
	auto TextLength=fTextData->ControlTextLength();
	auto &TextStyle=fTextData->ControlTextStyle();
	if(Text!=nullptr && TextLength!=0){
		if(TextStyle.BackgroundColor.Alpha!=0){
			::SetBkMode(DC,OPAQUE);
			::SetBkColor(DC,COLORREFFromUIColor(TextStyle.BackgroundColor));
		}
		else{
			::SetBkMode(DC,TRANSPARENT);
		}
		::SetTextColor(DC,COLORREFFromUIColor(TextStyle.TextColor));
		fPainter->PaintText(DC,TextRect,utow(Text),TextLength,fFont->GetFontHandle(),BtnState,CheckState);
	}
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cWinRadioButton::cWinRadioButton()noexcept
{
	TextAlign=Alignment::CenterLeft;
}
//---------------------------------------------------------------------------
void cWinRadioButton::ControlContentSetDefault(void)noexcept
{
	SetContent(vWinRadioButton::Create(this,this));
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cWinSpinButtonPainter_Normal::cWinSpinButtonPainter_Normal()noexcept
{
}
cWinSpinButtonPainter_Normal::~cWinSpinButtonPainter_Normal()noexcept
{
}
//---------------------------------------------------------------------------
UINT cWinSpinButtonPainter_Normal::ButtonStateFlag(eButtonState State)noexcept
{
	UINT StateFlag=0;

	switch(State){
	default:
	case ButtonState::Inactive:
	case ButtonState::Normal:
		break;
	case ButtonState::Hot:
		StateFlag|=DFCS_HOT;
		break;
	case ButtonState::Pressed:
		StateFlag|=DFCS_PUSHED;
		break;
	case ButtonState::Disabled:
		StateFlag|=DFCS_INACTIVE;
		break;
	}
	return StateFlag;
}
//---------------------------------------------------------------------------
void cWinSpinButtonPainter_Normal::PaintUpButton(HDC DC,const RECT &Rect,eButtonState State)noexcept
{
	UINT StateFlag=ButtonStateFlag(State);
	::DrawFrameControl(DC,(LPRECT)&Rect,DFC_SCROLL,DFCS_SCROLLUP|StateFlag);
}
//---------------------------------------------------------------------------
void cWinSpinButtonPainter_Normal::PaintDownButton(HDC DC,const RECT &Rect,eButtonState State)noexcept
{
	UINT StateFlag=ButtonStateFlag(State);
	::DrawFrameControl(DC,(LPRECT)&Rect,DFC_SCROLL,DFCS_SCROLLDOWN|StateFlag);
}
//---------------------------------------------------------------------------
void cWinSpinButtonPainter_Normal::PaintLeftButton(HDC DC,const RECT &Rect,eButtonState State)noexcept
{
	UINT StateFlag=ButtonStateFlag(State);
	::DrawFrameControl(DC,(LPRECT)&Rect,DFC_SCROLL,DFCS_SCROLLLEFT|StateFlag);
}
//---------------------------------------------------------------------------
void cWinSpinButtonPainter_Normal::PaintRightButton(HDC DC,const RECT &Rect,eButtonState State)noexcept
{
	UINT StateFlag=ButtonStateFlag(State);
	::DrawFrameControl(DC,(LPRECT)&Rect,DFC_SCROLL,DFCS_SCROLLRIGHT|StateFlag);
}
//---------------------------------------------------------------------------
#if 0
//---------------------------------------------------------------------------
vWinSpinButton::vWinSpinButton(viSpinData *Data)
{
	fData=Data;
}
//---------------------------------------------------------------------------
vWinSpinButton::~vWinSpinButton()
{
}
//---------------------------------------------------------------------------
rPtr<viControl> vWinSpinButton::Create(viSpinData *Data)
{
	return rCreate<vWinSpinButton>(Data);
}
//---------------------------------------------------------------------------
void vWinSpinButton::ThemeSetup(HWND WindowHandle)
{
	HTHEME Theme=nullptr;
	if(::IsAppThemed()){
		Theme=cWinSpinButtonPainter_Theme::OpenTheme(WindowHandle);
	}
	if(Theme!=nullptr){
		fPainter=new cWinSpinButtonPainter_Theme(Theme);
	}
	else{
		fPainter=new cWinSpinButtonPainter_Normal;
	}
}
//---------------------------------------------------------------------------
void vWinSpinButton::ThemeClear(void)
{
	delete fPainter;
	fPainter=nullptr;
}
//---------------------------------------------------------------------------
void vWinSpinButton::SetView(iUIView *View)
{
	fDCViewContent->SetView(View);
}
//---------------------------------------------------------------------------
void vWinSpinButton::SetZIndex(sfInt32n ZIndex)
{
	fDCViewContent->SetZIndex(ZIndex);
}
//---------------------------------------------------------------------------
void vWinSpinButton::SetVisible(bool Visible)
{
	fDCViewContent->SetVisible(Visible);
}
//---------------------------------------------------------------------------
void vWinSpinButton::Update(void)
{
	fDCViewContent->InvalidateRect(&fPaintRC);
}
//---------------------------------------------------------------------------
void vWinSpinButton::Paint(HDC DC)
{
	auto ButtonState=fData->ButtonState();
	fPainter->PaintUpButton(DC,fPaintRC,ButtonState);
}
#endif // 0
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
vWinDirectionButton::vWinDirectionButton(viButtonData *ButtonData)noexcept
	: bvWinThemeButton(ButtonData)
{
}
//---------------------------------------------------------------------------
vWinDirectionButton::~vWinDirectionButton()noexcept
{
}
//---------------------------------------------------------------------------
rPtr<viControl> vWinDirectionButton::Create(viButtonData *ButtonData,eDirection PointerDirection)noexcept
{
	auto v=rCreate< bwvControl<vWinDirectionButton> >(ButtonData);
	v->PointerDirection=PointerDirection;
	return v;
}
//---------------------------------------------------------------------------
void vWinDirectionButton::ThemeSetup(HWND WindowHandle)noexcept
{
	HTHEME Theme=nullptr;
	if(::IsAppThemed()){
		Theme=cWinSpinButtonPainter_Theme::OpenTheme(WindowHandle);
	}
	if(Theme!=nullptr){
		fPainter=new cWinSpinButtonPainter_Theme(Theme);
	}
	else{
		fPainter=new cWinSpinButtonPainter_Normal;
	}
}
//---------------------------------------------------------------------------
void vWinDirectionButton::ThemeClear(void)noexcept
{
	delete fPainter;
	fPainter=nullptr;
}
//---------------------------------------------------------------------------
void vWinDirectionButton::Update(void)noexcept
{
	fDCViewContent->InvalidateRect(&fPaintRC);
}
//---------------------------------------------------------------------------
void vWinDirectionButton::Paint(HDC DC,HRGN)noexcept
{
	eButtonState BtnState;
	if(fButtonData!=nullptr){
		BtnState=fButtonData->ButtonState();
	}
	else{
		BtnState=ButtonState::Disabled;
	}
	switch(PointerDirection){
	case Direction::Up:
		fPainter->PaintUpButton(DC,fPaintRC,BtnState);
		break;
	case Direction::Down:
		fPainter->PaintDownButton(DC,fPaintRC,BtnState);
		break;
	case Direction::Left:
		fPainter->PaintLeftButton(DC,fPaintRC,BtnState);
		break;
	case Direction::Right:
		fPainter->PaintRightButton(DC,fPaintRC,BtnState);
		break;
	}
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cWinEditBackgroundPainter_Normal::cWinEditBackgroundPainter_Normal()noexcept
{
}
//---------------------------------------------------------------------------
cWinEditBackgroundPainter_Normal::~cWinEditBackgroundPainter_Normal()noexcept
{
}
//---------------------------------------------------------------------------
cUIRectangle cWinEditBackgroundPainter_Normal::GetContentMarginRect(void)noexcept
{
	return {2,2,2,2};
}
//---------------------------------------------------------------------------
void cWinEditBackgroundPainter_Normal::Paint(HDC DC,const RECT &Rect,ControlState State)noexcept
{State;
	auto OldPen=::SelectObject(DC,::GetStockObject(BLACK_PEN));
	auto OldBrush=::SelectObject(DC,::GetStockObject(WHITE_BRUSH));
	::Rectangle(DC,Rect.left,Rect.top,Rect.right,Rect.bottom);
	::SelectObject(DC,OldPen);
	::SelectObject(DC,OldBrush);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
vWinEditBackground::vWinEditBackground(viControlStateData *Data)noexcept
	: fData(Data)
{
	DataInsertCallback();
}
//---------------------------------------------------------------------------
vWinEditBackground::~vWinEditBackground()noexcept
{
	DataRemoveCallback();
}
//---------------------------------------------------------------------------
rPtr<viControl> vWinEditBackground::Create(viControlStateData *Data)noexcept
{
	return rCreate< bwvControl<vWinEditBackground> >(Data);
}
//---------------------------------------------------------------------------
void vWinEditBackground::SetView(iUIView *View)noexcept
{
	ViewControl::SetView(View);
	vWinDCThemePainter::SetView(View);
}
//---------------------------------------------------------------------------
viControlStateData* vWinEditBackground::GetData(void)const noexcept
{
	return fData;
}
//---------------------------------------------------------------------------
void vWinEditBackground::SetData(viControlStateData *Data)noexcept
{
	if(fData==Data)
		return;

	DataRemoveCallback();
	fData=Data;
	DataInsertCallback();

	Update();
}
//---------------------------------------------------------------------------
void vWinEditBackground::Update(void)noexcept
{
	fDCViewContent->InvalidateRect(&fPaintRC);
}
//---------------------------------------------------------------------------
void vWinEditBackground::DataInsertCallback(void)noexcept
{
	if(fData!=nullptr){
		fControlStateDataNotifyToken=fData->ControlStateNotifySet.Insert([this]{
			Update();
		});
	}
}
//---------------------------------------------------------------------------
void vWinEditBackground::DataRemoveCallback(void)noexcept
{
	if(fData!=nullptr){
		fData->ControlStateNotifySet.Remove(fControlStateDataNotifyToken);
	}
}
//---------------------------------------------------------------------------
cUIRectangle vWinEditBackground::UIMargin(const cUIRectangle &Margin)noexcept
{
	if(fPainter==nullptr){
		return UIRectangleZero;
	}

	cUIRectangle FrameMargin=fPainter->GetContentMarginRect();
	return MergeMargin(FrameMargin,Margin);
}
//---------------------------------------------------------------------------
void vWinEditBackground::ThemeSetup(HWND WindowHandle)noexcept
{
	HTHEME Theme=nullptr;
	if(::IsAppThemed()){
		Theme=cWinEditBackgroundPainter_Theme::OpenTheme(WindowHandle);
	}
	if(Theme!=nullptr){
		fPainter=new cWinEditBackgroundPainter_Theme(Theme);
	}
	else{
		fPainter=new cWinEditBackgroundPainter_Normal;
	}
}
//---------------------------------------------------------------------------
void vWinEditBackground::ThemeClear(void)noexcept
{
	delete fPainter;
	fPainter=nullptr;
}
//---------------------------------------------------------------------------
void vWinEditBackground::Paint(HDC DC,HRGN)noexcept
{
	typedef cWinEditBackgroundPainter::ControlState ControlState;
	ControlState State;
	if(fData!=nullptr){
		if(fData->ControlFocused()){
			State=ControlState::Focused;
		}	
		else if(fData->ControlHot()){
			State=ControlState::Hot;
		}
		else if(fData->ControlDisabled()){
			State=ControlState::Disabled;
		}
		else{
			State=ControlState::Normal;
		}
	}
	else{
		State=ControlState::Disabled;
	}
	fPainter->Paint(DC,fPaintRC,State);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void lWinEditFrame::ControlBackgroundSetDefault(void)noexcept
{
	SetBackground(vWinEditBackground::Create(nullptr));
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
vWinTextCaret::vWinTextCaret(viTextCaretData *Data)noexcept
	: fData(Data)
{
	fVisible=true;
	fFocused=false;
	fCaretCreated=false;

	DataInsertCallback();
}
//---------------------------------------------------------------------------
vWinTextCaret::~vWinTextCaret()noexcept
{
	DataRemoveCallback();
	SetView(nullptr);
}
//---------------------------------------------------------------------------
rPtr<viControl> vWinTextCaret::Create(viTextCaretData *Data)noexcept
{
	return rCreate< bwvControl<vWinTextCaret> >(Data);
}
//---------------------------------------------------------------------------
void vWinTextCaret::CaretSetup(HWND WindowHandle,cUIPoint CaretSize)noexcept
{
	if(fCaretCreated==false){
		if(::CreateCaret(WindowHandle,nullptr,static_cast<sfInt32>(CaretSize.x),static_cast<sfInt32>(CaretSize.y))){
			// create caret failed
			fCaretCreated=true;
			fCaretVisible=false;
		}

	}
}
//---------------------------------------------------------------------------
void vWinTextCaret::CaretClear(void)noexcept
{
	if(fCaretCreated){
		::DestroyCaret();
		fCaretCreated=false;
	}
}
//---------------------------------------------------------------------------
void vWinTextCaret::StateFunction(void)noexcept
{
	if(fFocused==false || fVisible==false){
		CaretClear();
		return;
	}
	auto WindowHandle=GetWindowHandleFromUIView(fView);
	if(WindowHandle==nullptr){
		CaretClear();
		return;
	}

	cUIPoint CaretPos;
	cUIPoint CaretSize;
	auto CaretVisible=fData->TextCaretVisualPosition(CaretPos,CaretSize);
	if(CaretVisible){
		// create caret
		CaretSetup(WindowHandle,CaretSize);
		{	// update position
			cUIPoint ViewPos;
			auto Window=fView->GetWindow();
			fView->GetPosition(Window,ViewPos);
			::SetCaretPos(static_cast<sfInt32>(ViewPos.x+CaretPos.x),static_cast<sfInt32>(ViewPos.y+CaretPos.y));
		}
		if(fCaretVisible==false){
			fCaretVisible=true;
			::ShowCaret(WindowHandle);
		}
	}
	else{
		// hide caret if created
		if(fCaretVisible){
			fCaretVisible=false;
			::HideCaret(WindowHandle);
		}
	}
}
//---------------------------------------------------------------------------
void vWinTextCaret::DataInsertCallback(void)noexcept
{
	if(fData!=nullptr){
		fTextCaretNotifyToken=fData->TextCaretNotifySet.Insert([this]{
			StateFunction();
		});
	}
}
//---------------------------------------------------------------------------
void vWinTextCaret::DataRemoveCallback(void)noexcept
{
	if(fData!=nullptr){
		fData->TextCaretNotifySet.Remove(fTextCaretNotifyToken);
	}
}
//---------------------------------------------------------------------------
void vWinTextCaret::SetView(iUIView *View)noexcept
{
	if(fView!=nullptr){
		ViewRemoveKeyHandler(fView,this);
		fView->InsertViewHandler(this);
	}
	fView=View;
	if(fView!=nullptr){
		fView->InsertViewHandler(this);
		ViewInsertKeyHandler(fView,this);
		// update focus state
		auto WindowHandle=GetWindowHandleFromUIView(fView);
		if(WindowHandle!=nullptr){
			fFocused=::GetFocus()==WindowHandle;
		}
	}
	else{
		fFocused=false;
	}
	StateFunction();
}
//---------------------------------------------------------------------------
void vWinTextCaret::SetContentZPosition(Float32)noexcept
{
}
//---------------------------------------------------------------------------
void vWinTextCaret::SetContentVisible(bool Visible)noexcept
{
	fVisible=Visible;
	StateFunction();
}
//---------------------------------------------------------------------------
void vWinTextCaret::UILayout(void)noexcept
{
	StateFunction();
}
//---------------------------------------------------------------------------
void vWinTextCaret::KeyFocusEnter(iUIKeyEvent*)noexcept
{
	fFocused=true;
	StateFunction();
}
//---------------------------------------------------------------------------
void vWinTextCaret::KeyFocusLeave(iUIKeyEvent*)noexcept
{
	fFocused=false;
	StateFunction();
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cWinTabPainter_Normal::cWinTabPainter_Normal()noexcept
{
}
//---------------------------------------------------------------------------
cWinTabPainter_Normal::~cWinTabPainter_Normal()noexcept
{
}
//---------------------------------------------------------------------------
ufInt16n cWinTabPainter_Normal::TabBorderSize(void)noexcept
{
	return 4;
}
//---------------------------------------------------------------------------
void cWinTabPainter_Normal::PaintPanelHeader(HDC DC,const RECT &PanelRect)noexcept
{
	auto PenObject=fPanelHeaderPenRecycler.Query();

	auto OldPen=::SelectObject(DC,PenObject->Pen);
	::MoveToEx(DC,PanelRect.left,PanelRect.top,nullptr);
	::LineTo(DC,PanelRect.right,PanelRect.top);
	::SelectObject(DC,OldPen);
}
//---------------------------------------------------------------------------
void cWinTabPainter_Normal::TabToTextRect(RECT &Rect,TabState State)noexcept
{
	if(State==TabState::Active){
		Rect.left+=4;
		Rect.right-=4;
		Rect.bottom-=2;
	}
	else{
		Rect.left+=4;
		Rect.top+=2;
		Rect.right-=4;
		Rect.bottom-=2;
	}
}
//---------------------------------------------------------------------------
RECT cWinTabPainter_Normal::PaintTabItem(HDC DC,const RECT &Rect,TabState State)noexcept
{
	RECT PaintRect=Rect;

	switch(State){
	case TabState::Normal:
		PaintRect.top+=2;
		PaintRect.bottom-=1;
		::SetBkColor(DC,0x7F7F7F);
		::ExtTextOutW(DC,0,0,ETO_OPAQUE,&PaintRect,nullptr,0,nullptr);
		::DrawEdge(DC,&PaintRect,BDR_RAISEDINNER,BF_TOP|BF_LEFT|BF_RIGHT);
		break;
	case TabState::Hot:
		PaintRect.top+=2;
		PaintRect.bottom-=1;
		::SetBkColor(DC,::GetSysColor(COLOR_BTNFACE));
		::ExtTextOutW(DC,0,0,ETO_OPAQUE,&PaintRect,nullptr,0,nullptr);
		::DrawEdge(DC,&PaintRect,BDR_RAISEDINNER,BF_TOP|BF_LEFT|BF_RIGHT);
		break;
	case TabState::Active:
		PaintRect.left-=2;
		PaintRect.right+=2;
		::SetBkColor(DC,0xCCCCCC);
		::ExtTextOutW(DC,0,0,ETO_OPAQUE,&PaintRect,nullptr,0,nullptr);
		::DrawEdge(DC,&PaintRect,EDGE_RAISED,BF_TOP|BF_LEFT|BF_RIGHT);
		break;
	}


	TabToTextRect(PaintRect,State);
	return PaintRect;
}
//---------------------------------------------------------------------------
RECT cWinTabPainter_Normal::PaintItemLeft(HDC DC,const RECT &Rect,TabState State)noexcept
{
	return PaintTabItem(DC,Rect,State);
}
//---------------------------------------------------------------------------
RECT cWinTabPainter_Normal::PaintItemMiddle(HDC DC,const RECT &Rect,TabState State)noexcept
{
	return PaintTabItem(DC,Rect,State);
}
//---------------------------------------------------------------------------
RECT cWinTabPainter_Normal::PaintItemRight(HDC DC,const RECT &Rect,TabState State)noexcept
{
	return PaintTabItem(DC,Rect,State);
}
//---------------------------------------------------------------------------
RECT cWinTabPainter_Normal::PaintItemSingle(HDC DC,const RECT &Rect,TabState State)noexcept
{
	return PaintTabItem(DC,Rect,State);
}
//---------------------------------------------------------------------------
void cWinTabPainter_Normal::PaintItemText(HDC DC,const RECT &TextRect,const wchar_t *Text,uIntn TextLength,HFONT TextFont,TabState State)noexcept
{State;
	HGDIOBJ OldFont=nullptr;
	if(TextFont!=nullptr){
		OldFont=::SelectObject(DC,TextFont);
	}
	::SetTextColor(DC,0);
	::SetBkMode(DC,TRANSPARENT);

	DrawTextExW(DC,(LPWSTR)Text,static_cast<int>(TextLength),const_cast<LPRECT>(&TextRect),DT_SINGLELINE|DT_VCENTER|DT_CENTER,nullptr);

	if(OldFont!=nullptr){
		::SelectObject(DC,OldFont);
	}
}
//---------------------------------------------------------------------------
//--------------------------------------------------------------------------
vWinTab::vWinTab(viTabData *Data)noexcept
	: fData(Data)
{
	fUpdateTab=true;
	DataInsertCallback();
}
//---------------------------------------------------------------------------
vWinTab::~vWinTab()noexcept
{
	DataRemoveCallback();
}
//---------------------------------------------------------------------------
rPtr<viControl> vWinTab::Create(viTabData *Data)noexcept
{
	return rCreate< bwvControl<vWinTab> >(Data);
}
//---------------------------------------------------------------------------
viTabData* vWinTab::GetData(void)const noexcept
{
	return fData;
}
//---------------------------------------------------------------------------
void vWinTab::SetData(viTabData *Data)noexcept
{
	if(fData==Data)
		return;

	DataRemoveCallback();
	fData=Data;
	DataInsertCallback();

	Update();
}
//---------------------------------------------------------------------------
void vWinTab::DataInsertCallback(void)noexcept
{
	if(fData!=nullptr){
		fTabNotifyToken=fData->TabNotifySet.Insert([this]{
			Update();
		});
	}
}
//---------------------------------------------------------------------------
void vWinTab::DataRemoveCallback(void)noexcept
{
	if(fData!=nullptr){
		fData->TabNotifySet.Remove(fTabNotifyToken);
	}
}
//---------------------------------------------------------------------------
void vWinTab::Update(void)noexcept
{
	fTabCacheList.Clear();
	fUpdateTab=true;
	fDCViewContent->InvalidateRect(&fPaintRC);
}
//---------------------------------------------------------------------------
void vWinTab::UpdateState(void)noexcept
{
	fDCViewContent->InvalidateRect(&fPaintRC);
}
//---------------------------------------------------------------------------
void vWinTab::ThemeSetup(HWND WindowHandle)noexcept
{
	HTHEME Theme=nullptr;
	if(::IsAppThemed()){
		Theme=cWinTabPainter_Theme::OpenTheme(WindowHandle);
	}
	if(Theme!=nullptr){
		fPainter=new cWinTabPainter_Theme(Theme);
	}
	else{
		fPainter=new cWinTabPainter_Normal;
	}
}
//---------------------------------------------------------------------------
void vWinTab::ThemeClear(void)noexcept
{
	delete fPainter;
	fPainter=nullptr;
}
//---------------------------------------------------------------------------
void vWinTab::SetupTabCache(HDC DC)noexcept
{
	if(fData==nullptr){
		fTabCacheList.Clear();
		return;
	}
	auto TabCount=fData->TabCount();
	fTabCacheList.SetCount(TabCount);
	if(TabCount){
		auto BorderSize=fPainter->TabBorderSize();
		Float32 CurPos=4;
		for(uIntn i=0;i<TabCount;i++){
			auto Item=fData->TabGet(i);
			auto CacheItem=&fTabCacheList[i];
			CacheItem->Font=QueryGDIFont(Item.TextStyle);
			CacheItem->Text=cnRTL::cString<wchar_t>(cnRTL::utow(Item.Text),Item.TextLength);

			if(Item.TextLength!=0){
				::GetTextExtentPoint32W(DC,CacheItem->Text,static_cast<int>(Item.TextLength),&CacheItem->TextSize);
				CacheItem->TabSize=static_cast<Float32>(CacheItem->TextSize.cx);
			}
			else{
				CacheItem->TabSize=0;
				CacheItem->TextSize.cx=0;
				CacheItem->TextSize.cy=0;
			}
			CacheItem->TabSize+=Item.TextMargin*2;
			if(Item.TabSize>=CacheItem->TabSize){
				CacheItem->TabSize=static_cast<Float32>(Item.TabSize);
				CacheItem->TextPos=static_cast<sfInt16>((Item.TabSize-CacheItem->TextSize.cx)/2);
			}
			else{
				CacheItem->TextPos=Item.TextMargin;
			}

			
			CacheItem->TabSize+=BorderSize;
			CacheItem->TabPos=CurPos;

			CurPos+=CacheItem->TabSize;
		}

	}

	if(TabCount>1){
		fTabCacheList[0].Part=TabPart::Left;
		for(uIntn i=1,c=TabCount-1;i<c;i++){
			fTabCacheList[i].Part=TabPart::Middle;
		}
		fTabCacheList[TabCount-1].Part=TabPart::Right;
	}
	else if(TabCount==1){
		fTabCacheList[0].Part=TabPart::Single;
	}
}
//---------------------------------------------------------------------------
void vWinTab::Paint(HDC DC,HRGN)noexcept
{
	if(fPainter==nullptr)
		return;

	if(fData==nullptr)
		return;

	if(fUpdateTab){
		fUpdateTab=false;
		SetupTabCache(DC);
	}


	sfInt16 ActiveIndex=fData->TabActiveIndex();
	sfInt16 HotIndex=fData->TabHotIndex();

	{
		RECT PanelRect;
		PanelRect.left=fPaintRC.left;
		PanelRect.right=fPaintRC.right;
		PanelRect.bottom=fPaintRC.bottom;
		PanelRect.top=PanelRect.bottom-1;
		fPainter->PaintPanelHeader(DC,PanelRect);
	}

	// tabs
	uIntn TabCount=fTabCacheList.GetCount();
	if(TabCount==0)
		return;


	for(uIntn i=0;i<TabCount;i++){
		if(i==static_cast<uIntn>(ActiveIndex))
			continue;
		TabState ds=static_cast<uIntn>(HotIndex)==i?TabState::Hot:TabState::Normal;
		DrawTabItem(DC,fTabCacheList[i],ds);
	}
	if(static_cast<uIntn>(ActiveIndex)<TabCount){
		DrawTabItem(DC,fTabCacheList[ActiveIndex],TabState::Active);
	}
}
//---------------------------------------------------------------------------
void vWinTab::DrawTabItem(HDC DC,const cTabCacheItem &Item,TabState State)noexcept
{
	RECT TabRect;
	TabRect.left=fPaintRC.left+static_cast<sfInt32>(Item.TabPos);
	TabRect.right=fPaintRC.left+static_cast<sfInt32>(Item.TabPos+Item.TabSize);

	TabRect.top=fPaintRC.top;
	TabRect.bottom=fPaintRC.bottom;
	
	RECT TextRect;
	switch(Item.Part){
	case TabPart::Left:
		TextRect=fPainter->PaintItemLeft(DC,TabRect,State);
		break;
	case TabPart::Middle:
		TextRect=fPainter->PaintItemMiddle(DC,TabRect,State);
		break;
	case TabPart::Right:
		TextRect=fPainter->PaintItemRight(DC,TabRect,State);
		break;
	default:
	case TabPart::Single:
		TextRect=fPainter->PaintItemSingle(DC,TabRect,State);
		break;
	}

	HFONT TextFont;
	if(Item.Font!=nullptr){
		TextFont=Item.Font->GetFontHandle();
	}
	else{
		TextFont=nullptr;
	}
	fPainter->PaintItemText(DC,TextRect,Item.Text,Item.Text->Length,TextFont,State);
}
//---------------------------------------------------------------------------
sfInt16 vWinTab::TabHitTest(Float32 x,Float32 y)noexcept
{
	if(fUpdateTab){
		fUpdateTab=false;
		HDC DC=fDCViewContent->GetIC();
		SetupTabCache(DC);
		fDCViewContent->ReleaseIC(DC);
	}

	if(y<0)
		return -1;
	if(y>fPaintRC.bottom-fPaintRC.top)
		return -1;
	Float32 TabRight=static_cast<Float32>(fPaintRC.right-fPaintRC.left);
	if(x>TabRight)
		return -1;
	if(x<0)
		return -1;
	for(uIntn i=0,c=fTabCacheList.GetCount();i<c;i++){
		auto &CacheItem=fTabCacheList[i];

		if(x<CacheItem.TabPos+CacheItem.TabSize){
			return static_cast<sfInt16>(i);
		}
	}
	return -1;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void cWinTab::ControlContentSetDefault(void)noexcept
{
	SetContent(vWinTab::Create(this));
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cWinScrollBarPainter_Normal::cBrushBMP::cBrushBMP()noexcept
	: BrushPattern(nullptr)
	, Brush(nullptr)
{
}
//---------------------------------------------------------------------------
cWinScrollBarPainter_Normal::cBrushBMP::~cBrushBMP()noexcept
{
	if(Brush!=nullptr){
		Destroy();
	}
}
//---------------------------------------------------------------------------
void cWinScrollBarPainter_Normal::cBrushBMP::Destroy(void)noexcept
{
	::DeleteObject(Brush);
	::DeleteObject(BrushPattern);
}
//---------------------------------------------------------------------------
void cWinScrollBarPainter_Normal::cBrushBMP::CreateBmp32(HDC DC,int w,int h,const void *PixelData)noexcept
{
	BrushPattern=::CreateCompatibleBitmap(DC,w,h);
	if(BrushPattern==nullptr){
		Brush=nullptr;
		return;
	}

	union{
		BITMAPINFO BitmapInfo;
		BITMAPINFOHEADER BitmapHeader;
	};
	BitmapHeader.biSize=sizeof(BitmapHeader);
	BitmapHeader.biWidth=w;
	BitmapHeader.biHeight=h;
	BitmapHeader.biPlanes=1;
	BitmapHeader.biBitCount=32;
	BitmapHeader.biCompression=BI_RGB;
	BitmapHeader.biSizeImage=w*h*4;
	BitmapHeader.biXPelsPerMeter=0;
	BitmapHeader.biYPelsPerMeter=0;
	BitmapHeader.biClrUsed=0;
	BitmapHeader.biClrImportant=0;

	//static const uInt32 BrushBMPPixelNormal[4]={
	//	0xC8D0D4,0xFFFFFF,
	//	0xFFFFFF,0xC8D0D4,
	//};
	//static const uInt32 BrushBMPPixelPressed[4]={
	//	0xC8D0D4,0x372F2B,
	//	0x372F2B,0xC8D0D4,
	//};
	//auto BurshBMPPixel=Pressed?BrushBMPPixelPressed:BrushBMPPixelNormal;
	::SetDIBits(DC,BrushPattern,0,h,PixelData,&BitmapInfo,DIB_RGB_COLORS);

	Brush=::CreatePatternBrush(BrushPattern);
}
//---------------------------------------------------------------------------
void cWinScrollBarPainter_Normal::cBrushBMP::Setup(HDC DC,bool Pressed)noexcept
{
	BrushPattern=::CreateCompatibleBitmap(DC,2,2);
	if(BrushPattern==nullptr){
		Brush=nullptr;
		return;
	}

	union{
		BITMAPINFO BitmapInfo;
		BITMAPINFOHEADER BitmapHeader;
	};
	BitmapHeader.biSize=sizeof(BitmapHeader);
	BitmapHeader.biWidth=2;
	BitmapHeader.biHeight=2;
	BitmapHeader.biPlanes=1;
	BitmapHeader.biBitCount=32;
	BitmapHeader.biCompression=BI_RGB;
	BitmapHeader.biSizeImage=0;
	BitmapHeader.biXPelsPerMeter=0;
	BitmapHeader.biYPelsPerMeter=0;
	BitmapHeader.biClrUsed=0;
	BitmapHeader.biClrImportant=0;

	static const uInt32 BrushBMPPixelNormal[4]={
		0xC8D0D4,0xFFFFFF,
		0xFFFFFF,0xC8D0D4,
	};
	static const uInt32 BrushBMPPixelPressed[4]={
		0xC8D0D4,0x372F2B,
		0x372F2B,0xC8D0D4,
	};
	auto BurshBMPPixel=Pressed?BrushBMPPixelPressed:BrushBMPPixelNormal;
	::SetDIBits(DC,BrushPattern,0,2,BurshBMPPixel,&BitmapInfo,DIB_RGB_COLORS);

	Brush=::CreatePatternBrush(BrushPattern);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
const uInt32 cWinScrollBarPainter_Normal::cBrushBMPNormal::PixelData[4]={
	0xC8D0D4,0xFFFFFF,
	0xFFFFFF,0xC8D0D4,
};
//---------------------------------------------------------------------------
void cWinScrollBarPainter_Normal::cBrushBMPNormal::Setup(HDC DC)noexcept
{
	if(Brush==nullptr){
		cnLib_ASSERT(BrushPattern==nullptr);
		CreateBmp32(DC,2,2,PixelData);
	}
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
const uInt32 cWinScrollBarPainter_Normal::cBrushBMPPressed::PixelData[4]={
	0xC8D0D4,0x372F2B,
	0x372F2B,0xC8D0D4,
};
//---------------------------------------------------------------------------
void cWinScrollBarPainter_Normal::cBrushBMPPressed::Setup(HDC DC)noexcept
{
	if(Brush==nullptr){
		cnLib_ASSERT(BrushPattern==nullptr);
		CreateBmp32(DC,2,2,PixelData);
	}
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cWinScrollBarPainter_Normal::cWinScrollBarPainter_Normal()noexcept
{
}
cWinScrollBarPainter_Normal::~cWinScrollBarPainter_Normal()noexcept
{
}
//---------------------------------------------------------------------------
UINT cWinScrollBarPainter_Normal::BtnStateFlag(eButtonState State,bool IsActive)noexcept
{
	switch(State){
	default:
	case ButtonState::Inactive:
	case ButtonState::Normal:
		return 0;
	case ButtonState::Hot:
		if(IsActive){
			return DFCS_HOT;
		}
		else{
			return 0;
		}
	case ButtonState::Pressed:
		if(IsActive){
			return DFCS_PUSHED;
		}
		else{
			return 0;
		}
	case ButtonState::Disabled:
		return DFCS_INACTIVE;
	}
}
//---------------------------------------------------------------------------
Float32 cWinScrollBarPainter_Normal::GetMinTrackButtonSize(void)noexcept
{
	return 20;
}
//---------------------------------------------------------------------------
void cWinScrollBarPainter_Normal::PaintDec(const PaintParam &Param)noexcept
{
	DWORD ScrollBtnFlag;
	if(Param.IsVertical){
		ScrollBtnFlag=DFCS_SCROLLUP;
	}
	else{
		ScrollBtnFlag=DFCS_SCROLLLEFT;
	}
	ScrollBtnFlag|=BtnStateFlag(Param.State,Param.ActiveButton==ScrollBarButton::Dec);
	::DrawFrameControl(Param.DC,const_cast<LPRECT>(&Param.Rect),DFC_SCROLL,ScrollBtnFlag);
}
//---------------------------------------------------------------------------
void cWinScrollBarPainter_Normal::PaintInc(const PaintParam &Param)noexcept
{
	DWORD ScrollBtnFlag;
	if(Param.IsVertical){
		ScrollBtnFlag=DFCS_SCROLLDOWN;
	}
	else{
		ScrollBtnFlag=DFCS_SCROLLRIGHT;
	}
	ScrollBtnFlag|=BtnStateFlag(Param.State,Param.ActiveButton==ScrollBarButton::Inc);
	::DrawFrameControl(Param.DC,const_cast<LPRECT>(&Param.Rect),DFC_SCROLL,ScrollBtnFlag);
}
//---------------------------------------------------------------------------
void cWinScrollBarPainter_Normal::PaintBar(const PaintParam &Param,bool Active)noexcept
{
	if(Param.State==ButtonState::Pressed && Active){
		auto BrushPtr=fPressedBMPRecycler.Query();
		BrushPtr->Setup(Param.DC);
		::FillRect(Param.DC,&Param.Rect,BrushPtr->Brush);
	}
	else{
		auto BrushPtr=fNormalBMPRecycler.Query();
		BrushPtr->Setup(Param.DC);

		::FillRect(Param.DC,&Param.Rect,BrushPtr->Brush);
	}
}
//---------------------------------------------------------------------------
void cWinScrollBarPainter_Normal::PaintDecBar(const PaintParam &Param)noexcept
{
	return PaintBar(Param,Param.ActiveButton==ScrollBarButton::DecBar);
}
//---------------------------------------------------------------------------
void cWinScrollBarPainter_Normal::PaintIncBar(const PaintParam &Param)noexcept
{
	return PaintBar(Param,Param.ActiveButton==ScrollBarButton::IncBar);
}
//---------------------------------------------------------------------------
void cWinScrollBarPainter_Normal::PaintTrack(const PaintParam &Param)noexcept
{
	UINT ScrollBtnFlag=DFCS_BUTTONPUSH;
	switch(Param.State){
	default:
	case ButtonState::Inactive:
	case ButtonState::Normal:
	case ButtonState::Hot:
	case ButtonState::Pressed:
		break;
	case ButtonState::Disabled:
		ScrollBtnFlag=DFCS_INACTIVE;
	}
	
	::DrawFrameControl(Param.DC,const_cast<LPRECT>(&Param.Rect),DFC_BUTTON,ScrollBtnFlag);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
vWinScrollBar::vWinScrollBar(viScrollBarData *Data)noexcept
	: fData(Data)
{
	DataInsertCallback();
}
//---------------------------------------------------------------------------
vWinScrollBar::~vWinScrollBar()noexcept
{
	DataRemoveCallback();
}
//---------------------------------------------------------------------------
rPtr<viScrollBar> vWinScrollBar::Create(viScrollBarData *Data)noexcept
{
	return rCreate< bwvScrollBar<vWinScrollBar> >(Data);
}
//---------------------------------------------------------------------------
viScrollBarData* vWinScrollBar::GetData(void)const noexcept
{
	return fData;
}
//---------------------------------------------------------------------------
void vWinScrollBar::SetData(viScrollBarData *Data)noexcept
{
	if(fData==Data)
		return;

	DataRemoveCallback();
	fData=Data;
	DataInsertCallback();

	Update();
}
//---------------------------------------------------------------------------
void vWinScrollBar::DataInsertCallback(void)noexcept
{
	if(fData!=nullptr){
		fScrollBarNotifyToken=fData->ScrollBarNotifySet.Insert([this]{
			Update();
		});
	}
}
//---------------------------------------------------------------------------
void vWinScrollBar::DataRemoveCallback(void)noexcept
{
	if(fData!=nullptr){
		fData->ScrollBarNotifySet.Remove(fScrollBarNotifyToken);
	}
}
//---------------------------------------------------------------------------
void vWinScrollBar::Update(void)noexcept
{
	fDCViewContent->InvalidateRect(&fPaintRC);
}
//---------------------------------------------------------------------------
void vWinScrollBar::ThemeSetup(HWND WindowHandle)noexcept
{
	HTHEME Theme=nullptr;
	if(::IsAppThemed()){
		Theme=cWinScrollBarPainter_Theme::OpenTheme(WindowHandle);
	}
	if(Theme!=nullptr){
		fPainter=new cWinScrollBarPainter_Theme(Theme);
	}
	else{
		fPainter=new cWinScrollBarPainter_Normal;
	}
}
//---------------------------------------------------------------------------
void vWinScrollBar::ThemeClear(void)noexcept
{
	delete fPainter;
	fPainter=nullptr;
}
//---------------------------------------------------------------------------
void vWinScrollBar::CalculateParts(Float32 TotalSize)noexcept
{

	fTrackSize=TotalSize-fBtnSize*2;

	viScrollBarData::cScrollPosition ScrollPosition;
	if(fData!=nullptr){
		ScrollPosition=fData->ScrollBarPosition();
	}
	else{
		ScrollPosition.Total.Begin=0;
		ScrollPosition.Total.End=0;
		ScrollPosition.View.Begin=0;
		ScrollPosition.View.End=0;
	}

	fValueSize=ScrollPosition.Total.End-ScrollPosition.Total.Begin;
	Float32 ScrollViewSize=ScrollPosition.View.End-ScrollPosition.View.Begin;
	if(fValueSize<=0 || fValueSize<=ScrollViewSize){
		fTrackBtnPos=0;
		fTrackBtnSize=fTrackSize;

	}
	else{
		cnLib_ASSERT(fValueSize>0);
		if(ScrollViewSize<1)
			ScrollViewSize=1;
		fTrackBtnSize=fTrackSize*ScrollViewSize/fValueSize;
		auto MinTrackBtnSize=fPainter->GetMinTrackButtonSize();
		if(fTrackBtnSize<MinTrackBtnSize){
			fTrackBtnSize=MinTrackBtnSize;
		}
		if(fTrackBtnSize>=fTrackSize){
			fTrackBtnPos=0;
			fTrackBtnSize=fTrackSize;
		}
		else{
			fTrackBtnPos=fTrackSize-fTrackBtnSize;
			Float32 TrackBarSize=fValueSize-ScrollViewSize;
			fTrackBtnPos=fTrackBtnPos*ScrollPosition.View.Begin/TrackBarSize;
		}
	}
}
//---------------------------------------------------------------------------
void vWinScrollBar::Paint(HDC DC,HRGN)noexcept
{
	int PaintWidth=fPaintRC.right-fPaintRC.left;
	int PaintHeight=fPaintRC.bottom-fPaintRC.top;

	fIsVertical=PaintWidth<PaintHeight;

	if(fIsVertical){
		CalculateParts(static_cast<Float32>(PaintHeight));
	}
	else{
		CalculateParts(static_cast<Float32>(PaintWidth));
	}

	cWinScrollBarPainter::PaintParam PartPaintParam;
	PartPaintParam.DC=DC;
	PartPaintParam.Rect=fPaintRC;
	PartPaintParam.IsVertical=fIsVertical;
	if(fData!=nullptr){
		PartPaintParam.State=fData->ScrollBarButtonState();
		PartPaintParam.ActiveButton=fData->ScrollBarActiveButton();
	}
	else{
		PartPaintParam.State=ButtonState::Disabled;
		PartPaintParam.ActiveButton=ScrollBarButton::None;
	}

	LONG &RectStart=fIsVertical ? PartPaintParam.Rect.top : PartPaintParam.Rect.left;
	LONG &RectEnd=fIsVertical ? PartPaintParam.Rect.bottom : PartPaintParam.Rect.right;
	LONG PaintStart=RectStart;

	// Btn Dec
	RectStart=PaintStart;
	RectEnd=RectStart+fBtnSize;
	fPainter->PaintDec(PartPaintParam);

	// Btn Inc
	RectStart=PaintStart+fBtnSize+static_cast<sfInt32>(fTrackSize);
	RectEnd=RectStart+fBtnSize;
	fPainter->PaintInc(PartPaintParam);

	// TrackBar Dec
	RectStart=PaintStart+fBtnSize;
	RectEnd=RectStart+static_cast<sfInt32>(fTrackBtnPos);
	fPainter->PaintDecBar(PartPaintParam);

	// TrackBar Inc
	RectStart=PaintStart+fBtnSize+static_cast<sfInt32>(fTrackBtnPos+fTrackBtnSize);
	RectEnd=PaintStart+fBtnSize+static_cast<sfInt32>(fTrackSize);
	fPainter->PaintIncBar(PartPaintParam);

	// Track Button
	RectStart=PaintStart+fBtnSize+static_cast<sfInt32>(fTrackBtnPos);
	RectEnd=RectStart+static_cast<sfInt32>(fTrackBtnSize);
	fPainter->PaintTrack(PartPaintParam);
}
//---------------------------------------------------------------------------
eScrollBarButton vWinScrollBar::ScrollHitTest(Float32 x,Float32 y)noexcept
{
	if(x<0)
		return ScrollBarButton::None;
	if(y<0)
		return ScrollBarButton::None;

	Float32 PaintWidth=static_cast<Float32>(fPaintRC.right-fPaintRC.left);
	Float32 PaintHeight=static_cast<Float32>(fPaintRC.bottom-fPaintRC.top);

	Float32 PaintSize;
	Float32 &TestPos=fIsVertical ? y : x;
	if(fIsVertical){
		if(y>=PaintHeight)
			return ScrollBarButton::None;
		PaintSize=PaintHeight;
	}
	else{
		if(x>=PaintWidth)
			return ScrollBarButton::None;
		PaintSize=PaintWidth;
	}


	Float32 PartOffset;
	PartOffset=static_cast<Float32>(fBtnSize);
	if(TestPos<PartOffset)
		return eScrollBarButton::Dec;
	PartOffset+=fTrackBtnPos;
	if(TestPos<PartOffset)
		return eScrollBarButton::DecBar;
	PartOffset+=fTrackBtnSize;
	if(TestPos<PartOffset)
		return eScrollBarButton::Track;
	if(TestPos<fBtnSize+fTrackSize)
		return eScrollBarButton::IncBar;
	if(TestPos<PaintSize){
		return eScrollBarButton::Inc;
	}
	return ScrollBarButton::None;
}
//---------------------------------------------------------------------------
Float32 vWinScrollBar::ScrollOffsetToValue(Float32 OffsetX,Float32 OffsetY)noexcept
{
	if(fIsVertical){
		return OffsetY*fValueSize/fTrackSize;
	}
	else{
		return OffsetX*fValueSize/fTrackSize;
	}
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void cWinScrollBar::ControlContentSetDefault(void)noexcept
{
	SetContent(vWinScrollBar::Create(this));
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void cWinControlScrollBar::ControlContentSetDefault(void)noexcept
{
	SetContent(vWinScrollBar::Create(this));
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cWinScrollBarGripPainter_Normal::cWinScrollBarGripPainter_Normal()noexcept
{
}
cWinScrollBarGripPainter_Normal::~cWinScrollBarGripPainter_Normal()noexcept
{
}
//---------------------------------------------------------------------------
void cWinScrollBarGripPainter_Normal::Paint(HDC DC,const RECT &Rect,eAlignment BoxAlign)noexcept
{
	DWORD ScrollBtnFlag;
	ufInt8 AlignLeft,AlignTop;
	AlignLeft=(BoxAlign&Alignment::Left)!=Alignment::None;
	AlignTop=(BoxAlign&Alignment::Top)!=Alignment::None;
	switch(AlignLeft | (AlignTop<<1)){
	default:
	case 0:	// BottomRight
		ScrollBtnFlag=DFCS_SCROLLSIZEGRIP;
		break;
	case 1:	// BottomLeft
		ScrollBtnFlag=DFCS_SCROLLSIZEGRIPRIGHT;
		break;
	case 2:	// TopRight
		ScrollBtnFlag=DFCS_SCROLLSIZEGRIP;
		break;
	case 3:	// TopLeft
		ScrollBtnFlag=DFCS_SCROLLSIZEGRIPRIGHT;
		break;
	}
	::DrawFrameControl(DC,const_cast<LPRECT>(&Rect),DFC_SCROLL,ScrollBtnFlag);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
vWinScrollBarGrip::vWinScrollBarGrip(viScrollBarGripData *Data)noexcept
	: fData(Data)
{
	DataInsertCallback();
}
//---------------------------------------------------------------------------
vWinScrollBarGrip::~vWinScrollBarGrip()noexcept
{
	DataRemoveCallback();
}
//---------------------------------------------------------------------------
rPtr<viControl> vWinScrollBarGrip::Create(viScrollBarGripData *Data)noexcept
{
	return rCreate< bwvControl<vWinScrollBarGrip> >(Data);
}
//---------------------------------------------------------------------------
viScrollBarGripData* vWinScrollBarGrip::GetData(void)const noexcept
{
	return fData;
}
//---------------------------------------------------------------------------
void vWinScrollBarGrip::SetData(viScrollBarGripData *Data)noexcept
{
	if(fData==Data)
		return;

	DataRemoveCallback();
	fData=Data;
	DataInsertCallback();
}
//---------------------------------------------------------------------------
void vWinScrollBarGrip::Update(void)noexcept
{
	fDCViewContent->InvalidateRect(&fPaintRC);
}
//---------------------------------------------------------------------------
void vWinScrollBarGrip::DataInsertCallback(void)noexcept
{
	if(fData!=nullptr){
		fScrollBarGripNotifyToken=fData->ScrollBarGripNotifySet.Insert([this]{
			Update();
		});
	}
}
//---------------------------------------------------------------------------
void vWinScrollBarGrip::DataRemoveCallback(void)noexcept
{
	if(fData!=nullptr){
		fData->ScrollBarGripNotifySet.Remove(fScrollBarGripNotifyToken);
	}
}
//---------------------------------------------------------------------------
void vWinScrollBarGrip::ThemeSetup(HWND WindowHandle)noexcept
{
	HTHEME Theme=nullptr;
	if(::IsAppThemed()){
		Theme=cWinScrollBarGripPainter_Theme::OpenTheme(WindowHandle);
	}
	if(Theme!=nullptr){
		fPainter=new cWinScrollBarGripPainter_Theme(Theme);
	}
	else{
		fPainter=new cWinScrollBarGripPainter_Normal;
	}
}
//---------------------------------------------------------------------------
void vWinScrollBarGrip::ThemeClear(void)noexcept
{
	delete fPainter;
	fPainter=nullptr;
}
//---------------------------------------------------------------------------
void vWinScrollBarGrip::Paint(HDC DC,HRGN)noexcept
{
	eAlignment BoxAlign;
	if(fData!=nullptr){
		BoxAlign=fData->ScrollBarGripGetAlignment();
	}
	else{
		BoxAlign=Alignment::BottomRight;
	}
	fPainter->Paint(DC,fPaintRC,BoxAlign);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
kWinControlScrollBar::kWinControlScrollBar()noexcept
{
	ScrollBarHorizontal.SetContent(vWinScrollBar::Create(&ScrollBarHorizontal));
	ScrollBarVertical.SetContent(vWinScrollBar::Create(&ScrollBarVertical));
	ScrollBarGrip.SetBackground(vWinScrollBarGrip::Create(this));
}
//---------------------------------------------------------------------------
kWinControlScrollBar::~kWinControlScrollBar()noexcept
{
}
//---------------------------------------------------------------------------
rPtr<kiScrollBar> kWinControlScrollBar::Create(void)noexcept
{
	return rCreate< bwkScrollBar<kWinControlScrollBar> >();
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
vWinTreeBranchGraph::vWinTreeBranchGraph(viTreeBranchData *BranchData)noexcept
	: fBranchData(BranchData)
{
	BranchDataInsertCallback();
}
//---------------------------------------------------------------------------
vWinTreeBranchGraph::~vWinTreeBranchGraph()noexcept
{
	BranchDataRemoveCallback();
}
//---------------------------------------------------------------------------
rPtr<viControl> vWinTreeBranchGraph::Create(viTreeBranchData *BranchData)noexcept
{
	return rCreate< bwvControl<vWinTreeBranchGraph> >(BranchData);
}
//---------------------------------------------------------------------------
viTreeBranchData* vWinTreeBranchGraph::GetBranchData(void)const noexcept
{
	return fBranchData;
}
//---------------------------------------------------------------------------
void vWinTreeBranchGraph::SetBranchData(viTreeBranchData *Data)noexcept
{
	if(fBranchData==Data)
		return;

	BranchDataRemoveCallback();
	fBranchData=Data;
	BranchDataInsertCallback();

	Update();
}
//---------------------------------------------------------------------------
void vWinTreeBranchGraph::BranchDataInsertCallback(void)noexcept
{
	if(fBranchData!=nullptr){
		fTreeGraphNotifyToken=fBranchData->TreeBranchNotifySet.Insert([this]{
			Update();
		});
	}
}
//---------------------------------------------------------------------------
void vWinTreeBranchGraph::BranchDataRemoveCallback(void)noexcept
{
	if(fBranchData!=nullptr){
		fBranchData->TreeBranchNotifySet.Remove(fTreeGraphNotifyToken);
	}
}
//---------------------------------------------------------------------------
void vWinTreeBranchGraph::Update(void)noexcept
{
	fDCViewContent->InvalidateRect(&fPaintRC);
}
//---------------------------------------------------------------------------
void vWinTreeBranchGraph::Paint(HDC DC,HRGN)noexcept
{
	if(fBranchData==nullptr)
		return;

	auto TreeLinePen=fTreeLinePenRecycler.Query();

	auto OldPen=::SelectObject(DC,TreeLinePen->Pen);

	auto BranchInfo=fBranchData->TreeBranchInfo();

	for(ufInt16 bi=0;bi<BranchInfo.BranchCount;bi++){
		auto Branch=fBranchData->TreeBranchItem(bi);
		//DrawBranch(DC,BranchInfo.Offset,Branch);
	}

	::SelectObject(DC,OldPen);
}
//---------------------------------------------------------------------------
vWinTreeBranchGraph::cTreeLinePen::cTreeLinePen()noexcept
{
	LOGBRUSH BrushInfo;
	BrushInfo.lbColor=0x505050;
	BrushInfo.lbHatch=0;
	BrushInfo.lbStyle=BS_SOLID;
	Pen=::ExtCreatePen(PS_COSMETIC|PS_ALTERNATE,1,&BrushInfo,0,nullptr);
}
//---------------------------------------------------------------------------
vWinTreeBranchGraph::cTreeLinePen::~cTreeLinePen()noexcept
{
	::DeleteObject(Pen);
}
//---------------------------------------------------------------------------
void vWinTreeBranchGraph::DrawBranch(HDC DC,const cUIPoint &Offset,const viTreeBranchData::cTreeBranchItem &Branch)noexcept
{
	Float32 Top=fPaintRC.top+Offset.y;
	Float32 Left=fPaintRC.left+Offset.x;
	// level line
	Float32 LineX=(Branch.Rect.Left+Branch.Rect.Right)/2;
	::MoveToEx(DC,static_cast<int>(Left+LineX),static_cast<int>(Top+Branch.Rect.Top),nullptr);
	::LineTo(DC,static_cast<int>(Left+LineX),static_cast<int>(Top+Branch.Rect.Bottom));

	// node line
	for(uIntn i=0,c=Branch.NodeCount;i<c;i++){
		auto &NodeOffset=Branch.NodeOffsets[i];
		//::MoveToEx(DC,static_cast<int>(Left+LineX),static_cast<int>(Top+NodeOffset),nullptr);
		//::LineTo(DC,static_cast<int>(Left+Node.x),static_cast<int>(Top+NodeOffset));
	}
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
vWinTreeExpandButton::vWinTreeExpandButton(viButtonData *Data)noexcept
	: bvWinButton(Data)
{
}
//---------------------------------------------------------------------------
vWinTreeExpandButton::~vWinTreeExpandButton()noexcept
{
}
//---------------------------------------------------------------------------
rPtr<viControl> vWinTreeExpandButton::Create(viButtonData *Data)noexcept
{
	return rCreate< bwvControl<vWinTreeExpandButton> >(Data);
}
//---------------------------------------------------------------------------
void vWinTreeExpandButton::Update(void)noexcept
{
	fDCViewContent->InvalidateRect(&fPaintRC);
}
//---------------------------------------------------------------------------
void vWinTreeExpandButton::Paint(HDC DC,HRGN)noexcept
{
	static constexpr int ExpandBoxSize=9;

	int CenterX=(fPaintRC.left+fPaintRC.right)/2;
	int CenterY=(fPaintRC.top+fPaintRC.bottom)/2;

	RECT DrawRC;
	DrawRC.left=CenterX-ExpandBoxSize/2;
	DrawRC.right=DrawRC.left+ExpandBoxSize;
	DrawRC.top=CenterY-ExpandBoxSize/2;
	DrawRC.bottom=DrawRC.top+ExpandBoxSize;

	auto OldPen=::SelectObject(DC,::GetStockObject(BLACK_PEN));
	auto OldBrush=::SelectObject(DC,::GetStockObject(WHITE_BRUSH));
	// border
	::SetBkColor(DC,0xFF);
	::Rectangle(DC,DrawRC.left,DrawRC.top,DrawRC.right,DrawRC.bottom);

	RECT SignRC;
	SignRC.left=DrawRC.left+2;
	SignRC.right=DrawRC.right-2;
	SignRC.top=DrawRC.top+2;
	SignRC.bottom=DrawRC.bottom-2;
	// sign
	// -
	::MoveToEx(DC,SignRC.left,CenterY,nullptr);
	::LineTo(DC,SignRC.right,CenterY);

	ufInt8 CheckState;
	if(fButtonData!=nullptr){
		CheckState=fButtonData->CheckState();
	}
	else{
		CheckState=0;
	}

	if(CheckState==0){
		// |  (+)
		::MoveToEx(DC,CenterX,SignRC.top,nullptr);
		::LineTo(DC,CenterX,SignRC.bottom);
	}

	::SelectObject(DC,OldPen);
	::SelectObject(DC,OldBrush);
}
//---------------------------------------------------------------------------
