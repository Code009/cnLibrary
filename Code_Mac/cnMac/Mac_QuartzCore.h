/*--- Mac - Quartz  -------------------------------------------------------*/
/*         Developer : Code009                                             */
/*         Create on : 2017-08-28                                          */
/*-------------------------------------------------------------------------*/
#pragma once

#include <CoreGraphics/CoreGraphics.h>
#include <QuartzCore/QuartzCore.h>
#include <CoreText/CoreText.h>
//---------------------------------------------------------------------------
#include <cnMac/Mac_Header.h>
//---------------------------------------------------------------------------
#ifdef	__cplusplus
//---------------------------------------------------------------------------
#ifdef	__OBJC__
@interface cnLib_MacSimpleViewContent : NSObject
@end
#endif	// __OBJC__
//---------------------------------------------------------------------------
namespace cnLibrary{
namespace cnMac{
//---------------------------------------------------------------------------
class cCFCGImage : public iCFCGImage
{
public:
	cCFCGImage(CGImageRef Image);
	~cCFCGImage();

	virtual const void* cnLib_FUNC CastInterface(iTypeID InterfaceID)const override;

	// iCGContextDrawable

	virtual bool cnLib_FUNC DrawInContext(CGContextRef Context,const CGRect &Rect)override;

	// iCFCGImage

	virtual CGImageRef cnLib_FUNC GetCGImage(void)override;

protected:
	CGImageRef fImage=nullptr;
};
//---------------------------------------------------------------------------
class cBitmapSourceFromCGImage : public iBitmapDataSource, public cCFCGImage
{
public:
	cBitmapSourceFromCGImage(CGImageRef Image);
	~cBitmapSourceFromCGImage();

	using cCFCGImage::CastInterface;

	// iBitmapDataSource

	virtual cBitmapPixelFormat cnLib_FUNC GetPixelFormat(void)override;
	virtual cUIPoint cnLib_FUNC GetImageSize(void)override;
	virtual uIntn cnLib_FUNC GetDataPitch(void)override;
	virtual uIntn cnLib_FUNC CopyPixelBuffer(uIntn Offset,void *Dest,uIntn DestSize)override;
	virtual const void* cnLib_FUNC AcquirePixels(void)override;
	virtual void cnLib_FUNC ReleasePixels(void)override;
	virtual bool cnLib_FUNC IsTopDown(void)override;
protected:

	cnRTL::cMemoryBlock fCacheData;

	void CacheData(void);
};
//---------------------------------------------------------------------------
#if	0
class cBitmapSourceFromCGImageProvider : public iBitmapDataSource
{
public:
	cBitmapSourceFromCGImageProvider(CGDataProviderRef DataProvider);
	~cBitmapSourceFromCGImageProvider();

	virtual cBitmapPixelFormat cnLib_FUNC GetPixelFormat(void)override;
	virtual cUIPoint cnLib_FUNC GetImageSize(void)override;
	virtual uIntn cnLib_FUNC GetDataPitch(void)override;
	virtual uIntn cnLib_FUNC CopyPixelBuffer(uIntn Offset,void *Dest,uIntn DestSize)override;
	virtual const void* cnLib_FUNC AcquirePixels(void)override;
	virtual void cnLib_FUNC ReleasePixels(void)override;
	virtual bool cnLib_FUNC IsTopDown(void)override;
};
#endif
//---------------------------------------------------------------------------
CGImageRef LoadImageFromFile(iFileName *FileName);
//---------------------------------------------------------------------------
void ColorComponentFromA8R8G8B8(CGFloat Components[4],uInt32 Color);
//---------------------------------------------------------------------------
CGDataProviderRef CreateCGDataAtHeap(uIntn Size,void* &DataPointer,iMemoryHeap *Heap=cnSystem::DefaultMemoryHeap);
//---------------------------------------------------------------------------
struct CGDataFromBitmapDataSource
{
	static CGDataProviderDirectCallbacks GetCallbacks(void);

	static const void* GetBytePointer(void *info);
	static void ReleaseBytePointer(void *info,const void *pointer);
	static size_t GetBytesAtPosition(void *info, void *buffer,off_t pos,size_t cnt);
	static void ReleaseInfo(void *info);
};
CGDataProviderRef CreateCGDataFromBitmapDataSource(iBitmapDataSource *DataSource);
//---------------------------------------------------------------------------
class cCGSimplePaintContext : public iUISimplePaintContext
{
public:
	cCGSimplePaintContext();
	~cCGSimplePaintContext();

