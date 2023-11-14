/*- cnRTL - Windows - UPW -------------------------------------------------*/
/*         Developer : Code009                                             */
/*         Create on : 2021-01-12                                          */
/*-------------------------------------------------------------------------*/
#pragma once

#include <cnRTL\WinUWP.h>

//---------------------------------------------------------------------------
namespace cnLibrary{
//---------------------------------------------------------------------------
namespace cnRTL{
//---------------------------------------------------------------------------
namespace UWP{
//---------------------------------------------------------------------------
#if _WIN32_WINNT >= _WIN32_WINNT_WIN10
//---------------------------------------------------------------------------
class cGATTPeripheral;
class cGATTPeripheralCentral;
//---------------------------------------------------------------------------
class cBluetoothAddress : public iAddress
{
public:
	cnLib_INTERFACE_DEFINE(cBluetoothAddress,iAddress)

	virtual eiOrdering cnLib_FUNC Compare(iAddress *Dest)override;

	UINT64 Address;
};
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
class cGATTService : public iGATTService, public cDualReference
{
	typedef ABI::Windows::Devices::Bluetooth::GenericAttributeProfile::GattDeviceService BLEService;
	typedef ABI::Windows::Devices::Bluetooth::GenericAttributeProfile::IGattDeviceService IBLEService;
public:
	cGATTService(cGATTPeripheral *Peripheral);
	~cGATTService();

	virtual cGATTUUID cnLib_FUNC GetUUID(void)override;
	virtual iDispatch* cnLib_FUNC GetHandlerDispatch(void)override;
	virtual bool cnLib_FUNC InsertHandler(iGATTServiceHandler *Handler)override;
	virtual bool cnLib_FUNC RemoveHandler(iGATTServiceHandler *Handler)override;

	virtual eGATTFunctionState cnLib_FUNC GetFunctionState(void)override;
	virtual iGATTPeripheral* cnLib_FUNC GetPeripheral(void)override;
	virtual rPtr<iGATTCharacteristic> cnLib_FUNC PersistCharacteristic(const cGATTUUID &ID)override;
	virtual iPtr<iGATTCharacteristicAsyncFunction> cnLib_FUNC QueryCharacteristics(void)override;

	cGATTUUID ServiceUUID;
	void SetupService(COMPtr<IBLEService> Service);
	void ClearService(void);

	void PeripheralNotifyConnectionStatus(void);
protected:
	void VirtualStarted(void);
	void VirtualStopped(void);

	rPtr<cGATTPeripheral> fPeripheral;
	COMPtr<IBLEService> fService;
};
//---------------------------------------------------------------------------
class cGATTPeripheral : public iGATTPeripheral, public cDualReference
{
	typedef ABI::Windows::Devices::Bluetooth::BluetoothLEDevice BLEDevice;
	typedef ABI::Windows::Devices::Bluetooth::IBluetoothLEDevice IBLEDevice;
	typedef ABI::Windows::Devices::Bluetooth::IBluetoothLEDevice2 IBLEDevice2;
	typedef ABI::Windows::Devices::Bluetooth::IBluetoothLEDevice3 IBLEDevice3;
	typedef ABI::Windows::Foundation::IAsyncOperation<BLEDevice*> IBLEDeviceAsyncOp;
	typedef ABI::Windows::Foundation::IAsyncOperationCompletedHandler<BLEDevice*> IBLEDeviceAsyncOpCompletedHandler;
	typedef ABI::Windows::Devices::Bluetooth::GenericAttributeProfile::GattDeviceService BLEService;
	typedef ABI::Windows::Devices::Bluetooth::GenericAttributeProfile::IGattDeviceService IBLEService;

