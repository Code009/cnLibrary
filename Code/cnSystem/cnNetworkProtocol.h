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

typedef cnMemory::cPlainData<16> cGATTUUID;
cnLib_ENUM_BEGIN(ufInt8,GATTFunctionState)
{
	Absent,
	Scanning,
	Inactive,
	Active,
}cnLib_ENUM_END(GATTFunctionState);
cnLib_ENUM_BEGIN(ufInt8,GATTCharacteristicNotification)
{
	None,
	Notify,
	Indicate,
}cnLib_ENUM_END(GATTCharacteristicNotification);

class iGATTPeripheralCentral;
class iGATTPeripheral;
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
	virtual cGATTUUID cnLib_FUNC GetUUID(void)noexcept(true)=0;
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

	virtual iArrayReference< rPtr<iGATTDescriptor> > cnLib_FUNC QueryDescriptors(void)noexcept(true)=0;
	virtual void cnLib_FUNC DiscardChanges(void)noexcept(true)=0;
	virtual rPtr<iGATTDescriptor> cnLib_FUNC FetchChanged(bool &IsInserted)noexcept(true)=0;
};
//---------------------------------------------------------------------------
class cnLib_INTERFACE iGATTCharacteristicHandler
{
public:
	virtual void cnLib_FUNC GATTCharacteristStateChanged(void)noexcept(true)=0;

	virtual void cnLib_FUNC GATTCharacteristValueNotify(const void *Data,uIntn DataSize)noexcept(true)=0;
};
//---------------------------------------------------------------------------
class cnLib_INTERFACE iGATTCharacteristic : public iReference
{
public:
	virtual cGATTUUID cnLib_FUNC GetUUID(void)noexcept(true)=0;
	virtual iDispatch* cnLib_FUNC GetHandlerDispatch(void)noexcept(true)=0;
	virtual bool cnLib_FUNC InsertHandler(iGATTCharacteristicHandler *Handler)noexcept(true)=0;
	virtual bool cnLib_FUNC RemoveHandler(iGATTCharacteristicHandler *Handler)noexcept(true)=0;

	virtual eGATTFunctionState cnLib_FUNC GetFunctionState(void)noexcept(true)=0;
	virtual iGATTService* cnLib_FUNC GetService(void)noexcept(true)=0;
	virtual rPtr<iGATTDescriptor> cnLib_FUNC AccessDescriptor(const cGATTUUID &ID)noexcept(true)=0;
	virtual rPtr<iGATTDescriptorObserver> cnLib_FUNC CreateDescriptorObserver(void)noexcept(true)=0;

	virtual rPtr< iArrayReference<const void> > cnLib_FUNC Read(void)noexcept(true)=0;

	virtual iPtr<iAsyncTask> cnLib_FUNC Write(const void *Data,uIntn DataSize)noexcept(true)=0;
	virtual bool cnLib_FUNC WriteWithoutResponse(const void *Data,uIntn DataSize)noexcept(true)=0;

	virtual iPtr< iAsyncFunction<bool> > cnLib_FUNC ConfigureNotification(eGATTCharacteristicNotification Notification)noexcept(true)=0;
};
//---------------------------------------------------------------------------
class cnLib_INTERFACE iGATTCharacteristicObserver: public iAsyncNotification
{
public:
	virtual iDispatch* cnLib_FUNC GetHandlerDispatch(void)noexcept(true)=0;

	virtual iGATTService* cnLib_FUNC GetService(void)noexcept(true)=0;

	virtual iArrayReference< rPtr<iGATTCharacteristic> > cnLib_FUNC QueryCharacteristics(void)noexcept(true)=0;
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
	virtual cGATTUUID cnLib_FUNC GetUUID(void)noexcept(true)=0;
	virtual iDispatch* cnLib_FUNC GetHandlerDispatch(void)noexcept(true)=0;
	virtual bool cnLib_FUNC InsertHandler(iGATTServiceHandler *Handler)noexcept(true)=0;
	virtual bool cnLib_FUNC RemoveHandler(iGATTServiceHandler *Handler)noexcept(true)=0;

	virtual eGATTFunctionState cnLib_FUNC GetFunctionState(void)noexcept(true)=0;
	virtual iGATTPeripheral* cnLib_FUNC GetPeripheral(void)noexcept(true)=0;
	virtual rPtr<iGATTCharacteristic> cnLib_FUNC AccessCharacteristic(const cGATTUUID &ID)noexcept(true)=0;
	virtual iPtr<iGATTCharacteristicObserver> cnLib_FUNC CreateCharacteristicObserver(void)noexcept(true)=0;
};
//---------------------------------------------------------------------------
class cnLib_INTERFACE iGATTServiceObserver: public iAsyncNotification
{
public:
	virtual iDispatch* cnLib_FUNC GetHandlerDispatch(void)noexcept(true)=0;

