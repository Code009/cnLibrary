/*- cnRTL - Windows - WIC -------------------------------------------------*/
/*         Developer : Code009                                             */
/*         Create on : 2009-02-10                                          */
/*-------------------------------------------------------------------------*/
#pragma once
/*-------------------------------------------------------------------------*/
#include <cnRTL\WinSDKHeader.h>

#include <cnRTL\WinCommon.h>
#include <cnRTL\WinCOM.h>

#ifdef __cplusplus
//---------------------------------------------------------------------------
namespace cnLibrary{
//---------------------------------------------------------------------------
namespace cnRTL{
//---------------------------------------------------------------------------
namespace cnWinRTL{
//---------------------------------------------------------------------------
COMPtr<IWICImagingFactory> WICQueryImagingFactory(void)noexcept(true);
//---------------------------------------------------------------------------
iPtr<iBitmapDataSource> CreateBitmapDataSourceFromResourceBitmap(HMODULE hModule,LPCWSTR lpName)noexcept(true);
//---------------------------------------------------------------------------
#if 0

class cBitmapConvert
{
public:
	cBitmapConvert();
	~cBitmapConvert();

	bool SetIconData(const void *ResourceData,uIntn ResourceDataSize);

private:
	void ClearData(void);
	eBitmapFormat fFormat;
	cUIPoint fSize;
	const BITMAPINFO *fResourceData;
	const void *fResPixelData;
	const void *fResMaskData;
	uIntn fResImageLineSize;
	uIntn fResMaskLineSize;

	cMemoryBuffer fConvertedPixelBuffer;
	bool fTopDown;

	uInt16 fPixelLockCount=0;

	void SetupPixelBuffer_ConvertBMP4(void);
	void SetupPixelBuffer_ConvertBMP8(void);
	void SetupPixelBuffer_ConvertBMP24(void);
	void SetupPixelBuffer(void);
};
#endif // 0
//---------------------------------------------------------------------------
class cBitmapDataSourceFromIconResource : public iBitmapDataSource
{
public:
	cBitmapDataSourceFromIconResource()noexcept(true);
	~cBitmapDataSourceFromIconResource()noexcept(true);

	// iBitmapDataSource

	virtual cBitmapPixelFormat cnLib_FUNC GetPixelFormat(void)noexcept(true)override;
	virtual cUIPoint cnLib_FUNC GetImageSize(void)noexcept(true)override;
	virtual uIntn cnLib_FUNC GetDataPitch(void)noexcept(true)override;
	virtual uIntn cnLib_FUNC CopyPixelBuffer(uIntn Offset,void *Dest,uIntn DestSize)noexcept(true)override;
	virtual const void* cnLib_FUNC AcquirePixels(void)noexcept(true)override;
	virtual void cnLib_FUNC ReleasePixels(void)noexcept(true)override;
	virtual bool cnLib_FUNC IsTopDown(void)noexcept(true)override;

	bool SetIconData(const void *ResourceData,uIntn ResourceDataSize)noexcept(true);
	uIntn GetDataSize(void)noexcept(true);

private:
	void ClearData(void)noexcept(true);
	cBitmapPixelFormat fPixelFormat;
	cUIPoint fSize;
	const BITMAPINFO *fResourceData;
	const void *fResMaskData;
	uIntn fPaletteDataSize;
	uIntn fResMaskLineSize;

	cMemoryBuffer fConvertedPixelBuffer;
	ufInt32 fPixelBufferPitch;
	bool fTopDown;

	uInt16 fPixelLockCount=0;

	void SetupPixelBuffer(void)noexcept(true);
};
//---------------------------------------------------------------------------
iPtr<iBitmapDataSource> CreateBitmapDataSourceFromResourceIconItem(HMODULE hModule,LPCWSTR lpName)noexcept(true);
iPtr<iBitmapDataSource> CreateBitmapDataSourceFromResourceIcon(HMODULE hModule,LPCWSTR lpName,int LookupX,int LookupY,UINT LookupFlag=LR_DEFAULTCOLOR)noexcept(true);
//---------------------------------------------------------------------------
class cWICBitmapDataSource : public iBitmapDataSource, public iCOMInterface
{
public:
	cWICBitmapDataSource(IWICBitmap *Bitmap)noexcept(true);
	cWICBitmapDataSource(IWICBitmapSource *BitmapSource)noexcept(true);
	~cWICBitmapDataSource()noexcept(true);

	virtual void* cnLib_FUNC CastInterface(iTypeID IID)noexcept(true) override;

