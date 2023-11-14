/*--- Mac - Bluetooth LE --------------------------------------------------*/
/*         Developer : Code009                                             */
/*         Create on : 2020-10-06                                          */
/*-------------------------------------------------------------------------*/
#pragma once
//---------------------------------------------------------------------------
#include <cnMac/Mac_Header.h>
#include <siOS/POSIX/POSIX_Thread.h>
//---------------------------------------------------------------------------
#ifdef	__cplusplus
//---------------------------------------------------------------------------
namespace cnLibrary{
namespace cnRTL{


class bcBufferedRWQueue : public bcRWQueue
{
public:
	bcBufferedRWQueue();
	~bcBufferedRWQueue();

	void PutReadData(const void *Data,uIntn Size);
	cMemory QueryReadDataBuffer(uIntn QuerySize);
	uIntn AdvanceReadDataBuffer(uIntn Size);

	cConstMemory QueryWriteData(void);
	uIntn AdvanceWriteData(uIntn Size);
	bool IsWriteDataEnded(void);

protected:

	virtual cConstMemory cnLib_FUNC GatherReadBuffer(uIntn Size)override;
	virtual uIntn cnLib_FUNC DismissReadBuffer(uIntn Size)override;
	virtual void ReadQueueClosed(void)override;

	virtual cMemory cnLib_FUNC ReserveWriteBuffer(uIntn QuerySize)override;
	virtual uIntn cnLib_FUNC CommitWriteBuffer(uIntn Size)override;
	virtual void WriteQueueClosed(void)override;

	virtual void ReadBufferNotify(void)=0;
	virtual void WriteDataNotify(void)=0;

private:
	cnRTL::cAsyncLoopbackStreamBuffer fReadDataQueue;
	cnRTL::cAsyncLoopbackStreamBuffer fWriteDataQueue;
};

}	// namespace cnRTL
//---------------------------------------------------------------------------
namespace cnMac{
//---------------------------------------------------------------------------
class cGATTTunnelConectionDevice : public iReference, public iAddress, protected iGATTServiceHandler
{
public:
	cnLibOC_INTERFACE_DEFINE(cGATTTunnelConectionDevice,iAddress)

	cGATTTunnelConectionDevice(iPtr<iGATTService> Service,iPtr<iGATTCharacteristic> ReadChar,iPtr<iGATTCharacteristic> WriteChar);
	~cGATTTunnelConectionDevice();

	// iAddress

	virtual eOrdering cnLib_FUNC Compare(const iAddress *_Nullable Dest)const override;

	iGATTService*_Nullable GetService(void)const;
	iGATTCharacteristic*_Nullable GetReadCharacteristic(void)const;
	iGATTCharacteristic*_Nullable GetWriteCharacteristic(void)const;

	class cRWQueue : public cnRTL::bcBufferedRWQueue, public cnRTL::cDualReference
	{
	public:
		cRWQueue(rPtr<cGATTTunnelConectionDevice> Device);
		~cRWQueue();

		cGATTTunnelConectionDevice*_Nonnull GetDevice(void)const;

		using bcBufferedRWQueue::WriteQueueSetEndMode;

	protected:
		void VirtualStarted(void);
		void VirtualStopped(void);


		rPtr<cGATTTunnelConectionDevice> fDevice;
		//void BLEWriteProcedure(void);

		virtual iReference*_Nonnull RWQueueInnerReference(void)override;
		virtual void ReadBufferNotify(void)override;
		virtual void WriteDataNotify(void)override;
	};
	class iConnectCallback
	{
	public:
		virtual void TunnelDeviceOnConnectDone(rPtr<cRWQueue> RWQueue)=0;
		//virtual void TunnelDeviceOnConnectFailed(void)=0;
	};
	bool Connect(iConnectCallback *_Nonnull Callback);
protected:
	iPtr<iGATTService> fService;
	iPtr<iGATTCharacteristic> fReadChar;
	iPtr<iGATTCharacteristic> fWriteChar;
	iPtr<iGATTDescriptor> fReadNotifyDescriptor;

	siPOSIX::c_pthread_mutex fConnectProcedureMutex;
	class cConnectProcedure
	{
	public:
		operator bool ()const;

		void Start(iConnectCallback *_Nonnull Callback);

	protected:
	}fConnectProcedure;

	virtual void cnLib_FUNC GATTServiceStateChanged(void)override;
	virtual void cnLib_FUNC GATTServiceCharacteristListChanged(void)override;

	cnRTL::rInnerPtr<cRWQueue> fRWQueue;
	void RWQueueNotifyWrite(void);

	// connect procedure
	enum{
		csIdle,
		csWaitConnectPerpheral,
		csCheckService,
		csWaitNotifyDescriptor,
		csWaitRegisterNotify,
		csConnected,
	}fConnectState;
	iConnectCallback *_Nullable fConnectCallback=nullptr;

	void UpdateFunctionState(void);
	void ConnectProcSucceed(void);
	void ConnectProcFailed(void);

	class cReadCharHandler : public iGATTCharacteristHandler
	{
	public:
		virtual void cnLib_FUNC GATTCharacteristStateChanged(void)override;
		virtual void cnLib_FUNC GATTCharacteristDescriptorListChanged(void)override;
		virtual void cnLib_FUNC GATTCharacteristValueNotify(void)override;
	}fReadCharHandler;
	class cWriteCharHandler : public iGATTCharacteristHandler
	{
	public:
		virtual void cnLib_FUNC GATTCharacteristStateChanged(void)override;
		virtual void cnLib_FUNC GATTCharacteristDescriptorListChanged(void)override;
		virtual void cnLib_FUNC GATTCharacteristValueNotify(void)override;
	}fWriteCharHandler;

	void ReadCharValueNotify(void);
};
//---------------------------------------------------------------------------
class cGATTTunnelConectionConnector : public iConnectionConnector
{
public:
	cGATTTunnelConectionConnector(iTypeID ConnectionIID);
	~cGATTTunnelConectionConnector();

