/*- cnNet - HTTP ----------------------------------------------------------*/
/*         Developer : Code009                                             */
/*         Create on : 2016-12-19                                          */
/*-------------------------------------------------------------------------*/
#ifndef	__cnLib_cnNet_HTTP_H__
#define	__cnLib_cnNet_HTTP_H__
/*-------------------------------------------------------------------------*/
#include <cnNet/cnProtocol.h>
/*-------------------------------------------------------------------------*/
#ifdef __cplusplus
//---------------------------------------------------------------------------
namespace cnLibrary{
//---------------------------------------------------------------------------
namespace cnNet{
//---------------------------------------------------------------------------
namespace HTTP{
//---------------------------------------------------------------------------
extern const char TextLineReturn[2];
//---------------------------------------------------------------------------
struct URIPart
{
	ufInt16 Sc;
	ufInt16 ScLen;
	ufInt16 Host;
	ufInt16 HostLen;
	ufInt16 Path;
	ufInt16 PathLen;
};
URIPart ParseURI(const char *URL,uIntn Length)noexcept(true);
//---------------------------------------------------------------------------
bool ParseLine(uIntn &LineLength,const void *Buffer,uIntn BufferSize)noexcept(true);
uIntn ParseHeaderMessage(ufInt16 *Pos,ufInt16 *Len,uIntn PosCount,const char *Line,uIntn LineLength)noexcept(true);
bool ParseHeaderParameter(ufInt16 &NameLen,const char *Line,uIntn LineLength)noexcept(true);
void TrimLWS(ufInt16 &Pos,ufInt16 &Len,const char *Text,uIntn Length)noexcept(true);
//---------------------------------------------------------------------------
void AppendDate(cStringBuffer<char> &DateString,const iTimepoint *Time)noexcept(true);
//---------------------------------------------------------------------------
class cnLib_INTERFACE iClientProcessor
{
public:
	virtual void HTTPRequestStart(cArray<const char> &Method,cArray<const char> &Path,cArray<const char> &Version)noexcept(true)=0;
	virtual bool HTTPRequestHeader(cArray<const char> &Name,cArray<const char> &Value)noexcept(true)=0;
	virtual uIntn HTTPRequestBody(void *Buffer,uIntn BufferSize)noexcept(true)=0;
	
	virtual void HTTPResponseStatus(const char *Version,uIntn VersionLength,const char *Status,uIntn StatusLength,const char *StatusString,uIntn StatusStringLength)noexcept(true)=0;
	virtual void HTTPResponseHeader(const char *Name,uIntn NameLength,const char *Value,uIntn ValueLength)noexcept(true)=0;
	virtual void HTTPResponseHeaderFinish(uIntn &ExpectedBodySize)noexcept(true)=0;
	virtual uIntn HTTPResponseBody(const void *Buffer,uIntn Size)noexcept(true)=0;
	virtual void HTTPResponseBodyFinish(void)noexcept(true)=0;

	virtual void HTTPTerminated(void)noexcept(true)=0;
};
//---------------------------------------------------------------------------
class cnLib_INTERFACE iClientHost : public iReference
{
public:
	virtual bool HTTPStart(iClientProcessor *ClientProcessor)noexcept(true)=0;
	virtual void HTTPTerminate(void)noexcept(true)=0;
	virtual void HTTPNotifySendRequest(void)noexcept(true)=0;
	virtual void HTTPCompleteRequest(void)noexcept(true)=0;
	virtual void HTTPNotifyReceiveResponse(void)noexcept(true)=0;
};
//---------------------------------------------------------------------------
class cClientSession : public cnRTL::cDualReference, public iReference, private iProtocolProcessor
{
public:
	cClientSession()noexcept(true);
	~cClientSession()noexcept(true);
	
	bool SetProtocolHost(iProtocolProvider *Provider)noexcept(true);

	rPtr<iClientHost> CreateHost(void)noexcept(true);

protected:
	void VirtualStopped(void)noexcept(true);

	class cClientHost : public iClientHost
	{
	public:
		cClientHost *Next;

		cClientHost(cClientSession *Session)noexcept(true);

		virtual bool HTTPStart(iClientProcessor *ClientProcessor)noexcept(true)override;
		virtual void HTTPTerminate(void)noexcept(true)override;
		virtual void HTTPNotifySendRequest(void)noexcept(true)override;
		virtual void HTTPCompleteRequest(void)noexcept(true)override;
		virtual void HTTPNotifyReceiveResponse(void)noexcept(true)override;

