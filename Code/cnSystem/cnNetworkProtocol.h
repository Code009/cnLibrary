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
#ifdef	cnLibrary_CPPEXCLUDE_VIRTUAL_OVERRIDE
#define	override
#endif
//---------------------------------------------------------------------------
namespace cnLibrary{
class cnLib_INTERFACE iIPv4Address : public iAddress
{
public:
	struct tInterfaceID{	static iTypeID Value;	};
	virtual void* cnLib_FUNC CastInterface(iTypeID ID)noexcept(true) override{		return cnVar::ImplementCastInterface(this,ID);	}

	virtual uInt32 cnLib_FUNC IPv4Address(void)noexcept(true)=0;
	virtual uInt16 cnLib_FUNC IPv4Port(void)noexcept(true)=0;
};
//---------------------------------------------------------------------------
namespace cnSystem{
//---------------------------------------------------------------------------

iPtr<iIPv4Address>		cnLib_FUNC CreateAddressIPv4(uInt32 IP,uInt16 Port)noexcept(true);

extern iConnectionProtocol*const TCP;
extern iDatagramProtocol*const UDP;

//---------------------------------------------------------------------------
}   // namespace cnSystem
//---------------------------------------------------------------------------
// GATT
//---------------------------------------------------------------------------

cnLib_INTENUM_BEGIN(ufInt8,GATTCharacteristicProperties)
{
	None					=0x00,
	Broadcast				=0x01,
	Read					=0x02,
	WriteWithoutResponse	=0x04,
	Write					=0x08,
	Notify					=0x10,
	Indicate				=0x20,
	AuthenticatedSignedWrites =0x40,
	ExtendedProperties		=0x80,
//	ReliableWrites			=0x100,
//	WritableAuxiliaries		=0x200,
}cnLib_INTENUM_END(GATTCharacteristicProperties);

cnLib_ENUM_BEGIN(ufInt8,GATTCharacteristicNotification)
{
	None,
	Notify,
	Indicate,
}cnLib_ENUM_END(GATTCharacteristicNotification);

cnLib_ENUM_BEGIN(ufInt8,GATTFunctionState)
{
	Absent,
	Scanning,
	Inactive,
	Active,
}cnLib_ENUM_END(GATTFunctionState);

//---------------------------------------------------------------------------
class iGATTClient;
class iGATTService;
class iGATTCharacteristic;
class iGATTDescriptor;
//---------------------------------------------------------------------------
class cnLib_INTERFACE iGATTDescriptorHandler
{
public:
	virtual void cnLib_FUNC GATTDescriptorStateChanged(void)noexcept(true)=0;
};
//---------------------------------------------------------------------------
class cnLib_INTERFACE iGATTDescriptor : public iReference
{
public:
	virtual cUUID cnLib_FUNC GetUUID(void)noexcept(true)=0;
	virtual iDispatch* cnLib_FUNC GetHandlerDispatch(void)noexcept(true)=0;
	virtual bool cnLib_FUNC InsertHandler(iGATTDescriptorHandler *Handler)noexcept(true)=0;
	virtual bool cnLib_FUNC RemoveHandler(iGATTDescriptorHandler *Handler)noexcept(true)=0;

	virtual eGATTFunctionState cnLib_FUNC GetFunctionState(void)noexcept(true)=0;
	virtual iGATTCharacteristic* cnLib_FUNC GetCharacterist(void)noexcept(true)=0;
};
//---------------------------------------------------------------------------
class cnLib_INTERFACE iGATTDescriptorObserver: public iAsyncNotification
{
public:
	virtual iDispatch* cnLib_FUNC GetHandlerDispatch(void)noexcept(true)=0;

	virtual iGATTCharacteristic* cnLib_FUNC GetCharacterist(void)noexcept(true)=0;

