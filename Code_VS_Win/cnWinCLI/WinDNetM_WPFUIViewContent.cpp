#include "WinDNetM_WPFUIViewContent.h"

using namespace cnLibrary;
using namespace cnRTL;
using namespace cnWin;

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
mcWPFDrawingContext::mcWPFDrawingContext()
{
}
//---------------------------------------------------------------------------
mcWPFDrawingContext::~mcWPFDrawingContext()
{
}
//---------------------------------------------------------------------------
void mcWPFDrawingContext::Start(System::Windows::Media::DrawingContext ^Context)
{
	Handle.Alloc(Context);
}
//---------------------------------------------------------------------------
void mcWPFDrawingContext::Finish(void)
{
	Handle.Free();
}
//---------------------------------------------------------------------------
System::Windows::Media::DrawingContext^ mcWPFDrawingContext::Stop(void)
{
	return Handle.Discard();
}
//---------------------------------------------------------------------------
bool mcWPFDrawingContext::mFill(cUIPoint DrawPosition,cUIPoint DrawSize,cUIColor Color)
{
	System::Windows::Rect DrawRC;
	DrawRC.X=DrawPosition.x;
	DrawRC.Y=DrawPosition.y;
	DrawRC.Width=DrawSize.x;
	DrawRC.Height=DrawSize.y;
	auto Context=Handle.Get();
	auto Brush=gcnew System::Windows::Media::SolidColorBrush(System::Windows::Media::Color::FromScRgb(Color.Alpha,Color.Red,Color.Green,Color.Blue));
	Context->DrawRectangle(Brush,nullptr,DrawRC);
	return true;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
mcWPFDrawing::mcWPFDrawing(System::Windows::Media::Drawing ^Drawing)
	: Handle(Drawing)
{
}
//---------------------------------------------------------------------------
mcWPFDrawing::mcWPFDrawing(mcWPFDrawing &&Src)=default;
//---------------------------------------------------------------------------
mcWPFDrawing::~mcWPFDrawing()
{
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
mcWPFUIBitmap::mcWPFUIBitmap(System::Windows::Media::ImageSource ^Image)
	: Handle(Image)
{
	ImageSize.x=Image->Width;
	ImageSize.y=Image->Height;
}
//---------------------------------------------------------------------------
mcWPFUIBitmap::mcWPFUIBitmap(mcWPFUIBitmap &&Src)=default;
//---------------------------------------------------------------------------
mcWPFUIBitmap::~mcWPFUIBitmap()
{
}
//---------------------------------------------------------------------------
mcWPFUIBitmap mcWPFUIBitmap::FromDrawing(const mcWPFDrawing &Drawing)
{
	auto ImageSource=gcnew System::Windows::Media::DrawingImage(Drawing.Handle);

	ImageSource->Freeze();

	return mcWPFUIBitmap(ImageSource);
}
//---------------------------------------------------------------------------
void mcWPFUIBitmap::Draw(cGCRef &ContextHandle,cUIPoint DrawPosition,cUIPoint DrawSize)
{
	System::Windows::Rect DrawRect;
	DrawRect.X=DrawPosition.x;
	DrawRect.Y=DrawPosition.y;
	DrawRect.Width=DrawSize.x;
	DrawRect.Height=DrawSize.y;
	auto Context=ContextHandle.Cast<System::Windows::Media::DrawingContext>();
	Context->DrawImage(Handle,DrawRect);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
mcWPFBitmapDataSource::mcWPFBitmapDataSource(System::Windows::Media::Imaging::BitmapSource ^BitmapSource)
	: Handle(BitmapSource)
{
	ImageSize.x=BitmapSource->Width;
	ImageSize.y=BitmapSource->Height;
}
//---------------------------------------------------------------------------
mcWPFBitmapDataSource::mcWPFBitmapDataSource(mcWPFBitmapDataSource &&Src)=default;
//---------------------------------------------------------------------------
mcWPFBitmapDataSource::~mcWPFBitmapDataSource()
{
}
//---------------------------------------------------------------------------
mcWPFBitmapDataSource mcWPFBitmapDataSource::FromDrawing(const mcWPFDrawing &Drawing,cUIPoint Size)
{
	auto TempVisual=gcnew System::Windows::Media::DrawingVisual();
	auto Context=TempVisual->RenderOpen();
	Context->DrawDrawing(Drawing.Handle);
	Context->Close();
	auto BitmapTarget=gcnew System::Windows::Media::Imaging::RenderTargetBitmap(Size.x,Size.y,96,96,System::Windows::Media::PixelFormats::Bgra32);
	BitmapTarget->Render(TempVisual);

	//BitmapTarget->Freeze();

	return mcWPFBitmapDataSource(BitmapTarget);
}
//---------------------------------------------------------------------------
void mcWPFBitmapDataSource::Draw(cGCRef &ContextHandle,cUIPoint DrawPosition,cUIPoint DrawSize)
{
	System::Windows::Rect DrawRect;
	DrawRect.X=DrawPosition.x;
	DrawRect.Y=DrawPosition.y;
	DrawRect.Width=DrawSize.x;
	DrawRect.Height=DrawSize.y;
	auto Context=ContextHandle.Cast<System::Windows::Media::DrawingContext>();
	Context->DrawImage(Handle,DrawRect);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
mcWPFUISimpleBitmapCanvas::mcWPFUISimpleBitmapCanvas(cUIPoint Size)
{
	fBitmapSize.x=cnMath::FloatRoundNearest(Size.x);
	fBitmapSize.y=cnMath::FloatRoundNearest(Size.y);
}
//---------------------------------------------------------------------------
mcWPFUISimpleBitmapCanvas::mcWPFUISimpleBitmapCanvas(mcWPFUISimpleBitmapCanvas &&Src)=default;
//---------------------------------------------------------------------------
mcWPFUISimpleBitmapCanvas::~mcWPFUISimpleBitmapCanvas()
{
}
//---------------------------------------------------------------------------
void mcWPFUISimpleBitmapCanvas::StartContext(mcWPFDrawingContext &mContext,cUIColor Color)
{
	auto Drawing=gcnew System::Windows::Media::DrawingGroup();
	auto Context=Drawing->Open();

	if(Color.Alpha>0){
		auto Brush=gcnew System::Windows::Media::SolidColorBrush(System::Windows::Media::Color::FromScRgb(Color.Alpha,Color.Red,Color.Green,Color.Blue));
		Brush->Freeze();
		Context->DrawRectangle(Brush,nullptr,System::Windows::Rect(System::Windows::Point(0,0),System::Windows::Size(fBitmapSize.x,fBitmapSize.y)));
	}
	DrawingHandle.Set(Drawing);
	mContext.Start(Context);
}
//---------------------------------------------------------------------------
void mcWPFUISimpleBitmapCanvas::DiscardContext(mcWPFDrawingContext &mContext)
{
	auto Context=mContext.Stop();
	if(Context!=nullptr){
		Context->Close();
	}
	DrawingHandle.Set(nullptr);
}
//---------------------------------------------------------------------------
mcWPFDrawing mcWPFUISimpleBitmapCanvas::FinishContext(mcWPFDrawingContext &mContext,bool &Success)
{
	mcWPFDrawing RetHandle={
		cnVar::MoveCast(DrawingHandle)
	};

	auto Context=mContext.Stop();
	if(Context!=nullptr){
		Context->Close();

		Success=true;
	}
	else{
		Success=false;
	}
	return RetHandle;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
mcWPFImageDrawing::mcWPFImageDrawing()
	: Handle(gcnew System::Windows::Media::ImageDrawing())
{
}
//---------------------------------------------------------------------------
mcWPFImageDrawing::~mcWPFImageDrawing()
{
}
//---------------------------------------------------------------------------
void mcWPFImageDrawing::mUpdateRect(Float32 Width,Float32 Height)
{
	System::Windows::Rect ContentRect;
	ContentRect.X=0;
	ContentRect.Y=0;
	ContentRect.Width=Width;
	ContentRect.Height=Height;
	auto Drawing=Handle.Get();
	Drawing->Rect=ContentRect;
}
//---------------------------------------------------------------------------
void mcWPFImageDrawing::mApplyVisual(const cnRTL::cnWinRTL::cGDIBitmapSection &BitmapBuffer,Float32 Width,Float32 Height)
{
	auto Drawing=Handle.Get();

	auto InteropSource=dynamic_cast<System::Windows::Interop::InteropBitmap^>(Drawing->ImageSource);
	if(InteropSource!=nullptr){
		if(InteropSource->PixelWidth==Width && InteropSource->PixelHeight==Height){
			// invalid existing bitmap only
			InteropSource->Invalidate();
			return;
		}
	}
	// make bitmap source from memory
	auto NewBitmapSource=System::Windows::Interop::Imaging::CreateBitmapSourceFromMemorySection(System::IntPtr(BitmapBuffer.Section),Width,Height,
		System::Windows::Media::PixelFormats::Bgra32,BitmapBuffer.Stride,0);
	
	// apply image
	Drawing->ImageSource=NewBitmapSource;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
mcWPFTypeface::mcWPFTypeface(System::Windows::Media::Typeface ^Typeface)
	: Handle(Typeface)
{
	int GDIWeight=Typeface->Weight.ToOpenTypeWeight();
	if(GDIWeight<FW_THIN)
		GDIWeight=FW_THIN;
	if(GDIWeight>FW_HEAVY)
		GDIWeight=FW_HEAVY;
	FontWeight=(GDIWeight-FW_NORMAL)/500.f;

	if(Typeface->Style==System::Windows::FontStyles::Italic){
		FontStyle=UIFontStyle::Italic;
	}
	else{
		FontStyle=UIFontStyle::Normal;
	}
}
//---------------------------------------------------------------------------
mcWPFTypeface::mcWPFTypeface(mcWPFTypeface &&Src)=default;
//---------------------------------------------------------------------------
mcWPFTypeface::~mcWPFTypeface()
{
}
//---------------------------------------------------------------------------
mcWPFTypeface mcWPFTypeface::CreateTypeface(const uChar16 *Name,uIntn NameLength,eUIFontStyle FontStyle,Float32 FontWeight)
{
	auto FontNameString=System::Runtime::InteropServices::Marshal::PtrToStringUni(System::IntPtr(const_cast<uChar16*>(Name)),NameLength);
	auto FontFamily=gcnew System::Windows::Media::FontFamily(FontNameString);

	System::Windows::FontStyle ^TypefaceStyle;
	switch(FontStyle){
	case UIFontStyle::Normal:
		TypefaceStyle=System::Windows::FontStyles::Normal;
		break;
	case UIFontStyle::Italic:
		TypefaceStyle=System::Windows::FontStyles::Italic;
		break;
	}
	auto Typeface=gcnew System::Windows::Media::Typeface(FontFamily,*TypefaceStyle,System::Windows::FontWeights::Normal,System::Windows::FontStretches::Normal);

	return mcWPFTypeface(Typeface);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void mcWPFTextLayout::Setup(const uChar32 *Text,uIntn Length,const cUITextStyle &Style)
{
	rcWPFTextLayout TextLayout;
	TextLayout.SetupText(Text,Length,Style,nullptr,0,1.);
	TextLayout.CalculateTextLayout(*this);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
double rcWPFTextLayout::TextWidth::get(void)
{
	return fTextWidth;
}
//---------------------------------------------------------------------------
double rcWPFTextLayout::TextHeight::get(void)
{
	return fTextHeight;
}
//---------------------------------------------------------------------------
System::Collections::Generic::List<System::Double>^ rcWPFTextLayout::AdvanceWidths::get(void)
{
	return fAdvanceWidths;
}
//---------------------------------------------------------------------------
void rcWPFTextLayout::SetupText(const uChar32 *Text,uIntn Length,const cUITextStyle &Style,const Float32* TextDistance,Float32 TextMinHeight,Float32 LayoutScale)
{
	fGlyphTypeface=nullptr;

	auto FontObject=iCast<iCLIObject>(Style.Font);
	if(FontObject!=nullptr){
		auto Typeface=FontObject->Get<System::Windows::Media::Typeface^>();

		if(Typeface!=nullptr){
			Typeface->TryGetGlyphTypeface(fGlyphTypeface);
		}
	}

	if(fGlyphTypeface==nullptr){
		auto SysTypeface=gcnew System::Windows::Media::Typeface(System::Windows::SystemFonts::MessageFontFamily,System::Windows::FontStyles::Normal,System::Windows::FontWeights::Normal,System::Windows::FontStretches::Normal);
		SysTypeface->TryGetGlyphTypeface(fGlyphTypeface);
	}

	if(fGlyphTypeface==nullptr)
		return;

	fFontRenderingEmSize=Style.FontSize/LayoutScale;
	//fFontRenderingEmSize=System::Math::Round(fFontRenderingEmSize,System::MidpointRounding::AwayFromZero);

	auto FontCharMap=fGlyphTypeface->CharacterToGlyphMap;
	auto FontWidthMap=fGlyphTypeface->AdvanceWidths;
	auto FontHeightMap=fGlyphTypeface->AdvanceHeights;

	
	System::UInt16 GlyphIndexDefault=0;
	FontCharMap->TryGetValue('?',GlyphIndexDefault);
	fIndices=gcnew System::Collections::Generic::List<System::UInt16>(Length);
	fAdvanceWidths=gcnew System::Collections::Generic::List<System::Double>(Length);
	//fOffsets=gcnew System::Collections::Generic::List<System::Windows::Point>(Length);
	//fClusterMap=gcnew System::Collections::Generic::List<System::UInt16>(Length);
	//fCaretStops=gcnew System::Collections::Generic::List<System::Boolean>(Length+1);
	fTextWidth=0;
	fTextHeight=fGlyphTypeface->Height*fFontRenderingEmSize;
	if(fTextHeight<TextMinHeight){
		fTextHeight=TextMinHeight;
	}
	fTextHeight=System::Math::Round(fTextHeight,System::MidpointRounding::AwayFromZero);
	for(uIntn i=0;i<Length;i++){
		System::UInt16 GlyphIndex;
		if(FontCharMap->TryGetValue(Text[i],GlyphIndex)==false){
			GlyphIndex=GlyphIndexDefault;
		}
		fIndices->Add(GlyphIndex);

		System::Double GlyphAdvanceWidth;
		if(TextDistance!=nullptr){
			GlyphAdvanceWidth=TextDistance[i]/LayoutScale-fTextWidth;
		}
		else{
			GlyphAdvanceWidth=fFontRenderingEmSize*FontWidthMap[GlyphIndex];
		}
		GlyphAdvanceWidth=System::Math::Round(GlyphAdvanceWidth,System::MidpointRounding::AwayFromZero);
		fTextWidth+=GlyphAdvanceWidth;

		fAdvanceWidths->Add(GlyphAdvanceWidth);
		//fOffsets->Add(System::Windows::Point(0,0));
		//fClusterMap->Add(0);
		//fCaretStops->Add(false);
	}
	//fCaretStops->Add(false);
	
	fFontBaseline=fGlyphTypeface->Baseline*fFontRenderingEmSize;
	fTextWidth=System::Math::Round(fTextWidth,System::MidpointRounding::AwayFromZero);
	fTextHeight=System::Math::Round(fTextHeight,System::MidpointRounding::AwayFromZero);
	fLayoutScale=LayoutScale;
}
//---------------------------------------------------------------------------
void rcWPFTextLayout::CalculateTextLayout(mcWPFTextLayout &Layout)
{
	Layout.TextHeight=static_cast<Float32>(fTextHeight*fLayoutScale);
	Layout.TextHeight*=fLayoutScale;

	Layout.TextDistance.SetCount(fAdvanceWidths->Count);
	uIntn i=0;
	for each(auto Width in fAdvanceWidths){
		Layout.TextDistance[i++]=static_cast<Float32>(Width*fLayoutScale);
	}
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cUIPoint rcWPFTextGraph::SetupText(const uChar32 *Text,uIntn Length,const cUITextStyle &Style,const Float32* TextDistance,Float32 TextMinHeight)
{
	fGlyphTypeface=nullptr;

	auto FontObject=iCast<iCLIObject>(Style.Font);
	if(FontObject!=nullptr){
		auto Typeface=FontObject->Get<System::Windows::Media::Typeface^>();

		if(Typeface!=nullptr){
			Typeface->TryGetGlyphTypeface(fGlyphTypeface);
		}
	}

	if(fGlyphTypeface==nullptr){
		auto SysTypeface=gcnew System::Windows::Media::Typeface(System::Windows::SystemFonts::MessageFontFamily,System::Windows::FontStyles::Normal,System::Windows::FontWeights::Normal,System::Windows::FontStretches::Normal);
		SysTypeface->TryGetGlyphTypeface(fGlyphTypeface);
	}

	if(fGlyphTypeface==nullptr)
		return {0,0};

	fFontRenderingEmSize=Style.FontSize;
	fFontRenderingEmSize=System::Math::Round(fFontRenderingEmSize,System::MidpointRounding::AwayFromZero);

	auto FontCharMap=fGlyphTypeface->CharacterToGlyphMap;
	auto FontWidthMap=fGlyphTypeface->AdvanceWidths;
	auto FontHeightMap=fGlyphTypeface->AdvanceHeights;

	
	System::UInt16 GlyphIndexDefault=0;
	FontCharMap->TryGetValue('?',GlyphIndexDefault);
	fIndices=gcnew System::Collections::Generic::List<System::UInt16>(Length);
	fAdvanceWidths=gcnew System::Collections::Generic::List<System::Double>(Length);
	//fOffsets=gcnew System::Collections::Generic::List<System::Windows::Point>(Length);
	//fClusterMap=gcnew System::Collections::Generic::List<System::UInt16>(Length);
	//fCaretStops=gcnew System::Collections::Generic::List<System::Boolean>(Length+1);
	fTextWidth=0;
	fTextHeight=fGlyphTypeface->Height*fFontRenderingEmSize;
	if(fTextHeight<TextMinHeight){
		fTextHeight=TextMinHeight;
	}
	fTextHeight=System::Math::Round(fTextHeight,System::MidpointRounding::AwayFromZero);
	for(uIntn i=0;i<Length;i++){
		System::UInt16 GlyphIndex;
		if(FontCharMap->TryGetValue(Text[i],GlyphIndex)==false){
			GlyphIndex=GlyphIndexDefault;
		}
		fIndices->Add(GlyphIndex);

		System::Double GlyphAdvanceWidth;
		if(TextDistance!=nullptr){
			GlyphAdvanceWidth=TextDistance[i]-fTextWidth;
		}
		else{
			GlyphAdvanceWidth=fFontRenderingEmSize*FontWidthMap[GlyphIndex];
		}
		GlyphAdvanceWidth=System::Math::Round(GlyphAdvanceWidth,System::MidpointRounding::AwayFromZero);
		fTextWidth+=GlyphAdvanceWidth;

		fAdvanceWidths->Add(GlyphAdvanceWidth);
		//fOffsets->Add(System::Windows::Point(0,0));
		//fClusterMap->Add(0);
		//fCaretStops->Add(false);
	}
	//fCaretStops->Add(false);
	
	fFontBaseline=fGlyphTypeface->Baseline*fFontRenderingEmSize;
	fFontBaseline=System::Math::Round(fFontBaseline,System::MidpointRounding::AwayFromZero);


	fForegroundBrush=gcnew System::Windows::Media::SolidColorBrush(
		System::Windows::Media::Color::FromScRgb(Style.TextColor.Alpha,Style.TextColor.Red,Style.TextColor.Green,Style.TextColor.Blue)
	);
	fForegroundBrush->Freeze();


	if(Style.BackgroundColor.Alpha>0){
		// draw background
		fBackgroundBrush=gcnew System::Windows::Media::SolidColorBrush(
			System::Windows::Media::Color::FromScRgb(Style.BackgroundColor.Alpha,Style.BackgroundColor.Red,Style.BackgroundColor.Green,Style.BackgroundColor.Blue)
		);
		fBackgroundBrush->Freeze();
	}
	else{
		fBackgroundBrush=nullptr;
	}
	if(Style.Strike){
		fStrikePen=gcnew System::Windows::Media::Pen(fForegroundBrush,fFontRenderingEmSize*fGlyphTypeface->StrikethroughThickness);
		fStrikePen->Freeze();
		fFontStrikePos=fFontBaseline-fFontRenderingEmSize*fGlyphTypeface->StrikethroughPosition;
	}
	else{
		fStrikePen=nullptr;
	}
	if(Style.Underline){
		fUnderlinePen=gcnew System::Windows::Media::Pen(fForegroundBrush,fFontRenderingEmSize*fGlyphTypeface->UnderlinePosition);
		fUnderlinePen->Freeze();
		fFontUnderlinePos=fFontBaseline-fFontRenderingEmSize*fGlyphTypeface->UnderlinePosition;
	}
	else{
		fUnderlinePen=nullptr;
	}
	fTextWidth=System::Math::Round(fTextWidth,System::MidpointRounding::AwayFromZero);
	fTextHeight=System::Math::Round(fTextHeight,System::MidpointRounding::AwayFromZero);

	return {static_cast<Float32>(fTextWidth),static_cast<Float32>(fTextHeight)};
}
//---------------------------------------------------------------------------
void __clrcall rcWPFTextGraph::Draw(System::Windows::Media::DrawingContext ^Context,cUIPoint DrawPosition,cUIPoint DrawSize)
{
	if(fGlyphTypeface==nullptr)
		return;
	// setup text

	auto TextPos=System::Windows::Point(
		System::Math::Round(DrawPosition.x,System::MidpointRounding::AwayFromZero),
		System::Math::Round(DrawPosition.y,System::MidpointRounding::AwayFromZero)
	);

	System::Double DrawWidth=fTextWidth;
	auto TextGlyph=gcnew System::Windows::Media::GlyphRun();
	{
		auto TextGlyphInit=dynamic_cast<System::ComponentModel::ISupportInitialize^>(TextGlyph);
		TextGlyphInit->BeginInit();

		TextGlyph->GlyphTypeface=fGlyphTypeface;
		TextGlyph->IsSideways=false;
		TextGlyph->FontRenderingEmSize=fFontRenderingEmSize;

		if(DrawWidth<=DrawSize.x){
			//TextGlyph->Characters=fCharacters;
			TextGlyph->GlyphIndices=fIndices;
			TextGlyph->AdvanceWidths=fAdvanceWidths;

			//TextGlyph->GlyphOffsets=fOffsets;
			//TextGlyph->ClusterMap=fClusterMap;s
			//TextGlyph->CaretStops=fCaretStops;
		}
		else{
			DrawWidth=0;
			unsigned int c=fIndices->Count;
			auto ShortIndices=gcnew System::Collections::Generic::List<System::UInt16>(c);
			auto ShortAdvanceWidths=gcnew System::Collections::Generic::List<System::Double>(c);
			for(unsigned int i=0;i<c;i++){
				auto CurWidth=fAdvanceWidths[i];
				auto NextWidth=DrawWidth+CurWidth;
				if(NextWidth>DrawSize.x){
					break;
				}
				DrawWidth=NextWidth;
				ShortIndices->Add(fIndices[i]);
				ShortAdvanceWidths->Add(CurWidth);
			}
			TextGlyph->GlyphIndices=ShortIndices;
			TextGlyph->AdvanceWidths=ShortAdvanceWidths;
		}
		//TextGlyph->DeviceFontName="";
		//TextGlyph->Language=System::Windows::Markup::XmlLanguage::Empty;
		auto Baseline=TextPos;
		Baseline.Y+=fFontBaseline;
		TextGlyph->BaselineOrigin=Baseline;

		TextGlyphInit->EndInit();
	}

	// solid background

	if(fBackgroundBrush!=nullptr){
		auto TextRect=System::Windows::Rect(
			System::Math::Round(DrawPosition.x,System::MidpointRounding::AwayFromZero),
			System::Math::Round(DrawPosition.y,System::MidpointRounding::AwayFromZero),
			//System::Math::Round(DrawSize.x,System::MidpointRounding::AwayFromZero),
			//System::Math::Round(DrawSize.y,System::MidpointRounding::AwayFromZero)
			DrawWidth,
			fTextHeight
		);
		Context->DrawRectangle(fBackgroundBrush,nullptr,TextRect);
	}

	// draw text
	Context->DrawGlyphRun(fForegroundBrush,TextGlyph);

	// underline
	if(fUnderlinePen!=nullptr){
		auto DrawStart=TextPos;
		auto DrawEnd=TextPos;
		DrawStart.Y+=fFontUnderlinePos;
		DrawEnd.Y+=fFontUnderlinePos;
		DrawEnd.X+=DrawWidth;
		Context->DrawLine(fUnderlinePen,DrawStart,DrawEnd);
	}

	// strike line
	if(fStrikePen!=nullptr){
		auto DrawStart=TextPos;
		auto DrawEnd=TextPos;
		DrawStart.Y+=fFontStrikePos;
		DrawEnd.Y+=fFontStrikePos;
		DrawEnd.X+=DrawWidth;
		Context->DrawLine(fStrikePen,DrawStart,DrawEnd);
	}
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
mcWPFUISimpleTextGraph::mcWPFUISimpleTextGraph()
{
}
//---------------------------------------------------------------------------
mcWPFUISimpleTextGraph::~mcWPFUISimpleTextGraph()
{
}
//---------------------------------------------------------------------------
void mcWPFUISimpleTextGraph::SetupText(const uChar32 *Text,uIntn Length,const cUITextStyle &Style,const Float32* TextDistance,Float32 TextMinHeight)
{
	auto Graph=gcnew rcWPFTextGraph();
	fTextSize=Graph->SetupText(Text,Length,Style,TextDistance,TextMinHeight);
	fGraph.Set(Graph);
}
//---------------------------------------------------------------------------
void mcWPFUISimpleTextGraph::Draw(cGCRef &Handle,cUIPoint DrawPosition,cUIPoint DrawSize)
{
	auto Graph=fGraph.Get();
	if(Graph!=nullptr){
		auto Context=Handle.Cast<System::Windows::Media::DrawingContext>();
		Graph->Draw(Context,DrawPosition,DrawSize);
	}
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
mcWPFUIRichTextGraph::mcWPFUIRichTextGraph()
{
}
//---------------------------------------------------------------------------
mcWPFUIRichTextGraph::~mcWPFUIRichTextGraph()
{
}
//---------------------------------------------------------------------------
void mcWPFUIRichTextGraph::Draw(cGCRef &Handle,cUIPoint DrawPosition,cUIPoint DrawSize)
{
	auto Text=fText.Get();
	if(Text==nullptr)
		return;

	System::Windows::Point DrawPos;
	DrawPos.X=DrawPosition.x;
	DrawPos.Y=DrawPosition.y;
	auto Context=Handle.Cast<System::Windows::Media::DrawingContext>();
	Context->DrawText(Text,DrawPos);
}
//---------------------------------------------------------------------------
void mcWPFUIRichTextGraph::SetupText(const uChar16 *Text,uIntn Length,const cUITextStyle &Style,const Float32* TextDistance,Float32 TextMinHeight)
{
	auto String=System::Runtime::InteropServices::Marshal::PtrToStringUni(System::IntPtr(const_cast<uChar16*>(Text)),Length);

    auto formattedText = gcnew System::Windows::Media::FormattedText(
        String,
        System::Globalization::CultureInfo::GetCultureInfo("en-us"),
        System::Windows::FlowDirection::LeftToRight,
        gcnew System::Windows::Media::Typeface("Verdana"),
        Style.FontSize,
        System::Windows::Media::Brushes::Black);

    // Set a maximum width and height. If the text overflows these values, an ellipsis "..." appears.
    //formattedText->MaxTextWidth = 300;
    //formattedText->MaxTextHeight = 240;

	fTextSize.y=formattedText->Extent;
	fTextSize.x=formattedText->WidthIncludingTrailingWhitespace;
	fText.Set(formattedText);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
mcWPFUIViewContentDrawingGroup::mcWPFUIViewContentDrawingGroup()
	: fDrawingGroup(gcnew System::Windows::Media::DrawingGroup())
{
}
//---------------------------------------------------------------------------
mcWPFUIViewContentDrawingGroup::~mcWPFUIViewContentDrawingGroup()
{
}
//---------------------------------------------------------------------------
void mcWPFUIViewContentDrawingGroup::mSetScale(Float32 Scale)
{
	auto Drawing=fDrawingGroup.Get();
	
	if(Scale==1.){
		Drawing->Transform=nullptr;
	}
	else{
		System::Double DrawingRestoreScale=1./Scale;
		auto DrawingRestoreTransform=gcnew System::Windows::Media::ScaleTransform(DrawingRestoreScale,DrawingRestoreScale);
		Drawing->Transform=DrawingRestoreTransform;
	}
}
//---------------------------------------------------------------------------
void mcWPFUIViewContentDrawingGroup::mRenderVisual(iUISimplePainter *Painter)
{
	auto Drawing=fDrawingGroup.Get();
	auto Context=Drawing->Open();

	mcWPFDrawingContext SimpleContext;
	SimpleContext.Start(Context);
	Painter->Paint(&SimpleContext);

	SimpleContext.Finish();

	Context->Close();
	delete Context;
}
//---------------------------------------------------------------------------
rcDNetGDIThreadContext::rcDNetGDIThreadContext()
{
	CPP=cDNetGDIThreadCPPContext_New();
	Microsoft::Win32::SystemEvents::DisplaySettingsChanged+=gcnew System::EventHandler(this,&rcDNetGDIThreadContext::OnDisplaySettingsChanged);
}
//---------------------------------------------------------------------------
rcDNetGDIThreadContext::~rcDNetGDIThreadContext()
{
	this->!rcDNetGDIThreadContext();
}
//---------------------------------------------------------------------------
rcDNetGDIThreadContext::!rcDNetGDIThreadContext()
{
	cDNetGDIThreadCPPContext_Delete(CPP);
	Microsoft::Win32::SystemEvents::DisplaySettingsChanged-=gcnew System::EventHandler(this,&rcDNetGDIThreadContext::OnDisplaySettingsChanged);
}
//---------------------------------------------------------------------------
void rcDNetGDIThreadContext::OnDisplaySettingsChanged(System::Object^,System::EventArgs^)
{
	cDNetGDIThreadCPPContext_NotifyDisplayChanged(CPP);
}
//---------------------------------------------------------------------------
rcDNetGDIThreadContext^ rcDNetGDIThreadContext::CurrentContext(void)
{
	auto TLS=gTLS;
	if(TLS->IsValueCreated){
		return TLS->Value;
	}
	auto NewContext=gcnew rcDNetGDIThreadContext();

	TLS->Value=NewContext;
	return NewContext;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void* cnWin::mDNetGDIThreadContext_ThreadContext(void)
{
	auto Context=rcDNetGDIThreadContext::CurrentContext();
	return Context->CPP;
}
//---------------------------------------------------------------------------
rcWPFD3DImageDrawing::rcWPFD3DImageDrawing(mcWPFD3DImageDrawing *CPP)
	: CPP(CPP)
{
}
//---------------------------------------------------------------------------
void rcWPFD3DImageDrawing::OnIsFrontBufferAvailableChanged(System::Object^, System::Windows::DependencyPropertyChangedEventArgs e)
{
	CPP->FrontBufferAvailable=static_cast<System::Boolean>(e.NewValue);
	CPP->FrontBufferAvailableChanged->Execute();
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
mcWPFD3DImageDrawing::mcWPFD3DImageDrawing()
	: Handle(gcnew System::Windows::Media::ImageDrawing())
	, Image(gcnew System::Windows::Interop::D3DImage())
{
	auto Callback=gcnew rcWPFD3DImageDrawing(this);
	auto Drawing=Handle.Get();
	auto DImage=Image.Get();
	DImage->IsFrontBufferAvailableChanged+=gcnew System::Windows::DependencyPropertyChangedEventHandler(Callback,&rcWPFD3DImageDrawing::OnIsFrontBufferAvailableChanged);

	Drawing->ImageSource=DImage;
	FrontBufferAvailable=DImage->IsFrontBufferAvailable;
}
//---------------------------------------------------------------------------
mcWPFD3DImageDrawing::~mcWPFD3DImageDrawing()
{
}
//---------------------------------------------------------------------------
void mcWPFD3DImageDrawing::mUpdateRect(Float32 Width,Float32 Height)
{
	System::Windows::Rect ContentRect;
	ContentRect.X=0;
	ContentRect.Y=0;
	ContentRect.Width=Width;
	ContentRect.Height=Height;
	auto Drawing=Handle.Get();
	Drawing->Rect=ContentRect;
}
//---------------------------------------------------------------------------
void mcWPFD3DImageDrawing::mApplyBackBuffer(HANDLE BackBuffer,int Width,int Height)
{
	auto DImage=Image.Get();
	DImage->Lock();
	DImage->SetBackBuffer(System::Windows::Interop::D3DResourceType::IDirect3DSurface9,System::IntPtr(BackBuffer));
	DImage->AddDirtyRect(System::Windows::Int32Rect(0,0,Width,Height));
	DImage->Unlock();
}
//---------------------------------------------------------------------------
void mcWPFD3DImageDrawing::mSetBackBuffer(void* BackBuffer)
{
	auto Drawing=Handle.Get();
	auto DImage=Image.Get();
	DImage->Lock();
	DImage->SetBackBuffer(System::Windows::Interop::D3DResourceType::IDirect3DSurface9,System::IntPtr(BackBuffer));
	DImage->Unlock();

	Drawing->ImageSource=DImage;

}
//---------------------------------------------------------------------------
void mcWPFD3DImageDrawing::mPresent(int Width,int Height)
{
	auto DImage=Image.Get();

	DImage->Lock();
	DImage->AddDirtyRect(System::Windows::Int32Rect(0,0,Width,Height));
	DImage->Unlock();
}
//---------------------------------------------------------------------------
