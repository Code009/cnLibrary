/*- cnWinCLI - Managed - View Content -------------------------------------*/
/*         Developer : Code009                                             */
/*         Create on : 2020-04-15                                          */
/*-------------------------------------------------------------------------*/
#pragma once
/*-------------------------------------------------------------------------*/
#if _MANAGED
#using <PresentationCore.dll>
#using <PresentationFramework.dll>
//#using <System.Core.dll>
#endif	/* _MANAGED */

#include <cnWinCLI\WinCLIM_Common.h>
#include <cnWinCLI\WinCLIM_WPFUIView.h>
#include <cnSystem\cnUIGraph.h>

#ifdef __cplusplus
//---------------------------------------------------------------------------
namespace cnLibrary{
//---------------------------------------------------------------------------
namespace cnWin{
//---------------------------------------------------------------------------
#if _MANAGED
//---------------------------------------------------------------------------
private ref class rcNativeCaller_WPFUIViewContent sealed
{
public:
	rcNativeCaller_WPFUIViewContent(void *CPP);

	void mbcWPFGDI_OnDisplaySettingsChanged(System::Object^ sender,System::EventArgs^ e);

	void mbcWPFD3DImageDrawing_OnIsFrontBufferAvailableChanged(System::Object^ sender, System::Windows::DependencyPropertyChangedEventArgs e);
private:
	void *CPP;
};
//---------------------------------------------------------------------------
#endif	// _MANAGED
//---------------------------------------------------------------------------
class mcWPFDrawingContext
{
public:
	void mSetup()noexcept(true);
	void mClear()noexcept(true);

	const cGCHandle& GetContextHandle(void)const noexcept(true);

	bool mFill(cUIPoint DrawPosition,cUIPoint DrawSize,cUIColor Color)noexcept(true);

#if _MANAGED
	System::Windows::Media::DrawingContext^ __clrcall GetContext(void)const noexcept(true);
	void __clrcall Start(System::Windows::Media::DrawingContext ^Context)noexcept(true);
	void __clrcall Finish(void)noexcept(true);
	System::Windows::Media::DrawingContext^ __clrcall Stop(void)noexcept(true);
#endif	// _MANAGED

protected:
#if _MANAGED
	mcGCHandle<System::Windows::Media::DrawingContext,eGCHandleType::Normal> fHandle;
#else
	cGCHandle fHandle;
#endif
};
//---------------------------------------------------------------------------
class mcWPFImageSource
{
public:
	void mSetup(void)noexcept(true);
	void mClear(void)noexcept(true);

	cUIPoint mGetImageSize(void)noexcept(true);
	
	void mTakeDrawing(cGCHandle &Drawing)noexcept(true);
	void mDraw(const cGCHandle &ContextHandle,cUIPoint DrawPosition,cUIPoint DrawSize)noexcept(true);

protected:
#if _MANAGED
	mcGCHandle<System::Windows::Media::ImageSource,eGCHandleType::Normal> fHandle;
#else
	cGCHandle fHandle;
#endif
};
//---------------------------------------------------------------------------
class mcWPFBitmapSource
{
public:
	void mSetup(void)noexcept(true);
	void mClear(void)noexcept(true);

	cUIPoint mGetImageSize(void)noexcept(true);

	void mTakeDrawing(cGCHandle &Drawing,int Width,int Height)noexcept(true);
	void mDraw(const cGCHandle &ContextHandle,cUIPoint DrawPosition,cUIPoint DrawSize)noexcept(true);

protected:
#if _MANAGED
	mcGCHandle<System::Windows::Media::Imaging::BitmapSource,eGCHandleType::Normal> fHandle;
#else
	cGCHandle fHandle;
#endif
};
//---------------------------------------------------------------------------
class mcWPFDrawingGroup
{
public:
	void mSetup(bool Create)noexcept(true);
	void mClear(void)noexcept(true);

	bool mStartDraw(mcWPFDrawingContext &mContext)noexcept(true);
	bool mStopDraw(mcWPFDrawingContext &mContext)noexcept(true);
	void mDiscard(void)noexcept(true);
	const cGCHandle& GetHandle(void)noexcept(true);
	cGCHandle& Fetch(void)noexcept(true);