		iClientProcessor *ClientProcessor=nullptr;

		bool RequestCompleted;
	private:
		rPtr<cClientSession> fSession;
	};

	void MessageEnqueue(cClientHost *ClientHost)noexcept(true);
	void MessageTerminate(cClientHost *ClientHost)noexcept(true);
	void MessageNotifySendRequest(cClientHost *ClientHost)noexcept(true);
	void MessageCompleteRequest(cClientHost *ClientHost)noexcept(true);
	void MessageNotifyReceiveResponse(cClientHost *ClientHost)noexcept(true);
private:
	rPtr<iProtocolProvider> fProtocolProvider;

	virtual void ProtocolStarted(void)noexcept(true)override;
	virtual void ProtocolStopped(void)noexcept(true)override;
	virtual uIntn ProtocolInputPush(const void *Buffer,uIntn BufferSize,uIntn &LeastSizeNeeded)noexcept(true)override;
	virtual uIntn ProtocolOutputPull(void *Buffer,uIntn BufferSize,uIntn &LeastSizeNeeded)noexcept(true)override;
	virtual void ProtocolInputEnd(void)noexcept(true)override;
	virtual void ProtocolOutputEnd(void)noexcept(true)override;

	cAtomicVar<uInt8> fHostChannelRefCount=0;
	void HostChannelRelease(void)noexcept(true);

	ufInt8 fSessionStopState;

	enum{
		ssMethod,ssHeader,ssHeaderEnd1,ssHeaderEnd2,ssBody
	}fSendState;
	enum{
		rsHeader,rsBody
	}fResponseState;
	enum{
		hpsRequest,hpsParameter,hpsError
	}fResponseHeaderState;
	bool fShutdownAfterRequest=false;

	// pipeline

	cAtomicQueueSO<cClientHost> fSendQueue;
	cAtomicQueueSO<cClientHost> fReceiveQueue;
	rPtr<cClientHost> fSendingHost;
	rPtr<cClientHost> fReceivingHost;

	// request

	cMemoryBuffer fSendBuffer;

	bool PullHeader(void)noexcept(true);

	// response
	uIntn fExpectedResponseBodySize;

	const char* ParseResponseHeaderLine(uIntn &LineLength)noexcept(true);
	void ProcessReponseHeaderLine(const char *Line,uIntn LineSize)noexcept(true);

	void ProcessInputHeader(uIntn FirstLineLength)noexcept(true);
	void ProcessInputBody(void)noexcept(true);
	
	cnRTL::cMemoryBufferQueueReadBuffer fInputBufferReader;

};
//---------------------------------------------------------------------------
class cnLib_INTERFACE iServerMessageHost
{
public:
	virtual void HTTPTerminate(void)noexcept(true)=0;
	virtual void HTTPNotifyReceiveRequest(void)noexcept(true)=0;
	virtual void HTTPNotifySendResponse(void)noexcept(true)=0;
	virtual void HTTPCompleteResponse(void)noexcept(true)=0;
};
//---------------------------------------------------------------------------
class cnLib_INTERFACE iServerMessageProcessor
{
public:
	virtual uIntn HTTPRequestBody(const void *Buffer,uIntn Size)noexcept(true)=0;
	virtual void HTTPRequestBodyFinish(void)noexcept(true)=0;

	virtual void HTTPResponseStatus(cArray<const char> &Version,cArray<const char> &Status,cArray<const char> &StatusString)noexcept(true)=0;
	virtual bool HTTPResponseHeader(cArray<const char> &Name,cArray<const char> &Value)noexcept(true)=0;
	virtual uIntn HTTPResponseBody(void *Buffer,uIntn Size)noexcept(true)=0;
	virtual void HTTPResponseFinish(void)noexcept(true)=0;
	virtual void HTTPTerminated(void)noexcept(true)=0;
};
//---------------------------------------------------------------------------
class cServerSession : public iReference, private iProtocolProcessor
{
public:
	cServerSession()noexcept(true);
	~cServerSession()noexcept(true);

