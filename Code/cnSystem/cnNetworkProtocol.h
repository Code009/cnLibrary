/*- cnSystem - Network - Protocol -----------------------------------------*/
/*         Developer : Code009                                             */
/*         Create on : 2010-03-28                                          */
/*-------------------------------------------------------------------------*/
#ifndef __cnLibrary_cnSystem_cnNetworkProtocol_H__
#define	__cnLibrary_cnSystem_cnNetworkProtocol_H__
/*-------------------------------------------------------------------------*/
#include <cnSystem/cnFundamental.h>
#include <cnSystem/cnData.h>
#include <cnSystem/cnThread.h>
#include <cnSystem/cnStream.h>
#include <cnSystem/cnNetwork.h>
/*-------------------------------------------------------------------------*/
#ifdef __cplusplus
//---------------------------------------------------------------------------
namespace cnLibrary{
//---------------------------------------------------------------------------
class cnLib_INTERFACE iIPv4Address : public iAddress
{
public:
	cnLib_INTERFACE_DEFINE(iIPv4Address,iAddress)

	virtual uInt32 cnLib_FUNC IPv4Address(void)=0;
	virtual uInt16 cnLib_FUNC IPv4Port(void)=0;
};
//---------------------------------------------------------------------------
namespace cnSystem{
//---------------------------------------------------------------------------

iPtr<iIPv4Address>		cnLib_FUNC CreateAddressIPv4(uInt32 IP,uInt16 Port);

extern iConnectionProtocol*const TCP;
extern iDatagramProtocol*const UDP;

//---------------------------------------------------------------------------
}   // namespace cnSystem
//---------------------------------------------------------------------------
// GATT
//---------------------------------------------------------------------------

typedef cnMemory::cPlainData<16> cGATTUUID;
cnLib_ENUM_BEGIN(ufInt8,GATTFunctionState)
{
	Absent,
	Inactive,
	Active,
}cnLib_ENUM_END(GATTFunctionState);

class iGATTPeripheralCentral;
class iGATTPeripheral;
class iGATTService;
class iGATTCharacteristic;
class iGATTDescriptor;
//---------------------------------------------------------------------------
typedef iAsyncFunction< iArrayReference< rPtr<iGATTService> >* >		iGATTServiceAsyncFunction;
typedef iAsyncFunction< iArrayReference< rPtr<iGATTCharacteristic> >* >	iGATTCharacteristicAsyncFunction;
typedef iAsyncFunction< iArrayReference< rPtr<iGATTDescriptor> >* >		iGATTDescriptorAsyncFunction;
//---------------------------------------------------------------------------
class iGATTDescriptorHandler
{
public:
	virtual void cnLib_FUNC GATTDescriptorStateChanged(void)=0;
};
//---------------------------------------------------------------------------
class iGATTDescriptor : public iReference
{
public:
	virtual cGATTUUID cnLib_FUNC GetUUID(void)=0;
	virtual iDispatch* cnLib_FUNC GetHandlerDispatch(void)=0;
	virtual bool cnLib_FUNC InsertHandler(iGATTDescriptorHandler *Handler)=0;
	virtual bool cnLib_FUNC RemoveHandler(iGATTDescriptorHandler *Handler)=0;

	virtual eGATTFunctionState cnLib_FUNC GetFunctionState(void)=0;
	virtual iGATTCharacteristic* cnLib_FUNC GetCharacterist(void)=0;
};
//---------------------------------------------------------------------------
class iGATTCharacteristHandler
{
public:
	virtual void cnLib_FUNC GATTCharacteristStateChanged(void)=0;

	virtual void cnLib_FUNC GATTCharacteristDescriptorListChanged(void)=0;

	virtual void cnLib_FUNC GATTCharacteristValueNotify(void)=0;
};
//---------------------------------------------------------------------------
class iGATTCharacteristic : public iReference
{
public:
	virtual cGATTUUID cnLib_FUNC GetUUID(void)=0;
	virtual iDispatch* cnLib_FUNC GetHandlerDispatch(void)=0;
	virtual bool cnLib_FUNC InsertHandler(iGATTCharacteristHandler *Handler)=0;
	virtual bool cnLib_FUNC RemoveHandler(iGATTCharacteristHandler *Handler)=0;

