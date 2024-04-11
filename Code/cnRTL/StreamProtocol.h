/*- cnNet - Protocol Host -------------------------------------------------*/
/*         Developer : Code009                                             */
/*         Create on : 2019-03-15                                          */
/*-------------------------------------------------------------------------*/
#ifndef	__cnLibrary_cnRTL_StreamProtocol_H__
#define	__cnLibrary_cnRTL_StreamProtocol_H__
/*-------------------------------------------------------------------------*/
#include <cnRTL/cnRTLHeader.h>
#include <cnRTL/StreamBuffer.h>
#include <cnRTL/ObjectReference.h>
#include <cnRTL/AsyncTask.h>
#include <cnRTL/FunctionSet.h>
/*-------------------------------------------------------------------------*/
#ifdef __cplusplus
//---------------------------------------------------------------------------
namespace cnLibrary{
//---------------------------------------------------------------------------
namespace cnRTL{
//---------------------------------------------------------------------------
class cnLib_INTERFACE iProtocolSessionProcessor
{
public:
	virtual void ProtocolStarted(void)noexcept(true)=0;
	virtual void ProtocolStopped(void)noexcept(true)=0;
	virtual uIntn ProtocolInputPush(const void *Buffer,uIntn BufferSize)noexcept(true)=0;
	virtual uIntn ProtocolOutputPull(void *Buffer,uIntn BufferSize)noexcept(true)=0;
};
//---------------------------------------------------------------------------
class cnLib_INTERFACE iProtocolSession : public iReference
{
public:
	virtual iConnection* GetConnecton(void)noexcept(true)=0;
	virtual bool StartProcessor(iReference *Reference,iProtocolSessionProcessor *ProtocolProcessor,bool SingleThreaded)noexcept(true)=0;
	virtual void StopProcessor(bool Terminate)noexcept(true)=0;
	virtual void NotifyInput(void)noexcept(true)=0;
	virtual void NotifyOutput(void)noexcept(true)=0;
	virtual void SetEndOfOutput(void)noexcept(true)=0;
	virtual bool IsInputEnded(void)noexcept(true)=0;
	virtual bool IsOutputEnded(void)noexcept(true)=0;
};
//---------------------------------------------------------------------------
class cnLib_INTERFACE iProtocolProcessor
{
public:
	virtual void ProtocolStarted(void)noexcept(true)=0;
	virtual void ProtocolStopped(void)noexcept(true)=0;
	virtual uIntn ProtocolInputPush(const void *Buffer,uIntn BufferSize,uIntn &LeastSizeNeeded)noexcept(true)=0;
	virtual uIntn ProtocolOutputPull(void *Buffer,uIntn BufferSize,uIntn &LeastSizeNeeded)noexcept(true)=0;
	virtual void ProtocolInputEnd(void)noexcept(true)=0;
	virtual void ProtocolOutputEnd(void)noexcept(true)=0;
};
//---------------------------------------------------------------------------
class cnLib_INTERFACE iProtocolProvider : public iReference
{
public:
	virtual iConnection* GetConnecton(void)noexcept(true)=0;
	virtual bool StartProcessor(iReference *Reference,iProtocolProcessor *ProtocolProcessor,bool SingleThreaded)noexcept(true)=0;
	virtual void StopProcessor(bool Terminate)noexcept(true)=0;
	virtual void NotifyInput(uIntn LeastSizeNeeded=0)noexcept(true)=0;
	virtual void NotifyOutput(uIntn LeastSizeNeeded=0)noexcept(true)=0;
	virtual void SetEndOfOutput(void)noexcept(true)=0;
	virtual bool IsInputEnded(void)noexcept(true)=0;
	virtual bool IsOutputEnded(void)noexcept(true)=0;
};
//---------------------------------------------------------------------------
class bcProtocolProcessor : protected iProtocolProcessor
{
public:
	bcProtocolProcessor()noexcept(true);
	~bcProtocolProcessor()noexcept(true);

	iProtocolProvider* GetProvider(void)const noexcept(true);
	bool SetProvider(iProtocolProvider *ProtocolProvider,bool SingleThreaded)noexcept(true);

