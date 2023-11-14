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
COMPtr<IWICImagingFactory> WICQueryImagingFactory(void);
//---------------------------------------------------------------------------
iPtr<iBitmapDataSource> CreateBitmapDataSourceFromResourceBitmap(HMODULE hModule,LPCWSTR lpName);
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
	cBitmapDataSourceFromIconResource();
	~cBitmapDataSourceFromIconResource();

	// iBitmapDataSource

	virtual cBitmapPixelFormat cnLib_FUNC GetPixelFormat(void)override;
	virtual cUIPoint cnLib_FUNC GetImageSize(void)override;
	virtual uIntn cnLib_FUNC GetDataPitch(void)override;
	virtual uIntn cnLib_FUNC CopyPixelBuffer(uIntn Offset,void *Dest,uIntn DestSize)override;
	virtual const void* cnLib_FUNC AcquirePixels(void)override;
	virtual void cnLib_FUNC ReleasePixels(void)override;
	virtual bool cnLib_FUNC IsTopDown(void)override;

	bool SetIconData(const void *ResourceData,uIntn ResourceDataSize);
	uIntn GetDataSize(void);

private:
	void ClearData(void);
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

	void SetupPixelBuffer(void);
};
//---------------------------------------------------------------------------
iPtr<iBitmapDataSource> CreateBitmapDataSourceFromResourceIconItem(HMODULE hModule,LPCWSTR lpName);
iPtr<iBitmapDataSource> CreateBitmapDataSourceFromResourceIcon(HMODULE hModule,LPCWSTR lpName,int LookupX,int LookupY,UINT LookupFlag=LR_DEFAULTCOLOR);
//---------------------------------------------------------------------------
class cWICBitmapDataSource : public iBitmapDataSource, public iCOMInterface
{
public:
	cWICBitmapDataSource(IWICBitmap *Bitmap);
	cWICBitmapDataSource(IWICBitmapSource *BitmapSource);
	~cWICBitmapDataSource();

	virtual void* cnLib_FUNC CastInterface(iTypeID IID)noexcept(true) override;

	// iBitmapDataSource
	
	virtual cBitmapPixelFormat cnLib_FUNC GetPixelFormat(void)override;
	virtual cUIPoint cnLib_FUNC GetImageSize(void)override;
	virtual uIntn cnLib_FUNC GetDataPitch(void)override;
	virtual uIntn cnLib_FUNC CopyPixelBuffer(uIntn Offset,void *Dest,uIntn DestSize)override;
	virtual const void* cnLib_FUNC AcquirePixels(void)override;
	virtual void cnLib_FUNC ReleasePixels(void)override;
	virtual bool cnLib_FUNC IsTopDown(void)override;

	// iCOMInterface

	virtual IUnknown* cnLib_FUNC GetCOMInterface(void)override;

	uIntn GetDataSize(void);
private:
	COMPtr<IWICBitmapSource> fBitmapSource;
	COMPtr<IWICBitmap> fBitmap;
	COMPtr<IWICBitmapLock> fDataLock;

	bool fFormatValid;
	cBitmapPixelFormat fPixelFormat;
	static cBitmapPixelFormat GUIDToFormat(const WICPixelFormatGUID &FormatGUID);
	bool SetupData(void);
};
//---------------------------------------------------------------------------
iPtr<iBitmapDataSource> CreateBitmapSourceFromWIC(IWICBitmapSource *BitmapSource);
iPtr<iBitmapDataSource> CreateBitmapSourceFromWICBitmap(IWICBitmap *Bitmap);
//---------------------------------------------------------------------------
class cWICImageFactory
{
public:
	cWICImageFactory();
	~cWICImageFactory();

	IWICImagingFactory* GetImagingFactory(void);

	COMPtr<IWICBitmapSource> OpenImageFile(const wchar_t *FileName);
	COMPtr<IWICBitmapSource> OpenImageStream(IStream *Stream);
	COMPtr<IWICBitmapSource> OpenImageResource(HMODULE hModule,LPCWSTR lpName,LPCWSTR lpType);
	COMPtr<IWICBitmapSource> ImageConvertFormat(IWICBitmapSource *pISource,REFWICPixelFormatGUID dstFormat,
		WICBitmapDitherType dither=WICBitmapDitherTypeNone,IWICPalette *pIPalette=nullptr,
		double alphaThresholdPercent=0.0,WICBitmapPaletteType paletteTranslate=WICBitmapPaletteTypeCustom);
	COMPtr<IWICBitmapSource> ImageScale(IWICBitmapSource *Source,UINT uiWidth,UINT uiHeight,WICBitmapInterpolationMode mode=WICBitmapInterpolationModeLinear);

private:
	COMPtr<IWICImagingFactory> fImagingFactory;

};
COMPtr<IWICBitmapSource> WICOpenImageFile(const wchar_t *FileName);
COMPtr<IWICBitmapSource> WICOpenImageStream(IStream *Stream);
COMPtr<IWICBitmapSource> WICOpenImageResource(HMODULE hModule,LPCWSTR lpName,LPCWSTR lpType);
COMPtr<IWICBitmapSource> WICImageConvertFormat(IWICBitmapSource *pISource,REFWICPixelFormatGUID dstFormat,
	WICBitmapDitherType dither=WICBitmapDitherTypeNone,IWICPalette *pIPalette=nullptr,
	double alphaThresholdPercent=0.0,WICBitmapPaletteType paletteTranslate=WICBitmapPaletteTypeCustom);
COMPtr<IWICBitmapSource> WICImageScale(IWICBitmapSource *Source,UINT uiWidth,UINT uiHeight,WICBitmapInterpolationMode mode=WICBitmapInterpolationModeLinear);
//---------------------------------------------------------------------------
}	// namespace cnWinRTL
//---------------------------------------------------------------------------
}	// namespace cnRTL
//---------------------------------------------------------------------------
}	// namespace cnLibrary
//---------------------------------------------------------------------------
#endif  /* __cplusplus */
/*-------------------------------------------------------------------------*/