	virtual eGATTFunctionState cnLib_FUNC GetFunctionState(void)=0;
	virtual iGATTService* cnLib_FUNC GetService(void)=0;
	virtual rPtr<iGATTDescriptor> cnLib_FUNC PersistDescriptor(const cGATTUUID &ID)=0;
	virtual iPtr<iGATTDescriptorAsyncFunction> cnLib_FUNC QueryDescriptors(void)=0;
	//virtual void cnLib_FUNC ClearDescriptorCache(void)=0;

	virtual rPtr< iArrayReference<const void> > cnLib_FUNC GetValue(void)=0;

	virtual iPtr<iAsyncTask> cnLib_FUNC Write(const void *Data,uIntn DataSize)=0;
	virtual bool cnLib_FUNC WriteWithoutResponse(const void *Data,uIntn DataSize)=0;
};
//---------------------------------------------------------------------------
class iGATTServiceHandler
{
public:
	virtual void cnLib_FUNC GATTServiceStateChanged(void)=0;

	virtual void cnLib_FUNC GATTServiceCharacteristListChanged(void)=0;
};
//---------------------------------------------------------------------------
class iGATTService : public iReference
{
public:
	virtual cGATTUUID cnLib_FUNC GetUUID(void)=0;
	virtual iDispatch* cnLib_FUNC GetHandlerDispatch(void)=0;
	virtual bool cnLib_FUNC InsertHandler(iGATTServiceHandler *Handler)=0;
	virtual bool cnLib_FUNC RemoveHandler(iGATTServiceHandler *Handler)=0;

	virtual eGATTFunctionState cnLib_FUNC GetFunctionState(void)=0;
	virtual iGATTPeripheral* cnLib_FUNC GetPeripheral(void)=0;
	virtual rPtr<iGATTCharacteristic> cnLib_FUNC PersistCharacteristic(const cGATTUUID &ID)=0;
	virtual iPtr<iGATTCharacteristicAsyncFunction> cnLib_FUNC QueryCharacteristics(void)=0;

	//virtual void cnLib_FUNC ClearCharacteristicsCache(void)=0;
};
//---------------------------------------------------------------------------
class iGATTPeripheralHandler
{
public:
	virtual void cnLib_FUNC GATTPeripheralStateChanged(void)=0;
	virtual void cnLib_FUNC GATTPeripheralRSSIChanged(void)=0;
	virtual void cnLib_FUNC GATTPeripheralNameChanged(void)=0;
	virtual void cnLib_FUNC GATTPeripheralServicesChanged(void)=0;
};
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
class iGATTPeripheral : public iReference
{
public:
	virtual iDispatch* cnLib_FUNC GetHandlerDispatch(void)=0;
	virtual bool cnLib_FUNC InsertHandler(iGATTPeripheralHandler *Handler)=0;
	virtual bool cnLib_FUNC RemoveHandler(iGATTPeripheralHandler *Handler)=0;

	virtual iAddress* cnLib_FUNC GetPeripheralAddress(void)=0;
	virtual eGATTFunctionState cnLib_FUNC GetFunctionState(void)=0;
	virtual iGATTPeripheralCentral* cnLib_FUNC GetPeripheralCentral(void)=0;
	virtual rPtr< iArrayReference<const uChar16> > cnLib_FUNC GetName(void)=0;
	virtual rPtr<iGATTService> cnLib_FUNC AccessService(const cGATTUUID &ID)=0;
	virtual iPtr<iGATTServiceAsyncFunction> cnLib_FUNC QueryServices(void)=0;
	//virtual void cnLib_FUNC ClearServiceCache(void)=0;
	
	virtual iPtr<iAsyncTask> cnLib_FUNC Activate(void)=0;
};
//---------------------------------------------------------------------------
class iGATTPeripheralObserver : public iAsyncNotification
{
public:
	virtual iDispatch* cnLib_FUNC GetHandlerDispatch(void)=0;

	virtual iGATTPeripheralCentral* cnLib_FUNC GetPeripheralCentral(void)=0;