	bool Start(iProtocolProvider *Provider)noexcept(true);
	void Stop(void)noexcept(true);

protected:
	virtual void HTTPRequestStart(const char *Method,uIntn MethodLength,const char *Path,uIntn PathLength,const char *Version,uIntn VersionLength)noexcept(true)=0;
	virtual void HTTPRequestHeader(const char *Name,uIntn NameLength,const char *Value,uIntn ValueLength)noexcept(true)=0;
	virtual iServerMessageProcessor* HTTPRequestHeaderFinish(iServerMessageHost *Host,uIntn &ExpectedRequestBodySize)noexcept(true)=0;

private:

	class cMessage : public iServerMessageHost,public cnRTL::cRTLAllocator
	{
	public:
		cMessage *Next;

		cMessage(cServerSession *Session)noexcept(true);

		virtual void HTTPTerminate(void)noexcept(true)override;
		virtual void HTTPNotifyReceiveRequest(void)noexcept(true)override;
		virtual void HTTPNotifySendResponse(void)noexcept(true)override;
		virtual void HTTPCompleteResponse(void)noexcept(true)override;

		iServerMessageProcessor *Processor=nullptr;

		bool ResponseCompleted;
	private:
		rPtr<cServerSession> fSession;
	};

	void MessageTerminate(cMessage *Message)noexcept(true);
	void MessageNotifyReceiveRequest(cMessage *Message)noexcept(true);
	void MessageNotifySendResponse(cMessage *Message)noexcept(true);
	void MessageCompleteResponse(cMessage *Message)noexcept(true);
private:
	rPtr<iProtocolProvider> fProtocolPovider;

	virtual void ProtocolStarted(void)noexcept(true)override;
	virtual void ProtocolStopped(void)noexcept(true)override;
	virtual uIntn ProtocolInputPush(const void *Buffer,uIntn BufferSize,uIntn &LeastSizeNeeded)noexcept(true)override;
	virtual uIntn ProtocolOutputPull(void *Buffer,uIntn BufferSize,uIntn &LeastSizeNeeded)noexcept(true)override;
	virtual void ProtocolInputEnd(void)noexcept(true)override;
	virtual void ProtocolOutputEnd(void)noexcept(true)override;

	cAtomicVar<uInt8> fHostChannelRefCount=0;
	void HostChannelRelease(void)noexcept(true);

	ufInt8 fSessionStopState;

	enum{
		ssMethod,ssHeader,ssHeaderEnd,ssBody
	}fSendState;
	enum{
		rsHeader,rsBody,rsWait
	}fRequestState;
	enum{
		hpsRequest,hpsParameter,hpsError
	}fRequestHeaderState;
	uInt8 fResponseHeaderFinalLine;
	bool fShutdownAfterResponse=false;


	// request

	cnRTL::cMemoryBufferQueueReadBuffer fInputBufferReader;
	cMessage *fReceivingMessage;

	uIntn fExpectedRequestBodySize;

	const char* ParseRequestHeaderLine(uIntn &LineLength)noexcept(true);
	void ProcessRequestHeaderLine(const char *Line,uIntn LineSize)noexcept(true);

	void ProcessInputHeader(uIntn FirstLineLength)noexcept(true);
	void ProcessInputBody(void)noexcept(true);

	// response

	cAtomicVar<cMessage*> fPendingResponse;
	cMessage *fSendingMessage;
	

	cMemoryBuffer fSendBuffer;

	bool PullHeader(void)noexcept(true);
};
//---------------------------------------------------------------------------
cnLib_ENUM_BEGIN(ufInt8,ClientQueryState)
{
	Idle,
	Connecting,
	Requesting,
	Sending,
	WaitingResponse,
	ReceiveHeader,
	ReceiveBody,
	Done
}cnLib_ENUM_END(ClientQueryState);
//---------------------------------------------------------------------------
class cClientQuery : private iClientProcessor
{
public:
	cClientQuery()noexcept(true);
	~cClientQuery()noexcept(true);
	

	bool Start(iReference *Reference,iClientHost *ClientHost)noexcept(true);
	bool Finish(void)noexcept(true);

	eClientQueryState GetState(void)noexcept(true);

	const cString<char>& GetHost(void)noexcept(true);

	bool SetURL(const char *URL)noexcept(true);
	void SetMethod(cString<char> Method)noexcept(true);
	void SetHeader(cString<char> Name,cString<char> Value)noexcept(true);
	void SetRequestInput(iReadBuffer<void> *ReadBuffer)noexcept(true);
	void SetResponseOutput(iWriteBuffer<void> *WriteBuffer)noexcept(true);

