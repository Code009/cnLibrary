/*--- POSIX - AIO ---------------------------------------------------------*/
/*         Developer : Code009                                             */
/*         Create on : 2016-06-07                                          */
/*-------------------------------------------------------------------------*/
#ifndef __cnLib_siOS_POSIX_POSIX_AIO_H__
#define __cnLib_siOS_POSIX_POSIX_AIO_H__


#include <sys/aio.h>
#include <cnSystem/cnFileSystem.h>
#include <cnRTL/cnRTL.h>
/*-------------------------------------------------------------------------*/
#ifdef	__cplusplus
//---------------------------------------------------------------------------
namespace cnLibrary{
namespace siPOSIX{
//---------------------------------------------------------------------------
struct cAIOTaskBlock : aiocb
{
	cAIOTaskBlock *Next;
	cnRTL::cAsyncTaskState TaskState;
	bool Discard;
	bool Result;
	uIntn SizeCompleted;

	cnRTL::cFunction<void (void)> OnCompletion;

	void SetAccess(int fd,void *Buffer,uIntn Size,uInt64 Offset);
	void SetEventSignal(int signo=SIGIO);
	bool CheckCompletion(void);
	void Cancel(void);
};
//---------------------------------------------------------------------------
class cAIOTaskBlockManager
{
public:
	cAIOTaskBlockManager();
	~cAIOTaskBlockManager();

	void Register(cAIOTaskBlock *Block);
	void AIOSignalEvent(void);

private:
	cnRTL::cAtomicStack<cAIOTaskBlock> fBlockStack;
	cnRTL::cExclusiveFlag fSignalEventFlag;

	void CheckBlocks(void);
};
//---------------------------------------------------------------------------
class cAIOStreamTask : public iStreamTask
{
public:
	cAIOStreamTask();
	~cAIOStreamTask();

	void AIORegister(cAIOTaskBlockManager *Manager);
	void AIODiscard(cAIOTaskBlockManager *Manager);
	bool AIORead(int fFD,void *Buffer,uIntn ASSize,uInt64 Offset);
	bool AIOWrite(int fFD,const void *Buffer,uIntn ASSize,uInt64 Offset);

	virtual bool cnLib_FUNC IsDone(void)override;
	virtual bool cnLib_FUNC SetNotify(iProcedure *NotifyProcedure)override;
	virtual void cnLib_FUNC Cancel(void)override;
	virtual bool cnLib_FUNC GetResult(uIntn &SizeCompleted)override;
protected:
	cAIOTaskBlock fAIOBlock;
	void AIOCompleted(void);
};
//---------------------------------------------------------------------------
}	// namespace siPOSIX
}	// namespace cnLibrary
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
#endif	// __cplusplus
/*-------------------------------------------------------------------------*/
#endif
