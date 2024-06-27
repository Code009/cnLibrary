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
cUUID GATTUUIDFromShort(ufInt32 ShortUUID)noexcept(true);
//---------------------------------------------------------------------------
#if _WIN32_WINNT >= _WIN32_WINNT_WIN10
//---------------------------------------------------------------------------
class cGATTService;
class cGATTCharacteristic;
class cGATTPeripheral;
class cGATTPeripheralCentral;
//---------------------------------------------------------------------------
class cBluetoothAddress : public iAddress
{
public:
	struct tInterfaceID{	static iTypeID Value;	};
	virtual void* cnLib_FUNC CastInterface(iTypeID ID)noexcept(true) override{	return cnVar::ImplementCastInterface(this,ID);	}

	virtual eiOrdering cnLib_FUNC Compare(iAddress *Dest)noexcept(true)override;

	UINT64 Address;
};
//---------------------------------------------------------------------------
class cGATTDescriptor : public iGATTDescriptor, public cDualReference
{
	typedef ABI::Windows::Devices::Bluetooth::GenericAttributeProfile::GattDescriptor BLEDescriptor;
	typedef ABI::Windows::Devices::Bluetooth::GenericAttributeProfile::IGattDescriptor IBLEDescriptor;
public:
	cGATTDescriptor(cGATTCharacteristic *Owner,const cUUID &ID)noexcept(true);
	~cGATTDescriptor()noexcept(true);

	cUUID DescriptorUUID;

	virtual cUUID cnLib_FUNC GetUUID(void)noexcept(true)override;
	virtual iDispatch* cnLib_FUNC GetHandlerDispatch(void)noexcept(true)override;
	virtual bool cnLib_FUNC InsertHandler(iGATTDescriptorHandler *Handler)noexcept(true)override;
	virtual bool cnLib_FUNC RemoveHandler(iGATTDescriptorHandler *Handler)noexcept(true)override;

	virtual eGATTFunctionState cnLib_FUNC GetFunctionState(void)noexcept(true)override;
	virtual iGATTCharacteristic* cnLib_FUNC GetCharacterist(void)noexcept(true)override;

	void CharacteristicUpdateDescriptor(IBLEDescriptor *Descriptor)noexcept(true);
	void CharacteristicNotifyFunctionStatus(void)noexcept(true);

protected:
	void VirtualStarted(void)noexcept(true);
	void VirtualStopped(void)noexcept(true);

	rPtr<cGATTCharacteristic> fOwner;
	iPtr<iDispatch> fDispatch;
	COMPtr<IBLEDescriptor> fDescriptor;
	cSeqSet<iGATTDescriptorHandler*> fHandlers;

private:
	void SetupDescriptor(void)noexcept(true);
	void ClearDescriptor(void)noexcept(true);

	eGATTFunctionState fFuncState;

	rPtr<iAsyncProcedure> fMainProcessWork;

	void UpdateFuncState(void)noexcept(true);

};
//---------------------------------------------------------------------------
class cGATTCharacteristic : public iGATTCharacteristic, public cDualReference
{
	typedef ABI::Windows::Devices::Bluetooth::GenericAttributeProfile::GattCharacteristic BLECharacteristic;
	typedef ABI::Windows::Devices::Bluetooth::GenericAttributeProfile::IGattCharacteristic IBLECharacteristic;
	typedef ABI::Windows::Devices::Bluetooth::GenericAttributeProfile::IGattCharacteristic3 IBLECharacteristic3;

	typedef ABI::Windows::Devices::Bluetooth::GenericAttributeProfile::GattDescriptor BLEDescriptor;
	typedef ABI::Windows::Devices::Bluetooth::GenericAttributeProfile::IGattDescriptor IBLEDescriptor;
	typedef ABI::Windows::Devices::Bluetooth::GenericAttributeProfile::GattDescriptorsResult GattDescriptorsResult;
	typedef ABI::Windows::Devices::Bluetooth::GenericAttributeProfile::IGattDescriptorsResult IGattDescriptorsResult;
	typedef ABI::Windows::Foundation::IAsyncOperation<GattDescriptorsResult*> IBLEGetDescriptorsAsyncOp;

	typedef ABI::Windows::Devices::Bluetooth::GenericAttributeProfile::GattReadResult GattReadResult;
	typedef ABI::Windows::Foundation::IAsyncOperation<GattReadResult*> IBLEReadAsyncOp;
	typedef ABI::Windows::Devices::Bluetooth::GenericAttributeProfile::GattWriteResult GattWriteResult;
	typedef ABI::Windows::Devices::Bluetooth::GenericAttributeProfile::IGattWriteResult IGattWriteResult;
	typedef ABI::Windows::Foundation::IAsyncOperation<GattWriteResult*> IBLEWriteAsyncOp;
	typedef ABI::Windows::Devices::Bluetooth::GenericAttributeProfile::GattCommunicationStatus GattCommunicationStatus;
	typedef ABI::Windows::Foundation::IAsyncOperation<GattCommunicationStatus> IBLECommunicationAsyncOp;
	

public:
	cGATTCharacteristic(cGATTService *Owner,const cUUID & ID)noexcept(true);
	~cGATTCharacteristic()noexcept(true);
	
	iDispatch* GetDispatch(void)const noexcept(true);

	cUUID CharacteristcUUID;

	virtual cUUID cnLib_FUNC GetUUID(void)noexcept(true)override;
	virtual iDispatch* cnLib_FUNC GetHandlerDispatch(void)noexcept(true)override;
	virtual bool cnLib_FUNC InsertHandler(iGATTCharacteristicHandler *Handler)noexcept(true)override;
	virtual bool cnLib_FUNC RemoveHandler(iGATTCharacteristicHandler *Handler)noexcept(true)override;

	virtual eGATTFunctionState cnLib_FUNC GetFunctionState(void)noexcept(true)override;
	virtual iGATTService* cnLib_FUNC GetService(void)noexcept(true)override;
	virtual rPtr<iGATTDescriptor> cnLib_FUNC AccessDescriptor(const cUUID &ID)noexcept(true)override;
	virtual rPtr<iGATTDescriptorObserver> cnLib_FUNC CreateDescriptorObserver(void)noexcept(true)override;

	virtual iPtr< iAsyncFunction<cConstMemory> > cnLib_FUNC Read(void)noexcept(true)override;

	virtual iPtr<iAsyncTask> cnLib_FUNC Write(const void *Data,uIntn DataSize)noexcept(true)override;
	virtual bool cnLib_FUNC WriteWithoutResponse(const void *Data,uIntn DataSize)noexcept(true)override;

	virtual eGATTCharacteristicNotification cnLib_FUNC EffectiveValueNotification(void)noexcept(true)override;
	virtual eGATTCharacteristicNotification cnLib_FUNC GetValueNotification(void)noexcept(true)override;
	virtual void cnLib_FUNC SetValueNotification(eGATTCharacteristicNotification Notification)noexcept(true)override;

	void ServiceInvalidateCharacteristic(void)noexcept(true);
	void ServiceUpdateCharacteristic(IBLECharacteristic *Characteristic)noexcept(true);
	void ServiceNotifyFunctionStatus(void)noexcept(true);

	eGATTFunctionState CharacteristicGetFunctionState(void)noexcept(true);

protected:
	void VirtualStarted(void)noexcept(true);
	void VirtualStopped(void)noexcept(true);

	rPtr<cGATTService> fOwner;
	iPtr<iDispatch> fDispatch;
	COMPtr<IBLECharacteristic> fCharacteristic;
	COMPtr<IBLECharacteristic3> fCharacteristic3;
	cSeqSet<iGATTCharacteristicHandler*> fHandlers;

private:
	void SetupCharacteristic(void)noexcept(true);
	void ClearCharacteristic(void)noexcept(true);

	friend cGATTDescriptor;
	void DescriptorAttach(cGATTDescriptor *Descriptor)noexcept(true);
	void DescriptorDetach(cGATTDescriptor *Descriptor)noexcept(true);
	bool DescriptorIsRefreshing(void)const noexcept(true);

	EventRegistrationToken fBLEValueChangedToken;

	cExclusiveFlag fMainProcessExclusiveFlag;
	eGATTFunctionState fFuncState;
	enum{
		psIdle,
		psRefreshDescriptor,psRefreshDescriptorDone,
	}fMainProcessState;
	bool fNeedNotifyCharacteristicState=false;
	bool fDescriptorNeedRefresh=false;
	ufInt8 fConfigureNotificationState=0;

	eGATTCharacteristicNotification fEffectiveNotificationConfig=GATTCharacteristicNotification::None;
	eGATTCharacteristicNotification fTargetNotificationConfig=GATTCharacteristicNotification::None;
	eGATTCharacteristicNotification fWritingNotificationConfig;