	int GetResponseStatus(void)noexcept(true);
	const cString<char>& GetResponseStatusString(void)noexcept(true);

	cnRTL::cFunction<void (const char *Name,uIntn NameLength,const char *Value,uIntn ValueLength)noexcept(true)> OnResponseHeader;
	cnRTL::cFunction<void (void)noexcept(true)> OnStateChange;
	cnRTL::cFunction<void (void)noexcept(true)> OnCompleted;
	
protected:
private:

	rPtr<iReference> fHostReference;
	eClientQueryState fState;

	void CallEvent(cnRTL::cFunction<void (void)noexcept(true)> &Event)noexcept(true);
	void SetState(eClientQueryState State)noexcept(true);

	cString<char> fHost;
	cString<char> fPath;
	cString<char> fMethod;
	cSeqMap< cString<char>,cString<char> > fHeaderMap;
	uIntn fHeaderEnumIndex;

	rPtr<iClientHost> fHTTPHost;
	iReadBuffer<void> *fRequestReadBuffer;
	iWriteBuffer<void> *fResponseWriteBuffer;
	bool fResponseWriteBufferFull=false;

	int fResponseStatus;
	cString<char> fResponseStatusString;

	virtual void HTTPRequestStart(cArray<const char> &Method,cArray<const char> &Path,cArray<const char> &Version)noexcept(true)override;
	virtual bool HTTPRequestHeader(cArray<const char> &Name,cArray<const char> &Value)noexcept(true)override;
	virtual uIntn HTTPRequestBody(void *Buffer,uIntn BufferSize)noexcept(true)override;
	virtual void HTTPResponseStatus(const char *Version,uIntn VersionLength,const char *Status,uIntn StatusLength,const char *StatusString,uIntn StatusStringLength)noexcept(true)override;
	virtual void HTTPResponseHeader(const char *Name,uIntn NameLength,const char *Value,uIntn ValueLength)noexcept(true)override;
	virtual void HTTPResponseHeaderFinish(uIntn &ExpectedBodySize)noexcept(true)override;
	virtual uIntn HTTPResponseBody(const void *Buffer,uIntn Size)noexcept(true)override;
	virtual void HTTPResponseBodyFinish(void)noexcept(true)override;
	virtual void HTTPTerminated(void)noexcept(true)override;
};
//---------------------------------------------------------------------------
}	// namespace HTTP
#if 0
//---------------------------------------------------------------------------
class cnLib_INTERFACE iHTTPMessageHost
{
public:
	virtual void cnLib_FUNC HTTPTerminate(void)=0;
	virtual void cnLib_FUNC HTTPNotifyPull(void)=0;
	virtual void cnLib_FUNC HTTPEndPull(bool Shutdown)=0;
};
//---------------------------------------------------------------------------
class cnLib_INTERFACE iHTTPClientRequestor : public iInterface
{
public:
	virtual void cnLib_FUNC StartClient(iHTTPMessageHost *ClientHost)=0;
	virtual bool cnLib_FUNC PullRequest(void *Buffer,uIntn BufferSize,uIntn &SizeReturn)=0;
	virtual void cnLib_FUNC StatusLine(const char *Version,uIntn VersionLength,const char *Status,uIntn StatusLength,const char *StatusString,uIntn StatusStringLength)=0;
	virtual void cnLib_FUNC Parameter(const char *Name,uIntn NameLength,const char *Value,uIntn ValueLength)=0;
	virtual void cnLib_FUNC HeaderFinish(uIntn &ExpectedBodySize)=0;
	virtual void cnLib_FUNC Body(const void *Buffer,uIntn Size)=0;
	virtual void cnLib_FUNC BodyFinish(void)=0;
	virtual void cnLib_FUNC Terminated(void)=0;
};
//---------------------------------------------------------------------------
class cnLib_INTERFACE iHTTPServerResponder : public iInterface
{
public:
	virtual void cnLib_FUNC StartServer(iHTTPMessageHost *ServerHost)=0;
	virtual void cnLib_FUNC Parameter(const char *Name,uIntn NameLength,const char *Value,uIntn ValueLength)=0;
	virtual void cnLib_FUNC HeaderFinish(uIntn &ExpectedBodySize)=0;
	virtual void cnLib_FUNC Body(const void *Buffer,uIntn Size)=0;
	virtual void cnLib_FUNC BodyFinish(void)=0;
	virtual bool cnLib_FUNC PullResponse(void *Buffer,uIntn BufferSize,uIntn &SizeReturn)=0;
	virtual void cnLib_FUNC Terminated(void)=0;
};
//---------------------------------------------------------------------------
class cHTTPServerSession : public iReference, private iProtocolProcessor
{
protected:
public:

