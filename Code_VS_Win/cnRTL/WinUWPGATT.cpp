#include <stdio.h>
#include "WinUWPGATT.h"

#pragma comment(lib,"WindowsApp.lib")


using namespace cnLibrary;
using namespace cnRTL;
using namespace cnWinRTL;

#if _WIN32_WINNT >= _WIN32_WINNT_WIN10

using namespace cnRTL::UWP;


cnLib_INTERFACE_LOCALID_DEFINE(cBluetoothAddress);

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
cGATTDescriptor::cGATTDescriptor(cGATTCharacteristic *Owner,const cGATTUUID &ID)noexcept
	: DescriptorUUID(ID)
	, fOwner(Owner)
	, fDispatch(Owner->GetDispatch())
	, fFuncState(GATTFunctionState::Absent)
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
cGATTUUID cGATTDescriptor::GetUUID(void)noexcept
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
cGATTCharacteristic::cGATTCharacteristic(cGATTService *Owner,const cGATTUUID &ID)noexcept
	: CharacteristcUUID(ID)
	, fOwner(Owner)
	, fDispatch(Owner->GetDispatch())
	, fMainProcessWork(Owner->GetDispatch()->CreateWork(&fInnerReference,&fMainProcessProcedure))
	, fFuncState(GATTFunctionState::Absent)
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
cGATTUUID cGATTCharacteristic::GetUUID(void)noexcept
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
rPtr<iGATTDescriptor> cGATTCharacteristic::AccessDescriptor(const cGATTUUID &ID)noexcept
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
rPtr< iArrayReference<const void> > cGATTCharacteristic::Read(void)noexcept
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
	COMPtr<IBLEWriteAsyncOp> WriteOp;
	hr=fCharacteristic->WriteValueAsync(SendBuffer,COMRetPtrT(WriteOp));

	hr=WriteOp->put_Completed(&fWriteCompleteHandler);
	return nullptr;
}
//---------------------------------------------------------------------------
bool cGATTCharacteristic::WriteWithoutResponse(const void *Data,uIntn DataSize)noexcept
{

	//WriteOp->put_Completed();

	return false;
}
//---------------------------------------------------------------------------
iPtr< iAsyncFunction<bool> > cGATTCharacteristic::ConfigureNotification(eGATTCharacteristicNotification Notification)noexcept
{
	auto Task=iCreate<cConfigureNotificationTask>(this);

	auto NotifyConfigValue=static_cast<ABI::Windows::Devices::Bluetooth::GenericAttributeProfile::GattClientCharacteristicConfigurationDescriptorValue>(Notification);
	if(Task->Config(NotifyConfigValue)==false)
		return nullptr;

	return Task;
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
				printf("GATTChar %p test equal, discard\n",this);
				return;
			}
		}
		ClearCharacteristic();
	}

	fCharacteristic=Characteristic;
	printf("GATTChar %p replace char object\n",this);

	if(fCharacteristic!=nullptr){
		SetupCharacteristic();
	}

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
		MainProcess_Idle();
		return false;
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
}
//---------------------------------------------------------------------------
void cGATTCharacteristic::MainProcess_Idle(void)noexcept
{
	HRESULT hr;


	if(fDescriptorNeedRefresh){
		fDescriptorNeedRefresh=false;

		COMPtr<IBLEGetDescriptorsAsyncOp> CharOp;
		hr=fCharacteristic3->GetDescriptorsAsync(COMRetPtrT(CharOp));
		if(FAILED(hr)){
			return;
		}
		fMainProcessState=psRefreshDescriptor;

		for(auto Pair : fDescriptorMap){
			Pair.Value->CharacteristicNotifyFunctionStatus();
		}

		hr=CharOp->put_Completed(&fGetDescriptorsCompleteHandler);

		return;
	}

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

			auto Pair=fUpdateMap.GetPair(reinterpret_cast<cGATTUUID&>(IDValue));
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
	HRESULT hr=args->get_CharacteristicValue(COMRetPtrT(Buffer));

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
void cGATTCharacteristic::cWriteCompleteHandler::EventInvoke(IBLEWriteAsyncOp *AsyncOp,ABI::Windows::Foundation::AsyncStatus)noexcept
{
	GattCommunicationStatus Result=GattCommunicationStatus::GattCommunicationStatus_ProtocolError;
	HRESULT hr=AsyncOp->GetResults(&Result);
}
//---------------------------------------------------------------------------
cGATTCharacteristic::cConfigureNotificationTask::cConfigureNotificationTask(cGATTCharacteristic *Owner)noexcept
	: fOwner(Owner)
{
}
//---------------------------------------------------------------------------
bool cGATTCharacteristic::cConfigureNotificationTask::Config(ABI::Windows::Devices::Bluetooth::GenericAttributeProfile::GattClientCharacteristicConfigurationDescriptorValue DescValue)noexcept
{
	HRESULT hr;
	COMPtr<IBLEWriteAsyncOp> WriteOp;
	hr=fOwner->fCharacteristic->WriteClientCharacteristicConfigurationDescriptorAsync(DescValue,COMRetPtrT(WriteOp));
	if(FAILED(hr))
		return false;

	hr=WriteOp->put_Completed(&fConfigureNotificationHandler);
	if(FAILED(hr))
		return false;

	return true;
}
//---------------------------------------------------------------------------
bool cGATTCharacteristic::cConfigureNotificationTask::IsDone(void)noexcept
{
	return fTaskState.IsDone();
}
//---------------------------------------------------------------------------
bool cGATTCharacteristic::cConfigureNotificationTask::SetNotify(iProcedure *NotifyProcedure)noexcept
{
	return fTaskState.SetNotify(NotifyProcedure);
}
//---------------------------------------------------------------------------
void cGATTCharacteristic::cConfigureNotificationTask::Cancel(void)noexcept
{
}
//---------------------------------------------------------------------------
bool cGATTCharacteristic::cConfigureNotificationTask::GetResult(void)noexcept
{
	return fResult==GattCommunicationStatus::GattCommunicationStatus_Success;
}
//---------------------------------------------------------------------------
void cGATTCharacteristic::cConfigureNotificationTask::ConfigureDone(IBLEWriteAsyncOp *AsyncOp,ABI::Windows::Foundation::AsyncStatus)noexcept
{
	HRESULT hr=AsyncOp->GetResults(&fResult);
	fTaskState.SetDone();
}
//---------------------------------------------------------------------------
cGATTCharacteristic::cConfigureNotificationTask* cGATTCharacteristic::cConfigureNotificationTask::cConfigureNotificationHandler::GetHost(void)noexcept
{
	return cnMemory::GetObjectFromMemberPointer(this,&cConfigureNotificationTask::fConfigureNotificationHandler);
}
//---------------------------------------------------------------------------
void cGATTCharacteristic::cConfigureNotificationTask::cConfigureNotificationHandler::EventIncReference(void)noexcept
{
	rIncReference(GetHost(),'cfgn');
}
void cGATTCharacteristic::cConfigureNotificationTask::cConfigureNotificationHandler::EventDecReference(void)noexcept
{
	rDecReference(GetHost(),'cfgn');
}
void cGATTCharacteristic::cConfigureNotificationTask::cConfigureNotificationHandler::EventInvoke(IBLEWriteAsyncOp *AsyncOp,ABI::Windows::Foundation::AsyncStatus Status)noexcept
{
	GetHost()->ConfigureDone(AsyncOp,Status);
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
			cGATTUUID UUID;
		};
		hr=BLEService->get_Uuid(&IDValue);

		if(cnMemory::IsEqual(&UUID,&Service->ServiceUUID,sizeof(cGATTUUID))){
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
		printf("Service OpenAsync %x, %d\n",hr,(int)Status);
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
cGATTService::cGATTService(cGATTPeripheral *Owner,const cGATTUUID &ID)noexcept
	: ServiceUUID(ID)
	, fOwner(Owner)
	, fDispatch(Owner->GetDispatch())
	, fMainProcessWork(Owner->GetDispatch()->CreateWork(&fInnerReference,&fMainProcessProcedure))
	, fFuncState(GATTFunctionState::Absent)
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
cGATTUUID cGATTService::GetUUID(void)noexcept
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
rPtr<iGATTCharacteristic> cGATTService::AccessCharacteristic(const cGATTUUID &ID)noexcept
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
		printf("Char %p AddRef %u\n",this,v);
		return 0;
	}

	virtual ULONG STDMETHODCALLTYPE Release(void)noexcept override{
		auto v=--fRefCount.Free;
		printf("Char %p Release %u\n",this,v);
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

		printf("GATTService %p Refresh chars %x\n",this,Status);
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

			auto Pair=fUpdateMap.GetPair(reinterpret_cast<cGATTUUID&>(IDValue));
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
	, fFuncState(GATTFunctionState::Absent)
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
rPtr<iGATTService> cGATTPeripheral::AccessService(const cGATTUUID &ID)noexcept
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
	printf("Service Changed\n");
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
			cGATTUUID UUID;
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
const cGATTAdvertisementInfo& cGATTAdvertisementObserver::cAdvertisementData::GetInfo(void)noexcept
{
	return Info;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cGATTAdvertisementObserver::cGATTAdvertisementObserver(cGATTPeripheralCentral *Central,COMPtr<IBLEWatcher> Watcher)noexcept
	: fCentral(Central)
	, fWatcher(Watcher)
	, fCallback(nullptr)
{
	//fWatcher->add_Stopped;
	CreateQueueWork(fCentral->GetDispatch(),true);
	fWatcher->add_Received(&fBLEReceivedHandler,&fBLEReceivedToken);
}
//---------------------------------------------------------------------------
cGATTAdvertisementObserver::~cGATTAdvertisementObserver()noexcept
{
	fWatcher->remove_Received(fBLEReceivedToken);
}
//---------------------------------------------------------------------------
void cGATTAdvertisementObserver::VirtualStarted(void)noexcept
{
	InnerActivate('self');

}
//---------------------------------------------------------------------------
void cGATTAdvertisementObserver::VirtualStopped(void)noexcept
{
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
rPtr<iGATTAdvertisement> cGATTAdvertisementObserver::Fetch(void)noexcept
{
	auto pData=fAdvertisementQueue.Dequeue();
	if(pData==nullptr)
		return nullptr;
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

	AdvData->Info.LocalName=AdvData->LocalName.GetArray();
	AdvData->Info.Timestamp=AdvData->Timestamp;
	AdvData->Info.PeripheralAddress=AdvData->Address;

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
rPtr<iGATTPeripheral> cGATTPeripheralCentral::AccessPeripheral(iAddress *Address)noexcept
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
	//BLEWatcher->put_ScanningMode
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

rPtr<iGATTPeripheralCentral> UWP::OpenGATTPeripheral(iDispatch *Dispatch)noexcept
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