	virtual iAddress*_Nullable		cnLib_FUNC GetLocalAddress(void)override;
	virtual iPtr<iConnection>		cnLib_FUNC Connect(iAddress *_Nullable RemoteAddress)override;
	virtual iPtr<iConnectionTask>	cnLib_FUNC ConnectAsync(iAddress *_Nullable RemoteAddress)override;

protected:
	iTypeID fConnectionIID;


	class cEndpoint : public iConnection, public iEndpoint
	{
	public:
		cEndpoint(rPtr<cGATTTunnelConectionDevice::cRWQueue> RWQueue);
		~cEndpoint();

		virtual const void*_Nullable cnLib_FUNC CastInterface(iTypeID IID)const override;

		// iConnection

		virtual iAddress*_Nullable cnLib_FUNC GetLocalAddress(void)override;
		virtual iAddress*_Nullable cnLib_FUNC GetRemoteAddress(void)override;

		// iEndpoint

		virtual void cnLib_FUNC Close(void)override;
		virtual iReadQueue*_Nonnull cnLib_FUNC GetReadQueue(void)override;
		virtual iWriteQueue*_Nonnull cnLib_FUNC GetWriteQueue(void)override;
		virtual void cnLib_FUNC SetWriteEndMode(eEndpointWriteEndMode EndMode)override;

	protected:
		rPtr<cGATTTunnelConectionDevice::cRWQueue> fRWQueue;
	};

	static iPtr<iConnection> CreateConnection(iTypeID ConnectionIID,rPtr<cGATTTunnelConectionDevice::cRWQueue> RWQueue);
	class cConnectSyncObject : public cnRTL::cThreadOneTimeNotifier, public cGATTTunnelConectionDevice::iConnectCallback
	{
	public:
		iTypeID ConnectionIID;
		iPtr<iConnection> Connection;
	protected:
		virtual void TunnelDeviceOnConnectDone(rPtr<cGATTTunnelConectionDevice::cRWQueue> RWQueue)override;

	};
	class cConnectAsyncTask : public iReference,public iConnectionTask, public cGATTTunnelConectionDevice::iConnectCallback
	{
	public:
		virtual bool cnLib_FUNC IsDone(void)override;
		virtual bool cnLib_FUNC SetNotify(iProcedure *_Nonnull NotifyProcedure)override;
		
		virtual void cnLib_FUNC Cancel(void)override;
		virtual iConnection*_Nullable cnLib_FUNC GetConnection(void)override;

		//eStreamError cnLib_FUNC GetStreamError(void)override;
		//StreamError AcceptErrorCode;

		iTypeID ConnectionIID;
		cnRTL::cAsyncTaskState TaskState;

		void ConnectStart(void);
		void ConnectCancel(void);
	protected:
		iPtr<iConnection> fConnection;

		virtual void TunnelDeviceOnConnectDone(rPtr<cGATTTunnelConectionDevice::cRWQueue> RWQueue)override;

	};
};
//---------------------------------------------------------------------------
class cGATTTunnelConectionObserver : public iGATTTunnelConnectionDeviceObserver, public cnRTL::bcAsyncSignal, public cnRTL::cDualReference, protected iAsyncNotificationCallback
{
public:
	cGATTTunnelConectionObserver(iGATTPeripheralObserver *Observer);
	~cGATTTunnelConectionObserver();

	cnRTL::cSeqList<cGATTTunnelConnectionDeviceID> DeviceIDs;

	virtual bool cnLib_FUNC StartNotify(iReference *_Nullable Reference,iAsyncNotificationCallback *_Nonnull Callback)override;
	virtual void cnLib_FUNC StopNotify(void)override;
	virtual void cnLib_FUNC NotifyCallback(bool IdleNotify)override;
	virtual rPtr<iGATTTunnelConnectionEnumerator> cnLib_FUNC EnumAllDevices(void)override;
	virtual rPtr<iGATTTunnelConnectionEnumerator> cnLib_FUNC FetchDeviceChanges(void)override;
	virtual void cnLib_FUNC DiscardChanges(void)override;

protected:
	void VirtualStarted(void);
	void VirtualStopped(void);

	virtual iReference*_Nonnull AsyncSignalInnerReference(void)override;
	virtual void AsyncSignalStarted(void)override;
	virtual void AsyncSignalStopped(void)override;
	virtual Availability AsyncSignalAvailable(void)override;
	virtual void AsyncSignalNotify(void)override;
	virtual void AsyncSignalClosed(void)override;

	rPtr<iGATTPeripheralObserver> fObserver;
	iAsyncNotificationCallback *fCallback;
	rPtr<iReference> fCallbackReference;

	bool fPeripheralObserverError;
	bool fPeripheralObserverNotified;

	// iAsyncNotificationCallback

	virtual void cnLib_FUNC AsyncStarted(void)override;
	virtual void cnLib_FUNC AsyncStopped(void)override;
	virtual void cnLib_FUNC AsyncNotify(void)override;

	struct cDeviceItem : cnRTL::cRTLAllocator
	{
		cDeviceItem *_Nullable Next;
		iPtr<cGATTTunnelConectionDevice> Device;
	};
	cnRTL::cGlobalLinkItemRecycler<cDeviceItem> fDeviceItemRecycler;
	cnRTL::cAtomicQueueS<cDeviceItem> fDeviceQueue;

	virtual void cnLib_FUNC GATTPeripheralListChanged(void)override;
};
//---------------------------------------------------------------------------
class cGATTTunnelConectionProtocol : public iGATTTunnelConnectionProtocol
{
public:
	cGATTTunnelConectionProtocol(rPtr<iGATTPeripheralCentral> Central,rPtr<iGATTPeripheralDevice> Device);
	~cGATTTunnelConectionProtocol();

	// iConnectionProtocol

	virtual iPtr<iConnectionConnector>	cnLib_FUNC CreateStreamConnector(iAddress *_Nullable LocalAddress,iAddress *_Nullable RemoteAddress)override;
	virtual iPtr<iConnectionListener>	cnLib_FUNC CreateStreamListener(iAddress *_Nullable LocalAddress)override;
	virtual iPtr<iConnectionQueue>		cnLib_FUNC CreateStreamConnectionQueue(iAddress *_Nullable LocalAddress)override;