	rPtr<iAsyncProcedure> fMainProcessWork;

	class cMainProcessProcedure : public iProcedure
	{
		virtual void cnLib_FUNC Execute(void)noexcept(true)override;
	}fMainProcessProcedure;
	void MainProcessThread(void)noexcept(true);
	void NotifyMainProcess(void)noexcept(true);

	bool MainProcess(void)noexcept(true);
	void MainProcessStateChange(void)noexcept(true);
	bool MainProcess_Idle(void)noexcept(true);
	void MainProcess_RefreshDescriptorDone(void)noexcept(true);

	struct cValueNotification
	{
		cValueNotification *Next;
		uInt64 Timestamp;
		cMemoryBuffer Value;
	};
	cAtomicQueueSO<cValueNotification> fValueNotificationQueue;
	cSharedLinkItemRecycler<cValueNotification> fValueNotificationRecycler;
	
	void NotifyValue(uInt64 Timestamp,const void *Data,uIntn Size)noexcept(true);

	class cBLEValueChangedHandler
	{
		cGATTCharacteristic* GetHost(void)noexcept(true);
	public:
		void EventIncReference(void)noexcept(true);
		void EventDecReference(void)noexcept(true);
		void EventInvoke(IBLECharacteristic *sender,ABI::Windows::Devices::Bluetooth::GenericAttributeProfile::IGattValueChangedEventArgs *args)noexcept(true);
	};
	cTypedEventHandler<cBLEValueChangedHandler,BLECharacteristic*,ABI::Windows::Devices::Bluetooth::GenericAttributeProfile::GattValueChangedEventArgs*> fBLEValueChangedHandler;
	
	class cGetDescriptorsCompleteHandler
	{
		cGATTCharacteristic* GetHost(void)noexcept(true);
	public:
		void EventIncReference(void)noexcept(true);
		void EventDecReference(void)noexcept(true);
		void EventInvoke(IBLEGetDescriptorsAsyncOp *AsyncOp,ABI::Windows::Foundation::AsyncStatus)noexcept(true);

		HRESULT hrResult;
		COMPtr<IGattDescriptorsResult> ResultDescriptors;
	};
	cAsyncOperationCompletedHandler<cGetDescriptorsCompleteHandler,GattDescriptorsResult*> fGetDescriptorsCompleteHandler;


	//cSeqMap< cGATTUUID,cGATTDescriptor*,cnDataStruct::cRAWDataItemOrderOperator<cGATTUUID> > fDescriptorMap;
	cSeqMap<cUUID,cGATTDescriptor*> fDescriptorMap;

	struct cDescriptorPair
	{
		cGATTDescriptor *Descriptor;
		COMPtr<IBLEDescriptor> BLEDescriptor;
	};
	//cSeqMap< cGATTUUID,cDescriptorPair,cnDataStruct::cRAWDataItemOrderOperator<cGATTUUID> > fUpdateMap;
	cSeqMap<cUUID,cDescriptorPair> fUpdateMap;

	class cReadCompleteHandler
	{
		cGATTCharacteristic* GetHost(void)noexcept(true);
	public:
		void EventIncReference(void)noexcept(true);
		void EventDecReference(void)noexcept(true);
		void EventInvoke(IBLEReadAsyncOp *AsyncOp,ABI::Windows::Foundation::AsyncStatus)noexcept(true);
	};
	cAsyncOperationCompletedHandler<cReadCompleteHandler,GattReadResult*> fReadCompleteHandler;
	
	
	class cWriteCompleteHandler
	{
		cGATTCharacteristic* GetHost(void)noexcept(true);
	public:
		void EventIncReference(void)noexcept(true);
		void EventDecReference(void)noexcept(true);
		void EventInvoke(IBLECommunicationAsyncOp *AsyncOp,ABI::Windows::Foundation::AsyncStatus)noexcept(true);
	};
	cAsyncOperationCompletedHandler<cWriteCompleteHandler,GattCommunicationStatus> fWriteCompleteHandler;

	class cConfigureNotificationHandler
	{
		cGATTCharacteristic* GetHost(void)noexcept(true);
	public:
		void EventIncReference(void)noexcept(true);
		void EventDecReference(void)noexcept(true);
		void EventInvoke(IBLEWriteAsyncOp *AsyncOp,ABI::Windows::Foundation::AsyncStatus)noexcept(true);

		HRESULT hrResult;
		COMPtr<IGattWriteResult> Result;
	};
	cAsyncOperationCompletedHandler<cConfigureNotificationHandler,GattWriteResult*> fConfigureNotificationHandler;
};
//---------------------------------------------------------------------------
class cBluetoothCentral;
class cBluetoothSlave;
//---------------------------------------------------------------------------
class cGATTService : public iGATTService, public cDualReference
{
	typedef ABI::Windows::Devices::Bluetooth::BluetoothLEDevice BLEDevice;

	typedef ABI::Windows::Devices::Bluetooth::GenericAttributeProfile::GattDeviceService BLEService;
	typedef ABI::Windows::Devices::Bluetooth::GenericAttributeProfile::IGattDeviceService IBLEService;
	typedef ABI::Windows::Devices::Bluetooth::GenericAttributeProfile::IGattDeviceService3 IBLEService3;

	typedef ABI::Windows::Devices::Bluetooth::GenericAttributeProfile::GattDeviceServicesResult GattDeviceServiceResult;
	typedef ABI::Windows::Devices::Bluetooth::GenericAttributeProfile::IGattDeviceServicesResult IGattDeviceServiceResult;
	typedef ABI::Windows::Foundation::IAsyncOperation<BLEDevice*> IBLEConnectAsyncOp;
	typedef ABI::Windows::Foundation::IAsyncOperation<GattDeviceServiceResult*> IBLEGetServiceAsyncOp;
	typedef ABI::Windows::Foundation::IAsyncOperation<ABI::Windows::Devices::Bluetooth::GenericAttributeProfile::GattOpenStatus> IBLEOpenAsyncOp;
	
	typedef ABI::Windows::Devices::Bluetooth::GenericAttributeProfile::IGattCharacteristic IBLECharacteristic;

	typedef ABI::Windows::Devices::Bluetooth::GenericAttributeProfile::GattCharacteristicsResult GattCharacteristicsResult;
	typedef ABI::Windows::Devices::Bluetooth::GenericAttributeProfile::IGattCharacteristicsResult IGattCharacteristicsResult;
	typedef ABI::Windows::Foundation::IAsyncOperation<GattCharacteristicsResult*> IBLEGetCharacteristicAsyncOp;

public:
	cGATTService(cBluetoothSlave *Owner,const cUUID &ID)noexcept(true);
	~cGATTService()noexcept(true);

	iDispatch* GetDispatch(void)const noexcept(true);

	virtual cUUID cnLib_FUNC GetUUID(void)noexcept(true)override;
	virtual iDispatch* cnLib_FUNC GetHandlerDispatch(void)noexcept(true)override;
	virtual bool cnLib_FUNC InsertHandler(iGATTServiceHandler *Handler)noexcept(true)override;
	virtual bool cnLib_FUNC RemoveHandler(iGATTServiceHandler *Handler)noexcept(true)override;

	virtual eGATTFunctionState cnLib_FUNC GetFunctionState(void)noexcept(true)override;
	virtual iGATTClient* cnLib_FUNC GetClient(void)noexcept(true)override;
	virtual rPtr<iGATTCharacteristic> cnLib_FUNC AccessCharacteristic(const cUUID &ID)noexcept(true)override;
	virtual iPtr<iGATTCharacteristicObserver> cnLib_FUNC CreateCharacteristicObserver(void)noexcept(true)override;

	cUUID ServiceUUID;
	void PeripheralNotifyClose(void)noexcept(true);
	void PeripheralNotifyScanService(void)noexcept(true);
	void PeripheralNotifyConnectionStatus(void)noexcept(true);
	
	eGATTFunctionState ServiceGetFunctionState(void)noexcept(true);

	
	class cOpenServiceProcedure : public iReference
	{
	public:
		cOpenServiceProcedure *Next;
		rInnerPtr<cGATTService> Service;

		void GetService(ABI::Windows::Devices::Bluetooth::IBluetoothLEDevice3 *Device)noexcept(true);
		void ServiceScanned(IBLEService *Service)noexcept(true);