	typedef ABI::Windows::Devices::Bluetooth::GenericAttributeProfile::GattDeviceServicesResult GattDeviceServiceResult;
	typedef ABI::Windows::Devices::Bluetooth::GenericAttributeProfile::IGattDeviceServicesResult IGattDeviceServiceResult;
	typedef ABI::Windows::Foundation::IAsyncOperation<GattDeviceServiceResult*> IBLEGetServiceAsyncOp;
	typedef ABI::Windows::Foundation::IAsyncOperationCompletedHandler<GattDeviceServiceResult*> IBLEGetServiceAsyncOpCompletedHandler;
public:
	class bcConnector : public iReference
	{
	public:
		virtual void Setup(cGATTPeripheral *Owner)=0;
		virtual void Cancel(void)=0;
		virtual iAddress* GetAddress(void)=0;
		virtual rPtr< iArrayReference<const uChar16> > GetName(void)=0;
	};

	cGATTPeripheral(cGATTPeripheralCentral *Central,rPtr<bcConnector> Connector);
	~cGATTPeripheral();

	virtual iDispatch* cnLib_FUNC GetHandlerDispatch(void)override;
	virtual bool cnLib_FUNC InsertHandler(iGATTPeripheralHandler *Handler)override;
	virtual bool cnLib_FUNC RemoveHandler(iGATTPeripheralHandler *Handler)override;

	virtual iAddress* cnLib_FUNC GetPeripheralAddress(void)override;
	virtual eGATTFunctionState cnLib_FUNC GetFunctionState(void)override;
	virtual iGATTPeripheralCentral* cnLib_FUNC GetPeripheralCentral(void)override;
	virtual rPtr< iArrayReference<const uChar16> > cnLib_FUNC GetName(void)override;
	virtual rPtr<iGATTService> cnLib_FUNC AccessService(const cGATTUUID &ID)override;
	virtual iPtr<iGATTServiceAsyncFunction> cnLib_FUNC QueryServices(void)override;

	virtual iPtr<iAsyncTask> cnLib_FUNC Activate(void)override;

	class cAddressConnector : public cGATTPeripheral::bcConnector, private IBLEDeviceAsyncOpCompletedHandler
	{
	public:

		cAddressConnector(COMPtr<IBLEDeviceAsyncOp> AsyncOp);
		~cAddressConnector();
		
		virtual iAddress* GetAddress(void)override;
		virtual rPtr< iArrayReference<const uChar16> > GetName(void)override;
		virtual void Setup(cGATTPeripheral *Peripheral)override;
		virtual void Cancel(void)override;

		iPtr<cBluetoothAddress> Address;
	protected:
		rInnerPtr<cGATTPeripheral> fPeripheral;
		iPtr<cBluetoothAddress> fAddress;
		COMPtr<IBLEDeviceAsyncOp> fAsyncOp;

		virtual HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid,_COM_Outptr_ void __RPC_FAR *__RPC_FAR *ppvObject)override;

		virtual ULONG STDMETHODCALLTYPE AddRef(void)override;
		virtual ULONG STDMETHODCALLTYPE Release(void)override;

		virtual HRESULT STDMETHODCALLTYPE Invoke(IBLEDeviceAsyncOp*,ABI::Windows::Foundation::AsyncStatus)override;
	};

	eGATTFunctionState PeripheralGetFunctionState(void);

protected:
	void VirtualStarted(void);
	void VirtualStopped(void);

	rPtr<cGATTPeripheralCentral> fCentral;
	iPtr<cBluetoothAddress> fAddress;
	COMPtr<IBLEDevice> fDevice;
	COMPtr<IBLEDevice3> fDevice3;
	cString<uChar16> fDeviceName;
	ABI::Windows::Devices::Bluetooth::BluetoothConnectionStatus fConnectionStatus;

	rPtr<bcConnector> fConnector;

	cSeqSet<iGATTPeripheralHandler*> fHandlers;
private:

	void DeviceOpened(COMPtr<IBLEDevice> Device);
	void DeviceOpenFailed(HRESULT hr);
	void SetupDevice(void);
	void ClearDevice(void);
	void UpdateDeviceName(void);
	void UpdateConnectionStatus(void);
	void NotifyServicesChanged(void);
	void NotifyRSSIChanged(void);

