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
	, fFuncState(GATTFunctionState::Scanning)
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
eGATTFunctionState cGATTDescriptor::GetFunctionState(void)noexcept
{
	return fFuncState;
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
	eGATTFunctionState NewState;
	if(fDescriptor==nullptr){
		if(fOwner->DescriptorIsRefreshing()){
			NewState=GATTFunctionState::Scanning;
		}
		else{
			NewState=GATTFunctionState::Absent;
		}
	}
	else{
		NewState=fOwner->CharacteristicGetFunctionState();
	}

	if(fFuncState!=NewState){
		fFuncState=NewState;

		for(auto Handler : fHandlers){
			Handler->GATTDescriptorStateChanged();
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
	, fFuncState(GATTFunctionState::Scanning)
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
eGATTFunctionState cGATTCharacteristic::GetFunctionState(void)noexcept
{
	return fFuncState;
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
iPtr< iAsyncFunction<iConstMemoryReference> > cGATTCharacteristic::Read(void)noexcept
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
eGATTFunctionState cGATTCharacteristic::CharacteristicGetFunctionState(void)noexcept
{
	return fFuncState;
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
		
		eGATTFunctionState NewState;
		if(fCharacteristic==nullptr){
			if(fOwner->CharacteristicIsRefreshing()){
				NewState=GATTFunctionState::Scanning;
			}
			else{
				NewState=GATTFunctionState::Absent;
			}
		}
		else{
			NewState=fOwner->ServiceGetFunctionState();
		}

		if(fFuncState!=NewState){
			fFuncState=NewState;

			cSeqList< rInnerPtr<cGATTDescriptor> > DescListCache;
			for(auto Pair : fDescriptorMap){
				DescListCache.AppendMake(Pair.Value);
			}

			for(auto Desc : DescListCache){
				Desc->CharacteristicNotifyFunctionStatus();
			}

			for(auto Handler : fHandlers){
				Handler->GATTCharacteristStateChanged();
			}
		}
	}

	ArrangeValueNotification();
	while(fValueNotificationMap.GetCount()!=0){
		auto Pair=fValueNotificationMap.GetPairAt(0);
		auto NotificationItem=Pair->Value;
		for(auto Handler : fHandlers){
			Handler->GATTCharacteristValueNotify(NotificationItem->Value->Pointer,NotificationItem->Value->Length);
		}
		fValueNotificationRecycler.Recycle(NotificationItem);
		
		fValueNotificationMap.RemovePair(Pair);
		ArrangeValueNotification();
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
void cGATTCharacteristic::ArrangeValueNotification(void)noexcept
{
	auto ValueNotifications=fValueNotificationStack.Swap(nullptr);
	while(ValueNotifications!=nullptr){
		auto CurNotify=ValueNotifications;
		ValueNotifications=ValueNotifications->Next;

		fValueNotificationMap[CurNotify->Timestamp]=CurNotify;
	}
}
//---------------------------------------------------------------------------
void cGATTCharacteristic::NotifyValue(uInt64 Timestamp,const void *Data,uIntn Size)noexcept
{
	auto ValueNotificationItem=fValueNotificationRecycler.Query();
	ValueNotificationItem->Timestamp=Timestamp;
	ValueNotificationItem->Value.Copy(Data,Size);

	fValueNotificationStack.Push(ValueNotificationItem);
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
cGATTService::cGATTService(cGATTPeripheral *Owner,const cUUID &ID)noexcept
	: ServiceUUID(ID)
	, fOwner(Owner)
	, fDispatch(Owner->GetDispatch())
	, fMainProcessWork(Owner->GetDispatch()->CreateWork(&fInnerReference,&fMainProcessProcedure))
	, fFuncState(GATTFunctionState::Scanning)
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
eGATTFunctionState cGATTService::GetFunctionState(void)noexcept
{
	return fFuncState;
}
//---------------------------------------------------------------------------
iGATTPeripheral* cGATTService::GetPeripheral(void)noexcept
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
	NotifyMainProcess();
}
//---------------------------------------------------------------------------
void cGATTService::PeripheralInvalidateService(void)noexcept
{
	fNeedNotifyServiceState=true;
	NotifyMainProcess();

	for(auto CharPair : fCharacteristicMap){
		CharPair.Value->ServiceInvalidateCharacteristic();
	}
}
//---------------------------------------------------------------------------
eGATTFunctionState cGATTService::ServiceGetFunctionState(void)noexcept
{
	if(fService==nullptr){
		return GATTFunctionState::Absent;
	}

	return fOwner->PeripheralGetFunctionState();
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
	if(fNeedNotifyServiceState){
		fNeedNotifyServiceState=false;
		
		eGATTFunctionState NewState;
		if(fService==nullptr){
			if(fScaningService){
				NewState=GATTFunctionState::Scanning;
			}
			else{
				NewState=GATTFunctionState::Absent;
			}
		}
		else{
			NewState=fOwner->PeripheralGetFunctionState();
		}

		if(fFuncState!=NewState){
			fFuncState=NewState;

			cSeqList< rInnerPtr<cGATTCharacteristic> > CharListCache;
			for(auto Pair : fCharacteristicMap){
				CharListCache.AppendMake(Pair.Value);
			}

			for(auto Service : CharListCache){
				Service->ServiceNotifyFunctionStatus();
			}

			for(auto Handler : fHandlers){
				Handler->GATTServiceStateChanged();
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
cGATTPeripheral::cGATTPeripheral(cGATTPeripheralCentral *Central,iPtr<cBluetoothAddress> Address)noexcept
	: fCentral(Central)
	, fDispatch(Central->GetDispatch())
	, fAddress(cnVar::MoveCast(Address))
	, fMainProcessWork(Central->GetDispatch()->CreateWork(&fInnerReference,&fMainProcessProcedure))
	, fDeviceOpened(false)
	, fFuncState(GATTFunctionState::Scanning)
{
}
//---------------------------------------------------------------------------
cGATTPeripheral::~cGATTPeripheral()noexcept
{
}
//---------------------------------------------------------------------------
void cGATTPeripheral::VirtualStarted(void)noexcept
{
	InnerActivate('self');
}
//---------------------------------------------------------------------------
void cGATTPeripheral::VirtualStopped(void)noexcept
{
	if(fDevice!=nullptr){
		ClearDevice();
	}
	InnerDecReference('self');
}
//---------------------------------------------------------------------------
iDispatch* cGATTPeripheral::GetDispatch(void)const noexcept
{
	return fDispatch;
}
//---------------------------------------------------------------------------
iDispatch* cGATTPeripheral::GetHandlerDispatch(void)noexcept
{
	return fDispatch;
}
//---------------------------------------------------------------------------
bool cGATTPeripheral::InsertHandler(iGATTPeripheralHandler *Handler)noexcept
{
	if(fDispatch->IsCurrentThread()==false)
		return false;
	fHandlers.Insert(Handler);
	return true;
}
//---------------------------------------------------------------------------
bool cGATTPeripheral::RemoveHandler(iGATTPeripheralHandler *Handler)noexcept
{
	if(fDispatch->IsCurrentThread()==false)
		return false;
	fHandlers.Remove(Handler);
	return true;
}
//---------------------------------------------------------------------------
void cGATTPeripheral::Close(void)noexcept
{
}
//---------------------------------------------------------------------------
iAddress* cGATTPeripheral::GetPeripheralAddress(void)noexcept
{
	return fAddress;
}
//---------------------------------------------------------------------------
eGATTFunctionState cGATTPeripheral::GetFunctionState(void)noexcept
{
	return PeripheralGetFunctionState();
}
//---------------------------------------------------------------------------
eGATTFunctionState cGATTPeripheral::PeripheralGetFunctionState(void)noexcept
{
	if(fDevice==nullptr){
		return GATTFunctionState::Absent;
	}
	return fFuncState;
}
//---------------------------------------------------------------------------
iGATTPeripheralCentral* cGATTPeripheral::GetPeripheralCentral(void)noexcept
{
	return fCentral;
}
//---------------------------------------------------------------------------
rPtr< iArrayReference<const uChar16> > cGATTPeripheral::GetName(void)noexcept
{
	if(fDispatch->IsCurrentThread()==false)
		return nullptr;
	if(fDevice==nullptr){
		return nullptr;
	}

	return fDeviceName.Token();
}
//---------------------------------------------------------------------------
rPtr<iGATTService> cGATTPeripheral::AccessService(const cUUID &ID)noexcept
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
iPtr<iGATTServiceObserver> cGATTPeripheral::CreateServiceObserver(void)noexcept
{
	return nullptr;
}
//---------------------------------------------------------------------------
void cGATTPeripheral::DeviceOpened(COMPtr<IBLEDevice> Device)noexcept
{
	fDeviceOpened=true;
	fDevice=cnVar::MoveCast(Device);
	fDevice->QueryInterface(COMRetPtrT(fDevice3));
	SetupDevice();

	RefreshService();

	fNeedNotifyConnectionStateChange=true;
	fNeedNotifyNameChange=true;

	NotifyMainProcess();
}
//---------------------------------------------------------------------------
void cGATTPeripheral::DeviceOpenFailed(HRESULT hr)noexcept
{
	fDeviceOpened=true;
}
//---------------------------------------------------------------------------
void cGATTPeripheral::SetupDevice(void)noexcept
{
	fDevice->add_NameChanged(&fBLEDeviceNameChangedHandler,&fBLEDeviceNameChangedToken);
	fDevice->add_ConnectionStatusChanged(&fBLEDeviceConnectionStatusHandler,&fBLEDeviceConnectionStatusToken);
	fDevice->add_GattServicesChanged(&fBLEDeviceServicesChangedHandler,&fBLEDeviceServicesChangedToken);
}
//---------------------------------------------------------------------------
void cGATTPeripheral::ClearDevice(void)noexcept
{
	fDevice->remove_NameChanged(fBLEDeviceNameChangedToken);
	fDevice->remove_ConnectionStatusChanged(fBLEDeviceConnectionStatusToken);
	fDevice->remove_GattServicesChanged(fBLEDeviceServicesChangedToken);
}
//---------------------------------------------------------------------------
cGATTPeripheral* cGATTPeripheral::cBLEDeviceNameChangedHandler::GetHost(void)noexcept
{
	return cnMemory::GetObjectFromMemberPointer(this,&cGATTPeripheral::fBLEDeviceNameChangedHandler);
}
//---------------------------------------------------------------------------
void cGATTPeripheral::cBLEDeviceNameChangedHandler::EventIncReference(void)noexcept
{
	GetHost()->InnerIncReference('evet');
}
void cGATTPeripheral::cBLEDeviceNameChangedHandler::EventDecReference(void)noexcept
{
	GetHost()->InnerDecReference('evet');
}
void cGATTPeripheral::cBLEDeviceNameChangedHandler::EventInvoke(IBLEDevice*, IInspectable*)noexcept
{
	auto Host=GetHost();
	Host->fNeedNotifyNameChange=true;
	Host->NotifyMainProcess();
}
//---------------------------------------------------------------------------
cGATTPeripheral* cGATTPeripheral::cBLEDeviceConnectionStatusHandler::GetHost(void)noexcept
{
	return cnMemory::GetObjectFromMemberPointer(this,&cGATTPeripheral::fBLEDeviceConnectionStatusHandler);
}
//---------------------------------------------------------------------------
void cGATTPeripheral::cBLEDeviceConnectionStatusHandler::EventIncReference(void)noexcept
{
	GetHost()->InnerIncReference('evet');
}
//---------------------------------------------------------------------------
void cGATTPeripheral::cBLEDeviceConnectionStatusHandler::EventDecReference(void)noexcept
{
	GetHost()->InnerDecReference('evet');
}
//---------------------------------------------------------------------------
void cGATTPeripheral::cBLEDeviceConnectionStatusHandler::EventInvoke(IBLEDevice*, IInspectable*)noexcept
{
	auto Host=GetHost();
	Host->fNeedNotifyConnectionStateChange=true;
	Host->NotifyMainProcess();
}
//---------------------------------------------------------------------------
cGATTPeripheral* cGATTPeripheral::cBLEDeviceServicesChangedHandler::GetHost(void)noexcept
{
	return cnMemory::GetObjectFromMemberPointer(this,&cGATTPeripheral::fBLEDeviceServicesChangedHandler);
}
//---------------------------------------------------------------------------
void cGATTPeripheral::cBLEDeviceServicesChangedHandler::EventIncReference(void)noexcept
{
	GetHost()->InnerIncReference('evet');
}
//---------------------------------------------------------------------------
void cGATTPeripheral::cBLEDeviceServicesChangedHandler::EventDecReference(void)noexcept
{
	GetHost()->InnerDecReference('evet');
}
//---------------------------------------------------------------------------
void cGATTPeripheral::cBLEDeviceServicesChangedHandler::EventInvoke(IBLEDevice*, IInspectable*)noexcept
{
	GetHost()->RefreshService();
}
//---------------------------------------------------------------------------
cGATTPeripheral* cGATTPeripheral::cAddressConnectCompleteHandler::GetHost(void)noexcept
{
	return cnMemory::GetObjectFromMemberPointer(this,&cGATTPeripheral::fAddressConnectCompleteHandler);
}
//---------------------------------------------------------------------------
void cGATTPeripheral::cAddressConnectCompleteHandler::EventIncReference(void)noexcept
{
	GetHost()->InnerIncReference('from');
}
void cGATTPeripheral::cAddressConnectCompleteHandler::EventDecReference(void)noexcept
{
	GetHost()->InnerDecReference('from');
}
void cGATTPeripheral::cAddressConnectCompleteHandler::EventInvoke(IBLEDeviceAsyncOp *AsyncOp,ABI::Windows::Foundation::AsyncStatus)noexcept
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
void cGATTPeripheral::CentralConnectionOperation(COMPtr<IBLEConnectAsyncOp> ConnectOp)noexcept
{
	fNeedNotifyConnectionStateChange=true;
	NotifyMainProcess();
	ConnectOp->put_Completed(&fAddressConnectCompleteHandler);
}
//---------------------------------------------------------------------------
void cGATTPeripheral::ServiceAttach(cGATTService *Service)noexcept
{
	fServiceMap[Service->ServiceUUID]=Service;
}
//---------------------------------------------------------------------------
void cGATTPeripheral::ServiceDetach(cGATTService *Service)noexcept
{
	fServiceMap.Remove(Service->ServiceUUID);
}
//---------------------------------------------------------------------------
void cGATTPeripheral::ServiceScan(cGATTService::cOpenServiceProcedure *OpenProc)noexcept
{
	fOpenServiceStack.Push(OpenProc);

	RefreshService();
}
//---------------------------------------------------------------------------
void cGATTPeripheral::RefreshService(void)noexcept
{
	if(!fQueryServiceExclusiveFlag.Acquire())
		return;

	RefreshServiceProc();
}
//---------------------------------------------------------------------------
void cGATTPeripheral::RefreshServiceProc(void)noexcept
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
void cGATTPeripheral::OnServiceResult(IBLEGetServiceAsyncOp *AsyncOp)noexcept
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
bool cGATTPeripheral::OnServiceResultProcess(IBLEGetServiceAsyncOp *AsyncOp)noexcept
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
void cGATTPeripheral::cMainProcessProcedure::Execute(void)noexcept
{
	auto Host=cnMemory::GetObjectFromMemberPointer(this,&cGATTPeripheral::fMainProcessProcedure);
	return Host->MainProcessThread();
}
//---------------------------------------------------------------------------
void cGATTPeripheral::MainProcessThread(void)noexcept
{
	do{
		fMainProcessExclusiveFlag.Continue();
		
		MainProcess();

	}while(!fMainProcessExclusiveFlag.Release());
}
//---------------------------------------------------------------------------
void cGATTPeripheral::NotifyMainProcess(void)noexcept
{
	if(!fMainProcessExclusiveFlag.Acquire())
		return;

	fMainProcessWork->Start();
}
//---------------------------------------------------------------------------
void cGATTPeripheral::MainProcess(void)noexcept
{

	HRESULT hr;
	if(fNeedNotifyConnectionStateChange){
		fNeedNotifyConnectionStateChange=false;

		eGATTFunctionState NewFuncState;
		if(fDevice==nullptr){
			if(fDeviceOpened){
				NewFuncState=GATTFunctionState::Absent;
			}
			else{
				NewFuncState=GATTFunctionState::Scanning;
			}
		}
		else{
			ABI::Windows::Devices::Bluetooth::BluetoothConnectionStatus NewStatus;
			hr=fDevice->get_ConnectionStatus(&NewStatus);
			if(FAILED(hr)){
				// error
				NewFuncState=GATTFunctionState::Absent;
			}
			else{
				switch(NewStatus){
				case ABI::Windows::Devices::Bluetooth::BluetoothConnectionStatus::BluetoothConnectionStatus_Connected:
					NewFuncState=GATTFunctionState::Active;
					break;
				default:
				case ABI::Windows::Devices::Bluetooth::BluetoothConnectionStatus::BluetoothConnectionStatus_Disconnected:
					NewFuncState=GATTFunctionState::Inactive;
					break;
				}
			}
		}


		if(fFuncState!=NewFuncState){
			fFuncState=NewFuncState;

			cSeqList< rInnerPtr<cGATTService> > ServiceListCache;
			for(auto Pair : fServiceMap){
				ServiceListCache.AppendMake(Pair.Value);
			}

			for(auto Service : ServiceListCache){
				Service->PeripheralNotifyConnectionStatus();
			}
			for(auto Handler : fHandlers){
				Handler->GATTPeripheralStateChanged();
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
				Handler->GATTPeripheralNameChanged();
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
cGATTPeripheral* cGATTPeripheral::cGetServicesCompleteHandler::GetHost(void)noexcept
{
	return cnMemory::GetObjectFromMemberPointer(this,&cGATTPeripheral::fGetServicesCompleteHandler);
}
//---------------------------------------------------------------------------
void cGATTPeripheral::cGetServicesCompleteHandler::EventIncReference(void)noexcept
{
	GetHost()->InnerIncReference('gets');
}
void cGATTPeripheral::cGetServicesCompleteHandler::EventDecReference(void)noexcept
{
	GetHost()->InnerDecReference('gets');
}
void cGATTPeripheral::cGetServicesCompleteHandler::EventInvoke(IBLEGetServiceAsyncOp *AsyncOp,ABI::Windows::Foundation::AsyncStatus)noexcept
{
	GetHost()->OnServiceResult(AsyncOp);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
#if 0

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
#endif // 0
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cGATTAdvertisementObserver::cGATTAdvertisementObserver(cGATTPeripheralCentral *Central,COMPtr<IBLEWatcher> Watcher)noexcept
	: fCentral(Central)
	, fWatcher(Watcher)
	, fCallback(nullptr)
{
	//fWatcher->add_Stopped;
	CreateQueueWork(fCentral->GetDispatch(),true);
}
//---------------------------------------------------------------------------
cGATTAdvertisementObserver::~cGATTAdvertisementObserver()noexcept
{
}
//---------------------------------------------------------------------------
void cGATTAdvertisementObserver::VirtualStarted(void)noexcept
{
	InnerActivate('self');


	fWatcher->add_Received(&fBLEReceivedHandler,&fBLEReceivedToken);
}
//---------------------------------------------------------------------------
void cGATTAdvertisementObserver::VirtualStopped(void)noexcept
{
	fWatcher->remove_Received(fBLEReceivedToken);

	InnerDecReference('self');
}
//---------------------------------------------------------------------------
bool cGATTAdvertisementObserver::StartNotify(iReference *Reference,iAsyncNotificationCallback *Callback)noexcept
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
void cGATTAdvertisementObserver::StopNotify(void)noexcept
{
	if(bcAsyncQueue::StopNotify()){
		UpdateQueueState(false);
	}
}
//---------------------------------------------------------------------------
void cGATTAdvertisementObserver::NotifyCallback(bool IdleNotify)noexcept
{
	return bcAsyncQueue::NotifyQueue(IdleNotify);
}
//---------------------------------------------------------------------------
bool cGATTAdvertisementObserver::IsClosed(void)noexcept
{
	return false;
}
//---------------------------------------------------------------------------
iDispatch* cGATTAdvertisementObserver::GetHandlerDispatch(void)noexcept
{
	return nullptr;
}
//---------------------------------------------------------------------------
iGATTPeripheralCentral* cGATTAdvertisementObserver::GetPeripheralCentral(void)noexcept
{
	return fCentral;
}
//---------------------------------------------------------------------------
void cGATTAdvertisementObserver::DiscardQueue(void)noexcept
{
}
//---------------------------------------------------------------------------
rPtr<iReference> cGATTAdvertisementObserver::Fetch(cGATTAdvertisementInfo &Info)noexcept
{
	auto pData=fAdvertisementQueue.Dequeue();
	if(pData==nullptr)
		return nullptr;

	Info=pData->Info;
	return rTake(pData,'queu');
}
//---------------------------------------------------------------------------
iReference* cGATTAdvertisementObserver::NotificationInnerReference(void)noexcept
{
	return &fInnerReference;
}
//---------------------------------------------------------------------------
void cGATTAdvertisementObserver::NotificationStarted(void)noexcept
{
	fCallback->AsyncStarted();
}
//---------------------------------------------------------------------------
void cGATTAdvertisementObserver::NotificationStopped(void)noexcept
{
	fCallback->AsyncStopped();

	fCallback=nullptr;
	fCallbackReference=nullptr;
}
//---------------------------------------------------------------------------
void cGATTAdvertisementObserver::AsyncQueueNotify(void)noexcept
{
	fCallback->AsyncNotify();
}
//---------------------------------------------------------------------------
void cGATTAdvertisementObserver::BLEReceived(rPtr<cAdvertisementData> AdvData)noexcept
{
	auto pData=rExtract(AdvData,'queu');
	fAdvertisementQueue.Enqueue(pData);

	AsyncQueueSetAvailable(true);
}
//---------------------------------------------------------------------------
cGATTAdvertisementObserver* cGATTAdvertisementObserver::cBLEReceivedHandler::GetHost(void)noexcept
{
	return cnMemory::GetObjectFromMemberPointer(this,&cGATTAdvertisementObserver::fBLEReceivedHandler);
}
//---------------------------------------------------------------------------
HRESULT STDMETHODCALLTYPE cGATTAdvertisementObserver::cBLEReceivedHandler::QueryInterface(REFIID riid,_COM_Outptr_ void __RPC_FAR *__RPC_FAR *ppvObject)noexcept
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
ULONG STDMETHODCALLTYPE cGATTAdvertisementObserver::cBLEReceivedHandler::AddRef(void)noexcept
{
	GetHost()->InnerIncReference('evet');
	return 0;
}
//---------------------------------------------------------------------------
ULONG STDMETHODCALLTYPE cGATTAdvertisementObserver::cBLEReceivedHandler::Release(void)noexcept
{
	GetHost()->InnerDecReference('evet');
	return 0;
}
//---------------------------------------------------------------------------
HRESULT STDMETHODCALLTYPE cGATTAdvertisementObserver::cBLEReceivedHandler::Invoke(IBLEWatcher*, _In_ ABI::Windows::Devices::Bluetooth::Advertisement::IBluetoothLEAdvertisementReceivedEventArgs *args)noexcept
{

	HRESULT hr;
	UINT64 BLEAddress;
	hr=args->get_BluetoothAddress(&BLEAddress);
	if(FAILED(hr)){
		return S_OK;
	}

	COMPtr<ABI::Windows::Devices::Bluetooth::Advertisement::IBluetoothLEAdvertisementReceivedEventArgs2> args2;
	hr=args->QueryInterface(COMRetPtrT(args2));

	ABI::Windows::Devices::Bluetooth::BluetoothAddressType AddressType=ABI::Windows::Devices::Bluetooth::BluetoothAddressType::BluetoothAddressType_Unspecified;
	hr=args2->get_BluetoothAddressType(&AddressType);
	
	boolean IsScanable;
	hr=args2->get_IsScannable(&IsScanable);

	boolean IsScanResponse;
	args2->get_IsScanResponse(&IsScanResponse);

	boolean IsConnectable;
	args2->get_IsConnectable(&IsConnectable);

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
			AdvData->Info.Timestamp=0;
		}
		else{
			AdvData->Timestamp=Timestamp.UniversalTime;
			AdvData->Info.Timestamp=AdvData->Timestamp;
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

	AdvData->Info.AddressType=(uInt8)AddressType;
	AdvData->Info.AdvertiserAddress=BLEAddress;
	AdvData->Info.LocalName=AdvData->LocalName.GetArray();
	AdvData->Info.Timestamp=AdvData->Timestamp;
	AdvData->Info.PeripheralAddress=AdvData->Address;
	AdvData->Info.ServiceUUIDs=AdvData->ServiceUUIDs.Storage();

	if(AdvData->Info.Type!=GATTAdvertisementType::ScanResponse){
		if(IsScanable==false || IsConnectable==false){
			return S_OK;
		}
	}
	
	GetHost()->BLEReceived(cnVar::MoveCast(AdvData));
	return S_OK;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cGATTPeripheralCentral::cGATTPeripheralCentral(iDispatch *Dispatch,COMPtr<IBLEDeviceStatics> BLEStatic)noexcept
	: fBLEStatic(cnVar::MoveCast(BLEStatic))
	, fDispatch(Dispatch)
{
}
//---------------------------------------------------------------------------
cGATTPeripheralCentral::IBLEDeviceStatics* cGATTPeripheralCentral::GetBLEStatic(void)const noexcept
{
	return fBLEStatic;
}
//---------------------------------------------------------------------------
iDispatch* cGATTPeripheralCentral::GetHandlerDispatch(void)noexcept
{
	return fDispatch;
}
//---------------------------------------------------------------------------
iDispatch* cGATTPeripheralCentral::GetDispatch(void)const noexcept
{
	return fDispatch;
}
//---------------------------------------------------------------------------
bool cGATTPeripheralCentral::InsertHandler(iGATTPeripheralCentralHandler *Handler)noexcept
{
	return false;
}
//---------------------------------------------------------------------------
bool cGATTPeripheralCentral::RemoveHandler(iGATTPeripheralCentralHandler *Handler)noexcept
{
	return false;
}
//---------------------------------------------------------------------------
rPtr<iGATTPeripheral> cGATTPeripheralCentral::OpenPeripheral(iAddress *Address)noexcept
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

		auto Peripheral=rCreate<cGATTPeripheral>(this,BLEAddress);

		Peripheral->CentralConnectionOperation(cnVar::MoveCast(ConnectOp));
		return Peripheral;
	}
	return nullptr;
}
//---------------------------------------------------------------------------
rPtr<iGATTAdvertisementObserver> cGATTPeripheralCentral::CreateAdvertisementObserver(void)noexcept
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

	return rCreate<cGATTAdvertisementObserver>(this,cnVar::MoveCast(BLEWatcher));
}
//---------------------------------------------------------------------------
bool cGATTPeripheralCentral::IsEnabled(void)noexcept
{
	return true;
}
//---------------------------------------------------------------------------
bool cGATTPeripheralCentral::IsPowerOn(void)noexcept
{
	return true;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
rPtr<iGATTPeripheralCentral> UWP::OpenGATTCentral(iDispatch *Dispatch)noexcept
{
	COMPtr<ABI::Windows::Devices::Bluetooth::IBluetoothLEDeviceStatics> BLEStatic;
	cHString RCName_BLEDevice=RuntimeClass_Windows_Devices_Bluetooth_BluetoothLEDevice;
	if(FAILED(Windows::Foundation::GetActivationFactory(RCName_BLEDevice,COMRetPtrT(BLEStatic)))){
		return nullptr;
	}
	return rCreate<cGATTPeripheralCentral>(Dispatch,cnVar::MoveCast(BLEStatic));
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
	if(fShutdown==false){
		fShutdown=true;
		NotifyMainProcess();
	}

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
eGATTFunctionState cGATTServerDescriptor::ServiceGetFunctionState(void)noexcept
{
	return fFuncState;
}
//---------------------------------------------------------------------------
iDispatch* cGATTServerDescriptor::GetHandlerDispatch(void)noexcept
{
	return fDispatch;
}
//---------------------------------------------------------------------------
eGATTFunctionState cGATTServerDescriptor::GetFunctionState(void)noexcept
{
	return fFuncState;
}
//---------------------------------------------------------------------------
void cGATTServerDescriptor::Shutdown(void)noexcept
{
	DescriptorShutdown();
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
void cGATTServerDescriptor::NotifyMainProcess(void)noexcept
{
	if(!fMainProcessExclusiveFlag.Acquire())
		return;

	fMainProcessWork->Start();
}
//---------------------------------------------------------------------------
bool cGATTServerDescriptor::MainProcess(void)noexcept
{
	HRESULT hr;
	if(fNeedNotifyFunctionStateChange){
		fNeedNotifyFunctionStateChange=false;

		eGATTFunctionState NewFuncState;
		if(fDescriptor==nullptr){
			if(fCreateProviderState==0){
				NewFuncState=GATTFunctionState::Absent;
			}
			else{
				NewFuncState=GATTFunctionState::Scanning;
			}
		}
		else{
			auto OwnerFuncState=fOwner->ServiceGetFunctionState();
			if(OwnerFuncState==GATTFunctionState::Active){
				NewFuncState=GATTFunctionState::Active;
			}
			else{
				NewFuncState=GATTFunctionState::Inactive;
			}
		}

		bool NotifyStateChange=fFuncState!=NewFuncState;
		fFuncState=NewFuncState;

		if(fController!=nullptr){
			if(NotifyStateChange){
				fController->DescriptorStateChanged();
			}
			if(fShutdown && fDescriptor==nullptr){
				fController->DescriptorClosed();
				fController=nullptr;
			}
		}
	}

	
	if(fCreateProviderState==2){
		fCreateProviderState=0;

		if(FAILED(fCreateLocalDescCompleteHandler.hrResult)){
			return false;
		}

		HRESULT hr;
		hr=fCreateLocalDescCompleteHandler.Result->get_Descriptor(COMRetPtrT(fDescriptor));
		if(FAILED(hr)){
			return false;
		}

		SetupDescriptor();


		fOwner->DescriptorNotifyTreeChanged();

		fNeedNotifyFunctionStateChange=true;
		return true;
	}

	if(fCreateProviderState==0 && fShutdown){
		if(fDescriptor!=nullptr){
			ClearDescriptor();
			fDescriptor=nullptr;

			fOwner->DescriptorNotifyClosed(this);
			fNeedNotifyFunctionStateChange=true;
			return true;
		}
	}
	return false;
}
//---------------------------------------------------------------------------
void cGATTServerDescriptor::DescriptorShutdown(void)noexcept
{
	if(fShutdown==false){
		fShutdown=true;
		NotifyMainProcess();
	}
}
//---------------------------------------------------------------------------
void cGATTServerDescriptor::CharacteristicNotifyStateChange(void)noexcept
{
	if(fCreateProviderState!=0)
		return;

	if(fDescriptor!=nullptr){
		fNeedNotifyFunctionStateChange=true;
		return NotifyMainProcess();
	}

	if(fShutdown){
		// do not create descriptor after shutdown
		// function state will not change
		return;
	}

	auto OwnerCharacteristic=fOwner->GetLocalCharacteristic();
	if(OwnerCharacteristic==nullptr){
		return;
	}
	HRESULT hr;
	
	// look for existing descriptor
	COMPtr< ABI::Windows::Foundation::Collections::IVectorView<GattLocalDescriptor*> > Descriptors;
	hr=OwnerCharacteristic->get_Descriptors(COMRetPtrT(Descriptors));
	if(FAILED(hr))
		return;

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

			fNeedNotifyFunctionStateChange=true;
			NotifyMainProcess();
			return;
		}
	}

	// create new descriptor

	COMPtr< ABI::Windows::Foundation::IAsyncOperation<GattLocalDescriptorResult*> > AsyncOp;

	COMPtr<IGattLocalDescriptorParameters> Parameters;
	cHString RCName_Parameters=RuntimeClass_Windows_Devices_Bluetooth_GenericAttributeProfile_GattLocalDescriptorParameters;
	hr=Windows::Foundation::ActivateInstance(RCName_Parameters,COMRetPtrT(Parameters));
	if(FAILED(hr)){
		return;
	}

	//Parameters->put_UserDescription(ABI::Windows::Devices::Bluetooth::GenericAttributeProfile::GattProtectionLevel_Plain);
	Parameters->put_ReadProtectionLevel(ABI::Windows::Devices::Bluetooth::GenericAttributeProfile::GattProtectionLevel_Plain);
	Parameters->put_WriteProtectionLevel(ABI::Windows::Devices::Bluetooth::GenericAttributeProfile::GattProtectionLevel_Plain);

	hr=OwnerCharacteristic->CreateDescriptorAsync(reinterpret_cast<const GUID&>(fDescriptorID),Parameters,COMRetPtrT(AsyncOp));
	if(FAILED(hr)){
		return;
	}

	fCreateProviderState=1;
	AsyncOp->put_Completed(&fCreateLocalDescCompleteHandler);
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
	if(Status!=AsyncStatus::Completed){
		hrResult=E_FAIL;
		Host->fCreateProviderState=2;
		Host->NotifyMainProcess();
		return;
	}

	hrResult=AsyncOp->GetResults(COMRetPtrT(Result));
	Host->fCreateProviderState=2;
	Host->NotifyMainProcess();
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cGATTServerCharacteristic::cClientSubscription* cGATTServerCharacteristic::cClientSubscription::cClientNotifyValueCompleteHandler::GetHost(void)noexcept
{
	return cnMemory::GetObjectFromMemberPointer(this,&cClientSubscription::ClientNotifyValueCompleteHandler);
}
//---------------------------------------------------------------------------
void cGATTServerCharacteristic::cClientSubscription::cClientNotifyValueCompleteHandler::EventIncReference(void)noexcept
{
	rIncReference(GetHost(),'snot');
}
//---------------------------------------------------------------------------
void cGATTServerCharacteristic::cClientSubscription::cClientNotifyValueCompleteHandler::EventDecReference(void)noexcept
{
	rDecReference(GetHost(),'snot');
}
//---------------------------------------------------------------------------
void cGATTServerCharacteristic::cClientSubscription::cClientNotifyValueCompleteHandler::EventInvoke(IClientNotifyValueAsyncOp *AsyncOp,ABI::Windows::Foundation::AsyncStatus)noexcept
{
	COMPtr<IGattClientNotificationResult> Result;
	HRESULT hr;
	auto Host=GetHost();
	auto Owner=Host->Owner;
	
	hr=AsyncOp->GetResults(COMRetPtrT(Result));
	if(FAILED(hr)){
		// notify error
		auto Host=GetHost();
		Host->UpdateValueState=vnIdle;
		Owner->NotifyMainProcess();
		return;
	}

	GattCommunicationStatus Status;
	hr=Result->get_Status(&Status);
	if(FAILED(hr)){
		// notify error
		auto Host=GetHost();
		Host->UpdateValueState=vnIdle;
		Owner->NotifyMainProcess();
		return;
	}

	Host->UpdateValueState=vnCommand;
	Owner->fValueNotifyUpdate=true;
	Owner->NotifyMainProcess();
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
	InnerActivate('self');
}
//---------------------------------------------------------------------------
void cGATTServerCharacteristic::VirtualStopped(void)noexcept
{
	CharacteristicShutdown();
	InnerDecReference('self');
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
eGATTFunctionState cGATTServerCharacteristic::ServiceGetFunctionState(void)noexcept
{
	return fFuncState;
}
//---------------------------------------------------------------------------
iDispatch* cGATTServerCharacteristic::GetHandlerDispatch(void)noexcept
{
	return fDispatch;
}
//---------------------------------------------------------------------------
eGATTFunctionState cGATTServerCharacteristic::GetFunctionState(void)noexcept
{
	return fFuncState;
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
	
	NewDesc->CharacteristicNotifyStateChange();
	return NewDesc;
}
//---------------------------------------------------------------------------
void cGATTServerCharacteristic::Shutdown(void)noexcept
{
	CharacteristicShutdown();
}
//---------------------------------------------------------------------------
iGATTClientSubscription* cGATTServerCharacteristic::QuerySubscription(iGATTClientConnection *Connection)noexcept
{
	if(fDispatch->IsCurrentThread()==false)
		return nullptr;

	auto ClientConn=static_cast<cClientConnection*>(Connection);
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
void cGATTServerCharacteristic::NotifyValue(iGATTClientSubscription *Subscription)noexcept
{
	if(Subscription==nullptr){
		fBroadcastValueNotify=true;
	}
	else{
		auto ClientSubscription=static_cast<cClientSubscription*>(Subscription);
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
void cGATTServerCharacteristic::NotifyMainProcess(void)noexcept
{
	if(!fMainProcessExclusiveFlag.Acquire())
		return;

	fMainProcessWork->Start();
}
//---------------------------------------------------------------------------
bool cGATTServerCharacteristic::MainProcess(void)noexcept
{

	HRESULT hr;
	if(fNeedNotifyFunctionStateChange){
		fNeedNotifyFunctionStateChange=false;

		eGATTFunctionState NewFuncState;
		if(fCharacteristic==nullptr){
			if(fCreateProviderState==0){
				NewFuncState=GATTFunctionState::Absent;
			}
			else{
				NewFuncState=GATTFunctionState::Scanning;
			}
		}
		else{
			auto OwnerFuncState=fOwner->ServiceGetFunctionState();
			if(OwnerFuncState==GATTFunctionState::Active){
				NewFuncState=GATTFunctionState::Active;
			}
			else{
				NewFuncState=GATTFunctionState::Inactive;
			}
		}

		bool NotifyStateChang=fFuncState!=NewFuncState;
		if(NotifyStateChang){
			fFuncState=NewFuncState;

			for(auto Pair : fDescriptorMap){
				Pair.Value->CharacteristicNotifyStateChange();
			}

		}
		if(fController!=nullptr){
			if(NotifyStateChang){
				fController->CharacteristicStateChanged();
			}
			if(fShutdown && fCharacteristic==nullptr){
				fController->CharacteristicClosed();
				fController=nullptr;
			}
		}
		
	}

	
	if(fCreateProviderState==2){
		fCreateProviderState=0;

		if(FAILED(fCreateLocalCharCompleteHandler.hrResult)){
			return false;
		}

		hr=fCreateLocalCharCompleteHandler.Result->get_Characteristic(COMRetPtrT(fCharacteristic));
		if(FAILED(hr)){
			return false;
		}

		SetupCharacteristic();

		fNeedNotifyFunctionStateChange=true;
		return true;
	}

	if(fCreateProviderState==0 && fShutdown){
		if(fDescriptorMap.GetCount()==0){
			if(fCharacteristic!=nullptr){
				ClearCharacteristic();
				fCharacteristic=nullptr;

				fOwner->CharacteristicNotifyClosed(this);
				fNeedNotifyFunctionStateChange=true;
				return true;
			}
		}
		else{
			for(auto DescPair : fDescriptorMap){
				DescPair.Value->DescriptorShutdown();
			}
		}

		return false;
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
				auto NewClient=rCreate<cClientSubscription>();
				NewClient->Owner=this;
				NewClient->Client=cnVar::MoveCast(Client);
				cClientSubscription *pNewClient=NewClient;
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
			cClientSubscription *sc=p.Value;
			MainProcess_ValueNotify(sc,DataWriter);
		}

	}

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
void cGATTServerCharacteristic::MainProcess_ValueNotify(cClientSubscription *Subscription,ABI::Windows::Storage::Streams::IDataWriter *DataWriter)noexcept
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
	if(fController==nullptr || fController->ValueNotify(Subscription,&Buffer)==false){
		COMPtr<ABI::Windows::Storage::Streams::IBuffer> DataBuffer;
		hr=DataWriter->DetachBuffer(COMRetPtrT(DataBuffer));
		return;
	}
	COMPtr<ABI::Windows::Storage::Streams::IBuffer> DataBuffer;
	hr=DataWriter->DetachBuffer(COMRetPtrT(DataBuffer));

	COMPtr<IClientNotifyValueAsyncOp> AsyncOp;
	hr=fCharacteristic->NotifyValueForSubscribedClientAsync(DataBuffer,Subscription->Client,COMRetPtrT(AsyncOp));

	Subscription->UpdateValueState=vnAwait;
	hr=AsyncOp->put_Completed(&Subscription->ClientNotifyValueCompleteHandler);

}
//---------------------------------------------------------------------------
void cGATTServerCharacteristic::DescriptorNotifyClosed(cGATTServerDescriptor *Descriptor)noexcept
{
	if(fDescriptorMap.Remove(Descriptor->GetID())){
		if(fShutdown){
			if(fDescriptorMap.GetCount()==0){
				if(fCharacteristic!=nullptr){
					ClearCharacteristic();
					fCharacteristic=nullptr;

					fOwner->CharacteristicNotifyClosed(this);
					fNeedNotifyFunctionStateChange=true;
					NotifyMainProcess();
				}
			}
		}
	}
}
//---------------------------------------------------------------------------
void cGATTServerCharacteristic::DescriptorNotifyTreeChanged(void)noexcept
{
	fOwner->CharacteristicNotifyTreeChanged();
}
//---------------------------------------------------------------------------
void cGATTServerCharacteristic::CharacteristicShutdown(void)noexcept
{
	if(fShutdown==false){
		fShutdown=true;
		NotifyMainProcess();
	}
}
//---------------------------------------------------------------------------
void cGATTServerCharacteristic::ServiceNotifyStateChange(void)noexcept
{
	if(fCreateProviderState!=0 || fController==nullptr)
		return;

	if(fCharacteristic!=nullptr){
		fNeedNotifyFunctionStateChange=true;
		return NotifyMainProcess();
	}

	if(fOwner->ServiceGetFunctionState()<GATTFunctionState::Inactive)
		return;

	auto OwnerService=fOwner->GetLocalService();
	if(OwnerService==nullptr){
		return;
	}

	HRESULT hr;
	
	// look for existing descriptor
	COMPtr< ABI::Windows::Foundation::Collections::IVectorView<GattLocalCharacteristic*> > Characteristics;
	hr=OwnerService->get_Characteristics(COMRetPtrT(Characteristics));
	if(FAILED(hr))
		return;

	unsigned int CharacteristicCount;
	hr=Characteristics->get_Size(&CharacteristicCount);

	for(unsigned int i=0;i<CharacteristicCount;i++){
		COMPtr<IGattLocalCharacteristic> Char;
		hr=Characteristics->GetAt(i,COMRetPtrT(Char));

		GUID CharID;
		hr=Char->get_Uuid(&CharID);
		if(CharID==reinterpret_cast<GUID&>(fCharacteristicID)){
			fCharacteristic=cnVar::MoveCast(Char);
		
			SetupCharacteristic();

			fNeedNotifyFunctionStateChange=true;
			NotifyMainProcess();
			return;
		}
	}

	COMPtr< ABI::Windows::Foundation::IAsyncOperation<GattLocalCharacteristicResult*> > AsyncOp;

	COMPtr<IGattLocalCharacteristicParameters> Parameters;
	cHString RCName_Parameters=RuntimeClass_Windows_Devices_Bluetooth_GenericAttributeProfile_GattLocalCharacteristicParameters;
	hr=Windows::Foundation::ActivateInstance(RCName_Parameters,COMRetPtrT(Parameters));
	if(FAILED(hr)){
		return;
	}

	auto Parameter=fController->GetCharacteristicParameter();
	
	typedef ABI::Windows::Devices::Bluetooth::GenericAttributeProfile::GattCharacteristicProperties GattCharacteristicProperties;
	Parameters->put_CharacteristicProperties(static_cast<GattCharacteristicProperties>(Parameter.Properties));

	//Parameters->put_UserDescription(ABI::Windows::Devices::Bluetooth::GenericAttributeProfile::GattProtectionLevel_Plain);
	Parameters->put_ReadProtectionLevel(ABI::Windows::Devices::Bluetooth::GenericAttributeProfile::GattProtectionLevel_Plain);
	Parameters->put_WriteProtectionLevel(ABI::Windows::Devices::Bluetooth::GenericAttributeProfile::GattProtectionLevel_Plain);

	hr=OwnerService->CreateCharacteristicAsync(reinterpret_cast<const GUID&>(fCharacteristicID),Parameters,COMRetPtrT(AsyncOp));
	if(FAILED(hr)){
		return;
	}

	fCreateProviderState=1;
	AsyncOp->put_Completed(&fCreateLocalCharCompleteHandler);
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
	if(Status!=AsyncStatus::Completed){
		hrResult=E_FAIL;
		Host->fCreateProviderState=2;
		Host->NotifyMainProcess();
		return;
	}

	hrResult=AsyncOp->GetResults(COMRetPtrT(Result));
	Host->fCreateProviderState=2;
	Host->NotifyMainProcess();
}
//---------------------------------------------------------------------------
rPtr<cGATTServerCharacteristic::cClientConnection> cGATTServerCharacteristic::QueryClient(IGattSession *Session)noexcept
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

	auto Connection=rCreate<cClientConnection>();
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
	ServiceShutdown();
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
eGATTFunctionState cGATTServerService::ServiceGetFunctionState(void)noexcept
{
	return fFuncState;
}
//---------------------------------------------------------------------------
iDispatch* cGATTServerService::GetHandlerDispatch(void)noexcept
{
	return fDispatch;
}
//---------------------------------------------------------------------------
eGATTFunctionState cGATTServerService::GetFunctionState(void)noexcept
{
	return fFuncState;
}
//---------------------------------------------------------------------------
rPtr<iGATTServerCharacteristic> cGATTServerService::CreateGATTCharacteristic(const cUUID &ID,iReference *Reference,iGATTCharacteristicController *Controller)noexcept
{
	if(fShutdown || fDispatch->IsCurrentThread()==false)
		return nullptr;

	bool Exists;
	auto CharPair=fCharacteristicMap.InsertPair(ID,Exists);
	if(Exists){
		return nullptr;
	}

	auto NewChar=rCreate<cGATTServerCharacteristic>(this,ID,Reference,Controller);
	CharPair->Value=NewChar;
	
	NewChar->ServiceNotifyStateChange();
	return NewChar;
}
//---------------------------------------------------------------------------
void cGATTServerService::Shutdown(void)noexcept
{
	if(fShutdown==false){
		fShutdown=true;

		NotifyMainProcess();
	}
}
//---------------------------------------------------------------------------
bool cGATTServerService::IsAdvertising(void)noexcept
{
	return fAdvertisementActivated;
}
//---------------------------------------------------------------------------
bool cGATTServerService::GetAdvertisementIncluded(void)noexcept
{
	return fAdvertisementIncluded;
}
//---------------------------------------------------------------------------
void cGATTServerService::SetAdvertisementIncluded(bool Included)noexcept
{
	fAdvertisementIncluded=Included;
	fNeedUpdateAdvertisement=true;

	NotifyMainProcess();
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
void cGATTServerService::NotifyMainProcess(void)noexcept
{
	if(!fMainProcessExclusiveFlag.Acquire())
		return;

	fMainProcessWork->Start();
}
//---------------------------------------------------------------------------
bool cGATTServerService::MainProcess(void)noexcept
{

	HRESULT hr;
	if(fNeedNotifyFunctionStateChange){
		fNeedNotifyFunctionStateChange=false;

		eGATTFunctionState NewFuncState;
		if(fService==nullptr){
			if(fCreateProviderState==0){
				NewFuncState=GATTFunctionState::Absent;
			}
			else{
				NewFuncState=GATTFunctionState::Scanning;
			}
		}
		else{
			auto OwnerFuncState=fOwner->ServerGetFunctionState();
			if(OwnerFuncState==GATTFunctionState::Active){
				NewFuncState=GATTFunctionState::Active;
			}
			else{
				NewFuncState=GATTFunctionState::Inactive;
			}
		}

		bool NotifyStateChanged=fFuncState!=NewFuncState;
		if(NotifyStateChanged){
			fFuncState=NewFuncState;

			for(auto Pair : fCharacteristicMap){
				Pair.Value->ServiceNotifyStateChange();
			}
		}


		if(fController!=nullptr){
			if(NotifyStateChanged){
				fController->ServiceStateChanged();
			}
			if(fShutdown && fService==nullptr){
				fController->ServiceClosed();
				fController=nullptr;
			}
		}
	}

	
	if(fCreateProviderState==2){
		fCreateProviderState=0;

		if(FAILED(fCreateServiceProviderCompleteHandler.hrResult)){
			return false;
		}

		HRESULT hr;
		hr=fCreateServiceProviderCompleteHandler.Result->get_ServiceProvider(COMRetPtrT(fServiceProvider));
		if(FAILED(hr)){
			return false;
		}
		hr=fServiceProvider->get_Service(COMRetPtrT(fService));

		fAdvertisementActivated=false;
		hr=fServiceProvider->add_AdvertisementStatusChanged(&fAdvertisementStatusChangedHandler,&fAdvertisementStatusChangedToken);

		fOwner->ServiceNotifyCreated(this);

		fNeedNotifyFunctionStateChange=true;
		return true;
	}

	if(fCreateProviderState==0 && fShutdown){
		if(fCharacteristicMap.GetCount()==0){
			if(fServiceProvider!=nullptr){
				fService=nullptr;

				fServiceProvider->remove_AdvertisementStatusChanged(fAdvertisementStatusChangedToken);
				fServiceProvider=nullptr;

				fOwner->ServiceNotifyClosed(this);
				fNeedNotifyFunctionStateChange=true;
				return true;
			}
		}
		else{
			for(auto CharPair : fCharacteristicMap){
				CharPair.Value->CharacteristicShutdown();
			}
		}
	}

	if(fNeedUpdateAdvertisement){
		if(fServiceProvider!=nullptr){
			fNeedUpdateAdvertisement=false;
			ProcessAdvertisement();
		}
	}

	return false;
}
//---------------------------------------------------------------------------
void cGATTServerService::ProcessAdvertisement(void)noexcept
{
	HRESULT hr;

	bool ServerAdvertisementActive=fOwner->ServerAdvertisementIsActive();

	bool NeedAdvertisement=fAdvertisementIncluded && ServerAdvertisementActive;

	if(fAdvertisementActivated){
		if(fAdvertisementRequireRestart || NeedAdvertisement==false){
			// stop advertisement
			hr=fServiceProvider->StopAdvertising();
			if(SUCCEEDED(hr)){
				// BUG in UWP BLE: advertisement status not changed after stop
				fAdvertisementActivated=false;
			}
		}
	}
	else{
		fAdvertisementRequireRestart=false;
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

	if(fNotifiedIsAdvertising!=fAdvertisementActivated){
		fNotifiedIsAdvertising=fAdvertisementActivated;

		if(fController!=nullptr){
			fController->ServiceAdvertisementChanged();
		}
	}


}
//---------------------------------------------------------------------------
void cGATTServerService::CharacteristicNotifyClosed(cGATTServerCharacteristic *Characteristic)noexcept
{
	if(fCharacteristicMap.Remove(Characteristic->GetID())){
		if(fShutdown){
			if(fCharacteristicMap.GetCount()==0){
				if(fServiceProvider!=nullptr){
					fService=nullptr;
					fServiceProvider=nullptr;

					fOwner->ServiceNotifyClosed(this);
					fNeedNotifyFunctionStateChange=true;
					NotifyMainProcess();
				}
			}
		}
	}
}
//---------------------------------------------------------------------------
void cGATTServerService::CharacteristicNotifyTreeChanged(void)noexcept
{
	fAdvertisementRequireRestart=true;

	ProcessAdvertisement();
}
//---------------------------------------------------------------------------
void cGATTServerService::ServiceShutdown(void)noexcept
{
	if(fShutdown==false){
		fShutdown=true;

		NotifyMainProcess();
	}
}
//---------------------------------------------------------------------------
void cGATTServerService::ServerNotifyStateChange(void)noexcept
{
	if(fCreateProviderState!=0)
		return;

	if(fServiceProvider!=nullptr){
		fNeedNotifyFunctionStateChange=true;
		return NotifyMainProcess();
	}

	if(fOwner->ServerGetFunctionState()>=GATTFunctionState::Inactive){
		CreateServiceProvider();
	}
}
//---------------------------------------------------------------------------
void cGATTServerService::CreateServiceProvider(void)noexcept
{
	HRESULT hr;
	auto Statics=fOwner->GetProviderStatic();
	COMPtr< ABI::Windows::Foundation::IAsyncOperation<GattServiceProviderResult*> > AsyncOp;
	hr=Statics->CreateAsync(reinterpret_cast<const GUID&>(fServiceID),COMRetPtrT(AsyncOp));
	if(FAILED(hr)){
		return;
	}

	fCreateProviderState=1;
	AsyncOp->put_Completed(&fCreateServiceProviderCompleteHandler);
}
//---------------------------------------------------------------------------
void cGATTServerService::ServerNotifyAdvertisementState(void)noexcept
{
	ProcessAdvertisement();
}
//---------------------------------------------------------------------------
void cGATTServerService::ServerActivate(IGattServiceProvider *Provider)noexcept
{
	if(fShutdown || fCreateProviderState!=0)
		return;
	HRESULT hr;

	if(Provider!=nullptr){
		fServiceProvider=Provider;
	
		hr=fServiceProvider->get_Service(COMRetPtrT(fService));

		fNeedNotifyFunctionStateChange=true;
		NotifyMainProcess();
		return;
	}

	CreateServiceProvider();
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
	if(Status!=AsyncStatus::Completed){
		hrResult=E_FAIL;
		Host->fCreateProviderState=2;
		Host->NotifyMainProcess();
		return;
	}

	hrResult=AsyncOp->GetResults(COMRetPtrT(Result));
	Host->fCreateProviderState=2;
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
	Host->fNeedUpdateAdvertisement=true;
	Host->NotifyMainProcess();
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cGATTServer::cGATTServer(iDispatch *Dispatch,COMPtr<IGattServiceProviderStatics> ProviderStatic)noexcept
	: fProviderStatic(ProviderStatic)
	, fDispatch(Dispatch)
	, fFunctionState(GATTFunctionState::Active)
	, fMainProcessWork(fDispatch->CreateWork(&fInnerReference,&fMainProcessProcedure))
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
	ServerShutdown();
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
eGATTFunctionState cGATTServer::ServerGetFunctionState(void)noexcept
{
	return fFunctionState;
}
//---------------------------------------------------------------------------
eGATTFunctionState cGATTServer::GetFunctionState(void)noexcept
{
	return fFunctionState;
}
//---------------------------------------------------------------------------
bool cGATTServer::InsertHandler(iGATTServerHandler *Handler)noexcept
{
	if(fDispatch->IsCurrentThread()==false)
		return false;
	fHandlers.Insert(Handler);
	return true;
}
//---------------------------------------------------------------------------
bool cGATTServer::RemoveHandler(iGATTServerHandler *Handler)noexcept
{
	if(fDispatch->IsCurrentThread()==false)
		return false;
	fHandlers.Remove(Handler);
	return true;
}
//---------------------------------------------------------------------------
rPtr<iGATTServerService> cGATTServer::CreateGATTService(const cUUID &ID,iReference *Reference,iGATTServiceController *Controller)noexcept
{
	if(fShutdown || fDispatch->IsCurrentThread()==false)
		return nullptr;

	bool Exists;
	auto ServicePair=fServiceMap.InsertPair(ID,Exists);
	if(Exists){
		return nullptr;
	}

	auto NewService=rCreate<cGATTServerService>(this,ID,Reference,Controller);
	ServicePair->Value=NewService;

	auto ProviderPair=fProviderMap.GetPair(ID);
	if(ProviderPair!=nullptr){
		NewService->ServerActivate(ProviderPair->Value);
	}
	else{
		NewService->ServerActivate(nullptr);
	}

	return NewService;
}
//---------------------------------------------------------------------------
void cGATTServer::Shutdown(void)noexcept
{
	ServerShutdown();
}
//---------------------------------------------------------------------------
bool cGATTServer::GetAdvertisementActive(void)noexcept
{
	return fAdvertisementActive;
}
//---------------------------------------------------------------------------
void cGATTServer::SetAdvertisementActive(bool Active)noexcept
{
	fAdvertisementActive=Active;

	NotifyMainProcess();
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
void cGATTServer::NotifyMainProcess(void)noexcept
{
	if(!fMainProcessExclusiveFlag.Acquire())
		return;

	fMainProcessWork->Start();
}
//---------------------------------------------------------------------------
bool cGATTServer::MainProcess(void)noexcept
{
	HRESULT hr;
	if(fNeedNotifyFunctionStateChange){
		fNeedNotifyFunctionStateChange=false;

		eGATTFunctionState NewFuncState;
		if(fProviderStatic==nullptr){
			NewFuncState=GATTFunctionState::Absent;
		}
		else{
			NewFuncState=GATTFunctionState::Active;
		}


		if(fFunctionState!=NewFuncState){
			fFunctionState=NewFuncState;

			for(auto Pair : fServiceMap){
				if(Pair.Value!=nullptr){
					Pair.Value->ServerNotifyStateChange();
				}
			}

			for(auto Handler : fHandlers){
				Handler->ServerStateChanged();
			}
		}
	}
	if(fEffectiveAdvertisementActive!=fAdvertisementActive){
		fEffectiveAdvertisementActive=fAdvertisementActive;

		for(auto CharPair : fServiceMap){
			CharPair.Value->ServerNotifyAdvertisementState();
		}

	}
	
	if(fShutdown){
		if(fServiceMap.GetCount()==0){
			if(fProviderStatic!=nullptr){
				fProviderStatic=nullptr;

				for(auto Handler : fHandlers){
					Handler->ServerClosed();
				}
				fNeedNotifyFunctionStateChange=true;
				return true;
			}
		}
		else{
			for(auto CharPair : fServiceMap){
				CharPair.Value->ServiceShutdown();
			}
		}
	}

	return false;
}
//---------------------------------------------------------------------------
void cGATTServer::ServerShutdown(void)noexcept
{
	if(fShutdown==false){
		fShutdown=true;

		NotifyMainProcess();
	}
}
//---------------------------------------------------------------------------
bool cGATTServer::ServerAdvertisementIsActive(void)const noexcept
{
	return fEffectiveAdvertisementActive;
}
//---------------------------------------------------------------------------
void cGATTServer::ServiceNotifyCreated(cGATTServerService *Service)noexcept
{
	fProviderMap.Insert(Service->GetID(),Service->GetServiceProvider());
}
//---------------------------------------------------------------------------
void cGATTServer::ServiceNotifyClosed(cGATTServerService *Service)noexcept
{
	if(fServiceMap.Remove(Service->GetID())){
		if(fShutdown){
			if(fServiceMap.GetCount()==0){
				if(fProviderStatic!=nullptr){
					fProviderStatic=nullptr;

					for(auto Handler : fHandlers){
						Handler->ServerClosed();
					}
					fNeedNotifyFunctionStateChange=true;
					NotifyMainProcess();
				}
			}
		}
	}
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
rPtr<iGATTServer> UWP::CreateGATTServer(iDispatch *Dispatch)noexcept
{
	COMPtr<ABI::Windows::Devices::Bluetooth::GenericAttributeProfile::IGattServiceProviderStatics> ProviderStatics;
	cHString RCName_Provider=RuntimeClass_Windows_Devices_Bluetooth_GenericAttributeProfile_GattServiceProvider;
	if(FAILED(Windows::Foundation::GetActivationFactory(RCName_Provider,COMRetPtrT(ProviderStatics)))){
		return nullptr;
	}

	return rCreate<cGATTServer>(Dispatch,cnVar::MoveCast(ProviderStatics));
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
