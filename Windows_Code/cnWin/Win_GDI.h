/*- Unit: GDI -------------------------------------------------------------*/
/*         Developer : Code009                                             */
/*         Create on : 2009-02-09                                          */
/*-------------------------------------------------------------------------*/
#pragma once
/*-------------------------------------------------------------------------*/
#include <cnWin\Win_Common.h>
#include <cnWin\Win_Thread.h>

#ifdef __cplusplus
//---------------------------------------------------------------------------
namespace cnLibrary{
//---------------------------------------------------------------------------
namespace cnWin{
#if 0

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
class cGDIFontName : public iInterface
{
public:
	cnLib_INTERFACE_DEFINE(cGDIFontName,iInterface)

	cGDIFontName(const wchar_t *Name);
	cGDIFontName(cnRTL::cString<wchar_t> &&Name);

	const cnRTL::cString<wchar_t>& FontNameString(void);
	const wchar_t* FontName(void);
	uIntn FontLength(void);
private:
	cnRTL::cString<wchar_t> fName;
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
		ufInt16n Size;
		sInt16 Weight;
		bool Italic;
		bool Underline;
		bool Strike;
		cnRTL::cString<wchar_t> FontName;

		bool operator <(const cFontIndex &Src)const;
		bool operator ==(const cFontIndex &Src)const;
	};

	DEVMODE fDM;
	rPtr<cGDIDC> fScreenIC;
	uIntn fDCNumber;

	iPtr<iUIThread> fUIThread;
	cnRTL::cSeqMap<cFontIndex,rPtr<cGDIFontHandle>> fFontMap;

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

	class cBitmap : public cnRTL::Windows::cGDIBitmap, public cnRTL::bcVirtualLifeCycle
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
#endif // 0
//---------------------------------------------------------------------------
}	// namespace cnWin
//---------------------------------------------------------------------------
}	// namespace cnLibrary
//---------------------------------------------------------------------------
#endif  /* __cplusplus */
/*-------------------------------------------------------------------------*/