	virtual iPtr<iConnectionConnector>	cnLib_FUNC CreateEndpointConnector(iAddress *_Nullable LocalAddress,iAddress *_Nullable RemoteAddress)override;
	virtual iPtr<iConnectionListener>	cnLib_FUNC CreateEndpointListener(iAddress *_Nullable LocalAddress)override;
	virtual iPtr<iConnectionQueue>		cnLib_FUNC CreateEndpointConnectionQueue(iAddress *_Nullable LocalAddress)override;

	// iGATTTunnelConnectionProtocol

	virtual iPtr<iGATTTunnelConnectionBrowser> cnLib_FUNC CreateBrowser(const cGATTTunnelConnectionDeviceID *_Nonnull ServiceIDs,uIntn ServiceCount)override;

protected:

	rPtr<iGATTPeripheralCentral> fCentral;
	rPtr<iGATTPeripheralDevice> fDevice;

	iPtr<cGATTTunnelConectionConnector> fEndpointConnector;
	iPtr<cGATTTunnelConectionConnector> fStreamConnector;
};
//---------------------------------------------------------------------------
}	// namespace cnMac
//---------------------------------------------------------------------------
}	// namespace cnLibrary
//---------------------------------------------------------------------------
#ifdef	__OBJC__
//---------------------------------------------------------------------------
@interface cnLib_BluetoothLEConectionPeripheral : NSObject<CBPeripheralDelegate>

@property(nonatomic,readonly) CBPeripheral *_Nonnull peripheral;

@end

@interface cnLib_BluetoothLEPeripheralContext : NSObject
@end

@interface cnLib_BluetoothLECentralContext : NSObject
@end

//---------------------------------------------------------------------------
#endif	// __OBJC__
//---------------------------------------------------------------------------
namespace cnLibrary{
//---------------------------------------------------------------------------
namespace cnMac{
//---------------------------------------------------------------------------
class cBLECharacteristic;
class cBLEService;
class cBLEPeripheral;
class cBLEPeripheralCentral;
//---------------------------------------------------------------------------
class API_AVAILABLE(macos(10.7)) cBLEDescriptor : public iGATTDescriptor
{
public:
	cBLEDescriptor(cBLECharacteristic *_Nonnull Host);
	~cBLEDescriptor();

	virtual iDispatch*_Nonnull cnLib_FUNC GetHandlerDispatch(void)override;
	virtual bool cnLib_FUNC InsertHandler(iGATTDescriptorHandler *_Nullable Handler)override;
	virtual bool cnLib_FUNC RemoveHandler(iGATTDescriptorHandler *_Nullable Handler)override;

	virtual eGATTFunctionState cnLib_FUNC GetFunctionState(void)override;
	virtual iGATTCharacteristic*_Nonnull cnLib_FUNC GetCharacterist(void)override;

protected:
	rPtr<cBLECharacteristic> fHost;
	cBLEPeripheral *_Nonnull fPeripheral;
};
//---------------------------------------------------------------------------
class API_AVAILABLE(macos(10.7)) cBLECharacteristic : public iGATTCharacteristic
{
public:
	cBLECharacteristic(cBLEService *_Nonnull Host);
	~cBLECharacteristic();

	cBLEPeripheral*_Nonnull GetHostPeripheral(void);

	virtual iDispatch*_Nonnull cnLib_FUNC GetHandlerDispatch(void)override;
	virtual bool cnLib_FUNC InsertHandler(iGATTCharacteristHandler *_Nullable Handler)override;
	virtual bool cnLib_FUNC RemoveHandler(iGATTCharacteristHandler *_Nullable Handler)override;

	virtual eGATTFunctionState cnLib_FUNC GetFunctionState(void)override;
	virtual iGATTService*_Nonnull cnLib_FUNC GetService(void)override;
	virtual rPtr<iGATTDescriptor> cnLib_FUNC PersistDescriptor(const cGATTUUID &ID)override;
	virtual void cnLib_FUNC SetScanForDescriptors(bool AllowScan)override;

	virtual rPtr< iArrayReference<const void> > cnLib_FUNC GetValue(void)override;

	virtual iPtr<iAsyncTask> cnLib_FUNC Write(const void *_Nonnull Data,uIntn DataSize)override;
	virtual bool cnLib_FUNC WriteWithoutResponse(const void *_Nonnull Data,uIntn DataSize)override;

protected:

	rPtr<cBLEService> fHost;
	cBLEPeripheral *_Nonnull fPeripheral;

};
//---------------------------------------------------------------------------
class API_AVAILABLE(macos(10.7)) cBLEService : public iGATTService
{
public:
	cBLEService(cBLEPeripheral *_Nonnull Peripheral);
	~cBLEService();

	cBLEPeripheral*_Nonnull GetHostPeripheral(void);

	virtual iDispatch*_Nonnull cnLib_FUNC GetHandlerDispatch(void)override;
	virtual bool cnLib_FUNC InsertHandler(iGATTServiceHandler *_Nullable Handler)override;
	virtual bool cnLib_FUNC RemoveHandler(iGATTServiceHandler *_Nullable Handler)override;

	virtual eGATTFunctionState cnLib_FUNC GetFunctionState(void)override;
	virtual iGATTPeripheral*_Nonnull cnLib_FUNC GetPeripheral(void)override;
	virtual rPtr<iGATTCharacteristic> cnLib_FUNC PersistCharacteristic(const cGATTUUID &ID)override;
	virtual rPtr< iArrayReference< rPtr<iGATTCharacteristic> > > cnLib_FUNC QueryCharacteristics(void)override;

	virtual void cnLib_FUNC SetScanForCharacters(bool AllowScan)override;
protected:
	rPtr<cBLEPeripheral> fPeripheral;
};
//---------------------------------------------------------------------------
class API_AVAILABLE(macos(10.7)) cBLEPeripheral : public iGATTPeripheral
{
public:
	cBLEPeripheral(cnLib_BluetoothLEPeripheralContext *_Nonnull Context);
	~cBLEPeripheral();