	virtual iGATTPeripheral* cnLib_FUNC GetPeripheral(void)noexcept(true)=0;

	virtual iArrayReference< rPtr<iGATTService> > cnLib_FUNC QueryServices(void)noexcept(true)=0;
	virtual void cnLib_FUNC DiscardChanges(void)noexcept(true)=0;
	virtual rPtr<iGATTService> cnLib_FUNC FetchChanged(bool &IsInserted)noexcept(true)=0;
	virtual iPtr<iAsyncTask> cnLib_FUNC ScanServices(void)noexcept(true)=0;
};
//---------------------------------------------------------------------------
class cnLib_INTERFACE iGATTPeripheralHandler
{
public:
	virtual void cnLib_FUNC GATTPeripheralStateChanged(void)noexcept(true)=0;
	virtual void cnLib_FUNC GATTPeripheralNameChanged(void)noexcept(true)=0;
};
//---------------------------------------------------------------------------
class cnLib_INTERFACE iGATTPeripheral : public iReference
{
public:
	virtual iAddress* cnLib_FUNC GetPeripheralAddress(void)noexcept(true)=0;
	virtual iDispatch* cnLib_FUNC GetHandlerDispatch(void)noexcept(true)=0;
	virtual bool cnLib_FUNC InsertHandler(iGATTPeripheralHandler *Handler)noexcept(true)=0;
	virtual bool cnLib_FUNC RemoveHandler(iGATTPeripheralHandler *Handler)noexcept(true)=0;

	virtual eGATTFunctionState cnLib_FUNC GetFunctionState(void)noexcept(true)=0;
	virtual iGATTPeripheralCentral* cnLib_FUNC GetPeripheralCentral(void)noexcept(true)=0;
	virtual rPtr< iArrayReference<const uChar16> > cnLib_FUNC GetName(void)noexcept(true)=0;

	virtual rPtr<iGATTService> cnLib_FUNC AccessService(const cGATTUUID &ID)noexcept(true)=0;
	virtual iPtr<iGATTServiceObserver> cnLib_FUNC CreateServiceObserver(void)noexcept(true)=0;
	
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
struct cGATTAdvertisementManufacturerData : cConstMemory
{
	ufInt16 CompandID;
};
struct cGATTAdvertisementSectionData : cConstMemory
{
	ufInt8 SectionID;
};
struct cGATTAdvertisementInfo
{
	iAddress *PeripheralAddress;
	cArray<const uChar16> LocalName;
	uInt64 Timestamp;
	sfInt16 SignalStrength;
	eGATTAdvertisementType Type;
	eGATTAdvertisementFlags Flags;
	cArray<const cGATTUUID> ServiceUUIDs;
	cArray<const cGATTAdvertisementManufacturerData> ManufacturerData;
	cArray<const cGATTAdvertisementSectionData> SectionData;
};
//---------------------------------------------------------------------------
class iGATTAdvertisement : public iReference
{
public:
	virtual const cGATTAdvertisementInfo& cnLib_FUNC GetInfo(void)noexcept(true)=0;
};
//---------------------------------------------------------------------------
class iGATTAdvertisementObserver : public iAsyncNotification
{
public:
	virtual iDispatch* cnLib_FUNC GetHandlerDispatch(void)noexcept(true)=0;

	virtual iGATTPeripheralCentral* cnLib_FUNC GetPeripheralCentral(void)noexcept(true)=0;

	virtual void cnLib_FUNC DiscardQueue(void)noexcept(true)=0;
	virtual rPtr<iGATTAdvertisement> cnLib_FUNC Fetch(void)noexcept(true)=0;
};
//---------------------------------------------------------------------------
class iGATTPeripheralCentralHandler
{
public:
	virtual void cnLib_FUNC GATTCentralStateChanged(void)noexcept(true)=0;
};
//---------------------------------------------------------------------------
class iGATTPeripheralCentral : public iReference
{
public:
	virtual iDispatch* cnLib_FUNC GetHandlerDispatch(void)noexcept(true)=0;
	virtual bool cnLib_FUNC InsertHandler(iGATTPeripheralCentralHandler *Handler)noexcept(true)=0;
	virtual bool cnLib_FUNC RemoveHandler(iGATTPeripheralCentralHandler *Handler)noexcept(true)=0;

	virtual rPtr<iGATTPeripheral> cnLib_FUNC AccessPeripheral(iAddress *Address)noexcept(true)=0;

	virtual rPtr<iGATTAdvertisementObserver> cnLib_FUNC CreateAdvertisementObserver(void)noexcept(true)=0;

	virtual bool cnLib_FUNC IsEnabled(void)noexcept(true)=0;
	virtual bool cnLib_FUNC IsPowerOn(void)noexcept(true)=0;
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
