#include <errno.h>
#include <unistd.h>
#include <sys/stat.h>
#include "POSIX_AIO.h"

using namespace cnLibrary;
using namespace siPOSIX;

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void cAIOTaskBlock::SetAccess(int fd,void *Buffer,uIntn Size,uInt64 Offset)
{
	aio_buf=Buffer;
	aio_fildes=fd;
	aio_nbytes=Size;
	aio_offset=Offset;
	aio_reqprio=0;
	aio_lio_opcode=LIO_NOP;
}
//---------------------------------------------------------------------------
void cAIOTaskBlock::SetEventSignal(int signo)
{
	aio_sigevent.sigev_notify=SIGEV_SIGNAL;
	aio_sigevent.sigev_signo=signo;
	aio_sigevent.sigev_value.sival_ptr=0;
}
//---------------------------------------------------------------------------
bool cAIOTaskBlock::CheckCompletion(void)
{
	if(TaskState.IsDone())
		return true;

	if(Discard){
		// no aio
	}
	else{
		int err=aio_error(this);
		if(err==EINPROGRESS){
			return false;
		}
		if(err==0){
			// read ok
			Result=true;
			SizeCompleted=aio_return(this);
		}
		else{
			Result=false;
			SizeCompleted=0;
		}
	}
	TaskState.SetDone();
	if(OnCompletion!=nullptr)
		OnCompletion();
	return true;
}
//---------------------------------------------------------------------------
void cAIOTaskBlock::Cancel(void)
{
	if(TaskState.IsDone())
		return;

	if(Discard==false){
		aio_cancel(aio_fildes,this);
		Result=false;
		SizeCompleted=0;
	}

	TaskState.SetDone();
	if(OnCompletion!=nullptr)
		OnCompletion();
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cAIOTaskBlockManager::cAIOTaskBlockManager()
{
}
//---------------------------------------------------------------------------
cAIOTaskBlockManager::~cAIOTaskBlockManager()
{
	auto ClearList=fBlockStack.Swap(nullptr);
	while(ClearList!=nullptr){
		ClearList->Cancel();
		ClearList=ClearList->Next;
	}
}
//---------------------------------------------------------------------------
void cAIOTaskBlockManager::Register(cAIOTaskBlock *Block)
{
	fBlockStack.Push(Block);
}
//---------------------------------------------------------------------------
void cAIOTaskBlockManager::AIOSignalEvent(void)
{
	if(fSignalEventFlag.Acquire()==false)
		return;

	do{
		fSignalEventFlag.Continue();

		CheckBlocks();
		
	}while(fSignalEventFlag.Release()==false);
}
//---------------------------------------------------------------------------
void cAIOTaskBlockManager::CheckBlocks(void)
{
	auto CheckList=fBlockStack.Swap(nullptr);
	cAIOTaskBlock Head;
	Head.Next=CheckList;
	cAIOTaskBlock *Tail=&Head;
	while(CheckList!=nullptr){
		if(CheckList->CheckCompletion()){
			// item done
			Tail->Next=CheckList->Next;
		}
		else{
			Tail=CheckList;
		}

		CheckList=CheckList->Next;
	}
	if(Head.Next!=nullptr){
		fBlockStack.PushChain(Head.Next,Tail);
	}
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cAIOStreamTask::cAIOStreamTask()
{
}
//---------------------------------------------------------------------------
cAIOStreamTask::~cAIOStreamTask()
{
}
//---------------------------------------------------------------------------
void cAIOStreamTask::AIORegister(cAIOTaskBlockManager *Manager)
{
	iIncReference(this,'aios');
	fAIOBlock.Discard=false;
	fAIOBlock.Result=false;
	fAIOBlock.SizeCompleted=0;
	fAIOBlock.SetEventSignal();
	fAIOBlock.OnCompletion=[this]{
		AIOCompleted();
	};
	Manager->Register(&fAIOBlock);
}
//---------------------------------------------------------------------------
void cAIOStreamTask::AIODiscard(cAIOTaskBlockManager *Manager)
{
	fAIOBlock.Discard=true;
	Manager->AIOSignalEvent();
}
//---------------------------------------------------------------------------
void cAIOStreamTask::AIOCompleted(void)
{
	iDecReference(this,'aios');
}
//---------------------------------------------------------------------------
bool cAIOStreamTask::AIORead(int fd,void *Buffer,uIntn Size,uInt64 Offset)
{
	fAIOBlock.SetAccess(fd,Buffer,Size,Offset);
	int ret=aio_read(&fAIOBlock);
	return ret==0;
}
//---------------------------------------------------------------------------
bool cAIOStreamTask::AIOWrite(int fd,const void *Buffer,uIntn Size,uInt64 Offset)
{
	fAIOBlock.SetAccess(fd,const_cast<void*>(Buffer),Size,Offset);
	int ret=aio_write(&fAIOBlock);
	return ret==0;
}
//---------------------------------------------------------------------------
bool cAIOStreamTask::IsDone(void)
{
	return fAIOBlock.TaskState.IsDone();
}
//---------------------------------------------------------------------------
bool cAIOStreamTask::SetNotify(iProcedure *NotifyProcedure)
{
	return fAIOBlock.TaskState.SetNotify(NotifyProcedure);
}
//---------------------------------------------------------------------------
void cAIOStreamTask::Cancel(void)
{
	aio_cancel(fAIOBlock.aio_fildes,&fAIOBlock);
}
//---------------------------------------------------------------------------
bool cAIOStreamTask::GetResult(uIntn &SizeCompleted)
{
	SizeCompleted=fAIOBlock.SizeCompleted;
	return fAIOBlock.Result;
}
//---------------------------------------------------------------------------

