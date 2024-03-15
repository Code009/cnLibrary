/*--- Mac - GCD -----------------------------------------------------------*/
/*         Developer : Code009                                             */
/*         Create on : 2012-05-28                                          */
/*-------------------------------------------------------------------------*/

#pragma once

#include <dispatch/dispatch.h>

//---------------------------------------------------------------------------
#include <cnMac/Mac_Header.h>
/*-------------------------------------------------------------------------*/
#ifdef	__cplusplus
//---------------------------------------------------------------------------
namespace cnLibrary{
namespace cnMac{
//---------------------------------------------------------------------------
void iTimeToDispatchTimeNS(dispatch_time_t &dt,iTimepoint *Time,sInt64 Delay);
//---------------------------------------------------------------------------
#if !OS_OBJECT_USE_OBJC
void GCDSync(dispatch_queue_t Queue,iProcedure *Procedure);
void GCDAsync(dispatch_queue_t Queue,iProcedure *Procedure);
void GCDAsync(dispatch_queue_t Queue,rPtr<iReference> Reference,iProcedure *Procedure);
#endif
//---------------------------------------------------------------------------
class cGCDAsyncProc : public iAsyncProcedure
{
public:
	cGCDAsyncProc()=delete;
#if !OS_OBJECT_USE_OBJC
	cGCDAsyncProc(dispatch_queue_t Queue,iReference *ThreadReference,iProcedure *ThreadProcedure);
#endif
	~cGCDAsyncProc();

	virtual void cnLib_FUNC Start(void)override;
protected:
	dispatch_queue_t fQueue;
	iReference *fReference;
	iProcedure *fProcedure;

	static void AsyncProcedure(void *Parameter);
};
//---------------------------------------------------------------------------
class cGCDTimer : public iAsyncTimer
{
public:
	cGCDTimer()=delete;
#if !OS_OBJECT_USE_OBJC
	cGCDTimer(dispatch_queue_t Queue,iReference *ThreadReference,iProcedure *ThreadProcedure);
#endif
	~cGCDTimer();

	virtual void cnLib_FUNC Start(iTimepoint *DueTime,sInt64 DueTimeDelay,uInt64 Period)override;
	virtual void cnLib_FUNC Stop(void)override;
protected:
	dispatch_queue_t fQueue;
	iReference *fReference;
	iProcedure *fProcedure;

	dispatch_source_t fTimerSource;
	static void TimerSourceProcedure(void *Parameter);
	static void TimerSourceDeletor(void *Parameter);

	//rPtr<cGCDSource> fSourceContext;
	cnRTL::cAtomicVar<bool> fLocking;
	void ClearSource(void);
};
//---------------------------------------------------------------------------
class cGCDThreadPool : public iThreadPool, public iGCDDispatch
{
public:
#if !OS_OBJECT_USE_OBJC
	cGCDThreadPool(dispatch_queue_t Queue);
#endif
	~cGCDThreadPool();

	virtual void* cnLib_FUNC CastInterface(iTypeID IID)override;


	virtual void cnLib_FUNC Execute(iReference *Reference,iProcedure *Procedure)override;
	virtual rPtr<iAsyncProcedure>	cnLib_FUNC CreateWork(iReference *Reference,iProcedure *ThreadProcedure)override;
	virtual rPtr<iAsyncTimer>		cnLib_FUNC CreateTimer(iReference *Reference,iProcedure *ThreadProcedure)override;

	virtual dispatch_queue_t GetGCDDispatch(void)override;
protected:
	dispatch_queue_t fQueue;

	cGCDThreadPool();
};
//---------------------------------------------------------------------------
class cGCDDispatch : public iDispatch, public iGCDDispatch
{
protected:
	dispatch_queue_t fQueue;
	cGCDDispatch();
	~cGCDDispatch();
public:
#if !OS_OBJECT_USE_OBJC
	cGCDDispatch(dispatch_queue_t Queue);
#endif
	virtual void* cnLib_FUNC CastInterface(iTypeID ID)override;

	virtual bool cnLib_FUNC IsCurrentThread(void)override;

	virtual rPtr<iAsyncProcedure>		cnLib_FUNC CreateWork(iReference *Reference,iProcedure *Procedure)override;
	virtual rPtr<iAsyncTimer>			cnLib_FUNC CreateTimer(iReference *Reference,iProcedure *Procedure)override;

	virtual void cnLib_FUNC Execute(iReference *Reference,iProcedure *Procedure)override;
	virtual void cnLib_FUNC ExecuteSync(iProcedure *Procedure)override;
	virtual iPtr<iAsyncTask> cnLib_FUNC ExecuteAsync(iReference *Reference,iProcedure *Procedure)override;

	virtual dispatch_queue_t GetGCDDispatch(void)override;
};
//---------------------------------------------------------------------------
inline dispatch_queue_t GCDDefaultQueue(void)
{
	return dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT,0);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
}	// namespace cnMac
}	// namespace cnLibrary
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
#endif	// __cplusplus
/*-------------------------------------------------------------------------*/