	virtual rPtr<iReference> cnLib_FUNC QueryDescriptors(cArray< rPtr<iGATTDescriptor> > &Descriptor)noexcept(true)=0;
	virtual void cnLib_FUNC DiscardChanges(void)noexcept(true)=0;
	virtual rPtr<iGATTDescriptor> cnLib_FUNC FetchChanged(bool &IsInserted)noexcept(true)=0;
};
//---------------------------------------------------------------------------
class cnLib_INTERFACE iGATTCharacteristicHandler
{
public:
	virtual void cnLib_FUNC GATTCharacteristStateChanged(void)noexcept(true)=0;

	virtual void cnLib_FUNC GATTCharacteristEffectiveValueNotificationChanged(void)noexcept(true)=0;
	virtual void cnLib_FUNC GATTCharacteristValueNotify(const void *Data,uIntn DataSize)noexcept(true)=0;
};
//---------------------------------------------------------------------------
class cnLib_INTERFACE iGATTCharacteristic : public iReference
{
public:
	virtual cUUID cnLib_FUNC GetUUID(void)noexcept(true)=0;
	virtual iDispatch* cnLib_FUNC GetHandlerDispatch(void)noexcept(true)=0;
	virtual bool cnLib_FUNC InsertHandler(iGATTCharacteristicHandler *Handler)noexcept(true)=0;
	virtual bool cnLib_FUNC RemoveHandler(iGATTCharacteristicHandler *Handler)noexcept(true)=0;

	virtual eGATTFunctionState cnLib_FUNC GetFunctionState(void)noexcept(true)=0;
	virtual iGATTService* cnLib_FUNC GetService(void)noexcept(true)=0;
	virtual rPtr<iGATTDescriptor> cnLib_FUNC AccessDescriptor(const cUUID &ID)noexcept(true)=0;
	virtual rPtr<iGATTDescriptorObserver> cnLib_FUNC CreateDescriptorObserver(void)noexcept(true)=0;

	virtual iPtr< iAsyncFunction<cConstMemory> > cnLib_FUNC Read(void)noexcept(true)=0;
	virtual iPtr<iAsyncTask> cnLib_FUNC Write(const void *Data,uIntn DataSize)noexcept(true)=0;
	virtual bool cnLib_FUNC WriteWithoutResponse(const void *Data,uIntn DataSize)noexcept(true)=0;

	virtual eGATTCharacteristicNotification cnLib_FUNC EffectiveValueNotification(void)noexcept(true)=0;
	virtual eGATTCharacteristicNotification cnLib_FUNC GetValueNotification(void)noexcept(true)=0;
	virtual void cnLib_FUNC SetValueNotification(eGATTCharacteristicNotification Notification)noexcept(true)=0;
};
//---------------------------------------------------------------------------
class cnLib_INTERFACE iGATTCharacteristicObserver: public iAsyncNotification
{
public:
	virtual iDispatch* cnLib_FUNC GetHandlerDispatch(void)noexcept(true)=0;

	virtual iGATTService* cnLib_FUNC GetService(void)noexcept(true)=0;

	virtual rPtr<iReference> cnLib_FUNC QueryCharacteristics(cArray< rPtr<iGATTCharacteristic> > &Characteristics)noexcept(true)=0;
	virtual void cnLib_FUNC DiscardChanges(void)noexcept(true)=0;
	virtual rPtr<iGATTCharacteristic> cnLib_FUNC FetchChanged(bool &IsInserted)noexcept(true)=0;
};
//---------------------------------------------------------------------------
class cnLib_INTERFACE iGATTServiceHandler
{
public:
	virtual void cnLib_FUNC GATTServiceStateChanged(void)noexcept(true)=0;
};
//---------------------------------------------------------------------------
class cnLib_INTERFACE iGATTService : public iReference
{
public:
	virtual cUUID cnLib_FUNC GetUUID(void)noexcept(true)=0;
	virtual iDispatch* cnLib_FUNC GetHandlerDispatch(void)noexcept(true)=0;
	virtual bool cnLib_FUNC InsertHandler(iGATTServiceHandler *Handler)noexcept(true)=0;
	virtual bool cnLib_FUNC RemoveHandler(iGATTServiceHandler *Handler)noexcept(true)=0;

