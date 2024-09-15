#define	WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include "WinCLIM_WPFUIViewContent.h"

using namespace cnLibrary;
using namespace cnWin;

//---------------------------------------------------------------------------
rcNativeCaller_WPFUIViewContent::rcNativeCaller_WPFUIViewContent(void *CPP)
	: CPP(CPP)
{
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void mcWPFDrawingContext::mSetup(void)noexcept
{
	fHandle.Alloc(nullptr);
}
//---------------------------------------------------------------------------
void mcWPFDrawingContext::mClear(void)noexcept
{
	fHandle.Free();
}
//---------------------------------------------------------------------------
#pragma unmanaged
const cGCHandle& mcWPFDrawingContext::GetContextHandle(void)const noexcept
{
	return fHandle;
}
#pragma managed
//---------------------------------------------------------------------------
System::Windows::Media::DrawingContext^ mcWPFDrawingContext::GetContext(void)const noexcept
{
	return fHandle.Get();
}
//---------------------------------------------------------------------------
void mcWPFDrawingContext::Start(System::Windows::Media::DrawingContext ^Context)noexcept
{
	fHandle.Set(Context);
}
//---------------------------------------------------------------------------
void mcWPFDrawingContext::Finish(void)noexcept
{
	auto Context=fHandle.Get();
	fHandle.Set(nullptr);
}
//---------------------------------------------------------------------------
System::Windows::Media::DrawingContext^ mcWPFDrawingContext::Stop(void)noexcept
{
	auto Context=fHandle.Get();
	fHandle.Set(nullptr);
	return Context;
}
//---------------------------------------------------------------------------
bool mcWPFDrawingContext::mFill(cUIPoint DrawPosition,cUIPoint DrawSize,cUIColor Color)noexcept
{
	System::Windows::Rect DrawRC;
	DrawRC.X=DrawPosition.x;
	DrawRC.Y=DrawPosition.y;
	DrawRC.Width=DrawSize.x;
	DrawRC.Height=DrawSize.y;
	auto Context=fHandle.Get();
	auto Brush=gcnew System::Windows::Media::SolidColorBrush(System::Windows::Media::Color::FromScRgb(Color.Alpha,Color.Red,Color.Green,Color.Blue));
	Brush->Freeze();
	Context->DrawRectangle(Brush,nullptr,DrawRC);
	return true;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void mcWPFDrawingGroup::mSetup(bool Create)noexcept
{
	if(Create){
		fHandle.Alloc(gcnew System::Windows::Media::DrawingGroup());
	}
	else{
		fHandle.Alloc(nullptr);
	}
}
//---------------------------------------------------------------------------
void mcWPFDrawingGroup::mClear(void)noexcept
{
	fHandle.Free();
}
//---------------------------------------------------------------------------
bool mcWPFDrawingGroup::mStartDraw(mcWPFDrawingContext &mContext)noexcept
{
	auto Drawing=fHandle.Get();
	if(Drawing==nullptr){
		Drawing=gcnew System::Windows::Media::DrawingGroup();
		fHandle.Set(Drawing);
	}

	try{
		auto Context=Drawing->Open();
		mContext.Start(Context);
		return true;
	}
	catch(System::Exception ^){
		return false;
	}
}
//---------------------------------------------------------------------------
bool mcWPFDrawingGroup::mStopDraw(mcWPFDrawingContext &mContext)noexcept
{
	auto Context=mContext.Stop();
	if(Context!=nullptr){
		Context->Close();
		return true;
	}
	return false;
}
//---------------------------------------------------------------------------
void mcWPFDrawingGroup::mDiscard(void)noexcept
{
	fHandle.Set(nullptr);
}
//---------------------------------------------------------------------------
#pragma unmanaged
//---------------------------------------------------------------------------
const cGCHandle& mcWPFDrawingGroup::GetHandle(void)noexcept
{
	return fHandle;
}
//---------------------------------------------------------------------------
cGCHandle& mcWPFDrawingGroup::Fetch(void)noexcept
{
	return fHandle;
}
//---------------------------------------------------------------------------
#pragma managed
//---------------------------------------------------------------------------
void mcWPFDrawingGroup::mTransformScaleOffset(double X,double Y,double Scale)noexcept
{
	auto Drawing=fHandle.Get();

	if(X==0. && Y==0. && Scale==1.){
		Drawing->Transform=nullptr;
	}
	else{
		System::Double MatrixScale=1./Scale;
		auto m=System::Windows::Media::Matrix::Identity;
		m.Translate(X,Y);
		m.Scale(MatrixScale,MatrixScale);
		Drawing->Transform=gcnew System::Windows::Media::MatrixTransform(m);
	}
}
//---------------------------------------------------------------------------
void mcWPFDrawingGroup::mTransformScale(Float32 Scale)noexcept
{
	auto Drawing=fHandle.Get();

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
//---------------------------------------------------------------------------
void mcWPFImageSource::mSetup(void)noexcept
{
	fHandle.Alloc(nullptr);
}
//---------------------------------------------------------------------------
void mcWPFImageSource::mClear(void)noexcept
{
	fHandle.Free();
}
//---------------------------------------------------------------------------
cUIPoint mcWPFImageSource::mGetImageSize(void)noexcept
{
	auto ImageSource=fHandle.Get();

	cUIPoint ImageSize;
	ImageSize.x=static_cast<Float32>(ImageSource->Width);
	ImageSize.y=static_cast<Float32>(ImageSource->Height);
	return ImageSize;
}
//---------------------------------------------------------------------------
void mcWPFImageSource::mTakeDrawing(cGCHandle &DrawingHandle)noexcept
{
	auto Drawing=DrawingHandle.Cast<System::Windows::Media::Drawing>();
	if(Drawing==nullptr)
		return;
	DrawingHandle.Clear();

	auto ImageSource=gcnew System::Windows::Media::DrawingImage(Drawing);

	ImageSource->Freeze();

	fHandle.Set(ImageSource);
}
//---------------------------------------------------------------------------
void mcWPFImageSource::mDraw(const cGCHandle &ContextHandle,cUIPoint DrawPosition,cUIPoint DrawSize)noexcept
{
	auto Context=ContextHandle.DynamicCast<System::Windows::Media::DrawingContext>();
	if(Context==nullptr)
		return;
	System::Windows::Rect DrawRect;
	DrawRect.X=DrawPosition.x;
	DrawRect.Y=DrawPosition.y;
	DrawRect.Width=DrawSize.x;
	DrawRect.Height=DrawSize.y;
	Context->DrawImage(fHandle.Get(),DrawRect);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void mcWPFBitmapSource::mSetup(void)noexcept
{
	fHandle.Alloc(nullptr);
}
//---------------------------------------------------------------------------
void mcWPFBitmapSource::mClear(void)noexcept
{
	fHandle.Free();
}
//---------------------------------------------------------------------------
cUIPoint mcWPFBitmapSource::mGetImageSize(void)noexcept
{
	auto Source=fHandle.Get();

	cUIPoint ImageSize;
	ImageSize.x=static_cast<Float32>(Source->Width);
	ImageSize.y=static_cast<Float32>(Source->Height);
	return ImageSize;
}
//---------------------------------------------------------------------------
void mcWPFBitmapSource::mTakeDrawing(cGCHandle &DrawingHandle,int Width,int Height)noexcept
{
	auto Drawing=DrawingHandle.Cast<System::Windows::Media::Drawing>();
	if(Drawing==nullptr)
		return;
	DrawingHandle.Clear();

	auto TempVisual=gcnew System::Windows::Media::DrawingVisual();
	auto Context=TempVisual->RenderOpen();
	Context->DrawDrawing(Drawing);
	Context->Close();
	auto BitmapTarget=gcnew System::Windows::Media::Imaging::RenderTargetBitmap(Width,Height,96,96,System::Windows::Media::PixelFormats::Bgra32);
	BitmapTarget->Render(TempVisual);

	BitmapTarget->Freeze();

	fHandle.Set(BitmapTarget);
}
//---------------------------------------------------------------------------
void mcWPFBitmapSource::mDraw(const cGCHandle &ContextHandle,cUIPoint DrawPosition,cUIPoint DrawSize)noexcept
{
	auto Context=ContextHandle.DynamicCast<System::Windows::Media::DrawingContext>();
	if(Context==nullptr)
		return;
	System::Windows::Rect DrawRect;
	DrawRect.X=DrawPosition.x;
	DrawRect.Y=DrawPosition.y;
	DrawRect.Width=DrawSize.x;
	DrawRect.Height=DrawSize.y;
	Context->DrawImage(fHandle.Get(),DrawRect);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void mcWPFTypeface::mSetup(void)noexcept
{
	fHandle.Alloc(nullptr);
}
//---------------------------------------------------------------------------
void mcWPFTypeface::mClear(void)noexcept
{
	fHandle.Free();
}
//---------------------------------------------------------------------------
#pragma unmanaged
const cGCHandle& mcWPFTypeface::GetHandle(void)const noexcept
{
	return fHandle;
}
#pragma managed
//---------------------------------------------------------------------------
bool mcWPFTypeface::mCreate(const uChar16 *Name,uIntn NameLength,eUIFontStyle FontStyle,Float32 FontWeight)noexcept
{
	try{
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
		fHandle.Set(Typeface);
		return true;
	}
	catch(System::Exception^){
		return false;
	}
}
//---------------------------------------------------------------------------
Float32 mcWPFTypeface::mGetWeight(void)const noexcept
{
	auto Typeface=fHandle.Get();
	int GDIWeight=Typeface->Weight.ToOpenTypeWeight();
	if(GDIWeight<FW_THIN)
		GDIWeight=FW_THIN;
	if(GDIWeight>FW_HEAVY)
		GDIWeight=FW_HEAVY;
	Float32 FontWeight=(GDIWeight-FW_NORMAL)/500.f;
	return FontWeight;
}
//---------------------------------------------------------------------------
eUIFontStyle mcWPFTypeface::mGetFontStyle(void)const noexcept
{
	auto Typeface=fHandle.Get();
	if(Typeface->Style==System::Windows::FontStyles::Italic){
		return UIFontStyle::Italic;
	}
	else{
		return UIFontStyle::Normal;
	}
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void mcWPFImageDrawing::mSetup(void)noexcept
{
	fHandle.Alloc(gcnew System::Windows::Media::ImageDrawing());
}
//---------------------------------------------------------------------------
void mcWPFImageDrawing::mClear(void)noexcept
{
	fHandle.Free();
}
//---------------------------------------------------------------------------
#pragma unmanaged
const cGCHandle& mcWPFImageDrawing::GetDrawingHandle(void)const noexcept
{
	return fHandle;
}
#pragma managed
//---------------------------------------------------------------------------
void mcWPFImageDrawing::mUpdateRect(double X,double Y,double Width,double Height)noexcept
{
	System::Windows::Rect ContentRect;
	ContentRect.X=X;
	ContentRect.Y=Y;
	ContentRect.Width=Width;
	ContentRect.Height=Height;
	auto Drawing=fHandle.Get();
	Drawing->Rect=ContentRect;
}
//---------------------------------------------------------------------------
void mcWPFImageDrawing::mApplyVisual(void *Section,uIntn Stride,int Width,int Height)noexcept
{
	auto Drawing=fHandle.Get();

	auto InteropSource=dynamic_cast<System::Windows::Interop::InteropBitmap^>(Drawing->ImageSource);
	if(InteropSource!=nullptr){
		if(InteropSource->PixelWidth==Width && InteropSource->PixelHeight==Height){
			// invalid existing bitmap only
			InteropSource->Invalidate();
			return;
		}
	}
	// make bitmap source from memory
	auto NewBitmapSource=System::Windows::Interop::Imaging::CreateBitmapSourceFromMemorySection(System::IntPtr(Section),Width,Height,
		System::Windows::Media::PixelFormats::Bgra32,static_cast<int>(Stride),0);

	// apply image
	Drawing->ImageSource=NewBitmapSource;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void miWPFTextLayout::mCalculate(const uChar32 *Text,uIntn Length,const cUITextStyle &Style)noexcept
{
	rcWPFTextLayout TextLayout;
	TextLayout.SetupText(Text,Length,Style,nullptr,0,1.);
	TextLayout.CalculateTextLayout(this);
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
void rcWPFTextLayout::CalculateTextLayout(miWPFTextLayout *Layout)
{
	Float32 TextHeight=static_cast<Float32>(fTextHeight*fLayoutScale);
	TextHeight*=fLayoutScale;

	Layout->TextLayoutStoreTextHeight(TextHeight);

	Layout->TextLayoutMakeTextDistance(fAdvanceWidths->Count);
	uIntn i=0;
	for each(auto Width in fAdvanceWidths){
		Layout->TextLayoutStoreTextDistance(i++,static_cast<Float32>(Width*fLayoutScale));
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
void mcWPFUISimpleTextGraph::mSetup(void)noexcept
{
	fGraph.Alloc(nullptr);
}
//---------------------------------------------------------------------------
void mcWPFUISimpleTextGraph::mClear(void)noexcept
{
	fGraph.Free();
}
//---------------------------------------------------------------------------
cUIPoint mcWPFUISimpleTextGraph::mSetupText(const uChar32 *Text,uIntn Length,const cUITextStyle &Style,const Float32* TextDistance,Float32 TextMinHeight)noexcept
{
	auto Graph=gcnew rcWPFTextGraph();
	auto TextSize=Graph->SetupText(Text,Length,Style,TextDistance,TextMinHeight);
	fGraph.Set(Graph);
	return TextSize;
}
//---------------------------------------------------------------------------
void mcWPFUISimpleTextGraph::mDraw(const cGCHandle &Handle,cUIPoint DrawPosition,cUIPoint DrawSize)noexcept
{
	auto Graph=fGraph.Get();
	if(Graph!=nullptr){
		auto Context=Handle.Cast<System::Windows::Media::DrawingContext>();
		Graph->Draw(Context,DrawPosition,DrawSize);
	}
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void mcWPFUIRichTextGraph::mSetup(void)noexcept
{
	fText.Alloc(nullptr);
}
//---------------------------------------------------------------------------
void mcWPFUIRichTextGraph::mClear(void)noexcept
{
	fText.Free();
}
//---------------------------------------------------------------------------
void mcWPFUIRichTextGraph::mDraw(const cGCHandle &Handle,cUIPoint DrawPosition,cUIPoint DrawSize)noexcept
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
cUIPoint mcWPFUIRichTextGraph::mSetupText(const uChar16 *Text,uIntn Length,const cUITextStyle &Style,const Float32* TextDistance,Float32 TextMinHeight)noexcept
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

	cUIPoint TextSize;
	TextSize.y=formattedText->Extent;
	TextSize.x=formattedText->WidthIncludingTrailingWhitespace;
	fText.Set(formattedText);

	return TextSize;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void rcNativeCaller_WPFUIViewContent::mbcWPFGDI_OnDisplaySettingsChanged(System::Object^,System::EventArgs^)
{
	static_cast<mbcWPFGDI*>(CPP)->WPFDisplaySettingsChanged();
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void mbcWPFGDI::mSetup(void)noexcept
{
	auto Caller=gcnew rcNativeCaller_WPFUIViewContent(this);
	fCaller.Alloc(Caller);
	Microsoft::Win32::SystemEvents::DisplaySettingsChanged+=gcnew System::EventHandler(Caller,&rcNativeCaller_WPFUIViewContent::mbcWPFGDI_OnDisplaySettingsChanged);
}
//---------------------------------------------------------------------------
void mbcWPFGDI::mClear(void)noexcept
{
	auto Caller=fCaller.Get();
	fCaller.Free();
	Microsoft::Win32::SystemEvents::DisplaySettingsChanged-=gcnew System::EventHandler(Caller,&rcNativeCaller_WPFUIViewContent::mbcWPFGDI_OnDisplaySettingsChanged);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void rcNativeCaller_WPFUIViewContent::mbcWPFD3DImageDrawing_OnIsFrontBufferAvailableChanged(System::Object^, System::Windows::DependencyPropertyChangedEventArgs e)
{
	auto Drawing=static_cast<mbcWPFD3DImageDrawing*>(CPP);
	Drawing->fFrontBufferAvailable=static_cast<System::Boolean>(e.NewValue);
	Drawing->D3DFrontBufferAvailableChanged();
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void mbcWPFD3DImageDrawing::mSetup(void)noexcept
{
	auto Drawing=gcnew System::Windows::Media::ImageDrawing();
	auto DImage=gcnew System::Windows::Interop::D3DImage();
	fDrawingHandle.Alloc(Drawing);
	fImageHandle.Alloc(DImage);
	auto Callback=gcnew rcNativeCaller_WPFUIViewContent(this);
	DImage->IsFrontBufferAvailableChanged+=gcnew System::Windows::DependencyPropertyChangedEventHandler(Callback,&rcNativeCaller_WPFUIViewContent::mbcWPFD3DImageDrawing_OnIsFrontBufferAvailableChanged);

	Drawing->ImageSource=DImage;
	fFrontBufferAvailable=DImage->IsFrontBufferAvailable;
}
//---------------------------------------------------------------------------
void mbcWPFD3DImageDrawing::mClear(void)noexcept
{
	fDrawingHandle.Free();
	fImageHandle.Free();
}
//---------------------------------------------------------------------------
void mbcWPFD3DImageDrawing::mUpdateRect(double X,double Y,double Width,double Height)noexcept
{
	System::Windows::Rect ContentRect;
	ContentRect.X=X;
	ContentRect.Y=Y;
	ContentRect.Width=Width;
	ContentRect.Height=Height;
	auto Drawing=fDrawingHandle.Get();
	Drawing->Rect=ContentRect;
}
//---------------------------------------------------------------------------
void mbcWPFD3DImageDrawing::mApplyBackBuffer(void *BackBuffer,int Width,int Height)noexcept
{
	auto DImage=fImageHandle.Get();
	DImage->Lock();
	DImage->SetBackBuffer(System::Windows::Interop::D3DResourceType::IDirect3DSurface9,System::IntPtr(BackBuffer));
	DImage->AddDirtyRect(System::Windows::Int32Rect(0,0,Width,Height));
	DImage->Unlock();
}
//---------------------------------------------------------------------------
void mbcWPFD3DImageDrawing::mSetBackBuffer(void* BackBuffer)noexcept
{
	auto Drawing=fDrawingHandle.Get();
	auto DImage=fImageHandle.Get();
	DImage->Lock();
	DImage->SetBackBuffer(System::Windows::Interop::D3DResourceType::IDirect3DSurface9,System::IntPtr(BackBuffer));
	DImage->Unlock();

	Drawing->ImageSource=DImage;

}
//---------------------------------------------------------------------------
void mbcWPFD3DImageDrawing::mPresent(int Width,int Height)noexcept
{
	auto DImage=fImageHandle.Get();

	DImage->Lock();
	DImage->AddDirtyRect(System::Windows::Int32Rect(0,0,Width,Height));
	DImage->Unlock();
}
//---------------------------------------------------------------------------