	void InitializePeripheral(cBLEPeripheralCentral *Central,CBPeripheral *Peripheral);

	virtual void cnLib_FUNC IncReference(void)override;
	virtual void cnLib_FUNC DecReference(void)override;

	virtual iDispatch*_Nonnull cnLib_FUNC GetHandlerDispatch(void)override;
	virtual bool cnLib_FUNC InsertHandler(iGATTPeripheralHandler *_Nullable Handler)override;
	virtual bool cnLib_FUNC RemoveHandler(iGATTPeripheralHandler *_Nullable Handler)override;

	virtual eGATTFunctionState cnLib_FUNC GetFunctionState(void)override;
	virtual iGATTPeripheralCentral*_Nonnull cnLib_FUNC GetPeripheralCentral(void)override;
	virtual rPtr<iGATTService> cnLib_FUNC PersistService(const cGATTUUID &ID)override;
	virtual rPtr< iArrayReference< rPtr<iGATTService> > > cnLib_FUNC QueryServices(void)override;

	virtual void cnLib_FUNC SetScanForServices(bool AllowScan)override;

	virtual bool cnLib_FUNC IsConnecting(void)override;
	virtual void cnLib_FUNC Connect(void)override;
	virtual void cnLib_FUNC Disconnect(void)override;


	// CentralManager delegate

	void CentralDidConnect(void);
	void CentralDidFailToConnect(NSError *_Nonnull Error);
	void CentralDidDisconnect(NSError *_Nullable Error);

	// Peripheral delegate

	void PeripheralDidDiscoverService(NSError *_Nullable Error);
	void PeripheralDiscoverCharacteristicsForService(CBService *_Nonnull Service,NSError *_Nullable Error);
	void PeripheralDidUpdateValueForCharacteristic(CBCharacteristic *_Nonnull Characteristic,NSError *_Nullable Error);
	void PeripheralDidWriteValueForCharacteristic(CBCharacteristic *_Nonnull Characteristic,NSError *_Nullable Error);

protected:
	cnLib_BluetoothLEPeripheralContext *_Nonnull fContext;
	rPtr<cBLEPeripheralCentral> fHost;
	CBPeripheral *_Nullable fPeripheral;

	//cnRTL::cSeqMap< cGATTUUID,cBLEService*,cnDataStruct::cRAWDataItemOrderOperator<cGATTUUID> > fPersistServices;
};
//---------------------------------------------------------------------------
class API_AVAILABLE(macos(10.7)) cBLEPeripheralCentral : public iGATTPeripheralCentral
{
public:
	cBLEPeripheralCentral(cnLib_BluetoothLECentralContext *_Nonnull Context);
	~cBLEPeripheralCentral();

	iDispatch*_Nullable GetDispatch(void);
	bool SetupCentralManager(iDispatch *_Nonnull Dispatch,NSDictionary *_Nullable Options);

	virtual void cnLib_FUNC IncReference(void)override;
	virtual void cnLib_FUNC DecReference(void)override;

	virtual iDispatch*_Nullable cnLib_FUNC GetHandlerDispatch(void)override;
	virtual bool cnLib_FUNC InsertHandler(iGATTPeripheralCentralHandler *_Nullable Handler)override;
	virtual bool cnLib_FUNC RemoveHandler(iGATTPeripheralCentralHandler *_Nullable Handler)override;

	virtual rPtr<iGATTPeripheral> cnLib_FUNC PersistPeripheral(void)override;
	virtual rPtr<iGATTPeripheralQuery> cnLib_FUNC CreatePeripheralQuery(const cGATTUUID *Service,uIntn ServiceCount)override;
	virtual bool cnLib_FUNC ResetPeripheralCache(void)override;

	virtual bool cnLib_FUNC IsEnabled(void)override;
	virtual bool cnLib_FUNC IsPowerOn(void)override;

	// CentralManager delegate

	void DidUpdateState(void);
	void DidDiscoverPeripheral(CBPeripheral *_Nonnull Peripheral,NSDictionary *_Nullable AdvertisementData,NSNumber *_Nullable RSSI);

protected:
	cnLib_BluetoothLECentralContext *_Nonnull fContext;
	CBCentralManager *_Nullable fCentralManager;
	iPtr<iDispatch> fDispatch;

	template<class TCreator>
	bool SetupCentralManagerT(iDispatch *_Nonnull Dispatch,TCreator &&Creator);
};
//---------------------------------------------------------------------------
cBLEPeripheral*_Nonnull cnLib_BluetoothLEPeripheralContext_GetPeripheral(cnLib_BluetoothLEPeripheralContext *_Nonnull Context);
cBLEPeripheralCentral*_Nonnull cnLib_BluetoothLECentralContext_GetCentral(cnLib_BluetoothLECentralContext *_Nonnull Context);

#pragma mark - deprecated

#if	0
//---------------------------------------------------------------------------
class cBluetoothLEConectionDeviceEndpoint : public iConnection, public iEndpoint, public cnRTL::bcRWQueue
{
public:
	class cnLib_INTERFACE iCallback
	{
	public:
		virtual void BluetoothLENotifyWrite(void)=0;
	};
	iCallback *_Nullable Callback=nullptr;
	cBluetoothLEConectionDeviceEndpoint();
	~cBluetoothLEConectionDeviceEndpoint();

	virtual const void*_Nullable cnLib_FUNC CastInterface(iTypeID IID)const override;

	virtual void cnLib_FUNC Close(void)override;
	virtual iReadQueue*_Nonnull cnLib_FUNC GetReadQueue(void)override;
	virtual iWriteQueue*_Nonnull cnLib_FUNC GetWriteQueue(void)override;
	virtual void cnLib_FUNC SetWriteEndMode(eEndpointWriteEndMode EndMode)override;

	virtual iAddress*_Nullable	cnLib_FUNC GetLocalAddress(void)override;
	virtual iAddress*_Nullable	cnLib_FUNC GetRemoteAddress(void)override;

	void PushData(const void *_Nullable Data,uIntn Size);
	cMemory PullData(uIntn QuerySize);
	void AdvancePullData(uIntn Size);
protected:

