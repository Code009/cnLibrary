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
	cGATTDescriptor(cGATTCharacteristic *Owner,const cGATTUUID &ID)noexcept(true);
	~cGATTDescriptor()noexcept(true);

	cGATTUUID DescriptorUUID;

	virtual cGATTUUID cnLib_FUNC GetUUID(void)noexcept(true)override;
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
	typedef ABI::Windows::Devices::Bluetooth::GenericAttributeProfile::GattCommunicationStatus GattCommunicationStatus;
	typedef ABI::Windows::Foundation::IAsyncOperation<GattCommunicationStatus> IBLEWriteAsyncOp;

public:
	cGATTCharacteristic(cGATTService *Owner,const cGATTUUID & ID)noexcept(true);
	~cGATTCharacteristic()noexcept(true);
	
	iDispatch* GetDispatch(void)const noexcept(true);

	cGATTUUID CharacteristcUUID;

	virtual cGATTUUID cnLib_FUNC GetUUID(void)noexcept(true)override;
	virtual iDispatch* cnLib_FUNC GetHandlerDispatch(void)noexcept(true)override;
	virtual bool cnLib_FUNC InsertHandler(iGATTCharacteristicHandler *Handler)noexcept(true)override;
	virtual bool cnLib_FUNC RemoveHandler(iGATTCharacteristicHandler *Handler)noexcept(true)override;

	virtual eGATTFunctionState cnLib_FUNC GetFunctionState(void)noexcept(true)override;
	virtual iGATTService* cnLib_FUNC GetService(void)noexcept(true)override;
	virtual rPtr<iGATTDescriptor> cnLib_FUNC AccessDescriptor(const cGATTUUID &ID)noexcept(true)override;
	virtual rPtr<iGATTDescriptorObserver> cnLib_FUNC CreateDescriptorObserver(void)noexcept(true)override;

	virtual rPtr< iArrayReference<const void> > cnLib_FUNC Read(void)noexcept(true)override;

	virtual iPtr<iAsyncTask> cnLib_FUNC Write(const void *Data,uIntn DataSize)noexcept(true)override;
	virtual bool cnLib_FUNC WriteWithoutResponse(const void *Data,uIntn DataSize)noexcept(true)override;

	virtual iPtr< iAsyncFunction<bool> > cnLib_FUNC ConfigureNotification(eGATTCharacteristicNotification Notification)noexcept(true)override;

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

	rPtr<iAsyncProcedure> fMainProcessWork;

	class cMainProcessProcedure : public iProcedure
	{
		virtual void cnLib_FUNC Execute(void)noexcept(true)override;
	}fMainProcessProcedure;
	void MainProcessThread(void)noexcept(true);
	void NotifyMainProcess(void)noexcept(true);

	bool MainProcess(void)noexcept(true);
	void MainProcessStateChange(void)noexcept(true);
	void MainProcess_Idle(void)noexcept(true);
	void MainProcess_RefreshDescriptorDone(void)noexcept(true);

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


	cSeqMap< cGATTUUID,cGATTDescriptor*,cnDataStruct::cRAWDataItemOrderOperator<cGATTUUID> > fDescriptorMap;

	struct cDescriptorPair
	{
		cGATTDescriptor *Descriptor;
		COMPtr<IBLEDescriptor> BLEDescriptor;
	};
	cSeqMap< cGATTUUID,cDescriptorPair,cnDataStruct::cRAWDataItemOrderOperator<cGATTUUID> > fUpdateMap;

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
		void EventInvoke(IBLEWriteAsyncOp *AsyncOp,ABI::Windows::Foundation::AsyncStatus)noexcept(true);
	};
	cAsyncOperationCompletedHandler<cWriteCompleteHandler,GattCommunicationStatus> fWriteCompleteHandler;

	class cConfigureNotificationTask : public iAsyncFunction<bool>, public iReference
	{
	public:
		cConfigureNotificationTask(cGATTCharacteristic *Owner)noexcept(true);

		virtual bool cnLib_FUNC IsDone(void)noexcept(true)override;
		virtual bool cnLib_FUNC SetNotify(iProcedure *NotifyProcedure)noexcept(true)override;
		virtual void cnLib_FUNC Cancel(void)noexcept(true)override;
		virtual bool cnLib_FUNC GetResult(void)noexcept(true)override;

		bool Config(ABI::Windows::Devices::Bluetooth::GenericAttributeProfile::GattClientCharacteristicConfigurationDescriptorValue DescValue)noexcept(true);


	protected:
		rPtr<cGATTCharacteristic> fOwner;

		cAsyncTaskState fTaskState;
		GattCommunicationStatus fResult;

		void ConfigureDone(IBLEWriteAsyncOp *AsyncOp,ABI::Windows::Foundation::AsyncStatus)noexcept(true);

		class cConfigureNotificationHandler
		{
			cConfigureNotificationTask* GetHost(void)noexcept(true);
		public:
			void EventIncReference(void)noexcept(true);
			void EventDecReference(void)noexcept(true);
			void EventInvoke(IBLEWriteAsyncOp *AsyncOp,ABI::Windows::Foundation::AsyncStatus)noexcept(true);
		};
		cAsyncOperationCompletedHandler<cConfigureNotificationHandler,GattCommunicationStatus> fConfigureNotificationHandler;
	};
};
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
	cGATTService(cGATTPeripheral *Owner,const cGATTUUID & ID)noexcept(true);
	~cGATTService()noexcept(true);

	iDispatch* GetDispatch(void)const noexcept(true);

	virtual cGATTUUID cnLib_FUNC GetUUID(void)noexcept(true)override;
	virtual iDispatch* cnLib_FUNC GetHandlerDispatch(void)noexcept(true)override;
	virtual bool cnLib_FUNC InsertHandler(iGATTServiceHandler *Handler)noexcept(true)override;
	virtual bool cnLib_FUNC RemoveHandler(iGATTServiceHandler *Handler)noexcept(true)override;

	virtual eGATTFunctionState cnLib_FUNC GetFunctionState(void)noexcept(true)override;
	virtual iGATTPeripheral* cnLib_FUNC GetPeripheral(void)noexcept(true)override;
	virtual rPtr<iGATTCharacteristic> cnLib_FUNC AccessCharacteristic(const cGATTUUID &ID)noexcept(true)override;
	virtual iPtr<iGATTCharacteristicObserver> cnLib_FUNC CreateCharacteristicObserver(void)noexcept(true)override;

	cGATTUUID ServiceUUID;
	void PeripheralNotifyScanService(void)noexcept(true);
	void PeripheralNotifyConnectionStatus(void)noexcept(true);
	void PeripheralInvalidateService(void)noexcept(true);
	
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

		virtual iArrayReference< rPtr<iGATTCharacteristic> > cnLib_FUNC QueryCharacteristics(void)noexcept(true)override;
		virtual void cnLib_FUNC DiscardChanges(void)noexcept(true)override;
		virtual rPtr<iGATTCharacteristic> cnLib_FUNC FetchChanged(bool &IsInserted)noexcept(true)override;
	};