	private:
		class cGetServicesCompleteHandler
		{
			cOpenServiceProcedure* GetHost(void)noexcept(true);
		public:
			void EventIncReference(void)noexcept(true);
			void EventDecReference(void)noexcept(true);
			void EventInvoke(IBLEGetServiceAsyncOp *AsyncOp,ABI::Windows::Foundation::AsyncStatus)noexcept(true);
		};
		cAsyncOperationCompletedHandler<cGetServicesCompleteHandler,GattDeviceServiceResult*> fGetServicesCompleteHandler;
		class cOpenAsyncCompleteHandler
		{
			cOpenServiceProcedure* GetHost(void)noexcept(true);
		public:
			void EventIncReference(void)noexcept(true);
			void EventDecReference(void)noexcept(true);
			void EventInvoke(IBLEOpenAsyncOp *AsyncOp,ABI::Windows::Foundation::AsyncStatus)noexcept(true);
		};
		cAsyncOperationCompletedHandler<cOpenAsyncCompleteHandler,ABI::Windows::Devices::Bluetooth::GenericAttributeProfile::GattOpenStatus> fOpenAsyncCompleteHandler;

		void OnServiceResult(IBLEGetServiceAsyncOp *AsyncOp)noexcept(true);
		bool OnServiceResultProcess(IBLEGetServiceAsyncOp *AsyncOp)noexcept(true);
		void OpenResult(IBLEOpenAsyncOp *AsyncOp)noexcept(true);

		COMPtr<IBLEService> fService;
		COMPtr<IBLEService3> fService3;
		class cOpenResultProcedure : public iProcedure
		{
		public:
			virtual void cnLib_FUNC Execute(void)noexcept(true)override;
		}fOpenResultProcedure;
		void OpenSuccessThread(void)noexcept(true);
	};

	class cCharacteristicObserver : public iGATTCharacteristicObserver, public bcAsyncQueue
	{
	public:
		virtual bool cnLib_FUNC StartNotify(iReference *Reference,iAsyncNotificationCallback *Callback)noexcept(true)override;
		virtual void cnLib_FUNC StopNotify(void)noexcept(true)override;
		virtual void cnLib_FUNC NotifyCallback(bool IdleNotify)noexcept(true)override;
		virtual bool cnLib_FUNC IsClosed(void)noexcept(true)override;

		virtual iDispatch* cnLib_FUNC GetHandlerDispatch(void)noexcept(true)override;

		virtual iGATTService* cnLib_FUNC GetService(void)noexcept(true)override;

		virtual rPtr<iReference> cnLib_FUNC QueryCharacteristics(cArray< rPtr<iGATTCharacteristic> > &Characteristics)noexcept(true)override;
		virtual void cnLib_FUNC DiscardChanges(void)noexcept(true)override;
		virtual rPtr<iGATTCharacteristic> cnLib_FUNC FetchChanged(bool &IsInserted)noexcept(true)override;
	};

protected:
	void VirtualStarted(void)noexcept(true);
	void VirtualStopped(void)noexcept(true);

	rPtr<cBluetoothSlave> fOwner;
	iPtr<iDispatch> fDispatch;
	COMPtr<IBLEService> fService;
	COMPtr<IBLEService3> fService3;

	cSeqSet<iGATTServiceHandler*> fHandlers;

private:
	friend cGATTCharacteristic;
	void CharacteristicAttach(cGATTCharacteristic *Characteristic)noexcept(true);
	void CharacteristicDetach(cGATTCharacteristic *Characteristic)noexcept(true);
	bool CharacteristicIsRefreshing(void)const noexcept(true);

	void ServiceOpened(COMPtr<IBLEService> Service,COMPtr<IBLEService3> Service3)noexcept(true);
	void ServiceOpenFailed(void)noexcept(true);

	cExclusiveFlag fMainProcessExclusiveFlag;
	eGATTFunctionState fFuncState;
	ufInt8 fCloseState=0;
	bool fScaningService;
	enum{
		psIdle,
		psRefreshCharacteristic,psRefreshCharacteristicDone,
	}fMainProcessState;
	bool fNeedNotifyServiceState=false;
	bool fCharacteristicNeedRefresh=false;

	rPtr<iAsyncProcedure> fMainProcessWork;

	class cMainProcessProcedure : public iProcedure
	{
		virtual void cnLib_FUNC Execute(void)noexcept(true)override;
	}fMainProcessProcedure;
	void MainProcessThread(void)noexcept(true);
	void RunMainProcess(void)noexcept(true);
	void NotifyMainProcess(void)noexcept(true);

	bool MainProcess(void)noexcept(true);
	void MainProcessStateChange(void)noexcept(true);
	void MainProcess_Idle(void)noexcept(true);
	void MainProcess_RefreshCharacteristicDone(void)noexcept(true);


		
	class cGetCharacteristicsCompleteHandler
	{
		cGATTService* GetHost(void)noexcept(true);
	public:
		void EventIncReference(void)noexcept(true);
		void EventDecReference(void)noexcept(true);
		void EventInvoke(IBLEGetCharacteristicAsyncOp *AsyncOp,ABI::Windows::Foundation::AsyncStatus)noexcept(true);

		HRESULT hrResult;
		COMPtr<IGattCharacteristicsResult> ResultCharacteristics;
	};
	cAsyncOperationCompletedHandler<cGetCharacteristicsCompleteHandler,GattCharacteristicsResult*> fGetCharacteristicsCompleteHandler;


	cSeqMap<cUUID,cGATTCharacteristic*> fCharacteristicMap;

	struct cCharacteristicPair
	{
		cGATTCharacteristic *Characteristic;
		COMPtr<IBLECharacteristic> BLECharacteristic;
	};
	cSeqMap<cUUID,cCharacteristicPair> fUpdateMap;

};
//---------------------------------------------------------------------------
class cBluetoothSlave : public iBluetoothSlave, public cDualReference
{
	typedef ABI::Windows::Devices::Bluetooth::BluetoothLEDevice BLEDevice;
	typedef ABI::Windows::Devices::Bluetooth::IBluetoothLEDevice IBLEDevice;
	typedef ABI::Windows::Devices::Bluetooth::IBluetoothLEDevice2 IBLEDevice2;
	typedef ABI::Windows::Devices::Bluetooth::IBluetoothLEDevice3 IBLEDevice3;
	typedef ABI::Windows::Foundation::IAsyncOperation<BLEDevice*> IBLEDeviceAsyncOp;
	typedef ABI::Windows::Foundation::IAsyncOperationCompletedHandler<BLEDevice*> IBLEDeviceAsyncOpCompletedHandler;
	typedef ABI::Windows::Devices::Bluetooth::GenericAttributeProfile::GattDeviceService BLEService;
	typedef ABI::Windows::Devices::Bluetooth::GenericAttributeProfile::IGattDeviceService IBLEService;
	typedef ABI::Windows::Devices::Bluetooth::GenericAttributeProfile::IGattDeviceService3 IBLEService3;

	typedef ABI::Windows::Devices::Bluetooth::GenericAttributeProfile::GattDeviceServicesResult GattDeviceServiceResult;
	typedef ABI::Windows::Devices::Bluetooth::GenericAttributeProfile::IGattDeviceServicesResult IGattDeviceServiceResult;
	typedef ABI::Windows::Foundation::IAsyncOperation<BLEDevice*> IBLEConnectAsyncOp;
	typedef ABI::Windows::Foundation::IAsyncOperation<GattDeviceServiceResult*> IBLEGetServiceAsyncOp;
public:

	cBluetoothSlave(cBluetoothCentral *Central,iPtr<cBluetoothAddress> Address)noexcept(true);
	~cBluetoothSlave()noexcept(true);

	iDispatch* GetDispatch(void)const noexcept(true);

	virtual iDispatch* cnLib_FUNC GetHandlerDispatch(void)noexcept(true)override;
	virtual eGATTFunctionState cnLib_FUNC GetFunctionState(void)noexcept(true)override;

	virtual rPtr<iGATTService> cnLib_FUNC AccessService(const cUUID &ID)noexcept(true)override;
	virtual iPtr<iGATTServiceObserver> cnLib_FUNC CreateServiceObserver(void)noexcept(true)override;


	virtual void cnLib_FUNC Close(void)noexcept(true)override;
	virtual iAddress* cnLib_FUNC GetPeripheralAddress(void)noexcept(true)override;
	virtual bool cnLib_FUNC InsertHandler(iBluetoothSlaveHandler *Handler)noexcept(true)override;
	virtual bool cnLib_FUNC RemoveHandler(iBluetoothSlaveHandler *Handler)noexcept(true)override;
	virtual iBluetoothCentral* cnLib_FUNC GetCentral(void)noexcept(true)override;

	virtual rPtr<iReference> cnLib_FUNC QueryName(cArray<const uChar16> &Name)noexcept(true)override;

	void CentralConnectionOperation(COMPtr<IBLEConnectAsyncOp> ConnectOp)noexcept(true);
	
	eGATTFunctionState ClientGetFunctionState(void)const noexcept(true);

protected:
	void VirtualStarted(void)noexcept(true);
	void VirtualStopped(void)noexcept(true);