	virtual cConstMemory cnLib_FUNC GatherReadBuffer(uIntn Size)override;
	virtual uIntn cnLib_FUNC DismissReadBuffer(uIntn Size)override;
	virtual void ReadQueueClosed(void)override;

	virtual cMemory cnLib_FUNC ReserveWriteBuffer(uIntn Size)override;
	virtual uIntn cnLib_FUNC CommitWriteBuffer(uIntn Size)override;
	virtual void WriteQueueClosed(void)override;


	cnRTL::cAsyncLoopbackStreamBuffer fReadDataQueue;
	cnRTL::cAsyncLoopbackStreamBuffer fWriteDataQueue;
};
//---------------------------------------------------------------------------
#ifdef	__OBJC__
//---------------------------------------------------------------------------
class cBluetoothLEConectionService : public iAddress, public iOCObject,public iReference
{
public:
	cBluetoothLEConectionService(cnLib_BluetoothLEConnectionService *_Nonnull Service);
	~cBluetoothLEConectionService();

	virtual const void*_Nullable cnLib_FUNC CastInterface(iTypeID InterfaceID)const override;
	virtual void cnLib_FUNC IncReference(void)override;
	virtual void cnLib_FUNC DecReference(void)override;

	virtual eOrdering cnLib_FUNC Compare(const iAddress *_Nullable Dest)const override;

	virtual id _Nonnull GetOCObject(void)override;

protected:
	cnLib_BluetoothLEConnectionService *_Nonnull fService;
};
//---------------------------------------------------------------------------
struct cUUID
{
	cUUID()=default;
	cUUID(const cUUID&)=default;
	//cUUID(cUUID&&)=default;
	cUUID(const tUUID &p){
		cnMemory::Copy(Data,p,sizeof(tUUID));
	}
	cUUID(const void *_Nonnull p){
		cnMemory::Copy(Data,p,sizeof(tUUID));
	}
	tUUID Data;
};
template<class T>
class cBinarySortable : public T
{
public:
	using T::T;
	cBinarySortable(const T&Src):T(Src){}
	cBinarySortable(T&&Src):T(static_cast<T&&>(Src)){}

	bool operator ==(const T &Src)const{
		return 0==cnMemory::Compare(static_cast<const T*>(this),cnMemory::AddressOf(Src),sizeof(T));
	}
	bool operator !=(const T &Src)const{
		return 0!=cnMemory::Compare(static_cast<const T*>(this),cnMemory::AddressOf(Src),sizeof(T));
	}
	bool operator <(const T &Src)const{
		return 0>cnMemory::Compare(static_cast<const T*>(this),cnMemory::AddressOf(Src),sizeof(T));
	}
	bool operator <=(const T &Src)const{
		return 0>=cnMemory::Compare(static_cast<const T*>(this),cnMemory::AddressOf(Src),sizeof(T));
	}
	bool operator >(const T &Src)const{
		return 0<cnMemory::Compare(static_cast<const T*>(this),cnMemory::AddressOf(Src),sizeof(T));
	}
	bool operator >=(const T &Src)const{
		return 0<=cnMemory::Compare(static_cast<const T*>(this),cnMemory::AddressOf(Src),sizeof(T));
	}

	template<class TElement>
	bool operator ==(const TElement (&Src)[sizeof(T)/sizeof(TElement)])const{
		return 0==cnMemory::Compare(static_cast<const T*>(this),cnMemory::AddressOf(Src),sizeof(T));
	}
	template<class TElement>
	bool operator !=(const TElement (&Src)[sizeof(T)/sizeof(TElement)])const{
		return 0!=cnMemory::Compare(static_cast<const T*>(this),cnMemory::AddressOf(Src),sizeof(T));
	}
	template<class TElement>
	bool operator <(const TElement (&Src)[sizeof(T)/sizeof(TElement)])const{
		return 0>cnMemory::Compare(static_cast<const T*>(this),cnMemory::AddressOf(Src),sizeof(T));
	}
	template<class TElement>
	bool operator <=(const TElement (&Src)[sizeof(T)/sizeof(TElement)])const{
		return 0>=cnMemory::Compare(static_cast<const T*>(this),cnMemory::AddressOf(Src),sizeof(T));
	}
	template<class TElement>
	bool operator >(const TElement (&Src)[sizeof(T)/sizeof(TElement)])const{
		return 0<cnMemory::Compare(static_cast<const T*>(this),cnMemory::AddressOf(Src),sizeof(T));
	}
	template<class TElement>
	bool operator >=(const TElement (&Src)[sizeof(T)/sizeof(TElement)])const{
		return 0<=cnMemory::Compare(static_cast<const T*>(this),cnMemory::AddressOf(Src),sizeof(T));
	}
};
//---------------------------------------------------------------------------
class cBluetoothLEConectionDevice : public iAddress
{
public:

	cnLib_INTERFACE_DEFINE(cBluetoothLEConectionDevice,iAddress)

	cBluetoothLEConectionDevice(cnLib_BluetoothLEConectionPeripheral *_Nonnull Peripheral,CBService *_Nonnull Service,CBCharacteristic *_Nonnull ReadChar,CBCharacteristic *_Nonnull WriteChar);
	~cBluetoothLEConectionDevice();

	iPtr<iConnectionTask> ConnectAsync(iTypeID ConnectionIID);

	virtual eOrdering cnLib_FUNC Compare(const iAddress *_Nullable Dest)const override;

	void onDeviceChecked(void);
	void onDeviceRegisterNotification(bool Error);
	void onUpdateCharacteristicValue(void);
protected:
	cnLib_BluetoothLEConectionPeripheral *_Nonnull fPeripheral;
	void BLEWriteProcedure(void);

	CBService *_Nonnull fService;
	CBCharacteristic *_Nonnull fReadChar;
	CBCharacteristic *_Nonnull fWriteChar;

	class cConnectTask : public iConnectionTask
	{
	public:
		cConnectTask()=default;
		~cConnectTask()=default;
		iPtr<cBluetoothLEConectionDevice> Owner;