	virtual eGATTFunctionState cnLib_FUNC GetFunctionState(void)noexcept(true)=0;
	virtual iGATTClient* cnLib_FUNC GetClient(void)noexcept(true)=0;
	virtual rPtr<iGATTCharacteristic> cnLib_FUNC AccessCharacteristic(const cUUID &ID)noexcept(true)=0;
	virtual iPtr<iGATTCharacteristicObserver> cnLib_FUNC CreateCharacteristicObserver(void)noexcept(true)=0;
};
//---------------------------------------------------------------------------
class cnLib_INTERFACE iGATTServiceObserver: public iAsyncNotification
{
public:
	virtual iDispatch* cnLib_FUNC GetHandlerDispatch(void)noexcept(true)=0;

	virtual iGATTClient* cnLib_FUNC GetClient(void)noexcept(true)=0;

	virtual rPtr<iReference> cnLib_FUNC QueryServices(cArray< rPtr<iGATTService> > &Services)noexcept(true)=0;
	virtual void cnLib_FUNC DiscardChanges(void)noexcept(true)=0;
	virtual rPtr<iGATTService> cnLib_FUNC FetchChanged(bool &IsInserted)noexcept(true)=0;
};
//---------------------------------------------------------------------------
class cnLib_INTERFACE iGATTClient : public iReference
{
public:
	virtual iDispatch* cnLib_FUNC GetHandlerDispatch(void)noexcept(true)=0;
	virtual eGATTFunctionState cnLib_FUNC GetFunctionState(void)noexcept(true)=0;

	virtual rPtr<iGATTService> cnLib_FUNC AccessService(const cUUID &ID)noexcept(true)=0;
	virtual iPtr<iGATTServiceObserver> cnLib_FUNC CreateServiceObserver(void)noexcept(true)=0;

};
//---------------------------------------------------------------------------
class iGATTServerCharacteristic;
class iGATTServerService;
class iGATTServer;
//---------------------------------------------------------------------------
class iGATTServerConnection : public iReference
{
public:
	virtual eiOrdering cnLib_FUNC Compare(iGATTServerConnection *Dest)noexcept(true)=0;
	virtual uIntn cnLib_FUNC GetMaxValueLength(void)noexcept(true)=0;
};
//---------------------------------------------------------------------------
class iGATTServerSubscription
{
public:
	virtual rPtr<iGATTServerConnection> cnLib_FUNC GetConnection(void)noexcept(true)=0;
};
//---------------------------------------------------------------------------
class iGATTServerDescriptor : public iReference
{
public:
	virtual iGATTServerCharacteristic* cnLib_FUNC GetOwner(void)noexcept(true)=0;
	virtual iDispatch* cnLib_FUNC GetHandlerDispatch(void)noexcept(true)=0;
	virtual void cnLib_FUNC Shutdown(void)noexcept(true)=0;
};
//---------------------------------------------------------------------------
class iGATTDescriptorController
{
public:
	virtual void cnLib_FUNC DescriptorPublished(void)noexcept(true)=0;
	virtual void cnLib_FUNC DescriptorClosed(void)noexcept(true)=0;
	virtual void cnLib_FUNC DescriptorRead(iGATTServerConnection *Connection,iWriteStreamBuffer *WriteBuffer)noexcept(true)=0;
	virtual void cnLib_FUNC DescriptorWrite(iGATTServerConnection *Connection,const void *Data,uIntn Length)noexcept(true)=0;
};
//---------------------------------------------------------------------------
class iGATTServerCharacteristic : public iReference
{
public:
	virtual iGATTServerService* cnLib_FUNC GetOwner(void)noexcept(true)=0;
	virtual iDispatch* cnLib_FUNC GetHandlerDispatch(void)noexcept(true)=0;
	virtual void cnLib_FUNC Shutdown(void)noexcept(true)=0;