	bool IsActive(void)const noexcept(true);
	bool Start(iReference *Reference)noexcept(true);
	void Stop(void)noexcept(true);
	void Terminate(void)noexcept(true);

protected:
	rPtr<iProtocolProvider> fProtocolProvider;
	cAtomicVar<bool> fActiveMutex;
	bool fProtocolActive;
	bool fSingleThreaded;

	virtual void ProtocolStarted(void)noexcept(true)override;
	virtual void ProtocolStopped(void)noexcept(true)override;
	virtual void ProtocolInputEnd(void)noexcept(true)override{}
	virtual void ProtocolOutputEnd(void)noexcept(true)override{}
};
//---------------------------------------------------------------------------
#if 0
class cProtocolProviderFromStream : public iProtocolProvider, public cDualReference
{
public:
	cProtocolProviderFromStream(iPtr<iStream> Stream);
	~cProtocolProviderFromStream();

	void CloseProvider(void);

	virtual bool StartProcessor(iReference *Reference,iProtocolProcessor *ProtocolProcessor,bool SingleThreaded)override;
	virtual void StopProcessor(bool Terminate)override;
	virtual void NotifyInput(uIntn LeastSizeNeeded)override;
	virtual void NotifyOutput(uIntn LeastSizeNeeded)override;
	virtual void SetEndOfOutput(void)override;

	void NotifyInput(void);
	void NotifyOutput(void);

protected:
	void VirtualStarted(void);
	void VirtualStopped(void);

	iPtr<iStream> fStream;

private:
	iProtocolProcessor *fProtocolProcessor;
	rPtr<iReference> fProtocolProcessorReference;

	uIntn fInputNeedSize;
	cLoopbackStreamBuffer fInputPacker;
	bool QueryReadBuffer(cConstMemory &Buffer);
	void AdvanceReadBuffer(uIntn Size);

	uIntn fOutputNeedSize;
	cLoopbackStreamBuffer fOutputPacker;
	bool QueryWriteBuffer(cMemory &Buffer);
	void AdvanceWriteBuffer(uIntn Size);
	
	cExclusiveFlag fProcessorStateFlag;
	cExclusiveFlag fReadStateFlag;
	cExclusiveFlag fWriteStateFlag;

	cAtomicVar<bool> fRWQueueTerminate;
	cAtomicVar<bool> fProcessorStopped;

	bool fRWQueueConnected			:1;
	bool fProcessorSingleThreaded	:1;
	bool fProcessorStateStarted		:1;


	void ProcessorStateProcedure(void);
	void ProcessorStateProcess(void);

	bool fReadStopped		:1;
	bool fReadEnded			:1;
	bool fReadActive		:1;
	bool					:0;
	bool fWriteStopped		:1;
	bool fWriteEnded		:1;
	bool fWriteActive		:1;
	//bool					:0;

	bool fInputIdleNotify;
	bool fOutputIdleNotify;
	bool fSetWriteEnd;


	void SingleThreadStateProcedure(void);
	void ReadThreadStateProcedure(void);
	void WriteThreadStateProcedure(void);

	void ReadQueueProcess(void);
	void WriteQueueProcess(void);


};
#endif
//---------------------------------------------------------------------------
class cProtocolProviderFromRWQueue : public iProtocolProvider, public cDualReference, protected iWriteQueueCallback, protected iReadQueueCallback
{
public:
	cProtocolProviderFromRWQueue(iPtr<iConnection> Connection,rPtr<iReadQueue> ReadQueue,rPtr<iWriteQueue> WriteQueue)noexcept(true);
	~cProtocolProviderFromRWQueue()noexcept(true);

	void CloseProvider(void)noexcept(true);

	virtual iConnection* GetConnecton(void)noexcept(true)override;
	virtual bool StartProcessor(iReference *Reference,iProtocolProcessor *ProtocolProcessor,bool SingleThreaded)noexcept(true)override;
	virtual void StopProcessor(bool Terminate)noexcept(true)override;
	virtual void NotifyInput(uIntn LeastSizeNeeded)noexcept(true)override;
	virtual void NotifyOutput(uIntn LeastSizeNeeded)noexcept(true)override;
	virtual void SetEndOfOutput(void)noexcept(true)override;
	virtual bool IsInputEnded(void)noexcept(true)override;
	virtual bool IsOutputEnded(void)noexcept(true)override;

