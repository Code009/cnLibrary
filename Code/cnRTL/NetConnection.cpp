#include "NetConnection.h"
//---------------------------------------------------------------------------
using namespace cnLibrary;
using namespace cnRTL;

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
iAddress* cConnection::GetLocalAddress(void)
{
	return LocalAddress;
}
//---------------------------------------------------------------------------
iAddress* cConnection::GetRemoteAddress(void)
{
	return RemoteAddress;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
bcConnectionQueueProcessor::bcConnectionQueueProcessor(iConnectionQueue *Queue)
	: fActiveMutex(false)
	, fQueueActive(false)
	, fConnectionQueue(Queue)
{
}
//---------------------------------------------------------------------------
bcConnectionQueueProcessor::~bcConnectionQueueProcessor()
{
}
//---------------------------------------------------------------------------
iConnectionQueue* bcConnectionQueueProcessor::GetConnectionQueue(void)const
{
	return fConnectionQueue;
}
//---------------------------------------------------------------------------
bool bcConnectionQueueProcessor::SetConnectionQueue(iConnectionQueue *Queue)
{
	if(fActiveMutex.Acquire.Xchg(true)==true){
		return false;
	}
	if(fQueueActive==false){
		fConnectionQueue=Queue;
	}
	
	fActiveMutex.Release.Store(false);
	return true;
}
//---------------------------------------------------------------------------
bool bcConnectionQueueProcessor::IsActive(void)const
{
	return fQueueActive;
}
//---------------------------------------------------------------------------
bool bcConnectionQueueProcessor::Start(iReference *Reference)
{
	if(fConnectionQueue==nullptr){
		return false;
	}
	if(fQueueActive)
		return false;
	if(fActiveMutex.Acquire.Xchg(true)==true){
		return false;
	}

	if(fConnectionQueue->StartNotify(Reference,this)==false){
		fActiveMutex.Release.Store(false);
		return false;
	}

	return true;
}
//---------------------------------------------------------------------------
void bcConnectionQueueProcessor::Stop(void)
{
	if(fConnectionQueue!=nullptr){
		if(fQueueActive){
			fConnectionQueue->StopNotify();
		}
	}
}
//---------------------------------------------------------------------------
void bcConnectionQueueProcessor::AsyncStarted(void)
{
	fQueueActive=true;
	fActiveMutex.Release.Store(false);
}
//---------------------------------------------------------------------------
void bcConnectionQueueProcessor::AsyncStopped(void)
{
	fQueueActive=false;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
bcConnectionQueue::bcConnectionQueue()
{
	fConnectionQueueCallback=nullptr;
}
//---------------------------------------------------------------------------
bcConnectionQueue::~bcConnectionQueue()
{
}
//---------------------------------------------------------------------------
bool bcConnectionQueue::StartNotify(iReference *Reference,iAsyncNotificationCallback *Callback)
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
void bcConnectionQueue::StopNotify(void)
{
	if(bcAsyncQueue::StopNotify()){
		UpdateQueueState(false);
	}
}
//---------------------------------------------------------------------------
void bcConnectionQueue::NotifyCallback(bool IdleNotify)
{
	return NotifyQueue(IdleNotify);
}
//---------------------------------------------------------------------------
bool bcConnectionQueue::IsClosed(void)
{
	return IsNotificationClosed();
}
//---------------------------------------------------------------------------
void bcConnectionQueue::NotificationStarted(void)
{
	bcAsyncQueue::NotificationStarted();

	rIncReference(this,'cnqu');
	fConnectionQueueCallback->AsyncStarted();
}
//---------------------------------------------------------------------------
void bcConnectionQueue::NotificationStopped(void)
{
	bcAsyncQueue::NotificationStopped();

	auto Reference=cnVar::MoveCast(fConnectionQueueReference);
	auto Callback=fConnectionQueueCallback;
	fConnectionQueueCallback=nullptr;
	Callback->AsyncStopped();
	rDecReference(this,'cnqu');
}
//---------------------------------------------------------------------------
void bcConnectionQueue::AsyncQueueNotify(void)
{
	fConnectionQueueCallback->AsyncNotify();
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
bool bcConnectionListener::cAcceptTask::IsDone(void)
{
	return fTaskState.IsDone();
}
//---------------------------------------------------------------------------
bool bcConnectionListener::cAcceptTask::SetNotify(iProcedure *NotifyProcedure)
{
	return fTaskState.SetNotify(NotifyProcedure);
}
//---------------------------------------------------------------------------
void bcConnectionListener::cAcceptTask::Cancel(void)
{
	Owner->fAcceptTaskQueue.CancelTask(this);
}
//---------------------------------------------------------------------------
void bcConnectionListener::cAcceptTask::PrepareAccept(void)
{
}
//---------------------------------------------------------------------------
iConnection* bcConnectionListener::cAcceptTask::GetConnection(void)
{
	if(fTaskState.IsDone()==false){
		return nullptr;
	}
	return Connection;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
bcConnectionListener::bcConnectionListener()
{
	fListenerClosed=false;
}
//---------------------------------------------------------------------------
bcConnectionListener::~bcConnectionListener()
{
}
//---------------------------------------------------------------------------
void bcConnectionListener::CloseListener(void)
{
	fListenerClosed=true;
	UpdateAcceptTaskQueue();
}
//---------------------------------------------------------------------------
void bcConnectionListener::Close(void)
{
	CloseListener();
}
//---------------------------------------------------------------------------
iPtr<iConnection> bcConnectionListener::Accept(void)
{
	if(fListenerClosed){
		//fLastErrorCode=StreamError::Closed;
		return nullptr;
	}
	auto Task=iCreate<cAcceptTask>();
	Task->Owner=this;
	Task->PrepareAccept();
	fAcceptTaskQueue.EnqueueTask(Task);

	UpdateAcceptTaskQueue();
	// sync access

	WaitForTask(Task);

	//fLastErrorCode=Task->AccessErrorCode;
	return cnVar::MoveCast(Task->Connection);
}
//---------------------------------------------------------------------------
iPtr<iConnectionTask> bcConnectionListener::AcceptAsync(void)
{
	if(fListenerClosed){
		//fLastErrorCode=StreamError::Closed;
		return nullptr;
	}
	auto Task=iCreate<cAcceptTask>();
	Task->Owner=this;
	Task->PrepareAccept();
	fAcceptTaskQueue.EnqueueTask(Task);

	UpdateAcceptTaskQueue();
	return Task;
}
//---------------------------------------------------------------------------
rPtr<bcConnectionListener::cAcceptTask> bcConnectionListener::QueryAcceptTask(void)
{
	auto Task=fAcceptTaskQueue.DequeueTask();
	if(Task==nullptr)
		return nullptr;

	auto pTask=Task.ExtractToManual();
	return rPtr<cAcceptTask>::TakeFromManual(static_cast<cAcceptTask*>(pTask));
}
//---------------------------------------------------------------------------
void bcConnectionListener::CompleteAcceptTask(rPtr<cAcceptTask> Task)
{
	fAcceptTaskQueue.CompleteTask(cnVar::MoveCast(Task));
}
//---------------------------------------------------------------------------
void bcConnectionListener::UpdateAcceptTaskQueue(void)
{
	fProcessAcceptTaskQueueProcedure.Run();
}
//---------------------------------------------------------------------------
bool bcConnectionListener::cProcessAcceptTaskQueueProcedure::Procedure(void)
{
	auto Host=cnMemory::GetObjectFromMemberPointer(this,&bcConnectionListener::fProcessAcceptTaskQueueProcedure);
	return Host->ProcessAcceptTaskQueueProc();
}
//---------------------------------------------------------------------------
bool bcConnectionListener::ProcessAcceptTaskQueueProc(void)
{
	if(fListenerClosed){
		// close all accept task
		auto Task=fAcceptTaskQueue.DequeueTask();
		while(Task!=nullptr){
			auto *AcceptTask=static_cast<cAcceptTask*>(Task.Pointer());
			AcceptTask->AcceptErrorCode=StreamError::Closed;
			fAcceptTaskQueue.CompleteTask(Task);

			Task=fAcceptTaskQueue.DequeueTask();
		}

		// process when read end
		ConnectionProcessEnd();
		return false;
	}
	ConnectionProcessTask();
	return false;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cnLib_INTERFACE_LOCALID_DEFINE(cGATTTunnelConectionDevice);
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
bcBufferedRWQueue::bcBufferedRWQueue()
{
}
//---------------------------------------------------------------------------
bcBufferedRWQueue::~bcBufferedRWQueue()
{
}
//---------------------------------------------------------------------------
void bcBufferedRWQueue::PutReadData(const void *Data,uIntn Size)
{
	auto WriteMemory=fReadDataQueue.ReserveWriteBuffer(Size);
	cnMemory::Copy(WriteMemory.Pointer,Data,Size);
	fReadDataQueue.CommitWriteBuffer(Size);

	UpdateReadQueueState(false);
}
//---------------------------------------------------------------------------
cMemory bcBufferedRWQueue::QueryReadDataBuffer(uIntn QuerySize)
{
	return fReadDataQueue.ReserveWriteBuffer(QuerySize);
}
//---------------------------------------------------------------------------
void bcBufferedRWQueue::AdvanceReadDataBuffer(uIntn Size)
{
	return fReadDataQueue.CommitWriteBuffer(Size);
}
//---------------------------------------------------------------------------
cConstMemory bcBufferedRWQueue::QueryWriteData(void)
{
	return fWriteDataQueue.GatherReadBuffer(0);
}
//---------------------------------------------------------------------------
void bcBufferedRWQueue::AdvanceWriteData(uIntn Size)
{
	return fWriteDataQueue.DismissReadBuffer(Size);
}
//---------------------------------------------------------------------------
bool bcBufferedRWQueue::IsWriteDataEnded(void)
{
	return false;
}
//---------------------------------------------------------------------------
cConstMemory bcBufferedRWQueue::GatherReadBuffer(uIntn QuerySize)
{
	return fReadDataQueue.GatherReadBuffer(QuerySize);
}
//---------------------------------------------------------------------------
void bcBufferedRWQueue::DismissReadBuffer(uIntn Size)
{
	fReadDataQueue.DismissReadBuffer(Size);
	if(Size!=0){
		ReadBufferNotify();
	}
}
//---------------------------------------------------------------------------
cMemory bcBufferedRWQueue::ReserveWriteBuffer(uIntn QuerySize)
{
	return fWriteDataQueue.ReserveWriteBuffer(QuerySize);
}
//---------------------------------------------------------------------------
void bcBufferedRWQueue::CommitWriteBuffer(uIntn Size)
{
	fWriteDataQueue.CommitWriteBuffer(Size);
	if(Size!=0){
		WriteDataNotify();
	}
}
//---------------------------------------------------------------------------
void bcBufferedRWQueue::ReadQueueClosed(void)
{
}
//---------------------------------------------------------------------------
void bcBufferedRWQueue::WriteQueueClosed(void)
{
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cGATTTunnelConectionDevice::cRWQueue::cRWQueue(rPtr<cGATTTunnelConectionDevice> Device)
	: fDevice(cnVar::MoveCast(Device))
{
}
//---------------------------------------------------------------------------
cGATTTunnelConectionDevice::cRWQueue::~cRWQueue()
{
}
//---------------------------------------------------------------------------
void cGATTTunnelConectionDevice::cRWQueue::VirtualStarted(void)
{
}
//---------------------------------------------------------------------------
void cGATTTunnelConectionDevice::cRWQueue::VirtualStopped(void)
{
}
//---------------------------------------------------------------------------
cGATTTunnelConectionDevice* cGATTTunnelConectionDevice::cRWQueue::GetDevice(void)const
{
	return fDevice;
}
//---------------------------------------------------------------------------
iReference* cGATTTunnelConectionDevice::cRWQueue::RWQueueInnerReference(void)
{
	return &fInnerReference;
}
//---------------------------------------------------------------------------
void cGATTTunnelConectionDevice::cRWQueue::ReadBufferNotify(void)
{
}
//---------------------------------------------------------------------------
void cGATTTunnelConectionDevice::cRWQueue::WriteDataNotify(void)
{
	fDevice->RWQueueNotifyWrite();
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cGATTTunnelConectionDevice::cGATTTunnelConectionDevice(rPtr<iGATTService> Service,iPtr<iGATTCharacteristic> ReadChar,iPtr<iGATTCharacteristic> WriteChar)
	: fService(cnVar::MoveCast(Service))
	, fReadChar(cnVar::MoveCast(ReadChar))
	, fWriteChar(cnVar::MoveCast(WriteChar))
	, fConnectState(csIdle)
{
}
//---------------------------------------------------------------------------
cGATTTunnelConectionDevice::~cGATTTunnelConectionDevice()
{
}
//---------------------------------------------------------------------------
eiOrdering cGATTTunnelConectionDevice::Compare(iAddress *Dest)
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
iGATTService* cGATTTunnelConectionDevice::GetService(void)const
{
	return fService;
}
//---------------------------------------------------------------------------
iGATTCharacteristic* cGATTTunnelConectionDevice::GetReadCharacteristic(void)const
{
	return fReadChar;
}
//---------------------------------------------------------------------------
iGATTCharacteristic* cGATTTunnelConectionDevice::GetWriteCharacteristic(void)const
{
	return fWriteChar;
}
//---------------------------------------------------------------------------
bool cGATTTunnelConectionDevice::Connect(iConnectCallback *Callback)
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
	Peripheral->Activate();
	return true;
}
//---------------------------------------------------------------------------
void cGATTTunnelConectionDevice::GATTServiceStateChanged(void)
{
	UpdateFunctionState();
}
//---------------------------------------------------------------------------
void cGATTTunnelConectionDevice::GATTServiceCharacteristListChanged(void)
{
}
//---------------------------------------------------------------------------
void cGATTTunnelConectionDevice::UpdateFunctionState(void)
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
void cGATTTunnelConectionDevice::ConnectProcSucceed(void)
{
	auto RWQueue=rCreate<cRWQueue>(this);
	fRWQueue=RWQueue;
	fConnectCallback->TunnelDeviceOnConnectDone(cnVar::MoveCast(RWQueue));
	fConnectCallback=nullptr;
}
//---------------------------------------------------------------------------
void cGATTTunnelConectionDevice::ConnectProcFailed(void)
{
	//fConnectCallback->TunnelDeviceOnConnectFailed();
	fConnectCallback->TunnelDeviceOnConnectDone(nullptr);
	fConnectCallback=nullptr;
}
//---------------------------------------------------------------------------
void cGATTTunnelConectionDevice::RWQueueNotifyWrite(void)
{
	auto WriteData=fRWQueue->QueryWriteData();

	uIntn SendSize=14;
	if(SendSize>WriteData.Length)
		SendSize=WriteData.Length;
	fWriteChar->WriteWithoutResponse(WriteData.Pointer,SendSize);


	fRWQueue->AdvanceWriteData(SendSize);
}
//---------------------------------------------------------------------------
void cGATTTunnelConectionDevice::ReadCharValueNotify(void)
{
	auto Value=fReadChar->GetValue();
	uIntn DataLength;
	auto Data=Value->GetArray(DataLength);

	if(fRWQueue!=nullptr){
		fRWQueue->PutReadData(Data,DataLength);
	}
}
//---------------------------------------------------------------------------
void cGATTTunnelConectionDevice::cReadCharHandler::GATTCharacteristStateChanged(void)
{
}
//---------------------------------------------------------------------------
void cGATTTunnelConectionDevice::cReadCharHandler::GATTCharacteristDescriptorListChanged(void)
{
}
//---------------------------------------------------------------------------
void cGATTTunnelConectionDevice::cReadCharHandler::GATTCharacteristValueNotify(void)
{

}
//---------------------------------------------------------------------------
void cGATTTunnelConectionDevice::cWriteCharHandler::GATTCharacteristStateChanged(void)
{
}
//---------------------------------------------------------------------------
void cGATTTunnelConectionDevice::cWriteCharHandler::GATTCharacteristDescriptorListChanged(void)
{
}
//---------------------------------------------------------------------------
void cGATTTunnelConectionDevice::cWriteCharHandler::GATTCharacteristValueNotify(void)
{

}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cGATTTunnelConectionConnector::cEndpoint::cEndpoint(rPtr<cGATTTunnelConectionDevice::cRWQueue> RWQueue)
	: fRWQueue(cnVar::MoveCast(RWQueue))
{
}
//---------------------------------------------------------------------------
cGATTTunnelConectionConnector::cEndpoint::~cEndpoint()
{
}
//---------------------------------------------------------------------------
void* cGATTTunnelConectionConnector::cEndpoint::CastInterface(iTypeID IID)noexcept(true)
{
	return ImpCastInterface<iConnection,iEndpoint>(this,IID);
}
//---------------------------------------------------------------------------
iAddress* cGATTTunnelConectionConnector::cEndpoint::GetLocalAddress(void)
{
	return nullptr;
}
//---------------------------------------------------------------------------
iAddress* cGATTTunnelConectionConnector::cEndpoint::GetRemoteAddress(void)
{
	return fRWQueue->GetDevice();
}
//---------------------------------------------------------------------------
void cGATTTunnelConectionConnector::cEndpoint::Close(void)
{
	fRWQueue->CloseReadQueue();
	fRWQueue->CloseWriteQueue();
}
//---------------------------------------------------------------------------
iReadQueue* cGATTTunnelConectionConnector::cEndpoint::GetReadQueue(void)
{
	return fRWQueue;
}
//---------------------------------------------------------------------------
iWriteQueue* cGATTTunnelConectionConnector::cEndpoint::GetWriteQueue(void)
{
	return fRWQueue;
}
//---------------------------------------------------------------------------
void cGATTTunnelConectionConnector::cEndpoint::SetWriteEndMode(eEndpointWriteEndMode EndMode)
{
	return fRWQueue->WriteQueueSetEndMode(EndMode);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void cGATTTunnelConectionConnector::cConnectSyncObject::TunnelDeviceOnConnectDone(rPtr<cGATTTunnelConectionDevice::cRWQueue> RWQueue)
{
	//Connection=CreateConnection(ConnectionIID,cnVar::MoveCast(RWQueue));

	Notify();
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
bool cGATTTunnelConectionConnector::cConnectAsyncTask::IsDone(void)
{
	return TaskState.IsDone();
}
//---------------------------------------------------------------------------
bool cGATTTunnelConectionConnector::cConnectAsyncTask::SetNotify(iProcedure *NotifyProcedure)
{
	return TaskState.SetNotify(NotifyProcedure);
}
//---------------------------------------------------------------------------
void cGATTTunnelConectionConnector::cConnectAsyncTask::Cancel(void)
{
}
//---------------------------------------------------------------------------
iConnection* cGATTTunnelConectionConnector::cConnectAsyncTask::GetConnection(void)
{
	return fConnection;
}
//---------------------------------------------------------------------------
void cGATTTunnelConectionConnector::cConnectAsyncTask::ConnectStart(void)
{
	rIncReference(this,'conn');
}
//---------------------------------------------------------------------------
void cGATTTunnelConectionConnector::cConnectAsyncTask::ConnectCancel(void)
{
	rDecReference(this,'conn');
}
//---------------------------------------------------------------------------
void cGATTTunnelConectionConnector::cConnectAsyncTask::TunnelDeviceOnConnectDone(rPtr<cGATTTunnelConectionDevice::cRWQueue> RWQueue)
{
	//fConnection=CreateConnection(ConnectionIID,cnVar::MoveCast(RWQueue));

	TaskState.SetDone();

	rDecReference(this,'conn');
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cGATTTunnelConectionConnector::cGATTTunnelConectionConnector()
{
}
//---------------------------------------------------------------------------
cGATTTunnelConectionConnector::~cGATTTunnelConectionConnector()
{
}
//---------------------------------------------------------------------------
iAddress* cGATTTunnelConectionConnector::GetLocalAddress(void)
{
	return nullptr;
}
//---------------------------------------------------------------------------
iPtr<iConnection> cGATTTunnelConectionConnector::Connect(iAddress *RemoteAddress)
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
iPtr<iConnectionTask> cGATTTunnelConectionConnector::ConnectAsync(iAddress *RemoteAddress)
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
iPtr<iConnection> cGATTTunnelConectionConnector::CreateConnection(iTypeID ConnectionIID,rPtr<cGATTTunnelConectionDevice::cRWQueue> RWQueue)
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
cGATTTunnelConectionObserver::cGATTTunnelConectionObserver(iGATTPeripheralObserver *Observer)
	: fObserver(Observer)
{
}
//---------------------------------------------------------------------------
cGATTTunnelConectionObserver::~cGATTTunnelConectionObserver()
{
}
//---------------------------------------------------------------------------
void cGATTTunnelConectionObserver::VirtualStarted(void)
{
	InnerIncReference('self');
}
//---------------------------------------------------------------------------
void cGATTTunnelConectionObserver::VirtualStopped(void)
{
	CloseQueue();

	InnerDecReference('self');
}
//---------------------------------------------------------------------------
rPtr< iArrayReference< rPtr<cGATTTunnelConectionDevice> > > cGATTTunnelConectionObserver::QueryAllDevices(void)
{
	return nullptr;
}
//---------------------------------------------------------------------------
rPtr< iArrayReference< rPtr<cGATTTunnelConectionDevice> > > cGATTTunnelConectionObserver::FetchDeviceChanges(void)
{
	auto Changes=fObserver->QueryChanges();
	if(Changes==nullptr){
		// no changes
		return nullptr;
	}

	uIntn ArrayLength;
	auto Array=Changes->GetArray(ArrayLength);
	ArrayLength;
	Array[0];
	return nullptr;
}
//---------------------------------------------------------------------------
void cGATTTunnelConectionObserver::DiscardChanges(void)
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
iReference* cGATTTunnelConectionObserver::NotificationInnerReference(void)
{
	return &fInnerReference;
}
//---------------------------------------------------------------------------
void cGATTTunnelConectionObserver::NotificationStarted(void)
{
	bcAsyncNotification::NotificationStarted();

	fPeripheralObserverError=false;
	if(fObserver->StartNotify(&fInnerReference,this)==false){
		fPeripheralObserverError=true;
	}
}
//---------------------------------------------------------------------------
void cGATTTunnelConectionObserver::NotificationStopped(void)
{
	fObserver->StopNotify();

	bcAsyncNotification::NotificationStopped();
}
//---------------------------------------------------------------------------
cGATTTunnelConectionObserver::CycleState cGATTTunnelConectionObserver::NotificationCheckState(void)
{
	if(fPeripheralObserverError){
		return CycleState::Terminated;
	}
	return CycleState::Normal;
}
//---------------------------------------------------------------------------
void cGATTTunnelConectionObserver::AsyncStarted(void)
{
}
//---------------------------------------------------------------------------
void cGATTTunnelConectionObserver::AsyncStopped(void)
{
}
//---------------------------------------------------------------------------
void cGATTTunnelConectionObserver::AsyncNotify(void)
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
cGATTTunnelConectionProtocol::cGATTTunnelConectionProtocol(rPtr<iGATTPeripheralCentral> Central,rPtr<iGATTPeripheralDevice> Device)
	: fCentral(cnVar::MoveCast(Central))
	, fDevice(cnVar::MoveCast(Device))
{
	if(fCentral!=nullptr){
		//fStreamConnector=iCreate<cGATTTunnelConectionConnector>(TInterfaceID<iStream>::Value);
		//fEndpointConnector=iCreate<cGATTTunnelConectionConnector>(TInterfaceID<iEndpoint>::Value);
	}
}
//---------------------------------------------------------------------------
cGATTTunnelConectionProtocol::~cGATTTunnelConectionProtocol()
{
}
//---------------------------------------------------------------------------
iPtr<iConnectionConnector> cGATTTunnelConectionProtocol::CreateStreamConnector(iAddress *LocalAddress,iAddress *RemoteAddress)
{
	if(LocalAddress!=nullptr)
		return nullptr;

	return fStreamConnector;
}
//---------------------------------------------------------------------------
iPtr<iConnectionListener> cGATTTunnelConectionProtocol::CreateStreamListener(iAddress *LocalAddress)
{
	return nullptr;
}
//---------------------------------------------------------------------------
iPtr<iConnectionQueue> cGATTTunnelConectionProtocol::CreateStreamConnectionQueue(iAddress *LocalAddress)
{
	return nullptr;
}
//---------------------------------------------------------------------------
iPtr<iConnectionConnector> cGATTTunnelConectionProtocol::CreateEndpointConnector(iAddress *LocalAddress,iAddress *RemoteAddress)
{
	if(LocalAddress!=nullptr)
		return nullptr;

	return fEndpointConnector;
}
//---------------------------------------------------------------------------
iPtr<iConnectionListener> cGATTTunnelConectionProtocol::CreateEndpointListener(iAddress *LocalAddress)
{
	return nullptr;
}
//---------------------------------------------------------------------------
iPtr<iConnectionQueue> cGATTTunnelConectionProtocol::CreateEndpointConnectionQueue(iAddress *LocalAddress)
{
	return nullptr;
}
//---------------------------------------------------------------------------
rPtr<cGATTTunnelConectionObserver> cGATTTunnelConectionProtocol::CreateObserver(const cGATTTunnelConnectionDeviceID *ServiceIDs,uIntn ServiceCount)
{
	cnRTL::cArrayBlock<cGATTUUID> ServiceUUIDs;
	ServiceUUIDs.SetLength(ServiceCount);
	for(uIntn i=0;i<ServiceCount;i++){
		ServiceUUIDs.Pointer[i]=ServiceIDs[i].ServiceUUID;
	}
	auto PeripheralObserver=fCentral->CreatePeripheralObserver(ServiceUUIDs.Pointer,ServiceUUIDs.Length);
	if(PeripheralObserver==nullptr)
		return nullptr;

	auto ConnectionObserver=rCreate<cGATTTunnelConectionObserver>(PeripheralObserver);

	ConnectionObserver->DeviceIDs.Append(ServiceIDs,ServiceCount);

	return ConnectionObserver;
}
//---------------------------------------------------------------------------