		virtual bool cnLib_FUNC IsDone(void)override;
		virtual bool cnLib_FUNC SetNotify(iProcedure *_Nonnull NotifyProcedure)override;
		virtual void cnLib_FUNC Cancel(void)override;
		virtual iConnection*_Nullable cnLib_FUNC GetConnection(void)override;

		//eStreamError cnLib_FUNC GetStreamError(void)override;
		StreamError AcceptErrorCode;

		iPtr<iConnection> Connection;
		iTypeID ConnectionTypeID;
		cnRTL::cAsyncTaskState TaskState;

		void CreateConnection(iEndpoint *_Nonnull Endpoint);
	};
	iPtr<cConnectTask> fConnectTask;


	class cDeviceEndpoint : public cnRTL::cBufferedExtEndpoint
	{
	public:
		cDeviceEndpoint();
		~cDeviceEndpoint();

		iPtr<cBluetoothLEConectionDevice> Owner;

	protected:

		virtual iReference*_Nonnull RWQueueInnerReference(void)override;

		virtual uIntn cnLib_FUNC DismissReadBuffer(uIntn Size)override;
		virtual uIntn cnLib_FUNC CommitWriteBuffer(uIntn Size)override;
	};

	iPtr<cDeviceEndpoint> fEndpoint;
	void EndpointNotifyRead(void);
	void EndpointNotifyWrite(void);
};
//---------------------------------------------------------------------------
struct cBluetoothLEConnectionServiceIDItem
{
	cBluetoothLEConnectionServiceID ID;
	ocPtr<CBUUID> ServiceID;
	ocPtr<CBUUID> ReadCharID;
	ocPtr<CBUUID> WriteCharID;
};
//---------------------------------------------------------------------------
class cBluetoothLEConnectionDeviceBrowser : public iBluetoothLEConnectionBrowser, public cnRTL::bcAsyncSignal, public cnRTL::cDualReference
{
public:
	cBluetoothLEConnectionDeviceBrowser(cnLib_BluetoothLEConnectionProtocol *_Nonnull Protocol,const cBluetoothLEConnectionServiceID *_Nonnull Services,uIntn ServiceCount);
	~cBluetoothLEConnectionDeviceBrowser();

	cArray< const cnRTL::rClsRef<cBluetoothLEConnectionServiceIDItem> > GetServiceIDs(void)const;

	void BLEOnDeviceChecked(cnLib_BluetoothLEConectionPeripheral *_Nonnull ConnectionPeripheral);

	virtual iAddress*_Nonnull	cnLib_FUNC GetLocalAddress(void)override;

	virtual bool cnLib_FUNC StartNotify(iReference *_Nullable Reference,iAsyncNotificationCallback *_Nonnull Callback)override;
	virtual void cnLib_FUNC StopNotify(void)override;
	virtual void cnLib_FUNC NotifyCallback(bool IdleNotify)override;
	virtual iPtr<iAddress> cnLib_FUNC Fetch(void)override;
	virtual bool cnLib_FUNC IsClosed(void)override;

	virtual bool cnLib_FUNC FetchBLEDevice(cBluetoothLEDeviceInfo &DeviceInfo)override;

protected:
	void VirtualStopped(void);

	virtual iReference*_Nonnull AsyncSignalInnerReference(void)override;
	virtual void AsyncSignalStarted(void)override;
	virtual void AsyncSignalStopped(void)override;
	virtual Availability AsyncSignalAvailable(void)override;
	virtual void AsyncSignalNotify(void)override;
	virtual void AsyncSignalClosed(void)override;

	cnLib_BluetoothLEConnectionProtocol *_Nonnull fProtocol;
	iAsyncNotificationCallback *_Nullable fCallback;
	rPtr<iReference> fCallbackReference;

	struct cDeviceItem : cnRTL::cRTLAllocator
	{
		cDeviceItem *_Nullable Next;
		iPtr<cBluetoothLEConectionDevice> Device;
		ocPtr<cnLib_BluetoothLEConectionPeripheral> Peripheral;
	};
	cnRTL::cGlobalLinkItemRecycler<cDeviceItem> fDeviceItemRecycler;
	cnRTL::cAtomicQueueS<cDeviceItem> fDeviceQueue;


	cnRTL::cSeqList< cnRTL::rClsRef<cBluetoothLEConnectionServiceIDItem> > fServiceIDs;

};
//---------------------------------------------------------------------------
class cBluetoothLEConectionPeripheral : public iOCObject, public iReference
{
public:
	cBluetoothLEConectionPeripheral(cnLib_BluetoothLEConectionPeripheral *_Nonnull ConnectionPeripheral);
	~cBluetoothLEConectionPeripheral();

	void Initialize(cnLib_BluetoothLEConnectionProtocol *_Nonnull Protocol,CBPeripheral *_Nonnull peripheral);

	cnLib_BluetoothLEConnectionProtocol*_Nonnull GetProtocol(void)const;
	CBPeripheral*_Nonnull GetPeripheral(void)const;

	CBService*_Nullable GetServiceByUUID(const void *_Nonnull ID);
	CBCharacteristic*_Nullable GetCharacteristByUUID(const void *_Nonnull ServiceID,const void *_Nonnull CharacterID);

	cnRTL::cString<uChar16> GetName(void)const;

	virtual const void*_Nullable cnLib_FUNC CastInterface(iTypeID InterfaceID)const override;
	virtual void cnLib_FUNC IncReference(void)override;
	virtual void cnLib_FUNC DecReference(void)override;

	virtual id _Nonnull GetOCObject(void)override;

	void BLEOnConnected(void);
	void BLEOnConnectFailed(void);
	void BLEOnDisconnected(void);
	void BLEUpdateRSSI(NSNumber *_Nonnull Value);
	void BLEUpdateADData(NSDictionary *_Nonnull ADData);

	void ocOnDiscoveredService(void);
	void ocOnErrorDiscoverService(NSError *_Nullable Error);
	void ocOnDiscoveredCharacteristic(CBService *_Nonnull Service);
	void ocOnErrorDiscoverCharacteristic(CBService *_Nonnull Service,NSError *_Nonnull Error);
	void ocOnUpdateCharacteristValue(CBCharacteristic *_Nonnull Characteristic);
	void ocOnUpdateNotificationState(CBCharacteristic *_Nonnull Characteristic,NSError *_Nonnull Error);