	void NotifyInput(void)noexcept(true);
	void NotifyOutput(void)noexcept(true);

protected:
	void VirtualStarted(void)noexcept(true);
	void VirtualStopped(void)noexcept(true);
	
	iPtr<iConnection> fConnection;
	rPtr<iReadQueue> fReadQueue;
	rPtr<iWriteQueue> fWriteQueue;

	// iReadQueueCallback

	virtual void cnLib_FUNC ReadStarted(void)noexcept(true)override;
	virtual void cnLib_FUNC ReadStopped(void)noexcept(true)override;
	virtual void cnLib_FUNC ReadNotify(void)noexcept(true)override;

	// iWriteQueueCallback

	virtual void cnLib_FUNC WriteStarted(void)noexcept(true)override;
	virtual void cnLib_FUNC WriteStopped(void)noexcept(true)override;
	virtual void cnLib_FUNC WriteNotify(void)noexcept(true)override;


private:
	iProtocolProcessor *fProtocolProcessor;
	rPtr<iReference> fProtocolProcessorReference;

	uIntn fInputNeedSize;
	cLoopbackStreamBuffer fInputPacker;
	bool QueryReadBuffer(cConstMemory &Buffer)noexcept(true);
	void AdvanceReadBuffer(uIntn Size)noexcept(true);

	uIntn fOutputNeedSize;
	bool QueryWriteBuffer(cMemory &Buffer)noexcept(true);
	void AdvanceWriteBuffer(uIntn Size)noexcept(true);
	
	cExclusiveFlag fProcessorStateFlag;
	cExclusiveFlag fReadStateFlag;
	cExclusiveFlag fWriteStateFlag;

	cAtomicVar<bool> fRWQueueTerminate;
	cAtomicVar<bool> fStopProcessor;

	bool fRWQueueConnected			:1;
	bool fProcessorSingleThreaded	:1;
	bool fProcessorStateStarted		:1;


	void ProcessorStateProcedure(void)noexcept(true);
	void ProcessorStateProcess(void)noexcept(true);

	bool fReadStopped		:1;
	bool fReadEnded			:1;
	bool fReadActive		:1;
	bool					:0;
	bool fWriteStopped		:1;
	bool fWriteEnded		:1;
	bool fWriteActive		:1;
	//bool					:0;

	bool fInputIdleNotify;
	bool fOutputIdleNotify;
	bool fSetWriteEnd;


	void SingleThreadStateProcedure(void)noexcept(true);
	void ReadThreadStateProcedure(void)noexcept(true);
	void WriteThreadStateProcedure(void)noexcept(true);

	void ReadQueueProcess(void)noexcept(true);
	void WriteQueueProcess(void)noexcept(true);


};
//---------------------------------------------------------------------------
rPtr<cProtocolProviderFromRWQueue> CreateProtocolProviderFromEndpoint(iConnection *Connection,iEndpoint *Endpoint)noexcept(true);
//---------------------------------------------------------------------------
class cnLib_INTERFACE iProtocolQueueProcessor
{
public:
	virtual void ProtocolQueueStarted(void)noexcept(true)=0;
	virtual void ProtocolQueueStopped(void)noexcept(true)=0;
	virtual void ProtocolQueueConnected(iProtocolProvider *Provider)noexcept(true)=0;
};
//---------------------------------------------------------------------------
class cnLib_INTERFACE iProtocolQueueProvider : public iReference
{
public:
	virtual bool StartQueue(iReference *Reference,iProtocolQueueProcessor *ProtocolQueueProcessor)noexcept(true)=0;
	virtual void StopQueue(void)noexcept(true)=0;
	virtual void MakeConnection(void)noexcept(true)=0;
};
//---------------------------------------------------------------------------
class bcProtocolQueueProcessor : protected iProtocolQueueProcessor, protected iProtocolProcessor
{
public:
	bcProtocolQueueProcessor()noexcept(true);
	~bcProtocolQueueProcessor()noexcept(true);

