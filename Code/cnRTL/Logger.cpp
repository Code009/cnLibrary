//---------------------------------------------------------------------------
#include "Logger.h"
#include "TextProcess.h"
#include "StreamBuffer.h"

using namespace cnLibrary;
using namespace cnRTL;

//---------------------------------------------------------------------------
cLog<cnRTL_LOGLEVEL> cnRTL::gRTLLog;
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cErrorFrame::~cErrorFrame()noexcept(true)
{
	fMaker.Submit();
}
//---------------------------------------------------------------------------
void cErrorFrame::SetMsg(const uChar16 *Text,uIntn Length)noexcept(true)
{
	fMaker.SetMessage(Text,Length);
}
//---------------------------------------------------------------------------
cnSystem::ErrorReportMaker cErrorFrame::Make(const uChar16 *Function,uIntn Length)noexcept(true)
{
	// clear error in tls
	auto Report=cnSystem::ErrorReportManager::Fetch();	
	if(Report!=nullptr){
		// TODO : report ignored error report
	}

	return cnSystem::ErrorReportManager::MakeReport(Function,Length);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void cErrorReport::Retrieve(void)noexcept(true)
{
	fReport=cnSystem::ErrorReportManager::Fetch();
}
//---------------------------------------------------------------------------
bool cErrorReport::Query(tTypeID ErrorTypeID,const void* &Data)noexcept(true){
	if(fReport==nullptr)
		return false;

	auto Errors=fReport->Errors();
	for(uIntn i=0;i<Errors.Length;i++){
		auto ErrorData=Errors.Pointer[i];
			
		if(ErrorData.ErrorTypeID==ErrorTypeID){
			Data=ErrorData.Data;
			return true;
		}
	}
	return false;
}
//---------------------------------------------------------------------------
void cErrorReport::WriteError(iWriteBuffer<uChar16> *WriteBuffer,iErrorReport *Report)noexcept(true)
{
	auto Function=Report->FunctionName();
	auto Message=Report->ErrorMessage();
	auto Errors=Report->Errors();

	*WriteBuffer+=ArrayStreamArray(Function);
	*WriteBuffer+=ArrayStreamString(u" reports: ");
	*WriteBuffer+=ArrayStreamArray(Message);
	if(Errors.Length!=0){
		*WriteBuffer+=ArrayStreamString(u"\n errors: ");

		for(uIntn i=0;i<Errors.Length;i++){
			auto Error=Errors.Pointer[i];

			*WriteBuffer+=ArrayStreamElement(u'[');
			Error.WriteTypeDescription(WriteBuffer);
			*WriteBuffer+=ArrayStreamElement(u']');
			Error.WriteDescription(WriteBuffer,Error.Data);
			*WriteBuffer+=ArrayStreamString(u"; ");
		}
	}
}
//---------------------------------------------------------------------------
cStringBuffer<uChar16> cErrorReport::Descripe(void)noexcept(true)
{
	cStringBuffer<uChar16> Desc;
	if(fReport!=nullptr){
		auto WriteBuffer=Desc.StreamWriteBuffer();
		auto WriteBufferInterface=WriteBufferFromStreamBuffer(&WriteBuffer);
		WriteError(&WriteBufferInterface,fReport);
		iErrorReport *CurReport=fReport->ParentReport();
		uIntn InnerReportIndex=0;
		while(CurReport!=nullptr){
			InnerReportIndex++;
			WriteBuffer+=StringStreamFormat(u"\n(%d) ",InnerReportIndex);
			WriteError(&WriteBufferInterface,CurReport);
			CurReport=CurReport->ParentReport();
		}
	}

	return Desc;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void cLogMessageQueue::Submit(rPtr<cLogMessageRecord> Record)noexcept(true)
{
	auto SubmitItem=fMsgRecycler.Pop();
	if(SubmitItem==nullptr){
		SubmitItem=new cMsgItem;
	}
	SubmitItem->Record=cnVar::MoveCast(Record);
	fMsgQueue.Enqueue(SubmitItem);

	ProcessMsgQueue();
}
//---------------------------------------------------------------------------
void cLogMessageQueue::Cleanup(void)noexcept(true)
{
	// remove item cache
	auto DeleteItems=fMsgRecycler.Swap(nullptr);
	while(DeleteItems!=nullptr){
		auto CurItem=DeleteItems;
		DeleteItems=DeleteItems->Next;

		delete CurItem;
	}
}
//---------------------------------------------------------------------------
void cLogMessageQueue::Async(iAsyncExecution *Execution)noexcept(true)
{
	iIncReference(Execution,'logr');
	auto PrevExecution=fReplaceAsyncExecution.Barrier.Xchg(Execution);
	if(PrevExecution!=nullptr){
		iDecReference(PrevExecution,'logr');
	}

	fNeedAsync=true;
	ProcessMsgQueue();
}
//---------------------------------------------------------------------------
void cLogMessageQueue::Connect(iLogRecorder *Recorder)noexcept(true)
{
	if(Recorder!=nullptr){
		if(Recorder->Acquire(this)==false)
			return;
		rIncReference(Recorder,'logr');
	}
	auto PrevRecorder=fReplaceRecorder.Barrier.Xchg(Recorder);
	if(PrevRecorder!=nullptr){
		PrevRecorder->Release(this);
		rDecReference(PrevRecorder,'logr');
	}

	fNeedReplaceRecorder=true;
	ProcessMsgQueue();
}
//---------------------------------------------------------------------------
void cLogMessageQueue::Terminate(iProcedure *CompletionProcedure)noexcept(true)
{
	if(fTerminateState!=0)
		return;

	fTerminateState=1;
	fTerminateNotifyProcedure=CompletionProcedure;
	ProcessMsgQueue();
}
//---------------------------------------------------------------------------
void cLogMessageQueue::cAsyncContext::IncreaseReference(void)noexcept(true)
{
	auto Host=cnMemory::GetObjectFromMemberPointer(this,&cLogMessageQueue::fAsyncContext);
	++Host->fAsyncRefCount.Free;
}
//---------------------------------------------------------------------------
void cLogMessageQueue::cAsyncContext::DecreaseReference(void)noexcept(true)
{
	auto Host=cnMemory::GetObjectFromMemberPointer(this,&cLogMessageQueue::fAsyncContext);
	if(--Host->fAsyncRefCount.Free==0){
		// all message processed
		if(Host->fTerminateState==2){
			Host->fTerminateState=3;
			if(Host->fTerminateNotifyProcedure!=nullptr)
				Host->fTerminateNotifyProcedure->Execute();
		}
	}
}
//---------------------------------------------------------------------------
void cLogMessageQueue::cAsyncContext::Execute(void)noexcept(true)
{
	auto Host=cnMemory::GetObjectFromMemberPointer(this,&cLogMessageQueue::fAsyncContext);
	return Host->ProcessMsgQueueThread();
}
//---------------------------------------------------------------------------
void cLogMessageQueue::ProcessMsgQueue(void)noexcept(true)
{
	if(fRecordExclusive.Acquire()==false)
		return;

	ProcessUpdateAsync();
	if(fAsyncWork!=nullptr){
		fAsyncWork->Start();
	}
	else{
		ProcessMsgQueueThread();
	}
}
//---------------------------------------------------------------------------
void cLogMessageQueue::ProcessUpdateAsync(void)noexcept(true)
{
	if(fTerminateState){
		fNeedAsync=false;
		auto Execution=fReplaceAsyncExecution.Barrier.Xchg(nullptr);
		if(Execution!=nullptr){
			iDecReference(Execution,'logr');
		}
		return;
	}
	if(fNeedAsync){
		fNeedAsync=false;
		
		if(fAsyncInitialized==false){
			fAsyncContext.Construct();
			fAsyncInitialized=true;
		}

		auto Execution=fReplaceAsyncExecution.Barrier.Xchg(nullptr);
		if(Execution!=nullptr){
			auto AsyncWork=Execution->CreateWork(&fAsyncContext,&fAsyncContext);
			iDecReference(Execution,'logr');

			if(fAsyncWork!=nullptr){
				rDecReference(fAsyncWork,'logr');
			}
			fAsyncWork=rExtract(AsyncWork,'logr');
		}
		else{
			if(fAsyncWork!=nullptr){
				rDecReference(fAsyncWork,'logr');
				fAsyncWork=nullptr;
			}
		}
	}
}
//---------------------------------------------------------------------------
void cLogMessageQueue::ProcessUpdateRecorder(void)noexcept(true)
{
	if(fTerminateState){
		auto ReplaceRecorder=fReplaceRecorder.Barrier.Xchg(nullptr);
		if(ReplaceRecorder!=nullptr){
			fRecorder->Release(this);
			rDecReference(ReplaceRecorder,'logr');
		}
		return;
	}
	if(fNeedReplaceRecorder){
		fNeedReplaceRecorder=false;

		if(fRecorder!=nullptr){
			fRecorder->Release(this);
			rDecReference(fRecorder,'logr');
		}
		fRecorder=fReplaceRecorder.Barrier.Xchg(nullptr);
	}
}
//---------------------------------------------------------------------------
void cLogMessageQueue::ProcessMsgQueueThread(void)noexcept(true)
{
	do{
		fRecordExclusive.Continue();

		ProcessUpdateAsync();

		if(fRecorder==nullptr){
			ProcessUpdateRecorder();
		}

		cMsgItem *Item;
		if(fRecorder!=nullptr){
			while((Item=fMsgQueue.Dequeue())!=nullptr){
				SubmitMsgRecord(Item->Record);
				Item->Record=nullptr;
				fMsgRecycler.Push(Item);
			}
			if(fTerminateState==1){
				// all message submitted,  remove recorder
				fRecorder->Release(this);
				rDecReference(fRecorder,'logr');

				// remove async work
				if(fAsyncWork!=nullptr){
					rDecReference(fAsyncWork,'logr');
					fAsyncWork=nullptr;
				}

				fTerminateState=2;
			}
			else{
				ProcessUpdateRecorder();
			}
		}
		else{
			if(++fRecordMissingCount>64){
				fRecordMissingCount=0;
				while((Item=fMsgQueue.Dequeue())!=nullptr){
					Item->Record=nullptr;
					fMsgRecycler.Push(Item);
				}
			}

			if(fTerminateState==1){
				// all message submitted,  remove async work
				if(fAsyncWork!=nullptr){
					rDecReference(fAsyncWork,'logr');
					fAsyncWork=nullptr;
				}

				fTerminateState=2;
			}
		}
	}while(fRecordExclusive.Release()==false);
}
//---------------------------------------------------------------------------
void cLogMessageQueue::SubmitMsgRecord(cLogMessageRecord *Record)noexcept(true)
{
	cLogMessage Message;
	Message.Time=Record->Time=cnSystem::GetSystemTimeNow();
	Message.Level=Record->Level;
	Message.Path=Record->Path.GetArray();
	Message.Text=Record->TextBuffer.GetArray();
	fRecorder->Submit(Record,Message);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
bcLogRecordSubmission::bcLogRecordSubmission(cLogMessageQueue *Queue,rPtr<cLogMessageRecord> Record)noexcept(true)
	: fQueue(Queue)
	, fRecord(cnVar::MoveCast(Record))
{
}
//---------------------------------------------------------------------------
bcLogRecordSubmission::~bcLogRecordSubmission()noexcept(true)
{
	if(fRecord->TextBuffer.GetLength()!=0){
		fQueue->Submit(cnVar::MoveCast(fRecord));
	}
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cLogStreamBuffer::cLogStreamBuffer(cLogMessageQueue *Queue,rPtr<cLogMessageRecord> Record)noexcept(true)
	: bcLogRecordSubmission(Queue,cnVar::MoveCast(Record))
	, fWriteBuffer(fRecord->TextBuffer.StreamWriteBuffer())
{
}
//---------------------------------------------------------------------------
cLogStreamBuffer::~cLogStreamBuffer()noexcept(true)
{
}
//---------------------------------------------------------------------------
void cLogStreamBuffer::Reset(void)noexcept(true)
{
	fRecord->TextBuffer.Clear();
}
//---------------------------------------------------------------------------
cArray<uChar16> cLogStreamBuffer::ReserveWriteBuffer(uIntn QueryLength)noexcept(true)
{
	return fWriteBuffer.ReserveWriteBuffer(QueryLength);
}
//---------------------------------------------------------------------------
void cLogStreamBuffer::CommitWriteBuffer(uIntn Length)noexcept(true)
{
	return fWriteBuffer.CommitWriteBuffer(Length);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cLogRecordHub::cLogRecordHub()noexcept(true)
	: fOwner(nullptr)
{
}
//---------------------------------------------------------------------------
cLogRecordHub::~cLogRecordHub()noexcept(true)
{
}
//---------------------------------------------------------------------------
bool cLogRecordHub::Acquire(void *Owner)noexcept(true)
{
	return fOwner.Barrier.CmpStore(nullptr,Owner);
}
//---------------------------------------------------------------------------
bool cLogRecordHub::Release(void *Owner)noexcept(true)
{
	return fOwner.Barrier.CmpStore(Owner,nullptr);
}
//---------------------------------------------------------------------------
void cLogRecordHub::Submit(iReference *Reference,const cLogMessage &Message)noexcept(true)
{
	return SubmitMessage(Reference,Message);
}
//---------------------------------------------------------------------------
void cLogRecordHub::InsertRecorder(iLogRecorder *Recorder)noexcept(true)
{
	if(Recorder==nullptr)
		return;
	if(fOwner.Barrier.CmpStore(nullptr,this)==false){
		// cannot change recorder
		return;
	}

	if(Recorder->Acquire(this)){
		fRecorderSet.Insert(Recorder);
	}

	fOwner.Release.Store(nullptr);
}
//---------------------------------------------------------------------------
void cLogRecordHub::RemoveRecorder(iLogRecorder *Recorder)noexcept(true)
{
	if(Recorder==nullptr)
		return;
	if(fOwner.Barrier.CmpStore(nullptr,this)==false){
		// cannot change recorder
		return;
	}

	bool Removed;
	Removed=fRecorderSet.Remove(Recorder);

	fOwner.Release.Store(nullptr);

	if(Removed){
		Recorder->Release(this);
	}
}
//---------------------------------------------------------------------------
void cLogRecordHub::ClearRecorder(void)noexcept(true)
{
	if(fOwner.Barrier.CmpStore(nullptr,this)==false){
		// cannot change recorder
		return;
	}
	auto ClearList=cnVar::MoveCast(fRecorderSet);
	fOwner.Release.Store(nullptr);

	for(auto p : ClearList){
		p->Release(this);
	}
}
//---------------------------------------------------------------------------
void cLogRecordHub::SubmitMessage(iReference *Reference,const cLogMessage &Message)noexcept(true)
{
	auto Owner=fOwner.Free.Load();
	if(Owner==nullptr || Owner==this)
		return;

	for(auto &p : fRecorderSet){
		p->Submit(Reference,Message);
	}
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cTextStreamOutputLogRecorder::cTextStreamOutputLogRecorder(iTextStreamOutput *Output)noexcept(true)
	: fOutput(Output)
	, fOwner(nullptr)
{
}
//---------------------------------------------------------------------------
bool cTextStreamOutputLogRecorder::Acquire(void *Owner)noexcept(true)
{
	return fOwner.Free.CmpStore(nullptr,Owner);
}
//---------------------------------------------------------------------------
bool cTextStreamOutputLogRecorder::Release(void *Owner)noexcept(true)
{
	return fOwner.Free.CmpStore(Owner,nullptr);
}
//---------------------------------------------------------------------------
void cTextStreamOutputLogRecorder::Submit(iReference*,const cLogMessage &Message)noexcept(true)
{
	cStringBuffer<uChar16> Text;
	{
		cDateTime Date;
		cnSystem::UTCGregorianDateFromSystemTime(Date,Message.Time);
		auto WriteBuffer=Text.StreamWriteBuffer();
		StringStream::WriteFormatString(WriteBuffer,u" %.2d:%.2d:%.2d.%.3d - (%d) ",Date.Hour,Date.Minute,Date.Second,(Date.NanoSecond/Time_1ms),Message.Level);
		ArrayStream::WriteArray(WriteBuffer,Message.Path.Pointer,Message.Path.Length);
		ArrayStream::WriteElement(WriteBuffer,':');
		ArrayStream::WriteElement(WriteBuffer,' ');
		ArrayStream::WriteArray(WriteBuffer,Message.Text.Pointer,Message.Text.Length);
		ArrayStream::WriteElement(WriteBuffer,'\n');
	}
	fOutput->Print(Text->Pointer,Text->Length);
}
//---------------------------------------------------------------------------
