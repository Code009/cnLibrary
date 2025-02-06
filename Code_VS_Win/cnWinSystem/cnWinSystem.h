/*- Unit: Windows System Interface ----------------------------------------*/
/*         Developer : Code009                                             */
/*         Create on : 2009-02-10                                          */
/*-------------------------------------------------------------------------*/
#pragma once
/*-------------------------------------------------------------------------*/
#include "cnWinSystemHeader.h"

#ifdef __cplusplus

#define	cnWinSystem_INITSEG_SET								\
__pragma(warning(disable:4075))								\
using cnLibrary::cnWin::Initialization_AtExit;				\
__pragma(init_seg("cnWin_INIT$1",Initialization_AtExit))	\
__pragma(warning(default:4075))	

	
//---------------------------------------------------------------------------
namespace cnLibrary{
//---------------------------------------------------------------------------
namespace cnWin{
//---------------------------------------------------------------------------
int Initialization_AtExit(void (__cdecl *Proc)(void));
//---------------------------------------------------------------------------
extern cnRTL::cnWinRTL::cCOMApartmentThreadingModule gCOMApartmentThreadingModule;
//---------------------------------------------------------------------------
struct cSystemInterfaceSet
{
	// cnSystem

	iMemoryHeap*			DefaultMemoryHeap;
	iThreadPool*			DefaultThreadPool;
	sInt64					EpochSecondsSinceUnix;
	iConnectionProtocol*	TCP;
	iDatagramProtocol*		UDP;
	iConnectionProtocol*	InterprocessProtocol;

};
extern const cSystemInterfaceSet gSystemInterfaceSet;
//---------------------------------------------------------------------------

#ifdef cnWin_SYNCHRONIZATION_NAMESPACE

typedef cnWin_SYNCHRONIZATION_NAMESPACE::cSRWLock vcSRWLock;

#endif // cnWin_SYNCHRONIZATION_NAMESPACE



#ifdef cnWin_THREADING_NAMESPACE

using vcThreadPool = cnWin_THREADING_NAMESPACE::cThreadPool;
using vcDefaultThreadPool = cnWin_THREADING_NAMESPACE::cDefaultThreadPool;

#endif	// cnWin_THREADING_NAMESPACE

#ifdef cnWin_FILESYSTEM_NAMESPACE

namespace vnWin32FileSystem=cnWin_FILESYSTEM_NAMESPACE;

#endif	// cnWin_FILESYSTEM_NAMESPACE

// Audio

#ifdef cnWin_AUDIODEVICE_NAMESPACE

namespace vnAudioDevice=cnWin_AUDIODEVICE_NAMESPACE;

#endif	// cnWin_AUDIODEVICE_NAMESPACE

// Socket

#ifdef cnWin_WINSOCKIO_NAMESPACE

typedef cnWin_WINSOCKIO_NAMESPACE::cSocketStreamProtocol vSocketStreamProtocol;
typedef cnWin_WINSOCKIO_NAMESPACE::cSocketDatagramProtocol vSocketDatagramProtocol;

#endif // cnWin_WINSOCKIO_NAMESPACE

// Window

#ifdef cnWin_WINDOWSUBCLASS_NAMESPACE

namespace vnWindowSubclass=cnWin_WINDOWSUBCLASS_NAMESPACE;

#endif // cnWin_WINDOWSUBCLASS_NAMESPACE

//---------------------------------------------------------------------------
class cWinLogModule
{
public:
	cWinLogModule()noexcept{
		cnRTL::gRTLLog.Async(&cnWin_THREADING_NAMESPACE::cDefaultThreadPool::gInstance);
	}
	~cWinLogModule()noexcept{
		cTerminationCompletionProc TerminationProc;
		TerminationProc.Notification.Setup();
		cnRTL::gRTLLog.Terminate(&TerminationProc);
		TerminationProc.Notification.Wait();

		cnRTL::gRTLLog.Cleanup();
	}

	void LogConnectRecorder(iLogRecorder *Recorder)noexcept{
		return cnRTL::gRTLLog.Connect(Recorder);
	}

private:
	class cTerminationCompletionProc : public iProcedure
	{
	public:
		cnRTL::cnWinRTL::cWinSingleThreadNotification Notification;
		virtual void cnLib_FUNC Execute(void)noexcept(true)override{
			Notification.Notify();
		}
	};
};
//---------------------------------------------------------------------------
}	// namespace cnWin
//---------------------------------------------------------------------------
}	// namespace cnLibrary
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
#endif  /* __cplusplus */
/*-------------------------------------------------------------------------*/
