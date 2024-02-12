/*- cnSystem - User Interface ---------------------------------------------*/
/*         Developer : Code009                                             */
/*         Create on : 2015-05-15                                          */
/*-------------------------------------------------------------------------*/
#ifndef __cnLibrary_cnSystem_cnUIGraph_Header__
#define	__cnLibrary_cnSystem_cnUIGraph_Header__
/*-------------------------------------------------------------------------*/
#include <cnSystem/cnFundamental.h>
#include <cnSystem/cnFileSystem.h>
#include <cnSystem/cnUI.h>
/*-------------------------------------------------------------------------*/
#ifdef __cplusplus
//---------------------------------------------------------------------------
namespace cnLibrary{
//---------------------------------------------------------------------------
struct cUIColor
{
	Float32 Red;
	Float32 Green;
	Float32 Blue;
	Float32 Alpha;
};
//---------------------------------------------------------------------------
class cnLib_INTERFACE iUIViewContent : public iReference
{
public:
	virtual iUIView* cnLib_FUNC GetView(void)=0;
	virtual bool cnLib_FUNC SetView(iUIView *View)=0;

	virtual bool cnLib_FUNC GetVisible(void)=0;
	virtual bool cnLib_FUNC SetVisible(bool Visible)=0;

	virtual Float32 cnLib_FUNC GetZPosition(void)=0;
	virtual bool cnLib_FUNC SetZPosition(Float32 ZPosition)=0;

	virtual Float32 cnLib_FUNC GetContentScale(void)=0;

	//virtual bool cnLib_FUNC GetTopLayer(void)=0;
	//virtual bool cnLib_FUNC SetTopLayer(bool TopLayer)=0;
};
//---------------------------------------------------------------------------
class cnLib_INTERFACE iUIPainter
{
public:
	// PaintStarted
	//	called when content move to background
	virtual void cnLib_FUNC PaintStarted(void)=0;
	// PaintShow
	//	called when content become visible
	virtual void cnLib_FUNC PaintShow(void)=0;
	// PaintResume
	//	called when content become active
	virtual void cnLib_FUNC PaintResume(void)=0;
	// PaintPause
	//	called when content become inactive
	virtual void cnLib_FUNC PaintPaused(void)=0;
	// PaintHide
	//	called when content become invisible
	virtual void cnLib_FUNC PaintHide(void)=0;
	// PaintStopped
	//	called when content move to background
	virtual void cnLib_FUNC PaintStopped(void)=0;
};
//---------------------------------------------------------------------------
cnLib_ENUM_BEGIN(ufInt8,UIFontStyle)
{
	Normal,
	Italic,
}cnLib_ENUM_END(UIFontStyle);
//---------------------------------------------------------------------------
class cnLib_INTERFACE iUIFont : public iInterface
{
public:
	virtual rPtr< iArrayReference<const uChar16> > cnLib_FUNC GetName(void)=0;
	virtual eUIFontStyle cnLib_FUNC GetStyle(void)=0;

	// 0 is normal weight, range from -1.0 to 1.0
	virtual Float32 cnLib_FUNC GetWeight(void)=0;
};
//---------------------------------------------------------------------------
class cnLib_INTERFACE iUIGraph : public iInterface
{
public:
	virtual cUIPoint cnLib_FUNC GetSize(void)=0;
};
//---------------------------------------------------------------------------
class cnLib_INTERFACE iUIBitmap : public iUIGraph
{
public:
	//virtual iRet<iBitmapDataSource> cnLib_FUNC rpReadbackBitmapData(void)=0;
	//
	//iPtr<iBitmapDataSource> ReadbackBitmapData(void){
	//	return RetToPtr(rpReadbackBitmapData());
	//}
};
//---------------------------------------------------------------------------
struct cUITextStyle
{
	iPtr<iUIFont>	Font;
	Float32			FontSize;	// size in pixel
	bool			Strike		:1;
	bool			Underline	:1;

