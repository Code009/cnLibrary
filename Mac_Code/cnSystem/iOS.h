/*--- iOS Interface -------------------------------------------------------*/
/*         Developer : Code009                                             */
/*         Create on : 2015-01-27                                          */
/*-------------------------------------------------------------------------*/
#pragma once
#include <dispatch/dispatch.h>

#ifdef	__cplusplus

#include <cnSystem/cnSystem.h>
#include <cnSystem/cnMac_Sys.h>
#include <cnSystem/cnMac_UI.h>
#include <cnSystem/iOS_UI.h>

//---------------------------------------------------------------------------
namespace cnLibrary{
//---------------------------------------------------------------------------
namespace cniOS{
//---------------------------------------------------------------------------
extern iFileName*const	Folder_Application;
extern iFileName*const	Folder_Document;
extern iFileName*const	Folder_Temp;
//---------------------------------------------------------------------------
iPtr<iThreadPool>		cnLib_FUNC CreateThreadPool(void);
//---------------------------------------------------------------------------
//rRet<iCharacterSetConverter> cnLib_FUNC CFStringUTF16FromEncoding(CFStringEncoding FromEncoding);
//rRet<iCharacterSetConverter> cnLib_FUNC CFStringUTF16ToEncoding(CFStringEncoding ToEncoding);
//---------------------------------------------------------------------------
}	// cniOS
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
}	// namespace cnLibrary
//---------------------------------------------------------------------------
#endif	// __cplusplus