	class cBLEDeviceNameChangedHandler
	{
		cGATTPeripheral* GetHost(void);
	public:
		void EventIncReference(void);
		void EventDecReference(void);
		void EventInvoke(IBLEDevice *sender, IInspectable *args);
	};
	cTypedEventHandler<cBLEDeviceNameChangedHandler,BLEDevice> fBLEDeviceNameChangedHandler;
	class cBLEDeviceConnectionStatusHandler
	{
		cGATTPeripheral* GetHost(void);
	public:
		void EventIncReference(void);
		void EventDecReference(void);
		void EventInvoke(IBLEDevice *sender, IInspectable *args);
	};
	cTypedEventHandler<cBLEDeviceConnectionStatusHandler,BLEDevice> fBLEDeviceConnectionStatusHandler;
	class cBLEDeviceServicesChangedHandler
	{
		cGATTPeripheral* GetHost(void);
	public:
		void EventIncReference(void);
		void EventDecReference(void);
		void EventInvoke(IBLEDevice *sender, IInspectable *args);
	};
	cTypedEventHandler<cBLEDeviceServicesChangedHandler,BLEDevice> fBLEDeviceServicesChangedHandler;

	EventRegistrationToken fBLEDeviceNameChangedToken;
	EventRegistrationToken fBLEDeviceConnectionStatusToken;
	EventRegistrationToken fBLEDeviceServicesChangedToken;
		
	class cGetServicesTask : public cInterfaceFromAsyncOperation<iGATTServiceAsyncFunction,GattDeviceServiceResult>
	{
	public:
		cGetServicesTask();
		~cGetServicesTask();
		
		virtual bool cnLib_FUNC IsDone(void)override;
		virtual bool cnLib_FUNC SetNotify(iProcedure *NotifyProcedure)override;
		virtual void cnLib_FUNC Cancel(void)override;
		virtual iArrayReference< rPtr<iGATTService> >* cnLib_FUNC GetResult(void)override;

	protected:
		rInnerPtr<cGATTPeripheral> fPeripheral;
		cAsyncTaskState fTaskState;

		virtual HRESULT STDMETHODCALLTYPE Invoke(IBLEGetServiceAsyncOp*,ABI::Windows::Foundation::AsyncStatus)override;
	};

	struct cServiceItem
	{
		rInnerPtr<cGATTService> Service;
	};
	cSeqMap< cGATTUUID,cServiceItem,cnDataStruct::cRAWDataItemOrderOperator<cGATTUUID> > fActiveServiceMap;
	rPtr<iSharedMutex> fActiveServiceMapMutex;

	friend cGATTService;
	void ServiceAttach(cGATTService *Service);
	void ServiceDetach(cGATTService *Service);
};
//---------------------------------------------------------------------------
class cGATTPeripheralObserver : public iGATTPeripheralObserver, public bcAsyncQueue, public cDualReference
{
	typedef ABI::Windows::Devices::Bluetooth::Advertisement::BluetoothLEAdvertisementWatcher BLEWatcher;
	typedef ABI::Windows::Devices::Bluetooth::Advertisement::IBluetoothLEAdvertisementWatcher IBLEWatcher;
public:
	cGATTPeripheralObserver(cGATTPeripheralCentral *Central,COMPtr<IBLEWatcher> Watcher);
	~cGATTPeripheralObserver();

	virtual bool cnLib_FUNC StartNotify(iReference *Reference,iAsyncNotificationCallback *Callback)override;
	virtual void cnLib_FUNC StopNotify(void)override;
	virtual void cnLib_FUNC NotifyCallback(bool IdleNotify)override;
	virtual bool cnLib_FUNC IsClosed(void)override;


	virtual iDispatch* cnLib_FUNC GetHandlerDispatch(void)override;