protected:
	void VirtualStarted(void)noexcept(true);
	void VirtualStopped(void)noexcept(true);

	rPtr<cGATTPeripheral> fOwner;
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


	cSeqMap< cGATTUUID,cGATTCharacteristic*,cnDataStruct::cRAWDataItemOrderOperator<cGATTUUID> > fCharacteristicMap;

	struct cCharacteristicPair
	{
		cGATTCharacteristic *Characteristic;
		COMPtr<IBLECharacteristic> BLECharacteristic;
	};
	cSeqMap< cGATTUUID,cCharacteristicPair,cnDataStruct::cRAWDataItemOrderOperator<cGATTUUID> > fUpdateMap;

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
	typedef ABI::Windows::Devices::Bluetooth::GenericAttributeProfile::IGattDeviceService3 IBLEService3;

	typedef ABI::Windows::Devices::Bluetooth::GenericAttributeProfile::GattDeviceServicesResult GattDeviceServiceResult;
	typedef ABI::Windows::Devices::Bluetooth::GenericAttributeProfile::IGattDeviceServicesResult IGattDeviceServiceResult;
	typedef ABI::Windows::Foundation::IAsyncOperation<BLEDevice*> IBLEConnectAsyncOp;
	typedef ABI::Windows::Foundation::IAsyncOperation<GattDeviceServiceResult*> IBLEGetServiceAsyncOp;
public:

	cGATTPeripheral(cGATTPeripheralCentral *Central,iPtr<cBluetoothAddress> Address)noexcept(true);
	~cGATTPeripheral()noexcept(true);

	iDispatch* GetDispatch(void)const noexcept(true);

	virtual iAddress* cnLib_FUNC GetPeripheralAddress(void)noexcept(true)override;
	virtual iDispatch* cnLib_FUNC GetHandlerDispatch(void)noexcept(true)override;
	virtual bool cnLib_FUNC InsertHandler(iGATTPeripheralHandler *Handler)noexcept(true)override;
	virtual bool cnLib_FUNC RemoveHandler(iGATTPeripheralHandler *Handler)noexcept(true)override;

	virtual eGATTFunctionState cnLib_FUNC GetFunctionState(void)noexcept(true)override;
	virtual iGATTPeripheralCentral* cnLib_FUNC GetPeripheralCentral(void)noexcept(true)override;
	virtual rPtr< iArrayReference<const uChar16> > cnLib_FUNC GetName(void)noexcept(true)override;
	virtual rPtr<iGATTService> cnLib_FUNC AccessService(const cGATTUUID &ID)noexcept(true)override;
	virtual iPtr<iGATTServiceObserver> cnLib_FUNC CreateServiceObserver(void)noexcept(true)override;


	void CentralConnectionOperation(COMPtr<IBLEConnectAsyncOp> ConnectOp)noexcept(true);
	
	eGATTFunctionState PeripheralGetFunctionState(void)noexcept(true);

