/*--- Mac Interface -------------------------------------------------------*/
/*         Developer : Code009                                             */
/*         Create on : 2017-05-29                                          */
/*-------------------------------------------------------------------------*/
#pragma once
#include <CoreFoundation/CoreFoundation.h>
#include <dispatch/dispatch.h>

#ifdef	__OBJC__
#include <CoreBluetooth/CoreBluetooth.h>
#endif

#ifdef	__cplusplus

#include <cnSystem/cnSystem.h>
#include <cnSystem/POSIXSystem.h>
#include <cnRTL/cnRTL.h>

#define	cnLibOC_INTERFACE_DEFINE(_i_,_pi_)	\
	virtual const void*_Nullable cnLib_FUNC CastInterface(iTypeID ID)const override{	\
		if(ID==cnInterfaceID)return this;	return _pi_::CastInterface(ID);	\
	}	cnLib_INTERFACE_ID(_i_)


//---------------------------------------------------------------------------
namespace cnLibrary{
//---------------------------------------------------------------------------
#ifdef	__OBJC__
//---------------------------------------------------------------------------
class cnLib_INTERFACE iOCObject : public iInterface
{
public:
	cnLib_INTERFACE_DEFINE(iOCObject,iInterface);

	virtual id GetOCObject(void)=0;
};
//---------------------------------------------------------------------------
#endif	// __OBJC__
//---------------------------------------------------------------------------
//class cnLib_INTERFACE iMacFileName : public iFileName
//{
//public:
//	cnLib_INTERFACE_DEFINE(iMacFileName,iFileName);

//	virtual const char * GetMacFileName(void)=0;
//	virtual uIntn GetMacFileNameLength(void)=0;
//};
//---------------------------------------------------------------------------
class cnLib_INTERFACE iGCDDispatch : public iInterface
{
public:
	cnLib_INTERFACE_DEFINE(iGCDDispatch,iInterface);

	virtual dispatch_queue_t GetGCDDispatch(void)=0;
};
//---------------------------------------------------------------------------
struct cBluetoothLEDeviceInfo
{
	iPtr<iAddress> Address;
	rPtr< iArrayReference<const uChar16> > Name;
};
//---------------------------------------------------------------------------
class cnLib_INTERFACE iBluetoothLEConnectionBrowser : public iConnectionBrowser
{
public:
	virtual bool cnLib_FUNC FetchBLEDevice(cBluetoothLEDeviceInfo &DeviceInfo)=0;
};
//---------------------------------------------------------------------------
typedef uInt8 tUUID[16];
//---------------------------------------------------------------------------
struct cBluetoothLEConnectionServiceID
{
	tUUID ServiceUUID;
	tUUID ReadCharacteristUUID;
	tUUID WriteCharacteristUUID;
};
//---------------------------------------------------------------------------
}	// namespace cnLibrary
//---------------------------------------------------------------------------
#endif	// __cplusplus

#ifdef	__OBJC__

@interface cnLib_BluetoothLEConnectionService : NSObject

#ifdef	__cplusplus
@property(nonatomic,readonly) cnLibrary::iAddress * interfaceAddress;
#endif

#pragma mark subclass

@property(nonatomic,readonly) CBMutableService * service API_AVAILABLE(macos(10.9));
@property(nonatomic,readonly) CBCharacteristic * readCharacteristic;
@property(nonatomic,readonly) CBCharacteristic * writeCharacteristic;
//-(void)bleSendData:(NSData *)data;

@end

@interface cnLib_BluetoothLEConnectionProtocol : NSObject
<
	CBCentralManagerDelegate,
	CBPeripheralManagerDelegate
>

#ifdef	__cplusplus
@property(nonatomic,readonly) cnLibrary::iConnectionProtocol * interfaceConnectionProtocol;
-(cnLibrary::iPtr<cnLibrary::iBluetoothLEConnectionBrowser>)createScanner:(cnLibrary::cBluetoothLEConnectionServiceID)service;
-(cnLibrary::iPtr<cnLibrary::iBluetoothLEConnectionBrowser>)createScanner:(const cnLibrary::cBluetoothLEConnectionServiceID*)ServiceIDs length:(cnLibrary::uIntn)Length;
#endif

-(void)updatePeripheralAdvertisementData API_AVAILABLE(macos(10.9));


#pragma subclass

@property(nonatomic,readonly) dispatch_queue_t queue;
@property(nonatomic,readonly) CBCentralManager *centralManager;
@property(nonatomic,readonly) CBPeripheralManager *peripheralManager API_AVAILABLE(macos(10.9));
@property(nonatomic,readonly) NSDictionary *peripheralAdvertisementData API_AVAILABLE(macos(10.9));

@end

#endif	// __OBJC__

#ifdef	__cplusplus
//---------------------------------------------------------------------------
namespace cnLibrary{
//---------------------------------------------------------------------------
//iPtr<iFileName>			cnLib_FUNC CreateTemporaryFile(iFileName *FolderName);
//---------------------------------------------------------------------------
}	// namespace cnLibrary
//---------------------------------------------------------------------------
#endif	// __cplusplus
