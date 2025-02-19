/*- cnRTL - Windows - GDI -------------------------------------------------*/
/*         Developer : Code009                                             */
/*         Create on : 2020-05-03                                          */
/*-------------------------------------------------------------------------*/
#pragma once
/*-------------------------------------------------------------------------*/
#include <cnRTL\WinCommon.h>
#include <cnRTL\WinCOMWIC.h>

#ifdef __cplusplus

//---------------------------------------------------------------------------
namespace cnLibrary{
//---------------------------------------------------------------------------
namespace cnRTL{
//---------------------------------------------------------------------------
namespace cnWinRTL{
//---------------------------------------------------------------------------
COLORREF COLORREFFromUIColor(const cUIColor &UIColor)noexcept(true);
//---------------------------------------------------------------------------
struct cGDIBitmapBuffer
{
	cGDIBitmapBuffer()noexcept(true);
	~cGDIBitmapBuffer()noexcept(true);
	
	void Clear(void)noexcept(true);
	void Setup(HDC DC,int Width,int Height)noexcept(true);

	HBITMAP Bitmap;
	int Width;
	int Height;
	uIntn Stride;
	uIntn BitmapSize;
	void *BitmapData;
};
//---------------------------------------------------------------------------
struct cGDIBitmapSection
{
	cGDIBitmapSection()noexcept(true);
	~cGDIBitmapSection()noexcept(true);
	
	void Clear(void)noexcept(true);
	void Setup(HDC DC,int NewWidth,int NewHeight)noexcept(true);

	HANDLE Section;
	HBITMAP Bitmap;
	int Width;
	int Height;
	uIntn Stride;
	uIntn BitmapSize;
	void *BitmapData;
};
//---------------------------------------------------------------------------
class cGDIBitmap : public iUIBitmap, public iGDIBitmap
{
public:
	cGDIBitmap(HBITMAP Bitmap=nullptr,bool HasAlpha=false)noexcept(true);
	~cGDIBitmap()noexcept(true);

	virtual void* cnLib_FUNC CastInterface(iTypeID InterfaceID)noexcept(true) override;
	
	// iUIBitmap

	virtual cUIPoint cnLib_FUNC GetSize(void)noexcept(true)override;

	// iGDIBitmap

	virtual HBITMAP cnLib_FUNC GetBitmap(void)noexcept(true)override;
	virtual bool cnLib_FUNC HasAlpha(void)noexcept(true)override;
private:
	HBITMAP fBitmap;
	bool fHasAlpha;
};
//---------------------------------------------------------------------------
bool GDIDrawBitmap(HDC DC,int DrawX,int DrawY,int DrawWidth,int DrawHeight,HDC MemDC,HBITMAP Bitmap,int BmpWidth,int BmpHeight,bool HasAlpha)noexcept(true);
//---------------------------------------------------------------------------
HBITMAP DCCreateBitmapFromSource(HDC DC,IWICBitmapSource *BitmapSource,bool &HasAlpha)noexcept(true);
HBITMAP DCCreateBitmapFromSource(HDC DC,iBitmapDataSource *Source,bool &HasAlpha)noexcept(true);
//---------------------------------------------------------------------------
class cGDIDC : public iGDIDC
{
public:        
	cGDIDC(HDC DC)noexcept(true);
	~cGDIDC()noexcept(true);

	HDC GetHandle(void)noexcept(true);
	virtual HDC cnLib_FUNC GetDC(void)noexcept(true)override;

protected:
	HDC fDC;
};
//---------------------------------------------------------------------------
class cGDIFontHandle : public iGDIFontHandle
{
public:
	cGDIFontHandle(HFONT FontHandle)noexcept(true);
	~cGDIFontHandle()noexcept(true);

	virtual HFONT cnLib_FUNC GetFontHandle(void)noexcept(true)override;

protected:
	HFONT fFontHandle;
};
//---------------------------------------------------------------------------
class cDCPaintDevice : public iDCPaintDevice
{
public:
	cDCPaintDevice()noexcept(true);
	~cDCPaintDevice()noexcept(true);

	HDC GetScreenICHandle(void)noexcept(true);
	cGDIDC* GetScreenIC(void)noexcept(true);
	rPtr<cGDIDC> QueryMemDC(void)noexcept(true);

	void UpdateDisplaySetting(void)noexcept(true);

	virtual iGDIDC* cnLib_FUNC GetScreenInfomationDC(void)noexcept(true)override;
	virtual rPtr<iGDIDC> cnLib_FUNC QueryMemoryDC(void)noexcept(true)override;
	virtual rPtr<iGDIFontHandle> cnLib_FUNC QueryFont(const cUITextStyle &TextStyle)noexcept(true)override;
	virtual iPtr<iGDIBitmap> cnLib_FUNC CreateBitmapCopyFromSource(iBitmapDataSource *Source)noexcept(true)override;

private:
	// font
	struct cFontIndex{
		union{
			uInt32 InfoValue;
			struct{
				uInt16 Size;
				uInt16 Weight	:10;
				uInt16 Italic	:1;
				uInt16 Underline:1;
				uInt16 Strike	:1;
			}Info;
		};
		cnRTL::cString<wchar_t> FontName;

		bool operator <(const cFontIndex &Src)const noexcept(true);
		bool operator ==(const cFontIndex &Src)const noexcept(true);
	};

	DEVMODE fDM;
	rPtr<cGDIDC> fScreenIC;
	uIntn fDCNumber;

	iPtr<iUIThread> fUIThread;
	cnRTL::cSeqMap<cFontIndex,rPtr<cGDIFontHandle> > fFontMap;

	bool CheckThread(void)noexcept(true);


	class cMemDC : public cGDIDC
	{
	public:
		cMemDC()noexcept(true);


		void SetupDC(HDC IC,uIntn DCNumber)noexcept(true);

	protected:
		uIntn fDCNumber;
	};
	cnRTL::rSharedObjectRecycler<cMemDC> fMemDCRecycler;

	class cBitmap : public cGDIBitmap, public cnRTL::bcVirtualLifeCycle
	{
	public:
		cBitmap(cDCPaintDevice *Owner,iBitmapDataSource *Source,HBITMAP Bitmap,bool HasAlpha)noexcept(true);
		iBitmapDataSource *GetSource(void)const noexcept(true);

	protected:
		void VirtualStopped(void)noexcept(true);

		cDCPaintDevice *fOwner;
		iBitmapDataSource *fSource;

	private:
		class cThreadDeleteProcedure : public iProcedure
		{
			virtual void cnLib_FUNC Execute(void)noexcept(true)override;
		}fThreadDeleteProcedure;
		void ThreadDelete(void)noexcept(true);
	};

	cnRTL::cSeqMap<iBitmapDataSource*,cBitmap*> fBitmapCache;
};
//---------------------------------------------------------------------------
}	// namespace cnWinRTL
//---------------------------------------------------------------------------
}	// namespace cnRTL
//---------------------------------------------------------------------------
}	// namespace cnLibrary
//---------------------------------------------------------------------------
#endif  /* __cplusplus */
/*-------------------------------------------------------------------------*/