	virtual iGATTPeripheralCentral* cnLib_FUNC GetPeripheralCentral(void)override;

	virtual void cnLib_FUNC DiscardChanges(void)override;
	virtual rPtr< iArrayReference< rPtr<iGATTPeripheral> > > cnLib_FUNC QueryAll(void)override;
	virtual rPtr< iArrayReference< rPtr<iGATTPeripheral> > > cnLib_FUNC QueryChanges(void)override;

protected:
	void VirtualStarted(void);
	void VirtualStopped(void);

	rPtr<cGATTPeripheralCentral> fCentral;
	COMPtr<IBLEWatcher> fWatcher;

	rPtr<iReference> fCallbackReference;
	iAsyncNotificationCallback *fCallback;
	virtual iReference* NotificationInnerReference(void)override;
	virtual void NotificationStarted(void)override;
	virtual void NotificationStopped(void)override;
	//virtual void NotificationClosed(void)=0;
	//virtual CycleState NotificationCheckState(void)override;
	//virtual void NotificationProcedure(void)=0;
	virtual void AsyncQueueNotify(void)override;

private:
	typedef ABI::Windows::Foundation::ITypedEventHandler<
		BLEWatcher*,
		ABI::Windows::Devices::Bluetooth::Advertisement::BluetoothLEAdvertisementReceivedEventArgs*
	> IBLEWatcherHandler;

	class cBLEReceivedHandler : public IBLEWatcherHandler
	{
		cGATTPeripheralObserver* GetHost(void);
	public:
		virtual HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid,_COM_Outptr_ void __RPC_FAR *__RPC_FAR *ppvObject)override;

		virtual ULONG STDMETHODCALLTYPE AddRef(void)override;
		virtual ULONG STDMETHODCALLTYPE Release(void)override;
		virtual HRESULT STDMETHODCALLTYPE Invoke(IBLEWatcher *sender, _In_ ABI::Windows::Devices::Bluetooth::Advertisement::IBluetoothLEAdvertisementReceivedEventArgs *args)override;
	}fBLEReceivedHandler;
	EventRegistrationToken fBLEReceivedToken;


	struct cBLEAdvData
	{
		uInt64 Address;
		INT16 SignalStrength;
		INT64 Timestamp;
		ABI::Windows::Devices::Bluetooth::Advertisement::BluetoothLEAdvertisementType Type;
		enum ABI::Windows::Devices::Bluetooth::Advertisement::BluetoothLEAdvertisementFlags Flags;
	// flags
	//		ClassicNotSupported			4		Specifies Bluetooth BR/EDR not supported.
	//		DualModeControllerCapable	8		Specifies simultaneous Bluetooth LE and BR/EDR to same device capable (controller).
	//		DualModeHostCapable			16		Specifies simultaneous Bluetooth LE and BR/EDR to same device capable (host)
	//		GeneralDiscoverableMode		2		Specifies Bluetooth LE General Discoverable Mode.
	//		LimitedDiscoverableMode		1		Specifies Bluetooth LE Limited Discoverable Mode.
	//		None						0		Specifies no flag.

		cSeqList<GUID> ServiceIDs;
		cString<uChar16> LocalName;
		struct cManData{
			cMemoryBlock Data;
			UINT16 CompanyID;
		};
		cSeqList<cManData> ManufactureData;
		struct cDataSection{
			cMemoryBlock Data;
			uInt8 DataType;
		};
		cSeqList<cDataSection> SectionData;
	};
	void BLEReceived(aClsRef<cBLEAdvData> AdvData);
};
//---------------------------------------------------------------------------
class cGATTAdvertisementObserver : public iGATTAdvertisementObserver, public bcAsyncQueue, public cDualReference
{
	typedef ABI::Windows::Devices::Bluetooth::Advertisement::BluetoothLEAdvertisementWatcher BLEWatcher;
	typedef ABI::Windows::Devices::Bluetooth::Advertisement::IBluetoothLEAdvertisementWatcher IBLEWatcher;

public:
	cGATTAdvertisementObserver(cGATTPeripheralCentral *Central,COMPtr<IBLEWatcher> Watcher);
	~cGATTAdvertisementObserver();

