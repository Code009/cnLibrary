//
//  KernelQueue.cpp
//  cnPLMac
//
//  Created by Code009 on 12-11-21.
//  Copyright (c) 2012 Code009. All rights reserved.
//

#include <sys/stat.h>
#include <sys/errno.h>

#include "KernelQueue.h"

#include "Thread.h"

using namespace cnLib;
using namespace cnRTL;
using namespace cnPLMac;

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cIOKernelQueue::cIOKernelQueue()
{
// register io queue
	fKernelQueue=kqueue();

	static const struct kevent AIOSignalEvent={
		AIO_SignalNo,//uintptr_t	ident;
		EVFILT_SIGNAL,//int16_t		filter;
		EV_ADD,//uint16_t	flags;
		0,//uint32_t	fflags;
		0,//intptr_t	data;
		nullptr,//void		*udata;
	};
	kevent(fKernelQueue,&AIOSignalEvent,1,nullptr,0,nullptr);
	
	pthread_create(&fPollThread, nullptr, &PollThreadMainRoutine, this);
}
//---------------------------------------------------------------------------
cIOKernelQueue::~cIOKernelQueue()
{
	void *param;

	pthread_join(fPollThread,&param);
	close(fKernelQueue);
}
//---------------------------------------------------------------------------
void cIOKernelQueue::PollThread(void)
{
	struct kevent events[2];
	for(;;){
		int count=kevent(fKernelQueue,nullptr,0,events,2,nullptr);
		for(unsigned int i=0;i<count;i++){
			CheckEvent(events[i]);
		}
	}
}
//---------------------------------------------------------------------------
void* cIOKernelQueue::PollThreadMainRoutine(void* data)
{
	static_cast<cIOKernelQueue*>(data)->PollThread();
	return nullptr;
}
//---------------------------------------------------------------------------
void cIOKernelQueue::CheckEvent(struct kevent &event)
{
	switch(event.filter){
	case EVFILT_READ:
	case EVFILT_WRITE:
		{
			auto FileContext=static_cast<FileIOContext*>(event.udata);
			event.udata=nullptr;
			bool eof=(event.flags&EV_EOF)!=0;
			(*FileContext->NotifyProc)(FileContext,static_cast<int>(event.ident),
				event.data,event.fflags,eof,event.filter==EVFILT_READ);
		}
		break;
	case EVFILT_SIGNAL:
		switch(event.ident){
		case AIO_SignalNo:
			CheckAIO();
			break;
		}
		break;
	}
}
//---------------------------------------------------------------------------
void cIOKernelQueue::CheckAIO(void)
{
	for(uintn i=0,c=fAIOBlocks.GetCount();i<c;i++){
		auto &CurItem=fAIOBlocks[i];
		int status=aio_error( CurItem.cb );
    	if (status == EINPROGRESS) {
			continue;
		}
		CurItem.CompleteNotify(CurItem.cb);

		fAIOBlocks.Remove(i);
		i--;
		c--;
	}
}
//---------------------------------------------------------------------------
void cIOKernelQueue::AIONotify(aiocb *cb,AIOCompleteNotifyProc *NotifyProc)
{
	cAIOBlock block={cb,NotifyProc};
	fAIOBlocks.Append(block);
}
//---------------------------------------------------------------------------
void cIOKernelQueue::RegisterFileIO(int fd,FileIOContext *Context)
{
	struct kevent ke[2];
	ke[0].ident=fd;
	ke[0].flags=EV_ADD|EV_CLEAR;
	ke[0].data=0;
	ke[0].fflags=0;
	ke[0].filter=EVFILT_READ;
	ke[0].udata=Context;
	ke[1].ident=fd;
	ke[1].flags=EV_ADD|EV_CLEAR;
	ke[1].data=0;
	ke[1].fflags=0;
	ke[1].filter=EVFILT_WRITE;
	ke[1].udata=Context;
	kevent(fKernelQueue,ke,2,nullptr,0,nullptr);
	
}
//---------------------------------------------------------------------------
void cIOKernelQueue::UnregisterFileIO(int fd)
{
	struct kevent ke[2];
	ke[0].ident=fd;
	ke[0].flags=EV_DELETE;
	ke[0].data=0;
	ke[0].fflags=0;
	ke[0].filter=EVFILT_READ;
	ke[0].udata=0;
	ke[1].ident=fd;
	ke[1].flags=EV_DELETE;
	ke[1].data=0;
	ke[1].fflags=0;
	ke[1].filter=EVFILT_WRITE;
	ke[1].udata=0;
	kevent(fKernelQueue,ke,2,nullptr,0,nullptr);
}
//---------------------------------------------------------------------------
