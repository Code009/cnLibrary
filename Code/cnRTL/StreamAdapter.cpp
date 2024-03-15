#include "StreamAdapter.h"
//---------------------------------------------------------------------------
using namespace cnLibrary;
using namespace cnRTL;

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
eStreamError cnRTL::GetStreamError(iInterface *Stream)
{
	//auto ErrorReport=iCast<iStreamErrorReport>(Stream);
	//if(ErrorReport==nullptr){
		return StreamError::Unknown;
	//}
	//return ErrorReport->GetStreamError();
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cStreamFromEndpoint::cStreamFromEndpoint()
{
}
//---------------------------------------------------------------------------
cStreamFromEndpoint::~cStreamFromEndpoint()
{
}
//---------------------------------------------------------------------------
void cStreamFromEndpoint::VirtualStopped(void)
{
	StreamClose();
	cDualReference::VirtualStopped();
}
//---------------------------------------------------------------------------
void* cStreamFromEndpoint::CastInterface(iTypeID InterfaceID)noexcept(true)
{
	void* Ret=bcStream::CastInterface(InterfaceID);
	if(Ret!=nullptr){
		return Ret;
	}
	return fEndpoint->CastInterface(InterfaceID);
}
//---------------------------------------------------------------------------
bool cStreamFromEndpoint::Connect(iEndpoint *Endpoint)
{
	if(fReadQueue!=nullptr || fWriteQueue!=nullptr)
		return false;

#pragma message (cnLib_FILE_LINE " : TODO - better Endpoint management")
	fEndpoint=Endpoint;
	fReadQueue=fEndpoint->GetReadQueue();
	if(fReadQueue->StartRead(&fInnerReference,this)==false){
		fReadQueue=nullptr;
		fEndpoint=nullptr;
		return false;
	}
	fWriteQueue=fEndpoint->GetWriteQueue();
	if(fWriteQueue->StartWrite(&fInnerReference,this)==false){
		fReadQueue->StopRead();
		fWriteQueue=nullptr;
		return false;
	}

	return true;
}
//---------------------------------------------------------------------------
void cStreamFromEndpoint::StreamProcessReadTask(void)
{
	if(!fReadActive){
		return;
	}

	cConstMemory ReadQueueBuffer;
	while((ReadQueueBuffer=fReadQueue->GatherReadBuffer(0)).Length!=0){
		auto ReadTask=QueryReadTask();
		if(ReadTask==nullptr){
			fReadQueue->DismissReadBuffer(0);
			return;	// no pending task
		}

		uIntn SizeWritten=WriteToStream<cReadTask>(ReadQueueBuffer.Pointer,ReadQueueBuffer.Length,ReadTask.Pointer());

		// Read Task Completed

		ReadTask->AccessErrorCode=StreamError::Success;
		CompleteReadTask(cnVar::MoveCast(ReadTask));
	
		fReadQueue->DismissReadBuffer(SizeWritten);
	}


	bool ReadQueueGracefulClose;
	if(fReadQueue->IsReadClosed(ReadQueueGracefulClose)){
		fReadQueue->StopRead();
		fReadQueue=nullptr;

		SetReadEnd(ReadQueueGracefulClose);
	}
	else{
		fReadQueue->NotifyRead(1);
	}
}
//---------------------------------------------------------------------------
void cStreamFromEndpoint::StreamProcessReadEnd(void)
{
	if(fReadQueue!=nullptr){
		fReadQueue->StopRead();
		fReadQueue=nullptr;
	}
}
//---------------------------------------------------------------------------
void cStreamFromEndpoint::ReadStarted(void)
{
	fReadActive=true;
	fReadQueue->NotifyRead(1);
}
//---------------------------------------------------------------------------
void cStreamFromEndpoint::ReadStopped(void)
{
	fReadActive=false;
}
//---------------------------------------------------------------------------
void cStreamFromEndpoint::ReadNotify(void)
{
	UpdateReadTaskQueue();
}
//---------------------------------------------------------------------------
void cStreamFromEndpoint::StreamProcessWriteTask(void)
{
	if(!fWriteActive){
		return;
	}

	bool WriteQueueGracefulClose;
	if(fWriteQueue->IsWriteClosed(WriteQueueGracefulClose)){
		// closed before stream end
		fWriteQueue->StopWrite(false);
		fWriteQueue=nullptr;

		SetWriteEnd(WriteQueueGracefulClose);
		// stream terminated
		return;
	}

	cMemory WriteBuffer;
	while((WriteBuffer=fWriteQueue->ReserveWriteBuffer(0)).Length!=0){

		if(fWriteTask==nullptr){
			fWriteTask=QueryWriteTask();
			if(fWriteTask==nullptr){
				fWriteQueue->CommitWriteBuffer(0);
				return;
			}
		}


		uIntn SizeWritten=ReadFromStream(WriteBuffer.Pointer,WriteBuffer.Length,fWriteTask.Pointer());
		fWriteQueue->CommitWriteBuffer(SizeWritten);

		if(fWriteTask->IsAccessBufferCompleted()){
			fWriteTask->AccessErrorCode=StreamError::Success;
			CompleteWriteTask(cnVar::MoveCast(fWriteTask));
		}

	}
	fWriteQueue->NotifyWrite(1);
}
//---------------------------------------------------------------------------
void cStreamFromEndpoint::StreamProcessWriteSetEnd(void)
{
	if(fWriteActive){
		fWriteQueue->StopWrite(false);
	}
}
//---------------------------------------------------------------------------
void cStreamFromEndpoint::StreamProcessWriteEnd(void)
{
	if(fWriteQueue!=nullptr){
		fWriteQueue->StopWrite(true);
		fWriteQueue=nullptr;
	}
}
//---------------------------------------------------------------------------
void cStreamFromEndpoint::WriteStarted(void)
{
	fWriteActive=true;
	fWriteQueue->NotifyWrite(1);
}
//---------------------------------------------------------------------------
void cStreamFromEndpoint::WriteStopped(void)
{
	fWriteActive=false;
}
//---------------------------------------------------------------------------
void cStreamFromEndpoint::WriteNotify(void)
{
	UpdateWriteTaskQueue();
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cPacketStreamFromEndpoint::cPacketStreamFromEndpoint()
{
}
//---------------------------------------------------------------------------
cPacketStreamFromEndpoint::~cPacketStreamFromEndpoint()
{
}
//---------------------------------------------------------------------------
void cPacketStreamFromEndpoint::VirtualStopped(void)
{
	StreamClose();
	cDualReference::VirtualStopped();
}
//---------------------------------------------------------------------------
void* cPacketStreamFromEndpoint::CastInterface(iTypeID InterfaceID)noexcept(true)
{
	void* Ret=bcStream::CastInterface(InterfaceID);
	if(Ret!=nullptr){
		return Ret;
	}
	return fEndpoint->CastInterface(InterfaceID);
}
//---------------------------------------------------------------------------
bool cPacketStreamFromEndpoint::Connect(iEndpoint *Endpoint)
{
	if(fReadQueue!=nullptr || fWriteQueue!=nullptr)
		return false;

	fEndpoint=Endpoint;
	fReadQueue=fEndpoint->GetReadQueue();
	if(fReadQueue->StartRead(&fInnerReference,this)==false){
		fReadQueue=nullptr;
		fEndpoint=nullptr;
		return false;
	}
	fWriteQueue=fEndpoint->GetWriteQueue();
	if(fWriteQueue->StartWrite(&fInnerReference,this)==false){
		fReadQueue->StopRead();
		fWriteQueue=nullptr;
		return false;
	}


	return true;
}
//---------------------------------------------------------------------------
void cPacketStreamFromEndpoint::StreamProcessReadTask(void)
{
	if(!fReadActive){
		return;
	}

	cMemory ReadQueueBuffer;
	while((ReadQueueBuffer=fReadQueue->GatherReadBuffer(0)).Length!=0){

		auto ReadTask=QueryReadTask();
		if(ReadTask==nullptr){
			fReadQueue->DismissReadBuffer(0);
			return;
		}
		
		uIntn SizeWritten=WriteToStream(ReadQueueBuffer.Pointer,ReadQueueBuffer.Length,ReadTask.Pointer());

		// Read Task Completed

		if(SizeWritten>=ReadQueueBuffer.Length){
			ReadTask->AccessErrorCode=StreamError::Success;
		}
		else{
			ReadTask->AccessErrorCode=StreamError::MoreData;
		}
		CompleteReadTask(cnVar::MoveCast(ReadTask));
		
		fReadQueue->DismissReadBuffer(ReadQueueBuffer.Length);
	}

	bool ReadQueueGracefulClose;
	if(fReadQueue->IsReadClosed(ReadQueueGracefulClose)){
		fReadQueue->StopRead();
		fReadQueue=nullptr;

		SetReadEnd(ReadQueueGracefulClose);
	}
	else{
		fReadQueue->NotifyRead(1);
	}
}
//---------------------------------------------------------------------------
void cPacketStreamFromEndpoint::StreamProcessReadEnd(void)
{
	if(fReadActive){
		fReadQueue->StopRead();
		fReadQueue=nullptr;
	}
}
//---------------------------------------------------------------------------
void cPacketStreamFromEndpoint::ReadStarted(void)
{
	fReadActive=true;
	fReadQueue->NotifyRead(1);
}
//---------------------------------------------------------------------------
void cPacketStreamFromEndpoint::ReadStopped(void)
{
	fReadActive=false;
}
//---------------------------------------------------------------------------
void cPacketStreamFromEndpoint::ReadNotify(void)
{
	UpdateReadTaskQueue();
}
//---------------------------------------------------------------------------
void cPacketStreamFromEndpoint::StreamProcessWriteTask(void)
{
	if(!fWriteActive){
		return;
	}

	bool WriteQueueGracefulClose;
	if(fWriteQueue->IsWriteClosed(WriteQueueGracefulClose)){
		// closed before stream end
		fWriteQueue->StopWrite(false);
		fWriteQueue=nullptr;

		SetWriteEnd(WriteQueueGracefulClose); 
		// stream terminated
		return;
	}

	cMemory WriteBuffer;
	while((WriteBuffer=fWriteQueue->ReserveWriteBuffer(0)).Length!=0){

		auto WriteTask=QueryWriteTask();
		if(WriteTask==nullptr){
			fWriteQueue->CommitWriteBuffer(0);
			return;
		}

		uIntn SizeRead=ReadFromStream(WriteBuffer.Pointer,WriteBuffer.Length,WriteTask.Pointer());

		fWriteQueue->CommitWriteBuffer(SizeRead);

		WriteTask->AccessErrorCode=StreamError::Success;
		CompleteWriteTask(cnVar::MoveCast(WriteTask));

	}
	fWriteQueue->NotifyWrite(1);
}
//---------------------------------------------------------------------------
void cPacketStreamFromEndpoint::StreamProcessWriteSetEnd(void)
{
	if(fWriteActive){
		fWriteQueue->StopWrite(false);
		fWriteQueue=nullptr;
	} 
}
//---------------------------------------------------------------------------
void cPacketStreamFromEndpoint::StreamProcessWriteEnd(void)
{
	if(fWriteActive){
		fWriteQueue->StopWrite(true);
		fWriteQueue=nullptr;
	}
}
//---------------------------------------------------------------------------
void cPacketStreamFromEndpoint::WriteStarted(void)
{
	fWriteActive=true;
	fWriteQueue->NotifyWrite(1);
}
//---------------------------------------------------------------------------
void cPacketStreamFromEndpoint::WriteStopped(void)
{
	fWriteActive=false;
}
//---------------------------------------------------------------------------
void cPacketStreamFromEndpoint::WriteNotify(void)
{
	UpdateWriteTaskQueue();
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
iPtr<iStream> cnRTL::CreateStreamFromEndpoint(iEndpoint *Endpoint,bool KeepBoundary)
{
	if(KeepBoundary){
		auto sStream=iCreate<cPacketStreamFromEndpoint>();
		sStream->Connect(Endpoint);
		return cnVar::MoveCast(sStream);
	}
	else{
		auto sStream=iCreate<cStreamFromEndpoint>();
		sStream->Connect(Endpoint);
		return cnVar::MoveCast(sStream);
	}
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
bcReadQueueFromStream::bcReadQueueFromStream(iStream *Stream,uIntn PacketBufferSize)
	: fReadItem{{nullptr,nullptr,0},{nullptr,nullptr,0}}
{
	fReadBufferSize=PacketBufferSize;
	
	fReadItem[0].Buffer=cnSystem::DefaultHeap::Alloc(fReadBufferSize,0);
	fReadItem[1].Buffer=cnSystem::DefaultHeap::Alloc(fReadBufferSize,0);
	cnLib_ASSERT(Stream!=nullptr);
	fStream=Stream;
	fCurReadOffset=0;

	fReadItemRing.Reset();
}
//---------------------------------------------------------------------------
bcReadQueueFromStream::~bcReadQueueFromStream()
{
	cnSystem::DefaultHeap::Free(fReadItem[0].Buffer,fReadBufferSize,0);
	cnSystem::DefaultHeap::Free(fReadItem[1].Buffer,fReadBufferSize,0);
}
//---------------------------------------------------------------------------
void bcReadQueueFromStream::NotificationStarted(void)
{
	bcReadQueue::NotificationStarted();

	OnStartRead();
}
//---------------------------------------------------------------------------
cConstMemory bcReadQueueFromStream::GatherReadBuffer(uIntn QuerySize)
{	UnusedParameter(QuerySize);	// ignore query size, just return whatever available

	uInt8 ItemIndex;
	if(fReadItemRing.ReserveItem<2>(ItemIndex)){
		auto &Item=fReadItem[ItemIndex];
		cnLib_ASSERT(Item.Task==nullptr);
		if(Item.Result==false){
			// error in stream
			// clear read buffers
	
			auto Count=fReadItemRing.Reserve<2>(ItemIndex,4);
			fReadItemRing.Commit<2>(Count);

			ReadQueueReportBufferEmpty();

			return NullArray;
		}
		cConstMemory Buffer;
		Buffer.Pointer=cnMemory::PointerAddByteOffset(Item.Buffer,fCurReadOffset);
		Buffer.Length=Item.SizeCompleted-fCurReadOffset;
		return Buffer;
	}
	// no buffer available
	return NullArray;
}
//---------------------------------------------------------------------------
void bcReadQueueFromStream::DismissReadBuffer(uIntn Size)
{
	fCurReadOffset+=Size;
	uInt8 ItemIndex;
	if(fReadItemRing.ReserveItem<2>(ItemIndex)){
		auto &Item=fReadItem[ItemIndex];
		if(fCurReadOffset>=Item.SizeCompleted){
			fCurReadOffset=0;
			fReadItemRing.CommitItem<2>();
			UpdateQueueState(false);
			OnStartRead();
		}
	}
}
//---------------------------------------------------------------------------
void bcReadQueueFromStream::NotificationClosed(void)
{
	bcReadQueue::NotificationClosed();

	for(auto &Item : fReadItem){
		if(Item.Task!=nullptr){
			Item.Task->Cancel();
		}
	}
}
//---------------------------------------------------------------------------
void bcReadQueueFromStream::OnStartRead(void)
{
	// start read
	while(fStream->IsEndOfReading()==false){
		uInt8 ItemIndex;
		if(fReadItemRing.ReserveItem<0>(ItemIndex)==false)
			return;
		auto &Item=fReadItem[ItemIndex];
		Item.SizeCompleted=0;
		auto Task=fStream->ReadAsync(Item.Buffer,fReadBufferSize);
		Item.Task=Task;
		if(Task!=nullptr){
			fReadItemRing.CommitItem<0>();

			auto Reference=NotificationInnerReference();
			if(Reference!=nullptr){
				rIncReference(Reference,'task');
			}
			if(Task->SetNotify(&fTaskProcedure)==false){
				ProcessTask();
				if(Reference!=nullptr){
					rDecReference(Reference,'task');
				}
			}
		}
		else{
			// error
			CloseQueue();
		}
	}
}
//---------------------------------------------------------------------------
void bcReadQueueFromStream::ProcessTask(void)
{
	if(fProcessTaskExclusiveFlag.Acquire()==false)
		return;

	bool MoreAvailable=false;
	bool StreamEnded=false;
	do{
		fProcessTaskExclusiveFlag.Continue();

		uInt8 ItemIndex;
		while(fReadItemRing.ReserveItem<1>(ItemIndex)){
			auto &Item=fReadItem[ItemIndex];
			if(Item.Task->IsDone()==false)
				break;	// not completed

			auto Task=MoveCast(Item.Task);
			Item.Result=Task->GetResult(Item.SizeCompleted);
			if(Item.Result==false){
				auto ErrorCode=GetStreamError(Task);
				switch(ErrorCode){
				case StreamError::Success:
					break;
				case StreamError::Cancelled:
					// cancelled
					break;
				default:	// other error
					// stop source
					CloseQueue();
					break;
				}
			}
			if(Item.SizeCompleted==0 && fStream->IsEndOfReading()){
				// reached end of read stream
				StreamEnded=true;
				Item.Result=false;
			}
			else{
				MoreAvailable=true;
			}
			fReadItemRing.CommitItem<1>();
		}
	
	}while(fProcessTaskExclusiveFlag.Release()==false);
	if(MoreAvailable){
		ReadQueueReportBufferAvailable(false);
	}
	if(StreamEnded){
		ReadQueueReportEnded();
	}
}
//---------------------------------------------------------------------------
void bcReadQueueFromStream::cTaskProcedure::Execute(void)
{
	auto Host=cnMemory::GetObjectFromMemberPointer(this,&bcReadQueueFromStream::fTaskProcedure);
	Host->ProcessTask();
	auto Reference=Host->NotificationInnerReference();
	if(Reference!=nullptr){
		rDecReference(Reference,'task');
	}
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
bcWriteQueueFromStream::bcWriteQueueFromStream(iStream *Stream)
{
	cnLib_ASSERT(Stream!=nullptr);
	fStream=Stream;

	fWriteItemRing.Reset();

	for(auto &Item : fWriteItem){
		Item.SourceBuffer=cnSystem::DefaultHeap::Alloc(SourceBufferTotalSize,0);
		Item.SourceBufferSize=0;
	}
	WriteQueueReportBufferAvailable(false);
}
//---------------------------------------------------------------------------
bcWriteQueueFromStream::~bcWriteQueueFromStream()
{
	for(auto &Item : fWriteItem){
		cnSystem::DefaultHeap::Free(Item.SourceBuffer,SourceBufferTotalSize,0);
	}
}
//---------------------------------------------------------------------------
void bcWriteQueueFromStream::NotificationClosed(void)
{
	bcWriteQueue::NotificationClosed();

	for(auto &Item : fWriteItem){
		if(Item.Task!=nullptr){
			Item.Task->Cancel();
		}
	}

	if(IsNotificationEnded()){
		fStream->SetEndOfStream();
	}
}
//---------------------------------------------------------------------------
cMemory bcWriteQueueFromStream::ReserveWriteBuffer(uIntn QuerySize)
{	UnusedParameter(QuerySize);	// ignore query size, just return whatever
#pragma message (cnLib_FILE_LINE ": TODO dynamic expand buffer")

	uInt8 ItemIndex;
	if(fWriteItemRing.ReserveItem<0>(ItemIndex)==false){
		// no buffer for writing
		return NullArray;
	}
	auto &Item=fWriteItem[ItemIndex];
	cnLib_ASSERT(Item.SourceBufferSize==0);
	Item.SourceBufferSize=0;

	cMemory Buffer;
	Buffer.Pointer=Item.SourceBuffer;
	Buffer.Length=SourceBufferTotalSize;
	return Buffer;
}
//---------------------------------------------------------------------------
void bcWriteQueueFromStream::CommitWriteBuffer(uIntn Size)
{
	uInt8 ItemIndex;
	if(fWriteItemRing.ReserveItem<0>(ItemIndex)==false){
		// no buffer
		return;
	}
	auto &Item=fWriteItem[ItemIndex];
	Item.SourceBufferSize=Size;

	SendWriteBuffer(Item);
}
//---------------------------------------------------------------------------
void bcWriteQueueFromStream::SendWriteBuffer(cWriteItem &Item)
{
	// send to write
	auto Task=fStream->WriteAsync(Item.SourceBuffer,Item.SourceBufferSize);
	Item.Task=Task;
			
	if(Task!=nullptr){
		fWriteItemRing.CommitItem<0>();
		auto Reference=NotificationInnerReference();
		if(Reference!=nullptr){
			rIncReference(Reference,'task');
		}
		if(Task->SetNotify(&fTaskProcedure)==false){
			// already done
			ProcessTask();

			if(Reference!=nullptr){
				rDecReference(Reference,'task');
			}
			return;
		}
	}
	else{
		// error, close
		Item.SourceBufferSize=0;
		CloseQueue();
	}
}
//---------------------------------------------------------------------------
void bcWriteQueueFromStream::ProcessTask(void)
{
	if(fProcessTaskExclusiveFlag.Acquire()==false)
		return;

	bool MoreAvailable=false;
	do{
		fProcessTaskExclusiveFlag.Continue();

		uInt8 ItemIndex;
		while(fWriteItemRing.ReserveItem<1>(ItemIndex)){
			auto &Item=fWriteItem[ItemIndex];
			if(Item.Task->IsDone()==false)
				break;
			Item.SourceBufferSize=0;
			// get result
			uIntn SizeCompleted;
			auto Result=Item.Task->GetResult(SizeCompleted);
			if(Result==false){
				// error
				CloseQueue();
			}
			Item.Task=nullptr;
			fWriteItemRing.CommitItem<1>();
			MoreAvailable=true;
		}
	}while(fProcessTaskExclusiveFlag.Release()==false);
	if(MoreAvailable){
		WriteQueueReportBufferAvailable(false);
	}
}
//---------------------------------------------------------------------------
void bcWriteQueueFromStream::cTaskProcedure::Execute(void)
{
	auto Host=cnMemory::GetObjectFromMemberPointer(this,&bcWriteQueueFromStream::fTaskProcedure);

	Host->ProcessTask();

	auto Reference=Host->NotificationInnerReference();
	if(Reference!=nullptr){
		rDecReference(Reference,'task');
	}
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cEndpointFromStream::cReadQueue::cReadQueue(iStream *Stream,uIntn PacketBufferSize,iThreadPool *ThreadPool)
	: bcReadQueueFromStream(Stream,PacketBufferSize)
{
	if(ThreadPool==nullptr){
		ThreadPool=cnSystem::DefaultThreadPool;
	}
	CreateQueueWork(ThreadPool,false);
}
//---------------------------------------------------------------------------
cEndpointFromStream* cEndpointFromStream::cReadQueue::GetHost(void)
{
	return cnMemory::GetObjectFromMemberPointer(this,&cEndpointFromStream::fReadQueue);
}
//---------------------------------------------------------------------------
void cEndpointFromStream::cReadQueue::IncreaseReference(void)noexcept(true)
{
	rIncReference(GetHost(),'read');
}
void cEndpointFromStream::cReadQueue::DecreaseReference(void)noexcept(true)
{
	rDecReference(GetHost(),'read');
}
//---------------------------------------------------------------------------
iReference* cEndpointFromStream::cReadQueue::NotificationInnerReference(void)
{
	return &GetHost()->fInnerReference;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cEndpointFromStream::cWriteQueue::cWriteQueue(iStream *Stream,iThreadPool *ThreadPool)
	: bcWriteQueueFromStream(Stream)
{
	if(ThreadPool==nullptr){
		ThreadPool=cnSystem::DefaultThreadPool;
	}
	CreateQueueWork(ThreadPool,false);
}
//---------------------------------------------------------------------------
cEndpointFromStream* cEndpointFromStream::cWriteQueue::GetHost(void)
{
	return cnMemory::GetObjectFromMemberPointer(this,&cEndpointFromStream::fWriteQueue);
}
//---------------------------------------------------------------------------
void cEndpointFromStream::cWriteQueue::IncreaseReference(void)noexcept(true)
{
	rIncReference(GetHost(),'writ');
}
void cEndpointFromStream::cWriteQueue::DecreaseReference(void)noexcept(true)
{
	rDecReference(GetHost(),'writ');
}
//---------------------------------------------------------------------------
iReference* cEndpointFromStream::cWriteQueue::NotificationInnerReference(void)
{
	return &GetHost()->fInnerReference;
}
//---------------------------------------------------------------------------
void cEndpointFromStream::cWriteQueue::NotificationClosed(void)
{
	bcWriteQueueFromStream::NotificationClosed();

	if(IsNotificationEnded()==false){
		// need to terminate read queue as well
		GetHost()->WriteQueueTerminated();
	}
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cEndpointFromStream::cEndpointFromStream(rPtr<iReference> StreamReference,iStream *Stream,uIntn ReadBufferSize,iThreadPool *ThreadPool)
	: fStreamReference(cnVar::MoveCast(StreamReference))
	, fReadQueue(Stream,ReadBufferSize,ThreadPool)
	, fWriteQueue(Stream,ThreadPool)
{
}
//---------------------------------------------------------------------------
cEndpointFromStream::~cEndpointFromStream()
{
}
//---------------------------------------------------------------------------
void cEndpointFromStream::VirtualStopped(void)
{
	Close();
	InnerDecReference('self');
}
//---------------------------------------------------------------------------
void cEndpointFromStream::Close(void)
{
	fReadQueue.CloseQueue();
	fWriteQueue.CloseQueue();
}
//---------------------------------------------------------------------------
iReadQueue* cEndpointFromStream::GetReadQueue(void)
{
	return &fReadQueue;
}
//---------------------------------------------------------------------------
iWriteQueue* cEndpointFromStream::GetWriteQueue(void)
{
	return &fWriteQueue;
}
//---------------------------------------------------------------------------
void cEndpointFromStream::SetWriteEndMode(eEndpointWriteEndMode EndMode)
{
	fWriteQueue.WriteQueueSetEndMode(EndMode);
}
//---------------------------------------------------------------------------
void cEndpointFromStream::WriteQueueTerminated(void)
{
	fReadQueue.CloseQueue();
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
iPtr<iEndpoint> cnRTL::CreateEndpointFromSteam(iPtr<iStream> Stream,uIntn ReadBufferSize,iThreadPool *ThreadPool)
{
	iReference *StreamRef=iGetReference(Stream);
	iStream *StreamPtr=Stream.ExtractToManual();
	return iCreate<cEndpointFromStream>(rPtr<iReference>::TakeFromManual(StreamRef),StreamPtr,ReadBufferSize,ThreadPool);
}
//---------------------------------------------------------------------------
//iPtr<iEndpoint> cnRTL::CreateEndpointFromSteam(bcAggregable *StreamAggregable,iStream *Stream,uIntn ReadBufferSize,iThreadPool *ThreadPool)
//{
//	//auto AggregableStream=iAggregableCreate<cSocketStreamInnerRef>(cnVar::MoveCast(SocketIO),cnVar::MoveCast(LocalAddress),cnVar::MoveCast(RemoteAddress));
//	auto AggregableEndpoint=iAggregableCreate<cEndpointFromStream>(AggregableStream->GetInnerReference(),Stream);
//	auto ConnectionAgg=iAggregateCreate(AggregableStream,AggregableEndpoint);
//	return iCast<iConnection>(ConnectionAgg);
//}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cConnectionQueueFromListener::cConnectionQueueFromListener(iConnectionListener *Listener)
	: fListener(Listener)
{
	for(uIntn i=0;i<cnMemory::ArrayLength(fAcceptTasks);i++){
		fAcceptTasks[i].TaskIndex=i;
	}
}
//---------------------------------------------------------------------------
cConnectionQueueFromListener::~cConnectionQueueFromListener()
{
}
//---------------------------------------------------------------------------
void cConnectionQueueFromListener::VirtualStopped(void)
{
	CloseQueue();

	InnerDecReference('self');
}
//---------------------------------------------------------------------------
iAddress* cConnectionQueueFromListener::GetLocalAddress(void)
{
	return fListener->GetLocalAddress();
}
//---------------------------------------------------------------------------
iPtr<iConnection> cConnectionQueueFromListener::FetchConnection(void)
{
	uInt8 TaskItemIndex;
	if(fTaskRingIndex.ReserveItem<2>(TaskItemIndex)==false){
		return nullptr;
	}
	auto &TaskItem=fAcceptTasks[TaskItemIndex];
	cnLib_ASSERT(TaskItem.AcceptTask!=nullptr);
	cnLib_ASSERT(TaskItem.AcceptTask->IsDone());

	iPtr<iConnection> Connection=TaskItem.AcceptTask->GetConnection();
	TaskItem.AcceptTask=nullptr;

	fTaskRingIndex.CommitItem<2>();

	QueueTask();
	return Connection;
}
//---------------------------------------------------------------------------
iReference* cConnectionQueueFromListener::NotificationInnerReference(void)
{
	return &fInnerReference;
}
//---------------------------------------------------------------------------
void cConnectionQueueFromListener::NotificationStarted(void)
{
	bcConnectionQueue::NotificationStarted();
	QueueTask();
}
//---------------------------------------------------------------------------
void cConnectionQueueFromListener::NotificationStopped(void)
{
	bcConnectionQueue::NotificationStopped();
}
//---------------------------------------------------------------------------
void cConnectionQueueFromListener::NotificationClosed(void)
{
	bcConnectionQueue::NotificationClosed();

	for(auto &Item : fAcceptTasks){
		if(Item.AcceptTask!=nullptr){
			Item.AcceptTask->Cancel();
		}
	}
}
//---------------------------------------------------------------------------
cConnectionQueueFromListener::CycleState cConnectionQueueFromListener::NotificationCheckState(void)
{
	if(fQueueTerminated){
		return CycleState::Terminated;
	}
	return CycleState::Normal;

}
//---------------------------------------------------------------------------
void cConnectionQueueFromListener::QueueTask(void)
{
	if(fTaskQueueExclusiveFlag.Acquire()==false)
		return;

	do{
		fTaskQueueExclusiveFlag.Continue();

		if(IsNotificationClosed()==false){

			uInt8 TaskItemIndex;
			if(fTaskRingIndex.ReserveItem<0>(TaskItemIndex)){
				auto &TaskItem=fAcceptTasks[TaskItemIndex];
				TaskItem.AcceptTask=fListener->AcceptAsync();

				if(TaskItem.AcceptTask!=nullptr){
					fTaskRingIndex.CommitItem<0>();

					auto Reference=NotificationInnerReference();
					if(Reference!=nullptr){
						rIncReference(Reference,'task');
					}

					if(TaskItem.AcceptTask->SetNotify(&TaskItem)==false){
						UpdateQueueState(false);
						if(Reference!=nullptr){
							rDecReference(Reference,'task');
						}
					}

				}
				else{
					// error
					fQueueTerminated=true;
					UpdateQueueState(false);
				}
			}
		}
		

	}while(fTaskQueueExclusiveFlag.Release()==false);
}
//---------------------------------------------------------------------------
void cConnectionQueueFromListener::TaskCompletionProcess(void)
{
	if(fTaskCompletionExclusiveFlag.Acquire()==false)
		return;
	bool AnyAvailable=false;
	do{
		fTaskCompletionExclusiveFlag.Continue();

		uInt8 TaskItemIndex;
		while(fTaskRingIndex.ReserveItem<1>(TaskItemIndex)){
			auto &TaskItem=fAcceptTasks[TaskItemIndex];
			cnLib_ASSERT(TaskItem.AcceptTask!=nullptr);

			if(TaskItem.AcceptTask->IsDone()==false){
				break;
			}

			fTaskRingIndex.CommitItem<1>();
			AnyAvailable=true;
		}
	}while(fTaskCompletionExclusiveFlag.Release()==false);

	if(AnyAvailable){
		AsyncQueueSetAvailable(false);
	}
}
//---------------------------------------------------------------------------
void cConnectionQueueFromListener::cTaskItem::Execute(void)
{
	auto *pAcceptTasks=reinterpret_cast<cTaskItem(*)[TaskRingSize]>(this-TaskIndex);
	auto Host=cnMemory::GetObjectFromMemberPointer(pAcceptTasks,&cConnectionQueueFromListener::fAcceptTasks);

	Host->TaskCompletionProcess();
	auto Reference=Host->NotificationInnerReference();
	if(Reference!=nullptr){
		rDecReference(Reference,'task');
	}
}
//---------------------------------------------------------------------------
cConnectionListenerFromQueue::cConnectionListenerFromQueue(iConnectionQueue *ConnectionQueue)
	: bcConnectionQueueProcessor(ConnectionQueue)
{
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cConnectionListenerFromQueue::~cConnectionListenerFromQueue()
{
}
//---------------------------------------------------------------------------
void cConnectionListenerFromQueue::VirtualStarted(void)
{
	InnerIncReference('self');
	Start(&fInnerReference);
}
//---------------------------------------------------------------------------
void cConnectionListenerFromQueue::VirtualStopped(void)
{
	Stop();
	InnerDecReference('self');
}
//---------------------------------------------------------------------------
iAddress* cConnectionListenerFromQueue::GetLocalAddress(void)
{
	return fConnectionQueue->GetLocalAddress();
}
//---------------------------------------------------------------------------
void cConnectionListenerFromQueue::AsyncNotify(void)
{
	UpdateAcceptTaskQueue();
}
//---------------------------------------------------------------------------
void cConnectionListenerFromQueue::ConnectionProcessTask(void)
{
	if(fCurrentConnection==nullptr){
		fCurrentConnection=fConnectionQueue->FetchConnection();
		if(fCurrentConnection==nullptr)
			return;
	}

	//rPtr<cAcceptTask> AcceptingTask;
	auto AcceptingTask=QueryAcceptTask();
	if(AcceptingTask==nullptr)
		return;

	AcceptingTask->Connection=cnVar::MoveCast(fCurrentConnection);

	CompleteAcceptTask(cnVar::MoveCast(AcceptingTask));
}
//---------------------------------------------------------------------------
void cConnectionListenerFromQueue::ConnectionProcessEnd(void)
{
	Stop();
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cMultipointQueueFromStream::cMultipointQueueFromStream(iMultipointStream *Stream)
	: fStream(Stream)
{
	//fPacketSizeMax=fPortStream->GetMaxPacketSize();
	fPacketSizeMax=SendBufferSize;
	
	// prepare receive item
	for(uIntn i=0;i<fReceiveItemCount;i++){
		auto &Item=fReceiveItem[i];
		Item.Owner=this;
		Item.ReadBuffer=cnSystem::DefaultHeap::Alloc(fPacketSizeMax,0);
		Item.State=cReceiveItem::sIdle;
	}
	// prepare receive item
	for(uIntn i=0;i<fSendItemCount;i++){
		auto &Item=fSendItem[i];
		Item.Owner=this;
		Item.WriteBuffer=cnSystem::DefaultHeap::Alloc(fPacketSizeMax,0);
		Item.State=cSendItem::sIdle;
	}


	// stream io work
	fIOWork=cnSystem::DefaultThreadPool->CreateWork(&fInnerReference,fIOProcedure);
}
//---------------------------------------------------------------------------
cMultipointQueueFromStream::~cMultipointQueueFromStream()
{
	for(uIntn i=0;i<fReceiveItemCount;i++){
		cnSystem::DefaultHeap::Free(fReceiveItem[i].ReadBuffer,fPacketSizeMax,0);
	}
	for(uIntn i=0;i<fSendItemCount;i++){
		cnSystem::DefaultHeap::Free(fSendItem[i].WriteBuffer,fPacketSizeMax,0);
	}
}
//---------------------------------------------------------------------------
void cMultipointQueueFromStream::VirtualStarted(void)
{
	CreateMultipointQueueStateWriteWork(cnSystem::DefaultThreadPool,false);
}
//---------------------------------------------------------------------------
void cMultipointQueueFromStream::VirtualStopped(void)
{
	CloseMultipointQueue();
	InnerDecReference('self');
}
//---------------------------------------------------------------------------
iReference* cMultipointQueueFromStream::MultipointQueueInnerReference(void)
{
	return &fInnerReference;
}
//---------------------------------------------------------------------------
void cMultipointQueueFromStream::MultipointQueueStarted(void)
{
	bcMultipointQueue::MultipointQueueStarted();
	fIOProcedure.RunAsync(fIOWork);
}
//---------------------------------------------------------------------------
iAddress*	cMultipointQueueFromStream::GetLocalAddress(void)
{
	return fStream->GetLocalAddress();
}
//---------------------------------------------------------------------------
cConstMemory cMultipointQueueFromStream::GatherReadBuffer(iPtr<iAddress> &Address)
{
	if(MultipointQueueIsClosed())
		return NullArray;
	for(uIntn i=0;i<fReceiveItemCount;i++){
		auto &Item=fReceiveItem[i];
		if(Item.State==cReceiveItem::sPush){
			if(Item.Task!=nullptr){
				uIntn SizeCompleted;
				if(Item.Task->GetResult(SizeCompleted)==false){
					// receive error
					CloseMultipointQueue();
					return NullArray;
				}
				// send to endpoint
				Address=Item.Task->GetRemoteAddress();
				cConstMemory Memory;
				Memory.Pointer=Item.ReadBuffer;
				Memory.Length=SizeCompleted;

				fCurReceiveItemIndex=i;
				return Memory;
			}
		}
	}
	return NullArray;
}
//---------------------------------------------------------------------------
void cMultipointQueueFromStream::DismissReadBuffer(bool Dismiss)
{
	if(Dismiss==false)
		return;

	auto &Item=fReceiveItem[fCurReceiveItemIndex];
	Item.Task=nullptr;
	Item.State=cReceiveItem::sIdle;

	fIOProcedure.Run();
}
//---------------------------------------------------------------------------
cMemory cMultipointQueueFromStream::ReserveWriteBuffer(uIntn)
{
	if(MultipointQueueIsClosed())
		return NullArray;
	for(uIntn i=0;i<fSendItemCount;i++){
		auto &Item=fSendItem[i];

		if(Item.State==cSendItem::sIdle){

			cMemory Memory;
			Memory.Pointer=Item.WriteBuffer;
			Memory.Length=SendBufferSize;
			fCurSendItemIndex=i;
			return Memory;
		}
	}

	return NullArray;
}
//---------------------------------------------------------------------------
bool cMultipointQueueFromStream::CommitWriteBuffer(iAddress *Address,uIntn Length)
{
	auto &Item=fSendItem[fCurSendItemIndex];

	Item.Task=fStream->WriteAsync(Item.WriteBuffer,Length,Address);
	if(Item.Task==nullptr){
		// send error
		CloseMultipointQueue();
		return false;
	}

	Item.State=cSendItem::sSend;
	SendItemSetNotify(Item);
	return true;
}
//---------------------------------------------------------------------------
bool cMultipointQueueFromStream::cIOProcedure::Procedure(void)
{
	auto Host=cnMemory::GetObjectFromMemberPointer(this,&cMultipointQueueFromStream::fIOProcedure);
	return Host->IOProcedure();
}
//---------------------------------------------------------------------------
bool cMultipointQueueFromStream::IOProcedure(void)
{
	if(MultipointQueueIsClosed())
		return false;
	// Check send tasks

	for(uIntn i=0;i<fSendItemCount;i++){
		auto &Item=fSendItem[i];
		if(Item.State==cSendItem::sCompleted){
			uIntn SizeCompleted;
			auto Result=Item.Task->GetResult(SizeCompleted);
			if(Result==false){
				// error
				CloseMultipointQueue();
				return false;
			}
			Item.State=cSendItem::sIdle;
		}
	}

	// start recv tasks

	for(uIntn i=0;i<fReceiveItemCount;i++){
		auto &Item=fReceiveItem[i];
		if(Item.Task==nullptr){
			Item.Task=fStream->ReadAsync(Item.ReadBuffer,fPacketSizeMax);
			if(Item.Task==nullptr){
				// receive error
				CloseMultipointQueue();
				break;
			}
			ReceiveItemSetNotify(Item);

		}
	}
	return false;
}
//---------------------------------------------------------------------------
void cMultipointQueueFromStream::cSendItem::Execute(void)
{
	Owner->SendItemCompletionNotify(this);
}
//---------------------------------------------------------------------------
void cMultipointQueueFromStream::SendItemSetNotify(cSendItem &Item)
{
	InnerIncReference('stsk');
	if(Item.Task->SetNotify(&Item)==false){
		SendItemCompletionNotify(&Item);
	}
}
//---------------------------------------------------------------------------
void cMultipointQueueFromStream::SendItemCompletionNotify(cSendItem *Item)
{
	Item->State=cSendItem::sCompleted;

	// continue push
	MultipointQueueSetWriteBufferAvailable(false);
	InnerDecReference('stsk');
}
//---------------------------------------------------------------------------
void cMultipointQueueFromStream::cReceiveItem::Execute(void)
{
	Owner->ReceiveItemCompletionNotify(this);
}
//---------------------------------------------------------------------------
void cMultipointQueueFromStream::ReceiveItemSetNotify(cReceiveItem &Item)
{
	InnerIncReference('rtsk');
	if(Item.Task->SetNotify(&Item)==false){
		ReceiveItemCompletionNotify(&Item);
	}
}
//---------------------------------------------------------------------------
void cMultipointQueueFromStream::ReceiveItemCompletionNotify(cReceiveItem *Item)
{
	Item->State=cReceiveItem::sPush;

	// continue push
	MultipointQueueSetReadBufferAvailable(false);
	InnerDecReference('rtsk');
}
//---------------------------------------------------------------------------