	CGContextRef Context=nullptr;

	static CGBlendMode BlendModeFromBlendingOperator(eImageBlendingOperator BlendingOperator);
	void Clear(CGSize ImageSize,uInt32 Color);

	// iUISimplePaintContext

	virtual bool cnLib_FUNC Fill(cUIPoint DrawPosition,cUIPoint DrawSize,uInt32 Color)override;
	virtual bool cnLib_FUNC Graph(cUIPoint DrawPosition,cUIPoint DrawSize,iUIGraph *Graph)override;
	virtual bool cnLib_FUNC Image(cUIPoint DrawPosition,cUIPoint DrawSize,iUIBitmap *Bitmap,eImageBlendingOperator BlendingOperator)override;
protected:
};
//---------------------------------------------------------------------------
class cCTTextGraph : public iUITextGraph, public iCGContextDrawable
{
public:
	cCTTextGraph();
	~cCTTextGraph();

	virtual const void* cnLib_FUNC CastInterface(iTypeID InterfaceID)const override;

	virtual cUIPoint cnLib_FUNC GetSize(void)override;

	uIntn GetTextLength(void);
	uIntn GetTextPos(uIntn Length,uInt32 *TextPos);

	void Clear(void);
	void SetText(const uChar16 *Text,uIntn Length,const cUITextStyle &Style,const cUITextLineLayout *Layout);
	void Draw(CGContextRef Context,float x,float y)const;

// iCGContextDrawable

	virtual bool cnLib_FUNC DrawInContext(CGContextRef Context,const CGRect &Rect)override;

private:
	CTLineRef fLine;
	CGFloat fLineWidth;
	CGFloat fLineHeight;
	CGFloat fLineBase;
};
//---------------------------------------------------------------------------
#pragma mark Bitmap
//---------------------------------------------------------------------------
class cCGImageBitmap : public iUIBitmap, public cCFCGImage
{
public:
	cCGImageBitmap(CGImageRef Image);
	~cCGImageBitmap();

	using cCFCGImage::CastInterface;

	// iUIGraph

	virtual cUIPoint cnLib_FUNC GetSize(void)override;

	// iUIBitmap
};
//---------------------------------------------------------------------------
class cCGLayerBitmap : public iUIBitmap, public iCGContextDrawable
{
public:
	cCGLayerBitmap(CGLayerRef Layer);
	~cCGLayerBitmap();

	virtual const void* cnLib_FUNC CastInterface(iTypeID InterfaceID)const override;

	// iUIGraph

	virtual cUIPoint cnLib_FUNC GetSize(void)override;

	// iCGContextDrawable

	virtual bool cnLib_FUNC DrawInContext(CGContextRef Context,const CGRect &Rect)override;

protected:
	CGLayerRef fLayer;
};
//---------------------------------------------------------------------------
class cCGSimpleBitmapCanvas : public iUISimpleBitmapCanvas
{
public:
	cCGSimpleBitmapCanvas(CGLayerRef Layer);
	~cCGSimpleBitmapCanvas();

	// iUISimpleBitmapCanvas

	virtual iUISimplePaintContext* cnLib_FUNC GetContext(void)override;
	virtual void cnLib_FUNC Clear(uInt32 Color=0)override;
	virtual iPtr<iUIBitmap> cnLib_FUNC FinishBitmap(bool CopyContent)override;
	virtual iPtr<iBitmapDataSource> cnLib_FUNC FinishBitmapSource(bool CopyContent)override;

protected:
	CGLayerRef fDrawLayer;
	cCGSimplePaintContext fPaintContext;
	CGSize fImageSize;
	//uint16 fPaintCount=0;
	void Discard(void);

};
//---------------------------------------------------------------------------
#if	0
class cCGSimpleBitmapCanvas : public iUISimpleBitmapCanvas
{
public:
	cCGSimpleBitmapCanvas(cUIPoint ImageSize);
	~cCGSimpleBitmapCanvas();

	// iUISimpleBitmapCanvas