	// iBitmapDataSource
	
	virtual cBitmapPixelFormat cnLib_FUNC GetPixelFormat(void)noexcept(true)override;
	virtual cUIPoint cnLib_FUNC GetImageSize(void)noexcept(true)override;
	virtual uIntn cnLib_FUNC GetDataPitch(void)noexcept(true)override;
	virtual uIntn cnLib_FUNC CopyPixelBuffer(uIntn Offset,void *Dest,uIntn DestSize)noexcept(true)override;
	virtual const void* cnLib_FUNC AcquirePixels(void)noexcept(true)override;
	virtual void cnLib_FUNC ReleasePixels(void)noexcept(true)override;
	virtual bool cnLib_FUNC IsTopDown(void)noexcept(true)override;

	// iCOMInterface

	virtual IUnknown* cnLib_FUNC GetCOMInterface(void)noexcept(true)override;

	uIntn GetDataSize(void)noexcept(true);
private:
	COMPtr<IWICBitmapSource> fBitmapSource;
	COMPtr<IWICBitmap> fBitmap;
	COMPtr<IWICBitmapLock> fDataLock;

	bool fFormatValid;
	cBitmapPixelFormat fPixelFormat;
	static cBitmapPixelFormat GUIDToFormat(const WICPixelFormatGUID &FormatGUID)noexcept(true);
	bool SetupData(void)noexcept(true);
};
//---------------------------------------------------------------------------
iPtr<iBitmapDataSource> CreateBitmapSourceFromWIC(IWICBitmapSource *BitmapSource)noexcept(true);
iPtr<iBitmapDataSource> CreateBitmapSourceFromWICBitmap(IWICBitmap *Bitmap)noexcept(true);
//---------------------------------------------------------------------------
class cWICImageFactory
{
public:
	cWICImageFactory()noexcept(true);
	~cWICImageFactory()noexcept(true);

	IWICImagingFactory* GetImagingFactory(void)noexcept(true);

	COMPtr<IWICBitmapSource> OpenImageFile(const wchar_t *FileName)noexcept(true);
	COMPtr<IWICBitmapSource> OpenImageStream(IStream *Stream)noexcept(true);
	COMPtr<IWICBitmapSource> OpenImageResource(HMODULE hModule,LPCWSTR lpName,LPCWSTR lpType)noexcept(true);
	COMPtr<IWICBitmapSource> ImageConvertFormat(IWICBitmapSource *pISource,REFWICPixelFormatGUID dstFormat,
		WICBitmapDitherType dither=WICBitmapDitherTypeNone,IWICPalette *pIPalette=nullptr,
		double alphaThresholdPercent=0.0,WICBitmapPaletteType paletteTranslate=WICBitmapPaletteTypeCustom)noexcept(true);
	COMPtr<IWICBitmapSource> ImageScale(IWICBitmapSource *Source,UINT uiWidth,UINT uiHeight,WICBitmapInterpolationMode mode=WICBitmapInterpolationModeLinear)noexcept(true);

private:
	COMPtr<IWICImagingFactory> fImagingFactory;

};
COMPtr<IWICBitmapSource> WICOpenImageFile(const wchar_t *FileName)noexcept(true);
COMPtr<IWICBitmapSource> WICOpenImageStream(IStream *Stream)noexcept(true);
COMPtr<IWICBitmapSource> WICOpenImageResource(HMODULE hModule,LPCWSTR lpName,LPCWSTR lpType)noexcept(true);
COMPtr<IWICBitmapSource> WICImageConvertFormat(IWICBitmapSource *pISource,REFWICPixelFormatGUID dstFormat,
	WICBitmapDitherType dither=WICBitmapDitherTypeNone,IWICPalette *pIPalette=nullptr,
	double alphaThresholdPercent=0.0,WICBitmapPaletteType paletteTranslate=WICBitmapPaletteTypeCustom)noexcept(true);
COMPtr<IWICBitmapSource> WICImageScale(IWICBitmapSource *Source,UINT uiWidth,UINT uiHeight,WICBitmapInterpolationMode mode=WICBitmapInterpolationModeLinear)noexcept(true);
//---------------------------------------------------------------------------
}	// namespace cnWinRTL
//---------------------------------------------------------------------------
}	// namespace cnRTL
//---------------------------------------------------------------------------
}	// namespace cnLibrary
//---------------------------------------------------------------------------
#endif  /* __cplusplus */
/*-------------------------------------------------------------------------*/
