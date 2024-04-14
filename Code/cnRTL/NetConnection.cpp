#include "NetConnection.h"
//---------------------------------------------------------------------------
using namespace cnLibrary;
using namespace cnRTL;

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
iAddress* cConnection::GetLocalAddress(void)noexcept
{
	return LocalAddress;
}
//---------------------------------------------------------------------------
iAddress* cConnection::GetRemoteAddress(void)noexcept
{
	return RemoteAddress;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
bcConnectionListener::bcConnectionListener()noexcept
{
	fConnectionQueueCallback=nullptr;
}
//---------------------------------------------------------------------------
bcConnectionListener::~bcConnectionListener()noexcept
{
}
//---------------------------------------------------------------------------
bool bcConnectionListener::StartNotify(iReference *Reference,iAsyncNotificationCallback *Callback)noexcept
{
	if(PrepareStartNotify()==false)
		return false;

	fConnectionQueueCallback=Callback;
	fConnectionQueueReference=Reference;

	ConfirmStartNotify();
	UpdateQueueState(false);
	return true;
}
//---------------------------------------------------------------------------
void bcConnectionListener::StopNotify(void)noexcept
{
	if(bcAsyncQueue::StopNotify()){
		UpdateQueueState(false);
	}
}
//---------------------------------------------------------------------------
void bcConnectionListener::NotifyCallback(bool IdleNotify)noexcept
{
	return NotifyQueue(IdleNotify);
}
//---------------------------------------------------------------------------
bool bcConnectionListener::IsClosed(void)noexcept
{
	return IsNotificationClosed();
}
//---------------------------------------------------------------------------
void bcConnectionListener::Close(void)noexcept
{
	return CloseQueue();
}
//---------------------------------------------------------------------------
void bcConnectionListener::NotificationStarted(void)noexcept
{
	bcAsyncQueue::NotificationStarted();

	rIncReference(this,'cnqu');
	fConnectionQueueCallback->AsyncStarted();
}
//---------------------------------------------------------------------------
void bcConnectionListener::NotificationStopped(void)noexcept
{
	bcAsyncQueue::NotificationStopped();

	auto Reference=cnVar::MoveCast(fConnectionQueueReference);
	auto Callback=fConnectionQueueCallback;
	fConnectionQueueCallback=nullptr;
	Callback->AsyncStopped();
	rDecReference(this,'cnqu');
}
//---------------------------------------------------------------------------
void bcConnectionListener::AsyncQueueNotify(void)noexcept
{
	fConnectionQueueCallback->AsyncNotify();
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
bcBufferedRWQueue::bcBufferedRWQueue()noexcept
{
}
//---------------------------------------------------------------------------
bcBufferedRWQueue::~bcBufferedRWQueue()noexcept
{
}
//---------------------------------------------------------------------------
void bcBufferedRWQueue::PutReadData(const void *Data,uIntn Size)noexcept
{
	auto WriteMemory=fReadDataQueue.ReserveWriteBuffer(Size);
	cnMemory::Copy(WriteMemory.Pointer,Data,Size);
	fReadDataQueue.CommitWriteBuffer(Size);

	UpdateReadQueueState(false);
}
//---------------------------------------------------------------------------
cMemory bcBufferedRWQueue::QueryReadDataBuffer(uIntn QuerySize)noexcept
{
	return fReadDataQueue.ReserveWriteBuffer(QuerySize);
}
//---------------------------------------------------------------------------
void bcBufferedRWQueue::AdvanceReadDataBuffer(uIntn Size)noexcept
{
	return fReadDataQueue.CommitWriteBuffer(Size);
}
//---------------------------------------------------------------------------
cConstMemory bcBufferedRWQueue::QueryWriteData(void)noexcept
{
	return fWriteDataQueue.GatherReadBuffer(0);
}
//---------------------------------------------------------------------------
void bcBufferedRWQueue::AdvanceWriteData(uIntn Size)noexcept
{
	return fWriteDataQueue.DismissReadBuffer(Size);
}
//---------------------------------------------------------------------------
bool bcBufferedRWQueue::IsWriteDataEnded(void)noexcept
{
	return false;
}
//---------------------------------------------------------------------------
uIntn bcBufferedRWQueue::GetMaxReadBufferSize(void)noexcept
{
	return fReadDataQueue.BufferSizeLimit;
}
//---------------------------------------------------------------------------
cConstMemory bcBufferedRWQueue::GatherReadBuffer(uIntn QuerySize)noexcept
{
	return fReadDataQueue.GatherReadBuffer(QuerySize);
}
//---------------------------------------------------------------------------
void bcBufferedRWQueue::DismissReadBuffer(uIntn Size)noexcept
{
	fReadDataQueue.DismissReadBuffer(Size);
	if(Size!=0){
		ReadBufferNotify();
	}
}
//---------------------------------------------------------------------------
uIntn bcBufferedRWQueue::GetMaxWriteBufferSize(void)noexcept
{
	return fWriteDataQueue.BufferSizeLimit;
}
//---------------------------------------------------------------------------
cMemory bcBufferedRWQueue::ReserveWriteBuffer(uIntn QuerySize)noexcept
{
	return fWriteDataQueue.ReserveWriteBuffer(QuerySize);
}
//---------------------------------------------------------------------------
void bcBufferedRWQueue::CommitWriteBuffer(uIntn Size)noexcept
{
	fWriteDataQueue.CommitWriteBuffer(Size);
	if(Size!=0){
		WriteDataNotify();
	}
}
//---------------------------------------------------------------------------
void bcBufferedRWQueue::ReadQueueClosed(void)noexcept
{
}
//---------------------------------------------------------------------------
void bcBufferedRWQueue::WriteQueueClosed(void)noexcept
{
}
//---------------------------------------------------------------------------
cnLib_INTERFACE_LOCALID_DEFINE(cGATTTunnelConectionDevice);
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cGATTTunnelConectionDevice::cRWQueue::cRWQueue(rPtr<cGATTTunnelConectionDevice> Device)noexcept
	: fDevice(cnVar::MoveCast(Device))
{
}
//---------------------------------------------------------------------------
cGATTTunnelConectionDevice::cRWQueue::~cRWQueue()noexcept
{
}
//---------------------------------------------------------------------------
void cGATTTunnelConectionDevice::cRWQueue::VirtualStarted(void)noexcept
{
}
//---------------------------------------------------------------------------
void cGATTTunnelConectionDevice::cRWQueue::VirtualStopped(void)noexcept
{
}
//---------------------------------------------------------------------------
cGATTTunnelConectionDevice* cGATTTunnelConectionDevice::cRWQueue::GetDevice(void)const noexcept
{
	return fDevice;
}
//---------------------------------------------------------------------------
iReference* cGATTTunnelConectionDevice::cRWQueue::RWQueueInnerReference(void)noexcept
{
	return &fInnerReference;
}
//---------------------------------------------------------------------------
void cGATTTunnelConectionDevice::cRWQueue::ReadBufferNotify(void)noexcept
{
}
//---------------------------------------------------------------------------
void cGATTTunnelConectionDevice::cRWQueue::WriteDataNotify(void)noexcept
{
	fDevice->RWQueueNotifyWrite();
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cGATTTunnelConectionDevice::cGATTTunnelConectionDevice(rPtr<iGATTService> Service,rPtr<iGATTCharacteristic> ReadChar,rPtr<iGATTCharacteristic> WriteChar)noexcept
	: fService(cnVar::MoveCast(Service))
	, fReadChar(cnVar::MoveCast(ReadChar))
	, fWriteChar(cnVar::MoveCast(WriteChar))
	, fConnectState(csIdle)
{
}
//---------------------------------------------------------------------------
cGATTTunnelConectionDevice::~cGATTTunnelConectionDevice()noexcept
{
}
//---------------------------------------------------------------------------
eiOrdering cGATTTunnelConectionDevice::Compare(iAddress *Dest)noexcept
{
	auto DestDevice=iCast<cGATTTunnelConectionDevice>(Dest);
	if(DestDevice==nullptr){
		return iOrdering::Different;
	}

	if(this==DestDevice)
		return iOrdering::Equal;
	if(this<DestDevice){
		return iOrdering::Less;
	}
	return iOrdering::Greater;
}
//---------------------------------------------------------------------------
iGATTService* cGATTTunnelConectionDevice::GetService(void)const noexcept
{
	return fService;
}
//---------------------------------------------------------------------------
iGATTCharacteristic* cGATTTunnelConectionDevice::GetReadCharacteristic(void)const noexcept
{
	return fReadChar;
}
//---------------------------------------------------------------------------
iGATTCharacteristic* cGATTTunnelConectionDevice::GetWriteCharacteristic(void)const noexcept
{
	return fWriteChar;
}
//---------------------------------------------------------------------------
bool cGATTTunnelConectionDevice::Connect(iConnectCallback *Callback)noexcept
{
	if(fRWQueue!=nullptr || fConnectState!=csIdle)
		return false;

	auto AutoLock=TakeLock(fConnectProcedureMutex);

	if(fConnectState!=csIdle){
		// is connecting
		return false;
	}
	// start connect
	fConnectState=csWaitConnectPerpheral;
	fConnectCallback=Callback;

	auto Peripheral=fService->GetPeripheral();
	//Peripheral->Connect();
	return true;
}
//---------------------------------------------------------------------------
void cGATTTunnelConectionDevice::GATTServiceStateChanged(void)noexcept
{
	UpdateFunctionState();
}
//---------------------------------------------------------------------------
void cGATTTunnelConectionDevice::UpdateFunctionState(void)noexcept
{
	switch(fConnectState){
	case csIdle:
		break;
	case csWaitConnectPerpheral:
		break;
	case csCheckService:
		// check service state
		{
			auto ServiceState=fService->GetFunctionState();
			auto ReadCharState=fReadChar->GetFunctionState();
			auto WriteCharState=fWriteChar->GetFunctionState();
			if(ServiceState==GATTFunctionState::Absent || ReadCharState==GATTFunctionState::Absent || WriteCharState==GATTFunctionState::Absent){
				// cannot connect to device
				fConnectState=csIdle;
				ConnectProcFailed();
			}
			else if(ServiceState==GATTFunctionState::Active && ReadCharState==GATTFunctionState::Active && WriteCharState==GATTFunctionState::Active){
				// online
				if(fReadNotifyDescriptor!=nullptr){
					// operate in event mode
					fConnectState=csWaitNotifyDescriptor;
					goto case_csWaitNotifyDescriptor;
				}
				else{
					// operate in poll mode
					fConnectState=csConnected;
					ConnectProcSucceed();
				}

			}
		}
		break;
	case csWaitNotifyDescriptor:
		{
case_csWaitNotifyDescriptor:
			auto AutoNotifyFunctionState=fReadNotifyDescriptor->GetFunctionState();
			if(AutoNotifyFunctionState==GATTFunctionState::Absent){
				// operate in poll mode
				fConnectState=csConnected;
				ConnectProcSucceed();
			}
			else if(AutoNotifyFunctionState==GATTFunctionState::Active){
				//fReadNotifyDescriptor->Write(0);
				fConnectState=csWaitRegisterNotify;
			}
		}
		break;
	case csWaitRegisterNotify:
		//if(fReadNotifyDescriptor->GetValue()){
			fConnectState=csConnected;
			ConnectProcSucceed();
		//}
		break;
	case csConnected:
		break;
	}
}
//---------------------------------------------------------------------------
void cGATTTunnelConectionDevice::ConnectProcSucceed(void)noexcept
{
	auto RWQueue=rCreate<cRWQueue>(this);
	fRWQueue=RWQueue;
	fConnectCallback->TunnelDeviceOnConnectDone(cnVar::MoveCast(RWQueue));
	fConnectCallback=nullptr;
}
//---------------------------------------------------------------------------
void cGATTTunnelConectionDevice::ConnectProcFailed(void)noexcept
{
	//fConnectCallback->TunnelDeviceOnConnectFailed();
	fConnectCallback->TunnelDeviceOnConnectDone(nullptr);
	fConnectCallback=nullptr;
}
//---------------------------------------------------------------------------
void cGATTTunnelConectionDevice::RWQueueNotifyWrite(void)noexcept
{
	auto WriteData=fRWQueue->QueryWriteData();

	uIntn SendSize=14;
	if(SendSize>WriteData.Length)
		SendSize=WriteData.Length;
	fWriteChar->WriteWithoutResponse(WriteData.Pointer,SendSize);


	fRWQueue->AdvanceWriteData(SendSize);
}
//---------------------------------------------------------------------------
void cGATTTunnelConectionDevice::ReadCharValueNotify(void)noexcept
{
	auto Value=fReadChar->Read();
	uIntn DataLength;
	auto Data=Value->GetArray(DataLength);

	if(fRWQueue!=nullptr){
		fRWQueue->PutReadData(Data,DataLength);
	}
}
//---------------------------------------------------------------------------
void cGATTTunnelConectionDevice::cReadCharHandler::GATTCharacteristStateChanged(void)noexcept
{
}
//---------------------------------------------------------------------------
void cGATTTunnelConectionDevice::cReadCharHandler::GATTCharacteristValueNotify(const void *Data,uIntn DataSize)noexcept
{

}
//---------------------------------------------------------------------------
void cGATTTunnelConectionDevice::cWriteCharHandler::GATTCharacteristStateChanged(void)noexcept
{
}
//---------------------------------------------------------------------------
void cGATTTunnelConectionDevice::cWriteCharHandler::GATTCharacteristValueNotify(const void *Data,uIntn DataSize)noexcept
{

}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cGATTTunnelConectionConnector::cEndpoint::cEndpoint(rPtr<cGATTTunnelConectionDevice::cRWQueue> RWQueue)noexcept
	: fRWQueue(cnVar::MoveCast(RWQueue))
{
}
//---------------------------------------------------------------------------
cGATTTunnelConectionConnector::cEndpoint::~cEndpoint()noexcept
{
}
//---------------------------------------------------------------------------
void* cGATTTunnelConectionConnector::cEndpoint::CastInterface(iTypeID IID)noexcept
{
	return ImpCastInterface<iConnection,iEndpoint>(this,IID);
}
//---------------------------------------------------------------------------
iAddress* cGATTTunnelConectionConnector::cEndpoint::GetLocalAddress(void)noexcept
{
	return nullptr;
}
//---------------------------------------------------------------------------
iAddress* cGATTTunnelConectionConnector::cEndpoint::GetRemoteAddress(void)noexcept
{
	return fRWQueue->GetDevice();
}
//---------------------------------------------------------------------------
void cGATTTunnelConectionConnector::cEndpoint::Close(void)noexcept
{
	fRWQueue->CloseReadQueue();
	fRWQueue->CloseWriteQueue();
}
//---------------------------------------------------------------------------
iReadQueue* cGATTTunnelConectionConnector::cEndpoint::GetReadQueue(void)noexcept
{
	return fRWQueue;
}
//---------------------------------------------------------------------------
iWriteQueue* cGATTTunnelConectionConnector::cEndpoint::GetWriteQueue(void)noexcept
{
	return fRWQueue;
}
//---------------------------------------------------------------------------
void cGATTTunnelConectionConnector::cEndpoint::SetWriteEndMode(eEndpointWriteEndMode EndMode)noexcept
{
	return fRWQueue->WriteQueueSetEndMode(EndMode);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void cGATTTunnelConectionConnector::cConnectSyncObject::TunnelDeviceOnConnectDone(rPtr<cGATTTunnelConectionDevice::cRWQueue> RWQueue)noexcept
{
	//Connection=CreateConnection(ConnectionIID,cnVar::MoveCast(RWQueue));

	Notify();
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
bool cGATTTunnelConectionConnector::cConnectAsyncTask::IsDone(void)noexcept
{
	return TaskState.IsDone();
}
//---------------------------------------------------------------------------
bool cGATTTunnelConectionConnector::cConnectAsyncTask::SetNotify(iProcedure *NotifyProcedure)noexcept
{
	return TaskState.SetNotify(NotifyProcedure);
}
//---------------------------------------------------------------------------
void cGATTTunnelConectionConnector::cConnectAsyncTask::Cancel(void)noexcept
{
}
//---------------------------------------------------------------------------
iConnection* cGATTTunnelConectionConnector::cConnectAsyncTask::GetConnection(void)noexcept
{
	return fConnection;
}
//---------------------------------------------------------------------------
void cGATTTunnelConectionConnector::cConnectAsyncTask::ConnectStart(void)noexcept
{
	rIncReference(this,'conn');
}
//---------------------------------------------------------------------------
void cGATTTunnelConectionConnector::cConnectAsyncTask::ConnectCancel(void)noexcept
{
	rDecReference(this,'conn');
}
//---------------------------------------------------------------------------
void cGATTTunnelConectionConnector::cConnectAsyncTask::TunnelDeviceOnConnectDone(rPtr<cGATTTunnelConectionDevice::cRWQueue> RWQueue)noexcept
{
	//fConnection=CreateConnection(ConnectionIID,cnVar::MoveCast(RWQueue));

	TaskState.SetDone();

	rDecReference(this,'conn');
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cGATTTunnelConectionConnector::cGATTTunnelConectionConnector()noexcept
{
}
//---------------------------------------------------------------------------
cGATTTunnelConectionConnector::~cGATTTunnelConectionConnector()noexcept
{
}
//---------------------------------------------------------------------------
iAddress* cGATTTunnelConectionConnector::GetLocalAddress(void)noexcept
{
	return nullptr;
}
//---------------------------------------------------------------------------
iPtr<iConnection> cGATTTunnelConectionConnector::Connect(iAddress *RemoteAddress)noexcept
{
	if(RemoteAddress==nullptr)
		return nullptr;
	auto TunnelDevice=iCast<cGATTTunnelConectionDevice>(RemoteAddress);
	if(TunnelDevice==nullptr)
		return nullptr;

	cConnectSyncObject ConnectObject;
	//ConnectObject.ConnectionIID=fConnectionIID;
	if(TunnelDevice->Connect(&ConnectObject)==false)
		return nullptr;

	ConnectObject.Wait();

	return cnVar::MoveCast(ConnectObject.Connection);
}
//---------------------------------------------------------------------------
iPtr<iConnectionTask> cGATTTunnelConectionConnector::ConnectAsync(iAddress *RemoteAddress)noexcept
{
	if(RemoteAddress==nullptr)
		return nullptr;
	auto TunnelDevice=iCast<cGATTTunnelConectionDevice>(RemoteAddress);
	if(TunnelDevice==nullptr)
		return nullptr;

	auto Task=iCreate<cConnectAsyncTask>();
	Task->ConnectStart();
	if(TunnelDevice->Connect(Task)==false){
		Task->ConnectCancel();
		return nullptr;
	}

	return Task;
}
//---------------------------------------------------------------------------
iPtr<iConnection> cGATTTunnelConectionConnector::CreateConnection(iTypeID ConnectionIID,rPtr<cGATTTunnelConectionDevice::cRWQueue> RWQueue)noexcept
{
	if(ConnectionIID==TInterfaceID<iEndpoint>::Value){
		return iCreate<cEndpoint>(cnVar::MoveCast(RWQueue));
	}
	return nullptr;
	//if(ConnectionIID==iTypeOf<iStream>()){
	//}
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cGATTTunnelConectionObserver::cGATTTunnelConectionObserver(iGATTAdvertisementObserver *Observer)noexcept
	: fObserver(Observer)
{
}
//---------------------------------------------------------------------------
cGATTTunnelConectionObserver::~cGATTTunnelConectionObserver()noexcept
{
}
//---------------------------------------------------------------------------
void cGATTTunnelConectionObserver::VirtualStarted(void)noexcept
{
	InnerActivate('self');
}
//---------------------------------------------------------------------------
void cGATTTunnelConectionObserver::VirtualStopped(void)noexcept
{
	CloseQueue();

	InnerDecReference('self');
}
//---------------------------------------------------------------------------
rPtr< iArrayReference< rPtr<cGATTTunnelConectionDevice> > > cGATTTunnelConectionObserver::QueryAllDevices(void)noexcept
{
	return nullptr;
}
//---------------------------------------------------------------------------
rPtr< iArrayReference< rPtr<cGATTTunnelConectionDevice> > > cGATTTunnelConectionObserver::FetchDeviceChanges(void)noexcept
{
	iGATTAdvertisement *Advertisement;
	while((Advertisement=fObserver->Fetch())!=nullptr){
		auto &Info=Advertisement->GetInfo();
	}
	return nullptr;
}
//---------------------------------------------------------------------------
void cGATTTunnelConectionObserver::DiscardChanges(void)noexcept
{
}
//---------------------------------------------------------------------------
//iPtr<iAddress> cGATTTunnelConectionObserver::Fetch(void)
//{
//	auto Item=fDeviceQueue.Dequeue();
//	if(Item==nullptr)
//		return nullptr;
//
//	auto Device=cnVar::MoveCast(Item->Device);
//
//	fDeviceItemRecycler.Recycle(Item);
//
//	return Device;
//}
//---------------------------------------------------------------------------
//bool cGATTTunnelConectionObserver::FetchBLEDevice(cGATTTunnelConnectionDeviceInfo &DeviceInfo)
//{
//	auto Item=fDeviceQueue.Dequeue();
//	if(Item==nullptr)
//		return false;
//
//	auto Device=cnVar::MoveCast(Item->Device);
//
//	// address
//	DeviceInfo.Address=Device;
//
//	auto Service=Device->GetService();
//	auto ReadChar=Device->GetReadCharacteristic();
//	auto WrtteChar=Device->GetWriteCharacteristic();
//
//	// uuids
//	DeviceInfo.DeviceID.ServiceID=Service->GetUUID();
//	DeviceInfo.DeviceID.ReadCharID=ReadChar->GetUUID();
//	DeviceInfo.DeviceID.WriteCharID=WrtteChar->GetUUID();
//
//	// name
//	auto Peripheral=Service->GetPeripheral();
//	DeviceInfo.Name=Peripheral->GetName();
//
//	fDeviceItemRecycler.Recycle(Item);
//
//	return true;
//}
//---------------------------------------------------------------------------
iReference* cGATTTunnelConectionObserver::NotificationInnerReference(void)noexcept
{
	return &fInnerReference;
}
//---------------------------------------------------------------------------
void cGATTTunnelConectionObserver::NotificationStarted(void)noexcept
{
	bcAsyncNotification::NotificationStarted();

	fPeripheralObserverError=false;
	if(fObserver->StartNotify(&fInnerReference,this)==false){
		fPeripheralObserverError=true;
	}
}
//---------------------------------------------------------------------------
void cGATTTunnelConectionObserver::NotificationStopped(void)noexcept
{
	fObserver->StopNotify();

	bcAsyncNotification::NotificationStopped();
}
//---------------------------------------------------------------------------
cGATTTunnelConectionObserver::CycleState cGATTTunnelConectionObserver::NotificationCheckState(void)noexcept
{
	if(fPeripheralObserverError){
		return CycleState::Terminated;
	}
	return CycleState::Normal;
}
//---------------------------------------------------------------------------
void cGATTTunnelConectionObserver::AsyncStarted(void)noexcept
{
}
//---------------------------------------------------------------------------
void cGATTTunnelConectionObserver::AsyncStopped(void)noexcept
{
}
//---------------------------------------------------------------------------
void cGATTTunnelConectionObserver::AsyncNotify(void)noexcept
{
	// notify from perpheral
	AsyncQueueSetAvailable(false);
}
//---------------------------------------------------------------------------
//void cGATTTunnelConectionObserver::BLEOnDeviceChecked(cnLib_BluetoothLEConectionPeripheral *ConnectionPeripheral)
//{
//	bool UpdateQueue=false;
//	auto CPPConnectionPeripheral=CPPBluetoothLEConectionPeripheral(ConnectionPeripheral);
//	for(auto &ServiceIDItem : fServiceIDs){
//		auto Service=CPPConnectionPeripheral->GetServiceByUUID(ServiceIDItem->ID.ServiceUUID);
//		if(Service!=nullptr){
//			auto ReadChar=CPPConnectionPeripheral->GetCharacteristByUUID(ServiceIDItem->ID.ServiceUUID,ServiceIDItem->ID.ReadCharacteristUUID);
//			auto WriteChar=CPPConnectionPeripheral->GetCharacteristByUUID(ServiceIDItem->ID.ServiceUUID,ServiceIDItem->ID.WriteCharacteristUUID);
//			if(ReadChar!=nil && WriteChar!=nil){
//				UpdateQueue=true;
//				auto Device=iCreate<cBluetoothLEConectionDevice>(ConnectionPeripheral,Service,ReadChar,WriteChar);
//
//				auto Item=fDeviceItemRecycler.Query();
//				Item->Peripheral=ConnectionPeripheral;
//				Item->Device=Device;
//				fDeviceQueue.Enqueue(Item);
//			}
//
//		}
//	}
//	if(UpdateQueue){
//		UpdateSignalState();
//	}
//}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cGATTTunnelConectionProtocol::cGATTTunnelConectionProtocol(rPtr<iGATTPeripheralCentral> Central)noexcept
	: fCentral(cnVar::MoveCast(Central))
{
	if(fCentral!=nullptr){
		//fStreamConnector=iCreate<cGATTTunnelConectionConnector>(TInterfaceID<iStream>::Value);
		//fEndpointConnector=iCreate<cGATTTunnelConectionConnector>(TInterfaceID<iEndpoint>::Value);
	}
}
//---------------------------------------------------------------------------
cGATTTunnelConectionProtocol::~cGATTTunnelConectionProtocol()noexcept
{
}
//---------------------------------------------------------------------------
iPtr<iConnectionConnector> cGATTTunnelConectionProtocol::CreateStreamConnector(iAddress *LocalAddress,iAddress *RemoteAddress)noexcept
{
	if(LocalAddress!=nullptr)
		return nullptr;

	return fStreamConnector;
}
//---------------------------------------------------------------------------
iPtr<iConnectionListener> cGATTTunnelConectionProtocol::CreateStreamListener(iAddress *LocalAddress)noexcept
{
	return nullptr;
}
//---------------------------------------------------------------------------
iPtr<iConnectionConnector> cGATTTunnelConectionProtocol::CreateEndpointConnector(iAddress *LocalAddress,iAddress *RemoteAddress)noexcept
{
	if(LocalAddress!=nullptr)
		return nullptr;

	return fEndpointConnector;
}
//---------------------------------------------------------------------------
iPtr<iConnectionListener> cGATTTunnelConectionProtocol::CreateEndpointListener(iAddress *LocalAddress)noexcept
{
	return nullptr;
}
//---------------------------------------------------------------------------
rPtr<cGATTTunnelConectionObserver> cGATTTunnelConectionProtocol::CreateObserver(const cGATTTunnelConnectionDeviceID *ServiceIDs,uIntn ServiceCount)noexcept
{
	cnRTL::cArrayBlock<cGATTUUID> ServiceUUIDs;
	ServiceUUIDs.SetLength(ServiceCount);
	for(uIntn i=0;i<ServiceCount;i++){
		ServiceUUIDs.Pointer[i]=ServiceIDs[i].ServiceUUID;
	}
	auto AdvertisementObserver=fCentral->CreateAdvertisementObserver();
	if(AdvertisementObserver==nullptr)
		return nullptr;

	auto ConnectionObserver=rCreate<cGATTTunnelConectionObserver>(AdvertisementObserver);

	ConnectionObserver->DeviceIDs.Append(ServiceIDs,ServiceCount);

	return ConnectionObserver;
}
//---------------------------------------------------------------------------
