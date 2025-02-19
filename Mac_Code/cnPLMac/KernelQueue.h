//
//  KernelQueue.h
//  cnPLMac
//
//  Created by Code009 on 12-11-21.
//  Copyright (c) 2012 Code009. All rights reserved.
//

#ifndef __cnLib_cnPLMac_KernelQueue__
#define __cnLib_cnPLMac_KernelQueue__
#if	0
#include <aio.h>
#include <sys/event.h>
#include <pthread.h>

#include <cnRTL/Interface.h>
#include <cnRTL/MemoryBlock.h>
#include <cnRTL/SequenceData.h>
/*-------------------------------------------------------------------------*/
#ifdef	__cplusplus
//---------------------------------------------------------------------------
namespace cnLib{
namespace cnPLMac{
//---------------------------------------------------------------------------
typedef void AIOCompleteNotifyProc(aiocb *cb);
//---------------------------------------------------------------------------
class cIOKernelQueue
{
	int fKernelQueue;
	
	struct cAIOBlock
	{
		aiocb *cb;
		AIOCompleteNotifyProc *CompleteNotify;
	};
	
	cnRTL::cSeqList<cAIOBlock> fAIOBlocks;
	
	void PollThread(void);
	static void* PollThreadMainRoutine(void* data);
	pthread_t fPollThread;

	void CheckEvent(struct kevent &event);
	void CheckAIO(void);
public:
	static constexpr int AIO_SignalNo=SIGIO;

	cIOKernelQueue();
	~cIOKernelQueue();

	void AIONotify(aiocb *cb,AIOCompleteNotifyProc *NotifyProc);

	struct FileIOContext{
		void (*NotifyProc)(FileIOContext *Context,int fd,uintn BytesAvailable,int Error,bool EndOfFile,bool IsRead);
		
	};

	void RegisterFileIO(int fd,FileIOContext *Context);
	void UnregisterFileIO(int fd);
};
#endif
//---------------------------------------------------------------------------
}	// namespace cnPLMac
}	// namespace cnLib
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
#endif	// __cplusplus
/*-------------------------------------------------------------------------*/


#endif
