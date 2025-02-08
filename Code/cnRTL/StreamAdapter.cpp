#include "StreamAdapter.h"
//---------------------------------------------------------------------------
using namespace cnLibrary;
using namespace cnRTL;

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
eStreamError cnRTL::GetStreamError(iInterface *Stream)noexcept(true)
{
	//auto ErrorReport=iCast<iStreamErrorReport>(Stream);
	//if(ErrorReport==nullptr){
		return StreamError::Unknown;
	//}
	//return ErrorReport->GetStreamError();
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cStreamFromEndpoint::cStreamFromEndpoint()noexcept(true)
{
}
//---------------------------------------------------------------------------
cStreamFromEndpoint::~cStreamFromEndpoint()noexcept(true)
{
}
//---------------------------------------------------------------------------
void cStreamFromEndpoint::VirtualStopped(void)noexcept(true)
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
bool cStreamFromEndpoint::Connect(iEndpoint *Endpoint)noexcept(true)
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
void cStreamFromEndpoint::StreamProcessReadTask(void)noexcept(true)
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
void cStreamFromEndpoint::StreamProcessReadEnd(void)noexcept(true)
{
	if(fReadQueue!=nullptr){
		fReadQueue->StopRead();
		fReadQueue=nullptr;
	}
}
//---------------------------------------------------------------------------
void cStreamFromEndpoint::ReadStarted(void)noexcept(true)
{
	fReadActive=true;
	fReadQueue->NotifyRead(1);
}
//---------------------------------------------------------------------------
void cStreamFromEndpoint::ReadStopped(void)noexcept(true)
{
	fReadActive=false;
}
//---------------------------------------------------------------------------
void cStreamFromEndpoint::ReadNotify(void)noexcept(true)
{
	UpdateReadTaskQueue();
}
//---------------------------------------------------------------------------
void cStreamFromEndpoint::StreamProcessWriteTask(void)noexcept(true)
{
	if(!fWriteActive){
		return;
	}

	bool WriteQueueGracefulClose;
	if(fWriteQueue->IsWriteClosed(WriteQueueGracefulClose)){
		// closed before stream end
		fWriteQueue->StopWrite();
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
void cStreamFromEndpoint::StreamProcessWriteSetEnd(void)noexcept(true)
{
	if(fWriteActive){
		fWriteQueue->StopWrite();
	}
}
//---------------------------------------------------------------------------
void cStreamFromEndpoint::StreamProcessWriteEnd(void)noexcept(true)
{
	if(fWriteQueue!=nullptr){
		fWriteQueue->StopWrite();
		fWriteQueue->CloseWrite(true);
		fWriteQueue=nullptr;
	}
}
//---------------------------------------------------------------------------
void cStreamFromEndpoint::WriteStarted(void)noexcept(true)
{
	fWriteActive=true;
	fWriteQueue->NotifyWrite(1);
}
//---------------------------------------------------------------------------
void cStreamFromEndpoint::WriteStopped(void)noexcept(true)
{
	fWriteActive=false;
}
//---------------------------------------------------------------------------
void cStreamFromEndpoint::WriteNotify(void)noexcept(true)
{
	UpdateWriteTaskQueue();
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cPacketStreamFromEndpoint::cPacketStreamFromEndpoint()noexcept(true)
{
}
//---------------------------------------------------------------------------
cPacketStreamFromEndpoint::~cPacketStreamFromEndpoint()noexcept(true)
{
}
//---------------------------------------------------------------------------
void cPacketStreamFromEndpoint::VirtualStopped(void)noexcept(true)
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
bool cPacketStreamFromEndpoint::Connect(iEndpoint *Endpoint)noexcept(true)
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
void cPacketStreamFromEndpoint::StreamProcessReadTask(void)noexcept(true)
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
void cPacketStreamFromEndpoint::StreamProcessReadEnd(void)noexcept(true)
{
	if(fReadActive){
		fReadQueue->StopRead();
		fReadQueue=nullptr;
	}
}
//---------------------------------------------------------------------------
void cPacketStreamFromEndpoint::ReadStarted(void)noexcept(true)
{
	fReadActive=true;
	fReadQueue->NotifyRead(1);
}
//---------------------------------------------------------------------------
void cPacketStreamFromEndpoint::ReadStopped(void)noexcept(true)
{
	fReadActive=false;
}
//---------------------------------------------------------------------------
void cPacketStreamFromEndpoint::ReadNotify(void)noexcept(true)
{
	UpdateReadTaskQueue();
}
//---------------------------------------------------------------------------
void cPacketStreamFromEndpoint::StreamProcessWriteTask(void)noexcept(true)
{
	if(!fWriteActive){
		return;
	}

	bool WriteQueueGracefulClose;
	if(fWriteQueue->IsWriteClosed(WriteQueueGracefulClose)){
		// closed before stream end
		fWriteQueue->StopWrite();
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
void cPacketStreamFromEndpoint::StreamProcessWriteSetEnd(void)noexcept(true)
{
	if(fWriteActive){
		fWriteQueue->StopWrite();
		fWriteQueue=nullptr;
	} 
}
//---------------------------------------------------------------------------
void cPacketStreamFromEndpoint::StreamProcessWriteEnd(void)noexcept(true)
{
	if(fWriteActive){
		fWriteQueue->StopWrite();
		fWriteQueue=nullptr;
	}
}
//---------------------------------------------------------------------------
void cPacketStreamFromEndpoint::WriteStarted(void)noexcept(true)
{
	fWriteActive=true;
	fWriteQueue->NotifyWrite(1);
}
//---------------------------------------------------------------------------
void cPacketStreamFromEndpoint::WriteStopped(void)noexcept(true)
{
	fWriteActive=false;
}
//---------------------------------------------------------------------------
void cPacketStreamFromEndpoint::WriteNotify(void)noexcept(true)
{
	UpdateWriteTaskQueue();
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
iPtr<iStream> cnRTL::CreateStreamFromEndpoint(iEndpoint *Endpoint,bool KeepBoundary)noexcept(true)
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
bcReadQueueFromStream::bcReadQueueFromStream(iStream *Stream,uIntn PacketBufferSize)noexcept(true)
	: fReadItem{{nullptr,nullptr,0},{nullptr,nullptr,0}}
{
	fReadBufferSize=PacketBufferSize;
	
	fReadItem[0].Buffer=cnSystem::DefaultHeap::Alloc(fReadBufferSize,0);
	fReadItem[1].Buffer=cnSystem::DefaultHeap::Alloc(fReadBufferSize,0);
	cnLib_ASSERT(Stream!=nullptr);
	fStream=Stream;

	fReadItemRing.Reset();
}
//---------------------------------------------------------------------------
bcReadQueueFromStream::~bcReadQueueFromStream()noexcept(true)
{
	cnSystem::DefaultHeap::Free(fReadItem[0].Buffer,fReadBufferSize,0);
	cnSystem::DefaultHeap::Free(fReadItem[1].Buffer,fReadBufferSize,0);
}
//---------------------------------------------------------------------------
void bcReadQueueFromStream::NotificationStarted(void)noexcept(true)
{
	bcReadQueue::NotificationStarted();

	OnStartRead();
}
//---------------------------------------------------------------------------
uIntn bcReadQueueFromStream::GetMaxReadBufferSize(void)noexcept(true)
{
	return fReadBufferSize;
}
//---------------------------------------------------------------------------
cConstMemory bcReadQueueFromStream::GatherReadBuffer(uIntn QuerySize)noexcept(true)
{
	ufInt8 ItemIndex;
	ufInt8 ItemCount=fReadItemRing.Reserve<2>(ItemIndex,2);
	if(ItemCount==0){
		// no buffer available
		return NullArray;
	}
	auto &Item=fReadItem[ItemIndex];

	cnLib_ASSERT(Item.Task==nullptr);
		
	cConstMemory Buffer;
	Buffer.Pointer=cnMemory::PointerAddByteOffset(Item.Buffer,Item.ValidOffset);
	if(ItemCount==1 || QuerySize<=Item.ValidSize){
		Buffer.Length=Item.ValidSize;
		return Buffer;
	}
	// try to merge buffer
	auto &NextItem=fReadItem[!ItemIndex];
	
	if(Item.ValidOffset!=0){
		cnMemory::Copy(Item.Buffer,Buffer.Pointer,Item.ValidSize);
		Buffer.Pointer=Item.Buffer;
	}
	uIntn MergedSize=Item.ValidSize+NextItem.ValidSize;
	if(MergedSize<=fReadBufferSize){
		cnMemory::Copy(cnMemory::PointerAddByteOffset(Item.Buffer,Item.ValidSize),cnMemory::PointerAddByteOffset(NextItem.Buffer,NextItem.ValidOffset),NextItem.ValidSize);
		Buffer.Length=MergedSize;

		void *SwitchBuffer=NextItem.Buffer;
		// switch and release empty buffer
		NextItem.Buffer=Item.Buffer;
		NextItem.ValidOffset=0;
		NextItem.ValidSize=MergedSize;
	
		Item.Buffer=SwitchBuffer;
		
		fReadItemRing.Commit<2>(1);
		// notify continue read
		OnStartRead();
	}
	else{
		uIntn ReadNextSize=fReadBufferSize-Item.ValidSize;
		cnMemory::Copy(cnMemory::PointerAddByteOffset(Item.Buffer,Item.ValidSize),cnMemory::PointerAddByteOffset(NextItem.Buffer,NextItem.ValidOffset),ReadNextSize);
		Item.ValidSize=fReadBufferSize;
		Buffer.Length=fReadBufferSize;

		NextItem.ValidOffset+=ReadNextSize;
		NextItem.ValidSize-=ReadNextSize;
	}

	return Buffer;
}
//---------------------------------------------------------------------------
void bcReadQueueFromStream::DismissReadBuffer(uIntn Size)noexcept(true)
{
	uInt8 ItemIndex;
	if(fReadItemRing.ReserveItem<2>(ItemIndex)==false)
		return;

	auto &Item=fReadItem[ItemIndex];
	if(Size<Item.ValidSize){
		// advance read index
		Item.ValidOffset+=Size;
		Item.ValidSize-=Size;
	}
	else{
		// release buffer
		fReadItemRing.CommitItem<2>();
		UpdateQueueState(false);
		OnStartRead();

	}
}
//---------------------------------------------------------------------------
void bcReadQueueFromStream::NotificationClosed(void)noexcept(true)
{
	bcReadQueue::NotificationClosed();

	for(auto &Item : fReadItem){
		if(Item.Task!=nullptr){
			Item.Task->Cancel();
		}
	}
}
//---------------------------------------------------------------------------
void bcReadQueueFromStream::OnStartRead(void)noexcept(true)
{
	// start read
	while(fStream->IsEndOfReading()==false){
		uInt8 ItemIndex;
		if(fReadItemRing.ReserveItem<0>(ItemIndex)==false)
			return;
		auto &Item=fReadItem[ItemIndex];
		Item.ValidOffset=0;
		Item.ValidSize=0;
		auto Task=fStream->ReadAsync(Item.Buffer,fReadBufferSize);
		Item.Task=Task;
		if(Task!=nullptr){
			fReadItemRing.CommitItem<0>();

			auto Reference=NotificationInnerReference();
			if(Reference!=nullptr){
				rIncReference(Reference,'task');
			}
			if(Task->Await(&fTaskProcedure)==false){
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
void bcReadQueueFromStream::ProcessTask(void)noexcept(true)
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
			bool Result=Task->GetResult(Item.ValidSize);
			if(Result==false){
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
			if(Item.ValidSize==0 && fStream->IsEndOfReading()){
				// reached end of read stream
				StreamEnded=true;
			}
			else{
				fReadItemRing.CommitItem<1>();
				MoreAvailable=true;
			}
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
void bcReadQueueFromStream::cTaskProcedure::Execute(void)noexcept(true)
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
bcWriteQueueFromStream::bcWriteQueueFromStream(iStream *Stream)noexcept(true)
{
	cnLib_ASSERT(Stream!=nullptr);
	fStream=Stream;

	fWriteItemRing.Reset();

	WriteQueueReportBufferAvailable(false);
}
//---------------------------------------------------------------------------
bcWriteQueueFromStream::~bcWriteQueueFromStream()noexcept(true)
{
}
//---------------------------------------------------------------------------
void bcWriteQueueFromStream::NotificationClosed(void)noexcept(true)
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
uIntn bcWriteQueueFromStream::GetMaxWriteBufferSize(void)noexcept(true)
{
	return WriteBufferSizeLimit;
}
//---------------------------------------------------------------------------
cMemory bcWriteQueueFromStream::ReserveWriteBuffer(uIntn QuerySize)noexcept(true)
{	UnusedParameter(QuerySize);	// ignore query size, just return whatever

	uInt8 ItemIndex;
	if(fWriteItemRing.ReserveItem<0>(ItemIndex)==false){
		// no buffer for writing
		return NullArray;
	}
	auto &Item=fWriteItem[ItemIndex];
	cnLib_ASSERT(Item.SourceBufferSize==0);
	Item.SourceBufferSize=0;

	if(QuerySize>Item.SourceBuffer.Length){
		QuerySize=cAllocationOperator::RoundUpCapacity(QuerySize);
		if(QuerySize>WriteBufferSizeLimit){
			QuerySize=WriteBufferSizeLimit;
		}
		Item.SourceBuffer.SetLength(QuerySize);
	}

	cMemory Buffer;
	Buffer.Pointer=Item.SourceBuffer.Pointer;
	Buffer.Length=Item.SourceBuffer.Length;
	return Buffer;
}
//---------------------------------------------------------------------------
void bcWriteQueueFromStream::CommitWriteBuffer(uIntn Size)noexcept(true)
{
	uInt8 ItemIndex;
	if(fWriteItemRing.ReserveItem<0>(ItemIndex)==false){
		// no buffer
		return;
	}
	auto &Item=fWriteItem[ItemIndex];
	if(Size>Item.SourceBuffer.Length){
		Item.SourceBufferSize=Item.SourceBuffer.Length;
	}
	else{
		Item.SourceBufferSize=Size;
	}

	SendWriteBuffer(Item);
}
//---------------------------------------------------------------------------
void bcWriteQueueFromStream::SendWriteBuffer(cWriteItem &Item)noexcept(true)
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
		if(Task->Await(&fTaskProcedure)==false){
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
void bcWriteQueueFromStream::ProcessTask(void)noexcept(true)
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
void bcWriteQueueFromStream::cTaskProcedure::Execute(void)noexcept(true)
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
cEndpointFromStream::cReadQueue::cReadQueue(iStream *Stream,uIntn PacketBufferSize,iThreadPool *ThreadPool)noexcept(true)
	: bcReadQueueFromStream(Stream,PacketBufferSize)
{
	if(ThreadPool==nullptr){
		ThreadPool=cnSystem::DefaultThreadPool;
	}
	CreateQueueWork(ThreadPool,false);
}
//---------------------------------------------------------------------------
cEndpointFromStream* cEndpointFromStream::cReadQueue::GetHost(void)noexcept(true)
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
iReference* cEndpointFromStream::cReadQueue::NotificationInnerReference(void)noexcept(true)
{
	return &GetHost()->fInnerReference;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cEndpointFromStream::cWriteQueue::cWriteQueue(iStream *Stream,iThreadPool *ThreadPool)noexcept(true)
	: bcWriteQueueFromStream(Stream)
{
	if(ThreadPool==nullptr){
		ThreadPool=cnSystem::DefaultThreadPool;
	}
	CreateQueueWork(ThreadPool,false);
}
//---------------------------------------------------------------------------
cEndpointFromStream* cEndpointFromStream::cWriteQueue::GetHost(void)noexcept(true)
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
iReference* cEndpointFromStream::cWriteQueue::NotificationInnerReference(void)noexcept(true)
{
	return &GetHost()->fInnerReference;
}
//---------------------------------------------------------------------------
void cEndpointFromStream::cWriteQueue::NotificationClosed(void)noexcept(true)
{
	bcWriteQueueFromStream::NotificationClosed();

	if(IsNotificationEnded()==false){
		// need to terminate read queue as well
		GetHost()->WriteQueueTerminated();
	}
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cEndpointFromStream::cEndpointFromStream(rPtr<iReference> StreamReference,iStream *Stream,uIntn ReadBufferSize,iThreadPool *ThreadPool)noexcept(true)
	: fStreamReference(cnVar::MoveCast(StreamReference))
	, fReadQueue(Stream,ReadBufferSize,ThreadPool)
	, fWriteQueue(Stream,ThreadPool)
{
}
//---------------------------------------------------------------------------
cEndpointFromStream::~cEndpointFromStream()noexcept(true)
{
}
//---------------------------------------------------------------------------
void cEndpointFromStream::VirtualStarted(void)noexcept(true)
{
	InnerActivate('self');
}
//---------------------------------------------------------------------------
void cEndpointFromStream::VirtualStopped(void)noexcept(true)
{
	Close();
	InnerDecReference('self');
}
//---------------------------------------------------------------------------
void cEndpointFromStream::Close(void)noexcept(true)
{
	fReadQueue.CloseQueue();
	fWriteQueue.CloseQueue();
}
//---------------------------------------------------------------------------
iReadQueue* cEndpointFromStream::GetReadQueue(void)noexcept(true)
{
	return &fReadQueue;
}
//---------------------------------------------------------------------------
iWriteQueue* cEndpointFromStream::GetWriteQueue(void)noexcept(true)
{
	return &fWriteQueue;
}
//---------------------------------------------------------------------------
void cEndpointFromStream::WriteQueueTerminated(void)noexcept(true)
{
	fReadQueue.CloseQueue();
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
iPtr<iEndpoint> cnRTL::CreateEndpointFromSteam(iPtr<iStream> Stream,uIntn ReadBufferSize,iThreadPool *ThreadPool)noexcept(true)
{
	iReference *StreamRef=Stream.Reference();
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
cConnectionListenerFromAsyncListener::cConnectionListenerFromAsyncListener(iAsyncListener *Listener)noexcept(true)
	: fListener(Listener)
{
	for(uIntn i=0;i<cnMemory::ArrayLength(fAcceptTasks);i++){
		fAcceptTasks[i].TaskIndex=i;
	}
}
//---------------------------------------------------------------------------
cConnectionListenerFromAsyncListener::~cConnectionListenerFromAsyncListener()noexcept(true)
{
}
//---------------------------------------------------------------------------
void cConnectionListenerFromAsyncListener::VirtualStarted(void)noexcept(true)
{
	InnerActivate('self');
}
//---------------------------------------------------------------------------
void cConnectionListenerFromAsyncListener::VirtualStopped(void)noexcept(true)
{
	CloseQueue();

	InnerDecReference('self');
}
//---------------------------------------------------------------------------
iAddress* cConnectionListenerFromAsyncListener::GetLocalAddress(void)noexcept(true)
{
	return fListener->GetLocalAddress();
}
//---------------------------------------------------------------------------
iPtr<iConnection> cConnectionListenerFromAsyncListener::FetchConnection(void)noexcept(true)
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
iReference* cConnectionListenerFromAsyncListener::NotificationInnerReference(void)noexcept(true)
{
	return &fInnerReference;
}
//---------------------------------------------------------------------------
void cConnectionListenerFromAsyncListener::NotificationStarted(void)noexcept(true)
{
	bcConnectionListener::NotificationStarted();
	QueueTask();
}
//---------------------------------------------------------------------------
void cConnectionListenerFromAsyncListener::NotificationStopped(void)noexcept(true)
{
	bcConnectionListener::NotificationStopped();
}
//---------------------------------------------------------------------------
void cConnectionListenerFromAsyncListener::NotificationClosed(void)noexcept(true)
{
	bcConnectionListener::NotificationClosed();

	for(auto &Item : fAcceptTasks){
		if(Item.AcceptTask!=nullptr){
			Item.AcceptTask->Cancel();
		}
	}
}
//---------------------------------------------------------------------------
cConnectionListenerFromAsyncListener::CycleState cConnectionListenerFromAsyncListener::NotificationCheckState(void)noexcept(true)
{
	if(fQueueTerminated){
		return CycleState::Terminated;
	}
	return CycleState::Normal;

}
//---------------------------------------------------------------------------
void cConnectionListenerFromAsyncListener::QueueTask(void)noexcept(true)
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

					if(TaskItem.AcceptTask->Await(&TaskItem)==false){
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
void cConnectionListenerFromAsyncListener::TaskCompletionProcess(void)noexcept(true)
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
void cConnectionListenerFromAsyncListener::cTaskItem::Execute(void)noexcept(true)
{
	auto *pAcceptTasks=reinterpret_cast<cTaskItem(*)[TaskRingSize]>(this-TaskIndex);
	auto Host=cnMemory::GetObjectFromMemberPointer(pAcceptTasks,&cConnectionListenerFromAsyncListener::fAcceptTasks);

	Host->TaskCompletionProcess();
	auto Reference=Host->NotificationInnerReference();
	if(Reference!=nullptr){
		rDecReference(Reference,'task');
	}
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cMultipointQueueFromStream::cMultipointQueueFromStream(iMultipointStream *Stream)noexcept(true)
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
cMultipointQueueFromStream::~cMultipointQueueFromStream()noexcept(true)
{
	for(uIntn i=0;i<fReceiveItemCount;i++){
		cnSystem::DefaultHeap::Free(fReceiveItem[i].ReadBuffer,fPacketSizeMax,0);
	}
	for(uIntn i=0;i<fSendItemCount;i++){
		cnSystem::DefaultHeap::Free(fSendItem[i].WriteBuffer,fPacketSizeMax,0);
	}
}
//---------------------------------------------------------------------------
void cMultipointQueueFromStream::VirtualStarted(void)noexcept(true)
{
	InnerActivate('self');
	CreateMultipointQueueStateWriteWork(cnSystem::DefaultThreadPool,false);
}
//---------------------------------------------------------------------------
void cMultipointQueueFromStream::VirtualStopped(void)noexcept(true)
{
	CloseMultipointQueue();
	InnerDecReference('self');
}
//---------------------------------------------------------------------------
iReference* cMultipointQueueFromStream::MultipointQueueInnerReference(void)noexcept(true)
{
	return &fInnerReference;
}
//---------------------------------------------------------------------------
void cMultipointQueueFromStream::MultipointQueueStarted(void)noexcept(true)
{
	bcMultipointQueue::MultipointQueueStarted();
	fIOProcedure.RunAsync(fIOWork);
}
//---------------------------------------------------------------------------
iAddress*	cMultipointQueueFromStream::GetLocalAddress(void)noexcept(true)
{
	return fStream->GetLocalAddress();
}
//---------------------------------------------------------------------------
cConstMemory cMultipointQueueFromStream::GatherReadBuffer(iPtr<iAddress> &Address)noexcept(true)
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
void cMultipointQueueFromStream::DismissReadBuffer(bool Dismiss)noexcept(true)
{
	if(Dismiss==false)
		return;

	auto &Item=fReceiveItem[fCurReceiveItemIndex];
	Item.Task=nullptr;
	Item.State=cReceiveItem::sIdle;

	fIOProcedure.Run();
}
//---------------------------------------------------------------------------
cMemory cMultipointQueueFromStream::ReserveWriteBuffer(uIntn)noexcept(true)
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
bool cMultipointQueueFromStream::CommitWriteBuffer(iAddress *Address,uIntn Length)noexcept(true)
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
bool cMultipointQueueFromStream::cIOProcedure::Procedure(void)noexcept(true)
{
	auto Host=cnMemory::GetObjectFromMemberPointer(this,&cMultipointQueueFromStream::fIOProcedure);
	return Host->IOProcedure();
}
//---------------------------------------------------------------------------
bool cMultipointQueueFromStream::IOProcedure(void)noexcept(true)
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
void cMultipointQueueFromStream::cSendItem::Execute(void)noexcept(true)
{
	Owner->SendItemCompletionNotify(this);
}
//---------------------------------------------------------------------------
void cMultipointQueueFromStream::SendItemSetNotify(cSendItem &Item)noexcept(true)
{
	InnerIncReference('stsk');
	if(Item.Task->Await(&Item)==false){
		SendItemCompletionNotify(&Item);
	}
}
//---------------------------------------------------------------------------
void cMultipointQueueFromStream::SendItemCompletionNotify(cSendItem *Item)noexcept(true)
{
	Item->State=cSendItem::sCompleted;

	// continue push
	MultipointQueueSetWriteBufferAvailable(false);
	InnerDecReference('stsk');
}
//---------------------------------------------------------------------------
void cMultipointQueueFromStream::cReceiveItem::Execute(void)noexcept(true)
{
	Owner->ReceiveItemCompletionNotify(this);
}
//---------------------------------------------------------------------------
void cMultipointQueueFromStream::ReceiveItemSetNotify(cReceiveItem &Item)noexcept(true)
{
	InnerIncReference('rtsk');
	if(Item.Task->Await(&Item)==false){
		ReceiveItemCompletionNotify(&Item);
	}
}
//---------------------------------------------------------------------------
void cMultipointQueueFromStream::ReceiveItemCompletionNotify(cReceiveItem *Item)noexcept(true)
{
	Item->State=cReceiveItem::sPush;

	// continue push
	MultipointQueueSetReadBufferAvailable(false);
	InnerDecReference('rtsk');
}
//---------------------------------------------------------------------------