	virtual bool cnLib_FUNC StartNotify(iReference *Reference,iAsyncNotificationCallback *Callback)override;
	virtual void cnLib_FUNC StopNotify(void)override;
	virtual void cnLib_FUNC NotifyCallback(bool IdleNotify)override;
	virtual bool cnLib_FUNC IsClosed(void)override;


	virtual iDispatch* cnLib_FUNC GetHandlerDispatch(void)override;

	virtual iGATTPeripheralCentral* cnLib_FUNC GetPeripheralCentral(void)override;

	virtual void cnLib_FUNC DiscardQueue(void)override;
	virtual rPtr<iGATTAdvertisement> cnLib_FUNC Fetch(void)override;

protected:
	void VirtualStarted(void);
	void VirtualStopped(void);

	rPtr<cGATTPeripheralCentral> fCentral;
	COMPtr<IBLEWatcher> fWatcher;

	rPtr<iReference> fCallbackReference;
	iAsyncNotificationCallback *fCallback;
	virtual iReference* NotificationInnerReference(void)override;
	virtual void NotificationStarted(void)override;
	virtual void NotificationStopped(void)override;
	//virtual void NotificationClosed(void)=0;
	//virtual CycleState NotificationCheckState(void)override;
	//virtual void NotificationProcedure(void)=0;
	virtual void AsyncQueueNotify(void)override;


private:
	typedef ABI::Windows::Foundation::ITypedEventHandler<
		BLEWatcher*,
		ABI::Windows::Devices::Bluetooth::Advertisement::BluetoothLEAdvertisementReceivedEventArgs*
	> IBLEWatcherHandler;

	class cBLEReceivedHandler : public IBLEWatcherHandler
	{
		cGATTAdvertisementObserver* GetHost(void);
	public:
		virtual HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid,_COM_Outptr_ void __RPC_FAR *__RPC_FAR *ppvObject)override;

		virtual ULONG STDMETHODCALLTYPE AddRef(void)override;
		virtual ULONG STDMETHODCALLTYPE Release(void)override;
		virtual HRESULT STDMETHODCALLTYPE Invoke(IBLEWatcher *sender, _In_ ABI::Windows::Devices::Bluetooth::Advertisement::IBluetoothLEAdvertisementReceivedEventArgs *args)override;
	}fBLEReceivedHandler;
	EventRegistrationToken fBLEReceivedToken;


	class cAdvertisementData : public iGATTAdvertisement
	{
	public:
		cAdvertisementData *Next;
		virtual iAddress* cnLib_FUNC GetPeripheralAddress(void)override;
		virtual cGATTAdvertisementInfo cnLib_FUNC GetInfo(void)override;

		virtual uIntn cnLib_FUNC GetServiceUUIDCount(void)override;
		virtual cGATTUUID cnLib_FUNC GetServiceUUID(uIntn Index)override;

		virtual uIntn cnLib_FUNC GetManufactureDataCount(void)override;
		virtual cConstMemory cnLib_FUNC GetManufactureData(uIntn Index,ufInt16 &CompanyID)override;

		virtual uIntn cnLib_FUNC GetSectionDataCount(void)override;
		virtual cConstMemory cnLib_FUNC GetSectionData(uIntn Index,ufInt8 &DataType)override;

