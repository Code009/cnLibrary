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
URIPart ParseURI(const char *URL,uIntn Length);
//---------------------------------------------------------------------------
bool ParseLine(uIntn &LineLength,const void *Buffer,uIntn BufferSize);
uIntn ParseHeaderMessage(ufInt16 *Pos,ufInt16 *Len,uIntn PosCount,const char *Line,uIntn LineLength);
bool ParseHeaderParameter(ufInt16 &NameLen,const char *Line,uIntn LineLength);
void TrimLWS(ufInt16 &Pos,ufInt16 &Len,const char *Text,uIntn Length);
//---------------------------------------------------------------------------
void AppendDate(cStringBuffer<char> &DateString,const iTimepoint *Time);
//---------------------------------------------------------------------------
class cnLib_INTERFACE iClientProcessor
{
public:
	virtual void HTTPRequestStart(cArray<const char> &Method,cArray<const char> &Path,cArray<const char> &Version)=0;
	virtual bool HTTPRequestHeader(cArray<const char> &Name,cArray<const char> &Value)=0;
	virtual uIntn HTTPRequestBody(void *Buffer,uIntn BufferSize)=0;
	
	virtual void HTTPResponseStatus(const char *Version,uIntn VersionLength,const char *Status,uIntn StatusLength,const char *StatusString,uIntn StatusStringLength)=0;
	virtual void HTTPResponseHeader(const char *Name,uIntn NameLength,const char *Value,uIntn ValueLength)=0;
	virtual void HTTPResponseHeaderFinish(uIntn &ExpectedBodySize)=0;
	virtual uIntn HTTPResponseBody(const void *Buffer,uIntn Size)=0;
	virtual void HTTPResponseBodyFinish(void)=0;

	virtual void HTTPTerminated(void)=0;
};
//---------------------------------------------------------------------------
class cnLib_INTERFACE iClientHost : public iReference
{
public:
	virtual bool HTTPStart(iClientProcessor *ClientProcessor)=0;
	virtual void HTTPTerminate(void)=0;
	virtual void HTTPNotifySendRequest(void)=0;
	virtual void HTTPCompleteRequest(void)=0;
	virtual void HTTPNotifyReceiveResponse(void)=0;
};
//---------------------------------------------------------------------------
class cClientSession : public cnRTL::cDualReference, public iReference, private iProtocolProcessor
{
public:
	cClientSession();
	~cClientSession();
	
	bool SetProtocolHost(iProtocolProvider *Provider);

	rPtr<iClientHost> CreateHost(void);

protected:
	void VirtualStopped(void);

	class cClientHost : public iClientHost
	{
	public:
		cClientHost *Next;

		cClientHost(cClientSession *Session);

		virtual bool HTTPStart(iClientProcessor *ClientProcessor)override;
		virtual void HTTPTerminate(void)override;
		virtual void HTTPNotifySendRequest(void)override;
		virtual void HTTPCompleteRequest(void)override;
		virtual void HTTPNotifyReceiveResponse(void)override;

		iClientProcessor *ClientProcessor=nullptr;

		bool RequestCompleted;
	private:
		rPtr<cClientSession> fSession;
	};

	void MessageEnqueue(cClientHost *ClientHost);
	void MessageTerminate(cClientHost *ClientHost);
	void MessageNotifySendRequest(cClientHost *ClientHost);
	void MessageCompleteRequest(cClientHost *ClientHost);
	void MessageNotifyReceiveResponse(cClientHost *ClientHost);
private:
	rPtr<iProtocolProvider> fProtocolProvider;

	virtual void ProtocolStarted(void)override;
	virtual void ProtocolStopped(void)override;
	virtual uIntn ProtocolInputPush(const void *Buffer,uIntn BufferSize,uIntn &LeastSizeNeeded)override;
	virtual uIntn ProtocolOutputPull(void *Buffer,uIntn BufferSize,uIntn &LeastSizeNeeded)override;
	virtual void ProtocolInputEnd(void)override;
	virtual void ProtocolOutputEnd(void)override;

	cAtomicVar<uInt8> fHostChannelRefCount=0;
	void HostChannelRelease(void);

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

	bool PullHeader(void);

	// response
	uIntn fExpectedResponseBodySize;

	const char* ParseResponseHeaderLine(uIntn &LineLength);
	void ProcessReponseHeaderLine(const char *Line,uIntn LineSize);

	void ProcessInputHeader(uIntn FirstLineLength);
	void ProcessInputBody(void);
	
	cnRTL::cMemoryBufferQueueReadBuffer fInputBufferReader;

};
//---------------------------------------------------------------------------
class cnLib_INTERFACE iServerMessageHost
{
public:
	virtual void HTTPTerminate(void)=0;
	virtual void HTTPNotifyReceiveRequest(void)=0;
	virtual void HTTPNotifySendResponse(void)=0;
	virtual void HTTPCompleteResponse(void)=0;
};
//---------------------------------------------------------------------------
class cnLib_INTERFACE iServerMessageProcessor
{
public:
	virtual uIntn HTTPRequestBody(const void *Buffer,uIntn Size)=0;
	virtual void HTTPRequestBodyFinish(void)=0;