	virtual rPtr<iGATTServerDescriptor> cnLib_FUNC CreateGATTDescriptor(const cUUID &ID,iReference *Reference,iGATTDescriptorController *Controller)noexcept(true)=0;	
	virtual iGATTServerSubscription* cnLib_FUNC QuerySubscription(iGATTServerConnection *Connection)noexcept(true)=0;
	virtual void cnLib_FUNC NotifyValue(iGATTServerSubscription *Subscription)noexcept(true)=0;
};
//---------------------------------------------------------------------------
struct cGATTCharacteristicParameter
{
	eGATTCharacteristicProperties Properties;
	//eGATTProtectionLevel ReadProtection;
	//eGATTProtectionLevel WriteProtection;
};
class iGATTCharacteristicController
{
public:
	virtual void cnLib_FUNC CharacteristicPublished(void)noexcept(true)=0;
	virtual void cnLib_FUNC CharacteristicClosed(void)noexcept(true)=0;
	virtual cGATTCharacteristicParameter cnLib_FUNC GetCharacteristicParameter(void)noexcept(true)=0;
	virtual void cnLib_FUNC ValueRead(iGATTServerConnection *Connection,iWriteStreamBuffer *WriteBuffer)noexcept(true)=0;
	virtual void cnLib_FUNC ValueWrite(iGATTServerConnection *Connection,const void *Data,uIntn Length)noexcept(true)=0;
	virtual void cnLib_FUNC ClientSubscribe(iGATTServerSubscription *Subscription)noexcept(true)=0;
	virtual void cnLib_FUNC ClientUnsubscribe(iGATTServerSubscription *Subscription)noexcept(true)=0;
	virtual bool cnLib_FUNC ValueNotify(iGATTServerSubscription *Subscription,iWriteStreamBuffer *WriteBuffer)noexcept(true)=0;
};
//---------------------------------------------------------------------------
class iGATTServerService : public iReference
{
public:
	virtual iDispatch* cnLib_FUNC GetHandlerDispatch(void)noexcept(true)=0;
	virtual void cnLib_FUNC Shutdown(void)noexcept(true)=0;
	virtual rPtr<iGATTServerCharacteristic> cnLib_FUNC CreateGATTCharacteristic(const cUUID &ID,iReference *Reference,iGATTCharacteristicController *Controller)noexcept(true)=0;
};
//---------------------------------------------------------------------------
class iGATTServiceController
{
public:
	virtual void cnLib_FUNC ServicePublished(void)noexcept(true)=0;
	virtual void cnLib_FUNC ServiceClosed(void)noexcept(true)=0;
};
//---------------------------------------------------------------------------
class iGATTServer : public iReference
{
public:
	virtual iDispatch* cnLib_FUNC GetHandlerDispatch(void)noexcept(true)=0;
	virtual void cnLib_FUNC Shutdown(void)noexcept(true)=0;
	virtual void cnLib_FUNC Publish(void)noexcept(true)=0;
	virtual rPtr<iGATTServerService> cnLib_FUNC CreateGATTService(const cUUID &ID,iReference *Reference,iGATTServiceController *Controller)noexcept(true)=0;
};
//---------------------------------------------------------------------------
class iGATTServerController
{
public:
	virtual void cnLib_FUNC ServerPublished(void)noexcept(true)=0;
	virtual void cnLib_FUNC ServerClosed(void)noexcept(true)=0;
};
//---------------------------------------------------------------------------
class iBluetoothPeripheral;
class iBluetoothCentral;
//---------------------------------------------------------------------------
class cnLib_INTERFACE iBluetoothMasterHandler
{
public:
	virtual void cnLib_FUNC CentralClosed(void)noexcept(true)=0;
	virtual void cnLib_FUNC CentralStateChanged(void)noexcept(true)=0;
};
//---------------------------------------------------------------------------
class iBluetoothMaster : public iGATTClient
{
public:
	virtual void cnLib_FUNC Close(void)noexcept(true)=0;
	virtual iAddress* cnLib_FUNC GetCentralAddress(void)noexcept(true)=0;
	virtual bool cnLib_FUNC InsertHandler(iBluetoothMasterHandler *Handler)noexcept(true)=0;
	virtual bool cnLib_FUNC RemoveHandler(iBluetoothMasterHandler *Handler)noexcept(true)=0;