		iPtr<iAddress> Address;
		cString<uChar16> LocalName;
		cGATTAdvertisementInfo Info;
		cSeqList<cGATTUUID> ServiceUUIDs;
		struct cManufacturerDataItem{
			cMemoryBlock Data;
			UINT16 CompanyID;
		};
		cSeqList<cManufacturerDataItem> ManufacturerData;
		struct cDataSection{
			cMemoryBlock Data;
			uInt8 DataType;
		};
		cSeqList<cDataSection> SectionData;
		cTime Timestamp;
	};
	cAtomicQueueSO<cAdvertisementData> fAdvertisementQueue;
	void BLEReceived(rPtr<cAdvertisementData> AdvData);
	rObjectRecycler<cAdvertisementData> fAdvDataRecycler;
};
//---------------------------------------------------------------------------
class cGATTPeripheralCentral : public iGATTPeripheralCentral
{
	typedef ABI::Windows::Devices::Bluetooth::Advertisement::BluetoothLEAdvertisementWatcher BLEWatcher;
	typedef ABI::Windows::Devices::Bluetooth::Advertisement::IBluetoothLEAdvertisementWatcher IBLEWatcher;
	typedef ABI::Windows::Devices::Bluetooth::IBluetoothLEDeviceStatics IBLEDeviceStatics;
	typedef ABI::Windows::Foundation::IAsyncOperation<ABI::Windows::Devices::Bluetooth::BluetoothLEDevice*> IBLEConnectAsyncOp;
public:
	cGATTPeripheralCentral(iDispatch *Dispatch,COMPtr<IBLEDeviceStatics> BLEStatic);

	virtual iDispatch* cnLib_FUNC GetHandlerDispatch(void)override;
	virtual bool cnLib_FUNC InsertHandler(iGATTPeripheralCentralHandler *Handler)override;
	virtual bool cnLib_FUNC RemoveHandler(iGATTPeripheralCentralHandler *Handler)override;

	virtual rPtr<iGATTPeripheral> cnLib_FUNC AccessPeripheral(iAddress *Address)override;

	virtual rPtr<iGATTAdvertisementObserver> cnLib_FUNC CreateAdvertisementObserver(void)override;
	virtual rPtr<iGATTPeripheralObserver> cnLib_FUNC CreatePeripheralObserver(const cGATTUUID *Service,uIntn ServiceCount)override;
	virtual bool cnLib_FUNC ResetPeripheralCache(void)override;

	virtual bool cnLib_FUNC IsEnabled(void)override;
	virtual bool cnLib_FUNC IsPowerOn(void)override;

protected:
	COMPtr<IBLEDeviceStatics> fBLEStatic;
};

rPtr<iGATTPeripheralCentral> OpenGATTPeripheral(iDispatch *Dispatch);

#if 0


//---------------------------------------------------------------------------
struct cBLEConnectionUUID
{
	GUID Service;
	GUID WriteCharacteristics;
	GUID ReadCharacteristics;
};
//---------------------------------------------------------------------------
class cWindowsBluetoothLEConectionDeviceInfo : public iAddress
{
public:
	cnLib_INTERFACE_DEFINE(cWindowsBluetoothLEConectionDeviceInfo,iAddress)

	cWindowsBluetoothLEConectionDeviceInfo(ABI::Windows::Devices::Enumeration::IDeviceInformation *DeviceInfo);
	~cWindowsBluetoothLEConectionDeviceInfo();

	virtual eOrdering cnLib_FUNC Compare(const iComparable *Dest)const override;

	cnRTL::cWindowsString DeviceID;
	cnRTL::cSeqList<cBLEConnectionUUID> DeviceUUIDList;
	ABI::Windows::Devices::Enumeration::IDeviceInformation* GetDeviceInfo(void);
	GUID GetServiceGUID(void);
	GUID GetWriteCharactrisc(void);
	GUID GetReadCharactrisc(void);

protected:
	COMPtr<ABI::Windows::Devices::Enumeration::IDeviceInformation> fDeviceInfo;
};
//---------------------------------------------------------------------------
class cWindowsBluetoothLEDevice : public iReference
{
public:
	cWindowsBluetoothLEDevice(COMPtr<ABI::Windows::Devices::Bluetooth::IBluetoothLEDevice> Device);
	~cWindowsBluetoothLEDevice();

