/*--- Mac - UI Thread -----------------------------------------------------*/
/*         Developer : Code009                                             */
/*         Create on : 2012-05-25                                          */
/*-------------------------------------------------------------------------*/

#pragma once

//---------------------------------------------------------------------------
#include <cnMac/Mac_Header.h>

/*-------------------------------------------------------------------------*/
#ifdef	__cplusplus
//---------------------------------------------------------------------------
namespace cnLibrary{
namespace cnMac{
//---------------------------------------------------------------------------
static constexpr Float64 CFTime_RefTime_Unix=-978307200;	// = UnixTimeEpoch - CFTimeEpoch
//---------------------------------------------------------------------------
sInt64 CFTimeTocnSystemTime(CFAbsoluteTime Time);
CFAbsoluteTime cnSystemTimeToCFTime(sInt64 st);
CFAbsoluteTime TimeToCFTime(const iTimepoint *RefTime);
//---------------------------------------------------------------------------
class cRunLoopAsyncProc : public iAsyncProcedure
{
public:
	cRunLoopAsyncProc(CFRunLoopRef RunLoop,CFRunLoopMode RunLoopMode,iReference *Reference,iProcedure *Procedure);
	~cRunLoopAsyncProc();

	virtual void cnLib_FUNC Start(void)override;
protected:
	cfPtr<CFRunLoopRef> fRunLoop;
	cfPtr<CFRunLoopMode> fRunLoopMode;

	cfPtr<CFRunLoopSourceRef> fSource;
	iReference *fReference;
	iProcedure *fProcedure;

	cnRTL::cAtomicVar<uInt16> fRunCount;

	void CallProc(void);

	static void RunLoopSourcePerformRoutine(void *info);

};
//---------------------------------------------------------------------------
class cRunLoopAsyncTimer : public iAsyncTimer
{
public:
	cRunLoopAsyncTimer(CFRunLoopRef RunLoop,CFRunLoopMode RunLoopMode,iReference *Reference,iProcedure *Procedure);
	~cRunLoopAsyncTimer();

	virtual void cnLib_FUNC Start(const iTimepoint *DueTime,sInt64 DueTimeDelay,uInt64 Period)override;
	virtual void cnLib_FUNC Stop(void)override;
private:
	cfPtr<CFRunLoopRef> fRunLoop;
	cfPtr<CFRunLoopMode> fRunLoopMode;

	cfPtr<CFRunLoopTimerRef> fTimer;
	iProcedure *fProcedure;
	iReference *fReference;
	cnRTL::cAtomicVar<bool> fLocking;

	static void TimerCallback(CFRunLoopTimerRef timer,void *info);
	void TimerProcedure(void);
	void StartTimer(const iTimepoint *DueTime,sInt64 DueTimeDelay,uInt64 Period);
	void ClearTimer(void);
};
//---------------------------------------------------------------------------
class cRunLoopDispatch : public iDispatch
{
public:
	cRunLoopDispatch(CFRunLoopRef RunLoop,CFRunLoopMode RunLoopMode);
	~cRunLoopDispatch();

	// iDispatch method

	virtual bool cnLib_FUNC IsCurrentThread(void)override;

	virtual rPtr<iAsyncProcedure>	cnLib_FUNC CreateWork(iReference *Reference,iProcedure *Procedure)override;
	virtual rPtr<iAsyncTimer>		cnLib_FUNC CreateTimer(iReference *Reference,iProcedure *Procedure)override;

	virtual void cnLib_FUNC Execute(iReference *Reference,iProcedure *Procedure)override;
	virtual void cnLib_FUNC ExecuteSync(iProcedure *Procedure)override;
	virtual iPtr<iAsyncTask> cnLib_FUNC ExecuteAsync(iReference *Reference,iProcedure *Procedure)override;


protected:
	cfPtr<CFRunLoopRef> fRunLoop;
	cfPtr<CFRunLoopMode> fRunLoopMode;

	class cAsyncExecuteTask : public iAsyncTask
	{
	public:
		cAsyncExecuteTask();
		~cAsyncExecuteTask();

		void Setup(cRunLoopDispatch *Host,iReference *Reference,iProcedure *Procedure);

		virtual bool cnLib_FUNC IsDone(void)override;
		virtual bool cnLib_FUNC SetNotify(iProcedure *NotifyProcedure)override;

	protected:
		iPtr<cRunLoopDispatch> fHost;
		cfPtr<CFRunLoopSourceRef> fSource;
		rPtr<iReference> fReference;
		iProcedure *fProcedure;

		cnRTL::cAsyncTaskState fTaskState;

		void CallProc(void);

		static void RunLoopSourcePerformRoutine(void *info);

	};
	cnRTL::iSharedObjectRecycler<cAsyncExecuteTask> fAsyncExecuteTaskRecycler;
};
//---------------------------------------------------------------------------
class cUIThread : public iUIThread
{
public:
	cUIThread();
	~cUIThread();

	virtual bool cnLib_FUNC IsCurrentThread(void)override;
	virtual iDispatch* cnLib_FUNC GetDispatch(bool HighPriority)override;

	virtual iUIKeyboard* cnLib_FUNC GetDefaultKeyboard(void)override;
	virtual iUIMouse* cnLib_FUNC GetDefaultMouse(void)override;


protected:
	mach_port_t fMacThread;
	CFRunLoopRef fRunLoop;

	iPtr<cRunLoopDispatch> fDispatchCommon;
	iPtr<cRunLoopDispatch> fDispatchDefault;
};
//---------------------------------------------------------------------------
}	// namespace cnMac
}	// namespace cnLibrary
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
#endif	// __cplusplus
/*-------------------------------------------------------------------------*/

