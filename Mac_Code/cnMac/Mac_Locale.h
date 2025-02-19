/*--- Mac - Locale --------------------------------------------------------*/
/*         Developer : Code009                                             */
/*         Create on : 2020-09-16                                          */
/*-------------------------------------------------------------------------*/

#pragma once

//---------------------------------------------------------------------------
#include <cnMac/Mac_Header.h>
/*-------------------------------------------------------------------------*/
#ifdef	__cplusplus
//---------------------------------------------------------------------------
namespace cnLibrary{
//---------------------------------------------------------------------------
namespace cnRTL{
//---------------------------------------------------------------------------
#pragma mark - TODO : move to cnRTL
//---------------------------------------------------------------------------
class cTextEncodingConvetThrough : public iTextEncodingConverter
{
public:
	cTextEncodingConvetThrough(iTextEncodingConverter *From,iTextEncodingConverter *To);
	virtual uIntn cnLib_FUNC Convert(void *Dest,uIntn DestSize,const void *Src,uIntn SrcSize,uIntn *SrcConvertedSize)override;
private:
	rPtr<iTextEncodingConverter> fFrom;
	rPtr<iTextEncodingConverter> fTo;
};
}	// namespace cnRTL
//---------------------------------------------------------------------------
namespace cnMac{
//---------------------------------------------------------------------------
class cCFStringUTF16FromEncoding : public iTextEncodingConverter
{
public:
	cCFStringUTF16FromEncoding();
	~cCFStringUTF16FromEncoding();

	CFStringEncoding FromEncoding;

	virtual uIntn cnLib_FUNC Convert(void *Dest,uIntn DestSize,const void *Src,uIntn SrcSize,uIntn *SrcConvertedSize)override;
};
//---------------------------------------------------------------------------
class cCFStringUTF16ToEncoding : public iTextEncodingConverter
{
public:
	cCFStringUTF16ToEncoding();
	~cCFStringUTF16ToEncoding();

	CFStringEncoding ToEncoding;

	virtual uIntn cnLib_FUNC Convert(void *Dest,uIntn DestSize,const void *Src,uIntn SrcSize,uIntn *SrcConvertedSize)override;
protected:
};
//---------------------------------------------------------------------------
}	// namespace cnMac
}	// namespace cnLibrary
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
#endif	// __cplusplus
/*-------------------------------------------------------------------------*/

