#include "StreamProtocol.h"
#include "String.h"
//---------------------------------------------------------------------------
using namespace cnLibrary;
using namespace cnRTL;

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
bcProtocolProcessor::bcProtocolProcessor(void)noexcept
	: fProtocolProvider(nullptr)
	, fActiveMutex(false)
	, fProtocolActive(false)
{
}
//---------------------------------------------------------------------------
bcProtocolProcessor::~bcProtocolProcessor()noexcept
{
}
//---------------------------------------------------------------------------
iProtocolProvider* bcProtocolProcessor::GetProvider(void)const noexcept
{
	return fProtocolProvider;
}
//---------------------------------------------------------------------------
bool bcProtocolProcessor::SetProvider(iProtocolProvider *ProtocolProvider,bool SingleThreaded)noexcept
{
	if(fActiveMutex.Acquire.Xchg(true)==true){
		return false;
	}
	if(fProtocolActive==false){
		fProtocolProvider=ProtocolProvider;
		fSingleThreaded=SingleThreaded;
	}
	
	fActiveMutex.Release.Store(false);
	return true;
}
//---------------------------------------------------------------------------
bool bcProtocolProcessor::IsActive(void)const noexcept
{
	return fProtocolActive;
}
//---------------------------------------------------------------------------
bool bcProtocolProcessor::Start(iReference *Reference)noexcept
{
	if(fProtocolProvider==nullptr){
		return false;
	}
	if(fProtocolActive)
		return false;
	if(fActiveMutex.Acquire.Xchg(true)==true){
		return false;
	}

	if(fProtocolProvider->StartProcessor(Reference,this,fSingleThreaded)==false){
		fActiveMutex.Release.Store(false);
		return false;
	}

	fProtocolActive=true;
	fActiveMutex.Release.Store(false);
	return true;
}
//---------------------------------------------------------------------------
void bcProtocolProcessor::Stop(void)noexcept
{
	if(fProtocolActive){
		if(fProtocolProvider!=nullptr){
			fProtocolProvider->StopProcessor(false);
		}
	}
}
//---------------------------------------------------------------------------
void bcProtocolProcessor::Terminate(void)noexcept
{
	if(fProtocolActive){
		if(fProtocolProvider!=nullptr){
			fProtocolProvider->StopProcessor(true);
		}
	}
}
//---------------------------------------------------------------------------
void bcProtocolProcessor::ProtocolStarted(void)noexcept
{
	fProtocolActive=true;
	fActiveMutex.Release.Store(false);
}
//---------------------------------------------------------------------------
void bcProtocolProcessor::ProtocolStopped(void)noexcept
{
	fProtocolActive=false;
}
//---------------------------------------------------------------------------
#if 0
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cProtocolProviderFromStream::cProtocolProviderFromStream(iPtr<iStream> Stream)
	: fProtocolProcessor(nullptr)
	, fStream(cnVar::MoveCast(Stream))
	, fRWQueueTerminate(false)
	, fRWQueueConnected(false)
	, fProcessorSingleThreaded(false)
	, fProcessorStateStarted(false)
	, fReadStopped(false)
	, fReadEnded(false)
	, fReadActive(false)
	, fWriteStopped(false)
	, fWriteEnded(false)
	, fWriteActive(false)
{
}
//---------------------------------------------------------------------------
cProtocolProviderFromStream::~cProtocolProviderFromStream()
{
}
//---------------------------------------------------------------------------
void cProtocolProviderFromStream::VirtualStarted(void)
{
	fInputNeedSize=0;
	fOutputNeedSize=0;
	fSetWriteEnd=false;
	fInputIdleNotify=false;
	fOutputIdleNotify=false;
	fWriteActive=true;
	fReadActive=true;

	if(fWriteQueue->StartWrite(this,this)==false){
		fReadActive=false;
		fWriteActive=false;
		fReadStopped=true;
		fWriteStopped=true;
		return;
	}
	if(fReadQueue->StartRead(this,this)==false){
		fReadActive=false;
		fReadStopped=true;
		fWriteStopped=true;
		fWriteQueue->StopWrite(false);
		return;
	}
	fRWQueueConnected=true;
}
//---------------------------------------------------------------------------
void cProtocolProviderFromStream::VirtualStopped(void)
{
	CloseProvider();
	InnerDecReference('self');
}
//---------------------------------------------------------------------------
void cProtocolProviderFromStream::CloseProvider(void)
{
	if(fRWQueueTerminate.Acquire.Xchg(true)==false){
		if(fProcessorSingleThreaded){
			SingleThreadStateProcedure();
		}
		else{
			ReadThreadStateProcedure();
			WriteThreadStateProcedure();
		}
	}
}
//---------------------------------------------------------------------------
bool cProtocolProviderFromStream::StartProcessor(iReference *Reference,iProtocolProcessor *ProtocolProcessor,bool SingleThreaded)
{
	if(fRWQueueConnected==false)
		return false;
	if(fProtocolProcessor!=nullptr)
		return false;

	fProtocolProcessor=ProtocolProcessor;
	fProtocolProcessorReference=Reference;
	fProcessorStopped=false;
	fProcessorSingleThreaded=SingleThreaded;

	ProcessorStateProcedure();
	return true;
}
//---------------------------------------------------------------------------
void cProtocolProviderFromStream::StopProcessor(void)
{
	if(fProcessorStopped.Free.Xchg(true)==false){
		ProcessorStateProcedure();
	}
}
//---------------------------------------------------------------------------
void cProtocolProviderFromStream::TerminateConnection(void)
{
	CloseProvider();
}
//---------------------------------------------------------------------------
void cProtocolProviderFromStream::ProcessorStateProcedure(void)
{
	if(fProcessorStateFlag.Acquire()==false)
		return;

	do{
		fProcessorStateFlag.Continue();
		ProcessorStateProcess();
	}while(fProcessorStateFlag.Release()==false);

}
//---------------------------------------------------------------------------
void cProtocolProviderFromStream::ProcessorStateProcess(void)
{
	if(fRWQueueConnected){
		if(fReadActive || fWriteActive){
			// r/w queue running
			if(fProcessorStateStarted==false){
				if(fProtocolProcessor!=nullptr){
					// processor started
					fProtocolProcessor->ProtocolStarted();
					fProcessorStateStarted=true;

					if(fProcessorSingleThreaded){
						SingleThreadStateProcedure();
					}
					else{
						ReadThreadStateProcedure();
						WriteThreadStateProcedure();
					}

				}
			}
			return;
		}
		// connection stopped
		fRWQueueConnected=false;
	}
	// connection lost
	
	if(fProcessorStateStarted){
		// check processor state
		if(fProcessorStopped==false){
			// wait for processor stop
			return;
		}
		// processor stopped
		fProtocolProcessor->ProtocolStopped();
		fProtocolProcessor=nullptr;
		fProtocolProcessorReference=nullptr;
		fProcessorStateStarted=false;
	}
}
//---------------------------------------------------------------------------
void cProtocolProviderFromStream::SingleThreadStateProcedure(void)
{
	if(fWriteStateFlag.Acquire()==false){
		return;
	}
	do{
		fWriteStateFlag.Continue();

		ReadQueueProcess();
		WriteQueueProcess();

	}while(fWriteStateFlag.Release()==false);
}
//---------------------------------------------------------------------------
void cProtocolProviderFromStream::ReadThreadStateProcedure(void)
{
	if(fReadStateFlag.Acquire()==false){
		return;
	}
	do{
		fReadStateFlag.Continue();

		ReadQueueProcess();

	}while(fReadStateFlag.Release()==false);
}
//---------------------------------------------------------------------------
void cProtocolProviderFromStream::WriteThreadStateProcedure(void)
{
	if(fWriteStateFlag.Acquire()==false){
		return;
	}
	do{
		fWriteStateFlag.Continue();
		
		WriteQueueProcess();

	}while(fWriteStateFlag.Release()==false);
}
//---------------------------------------------------------------------------
void cProtocolProviderFromStream::NotifyInput(void)
{
	if(fRWQueueConnected){
		if(fProcessorSingleThreaded){
			return SingleThreadStateProcedure();
		}
		else{
			return ReadThreadStateProcedure();
		}
	}
}
//---------------------------------------------------------------------------
void cProtocolProviderFromStream::NotifyInput(uIntn LeastSizeNeeded)
{
	fInputNeedSize=LeastSizeNeeded;
	if(LeastSizeNeeded==0){
		fInputIdleNotify=true;
	}
	NotifyInput();
}
//---------------------------------------------------------------------------
void cProtocolProviderFromStream::NotifyOutput(void)
{
	if(fRWQueueConnected){
		if(fProcessorSingleThreaded){
			return SingleThreadStateProcedure();
		}
		else{
			return WriteThreadStateProcedure();
		}
	}
}
//---------------------------------------------------------------------------
void cProtocolProviderFromStream::NotifyOutput(uIntn LeastSizeNeeded)
{
	fOutputNeedSize=LeastSizeNeeded;
	if(LeastSizeNeeded==0){
		fOutputIdleNotify=true;
	}
	NotifyOutput();
}
//---------------------------------------------------------------------------
void cProtocolProviderFromStream::SetEndOfOutput(void)
{
	if(fSetWriteEnd==false){
		fSetWriteEnd=true;
		NotifyOutput();
	}
}
//---------------------------------------------------------------------------
bool cProtocolProviderFromStream::QueryReadBuffer(cConstMemory &Buffer)
{
	auto PackBuffer=fInputPacker.GetReadBuffer();
	if(PackBuffer.Length>fInputNeedSize){
		Buffer=PackBuffer;
		return true;
	}
	uIntn QuerySize=fInputNeedSize-PackBuffer.Length;
	auto EndpointBuffer=fReadQueue->GatherReadBuffer(QuerySize);
	if(EndpointBuffer.Length==0){
		// no more buffer
		bool GracefulClose;
		if(fReadQueue->IsReadClosed(GracefulClose)){
			fReadEnded=GracefulClose;
			fReadQueue->StopRead();
			fReadStopped=true;
		}
		else{
			// wait notify when more buffer available
			fReadQueue->NotifyRead(1);
		}
		return false;
	}
	if(PackBuffer.Length==0){
		if(EndpointBuffer.Length>=fInputNeedSize){
			Buffer=EndpointBuffer;
			return true;
		}
	}

	// join up buffer
	auto AppendBuffer=fInputPacker.ReserveWriteBuffer(EndpointBuffer.Length);
	if(AppendBuffer.Length>EndpointBuffer.Length){
		AppendBuffer.Length=EndpointBuffer.Length;
	}
	cnMemory::Copy(AppendBuffer.Pointer,EndpointBuffer.Pointer,AppendBuffer.Length);
	fInputPacker.CommitWriteBuffer(AppendBuffer.Length);

	fReadQueue->DismissReadBuffer(AppendBuffer.Length);

	// check if total buffer fits needs
	PackBuffer=fInputPacker.GetReadBuffer();
	if(PackBuffer.Length>fInputNeedSize){
		Buffer=PackBuffer;
		return true;
	}

	return false;
}
//---------------------------------------------------------------------------
void cProtocolProviderFromStream::AdvanceReadBuffer(uIntn Size)
{
	auto PackBuffer=fInputPacker.GetReadBuffer();
	if(PackBuffer.Length==0){
		fReadQueue->DismissReadBuffer(Size);
	}
	else{
		fInputPacker.DismissReadBuffer(Size);
	}
}
//---------------------------------------------------------------------------
void cProtocolProviderFromStream::ReadQueueProcess(void)
{
	if(fReadStopped)
		return;
	if(fRWQueueTerminate){
		// terminate read queue
		fReadQueue->StopRead();
		fReadStopped=true;
		if(fProcessorStateStarted){
			fProtocolProcessor->ProtocolInputClosed(false);
		}
		return;
	}
	
	if(fProcessorStateStarted==false)
		return;
	cConstMemory ReadBuffer;
	while(QueryReadBuffer(ReadBuffer)){
		fInputNeedSize=0;
		uIntn SizePushed=fProtocolProcessor->ProtocolInputPush(ReadBuffer.Pointer,ReadBuffer.Length,fInputNeedSize);
		AdvanceReadBuffer(SizePushed);
		if(SizePushed==0){
			if(fInputNeedSize<=ReadBuffer.Length){
				fInputNeedSize=ReadBuffer.Length+1;
			}
			// notify when more buffer available
			fReadQueue->NotifyRead(1);
			return;
		}
	}
	if(fReadStopped){
		// read ended
		fProtocolProcessor->ProtocolInputClosed(fReadEnded);
	}
	else if(fInputIdleNotify){
		fInputIdleNotify=false;
		// idle notify
		fProtocolProcessor->ProtocolInputPush(nullptr,0,fInputNeedSize);
	}
}
//---------------------------------------------------------------------------
bool cProtocolProviderFromStream::QueryWriteBuffer(cMemory &Buffer)
{
	Buffer=fWriteQueue->ReserveWriteBuffer(fOutputNeedSize);
	if(Buffer.Length>=fOutputNeedSize){
		return true;
	}
	if(Buffer.Length==0){
		bool GracefulClose;
		if(fWriteQueue->IsWriteClosed(GracefulClose)){
			fWriteEnded=GracefulClose;
			fWriteStopped=true;
			fWriteQueue->StopWrite(false);
			return false;
		}

		// wait for more buffer available
		fWriteQueue->NotifyWrite(false);
	}
	return false;
}
//---------------------------------------------------------------------------
void cProtocolProviderFromStream::AdvanceWriteBuffer(uIntn Size)
{
	fWriteQueue->CommitWriteBuffer(Size);
}
//---------------------------------------------------------------------------
void cProtocolProviderFromStream::WriteQueueProcess(void)
{
	if(fWriteStopped)
		return;
	if(fRWQueueTerminate){
		// terminate read queue
		fWriteQueue->StopWrite(true);
		fWriteStopped=true;
		if(fProcessorStateStarted){
			fProtocolProcessor->ProtocolOutputClosed(false);
		}
		return;
	}

	if(fProcessorStateStarted==false)
		return;
	cMemory WriteBuffer;
	while(QueryWriteBuffer(WriteBuffer)){
		fOutputNeedSize=0;
		uIntn SizePulled=fProtocolProcessor->ProtocolOutputPull(WriteBuffer.Pointer,WriteBuffer.Length,fOutputNeedSize);
		AdvanceWriteBuffer(SizePulled);
		if(SizePulled==0){
			if(fOutputNeedSize==0){
				if(fSetWriteEnd){
					// no more pulling
					fWriteQueue->StopWrite(false);
					fWriteStopped=true;
					fWriteEnded=true;
					fProtocolProcessor->ProtocolOutputClosed(true);
					return;
				}
			}
			if(fOutputNeedSize<=WriteBuffer.Length){
				fOutputNeedSize=WriteBuffer.Length+1;
			}
			// wait for more buffer available
			fWriteQueue->NotifyWrite(false);
			return;
		}
	}
	if(fWriteStopped){
		// terminated
		fProtocolProcessor->ProtocolOutputClosed(false);
	}
	else if(fOutputIdleNotify){
		fOutputIdleNotify=false;
		// idle notify
		fProtocolProcessor->ProtocolOutputPull(nullptr,0,fOutputNeedSize);
	}
}
#endif // 0
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cProtocolProviderFromRWQueue::cProtocolProviderFromRWQueue(iPtr<iConnection> Connection,rPtr<iReadQueue> ReadQueue,rPtr<iWriteQueue> WriteQueue)noexcept
	: fProtocolProcessor(nullptr)
	, fConnection(cnVar::MoveCast(Connection))
	, fReadQueue(cnVar::MoveCast(ReadQueue))
	, fWriteQueue(cnVar::MoveCast(WriteQueue))
	, fRWQueueTerminate(false)
	, fRWQueueConnected(false)
	, fProcessorSingleThreaded(false)
	, fProcessorStateStarted(false)
	, fReadStopped(false)
	, fReadEnded(false)
	, fReadActive(false)
	, fWriteStopped(false)
	, fWriteEnded(false)
	, fWriteActive(false)
{
}
//---------------------------------------------------------------------------
cProtocolProviderFromRWQueue::~cProtocolProviderFromRWQueue()noexcept
{
}
//---------------------------------------------------------------------------
void cProtocolProviderFromRWQueue::VirtualStarted(void)noexcept
{
	InnerActivate('self');

	fInputNeedSize=0;
	fOutputNeedSize=0;
	fSetWriteEnd=false;
	fInputIdleNotify=false;
	fOutputIdleNotify=false;
	fWriteActive=true;
	fReadActive=true;

	if(fWriteQueue->StartWrite(this,this)==false){
		fReadActive=false;
		fWriteActive=false;
		fReadStopped=true;
		fWriteStopped=true;
		return;
	}
	if(fReadQueue->StartRead(this,this)==false){
		fReadActive=false;
		fReadStopped=true;
		fWriteStopped=true;
		fWriteQueue->StopWrite();
		return;
	}
	fRWQueueConnected=true;
}
//---------------------------------------------------------------------------
void cProtocolProviderFromRWQueue::VirtualStopped(void)noexcept
{
	CloseProvider();
	InnerDecReference('self');
}
//---------------------------------------------------------------------------
void cProtocolProviderFromRWQueue::CloseProvider(void)noexcept
{
	if(fRWQueueTerminate.Acquire.Xchg(true)==false){
		if(fProcessorSingleThreaded){
			SingleThreadStateProcedure();
		}
		else{
			ReadThreadStateProcedure();
			WriteThreadStateProcedure();
		}
	}
}
//---------------------------------------------------------------------------
iConnection* cProtocolProviderFromRWQueue::GetConnecton(void)noexcept
{
	return fConnection;
}
//---------------------------------------------------------------------------
bool cProtocolProviderFromRWQueue::StartProcessor(iReference *Reference,iProtocolProcessor *ProtocolProcessor,bool SingleThreaded)noexcept
{
	if(fRWQueueConnected==false)
		return false;
	if(fProtocolProcessor!=nullptr)
		return false;

	fProtocolProcessor=ProtocolProcessor;
	fProtocolProcessorReference=Reference;
	fStopProcessor=false;
	fProcessorSingleThreaded=SingleThreaded;

	ProcessorStateProcedure();
	return true;
}
//---------------------------------------------------------------------------
void cProtocolProviderFromRWQueue::StopProcessor(bool Terminate)noexcept
{
	if(Terminate){
		CloseProvider();
	}
	if(fStopProcessor.Free.Xchg(true)==false){
		ProcessorStateProcedure();
	}
}
//---------------------------------------------------------------------------
void cProtocolProviderFromRWQueue::ProcessorStateProcedure(void)noexcept
{
	if(fProcessorStateFlag.Acquire()==false)
		return;

	InnerIncReference('stat');
	do{
		fProcessorStateFlag.Continue();
		ProcessorStateProcess();
	}while(fProcessorStateFlag.Release()==false);

	InnerDecReference('stat');
}
//---------------------------------------------------------------------------
void cProtocolProviderFromRWQueue::ProcessorStateProcess(void)noexcept
{
	if(fRWQueueConnected){
		if(fReadActive || fWriteActive){
			// r/w queue running
			if(fProcessorStateStarted==false){
				if(fProtocolProcessor!=nullptr){
					// processor started
					fProtocolProcessor->ProtocolStarted();
					fProcessorStateStarted=true;


					fReadQueue->NotifyRead(0);
					fWriteQueue->NotifyWrite(0);
					//if(fProcessorSingleThreaded){
					//	SingleThreadStateProcedure();
					//}
					//else{
					//	ReadThreadStateProcedure();
					//	WriteThreadStateProcedure();
					//}

				}
			}
			return;
		}
		// connection stopped
		fRWQueueConnected=false;
	}
	// connection lost
	
	if(fProcessorStateStarted){
		// processor stopped
		fProtocolProcessor->ProtocolStopped();
		fProtocolProcessor=nullptr;
		fProtocolProcessorReference=nullptr;
		fProcessorStateStarted=false;
	}
}
//---------------------------------------------------------------------------
void cProtocolProviderFromRWQueue::SingleThreadStateProcedure(void)noexcept
{
	if(fWriteStateFlag.Acquire()==false){
		return;
	}
	do{
		fWriteStateFlag.Continue();

		ReadQueueProcess();
		WriteQueueProcess();

	}while(fWriteStateFlag.Release()==false);
}
//---------------------------------------------------------------------------
void cProtocolProviderFromRWQueue::ReadThreadStateProcedure(void)noexcept
{
	if(fReadStateFlag.Acquire()==false){
		return;
	}
	do{
		fReadStateFlag.Continue();

		ReadQueueProcess();

	}while(fReadStateFlag.Release()==false);
}
//---------------------------------------------------------------------------
void cProtocolProviderFromRWQueue::WriteThreadStateProcedure(void)noexcept
{
	if(fWriteStateFlag.Acquire()==false){
		return;
	}
	do{
		fWriteStateFlag.Continue();
		
		WriteQueueProcess();

	}while(fWriteStateFlag.Release()==false);
}
//---------------------------------------------------------------------------
void cProtocolProviderFromRWQueue::NotifyInput(void)noexcept
{
	if(fRWQueueConnected){
		if(fProcessorSingleThreaded){
			return SingleThreadStateProcedure();
		}
		else{
			return ReadThreadStateProcedure();
		}
	}
}
//---------------------------------------------------------------------------
void cProtocolProviderFromRWQueue::NotifyInput(uIntn LeastSizeNeeded)noexcept
{
	fInputNeedSize=LeastSizeNeeded;
	if(LeastSizeNeeded==0){
		fInputIdleNotify=true;
	}
	NotifyInput();
}
//---------------------------------------------------------------------------
void cProtocolProviderFromRWQueue::NotifyOutput(void)noexcept
{
	if(fRWQueueConnected){
		if(fProcessorSingleThreaded){
			return SingleThreadStateProcedure();
		}
		else{
			return WriteThreadStateProcedure();
		}
	}
}
//---------------------------------------------------------------------------
void cProtocolProviderFromRWQueue::NotifyOutput(uIntn LeastSizeNeeded)noexcept
{
	fOutputNeedSize=LeastSizeNeeded;
	if(LeastSizeNeeded==0){
		fOutputIdleNotify=true;
	}
	NotifyOutput();
}
//---------------------------------------------------------------------------
void cProtocolProviderFromRWQueue::SetEndOfOutput(void)noexcept
{
	if(fSetWriteEnd==false){
		fSetWriteEnd=true;
		NotifyOutput();
	}
}
//---------------------------------------------------------------------------
bool cProtocolProviderFromRWQueue::IsInputEnded(void)noexcept
{
	return fReadEnded;
}
//---------------------------------------------------------------------------
bool cProtocolProviderFromRWQueue::IsOutputEnded(void)noexcept
{
	return fWriteEnded;
}
//---------------------------------------------------------------------------
void cProtocolProviderFromRWQueue::ReadStarted(void)noexcept
{
	cnLib_ASSERT(fReadActive==true);
	fReadQueue->NotifyRead(1);
}
//---------------------------------------------------------------------------
void cProtocolProviderFromRWQueue::ReadStopped(void)noexcept
{
	fReadActive=false;
	ProcessorStateProcedure();
}
//---------------------------------------------------------------------------
void cProtocolProviderFromRWQueue::ReadNotify(void)noexcept
{
	NotifyInput();
}
//---------------------------------------------------------------------------
void cProtocolProviderFromRWQueue::WriteStarted(void)noexcept
{
	cnLib_ASSERT(fWriteActive==true);
	fWriteQueue->NotifyWrite(false);
}
//---------------------------------------------------------------------------
void cProtocolProviderFromRWQueue::WriteStopped(void)noexcept
{
	fWriteActive=false;
	ProcessorStateProcedure();
}
//---------------------------------------------------------------------------
void cProtocolProviderFromRWQueue::WriteNotify(void)noexcept
{
	NotifyOutput();
}
//---------------------------------------------------------------------------
bool cProtocolProviderFromRWQueue::QueryReadBuffer(cConstMemory &Buffer)noexcept
{
	auto PackBuffer=fInputPacker.GetReadBuffer();
	if(PackBuffer.Length>fInputNeedSize){
		Buffer=PackBuffer;
		return true;
	}
	uIntn QuerySize=fInputNeedSize-PackBuffer.Length;
	auto EndpointBuffer=fReadQueue->GatherReadBuffer(QuerySize);
	if(EndpointBuffer.Length==0){
		// no more buffer
		bool GracefulClose;
		if(fReadQueue->IsReadClosed(GracefulClose)){
			fReadEnded=GracefulClose;
			fReadQueue->StopRead();
			fReadStopped=true;
		}
		else{
			// wait notify when more buffer available
			fReadQueue->NotifyRead(1);
		}
		return false;
	}
	if(PackBuffer.Length==0){
		if(EndpointBuffer.Length>=fInputNeedSize){
			Buffer=EndpointBuffer;
			return true;
		}
	}

	// join up buffer
	auto AppendBuffer=fInputPacker.ReserveWriteBuffer(EndpointBuffer.Length);
	if(AppendBuffer.Length>EndpointBuffer.Length){
		AppendBuffer.Length=EndpointBuffer.Length;
	}
	cnMemory::Copy(AppendBuffer.Pointer,EndpointBuffer.Pointer,AppendBuffer.Length);
	fInputPacker.CommitWriteBuffer(AppendBuffer.Length);

	fReadQueue->DismissReadBuffer(AppendBuffer.Length);

	// check if total buffer fits needs
	PackBuffer=fInputPacker.GetReadBuffer();
	if(PackBuffer.Length>fInputNeedSize){
		Buffer=PackBuffer;
		return true;
	}

	return false;
}
//---------------------------------------------------------------------------
void cProtocolProviderFromRWQueue::AdvanceReadBuffer(uIntn Size)noexcept
{
	auto PackBuffer=fInputPacker.GetReadBuffer();
	if(PackBuffer.Length==0){
		fReadQueue->DismissReadBuffer(Size);
	}
	else{
		fInputPacker.DismissReadBuffer(Size);
	}
}
//---------------------------------------------------------------------------
void cProtocolProviderFromRWQueue::ReadQueueProcess(void)noexcept
{
	if(fReadStopped)
		return;
	if(fRWQueueTerminate){
		// terminate read queue
		fReadQueue->StopRead();
		fReadStopped=true;
		return;
	}
	
	if(fProcessorStateStarted==false)
		return;
	cConstMemory ReadBuffer;
	while(QueryReadBuffer(ReadBuffer)){
		fInputNeedSize=0;
		uIntn SizePushed=fProtocolProcessor->ProtocolInputPush(ReadBuffer.Pointer,ReadBuffer.Length,fInputNeedSize);
		AdvanceReadBuffer(SizePushed);
		if(SizePushed==0){
			if(fInputNeedSize<=ReadBuffer.Length){
				fInputNeedSize=ReadBuffer.Length+1;
			}
			// notify when more buffer available
			fReadQueue->NotifyRead(1);
			return;
		}
	}
	if(fReadStopped){
		// read ended
		if(fReadEnded){
			fProtocolProcessor->ProtocolInputEnd();
		}
	}
	else if(fInputIdleNotify){
		fInputIdleNotify=false;
		// idle notify
		fProtocolProcessor->ProtocolInputPush(nullptr,0,fInputNeedSize);
	}
}
//---------------------------------------------------------------------------
bool cProtocolProviderFromRWQueue::QueryWriteBuffer(cMemory &Buffer)noexcept
{
	Buffer=fWriteQueue->ReserveWriteBuffer(fOutputNeedSize);
	if(Buffer.Length==0){
		bool GracefulClose;
		if(fWriteQueue->IsWriteClosed(GracefulClose)){
			fWriteEnded=GracefulClose;
			fWriteStopped=true;
			fWriteQueue->StopWrite();
			return false;
		}

		// wait for more buffer available
		fWriteQueue->NotifyWrite(false);
	}
	if(Buffer.Length>=fOutputNeedSize){
		return true;
	}
	return false;
}
//---------------------------------------------------------------------------
void cProtocolProviderFromRWQueue::AdvanceWriteBuffer(uIntn Size)noexcept
{
	fWriteQueue->CommitWriteBuffer(Size);
}
//---------------------------------------------------------------------------
void cProtocolProviderFromRWQueue::WriteQueueProcess(void)noexcept
{
	if(fWriteStopped)
		return;
	if(fRWQueueTerminate){
		// terminate read queue
		fWriteQueue->StopWrite();
		fWriteQueue->CloseWrite(true);
		fWriteStopped=true;
		return;
	}

	if(fProcessorStateStarted==false)
		return;
	cMemory WriteBuffer;
	while(QueryWriteBuffer(WriteBuffer)){
		fOutputNeedSize=0;
		uIntn SizePulled=fProtocolProcessor->ProtocolOutputPull(WriteBuffer.Pointer,WriteBuffer.Length,fOutputNeedSize);
		AdvanceWriteBuffer(SizePulled);
		if(SizePulled==0){
			if(fOutputNeedSize==0){
				if(fSetWriteEnd){
					// no more pulling
					fProtocolProcessor->ProtocolOutputEnd();
					fWriteQueue->StopWrite();
					fWriteStopped=true;
					fWriteEnded=true;
					return;
				}
			}
			if(fOutputNeedSize>WriteBuffer.Length){
				// wait for more buffer available
				fWriteQueue->NotifyWrite(false);
			}
			return;
		}
	}
	if(fWriteStopped){
		// terminated
	}
	else if(fOutputIdleNotify){
		fOutputIdleNotify=false;
		// idle notify
		fProtocolProcessor->ProtocolOutputPull(nullptr,0,fOutputNeedSize);
	}
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
rPtr<cProtocolProviderFromRWQueue> cnRTL::CreateProtocolProviderFromEndpoint(iConnection *Connection,iEndpoint *Endpoint)noexcept
{
	return rCreate<cProtocolProviderFromRWQueue>(Connection,Endpoint->GetReadQueue(),Endpoint->GetWriteQueue());
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
bcProtocolQueueProcessor::bcProtocolQueueProcessor()noexcept
	: fProcessorSingleThreaded(false)
	, fQueueStateActive(false)
	, fQueueStateConnected(false)
	, fProcessorSetQueueEnded(0)
	, fConnectionNotifyInput(false)
	, fConnectionNotifyOutput(false)
{
}
//---------------------------------------------------------------------------
bcProtocolQueueProcessor::~bcProtocolQueueProcessor()noexcept
{
}
//---------------------------------------------------------------------------
iProtocolQueueProvider* bcProtocolQueueProcessor::GetQueueProvider(void)const noexcept
{
	return fProtocolQueueProvider;
}
//---------------------------------------------------------------------------
bool bcProtocolQueueProcessor::SetQueueProvider(iProtocolQueueProvider *QueueProvider,bool ProtocolSingleThreaded)noexcept
{
	if(fActiveMutex.Acquire.Xchg(true)==true){
		return false;
	}
	if(fQueueStateActive==false){
		fProtocolQueueProvider=QueueProvider;
		fSingleThreaded=ProtocolSingleThreaded;
	}
	
	fActiveMutex.Release.Store(false);
	return true;
}
//---------------------------------------------------------------------------
bool bcProtocolQueueProcessor::IsActive(void)const noexcept
{
	return fQueueStateActive;
}
//---------------------------------------------------------------------------
bool bcProtocolQueueProcessor::IsConnected(void)const noexcept
{
	return fProviderConnected;
}
//---------------------------------------------------------------------------
bool bcProtocolQueueProcessor::Start(void)noexcept
{
	if(fProtocolQueueProvider==nullptr){
		return false;
	}
	if(fQueueStateActive)
		return false;
	if(fActiveMutex.Acquire.Xchg(true)==true){
		return false;
	}

	fQueueStateActive=true;
	auto Reference=ProcessorReference();
	if(fProtocolQueueProvider->StartQueue(Reference,this)==false){
		fQueueStateActive=false;
		fActiveMutex.Release.Store(false);
		return false;
	}

	fActiveMutex.Release.Store(false);
	return true;
}
//---------------------------------------------------------------------------
void bcProtocolQueueProcessor::Stop(void)noexcept
{
	if(fQueueStateActive){
		if(fProtocolQueueProvider!=nullptr){
			fProtocolQueueProvider->StopQueue();
		}
	}
}
//---------------------------------------------------------------------------
void bcProtocolQueueProcessor::Terminate(void)noexcept
{
	DiscardConnection();
	Stop();
}
//---------------------------------------------------------------------------
void bcProtocolQueueProcessor::DiscardConnection(void)noexcept
{
	if(fConnectionStop==false){
		fConnectionStop=true;
		ProviderStateProcedure();
	}
}
//---------------------------------------------------------------------------
void bcProtocolQueueProcessor::ProtocolQueueStarted(void)noexcept
{
	cnLib_ASSERT(fQueueStateActive);
	fProtocolQueueProvider->MakeConnection();
}
//---------------------------------------------------------------------------
void bcProtocolQueueProcessor::ProtocolQueueStopped(void)noexcept
{
	fQueueStateActive=false;
}
//---------------------------------------------------------------------------
void bcProtocolQueueProcessor::ProtocolQueueConnected(iProtocolProvider *Provider)noexcept
{
	if(fProtocolProvider!=nullptr)
		return;

	fProtocolProvider=Provider;
	auto Reference=ProcessorReference();
	if(fProtocolProvider->StartProcessor(Reference,this,fSingleThreaded)){
		return;
	}
	fProtocolProvider=nullptr;
	// error
	fProtocolQueueProvider->StopQueue();
}
//---------------------------------------------------------------------------
void bcProtocolQueueProcessor::ProviderStateProcedure(void)noexcept
{
	if(fProviderStateFlag.Acquire()==false)
		return;

	ProviderStateThreadProc();
}
//---------------------------------------------------------------------------
void bcProtocolQueueProcessor::ProviderStateThreadProc(void)noexcept
{
	do{
		fProviderStateFlag.Continue();
		ProviderStateProcess();
	}while(fProviderStateFlag.Release()==false);
}
//---------------------------------------------------------------------------
void bcProtocolQueueProcessor::ProviderStateProcess(void)noexcept
{
	if(fQueueStateConnected){
		cnLib_ASSERT(fProtocolProvider!=nullptr);
		if(fProviderConnected){
			// is connected and running

			ProviderNotification();
			return;
		}

		// provider stopped
		fProtocolProvider=nullptr;
		// connection stopped
		fQueueStateConnected=false;
		if(fProcessorSetQueueEnded){
			fProcessorSetQueueEnded=2;
			// close queue
			fProtocolQueueProvider->StopQueue();
		}
		else{
			fProtocolQueueProvider->MakeConnection();
		}
	}
	else{
		if(fProviderConnected){
			// connected
			fQueueStateConnected=true;

			ProviderNotification();
		}
	}
}
//---------------------------------------------------------------------------
void bcProtocolQueueProcessor::ProviderNotification(void)noexcept
{
	if(fConnectionStop){
		fProtocolProvider->StopProcessor(true);
		return;
	}
	if(fConnectionNotifyInput){
		fConnectionNotifyInput=false;
		fProtocolProvider->NotifyInput();
	}
	if(fConnectionNotifyOutput){
		fConnectionNotifyOutput=false;
		fProtocolProvider->NotifyOutput();
	}
	if(fConnectionSetOutputEnded==1){
		fConnectionSetOutputEnded=2;
		fProtocolProvider->SetEndOfOutput();
	}
}
//---------------------------------------------------------------------------
void bcProtocolQueueProcessor::ProtocolStarted(void)noexcept
{
	fProviderConnected=true;
	fConnectionSetOutputEnded=0;
	fConnectionStop=false;

	ProviderStateProcedure();
}
//---------------------------------------------------------------------------
void bcProtocolQueueProcessor::ProtocolStopped(void)noexcept
{
	fProviderConnected=false;

	ProviderStateProcedure();
}
//---------------------------------------------------------------------------
void bcProtocolQueueProcessor::ProviderNotifyInput(uIntn LeastSizeNeeded)noexcept
{
	fConnectionNotifyInput=true;
	if(fProviderStateFlag.Acquire()){
		if(fProtocolProvider!=nullptr){
			fConnectionNotifyInput=false;
			fProtocolProvider->NotifyInput(LeastSizeNeeded);
		}

		ProviderStateThreadProc();
	}
}
//---------------------------------------------------------------------------
void bcProtocolQueueProcessor::ProviderNotifyOutput(uIntn LeastSizeNeeded)noexcept
{
	fConnectionNotifyOutput=true;
	if(fProviderStateFlag.Acquire()){
		if(fProtocolProvider!=nullptr){
			fConnectionNotifyOutput=false;
			fProtocolProvider->NotifyOutput(LeastSizeNeeded);
		}

		ProviderStateThreadProc();
	}
}
//---------------------------------------------------------------------------
void bcProtocolQueueProcessor::ProviderSetEndOfOutput(void)noexcept
{
	if(fConnectionSetOutputEnded)
		return;

	fConnectionSetOutputEnded=1;
	ProviderStateProcedure();
}
//---------------------------------------------------------------------------
void bcProtocolQueueProcessor::ProviderSetEndOfQueue(void)noexcept
{
	if(fProcessorSetQueueEnded)
		return;

	fProcessorSetQueueEnded=1;
	if(fConnectionSetOutputEnded==0)
		fConnectionSetOutputEnded=1;
	ProviderStateProcedure();
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
bcProtocolQueueProvider::bcProtocolQueueProvider()noexcept
	: fProviderQueueCloseState(0)
	, fProviderQueueIsEnded(false)
	, fProcessorNeedConnection(false)
	, fProcessorStarted(false)
{
}
//---------------------------------------------------------------------------
bcProtocolQueueProvider::~bcProtocolQueueProvider()noexcept
{
}
//---------------------------------------------------------------------------
bool bcProtocolQueueProvider::StartQueue(iReference *Reference,iProtocolQueueProcessor *ProtocolQueueProcessor)noexcept
{
	if(fQueueProcessor!=nullptr)
		return false;

	fQueueProcessor=ProtocolQueueProcessor;
	fQueueProcessorReference=Reference;
	fStopProcessor=false;

	ProviderQueueStateProcedure();
	return true;
}
//---------------------------------------------------------------------------
void bcProtocolQueueProvider::StopQueue(void)noexcept
{
	if(fStopProcessor.Free.Xchg(true)==false){
		ProviderQueueStateProcedure();
	}
}
//---------------------------------------------------------------------------
void bcProtocolQueueProvider::MakeConnection(void)noexcept
{
	fProcessorNeedConnection=true;
	ProviderQueueStateProcedure();
}
//---------------------------------------------------------------------------
void bcProtocolQueueProvider::CloseQueue(void)noexcept
{
	if(fProviderQueueCloseState.Acquire.CmpStore(0,1)){
		ProviderQueueStateProcedure();
	}
}
//---------------------------------------------------------------------------
void bcProtocolQueueProvider::MarkQueueEnd(void)noexcept
{
	if(fProviderQueueIsEnded==false){
		fProviderQueueIsEnded=true;
		ProviderQueueStateProcedure();
	}
}
//---------------------------------------------------------------------------
void bcProtocolQueueProvider::ProtocolQueueNotify(void)noexcept
{
	ProviderQueueStateProcedure();
}
//---------------------------------------------------------------------------
void bcProtocolQueueProvider::ProviderQueueStateProcedure(void)noexcept
{
	if(fProviderQueueStateFlag.Acquire()==false)
		return;

	do{
		fProviderQueueStateFlag.Continue();
		ProviderQueueStateProcess();
	}while(fProviderQueueStateFlag.Release()==false);
}
//---------------------------------------------------------------------------
void bcProtocolQueueProvider::ProviderQueueStateProcess(void)noexcept
{
	if(fProviderQueueCloseState){
		if(fQueueProcessor!=nullptr){
			if(fProcessorStarted){
				ProcessorStopped();
			}
			fQueueProcessorReference=nullptr;
			fQueueProcessor=nullptr;
		}
		if(fProviderQueueCloseState==1){
			ProtocolQueueClosed();
			fProviderQueueCloseState=2;
		}
		return;
	}
	if(fQueueProcessor!=nullptr){
		if(fStopProcessor){
			if(fProcessorStarted){
				ProcessorStopped();
			}
			fQueueProcessorReference=nullptr;
			fQueueProcessor=nullptr;
		}
		else{
			if(fProcessorStarted==false){
				ProcessorStarted();
			}
			// check connection
			if(fProcessorNeedConnection){
				rPtr<iProtocolProvider> ProtocolProvider;
				if((ProtocolProvider=ProtocolQueueFetch())!=nullptr){
					fProcessorNeedConnection=false;
					fQueueProcessor->ProtocolQueueConnected(ProtocolProvider);
				}
				else{
					if(fProviderQueueIsEnded){
						// no more connection, close queue
						fProviderQueueCloseState=2;
						ProcessorStopped();
						fQueueProcessorReference=nullptr;
						fQueueProcessor=nullptr;

						ProtocolQueueClosed();
					}
				}
			}
		}
	}
}
//---------------------------------------------------------------------------
void bcProtocolQueueProvider::ProcessorStarted(void)noexcept
{
	fProcessorStarted=true;
	fQueueProcessor->ProtocolQueueStarted();
	ProtocolQueueStarted();
}
//---------------------------------------------------------------------------
void bcProtocolQueueProvider::ProcessorStopped(void)noexcept
{
	fQueueProcessor->ProtocolQueueStopped();
	fProcessorStarted=false;
	ProtocolQueueStopped();
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cProtocolQueueFromEndpoints::cProtocolQueueFromEndpoints()noexcept
{
}
//---------------------------------------------------------------------------
cProtocolQueueFromEndpoints::~cProtocolQueueFromEndpoints()noexcept
{
}
//---------------------------------------------------------------------------
rPtr<iProtocolProvider> cProtocolQueueFromEndpoints::ProtocolQueueFetch(void)noexcept
{
	auto CurItem=fEndpointQueue.Dequeue();
	if(CurItem==nullptr){
		return nullptr;
	}

	auto Provider=CreateProtocolProviderFromEndpoint(CurItem->Connection,CurItem->Endpoint);
	delete CurItem;
	return Provider;
}
//---------------------------------------------------------------------------
void cProtocolQueueFromEndpoints::QueueEndpoint(iEndpoint *Endpoint)noexcept
{
	auto Item=new cEndpointItem;
	Item->Endpoint=Endpoint;
	Item->Connection=iCast<iConnection>(Endpoint);
	fEndpointQueue.Enqueue(Item);

	ProtocolQueueNotify();
}
//---------------------------------------------------------------------------
void cProtocolQueueFromEndpoints::QueueEndpoint(iConnection *Connection,iEndpoint *Endpoint)noexcept
{
	auto Item=new cEndpointItem;
	Item->Endpoint=Endpoint;
	Item->Connection=Connection;
	fEndpointQueue.Enqueue(Item);

	ProtocolQueueNotify();
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
rPtr<cProtocolQueueFromEndpoints> cnRTL::CreateProtocolQueueProviderFromEndpoint(iConnection *Connection,iEndpoint *Endpoint)noexcept
{
	auto Queue=rCreate<cProtocolQueueFromEndpoints>();
	Queue->QueueEndpoint(Connection,Endpoint);
	Queue->MarkQueueEnd();
	return Queue;
}
//---------------------------------------------------------------------------
rPtr<cProtocolQueueFromEndpoints> cnRTL::CreateProtocolQueueProviderFromEndpoint(iEndpoint *Endpoint)noexcept
{
	return CreateProtocolQueueProviderFromEndpoint(iCast<iConnection>(Endpoint),Endpoint);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cProtocolQueueFromConnector::cProtocolQueueFromConnector(iConnectionConnector *Connector,iAddress *RemoteAddress)noexcept
	: fConnector(Connector)
	, fRemoteAddress(RemoteAddress)
{
	fConnectRetryLimit=0xFF;
}
//---------------------------------------------------------------------------
cProtocolQueueFromConnector::~cProtocolQueueFromConnector()noexcept
{
}
//---------------------------------------------------------------------------
void cProtocolQueueFromConnector::VirtualStarted(void)noexcept
{
	InnerActivate('self');
}
//---------------------------------------------------------------------------
void cProtocolQueueFromConnector::VirtualStopped(void)noexcept
{
	if(fConnectTask!=nullptr){
		fConnectTask->Cancel();
	}
	InnerDecReference('self');
}
//---------------------------------------------------------------------------
void cProtocolQueueFromConnector::SetRetryLimit(ufInt8 Limit)noexcept
{
	fConnectRetryLimit=Limit;
}
//---------------------------------------------------------------------------
void cProtocolQueueFromConnector::SetupConnectTask(void)noexcept
{
	fConnectRetryCount++;
	fConnectTask=fConnector->ConnectAsync(fRemoteAddress);
	if(fConnectTask==nullptr){
		return;
	}
	if(fConnectTask->Await(&fConnectTaskCompleteProcedure)){
		InnerIncReference('ctsk');
		return;
	}
}
//---------------------------------------------------------------------------
void cProtocolQueueFromConnector::TaskCompleted(void)noexcept
{
	ProtocolQueueNotify();
	InnerDecReference('ctsk');
}
//---------------------------------------------------------------------------
void cProtocolQueueFromConnector::cConnectTaskCompleteProcedure::Execute(void)noexcept
{
	auto Host=cnMemory::GetObjectFromMemberPointer(this,&cProtocolQueueFromConnector::fConnectTaskCompleteProcedure);
	Host->TaskCompleted();
}
//---------------------------------------------------------------------------
rPtr<iProtocolProvider> cProtocolQueueFromConnector::ProtocolQueueFetch(void)noexcept
{
	if(fConnectTask==nullptr){
		SetupConnectTask();
		if(fConnectTask==nullptr){
			return nullptr;
		}
	}
	if(fConnectTask->IsDone()==false)
		return nullptr;

	auto ConnectTask=cnVar::MoveCast(fConnectTask);
	auto Connection=ConnectTask->GetConnection();
	if(Connection==nullptr){
		// cannot create onnection
		// retry connect
		SetupConnectTask();
		return nullptr;
	}
	auto Endpoint=iCast<iEndpoint>(Connection);
	if(Endpoint==nullptr){
		// error connection
		if(fConnectRetryCount>=fConnectRetryLimit){
			fConnectionActive=false;
		}
		return nullptr;
	}
	return CreateProtocolProviderFromEndpoint(Connection,Endpoint);
}
//---------------------------------------------------------------------------

#if 0


//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
bool bcProtocolQueueProcessor::bcProtocolQueueProvider::SetupProcessor(bcProtocolQueueProcessor *o)
{
	if(Owner!=nullptr)
		return false;

	Owner=o;
	fClearOwner=false;
	return true;
}
//---------------------------------------------------------------------------
void bcProtocolQueueProcessor::bcProtocolQueueProvider::ClearProcessor(void)
{
	fClearOwner=true;

	if(fClearFlag.Acquire()){
	
		fClearFlag.Continue();
		Owner=nullptr;

		if(fClearFlag.Release()==false){
			OwnerNotifyProc();
		}

	}
}
//---------------------------------------------------------------------------
void bcProtocolQueueProcessor::bcProtocolQueueProvider::OwnerNotifyProc(void)
{
	do{
		fClearFlag.Continue();

		if(fClearOwner){
			Owner=nullptr;
		}
		else{
			if(Owner!=nullptr)
				Owner->UpdateProviderQueue();
		}
	}while(fClearFlag.Release()==false);
}
//---------------------------------------------------------------------------
void bcProtocolQueueProcessor::bcProtocolQueueProvider::QueueNotify(void)
{
	if(fClearFlag.Acquire()){
		OwnerNotifyProc();
	}
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
bcProtocolQueueProcessor::cProtocolQueueProviderFromProtocolProvider::cProtocolQueueProviderFromProtocolProvider(bcProtocolQueueProcessor *o,iProtocolProvider *ProtocolProvider)
	: fProvider(ProtocolProvider)
{
	Owner=o;
}
//---------------------------------------------------------------------------
rPtr<iProtocolProvider> bcProtocolQueueProcessor::cProtocolQueueProviderFromProtocolProvider::ProtocolQueueFetch(void)
{
	if(fProvider==nullptr){
		return nullptr;
	}
	auto Provider=cnVar::MoveCast(fProvider);
	fProvider=nullptr;
	QueueIsEnded=true;
	return Provider;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
bcProtocolQueueProcessor::bcProtocolQueueProcessor()
	: fProviderQueueCloseState(0)
	, fProviderQueueActive(false)
	, fProcessorSingleThreaded(false)
	, fQueueStateConnected(false)
	, fQueueStateRunning(false)
	, fProcessorSetProviderEnded(0)
	, fProcessorSetQueueEnded(0)
	, fProcessorNotifyInput(false)
	, fProcessorNotifyOutput(false)
{
}
//---------------------------------------------------------------------------
bcProtocolQueueProcessor::~bcProtocolQueueProcessor()
{
}
//---------------------------------------------------------------------------
bool bcProtocolQueueProcessor::SetQueue(bcProtocolQueueProvider *QueueProvider)
{
	if(fQueueStateRunning){
		return false;
	}

	if(QueueProvider->SetupProcessor(this)==false){
		return false;
	}
	if(fProtocolQueueProvider!=nullptr){
		fProtocolQueueProvider->ClearProcessor();
	}
	fProtocolQueueProvider=QueueProvider;

	return true;
}
//---------------------------------------------------------------------------
bool bcProtocolQueueProcessor::SetProtocolProvider(iProtocolProvider *Provider)
{
	if(fQueueStateRunning){
		return false;
	}

	if(fProtocolQueueProvider!=nullptr){
		fProtocolQueueProvider->ClearProcessor();
	}
	fProtocolQueueProvider=rCreate<cProtocolQueueProviderFromProtocolProvider>(this,Provider);

	return true;
}
//---------------------------------------------------------------------------
bool bcProtocolQueueProcessor::IsConnected(void)const
{
	return fQueueStateConnected;
}
//---------------------------------------------------------------------------
void bcProtocolQueueProcessor::CloseQueue(void)
{
	if(fProviderQueueCloseState.Acquire.CmpStore(0,1)){
		ProviderQueueStateProcedure();
	}
}
//---------------------------------------------------------------------------
bool bcProtocolQueueProcessor::GetQueueActive(void)const
{
	return fProviderQueueActive;
}
//---------------------------------------------------------------------------
bool bcProtocolQueueProcessor::SetQueueActive(bool Active)
{
	if(fProviderQueueActive==Active)
		return false;

	if(fProtocolQueueProvider==nullptr)
		return false;

	fProviderQueueActive=Active;
	ProviderQueueStateProcedure();

	return true;
}
//---------------------------------------------------------------------------
void bcProtocolQueueProcessor::UpdateProviderQueue(void)
{
	ProviderQueueStateProcedure();
}
//---------------------------------------------------------------------------
void bcProtocolQueueProcessor::ProviderQueueStateProcedure(void)
{
	if(fProviderQueueStateFlag.Acquire()==false)
		return;

	ProviderQueueStateThreadProc();
}
//---------------------------------------------------------------------------
void bcProtocolQueueProcessor::ProviderQueueStateThreadProc(void)
{
	do{
		fProviderQueueStateFlag.Continue();
		ProviderQueueStateProcess();
	}while(fProviderQueueStateFlag.Release()==false);
}
//---------------------------------------------------------------------------
void bcProtocolQueueProcessor::ProviderQueueStateProcess(void)
{
	if(fProtocolProvider!=nullptr){
		if(fProviderConnected){
			if(fProviderQueueCloseState==1){
				fProviderQueueCloseState=2;
				fProtocolProvider->StopProcessor(true);
			}
			else{
				// is connected and running
				if(fProcessorNotifyInput){
					fProcessorNotifyInput=false;
					fProtocolProvider->NotifyInput();
				}
				if(fProcessorNotifyOutput){
					fProcessorNotifyOutput=false;
					fProtocolProvider->NotifyOutput();
				}
				if(fProcessorSetProviderEnded==1){
					fProcessorSetProviderEnded=2;
					fProtocolProvider->SetEndOfOutput();
				}
			}
			return;
		}

		// provider stopped
		fProtocolProvider=nullptr;
		// connection stopped
		fQueueStateConnected=false;
		if(fProcessorSetQueueEnded){
			fProcessorSetQueueEnded=1;
			if(fProviderQueueCloseState==0){
				fProviderQueueCloseState=2;
			}
		}
	}
	cnLib_ASSERT(fProtocolProvider==nullptr);
	if(fProviderQueueCloseState){
		if(fQueueStateRunning){
			// stop running
			fQueueStateRunning=false;
			ProtocolQueueStopped();
		}
		if(fProviderQueueCloseState==2){
			ProtocolQueueClosed();
			fProviderQueueCloseState=3;
		}
		return;
	}
	if(fProtocolQueueProvider==nullptr){
		// no queue
		return;
	}

	// not connected
	if(fQueueStateRunning==false){
		// idle
		if(fProviderQueueActive==false){
			// keep idle
			return;
		}
		// start running
		fQueueStateRunning=true;
		ProtocolQueueStarted();
		fProtocolQueueProvider->ProtocolQueueStarted();
	}
	else{
		if(fProviderQueueActive==false){
			// stop running
			fQueueStateRunning=false;
			fProtocolQueueProvider->ProtocolQueueStopped();
			ProtocolQueueStopped();
			return;
		}
	}
	// is running
	// try to connect new provider
	while((fProtocolProvider=fProtocolQueueProvider->ProtocolQueueFetch())!=nullptr){
		fProviderConnected=true;
		fQueueStateConnected=true;
		if(fProtocolProvider->StartProcessor(ProtocolQueueReference(),this,fProcessorSingleThreaded)){
			return;
		}
		fQueueStateConnected=false;

		// retry fetch new provider
		fProtocolProvider=nullptr;
	}
	if(fProtocolQueueProvider->QueueIsEnded){
		// no more provider
		fProtocolQueueProvider->ProtocolQueueStopped();
		fProtocolQueueProvider=nullptr;
		fQueueStateRunning=false;
	}
	// wait for more provider
}
//---------------------------------------------------------------------------
void bcProtocolQueueProcessor::ProtocolStarted(void)
{
	cnLib_ASSERT(fProviderConnected);
	fProcessorSetProviderEnded=0;
}
//---------------------------------------------------------------------------
void bcProtocolQueueProcessor::ProtocolStopped(void)
{
	fProviderConnected=false;
	ProviderQueueStateProcedure();
}
//---------------------------------------------------------------------------
void bcProtocolQueueProcessor::ProviderNotifyInput(uIntn LeastSizeNeeded)
{
	fProcessorNotifyInput=true;
	if(fProviderQueueStateFlag.Acquire()){
		if(fProtocolProvider!=nullptr){
			fProcessorNotifyInput=false;
			fProtocolProvider->NotifyInput(LeastSizeNeeded);
		}

		ProviderQueueStateThreadProc();
	}
}
//---------------------------------------------------------------------------
void bcProtocolQueueProcessor::ProviderNotifyOutput(uIntn LeastSizeNeeded)
{
	fProcessorNotifyOutput=true;
	if(fProviderQueueStateFlag.Acquire()){
		if(fProtocolProvider!=nullptr){
			fProcessorNotifyOutput=false;
			fProtocolProvider->NotifyOutput(LeastSizeNeeded);
		}

		ProviderQueueStateThreadProc();
	}
}
//---------------------------------------------------------------------------
void bcProtocolQueueProcessor::ProviderSetEndOfOutput(void)
{
	if(fProcessorSetProviderEnded)
		return;

	fProcessorSetProviderEnded=1;
	ProviderQueueStateProcedure();
}
//---------------------------------------------------------------------------
void bcProtocolQueueProcessor::ProviderSetEndOfQueue(void)
{
	if(fProcessorSetQueueEnded)
		return;

	fProcessorSetQueueEnded=1;
	if(fProcessorSetProviderEnded==0)
		fProcessorSetProviderEnded=1;
	ProviderQueueStateProcedure();
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cProtocolConnectorQueueProvider::cProtocolConnectorQueueProvider(iConnectionConnector *Connector,iAddress *RemoteAddress)
	: fConnector(Connector)
	, fRemoteAddress(RemoteAddress)
{
	fConnectRetryLimit=0xFF;
}
//---------------------------------------------------------------------------
cProtocolConnectorQueueProvider::~cProtocolConnectorQueueProvider()
{
}
//---------------------------------------------------------------------------
void cProtocolConnectorQueueProvider::VirtualStopped(void)
{
	if(fConnectTask!=nullptr){
		fConnectTask->Cancel();
	}
	InnerDecReference('self');
}
//---------------------------------------------------------------------------
void cProtocolConnectorQueueProvider::SetRetryLimit(ufInt8 Limit)
{
	fConnectRetryLimit=Limit;
}
//---------------------------------------------------------------------------
void cProtocolConnectorQueueProvider::SetupConnectTask(void)
{
	fConnectRetryCount++;
	fConnectTask=fConnector->ConnectAsync(fRemoteAddress);
	if(fConnectTask==nullptr){
		return;
	}
	if(fConnectTask->SetNotify(&fConnectTaskCompleteProcedure)){
		InnerIncReference('ctsk');
		return;
	}
}
//---------------------------------------------------------------------------
void cProtocolConnectorQueueProvider::TaskCompleted(void)
{
	QueueNotify();
	InnerDecReference('ctsk');
}
//---------------------------------------------------------------------------
void cProtocolConnectorQueueProvider::cConnectTaskCompleteProcedure::Execute(void)
{
	auto Host=cnMemory::GetObjectFromMemberPointer(this,&cProtocolConnectorQueueProvider::fConnectTaskCompleteProcedure);
	Host->TaskCompleted();
}
//---------------------------------------------------------------------------
void cProtocolConnectorQueueProvider::ProtocolQueueStarted(void)
{
	if(fConnectTask==nullptr){
		SetupConnectTask();
	}
}
//---------------------------------------------------------------------------
void cProtocolConnectorQueueProvider::ProtocolQueueStopped(void)
{
}
//---------------------------------------------------------------------------
rPtr<iProtocolProvider> cProtocolConnectorQueueProvider::ProtocolQueueFetch(void)
{
	if(fConnectTask==nullptr){
		SetupConnectTask();
		if(fConnectTask==nullptr){
			return nullptr;
		}
	}
	else{
		if(fConnectTask->IsDone()==false)
			return nullptr;
	}

	auto ConnectTask=cnVar::MoveCast(fConnectTask);
	auto Connection=ConnectTask->GetConnection();
	if(Connection==nullptr){
		// cannot create onnection
		// retry connect
		SetupConnectTask();
		return nullptr;
	}
	auto Endpoint=iCast<iEndpoint>(Connection);
	if(Endpoint==nullptr){
		// error connection
		if(fConnectRetryCount>=fConnectRetryLimit){
			fConnectionActive=false;
		}
		return nullptr;
	}
	return CreateProtocolProviderFromEndpoint(Connection,Endpoint);
}
#endif // 0

#if 0

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
bcProtocolProviderFromRWQueue::bcProtocolProviderFromRWQueue()
	: fProtocolProcessor(nullptr)
{
	fConnStateConnected=false;
	fConnStateProcessStarted=false;
	fProcessorStopped=true;
	fReadEnded=false;
	fReadActive=false;
	fWriteEnded=false;
	fWriteActive=false;
	fProviderCloseState=0;
	fTerminateConnection=false;
}
//---------------------------------------------------------------------------
bcProtocolProviderFromRWQueue::~bcProtocolProviderFromRWQueue()
{
}
//---------------------------------------------------------------------------
bool bcProtocolProviderFromRWQueue::IsConnected(void)const
{
	return fConnStateConnected;
}
//---------------------------------------------------------------------------
void bcProtocolProviderFromRWQueue::CloseProvider(bool Terminate)
{
	if(Terminate){
		fTerminateConnection=true;
	}
	if(fProviderCloseState.Acquire.CmpStore(0,1)){
		ConnectionStateProcedure();
	}
}
//---------------------------------------------------------------------------
void bcProtocolProviderFromRWQueue::StartRWQueue(void)
{
	fInputNeedSize=0;
	fOutputNeedSize=0;
	fSetWriteEnd=false;
	fWriteClose=false;
	fReadClose=false;
	fReadEnded=false;
	fWriteEnded=false;
	fProcessorInputNotify=false;
	fProcessorOutputNotify=false;
	fTerminateConnection=false;
	fWriteActive=true;
	fReadActive=true;

	if(fWriteQueue->StartWrite(this,this)==false){
		fReadActive=false;
		fWriteActive=false;
		fWriteQueue=nullptr;
		fReadQueue=nullptr;
		return;
	}
	fConnStateConnected=true;
	if(fReadQueue->StartRead(this,this)==false){
		fReadActive=false;
		fWriteQueue->StopWrite(false);
		fConnStateConnectSuccess=false;
		return;
	}
	fConnStateConnectSuccess=true;
}
//---------------------------------------------------------------------------
void bcProtocolProviderFromRWQueue::TerminateRWQueue(void)
{
	if(fProcessorSingleThreaded){
		bool Closed=false;
		if(fReadClose==false){
			fReadClose=true;
			Closed=true;
		}
		if(fWriteClose==false){
			fWriteClose=true;
			Closed=true;
		}
		if(Closed){
			return SingleThreadStateProcedure();
		}
		return;
	}
	else{
		if(fReadClose==false){
			fReadClose=true;
			ReadThreadStateProcedure();
		}
		if(fWriteClose==false){
			fWriteClose=true;
			WriteThreadStateProcedure();
		}
	}
}
//---------------------------------------------------------------------------
void bcProtocolProviderFromRWQueue::NotifyConnectionState(void)
{
	ConnectionStateProcedure();
}
//---------------------------------------------------------------------------
bool bcProtocolProviderFromRWQueue::StartProcessor(iReference *Reference,iProtocolProcessor *ProtocolProcessor,bool SingleThreaded)
{
	if(fProviderCloseState)
		return false;
	if(fProtocolProcessor!=nullptr)
		return false;

	fProtocolProcessor=ProtocolProcessor;
	fProtocolProcessorReference=Reference;
	fProcessorStopped=false;
	fProcessorSingleThreaded=SingleThreaded;

	ConnectionStateProcedure();
	return true;
}
//---------------------------------------------------------------------------
void bcProtocolProviderFromRWQueue::StopProcessor(void)
{
	if(fProcessorStopped.Free.Xchg(true)==false){
		ConnectionStateProcedure();
	}
}
//---------------------------------------------------------------------------
void bcProtocolProviderFromRWQueue::TerminateConnection(void)
{
	TerminateRWQueue();
}
//---------------------------------------------------------------------------
void bcProtocolProviderFromRWQueue::ConnectionStateProcedure(void)
{
	if(fConnectionStateFlag.Acquire()==false)
		return;

	do{
		fConnectionStateFlag.Continue();
		while(ConnectionStateProcess());
	}while(fConnectionStateFlag.Release()==false);

}
//---------------------------------------------------------------------------
bool bcProtocolProviderFromRWQueue::ConnectionStateProcess(void)
{
	if(fConnStateConnected){
		cnLib_ASSERT(fProtocolProcessor!=nullptr);	// should not enter connected state without the processor

		if(fReadActive==false && fWriteActive==false){
			fReadQueue=nullptr;
			fWriteQueue=nullptr;
			// connection stopped
			fConnStateConnected=false;

			if(fConnStateConnectSuccess){
				fProtocolProcessor->ProtocolDisconnected();

				ProviderConnectionStopped();
			}

			goto ContinueConnect;
		}
		else{
			if(fTerminateConnection){
				TerminateRWQueue();
			}
		}

	}
	else{
		// check processor state
		if(fConnStateProcessStarted){
ContinueConnect:
			if(fProcessorStopped){
				// processor stopped
				fProtocolProcessor->ProtocolStopped();
				fProtocolProcessor=nullptr;
				fProtocolProcessorReference=nullptr;
				fConnStateProcessStarted=false;
				return true;
			}
			else{
				// start connect
				if(ProviderFetchQueue()){
					if(fReadQueue==nullptr){
						fWriteQueue=nullptr;
						// unusable queue
						ProviderConnectionFailed();
					}
					else if(fWriteQueue==nullptr){
						fReadQueue=nullptr;
						// unusable queue
						ProviderConnectionFailed();
					}
					else{
						StartRWQueue();
						if(fConnStateConnectSuccess){
							fProtocolProcessor->ProtocolConnected();
							ProviderConnectionStarted();
						}
						else{
							ProviderConnectionFailed();
						}
					}
				}

			}
		}
		else{
			if(fProtocolProcessor!=nullptr){
				// processor started
				fProtocolProcessor->ProtocolStarted();
				fConnStateProcessStarted=true;
				RWStateProcedure();
				return true;
			}
			else{
				if(fProviderCloseState){
					if(fProviderCloseState==1){
						fProviderCloseState=2;
						ProviderClosed();
					}
				}
				else{
					ProviderIdle();
				}
			}
		}
	}
	return false;
}
//---------------------------------------------------------------------------
bool bcProtocolProviderFromRWQueue::ReadStateProcess(void)
{
	if(fConnStateConnected==false)
		return false;

	if(fReadQueue==nullptr){
		return false;
	}

	if(fReadClose){
		fProtocolProcessor->ProtocolInputClosed(fReadEnded);
		fReadQueue->StopRead();

		return false;
	}

	bool Continue=false;
	if(fProcessorInputNotify){
		Continue=ReadQueueProcess();
		fProcessorInputNotify=false;
	}
	if(Continue==false){
		fReadQueue->NotifyRead(1);
	}
	return Continue;
}
//---------------------------------------------------------------------------
bool bcProtocolProviderFromRWQueue::WriteStateProcess(void)
{
	if(fConnStateConnected==false)
		return false;

	if(fWriteQueue==nullptr)
		return false;


	if(fWriteClose){
		bool Terminate=fWriteEnded;
		fProtocolProcessor->ProtocolOutputClosed(!Terminate);
		fWriteQueue->StopWrite(Terminate);
		return false;
	}

	bool Continue=false;
	if(fProcessorOutputNotify){
		Continue=WriteQueueProcess();
		fProcessorOutputNotify=false;
	}
	if(Continue==false){
		fWriteQueue->NotifyWrite(fSetWriteEnd);
	}
	return Continue;
}
//---------------------------------------------------------------------------
void bcProtocolProviderFromRWQueue::ReadStateProcedure(void)
{
	if(fProcessorSingleThreaded){
		return SingleThreadStateProcedure();
	}
	else{
		return ReadThreadStateProcedure();
	}
}
//---------------------------------------------------------------------------
void bcProtocolProviderFromRWQueue::WriteStateProcedure(void)
{
	if(fProcessorSingleThreaded){
		return SingleThreadStateProcedure();
	}
	else{
		return WriteThreadStateProcedure();
	}
}
//---------------------------------------------------------------------------
void bcProtocolProviderFromRWQueue::RWStateProcedure(void)
{
	if(fProcessorSingleThreaded){
		SingleThreadStateProcedure();
	}
	else{
		WriteThreadStateProcedure();
		ReadThreadStateProcedure();
	}
}
//---------------------------------------------------------------------------
void bcProtocolProviderFromRWQueue::SingleThreadStateProcedure(void)
{
	if(fWriteStateFlag.Acquire()==false){
		return;
	}
	do{
		fWriteStateFlag.Continue();

		ReadStateProcess();
		WriteStateProcess();

	}while(fWriteStateFlag.Release()==false);
}
//---------------------------------------------------------------------------
void bcProtocolProviderFromRWQueue::ReadThreadStateProcedure(void)
{
	if(fReadStateFlag.Acquire()==false){
		return;
	}
	do{
		do{
			fReadStateFlag.Continue();

		}while(ReadStateProcess());

	}while(fReadStateFlag.Release()==false);
}
//---------------------------------------------------------------------------
void bcProtocolProviderFromRWQueue::WriteThreadStateProcedure(void)
{
	if(fWriteStateFlag.Acquire()==false){
		return;
	}
	do{

		do{
			fWriteStateFlag.Continue();

		}while(WriteStateProcess());
	}while(fWriteStateFlag.Release()==false);
}
//---------------------------------------------------------------------------
void bcProtocolProviderFromRWQueue::NotifyInput(uIntn LeastSizeNeeded)
{
	fInputNeedSize=LeastSizeNeeded;
	fProcessorInputNotify=true;
	if(fConnStateConnected){
		ReadStateProcedure();
	}
}
//---------------------------------------------------------------------------
void bcProtocolProviderFromRWQueue::NotifyOutput(uIntn LeastSizeNeeded)
{
	fOutputNeedSize=LeastSizeNeeded;
	fProcessorOutputNotify=true;
	if(fConnStateConnected){
		WriteStateProcedure();
	}
}
//---------------------------------------------------------------------------
void bcProtocolProviderFromRWQueue::SetEndOfOutput(void)
{
	if(fWriteClose)
		return;

	fProcessorOutputNotify=true;
	fSetWriteEnd=true;
	WriteStateProcedure();
}
//---------------------------------------------------------------------------
void bcProtocolProviderFromRWQueue::ReadStarted(void)
{
	cnLib_ASSERT(fReadActive==true);
	fReadQueue->NotifyRead(1);
	ConnectionStateProcedure();
}
//---------------------------------------------------------------------------
void bcProtocolProviderFromRWQueue::ReadStopped(void)
{
	fReadActive=false;
	ConnectionStateProcedure();
}
//---------------------------------------------------------------------------
void bcProtocolProviderFromRWQueue::ReadNotify(void)
{
	fProcessorInputNotify=true;
	ReadStateProcedure();
}
//---------------------------------------------------------------------------
void bcProtocolProviderFromRWQueue::WriteStarted(void)
{
	cnLib_ASSERT(fWriteActive==true);
	fWriteQueue->NotifyWrite(false);
	ConnectionStateProcedure();
}
//---------------------------------------------------------------------------
void bcProtocolProviderFromRWQueue::WriteStopped(void)
{
	fWriteActive=false;
	ConnectionStateProcedure();
}
//---------------------------------------------------------------------------
void bcProtocolProviderFromRWQueue::WriteNotify(void)
{
	fProcessorOutputNotify=true;
	WriteStateProcedure();
}
//---------------------------------------------------------------------------
bool bcProtocolProviderFromRWQueue::QueryReadBuffer(cConstMemory &Buffer)
{
	auto PackBuffer=fInputPacker.GetReadBuffer();
	if(PackBuffer.Length>fInputNeedSize){
		Buffer=PackBuffer;
		return true;
	}
	uIntn QuerySize=fInputNeedSize-PackBuffer.Length;
	auto EndpointBuffer=fReadQueue->GatherReadBuffer(QuerySize);
	if(EndpointBuffer.Length==0)
		return false;	// no more buffer
	if(PackBuffer.Length==0){
		if(EndpointBuffer.Length>=fInputNeedSize){
			Buffer=EndpointBuffer;
			return true;
		}
	}

	// join up buffer
	auto AppendBuffer=fInputPacker.ReserveWriteBuffer(EndpointBuffer.Length);
	if(AppendBuffer.Length>EndpointBuffer.Length){
		AppendBuffer.Length=EndpointBuffer.Length;
	}
	cnMemory::Copy(AppendBuffer.Pointer,EndpointBuffer.Pointer,AppendBuffer.Length);
	fInputPacker.CommitWriteBuffer(AppendBuffer.Length);

	fReadQueue->DismissReadBuffer(AppendBuffer.Length);

	// check if total buffer fits needs
	PackBuffer=fInputPacker.GetReadBuffer();
	if(PackBuffer.Length>fInputNeedSize){
		Buffer=PackBuffer;
		return true;
	}

	return false;
}
//---------------------------------------------------------------------------
void bcProtocolProviderFromRWQueue::AdvanceReadBuffer(uIntn Size)
{
	auto PackBuffer=fInputPacker.GetReadBuffer();
	if(PackBuffer.Length==0){
		fReadQueue->DismissReadBuffer(Size);
	}
	else{
		fInputPacker.DismissReadBuffer(Size);
	}
}
//---------------------------------------------------------------------------
bool bcProtocolProviderFromRWQueue::ReadQueueProcess(void)
{
	cConstMemory ReadBuffer;
	while(QueryReadBuffer(ReadBuffer)){
		fInputNeedSize=0;
		uIntn SizePushed=fProtocolProcessor->ProtocolInputPush(ReadBuffer.Pointer,ReadBuffer.Length,fInputNeedSize);
		if(SizePushed==0){
			if(fInputNeedSize<=ReadBuffer.Length){
				fInputNeedSize=ReadBuffer.Length+1;
			}
		}
		else{
			AdvanceReadBuffer(SizePushed);
		}
	}

	bool GracefulClose;
	if(fReadQueue->IsReadClosed(GracefulClose)){
		fReadEnded=GracefulClose;
		fReadClose=true;
		return true;
	}
	return false;
}
//---------------------------------------------------------------------------
bool bcProtocolProviderFromRWQueue::QueryWriteBuffer(cMemory &Buffer)
{
	Buffer=fWriteQueue->ReserveWriteBuffer(fOutputNeedSize);
	if(Buffer.Length>=fOutputNeedSize){
		return true;
	}
	return Buffer.Length>=fOutputNeedSize;
}
//---------------------------------------------------------------------------
void bcProtocolProviderFromRWQueue::AdvanceWriteBuffer(uIntn Size)
{
	fWriteQueue->CommitWriteBuffer(Size);
}
//---------------------------------------------------------------------------
bool bcProtocolProviderFromRWQueue::WriteQueueProcess(void)
{
	bool GracefulClose;
	if(fWriteQueue->IsWriteClosed(GracefulClose)){
		fSetWriteEnd=false;
		fWriteClose=true;
		fWriteEnded=GracefulClose;
		return true;
	}

	cMemory WriteBuffer;
	while(QueryWriteBuffer(WriteBuffer)){
		fOutputNeedSize=0;
		uIntn SizePulled=fProtocolProcessor->ProtocolOutputPull(WriteBuffer.Pointer,WriteBuffer.Length,fOutputNeedSize);
		if(SizePulled==0){
			if(fSetWriteEnd){
				// no more pulling
				fWriteClose=true;
				fWriteEnded=true;
				return true;
			}
			return false;
		}
		AdvanceWriteBuffer(SizePulled);
	}
	return false;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cProtocolProviderFromEndpoint::cProtocolProviderFromEndpoint()
{
	fQueueSetEnd=false;
}
//---------------------------------------------------------------------------
cProtocolProviderFromEndpoint::~cProtocolProviderFromEndpoint()
{
}
//---------------------------------------------------------------------------
bool cProtocolProviderFromEndpoint::ProviderFetchQueue(void)
{
	if(fCurItem!=nullptr)
		return false;
	fCurItem=fEndpointQueue.Dequeue();
	if(fCurItem==nullptr){
		if(fQueueSetEnd){
			CloseProvider(false);
		}
		return false;
	}
	fReadQueue=fCurItem->Endpoint->GetReadQueue();
	fWriteQueue=fCurItem->Endpoint->GetWriteQueue();

	return true;
}
//---------------------------------------------------------------------------
void cProtocolProviderFromEndpoint::ProviderConnectionFailed(void)
{
	if(fCurItem->KeepOpen==false){
		fCurItem->Endpoint->SetWriteEnd();
	}
	delete fCurItem;
	fCurItem=nullptr;
}
//---------------------------------------------------------------------------
void cProtocolProviderFromEndpoint::ProviderConnectionStarted(void)
{
	if(fCurItem->KeepOpen==false){
		fCurItem->Endpoint->SetWriteEnd();
	}
	delete fCurItem;
	fCurItem=nullptr;
}
//---------------------------------------------------------------------------
void cProtocolProviderFromEndpoint::ProviderIdle(void)
{
	if(fQueueSetEnd){
		if(fEndpointQueue.IsEmpty()){
			CloseProvider(false);
		}
	}
}
//---------------------------------------------------------------------------
void cProtocolProviderFromEndpoint::QueueEndpoint(iEndpoint *Endpoint,bool KeepOpen)
{
	auto Item=new cEndpointItem;
	Item->Endpoint=Endpoint;
	Item->KeepOpen=KeepOpen;
	fEndpointQueue.Enqueue(Item);

	NotifyConnectionState();
}
//---------------------------------------------------------------------------
void cProtocolProviderFromEndpoint::MarkQueueEnd(void)
{
	fQueueSetEnd=true;
	NotifyConnectionState();
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cProtocolProviderFromConnector::cProtocolProviderFromConnector()
{
	fConnectRetryLimit=0xFF;
}
//---------------------------------------------------------------------------
cProtocolProviderFromConnector::~cProtocolProviderFromConnector()
{
}
//---------------------------------------------------------------------------
void cProtocolProviderFromConnector::VirtualStopped(void)
{
	CloseProvider(true);

	InnerDecReference('self');
}
//---------------------------------------------------------------------------
bool cProtocolProviderFromConnector::SetConnector(iConnectionConnector *Connector,iAddress *RemoteAddress)
{
	fConnector=Connector;
	fRemoteAddress=RemoteAddress;
	return true;
}
//---------------------------------------------------------------------------
bool cProtocolProviderFromConnector::SetConnectionProtocol(iConnectionProtocol *Protocol,iAddress *RemoteAddress,iAddress *LocalAddress)
{
	fConnector=Protocol->Connector(iTypeOf<iEndpointConnection>(),LocalAddress,RemoteAddress);
	fRemoteAddress=RemoteAddress;
	return true;
}
//---------------------------------------------------------------------------
bool cProtocolProviderFromConnector::GetConnectionActive(void)const
{
	return fConnectionActive;
}
//---------------------------------------------------------------------------
void cProtocolProviderFromConnector::SetConnectionActive(bool Active)
{
	fConnectRetryCount=0;
	fConnectionActive=Active;
	NotifyConnectionState();
}
//---------------------------------------------------------------------------
void cProtocolProviderFromConnector::SetConnectionRetryLimit(ufInt8 Limit)
{
	fConnectRetryLimit=Limit;
}
//---------------------------------------------------------------------------
void cProtocolProviderFromConnector::ProviderClosed(void)
{
	if(fConnectTask!=nullptr){
		fConnectTask->Cancel();
	}
}
//---------------------------------------------------------------------------
void cProtocolProviderFromConnector::ProviderConnectionStarted(void)
{
	OnConnectionStateChanged();
}
//---------------------------------------------------------------------------
void cProtocolProviderFromConnector::ProviderConnectionStopped(void)
{
	OnConnectionStateChanged();
}
//---------------------------------------------------------------------------
void cProtocolProviderFromConnector::SetupConnectTask(void)
{
	fConnectRetryCount++;
	fConnectTask=fConnector->ConnectAsync(fRemoteAddress);
	if(fConnectTask==nullptr){
		return;
	}
	if(fConnectTask->SetNotify(&fConnectTaskCompleteProcedure)){
		InnerIncReference('ctsk');
		return;
	}
}
//---------------------------------------------------------------------------
bool cProtocolProviderFromConnector::ProviderFetchQueue(void)
{
	if(fConnectionActive==false)
		return false;
	if(fConnectTask==nullptr){
		SetupConnectTask();
		if(fConnectTask==nullptr){
			return false;
		}
		if(fConnectTask->SetNotify(&fConnectTaskCompleteProcedure)){
			InnerIncReference('ctsk');
			return false;
		}
	}
	else{
		if(fConnectTask->IsDone()==false)
			return false;
	}

	auto ConnectTask=cnVar::MoveCast(fConnectTask);
	auto Connection=ConnectTask->GetConnection();
	if(Connection==nullptr){
		// cannot create onnection
		// retry connect
		SetupConnectTask();
		return false;
	}
	auto EndpointConnection=iCast<iEndpointConnection>(Connection);
	if(EndpointConnection==nullptr){
		// error connection
		if(fConnectRetryCount>=fConnectRetryLimit){
			fConnectionActive=false;
		}
		return false;
	}
	auto Endpoint=EndpointConnection->GetEndpoint();
	fReadQueue=Endpoint->GetReadQueue();
	fWriteQueue=Endpoint->GetWriteQueue();
	return true;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void cProtocolProviderFromConnector::cConnectTaskCompleteProcedure::Execute(void)
{
	auto Host=cnMemory::GetObjectFromMemberPointer(this,&cProtocolProviderFromConnector::fConnectTaskCompleteProcedure);
	Host->NotifyConnectionState();
	Host->InnerDecReference('ctsk');
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cPipelineMessageProtocolHost::cPipelineMessageProtocolHost()
{
}
//---------------------------------------------------------------------------
cPipelineMessageProtocolHost::~cPipelineMessageProtocolHost()
{
}
//---------------------------------------------------------------------------
void cPipelineMessageProtocolHost::NotifyRequest(void)
{
	fProtocolProvider->NotifyInput();
}
//---------------------------------------------------------------------------
bool cPipelineMessageProtocolHost::Start(iProtocolProvider *ProtocolProvider)
{
	if(fProtocolProvider!=nullptr)
		return false;
	
	rIncReference(this,'http');
	fHostChannelRefCount=2;

	fProtocolProvider=ProtocolProvider;

	return fProtocolProvider->StartProcessor(nullptr,this,false);
}
//---------------------------------------------------------------------------
void cPipelineMessageProtocolHost::Stop(void)
{
	fProtocolProvider->NotifyOutput(0);
}
//---------------------------------------------------------------------------
bool cPipelineMessageProtocolHost::StartProcessor(iReference *Reference,iProtocolProcessor *ProtocolProcessor,bool SingleThreaded)
{
	return false;
}
//---------------------------------------------------------------------------
void cPipelineMessageProtocolHost::StopProcessor(void)
{
}
//---------------------------------------------------------------------------
void cPipelineMessageProtocolHost::ProtocolStarted(void)
{
}
//---------------------------------------------------------------------------
void cPipelineMessageProtocolHost::ProtocolStopped(void)
{
}
//---------------------------------------------------------------------------
void cPipelineMessageProtocolHost::ProtocolConnected(void)
{
}
//---------------------------------------------------------------------------
void cPipelineMessageProtocolHost::ProtocolDisconnected(void)
{
}
//---------------------------------------------------------------------------
void cPipelineMessageProtocolHost::HostChannelRelease(void)
{
	if(fHostChannelRefCount.Free--==1){
		fProtocolProvider=nullptr;

		fSendingMessage=nullptr;
		fReceivingMessage=nullptr;

		rDecReference(this,'http');
	}
}
//---------------------------------------------------------------------------
void cPipelineMessageProtocolHost::NotifyInput(uIntn LeastSizeNeeded)
{
	fProtocolProvider->NotifyInput(LeastSizeNeeded);
}
//---------------------------------------------------------------------------
void cPipelineMessageProtocolHost::TerminateConnection(void)
{
	fMessageInputDone=true;
	fProtocolProvider->NotifyInput(0);
	fProtocolProvider->NotifyOutput(0);
}
//---------------------------------------------------------------------------
void cPipelineMessageProtocolHost::NotifyOutput(uIntn LeastSizeNeeded)
{
	fProtocolProvider->NotifyOutput(LeastSizeNeeded);
}
//---------------------------------------------------------------------------
void cPipelineMessageProtocolHost::SetEndOfOutput(void)
{
	fMessageOutputDone=true;
	fProtocolProvider->NotifyOutput(0);
}
//---------------------------------------------------------------------------
void cPipelineMessageProtocolHost::FetchSendMessage(void)
{
	if(Callback!=nullptr){
		fMessageOutputDone=false;
		fSendingMessage=Callback->QueryMessage(this);
		if(fSendingMessage!=nullptr){
			// send to wait response queue
			auto RequestItem=new cRequestItem;
			RequestItem->Message=fSendingMessage;
			fReceiveQueue.Enqueue(RequestItem);

			fProtocolProvider->NotifyInput();
		}
	}
}
//---------------------------------------------------------------------------
uIntn cPipelineMessageProtocolHost::ProtocolOutputPull(void *Buffer,uIntn BufferSize,uIntn &)
{
	if(fSendingMessage==nullptr){
		FetchSendMessage();
		if(fSendingMessage==nullptr){
			// no sending message
			return 0;
		}
	}
	// fSendingMessage!=nullptr;
	do{
		uIntn LeastSizeNeeded=0;
		uIntn PulledSize=fSendingMessage->ProtocolOutputPull(Buffer,BufferSize,LeastSizeNeeded);
		if(PulledSize!=0){
			// return sent data
			return PulledSize;
		}
		// no data to send
		if(fMessageOutputDone==false){
			// wait for more message data
			return 0;
		}
		// message done, next message
		fSendingMessage=nullptr;
		FetchSendMessage();

	}while(fSendingMessage!=nullptr);
	return 0;
}
//---------------------------------------------------------------------------
void cPipelineMessageProtocolHost::ProtocolOutputClosed(bool GracefulClose)
{
	if(fSendingMessage!=nullptr){
		fSendingMessage->ProtocolOutputClosed(GracefulClose);
		fSendingMessage=nullptr;
	}
	HostChannelRelease();
}
//---------------------------------------------------------------------------
void cPipelineMessageProtocolHost::FetchReceiveMessage(void)
{
	// pull request
	auto QueueItem=fReceiveQueue.Dequeue();
	if(QueueItem==nullptr){
		// no request??
		return;
	}
	fMessageInputDone=false;
	fReceivingMessage=cnVar::MoveCast(QueueItem->Message);
	delete QueueItem;
}
//---------------------------------------------------------------------------
uIntn cPipelineMessageProtocolHost::ProtocolInputPush(const void *Buffer,uIntn BufferSize,uIntn &)
{
	if(fReceivingMessage==nullptr){
		// pull request
		FetchReceiveMessage();
		if(fReceivingMessage==nullptr)
			return 0;
	}
	else{
		if(fMessageInputDone){
			fReceivingMessage->ProtocolInputClosed(true);
			fReceivingMessage=nullptr;
			FetchReceiveMessage();
			if(fReceivingMessage==nullptr)
				return 0;
		}
	}
	// fReceivingMessage!=nullptr
	do{
		uIntn LeastSizeNeeded=0;
		uIntn ProcessedSize=fReceivingMessage->ProtocolInputPush(Buffer,BufferSize,LeastSizeNeeded);

		if(fMessageInputDone==false){
			// continue message
			
			return ProcessedSize;
		}

		// message done

		fReceivingMessage->ProtocolInputClosed(true);
		fReceivingMessage=nullptr;

		// continue next message
		FetchReceiveMessage();
	}while(fReceivingMessage!=nullptr);
	return 0;
}
//---------------------------------------------------------------------------
void cPipelineMessageProtocolHost::ProtocolInputClosed(bool GracefulClose)
{
	if(fReceivingMessage!=nullptr){
		fReceivingMessage->ProtocolInputClosed(GracefulClose);
		fReceivingMessage=nullptr;
	}
	auto PendingRespondItem=fReceiveQueue.DequeueAll();
	while(PendingRespondItem!=nullptr){
		auto DeleteItem=PendingRespondItem;
		PendingRespondItem=PendingRespondItem->Next;

		DeleteItem->Message->ProtocolInputClosed(false);
		delete DeleteItem;
	}


	HostChannelRelease();
}
#endif // 0
//---------------------------------------------------------------------------