	//void ocReceivedData(NSData *Data);
protected:
	COMPtr<ABI::Windows::Devices::Bluetooth::IBluetoothLEDevice> fDevice;
	iPtr<cnRTL::cBufferedExtEndpoint> fEndpoint;
	//virtual void BluetoothLENotifyWrite(void)override;
};
//---------------------------------------------------------------------------
class cBluetoothLETunnelStreamEndpointDevice : public cnRTL::cBufferedExtEndpoint, public iReference,public cnRTL::cDualReference
{
public:
	cBluetoothLETunnelStreamEndpointDevice(
		COMPtr<ABI::Windows::Devices::Bluetooth::IBluetoothLEDevice> Device,
		COMPtr<ABI::Windows::Devices::Bluetooth::GenericAttributeProfile::IGattCharacteristic> ReadChar,
		COMPtr<ABI::Windows::Devices::Bluetooth::GenericAttributeProfile::IGattCharacteristic> WriteChar
	);
	~cBluetoothLETunnelStreamEndpointDevice();


	//void ocReceivedData(NSData *Data);
protected:
	void VirtualStarted(void);
	void VirtualStopped(void);

	COMPtr<ABI::Windows::Storage::Streams::IDataReaderStatics> fDataReaderStatic;
	COMPtr<ABI::Windows::Devices::Bluetooth::IBluetoothLEDevice> fDevice;
	COMPtr<ABI::Windows::Devices::Bluetooth::GenericAttributeProfile::IGattCharacteristic> fReadCharacteristic;
	COMPtr<ABI::Windows::Devices::Bluetooth::GenericAttributeProfile::IGattCharacteristic> fWriteCharacteristic;
	//virtual void BluetoothLENotifyWrite(void)override;

	void ConnectionStatusEvent(void);
	void GattServicesChangedEvent(void);
	void NameChangedEvent(void);
	void ValueChangedEvent(ABI::Windows::Storage::Streams::IBuffer *Buffer);

	EventRegistrationToken fConnectionStatusEventToken;
	EventRegistrationToken fGattServicesChangedEventToken;
	EventRegistrationToken fNameChangedEventToken;
	EventRegistrationToken fValueChangedEventToken;

	class cConnectionStatusHandler : public ABI::Windows::Foundation::ITypedEventHandler<ABI::Windows::Devices::Bluetooth::BluetoothLEDevice*,IInspectable*>
	{
		cBluetoothLETunnelStreamEndpointDevice *GetHost(void);
	public:
		HRESULT STDMETHODCALLTYPE QueryInterface( REFIID riid,_COM_Outptr_ void **ppvObject)override;
		virtual ULONG STDMETHODCALLTYPE AddRef(void)override;
		virtual ULONG STDMETHODCALLTYPE Release(void)override;
        virtual HRESULT STDMETHODCALLTYPE Invoke(ABI::Windows::Devices::Bluetooth::IBluetoothLEDevice*,IInspectable*)override;

	}fConnectionStatusHandler;

	class cGattServicesChangedHandler : public ABI::Windows::Foundation::ITypedEventHandler<ABI::Windows::Devices::Bluetooth::BluetoothLEDevice*,IInspectable*>
	{
		cBluetoothLETunnelStreamEndpointDevice *GetHost(void);
	public:
		HRESULT STDMETHODCALLTYPE QueryInterface( REFIID riid,_COM_Outptr_ void **ppvObject)override;
		virtual ULONG STDMETHODCALLTYPE AddRef(void)override;
		virtual ULONG STDMETHODCALLTYPE Release(void)override;
        virtual HRESULT STDMETHODCALLTYPE Invoke(ABI::Windows::Devices::Bluetooth::IBluetoothLEDevice*,IInspectable*)override;

	}fGattServicesChangedHandler;

