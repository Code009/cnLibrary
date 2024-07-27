/*- cnWin - Thread --------------------------------------------------------*/
/*         Developer : Code009                                             */
/*         Create on : 2008-06-05                                          */
/*-------------------------------------------------------------------------*/
#pragma once
/*-------------------------------------------------------------------------*/
#include <cnWin\Win_Common.h>

#ifdef __cplusplus
//---------------------------------------------------------------------------
namespace cnLibrary{
//---------------------------------------------------------------------------
namespace cnWin{
//---------------------------------------------------------------------------
#if _WIN32_WINNT >= _WIN32_WINNT_WIN2K
//---------------------------------------------------------------------------
namespace cnWinNT5{
//---------------------------------------------------------------------------
namespace Synchronization{
//---------------------------------------------------------------------------
typedef cnRTL::cnWinRTL::cSRWLockByCriticalSection cSRWLock;
//---------------------------------------------------------------------------
}	//	namespace Synchronization
//---------------------------------------------------------------------------
}	//	namespace cnWinNT5
//---------------------------------------------------------------------------
#if cnWin_SYNCHRONIZATION_VERSION < _WIN32_WINNT_WIN2K
#undef	cnWin_SYNCHRONIZATION_VERSION
#undef	cnWin_SYNCHRONIZATION_NAMESPACE
#define	cnWin_SYNCHRONIZATION_VERSION		_WIN32_WINNT_WIN2K
#define	cnWin_SYNCHRONIZATION_NAMESPACE		cnWinNT5::Synchronization
#endif // cnWin_SYNCHRONIZATION_VERSION < _WIN32_WINNT_WIN2K
//---------------------------------------------------------------------------
#endif	// >= _WIN32_WINNT_WIN2K
//---------------------------------------------------------------------------
#if _WIN32_WINNT >= _WIN32_WINNT_WIN7
//---------------------------------------------------------------------------
namespace cnWinNT6_1{
//---------------------------------------------------------------------------
namespace Synchronization{
//---------------------------------------------------------------------------
typedef cnRTL::cnWinRTL::cSRWLock cSRWLock;
//---------------------------------------------------------------------------
}	//	namespace Synchronization
//---------------------------------------------------------------------------
}	//	namespace cnWinNT6_1
//---------------------------------------------------------------------------
#if cnWin_SYNCHRONIZATION_VERSION < _WIN32_WINNT_WIN7
#undef	cnWin_SYNCHRONIZATION_VERSION
#undef	cnWin_SYNCHRONIZATION_NAMESPACE
#define	cnWin_SYNCHRONIZATION_VERSION		_WIN32_WINNT_WIN7
#define	cnWin_SYNCHRONIZATION_NAMESPACE		cnWinNT6_1::Synchronization
#endif // cnWin_SYNCHRONIZATION_VERSION < _WIN32_WINNT_WIN7
//---------------------------------------------------------------------------
#endif	// >= _WIN32_WINNT_WIN7
//---------------------------------------------------------------------------
class cThread : public cnRTL::cnWinRTL::cThreadHandle
{
public:
	cThread()noexcept(true);
	~cThread()noexcept(true);

	static cThread* QueryCurrent(void)noexcept(true);
	static iPtr<cThread> StartThread(iProcedure *ThreadProcedure)noexcept(true);

	bool IsRunning(void)noexcept(true);

	static iThreadLocalVariable *const gTLS;
protected:

	bool fThreadRunning;
	struct cThreadCreateParameter : cnRTL::cRTLAllocator
	{
		iPtr<cThread> Thread;
		iProcedure *Procedure;
	};
	static DWORD WINAPI ThreadEntry(LPVOID Parameter)noexcept(true);

	virtual void cnLib_FUNC DependentShutdownNotification(void)noexcept(true)override;

private:
	class cThreadExitNotifyProc : public iThreadExitNotifyProc
	{
		virtual void cnLib_FUNC Execute(rPtr<iReference>,void*)noexcept(true)override;
	};
	static void TLSSetExitNotify(void)noexcept(true);
};
//---------------------------------------------------------------------------
class cErrorReportRecord : public iErrorReport
{
public:
	cErrorReportRecord()noexcept(true);
	~cErrorReportRecord()noexcept(true);


	rPtr<cErrorReportRecord> Parent;
	cnRTL::cStringBuffer<uChar16> FuncName;
	cnRTL::cStringBuffer<uChar16> ErrorMsg;
	cnRTL::cSeqList<cErrorInfo> ErrorList;
	cnRTL::cMemoryBuffer ErrorData;

	static iThreadLocalVariable *const gTLSRecord;


	virtual iErrorReport* cnLib_FUNC ParentReport(void)noexcept(true)override;
	virtual cArray<const uChar16> cnLib_FUNC FunctionName(void)noexcept(true)override;
	virtual cArray<const uChar16> cnLib_FUNC ErrorMessage(void)noexcept(true)override;
	virtual cArray<const cErrorInfo> cnLib_FUNC Errors(void)noexcept(true)override;
};
//---------------------------------------------------------------------------
#if _WIN32_WINNT >= _WIN32_WINNT_VISTA
//---------------------------------------------------------------------------
namespace cnWinNT6{
//---------------------------------------------------------------------------
class cDefaultThreadPool : public cnRTL::cnWinRTL::bcNT6ThreadPoolEnvironment, public iWinThreadPool
{
public:
	cDefaultThreadPool()noexcept(true);
	~cDefaultThreadPool()noexcept(true);

	using iWinThreadPool::CastInterface;

	static cDefaultThreadPool gInstance;

	virtual rPtr<iThreadPoolHandleWaiter> CreateHandleWaiter(iReference *Reference,iFunction<void (DWORD)noexcept(true)> *Callback)noexcept(true) override;
};
typedef cnRTL::cnWinRTL::cNT6ThreadPoolEnvironment cThreadPool;
//---------------------------------------------------------------------------
}	// namespace cnWinNT6
//---------------------------------------------------------------------------
#if cnWin_THREADING_VERSION < _WIN32_WINNT_VISTA
#undef	cnWin_THREADING_VERSION
#undef	cnWin_THREADING_NAMESPACE
#define	cnWin_THREADING_VERSION			_WIN32_WINNT_VISTA
#define	cnWin_THREADING_NAMESPACE		cnWinNT6
#endif // cnWin_THREADING_VERSION < _WIN32_WINNT_VISTA
//---------------------------------------------------------------------------
#endif	// _WIN32_WINNT >= _WIN32_WINNT_VISTA
//---------------------------------------------------------------------------
class cWinLogModule
{
public:
	cWinLogModule()noexcept;
	~cWinLogModule()noexcept;
};
//---------------------------------------------------------------------------
}	// namespace cnWin
//---------------------------------------------------------------------------
}	// namespace cnLibrary
//---------------------------------------------------------------------------
#endif  /* __cplusplus */
/*-------------------------------------------------------------------------*/