	cUIColor TextColor;
	cUIColor BackgroundColor;
};
//---------------------------------------------------------------------------
class cnLib_INTERFACE iUITextLineLayout : public iReference
{
public:
	virtual uIntn cnLib_FUNC GetTextLength(void)=0;
	virtual Float32 cnLib_FUNC GetTextHeight(void)=0;
	virtual const Float32* cnLib_FUNC GetTextDistance(void)=0;
};
//---------------------------------------------------------------------------
class cnLib_INTERFACE iUITextGraph : public iUIGraph
{
public:
	//virtual rPtr<iUITextLineLayout> QueryTextLayout(void)=0;
};
//---------------------------------------------------------------------------
cnLib_ENUM_BEGIN(ufInt8,ImageBlendingOperator)
{
	Noop,		// ignore any input image
	Copy,		// dest = src

	Add,		// dest = dest + src
	Mul,		// dest = dest * src
	Alpha,		// dest.rgb = dest.rgb*(1-src.a) + src.rgb*src.a
				// dest.a   = dest.a*(1-src.a) + src.a
				// if src has no alpha : dest = src
}cnLib_ENUM_END(ImageBlendingOperator);
//---------------------------------------------------------------------------
//class cnLib_INTERFACE iUIImage : public iUIGraph
//{
//public:
//	virtual eImageBlendingOperator cnLib_FUNC GetBlendingOperator(void)=0;
//};
//---------------------------------------------------------------------------
class cnLib_INTERFACE iUISimplePaintContext
{
public:
	virtual bool cnLib_FUNC Fill(cUIPoint DrawPosition,cUIPoint DrawSize,cUIColor Color)=0;
	virtual bool cnLib_FUNC Graph(cUIPoint DrawPosition,cUIPoint DrawSize,iUIGraph *Graph)=0;
	virtual bool cnLib_FUNC Image(cUIPoint DrawPosition,cUIPoint DrawSize,iUIBitmap *Bitmap,eImageBlendingOperator BlendingOperator=ImageBlendingOperator::Alpha)=0;
};
//---------------------------------------------------------------------------
cnLib_ENUM_BEGIN(uInt8,BitmapChannelColor)
{
	Unknown,

	Luminance,
	Alpha,
	LA,

	RGB,
	RGBA,

	BGR,
	BGRA,
}cnLib_ENUM_END(BitmapChannelColor);
//---------------------------------------------------------------------------
cnLib_ENUM_BEGIN(uInt8,BitmapChannelType)
{
	UNorm,
	SNorm,
	Float,
	UIntegral,
	SIntegral,
}cnLib_ENUM_END(BitmapChannelType);
//---------------------------------------------------------------------------
struct cBitmapPixelFormat
{
	eBitmapChannelColor ChannelColor;
	eBitmapChannelType ChannelType;
	uInt8 ChannelCount			:4;
	uInt8 ChannelSize			:4;
	bool AlphaPremultiplied		:1;
};
//---------------------------------------------------------------------------
class iBitmapDataSource;
//---------------------------------------------------------------------------
class cnLib_INTERFACE iBitmapDataSource : public iInterface
{
public:
	virtual cBitmapPixelFormat cnLib_FUNC GetPixelFormat(void)=0;
	virtual cUIPoint cnLib_FUNC GetImageSize(void)=0;
	virtual uIntn cnLib_FUNC GetDataPitch(void)=0;
	virtual uIntn cnLib_FUNC CopyPixelBuffer(uIntn Offset,void *Dest,uIntn DestSize)=0;
	virtual const void* cnLib_FUNC AcquirePixels(void)=0;
	virtual void cnLib_FUNC ReleasePixels(void)=0;
	virtual bool cnLib_FUNC IsTopDown(void)=0;
};
//---------------------------------------------------------------------------
class cnLib_INTERFACE iMipmapDataSource : public iReference
{
public:
	virtual uIntn cnLib_FUNC GetBitmapCount(void)=0;
	virtual iBitmapDataSource* cnLib_FUNC GetBitmapData(uIntn Index)=0;
	virtual bool Lookup(uIntn &Index,cUIPoint Size)=0;
};
//---------------------------------------------------------------------------
class cnLib_INTERFACE iUISimpleBitmapCanvas : public iReference
{
public:
	virtual iUISimplePaintContext* cnLib_FUNC StartContext(cUIColor Color)=0;
	virtual void cnLib_FUNC DiscardContext(void)=0;
	virtual iPtr<iUIBitmap> cnLib_FUNC FinishBitmap(bool CopyContent)=0;
	virtual iPtr<iBitmapDataSource> cnLib_FUNC FinishBitmapSource(bool CopyContent)=0;
};
//---------------------------------------------------------------------------
class cnLib_INTERFACE iUISimplePaintDevice : public iReference
{
public:
	virtual rPtr<iUISimpleBitmapCanvas> cnLib_FUNC CreateBitmapCanvas(cUIPoint Size)=0;
	virtual iPtr<iUITextGraph> cnLib_FUNC CreateTextGraph(const uChar16 *Text,uIntn Length,const cUITextStyle &Style,const Float32* TextDistance=0,Float32 TextMinHeight=0.)=0;

	virtual iPtr<iUIBitmap> cnLib_FUNC CreateBitmapCopyFromSource(iBitmapDataSource *Source)=0;
};
//---------------------------------------------------------------------------
class cnLib_INTERFACE iUISimplePainter : public iUIPainter
{
public:
	// PaintRectChanged
	//	called when rectangle of the content changed
	virtual void cnLib_FUNC PaintRectChanged(void)=0;
	// Paint
	//	paint content to context
	virtual void cnLib_FUNC Paint(iUISimplePaintContext *Context)=0;
};
//---------------------------------------------------------------------------
class cnLib_INTERFACE iUISimpleViewContent : public iUIViewContent
{
public:
	virtual rPtr<iUISimplePaintDevice> cnLib_FUNC QueryDevice(void)=0;
	virtual eUIState cnLib_FUNC GetPaintState(void)=0;
	virtual cUIPoint cnLib_FUNC GetPaintSize(void)=0;
	virtual void cnLib_FUNC QueryUpdate(void)=0;

};
//---------------------------------------------------------------------------
namespace cnSystem{
//---------------------------------------------------------------------------
iPtr<iUIFont> cnLib_FUNC QueryFont(const uChar16 *Name,uIntn NameLength,eUIFontStyle FontStyle=UIFontStyle::Normal,Float32 FontWeight=0);
rPtr<iUITextLineLayout> CreateTextLineLayout(const uChar16 *Text,uIntn Length,const cUITextStyle &Style);
iPtr<iBitmapDataSource> cnLib_FUNC CreateBitmapSourceFromFile(iFileName *FileName);

//---------------------------------------------------------------------------
rPtr<iUISimplePaintDevice> cnLib_FUNC QueryUISimplePaintDevice(iUIView *ForView);
rPtr<iUISimpleViewContent> cnLib_FUNC CreateUISimpleViewContent(iUISimplePainter *Painter);
//---------------------------------------------------------------------------
}   // namespace cnSystem
//---------------------------------------------------------------------------
}	// namespace cnLibrary
//---------------------------------------------------------------------------
#endif  /* __cplusplus */
/*-------------------------------------------------------------------------*/
#endif