	void BLEDeviceActive(cBluetoothLEConectionDevice *_Nonnull Device);
	void BLEDeviceInactive(cBluetoothLEConectionDevice *_Nonnull Device);

	void BLEDeviceConnect(CBCharacteristic *_Nonnull ReadCharacteristic,cBluetoothLEConectionDevice *_Nonnull Device);
	void BLEDeviceDisconnect(CBCharacteristic *_Nonnull ReadCharacteristic,cBluetoothLEConectionDevice *_Nonnull Device);
protected:
	cnLib_BluetoothLEConnectionProtocol *_Nonnull fProtocol;
	CBPeripheral *_Nonnull fPeripheral;
	cnLib_BluetoothLEConectionPeripheral *_Nonnull fConnectionPeripheral;


	cnRTL::cString<uChar16> fName;
	sfInt16 fRSSI;

	bool fActive=false;
	bool fNeedScanService=false;
	bool fPeripheralCommandInProgress=false;
	bool fConnected=false;
	bool fServiceChecked=false;
	ufInt16 fCharacteriscCheckedIndex=0;

	cnRTL::cSeqSet<cBluetoothLEConectionDevice*> fActiveDevice;
	cnRTL::cSeqMap<CBCharacteristic*,cBluetoothLEConectionDevice*> fConnectDeviceMap;
	cnRTL::cExclusiveFlag fStateProcExclusiveFlag;

	void NotifyPeripheralStateProcedure(void);

	void PeripheralStateProcedure(void);

	void PeripheralStateProcedure_Active(void);
	void PeripheralStateProcedure_Inactive(void);
};
//---------------------------------------------------------------------------
class API_AVAILABLE(macos(10.9)) cBluetoothLEConnectionQueue : public cnRTL::bcConnectionQueue, public cnRTL::cDualReference
{
public:
	cBluetoothLEConnectionQueue(cnLib_BluetoothLEConnectionProtocol *_Nonnull Owner,cnLib_BluetoothLEConnectionService *_Nonnull Service);
	~cBluetoothLEConnectionQueue();

	cnLib_BluetoothLEConnectionService *_Nonnull GetService(void)const;
	void OnAddServiceError(NSError *_Nonnull _NonnullError);
	void OnPeripheralClosed(void);
	void OnCentralSubscribe(CBCentral *_Nonnull Central);
	void OnCentralUnsubscribe(CBCentral *_Nonnull Central);
	bool TestRequestWritable(CBATTRequest *_Nonnull Request);
	void OnCentralWrite(CBATTRequest *_Nonnull Request);
	void OnUpdateSubscribers(void);

	virtual iAddress*_Nonnull	cnLib_FUNC GetLocalAddress(void)override;
	virtual iPtr<iConnection> cnLib_FUNC FetchConnection(void)override;
protected:
	void VirtualStopped(void);

	ocPtr<cnLib_BluetoothLEConnectionProtocol> fOwner;
	ocPtr<cnLib_BluetoothLEConnectionService> fService;
	virtual iReference*_Nonnull AsyncSignalInnerReference(void)override;
	virtual void AsyncSignalStarted(void)override;
	virtual void AsyncSignalStopped(void)override;
	virtual Availability AsyncSignalAvailable(void)override;
	virtual void AsyncSignalClosed(void)override;

	class cCentralEndpoint : public cnRTL::cBufferedExtEndpoint
	{
	public:
		cCentralEndpoint();
		~cCentralEndpoint();

		iPtr<cBluetoothLEConnectionQueue> Owner;
		ocPtr<CBCentral> Central;
	protected:


		virtual iReference*_Nonnull RWQueueInnerReference(void)override;
		virtual uIntn cnLib_FUNC DismissReadBuffer(uIntn Size)override;
		virtual uIntn cnLib_FUNC CommitWriteBuffer(uIntn Size)override;
		//virtual void WriteQueueSetQueueEnd(bool StreamEnd)override;
	};
	cnRTL::cSeqMap< CBCentral*,iPtr<cCentralEndpoint> > fCentralMap;
	cCentralEndpoint*_Nonnull SetupCentral(CBCentral *_Nonnull Central);

	void EndpointNotifyRead(cCentralEndpoint *_Nonnull Endpoint);
	void EndpointNotifyWrite(cCentralEndpoint *_Nonnull Endpoint);
	struct cConnectionQueueItem
	{
		cConnectionQueueItem *_Nullable Next;
		iPtr<cCentralEndpoint> Endpoint;
	};
	cnRTL::cAtomicQueue<cConnectionQueueItem> fConnectionQueue;
	cnRTL::cGlobalLinkItemRecycler<cConnectionQueueItem> fConnectionQueueItemRecycler;
};
//---------------------------------------------------------------------------
class bcBluetoothLEConectionProtocol : public iConnectionProtocol, public iReference
{
public:
	bcBluetoothLEConectionProtocol(cnLib_BluetoothLEConnectionProtocol *_Nonnull Protocol);
	~bcBluetoothLEConectionProtocol();

	virtual const void*_Nullable cnLib_FUNC CastInterface(iTypeID TypeID)const override;

	virtual iPtr<iConnectionConnector>	cnLib_FUNC CreateStreamConnector(iAddress *_Nullable LocalAddress,iAddress *_Nullable RemoteAddress)override;
	virtual iPtr<iConnectionListener>	cnLib_FUNC CreateStreamListener(iAddress *_Nullable LocalAddress)override;
	virtual iPtr<iConnectionQueue>		cnLib_FUNC CreateStreamConnectionQueue(iAddress *_Nullable LocalAddress)override;

	virtual iPtr<iConnectionConnector>	cnLib_FUNC CreateEndpointConnector(iAddress *_Nullable LocalAddress,iAddress *_Nullable RemoteAddress)override;
	virtual iPtr<iConnectionListener>	cnLib_FUNC CreateEndpointListener(iAddress *_Nullable LocalAddress)override;
	virtual iPtr<iConnectionQueue>		cnLib_FUNC CreateEndpointConnectionQueue(iAddress *_Nullable LocalAddress)override;