	virtual void HTTPResponseStatus(cArray<const char> &Version,cArray<const char> &Status,cArray<const char> &StatusString)=0;
	virtual bool HTTPResponseHeader(cArray<const char> &Name,cArray<const char> &Value)=0;
	virtual uIntn HTTPResponseBody(void *Buffer,uIntn Size)=0;
	virtual void HTTPResponseFinish(void)=0;
	virtual void HTTPTerminated(void)=0;
};
//---------------------------------------------------------------------------
class cServerSession : public iReference, private iProtocolProcessor
{
public:
	cServerSession();
	~cServerSession();

	bool Start(iProtocolProvider *Provider);
	void Stop(void);

protected:
	virtual void HTTPRequestStart(const char *Method,uIntn MethodLength,const char *Path,uIntn PathLength,const char *Version,uIntn VersionLength)=0;
	virtual void HTTPRequestHeader(const char *Name,uIntn NameLength,const char *Value,uIntn ValueLength)=0;
	virtual iServerMessageProcessor* HTTPRequestHeaderFinish(iServerMessageHost *Host,uIntn &ExpectedRequestBodySize)=0;

private:

	class cMessage : public iServerMessageHost,public cnRTL::cRTLAllocator
	{
	public:
		cMessage *Next;

		cMessage(cServerSession *Session);

		virtual void HTTPTerminate(void)override;
		virtual void HTTPNotifyReceiveRequest(void)override;
		virtual void HTTPNotifySendResponse(void)override;
		virtual void HTTPCompleteResponse(void)override;

		iServerMessageProcessor *Processor=nullptr;

		bool ResponseCompleted;
	private:
		rPtr<cServerSession> fSession;
	};

	void MessageTerminate(cMessage *Message);
	void MessageNotifyReceiveRequest(cMessage *Message);
	void MessageNotifySendResponse(cMessage *Message);
	void MessageCompleteResponse(cMessage *Message);
private:
	rPtr<iProtocolProvider> fProtocolPovider;

	virtual void ProtocolStarted(void)override;
	virtual void ProtocolStopped(void)override;
	virtual uIntn ProtocolInputPush(const void *Buffer,uIntn BufferSize,uIntn &LeastSizeNeeded)override;
	virtual uIntn ProtocolOutputPull(void *Buffer,uIntn BufferSize,uIntn &LeastSizeNeeded)override;
	virtual void ProtocolInputEnd(void)override;
	virtual void ProtocolOutputEnd(void)override;

	cAtomicVar<uInt8> fHostChannelRefCount=0;
	void HostChannelRelease(void);

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

	const char* ParseRequestHeaderLine(uIntn &LineLength);
	void ProcessRequestHeaderLine(const char *Line,uIntn LineSize);

	void ProcessInputHeader(uIntn FirstLineLength);
	void ProcessInputBody(void);

	// response

	cAtomicVar<cMessage*> fPendingResponse;
	cMessage *fSendingMessage;
	

	cMemoryBuffer fSendBuffer;

	bool PullHeader(void);
};
//---------------------------------------------------------------------------
cnLib_ENUM_BEGIN(ufInt8,ClientQueryState)
	Idle,
	Connecting,
	Requesting,
	Sending,
	WaitingResponse,
	ReceiveHeader,
	ReceiveBody,
	Done
cnLib_ENUM_END(ClientQueryState);
//---------------------------------------------------------------------------
class cClientQuery : private iClientProcessor
{
public:
	cClientQuery();
	~cClientQuery();
	

	bool Start(iReference *Reference,iClientHost *ClientHost);
	bool Finish(void);

	eClientQueryState GetState(void);

	const cString<char>& GetHost(void);

	bool SetURL(const char *URL);
	void SetMethod(cString<char> Method);
	void SetHeader(cString<char> Name,cString<char> Value);
	void SetRequestInput(iReadBuffer<void> *ReadBuffer);
	void SetResponseOutput(iWriteBuffer<void> *WriteBuffer);

	int GetResponseStatus(void);
	const cString<char>& GetResponseStatusString(void);

	cnRTL::cFunction<void (const char *Name,uIntn NameLength,const char *Value,uIntn ValueLength)> OnResponseHeader;
	cnRTL::cFunction<void (void)> OnStateChange;
	cnRTL::cFunction<void (void)> OnCompleted;
	
protected:
private:

	rPtr<iReference> fHostReference;
	eClientQueryState fState;

	void CallEvent(cnRTL::cFunction<void (void)> &Event);
	void SetState(eClientQueryState State);

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

	virtual void HTTPRequestStart(cArray<const char> &Method,cArray<const char> &Path,cArray<const char> &Version)override;
	virtual bool HTTPRequestHeader(cArray<const char> &Name,cArray<const char> &Value)override;
	virtual uIntn HTTPRequestBody(void *Buffer,uIntn BufferSize)override;
	virtual void HTTPResponseStatus(const char *Version,uIntn VersionLength,const char *Status,uIntn StatusLength,const char *StatusString,uIntn StatusStringLength)override;
	virtual void HTTPResponseHeader(const char *Name,uIntn NameLength,const char *Value,uIntn ValueLength)override;
	virtual void HTTPResponseHeaderFinish(uIntn &ExpectedBodySize)override;
	virtual uIntn HTTPResponseBody(const void *Buffer,uIntn Size)override;
	virtual void HTTPResponseBodyFinish(void)override;
	virtual void HTTPTerminated(void)override;
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

