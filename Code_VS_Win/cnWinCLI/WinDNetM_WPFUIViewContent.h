/*- cnWinWPF - Managed - View Content -------------------------------------*/
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

#include <cnWinCLI\WinDNetM_Common.h>
#include <cnWinCLI\WinDNetM_WPFUIView.h>

#ifdef __cplusplus
//---------------------------------------------------------------------------
namespace cnLibrary{
//---------------------------------------------------------------------------
namespace cnWin{
//---------------------------------------------------------------------------
class mcWPFDrawingContext : public iUISimplePaintContext
{
public:
	mcWPFDrawingContext()noexcept(true);
	~mcWPFDrawingContext()noexcept(true);

	virtual bool cnLib_FUNC Fill(cUIPoint DrawPosition,cUIPoint DrawSize,cUIColor Color)noexcept(true)override;
	virtual bool cnLib_FUNC Graph(cUIPoint DrawPosition,cUIPoint DrawSize,iUIGraph *Graph)noexcept(true)override;
	virtual bool cnLib_FUNC Image(cUIPoint DrawPosition,cUIPoint DrawSize,iUIBitmap *Bitmap,eImageBlendingOperator BlendingOperator=ImageBlendingOperator::Alpha)noexcept(true)override;

	bool mFill(cUIPoint DrawPosition,cUIPoint DrawSize,cUIColor Color)noexcept(true);

#if _MANAGED
	void __clrcall Start(System::Windows::Media::DrawingContext ^Context)noexcept(true);
	void __clrcall Finish(void)noexcept(true);
	System::Windows::Media::DrawingContext^ __clrcall Stop(void)noexcept(true);
#endif	// _MANAGED

protected:
#if _MANAGED
	mcGCHandleT<System::Windows::Media::DrawingContext> Handle;
#else
	cGCRef Handle;
#endif
};
//---------------------------------------------------------------------------
struct mcWPFDrawing
{
#if _MANAGED
	mcWPFDrawing(System::Windows::Media::Drawing ^Drawing)noexcept(true);
	template<class TSrc>	mcWPFDrawing(mcGCRefT<TSrc> &&Src)noexcept(true): Handle(cnVar::MoveCast(Src)){}
#endif // _MANAGED
	mcWPFDrawing(mcWPFDrawing &&Src)noexcept(true);
	~mcWPFDrawing()noexcept(true);

#if _MANAGED
	mcGCRefT<System::Windows::Media::Drawing> Handle;
#else
	cGCRef Handle;
#endif
};
//---------------------------------------------------------------------------
struct mcWPFUIBitmap
{
#if _MANAGED
	mcWPFUIBitmap(System::Windows::Media::ImageSource ^Image)noexcept(true);
#endif // _MANAGED

	mcWPFUIBitmap(mcWPFUIBitmap &&Src)noexcept(true);
	~mcWPFUIBitmap()noexcept(true);

#if _MANAGED
	mcGCRefT<System::Windows::Media::ImageSource> Handle;
#else
	cGCRef Handle;
#endif

	cUIPoint ImageSize;

	void Draw(cGCRef &ContextHandle,cUIPoint DrawPosition,cUIPoint DrawSize)noexcept(true);

	static mcWPFUIBitmap FromDrawing(const mcWPFDrawing &Drawing)noexcept(true);
};
//---------------------------------------------------------------------------
struct mcWPFBitmapDataSource
{
#if _MANAGED
	mcWPFBitmapDataSource(System::Windows::Media::Imaging::BitmapSource ^BitmapSource)noexcept(true);
#endif

	mcWPFBitmapDataSource(mcWPFBitmapDataSource &&Src)noexcept(true);
	~mcWPFBitmapDataSource()noexcept(true);


#if _MANAGED
	mcGCRefT<System::Windows::Media::Imaging::BitmapSource> Handle;
#else
	cGCRef Handle;
#endif

	cUIPoint ImageSize;
	
	void Draw(cGCRef &ContextHandle,cUIPoint DrawPosition,cUIPoint DrawSize)noexcept(true);

	static mcWPFBitmapDataSource FromDrawing(const mcWPFDrawing &Drawing,cUIPoint Size)noexcept(true);
};
//---------------------------------------------------------------------------
class mcWPFUISimpleBitmapCanvas
{
public:
	mcWPFUISimpleBitmapCanvas(cUIPoint Size)noexcept(true);
	mcWPFUISimpleBitmapCanvas(mcWPFUISimpleBitmapCanvas &&Src)noexcept(true);
	~mcWPFUISimpleBitmapCanvas()noexcept(true);

protected:
	cUIPoint fBitmapSize;

#if _MANAGED
	mcGCRefT<System::Windows::Media::DrawingGroup> DrawingHandle;
#else
	cGCRef DrawingHandle;
#endif

	void StartContext(mcWPFDrawingContext &mContext,cUIColor Color)noexcept(true);
	void DiscardContext(mcWPFDrawingContext &mContext)noexcept(true);
	mcWPFDrawing FinishContext(mcWPFDrawingContext &mContext,bool &Success)noexcept(true);
};
//---------------------------------------------------------------------------
struct mcWPFImageDrawing
{
	mcWPFImageDrawing()noexcept(true);
	~mcWPFImageDrawing()noexcept(true);

#if _MANAGED
	mcGCRefT<System::Windows::Media::ImageDrawing> Handle;
#else
	cGCRef Handle;
#endif

	void mUpdateRect(Float32 Width,Float32 Height)noexcept(true);