	iProtocolQueueProvider* GetQueueProvider(void)const noexcept(true);
	bool SetQueueProvider(iProtocolQueueProvider *QueueProvider,bool ProtocolSingleThreaded)noexcept(true);

	bool IsConnected(void)const noexcept(true);

	bool IsActive(void)const noexcept(true);
	bool Start(void)noexcept(true);
	void Stop(void)noexcept(true);
	void Terminate(void)noexcept(true);
	void DiscardConnection(void)noexcept(true);

protected:

	virtual iReference* ProcessorReference(void)noexcept(true)=0;

	// iProtocolQueueProcessor

	rPtr<iProtocolQueueProvider> fProtocolQueueProvider;
	virtual void ProtocolQueueStarted(void)noexcept(true)override;
	virtual void ProtocolQueueStopped(void)noexcept(true)override;
	virtual void ProtocolQueueConnected(iProtocolProvider *Provider)noexcept(true)override;

	// iProtocolProcessor

	virtual void ProtocolStarted(void)noexcept(true)override;
	virtual void ProtocolStopped(void)noexcept(true)override;
	virtual void ProtocolInputEnd(void)noexcept(true)override{}
	virtual void ProtocolOutputEnd(void)noexcept(true)override{}

	rPtr<iProtocolProvider> fProtocolProvider;

	void ProviderNotifyInput(uIntn LeastSizeNeeded=0)noexcept(true);
	void ProviderNotifyOutput(uIntn LeastSizeNeeded=0)noexcept(true);
	void ProviderSetEndOfOutput(void)noexcept(true);
	void ProviderSetEndOfQueue(void)noexcept(true);

	cAtomicVar<bool> fActiveMutex;
	bool fSingleThreaded;

private:

	cExclusiveFlag fProviderStateFlag;

	// queue state

	bool fQueueStateConnected		:1;
	bool fQueueStateActive			:1;
	bool					:0;

	// provider state

	bool fProviderConnected;

	// processor parameters

	ufInt8 fProcessorSetQueueEnded;
	ufInt8 fConnectionSetOutputEnded;
	bool fConnectionNotifyInput;
	bool fConnectionNotifyOutput;
	bool fConnectionStop;

	// user parameters

	bool fProcessorSingleThreaded	:1;
	bool							:0;

	void ProviderStateProcedure(void)noexcept(true);
	void ProviderStateThreadProc(void)noexcept(true);
	void ProviderStateProcess(void)noexcept(true);
	void ProviderNotification(void)noexcept(true);
};
//---------------------------------------------------------------------------
class bcProtocolQueueProvider : public iProtocolQueueProvider
{
public:
	bcProtocolQueueProvider()noexcept(true);
	~bcProtocolQueueProvider()noexcept(true);

	virtual bool StartQueue(iReference *Reference,iProtocolQueueProcessor *ProtocolQueueProcessor)noexcept(true)override;
	virtual void StopQueue(void)noexcept(true)override;
	virtual void MakeConnection(void)noexcept(true)override;

	void CloseQueue(void)noexcept(true);
	void MarkQueueEnd(void)noexcept(true);
protected:

	rPtr<iReference> fQueueProcessorReference;
	iProtocolQueueProcessor *fQueueProcessor;

	virtual void ProtocolQueueStarted(void)noexcept(true){}
	virtual void ProtocolQueueStopped(void)noexcept(true){}
	virtual void ProtocolQueueClosed(void)noexcept(true){}
	virtual rPtr<iProtocolProvider> ProtocolQueueFetch(void)noexcept(true)=0;

	void ProtocolQueueNotify(void)noexcept(true);


private:

	cExclusiveFlag fProviderQueueStateFlag;
	cAtomicVar<ufInt8> fProviderQueueCloseState;
	bool fProviderQueueIsEnded;

	// processor parameters

	cAtomicVar<bool> fStopProcessor;
	bool fProcessorStarted;
	bool fProcessorNeedConnection;

	void ProcessorStarted(void)noexcept(true);
	void ProcessorStopped(void)noexcept(true);

