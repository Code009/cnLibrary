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
COLORREF COLORREFFromUIColor(const cUIColor &UIColor);
//---------------------------------------------------------------------------
struct cGDIBitmapBuffer
{
	cGDIBitmapBuffer();
	~cGDIBitmapBuffer();
	
	void Clear(void);
	void Setup(HDC DC,int Width,int Height);

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
	cGDIBitmapSection();
	~cGDIBitmapSection();
	
	void Clear(void);
	void Setup(HDC DC,int NewWidth,int NewHeight);

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
	cGDIBitmap(HBITMAP Bitmap=nullptr,bool HasAlpha=false);
	~cGDIBitmap();

	virtual void* cnLib_FUNC CastInterface(iTypeID InterfaceID)noexcept(true) override;
	
	// iUIBitmap

	virtual cUIPoint cnLib_FUNC GetSize(void)override;

	// iGDIBitmap

	virtual HBITMAP cnLib_FUNC GetBitmap(void)override;
	virtual bool cnLib_FUNC HasAlpha(void)override;
private:
	HBITMAP fBitmap;
	bool fHasAlpha;
};
//---------------------------------------------------------------------------
bool GDIDrawBitmap(HDC DC,int DrawX,int DrawY,int DrawWidth,int DrawHeight,HDC MemDC,HBITMAP Bitmap,int BmpWidth,int BmpHeight,bool HasAlpha);
//---------------------------------------------------------------------------
HBITMAP DCCreateBitmapFromSource(HDC DC,IWICBitmapSource *BitmapSource,bool &HasAlpha);
HBITMAP DCCreateBitmapFromSource(HDC DC,iBitmapDataSource *Source,bool &HasAlpha);
//---------------------------------------------------------------------------
class cGDIDC : public iGDIDC
{
public:        
	cGDIDC(HDC DC);
	~cGDIDC();

	HDC GetHandle(void);
	virtual HDC cnLib_FUNC GetDC(void)override;

protected:
	HDC fDC;
};
//---------------------------------------------------------------------------
class cGDIFontHandle : public iGDIFontHandle
{
public:
	cGDIFontHandle(HFONT FontHandle);
	~cGDIFontHandle();

	virtual HFONT cnLib_FUNC GetFontHandle(void)override;

protected:
	HFONT fFontHandle;
};
//---------------------------------------------------------------------------
class cDCPaintDevice : public iDCPaintDevice
{
public:
	cDCPaintDevice();
	~cDCPaintDevice();

	HDC GetScreenICHandle(void);
	cGDIDC* GetScreenIC(void);
	rPtr<cGDIDC> QueryMemDC(void);

	void UpdateDisplaySetting(void);

	virtual iGDIDC* cnLib_FUNC GetScreenInfomationDC(void)override;
	virtual rPtr<iGDIDC> cnLib_FUNC QueryMemoryDC(void)override;
	virtual rPtr<iGDIFontHandle> cnLib_FUNC QueryFont(const cUITextStyle &TextStyle)override;
	virtual iPtr<iGDIBitmap> cnLib_FUNC CreateBitmapCopyFromSource(iBitmapDataSource *Source)override;

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

		bool operator <(const cFontIndex &Src)const;
		bool operator ==(const cFontIndex &Src)const;
	};

	DEVMODE fDM;
	rPtr<cGDIDC> fScreenIC;
	uIntn fDCNumber;

	iPtr<iUIThread> fUIThread;
	cnRTL::cSeqMap<cFontIndex,rPtr<cGDIFontHandle> > fFontMap;

	bool CheckThread(void);


	class cMemDC : public cGDIDC
	{
	public:
		cMemDC();


		void SetupDC(HDC IC,uIntn DCNumber);

	protected:
		uIntn fDCNumber;
	};
	cnRTL::rSharedObjectRecycler<cMemDC> fMemDCRecycler;

	class cBitmap : public cGDIBitmap, public cnRTL::bcVirtualLifeCycle
	{
	public:
		cBitmap(cDCPaintDevice *Owner,iBitmapDataSource *Source,HBITMAP Bitmap,bool HasAlpha);
		iBitmapDataSource *GetSource(void)const;

	protected:
		void VirtualStopped(void);

		cDCPaintDevice *fOwner;
		iBitmapDataSource *fSource;

	private:
		class cThreadDeleteProcedure : public iProcedure
		{
			virtual void cnLib_FUNC Execute(void)override;
		}fThreadDeleteProcedure;
		void ThreadDelete(void);
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