	void mApplyVisual(const cnRTL::cnWinRTL::cGDIBitmapSection &BitmapBuffer,Float32 Width,Float32 Height)noexcept(true);
};
//---------------------------------------------------------------------------
struct mcWPFTypeface
{
#if _MANAGED
	mcWPFTypeface(System::Windows::Media::Typeface ^Typeface)noexcept(true);
#endif // _MANAGED
	~mcWPFTypeface()noexcept(true);
	mcWPFTypeface(mcWPFTypeface &&Src)noexcept(true);

	static mcWPFTypeface CreateTypeface(const uChar16 *Name,uIntn NameLength,eUIFontStyle FontStyle,Float32 FontWeight)noexcept(true);

#if _MANAGED
	mcGCRefT<System::Windows::Media::Typeface> Handle;
#else
	cGCRef Handle;
#endif

	Float32 FontWeight;
	eUIFontStyle FontStyle;
};
//---------------------------------------------------------------------------
struct mcWPFTextLayout
{
	Float32 TextHeight;
	cnRTL::cSeqList<Float32> TextDistance;

	void Setup(const uChar32 *Text,uIntn Length,const cUITextStyle &Style)noexcept(true);
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

	void CalculateTextLayout(mcWPFTextLayout &Layout);

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
private ref class rcWPFTextGraph
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
	mcWPFUISimpleTextGraph()noexcept(true);
	~mcWPFUISimpleTextGraph()noexcept(true);

	void SetupText(const uChar32 *Text,uIntn Length,const cUITextStyle &Style,const Float32* TextDistance,Float32 TextMinHeight)noexcept(true);

	void Draw(cGCRef &ContextHandle,cUIPoint DrawPosition,cUIPoint DrawSize)noexcept(true);

protected:

#if _MANAGED
	mcGCRefT<rcWPFTextGraph> fGraph;
#else
	cGCRef fGraph;
#endif

	cUIPoint fTextSize;
	
	
};
//---------------------------------------------------------------------------
class mcWPFUIRichTextGraph
{
public:
	mcWPFUIRichTextGraph()noexcept(true);
	~mcWPFUIRichTextGraph()noexcept(true);


	void SetupText(const uChar16 *Text,uIntn Length,const cUITextStyle &Style,const Float32* TextDistance,Float32 TextMinHeight)noexcept(true);

	void Draw(cGCRef &Handle,cUIPoint DrawPosition,cUIPoint DrawSize)noexcept(true);
protected:

#if _MANAGED
	mcGCRefT<System::Windows::Media::FormattedText> fText;
#else
	cGCRef fText;
#endif

	cUIPoint fTextSize;


};
//---------------------------------------------------------------------------
class mcWPFUIViewContentDrawingGroup
{
public:
	mcWPFUIViewContentDrawingGroup()noexcept(true);
	~mcWPFUIViewContentDrawingGroup()noexcept(true);

	cGCRef& GetDrawingGroup(void)noexcept(true);

	void mSetScale(Float32 Scale)noexcept(true);
	void mRenderVisual(iUISimplePainter *Painter)noexcept(true);
protected:
#if _MANAGED
	mcGCRefT<System::Windows::Media::DrawingGroup> fDrawingGroup;
#else
	cGCRef fDrawingGroup;
#endif

};
//---------------------------------------------------------------------------
#if _MANAGED
//---------------------------------------------------------------------------
private ref class rcDNetGDIThreadContext
{
public:
	rcDNetGDIThreadContext();
	~rcDNetGDIThreadContext();
	!rcDNetGDIThreadContext();
	
	static rcDNetGDIThreadContext^ CurrentContext(void);

	void *CPP;

private:
	void OnDisplaySettingsChanged(System::Object^ sender,System::EventArgs^ e);
	static System::Threading::ThreadLocal<rcDNetGDIThreadContext^> ^gTLS=gcnew System::Threading::ThreadLocal<rcDNetGDIThreadContext^>();
};
//---------------------------------------------------------------------------
#endif	// _MANAGED
void* mDNetGDIThreadContext_ThreadContext(void)noexcept(true);
void* cDNetGDIThreadCPPContext_New(void)noexcept(true);
void cDNetGDIThreadCPPContext_Delete(void *p)noexcept(true);
void cDNetGDIThreadCPPContext_NotifyDisplayChanged(void *p)noexcept(true);
//---------------------------------------------------------------------------
struct mcWPFD3DImageDrawing;
#if _MANAGED
private ref class rcWPFD3DImageDrawing
{
internal:
	rcWPFD3DImageDrawing(mcWPFD3DImageDrawing *CPP);

	mcWPFD3DImageDrawing *CPP;
	void OnIsFrontBufferAvailableChanged(System::Object^ sender, System::Windows::DependencyPropertyChangedEventArgs e);
};
#endif // _MANAGED
//---------------------------------------------------------------------------
struct mcWPFD3DImageDrawing
{
	mcWPFD3DImageDrawing()noexcept(true);
	~mcWPFD3DImageDrawing()noexcept(true);

#if _MANAGED
	mcGCRefT<System::Windows::Media::ImageDrawing> Handle;
	mcGCRefT<System::Windows::Interop::D3DImage> Image;
#else
	cGCRef Handle;
	cGCRef Image;
#endif

	void mUpdateRect(Float32 Width,Float32 Height)noexcept(true);

	iProcedure *FrontBufferAvailableChanged;
	bool FrontBufferAvailable;

	void mApplyBackBuffer(HANDLE BackBuffer,int Width,int Height)noexcept(true);
	void mSetBackBuffer(void *BackBuffer)noexcept(true);
	void mPresent(int Width,int Height)noexcept(true);

};
//---------------------------------------------------------------------------
}	// namespace cnWin
//---------------------------------------------------------------------------
}	// namespace cnLibrary
//---------------------------------------------------------------------------
#endif  /* __cplusplus */
/*-------------------------------------------------------------------------*/