	virtual iUISimplePaintContext* cnLib_FUNC GetContext(void)override;
	virtual void cnLib_FUNC Clear(uInt32 Color=0)override;
	virtual iPtr<iUIBitmap> cnLib_FUNC FinishBitmap(void)override;
	virtual iPtr<iBitmapDataSource> cnLib_FUNC FinishBitmapSource(void)override;

protected:

	cCGSimplePaintContext fPaintContext;
	cUIPoint fImageSize;
	//uint16 fPaintCount=0;
	void Discard(void);

};
#endif
//---------------------------------------------------------------------------
class cCGSimplePaintDevice : public iUISimplePaintDevice
{
public:
	virtual rPtr<iUISimpleBitmapCanvas> cnLib_FUNC CreateBitmapCanvas(cUIPoint Size)override;
	virtual iPtr<iUITextGraph> cnLib_FUNC CreateTextGraph(const uChar16 *Text,uIntn Length,const cUITextStyle &Style,const cUITextLineLayout *Layout)override;

	virtual iPtr<iUIBitmap> cnLib_FUNC CreateBitmapCopyFromSource(iBitmapDataSource *Source)override;
};
extern cnRTL::cReferenceStaticImplementT<cCGSimplePaintDevice> gCGSimplePaintDevice;
//---------------------------------------------------------------------------
#ifdef	__OBJC__
//---------------------------------------------------------------------------
class cnLib_INTERFACE iUIViewCALayer : public iOCObject
{
public:
	virtual void SetLayerFrame(CGRect Frame)=0;
	virtual void SetLayerScale(CGFloat Scale)=0;
	virtual void OnViewStateChanged(void)=0;
};
//---------------------------------------------------------------------------
class cnLib_INTERFACE iUIViewCALayerHost : public iInterface
{
public:
	cnLib_INTERFACE_DEFINE(iUIViewCALayerHost,iInterface);

	virtual bool InsertViewCALayer(iUIViewCALayer *ViewLayer)=0;
	virtual bool RemoveViewCALayer(iUIViewCALayer *ViewLayer)=0;
};
//---------------------------------------------------------------------------
#pragma mark - Simple ViewContent
//---------------------------------------------------------------------------
class cMacSimpleViewContent : public iUISimpleViewContent, public iUIViewCALayer
{
public:
	cMacSimpleViewContent(cnLib_MacSimpleViewContent *OCHost);
	~cMacSimpleViewContent();

	cMacSimpleViewContent(const cMacSimpleViewContent&)=delete;

	virtual const void* cnLib_FUNC CastInterface(iTypeID IID)const override;

	void OnLayerDisplay(void);
	CALayer *GetLayer(void);

// iUIViewContent

	virtual iUIView* cnLib_FUNC GetView(void)override;
	virtual bool cnLib_FUNC SetView(iUIView *View)override;
	virtual bool cnLib_FUNC GetVisible(void)override;
	virtual bool cnLib_FUNC SetVisible(bool Visible)override;
	virtual sfInt16 cnLib_FUNC GetZIndex(void)override;
	virtual bool cnLib_FUNC SetZIndex(sfInt16 ZIndex)override;

// iUISimpleViewContent

	virtual rPtr<iUISimplePaintDevice> cnLib_FUNC QueryDevice(void)override;
	virtual void cnLib_FUNC SetPainter(iUISimplePainter *Painter)override;
	virtual eUIState cnLib_FUNC GetPaintState(void)override;
	virtual cUIPoint cnLib_FUNC GetPaintSize(void)override;
	virtual void cnLib_FUNC QueryUpdate(void)override;


// iUIViewCALayer

	virtual id cnLib_FUNC GetOCObject(void)override;
	virtual void SetLayerFrame(CGRect Frame)override;
	virtual void SetLayerScale(CGFloat Scale)override;
	virtual void OnViewStateChanged(void)override;

protected:
	CALayer *fLayer;
	cnLib_MacSimpleViewContent *fOCHost;
	iPtr<iUIView> fHostView=nullptr;
	iUISimplePainter *fPainter=nullptr;

	UIState fPainterState;
	void UpdatePainterState(void);
	struct cPainterNotifier{
		cMacSimpleViewContent *This;
		void UIStarted(void);
		void UIShow(void);
		void UIResume(void);
		void UIPaused(void);
		void UIHide(void);
		void UIStopped(void);
	};
	void NotifyPainterState(UIState PrevState,UIState NextState);

	bool fNeedNotifyRectChange=false;
	bool fEmptySize;