	virtual iBluetoothPeripheral* cnLib_FUNC GetPeripheral(void)noexcept(true)=0;
};
//---------------------------------------------------------------------------
class iBluetoothPeripheralHandler
{
public:
	virtual void cnLib_FUNC PeripheralStateChanged(void)noexcept(true)=0;
	virtual void cnLib_FUNC PeripheralAdvertisementStateChanged(void)noexcept(true)=0;
};
//---------------------------------------------------------------------------
class iBluetoothPeripheral : public iReference
{
public:
	virtual iDispatch* cnLib_FUNC GetHandlerDispatch(void)noexcept(true)=0;
	virtual bool cnLib_FUNC InsertHandler(iBluetoothPeripheralHandler *Handler)noexcept(true)=0;
	virtual bool cnLib_FUNC RemoveHandler(iBluetoothPeripheralHandler *Handler)noexcept(true)=0;
	virtual bool cnLib_FUNC IsEnabled(void)noexcept(true)=0;
	virtual bool cnLib_FUNC IsPowerOn(void)noexcept(true)=0;

	virtual bool cnLib_FUNC IsAdvertising(void)noexcept(true)=0;
	virtual bool cnLib_FUNC GetAdvertisementActive(void)noexcept(true)=0;
	virtual void cnLib_FUNC SetAdvertisementActive(bool Active)noexcept(true)=0;

	virtual rPtr<iGATTServer> cnLib_FUNC CreateServer(iReference *ControllerReference,iGATTServerController *Controller)noexcept(true)=0;
	//virtual rPtr<iBluetoothMaster> cnLib_FUNC OpenCentral(void)noexcept(true)=0;
};
//---------------------------------------------------------------------------
class cnLib_INTERFACE iBluetoothSlaveHandler
{
public:
	virtual void cnLib_FUNC PeripheralClosed(void)noexcept(true)=0;
	virtual void cnLib_FUNC PeripheralStateChanged(void)noexcept(true)=0;
	virtual void cnLib_FUNC PeripheralNameChanged(void)noexcept(true)=0;
};
//---------------------------------------------------------------------------
class cnLib_INTERFACE iBluetoothSlave : public iGATTClient
{
public:
	virtual void cnLib_FUNC Close(void)noexcept(true)=0;
	virtual iAddress* cnLib_FUNC GetPeripheralAddress(void)noexcept(true)=0;
	virtual bool cnLib_FUNC InsertHandler(iBluetoothSlaveHandler *Handler)noexcept(true)=0;
	virtual bool cnLib_FUNC RemoveHandler(iBluetoothSlaveHandler *Handler)noexcept(true)=0;