	void ProviderQueueStateProcedure(void)noexcept(true);
	void ProviderQueueStateProcess(void)noexcept(true);
};
//---------------------------------------------------------------------------
class cProtocolQueueFromEndpoints : public bcProtocolQueueProvider
{
public:
	cProtocolQueueFromEndpoints()noexcept(true);
	~cProtocolQueueFromEndpoints()noexcept(true);

	void QueueEndpoint(iEndpoint *Endpoint)noexcept(true);
	void QueueEndpoint(iConnection *Connection,iEndpoint *Endpoint)noexcept(true);

protected:
	virtual rPtr<iProtocolProvider> ProtocolQueueFetch(void)noexcept(true)override;

private:

	struct cEndpointItem : cRTLAllocator
	{
		cEndpointItem *Next;
		iPtr<iEndpoint> Endpoint;
		iConnection *Connection;
	};

	cAtomicQueueSO<cEndpointItem> fEndpointQueue;
};
//---------------------------------------------------------------------------
rPtr<cProtocolQueueFromEndpoints> CreateProtocolQueueProviderFromEndpoint(iEndpoint *Endpoint)noexcept(true);
rPtr<cProtocolQueueFromEndpoints> CreateProtocolQueueProviderFromEndpoint(iConnection *Connection,iEndpoint *Endpoint)noexcept(true);
//---------------------------------------------------------------------------
class cProtocolQueueFromConnector : public bcProtocolQueueProvider, public cDualReference
{
public:
	cProtocolQueueFromConnector(iConnectionConnector *Connector,iAddress *RemoteAddress)noexcept(true);
	~cProtocolQueueFromConnector()noexcept(true);

	//ufInt8 ConnectRetryLimit=0;			// 0=no limit
	//ufInt8 ConnectRetryDelaySeconds=0;	// seconds
	//uIntn GetConnectionState(void)const;
	void SetRetryLimit(ufInt8 RetryCount)noexcept(true);	// RetryLimit default to 0xFF(unlimited)

protected:
	void VirtualStarted(void)noexcept(true);
	void VirtualStopped(void)noexcept(true);

	iPtr<iConnectionConnector> fConnector;
	iPtr<iAddress> fRemoteAddress;

	virtual rPtr<iProtocolProvider> ProtocolQueueFetch(void)noexcept(true)override;

private:
	cExclusiveFlag fConnectFlag;
	bool fConnectionActive;
	bool fEndpointSetWriteEnd;
	ufInt8 fConnectRetryCount;
	ufInt8 fConnectRetryLimit;

	iPtr<iConnectionTask> fConnectTask;

	void SetupConnectTask(void)noexcept(true);
	void TaskCompleted(void)noexcept(true);
	
	class cConnectTaskCompleteProcedure : public iProcedure
	{
		virtual void cnLib_FUNC Execute(void)noexcept(true)override;
	}fConnectTaskCompleteProcedure;
};
//---------------------------------------------------------------------------
#if 0

class bcProtocolQueueProcessor : protected iProtocolProcessor
{
public:
	
	class bcProtocolQueueProvider : public iReference
	{
	public:
		bool QueueIsEnded=false;
		virtual void ProtocolQueueStarted(void){};
		virtual void ProtocolQueueStopped(void){};
		virtual rPtr<iProtocolProvider> ProtocolQueueFetch(void)=0;

		bool SetupProcessor(bcProtocolQueueProcessor *Owner);
		void ClearProcessor(void);
		void QueueNotify(void);
	protected:

		bcProtocolQueueProcessor *Owner=nullptr;

		cExclusiveFlag fClearFlag;
		bool fClearOwner=false;
		void OwnerNotifyProc(void);
	};

	class cProtocolQueueProviderFromProtocolProvider : public bcProtocolQueueProvider
	{
	public:
		cProtocolQueueProviderFromProtocolProvider(bcProtocolQueueProcessor *Owner,iProtocolProvider *ProtocolProvider);
		virtual rPtr<iProtocolProvider> ProtocolQueueFetch(void)override;
	protected:
		rPtr<iProtocolProvider> fProvider;
	};

	bcProtocolQueueProcessor();
	~bcProtocolQueueProcessor();

	bool SetQueue(bcProtocolQueueProvider *QueueProvider);
	bool SetProtocolProvider(iProtocolProvider *ProtocolProvider);