	cHTTPServerSession();
	~cHTTPServerSession();
	class cnLib_INTERFACE iCallback
	{
	public:
		virtual void HTTPSessionStarted(cHTTPServerSession *Session)=0;
		virtual void HTTPSessionStopped(cHTTPServerSession *Session)=0;
		virtual iPtr<iHTTPServerResponder> HTTPCreateResponder(iAddress *ClientAddress,const char *Method,uIntn MethodLength,const char *Path,uIntn PathLength,const char *Version,uIntn VersionLength)=0;
	};
	iCallback *Callback=nullptr;

	bool Start(iProtocolHost *ProtocolHost);
	void Stop(void);

protected:

	rPtr<iProtocolHost> fProtocolHost;
	virtual bool InputPush(const void *Buffer,uIntn BufferSize)override;
	virtual void InputClose(bool StreamEnded)override;

	virtual bool OutputPull(void *Buffer,uIntn BufferSize,uIntn &SizeReturn)override;
	virtual void OutputClose(void)override;

	cAtomicVar<uInt8> fStreamRefCount=0;
	cnRTL::cExclusiveFlag fPullStreamFlag;
	bool fStopPull=false;
	bool fPullStreamEnded=false;
	bool fPullIdleNotify=false;
	void StreamClosed(void);


	cBufferPacker fPushBufferPacker;

	class cResponseMessage : public iHTTPMessageHost
	{
	public:
		cResponseMessage *Next;
		iPtr<iHTTPServerResponder> Responder;
		bool EndPull=false;
		bool ShutdownAfterResponse=false;

		void SessionMessageEnd(void);

		virtual void cnLib_FUNC HTTPTerminate(void)override;
		virtual void cnLib_FUNC HTTPNotifyPull(void)override;
		virtual void cnLib_FUNC HTTPEndPull(bool Shutdown)override;

		cResponseMessage(cHTTPServerSession *SessionHost,iHTTPServerResponder *MessageResponder);
	protected:
		~cResponseMessage();
	private:

		bool fSessionEnded;
		cnRTL::cExclusiveFlag fDeleteExclusiveFlag;
		cHTTPServerSession *fSessionHost;

		void CheckDelete(void);
		void ResponderEnd(void);
	};
	cResponseMessage *fPushMessage;
	cResponseMessage *fResponseMessage;
	void HTTPTerminate(cResponseMessage *ResponseMessage);
	void HTTPNotifyPull(cResponseMessage *ResponseMessage);
	void HTTPEndPull(cResponseMessage *ResponseMessage);


	cAtomicQueueS<cResponseMessage> fPullMessageQueue;

	/*
	struct cResponseItem  : cSingleLinkedListItem
	{
		cResponseMessage *ResponseMessage;
		iPtr<iHTTPServerResponder> Responder;
	};
	cInterlockedQueueS<cResponseItem> fPullResponders;*/

	enum{
		rsError,rsHeader,rsBody
	}fRequestState=rsHeader;
	enum{
		hpsRequest,hpsParameter
	}fHeaderProcessState=hpsRequest;
	bool fShutdownAfterResponse=false;

	bool fStopRequest;

	uIntn fExpectedBodySize;

	bool ProcessRequestHeaderLine(const char *Line,uIntn LineSize);
	void ProcessRequestParameterLine(const char *Line,uIntn LineSize);
	void RequestComplete(void);

	void NotifyPull(void);
};
//---------------------------------------------------------------------------
class cHTTPServerManager : protected iConnectionQueueCallback, private cHTTPServerSession::iCallback
{
public:
	cHTTPServerManager();
	~cHTTPServerManager();
	
	// Reference
	//	reference for owner. referenced when the object is active, and should not be deleted
	iReference *Reference;


	class cnLib_INTERFACE iCallback
	{
	public:
		virtual iPtr<iHTTPServerResponder> CreateResponder(iAddress *ClientAddress,const char *Method,uIntn MethodLength,const char *Path,uIntn PathLength,const char *Version,uIntn VersionLength)=0;
	};
	iCallback *Callback=nullptr;