	virtual void cnLib_FUNC DiscardChanges(void)=0;
	virtual rPtr< iArrayReference< rPtr<iGATTPeripheral> > > cnLib_FUNC QueryAll(void)=0;
	virtual rPtr< iArrayReference< rPtr<iGATTPeripheral> > > cnLib_FUNC QueryChanges(void)=0;
};
//---------------------------------------------------------------------------
cnLib_ENUM_BEGIN(ufInt8,GATTAdvertisementType)
{
	ConnectableUndirected		=0,
	ConnectableDirected			=1,
	ScannableUndirected			=2,
	NonConnectableUndirected	=3,
	ScanResponse				=4,
    Extended					=5,
}cnLib_ENUM_END(GATTAdvertisementType);
//---------------------------------------------------------------------------
cnLib_INTENUM_BEGIN(ufInt8,GATTAdvertisementFlags)
{
	None						=0,		// no flag.
	LimitedDiscoverableMode		=1,		// Bluetooth LE Limited Discoverable Mode.
	GeneralDiscoverableMode		=2,		// Bluetooth LE General Discoverable Mode.
	ClassicNotSupported			=4,		// Bluetooth BR/EDR not supported.
	DualModeControllerCapable	=8,		// simultaneous Bluetooth LE and BR/EDR to same device capable (controller).
	DualModeHostCapable			=16,	// simultaneous Bluetooth LE and BR/EDR to same device capable (host)
}cnLib_INTENUM_END(GATTAdvertisementFlags);
//---------------------------------------------------------------------------
struct cGATTAdvertisementInfo
{
	cArray<const uChar16> LocalName;
	iTimepoint* Timestamp;
	sfInt16 SignalStrength;
	eGATTAdvertisementType Type;
	eGATTAdvertisementFlags Flags;
};
//---------------------------------------------------------------------------
class iGATTAdvertisement : public iReference
{
public:
	virtual iAddress* cnLib_FUNC GetPeripheralAddress(void)=0;
	virtual cGATTAdvertisementInfo cnLib_FUNC GetInfo(void)=0;

	virtual uIntn cnLib_FUNC GetServiceUUIDCount(void)=0;
	virtual cGATTUUID cnLib_FUNC GetServiceUUID(uIntn Index)=0;

	virtual uIntn cnLib_FUNC GetManufactureDataCount(void)=0;
	virtual cConstMemory cnLib_FUNC GetManufactureData(uIntn Index,ufInt16 &CompanyID)=0;

	virtual uIntn cnLib_FUNC GetSectionDataCount(void)=0;
	virtual cConstMemory cnLib_FUNC GetSectionData(uIntn Index,ufInt8 &DataType)=0;
};
//---------------------------------------------------------------------------
class iGATTAdvertisementObserver : public iAsyncNotification
{
public:
	virtual iDispatch* cnLib_FUNC GetHandlerDispatch(void)=0;

	virtual iGATTPeripheralCentral* cnLib_FUNC GetPeripheralCentral(void)=0;

	virtual void cnLib_FUNC DiscardQueue(void)=0;
	virtual rPtr<iGATTAdvertisement> cnLib_FUNC Fetch(void)=0;
};
//---------------------------------------------------------------------------
class iGATTPeripheralCentralHandler
{
public:
	virtual void cnLib_FUNC GATTCentralStateChanged(void)=0;
	virtual void cnLib_FUNC GATTCentralPeripheralListChanged(void)=0;
};
//---------------------------------------------------------------------------
class iGATTPeripheralCentral : public iReference
{
public:
	virtual iDispatch* cnLib_FUNC GetHandlerDispatch(void)=0;
	virtual bool cnLib_FUNC InsertHandler(iGATTPeripheralCentralHandler *Handler)=0;
	virtual bool cnLib_FUNC RemoveHandler(iGATTPeripheralCentralHandler *Handler)=0;

	virtual rPtr<iGATTPeripheral> cnLib_FUNC AccessPeripheral(iAddress *Address)=0;

	virtual rPtr<iGATTAdvertisementObserver> cnLib_FUNC CreateAdvertisementObserver(void)=0;
	virtual rPtr<iGATTPeripheralObserver> cnLib_FUNC CreatePeripheralObserver(const cGATTUUID *Service,uIntn ServiceCount)=0;
	virtual bool cnLib_FUNC ResetPeripheralCache(void)=0;

	virtual bool cnLib_FUNC IsEnabled(void)=0;
	virtual bool cnLib_FUNC IsPowerOn(void)=0;
};
//---------------------------------------------------------------------------
class iGATTPeripheralDevice : public iReference
{
public:
};
//---------------------------------------------------------------------------
namespace cnSystem{
//---------------------------------------------------------------------------
}   // namespace cnSystem
//---------------------------------------------------------------------------
}	// namespace cnLibrary
//---------------------------------------------------------------------------
#endif  /* __cplusplus */
/*-------------------------------------------------------------------------*/
#endif