	rPtr<cBluetoothCentral> fCentral;
	iPtr<iDispatch> fDispatch;
	iPtr<cBluetoothAddress> fAddress;
	COMPtr<IBLEDevice> fDevice;
	COMPtr<IBLEDevice3> fDevice3;
	cString<uChar16> fDeviceName;


	cSeqSet<iBluetoothSlaveHandler*> fHandlers;

	class cServiceObserver : public iGATTServiceObserver, public bcAsyncQueue
	{
	public:
		virtual bool cnLib_FUNC StartNotify(iReference *Reference,iAsyncNotificationCallback *Callback)noexcept(true)override;
		virtual void cnLib_FUNC StopNotify(void)noexcept(true)override;
		virtual void cnLib_FUNC NotifyCallback(bool IdleNotify)noexcept(true)override;
		virtual bool cnLib_FUNC IsClosed(void)noexcept(true)override;

		virtual iDispatch* cnLib_FUNC GetHandlerDispatch(void)noexcept(true)override;

		virtual iGATTClient* cnLib_FUNC GetClient(void)noexcept(true)override;

		virtual rPtr<iReference> cnLib_FUNC QueryServices(cArray< rPtr<iGATTService> > &Services)noexcept(true)override;
		virtual void cnLib_FUNC DiscardChanges(void)noexcept(true)override;
		virtual rPtr<iGATTService> cnLib_FUNC FetchChanged(bool &IsInserted)noexcept(true)override;
	};
private:

	friend cGATTService;
	void ServiceAttach(cGATTService *Service)noexcept(true);
	void ServiceDetach(cGATTService *Service)noexcept(true);
	void ServiceScan(cGATTService::cOpenServiceProcedure *OpenProc)noexcept(true);

	void RefreshService(void)noexcept(true);
	void RefreshServiceProc(void)noexcept(true);
	void OnServiceResult(IBLEGetServiceAsyncOp *AsyncOp)noexcept(true);
	bool OnServiceResultProcess(IBLEGetServiceAsyncOp *AsyncOp)noexcept(true);
	
	cAtomicVar<uIntn> fServiceObserverCount=0;
	cAtomicStackSO<cGATTService::cOpenServiceProcedure> fOpenServiceStack;

	struct cGetServicesResult
	{
		cSeqMap< cUUID,COMPtr<IBLEService> > Map;
	};
	aClsAtomicRef<cGetServicesResult> fGetServicesResult;

	cExclusiveFlag fMainProcessExclusiveFlag;
	cExclusiveFlag fQueryServiceExclusiveFlag;
	eGATTFunctionState fFuncState;
	ufInt8 fCloseState=0;
	bool fConnectOpRunning=false;
	bool fNeedNotifyConnectionStateChange=false;
	bool fNeedNotifyNameChange=false;
	bool fProcessGetServicesResult=false;

	rPtr<iAsyncProcedure> fMainProcessWork;

	class cMainProcessProcedure : public iProcedure
	{
		virtual void cnLib_FUNC Execute(void)noexcept(true)override;
	}fMainProcessProcedure;
	void MainProcessThread(void)noexcept(true);
	void NotifyMainProcess(void)noexcept(true);

	void MainProcess(void)noexcept(true);

	void DeviceOpened(COMPtr<IBLEDevice> Device)noexcept(true);
	void DeviceOpenFailed(HRESULT hr)noexcept(true);
	void SetupDevice(void)noexcept(true);
	void ClearDevice(void)noexcept(true);
	void NotifyServicesChanged(void)noexcept(true);

	
	class cAddressConnectCompleteHandler
	{
		cBluetoothSlave* GetHost(void)noexcept(true);
	public:
		void EventIncReference(void)noexcept(true);
		void EventDecReference(void)noexcept(true);
		void EventInvoke(IBLEDeviceAsyncOp *AsyncOp,ABI::Windows::Foundation::AsyncStatus)noexcept(true);
	};
	cAsyncOperationCompletedHandler<cAddressConnectCompleteHandler,BLEDevice*> fAddressConnectCompleteHandler;

	class cBLEDeviceNameChangedHandler
	{
		cBluetoothSlave* GetHost(void)noexcept(true);
	public:
		void EventIncReference(void)noexcept(true);
		void EventDecReference(void)noexcept(true);
		void EventInvoke(IBLEDevice *sender, IInspectable *args)noexcept(true);
	};
	cTypedEventHandler<cBLEDeviceNameChangedHandler,BLEDevice*,IInspectable*> fBLEDeviceNameChangedHandler;
	class cBLEDeviceConnectionStatusHandler
	{
		cBluetoothSlave* GetHost(void)noexcept(true);
	public:
		void EventIncReference(void)noexcept(true);
		void EventDecReference(void)noexcept(true);
		void EventInvoke(IBLEDevice *sender, IInspectable *args)noexcept(true);
	};
	cTypedEventHandler<cBLEDeviceConnectionStatusHandler,BLEDevice*,IInspectable*> fBLEDeviceConnectionStatusHandler;
	class cBLEDeviceServicesChangedHandler
	{
		cBluetoothSlave* GetHost(void)noexcept(true);
	public:
		void EventIncReference(void)noexcept(true);
		void EventDecReference(void)noexcept(true);
		void EventInvoke(IBLEDevice *sender, IInspectable *args)noexcept(true);
	};
	cTypedEventHandler<cBLEDeviceServicesChangedHandler,BLEDevice*,IInspectable*> fBLEDeviceServicesChangedHandler;

	EventRegistrationToken fBLEDeviceNameChangedToken;
	EventRegistrationToken fBLEDeviceConnectionStatusToken;
	EventRegistrationToken fBLEDeviceServicesChangedToken;
		
	class cGetServicesCompleteHandler
	{
		cBluetoothSlave* GetHost(void)noexcept(true);
	public:
		void EventIncReference(void)noexcept(true);
		void EventDecReference(void)noexcept(true);
		void EventInvoke(IBLEGetServiceAsyncOp *AsyncOp,ABI::Windows::Foundation::AsyncStatus)noexcept(true);
	};
	cAsyncOperationCompletedHandler<cGetServicesCompleteHandler,GattDeviceServiceResult*> fGetServicesCompleteHandler;

	cSeqMap<cUUID,cGATTService*> fServiceMap;
};
//---------------------------------------------------------------------------
class cBluetoothLEAdvertisementObserver : public iBluetoothLEAdvertisementObserver, public bcAsyncQueue, public cDualReference
{
	typedef ABI::Windows::Devices::Bluetooth::Advertisement::BluetoothLEAdvertisementWatcher BLEWatcher;
	typedef ABI::Windows::Devices::Bluetooth::Advertisement::IBluetoothLEAdvertisementWatcher IBLEWatcher;

public:
	cBluetoothLEAdvertisementObserver(cBluetoothCentral *Central,COMPtr<IBLEWatcher> Watcher)noexcept(true);
	~cBluetoothLEAdvertisementObserver()noexcept(true);

	virtual bool cnLib_FUNC StartNotify(iReference *Reference,iAsyncNotificationCallback *Callback)noexcept(true)override;
	virtual void cnLib_FUNC StopNotify(void)noexcept(true)override;
	virtual void cnLib_FUNC NotifyCallback(bool IdleNotify)noexcept(true)override;
	virtual bool cnLib_FUNC IsClosed(void)noexcept(true)override;


	virtual iDispatch* cnLib_FUNC GetHandlerDispatch(void)noexcept(true)override;
	virtual iBluetoothCentral* cnLib_FUNC GetCentral(void)noexcept(true)override;

	virtual void cnLib_FUNC DiscardQueue(void)noexcept(true)override;
	virtual rPtr<iReference> cnLib_FUNC Fetch(cBluetoothLEAdvertisementInfo &Info)noexcept(true)override;

protected:
	void VirtualStarted(void)noexcept(true);
	void VirtualStopped(void)noexcept(true);

	rPtr<cBluetoothCentral> fCentral;
	COMPtr<IBLEWatcher> fWatcher;

	rPtr<iReference> fCallbackReference;
	iAsyncNotificationCallback *fCallback;
	virtual iReference* NotificationInnerReference(void)noexcept(true)override;
	virtual void NotificationStarted(void)noexcept(true)override;
	virtual void NotificationStopped(void)noexcept(true)override;
	//virtual void NotificationClosed(void)=0;
	//virtual CycleState NotificationCheckState(void)override;
	//virtual void NotificationProcedure(void)=0;
	virtual void AsyncQueueNotify(void)noexcept(true)override;


private:
	typedef ABI::Windows::Foundation::ITypedEventHandler<
		BLEWatcher*,
		ABI::Windows::Devices::Bluetooth::Advertisement::BluetoothLEAdvertisementReceivedEventArgs*
	> IBLEWatcherHandler;