	bool Start(iConnectionQueue *Queue);
	void Stop(void);

private:
	iPtr<iConnectionQueue> fQueue;

	virtual void cnLib_FUNC QueueStarted(void)override;
	virtual void cnLib_FUNC QueueStopped(void)override;
	virtual void cnLib_FUNC QueueNotify(void)override;

	bool ProcessQueue(void);

	rPtr<iResLock> fSessionListLock;
	cSeqList<rPtr<cHTTPServerSession>> fSessionList;


	virtual void HTTPSessionStarted(cHTTPServerSession *Session)override;
	virtual void HTTPSessionStopped(cHTTPServerSession *Session)override;
	virtual iPtr<iHTTPServerResponder> HTTPCreateResponder(iAddress *ClientAddress,const char *Method,uIntn MethodLength,const char *Path,uIntn PathLength,const char *Version,uIntn VersionLength)override;
		
	rPtr<cHTTPServerSession> QuerySession(void);

};
//---------------------------------------------------------------------------
class cUHTTPManager : protected iMultipointQueueCallback
{
public:
	cUHTTPManager();
	~cUHTTPManager();

	// Reference
	//	reference for owner. referenced when the object is active, and should not be deleted
	iReference *Reference=nullptr;


	class cnLib_INTERFACE iCallback
	{
	public:
		virtual iPtr<iHTTPServerResponder> CreateResponder(iAddress *ClientAddress,const char *Method,uIntn MethodLength,const char *Path,uIntn PathLength,const char *Version,uIntn VersionLength)=0;
		virtual iPtr<iHTTPClientRequestor> UnknownResponse(iAddress *ResponseAddress)=0;
	};
	iCallback *Callback=nullptr;

	bool Start(iMultipointQueue *Port);
	void Stop(void);

	bool StartClient(iAddress *ServerAddress,iHTTPClientRequestor *Requestor,bool NoResponse);
private:
protected:
	iPtr<iMultipointQueue> fPort;

	cAtomicVar<bool> fStopFlag=true;

	virtual void cnLib_FUNC AccessStarted(void)override;
	virtual void cnLib_FUNC AccessStopped(void)override;
	virtual void cnLib_FUNC ReadNotify(void)override;
	virtual void cnLib_FUNC WriteNotify(void)override;

	void ProcessRequest(iAddress *Address,const char *Text,uIntn Length);
	void ProcessResponse(iHTTPClientRequestor *Requestor,const char *Text,uIntn Length);

	struct cResponseItem : cnRTL::cRTLClassAllocator, iHTTPMessageHost
	{
		cResponseItem *Next;

		cUHTTPManager *Manager;
		iPtr<iAddress> Address;
		iPtr<iHTTPServerResponder> Responder;
		bool InQueue=false;
		bool ResponseEnded=false;
		virtual void cnLib_FUNC HTTPTerminate(void)override;
		virtual void cnLib_FUNC HTTPNotifyPull(void)override;
		virtual void cnLib_FUNC HTTPEndPull(bool Shutdown)override;
	};
	cAtomicQueue<cResponseItem> fResponseQueue;
	struct cRequestItem : cnRTL::cRTLClassAllocator, iHTTPMessageHost
	{
		cRequestItem *Next;

		cUHTTPManager *Manager;
		iPtr<iAddress> Address;
		iPtr<iHTTPClientRequestor> Requestor;
		bool InQueue=false;
		bool NoResponse;
		bool RequestEnded=false;

		virtual void cnLib_FUNC HTTPTerminate(void)override;
		virtual void cnLib_FUNC HTTPNotifyPull(void)override;
		virtual void cnLib_FUNC HTTPEndPull(bool Shutdown)override;
	};
	cAtomicQueue<cRequestItem> fResquestQueue;
	cAtomicQueue<cRequestItem> fPendingResquestQueue;


	cSeqMap<cnRTL::iSortablePtr<iAddress>,iPtr<iHTTPClientRequestor>> fPendingRequestMap;
};
#endif // 0
//---------------------------------------------------------------------------
}	// namespace cnNet
//---------------------------------------------------------------------------
}	// namespace cnLibrary
#endif
//---------------------------------------------------------------------------
#endif	// __cplusplus
/*-------------------------------------------------------------------------*/