	class cNameChangedHandler : public ABI::Windows::Foundation::ITypedEventHandler<ABI::Windows::Devices::Bluetooth::BluetoothLEDevice*,IInspectable*>
	{
		cBluetoothLETunnelStreamEndpointDevice *GetHost(void);
	public:
		HRESULT STDMETHODCALLTYPE QueryInterface( REFIID riid,_COM_Outptr_ void **ppvObject)override;
		virtual ULONG STDMETHODCALLTYPE AddRef(void)override;
		virtual ULONG STDMETHODCALLTYPE Release(void)override;
        virtual HRESULT STDMETHODCALLTYPE Invoke(ABI::Windows::Devices::Bluetooth::IBluetoothLEDevice*,IInspectable*)override;

	}fNameChangedHandler;

	class cValueChangedHandler : public ABI::Windows::Foundation::ITypedEventHandler<ABI::Windows::Devices::Bluetooth::GenericAttributeProfile::GattCharacteristic*,ABI::Windows::Devices::Bluetooth::GenericAttributeProfile::GattValueChangedEventArgs*>
	{
		cBluetoothLETunnelStreamEndpointDevice *GetHost(void);
	public:
		HRESULT STDMETHODCALLTYPE QueryInterface( REFIID riid,_COM_Outptr_ void **ppvObject)override;
		virtual ULONG STDMETHODCALLTYPE AddRef(void)override;
		virtual ULONG STDMETHODCALLTYPE Release(void)override;
        virtual HRESULT STDMETHODCALLTYPE Invoke(ABI::Windows::Devices::Bluetooth::GenericAttributeProfile::IGattCharacteristic*,ABI::Windows::Devices::Bluetooth::GenericAttributeProfile::IGattValueChangedEventArgs *Args)override;

	}fValueChangedHandler;
};
//---------------------------------------------------------------------------
class cBluetoothLETunnelStreamProtocol : public iConnectionProtocol
{
public:
	cBluetoothLETunnelStreamProtocol();
	~cBluetoothLETunnelStreamProtocol();

	virtual iPtr<iConnection>			cnLib_FUNC Connect(iTypeID ConnectionIID,iAddress *RemoteAddress,iAddress *LocalAddress)override;
	virtual iPtr<iConnectionTask>		cnLib_FUNC ConnectAsync(iTypeID ConnectionIID,iAddress *RemoteAddress,iAddress *LocalAddress)override;
	virtual iPtr<iConnectionListener>	cnLib_FUNC Listen(iTypeID ConnectionIID,iAddress *LocalAddress)override;
	virtual iPtr<iConnectionQueue>		cnLib_FUNC ListenQueue(iTypeID ConnectionIID,iAddress *LocalAddress)override;

protected:

	class cConnectAndCheckTask : public iConnectionTask
	{
	public:
		cConnectAndCheckTask();
		~cConnectAndCheckTask();

		cnRTL::cCoroutine<void> RunAsync(iPtr<cWindowsBluetoothLEConectionDeviceInfo> Info,COMPtr< ABI::Windows::Foundation::IAsyncOperation<ABI::Windows::Devices::Bluetooth::BluetoothLEDevice*> > AsyncOp);

		virtual bool cnLib_FUNC IsDone(void)override;
		virtual bool cnLib_FUNC SetNotify(iProcedure *NotifyProcedure)override;
		virtual void cnLib_FUNC Cancel(void)override;
		virtual iConnection* cnLib_FUNC GetConnection(void)override;

		iTypeID ConnectionTypeID;
	protected:

		COMPtr<ABI::Windows::Foundation::IAsyncInfo> fAsyncInfo;
		iPtr<iConnection> fConnection;

		cnRTL::cAsyncTaskState fTaskState;

	};

};
#endif // 0
//---------------------------------------------------------------------------
#endif // >= _WIN32_WINNT_WIN10
//---------------------------------------------------------------------------
}	// namespace UWP
//---------------------------------------------------------------------------
}	// namespace cnRTL
//---------------------------------------------------------------------------
}	// namespace cnLibrary
//---------------------------------------------------------------------------