	class cBLEReceivedHandler : public IBLEWatcherHandler
	{
		cBluetoothLEAdvertisementObserver* GetHost(void)noexcept(true);
	public:
		virtual HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid,_COM_Outptr_ void __RPC_FAR *__RPC_FAR *ppvObject)noexcept(true)override;

		virtual ULONG STDMETHODCALLTYPE AddRef(void)noexcept(true)override;
		virtual ULONG STDMETHODCALLTYPE Release(void)noexcept(true)override;
		virtual HRESULT STDMETHODCALLTYPE Invoke(IBLEWatcher *sender, _In_ ABI::Windows::Devices::Bluetooth::Advertisement::IBluetoothLEAdvertisementReceivedEventArgs *args)noexcept(true)override;
	}fBLEReceivedHandler;
	EventRegistrationToken fBLEReceivedToken;
	
	class cAdvertisementData : public iReference
	{
	public:
		cAdvertisementData *Next;

		iPtr<cBluetoothAddress> Address;
		cString<uChar16> LocalName;
		cSeqList<cUUID> ServiceUUIDs;
		cSeqList<cBluetoothLEAdvertisementManufacturerData> ManufacturerDataList;
		cSeqList<cBluetoothLEAdvertisementSectionData> SectionDataList;
		cMemoryBuffer ManufacturerDataBuffer;
		cMemoryBuffer SectionDataBuffer;
		uInt64 Timestamp;
		INT16 SignalStrength;
		boolean IsConnectable;
		boolean IsScannable;

		eBluetoothLEAdvertisementType Type;
		eBluetoothLEAdvertisementFlags Flags;
	};
	cAtomicQueueSO<cAdvertisementData> fAdvertisementQueue;
	void BLEReceived(rPtr<cAdvertisementData> AdvData)noexcept(true);
	rObjectRecycler<cAdvertisementData> fAdvDataRecycler;
};
//---------------------------------------------------------------------------
class cBluetoothCentral : public iBluetoothCentral
{
	typedef ABI::Windows::Devices::Bluetooth::Advertisement::BluetoothLEAdvertisementWatcher BLEWatcher;
	typedef ABI::Windows::Devices::Bluetooth::Advertisement::IBluetoothLEAdvertisementWatcher IBLEWatcher;
	typedef ABI::Windows::Devices::Bluetooth::IBluetoothLEDeviceStatics IBLEDeviceStatics;
	typedef ABI::Windows::Foundation::IAsyncOperation<ABI::Windows::Devices::Bluetooth::BluetoothLEDevice*> IBLEConnectAsyncOp;
public:
	cBluetoothCentral(iDispatch *Dispatch,COMPtr<IBLEDeviceStatics> BLEStatic)noexcept(true);

	IBLEDeviceStatics* GetBLEStatic(void)const noexcept(true);
	iDispatch* GetDispatch(void)const noexcept(true);



	virtual iDispatch* cnLib_FUNC GetHandlerDispatch(void)noexcept(true)override;
	virtual bool cnLib_FUNC InsertHandler(iBluetoothCentralHandler *Handler)noexcept(true)override;
	virtual bool cnLib_FUNC RemoveHandler(iBluetoothCentralHandler *Handler)noexcept(true)override;
	virtual bool cnLib_FUNC IsEnabled(void)noexcept(true)override;
	virtual bool cnLib_FUNC IsPowerOn(void)noexcept(true)override;

	virtual rPtr<iBluetoothLEAdvertisementObserver> cnLib_FUNC CreateLEAdvertisementObserver(void)noexcept(true)override;

	//virtual rPtr<iGATTServer> cnLib_FUNC CreateServer(iReference *ControllerReference,iGATTServerController *Controller)noexcept(true)=0;
	virtual rPtr<iBluetoothSlave> cnLib_FUNC OpenPeripheral(iAddress *Address)noexcept(true)override;

protected:
	COMPtr<IBLEDeviceStatics> fBLEStatic;
	iPtr<iDispatch> fDispatch;

};
//---------------------------------------------------------------------------
class cGATTServer;
class cGATTServerService;
class cGATTServerCharacteristic;
//---------------------------------------------------------------------------
class cGATTServerDescriptor : public iGATTServerDescriptor, public cDualReference
{
	typedef ABI::Windows::Devices::Bluetooth::GenericAttributeProfile::GattLocalDescriptor GattLocalDescriptor;
	typedef ABI::Windows::Devices::Bluetooth::GenericAttributeProfile::IGattLocalDescriptor IGattLocalDescriptor;
	typedef ABI::Windows::Devices::Bluetooth::GenericAttributeProfile::GattLocalDescriptorParameters GattLocalDescriptorParameters;
	typedef ABI::Windows::Devices::Bluetooth::GenericAttributeProfile::IGattLocalDescriptorParameters IGattLocalDescriptorParameters;

	typedef ABI::Windows::Devices::Bluetooth::GenericAttributeProfile::GattLocalDescriptorResult GattLocalDescriptorResult;
	typedef ABI::Windows::Devices::Bluetooth::GenericAttributeProfile::IGattLocalDescriptorResult IGattLocalDescriptorResult;
	typedef ABI::Windows::Foundation::IAsyncOperation<GattLocalDescriptorResult*> ICreateLocalDescAsyncOp;
public:
	cGATTServerDescriptor(cGATTServerCharacteristic *Owner,const cUUID &ID,iReference *Reference,iGATTDescriptorController *Controller)noexcept(true);

	const cUUID &GetID(void)const noexcept;
	iDispatch* GetDispatch(void)const noexcept(true);

	virtual iGATTServerCharacteristic* cnLib_FUNC GetOwner(void)noexcept(true)override;
	virtual iDispatch* cnLib_FUNC GetHandlerDispatch(void)noexcept(true)override;
	virtual void cnLib_FUNC Shutdown(void)noexcept(true)override;

	void ShutdownDescriptor(void)noexcept(true);

	void CharacteristicNotifyState(bool Active)noexcept(true);
	void CharacteristicNotifyShutdown(void)noexcept(true);
protected:
	void VirtualStarted(void)noexcept(true);
	void VirtualStopped(void)noexcept(true);

	rPtr<cGATTServerCharacteristic> fOwner;
	cUUID fDescriptorID;
	iPtr<iDispatch> fDispatch;
	COMPtr<IGattLocalDescriptor> fDescriptor;
	rPtr<iReference> fControllerReference;
	iGATTDescriptorController *fController;

private:

	rPtr<iAsyncProcedure> fMainProcessWork;

	cExclusiveFlag fMainProcessExclusiveFlag;
	bool fCharacteristicActive=false;
	bool fNeedNotifyActiveStateChange=false;
	bool fShutdown=false;
	enum{
		gsNone,			// no gatt interface
		gsCreating,		// gatt creating interface
		gsCreateResult,	// need process gatt create interface result
		gsCreated,		// gatt interface created
		gsShutdown
	};
	ufInt8 fUWPGattState=gsNone;

	void SetupDescriptor(void)noexcept(true);
	void ClearDescriptor(void)noexcept(true);

	void RunMainProcess(void)noexcept(true);
	void NotifyMainProcess(void)noexcept(true);
	
	class cMainProcessProcedure : public iProcedure
	{
		virtual void cnLib_FUNC Execute(void)noexcept(true)override;
	}fMainProcessProcedure;
	void MainProcessThread(void)noexcept(true);

	bool MainProcess(void)noexcept(true);
	bool MainProcess_CreateDescriptor(void)noexcept(true);

	class cCreateLocalDescCompleteHandler
	{
		cGATTServerDescriptor* GetHost(void)noexcept(true);
	public:
		void EventIncReference(void)noexcept(true);
		void EventDecReference(void)noexcept(true);
		void EventInvoke(ICreateLocalDescAsyncOp *AsyncOp,ABI::Windows::Foundation::AsyncStatus)noexcept(true);

		HRESULT hrResult;
		COMPtr<IGattLocalDescriptorResult> Result;
	};
	cAsyncOperationCompletedHandler<cCreateLocalDescCompleteHandler,GattLocalDescriptorResult*> fCreateLocalDescCompleteHandler;

};
//---------------------------------------------------------------------------
class cGATTServerCharacteristic : public iGATTServerCharacteristic, public cDualReference
{
	typedef ABI::Windows::Devices::Bluetooth::GenericAttributeProfile::GattLocalCharacteristic GattLocalCharacteristic;
	typedef ABI::Windows::Devices::Bluetooth::GenericAttributeProfile::IGattLocalCharacteristic IGattLocalCharacteristic;
	typedef ABI::Windows::Devices::Bluetooth::GenericAttributeProfile::GattLocalCharacteristicParameters GattLocalCharacteristicParameters;
	typedef ABI::Windows::Devices::Bluetooth::GenericAttributeProfile::IGattLocalCharacteristicParameters IGattLocalCharacteristicParameters;