	virtual void cnLib_FUNC IncReference(void)override;
	virtual void cnLib_FUNC DecReference(void)override;

	void OnLayerLayout(void);

	void RefreshLayer(void);
};
//---------------------------------------------------------------------------
cMacSimpleViewContent* cnLib_MacSimpleViewContent_Interface(cnLib_MacSimpleViewContent *Object);
//---------------------------------------------------------------------------
#endif	// __OBJC__
//---------------------------------------------------------------------------
}	// namespace cnMac
}	// namespace cnLibrary
//---------------------------------------------------------------------------
#endif	// __cplusplus
/*-------------------------------------------------------------------------*/
#if	0
class cCGImageSimplePaintBitmap : public iUISimpleBitmapCanvas, public iCFCGImage
{
protected:
	~cCGImageSimplePaintBitmap();
public:
	cCGImageSimplePaintBitmap(cUIPoint ImageSize);

	virtual const void* cnLib_FUNC CastInterface(iTypeID InterfaceID)const override;

	// iCGContextDrawable

	virtual bool cnLib_FUNC DrawInContext(CGContextRef Context,const CGRect &Rect)override;

	// iCFCGImage

	virtual CGImageRef cnLib_FUNC GetCGImage(void)override;

	// iUIBitmap

	virtual iUISimplePaintContext* cnLib_FUNC GetContext(void)=0;
	virtual void cnLib_FUNC Clear(uInt32 Color=0)=0;
	virtual iRet<iUIBitmap> cnLib_FUNC rpFinishBitmap(void)=0;

	virtual cUIPoint cnLib_FUNC GetSize(void)override;
	virtual void cnLib_FUNC GetInfo(cUIBitmapInfo &BitmapInfo)override;
	virtual uintn cnLib_FUNC GetDataSize(void)override;
	virtual bool cnLib_FUNC CopyPixels(void *Dest,uintn DestSize)override;
	virtual const void* cnLib_FUNC AcquirePixels(void)override;
	virtual void cnLib_FUNC ReleasePixels(void)override;

	// iUISimplePaintBitmap

	virtual iUISimplePaintContext* cnLib_FUNC Paint(void)override;
	virtual void cnLib_FUNC CommitPaint(void)override;


protected:

	cCGSimplePaintContext fPaintContext;
	CGImageRef fCachedImage=nullptr;
	cUIPoint fImageSize;
	uint16 fPaintCount=0;
	void Discard(void);

};
class cCGImageBitmapBuffer : public iUIBitmapBuffer, public iCFCGImage
{
protected:
	~cCGImageBitmapBuffer();
public:
	cCGImageBitmapBuffer();

	virtual const void* cnLib_FUNC CastInterface(iTypeID InterfaceID)const override;

	// iCGContextDrawable

	virtual bool cnLib_FUNC DrawInContext(CGContextRef Context,const CGRect &Rect)override;

	// iCFCGImage

	virtual CGImageRef cnLib_FUNC GetCGImage(void)override;

	// iUIGraph

	virtual cUIPoint cnLib_FUNC GetSize(void)override;

	// iUIBitmap

	virtual void cnLib_FUNC GetInfo(cUIBitmapInfo &BitmapInfo)override;
	virtual uintn cnLib_FUNC GetDataSize(void)override;
	virtual bool cnLib_FUNC CopyPixels(void *Dest,uintn DestSize)override;
	virtual const void* cnLib_FUNC AcquirePixels(void)override;
	virtual void cnLib_FUNC ReleasePixels(void)override;

	// iUIBitmapBuffer

	virtual void* cnLib_FUNC AcquireEditPixels(void)override;
	virtual void cnLib_FUNC ReleaseEditPixels(void)override;
	virtual bool cnLib_FUNC SetParameter(const cUIBitmapInfo &BitmapInfo,sint32 Width,sint32 Height)override;


protected:
	sint32 fBitmapWidth=0;
	sint32 fBitmapHeight=0;
	CGImageRef fBitmapImage=nullptr;
	CGDataProviderRef fBitmapProvider=nullptr;
	void *fBitmapData=nullptr;
	uintn fBitmapDataSize=0;
	uint16 fPixelLockCount=0;

	void CreateBitmap(void);
	void ClearBitmap(void);

	bool Draw(CGContextRef Context,const CGRect &Rect);
};
#endif