protected:
	void VirtualStarted(void)noexcept(true);
	void VirtualStopped(void)noexcept(true);

	rPtr<cGATTPeripheralCentral> fCentral;
	iPtr<iDispatch> fDispatch;
	iPtr<cBluetoothAddress> fAddress;
	COMPtr<IBLEDevice> fDevice;
	COMPtr<IBLEDevice3> fDevice3;
	cString<uChar16> fDeviceName;


	cSeqSet<iGATTPeripheralHandler*> fHandlers;

	class cServiceObserver : public iGATTServiceObserver, public bcAsyncQueue
	{
	public:
		virtual bool cnLib_FUNC StartNotify(iReference *Reference,iAsyncNotificationCallback *Callback)noexcept(true)override;
		virtual void cnLib_FUNC StopNotify(void)noexcept(true)override;
		virtual void cnLib_FUNC NotifyCallback(bool IdleNotify)noexcept(true)override;
		virtual bool cnLib_FUNC IsClosed(void)noexcept(true)override;

		virtual iDispatch* cnLib_FUNC GetHandlerDispatch(void)noexcept(true)override;

		virtual iGATTPeripheral* cnLib_FUNC GetPeripheral(void)noexcept(true)override;

		virtual iArrayReference< rPtr<iGATTService> > cnLib_FUNC QueryServices(void)noexcept(true)override;
		virtual void cnLib_FUNC DiscardChanges(void)noexcept(true)override;
		virtual rPtr<iGATTService> cnLib_FUNC FetchChanged(bool &IsInserted)noexcept(true)override;
		virtual iPtr<iAsyncTask> cnLib_FUNC ScanServices(void)noexcept(true)override;
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
		cSeqMap< cGATTUUID,COMPtr<IBLEService>,cnDataStruct::cRAWDataItemOrderOperator<cGATTUUID> > Map;
	};
	aClsAtomicRef<cGetServicesResult> fGetServicesResult;

	cExclusiveFlag fMainProcessExclusiveFlag;
	cExclusiveFlag fQueryServiceExclusiveFlag;
	eGATTFunctionState fFuncState;
	bool fDeviceOpened;
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
		cGATTPeripheral* GetHost(void)noexcept(true);
	public:
		void EventIncReference(void)noexcept(true);
		void EventDecReference(void)noexcept(true);
		void EventInvoke(IBLEDeviceAsyncOp *AsyncOp,ABI::Windows::Foundation::AsyncStatus)noexcept(true);
	};
	cAsyncOperationCompletedHandler<cAddressConnectCompleteHandler,BLEDevice*> fAddressConnectCompleteHandler;

	class cBLEDeviceNameChangedHandler
	{
		cGATTPeripheral* GetHost(void)noexcept(true);
	public:
		void EventIncReference(void)noexcept(true);
		void EventDecReference(void)noexcept(true);
		void EventInvoke(IBLEDevice *sender, IInspectable *args)noexcept(true);
	};
	cTypedEventHandler<cBLEDeviceNameChangedHandler,BLEDevice*,IInspectable*> fBLEDeviceNameChangedHandler;
	class cBLEDeviceConnectionStatusHandler
	{
		cGATTPeripheral* GetHost(void)noexcept(true);
	public:
		void EventIncReference(void)noexcept(true);
		void EventDecReference(void)noexcept(true);
		void EventInvoke(IBLEDevice *sender, IInspectable *args)noexcept(true);
	};
	cTypedEventHandler<cBLEDeviceConnectionStatusHandler,BLEDevice*,IInspectable*> fBLEDeviceConnectionStatusHandler;
	class cBLEDeviceServicesChangedHandler
	{
		cGATTPeripheral* GetHost(void)noexcept(true);
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
		cGATTPeripheral* GetHost(void)noexcept(true);
	public:
		void EventIncReference(void)noexcept(true);
		void EventDecReference(void)noexcept(true);
		void EventInvoke(IBLEGetServiceAsyncOp *AsyncOp,ABI::Windows::Foundation::AsyncStatus)noexcept(true);
	};
	cAsyncOperationCompletedHandler<cGetServicesCompleteHandler,GattDeviceServiceResult*> fGetServicesCompleteHandler;

	cSeqMap< cGATTUUID,cGATTService*,cnDataStruct::cRAWDataItemOrderOperator<cGATTUUID> > fServiceMap;
};
//---------------------------------------------------------------------------
class cGATTAdvertisementObserver : public iGATTAdvertisementObserver, public bcAsyncQueue, public cDualReference
{
	typedef ABI::Windows::Devices::Bluetooth::Advertisement::BluetoothLEAdvertisementWatcher BLEWatcher;
	typedef ABI::Windows::Devices::Bluetooth::Advertisement::IBluetoothLEAdvertisementWatcher IBLEWatcher;

public:
	cGATTAdvertisementObserver(cGATTPeripheralCentral *Central,COMPtr<IBLEWatcher> Watcher)noexcept(true);
	~cGATTAdvertisementObserver()noexcept(true);

