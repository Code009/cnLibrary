#include <coroutine>
#include "WinUWPGATT.h"

#pragma comment(lib,"WindowsApp.lib")


using namespace cnLibrary;
using namespace cnRTL;
using namespace cnWinRTL;

#if _WIN32_WINNT >= _WIN32_WINNT_WIN10

using namespace cnRTL::UWP;


cnLib_INTERFACE_LOCALID_DEFINE(cBluetoothAddress);


static const cUUID GATTBaseUUID={0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x10,0x80,0x00,0x00,0x80,0x5F,0x9B,0x34,0xFB};

cUUID UWP::GATTUUIDFromShort(ufInt32 ShortUUID)noexcept
{
	cUUID ID=GATTBaseUUID;
	*reinterpret_cast<uInt32*>(&ID)=cnMemory::SwapLittleEndian(static_cast<uInt32>(ShortUUID));
	return ID;
}

//---------------------------------------------------------------------------
template<class TAllocationOperator,class TElement,class TVectorClass>
inline HRESULT CopyVectorView(cnDataStruct::cArrayStorage<TAllocationOperator,TElement> &Storage,ABI::Windows::Foundation::Collections::IVectorView<TVectorClass> *Vector)noexcept
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
inline HRESULT CopyVectorView(cnDataStruct::cArrayStorage<TAllocationOperator,TElement> &Storage,const COMPtr<TVector> &Vector)noexcept
{
	return CopyVectorView(Storage,Vector.Pointer());
}
//---------------------------------------------------------------------------
eiOrdering cBluetoothAddress::Compare(iAddress *Dest)noexcept
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
cGATTDescriptor::cGATTDescriptor(cGATTCharacteristic *Owner,const cUUID &ID)noexcept
	: DescriptorUUID(ID)
	, fOwner(Owner)
	, fDispatch(Owner->GetDispatch())
	, fAvailability(GATTAvailability::Scanning)
{
}
//---------------------------------------------------------------------------
cGATTDescriptor::~cGATTDescriptor()noexcept
{
}
//---------------------------------------------------------------------------
void cGATTDescriptor::VirtualStarted(void)noexcept
{
	InnerActivate('self');
}
//---------------------------------------------------------------------------
void cGATTDescriptor::VirtualStopped(void)noexcept
{
	if(fDescriptor!=nullptr){
		ClearDescriptor();
	}
	InnerDecReference('self');
}
//---------------------------------------------------------------------------
cUUID cGATTDescriptor::GetUUID(void)noexcept
{
	return DescriptorUUID;
}
//---------------------------------------------------------------------------
iDispatch* cGATTDescriptor::GetHandlerDispatch(void)noexcept
{
	return fDispatch;
}
//---------------------------------------------------------------------------
bool cGATTDescriptor::InsertHandler(iGATTDescriptorHandler *Handler)noexcept
{
	if(fDispatch->IsCurrentThread()==false)
		return false;
	fHandlers.Insert(Handler);
	return true;
}
//---------------------------------------------------------------------------
bool cGATTDescriptor::RemoveHandler(iGATTDescriptorHandler *Handler)noexcept
{
	if(fDispatch->IsCurrentThread()==false)
		return false;
	fHandlers.Insert(Handler);
	return true;
}
//---------------------------------------------------------------------------
eGATTAvailability cGATTDescriptor::GetAvailability(void)noexcept
{
	return fAvailability;
}
//---------------------------------------------------------------------------
iGATTCharacteristic* cGATTDescriptor::GetCharacterist(void)noexcept
{
	return fOwner;
}
//---------------------------------------------------------------------------
void cGATTDescriptor::SetupDescriptor(void)noexcept
{
	//fDevice->add_NameChanged(&fBLEDeviceNameChangedHandler,&fBLEDeviceNameChangedToken);
	//fDevice->add_ConnectionStatusChanged(&fBLEDeviceConnectionStatusHandler,&fBLEDeviceConnectionStatusToken);
	//fDevice->add_GattServicesChanged(&fBLEDeviceServicesChangedHandler,&fBLEDeviceServicesChangedToken);
}
//---------------------------------------------------------------------------
void cGATTDescriptor::ClearDescriptor(void)noexcept
{
	//fDevice->remove_NameChanged(fBLEDeviceNameChangedToken);
	//fDevice->remove_ConnectionStatusChanged(fBLEDeviceConnectionStatusToken);
	//fDevice->remove_GattServicesChanged(fBLEDeviceServicesChangedToken);
}
//---------------------------------------------------------------------------
void cGATTDescriptor::CharacteristicUpdateDescriptor(IBLEDescriptor *Descriptor)noexcept
{
	if(fDescriptor!=nullptr){
		HRESULT hr;
		UINT16 ahNew;
		UINT16 ahOld;
		hr=Descriptor->get_AttributeHandle(&ahNew);
		hr=fDescriptor->get_AttributeHandle(&ahOld);
		if(ahOld==ahNew)
			return;

		ClearDescriptor();
	}

	fDescriptor=Descriptor;

	if(fDescriptor!=nullptr){
		SetupDescriptor();
	}

	UpdateFuncState();
}
//---------------------------------------------------------------------------
void cGATTDescriptor::CharacteristicNotifyFunctionStatus(void)noexcept
{
	UpdateFuncState();
}
//---------------------------------------------------------------------------
void cGATTDescriptor::UpdateFuncState(void)noexcept
{
	eGATTAvailability NewState;
	if(fDescriptor==nullptr){
		if(fOwner->DescriptorIsRefreshing()){
			NewState=GATTAvailability::Scanning;
		}
		else{
			NewState=GATTAvailability::Absent;
		}
	}
	else{
		NewState=GATTAvailability::Available;
	}

	if(fAvailability!=NewState){
		fAvailability=NewState;

		for(auto Handler : fHandlers){
			Handler->GATTDescriptorAvailabilityChanged();
		}
	}
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cGATTCharacteristic::cGATTCharacteristic(cGATTService *Owner,const cUUID &ID)noexcept
	: CharacteristcUUID(ID)
	, fOwner(Owner)
	, fDispatch(Owner->GetDispatch())
	, fMainProcessWork(Owner->GetDispatch()->CreateWork(&fInnerReference,&fMainProcessProcedure))
	, fAvailability(GATTAvailability::Scanning)
	, fMainProcessState(psIdle)
{
	fOwner->CharacteristicAttach(this);
}
//---------------------------------------------------------------------------
cGATTCharacteristic::~cGATTCharacteristic()noexcept
{
	fOwner->CharacteristicDetach(this);
}
//---------------------------------------------------------------------------
void cGATTCharacteristic::VirtualStarted(void)noexcept
{
	InnerActivate('self');
}
//---------------------------------------------------------------------------
void cGATTCharacteristic::VirtualStopped(void)noexcept
{
	if(fCharacteristic!=nullptr){
		ClearCharacteristic();
	}
	InnerDecReference('self');
}
//---------------------------------------------------------------------------
iDispatch* cGATTCharacteristic::GetDispatch(void)const noexcept
{
	return fDispatch;
}
//---------------------------------------------------------------------------
cUUID cGATTCharacteristic::GetUUID(void)noexcept
{
	return CharacteristcUUID;
}
//---------------------------------------------------------------------------
iDispatch* cGATTCharacteristic::GetHandlerDispatch(void)noexcept
{
	return fDispatch;
}
//---------------------------------------------------------------------------
bool cGATTCharacteristic::InsertHandler(iGATTCharacteristicHandler *Handler)noexcept
{
	if(fDispatch->IsCurrentThread()==false)
		return false;
	fHandlers.Insert(Handler);
	return true;
}
//---------------------------------------------------------------------------
bool cGATTCharacteristic::RemoveHandler(iGATTCharacteristicHandler *Handler)noexcept
{
	if(fDispatch->IsCurrentThread()==false)
		return false;
	fHandlers.Remove(Handler);
	return true;
}
//---------------------------------------------------------------------------
eGATTAvailability cGATTCharacteristic::GetAvailability(void)noexcept
{
	return fAvailability;
}
//---------------------------------------------------------------------------
iGATTService* cGATTCharacteristic::GetService(void)noexcept
{
	return fOwner;
}
//---------------------------------------------------------------------------
rPtr<iGATTDescriptor> cGATTCharacteristic::AccessDescriptor(const cUUID &ID)noexcept
{
	if(fDispatch->IsCurrentThread()==false)
		return nullptr;
	auto Descriptor=rCreate<cGATTDescriptor>(this,ID);
	return Descriptor;
}
//---------------------------------------------------------------------------
rPtr<iGATTDescriptorObserver> cGATTCharacteristic::CreateDescriptorObserver(void)noexcept
{
	return nullptr;
}
//---------------------------------------------------------------------------
iPtr< iAsyncFunction<cConstMemory> > cGATTCharacteristic::Read(void)noexcept
{
	HRESULT hr;
	ABI::Windows::Devices::Bluetooth::GenericAttributeProfile::GattCharacteristicProperties prop;
	hr=fCharacteristic->get_CharacteristicProperties(&prop);
	ABI::Windows::Devices::Bluetooth::GenericAttributeProfile::GattProtectionLevel pl;
	
	hr=fCharacteristic->get_ProtectionLevel(&pl);
	COMPtr<IBLEReadAsyncOp> ReadOp;
	hr=fCharacteristic->ReadValueAsync(COMRetPtrT(ReadOp));

	ReadOp->put_Completed(&fReadCompleteHandler);
	return nullptr;
}
//---------------------------------------------------------------------------
iPtr<iAsyncTask> cGATTCharacteristic::Write(const void *Data,uIntn DataSize)noexcept
{
	HRESULT hr;
	auto SendBuffer=MakeBufferFromData(Data,DataSize);
	COMPtr<IBLECommunicationAsyncOp> WriteOp;
	hr=fCharacteristic->WriteValueAsync(SendBuffer,COMRetPtrT(WriteOp));

	hr=WriteOp->put_Completed(&fWriteCompleteHandler);
	return nullptr;
}
//---------------------------------------------------------------------------
bool cGATTCharacteristic::WriteWithoutResponse(const void *Data,uIntn DataSize)noexcept
{
	HRESULT hr;
	auto SendBuffer=MakeBufferFromData(Data,DataSize);
	COMPtr<IBLECommunicationAsyncOp> WriteOp;
	hr=fCharacteristic->WriteValueWithOptionAsync(SendBuffer,ABI::Windows::Devices::Bluetooth::GenericAttributeProfile::GattWriteOption_WriteWithoutResponse,COMRetPtrT(WriteOp));

	hr=WriteOp->put_Completed(&fWriteCompleteHandler);

	return false;
}
//---------------------------------------------------------------------------
eGATTCharacteristicNotification cGATTCharacteristic::EffectiveValueNotification(void)noexcept
{
	return fEffectiveNotificationConfig;
}
//---------------------------------------------------------------------------
eGATTCharacteristicNotification cGATTCharacteristic::GetValueNotification(void)noexcept
{
	return fTargetNotificationConfig;
}
//---------------------------------------------------------------------------
void cGATTCharacteristic::SetValueNotification(eGATTCharacteristicNotification Notification)noexcept
{
	fTargetNotificationConfig=Notification;
	NotifyMainProcess();
}
//---------------------------------------------------------------------------
void cGATTCharacteristic::SetupCharacteristic(void)noexcept
{
	cnLib_ASSERT(fCharacteristic3==nullptr);
	fCharacteristic->QueryInterface(COMRetPtrT(fCharacteristic3));

	fCharacteristic->add_ValueChanged(&fBLEValueChangedHandler,&fBLEValueChangedToken);
}
//---------------------------------------------------------------------------
void cGATTCharacteristic::ClearCharacteristic(void)noexcept
{
	fCharacteristic3=nullptr;
	fCharacteristic->remove_ValueChanged(fBLEValueChangedToken);
}
//---------------------------------------------------------------------------
void cGATTCharacteristic::ServiceInvalidateCharacteristic(void)noexcept
{
	fNeedNotifyCharacteristicState=true;
	NotifyMainProcess();

	for(auto CharPair : fDescriptorMap){
		CharPair.Value->CharacteristicNotifyFunctionStatus();
	}
}
//---------------------------------------------------------------------------
void cGATTCharacteristic::ServiceUpdateCharacteristic(IBLECharacteristic *Characteristic)noexcept
{
	if(fCharacteristic!=nullptr){
		if(Characteristic!=nullptr){
			HRESULT hr;
			UINT16 ahNew;
			UINT16 ahOld;
			hr=Characteristic->get_AttributeHandle(&ahNew);
			hr=fCharacteristic->get_AttributeHandle(&ahOld);
			if(ahOld==ahNew){
				return;
			}
		}
		ClearCharacteristic();
	}

	fCharacteristic=Characteristic;

	if(fCharacteristic!=nullptr){
		SetupCharacteristic();
	}

	fEffectiveNotificationConfig=GATTCharacteristicNotification::None;

	fDescriptorNeedRefresh=true;
	fNeedNotifyCharacteristicState=true;
	NotifyMainProcess();
}
//---------------------------------------------------------------------------
void cGATTCharacteristic::ServiceNotifyFunctionStatus(void)noexcept
{
	fNeedNotifyCharacteristicState=true;
	NotifyMainProcess();
}
//---------------------------------------------------------------------------
void cGATTCharacteristic::DescriptorAttach(cGATTDescriptor *Descriptor)noexcept
{
	fDescriptorMap[Descriptor->DescriptorUUID]=Descriptor;
}
//---------------------------------------------------------------------------
void cGATTCharacteristic::DescriptorDetach(cGATTDescriptor *Descriptor)noexcept
{
	fDescriptorMap.Remove(Descriptor->DescriptorUUID);
}
//---------------------------------------------------------------------------
bool cGATTCharacteristic::DescriptorIsRefreshing(void)const noexcept
{
	return fMainProcessState==psRefreshDescriptor;
}
//---------------------------------------------------------------------------
void cGATTCharacteristic::cMainProcessProcedure::Execute(void)noexcept
{
	auto Host=cnMemory::GetObjectFromMemberPointer(this,&cGATTCharacteristic::fMainProcessProcedure);
	return Host->MainProcessThread();
}
//---------------------------------------------------------------------------
void cGATTCharacteristic::MainProcessThread(void)noexcept
{
	do{
		do{
			fMainProcessExclusiveFlag.Continue();

		}while(MainProcess());

	}while(!fMainProcessExclusiveFlag.Release());
}
//---------------------------------------------------------------------------
void cGATTCharacteristic::NotifyMainProcess(void)noexcept
{
	if(!fMainProcessExclusiveFlag.Acquire())
		return;

	fMainProcessWork->Start();		
}
//---------------------------------------------------------------------------
bool cGATTCharacteristic::MainProcess(void)noexcept
{
	MainProcessStateChange();

	if(fCharacteristic==nullptr)
		return false;

	switch(fMainProcessState){
	default:
		return false;
	case psIdle:
		return MainProcess_Idle();
	case psRefreshDescriptorDone:
		MainProcess_RefreshDescriptorDone();
		return true;
	}
}
//---------------------------------------------------------------------------
void cGATTCharacteristic::MainProcessStateChange(void)noexcept
{
	if(fNeedNotifyCharacteristicState){
		fNeedNotifyCharacteristicState=false;
		
		eGATTAvailability NewState;
		if(fCharacteristic==nullptr){
			if(fOwner->CharacteristicIsRefreshing()){
				NewState=GATTAvailability::Scanning;
			}
			else{
				NewState=GATTAvailability::Absent;
			}
		}
		else{
			NewState=GATTAvailability::Available;
		}

		if(fAvailability!=NewState){
			fAvailability=NewState;

			cSeqList< rInnerPtr<cGATTDescriptor> > DescListCache;
			for(auto Pair : fDescriptorMap){
				DescListCache.AppendMake(Pair.Value);
			}

			for(auto Desc : DescListCache){
				Desc->CharacteristicNotifyFunctionStatus();
			}

			for(auto Handler : fHandlers){
				Handler->GATTCharacteristAvailabilityChanged();
			}
		}
	}

	
	auto ValueNotificationItems=fValueNotificationQueue.DequeueAll();
	while(ValueNotificationItems!=nullptr){
		auto NotificationItem=ValueNotificationItems;
		ValueNotificationItems=ValueNotificationItems->Next;

		for(auto Handler : fHandlers){
			Handler->GATTCharacteristValueNotify(NotificationItem->Value->Pointer,NotificationItem->Value->Length);
		}
		fValueNotificationRecycler.Recycle(NotificationItem);
	}
}
//---------------------------------------------------------------------------
bool cGATTCharacteristic::MainProcess_Idle(void)noexcept
{
	HRESULT hr;


	if(fDescriptorNeedRefresh){
		fDescriptorNeedRefresh=false;

		COMPtr<IBLEGetDescriptorsAsyncOp> CharOp;
		hr=fCharacteristic3->GetDescriptorsAsync(COMRetPtrT(CharOp));
		if(SUCCEEDED(hr)){
			fMainProcessState=psRefreshDescriptor;

			for(auto Pair : fDescriptorMap){
				Pair.Value->CharacteristicNotifyFunctionStatus();
			}

			hr=CharOp->put_Completed(&fGetDescriptorsCompleteHandler);

			// wait refresh complete
			return false;
		}
	}

	if(fConfigureNotificationState==0){
		if(fCharacteristic3!=nullptr && fTargetNotificationConfig!=fEffectiveNotificationConfig){

			fWritingNotificationConfig=fTargetNotificationConfig;
			auto Value=static_cast<ABI::Windows::Devices::Bluetooth::GenericAttributeProfile::GattClientCharacteristicConfigurationDescriptorValue>(fWritingNotificationConfig);
			COMPtr<IBLEWriteAsyncOp> WriteOp;
			hr=fCharacteristic3->WriteClientCharacteristicConfigurationDescriptorWithResultAsync(Value,COMRetPtrT(WriteOp));
			if(SUCCEEDED(hr)){
				fConfigureNotificationState=1;
			
				hr=WriteOp->put_Completed(&fConfigureNotificationHandler);
				if(FAILED(hr)){
					fConfigureNotificationState=0;
				}
			}
		}
	}
	else if(fConfigureNotificationState==2){

		fConfigureNotificationState=0;

		GattCommunicationStatus Status;
		hr=fConfigureNotificationHandler.Result->get_Status(&Status);
		if(Status==GattCommunicationStatus::GattCommunicationStatus_Success){
			fEffectiveNotificationConfig=fWritingNotificationConfig;

			for(auto Handler : fHandlers){
				Handler->GATTCharacteristEffectiveValueNotificationChanged();
			}
		}
		else{
			// need retry
			return true;
		}
	}

	return false;
}
//---------------------------------------------------------------------------
void cGATTCharacteristic::MainProcess_RefreshDescriptorDone(void)noexcept
{
	if(fGetDescriptorsCompleteHandler.ResultDescriptors!=nullptr){
		HRESULT hr;
		auto ResultDescriptors=cnVar::MoveCast(fGetDescriptorsCompleteHandler.ResultDescriptors);
		ABI::Windows::Devices::Bluetooth::GenericAttributeProfile::GattCommunicationStatus Status;
		hr=ResultDescriptors->get_Status(&Status);

		COMPtr< ABI::Windows::Foundation::Collections::IVectorView<ABI::Windows::Devices::Bluetooth::GenericAttributeProfile::GattDescriptor*> > DescriptorVector;
		hr=ResultDescriptors->get_Descriptors(COMRetPtrT(DescriptorVector));
		
		cSeqList<IBLEDescriptor*> PtrList;
		hr=CopyVectorView(PtrList.Storage(),DescriptorVector);


		for(auto CharPair : fDescriptorMap){
			fUpdateMap[CharPair.Key].Descriptor=CharPair.Value;
		}
	
		for(uIntn i=0,c=PtrList.GetCount();i<c;i++){
			auto BLEDesc=COMTake(PtrList[i]);

			GUID IDValue;
			hr=BLEDesc->get_Uuid(&IDValue);

			auto Pair=fUpdateMap.GetPair(reinterpret_cast<cUUID&>(IDValue));
			if(Pair!=nullptr){
				Pair->Value.BLEDescriptor=BLEDesc;
			}

		}

		for(auto Pair : fUpdateMap){
			Pair.Value.Descriptor->CharacteristicUpdateDescriptor(Pair.Value.BLEDescriptor);
		}

		fUpdateMap.Clear();

	}
	else{
		fMainProcessState=psIdle;
	}
}
//---------------------------------------------------------------------------
void cGATTCharacteristic::NotifyValue(uInt64 Timestamp,const void *Data,uIntn Size)noexcept
{
	auto ValueNotificationItem=fValueNotificationRecycler.Query();
	ValueNotificationItem->Timestamp=Timestamp;
	ValueNotificationItem->Value.Copy(Data,Size);

	cnLib_ASSERT(ValueNotificationItem->Value.GetSize()==Size);

	fValueNotificationQueue.Enqueue(ValueNotificationItem);
	NotifyMainProcess();
}
//---------------------------------------------------------------------------
cGATTCharacteristic* cGATTCharacteristic::cBLEValueChangedHandler::GetHost(void)noexcept
{
	return cnMemory::GetObjectFromMemberPointer(this,&cGATTCharacteristic::fBLEValueChangedHandler);
}
//---------------------------------------------------------------------------
void cGATTCharacteristic::cBLEValueChangedHandler::EventIncReference(void)noexcept
{
	GetHost()->InnerIncReference('vchg');
}
void cGATTCharacteristic::cBLEValueChangedHandler::EventDecReference(void)noexcept
{
	GetHost()->InnerDecReference('vchg');
}
void cGATTCharacteristic::cBLEValueChangedHandler::EventInvoke(IBLECharacteristic*,ABI::Windows::Devices::Bluetooth::GenericAttributeProfile::IGattValueChangedEventArgs *args)noexcept
{
	COMPtr<ABI::Windows::Storage::Streams::IBuffer> Buffer;
	ABI::Windows::Foundation::DateTime ValueTimeStamp;
	HRESULT hr;
	hr=args->get_Timestamp(&ValueTimeStamp);
	if(FAILED(hr))
		return;
	hr=args->get_CharacteristicValue(COMRetPtrT(Buffer));
	if(FAILED(hr))
		return;
	if(Buffer!=nullptr){
		auto BufferMemory=UWP::GetBufferData(Buffer);
		GetHost()->NotifyValue(ValueTimeStamp.UniversalTime,BufferMemory.Pointer,BufferMemory.Length);
	}
}
//---------------------------------------------------------------------------
cGATTCharacteristic* cGATTCharacteristic::cGetDescriptorsCompleteHandler::GetHost(void)noexcept
{
	return cnMemory::GetObjectFromMemberPointer(this,&cGATTCharacteristic::fGetDescriptorsCompleteHandler);
}
//---------------------------------------------------------------------------
void cGATTCharacteristic::cGetDescriptorsCompleteHandler::EventIncReference(void)noexcept
{
	GetHost()->InnerIncReference('getd');
}
void cGATTCharacteristic::cGetDescriptorsCompleteHandler::EventDecReference(void)noexcept
{
	GetHost()->InnerDecReference('getd');
}
void cGATTCharacteristic::cGetDescriptorsCompleteHandler::EventInvoke(IBLEGetDescriptorsAsyncOp *AsyncOp,ABI::Windows::Foundation::AsyncStatus)noexcept
{
	hrResult=AsyncOp->GetResults(COMRetPtrT(ResultDescriptors));

	auto Host=GetHost();
	Host->fMainProcessState=psRefreshDescriptorDone;
	Host->NotifyMainProcess();
}
//---------------------------------------------------------------------------
cGATTCharacteristic* cGATTCharacteristic::cReadCompleteHandler::GetHost(void)noexcept
{
	return cnMemory::GetObjectFromMemberPointer(this,&cGATTCharacteristic::fReadCompleteHandler);
}
//---------------------------------------------------------------------------
void cGATTCharacteristic::cReadCompleteHandler::EventIncReference(void)noexcept
{
	GetHost()->InnerIncReference('read');
}
void cGATTCharacteristic::cReadCompleteHandler::EventDecReference(void)noexcept
{
	GetHost()->InnerDecReference('read');
}
void cGATTCharacteristic::cReadCompleteHandler::EventInvoke(IBLEReadAsyncOp *AsyncOp,ABI::Windows::Foundation::AsyncStatus)noexcept
{
	COMPtr<ABI::Windows::Devices::Bluetooth::GenericAttributeProfile::IGattReadResult> Result;
	HRESULT hr=AsyncOp->GetResults(COMRetPtrT(Result));

	COMPtr<ABI::Windows::Storage::Streams::IBuffer> ValueBuffer;
	hr=Result->get_Value(COMRetPtrT(ValueBuffer));


	auto BufferCcontent=GetBufferData(ValueBuffer);

}
//---------------------------------------------------------------------------
cGATTCharacteristic* cGATTCharacteristic::cWriteCompleteHandler::GetHost(void)noexcept
{
	return cnMemory::GetObjectFromMemberPointer(this,&cGATTCharacteristic::fWriteCompleteHandler);
}
//---------------------------------------------------------------------------
void cGATTCharacteristic::cWriteCompleteHandler::EventIncReference(void)noexcept
{
	GetHost()->InnerIncReference('wrtr');
}
void cGATTCharacteristic::cWriteCompleteHandler::EventDecReference(void)noexcept
{
	GetHost()->InnerDecReference('wrtr');
}
void cGATTCharacteristic::cWriteCompleteHandler::EventInvoke(IBLECommunicationAsyncOp *AsyncOp,ABI::Windows::Foundation::AsyncStatus)noexcept
{
	GattCommunicationStatus Result=GattCommunicationStatus::GattCommunicationStatus_ProtocolError;
	HRESULT hr=AsyncOp->GetResults(&Result);
}
//---------------------------------------------------------------------------
cGATTCharacteristic* cGATTCharacteristic::cConfigureNotificationHandler::GetHost(void)noexcept
{
	return cnMemory::GetObjectFromMemberPointer(this,&cGATTCharacteristic::fConfigureNotificationHandler);
}
//---------------------------------------------------------------------------
void cGATTCharacteristic::cConfigureNotificationHandler::EventIncReference(void)noexcept
{
	rIncReference(GetHost(),'cfgn');
}
void cGATTCharacteristic::cConfigureNotificationHandler::EventDecReference(void)noexcept
{
	rDecReference(GetHost(),'cfgn');
}
void cGATTCharacteristic::cConfigureNotificationHandler::EventInvoke(IBLEWriteAsyncOp *AsyncOp,ABI::Windows::Foundation::AsyncStatus Status)noexcept
{
	hrResult=AsyncOp->GetResults(COMRetPtrT(Result));

	auto Host=GetHost();
	Host->fConfigureNotificationState=2;
	Host->NotifyMainProcess();
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void cGATTService::cOpenServiceProcedure::GetService(ABI::Windows::Devices::Bluetooth::IBluetoothLEDevice3 *Device)noexcept
{
	COMPtr<IBLEGetServiceAsyncOp> ServiceOp;
	HRESULT hr=Device->GetGattServicesForUuidWithCacheModeAsync(reinterpret_cast<const GUID&>(Service->ServiceUUID),ABI::Windows::Devices::Bluetooth::BluetoothCacheMode_Cached,COMRetPtrT(ServiceOp));
	if(SUCCEEDED(hr)){
		hr=ServiceOp->put_Completed(&fGetServicesCompleteHandler);
		if(SUCCEEDED(hr)){
			// wait for completion handler calls ServiceScanned
			return;
		}
	}
	// failed
}
//---------------------------------------------------------------------------
void cGATTService::cOpenServiceProcedure::OnServiceResult(IBLEGetServiceAsyncOp *AsyncOp)noexcept
{
	OnServiceResultProcess(AsyncOp);
}
//---------------------------------------------------------------------------
bool cGATTService::cOpenServiceProcedure::OnServiceResultProcess(IBLEGetServiceAsyncOp *AsyncOp)noexcept
{
	HRESULT hr;
	COMPtr<IGattDeviceServiceResult> ResultServices;
	hr=AsyncOp->GetResults(COMRetPtrT(ResultServices));
	if(FAILED(hr))
		return false;

	ABI::Windows::Devices::Bluetooth::GenericAttributeProfile::GattCommunicationStatus Status;
	hr=ResultServices->get_Status(&Status);

	if(FAILED(hr))
		return false;

	COMPtr< ABI::Windows::Foundation::Collections::IVectorView<BLEService*> > ServiceVector;
	hr=ResultServices->get_Services(COMRetPtrT(ServiceVector));
	
	unsigned int ServiceCount;
	hr=ServiceVector->get_Size(&ServiceCount);
	if(FAILED(hr))
		return false;
	
	for(unsigned int i=0;i<ServiceCount;i++){
		COMPtr<IBLEService> BLEService;
		hr=ServiceVector->GetAt(i,COMRetPtrT(BLEService));

		union{
			GUID IDValue;
			cUUID UUID;
		};
		hr=BLEService->get_Uuid(&IDValue);

		if(cnMemory::IsEqual(&UUID,&Service->ServiceUUID,sizeof(cUUID))){
			this->ServiceScanned(BLEService);
			return true;
		}

	}
	// service cannot be found
	return false;
}
//---------------------------------------------------------------------------
void cGATTService::cOpenServiceProcedure::ServiceScanned(IBLEService *BLEService)noexcept
{
	fService=BLEService;
	HRESULT hr;
	hr=BLEService->QueryInterface(COMRetPtrT(fService3));
	COMPtr<IBLEOpenAsyncOp> AsyncOp;
	hr=fService3->OpenAsync(ABI::Windows::Devices::Bluetooth::GenericAttributeProfile::GattSharingMode_SharedReadAndWrite,COMRetPtrT(AsyncOp));
	hr=AsyncOp->put_Completed(&fOpenAsyncCompleteHandler);
}
//---------------------------------------------------------------------------
void cGATTService::cOpenServiceProcedure::OpenResult(IBLEOpenAsyncOp *AsyncOp)noexcept
{
	ABI::Windows::Devices::Bluetooth::GenericAttributeProfile::GattOpenStatus Status;
	HRESULT hr=AsyncOp->GetResults(&Status);
	auto Dispatch=Service->GetDispatch();
	auto OpenProc=rTake(this,'open');

	Dispatch->Execute(this,&fOpenResultProcedure);
}
//---------------------------------------------------------------------------
void cGATTService::cOpenServiceProcedure::cOpenResultProcedure::Execute(void)noexcept
{
	auto Host=cnMemory::GetObjectFromMemberPointer(this,&cOpenServiceProcedure::fOpenResultProcedure);
	Host->OpenSuccessThread();
}
//---------------------------------------------------------------------------
void cGATTService::cOpenServiceProcedure::OpenSuccessThread(void)noexcept
{
	Service->ServiceOpened(cnVar::MoveCast(fService),cnVar::MoveCast(fService3));
}
//---------------------------------------------------------------------------
cGATTService::cOpenServiceProcedure* cGATTService::cOpenServiceProcedure::cGetServicesCompleteHandler::GetHost(void)noexcept
{
	return cnMemory::GetObjectFromMemberPointer(this,&cOpenServiceProcedure::fGetServicesCompleteHandler);
}
//---------------------------------------------------------------------------
void cGATTService::cOpenServiceProcedure::cGetServicesCompleteHandler::EventIncReference(void)noexcept
{
	rIncReference(GetHost(),'gets');
}
void cGATTService::cOpenServiceProcedure::cGetServicesCompleteHandler::EventDecReference(void)noexcept
{
	rDecReference(GetHost(),'gets');
}
void cGATTService::cOpenServiceProcedure::cGetServicesCompleteHandler::EventInvoke(IBLEGetServiceAsyncOp *AsyncOp,ABI::Windows::Foundation::AsyncStatus)noexcept
{
	GetHost()->OnServiceResult(AsyncOp);
}
//---------------------------------------------------------------------------
cGATTService::cOpenServiceProcedure* cGATTService::cOpenServiceProcedure::cOpenAsyncCompleteHandler::GetHost(void)noexcept
{
	return cnMemory::GetObjectFromMemberPointer(this,&cOpenServiceProcedure::fOpenAsyncCompleteHandler);
}
//---------------------------------------------------------------------------
void cGATTService::cOpenServiceProcedure::cOpenAsyncCompleteHandler::EventIncReference(void)noexcept
{
	rIncReference(GetHost(),'opsv');
}
void cGATTService::cOpenServiceProcedure::cOpenAsyncCompleteHandler::EventDecReference(void)noexcept
{
	rDecReference(GetHost(),'opsv');
}
void cGATTService::cOpenServiceProcedure::cOpenAsyncCompleteHandler::EventInvoke(IBLEOpenAsyncOp *AsyncOp,ABI::Windows::Foundation::AsyncStatus)noexcept
{
	GetHost()->OpenResult(AsyncOp);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cGATTService::cGATTService(cBluetoothSlave *Owner,const cUUID &ID)noexcept
	: ServiceUUID(ID)
	, fOwner(Owner)
	, fDispatch(Owner->GetDispatch())
	, fMainProcessWork(Owner->GetDispatch()->CreateWork(&fInnerReference,&fMainProcessProcedure))
	, fAvailability(GATTAvailability::Scanning)
	, fMainProcessState(psIdle)
	, fScaningService(false)
{
	fOwner->ServiceAttach(this);
}
//---------------------------------------------------------------------------
cGATTService::~cGATTService()noexcept
{
	fOwner->ServiceDetach(this);
}
//---------------------------------------------------------------------------
void cGATTService::VirtualStarted(void)noexcept
{
	InnerActivate('self');
}
//---------------------------------------------------------------------------
void cGATTService::VirtualStopped(void)noexcept
{
	InnerDecReference('self');
}
//---------------------------------------------------------------------------
iDispatch* cGATTService::GetDispatch(void)const noexcept
{
	return fDispatch;
}
//---------------------------------------------------------------------------
cUUID cGATTService::GetUUID(void)noexcept
{
	return ServiceUUID;
}
//---------------------------------------------------------------------------
iDispatch* cGATTService::GetHandlerDispatch(void)noexcept
{
	return fDispatch;
}
//---------------------------------------------------------------------------
bool cGATTService::InsertHandler(iGATTServiceHandler *Handler)noexcept
{
	if(fDispatch->IsCurrentThread()==false)
		return false;
	fHandlers.Insert(Handler);
	return true;
}
//---------------------------------------------------------------------------
bool cGATTService::RemoveHandler(iGATTServiceHandler *Handler)noexcept
{
	if(fDispatch->IsCurrentThread()==false)
		return false;
	fHandlers.Remove(Handler);
	return true;
}
//---------------------------------------------------------------------------
eGATTAvailability cGATTService::GetAvailability(void)noexcept
{
	return fAvailability;
}
//---------------------------------------------------------------------------
iGATTClient* cGATTService::GetClient(void)noexcept
{
	return fOwner;
}
//---------------------------------------------------------------------------
rPtr<iGATTCharacteristic> cGATTService::AccessCharacteristic(const cUUID &ID)noexcept
{
	if(fDispatch->IsCurrentThread()==false)
		return nullptr;
	auto Characteristic=rCreate<cGATTCharacteristic>(this,ID);
	return Characteristic;
}
//---------------------------------------------------------------------------
iPtr<iGATTCharacteristicObserver> cGATTService::CreateCharacteristicObserver(void)noexcept
{
	return nullptr;
}
//---------------------------------------------------------------------------
void cGATTService::ServiceOpened(COMPtr<IBLEService> Service,COMPtr<IBLEService3> Service3)noexcept
{
	cnLib_ASSERT(fService==nullptr);
	fScaningService=false;
	
	fService=cnVar::MoveCast(Service);
	fService3=cnVar::MoveCast(Service3);

	fCharacteristicNeedRefresh=true;
	fNeedNotifyServiceState=true;
	NotifyMainProcess();
}
//---------------------------------------------------------------------------
void cGATTService::ServiceOpenFailed(void)noexcept
{
	fScaningService=false;

	fCharacteristicNeedRefresh=true;
	fNeedNotifyServiceState=true;
	NotifyMainProcess();
}
//---------------------------------------------------------------------------
void cGATTService::PeripheralNotifyClose(void)noexcept
{
	fCloseState=1;
	RunMainProcess();
}
//---------------------------------------------------------------------------
void cGATTService::PeripheralNotifyScanService(void)noexcept
{
	auto OpenProc=rCreate<cOpenServiceProcedure>();
	OpenProc->Service=this;
	fScaningService=true;

	fOwner->ServiceScan(rExtract(OpenProc,'open'));
}
//---------------------------------------------------------------------------
void cGATTService::PeripheralNotifyConnectionStatus(void)noexcept
{
	fNeedNotifyServiceState=true;
	RunMainProcess();
}
//---------------------------------------------------------------------------
void cGATTService::CharacteristicAttach(cGATTCharacteristic *Characteristic)noexcept
{
	fCharacteristicMap[Characteristic->CharacteristcUUID]=Characteristic;
}
//---------------------------------------------------------------------------
void cGATTService::CharacteristicDetach(cGATTCharacteristic *Characteristic)noexcept
{
	fCharacteristicMap.Remove(Characteristic->CharacteristcUUID);
}
//---------------------------------------------------------------------------
bool cGATTService::CharacteristicIsRefreshing(void)const noexcept
{
	return fMainProcessState==psRefreshCharacteristic;
}
//---------------------------------------------------------------------------
void cGATTService::cMainProcessProcedure::Execute(void)noexcept
{
	auto Host=cnMemory::GetObjectFromMemberPointer(this,&cGATTService::fMainProcessProcedure);
	return Host->MainProcessThread();
}
//---------------------------------------------------------------------------
void cGATTService::MainProcessThread(void)noexcept
{
	do{
		do{
			fMainProcessExclusiveFlag.Continue();

		}while(MainProcess());

	}while(!fMainProcessExclusiveFlag.Release());
}
//---------------------------------------------------------------------------
void cGATTService::RunMainProcess(void)noexcept
{
	if(!fMainProcessExclusiveFlag.Acquire())
		return;

	if(fDispatch->IsCurrentThread()){
		MainProcessThread();
	}
	else{
		fMainProcessWork->Start();
	}
}
//---------------------------------------------------------------------------
void cGATTService::NotifyMainProcess(void)noexcept
{
	if(!fMainProcessExclusiveFlag.Acquire())
		return;

	fMainProcessWork->Start();
}
//---------------------------------------------------------------------------
bool cGATTService::MainProcess(void)noexcept
{
	MainProcessStateChange();

	if(fService==nullptr)
		return false;

	switch(fMainProcessState){
	default:
		return false;
	case psIdle:
		MainProcess_Idle();
		return false;
	case psRefreshCharacteristicDone:
		MainProcess_RefreshCharacteristicDone();
		return true;
	}
}
//---------------------------------------------------------------------------
void cGATTService::MainProcessStateChange(void)noexcept
{
	if(fCloseState){
		if(fCloseState==1){
			for(auto Pair : fCharacteristicMap){
				Pair.Value->ServiceInvalidateCharacteristic();
			}
			fService=nullptr;
			fNeedNotifyServiceState=true;
			fCloseState=2;
		}
	}

	if(fNeedNotifyServiceState){
		fNeedNotifyServiceState=false;
		
		eGATTAvailability NewState;
		if(fService==nullptr){
			if(fScaningService){
				NewState=GATTAvailability::Scanning;
			}
			else{
				NewState=GATTAvailability::Absent;
			}
		}
		else{
			NewState=GATTAvailability::Available;
		}

		if(fAvailability!=NewState){
			fAvailability=NewState;

			for(auto Pair : fCharacteristicMap){
				Pair.Value->ServiceNotifyFunctionStatus();
			}

			for(auto Handler : fHandlers){
				Handler->GATTServiceAvailabilityChanged();
			}
		}
	}
}
//---------------------------------------------------------------------------
void cGATTService::MainProcess_Idle(void)noexcept
{
	HRESULT hr;

	if(fCharacteristicNeedRefresh){
		fCharacteristicNeedRefresh=false;

		for(auto Pair : fCharacteristicMap){
			Pair.Value->ServiceInvalidateCharacteristic();
		}

		COMPtr<IBLEGetCharacteristicAsyncOp> CharOp;
		hr=fService3->GetCharacteristicsAsync(COMRetPtrT(CharOp));
		if(FAILED(hr)){
			return;
		}
		fMainProcessState=psRefreshCharacteristic;

		hr=CharOp->put_Completed(&fGetCharacteristicsCompleteHandler);

		return;
	}

}
//---------------------------------------------------------------------------
class cCharRefer : public ABI::Windows::Foundation::ITypedEventHandler<ABI::Windows::Devices::Bluetooth::GenericAttributeProfile::GattCharacteristic*,ABI::Windows::Devices::Bluetooth::GenericAttributeProfile::GattValueChangedEventArgs*>
{
public:
	cAtomicVar<uIntn> fRefCount=1;
	virtual HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid,_COM_Outptr_ void __RPC_FAR *__RPC_FAR *ppvObject)noexcept override{
		if(riid==__uuidof(IUnknown)){
			++fRefCount.Free;
			*ppvObject=static_cast<IUnknown*>(this);
			return S_OK;
		}
		if(riid==__uuidof(ABI::Windows::Foundation::ITypedEventHandler<ABI::Windows::Devices::Bluetooth::GenericAttributeProfile::GattCharacteristic*,ABI::Windows::Devices::Bluetooth::GenericAttributeProfile::GattValueChangedEventArgs*>)){
			++fRefCount.Free;
			*ppvObject=static_cast<ABI::Windows::Foundation::ITypedEventHandler<ABI::Windows::Devices::Bluetooth::GenericAttributeProfile::GattCharacteristic*,ABI::Windows::Devices::Bluetooth::GenericAttributeProfile::GattValueChangedEventArgs*>*>(this);
			return S_OK;
		}
		*ppvObject=nullptr;
		return E_NOINTERFACE;
	}

	virtual ULONG STDMETHODCALLTYPE AddRef(void)noexcept override{
		auto v=++fRefCount.Free;
		return 0;
	}

	virtual ULONG STDMETHODCALLTYPE Release(void)noexcept override{
		auto v=--fRefCount.Free;
		if(v==0){
			delete this;
		}
		return 0;
	}

	virtual HRESULT STDMETHODCALLTYPE Invoke(ABI::Windows::Devices::Bluetooth::GenericAttributeProfile::IGattCharacteristic*,ABI::Windows::Devices::Bluetooth::GenericAttributeProfile::IGattValueChangedEventArgs*)noexcept override{
		return S_OK;
	}
};
//---------------------------------------------------------------------------
void cGATTService::MainProcess_RefreshCharacteristicDone(void)noexcept
{
	if(fGetCharacteristicsCompleteHandler.ResultCharacteristics!=nullptr){
		
		HRESULT hr;
		auto ResultCharacteristics=cnVar::MoveCast(fGetCharacteristicsCompleteHandler.ResultCharacteristics);
		ABI::Windows::Devices::Bluetooth::GenericAttributeProfile::GattCommunicationStatus Status;
		hr=ResultCharacteristics->get_Status(&Status);

		if(Status!=ABI::Windows::Devices::Bluetooth::GenericAttributeProfile::GattCommunicationStatus_Success)
			return;

		COMPtr< ABI::Windows::Foundation::Collections::IVectorView<ABI::Windows::Devices::Bluetooth::GenericAttributeProfile::GattCharacteristic*> > CharacteristicVector;
		hr=ResultCharacteristics->get_Characteristics(COMRetPtrT(CharacteristicVector));
		
		cSeqList<IBLECharacteristic*> PtrList;
		hr=CopyVectorView(PtrList.Storage(),CharacteristicVector);


		for(auto CharPair : fCharacteristicMap){
			fUpdateMap[CharPair.Key].Characteristic=CharPair.Value;
		}
	
		for(uIntn i=0,c=PtrList.GetCount();i<c;i++){
			auto BLEChar=COMTake(PtrList[i]);

			auto Refere=new cCharRefer;
			EventRegistrationToken Token;
			BLEChar->add_ValueChanged(Refere,&Token);
			Refere->Release();

			GUID IDValue;
			hr=BLEChar->get_Uuid(&IDValue);

			auto Pair=fUpdateMap.GetPair(reinterpret_cast<cUUID&>(IDValue));
			if(Pair!=nullptr){
				Pair->Value.BLECharacteristic=BLEChar;
			}

		}

		for(auto Pair : fUpdateMap){
			Pair.Value.Characteristic->ServiceUpdateCharacteristic(Pair.Value.BLECharacteristic);
		}

		fUpdateMap.Clear();

	}
	else{
		fMainProcessState=psIdle;
	}
}
//---------------------------------------------------------------------------
cGATTService* cGATTService::cGetCharacteristicsCompleteHandler::GetHost(void)noexcept
{
	return cnMemory::GetObjectFromMemberPointer(this,&cGATTService::fGetCharacteristicsCompleteHandler);
}
//---------------------------------------------------------------------------
void cGATTService::cGetCharacteristicsCompleteHandler::EventIncReference(void)noexcept
{
	GetHost()->InnerIncReference('getc');
}
void cGATTService::cGetCharacteristicsCompleteHandler::EventDecReference(void)noexcept
{
	GetHost()->InnerDecReference('getc');
}
void cGATTService::cGetCharacteristicsCompleteHandler::EventInvoke(IBLEGetCharacteristicAsyncOp *AsyncOp,ABI::Windows::Foundation::AsyncStatus)noexcept
{
	hrResult=AsyncOp->GetResults(COMRetPtrT(ResultCharacteristics));

	auto Host=GetHost();
	Host->fMainProcessState=psRefreshCharacteristicDone;
	Host->NotifyMainProcess();
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cBluetoothSlave::cBluetoothSlave(cBluetoothCentral *Central,iPtr<cBluetoothAddress> Address)noexcept
	: fCentral(Central)
	, fDispatch(Central->GetDispatch())
	, fAddress(cnVar::MoveCast(Address))
	, fMainProcessWork(Central->GetDispatch()->CreateWork(&fInnerReference,&fMainProcessProcedure))
	, fAvailability(GATTAvailability::Scanning)
{
}
//---------------------------------------------------------------------------
cBluetoothSlave::~cBluetoothSlave()noexcept
{
}
//---------------------------------------------------------------------------
void cBluetoothSlave::VirtualStarted(void)noexcept
{
	cDualReference::VirtualStarted();
}
//---------------------------------------------------------------------------
void cBluetoothSlave::VirtualStopped(void)noexcept
{
	if(fDevice!=nullptr){
		ClearDevice();
	}
	cDualReference::VirtualStopped();
}
//---------------------------------------------------------------------------
iDispatch* cBluetoothSlave::GetDispatch(void)const noexcept
{
	return fDispatch;
}
//---------------------------------------------------------------------------
iDispatch* cBluetoothSlave::GetHandlerDispatch(void)noexcept
{
	return fDispatch;
}
//---------------------------------------------------------------------------
bool cBluetoothSlave::InsertHandler(iBluetoothSlaveHandler *Handler)noexcept
{
	if(fDispatch->IsCurrentThread()==false)
		return false;
	fHandlers.Insert(Handler);
	return true;
}
//---------------------------------------------------------------------------
bool cBluetoothSlave::RemoveHandler(iBluetoothSlaveHandler *Handler)noexcept
{
	if(fDispatch->IsCurrentThread()==false)
		return false;
	fHandlers.Remove(Handler);
	return true;
}
//---------------------------------------------------------------------------
void cBluetoothSlave::Close(void)noexcept
{
	fCloseState=1;
	NotifyMainProcess();
}
//---------------------------------------------------------------------------
iAddress* cBluetoothSlave::GetPeripheralAddress(void)noexcept
{
	return fAddress;
}
//---------------------------------------------------------------------------
eGATTAvailability cBluetoothSlave::GetAvailability(void)noexcept
{
	return fAvailability;
}
//---------------------------------------------------------------------------
bool cBluetoothSlave::IsConnected(void)noexcept
{
	return false;
}
//---------------------------------------------------------------------------
iBluetoothCentral* cBluetoothSlave::GetCentral(void)noexcept
{
	return fCentral;
}
//---------------------------------------------------------------------------
rPtr<iReference> cBluetoothSlave::QueryName(cArray<const uChar16> &Name)noexcept
{
	if(fDispatch->IsCurrentThread()==false)
		return nullptr;
	if(fDevice==nullptr){
		return nullptr;
	}

	Name=fDeviceName.GetArray();
	return fDeviceName.Token();
}
//---------------------------------------------------------------------------
rPtr<iGATTService> cBluetoothSlave::AccessService(const cUUID &ID)noexcept
{
	if(fDispatch->IsCurrentThread()==false)
		return nullptr;

	auto ServicePair=fServiceMap.GetPair(ID);
	if(ServicePair!=nullptr){
		return ServicePair->Value;
	}
	auto Service=rCreate<cGATTService>(this,ID);
	Service->PeripheralNotifyScanService();
	return Service;
}
//---------------------------------------------------------------------------
iPtr<iGATTServiceObserver> cBluetoothSlave::CreateServiceObserver(void)noexcept
{
	return nullptr;
}
//---------------------------------------------------------------------------
bool cBluetoothSlave::GetConnect(void)noexcept
{
	return false;
}
//---------------------------------------------------------------------------
bool cBluetoothSlave::SetConnect(bool Value)noexcept
{
	return false;
}
//---------------------------------------------------------------------------
void cBluetoothSlave::DeviceOpened(COMPtr<IBLEDevice> Device)noexcept
{
	fConnectOpRunning=false;
	fDevice=cnVar::MoveCast(Device);
	fDevice->QueryInterface(COMRetPtrT(fDevice3));
	SetupDevice();

	RefreshService();

	fNeedNotifyConnectionStateChange=true;
	fNeedNotifyNameChange=true;

	NotifyMainProcess();
}
//---------------------------------------------------------------------------
void cBluetoothSlave::DeviceOpenFailed(HRESULT hr)noexcept
{
	fConnectOpRunning=false;
	fNeedNotifyConnectionStateChange=true;
	NotifyMainProcess();
}
//---------------------------------------------------------------------------
void cBluetoothSlave::SetupDevice(void)noexcept
{
	fDevice->add_NameChanged(&fBLEDeviceNameChangedHandler,&fBLEDeviceNameChangedToken);
	fDevice->add_ConnectionStatusChanged(&fBLEDeviceConnectionStatusHandler,&fBLEDeviceConnectionStatusToken);
	fDevice->add_GattServicesChanged(&fBLEDeviceServicesChangedHandler,&fBLEDeviceServicesChangedToken);
}
//---------------------------------------------------------------------------
void cBluetoothSlave::ClearDevice(void)noexcept
{
	fDevice->remove_NameChanged(fBLEDeviceNameChangedToken);
	fDevice->remove_ConnectionStatusChanged(fBLEDeviceConnectionStatusToken);
	fDevice->remove_GattServicesChanged(fBLEDeviceServicesChangedToken);
}
//---------------------------------------------------------------------------
cBluetoothSlave* cBluetoothSlave::cBLEDeviceNameChangedHandler::GetHost(void)noexcept
{
	return cnMemory::GetObjectFromMemberPointer(this,&cBluetoothSlave::fBLEDeviceNameChangedHandler);
}
//---------------------------------------------------------------------------
void cBluetoothSlave::cBLEDeviceNameChangedHandler::EventIncReference(void)noexcept
{
	GetHost()->InnerIncReference('evet');
}
void cBluetoothSlave::cBLEDeviceNameChangedHandler::EventDecReference(void)noexcept
{
	GetHost()->InnerDecReference('evet');
}
void cBluetoothSlave::cBLEDeviceNameChangedHandler::EventInvoke(IBLEDevice*, IInspectable*)noexcept
{
	auto Host=GetHost();
	Host->fNeedNotifyNameChange=true;
	Host->NotifyMainProcess();
}
//---------------------------------------------------------------------------
cBluetoothSlave* cBluetoothSlave::cBLEDeviceConnectionStatusHandler::GetHost(void)noexcept
{
	return cnMemory::GetObjectFromMemberPointer(this,&cBluetoothSlave::fBLEDeviceConnectionStatusHandler);
}
//---------------------------------------------------------------------------
void cBluetoothSlave::cBLEDeviceConnectionStatusHandler::EventIncReference(void)noexcept
{
	GetHost()->InnerIncReference('evet');
}
//---------------------------------------------------------------------------
void cBluetoothSlave::cBLEDeviceConnectionStatusHandler::EventDecReference(void)noexcept
{
	GetHost()->InnerDecReference('evet');
}
//---------------------------------------------------------------------------
void cBluetoothSlave::cBLEDeviceConnectionStatusHandler::EventInvoke(IBLEDevice*, IInspectable*)noexcept
{
	auto Host=GetHost();
	Host->fNeedNotifyConnectionStateChange=true;
	Host->NotifyMainProcess();
}
//---------------------------------------------------------------------------
cBluetoothSlave* cBluetoothSlave::cBLEDeviceServicesChangedHandler::GetHost(void)noexcept
{
	return cnMemory::GetObjectFromMemberPointer(this,&cBluetoothSlave::fBLEDeviceServicesChangedHandler);
}
//---------------------------------------------------------------------------
void cBluetoothSlave::cBLEDeviceServicesChangedHandler::EventIncReference(void)noexcept
{
	GetHost()->InnerIncReference('evet');
}
//---------------------------------------------------------------------------
void cBluetoothSlave::cBLEDeviceServicesChangedHandler::EventDecReference(void)noexcept
{
	GetHost()->InnerDecReference('evet');
}
//---------------------------------------------------------------------------
void cBluetoothSlave::cBLEDeviceServicesChangedHandler::EventInvoke(IBLEDevice*, IInspectable*)noexcept
{
	GetHost()->RefreshService();
}
//---------------------------------------------------------------------------
cBluetoothSlave* cBluetoothSlave::cAddressConnectCompleteHandler::GetHost(void)noexcept
{
	return cnMemory::GetObjectFromMemberPointer(this,&cBluetoothSlave::fAddressConnectCompleteHandler);
}
//---------------------------------------------------------------------------
void cBluetoothSlave::cAddressConnectCompleteHandler::EventIncReference(void)noexcept
{
	GetHost()->InnerIncReference('from');
}
void cBluetoothSlave::cAddressConnectCompleteHandler::EventDecReference(void)noexcept
{
	GetHost()->InnerDecReference('from');
}
void cBluetoothSlave::cAddressConnectCompleteHandler::EventInvoke(IBLEDeviceAsyncOp *AsyncOp,ABI::Windows::Foundation::AsyncStatus)noexcept
{
	HRESULT hr;
	COMPtr<IBLEDevice> ResultDevice;
	hr=AsyncOp->GetResults(COMRetPtrT(ResultDevice));
	if(SUCCEEDED(hr) && ResultDevice!=nullptr){
		GetHost()->DeviceOpened(cnVar::MoveCast(ResultDevice));
	}
	else{
		GetHost()->DeviceOpenFailed(hr);
	}
}
//---------------------------------------------------------------------------
void cBluetoothSlave::CentralConnectionOperation(COMPtr<IBLEConnectAsyncOp> ConnectOp)noexcept
{
	fConnectOpRunning=true;
	fNeedNotifyConnectionStateChange=true;
	NotifyMainProcess();
	ConnectOp->put_Completed(&fAddressConnectCompleteHandler);
}
//---------------------------------------------------------------------------
void cBluetoothSlave::ServiceAttach(cGATTService *Service)noexcept
{
	fServiceMap[Service->ServiceUUID]=Service;
}
//---------------------------------------------------------------------------
void cBluetoothSlave::ServiceDetach(cGATTService *Service)noexcept
{
	fServiceMap.Remove(Service->ServiceUUID);
}
//---------------------------------------------------------------------------
void cBluetoothSlave::ServiceScan(cGATTService::cOpenServiceProcedure *OpenProc)noexcept
{
	fOpenServiceStack.Push(OpenProc);

	RefreshService();
}
//---------------------------------------------------------------------------
void cBluetoothSlave::RefreshService(void)noexcept
{
	if(!fQueryServiceExclusiveFlag.Acquire())
		return;

	RefreshServiceProc();
}
//---------------------------------------------------------------------------
void cBluetoothSlave::RefreshServiceProc(void)noexcept
{
	do{
		fQueryServiceExclusiveFlag.Continue();

		if(fDevice!=nullptr){
			if(fServiceObserverCount!=0){
				COMPtr<IBLEGetServiceAsyncOp> ServiceOp;
				HRESULT hr=fDevice3->GetGattServicesWithCacheModeAsync(ABI::Windows::Devices::Bluetooth::BluetoothCacheMode_Cached,COMRetPtrT(ServiceOp));
				if(SUCCEEDED(hr)){
					hr=ServiceOp->put_Completed(&fGetServicesCompleteHandler);
					if(SUCCEEDED(hr)){
						// wait for completion handler calls OnServiceResult
						return;
					}
				}
			}
			else{
				auto OpenProcedures=fOpenServiceStack.Swap(nullptr);
				while(OpenProcedures!=nullptr){
					auto CurProc=OpenProcedures;
					OpenProcedures=OpenProcedures->Next;

					CurProc->GetService(fDevice3);
				}
			}
		}

	}while(!fQueryServiceExclusiveFlag.Release());
}
//---------------------------------------------------------------------------
void cBluetoothSlave::OnServiceResult(IBLEGetServiceAsyncOp *AsyncOp)noexcept
{
	if(OnServiceResultProcess(AsyncOp)){
		// wait process in main process
		fProcessGetServicesResult=true;
		NotifyMainProcess();
	}
	else{
		if(!fQueryServiceExclusiveFlag.Release()){
			RefreshServiceProc();
		}
	}
}
//---------------------------------------------------------------------------
bool cBluetoothSlave::OnServiceResultProcess(IBLEGetServiceAsyncOp *AsyncOp)noexcept
{
	HRESULT hr;
	COMPtr<IGattDeviceServiceResult> ResultServices;
	hr=AsyncOp->GetResults(COMRetPtrT(ResultServices));
	if(FAILED(hr))
		return false;

	ABI::Windows::Devices::Bluetooth::GenericAttributeProfile::GattCommunicationStatus Status;
	hr=ResultServices->get_Status(&Status);

	if(FAILED(hr))
		return false;

	COMPtr< ABI::Windows::Foundation::Collections::IVectorView<BLEService*> > ServiceVector;
	hr=ResultServices->get_Services(COMRetPtrT(ServiceVector));
	
	unsigned int ServiceCount;
	hr=ServiceVector->get_Size(&ServiceCount);
	if(FAILED(hr))
		return false;
	
	auto Result=fGetServicesResult<<=nullptr;
	if(Result==nullptr){
		Result=aClsCreate<cGetServicesResult>();
	}

	for(unsigned int i=0;i<ServiceCount;i++){
		COMPtr<IBLEService> Service;
		hr=ServiceVector->GetAt(i,COMRetPtrT(Service));

		union{
			GUID IDValue;
			cUUID UUID;
		};
		hr=Service->get_Uuid(&IDValue);

		bool Existed;
		auto ServicePair=Result->Map.InsertPair(UUID,Existed);
		if(!Existed){
			ServicePair->Value=cnVar::MoveCast(Service);
		}

	}

	auto OpenProcedures=fOpenServiceStack.Swap(nullptr);
	while(OpenProcedures!=nullptr){
		auto CurProc=OpenProcedures;
		OpenProcedures=OpenProcedures->Next;

		auto Pair=Result->Map.GetPair(CurProc->Service->ServiceUUID);
		if(Pair!=nullptr){
			CurProc->ServiceScanned(Pair->Value);
		}
		else{
			CurProc->ServiceScanned(nullptr);
		}

	}

	fGetServicesResult^=Result;
	return true;
}
//---------------------------------------------------------------------------
void cBluetoothSlave::cMainProcessProcedure::Execute(void)noexcept
{
	auto Host=cnMemory::GetObjectFromMemberPointer(this,&cBluetoothSlave::fMainProcessProcedure);
	return Host->MainProcessThread();
}
//---------------------------------------------------------------------------
void cBluetoothSlave::MainProcessThread(void)noexcept
{
	do{
		fMainProcessExclusiveFlag.Continue();
		
		MainProcess();

	}while(!fMainProcessExclusiveFlag.Release());
}
//---------------------------------------------------------------------------
void cBluetoothSlave::NotifyMainProcess(void)noexcept
{
	if(!fMainProcessExclusiveFlag.Acquire())
		return;

	fMainProcessWork->Start();
}
//---------------------------------------------------------------------------
void cBluetoothSlave::MainProcess(void)noexcept
{
	HRESULT hr;

	if(fCloseState){
		switch(fCloseState){
		case 1:
			for(auto Pair : fServiceMap){
				Pair.Value->PeripheralNotifyClose();
			}

			ClearDevice();
			{
				COMPtr<ABI::Windows::Foundation::IClosable> DeviceClosable;
				hr=fDevice->QueryInterface(COMRetPtrT(DeviceClosable));

				hr=DeviceClosable->Close();
			}

			fDevice=nullptr;
			fDevice3=nullptr;

			fNeedNotifyConnectionStateChange=true;

			for(auto Handler : fHandlers){
				Handler->PeripheralClosed();
			}				
			fCloseState=2;
			[[fallthrough]];
		case 2:
			return;
		}
	}

	if(fNeedNotifyConnectionStateChange){
		fNeedNotifyConnectionStateChange=false;

		eGATTAvailability NewFuncState;
		if(fDevice==nullptr){
			if(fConnectOpRunning){
				NewFuncState=GATTAvailability::Scanning;
			}
			else{
				NewFuncState=GATTAvailability::Absent;
			}
		}
		else{
			ABI::Windows::Devices::Bluetooth::BluetoothConnectionStatus NewStatus;
			hr=fDevice->get_ConnectionStatus(&NewStatus);
			if(FAILED(hr)){
				// error
				NewFuncState=GATTAvailability::Absent;
			}
			else{
				NewFuncState=GATTAvailability::Available;
			}
		}


		if(fAvailability!=NewFuncState){
			fAvailability=NewFuncState;

			for(auto Pair : fServiceMap){
				Pair.Value->PeripheralNotifyConnectionStatus();
			}

			for(auto Handler : fHandlers){
				Handler->PeripheralStateChanged();
			}
		}
	}
	if(fNeedNotifyNameChange){
		fNeedNotifyNameChange=false;

		if(fDevice==nullptr){
			fDeviceName.Clear();
		}
		else{
			cHString DeviceName;
			hr=fDevice->get_Name(&DeviceName);
			if(SUCCEEDED(hr)){
				auto NameArray=DeviceName.GetArray();
				fDeviceName.SetString(wtou(NameArray.Pointer),NameArray.Length);
			}
			for(auto Handler : fHandlers){
				Handler->PeripheralNameChanged();
			}
		}
	}

	if(fProcessGetServicesResult){
		fProcessGetServicesResult=false;

		auto Result=fGetServicesResult<<=nullptr;
		if(Result!=nullptr){

			fGetServicesResult^=Result;
		}

	}

}
//---------------------------------------------------------------------------
cBluetoothSlave* cBluetoothSlave::cGetServicesCompleteHandler::GetHost(void)noexcept
{
	return cnMemory::GetObjectFromMemberPointer(this,&cBluetoothSlave::fGetServicesCompleteHandler);
}
//---------------------------------------------------------------------------
void cBluetoothSlave::cGetServicesCompleteHandler::EventIncReference(void)noexcept
{
	GetHost()->InnerIncReference('gets');
}
void cBluetoothSlave::cGetServicesCompleteHandler::EventDecReference(void)noexcept
{
	GetHost()->InnerDecReference('gets');
}
void cBluetoothSlave::cGetServicesCompleteHandler::EventInvoke(IBLEGetServiceAsyncOp *AsyncOp,ABI::Windows::Foundation::AsyncStatus)noexcept
{
	GetHost()->OnServiceResult(AsyncOp);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cBluetoothLEAdvertisementObserver::cBluetoothLEAdvertisementObserver(cBluetoothCentral *Central,COMPtr<IBLEWatcher> Watcher)noexcept
	: fCentral(Central)
	, fWatcher(Watcher)
	, fCallback(nullptr)
{
	CreateQueueWork(fCentral->GetDispatch(),true);
}
//---------------------------------------------------------------------------
cBluetoothLEAdvertisementObserver::~cBluetoothLEAdvertisementObserver()noexcept
{
}
//---------------------------------------------------------------------------
void cBluetoothLEAdvertisementObserver::VirtualStarted(void)noexcept
{
	cDualReference::VirtualStarted();

	fWatcher->add_Received(&fBLEReceivedHandler,&fBLEReceivedToken);
}
//---------------------------------------------------------------------------
void cBluetoothLEAdvertisementObserver::VirtualStopped(void)noexcept
{
	fWatcher->remove_Received(fBLEReceivedToken);

	cDualReference::VirtualStopped();
}
//---------------------------------------------------------------------------
bool cBluetoothLEAdvertisementObserver::StartNotify(iReference *Reference,iAsyncNotificationCallback *Callback)noexcept
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
void cBluetoothLEAdvertisementObserver::StopNotify(void)noexcept
{
	if(bcAsyncQueue::StopNotify()){
		UpdateQueueState(false);
	}
}
//---------------------------------------------------------------------------
void cBluetoothLEAdvertisementObserver::NotifyCallback(bool IdleNotify)noexcept
{
	return bcAsyncQueue::NotifyQueue(IdleNotify);
}
//---------------------------------------------------------------------------
bool cBluetoothLEAdvertisementObserver::IsClosed(void)noexcept
{
	return bcAsyncQueue::IsNotificationClosed();
}
//---------------------------------------------------------------------------
iDispatch* cBluetoothLEAdvertisementObserver::GetHandlerDispatch(void)noexcept
{
	return fCentral->GetDispatch();
}
//---------------------------------------------------------------------------
iBluetoothCentral* cBluetoothLEAdvertisementObserver::GetCentral(void)noexcept
{
	return fCentral;
}
//---------------------------------------------------------------------------
void cBluetoothLEAdvertisementObserver::DiscardQueue(void)noexcept
{
	cAdvertisementData *DataQueue=fAdvertisementQueue.DequeueAll();
	while(DataQueue!=nullptr){
		auto pData=DataQueue;
		DataQueue=DataQueue->Next;

		rDecReference(pData,'queu');
	}
}
//---------------------------------------------------------------------------
rPtr<iReference> cBluetoothLEAdvertisementObserver::Fetch(cBluetoothLEAdvertisementInfo &Info)noexcept
{
	auto pData=fAdvertisementQueue.Dequeue();
	if(pData==nullptr)
		return nullptr;

	eBluetoothLEAdvertisementType Type;
	eBluetoothLEAdvertisementFlags Flags;
	sfInt16 SignalStrength;
	cArray<const cUUID> ServiceUUIDs;
	cArray<const cBluetoothLEAdvertisementManufacturerData> ManufacturerData;
	cArray<const cBluetoothLEAdvertisementSectionData> SectionData;

	Info.Address=pData->Address;
	Info.LocalName=pData->LocalName.GetArray();
	Info.Timestamp=pData->Timestamp;
	Info.PeripheralAddress=pData->Address->Address;
	Info.Type=pData->Type;
	Info.Flags=pData->Flags;
	Info.SignalStrength=pData->SignalStrength/256;
	Info.IsConnectable=pData->IsConnectable;
	Info.IsDiscoverable=pData->IsScannable;
	Info.ServiceUUIDs=pData->ServiceUUIDs.Storage();
	Info.ManufacturerData=pData->ManufacturerDataList.Storage();
	Info.SectionData=pData->SectionDataList.Storage();
	return rTake(pData,'queu');
}
//---------------------------------------------------------------------------
iReference* cBluetoothLEAdvertisementObserver::NotificationInnerReference(void)noexcept
{
	return &fInnerReference;
}
//---------------------------------------------------------------------------
void cBluetoothLEAdvertisementObserver::NotificationStarted(void)noexcept
{
	fCallback->AsyncStarted();
}
//---------------------------------------------------------------------------
void cBluetoothLEAdvertisementObserver::NotificationStopped(void)noexcept
{
	fCallback->AsyncStopped();

	fCallback=nullptr;
	fCallbackReference=nullptr;
}
//---------------------------------------------------------------------------
void cBluetoothLEAdvertisementObserver::AsyncQueueNotify(void)noexcept
{
	fCallback->AsyncNotify();
}
//---------------------------------------------------------------------------
void cBluetoothLEAdvertisementObserver::BLEReceived(rPtr<cAdvertisementData> AdvData)noexcept
{
	auto pData=rExtract(AdvData,'queu');
	fAdvertisementQueue.Enqueue(pData);

	AsyncQueueSetAvailable(true);
}
//---------------------------------------------------------------------------
cBluetoothLEAdvertisementObserver* cBluetoothLEAdvertisementObserver::cBLEReceivedHandler::GetHost(void)noexcept
{
	return cnMemory::GetObjectFromMemberPointer(this,&cBluetoothLEAdvertisementObserver::fBLEReceivedHandler);
}
//---------------------------------------------------------------------------
HRESULT STDMETHODCALLTYPE cBluetoothLEAdvertisementObserver::cBLEReceivedHandler::QueryInterface(REFIID riid,_COM_Outptr_ void __RPC_FAR *__RPC_FAR *ppvObject)noexcept
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
ULONG STDMETHODCALLTYPE cBluetoothLEAdvertisementObserver::cBLEReceivedHandler::AddRef(void)noexcept
{
	GetHost()->InnerIncReference('evet');
	return 0;
}
//---------------------------------------------------------------------------
ULONG STDMETHODCALLTYPE cBluetoothLEAdvertisementObserver::cBLEReceivedHandler::Release(void)noexcept
{
	GetHost()->InnerDecReference('evet');
	return 0;
}
//---------------------------------------------------------------------------
HRESULT STDMETHODCALLTYPE cBluetoothLEAdvertisementObserver::cBLEReceivedHandler::Invoke(IBLEWatcher*, _In_ ABI::Windows::Devices::Bluetooth::Advertisement::IBluetoothLEAdvertisementReceivedEventArgs *args)noexcept
{

	HRESULT hr;
	UINT64 BLEAddress;
	hr=args->get_BluetoothAddress(&BLEAddress);
	if(FAILED(hr)){
		return S_OK;
	}

	COMPtr<ABI::Windows::Devices::Bluetooth::Advertisement::IBluetoothLEAdvertisementReceivedEventArgs2> args2;
	hr=args->QueryInterface(COMRetPtrT(args2));

	auto AdvData=GetHost()->fAdvDataRecycler.Query();
	AdvData->ServiceUUIDs.Clear();
	AdvData->ManufacturerDataList.Clear();
	AdvData->SectionDataList.Clear();
	AdvData->LocalName.Clear();
	AdvData->ManufacturerDataBuffer.Clear();
	AdvData->SectionDataBuffer.Clear();


	hr=args2->get_IsScannable(&AdvData->IsScannable);
	hr=args2->get_IsConnectable(&AdvData->IsConnectable);

	//boolean IsScanResponse;
	//args2->get_IsScanResponse(&IsScanResponse);


	auto DeviceAddress=iCreate<cBluetoothAddress>();
	DeviceAddress->Address=BLEAddress;
	AdvData->Address=DeviceAddress;
	
	hr=args->get_RawSignalStrengthInDBm(&AdvData->SignalStrength);
	if(FAILED(hr)){
		AdvData->SignalStrength=32768;
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
	ABI::Windows::Devices::Bluetooth::Advertisement::BluetoothLEAdvertisementType AdvType;
	hr=args->get_AdvertisementType(&AdvType);
	if(FAILED(hr)){
		AdvData->Type=BluetoothLEAdvertisementType::ConnectableUndirected;
	}
	else{
		switch(AdvType){
		case ABI::Windows::Devices::Bluetooth::Advertisement::BluetoothLEAdvertisementType::BluetoothLEAdvertisementType_ConnectableUndirected:
			AdvData->Type=BluetoothLEAdvertisementType::ConnectableUndirected;
			break;
		case ABI::Windows::Devices::Bluetooth::Advertisement::BluetoothLEAdvertisementType::BluetoothLEAdvertisementType_ConnectableDirected:
			AdvData->Type=BluetoothLEAdvertisementType::ConnectableDirected;
			break;
		case ABI::Windows::Devices::Bluetooth::Advertisement::BluetoothLEAdvertisementType::BluetoothLEAdvertisementType_ScannableUndirected:
			AdvData->Type=BluetoothLEAdvertisementType::ScannableUndirected;
			break;
		case ABI::Windows::Devices::Bluetooth::Advertisement::BluetoothLEAdvertisementType::BluetoothLEAdvertisementType_NonConnectableUndirected:
			AdvData->Type=BluetoothLEAdvertisementType::NonConnectableUndirected;
			break;
		case ABI::Windows::Devices::Bluetooth::Advertisement::BluetoothLEAdvertisementType::BluetoothLEAdvertisementType_ScanResponse:
			AdvData->Type=BluetoothLEAdvertisementType::ScanResponse;
			break;
		//case ABI::Windows::Devices::Bluetooth::Advertisement::BluetoothLEAdvertisementType::BluetoothLEAdvertisementType_Extended:
		//	AdvData->Info.Type=BluetoothLEAdvertisementType::Extended;
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
				AdvData->Flags=BluetoothLEAdvertisementFlags::None;
			}
			else{
				AdvData->Flags=0;
				if(FlagValue&ABI::Windows::Devices::Bluetooth::Advertisement::BluetoothLEAdvertisementFlags::BluetoothLEAdvertisementFlags_LimitedDiscoverableMode){
					AdvData->Flags|=BluetoothLEAdvertisementFlags::LimitedDiscoverableMode;
				}
				if(FlagValue&ABI::Windows::Devices::Bluetooth::Advertisement::BluetoothLEAdvertisementFlags::BluetoothLEAdvertisementFlags_GeneralDiscoverableMode){
					AdvData->Flags|=BluetoothLEAdvertisementFlags::GeneralDiscoverableMode;
				}
				if(FlagValue&ABI::Windows::Devices::Bluetooth::Advertisement::BluetoothLEAdvertisementFlags::BluetoothLEAdvertisementFlags_ClassicNotSupported){
					AdvData->Flags|=BluetoothLEAdvertisementFlags::ClassicNotSupported;
				}
				if(FlagValue&ABI::Windows::Devices::Bluetooth::Advertisement::BluetoothLEAdvertisementFlags::BluetoothLEAdvertisementFlags_DualModeControllerCapable){
					AdvData->Flags|=BluetoothLEAdvertisementFlags::DualModeControllerCapable;
				}
				if(FlagValue&ABI::Windows::Devices::Bluetooth::Advertisement::BluetoothLEAdvertisementFlags::BluetoothLEAdvertisementFlags_DualModeHostCapable){
					AdvData->Flags|=BluetoothLEAdvertisementFlags::DualModeHostCapable;
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

					auto NewItem=AdvData->ManufacturerDataList.Append();
					NewItem->CompanyID=CompanyID;
					NewItem->Pointer=reinterpret_cast<void*>(AdvData->ManufacturerDataBuffer.GetSize());
					NewItem->Length=DataArray.Length;
					AdvData->ManufacturerDataBuffer.Append(DataArray.Pointer,DataArray.Length);
				}
			}

			auto ManufacturerDataBuffer=AdvData->ManufacturerDataBuffer->Pointer;
			for(unsigned int i=0;i<ManufacturerDataCount;i++){
				auto &Item=AdvData->ManufacturerDataList[i];
				Item.Pointer=cnMemory::PointerAddByteOffset(ManufacturerDataBuffer,reinterpret_cast<uIntn>(Item.Pointer));
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
					auto NewItem=AdvData->SectionDataList.Append();
					NewItem->SectionType=DataType;
					NewItem->Pointer=reinterpret_cast<void*>(AdvData->SectionDataBuffer.GetSize());
					NewItem->Length=DataArray.Length;
					AdvData->SectionDataBuffer.Append(DataArray.Pointer,DataArray.Length);
				}
			}

			auto SectionDataBuffer=AdvData->SectionDataBuffer->Pointer;
			for(unsigned int i=0;i<DataSectionCount;i++){
				auto &Item=AdvData->SectionDataList[i];
				Item.Pointer=cnMemory::PointerAddByteOffset(SectionDataBuffer,reinterpret_cast<uIntn>(Item.Pointer));
			}
		}
	}


	GetHost()->BLEReceived(cnVar::MoveCast(AdvData));
	return S_OK;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cBluetoothCentral::cBluetoothCentral(iDispatch *Dispatch,COMPtr<IBLEDeviceStatics> BLEStatic)noexcept
	: fBLEStatic(cnVar::MoveCast(BLEStatic))
	, fDispatch(Dispatch)
{
}
//---------------------------------------------------------------------------
cBluetoothCentral::IBLEDeviceStatics* cBluetoothCentral::GetBLEStatic(void)const noexcept
{
	return fBLEStatic;
}
//---------------------------------------------------------------------------
iDispatch* cBluetoothCentral::GetHandlerDispatch(void)noexcept
{
	return fDispatch;
}
//---------------------------------------------------------------------------
iDispatch* cBluetoothCentral::GetDispatch(void)const noexcept
{
	return fDispatch;
}
//---------------------------------------------------------------------------
bool cBluetoothCentral::InsertHandler(iBluetoothCentralHandler *Handler)noexcept
{
	return false;
}
//---------------------------------------------------------------------------
bool cBluetoothCentral::RemoveHandler(iBluetoothCentralHandler *Handler)noexcept
{
	return false;
}
//---------------------------------------------------------------------------
bool cBluetoothCentral::IsEnabled(void)noexcept
{
	return true;
}
//---------------------------------------------------------------------------
bool cBluetoothCentral::IsPowerOn(void)noexcept
{
	return true;
}
//---------------------------------------------------------------------------
rPtr<iBluetoothLEAdvertisementObserver> cBluetoothCentral::CreateLEAdvertisementObserver(void)noexcept
{
	cHString RCName_BluetoothLEAdvertisementWatcher;
	RCName_BluetoothLEAdvertisementWatcher=RuntimeClass_Windows_Devices_Bluetooth_Advertisement_BluetoothLEAdvertisementWatcher;

	HRESULT hr;
	COMPtr<IBLEWatcher> BLEWatcher;
	hr=::Windows::Foundation::ActivateInstance(RCName_BluetoothLEAdvertisementWatcher,COMRetPtrT(BLEWatcher));
	if(FAILED(hr))
		return nullptr;

	//BLEWatcher->get_SignalStrengthFilter;
	hr=BLEWatcher->put_ScanningMode(ABI::Windows::Devices::Bluetooth::Advertisement::BluetoothLEScanningMode_Active);
	//BLEWatcher->put_AdvertisementFilter

	return rCreate<cBluetoothLEAdvertisementObserver>(this,cnVar::MoveCast(BLEWatcher));
}
//---------------------------------------------------------------------------
rPtr<iBluetoothSlave> cBluetoothCentral::OpenPeripheral(iAddress *Address)noexcept
{
	if(fDispatch->IsCurrentThread()==false)
		return nullptr;

	auto BLEAddress=iCast<cBluetoothAddress>(Address);
	if(BLEAddress!=nullptr){
		HRESULT hr;
		COMPtr<IBLEConnectAsyncOp> ConnectOp;
		hr=fBLEStatic->FromBluetoothAddressAsync(BLEAddress->Address,COMRetPtrT(ConnectOp));
		if(FAILED(hr)){
			return nullptr;
		}

		auto Slave=rCreate<cBluetoothSlave>(this,BLEAddress);

		Slave->CentralConnectionOperation(cnVar::MoveCast(ConnectOp));
		return Slave;
	}
	return nullptr;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cGATTServerDescriptor::cGATTServerDescriptor(cGATTServerCharacteristic *Owner,const cUUID &ID,iReference *Reference,iGATTDescriptorController *Controller)noexcept
	: fOwner(Owner)
	, fDispatch(Owner->GetDispatch())
	, fDescriptorID(ID)
	, fControllerReference(Reference)
	, fController(Controller)
	, fMainProcessWork(fDispatch->CreateWork(&fInnerReference,&fMainProcessProcedure))
{
}
//---------------------------------------------------------------------------
void cGATTServerDescriptor::VirtualStarted(void)noexcept
{
	cDualReference::VirtualStarted();
}
//---------------------------------------------------------------------------
void cGATTServerDescriptor::VirtualStopped(void)noexcept
{
	ShutdownDescriptor();

	cDualReference::VirtualStopped();
}
//---------------------------------------------------------------------------
const cUUID &cGATTServerDescriptor::GetID(void)const noexcept
{
	return fDescriptorID;
}
//---------------------------------------------------------------------------
iDispatch* cGATTServerDescriptor::GetDispatch(void)const noexcept
{
	return fDispatch;
}
//---------------------------------------------------------------------------
iGATTServerCharacteristic* cGATTServerDescriptor::GetOwner(void)noexcept
{
	return fOwner;
}
//---------------------------------------------------------------------------
iDispatch* cGATTServerDescriptor::GetHandlerDispatch(void)noexcept
{
	return fDispatch;
}
//---------------------------------------------------------------------------
void cGATTServerDescriptor::Shutdown(void)noexcept
{
	return ShutdownDescriptor();
}
//---------------------------------------------------------------------------
void cGATTServerDescriptor::ShutdownDescriptor(void)noexcept
{
	if(fShutdown==false){
		fShutdown=true;
		NotifyMainProcess();
	}
}
//---------------------------------------------------------------------------
void cGATTServerDescriptor::SetupDescriptor(void)noexcept
{
	//fDescriptor->add_ReadRequested();
	//fDescriptor->add_WriteRequested();
}
//---------------------------------------------------------------------------
void cGATTServerDescriptor::ClearDescriptor(void)noexcept
{
	//fDescriptor->remove_ReadRequested;
	//fDescriptor->remove_WriteRequested;
}
//---------------------------------------------------------------------------
void cGATTServerDescriptor::RunMainProcess(void)noexcept
{
	if(!fMainProcessExclusiveFlag.Acquire())
		return;

	if(fDispatch->IsCurrentThread()){
		MainProcessThread();
	}
	else{
		fMainProcessWork->Start();
	}
}
//---------------------------------------------------------------------------
void cGATTServerDescriptor::NotifyMainProcess(void)noexcept
{
	if(!fMainProcessExclusiveFlag.Acquire())
		return;

	fMainProcessWork->Start();
}
//---------------------------------------------------------------------------
void cGATTServerDescriptor::cMainProcessProcedure::Execute(void)noexcept
{
	auto Host=cnMemory::GetObjectFromMemberPointer(this,&cGATTServerDescriptor::fMainProcessProcedure);
	return Host->MainProcessThread();
}
//---------------------------------------------------------------------------
void cGATTServerDescriptor::MainProcessThread(void)noexcept
{
	do{
		do{
			fMainProcessExclusiveFlag.Continue();

		}while(MainProcess());

	}while(!fMainProcessExclusiveFlag.Release());
}
//---------------------------------------------------------------------------
bool cGATTServerDescriptor::MainProcess(void)noexcept
{
	HRESULT hr;
	if(fNeedNotifyActiveStateChange){
		fNeedNotifyActiveStateChange=false;

		bool NewActiveState;
		if(fDescriptor==nullptr){
			NewActiveState=false;
		}
		else{
			NewActiveState=fCharacteristicActive;
		}

		//bool NotifyStateChange=fFuncState!=NewFuncState;
		//fFuncState=NewFuncState;

	}

	switch(fUWPGattState){
	case gsNone:
		if(fShutdown){
			fOwner->DescriptorNotifyShutdown(this);

			fController->DescriptorClosed();
			fController=nullptr;
			fControllerReference=nullptr;

			fUWPGattState=gsShutdown;
			return false;
		}
		if(fCharacteristicActive==false){
			// owner characteristic not active
			break;
		}
		// create Interface
		return MainProcess_CreateDescriptor();
	case gsCreating:
		break;
	case gsCreateResult:
		if(FAILED(fCreateLocalDescCompleteHandler.hrResult)){
			fUWPGattState=gsNone;
			return false;
		}

		HRESULT hr;
		hr=fCreateLocalDescCompleteHandler.Result->get_Descriptor(COMRetPtrT(fDescriptor));
		if(FAILED(hr)){
			fUWPGattState=gsNone;
			return false;
		}

		SetupDescriptor();

		fUWPGattState=gsCreated;
		fNeedNotifyActiveStateChange=true;
		return true;
	case gsCreated:
		if(fShutdown){
			fOwner->DescriptorNotifyShutdown(this);
			fUWPGattState=gsShutdown;
		}
		break;
	case gsShutdown:
		break;
	}

	return false;
}
//---------------------------------------------------------------------------
bool cGATTServerDescriptor::MainProcess_CreateDescriptor(void)noexcept
{
	auto OwnerCharacteristic=fOwner->GetLocalCharacteristic();
	if(OwnerCharacteristic==nullptr){
		return false;
	}
	HRESULT hr;
	
	// look for existing descriptor
	COMPtr< ABI::Windows::Foundation::Collections::IVectorView<GattLocalDescriptor*> > Descriptors;
	hr=OwnerCharacteristic->get_Descriptors(COMRetPtrT(Descriptors));
	if(FAILED(hr))
		return false;

	unsigned int DescriptorCount;
	hr=Descriptors->get_Size(&DescriptorCount);

	for(unsigned int i=0;i<DescriptorCount;i++){
		COMPtr<IGattLocalDescriptor> Desc;
		hr=Descriptors->GetAt(i,COMRetPtrT(Desc));

		GUID DescID;
		hr=Desc->get_Uuid(&DescID);
		if(DescID==reinterpret_cast<GUID&>(fDescriptorID)){
			fDescriptor=cnVar::MoveCast(Desc);
		
			SetupDescriptor();

			fNeedNotifyActiveStateChange=true;
			return true;
		}
	}

	// create new descriptor

	COMPtr< ABI::Windows::Foundation::IAsyncOperation<GattLocalDescriptorResult*> > AsyncOp;

	COMPtr<IGattLocalDescriptorParameters> Parameters;
	cHString RCName_Parameters=RuntimeClass_Windows_Devices_Bluetooth_GenericAttributeProfile_GattLocalDescriptorParameters;
	hr=Windows::Foundation::ActivateInstance(RCName_Parameters,COMRetPtrT(Parameters));
	if(FAILED(hr)){
		return false;
	}

	//Parameters->put_UserDescription(ABI::Windows::Devices::Bluetooth::GenericAttributeProfile::GattProtectionLevel_Plain);
	Parameters->put_ReadProtectionLevel(ABI::Windows::Devices::Bluetooth::GenericAttributeProfile::GattProtectionLevel_Plain);
	Parameters->put_WriteProtectionLevel(ABI::Windows::Devices::Bluetooth::GenericAttributeProfile::GattProtectionLevel_Plain);

	hr=OwnerCharacteristic->CreateDescriptorAsync(reinterpret_cast<const GUID&>(fDescriptorID),Parameters,COMRetPtrT(AsyncOp));
	if(FAILED(hr)){
		return false;
	}

	fUWPGattState=gsCreating;
	AsyncOp->put_Completed(&fCreateLocalDescCompleteHandler);
	return false;
}
//---------------------------------------------------------------------------
void cGATTServerDescriptor::CharacteristicNotifyState(bool Active)noexcept
{
	fCharacteristicActive=Active;
	fNeedNotifyActiveStateChange=true;
	RunMainProcess();
}
//---------------------------------------------------------------------------
void cGATTServerDescriptor::CharacteristicNotifyShutdown(void)noexcept
{
	if(fShutdown==false){
		fShutdown=true;
		RunMainProcess();
	}
}
//---------------------------------------------------------------------------
cGATTServerDescriptor* cGATTServerDescriptor::cCreateLocalDescCompleteHandler::GetHost(void)noexcept
{
	return cnMemory::GetObjectFromMemberPointer(this,&cGATTServerDescriptor::fCreateLocalDescCompleteHandler);
}
//---------------------------------------------------------------------------
void cGATTServerDescriptor::cCreateLocalDescCompleteHandler::EventIncReference(void)noexcept
{
	GetHost()->InnerIncReference('crat');
}
//---------------------------------------------------------------------------
void cGATTServerDescriptor::cCreateLocalDescCompleteHandler::EventDecReference(void)noexcept
{
	GetHost()->InnerDecReference('crat');
}
//---------------------------------------------------------------------------
void cGATTServerDescriptor::cCreateLocalDescCompleteHandler::EventInvoke(ICreateLocalDescAsyncOp *AsyncOp,ABI::Windows::Foundation::AsyncStatus Status)noexcept
{
	auto Host=GetHost();
	Host->fUWPGattState=gsCreateResult;
	if(Status!=AsyncStatus::Completed){
		hrResult=E_FAIL;
	}
	else{
		hrResult=AsyncOp->GetResults(COMRetPtrT(Result));
	}
	Host->NotifyMainProcess();
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cGATTServerCharacteristic::cSubscription* cGATTServerCharacteristic::cSubscription::cClientNotifyValueCompleteHandler::GetHost(void)noexcept
{
	return cnMemory::GetObjectFromMemberPointer(this,&cSubscription::ClientNotifyValueCompleteHandler);
}
//---------------------------------------------------------------------------
void cGATTServerCharacteristic::cSubscription::cClientNotifyValueCompleteHandler::EventIncReference(void)noexcept
{
	rIncReference(GetHost(),'snot');
}
//---------------------------------------------------------------------------
void cGATTServerCharacteristic::cSubscription::cClientNotifyValueCompleteHandler::EventDecReference(void)noexcept
{
	rDecReference(GetHost(),'snot');
}
//---------------------------------------------------------------------------
void cGATTServerCharacteristic::cSubscription::cClientNotifyValueCompleteHandler::EventInvoke(IClientNotifyValueAsyncOp *AsyncOp,ABI::Windows::Foundation::AsyncStatus)noexcept
{
	COMPtr<IGattClientNotificationResult> Result;
	HRESULT hr;
	auto Host=GetHost();
	auto HostOwner=Host->Owner;
	
	hr=AsyncOp->GetResults(COMRetPtrT(Result));
	if(FAILED(hr)){
		// notify error
		auto Host=GetHost();
		Host->UpdateValueState=vnIdle;
		HostOwner->NotifyMainProcess();
		return;
	}

	GattCommunicationStatus Status;
	hr=Result->get_Status(&Status);
	if(FAILED(hr)){
		// notify error
		auto Host=GetHost();
		Host->UpdateValueState=vnIdle;
		HostOwner->NotifyMainProcess();
		return;
	}

	Host->UpdateValueState=vnCommand;
	HostOwner->fValueNotifyUpdate=true;
	HostOwner->NotifyMainProcess();
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cGATTServerCharacteristic::cGATTServerCharacteristic(cGATTServerService *Owner,const cUUID &ID,iReference *Reference,iGATTCharacteristicController *Controller)noexcept
	: fOwner(Owner)
	, fDispatch(Owner->GetDispatch())
	, fCharacteristicID(ID)
	, fControllerReference(Reference)
	, fController(Controller)
	, fMainProcessWork(fDispatch->CreateWork(&fInnerReference,&fMainProcessProcedure))
	, fRequestProcessWork(fDispatch->CreateWork(&fInnerReference,&fRequestProcessProcedure))	
{
}
//---------------------------------------------------------------------------
ABI::Windows::Devices::Bluetooth::GenericAttributeProfile::IGattLocalCharacteristic* cGATTServerCharacteristic::GetLocalCharacteristic(void)const noexcept
{
	return fCharacteristic;
}
//---------------------------------------------------------------------------
void cGATTServerCharacteristic::VirtualStarted(void)noexcept
{
	cDualReference::VirtualStarted();
}
//---------------------------------------------------------------------------
void cGATTServerCharacteristic::VirtualStopped(void)noexcept
{
	ShutdownCharacteristic();

	cDualReference::VirtualStopped();
}
//---------------------------------------------------------------------------
const cUUID &cGATTServerCharacteristic::GetID(void)const noexcept
{
	return fCharacteristicID;
}
//---------------------------------------------------------------------------
iDispatch* cGATTServerCharacteristic::GetDispatch(void)const noexcept
{
	return fDispatch;
}
//---------------------------------------------------------------------------
void cGATTServerCharacteristic::ShutdownCharacteristic(void)noexcept
{
	if(fShutdown==false){
		fShutdown=true;
		NotifyMainProcess();
	}
}
//---------------------------------------------------------------------------
iGATTServerService* cGATTServerCharacteristic::GetOwner(void)noexcept
{
	return fOwner;
}
//---------------------------------------------------------------------------
iDispatch* cGATTServerCharacteristic::GetHandlerDispatch(void)noexcept
{
	return fDispatch;
}
//---------------------------------------------------------------------------
rPtr<iGATTServerDescriptor> cGATTServerCharacteristic::CreateGATTDescriptor(const cUUID &ID,iReference *Reference,iGATTDescriptorController *Controller)noexcept
{
	if(fDispatch->IsCurrentThread()==false)
		return nullptr;
	
	bool Exists;
	auto DescPair=fDescriptorMap.InsertPair(ID,Exists);
	if(Exists){
		return nullptr;
	}
	
	auto NewDesc=rCreate<cGATTServerDescriptor>(this,ID,Reference,Controller);
	DescPair->Value=NewDesc;
	
	NewDesc->CharacteristicNotifyState(fCharacteristicActive);
	return NewDesc;
}
//---------------------------------------------------------------------------
void cGATTServerCharacteristic::Shutdown(void)noexcept
{
	ShutdownCharacteristic();
}
//---------------------------------------------------------------------------
iGATTServerSubscription* cGATTServerCharacteristic::QuerySubscription(iGATTServerConnection *Connection)noexcept
{
	if(fDispatch->IsCurrentThread()==false)
		return nullptr;

	auto ClientConn=static_cast<cConnection*>(Connection);
	COMPtr<IBluetoothDeviceId> DeviceID;
	HRESULT hr;
	hr=ClientConn->Session->get_DeviceId(COMRetPtrT(DeviceID));

	cHString ClientDeviceID;
	hr=DeviceID->get_Id(&ClientDeviceID);


	auto ClientPair=fSubscribedClientMap.GetPair(ClientDeviceID);
	if(ClientPair==nullptr)
		return nullptr;

	return ClientPair->Value;
}
//---------------------------------------------------------------------------
void cGATTServerCharacteristic::NotifyValue(iGATTServerSubscription *Subscription)noexcept
{
	if(Subscription==nullptr){
		fBroadcastValueNotify=true;
	}
	else{
		auto ClientSubscription=static_cast<cSubscription*>(Subscription);
		ClientSubscription->NeedUpdateValue=true;
	}
	fValueNotifyUpdate=true;
	NotifyMainProcess();
}
//---------------------------------------------------------------------------
void cGATTServerCharacteristic::SetupCharacteristic(void)noexcept
{
	fCharacteristic->add_ReadRequested(&fReadRequestedHandler,&fReadRequestedEventToken);
	fCharacteristic->add_WriteRequested(&fWriteRequestedHandler,&fWriteRequestedEventToken);
	fCharacteristic->add_SubscribedClientsChanged(&fSubscribedClientsChangedHandler,&fSubscribedClientsChangedEventToken);
}
//---------------------------------------------------------------------------
void cGATTServerCharacteristic::ClearCharacteristic(void)noexcept
{
	fCharacteristic->remove_ReadRequested(fReadRequestedEventToken);
	fCharacteristic->remove_WriteRequested(fWriteRequestedEventToken);
	fCharacteristic->remove_SubscribedClientsChanged(fSubscribedClientsChangedEventToken);
}
//---------------------------------------------------------------------------
void cGATTServerCharacteristic::RunMainProcess(void)noexcept
{
	if(!fMainProcessExclusiveFlag.Acquire())
		return;

	if(fDispatch->IsCurrentThread()){
		MainProcessThread();
	}
	else{
		fMainProcessWork->Start();
	}
}
//---------------------------------------------------------------------------
void cGATTServerCharacteristic::NotifyMainProcess(void)noexcept
{
	if(!fMainProcessExclusiveFlag.Acquire())
		return;

	fMainProcessWork->Start();
}
//---------------------------------------------------------------------------
void cGATTServerCharacteristic::cMainProcessProcedure::Execute(void)noexcept
{
	auto Host=cnMemory::GetObjectFromMemberPointer(this,&cGATTServerCharacteristic::fMainProcessProcedure);
	return Host->MainProcessThread();
}
//---------------------------------------------------------------------------
void cGATTServerCharacteristic::MainProcessThread(void)noexcept
{
	do{
		do{
			fMainProcessExclusiveFlag.Continue();

		}while(MainProcess());

	}while(!fMainProcessExclusiveFlag.Release());
}
//---------------------------------------------------------------------------
bool cGATTServerCharacteristic::MainProcess(void)noexcept
{

	HRESULT hr;
	if(fNeedNotifyActiveStateChange){
		fNeedNotifyActiveStateChange=false;

		bool NewActiveState;
		if(fCharacteristic==nullptr){
			NewActiveState=false;
		}
		else{
			NewActiveState=fServiceActive;
		}

		if(fCharacteristicActive!=NewActiveState){
			fCharacteristicActive=NewActiveState;

			for(auto Pair : fDescriptorMap){
				Pair.Value->CharacteristicNotifyState(fCharacteristicActive);
			}

		}
	}

	switch(fUWPGattState){
	case gsNone:
		if(fShutdown){
			for(auto DescPair : fDescriptorMap){
				DescPair.Value->CharacteristicNotifyShutdown();
			}
			fUWPGattState=gsShutdownTree;
			return true;
		}
		if(fServiceActive==false){
			// server not active
			break;
		}
		// create Interface
		fReleaseGatt=false;
		return MainProcess_CreateCharacteristic();
	case gsCreating:
		break;
	case gsCreateResult:
		
		if(FAILED(fCreateLocalCharCompleteHandler.hrResult)){
			fUWPGattState=gsNone;
			return false;
		}

		hr=fCreateLocalCharCompleteHandler.Result->get_Characteristic(COMRetPtrT(fCharacteristic));
		if(FAILED(hr)){
			fUWPGattState=gsNone;
			return false;
		}

		SetupCharacteristic();

		fUWPGattState=gsCreated;
		fNeedNotifyActiveStateChange=true;
		return true;
	case gsCreated:
		if(fShutdown){
			for(auto DescPair : fDescriptorMap){
				DescPair.Value->CharacteristicNotifyShutdown();
			}
			fUWPGattState=gsShutdownTree;
			return true;
		}
		break;
	case gsShutdownTree:
		if(fDescriptorMap.GetCount()==0){
			// all descriptor shutdown

			ClearCharacteristic();
			fCharacteristic=nullptr;

			fController->CharacteristicClosed();
			fController=nullptr;
			fControllerReference=nullptr;

			fOwner->CharacteristicNotifyShutdown(this);
			fUWPGattState=gsShutdown;
			return false;
		}
	case gsShutdown:
		break;
	}
	
	if(fSubscribedClientChanged){
		fSubscribedClientChanged=false;

		COMPtr< ABI::Windows::Foundation::Collections::IVectorView<GattSubscribedClient*> > ClientsView;
		hr=fCharacteristic->get_SubscribedClients(COMRetPtrT(ClientsView));

		unsigned int ClientCount;
		hr=ClientsView->get_Size(&ClientCount);

		COMPtr<IGattSubscribedClient> Client;

		auto PrevClientMap=cnVar::MoveCast(fSubscribedClientMap);
		for(unsigned int i=0;i<ClientCount;i++){
			hr=ClientsView->GetAt(i,COMRetPtrT(Client));

			COMPtr<IGattSession> Session;
			hr=Client->get_Session(COMRetPtrT(Session));

			ABI::Windows::Devices::Bluetooth::GenericAttributeProfile::GattSessionStatus SessionStatus;
			hr=Session->get_SessionStatus(&SessionStatus);
			if(SessionStatus!=ABI::Windows::Devices::Bluetooth::GenericAttributeProfile::GattSessionStatus_Active){
				continue;
			}

			COMPtr<IBluetoothDeviceId> ClientDeviceID;
			hr=Session->get_DeviceId(COMRetPtrT(ClientDeviceID));

			cHString DeviceIDString;
			hr=ClientDeviceID->get_Id(&DeviceIDString);


			auto PrevPair=PrevClientMap.GetPair(DeviceIDString);
			bool Exists;
			if(PrevPair!=nullptr){
				fSubscribedClientMap.InsertPair(DeviceIDString,Exists,PrevPair->Value);
			}
			else{
				auto NewClient=rCreate<cSubscription>();
				NewClient->Owner=this;
				NewClient->Client=cnVar::MoveCast(Client);
				cSubscription *pNewClient=NewClient;
				fSubscribedClientMap.InsertPair(DeviceIDString,Exists,cnVar::MoveCast(NewClient));
				if(fController!=nullptr){
					fController->ClientSubscribe(pNewClient);
				}
			}
		}

		
		if(fController!=nullptr){
			for(auto p : PrevClientMap){
				fController->ClientUnsubscribe(p.Value);
			}
		}

	}

	if(fValueNotifyUpdate && fCharacteristic!=nullptr){
		fValueNotifyUpdate=false;
	
		COMPtr<ABI::Windows::Storage::Streams::IDataWriter> DataWriter;
		cHString RCName_DataWriter=RuntimeClass_Windows_Storage_Streams_DataWriter;
		hr=Windows::Foundation::ActivateInstance(RCName_DataWriter,COMRetPtrT(DataWriter));

		if(fBroadcastValueNotifyState==vnIdle){
			if(fBroadcastValueNotify){
				fBroadcastValueNotify=false;
				fBroadcastValueNotifyState=vnCommand;
			}
		}
		if(fBroadcastValueNotifyState==vnCommand){
			MainProcess_ValueBroadcastNotify(DataWriter);
		}

		for(auto p : fSubscribedClientMap){
			cSubscription *sc=p.Value;
			MainProcess_ValueNotify(sc,DataWriter);
		}

	}

	return false;
}
//---------------------------------------------------------------------------
bool cGATTServerCharacteristic::MainProcess_CreateCharacteristic(void)noexcept
{
	if(fServiceActive==false)
		return false;

	auto OwnerService=fOwner->GetLocalService();
	if(OwnerService==nullptr){
		return false;
	}

	HRESULT hr;
	
	// look for existing descriptor
	COMPtr< ABI::Windows::Foundation::Collections::IVectorView<GattLocalCharacteristic*> > Characteristics;
	hr=OwnerService->get_Characteristics(COMRetPtrT(Characteristics));
	if(FAILED(hr))
		return false;

	unsigned int CharacteristicCount;
	hr=Characteristics->get_Size(&CharacteristicCount);

	for(unsigned int i=0;i<CharacteristicCount;i++){
		COMPtr<IGattLocalCharacteristic> Char;
		hr=Characteristics->GetAt(i,COMRetPtrT(Char));

		GUID CharID;
		hr=Char->get_Uuid(&CharID);
		if(CharID==reinterpret_cast<GUID&>(fCharacteristicID)){
			fCharacteristic=cnVar::MoveCast(Char);
			fUWPGattState=gsCreated;
		
			SetupCharacteristic();

			fNeedNotifyActiveStateChange=true;
			return true;
		}
	}

	COMPtr< ABI::Windows::Foundation::IAsyncOperation<GattLocalCharacteristicResult*> > AsyncOp;

	COMPtr<IGattLocalCharacteristicParameters> Parameters;
	cHString RCName_Parameters=RuntimeClass_Windows_Devices_Bluetooth_GenericAttributeProfile_GattLocalCharacteristicParameters;
	hr=Windows::Foundation::ActivateInstance(RCName_Parameters,COMRetPtrT(Parameters));
	if(FAILED(hr)){
		return false;
	}

	auto Parameter=fController->GetCharacteristicParameter();
	
	typedef ABI::Windows::Devices::Bluetooth::GenericAttributeProfile::GattCharacteristicProperties GattCharacteristicProperties;
	Parameters->put_CharacteristicProperties(static_cast<GattCharacteristicProperties>(Parameter.Properties));

	//Parameters->put_UserDescription(ABI::Windows::Devices::Bluetooth::GenericAttributeProfile::GattProtectionLevel_Plain);
	Parameters->put_ReadProtectionLevel(ABI::Windows::Devices::Bluetooth::GenericAttributeProfile::GattProtectionLevel_Plain);
	Parameters->put_WriteProtectionLevel(ABI::Windows::Devices::Bluetooth::GenericAttributeProfile::GattProtectionLevel_Plain);

	hr=OwnerService->CreateCharacteristicAsync(reinterpret_cast<const GUID&>(fCharacteristicID),Parameters,COMRetPtrT(AsyncOp));
	if(FAILED(hr)){
		return false;
	}

	fUWPGattState=gsCreating;
	AsyncOp->put_Completed(&fCreateLocalCharCompleteHandler);
	return false;
}
//---------------------------------------------------------------------------
void cGATTServerCharacteristic::MainProcess_ValueBroadcastNotify(ABI::Windows::Storage::Streams::IDataWriter *DataWriter)noexcept
{
	switch(fBroadcastValueNotifyState){
	default:
		return;
	case vnIdle:
		if(fBroadcastValueNotify==false){
			return;
		}
		fBroadcastValueNotify=false;
		break;
	case vnCommand:
		fBroadcastValueNotify=false;
		break;
	}
	/*
	HRESULT hr;
	cUWPMemoryBufferWriteStreamBuffer Buffer(DataWriter);
	if(fController->ValueNotify(nullptr,&Buffer)==false){
		COMPtr<ABI::Windows::Storage::Streams::IBuffer> DataBuffer;
		hr=DataWriter->DetachBuffer(COMRetPtrT(DataBuffer));
		return;
	}
	COMPtr<ABI::Windows::Storage::Streams::IBuffer> DataBuffer;
	hr=DataWriter->DetachBuffer(COMRetPtrT(DataBuffer));

	COMPtr<IClientNotifyValueAsyncOp> AsyncOp;
	hr=fCharacteristic->NotifyValueAsync(DataBuffer,COMRetPtrT(AsyncOp));

	fBroadcastValueNotifyState=vnAwait;
	hr=AsyncOp->put_Completed(&Subscription->ClientNotifyValueCompleteHandler);*/
}
//---------------------------------------------------------------------------
void cGATTServerCharacteristic::MainProcess_ValueNotify(cSubscription *Subscription,ABI::Windows::Storage::Streams::IDataWriter *DataWriter)noexcept
{
	switch(Subscription->UpdateValueState){
	default:
		return;
	case vnIdle:
		if(Subscription->NeedUpdateValue==false){
			return;
		}
		Subscription->NeedUpdateValue=false;
		break;
	case vnCommand:
		Subscription->NeedUpdateValue=false;
		break;
	}


	HRESULT hr;
	cUWPMemoryBufferWriteStreamBuffer Buffer(DataWriter);
	COMPtr<ABI::Windows::Storage::Streams::IBuffer> DataBuffer;
	if(fController==nullptr || fController->ValueNotify(Subscription,&Buffer)==false){
		hr=DataWriter->DetachBuffer(COMRetPtrT(DataBuffer));
		return;
	}
	hr=DataWriter->DetachBuffer(COMRetPtrT(DataBuffer));

	COMPtr<IClientNotifyValueAsyncOp> AsyncOp;
	hr=fCharacteristic->NotifyValueForSubscribedClientAsync(DataBuffer,Subscription->Client,COMRetPtrT(AsyncOp));

	Subscription->UpdateValueState=vnAwait;
	hr=AsyncOp->put_Completed(&Subscription->ClientNotifyValueCompleteHandler);

}
//---------------------------------------------------------------------------
void cGATTServerCharacteristic::DescriptorNotifyShutdown(cGATTServerDescriptor *Descriptor)noexcept
{
	if(fDescriptorMap.Remove(Descriptor->GetID())){
		if(fDescriptorMap.GetCount()==0){
			RunMainProcess();
		}
	}
}
//---------------------------------------------------------------------------
void cGATTServerCharacteristic::ServiceNotifyState(bool Active)noexcept
{
	fServiceActive=Active;
	fNeedNotifyActiveStateChange=true;
	RunMainProcess();
}
//---------------------------------------------------------------------------
void cGATTServerCharacteristic::ServiceNotifyPublish(void)noexcept
{
	fCharacteristicPublished=true;

	if(fController!=nullptr){
		fController->CharacteristicPublished();
	}
}
//---------------------------------------------------------------------------
void cGATTServerCharacteristic::ServiceNotifyShutdown(void)noexcept
{
	if(fShutdown==false){
		fShutdown=true;
		RunMainProcess();
	}
}
//---------------------------------------------------------------------------
cGATTServerCharacteristic* cGATTServerCharacteristic::cCreateLocalCharCompleteHandler::GetHost(void)noexcept
{
	return cnMemory::GetObjectFromMemberPointer(this,&cGATTServerCharacteristic::fCreateLocalCharCompleteHandler);
}
//---------------------------------------------------------------------------
void cGATTServerCharacteristic::cCreateLocalCharCompleteHandler::EventIncReference(void)noexcept
{
	GetHost()->InnerIncReference('crat');
}
//---------------------------------------------------------------------------
void cGATTServerCharacteristic::cCreateLocalCharCompleteHandler::EventDecReference(void)noexcept
{
	GetHost()->InnerDecReference('crat');
}
//---------------------------------------------------------------------------
void cGATTServerCharacteristic::cCreateLocalCharCompleteHandler::EventInvoke(ICreateLocalCharAsyncOp *AsyncOp,ABI::Windows::Foundation::AsyncStatus Status)noexcept
{
	auto Host=GetHost();
	Host->fUWPGattState=gsCreateResult;
	if(Status!=AsyncStatus::Completed){
		hrResult=E_FAIL;
	}
	else{
		hrResult=AsyncOp->GetResults(COMRetPtrT(Result));
	}
	Host->NotifyMainProcess();
}
//---------------------------------------------------------------------------
rPtr<cGATTServerCharacteristic::cConnection> cGATTServerCharacteristic::QueryClient(IGattSession *Session)noexcept
{
	cnLib_ASSERT(fDispatch->IsCurrentThread());
	/*
	HRESULT hr;
	COMPtr<ABI::Windows::Devices::Bluetooth::IBluetoothDeviceId> ClientDeviceID;
	hr=Session->get_DeviceId(COMRetPtrT(ClientDeviceID));

	cHString DeviceIDString;
	hr=ClientDeviceID->get_Id(&DeviceIDString);

	bool Exists;
	auto SessionPair=fClientSessionMap.InsertPair(DeviceIDString);
	if(SessionPair!=nullptr){
		return SessionPair->Value;
	}
	*/

	auto Connection=rCreate<cConnection>();
	Connection->Session=Session;
	return Connection;
}
//---------------------------------------------------------------------------
void cGATTServerCharacteristic::cRequestProcessProcedure::Execute(void)noexcept
{
	auto Host=cnMemory::GetObjectFromMemberPointer(this,&cGATTServerCharacteristic::fRequestProcessProcedure);
	return Host->RequestProcessThread();
}
//---------------------------------------------------------------------------
void cGATTServerCharacteristic::RequestProcessThread(void)noexcept
{
	do{
		fRequestProcessExclusiveFlag.Continue();

		RequestProcess();

	}while(!fRequestProcessExclusiveFlag.Release());
}
//---------------------------------------------------------------------------
void cGATTServerCharacteristic::NotifyRequestProcess(void)noexcept
{
	if(!fRequestProcessExclusiveFlag.Acquire())
		return;

	fRequestProcessWork->Start();
}
//---------------------------------------------------------------------------
void cGATTServerCharacteristic::RequestProcess(void)noexcept
{
	HRESULT hr;


	auto ReadRequests=fReadRequestQueue.DequeueAll();
	while(ReadRequests!=nullptr){
		auto CurRequest=arClsTake(ReadRequests,'rreq');
		ReadRequests=ReadRequests->Next;

		UINT32 ReadOffset;
		hr=CurRequest->Request->get_Offset(&ReadOffset);
		auto ClientConnection=QueryClient(CurRequest->Session);
		{
			COMPtr<ABI::Windows::Storage::Streams::IDataWriter> DataWriter;
			cHString RCName_DataWriter=RuntimeClass_Windows_Storage_Streams_DataWriter;
			hr=Windows::Foundation::ActivateInstance(RCName_DataWriter,COMRetPtrT(DataWriter));
			cUWPMemoryBufferWriteStreamBuffer Buffer(DataWriter);
			if(fController!=nullptr){
				fController->ValueRead(ClientConnection,&Buffer);
			}

			COMPtr<ABI::Windows::Storage::Streams::IBuffer> DataBuffer;
			hr=DataWriter->DetachBuffer(COMRetPtrT(DataBuffer));
		
			hr=CurRequest->Request->RespondWithValue(DataBuffer);
		}
		hr=CurRequest->Deferal->Complete();

		
		COMRelaseAsync(cnVar::MoveCast(CurRequest->Session));
		COMRelaseAsync(cnVar::MoveCast(CurRequest->Request));
		COMRelaseAsync(cnVar::MoveCast(CurRequest->Deferal));
		InnerDecReference('rdrq');
	}

	auto WriteRequests=fWriteRequestQueue.DequeueAll();
	while(WriteRequests!=nullptr){
		auto CurRequest=arClsTake(WriteRequests,'wreq');
		WriteRequests=WriteRequests->Next;

		UINT32 WriteOffset;
		hr=CurRequest->Request->get_Offset(&WriteOffset);
		auto ClientConnection=QueryClient(CurRequest->Session);
		COMPtr<ABI::Windows::Storage::Streams::IBuffer> WriteValue;
		hr=CurRequest->Request->get_Value(COMRetPtrT(WriteValue));
		auto WriteMemory=UWP::GetBufferData(WriteValue);
		if(fController!=nullptr){
			fController->ValueWrite(ClientConnection,WriteMemory.Pointer,WriteMemory.Length);
		}
		ABI::Windows::Devices::Bluetooth::GenericAttributeProfile::GattWriteOption WriteOptions;
		CurRequest->Request->get_Option(&WriteOptions);
		if(WriteOptions==ABI::Windows::Devices::Bluetooth::GenericAttributeProfile::GattWriteOption_WriteWithResponse){
			hr=CurRequest->Request->Respond();
		}
		CurRequest->Deferal->Complete();

		COMRelaseAsync(cnVar::MoveCast(CurRequest->Session));
		COMRelaseAsync(cnVar::MoveCast(CurRequest->Request));
		COMRelaseAsync(cnVar::MoveCast(CurRequest->Deferal));
		InnerDecReference('wtrq');
	}
}
//---------------------------------------------------------------------------
cGATTServerCharacteristic* cGATTServerCharacteristic::cReadRequestedHandler::GetHost(void)noexcept
{
	return cnMemory::GetObjectFromMemberPointer(this,&cGATTServerCharacteristic::fReadRequestedHandler);
}
//---------------------------------------------------------------------------
void cGATTServerCharacteristic::cReadRequestedHandler::EventIncReference(void)noexcept
{
	GetHost()->InnerIncReference('read');
}
//---------------------------------------------------------------------------
void cGATTServerCharacteristic::cReadRequestedHandler::EventDecReference(void)noexcept
{
	GetHost()->InnerDecReference('read');
}
//---------------------------------------------------------------------------
void cGATTServerCharacteristic::cReadRequestedHandler::EventInvoke(IGattLocalCharacteristic *sender,IGattReadRequestedEventArgs *args)noexcept
{
	HRESULT hr;

	auto Host=GetHost();
	auto Request=Host->fReadRequestItemRecycler.Query();
	Request->GetReadRequestCompleteHandler.Owner=Host;
	hr=args->get_Session(COMRetPtrT(Request->Session));
	hr=args->GetDeferral(COMRetPtrT(Request->Deferal));

	COMPtr<IGetReadRequestAsyncOp> AsyncOp;
	hr=args->GetRequestAsync(COMRetPtrT(AsyncOp));
	if(FAILED(hr)){
		return;
	}

	hr=AsyncOp->put_Completed(&Request->GetReadRequestCompleteHandler);
	if(SUCCEEDED(hr)){
		Host->InnerIncReference('rdrq');
	}
	
	arClsExtract(Request,'rreq');
}
//---------------------------------------------------------------------------
void cGATTServerCharacteristic::cReadRequestItem::cGetReadRequestCompleteHandler::EventIncReference(void)noexcept
{
	Owner->InnerIncReference('grdr');
}
//---------------------------------------------------------------------------
void cGATTServerCharacteristic::cReadRequestItem::cGetReadRequestCompleteHandler::EventDecReference(void)noexcept
{
	Owner->InnerDecReference('grdr');
}
//---------------------------------------------------------------------------
void cGATTServerCharacteristic::cReadRequestItem::cGetReadRequestCompleteHandler::EventInvoke(IGetReadRequestAsyncOp *AsyncOp,ABI::Windows::Foundation::AsyncStatus)noexcept
{
	auto pItem=cnMemory::GetObjectFromMemberPointer(this,&cReadRequestItem::GetReadRequestCompleteHandler);
	auto Item=static_cast<arCls<cReadRequestItem>*>(pItem);
	AsyncOp->GetResults(COMRetPtrT(Item->Request));

	Owner->fReadRequestQueue.Enqueue(Item);
	Owner->NotifyRequestProcess();
}
//---------------------------------------------------------------------------
cGATTServerCharacteristic* cGATTServerCharacteristic::cWriteRequestedHandler::GetHost(void)noexcept
{
	return cnMemory::GetObjectFromMemberPointer(this,&cGATTServerCharacteristic::fWriteRequestedHandler);
}
//---------------------------------------------------------------------------
void cGATTServerCharacteristic::cWriteRequestedHandler::EventIncReference(void)noexcept
{
	GetHost()->InnerIncReference('read');
}
//---------------------------------------------------------------------------
void cGATTServerCharacteristic::cWriteRequestedHandler::EventDecReference(void)noexcept
{
	GetHost()->InnerDecReference('read');
}
//---------------------------------------------------------------------------
void cGATTServerCharacteristic::cWriteRequestedHandler::EventInvoke(IGattLocalCharacteristic *sender,IGattWriteRequestedEventArgs *args)noexcept
{
	HRESULT hr;

	auto Host=GetHost();
	auto Request=Host->fWriteRequestItemRecycler.Query();
	Request->GetWriteRequestCompleteHandler.Owner=Host;
	hr=args->get_Session(COMRetPtrT(Request->Session));
	hr=args->GetDeferral(COMRetPtrT(Request->Deferal));

	COMPtr<IGetWriteRequestAsyncOp> AsyncOp;
	hr=args->GetRequestAsync(COMRetPtrT(AsyncOp));
	if(FAILED(hr)){
		return;
	}

	hr=AsyncOp->put_Completed(&Request->GetWriteRequestCompleteHandler);
	if(SUCCEEDED(hr)){
		Host->InnerIncReference('wtrq');
	}
	
	arClsExtract(Request,'wreq');

}
//---------------------------------------------------------------------------
void cGATTServerCharacteristic::cWriteRequestItem::cGetWriteRequestCompleteHandler::EventIncReference(void)noexcept
{
	Owner->InnerIncReference('grdr');
}
//---------------------------------------------------------------------------
void cGATTServerCharacteristic::cWriteRequestItem::cGetWriteRequestCompleteHandler::EventDecReference(void)noexcept
{
	Owner->InnerDecReference('grdr');
}
//---------------------------------------------------------------------------
void cGATTServerCharacteristic::cWriteRequestItem::cGetWriteRequestCompleteHandler::EventInvoke(IGetWriteRequestAsyncOp *AsyncOp,ABI::Windows::Foundation::AsyncStatus)noexcept
{
	auto pItem=cnMemory::GetObjectFromMemberPointer(this,&cWriteRequestItem::GetWriteRequestCompleteHandler);
	auto Item=static_cast<arCls<cWriteRequestItem>*>(pItem);
	AsyncOp->GetResults(COMRetPtrT(Item->Request));

	Owner->fWriteRequestQueue.Enqueue(Item);
	Owner->NotifyRequestProcess();
}
//---------------------------------------------------------------------------
cGATTServerCharacteristic* cGATTServerCharacteristic::cSubscribedClientsChangedHandler::GetHost(void)noexcept
{
	return cnMemory::GetObjectFromMemberPointer(this,&cGATTServerCharacteristic::fSubscribedClientsChangedHandler);
}
//---------------------------------------------------------------------------
void cGATTServerCharacteristic::cSubscribedClientsChangedHandler::EventIncReference(void)noexcept
{
	GetHost()->InnerIncReference('scch');
}
//---------------------------------------------------------------------------
void cGATTServerCharacteristic::cSubscribedClientsChangedHandler::EventDecReference(void)noexcept
{
	GetHost()->InnerDecReference('scch');
}
//---------------------------------------------------------------------------
void cGATTServerCharacteristic::cSubscribedClientsChangedHandler::EventInvoke(IGattLocalCharacteristic *sender,IInspectable *args)noexcept
{
	auto Host=GetHost();
	Host->fSubscribedClientChanged=true;
	Host->NotifyMainProcess();
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cGATTServerService::cGATTServerService(cGATTServer *Owner,const cUUID &ID,iReference *Reference,iGATTServiceController *Controller)noexcept
	: fOwner(Owner)
	, fDispatch(Owner->GetDispatch())
	, fServiceID(ID)
	, fControllerReference(Reference)
	, fController(Controller)
	, fMainProcessWork(fDispatch->CreateWork(&fInnerReference,&fMainProcessProcedure))
{
}
//---------------------------------------------------------------------------
void cGATTServerService::VirtualStarted(void)noexcept
{
	cDualReference::VirtualStarted();
}
//---------------------------------------------------------------------------
void cGATTServerService::VirtualStopped(void)noexcept
{
	ShutdownService();
	cDualReference::VirtualStopped();
}
//---------------------------------------------------------------------------
ABI::Windows::Devices::Bluetooth::GenericAttributeProfile::IGattServiceProvider* cGATTServerService::GetServiceProvider(void)const noexcept
{
	return fServiceProvider;
}
//---------------------------------------------------------------------------
ABI::Windows::Devices::Bluetooth::GenericAttributeProfile::IGattLocalService* cGATTServerService::GetLocalService(void)const noexcept
{
	return fService;
}
//---------------------------------------------------------------------------
const cUUID &cGATTServerService::GetID(void)const noexcept
{
	return fServiceID;
}
//---------------------------------------------------------------------------
iDispatch* cGATTServerService::GetDispatch(void)const noexcept
{
	return fDispatch;
}
//---------------------------------------------------------------------------
void cGATTServerService::ShutdownService(void)noexcept
{
	if(fShutdown==false){
		fShutdown=true;

		NotifyMainProcess();
	}
}
//---------------------------------------------------------------------------
iDispatch* cGATTServerService::GetHandlerDispatch(void)noexcept
{
	return fDispatch;
}
//---------------------------------------------------------------------------
rPtr<iGATTServerCharacteristic> cGATTServerService::CreateGATTCharacteristic(const cUUID &ID,iReference *Reference,iGATTCharacteristicController *Controller)noexcept
{
	if(fServicePublished || fShutdown || fDispatch->IsCurrentThread()==false)
		return nullptr;

	bool Exists;
	auto CharPair=fCharacteristicMap.InsertPair(ID,Exists);
	if(Exists){
		return nullptr;
	}

	auto NewChar=rCreate<cGATTServerCharacteristic>(this,ID,Reference,Controller);
	CharPair->Value=NewChar;
	
	NewChar->ServiceNotifyState(fServiceActive);
	return NewChar;
}
//---------------------------------------------------------------------------
void cGATTServerService::Shutdown(void)noexcept
{
	ShutdownService();
}
//---------------------------------------------------------------------------
void cGATTServerService::NotifyMainProcess(void)noexcept
{
	if(!fMainProcessExclusiveFlag.Acquire())
		return;

	fMainProcessWork->Start();
}
//---------------------------------------------------------------------------
void cGATTServerService::RunMainProcess(void)noexcept
{
	if(!fMainProcessExclusiveFlag.Acquire())
		return;

	if(fDispatch->IsCurrentThread()){
		MainProcessThread();
	}
	else{
		fMainProcessWork->Start();
	}
}
//---------------------------------------------------------------------------
void cGATTServerService::cMainProcessProcedure::Execute(void)noexcept
{
	auto Host=cnMemory::GetObjectFromMemberPointer(this,&cGATTServerService::fMainProcessProcedure);
	return Host->MainProcessThread();
}
//---------------------------------------------------------------------------
void cGATTServerService::MainProcessThread(void)noexcept
{
	do{
		do{
			fMainProcessExclusiveFlag.Continue();

		}while(MainProcess());

	}while(!fMainProcessExclusiveFlag.Release());
}
//---------------------------------------------------------------------------
bool cGATTServerService::MainProcess(void)noexcept
{
	HRESULT hr;
	if(fNeedNotifyActiveStateChange){
		fNeedNotifyActiveStateChange=false;

		bool NewActiveState;
		if(fService==nullptr){
			NewActiveState=false;
		}
		else{
			NewActiveState=fServerActive;
		}

		if(fServiceActive!=NewActiveState){
			fServiceActive=NewActiveState;

			for(auto Pair : fCharacteristicMap){
				Pair.Value->ServiceNotifyState(fServiceActive);
			}
		}


		if(fController!=nullptr){
			if(fShutdown && fService==nullptr){
				fController->ServiceClosed();
				fController=nullptr;
			}
		}
	}

	switch(fUWPGattState){
	case gsNone:
		if(fShutdown){
			MainProcess_ShutdownTree();
			return true;
		}

		if(fServerActive==false){
			// server not active
			break;
		}
		// create Interface
		{
			auto Statics=fOwner->GetProviderStatic();
			COMPtr< ABI::Windows::Foundation::IAsyncOperation<GattServiceProviderResult*> > AsyncOp;
			hr=Statics->CreateAsync(reinterpret_cast<const GUID&>(fServiceID),COMRetPtrT(AsyncOp));
			if(FAILED(hr)){
				break;
			}

			fUWPGattState=gsCreating;
			hr=AsyncOp->put_Completed(&fCreateServiceProviderCompleteHandler);
		}

		break;
	case gsCreating:
		break;
	case gsCreateResult:
		
		if(FAILED(fCreateServiceProviderCompleteHandler.hrResult)){
			fUWPGattState=gsNone;
			break;
		}
		{
			auto Result=cnVar::MoveCast(fCreateServiceProviderCompleteHandler.Result);
			hr=Result->get_ServiceProvider(COMRetPtrT(fServiceProvider));
			if(FAILED(hr)){
				fUWPGattState=gsNone;
				break;
			}
		}
		hr=fServiceProvider->get_Service(COMRetPtrT(fService));
		if(FAILED(hr)){
			fServiceProvider=nullptr;
			fUWPGattState=gsNone;
			break;
		}
		fUWPGattState=gsCreated;
		fGattAdvStarted=false;
		hr=fServiceProvider->add_AdvertisementStatusChanged(&fAdvertisementStatusChangedHandler,&fAdvertisementStatusChangedToken);
		fNeedNotifyActiveStateChange=true;
		return true;
	case gsCreated:
		if(fShutdown){
			MainProcess_ShutdownTree();
			return true;
		}

		if(fServicePublished && fServerIsAdvertising){
			// start advertisement for the first time
			COMPtr<IGattServiceProviderAdvertisingParameters> Parameters;
			cHString RCName_Parameters=RuntimeClass_Windows_Devices_Bluetooth_GenericAttributeProfile_GattServiceProviderAdvertisingParameters;
			hr=Windows::Foundation::ActivateInstance(RCName_Parameters,COMRetPtrT(Parameters));
			if(SUCCEEDED(hr)){
				Parameters->put_IsConnectable(true);
				Parameters->put_IsDiscoverable(true);
				hr=fServiceProvider->StartAdvertisingWithParameters(Parameters);
				if(SUCCEEDED(hr)){
					fUWPGattState=gsLocked;
					return true;
				}
			}

		}
		break;
	case gsLocked:
		if(!fShutdown && !fGattAdvStarted){
			break;
		}
		fServiceProvider->remove_AdvertisementStatusChanged(fAdvertisementStatusChangedToken);
		fUWPGattState=gsAdvertised;
		fAdvertisementActivated=true;
		cnLib_SWITCH_FALLTHROUGH;
	case gsAdvertised:
		ProcessAdvertisement();
		if(fShutdown && fAdvertisementActivated==false){
			MainProcess_ShutdownTree();
			return true;
		}
		break;
	case gsShutdownTree:
		if(fCharacteristicMap.GetCount()==0){
			// all characteristic is shut

			fServiceProvider=nullptr;
			fService=nullptr;

			fController->ServiceClosed();
			fController=nullptr;
			fControllerReference=nullptr;

			fOwner->ServiceNotifyShutdown(this);
			fUWPGattState=gsShutdown;
			return false;
		}
		break;
	case gsShutdown:
		break;
	}
	
	return false;
}
//---------------------------------------------------------------------------
void cGATTServerService::MainProcess_ShutdownTree(void)noexcept
{
	for(auto CharPair : fCharacteristicMap){
		CharPair.Value->ServiceNotifyShutdown();
	}
	fUWPGattState=gsShutdownTree;
}
//---------------------------------------------------------------------------
void cGATTServerService::ProcessAdvertisement(void)noexcept
{
	HRESULT hr;

	bool NeedAdvertisement=fServicePublished && fServerIsAdvertising;
	if(fAdvertisementActivated){
		if(NeedAdvertisement==false){
			// stop advertisement
			hr=fServiceProvider->StopAdvertising();
			if(SUCCEEDED(hr)){
				// BUG in UWP BLE: advertisement status not changed after stop
				fAdvertisementActivated=false;
			}
		}
	}
	else{
		if(NeedAdvertisement){
			// start advertisement
			COMPtr<IGattServiceProviderAdvertisingParameters> Parameters;
			cHString RCName_Parameters=RuntimeClass_Windows_Devices_Bluetooth_GenericAttributeProfile_GattServiceProviderAdvertisingParameters;
			hr=Windows::Foundation::ActivateInstance(RCName_Parameters,COMRetPtrT(Parameters));
			if(SUCCEEDED(hr)){
				Parameters->put_IsConnectable(true);
				Parameters->put_IsDiscoverable(true);
				hr=fServiceProvider->StartAdvertisingWithParameters(Parameters);
				if(SUCCEEDED(hr)){
					fAdvertisementActivated=true;
				}
			}
		}
	}

}
//---------------------------------------------------------------------------
void cGATTServerService::CharacteristicNotifyShutdown(cGATTServerCharacteristic *Characteristic)noexcept
{
	if(fCharacteristicMap.Remove(Characteristic->GetID())){
		if(fShutdown){
			RunMainProcess();
		}
	}
}
//---------------------------------------------------------------------------
void cGATTServerService::ServerNotifyState(bool Active)noexcept
{
	fServerActive=Active;
	fNeedNotifyActiveStateChange=true;
	RunMainProcess();
}
//---------------------------------------------------------------------------
void cGATTServerService::ServerNotifyPublish(void)noexcept
{
	fServicePublished=true;

	for(auto CharPair : fCharacteristicMap){
		CharPair.Value->ServiceNotifyPublish();
	}
	if(fController!=nullptr){
		fController->ServicePublished();
	}
}
//---------------------------------------------------------------------------
void cGATTServerService::ServerNotifyShutdown(void)noexcept
{
	if(fShutdown==false){
		fShutdown=true;
		RunMainProcess();
	}
}
//---------------------------------------------------------------------------
void cGATTServerService::ServerNotifyAdvertisementState(bool Advertising)noexcept
{
	fServerIsAdvertising=Advertising;
	RunMainProcess();
}
//---------------------------------------------------------------------------
cGATTServerService* cGATTServerService::cCreateServiceProviderCompleteHandler::GetHost(void)noexcept
{
	return cnMemory::GetObjectFromMemberPointer(this,&cGATTServerService::fCreateServiceProviderCompleteHandler);
}
//---------------------------------------------------------------------------
void cGATTServerService::cCreateServiceProviderCompleteHandler::EventIncReference(void)noexcept
{
	GetHost()->InnerIncReference('crat');
}
//---------------------------------------------------------------------------
void cGATTServerService::cCreateServiceProviderCompleteHandler::EventDecReference(void)noexcept
{
	GetHost()->InnerDecReference('crat');
}
//---------------------------------------------------------------------------
void cGATTServerService::cCreateServiceProviderCompleteHandler::EventInvoke(ICreateServiceProviderAsyncOp *AsyncOp,ABI::Windows::Foundation::AsyncStatus Status)noexcept
{
	auto Host=GetHost();
	Host->fUWPGattState=gsCreateResult;
	if(Status!=AsyncStatus::Completed){
		hrResult=E_FAIL;
	}
	else{
		hrResult=AsyncOp->GetResults(COMRetPtrT(Result));
	}
	Host->NotifyMainProcess();
}
//---------------------------------------------------------------------------
cGATTServerService* cGATTServerService::cAdvertisementStatusChangedHandler::GetHost(void)noexcept
{
	return cnMemory::GetObjectFromMemberPointer(this,&cGATTServerService::fAdvertisementStatusChangedHandler);
}
//---------------------------------------------------------------------------
void cGATTServerService::cAdvertisementStatusChangedHandler::EventIncReference(void)noexcept
{
	GetHost()->InnerIncReference('adst');
}
void cGATTServerService::cAdvertisementStatusChangedHandler::EventDecReference(void)noexcept
{
	GetHost()->InnerDecReference('adst');
}
void cGATTServerService::cAdvertisementStatusChangedHandler::EventInvoke(IGattServiceProvider *sender,IGattServiceProviderAdvertisementStatusChangedEventArgs *args)noexcept
{
	auto Host=GetHost();
	if(Host->fGattAdvStarted)
		return;

	HRESULT hr;
	GattServiceProviderAdvertisementStatus Status;
	hr=args->get_Status(&Status);
	if(FAILED(hr)){
		ABI::Windows::Devices::Bluetooth::BluetoothError Error;
		hr=args->get_Error(&Error);
		if(FAILED(hr))
			return;
		return;
	}
	switch(Status){
	case GattServiceProviderAdvertisementStatus::GattServiceProviderAdvertisementStatus_Started:
	case GattServiceProviderAdvertisementStatus::GattServiceProviderAdvertisementStatus_StartedWithoutAllAdvertisementData:
		Host->fGattAdvStarted=true;
		Host->NotifyMainProcess();
		break;
	}
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cGATTServer::cGATTServer(iServerOwner *Owner,iDispatch *Dispatch,COMPtr<IGattServiceProviderStatics> ProviderStatic,iReference *ControllerReference,iGATTServerController *Controller)noexcept
	: fOwner(Owner)
	, fProviderStatic(ProviderStatic)
	, fDispatch(Dispatch)
	, fControllerReference(ControllerReference)
	, fController(Controller)
	, fMainProcessWork(fDispatch->CreateWork(&fInnerReference,&fMainProcessProcedure))
{
}
//---------------------------------------------------------------------------
cGATTServer::~cGATTServer()
{
}
//---------------------------------------------------------------------------
void cGATTServer::VirtualStarted(void)noexcept
{
	cDualReference::VirtualStarted();
}
//---------------------------------------------------------------------------
void cGATTServer::VirtualStopped(void)noexcept
{
	ShutdownServer();
	cDualReference::VirtualStopped();
}
//---------------------------------------------------------------------------
ABI::Windows::Devices::Bluetooth::GenericAttributeProfile::IGattServiceProviderStatics* cGATTServer::GetProviderStatic(void)const noexcept
{
	return fProviderStatic;
}
//---------------------------------------------------------------------------
iDispatch* cGATTServer::GetDispatch(void)const noexcept
{
	return fDispatch;
}
//---------------------------------------------------------------------------
iDispatch* cGATTServer::GetHandlerDispatch(void)noexcept
{
	return fDispatch;
}
//---------------------------------------------------------------------------
void cGATTServer::Shutdown(void)noexcept
{
	ShutdownServer();
}
//---------------------------------------------------------------------------
void cGATTServer::Publish(void)noexcept
{
	if(fNeedPublish==0){
		fNeedPublish=true;

		NotifyMainProcess();
	}
}
//---------------------------------------------------------------------------
rPtr<iGATTServerService> cGATTServer::CreateGATTService(const cUUID &ID,iReference *Reference,iGATTServiceController *Controller)noexcept
{
	if(fServerPublished || fShutdown || fDispatch->IsCurrentThread()==false)
		return nullptr;

	bool Exists;
	auto ServicePair=fServiceMap.InsertPair(ID,Exists);
	if(Exists){
		return nullptr;
	}

	auto NewService=rCreate<cGATTServerService>(this,ID,Reference,Controller);
	ServicePair->Value=NewService;

	NewService->ServerNotifyState(fServerActive);
	return NewService;
}
//---------------------------------------------------------------------------
void cGATTServer::RunMainProcess(void)noexcept
{
	if(!fMainProcessExclusiveFlag.Acquire())
		return;

	if(fDispatch->IsCurrentThread()){
		MainProcessThread();
	}
	else{
		fMainProcessWork->Start();
	}
}
//---------------------------------------------------------------------------
void cGATTServer::NotifyMainProcess(void)noexcept
{
	if(!fMainProcessExclusiveFlag.Acquire())
		return;

	fMainProcessWork->Start();
}
//---------------------------------------------------------------------------
void cGATTServer::cMainProcessProcedure::Execute(void)noexcept
{
	auto Host=cnMemory::GetObjectFromMemberPointer(this,&cGATTServer::fMainProcessProcedure);
	return Host->MainProcessThread();
}
//---------------------------------------------------------------------------
void cGATTServer::MainProcessThread(void)noexcept
{
	do{
		do{
			fMainProcessExclusiveFlag.Continue();

		}while(MainProcess());

	}while(!fMainProcessExclusiveFlag.Release());
}
//---------------------------------------------------------------------------
bool cGATTServer::MainProcess(void)noexcept
{
	if(fNeedPublish){
		fNeedPublish=false;
		if(fServerPublished==false){
			fServerPublished=true;
			for(auto ServicePair : fServiceMap){
				ServicePair.Value->ServerNotifyPublish();
			}
			fController->ServerPublished();
		}
	}

	HRESULT hr;
	if(fNeedNotifyServerStateChange){
		fNeedNotifyServerStateChange=false;

		bool NewActiveState;
		if(fOwner==nullptr){
			NewActiveState=false;
		}
		else{
			NewActiveState=fOwner->OwnerIsActive();
		}

		if(fServerActive!=NewActiveState){
			fServerActive=NewActiveState;

			for(auto Pair : fServiceMap){
				if(Pair.Value!=nullptr){
					Pair.Value->ServerNotifyState(fServerActive);
				}
			}
		}
	}
	
	if(fShutdown){
		switch(fShutdownState){
		case 0:
			for(auto ServicePair : fServiceMap){
				ServicePair.Value->ServerNotifyShutdown();
			}
			fServerPublished=true;
			fShutdownState=1;
			cnLib_SWITCH_FALLTHROUGH;
		case 1:
			if(fServiceMap.GetCount()==0){
				fOwner->ServerClosed(this);
				fOwner=nullptr;
				fProviderStatic=nullptr;

				fController->ServerClosed();
				fController=nullptr;
				fControllerReference=nullptr;

				fShutdownState=2;
			}
			break;
		default:
		case 2:
			break;
		}
	}

	return false;
}
//---------------------------------------------------------------------------
void cGATTServer::ShutdownServer(void)noexcept
{
	if(fShutdown==false){
		fShutdown=true;

		NotifyMainProcess();
	}
}
//---------------------------------------------------------------------------
void cGATTServer::OwnerNotifyStateChange(void)noexcept
{
	fNeedNotifyServerStateChange=true;
	RunMainProcess();
}
//---------------------------------------------------------------------------
void cGATTServer::OwnerNotifyAdvertisementState(void)noexcept
{
	bool IsAdvertising=fOwner->OwnerIsAdvertising();
	for(auto ServicePair : fServiceMap){
		ServicePair.Value->ServerNotifyAdvertisementState(IsAdvertising);
	}
}
//---------------------------------------------------------------------------
void cGATTServer::ServiceNotifyShutdown(cGATTServerService *Service)noexcept
{
	if(fServiceMap.Remove(Service->GetID())){
		if(fShutdown){
			RunMainProcess();
		}
	}
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cBluetoothPeripheral::cBluetoothPeripheral(iDispatch *Dispatch,COMPtr<IGattServiceProviderStatics> ProviderStatic)noexcept
	: fProviderStatic(ProviderStatic)
	, fDispatch(Dispatch)
	, fMainProcessWork(fDispatch->CreateWork(static_cast<iBluetoothPeripheral*>(this),&fMainProcessProcedure))
{
}
//---------------------------------------------------------------------------
ABI::Windows::Devices::Bluetooth::GenericAttributeProfile::IGattServiceProviderStatics* cBluetoothPeripheral::GetProviderStatic(void)const noexcept
{
	return fProviderStatic;
}
//---------------------------------------------------------------------------
iDispatch* cBluetoothPeripheral::GetDispatch(void)const noexcept
{
	return fDispatch;
}
//---------------------------------------------------------------------------
iDispatch* cBluetoothPeripheral::GetHandlerDispatch(void)noexcept
{
	return fDispatch;
}
//---------------------------------------------------------------------------
bool cBluetoothPeripheral::InsertHandler(iBluetoothPeripheralHandler *Handler)noexcept
{
	if(fDispatch->IsCurrentThread()==false)
		return false;
	fHandlers.Insert(Handler);
	return true;
}
//---------------------------------------------------------------------------
bool cBluetoothPeripheral::RemoveHandler(iBluetoothPeripheralHandler *Handler)noexcept
{
	if(fDispatch->IsCurrentThread()==false)
		return false;
	fHandlers.Remove(Handler);
	return true;
}
//---------------------------------------------------------------------------
bool cBluetoothPeripheral::IsEnabled(void)noexcept
{
	return true;
}
//---------------------------------------------------------------------------
bool cBluetoothPeripheral::IsPowerOn(void)noexcept
{
	return true;
}
//---------------------------------------------------------------------------
bool cBluetoothPeripheral::IsAdvertising(void)noexcept
{
	return fEffectiveAdvertisementActive;
}
//---------------------------------------------------------------------------
bool cBluetoothPeripheral::GetAdvertisementActive(void)noexcept
{
	return fAdvertisementActive;
}
//---------------------------------------------------------------------------
void cBluetoothPeripheral::SetAdvertisementActive(bool Active)noexcept
{
	fAdvertisementActive=Active;

	NotifyMainProcess();
}
//---------------------------------------------------------------------------
rPtr<iGATTServer> cBluetoothPeripheral::CreateServer(iReference *ControllerReference,iGATTServerController *Controller)noexcept
{
	auto Server=rCreate<cGATTServer>(static_cast<cGATTServer::iServerOwner*>(this),fDispatch,fProviderStatic,ControllerReference,Controller);

	rIncReference(this,'serv');
	fServerSet.Insert(Server);

	Server->OwnerNotifyStateChange();
	return Server;
}
//---------------------------------------------------------------------------
void cBluetoothPeripheral::cMainProcessProcedure::Execute(void)noexcept
{
	auto Host=cnMemory::GetObjectFromMemberPointer(this,&cBluetoothPeripheral::fMainProcessProcedure);
	return Host->MainProcessThread();
}
//---------------------------------------------------------------------------
void cBluetoothPeripheral::MainProcessThread(void)noexcept
{
	do{
		do{
			fMainProcessExclusiveFlag.Continue();

		}while(MainProcess());

	}while(!fMainProcessExclusiveFlag.Release());
}
//---------------------------------------------------------------------------
void cBluetoothPeripheral::NotifyMainProcess(void)noexcept
{
	if(!fMainProcessExclusiveFlag.Acquire())
		return;

	fMainProcessWork->Start();
}
//---------------------------------------------------------------------------
bool cBluetoothPeripheral::MainProcess(void)noexcept
{
	HRESULT hr;
	if(fNeedNotifyStateChange){
		fNeedNotifyStateChange=false;

		for(auto Handler : fHandlers){
			Handler->PeripheralStateChanged();
		}
	}
	if(fEffectiveAdvertisementActive!=fAdvertisementActive){
		fEffectiveAdvertisementActive=fAdvertisementActive;

		for(auto Server : fServerSet){
			Server->OwnerNotifyAdvertisementState();
		}

		for(auto Handler : fHandlers){
			Handler->PeripheralAdvertisementStateChanged();
		}
	}
	return false;
}
//---------------------------------------------------------------------------
void cBluetoothPeripheral::ServerClosed(cGATTServer *Server)noexcept
{
	if(fServerSet.Remove(Server)){
		rDecReference(this,'serv');
	}
}
//---------------------------------------------------------------------------
bool cBluetoothPeripheral::OwnerIsActive(void)noexcept
{
	return true;
}
//---------------------------------------------------------------------------
bool cBluetoothPeripheral::OwnerIsAdvertising(void)noexcept
{
	return fEffectiveAdvertisementActive;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
rPtr<iBluetoothCentral> UWP::OpenBluetoothCentral(iDispatch *Dispatch)noexcept
{
	COMPtr<ABI::Windows::Devices::Bluetooth::IBluetoothLEDeviceStatics> BLEStatic;
	cHString RCName_BLEDevice=RuntimeClass_Windows_Devices_Bluetooth_BluetoothLEDevice;
	if(FAILED(Windows::Foundation::GetActivationFactory(RCName_BLEDevice,COMRetPtrT(BLEStatic)))){
		return nullptr;
	}
	return rCreate<cBluetoothCentral>(Dispatch,cnVar::MoveCast(BLEStatic));
}
//---------------------------------------------------------------------------
rPtr<iBluetoothPeripheral> UWP::OpenBluetoothPeripheral(iDispatch *Dispatch)noexcept
{
	COMPtr<ABI::Windows::Devices::Bluetooth::GenericAttributeProfile::IGattServiceProviderStatics> ProviderStatics;
	cHString RCName_Provider=RuntimeClass_Windows_Devices_Bluetooth_GenericAttributeProfile_GattServiceProvider;
	if(FAILED(Windows::Foundation::GetActivationFactory(RCName_Provider,COMRetPtrT(ProviderStatics)))){
		return nullptr;
	}
	return rCreate<cBluetoothPeripheral>(Dispatch,cnVar::MoveCast(ProviderStatics));
}
//---------------------------------------------------------------------------

#endif	// _WIN32_WINNT >= _WIN32_WINNT_WIN10