	typedef ABI::Windows::Devices::Bluetooth::GenericAttributeProfile::GattLocalCharacteristicResult GattLocalCharacteristicResult;
	typedef ABI::Windows::Devices::Bluetooth::GenericAttributeProfile::IGattLocalCharacteristicResult IGattLocalCharacteristicResult;
	typedef ABI::Windows::Foundation::IAsyncOperation<GattLocalCharacteristicResult*> ICreateLocalCharAsyncOp;

	typedef ABI::Windows::Devices::Bluetooth::GenericAttributeProfile::GattCommunicationStatus GattCommunicationStatus;

	typedef ABI::Windows::Devices::Bluetooth::GenericAttributeProfile::GattReadRequestedEventArgs GattReadRequestedEventArgs;
	typedef ABI::Windows::Devices::Bluetooth::GenericAttributeProfile::IGattReadRequestedEventArgs IGattReadRequestedEventArgs;
	typedef ABI::Windows::Devices::Bluetooth::GenericAttributeProfile::GattWriteRequestedEventArgs GattWriteRequestedEventArgs;
	typedef ABI::Windows::Devices::Bluetooth::GenericAttributeProfile::IGattWriteRequestedEventArgs IGattWriteRequestedEventArgs;

	typedef ABI::Windows::Foundation::IDeferral IDeferral;
	typedef ABI::Windows::Devices::Bluetooth::GenericAttributeProfile::IGattSession IGattSession;
	typedef ABI::Windows::Devices::Bluetooth::IBluetoothDeviceId IBluetoothDeviceId;
	
	typedef ABI::Windows::Devices::Bluetooth::GenericAttributeProfile::GattSubscribedClient GattSubscribedClient;
	typedef ABI::Windows::Devices::Bluetooth::GenericAttributeProfile::IGattSubscribedClient IGattSubscribedClient;

	typedef ABI::Windows::Devices::Bluetooth::GenericAttributeProfile::GattReadRequest GattReadRequest;
	typedef ABI::Windows::Devices::Bluetooth::GenericAttributeProfile::IGattReadRequest IGattReadRequest;
	typedef ABI::Windows::Devices::Bluetooth::GenericAttributeProfile::GattWriteRequest GattWriteRequest;
	typedef ABI::Windows::Devices::Bluetooth::GenericAttributeProfile::IGattWriteRequest IGattWriteRequest;
	
	typedef ABI::Windows::Foundation::IAsyncOperation<GattReadRequest*> IGetReadRequestAsyncOp;
	typedef ABI::Windows::Foundation::IAsyncOperation<GattWriteRequest*> IGetWriteRequestAsyncOp;

	typedef ABI::Windows::Devices::Bluetooth::GenericAttributeProfile::GattClientNotificationResult GattClientNotificationResult;
	typedef ABI::Windows::Devices::Bluetooth::GenericAttributeProfile::IGattClientNotificationResult IGattClientNotificationResult;
	typedef ABI::Windows::Foundation::IAsyncOperation<GattClientNotificationResult*> IClientNotifyValueAsyncOp;

public:
	cGATTServerCharacteristic(cGATTServerService *Owner,const cUUID &ID,iReference *Reference,iGATTCharacteristicController *Controller)noexcept(true);

	IGattLocalCharacteristic* GetLocalCharacteristic(void)const noexcept(true);
	const cUUID &GetID(void)const noexcept;
	iDispatch* GetDispatch(void)const noexcept(true);
	void ShutdownCharacteristic(void)noexcept(true);

	virtual iGATTServerService* cnLib_FUNC GetOwner(void)noexcept(true)override;
	virtual iDispatch* cnLib_FUNC GetHandlerDispatch(void)noexcept(true)override;
	virtual rPtr<iGATTServerDescriptor> cnLib_FUNC CreateGATTDescriptor(const cUUID &ID,iReference *Reference,iGATTDescriptorController *Controller)noexcept(true)override;

	virtual void cnLib_FUNC Shutdown(void)noexcept(true)override;
	virtual void cnLib_FUNC NotifyValue(iGATTServerSubscription *Subscription)noexcept(true)override;
	virtual iGATTServerSubscription* cnLib_FUNC QuerySubscription(iGATTServerConnection *Connection)noexcept(true)override;

	void DescriptorNotifyShutdown(cGATTServerDescriptor *Descriptor)noexcept(true);

	void ServiceNotifyState(bool Active)noexcept(true);
	void ServiceNotifyPublish(void)noexcept(true);
	void ServiceNotifyShutdown(void)noexcept(true);

protected:
	void VirtualStarted(void)noexcept(true);
	void VirtualStopped(void)noexcept(true);

	rPtr<cGATTServerService> fOwner;
	cUUID fCharacteristicID;
	COMPtr<IGattLocalCharacteristic> fCharacteristic;
	iPtr<iDispatch> fDispatch;
	rPtr<iReference> fControllerReference;
	iGATTCharacteristicController *fController;
	cSeqMap< cUUID,rInnerPtr<cGATTServerDescriptor> > fDescriptorMap;
	
private:

	rPtr<iAsyncProcedure> fMainProcessWork;
	rPtr<iAsyncProcedure> fRequestProcessWork;

	enum{
		 vnIdle,
		 vnCommand,
		 vnAwait,
	};

	cExclusiveFlag fMainProcessExclusiveFlag;
	cExclusiveFlag fRequestProcessExclusiveFlag;
	bool fCharacteristicActive=false;
	bool fCharacteristicPublished=false;
	bool fNeedNotifyActiveStateChange=false;
	bool fServiceActive=false;
	bool fShutdown=false;
	bool fReleaseGatt=false;

	enum{
		gsNone,			// no gatt interface
		gsCreating,		// gatt creating interface
		gsCreateResult,	// need process gatt create interface result
		gsCreated,		// gatt interface created
		gsDestroyTree,	// release gatt interface tree
		gsShutdownTree,	// shutdown and wait for descriptors
		gsShutdown
	};
	ufInt8 fUWPGattState=gsNone;

	bool fValueNotifyUpdate=false;
	bool fBroadcastValueNotify=false;
	ufInt8 fBroadcastValueNotifyState=0;
	bool fSubscribedClientChanged=false;

	void SetupCharacteristic(void)noexcept(true);
	void ClearCharacteristic(void)noexcept(true);

	void RunMainProcess(void)noexcept(true);
	void NotifyMainProcess(void)noexcept(true);

	class cMainProcessProcedure : public iProcedure
	{
		virtual void cnLib_FUNC Execute(void)noexcept(true)override;
	}fMainProcessProcedure;
	void MainProcessThread(void)noexcept(true);

	bool MainProcess(void)noexcept(true);

	bool MainProcess_CreateCharacteristic(void)noexcept(true);


	class cConnection : public iGATTServerConnection
	{
	public:
		COMPtr<IGattSession> Session;

		virtual eiOrdering cnLib_FUNC Compare(iGATTServerConnection *Dest)noexcept(true)override{
			return iOrdering::Different;
		}

		virtual uIntn cnLib_FUNC GetMaxValueLength(void)noexcept(true)override{
			return 20;
		}
	};

	class cSubscription : public iGATTServerSubscription, public iReference
	{
	public:
		rInnerPtr<cGATTServerCharacteristic> Owner;
		COMPtr<IGattSubscribedClient> Client;

		virtual rPtr<iGATTServerConnection> cnLib_FUNC GetConnection(void)noexcept(true)override{return nullptr;}

		bool NeedUpdateValue=false;
		ufInt8 UpdateValueState=0;

		class cClientNotifyValueCompleteHandler
		{
			cSubscription* GetHost(void)noexcept(true);
		public:
			void EventIncReference(void)noexcept(true);
			void EventDecReference(void)noexcept(true);
			void EventInvoke(IClientNotifyValueAsyncOp *AsyncOp,ABI::Windows::Foundation::AsyncStatus)noexcept(true);
		};
		cAsyncOperationCompletedHandler<cClientNotifyValueCompleteHandler,GattClientNotificationResult*> ClientNotifyValueCompleteHandler;
	};

	cnRTL::cSeqMap< cHString,rPtr<cSubscription> > fSubscribedClientMap;

	rPtr<cConnection> QueryClient(IGattSession *Session)noexcept(true);

	void MainProcess_ValueBroadcastNotify(ABI::Windows::Storage::Streams::IDataWriter *DataWriter)noexcept(true);
	void MainProcess_ValueNotify(cSubscription *Subscription,ABI::Windows::Storage::Streams::IDataWriter *DataWriter)noexcept(true);