	void CloseQueue(void);
	bool IsConnected(void)const;
	bool GetQueueActive(void)const;
	bool SetQueueActive(bool Active);


protected:

	virtual void ProtocolStarted(void)override;
	virtual void ProtocolStopped(void)override;
	virtual void ProtocolInputEnd(void)override{}
	virtual void ProtocolOutputEnd(void)override{}

	rPtr<bcProtocolQueueProvider> fProtocolQueueProvider;
	
	virtual iReference* ProtocolQueueReference(void)=0;
	virtual void ProtocolQueueStarted(void){}
	virtual void ProtocolQueueStopped(void){}
	virtual void ProtocolQueueClosed(void){}

	void UpdateProviderQueue(void);

	rPtr<iProtocolProvider> fProtocolProvider;

	void ProviderNotifyInput(uIntn LeastSizeNeeded=0);
	void ProviderNotifyOutput(uIntn LeastSizeNeeded=0);
	void ProviderSetEndOfOutput(void);
	void ProviderSetEndOfQueue(void);
private:

	cExclusiveFlag fProviderQueueStateFlag;
	cAtomicVar<ufInt8> fProviderQueueCloseState;

	// queue state

	bool fQueueStateConnected		:1;
	bool fQueueStateRunning			:1;
	//bool fReadQueueNotify	:1;
	//bool fReadEnded			:1;
	bool					:0;

	// provider state

	bool fProviderConnected;

	// processor parameters

	ufInt8 fProcessorSetQueueEnded;
	ufInt8 fProcessorSetProviderEnded;
	bool fProcessorNotifyInput;
	bool fProcessorNotifyOutput;

	// user parameters

	bool fProviderQueueActive		:1;
	bool fProcessorSingleThreaded	:1;
	bool							:0;

	void ProviderQueueStateProcedure(void);
	void ProviderQueueStateThreadProc(void);
	void ProviderQueueStateProcess(void);
};
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
class cProtocolConnectorQueueProvider : public bcProtocolQueueProcessor::bcProtocolQueueProvider, public cDualReference
{
public:
	cProtocolConnectorQueueProvider(iConnectionConnector *Connector,iAddress *RemoteAddress);
	~cProtocolConnectorQueueProvider();

	virtual void ProtocolQueueStarted(void)override;
	virtual void ProtocolQueueStopped(void)override;
	virtual rPtr<iProtocolProvider> ProtocolQueueFetch(void)override;
	
	//ufInt8 ConnectRetryLimit=0;			// 0=no limit
	//ufInt8 ConnectRetryDelaySeconds=0;	// seconds
	//uIntn GetConnectionState(void)const;
	void SetRetryLimit(ufInt8 RetryCount);	// RetryLimit default to 0xFF(unlimited)

protected:
	void VirtualStopped(void);

	iPtr<iConnectionConnector> fConnector;
	iPtr<iAddress> fRemoteAddress;

private:
	cExclusiveFlag fConnectFlag;
	bool fConnectionActive;
	bool fEndpointSetWriteEnd;
	ufInt8 fConnectRetryCount;
	ufInt8 fConnectRetryLimit;

	iPtr<iConnectionTask> fConnectTask;

	void SetupConnectTask(void);
	void TaskCompleted(void);
	
	class cConnectTaskCompleteProcedure : public iProcedure
	{
		virtual void cnLib_FUNC Execute(void)override;
	}fConnectTaskCompleteProcedure;
};
#endif // 0
//---------------------------------------------------------------------------
#if 0

class bcProtocolProviderFromRWQueue : public iProtocolProvider, protected iWriteQueueCallback, protected iReadQueueCallback
{
public:
	bcProtocolProviderFromRWQueue();
	~bcProtocolProviderFromRWQueue();

	bool IsConnected(void)const;
	void CloseProvider(bool Terminate);

	virtual bool StartProcessor(iReference *Reference,iProtocolProcessor *ProtocolProcessor,bool SingleThreaded)override;
	virtual void StopProcessor(void)override;
	virtual void TerminateConnection(void)override;
	virtual void NotifyInput(uIntn LeastSizeNeeded)override;
	virtual void NotifyOutput(uIntn LeastSizeNeeded)override;
	virtual void SetEndOfOutput(void)override;

protected:

	rPtr<iReadQueue> fReadQueue;
	rPtr<iWriteQueue> fWriteQueue;

	// iReadQueueCallback

	virtual void cnLib_FUNC ReadStarted(void)override;
	virtual void cnLib_FUNC ReadStopped(void)override;
	virtual void cnLib_FUNC ReadNotify(void)override;

	// iWriteQueueCallback

	virtual void cnLib_FUNC WriteStarted(void)override;
	virtual void cnLib_FUNC WriteStopped(void)override;
	virtual void cnLib_FUNC WriteNotify(void)override;


	// subclass

	virtual void ProviderClosed(void){}
	virtual bool ProviderFetchQueue(void)=0;
	virtual void ProviderConnectionFailed(void){}
	virtual void ProviderConnectionStarted(void){}
	virtual void ProviderConnectionStopped(void){}
	virtual void ProviderIdle(void){}

	void NotifyConnectionState(void);

private:
	iProtocolProcessor *fProtocolProcessor;
	rPtr<iReference> fProtocolProcessorReference;

	void StartRWQueue(void);
	void TerminateRWQueue(void);


	uIntn fInputNeedSize;
	cLoopbackStreamBuffer fInputPacker;
	bool QueryReadBuffer(cConstMemory &Buffer);
	void AdvanceReadBuffer(uIntn Size);

	uIntn fOutputNeedSize;
	bool QueryWriteBuffer(cMemory &Buffer);
	void AdvanceWriteBuffer(uIntn Size);
	
	cExclusiveFlag fConnectionStateFlag;
	cExclusiveFlag fReadStateFlag;
	cExclusiveFlag fWriteStateFlag;

	cAtomicVar<bool> fProcessorStopped;

	cAtomicVar<ufInt8> fProviderCloseState;

	bool fProcessorSingleThreaded	:1;
	bool fConnStateConnected		:1;
	bool fConnStateConnectSuccess	:1;
	bool fConnStateProcessStarted	:1;


	void ConnectionStateProcedure(void);
	bool ConnectionStateProcess(void);

	void ReadStateProcedure(void);
	void WriteStateProcedure(void);
	void RWStateProcedure(void);

	//bool fReadQueueNotify	:1;
	bool fReadEnded			:1;
	bool fReadActive		:1;
	bool					:0;
	//bool fWriteQueueNotify	:1;
	bool fWriteEnded		:1;
	bool fWriteActive		:1;
	//bool					:0;

	bool fReadClose;
	bool fWriteClose;
	bool fProcessorInputNotify;
	bool fProcessorOutputNotify;
	bool fSetWriteEnd;
	bool fTerminateConnection;


	void SingleThreadStateProcedure(void);
	void ReadThreadStateProcedure(void);
	void WriteThreadStateProcedure(void);
	bool ReadStateProcess(void);
	bool WriteStateProcess(void);

	bool ReadQueueProcess(void);
	bool WriteQueueProcess(void);


};
//---------------------------------------------------------------------------
class cProtocolProviderFromEndpoint : public bcProtocolProviderFromRWQueue
{
public:
	cProtocolProviderFromEndpoint();
	~cProtocolProviderFromEndpoint();

	void QueueEndpoint(iEndpoint *Endpoint,bool KeepOpen=false);
	void MarkQueueEnd(void);

protected:
	virtual bool ProviderFetchQueue(void)override;
	virtual void ProviderConnectionFailed(void)override;
	virtual void ProviderConnectionStarted(void)override;
	virtual void ProviderIdle(void)override;

private:

	struct cEndpointItem : cRTLAllocator
	{
		cEndpointItem *Next;
		iPtr<iEndpoint> Endpoint;

		bool KeepOpen;
	};

	cAtomicQueueS<cEndpointItem> fEndpointQueue;
	bool fQueueSetEnd;

	cEndpointItem *fCurItem;
};
//---------------------------------------------------------------------------
class cProtocolProviderFromConnector: public bcProtocolProviderFromRWQueue, public cDualReference
{
public:
	cProtocolProviderFromConnector();
	~cProtocolProviderFromConnector();