	void mTransformScaleOffset(double X,double Y,double Scale)noexcept(true);
	void mTransformScale(Float32 Scale)noexcept(true);
protected:
#if _MANAGED
	mcGCHandle<System::Windows::Media::DrawingGroup,eGCHandleType::Normal> fHandle;
#else
	cGCHandle fHandle;
#endif

};
//---------------------------------------------------------------------------
class mcWPFTypeface
{
public:
	void mSetup(void)noexcept(true);
	void mClear(void)noexcept(true);

	const cGCHandle& GetHandle(void)const noexcept(true);

	Float32 mGetWeight(void)const noexcept(true);
	eUIFontStyle mGetFontStyle(void)const noexcept(true);

	bool mCreate(const uChar16 *Name,uIntn NameLength,eUIFontStyle FontStyle,Float32 FontWeight)noexcept(true);
protected:
#if _MANAGED
	mcGCHandle<System::Windows::Media::Typeface,eGCHandleType::Normal> fHandle;
#else
	cGCHandle fHandle;
#endif
};
//---------------------------------------------------------------------------
class mcWPFImageDrawing
{
public:
	void mSetup(void)noexcept(true);
	void mClear(void)noexcept(true);

	const cGCHandle& GetDrawingHandle(void)const noexcept(true);

	void mUpdateRect(double X,double Y,double Width,double Height)noexcept(true);

	void mApplyVisual(void *Section,uIntn Stride,int Width,int Height)noexcept(true);
protected:
#if _MANAGED
	mcGCHandle<System::Windows::Media::ImageDrawing,eGCHandleType::Normal> fHandle;
#else
	cGCHandle fHandle;
#endif

};
class miWPFTextLayout
{
public:
	void mCalculate(const uChar32 *Text,uIntn Length,const cUITextStyle &Style)noexcept(true);

	virtual void TextLayoutStoreTextHeight(Float32 Height)noexcept(true)=0;
	virtual void TextLayoutMakeTextDistance(uIntn Count)noexcept(true)=0;
	virtual void TextLayoutStoreTextDistance(uIntn Index,Float32 Distance)noexcept(true)=0;
};
//---------------------------------------------------------------------------
#if _MANAGED
//---------------------------------------------------------------------------
private value struct rcWPFTextLayout
{
public:

	property double TextWidth{	double get(void);	}
	property double TextHeight{	double get(void);	}
	property System::Collections::Generic::List<System::Double>^ AdvanceWidths{	System::Collections::Generic::List<System::Double>^ get(void);	}

	void __clrcall SetupText(const uChar32 *Text,uIntn Length,const cUITextStyle &Style,const Float32* TextDistance,Float32 TextMinHeight,Float32 LayoutScale);

	void CalculateTextLayout(miWPFTextLayout *Layout);

protected:
	System::Windows::Media::GlyphTypeface ^fGlyphTypeface;
	System::Collections::Generic::List<System::UInt16> ^fIndices;
	System::Collections::Generic::List<System::Double> ^fAdvanceWidths;
	System::Double fFontRenderingEmSize;
	System::Double fFontBaseline;
	System::Double fFontStrikePos;
	System::Double fFontUnderlinePos;
	System::Double fTextWidth;
	System::Double fTextHeight;
	System::Double fLayoutScale;
};
//---------------------------------------------------------------------------
#endif	// _MANAGED
//---------------------------------------------------------------------------
#if _MANAGED
private ref class rcWPFTextGraph sealed
{
public:

	cUIPoint __clrcall SetupText(const uChar32 *Text,uIntn Length,const cUITextStyle &Style,const Float32* TextDistance,Float32 TextMinHeight);
	void __clrcall Draw(System::Windows::Media::DrawingContext ^Context,cUIPoint DrawPosition,cUIPoint DrawSize);

protected:
	System::Windows::Media::GlyphTypeface ^fGlyphTypeface;
	System::Collections::Generic::List<System::UInt16> ^fIndices;
	System::Collections::Generic::List<System::Double> ^fAdvanceWidths;
	//System::Collections::Generic::List<System::UInt16> ^fClusterMap;
	//System::Collections::Generic::List<System::Windows::Point> ^fOffsets;
	//System::Collections::Generic::List<System::Boolean> ^fCaretStops;
	System::Windows::Media::Brush ^fForegroundBrush;
	System::Windows::Media::Brush ^fBackgroundBrush;
	System::Windows::Media::Pen ^fStrikePen;
	System::Windows::Media::Pen ^fUnderlinePen;
	System::Double fFontRenderingEmSize;
	System::Double fFontBaseline;
	System::Double fFontStrikePos;
	System::Double fFontUnderlinePos;
	System::Double fTextWidth;
	System::Double fTextHeight;
};
#endif // _MANAGED
//---------------------------------------------------------------------------
class mcWPFUISimpleTextGraph
{
public:
	void mSetup(void)noexcept(true);
	void mClear(void)noexcept(true);