	struct cReadRequestItem
	{
		arCls<cReadRequestItem> *Next;
		COMPtr<IGattSession> Session;
		COMPtr<IDeferral> Deferal;
		COMPtr<IGattReadRequest> Request;

		class cGetReadRequestCompleteHandler
		{
		public:
			cGATTServerCharacteristic *Owner;
			void EventIncReference(void)noexcept(true);
			void EventDecReference(void)noexcept(true);
			void EventInvoke(IGetReadRequestAsyncOp *AsyncOp,ABI::Windows::Foundation::AsyncStatus)noexcept(true);
		};
		cAsyncOperationCompletedHandler<cGetReadRequestCompleteHandler,GattReadRequest*> GetReadRequestCompleteHandler;
	};
	cnRTL::cAtomicQueueSO< arCls<cReadRequestItem> > fReadRequestQueue;
	cnRTL::arClsSharedObjectRecycler<cReadRequestItem> fReadRequestItemRecycler;

	struct cWriteRequestItem
	{
		arCls<cWriteRequestItem> *Next;
		COMPtr<IGattSession> Session;
		COMPtr<IDeferral> Deferal;
		COMPtr<IGattWriteRequest> Request;

		class cGetWriteRequestCompleteHandler
		{
		public:
			cGATTServerCharacteristic *Owner;
			void EventIncReference(void)noexcept(true);
			void EventDecReference(void)noexcept(true);
			void EventInvoke(IGetWriteRequestAsyncOp *AsyncOp,ABI::Windows::Foundation::AsyncStatus)noexcept(true);
		};
		cAsyncOperationCompletedHandler<cGetWriteRequestCompleteHandler,GattWriteRequest*> GetWriteRequestCompleteHandler;
	};
	cnRTL::cAtomicQueueSO< arCls<cWriteRequestItem> > fWriteRequestQueue;
	cnRTL::arClsSharedObjectRecycler<cWriteRequestItem> fWriteRequestItemRecycler;

	class cRequestProcessProcedure : public iProcedure
	{
		virtual void cnLib_FUNC Execute(void)noexcept(true)override;
	}fRequestProcessProcedure;
	void RequestProcessThread(void)noexcept(true);
	void NotifyRequestProcess(void)noexcept(true);

	void RequestProcess(void)noexcept(true);
	
	class cReadRequestedHandler
	{
		cGATTServerCharacteristic* GetHost(void)noexcept(true);
	public:
		void EventIncReference(void)noexcept(true);
		void EventDecReference(void)noexcept(true);
		void EventInvoke(IGattLocalCharacteristic *sender,IGattReadRequestedEventArgs *args)noexcept(true);
	};
	cTypedEventHandler<cReadRequestedHandler,GattLocalCharacteristic*,GattReadRequestedEventArgs*> fReadRequestedHandler;

	class cWriteRequestedHandler
	{
		cGATTServerCharacteristic* GetHost(void)noexcept(true);
	public:
		void EventIncReference(void)noexcept(true);
		void EventDecReference(void)noexcept(true);
		void EventInvoke(IGattLocalCharacteristic *sender,IGattWriteRequestedEventArgs *args)noexcept(true);
	};
	cTypedEventHandler<cWriteRequestedHandler,GattLocalCharacteristic*,GattWriteRequestedEventArgs*> fWriteRequestedHandler;

	class cSubscribedClientsChangedHandler
	{
		cGATTServerCharacteristic* GetHost(void)noexcept(true);
	public:
		void EventIncReference(void)noexcept(true);
		void EventDecReference(void)noexcept(true);
		void EventInvoke(IGattLocalCharacteristic *sender,IInspectable *args)noexcept(true);
	};
	cTypedEventHandler<cSubscribedClientsChangedHandler,GattLocalCharacteristic*,IInspectable*> fSubscribedClientsChangedHandler;

	EventRegistrationToken fReadRequestedEventToken;
	EventRegistrationToken fWriteRequestedEventToken;
	EventRegistrationToken fSubscribedClientsChangedEventToken;


	class cCreateLocalCharCompleteHandler
	{
		cGATTServerCharacteristic* GetHost(void)noexcept(true);
	public:
		void EventIncReference(void)noexcept(true);
		void EventDecReference(void)noexcept(true);
		void EventInvoke(ICreateLocalCharAsyncOp *AsyncOp,ABI::Windows::Foundation::AsyncStatus)noexcept(true);

		HRESULT hrResult;
		COMPtr<IGattLocalCharacteristicResult> Result;
	};
	cAsyncOperationCompletedHandler<cCreateLocalCharCompleteHandler,GattLocalCharacteristicResult*> fCreateLocalCharCompleteHandler;

};
//---------------------------------------------------------------------------
class cGATTServerService : public iGATTServerService, public cDualReference
{
	typedef ABI::Windows::Devices::Bluetooth::GenericAttributeProfile::GattServiceProvider GattServiceProvider;
	typedef ABI::Windows::Devices::Bluetooth::GenericAttributeProfile::IGattServiceProvider IGattServiceProvider;
	typedef ABI::Windows::Devices::Bluetooth::GenericAttributeProfile::GattLocalService GattLocalService;
	typedef ABI::Windows::Devices::Bluetooth::GenericAttributeProfile::IGattLocalService IGattLocalService;
	
	typedef ABI::Windows::Devices::Bluetooth::GenericAttributeProfile::GattServiceProviderAdvertisementStatus GattServiceProviderAdvertisementStatus;
	typedef ABI::Windows::Devices::Bluetooth::GenericAttributeProfile::IGattServiceProviderAdvertisingParameters IGattServiceProviderAdvertisingParameters;
	typedef ABI::Windows::Devices::Bluetooth::GenericAttributeProfile::GattServiceProviderAdvertisementStatusChangedEventArgs GattServiceProviderAdvertisementStatusChangedEventArgs;
	typedef ABI::Windows::Devices::Bluetooth::GenericAttributeProfile::IGattServiceProviderAdvertisementStatusChangedEventArgs IGattServiceProviderAdvertisementStatusChangedEventArgs;
	
	typedef ABI::Windows::Devices::Bluetooth::GenericAttributeProfile::GattServiceProviderResult GattServiceProviderResult;
	typedef ABI::Windows::Devices::Bluetooth::GenericAttributeProfile::IGattServiceProviderResult IGattServiceProviderResult;
	typedef ABI::Windows::Foundation::IAsyncOperation<GattServiceProviderResult*> ICreateServiceProviderAsyncOp;
public:
	cGATTServerService(cGATTServer *Owner,const cUUID &ID,iReference *Reference,iGATTServiceController *Controller)noexcept(true);

	IGattServiceProvider* GetServiceProvider(void)const noexcept(true);
	IGattLocalService* GetLocalService(void)const noexcept(true);
	const cUUID &GetID(void)const noexcept;
	iDispatch* GetDispatch(void)const noexcept(true);
	void ShutdownService(void)noexcept(true);

	virtual iDispatch* cnLib_FUNC GetHandlerDispatch(void)noexcept(true)override;
	virtual rPtr<iGATTServerCharacteristic> cnLib_FUNC CreateGATTCharacteristic(const cUUID &ID,iReference *Reference,iGATTCharacteristicController *Controller)noexcept(true)override;
	virtual void cnLib_FUNC Shutdown(void)noexcept(true)override;


	void CharacteristicNotifyShutdown(cGATTServerCharacteristic *Characteristic)noexcept(true);
	void ServerNotifyState(bool Active)noexcept(true);
	void ServerNotifyPublish(void)noexcept(true);
	void ServerNotifyAdvertisementState(bool Advertising)noexcept(true);
	void ServerNotifyShutdown(void)noexcept(true);
protected:
	void VirtualStarted(void)noexcept(true);
	void VirtualStopped(void)noexcept(true);
	rPtr<cGATTServer> fOwner;
	cUUID fServiceID;
	COMPtr<IGattServiceProvider> fServiceProvider;
	COMPtr<IGattLocalService> fService;
	iPtr<iDispatch> fDispatch;
	rPtr<iReference> fControllerReference;
	iGATTServiceController *fController;
	cSeqMap< cUUID,rInnerPtr<cGATTServerCharacteristic> > fCharacteristicMap;

private:

	rPtr<iAsyncProcedure> fMainProcessWork;

	cExclusiveFlag fMainProcessExclusiveFlag;
	bool fShutdown=false;
	bool fServiceActive=false;
	bool fServicePublished=false;
	bool fNeedNotifyActiveStateChange=false;
	bool fServerActive=false;
	enum{
		gsNone,			// no gatt interface
		gsCreating,		// gatt creating interface
		gsCreateResult,	// need process gatt create interface result
		gsCreated,		// gatt interface created
		gsLocked,		// gatt tree locked after first advertisement
		gsAdvertised,	// advertised
		gsShutdownTree,	// shutdown and wait for tree
		gsShutdown
	};
	ufInt8 fUWPGattState=gsNone;