	//ufInt8 ConnectRetryLimit=0;			// 0=no limit
	//ufInt8 ConnectRetryDelaySeconds=0;	// seconds
	//uIntn GetConnectionState(void)const;
	cFunctionSet<void (void)> OnConnectionStateChanged;

	bool SetConnector(iConnectionConnector *Connector,iAddress *RemoteAddress);
	bool SetConnectionProtocol(iConnectionProtocol *Protocol,iAddress *RemoteAddress,iAddress *LocalAddress);
	bool GetConnectionActive(void)const;
	void SetConnectionActive(bool Active);
	void SetConnectionRetryLimit(ufInt8 RetryCount);	// RetryLimit default to 0xFF(unlimited)

protected:
	void VirtualStopped(void);

	virtual void ProviderClosed(void)override;
	virtual bool ProviderFetchQueue(void)override;
	virtual void ProviderConnectionStarted(void)override;
	virtual void ProviderConnectionStopped(void)override;

	iPtr<iConnectionConnector> fConnector;
	iPtr<iAddress> fRemoteAddress;

private:
	cExclusiveFlag fConnectFlag;
	bool fConnectionActive;
	bool fEndpointSetWriteEnd;
	ufInt8 fConnectRetryCount;
	ufInt8 fConnectRetryLimit;

	iPtr<iConnectionTask> fConnectTask;

	void SetupConnectTask(void);
	
	class cConnectTaskCompleteProcedure : public iProcedure
	{
		virtual void cnLib_FUNC Execute(void)override;
	}fConnectTaskCompleteProcedure;
};
//---------------------------------------------------------------------------
class cPipelineMessageProtocolHost : public iProtocolProvider,private iProtocolProcessor
{
public:
	cPipelineMessageProtocolHost();
	~cPipelineMessageProtocolHost();


	// Start
	//	start protocol
	bool Start(iProtocolProvider *ProtocolProvider);
	// Stop
	//	stop protocol
	void Stop(void);

	class cnLib_INTERFACE iCallback
	{
	public:
		virtual iProtocolProcessor* QueryMessage(iProtocolProvider *ProtocolProvider)=0;
	};
	iCallback *Callback=nullptr;

	void NotifyRequest(void);

	virtual bool StartProcessor(iReference *Reference,iProtocolProcessor *ProtocolProcessor,bool SingleThreaded)override;
	virtual void StopProcessor(void)override;
	virtual void TerminateConnection(void)override;
	virtual void NotifyInput(uIntn LeastSizeNeeded)override;
	virtual void NotifyOutput(uIntn LeastSizeNeeded)override;
	virtual void SetEndOfOutput(void)override;
protected:

	rPtr<iProtocolProvider> fProtocolProvider;

	virtual void ProtocolStarted(void)override;
	virtual void ProtocolStopped(void)override;
	virtual void ProtocolConnected(void)override;
	virtual void ProtocolDisconnected(void)override;
	virtual uIntn ProtocolInputPush(const void *Buffer,uIntn BufferSize,uIntn &LeastSizeNeeded)override;
	virtual uIntn ProtocolOutputPull(void *Buffer,uIntn BufferSize,uIntn &LeastSizeNeeded)override;
	virtual void ProtocolInputClosed(bool GracefulClose)override;
	virtual void ProtocolOutputClosed(bool GracefulClose)override;

	cAtomicVar<uInt8> fHostChannelRefCount=0;
	void HostChannelRelease(void);

	bool fMessageInputDone;
	bool fMessageOutputDone;

	iProtocolProcessor *fSendingMessage;
	void FetchSendMessage(void);
	void FetchReceiveMessage(void);
	
	struct cRequestItem
	{
		cRequestItem *Next;
		iProtocolProcessor *Message;
	};
	cAtomicQueueS<cRequestItem> fReceiveQueue;
	iProtocolProcessor *fReceivingMessage;

};
#endif // 0
//---------------------------------------------------------------------------
}	// namespace cnRTL
//---------------------------------------------------------------------------
}	// namespace cnLibrary
//---------------------------------------------------------------------------
#endif  /* __cplusplus */
/*-------------------------------------------------------------------------*/
#endif
