#include "WinUWPGATT.h"


#pragma comment(lib,"WindowsApp.lib")


using namespace cnLibrary;
using namespace cnRTL;
using namespace cnWinRTL;

#if _WIN32_WINNT >= _WIN32_WINNT_WIN10

using namespace cnRTL::UWP;


cnLib_INTERFACE_LOCALID_DEFINE(cBluetoothAddress);

//---------------------------------------------------------------------------
eiOrdering cBluetoothAddress::Compare(iAddress *Dest)
{
	auto DestAddr=iCast<cBluetoothAddress>(Dest);
	if(DestAddr==nullptr)
		return iOrdering::Different;

	if(Address==DestAddr->Address)
		return iOrdering::Equal;

	if(Address<DestAddr->Address)
		return iOrdering::Less;

	return iOrdering::Greater;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cGATTService::cGATTService(cGATTPeripheral *Peripheral)
	: fPeripheral(Peripheral)
{
}
//---------------------------------------------------------------------------
cGATTService::~cGATTService()
{
}
//---------------------------------------------------------------------------
cGATTUUID cGATTService::GetUUID(void)
{
	return ServiceUUID;
}
//---------------------------------------------------------------------------
iDispatch* cGATTService::GetHandlerDispatch(void)
{
	return nullptr;
}
//---------------------------------------------------------------------------
bool cGATTService::InsertHandler(iGATTServiceHandler *Handler)
{
	return false;
}
//---------------------------------------------------------------------------
bool cGATTService::RemoveHandler(iGATTServiceHandler *Handler)
{
	return false;
}
//---------------------------------------------------------------------------
eGATTFunctionState cGATTService::GetFunctionState(void)
{
	if(fService==nullptr){
		return GATTFunctionState::Absent;
	}
	return fPeripheral->PeripheralGetFunctionState();
}
//---------------------------------------------------------------------------
iGATTPeripheral* cGATTService::GetPeripheral(void)
{
	return fPeripheral;
}
//---------------------------------------------------------------------------
rPtr<iGATTCharacteristic> cGATTService::PersistCharacteristic(const cGATTUUID &ID)
{
	return nullptr;
}
//---------------------------------------------------------------------------
iPtr<iGATTCharacteristicAsyncFunction> cGATTService::QueryCharacteristics(void)
{
	return nullptr;
}
//---------------------------------------------------------------------------
void cGATTService::PeripheralNotifyConnectionStatus(void)
{
}
//---------------------------------------------------------------------------
void cGATTService::SetupService(COMPtr<IBLEService> Service)
{
	fService=cnVar::MoveCast(Service);
}
//---------------------------------------------------------------------------
void cGATTService::ClearService(void)
{
	fService=nullptr;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cGATTPeripheral::cGATTPeripheral(cGATTPeripheralCentral *Central,rPtr<bcConnector> Connector)
	: fCentral(Central)
	, fConnector(cnVar::MoveCast(Connector))
	, fActiveServiceMapMutex(cnSystem::CreateSharedLock())
{
	
}
//---------------------------------------------------------------------------
cGATTPeripheral::~cGATTPeripheral()
{
}
//---------------------------------------------------------------------------
void cGATTPeripheral::VirtualStarted(void)
{
	InnerIncReference('self');

	if(fConnector!=nullptr){
		fConnector->Setup(this);
	}
}
//---------------------------------------------------------------------------
void cGATTPeripheral::VirtualStopped(void)
{
	InnerDecReference('self');
}
//---------------------------------------------------------------------------
iDispatch* cGATTPeripheral::GetHandlerDispatch(void)
{
	return nullptr;
}
//---------------------------------------------------------------------------
bool cGATTPeripheral::InsertHandler(iGATTPeripheralHandler *Handler)
{
	fHandlers.Insert(Handler);
	return true;
}
//---------------------------------------------------------------------------
bool cGATTPeripheral::RemoveHandler(iGATTPeripheralHandler *Handler)
{
	fHandlers.Remove(Handler);
	return true;
}
//---------------------------------------------------------------------------
iAddress* cGATTPeripheral::GetPeripheralAddress(void)
{
	return fAddress;
}
//---------------------------------------------------------------------------
eGATTFunctionState cGATTPeripheral::GetFunctionState(void)
{
	return PeripheralGetFunctionState();
}
//---------------------------------------------------------------------------
eGATTFunctionState cGATTPeripheral::PeripheralGetFunctionState(void)
{
	if(fDevice==nullptr){
		return GATTFunctionState::Absent;
	}
	ABI::Windows::Devices::Bluetooth::BluetoothConnectionStatus Status;
	HRESULT hr;
	hr=fDevice->get_ConnectionStatus(&Status);
	if(FAILED(hr)){
		return GATTFunctionState::Absent;
	}
	switch(Status){
	case ABI::Windows::Devices::Bluetooth::BluetoothConnectionStatus::BluetoothConnectionStatus_Connected:
		return GATTFunctionState::Active;
	default:
	case ABI::Windows::Devices::Bluetooth::BluetoothConnectionStatus::BluetoothConnectionStatus_Disconnected:
		return GATTFunctionState::Inactive;
	}
}
//---------------------------------------------------------------------------
iGATTPeripheralCentral* cGATTPeripheral::GetPeripheralCentral(void)
{
	return fCentral;
}
//---------------------------------------------------------------------------
rPtr< iArrayReference<const uChar16> > cGATTPeripheral::GetName(void)
{
	if(fDevice==nullptr){
		if(fConnector==nullptr){
			return nullptr;
		}
		return fConnector->GetName();
	}
	cHString DeviceName;
	fDevice->get_Name(&DeviceName);

	cString<uChar16> Name=CreateStringFromHandle(DeviceName);
	return Name.Token();
}
//---------------------------------------------------------------------------
rPtr<iGATTService> cGATTPeripheral::AccessService(const cGATTUUID &ID)
{
	return nullptr;
}
//---------------------------------------------------------------------------
iPtr<iGATTServiceAsyncFunction> cGATTPeripheral::QueryServices(void)
{
	if(fDevice==nullptr)
		return nullptr;
	if(fDevice3!=nullptr){
		HRESULT hr;
		COMPtr<IBLEGetServiceAsyncOp> AsyncOp;
		hr=fDevice3->GetGattServicesAsync(COMRetPtrT(AsyncOp));
		if(FAILED(hr))
			return nullptr;

		auto Task=iCreate<cGetServicesTask>();
		if(Task->SetupAsyncOp(cnVar::MoveCast(AsyncOp))==false)
			return nullptr;
		return Task;
	}
	return nullptr;
}
//---------------------------------------------------------------------------
iPtr<iAsyncTask> cGATTPeripheral::Activate(void)
{
	return nullptr;
}
//---------------------------------------------------------------------------
void cGATTPeripheral::DeviceOpened(COMPtr<IBLEDevice> Device)
{
	fDevice=cnVar::MoveCast(Device);
	fDevice->QueryInterface(COMRetPtrT(fDevice3));
	fConnector=nullptr;
	SetupDevice();

	UpdateDeviceName();
	UpdateConnectionStatus();
	NotifyRSSIChanged();
}
//---------------------------------------------------------------------------
void cGATTPeripheral::DeviceOpenFailed(HRESULT hr)
{
	fConnector=nullptr;
}
//---------------------------------------------------------------------------
void cGATTPeripheral::SetupDevice(void)
{
	fDevice->add_NameChanged(&fBLEDeviceNameChangedHandler,&fBLEDeviceNameChangedToken);
	fDevice->add_ConnectionStatusChanged(&fBLEDeviceConnectionStatusHandler,&fBLEDeviceConnectionStatusToken);
	fDevice->add_GattServicesChanged(&fBLEDeviceServicesChangedHandler,&fBLEDeviceServicesChangedToken);
}
//---------------------------------------------------------------------------
void cGATTPeripheral::ClearDevice(void)
{
	fDevice->remove_NameChanged(fBLEDeviceNameChangedToken);
	fDevice->remove_ConnectionStatusChanged(fBLEDeviceConnectionStatusToken);
	fDevice->remove_GattServicesChanged(fBLEDeviceServicesChangedToken);
}
//---------------------------------------------------------------------------
void cGATTPeripheral::UpdateDeviceName(void)
{
	HRESULT hr;
	cHString DeviceName;
	hr=fDevice->get_Name(&DeviceName);
	if(SUCCEEDED(hr)){
		auto NameArray=DeviceName.GetArray();
		fDeviceName.SetString(wtou(NameArray.Pointer),NameArray.Length);
	}
	for(auto Handler : fHandlers){
		Handler->GATTPeripheralNameChanged();
	}
}
//---------------------------------------------------------------------------
void cGATTPeripheral::UpdateConnectionStatus(void)
{
	HRESULT hr;
	hr=fDevice->get_ConnectionStatus(&fConnectionStatus);
	if(FAILED(hr)){
		// error
	}
	for(auto Handler : fHandlers){
		Handler->GATTPeripheralStateChanged();
	}
}
//---------------------------------------------------------------------------
void cGATTPeripheral::NotifyServicesChanged(void)
{
	for(auto Handler : fHandlers){
		Handler->GATTPeripheralServicesChanged();
	}
}
//---------------------------------------------------------------------------
void cGATTPeripheral::NotifyRSSIChanged(void)
{
	for(auto Handler : fHandlers){
		Handler->GATTPeripheralRSSIChanged();
	}
}
//---------------------------------------------------------------------------
cGATTPeripheral* cGATTPeripheral::cBLEDeviceNameChangedHandler::GetHost(void)
{
	return cnMemory::GetObjectFromMemberPointer(this,&cGATTPeripheral::fBLEDeviceNameChangedHandler);
}
//---------------------------------------------------------------------------
void cGATTPeripheral::cBLEDeviceNameChangedHandler::EventIncReference(void)
{
	GetHost()->InnerIncReference('evet');
}
void cGATTPeripheral::cBLEDeviceNameChangedHandler::EventDecReference(void)
{
	GetHost()->InnerDecReference('evet');
}
void cGATTPeripheral::cBLEDeviceNameChangedHandler::EventInvoke(IBLEDevice *sender, IInspectable *args)
{
	GetHost()->UpdateDeviceName();
}
//---------------------------------------------------------------------------
cGATTPeripheral* cGATTPeripheral::cBLEDeviceConnectionStatusHandler::GetHost(void)
{
	return cnMemory::GetObjectFromMemberPointer(this,&cGATTPeripheral::fBLEDeviceConnectionStatusHandler);
}
//---------------------------------------------------------------------------
void cGATTPeripheral::cBLEDeviceConnectionStatusHandler::EventIncReference(void)
{
	GetHost()->InnerIncReference('evet');
}
//---------------------------------------------------------------------------
void cGATTPeripheral::cBLEDeviceConnectionStatusHandler::EventDecReference(void)
{
	GetHost()->InnerDecReference('evet');
}
//---------------------------------------------------------------------------
void cGATTPeripheral::cBLEDeviceConnectionStatusHandler::EventInvoke(IBLEDevice *sender, IInspectable *args)
{
	GetHost()->UpdateConnectionStatus();
}
//---------------------------------------------------------------------------
cGATTPeripheral* cGATTPeripheral::cBLEDeviceServicesChangedHandler::GetHost(void)
{
	return cnMemory::GetObjectFromMemberPointer(this,&cGATTPeripheral::fBLEDeviceServicesChangedHandler);
}
//---------------------------------------------------------------------------
void cGATTPeripheral::cBLEDeviceServicesChangedHandler::EventIncReference(void)
{
	GetHost()->InnerIncReference('evet');
}
//---------------------------------------------------------------------------
void cGATTPeripheral::cBLEDeviceServicesChangedHandler::EventDecReference(void)
{
	GetHost()->InnerDecReference('evet');
}
//---------------------------------------------------------------------------
void cGATTPeripheral::cBLEDeviceServicesChangedHandler::EventInvoke(IBLEDevice *sender, IInspectable *args)
{
	GetHost()->NotifyServicesChanged();
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cGATTPeripheral::cAddressConnector::cAddressConnector(COMPtr<IBLEDeviceAsyncOp> AsyncOp)
	: fAsyncOp(cnVar::MoveCast(AsyncOp))
{
}
//---------------------------------------------------------------------------
cGATTPeripheral::cAddressConnector::~cAddressConnector()
{
}
//---------------------------------------------------------------------------		
iAddress* cGATTPeripheral::cAddressConnector::GetAddress(void)
{
	return Address;
}
//---------------------------------------------------------------------------
rPtr< iArrayReference<const uChar16> > cGATTPeripheral::cAddressConnector::GetName(void)
{
	return nullptr;
}
//---------------------------------------------------------------------------
void cGATTPeripheral::cAddressConnector::Setup(cGATTPeripheral *Peripheral)
{
	fPeripheral=Peripheral;
	fAsyncOp->put_Completed(this);
}
//---------------------------------------------------------------------------
void cGATTPeripheral::cAddressConnector::Cancel(void)
{
}
//---------------------------------------------------------------------------
HRESULT STDMETHODCALLTYPE cGATTPeripheral::cAddressConnector::QueryInterface(REFIID riid,_COM_Outptr_ void __RPC_FAR *__RPC_FAR *ppvObject)
{
	if(riid==__uuidof(IUnknown)){
		rIncReference(this,'evet');
		*ppvObject=static_cast<IUnknown*>(this);
		return S_OK;
	}
	if(riid==__uuidof(IBLEDeviceAsyncOpCompletedHandler)){
		rIncReference(this,'evet');
		*ppvObject=static_cast<IBLEDeviceAsyncOpCompletedHandler*>(this);
		return S_OK;
	}
	*ppvObject=nullptr;
	return E_NOINTERFACE;
}
//---------------------------------------------------------------------------
ULONG STDMETHODCALLTYPE cGATTPeripheral::cAddressConnector::AddRef(void)
{
	rIncReference(this,'evet');
	return 0;
}
//---------------------------------------------------------------------------
ULONG STDMETHODCALLTYPE cGATTPeripheral::cAddressConnector::Release(void)
{
	rDecReference(this,'evet');
	return 0;
}
//---------------------------------------------------------------------------
HRESULT STDMETHODCALLTYPE cGATTPeripheral::cAddressConnector::Invoke(IBLEDeviceAsyncOp*,ABI::Windows::Foundation::AsyncStatus)
{
	HRESULT hr;
	COMPtr<IBLEDevice> ResultDevice;
	hr=fAsyncOp->GetResults(COMRetPtrT(ResultDevice));
	if(SUCCEEDED(hr) && ResultDevice!=nullptr){
		fPeripheral->DeviceOpened(cnVar::MoveCast(ResultDevice));
	}
	else{
		fPeripheral->DeviceOpenFailed(hr);
	}
	return S_OK;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cGATTPeripheral::cGetServicesTask::cGetServicesTask()
{
}
//---------------------------------------------------------------------------
cGATTPeripheral::cGetServicesTask::~cGetServicesTask()
{
}
//---------------------------------------------------------------------------
bool cGATTPeripheral::cGetServicesTask::IsDone(void)
{
	return fTaskState.IsDone();
}
//---------------------------------------------------------------------------
bool cGATTPeripheral::cGetServicesTask::SetNotify(iProcedure *NotifyProcedure)
{
	return fTaskState.SetNotify(NotifyProcedure);
}
//---------------------------------------------------------------------------
void cGATTPeripheral::cGetServicesTask::Cancel(void)
{
	return CancelAsyncOp();
}
//---------------------------------------------------------------------------
iArrayReference< rPtr<iGATTService> >* cGATTPeripheral::cGetServicesTask::GetResult(void)
{
	return nullptr;
}
//---------------------------------------------------------------------------
template<class TAllocationOperator,class TElement,class TVectorClass>
inline HRESULT CopyVectorView(cnDataStruct::cArrayStorage<TAllocationOperator,TElement> &Storage,ABI::Windows::Foundation::Collections::IVectorView<TVectorClass> *Vector)
{
	HRESULT hr;

	unsigned int ElementCount;
	hr=Vector->get_Size(&ElementCount);
	if(FAILED(hr))
		return hr;
		
	Storage.SetLength(ElementCount);
	hr=Vector->GetMany(0,ElementCount,Storage.Pointer,&ElementCount);
	if(FAILED(hr)){
		return hr;
	}

	Storage.SetLength(ElementCount);
	return S_OK;
}
//---------------------------------------------------------------------------
template<class TAllocationOperator,class TElement,class TVector>
inline HRESULT CopyVectorView(cnDataStruct::cArrayStorage<TAllocationOperator,TElement> &Storage,const COMPtr<TVector> &Vector)
{
	return CopyVectorView(Storage,Vector.Pointer());
}
//---------------------------------------------------------------------------
HRESULT STDMETHODCALLTYPE cGATTPeripheral::cGetServicesTask::Invoke(IBLEGetServiceAsyncOp*,ABI::Windows::Foundation::AsyncStatus)
{
	HRESULT hr;
	COMPtr<IGattDeviceServiceResult> ResultServices;
	hr=fAsyncOp->GetResults(COMRetPtrT(ResultServices));

	if(ResultServices!=nullptr){
		ABI::Windows::Devices::Bluetooth::GenericAttributeProfile::GattCommunicationStatus Status;
		hr=ResultServices->get_Status(&Status);
		COMPtr< ABI::Windows::Foundation::Collections::IVectorView<ABI::Windows::Devices::Bluetooth::GenericAttributeProfile::GattDeviceService*> > ServiceVector;
		hr=ResultServices->get_Services(COMRetPtrT(ServiceVector));
		
		cSeqList<ABI::Windows::Devices::Bluetooth::GenericAttributeProfile::IGattDeviceService*> PtrList;
		hr=CopyVectorView(PtrList.Storage(),ServiceVector);


		for(uIntn i=0,c=PtrList.GetCount();i<c;i++){
			auto Service=COMTake(PtrList[i]);

			GUID IDValue;
			hr=Service->get_Uuid(&IDValue);
		}
	}

	fTaskState.SetDone();
	return S_OK;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cGATTPeripheralObserver::cGATTPeripheralObserver(cGATTPeripheralCentral *Central,COMPtr<IBLEWatcher> Watcher)
	: fCentral(Central)
	, fWatcher(Watcher)
	, fCallback(nullptr)
{
	//fWatcher->add_Stopped;
	fWatcher->add_Received(&fBLEReceivedHandler,&fBLEReceivedToken);
}
//---------------------------------------------------------------------------
cGATTPeripheralObserver::~cGATTPeripheralObserver()
{
	fWatcher->remove_Received(fBLEReceivedToken);
}
//---------------------------------------------------------------------------
void cGATTPeripheralObserver::VirtualStarted(void)
{
	InnerIncReference('self');
}
//---------------------------------------------------------------------------
void cGATTPeripheralObserver::VirtualStopped(void)
{
	InnerDecReference('self');
}
//---------------------------------------------------------------------------
bool cGATTPeripheralObserver::StartNotify(iReference *Reference,iAsyncNotificationCallback *Callback)
{
	if(PrepareStartNotify()==false){
		return false;
	}

	HRESULT hr;
	hr=fWatcher->Start();
	if(FAILED(hr)){
		CancelStartNotify();
		return false;
	}

	fCallback=Callback;
	fCallbackReference=Reference;

	ConfirmStartNotify();
	return true;
}
//---------------------------------------------------------------------------
void cGATTPeripheralObserver::StopNotify(void)
{
	if(bcAsyncQueue::StopNotify()){
		UpdateQueueState(false);
	}
}
//---------------------------------------------------------------------------
void cGATTPeripheralObserver::NotifyCallback(bool IdleNotify)
{
	return bcAsyncQueue::NotifyQueue(IdleNotify);
}
//---------------------------------------------------------------------------
bool cGATTPeripheralObserver::IsClosed(void)
{
	return false;
}
//---------------------------------------------------------------------------
iDispatch* cGATTPeripheralObserver::GetHandlerDispatch(void)
{
	return nullptr;
}
//---------------------------------------------------------------------------
iGATTPeripheralCentral* cGATTPeripheralObserver::GetPeripheralCentral(void)
{
	return fCentral;
}
//---------------------------------------------------------------------------
void cGATTPeripheralObserver::DiscardChanges(void)
{
}
//---------------------------------------------------------------------------
rPtr< iArrayReference< rPtr<iGATTPeripheral> > > cGATTPeripheralObserver::QueryAll(void)
{
	return nullptr;
}
//---------------------------------------------------------------------------
rPtr< iArrayReference< rPtr<iGATTPeripheral> > > cGATTPeripheralObserver::QueryChanges(void)
{
	return nullptr;
}
//---------------------------------------------------------------------------
iReference* cGATTPeripheralObserver::NotificationInnerReference(void)
{
	return &fInnerReference;
}
//---------------------------------------------------------------------------
void cGATTPeripheralObserver::NotificationStarted(void)
{
	fCallback->AsyncStarted();
}
//---------------------------------------------------------------------------
void cGATTPeripheralObserver::NotificationStopped(void)
{
	fCallback->AsyncStopped();
}
//---------------------------------------------------------------------------
void cGATTPeripheralObserver::AsyncQueueNotify(void)
{
	fCallback->AsyncNotify();
}
//---------------------------------------------------------------------------
void cGATTPeripheralObserver::BLEReceived(aClsRef<cBLEAdvData> AdvData)
{
	cStringBuffer<char> MsgText;
	{
		auto MsgTextStream=MsgText.StreamWriteBuffer();
		StringStream::WriteFormatString(MsgTextStream,"---BLE Advertisement ---\nAddress %llx\n",AdvData->Address);
		StringStream::WriteFormatString(MsgTextStream,"SignalStrength %d\n",AdvData->SignalStrength);
		StringStream::WriteFormatString(MsgTextStream,"Timestamp %lld\n",AdvData->Timestamp);
		if(AdvData->Flags==ABI::Windows::Devices::Bluetooth::Advertisement::BluetoothLEAdvertisementFlags::BluetoothLEAdvertisementFlags_None){
			StringStream::WriteFormatString(MsgTextStream,"NoFlags\n",0);
		}
		else{
			if(AdvData->Flags&ABI::Windows::Devices::Bluetooth::Advertisement::BluetoothLEAdvertisementFlags::BluetoothLEAdvertisementFlags_LimitedDiscoverableMode){
				StringStream::WriteFormatString(MsgTextStream,"Flags:BluetoothLEAdvertisementFlags_LimitedDiscoverableMode\n",0);
			}
			if(AdvData->Flags&ABI::Windows::Devices::Bluetooth::Advertisement::BluetoothLEAdvertisementFlags::BluetoothLEAdvertisementFlags_GeneralDiscoverableMode){
				StringStream::WriteFormatString(MsgTextStream,"Flags:BluetoothLEAdvertisementFlags_GeneralDiscoverableMode\n",0);
			}
			if(AdvData->Flags&ABI::Windows::Devices::Bluetooth::Advertisement::BluetoothLEAdvertisementFlags::BluetoothLEAdvertisementFlags_ClassicNotSupported){
				StringStream::WriteFormatString(MsgTextStream,"Flags:ClassicNotSupported\n",0);
			}
			if(AdvData->Flags&ABI::Windows::Devices::Bluetooth::Advertisement::BluetoothLEAdvertisementFlags::BluetoothLEAdvertisementFlags_DualModeControllerCapable){
				StringStream::WriteFormatString(MsgTextStream,"Flags:BluetoothLEAdvertisementFlags_DualModeControllerCapable\n",0);
			}
			if(AdvData->Flags&ABI::Windows::Devices::Bluetooth::Advertisement::BluetoothLEAdvertisementFlags::BluetoothLEAdvertisementFlags_DualModeHostCapable){
				StringStream::WriteFormatString(MsgTextStream,"Flags:BluetoothLEAdvertisementFlags_DualModeHostCapable\n",0);
			}
		}
		StringStream::WriteFormatString(MsgTextStream,"LocalName : ",0);
		StringStream::WriteConvertEncoding(MsgTextStream,cnWindows::CodePageFromUTF16(CP_ACP),utow(AdvData->LocalName),AdvData->LocalName->Length);

		StringStream::WriteFormatString(MsgTextStream,"\n-Services-%d-\n",AdvData->ServiceIDs.GetCount());
		for(auto &Item : AdvData->ServiceIDs){
			StringStream::WriteFormatString(MsgTextStream,"{",0);
			StringStream::WriteGUID(MsgTextStream,&Item);
			StringStream::WriteFormatString(MsgTextStream,"}\n",0);
		}
		StringStream::WriteFormatString(MsgTextStream,"-ManufactureData-\n",0);
		for(auto &Item : AdvData->ManufactureData){
			StringStream::WriteFormatString(MsgTextStream,"CompanyID = %x ; DataSize=%d\n",Item.CompanyID,Item.Data.Length);
		}
		StringStream::WriteFormatString(MsgTextStream,"-SectionData-\n",0);
		for(auto &Item : AdvData->SectionData){
			StringStream::WriteFormatString(MsgTextStream,"DataType = %d ; DataSize=%d\n",Item.DataType,Item.Data.Length);
		}
	}
	//puts(MsgText->Pointer);

	AsyncQueueSetAvailable(true);
}
//---------------------------------------------------------------------------
cGATTPeripheralObserver* cGATTPeripheralObserver::cBLEReceivedHandler::GetHost(void)
{
	return cnMemory::GetObjectFromMemberPointer(this,&cGATTPeripheralObserver::fBLEReceivedHandler);
}
//---------------------------------------------------------------------------
HRESULT STDMETHODCALLTYPE cGATTPeripheralObserver::cBLEReceivedHandler::QueryInterface(REFIID riid,_COM_Outptr_ void __RPC_FAR *__RPC_FAR *ppvObject)
{
	if(riid==__uuidof(IUnknown)){
		GetHost()->InnerIncReference('evet');
		*ppvObject=static_cast<IUnknown*>(this);
		return S_OK;
	}
	if(riid==__uuidof(IBLEWatcherHandler)){
		GetHost()->InnerIncReference('evet');
		*ppvObject=static_cast<IBLEWatcherHandler*>(this);
		return S_OK;
	}
	*ppvObject=nullptr;
	return E_NOINTERFACE;
}
//---------------------------------------------------------------------------
ULONG STDMETHODCALLTYPE cGATTPeripheralObserver::cBLEReceivedHandler::AddRef(void)
{
	GetHost()->InnerIncReference('evet');
	return 0;
}
//---------------------------------------------------------------------------
ULONG STDMETHODCALLTYPE cGATTPeripheralObserver::cBLEReceivedHandler::Release(void)
{
	GetHost()->InnerDecReference('evet');
	return 0;
}
//---------------------------------------------------------------------------
HRESULT STDMETHODCALLTYPE cGATTPeripheralObserver::cBLEReceivedHandler::Invoke(IBLEWatcher *sender, _In_ ABI::Windows::Devices::Bluetooth::Advertisement::IBluetoothLEAdvertisementReceivedEventArgs *args)
{
	auto AdvData=aClsCreate<cBLEAdvData>();

	HRESULT hr;

	hr=args->get_BluetoothAddress(&AdvData->Address);
	if(FAILED(hr)){
		return S_OK;
	}

	
	hr=args->get_RawSignalStrengthInDBm(&AdvData->SignalStrength);
	if(FAILED(hr)){
		AdvData->SignalStrength=127;
	}

	{
		ABI::Windows::Foundation::DateTime Timestamp;
		hr=args->get_Timestamp(&Timestamp);
		if(FAILED(hr)){
			AdvData->Timestamp=0;
		}
		else{
			AdvData->Timestamp=Timestamp.UniversalTime;
		}
	}
	hr=args->get_AdvertisementType(&AdvData->Type);
	if(FAILED(hr))
		AdvData->Type=ABI::Windows::Devices::Bluetooth::Advertisement::BluetoothLEAdvertisementType::BluetoothLEAdvertisementType_ConnectableUndirected;

	COMPtr<ABI::Windows::Devices::Bluetooth::Advertisement::IBluetoothLEAdvertisement> Adv;
	hr=args->get_Advertisement(COMRetPtrT(Adv));
	if(SUCCEEDED(hr) && Adv!=nullptr){
		HSTRING LocalName;
		hr=Adv->get_LocalName(&LocalName);
		if(SUCCEEDED(hr) && LocalName!=nullptr){
			AdvData->LocalName=CreateStringFromHandle(LocalName);
			::WindowsDeleteString(LocalName);
		}

		COMPtr<ABI::Windows::Foundation::Collections::IVector<GUID> > ServiceUuids;
		hr=Adv->get_ServiceUuids(COMRetPtrT(ServiceUuids));
		if(SUCCEEDED(hr) && ServiceUuids!=nullptr){
			unsigned int ServiceCount;
			hr=ServiceUuids->get_Size(&ServiceCount);
			if(SUCCEEDED(hr) && ServiceCount!=0){
				AdvData->ServiceIDs.SetCount(ServiceCount);
				hr=ServiceUuids->GetMany(0,ServiceCount,AdvData->ServiceIDs.GetItems(),&ServiceCount);
				if(FAILED(hr)){
					ServiceCount=0;
				}
				AdvData->ServiceIDs.SetCount(ServiceCount);
			}
		}
		COMPtr<ABI::Windows::Foundation::IReference<enum ABI::Windows::Devices::Bluetooth::Advertisement::BluetoothLEAdvertisementFlags> > Flags;
		hr=Adv->get_Flags(COMRetPtrT(Flags));
		if(SUCCEEDED(hr) && Flags!=nullptr){
			hr=Flags->get_Value(&AdvData->Flags);
			if(FAILED(hr)){
				AdvData->Flags=ABI::Windows::Devices::Bluetooth::Advertisement::BluetoothLEAdvertisementFlags::BluetoothLEAdvertisementFlags_None;
			}
		}

		COMPtr<ABI::Windows::Foundation::Collections::IVector<ABI::Windows::Devices::Bluetooth::Advertisement::BluetoothLEManufacturerData*> > ManufacturerData;
		hr=Adv->get_ManufacturerData(COMRetPtrT(ManufacturerData));
		if(SUCCEEDED(hr) && ManufacturerData!=nullptr){
			unsigned int ManufacturerDataCount;
			hr=ManufacturerData->get_Size(&ManufacturerDataCount);
			if(FAILED(hr))
				ManufacturerDataCount=0;

			for(unsigned int i=0;i<ManufacturerDataCount;i++){
				COMPtr<ABI::Windows::Devices::Bluetooth::Advertisement::IBluetoothLEManufacturerData> DataItem;
				hr=ManufacturerData->GetAt(i,COMRetPtrT(DataItem));
				if(SUCCEEDED(hr) && DataItem!=nullptr){
					COMPtr<ABI::Windows::Storage::Streams::IBuffer> Buffer;
					UINT16 CompanyID;
					hr=DataItem->get_CompanyId(&CompanyID);
					if(FAILED(hr))
						continue;
					hr=DataItem->get_Data(COMRetPtrT(Buffer));
					if(FAILED(hr))
						continue;
					auto DataArray=GetBufferData(Buffer);
					auto NewItem=AdvData->ManufactureData.Append();
					NewItem->CompanyID=CompanyID;
					NewItem->Data.Copy(DataArray.Pointer,DataArray.Length);
				}
			}

		}
		COMPtr<ABI::Windows::Foundation::Collections::IVector<ABI::Windows::Devices::Bluetooth::Advertisement::BluetoothLEAdvertisementDataSection*> > DataSection;
		hr=Adv->get_DataSections(COMRetPtrT(DataSection));
		if(SUCCEEDED(hr) && DataSection!=nullptr){
			unsigned int DataSectionCount;
			hr=DataSection->get_Size(&DataSectionCount);
			if(FAILED(hr))
				DataSectionCount=0;

			for(unsigned int i=0;i<DataSectionCount;i++){
				COMPtr<ABI::Windows::Devices::Bluetooth::Advertisement::IBluetoothLEAdvertisementDataSection> DataItem;
				hr=DataSection->GetAt(i,COMRetPtrT(DataItem));
				if(SUCCEEDED(hr) && DataItem!=nullptr){
					COMPtr<ABI::Windows::Storage::Streams::IBuffer> Buffer;
					BYTE DataType;
					hr=DataItem->get_DataType(&DataType);
					if(FAILED(hr))
						DataType=255;
					hr=DataItem->get_Data(COMRetPtrT(Buffer));
					if(FAILED(hr))
						continue;
					auto DataArray=GetBufferData(Buffer);
					auto NewItem=AdvData->SectionData.Append();
					NewItem->DataType=DataType;
					NewItem->Data.Copy(DataArray.Pointer,DataArray.Length);
				}
			}

		}
	}

	GetHost()->BLEReceived(cnVar::MoveCast(AdvData));
	return S_OK;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
iAddress* cGATTAdvertisementObserver::cAdvertisementData::GetPeripheralAddress(void)
{
	return Address;
}
//---------------------------------------------------------------------------
cGATTAdvertisementInfo cGATTAdvertisementObserver::cAdvertisementData::GetInfo(void)
{
	return Info;
}
//---------------------------------------------------------------------------
uIntn cGATTAdvertisementObserver::cAdvertisementData::GetServiceUUIDCount(void)
{
	return ServiceUUIDs.GetCount();
}
//---------------------------------------------------------------------------
cGATTUUID cGATTAdvertisementObserver::cAdvertisementData::GetServiceUUID(uIntn Index)
{
	return ServiceUUIDs[Index];
}
//---------------------------------------------------------------------------
uIntn cGATTAdvertisementObserver::cAdvertisementData::GetManufactureDataCount(void)
{
	return ManufacturerData.GetCount();
}
//---------------------------------------------------------------------------
cConstMemory cGATTAdvertisementObserver::cAdvertisementData::GetManufactureData(uIntn Index,ufInt16 &CompanyID)
{
	auto &Item=ManufacturerData[Index];
	CompanyID=Item.CompanyID;
	return Item.Data;
}
//---------------------------------------------------------------------------
uIntn cGATTAdvertisementObserver::cAdvertisementData::GetSectionDataCount(void)
{
	return SectionData.GetCount();
}
//---------------------------------------------------------------------------
cConstMemory cGATTAdvertisementObserver::cAdvertisementData::GetSectionData(uIntn Index,ufInt8 &DataType)
{
	auto &Item=SectionData[Index];
	DataType=Item.DataType;
	return Item.Data;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cGATTAdvertisementObserver::cGATTAdvertisementObserver(cGATTPeripheralCentral *Central,COMPtr<IBLEWatcher> Watcher)
	: fCentral(Central)
	, fWatcher(Watcher)
	, fCallback(nullptr)
{
	//fWatcher->add_Stopped;
	fWatcher->add_Received(&fBLEReceivedHandler,&fBLEReceivedToken);
}
//---------------------------------------------------------------------------
cGATTAdvertisementObserver::~cGATTAdvertisementObserver()
{
	fWatcher->remove_Received(fBLEReceivedToken);
}
//---------------------------------------------------------------------------
void cGATTAdvertisementObserver::VirtualStarted(void)
{
	InnerIncReference('self');
}
//---------------------------------------------------------------------------
void cGATTAdvertisementObserver::VirtualStopped(void)
{
	InnerDecReference('self');
}
//---------------------------------------------------------------------------
bool cGATTAdvertisementObserver::StartNotify(iReference *Reference,iAsyncNotificationCallback *Callback)
{
	if(PrepareStartNotify()==false){
		return false;
	}

	HRESULT hr;
	hr=fWatcher->Start();
	if(FAILED(hr)){
		CancelStartNotify();
		return false;
	}

	fCallback=Callback;
	fCallbackReference=Reference;

	ConfirmStartNotify();
	UpdateQueueState(true);
	return true;
}
//---------------------------------------------------------------------------
void cGATTAdvertisementObserver::StopNotify(void)
{
	if(bcAsyncQueue::StopNotify()){
		UpdateQueueState(false);
	}
}
//---------------------------------------------------------------------------
void cGATTAdvertisementObserver::NotifyCallback(bool IdleNotify)
{
	return bcAsyncQueue::NotifyQueue(IdleNotify);
}
//---------------------------------------------------------------------------
bool cGATTAdvertisementObserver::IsClosed(void)
{
	return false;
}
//---------------------------------------------------------------------------
iDispatch* cGATTAdvertisementObserver::GetHandlerDispatch(void)
{
	return nullptr;
}
//---------------------------------------------------------------------------
iGATTPeripheralCentral* cGATTAdvertisementObserver::GetPeripheralCentral(void)
{
	return fCentral;
}
//---------------------------------------------------------------------------
void cGATTAdvertisementObserver::DiscardQueue(void)
{
}
//---------------------------------------------------------------------------
rPtr<iGATTAdvertisement> cGATTAdvertisementObserver::Fetch(void)
{
	auto pData=fAdvertisementQueue.Dequeue();
	if(pData==nullptr)
		return nullptr;
	return rTake(pData,'queu');
}
//---------------------------------------------------------------------------
iReference* cGATTAdvertisementObserver::NotificationInnerReference(void)
{
	return &fInnerReference;
}
//---------------------------------------------------------------------------
void cGATTAdvertisementObserver::NotificationStarted(void)
{
	fCallback->AsyncStarted();
}
//---------------------------------------------------------------------------
void cGATTAdvertisementObserver::NotificationStopped(void)
{
	fCallback->AsyncStopped();

	fCallback=nullptr;
	fCallbackReference=nullptr;
}
//---------------------------------------------------------------------------
void cGATTAdvertisementObserver::AsyncQueueNotify(void)
{
	fCallback->AsyncNotify();
}
//---------------------------------------------------------------------------
void cGATTAdvertisementObserver::BLEReceived(rPtr<cAdvertisementData> AdvData)
{
	auto pData=rExtract(AdvData,'queu');
	fAdvertisementQueue.Enqueue(pData);

	AsyncQueueSetAvailable(true);
}
//---------------------------------------------------------------------------
cGATTAdvertisementObserver* cGATTAdvertisementObserver::cBLEReceivedHandler::GetHost(void)
{
	return cnMemory::GetObjectFromMemberPointer(this,&cGATTAdvertisementObserver::fBLEReceivedHandler);
}
//---------------------------------------------------------------------------
HRESULT STDMETHODCALLTYPE cGATTAdvertisementObserver::cBLEReceivedHandler::QueryInterface(REFIID riid,_COM_Outptr_ void __RPC_FAR *__RPC_FAR *ppvObject)
{
	if(riid==__uuidof(IUnknown)){
		GetHost()->InnerIncReference('evet');
		*ppvObject=static_cast<IUnknown*>(this);
		return S_OK;
	}
	if(riid==__uuidof(IBLEWatcherHandler)){
		GetHost()->InnerIncReference('evet');
		*ppvObject=static_cast<IBLEWatcherHandler*>(this);
		return S_OK;
	}
	*ppvObject=nullptr;
	return E_NOINTERFACE;
}
//---------------------------------------------------------------------------
ULONG STDMETHODCALLTYPE cGATTAdvertisementObserver::cBLEReceivedHandler::AddRef(void)
{
	GetHost()->InnerIncReference('evet');
	return 0;
}
//---------------------------------------------------------------------------
ULONG STDMETHODCALLTYPE cGATTAdvertisementObserver::cBLEReceivedHandler::Release(void)
{
	GetHost()->InnerDecReference('evet');
	return 0;
}
//---------------------------------------------------------------------------
HRESULT STDMETHODCALLTYPE cGATTAdvertisementObserver::cBLEReceivedHandler::Invoke(IBLEWatcher *sender, _In_ ABI::Windows::Devices::Bluetooth::Advertisement::IBluetoothLEAdvertisementReceivedEventArgs *args)
{

	HRESULT hr;
	UINT64 BLEAddress;
	hr=args->get_BluetoothAddress(&BLEAddress);
	if(FAILED(hr)){
		return S_OK;
	}


	auto AdvData=GetHost()->fAdvDataRecycler.Query();
	AdvData->ServiceUUIDs.Clear();
	AdvData->ManufacturerData.Clear();
	AdvData->SectionData.Clear();
	AdvData->LocalName.Clear();

	auto DeviceAddress=iCreate<cBluetoothAddress>();
	DeviceAddress->Address=BLEAddress;
	AdvData->Address=DeviceAddress;
	
	INT16 SignalStrength;
	hr=args->get_RawSignalStrengthInDBm(&SignalStrength);
	if(FAILED(hr)){
		AdvData->Info.SignalStrength=32768;
	}
	else{
		AdvData->Info.SignalStrength=SignalStrength;
	}

	{
		ABI::Windows::Foundation::DateTime Timestamp;
		hr=args->get_Timestamp(&Timestamp);
		if(FAILED(hr)){
			AdvData->Info.Timestamp=nullptr;
		}
		else{
			//AdvData->Timestamp=Timestamp.UniversalTime;
			AdvData->Info.Timestamp=&AdvData->Timestamp;
		}
	}
	ABI::Windows::Devices::Bluetooth::Advertisement::BluetoothLEAdvertisementType AdvType;
	hr=args->get_AdvertisementType(&AdvType);
	if(FAILED(hr)){
		AdvData->Info.Type=GATTAdvertisementType::ConnectableUndirected;
	}
	else{
		switch(AdvType){
		case ABI::Windows::Devices::Bluetooth::Advertisement::BluetoothLEAdvertisementType::BluetoothLEAdvertisementType_ConnectableUndirected:
			AdvData->Info.Type=GATTAdvertisementType::ConnectableUndirected;
			break;
		case ABI::Windows::Devices::Bluetooth::Advertisement::BluetoothLEAdvertisementType::BluetoothLEAdvertisementType_ConnectableDirected:
			AdvData->Info.Type=GATTAdvertisementType::ConnectableDirected;
			break;
		case ABI::Windows::Devices::Bluetooth::Advertisement::BluetoothLEAdvertisementType::BluetoothLEAdvertisementType_ScannableUndirected:
			AdvData->Info.Type=GATTAdvertisementType::ScannableUndirected;
			break;
		case ABI::Windows::Devices::Bluetooth::Advertisement::BluetoothLEAdvertisementType::BluetoothLEAdvertisementType_NonConnectableUndirected:
			AdvData->Info.Type=GATTAdvertisementType::NonConnectableUndirected;
			break;
		case ABI::Windows::Devices::Bluetooth::Advertisement::BluetoothLEAdvertisementType::BluetoothLEAdvertisementType_ScanResponse:
			AdvData->Info.Type=GATTAdvertisementType::ScanResponse;
			break;
		//case ABI::Windows::Devices::Bluetooth::Advertisement::BluetoothLEAdvertisementType::BluetoothLEAdvertisementType_Extended:
		//	AdvData->Info.Type=GATTAdvertisementType::Extended;
		//	break;
		}
	}

	COMPtr<ABI::Windows::Devices::Bluetooth::Advertisement::IBluetoothLEAdvertisement> Adv;
	hr=args->get_Advertisement(COMRetPtrT(Adv));
	if(SUCCEEDED(hr) && Adv!=nullptr){
		HSTRING LocalName;
		hr=Adv->get_LocalName(&LocalName);
		if(SUCCEEDED(hr) && LocalName!=nullptr){
			AdvData->LocalName=CreateStringFromHandle(LocalName);
			::WindowsDeleteString(LocalName);
		}

		COMPtr<ABI::Windows::Foundation::Collections::IVector<GUID> > ServiceUuids;
		hr=Adv->get_ServiceUuids(COMRetPtrT(ServiceUuids));
		if(SUCCEEDED(hr) && ServiceUuids!=nullptr){
			unsigned int ServiceCount;
			hr=ServiceUuids->get_Size(&ServiceCount);
			if(SUCCEEDED(hr) && ServiceCount!=0){
				AdvData->ServiceUUIDs.SetCount(ServiceCount);
				hr=ServiceUuids->GetMany(0,ServiceCount,reinterpret_cast<GUID*>(AdvData->ServiceUUIDs.GetItems()),&ServiceCount);
				if(FAILED(hr)){
					ServiceCount=0;
				}
				AdvData->ServiceUUIDs.SetCount(ServiceCount);
			}
		}
		COMPtr<ABI::Windows::Foundation::IReference<enum ABI::Windows::Devices::Bluetooth::Advertisement::BluetoothLEAdvertisementFlags> > AdvFlags;
		hr=Adv->get_Flags(COMRetPtrT(AdvFlags));
		if(SUCCEEDED(hr) && AdvFlags!=nullptr){
			enum ABI::Windows::Devices::Bluetooth::Advertisement::BluetoothLEAdvertisementFlags FlagValue;
			hr=AdvFlags->get_Value(&FlagValue);
			if(FAILED(hr)){
				AdvData->Info.Flags=GATTAdvertisementFlags::None;
			}
			else{
				AdvData->Info.Flags=0;
				if(FlagValue&ABI::Windows::Devices::Bluetooth::Advertisement::BluetoothLEAdvertisementFlags::BluetoothLEAdvertisementFlags_LimitedDiscoverableMode){
					AdvData->Info.Flags|=GATTAdvertisementFlags::LimitedDiscoverableMode;
				}
				if(FlagValue&ABI::Windows::Devices::Bluetooth::Advertisement::BluetoothLEAdvertisementFlags::BluetoothLEAdvertisementFlags_GeneralDiscoverableMode){
					AdvData->Info.Flags|=GATTAdvertisementFlags::GeneralDiscoverableMode;
				}
				if(FlagValue&ABI::Windows::Devices::Bluetooth::Advertisement::BluetoothLEAdvertisementFlags::BluetoothLEAdvertisementFlags_ClassicNotSupported){
					AdvData->Info.Flags|=GATTAdvertisementFlags::ClassicNotSupported;
				}
				if(FlagValue&ABI::Windows::Devices::Bluetooth::Advertisement::BluetoothLEAdvertisementFlags::BluetoothLEAdvertisementFlags_DualModeControllerCapable){
					AdvData->Info.Flags|=GATTAdvertisementFlags::DualModeControllerCapable;
				}
				if(FlagValue&ABI::Windows::Devices::Bluetooth::Advertisement::BluetoothLEAdvertisementFlags::BluetoothLEAdvertisementFlags_DualModeHostCapable){
					AdvData->Info.Flags|=GATTAdvertisementFlags::DualModeHostCapable;
				}
			}
		}

		COMPtr<ABI::Windows::Foundation::Collections::IVector<ABI::Windows::Devices::Bluetooth::Advertisement::BluetoothLEManufacturerData*> > ManufacturerData;
		hr=Adv->get_ManufacturerData(COMRetPtrT(ManufacturerData));
		if(SUCCEEDED(hr) && ManufacturerData!=nullptr){
			unsigned int ManufacturerDataCount;
			hr=ManufacturerData->get_Size(&ManufacturerDataCount);
			if(FAILED(hr))
				ManufacturerDataCount=0;

			for(unsigned int i=0;i<ManufacturerDataCount;i++){
				COMPtr<ABI::Windows::Devices::Bluetooth::Advertisement::IBluetoothLEManufacturerData> DataItem;
				hr=ManufacturerData->GetAt(i,COMRetPtrT(DataItem));
				if(SUCCEEDED(hr) && DataItem!=nullptr){
					COMPtr<ABI::Windows::Storage::Streams::IBuffer> Buffer;
					UINT16 CompanyID;
					hr=DataItem->get_CompanyId(&CompanyID);
					if(FAILED(hr))
						continue;
					hr=DataItem->get_Data(COMRetPtrT(Buffer));
					if(FAILED(hr))
						continue;
					auto DataArray=GetBufferData(Buffer);
					auto NewItem=AdvData->ManufacturerData.Append();
					NewItem->CompanyID=CompanyID;
					NewItem->Data.Copy(DataArray.Pointer,DataArray.Length);
				}
			}

		}
		COMPtr<ABI::Windows::Foundation::Collections::IVector<ABI::Windows::Devices::Bluetooth::Advertisement::BluetoothLEAdvertisementDataSection*> > DataSection;
		hr=Adv->get_DataSections(COMRetPtrT(DataSection));
		if(SUCCEEDED(hr) && DataSection!=nullptr){
			unsigned int DataSectionCount;
			hr=DataSection->get_Size(&DataSectionCount);
			if(FAILED(hr))
				DataSectionCount=0;

			for(unsigned int i=0;i<DataSectionCount;i++){
				COMPtr<ABI::Windows::Devices::Bluetooth::Advertisement::IBluetoothLEAdvertisementDataSection> DataItem;
				hr=DataSection->GetAt(i,COMRetPtrT(DataItem));
				if(SUCCEEDED(hr) && DataItem!=nullptr){
					COMPtr<ABI::Windows::Storage::Streams::IBuffer> Buffer;
					BYTE DataType;
					hr=DataItem->get_DataType(&DataType);
					if(FAILED(hr))
						DataType=255;
					hr=DataItem->get_Data(COMRetPtrT(Buffer));
					if(FAILED(hr))
						continue;
					auto DataArray=GetBufferData(Buffer);
					auto NewItem=AdvData->SectionData.Append();
					NewItem->DataType=DataType;
					NewItem->Data.Copy(DataArray.Pointer,DataArray.Length);
				}
			}

		}
	}

	AdvData->Info.LocalName=AdvData->LocalName.GetArray();
	AdvData->Info.Timestamp=AdvData->Timestamp;

	GetHost()->BLEReceived(cnVar::MoveCast(AdvData));
	return S_OK;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cGATTPeripheralCentral::cGATTPeripheralCentral(iDispatch *Dispatch,COMPtr<IBLEDeviceStatics> BLEStatic)
	: fBLEStatic(cnVar::MoveCast(BLEStatic))
{
}
//---------------------------------------------------------------------------
iDispatch* cGATTPeripheralCentral::GetHandlerDispatch(void)
{
	return nullptr;
}
//---------------------------------------------------------------------------
bool cGATTPeripheralCentral::InsertHandler(iGATTPeripheralCentralHandler *Handler)
{
	return false;
}
//---------------------------------------------------------------------------
bool cGATTPeripheralCentral::RemoveHandler(iGATTPeripheralCentralHandler *Handler)
{
	return false;
}
//---------------------------------------------------------------------------
rPtr<iGATTPeripheral> cGATTPeripheralCentral::AccessPeripheral(iAddress *Address)
{
	HRESULT hr;
	auto BLEAddress=iCast<cBluetoothAddress>(Address);
	if(BLEAddress!=nullptr){
		COMPtr<IBLEConnectAsyncOp> ConnectOp;
		hr=fBLEStatic->FromBluetoothAddressAsync(BLEAddress->Address,COMRetPtrT(ConnectOp));
		if(FAILED(hr)){
			return nullptr;
		}
		auto Connector=rCreate<cGATTPeripheral::cAddressConnector>(ConnectOp);
		Connector->Address=BLEAddress;
		return rCreate<cGATTPeripheral>(this,cnVar::MoveCast(Connector));
	}
	return nullptr;
}
//---------------------------------------------------------------------------
rPtr<iGATTAdvertisementObserver> cGATTPeripheralCentral::CreateAdvertisementObserver(void)
{
	cHString RCName_BluetoothLEAdvertisementWatcher;
	RCName_BluetoothLEAdvertisementWatcher=RuntimeClass_Windows_Devices_Bluetooth_Advertisement_BluetoothLEAdvertisementWatcher;

	HRESULT hr;
	COMPtr<IBLEWatcher> BLEWatcher;
	hr=::Windows::Foundation::ActivateInstance(RCName_BluetoothLEAdvertisementWatcher,COMRetPtrT(BLEWatcher));
	if(FAILED(hr))
		return nullptr;

	//BLEWatcher->get_SignalStrengthFilter;
	//BLEWatcher->put_ScanningMode
	//BLEWatcher->put_AdvertisementFilter

	return rCreate<cGATTAdvertisementObserver>(this,cnVar::MoveCast(BLEWatcher));
}
//---------------------------------------------------------------------------
rPtr<iGATTPeripheralObserver> cGATTPeripheralCentral::CreatePeripheralObserver(const cGATTUUID *Service,uIntn ServiceCount)
{
	cHString RCName_BluetoothLEAdvertisementWatcher;
	RCName_BluetoothLEAdvertisementWatcher=RuntimeClass_Windows_Devices_Bluetooth_Advertisement_BluetoothLEAdvertisementWatcher;

	HRESULT hr;
	COMPtr<IBLEWatcher> BLEWatcher;
	hr=::Windows::Foundation::ActivateInstance(RCName_BluetoothLEAdvertisementWatcher,COMRetPtrT(BLEWatcher));
	if(FAILED(hr))
		return nullptr;

	//BLEWatcher->get_SignalStrengthFilter;
	//BLEWatcher->put_ScanningMode
	//BLEWatcher->put_AdvertisementFilter

	return rCreate<cGATTPeripheralObserver>(this,cnVar::MoveCast(BLEWatcher));
}
//---------------------------------------------------------------------------
bool cGATTPeripheralCentral::ResetPeripheralCache(void)
{
	return false;
}
//---------------------------------------------------------------------------
bool cGATTPeripheralCentral::IsEnabled(void)
{
	return true;
}
//---------------------------------------------------------------------------
bool cGATTPeripheralCentral::IsPowerOn(void)
{
	return true;
}
//---------------------------------------------------------------------------

rPtr<iGATTPeripheralCentral> UWP::OpenGATTPeripheral(iDispatch *Dispatch)
{
	COMPtr<ABI::Windows::Devices::Bluetooth::IBluetoothLEDeviceStatics> BLEStatic;
	cHString RCName_BLEDevice=RuntimeClass_Windows_Devices_Bluetooth_BluetoothLEDevice;
	if(FAILED(Windows::Foundation::GetActivationFactory(RCName_BLEDevice,COMRetPtrT(BLEStatic)))){
		return nullptr;
	}
	return rCreate<cGATTPeripheralCentral>(Dispatch,cnVar::MoveCast(BLEStatic));
}

//---------------------------------------------------------------------------

#endif	// _WIN32_WINNT >= _WIN32_WINNT_WIN10

#if 0

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

cnLib_INTERFACE_DEFINE_LOCALID(cWindowsBluetoothLEConectionDeviceInfo);

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cWindowsBluetoothLEConectionDeviceInfo::cWindowsBluetoothLEConectionDeviceInfo(ABI::Windows::Devices::Enumeration::IDeviceInformation *DeviceInfo)
	: fDeviceInfo(DeviceInfo)
{
}
//---------------------------------------------------------------------------
cWindowsBluetoothLEConectionDeviceInfo::~cWindowsBluetoothLEConectionDeviceInfo()
{
}
//---------------------------------------------------------------------------
eOrdering cWindowsBluetoothLEConectionDeviceInfo::Compare(const iComparable *Dest)const
{
	auto DestUnknown=iCastCOM(Dest);
	if(DestUnknown==nullptr)
		return Ordering::Different;

	if(fDeviceInfo<DestUnknown)
		return Ordering::Less;
	if(fDeviceInfo==DestUnknown)
		return Ordering::Equal;
	return Ordering::Greater;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cBluetoothLETunnelStreamEndpointDevice::cBluetoothLETunnelStreamEndpointDevice(
	COMPtr<ABI::Windows::Devices::Bluetooth::IBluetoothLEDevice> Device,
	COMPtr<ABI::Windows::Devices::Bluetooth::GenericAttributeProfile::IGattCharacteristic> ReadChar,
	COMPtr<ABI::Windows::Devices::Bluetooth::GenericAttributeProfile::IGattCharacteristic> WriteChar
)
	: fDevice(cnVar::MoveCast(Device))
	, fReadCharacteristic(cnVar::MoveCast(ReadChar))
	, fWriteCharacteristic(cnVar::MoveCast(WriteChar))
{
	cWindowsStringReference DataReaderClassName;
	DataReaderClassName.Create(RuntimeClass_Windows_Storage_Streams_DataReader);
	Windows::Foundation::GetActivationFactory(DataReaderClassName,COMRetPtrT(fDataReaderStatic));
}
//---------------------------------------------------------------------------
cBluetoothLETunnelStreamEndpointDevice::~cBluetoothLETunnelStreamEndpointDevice()
{
	//fDevice->get_GattServices(0);
	//COMPtr<ABI::Windows::Devices::Bluetooth::GenericAttributeProfile::IGattDeviceService> BLEService;
	//BLEService->GetCharacteristics(UUID,0);
	//COMPtr<ABI::Windows::Devices::Bluetooth::GenericAttributeProfile::IGattCharacteristic> BLEChar;
	//BLEChar->WriteValueWithOptionAsync
}
//---------------------------------------------------------------------------
void cBluetoothLETunnelStreamEndpointDevice::VirtualStarted(void)
{
	fDevice->add_ConnectionStatusChanged(&fConnectionStatusHandler,&fConnectionStatusEventToken);
	fDevice->add_GattServicesChanged(&fGattServicesChangedHandler,&fGattServicesChangedEventToken);
	fDevice->add_NameChanged(&fNameChangedHandler,&fNameChangedEventToken);

	fReadCharacteristic->add_ValueChanged(&fValueChangedHandler,&fValueChangedEventToken);
}
//---------------------------------------------------------------------------
void cBluetoothLETunnelStreamEndpointDevice::VirtualStopped(void)
{
	fDevice->remove_ConnectionStatusChanged(fConnectionStatusEventToken);
	fDevice->remove_GattServicesChanged(fGattServicesChangedEventToken);
	fDevice->remove_NameChanged(fNameChangedEventToken);

	fReadCharacteristic->remove_ValueChanged(fValueChangedEventToken);

	InnerDecReference('self');
}
//---------------------------------------------------------------------------
void cBluetoothLETunnelStreamEndpointDevice::ConnectionStatusEvent(void)
{
}
//---------------------------------------------------------------------------
void cBluetoothLETunnelStreamEndpointDevice::GattServicesChangedEvent(void)
{
}
//---------------------------------------------------------------------------
void cBluetoothLETunnelStreamEndpointDevice::NameChangedEvent(void)
{
}
//---------------------------------------------------------------------------
void cBluetoothLETunnelStreamEndpointDevice::ValueChangedEvent(ABI::Windows::Storage::Streams::IBuffer *Buffer)
{
	UINT32 BufferLength;
	if(FAILED(Buffer->get_Length(&BufferLength))){
		return;
	}
	if(BufferLength==0)
		return;
	
	COMPtr<ABI::Windows::Storage::Streams::IDataReader> DataReader;
	if(FAILED(fDataReaderStatic->FromBuffer(Buffer,COMRetPtrT(DataReader)))){
		return;
	}

	while(BufferLength!=0){
		auto EndpointReadBuffer=QueryReadDataBuffer(BufferLength);

		if(FAILED(DataReader->ReadBytes(EndpointReadBuffer.Length,static_cast<BYTE*>(EndpointReadBuffer.Pointer)))){
			return;
		}
		uIntn ReadLength=EndpointReadBuffer.Length;

		AdvanceReadDataBuffer(ReadLength);
		BufferLength-=ReadLength;
	}
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cBluetoothLETunnelStreamEndpointDevice *cBluetoothLETunnelStreamEndpointDevice::cConnectionStatusHandler::GetHost(void)
{
	return cnMemory::GetObjectFromMemberPointer(this,&cBluetoothLETunnelStreamEndpointDevice::fConnectionStatusHandler);
}
//---------------------------------------------------------------------------
HRESULT cBluetoothLETunnelStreamEndpointDevice::cConnectionStatusHandler::QueryInterface( REFIID riid,_COM_Outptr_ void **ppvObject)
{
	if(impCOMQueryInterface<IUnknown,ABI::Windows::Foundation::ITypedEventHandler<ABI::Windows::Devices::Bluetooth::BluetoothLEDevice*,IInspectable*> >(this,riid,ppvObject)){
		GetHost()->IncreaseReference();
		return S_OK;
	}
	return E_NOINTERFACE;
}
ULONG cBluetoothLETunnelStreamEndpointDevice::cConnectionStatusHandler::AddRef(void){	GetHost()->IncreaseReference();	return 0;	}
ULONG cBluetoothLETunnelStreamEndpointDevice::cConnectionStatusHandler::Release(void){	GetHost()->DecreaseReference();	return 0;	}
HRESULT cBluetoothLETunnelStreamEndpointDevice::cConnectionStatusHandler::Invoke(ABI::Windows::Devices::Bluetooth::IBluetoothLEDevice*,IInspectable*)
{
	GetHost()->ConnectionStatusEvent();
	return S_OK;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cBluetoothLETunnelStreamEndpointDevice *cBluetoothLETunnelStreamEndpointDevice::cGattServicesChangedHandler::GetHost(void)
{
	return cnMemory::GetObjectFromMemberPointer(this,&cBluetoothLETunnelStreamEndpointDevice::fGattServicesChangedHandler);
}
//---------------------------------------------------------------------------
HRESULT cBluetoothLETunnelStreamEndpointDevice::cGattServicesChangedHandler::QueryInterface( REFIID riid,_COM_Outptr_ void **ppvObject)
{
	if(impCOMQueryInterface<IUnknown,ABI::Windows::Foundation::ITypedEventHandler<ABI::Windows::Devices::Bluetooth::BluetoothLEDevice*,IInspectable*> >(this,riid,ppvObject)){
		GetHost()->IncreaseReference();
		return S_OK;
	}
	return E_NOINTERFACE;
}
ULONG cBluetoothLETunnelStreamEndpointDevice::cGattServicesChangedHandler::AddRef(void){	GetHost()->IncreaseReference();	return 0;	}
ULONG cBluetoothLETunnelStreamEndpointDevice::cGattServicesChangedHandler::Release(void){	GetHost()->DecreaseReference();	return 0;	}
HRESULT cBluetoothLETunnelStreamEndpointDevice::cGattServicesChangedHandler::Invoke(ABI::Windows::Devices::Bluetooth::IBluetoothLEDevice*,IInspectable*)
{
	GetHost()->GattServicesChangedEvent();
	return S_OK;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cBluetoothLETunnelStreamEndpointDevice *cBluetoothLETunnelStreamEndpointDevice::cNameChangedHandler::GetHost(void)
{
	return cnMemory::GetObjectFromMemberPointer(this,&cBluetoothLETunnelStreamEndpointDevice::fNameChangedHandler);
}
//---------------------------------------------------------------------------
HRESULT cBluetoothLETunnelStreamEndpointDevice::cNameChangedHandler::QueryInterface( REFIID riid,_COM_Outptr_ void **ppvObject)
{
	if(impCOMQueryInterface<IUnknown,ABI::Windows::Foundation::ITypedEventHandler<ABI::Windows::Devices::Bluetooth::BluetoothLEDevice*,IInspectable*> >(this,riid,ppvObject)){
		GetHost()->IncreaseReference();
		return S_OK;
	}
	return E_NOINTERFACE;
}
ULONG cBluetoothLETunnelStreamEndpointDevice::cNameChangedHandler::AddRef(void){	GetHost()->IncreaseReference();	return 0;	}
ULONG cBluetoothLETunnelStreamEndpointDevice::cNameChangedHandler::Release(void){	GetHost()->DecreaseReference();	return 0;	}
HRESULT cBluetoothLETunnelStreamEndpointDevice::cNameChangedHandler::Invoke(ABI::Windows::Devices::Bluetooth::IBluetoothLEDevice*,IInspectable*)
{
	 GetHost()->NameChangedEvent();
	 return S_OK;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cBluetoothLETunnelStreamEndpointDevice *cBluetoothLETunnelStreamEndpointDevice::cValueChangedHandler::GetHost(void)
{
	return cnMemory::GetObjectFromMemberPointer(this,&cBluetoothLETunnelStreamEndpointDevice::fValueChangedHandler);
}
//---------------------------------------------------------------------------
HRESULT cBluetoothLETunnelStreamEndpointDevice::cValueChangedHandler::QueryInterface( REFIID riid,_COM_Outptr_ void **ppvObject)
{
	if(impCOMQueryInterface<IUnknown,ABI::Windows::Foundation::ITypedEventHandler<ABI::Windows::Devices::Bluetooth::GenericAttributeProfile::GattCharacteristic*,ABI::Windows::Devices::Bluetooth::GenericAttributeProfile::GattValueChangedEventArgs*> >(this,riid,ppvObject)){
		GetHost()->IncreaseReference();
		return S_OK;
	}
	return E_NOINTERFACE;
}
ULONG cBluetoothLETunnelStreamEndpointDevice::cValueChangedHandler::AddRef(void){	GetHost()->IncreaseReference();	return 0;	}
ULONG cBluetoothLETunnelStreamEndpointDevice::cValueChangedHandler::Release(void){	GetHost()->DecreaseReference();	return 0;	}
HRESULT cBluetoothLETunnelStreamEndpointDevice::cValueChangedHandler::Invoke(ABI::Windows::Devices::Bluetooth::GenericAttributeProfile::IGattCharacteristic*,ABI::Windows::Devices::Bluetooth::GenericAttributeProfile::IGattValueChangedEventArgs *Args)
{
	COMPtr<ABI::Windows::Storage::Streams::IBuffer> Buffer;
	Args->get_CharacteristicValue(COMRetPtrT(Buffer));

	GetHost()->ValueChangedEvent(Buffer);
	return S_OK;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cBluetoothLETunnelStreamProtocol::cConnectAndCheckTask::cConnectAndCheckTask()
{
}
//---------------------------------------------------------------------------
cBluetoothLETunnelStreamProtocol::cConnectAndCheckTask::~cConnectAndCheckTask()
{
}
//---------------------------------------------------------------------------
cCoroutine<void> cBluetoothLETunnelStreamProtocol::cConnectAndCheckTask::RunAsync(iPtr<cWindowsBluetoothLEConectionDeviceInfo> Info,COMPtr< ABI::Windows::Foundation::IAsyncOperation<ABI::Windows::Devices::Bluetooth::BluetoothLEDevice*> > AsyncOp)
{
	auto Device=co_await WindowsAsyncOperationAwaiter(AsyncOp);
	if(Device!=nullptr){
		COMPtr<ABI::Windows::Devices::Bluetooth::IBluetoothLEDevice3> Device3;
		Device->QueryInterface(COMRetPtrT(Device3));
		COMPtr< ABI::Windows::Foundation::IAsyncOperation<ABI::Windows::Devices::Bluetooth::GenericAttributeProfile::GattDeviceServicesResult*> > ServiceOp;
		if(SUCCEEDED(Device3->GetGattServicesAsync(COMRetPtrT(ServiceOp)))){
			auto ServiceResult=co_await WindowsAsyncOperationAwaiter(ServiceOp);

			COMPtr< ABI::Windows::Foundation::Collections::IVectorView<ABI::Windows::Devices::Bluetooth::GenericAttributeProfile::GattDeviceService*> > Services;
			ServiceResult->get_Services(COMRetPtrT(Services));

			if(Services!=nullptr){
				unsigned int ServiceCount;
				Services->get_Size(&ServiceCount);

				for(unsigned int i=0;i<ServiceCount;i++){
					COMPtr<ABI::Windows::Devices::Bluetooth::GenericAttributeProfile::IGattDeviceService> Service;
					Services->GetAt(i,COMRetPtrT(Service));

					if(Service!=nullptr){
						GUID ServiceGUID;
						Service->get_Uuid(&ServiceGUID);

						for(uIntn DestServiceIndex=0;DestServiceIndex<Info->DeviceUUIDList.GetCount();DestServiceIndex++){
							auto &ServiceIDs=Info->DeviceUUIDList[DestServiceIndex];
							if(ServiceIDs.Service==ServiceGUID){
								// try get characterist
								COMPtr<ABI::Windows::Devices::Bluetooth::GenericAttributeProfile::IGattDeviceService3> Service3;
								Service->QueryInterface(COMRetPtrT(Service3));

								if(ServiceIDs.ReadCharacteristics==ServiceIDs.WriteCharacteristics){
								}
								else{
									COMPtr< ABI::Windows::Foundation::IAsyncOperation<ABI::Windows::Devices::Bluetooth::GenericAttributeProfile::GattCharacteristicsResult*> > GetCharOp;

									if(SUCCEEDED(Service3->GetCharacteristicsAsync(COMRetPtrT(GetCharOp)))){
										auto GetCharResult=co_await WindowsAsyncOperationAwaiter(GetCharOp);

										COMPtr< ABI::Windows::Foundation::Collections::IVectorView<ABI::Windows::Devices::Bluetooth::GenericAttributeProfile::GattCharacteristic*> > Characterists;
										if(SUCCEEDED(GetCharResult->get_Characteristics(COMRetPtrT(Characterists)))){

											// get match characterist

											unsigned int CharCount;
											Characterists->get_Size(&CharCount);

											COMPtr<ABI::Windows::Devices::Bluetooth::GenericAttributeProfile::IGattCharacteristic> ReadChar;
											COMPtr<ABI::Windows::Devices::Bluetooth::GenericAttributeProfile::IGattCharacteristic> WriteChar;

											for(uIntn DestCharIndex=0;DestCharIndex<CharCount;DestCharIndex++){
												COMPtr<ABI::Windows::Devices::Bluetooth::GenericAttributeProfile::IGattCharacteristic> Char;
												Characterists->GetAt(DestCharIndex,	COMRetPtrT(Char));

												GUID CharUUID;
												Char->get_Uuid(&CharUUID);
												if(CharUUID==ServiceIDs.ReadCharacteristics){
													ReadChar=cnVar::MoveCast(Char);
												}
												else if(CharUUID==ServiceIDs.WriteCharacteristics){
													WriteChar=cnVar::MoveCast(Char);
												}
											}
											if(ReadChar!=nullptr && WriteChar!=nullptr){
												// connect ok
												//WriteChar->get_CharacteristicProperties(
												// if(Can Notify){
												auto Endpoint=iCreate<cBluetoothLETunnelStreamEndpointDevice>(Device,ReadChar,WriteChar);

												COMPtr< ABI::Windows::Foundation::IAsyncOperation<enum ABI::Windows::Devices::Bluetooth::GenericAttributeProfile::GattCommunicationStatus> > SetReadNotifyAsyncOp;
												if(SUCCEEDED(ReadChar->WriteClientCharacteristicConfigurationDescriptorAsync(
													ABI::Windows::Devices::Bluetooth::GenericAttributeProfile::GattClientCharacteristicConfigurationDescriptorValue_Notify,
													COMRetPtrT(SetReadNotifyAsyncOp)
												))){
													auto SetCharNotifyResult=co_await WindowsAsyncOperationAwaiter(SetReadNotifyAsyncOp);
													if(SetCharNotifyResult==ABI::Windows::Devices::Bluetooth::GenericAttributeProfile::GattCommunicationStatus_Success){
														// success
														fConnection=CreateConnectionFromEndpoint(ConnectionTypeID,Endpoint,nullptr,nullptr,false);
													}
												}

												//}
												fTaskState.SetDone();
												co_return;
											}
										}
									}
								}
							}
						}
					}
					
				}
			}


			//Service->GetCharacteristics
			
		}
	}
	fTaskState.SetDone();
}
//---------------------------------------------------------------------------
bool cBluetoothLETunnelStreamProtocol::cConnectAndCheckTask::IsDone(void)
{
	return fTaskState.IsDone();
}
//---------------------------------------------------------------------------
bool cBluetoothLETunnelStreamProtocol::cConnectAndCheckTask::SetNotify(iProcedure *NotifyProcedure)
{
	return fTaskState.SetNotify(NotifyProcedure);
}
//---------------------------------------------------------------------------
void cBluetoothLETunnelStreamProtocol::cConnectAndCheckTask::Cancel(void)
{
	fAsyncInfo->Cancel();
}
//---------------------------------------------------------------------------
iConnection* cBluetoothLETunnelStreamProtocol::cConnectAndCheckTask::GetConnection(void)
{
	return fConnection;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cBluetoothLETunnelStreamProtocol::cBluetoothLETunnelStreamProtocol()
{
}
//---------------------------------------------------------------------------
cBluetoothLETunnelStreamProtocol::~cBluetoothLETunnelStreamProtocol()
{
}
//---------------------------------------------------------------------------
iPtr<iConnection> cBluetoothLETunnelStreamProtocol::Connect(iTypeID ConnectionIID,iAddress *RemoteAddress,iAddress *LocalAddress)
{
	return nullptr;
}
//---------------------------------------------------------------------------
iPtr<iConnectionTask> cBluetoothLETunnelStreamProtocol::ConnectAsync(iTypeID ConnectionIID,iAddress *RemoteAddress,iAddress *LocalAddress)
{
	if(LocalAddress!=nullptr)
		return nullptr;

	auto DeviceInfo=iCast<cWindowsBluetoothLEConectionDeviceInfo>(RemoteAddress);
	if(DeviceInfo==nullptr)
		return nullptr;

	COMPtr<ABI::Windows::Devices::Bluetooth::IBluetoothLEDeviceStatics> BLEStatic;
	cWindowsStringReference RCName_BLEDevice;
	RCName_BLEDevice.Create(RuntimeClass_Windows_Devices_Bluetooth_BluetoothLEDevice);
	if(FAILED(Windows::Foundation::GetActivationFactory(RCName_BLEDevice,COMRetPtrT(BLEStatic)))){
		return nullptr;
	}

	COMPtr< ABI::Windows::Foundation::IAsyncOperation<ABI::Windows::Devices::Bluetooth::BluetoothLEDevice*> > DeviceConnectAsyncOp;
	if(FAILED(BLEStatic->FromIdAsync(DeviceInfo->DeviceID,COMRetPtrT(DeviceConnectAsyncOp)))){
		return nullptr;
	}

	auto Task=iCreate<cConnectAndCheckTask>();
	Task->ConnectionTypeID=ConnectionIID;
	Task->RunAsync(DeviceInfo,cnVar::MoveCast(DeviceConnectAsyncOp));
	return Task;
}
//---------------------------------------------------------------------------
iPtr<iConnectionListener> cBluetoothLETunnelStreamProtocol::Listen(iTypeID ConnectionIID,iAddress *LocalAddress)
{
	return nullptr;
}
//---------------------------------------------------------------------------
iPtr<iConnectionQueue> cBluetoothLETunnelStreamProtocol::ListenQueue(iTypeID ConnectionIID,iAddress *LocalAddress)
{
	return nullptr;
}
/*
//---------------------------------------------------------------------------
void cBluetoothLETunnelStreamProtocol::BLEOnDiscovered(CBPeripheral *peripheral,NSDictionary *advertisementData,NSNumber *RSSI)
{
	auto &DeviceItem=fDeviceMap[peripheral];
	if(DeviceItem==nullptr){
//		NSLog(@"%p RSSI %@",peripheral,RSSI);
		//DeviceItem=CreateDevice
	}
	//DeviceItem->SetAdvertisementData(advertisementData);
}
//---------------------------------------------------------------------------
void cBluetoothLETunnelStreamProtocol::BLEOnConnected(CBPeripheral *Peripheral)
{
	auto ConnectPair=fConnectionTaskMap.GetPair(Peripheral);
	if(ConnectPair==nullptr)
		return;

	auto Task=cnVar::MoveCast(ConnectPair->Value);
	fConnectionTaskMap.RemovePair(ConnectPair);

	auto Endpoint=[Task->Device createEndpoint];
	if(Endpoint!=nullptr){
		//Task->Connection=cnRTL::CreateConnectionFromEndpoint(Task->ConnectionTypeID,Endpoint,
		//	nullptr,[Task->Device cnLib_AddressInterface],false);
	}
	Task->TaskState.SetDone();
}
//---------------------------------------------------------------------------
void cBluetoothLETunnelStreamProtocol::BLEOnConnectFailed(CBPeripheral *Peripheral,NSError *Error)
{
	auto ConnectPair=fConnectionTaskMap.GetPair(Peripheral);
	if(ConnectPair==nullptr)
		return;

	auto Task=cnVar::MoveCast(ConnectPair->Value);
	fConnectionTaskMap.RemovePair(ConnectPair);

	Task->Connection=nullptr;
	Task->TaskState.SetDone();
}
//---------------------------------------------------------------------------
void cBluetoothLETunnelStreamProtocol::BLEOnDisconnected(CBPeripheral *Peripheral,NSError *Error)
{
	auto ConnectPair=fConnectionTaskMap.GetPair(Peripheral);
	if(ConnectPair==nullptr)
		return;

	//auto Task=cnVar::MoveCast(ConnectPair->Value);
	//fConnectionTaskMap.RemovePair(ConnectPair);

	//Task->Connection=nullptr;
	//Task->TaskState.SetDone();
}
*/
//---------------------------------------------------------------------------


iConnectionProtocol *ScopeController::BLEConnectionProtocol;
iAddress *ScopeController::BLELocalMeade1DeviceAddress;


cApp::cApp()
{
	auto DispatchHigh=cnSystem::CurrentUIThread::GetDispatch(true);
	fTelescopeController=rCreate<cTelescopeController>();

	MainForm.ScopeControllerForm.SetTelescopeController(fTelescopeController);
}
cApp::~cApp()
{
}

void cApp::UIStarted(void)
{
	gAppModule.MainUIWindow->SetClient(MainForm);

	//fBLEConnectionQueue=BLEConnectionProtocol->ListenQueue(iTypeOf<iEndpointConnection>(),BLELocalMeade1DeviceAddress);
	//fBLEConnectionQueue->StartNotify(this);
	MainForm.TelescopeListForm.DeviceListSource.OnSelectDevice=[this](iAddress *Address){
		ConnectDevice(Address);
	};
}
//	virtual void cnLib_FUNC UIShow(void)override;
//	virtual void cnLib_FUNC UIResume(void)override;
//	virtual void cnLib_FUNC UIPaused(void)override;
//	virtual void cnLib_FUNC UIHide(void)override;

void cApp::UIStopped(void)
{
	gAppModule.MainUIWindow->SetClient(nullptr);
}

cCoroutine<void> cApp::ConnectDevice(iAddress *Address)
{
	auto ConnectTask=BLEConnectionProtocol->ConnectAsync(iTypeOf<iEndpointConnection>(),Address,nullptr);
	co_await TaskAwaiter(ConnectTask);

	auto Connection=ConnectTask->GetConnection();
	if(Connection!=nullptr){
		auto epConnection=iCast<iEndpointConnection>(Connection);

		auto Endpoint=epConnection->GetEndpoint();

		auto ProtocolProvider=rCreate<cProtocolProviderFromEndpoint>();
		ProtocolProvider->SetEndpoint(Endpoint);

		auto ClientProtocolProcessor=rCreate<cMeadeTelescope1ClientProtocolProcessor>();
		ClientProtocolProcessor->Connect(ProtocolProvider);


		fTelescopeController->SetMotor(ClientProtocolProcessor);
	}
}

cAppModule::cAppModule()
	: WindowsApplication(cnWindows::CreateWindowsUIApplication())
	, MainWindowProvider(cnWindows::CreateWindowProvider())
	, MainWindowClient(cnWindows::CreateWindowClient())
	, BLETunnelStreamProtocol(iCreate<cBluetoothLETunnelStreamProtocol>())
{
	BLEConnectionProtocol=BLETunnelStreamProtocol;

	MainWindowClient=cnWindows::CreateWindowClient();

	MainWindowProvider->Create(nullptr,L"Test");

	MainUIWindow=iCast<iUIWindow>(MainWindowClient);

	WindowsApplication->SetAppHandler(&App);
}

cAppModule::~cAppModule()
{
	UIReference.Wait();
}

void cAppModule::UIMain(void)
{
	auto Window=iCast<iWindow>(MainWindowProvider);

	MainWindowClient->SetWindow(Window);

	WindowsApplication->UIMain(Window,SW_SHOWDEFAULT);


	MainWindowClient->SetWindow(nullptr);
}



#endif // 0