	virtual bool cnLib_FUNC StartNotify(iReference *Reference,iAsyncNotificationCallback *Callback)noexcept(true)override;
	virtual void cnLib_FUNC StopNotify(void)noexcept(true)override;
	virtual void cnLib_FUNC NotifyCallback(bool IdleNotify)noexcept(true)override;
	virtual bool cnLib_FUNC IsClosed(void)noexcept(true)override;


	virtual iDispatch* cnLib_FUNC GetHandlerDispatch(void)noexcept(true)override;

	virtual iGATTPeripheralCentral* cnLib_FUNC GetPeripheralCentral(void)noexcept(true)override;

	virtual void cnLib_FUNC DiscardQueue(void)noexcept(true)override;
	virtual rPtr<iGATTAdvertisement> cnLib_FUNC Fetch(void)noexcept(true)override;

protected:
	void VirtualStarted(void)noexcept(true);
	void VirtualStopped(void)noexcept(true);

	rPtr<cGATTPeripheralCentral> fCentral;
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
		cGATTAdvertisementObserver* GetHost(void)noexcept(true);
	public:
		virtual HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid,_COM_Outptr_ void __RPC_FAR *__RPC_FAR *ppvObject)noexcept(true)override;

		virtual ULONG STDMETHODCALLTYPE AddRef(void)noexcept(true)override;
		virtual ULONG STDMETHODCALLTYPE Release(void)noexcept(true)override;
		virtual HRESULT STDMETHODCALLTYPE Invoke(IBLEWatcher *sender, _In_ ABI::Windows::Devices::Bluetooth::Advertisement::IBluetoothLEAdvertisementReceivedEventArgs *args)noexcept(true)override;
	}fBLEReceivedHandler;
	EventRegistrationToken fBLEReceivedToken;


	class cAdvertisementData : public iGATTAdvertisement
	{
	public:
		cAdvertisementData *Next;
		virtual const cGATTAdvertisementInfo& cnLib_FUNC GetInfo(void)noexcept(true)override;

		cGATTAdvertisementInfo Info;
		iPtr<iAddress> Address;
		cString<uChar16> LocalName;
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
		uInt64 Timestamp;
	};
	cAtomicQueueSO<cAdvertisementData> fAdvertisementQueue;
	void BLEReceived(rPtr<cAdvertisementData> AdvData)noexcept(true);
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
	cGATTPeripheralCentral(iDispatch *Dispatch,COMPtr<IBLEDeviceStatics> BLEStatic)noexcept(true);

	iDispatch* GetDispatch(void)const noexcept(true);

	virtual iDispatch* cnLib_FUNC GetHandlerDispatch(void)noexcept(true)override;
	virtual bool cnLib_FUNC InsertHandler(iGATTPeripheralCentralHandler *Handler)noexcept(true)override;
	virtual bool cnLib_FUNC RemoveHandler(iGATTPeripheralCentralHandler *Handler)noexcept(true)override;

	virtual rPtr<iGATTPeripheral> cnLib_FUNC AccessPeripheral(iAddress *Address)noexcept(true)override;
	virtual rPtr<iGATTAdvertisementObserver> cnLib_FUNC CreateAdvertisementObserver(void)noexcept(true)override;

	virtual bool cnLib_FUNC IsEnabled(void)noexcept(true)override;
	virtual bool cnLib_FUNC IsPowerOn(void)noexcept(true)override;

protected:
	COMPtr<IBLEDeviceStatics> fBLEStatic;
	iPtr<iDispatch> fDispatch;

};

rPtr<iGATTPeripheralCentral> OpenGATTPeripheral(iDispatch *Dispatch)noexcept(true);

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