	cUIPoint mSetupText(const uChar32 *Text,uIntn Length,const cUITextStyle &Style,const Float32* TextDistance,Float32 TextMinHeight)noexcept(true);

	void mDraw(const cGCHandle &ContextHandle,cUIPoint DrawPosition,cUIPoint DrawSize)noexcept(true);

protected:
#if _MANAGED
	mcGCHandle<rcWPFTextGraph,eGCHandleType::Normal> fGraph;
#else
	cGCHandle fGraph;
#endif
	
};
//---------------------------------------------------------------------------
class mcWPFUIRichTextGraph
{
public:
	void mSetup(void)noexcept(true);
	void mClear(void)noexcept(true);

	cUIPoint mSetupText(const uChar16 *Text,uIntn Length,const cUITextStyle &Style,const Float32* TextDistance,Float32 TextMinHeight)noexcept(true);

	void mDraw(const cGCHandle &Handle,cUIPoint DrawPosition,cUIPoint DrawSize)noexcept(true);
protected:

#if _MANAGED
	mcGCHandle<System::Windows::Media::FormattedText,eGCHandleType::Normal> fText;
#else
	cGCHandle fText;
#endif
};
//---------------------------------------------------------------------------
class mbcWPFGDI
{
public:
	void mSetup(void)noexcept(true);
	void mClear(void)noexcept(true);

protected:

#if _MANAGED
	mcGCHandle<rcNativeCaller_WPFUIViewContent> fCaller;
#else
	cGCHandle fCaller;
#endif
	virtual void WPFDisplaySettingsChanged(void)noexcept(true)=0;

private:
#if _MANAGED
	friend rcNativeCaller_WPFUIViewContent;
#endif

};
//---------------------------------------------------------------------------
class mbcWPFD3DImageDrawing
{
public:
	void mSetup(void)noexcept(true);
	void mClear(void)noexcept(true);

	void mUpdateRect(double X,double Y,double Width,double Height)noexcept(true);

	void mApplyBackBuffer(void *BackBuffer,int Width,int Height)noexcept(true);
	void mSetBackBuffer(void *BackBuffer)noexcept(true);
	void mPresent(int Width,int Height)noexcept(true);
protected:
#if _MANAGED
	mcGCHandle<System::Windows::Media::ImageDrawing,eGCHandleType::Normal> fDrawingHandle;
	mcGCHandle<System::Windows::Interop::D3DImage,eGCHandleType::Normal> fImageHandle;
#else
	cGCHandle fDrawingHandle;
	cGCHandle fImageHandle;
#endif

	bool fFrontBufferAvailable;

	virtual void D3DFrontBufferAvailableChanged(void)noexcept(true)=0;

private:
#if _MANAGED
	friend rcNativeCaller_WPFUIViewContent;
#endif
};
//---------------------------------------------------------------------------
}	// namespace cnWin
//---------------------------------------------------------------------------
}	// namespace cnLibrary
//---------------------------------------------------------------------------
#endif  /* __cplusplus */
/*-------------------------------------------------------------------------*/
