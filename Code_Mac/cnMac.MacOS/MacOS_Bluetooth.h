/*--- Mac - Socket --------------------------------------------------------*/
/*         Developer : Code009                                             */
/*         Create on : 2012-11-20                                          */
/*-------------------------------------------------------------------------*/

#pragma once

#ifdef	__OBJC__
#import <IOBluetooth/IOBluetooth.h>
#endif

#include <cnRTL/cnRTL.h>

/*-------------------------------------------------------------------------*/

#ifdef	__OBJC__
@class cnLib_BluetoothRFCOMMChannelControl;
#else
struct cnLib_BluetoothRFCOMMChannelControl;
@end
#endif	// __OBJC__

/*-------------------------------------------------------------------------*/
#ifdef	__cplusplus
//---------------------------------------------------------------------------
namespace cnLibrary{
namespace cnMac{

using cnRTL::cAsyncNotifier;
using cnRTL::cInnerReference;
//---------------------------------------------------------------------------
class cExtConnectionTask : public iConnectionTask, public cInnerReference
{
public:
	class cnLib_INTERFACE iCallback
	{
	public:
		virtual void ConnectionOnCancel(cExtConnectionTask *Task)=0;
	};

	cExtConnectionTask(iCallback *Callback);

	bool ConnectToDevice(IOBluetoothDevice *Device,UInt8 ChannelID);

	void ConnectStarted(void);
	void ConnectCompleted(iConnection *Connection);

	// iConnectionTask

	virtual bool cnLib_FUNC IsDone(void)override;
	virtual bool cnLib_FUNC SetNotify(iProcedure *NotifyProcedure)override;
	virtual void cnLib_FUNC Cancel(void)override;
	virtual iConnection* cnLib_FUNC GetConnection(void)override;

	//virtual ConnectError cnLib_FUNC GetConnectError(void)=0;

private:
	iCallback *fCallback;
	iPtr<iConnection> fConnection;
	cAsyncNotifier fNotify;
	bool fCompleted=false;

protected:
	void InterfaceDelete(void);
	~cExtConnectionTask();

	class cEmptyCallback : public iCallback
	{
	public:
		virtual void ConnectionOnCancel(cExtConnectionTask *Task)override;
	};
	static cEmptyCallback EmptyCallback;
};
//---------------------------------------------------------------------------
class cBluetoothRFCOMMEndpointManager : protected cnRTL::cExtEndpoint::iCallback,
	protected cExtConnectionTask::iCallback
{
public:
	cBluetoothRFCOMMEndpointManager()=default;
	~cBluetoothRFCOMMEndpointManager()=default;

	iPtr<iConnectionTask> Channel_OnStartOpen(void);
	void Channel_OnFinishOpen(IOReturn error);

	void Channel_OnClosed(void);
	void Channel_OnData(const void *Data,uintn Size);
	void Channel_OnWriteCompleted(void);

private:
	cnLib_BluetoothRFCOMMChannelControl *fControl;
	bool fWriteBusy=false;

	class cPullProcedure : public cnRTL::cExclusiveProcedure
	{
	protected:
		virtual bool HostProcedure(void)override;
	}fPullProc;
	bool PullProcedure(void);

	class cPushProcedure : public cnRTL::cExclusiveProcedure
	{
	protected:
		virtual bool HostProcedure(void)override;
	}fPushProc;
	cnRTL::cStreamBufferQueue fPushQueue;
	bool PushProcedure(void);
	union{
		cnRTL::iInnerPtr<cExtConnectionTask> fTask;
		cnRTL::iInnerPtr<cnRTL::cExtEndpoint> fEndpoint;
	};
	void EndpointEnded(void);

protected:
	virtual void OnEndpointNotifyPush(cnRTL::cExtEndpoint *Endpoint,bool IdleNotify)override;
	virtual void OnEndpointNotifyPull(cnRTL::cExtEndpoint *Endpoint,bool IdleNotify)override;
	virtual void OnEndpointClose(cnRTL::cExtEndpoint *Endpoint)override;

	virtual void ConnectionOnCancel(cExtConnectionTask *Task)override;
};
//---------------------------------------------------------------------------
/*
class cBluetoothRFCOMMEndpoint : public cnRTL::cEndpoint, public cnRTL::cInterfaceVirtualDeletion
{
private:
	cnLib_BluetoothRFCOMMChannelControl *fControl;
	cnRTL::cAtomicVar<uint8> fInnerRef;
	bool fWriteBusy=false;

	class cPullProcedure : public cnRTL::cExclusiveProcedure
	{
	protected:
		virtual bool HostProcedure(void)override;
	}fPullProc;
	bool Pull(void);

	class cPushProcedure : public cnRTL::cExclusiveProcedure
	{
	protected:
		virtual bool HostProcedure(void)override;
	}fPushProc;
	cnRTL::cStreamBufferQueue fPushQueue;
	bool Push(void);

protected:
	void InterfaceDelete(void);

	virtual void cnLib_FUNC NotifyPull(void)override;
	virtual void cnLib_FUNC ResumePush(void)override;
	virtual void cnLib_FUNC PausePush(void)override;

public:
	cBluetoothRFCOMMEndpoint(cnLib_BluetoothRFCOMMChannelControl *Control);
	~cBluetoothRFCOMMEndpoint();

	void Channel_OnClosed(void);
	void Channel_OnData(const void *Data,uintn Size);
	void Channel_OnWriteCompleted(void);

	// iEndpoint

	virtual void Close(void)override;
};
//---------------------------------------------------------------------------
class cBluetoothRFCOMMConnectTask : public iConnectionTask//, public iConnectErrorReport
{
private:
	cnLib_BluetoothRFCOMMChannelControl *fControl;
	iPtr<iConnection> fConnection;
	cnRTL::cAsyncNotifier fNotify;
	bool fCompleted=false;
protected:
	~cBluetoothRFCOMMConnectTask();
public:
	cBluetoothRFCOMMConnectTask();

	bool ConnectToDevice(IOBluetoothDevice *Device,UInt8 ChannelID);

	void Channel_OnOpenCompleted(IOReturn error);

	virtual void cnLib_FUNC Cancel(void)override;

	virtual bool cnLib_FUNC IsDone(void)override;
	virtual bool cnLib_FUNC SetNotify(iProcedure *NotifyProcedure)override;
	virtual iConnection* cnLib_FUNC GetConnection(void)override;

	//virtual ConnectError cnLib_FUNC GetConnectError(void)=0;
};
*/
//---------------------------------------------------------------------------
#ifdef	__OBJC__
//---------------------------------------------------------------------------
iPtr<iConnectionTask> BluetoothRFCOMMConnect(IOBluetoothDevice *Device,UInt8 ChannelID);
//---------------------------------------------------------------------------
#endif	// __OBJC__
//---------------------------------------------------------------------------
}	// namespace cnMac
}	// namespace cnLibrary
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
#endif	// __cplusplus

/*-------------------------------------------------------------------------*/