	virtual	void cnLib_FUNC IncReference(void)override;
	virtual	void cnLib_FUNC DecReference(void)override;

protected:
	cnLib_BluetoothLEConnectionProtocol *_Nonnull fProtocol;
};
//---------------------------------------------------------------------------
class API_AVAILABLE(macos(10.7)) cBluetoothLEConectionProtocol_10_7 : public bcBluetoothLEConectionProtocol
{
public:
	cBluetoothLEConectionProtocol_10_7(cnLib_BluetoothLEConnectionProtocol *_Nonnull Protocol);
	~cBluetoothLEConectionProtocol_10_7();

	iPtr<iBluetoothLEConnectionBrowser> CreateBrowser(const cBluetoothLEConnectionServiceID *_Nonnull Services,uIntn ServiceCount);

	virtual iPtr<iConnectionConnector>	cnLib_FUNC CreateStreamConnector(iAddress *_Nullable LocalAddress,iAddress *_Nullable RemoteAddress)override;
	virtual iPtr<iConnectionConnector>	cnLib_FUNC CreateEndpointConnector(iAddress *_Nullable LocalAddress,iAddress *_Nullable RemoteAddress)override;

	void BLEOnCentralStateChanged(void);
	void BLEOnDiscovered(CBPeripheral *_Nonnull Peripheral,NSDictionary *_Nullable AdvertisementData,NSNumber *_Nullable RSSI);
	void BLEOnConnected(CBPeripheral *_Nonnull Peripheral);
	void BLEOnConnectFailed(CBPeripheral *_Nonnull Peripheral,NSError *_Nonnull Error);
	void BLEOnDisconnected(CBPeripheral *_Nonnull Peripheral,NSError *_Nonnull Error);


	void BrowserClosed(cBluetoothLEConnectionDeviceBrowser *_Nonnull Browser);
	void BrowserStarted(cBluetoothLEConnectionDeviceBrowser *_Nonnull Browser);
	void BrowserStopped(cBluetoothLEConnectionDeviceBrowser *_Nonnull Browser);

	void PerpheralChecked(cnLib_BluetoothLEConectionPeripheral *_Nonnull ConnectionPeripheral);
protected:

	cnRTL::cSeqSet< ocPtr<cnLib_BluetoothLEConectionPeripheral> > fPeripherals;

	cnRTL::cSeqSet< cnRTL::iInnerPtr<cBluetoothLEConnectionDeviceBrowser> > fBrowserSet;

	struct cServiceIDItem
	{
		uIntn Count=0;
		ocPtr<CBUUID> IDPtr;
	};
	cnRTL::cSeqMap<cBinarySortable<cUUID>,cServiceIDItem> fBrowserServiceIDMap;
	ocPtr<NSArray> fBrowserScanServiceArray;

	bool fBLECentralActivated;
	bool fNeedScanDevice;
	bool fScanStarted;
	void StartScan(void);
	void StopScan(void);
};
//---------------------------------------------------------------------------
class API_AVAILABLE(macos(10.9)) cBluetoothLEConectionProtocol_10_9 : public cBluetoothLEConectionProtocol_10_7
{
public:
	cBluetoothLEConectionProtocol_10_9(cnLib_BluetoothLEConnectionProtocol *_Nonnull Protocol);
	~cBluetoothLEConectionProtocol_10_9();

	virtual iPtr<iConnectionListener>	cnLib_FUNC CreateStreamListener(iAddress *_Nullable LocalAddress)override;
	virtual iPtr<iConnectionQueue>		cnLib_FUNC CreateStreamConnectionQueue(iAddress *_Nullable LocalAddress)override;
	virtual iPtr<iConnectionListener>	cnLib_FUNC CreateEndpointListener(iAddress *_Nullable LocalAddress)override;
	virtual iPtr<iConnectionQueue>		cnLib_FUNC CreateEndpointConnectionQueue(iAddress *_Nullable LocalAddress)override;

	void BLEOnPeripheralStateChanged(void);
	void BLEOnAddService(CBMutableService *_Nonnull Service,NSError *Error) API_AVAILABLE(macos(10.9));
	void BLEOnSubscribe(CBCentral *_Nonnull RemoteCentral,CBCharacteristic *_Nonnull Characteristic) API_AVAILABLE(macos(10.9));
	void BLEOnUnsubscribe(CBCentral *_Nonnull RemoteCentral,CBCharacteristic *_Nonnull Characteristic) API_AVAILABLE(macos(10.9));
	void BLEOnWriteRequests(NSArray<CBATTRequest *>*_Nonnull Requests) API_AVAILABLE(macos(10.9));
	void BLEOnUpdateSubscribers(void);

	void BLEOnUpdateAdvertisementData(void);
	
	void QueueListenStarted(cnRTL::iInnerPtr<cBluetoothLEConnectionQueue> Queue);
	void QueueListenStopped(cBluetoothLEConnectionQueue *_Nonnull Queue);
protected:

	ufInt16 fBLEPeripheralAdvertisementLockCount;
	bool fBLEPeripheralActivated;
	bool fBLENeedStartAdvertisement;
	void SetupPeripheral(void);
	void ClearPeripheral(void);
	void ResumeAdvertisement(void);
	void PauseAdvertisement(void);
	CBATTError TestRequestsWritable(NSArray<CBATTRequest *>*_Nonnull Requests);

	NSDictionary*_Nonnull CreateADData(void);

	cnRTL::cSeqMap< CBService*,cnRTL::iInnerPtr<cBluetoothLEConnectionQueue> > fServiceMap;

	cnRTL::cSeqSet<cBluetoothLEConnectionQueue*> fServiceAddSet;
};
#endif
//---------------------------------------------------------------------------
#endif	// __OBJC__
//---------------------------------------------------------------------------
}	// namespace cnMac
}	// namespace cnLibrary
//---------------------------------------------------------------------------
#endif	// __cplusplus
/*-------------------------------------------------------------------------*/