	virtual iBluetoothCentral* cnLib_FUNC GetCentral(void)noexcept(true)=0;
	virtual rPtr<iReference> cnLib_FUNC QueryName(cArray<const uChar16> &Name)noexcept(true)=0;
};
//---------------------------------------------------------------------------
cnLib_ENUM_BEGIN(ufInt8,BluetoothLEAdvertisementType)
{
	ConnectableUndirected		=0,
	ConnectableDirected			=1,
	ScannableUndirected			=2,
	NonConnectableUndirected	=3,
	ScanResponse				=4,
    Extended					=5,
}cnLib_ENUM_END(BluetoothLEAdvertisementType);
//---------------------------------------------------------------------------
cnLib_INTENUM_BEGIN(ufInt8,BluetoothLEAdvertisementFlags)
{
	None						=0,		// no flag.
	LimitedDiscoverableMode		=1,		// Bluetooth LE Limited Discoverable Mode.
	GeneralDiscoverableMode		=2,		// Bluetooth LE General Discoverable Mode.
	ClassicNotSupported			=4,		// Bluetooth BR/EDR not supported.
	DualModeControllerCapable	=8,		// simultaneous Bluetooth LE and BR/EDR to same device capable (controller).
	DualModeHostCapable			=16,	// simultaneous Bluetooth LE and BR/EDR to same device capable (host)
}cnLib_INTENUM_END(BluetoothLEAdvertisementFlags);
//---------------------------------------------------------------------------
struct cBluetoothLEAdvertisementManufacturerData : cConstMemory
{
	ufInt16 CompanyID;
};
struct cBluetoothLEAdvertisementSectionData : cConstMemory
{
	ufInt8 SectionType;
};
struct cBluetoothLEAdvertisementInfo
{
	iAddress *Address;
	cArray<const uChar16> LocalName;
	uInt64 Timestamp;
	uInt64 PeripheralAddress;
	eBluetoothLEAdvertisementType Type;
	eBluetoothLEAdvertisementFlags Flags;
	sfInt8 SignalStrength;
	bool IsConnectable	:1;
	bool IsDiscoverable	:1;
	cArray<const cUUID> ServiceUUIDs;
	cArray<const cBluetoothLEAdvertisementManufacturerData> ManufacturerData;
	cArray<const cBluetoothLEAdvertisementSectionData> SectionData;
};
//---------------------------------------------------------------------------
class iBluetoothLEAdvertisementObserver : public iAsyncNotification
{
public:
	virtual iDispatch* cnLib_FUNC GetHandlerDispatch(void)noexcept(true)=0;
	virtual iBluetoothCentral* cnLib_FUNC GetCentral(void)noexcept(true)=0;

	virtual void cnLib_FUNC DiscardQueue(void)noexcept(true)=0;
	virtual rPtr<iReference> cnLib_FUNC Fetch(cBluetoothLEAdvertisementInfo &Info)noexcept(true)=0;
};
//---------------------------------------------------------------------------
class iBluetoothCentralHandler
{
public:
	virtual void cnLib_FUNC CentralStateChanged(void)noexcept(true)=0;
};
//---------------------------------------------------------------------------
class iBluetoothCentral : public iReference
{
public:
	virtual iDispatch* cnLib_FUNC GetHandlerDispatch(void)noexcept(true)=0;
	virtual bool cnLib_FUNC InsertHandler(iBluetoothCentralHandler *Handler)noexcept(true)=0;
	virtual bool cnLib_FUNC RemoveHandler(iBluetoothCentralHandler *Handler)noexcept(true)=0;
	virtual bool cnLib_FUNC IsEnabled(void)noexcept(true)=0;
	virtual bool cnLib_FUNC IsPowerOn(void)noexcept(true)=0;

	virtual rPtr<iBluetoothLEAdvertisementObserver> cnLib_FUNC CreateLEAdvertisementObserver(void)noexcept(true)=0;

	//virtual rPtr<iGATTServer> cnLib_FUNC CreateServer(iReference *ControllerReference,iGATTServerController *Controller)noexcept(true)=0;
	virtual rPtr<iBluetoothSlave> cnLib_FUNC OpenPeripheral(iAddress *Address)noexcept(true)=0;
};
//---------------------------------------------------------------------------
namespace cnSystem{
//---------------------------------------------------------------------------
}   // namespace cnSystem
//---------------------------------------------------------------------------
}	// namespace cnLibrary
//---------------------------------------------------------------------------
#ifdef	cnLibrary_CPPEXCLUDE_VIRTUAL_OVERRIDE
#undef	override
#endif
//---------------------------------------------------------------------------
#endif  /* __cplusplus */
/*-------------------------------------------------------------------------*/
#endif
