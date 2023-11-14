/*--- MacOS Interface -----------------------------------------------------*/
/*         Developer : Code009                                             */
/*         Create on : 2016-06-26                                          */
/*-------------------------------------------------------------------------*/
#pragma once

#include <sys/socket.h>
#ifdef	__OBJC__
#import <IOBluetooth/IOBluetooth.h>
#endif

#ifdef	__cplusplus

#include <cnSystem/cnSystem.h>
#include <cnSystem/cnMac_Sys.h>
#include <cnSystem/cnMac_UI.h>
#include <cnSystem/MacOS_UI.h>

//---------------------------------------------------------------------------
namespace cnLibrary{
//---------------------------------------------------------------------------
namespace cnMacOS{
//---------------------------------------------------------------------------
extern iFileName*const	Folder_Application;
extern iFileName*const	Folder_Document;
extern iFileName*const	Folder_Temp;
//---------------------------------------------------------------------------
#ifdef	__OBJC__
iPtr<iConnectionTask> BluetoothRFCOMMConnect(IOBluetoothDevice *Device,UInt8 ChannelID);
#endif

//---------------------------------------------------------------------------
iPtr<iThreadPool>		cnLib_FUNC CreateThreadPool(void);

rPtr<iTextEncodingConverter> cnLib_FUNC TextEncodingConverter(const char *ToEncoding,const char *FromEncoding);
//---------------------------------------------------------------------------
}	// cnMacOS
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
}	// namespace cnLibrary
//---------------------------------------------------------------------------
#endif	// __cplusplus