	bool fAdvertisementActivated=false;
	bool fGattAdvStarted=false;
	bool fServerIsAdvertising=false;

	void NotifyMainProcess(void)noexcept(true);
	void RunMainProcess(void)noexcept(true);
	class cMainProcessProcedure : public iProcedure
	{
		virtual void cnLib_FUNC Execute(void)noexcept(true)override;
	}fMainProcessProcedure;
	void MainProcessThread(void)noexcept(true);

	bool MainProcess(void)noexcept(true);

	void MainProcess_ShutdownTree(void)noexcept(true);
	void ProcessAdvertisement(void)noexcept(true);

	class cCreateServiceProviderCompleteHandler
	{
		cGATTServerService* GetHost(void)noexcept(true);
	public:
		void EventIncReference(void)noexcept(true);
		void EventDecReference(void)noexcept(true);
		void EventInvoke(ICreateServiceProviderAsyncOp *AsyncOp,ABI::Windows::Foundation::AsyncStatus)noexcept(true);

		HRESULT hrResult;
		COMPtr<IGattServiceProviderResult> Result;
	};
	cAsyncOperationCompletedHandler<cCreateServiceProviderCompleteHandler,GattServiceProviderResult*> fCreateServiceProviderCompleteHandler;

	class cAdvertisementStatusChangedHandler
	{
		cGATTServerService* GetHost(void)noexcept(true);
	public:
		void EventIncReference(void)noexcept(true);
		void EventDecReference(void)noexcept(true);
		void EventInvoke(IGattServiceProvider *sender,IGattServiceProviderAdvertisementStatusChangedEventArgs *args)noexcept(true);
	};
	cTypedEventHandler<cAdvertisementStatusChangedHandler,GattServiceProvider*,GattServiceProviderAdvertisementStatusChangedEventArgs*> fAdvertisementStatusChangedHandler;

	EventRegistrationToken fAdvertisementStatusChangedToken;
};
//---------------------------------------------------------------------------
class cGATTServer : public iGATTServer, public cDualReference
{
	typedef ABI::Windows::Devices::Bluetooth::GenericAttributeProfile::GattServiceProvider GattServiceProvider;
	typedef ABI::Windows::Devices::Bluetooth::GenericAttributeProfile::IGattServiceProvider IGattServiceProvider;
	
	typedef ABI::Windows::Devices::Bluetooth::GenericAttributeProfile::IGattServiceProviderStatics IGattServiceProviderStatics;
public:
	class iServerOwner
	{
	public:
		virtual void ServerClosed(cGATTServer *Server)noexcept(true)=0;
		virtual bool OwnerIsActive(void)noexcept(true)=0;
		virtual bool OwnerIsAdvertising(void)noexcept(true)=0;
	};
	cGATTServer(iServerOwner *Owner,iDispatch *Dispatch,COMPtr<IGattServiceProviderStatics> ProviderStatic,iReference *ControllerReference,iGATTServerController *Controller)noexcept(true);
	~cGATTServer();

	IGattServiceProviderStatics* GetProviderStatic(void)const noexcept(true);
	iDispatch* GetDispatch(void)const noexcept(true);

	virtual iDispatch* cnLib_FUNC GetHandlerDispatch(void)noexcept(true)override;
	virtual void cnLib_FUNC Shutdown(void)noexcept(true)override;
	virtual void cnLib_FUNC Publish(void)noexcept(true)override;
	virtual rPtr<iGATTServerService> cnLib_FUNC CreateGATTService(const cUUID &ID,iReference *Reference,iGATTServiceController *Controller)noexcept(true)override;

	void ServiceNotifyShutdown(cGATTServerService *Service)noexcept(true);

	void ShutdownServer(void)noexcept(true);

	void OwnerNotifyStateChange(void)noexcept(true);
	void OwnerNotifyAdvertisementState(void)noexcept(true);
protected:
	void VirtualStarted(void)noexcept(true);
	void VirtualStopped(void)noexcept(true);

	iServerOwner *fOwner;
	COMPtr<IGattServiceProviderStatics> fProviderStatic;
	iPtr<iDispatch> fDispatch;

private:

	rPtr<iReference> fControllerReference;
	iGATTServerController *fController;
	rPtr<iAsyncProcedure> fMainProcessWork;

	cSeqMap< cUUID,rInnerPtr<cGATTServerService> > fServiceMap;

	cExclusiveFlag fMainProcessExclusiveFlag;
	bool fShutdown=false;
	bool fServerActive=false;
	bool fServerPublished=false;
	bool fNeedPublish=false;
	bool fNeedNotifyServerStateChange=false;
	ufInt8 fShutdownState=0;

	void RunMainProcess(void)noexcept(true);
	void NotifyMainProcess(void)noexcept(true);

	class cMainProcessProcedure : public iProcedure
	{
		virtual void cnLib_FUNC Execute(void)noexcept(true)override;
	}fMainProcessProcedure;
	void MainProcessThread(void)noexcept(true);

	bool MainProcess(void)noexcept(true);

};
//---------------------------------------------------------------------------
class cBluetoothPeripheral : public iBluetoothPeripheral, private cGATTServer::iServerOwner
{
	typedef ABI::Windows::Devices::Bluetooth::GenericAttributeProfile::GattServiceProvider GattServiceProvider;
	typedef ABI::Windows::Devices::Bluetooth::GenericAttributeProfile::IGattServiceProvider IGattServiceProvider;
	
	typedef ABI::Windows::Devices::Bluetooth::GenericAttributeProfile::IGattServiceProviderStatics IGattServiceProviderStatics;
public:
	using iBluetoothPeripheral::IncreaseReference;
	using iBluetoothPeripheral::DecreaseReference;

	cBluetoothPeripheral(iDispatch *Dispatch,COMPtr<IGattServiceProviderStatics> ProviderStatic)noexcept(true);

	IGattServiceProviderStatics* GetProviderStatic(void)const noexcept(true);
	iDispatch* GetDispatch(void)const noexcept(true);

	virtual iDispatch* cnLib_FUNC GetHandlerDispatch(void)noexcept(true)override;
	virtual bool cnLib_FUNC InsertHandler(iBluetoothPeripheralHandler *Handler)noexcept(true)override;
	virtual bool cnLib_FUNC RemoveHandler(iBluetoothPeripheralHandler *Handler)noexcept(true)override;
	virtual bool cnLib_FUNC IsEnabled(void)noexcept(true)override;
	virtual bool cnLib_FUNC IsPowerOn(void)noexcept(true)override;

	virtual bool cnLib_FUNC IsAdvertising(void)noexcept(true)override;
	virtual bool cnLib_FUNC GetAdvertisementActive(void)noexcept(true)override;
	virtual void cnLib_FUNC SetAdvertisementActive(bool Active)noexcept(true)override;

	virtual rPtr<iGATTServer> cnLib_FUNC CreateServer(iReference *ControllerReference,iGATTServerController *Controller)noexcept(true)override;

protected:

	COMPtr<IGattServiceProviderStatics> fProviderStatic;
	iPtr<iDispatch> fDispatch;

private:
	virtual void ServerClosed(cGATTServer *Server)noexcept(true)override;
	virtual bool OwnerIsActive(void)noexcept(true)override;
	virtual bool OwnerIsAdvertising(void)noexcept(true)override;

	cSeqSet<iBluetoothPeripheralHandler*> fHandlers;
	rPtr<iAsyncProcedure> fMainProcessWork;

	cSeqSet< rInnerPtr<cGATTServer> > fServerSet;

	cExclusiveFlag fMainProcessExclusiveFlag;
	bool fNeedNotifyStateChange=false;
	bool fEffectiveAdvertisementActive=false;

	bool fAdvertisementActive=false;

	void NotifyMainProcess(void)noexcept(true);
	
	class cMainProcessProcedure : public iProcedure
	{
		virtual void cnLib_FUNC Execute(void)noexcept(true)override;
	}fMainProcessProcedure;
	void MainProcessThread(void)noexcept(true);

	bool MainProcess(void)noexcept(true);

};
//---------------------------------------------------------------------------
rPtr<iBluetoothCentral> OpenBluetoothCentral(iDispatch *Dispatch)noexcept(true);
rPtr<iBluetoothPeripheral> OpenBluetoothPeripheral(iDispatch *Dispatch)noexcept(true);
//---------------------------------------------------------------------------
#endif // >= _WIN32_WINNT_WIN10
//---------------------------------------------------------------------------
}	// namespace UWP
//---------------------------------------------------------------------------
}	// namespace cnRTL
//---------------------------------------------------------------------------
}	// namespace cnLibrary
//---------------------------------------------------------------------------