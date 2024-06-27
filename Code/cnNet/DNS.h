/*- cnNet - DNS -----------------------------------------------------------*/
/*         Developer : Code009                                             */
/*         Create on : 2018-12-15                                          */
/*-------------------------------------------------------------------------*/
#ifndef	__cnLibrary_cnNet_DNS_H__
#define	__cnLibrary_cnNet_DNS_H__
/*-------------------------------------------------------------------------*/
#include <cnNet/cnProtocol.h>
/*-------------------------------------------------------------------------*/
#ifdef __cplusplus
//---------------------------------------------------------------------------
namespace cnLibrary{
//---------------------------------------------------------------------------
namespace cnRTL{
//---------------------------------------------------------------------------
	/*
template<class TReadBuffer>
class cBinaryReadStream : public cnStream::cBinaryReadStream<TReadBuffer>
{
public:
	using cBinaryReadStream<TReadBuffer>::cBinaryReadStream;

	uIntn Access(void *Dest,uIntn Size){
		Size=this->CheckSize(Size);
		if(Size!=0){
			cnMemory::Copy(Dest,fReadMemory.Pointer,Size);
			this->Advance(Size);
		}
		return Size;
	}

	bool Block(void *Dest,uIntn Size){
		auto ReadPtr=this->Ptr(Size);
		if(ReadPtr==nullptr)
			return false;
		cnMemory::Copy(Dest,ReadPtr,Size);
		this->Advance(Size);
		return true;
	}

};
//---------------------------------------------------------------------------
template<class TWriteBuffer>
class cBinaryWriteStream : public cnStream::cBinaryWriteStream<TWriteBuffer>
{
public:
	using cBinaryWriteStream<TWriteBuffer>::cBinaryWriteStream;
	
	uIntn Access(const void *Dest,uIntn Size){
		Size=this->CheckSize(Size);
		if(CheckSize!=0){
			cnMemory::Copy(Dest,fWriteMemory.Pointer,Size);
		}
		this->Advance(Size);
		return Size;
	}

	bool Block(const void *Src,uIntn Size){
		auto WritePtr=this->Ptr(Size);
		if(WritePtr==nullptr){
			return false;
		}
		cnMemory::Copy(WritePtr,Src,Size);
		this->Advance(Size);
		return true;
	}
};
//---------------------------------------------------------------------------
template<class T,uIntn Length>
inline cBinaryReadStream<cnStream::cStaticBinaryReadBuffer> BinaryReadStream(T (&Pointer)[Length])
{
	return cBinaryReadStream<cnStream::cStaticBinaryReadBuffer>(Pointer,Length*sizeof(T));
}
template<class T>
inline cBinaryReadStream<cnStream::cStaticBinaryReadBuffer> BinaryReadStream(const T *Pointer,uIntn Length){
	return cBinaryReadStream<cnStream::cStaticBinaryReadBuffer>(Pointer,Length*sizeof(T));
}
//---------------------------------------------------------------------------
template<class T,uIntn Length>
inline cBinaryWriteStream<cnStream::cStaticBinaryWriteBuffer> BinaryWriteStream(T (&Pointer)[Length]){
	return cBinaryWriteStream<cnStream::cStaticBinaryWriteBuffer>(Pointer,Length*sizeof(T));
}
template<class T>
inline cBinaryWriteStream<cnStream::cStaticBinaryWriteBuffer> BinaryWriteStream(T *Pointer,uIntn Length){
	return cBinaryWriteStream<cnStream::cStaticBinaryWriteBuffer>(Pointer,Length*sizeof(T));
}
//---------------------------------------------------------------------------
inline cBinaryReadStream<cnStream::cStaticBinaryReadBuffer> BinaryReadStream(const void *Pointer,uIntn Length){
	return cBinaryReadStream<cnStream::cStaticBinaryReadBuffer>(Pointer,Length);
}
inline cBinaryWriteStream<cnStream::cStaticBinaryWriteBuffer> BinaryWriteStream(void *Pointer,uIntn Length){
	return cBinaryWriteStream<cnStream::cStaticBinaryWriteBuffer>(Pointer,Length);
}
*/
//---------------------------------------------------------------------------
}	// namespace cnRTL

#if 0

//---------------------------------------------------------------------------
namespace cnNet{
//---------------------------------------------------------------------------
namespace DNS{
//---------------------------------------------------------------------------
enum class QueryType : ufInt8
{
	Query=0,		// a standard query (QUERY)
	InverseQuery=1,	// an inverse query (IQUERY)
	ServerStatus=2,	// a server status request (STATUS)
};
enum class ResponseCode : ufInt8
{
	OK					=0,		// No error condition
	Format				=1,		// Format error - The name server was unable to interpret the query.
	ServerFailure		=2,		// Server failure - The name server was unable to process this query due to a problem with the name server.
	NameError			=3,		// Name Error - Meaningful only for responses from an authoritative name server, this code signifies that the domain name referenced in the query does not exist.
	NotImplemented		=4,		// Not Implemented - The name server does not support the requested kind of query.
	Refused				=5,		// Refused - The name server refuses to perform the specified operation for policy reasons.
								//	For example, a name server may not wish to provide the information to the particular requester,
								//	or a name server may not wish to perform a particular operation (e.g., zone transfer) for particular data.
};
//---------------------------------------------------------------------------
struct MessageHeader
{
	uInt16 ID;
	uInt8 RD		:1;		// Recursion Desired - this bit may be set in a query and is copied into the response.
							//	If RD is set, it directs the name server to pursue the query recursively. Recursive query support is optional.
	uInt8 TC		:1;		// Specifies that this message was truncated due to length greater than that permitted on the transmission channel.
	uInt8 AA		:1;		// Authoritative Answer - this bit is valid in responses, and specifies that the responding name server is an authority for the domain name in question section.
							//	Note: the contents of the answer section may have multiple owner names because of aliases.
							//	The AA bit corresponds to the name which matches the query name, or the first owner name in the answer section.
	uInt8 OPCode	:4;		// Value from OpCode. Specifies kind of query in this message. This value is set by the originator of a query and copied into the response.
	uInt8 QR		:1;		// Specifies whether this message is a query (0), or a response (1)
	uInt8 RCODE		:4;		// Value from RCode. Response code for responses.
	uInt8 Reserved	:3;		// Zero
	uInt8 RA		:1;		// Recursion Available - this be is set or cleared in a response, and denotes whether recursive query support is available in the name server.
	uInt16 QuestionCount;	// the number of entries in the question section.
	uInt16 AnswerCount;		// the number of resource records in the answer section.
	uInt16 NSCount;			// the number of name server resource records in the authority records section.
	uInt16 ARCount;			// the number of resource records in the additional records section.
};
//---------------------------------------------------------------------------
enum class ItemType : ufInt16
{
	A		=1,		// a host address
	NS		=2,		// an authoritative name server
					// MD=3, a mail destination (Obsolete - use MX)
					// MF=4, a mail forwarder (Obsolete - use MX)
	CNAME	=5,		// the canonical name for an alias
	SOA		=6,		// marks the start of a zone of authority
	MB		=7,		// a mailbox domain name (EXPERIMENTAL)
	MG		=8,		// a mail group member (EXPERIMENTAL)
	MR		=9,		// a mail rename domain name (EXPERIMENTAL)
	Null	=10,	// a null RR (EXPERIMENTAL)
	WKS		=11,	// a well known service description
	PTR		=12,	// a domain name pointer
	HINFO	=13,	// host information
	MINFO	=14,	// mailbox or mail list information
	MX		=15,	// mail exchange
	TXT		=16,	// text strings

// Question Type

	QAXFR	=252,	// A request for a transfer of an entire zone
	QMAILB	=253,	// A request for mailbox-related records (MB, MG or MR)
	QMAILA	=254,	// A request for mail agent RRs (Obsolete - see MX)
	QAll	=255,	// A request for all records
};
enum class ItemClass : ufInt16
{
	INet	=1,		// the Internet
					// CS=2, the CSNET class (Obsolete - used only for examples in some obsolete RFCs)
	CH		=3,		// the CHAOS class
	HS		=4,		// Hesiod [Dyer 87]

// Question Class

	QAny	=255,	// any class

};
//---------------------------------------------------------------------------
struct QuestionInfo{	// packet structure
	uInt16 QType;
	uInt16 QClass;
};
//---------------------------------------------------------------------------
struct ResourceInfo{	// packet structure
	uInt16 RRType;
	uInt16 RRClass;
	uInt32 TTL;
};
//---------------------------------------------------------------------------
//<domain-name> is a domain name represented as a series of labels, and terminated by a label with zero length. compressed domain name can also be terminated by pointer
// domain-name use class cSeqList<cString<char>>

//<character-string> is a single length octet followed by that number of characters.  <character-string> is treated as binary information, and can be up to 256 characters in length (including the length octet).
// character-string use class cString<char>
//---------------------------------------------------------------------------

// general record format

struct cResourceData_CNAME
{
	static constexpr ItemType RType=ItemType::CNAME;

	cSeqList<cString<char>> Name;	// A <domain-name> which specifies the canonical or primary name for the owner.  The owner name is an alias.

};
struct cResourceData_HINFO
{
	static constexpr ItemType RType=ItemType::HINFO;
	// Standard values for CPU and OS can be found in [RFC-1010].

	cString<char> CPU;		// A <character-string> which specifies the CPU type.
	cString<char> OS;		// A <character-string> which specifies the operating system type.
};
struct cResourceData_MB
{
	static constexpr ItemType RType=ItemType::MB;
	cSeqList<cString<char>> MailboxName;	// A <domain-name> which specifies a host which has the specified mailbox.
};
struct cResourceData_MG
{
	static constexpr ItemType RType=ItemType::MG;
	cSeqList<cString<char>> MailboxName;	// A <domain-name> which specifies a mailbox which is a member of the mail group specified by the domain name.
};
struct cResourceData_MINFO
{
	static constexpr ItemType RType=ItemType::MINFO;
	cSeqList<cString<char>> ResponseMailboxName;	// A <domain-name> which specifies a mailbox which is responsible for the mailing list or mailbox. 
	cSeqList<cString<char>> ErrorMailboxName;	// A <domain-name> which specifies a mailbox which is to receive error messages related to the mailing list or mailbox specified by the owner of the MINFO RR (similar to the ERRORS-TO: field which has been proposed).
};
struct cResourceData_MR
{
	static constexpr ItemType RType=ItemType::MR;
	cSeqList<cString<char>> NewMailboxName;		// A <domain-name> which specifies a mailbox which is the proper rename of the specified mailbox.
};
struct cResourceData_MX
{
	static constexpr ItemType RType=ItemType::MX;
	uInt16 Preference;							// A 16 bit integer which specifies the preference given to this RR among others at the same owner.  Lower values are preferred.
	cSeqList<cString<char>> MailExchangeName;	// A <domain-name> which specifies a host willing to act as a mail exchange for the owner name.
};
struct cResourceData_NS
{
	static constexpr ItemType RType=ItemType::NS;
	cSeqList<cString<char>> NS;					//  A <domain-name> which specifies a host which should be authoritative for the specified class and domain.
};
struct cResourceData_PTR
{
	static constexpr ItemType RType=ItemType::PTR;
	cSeqList<cString<char>> PTR;				//  A <domain-name> which points to some location in the domain name space.
};
struct cResourceData_SOA
{
	static constexpr ItemType RType=ItemType::SOA;
	cSeqList<cString<char>> M;				// The <domain-name> of the name server that was the original or primary source of data for this zone.
	cSeqList<cString<char>> R;				// A <domain-name> which specifies the mailbox of the person responsible for this zone.
	uInt32 Serial;							// The unsigned 32 bit version number of the original copy of the zone.  Zone transfers preserve this value.
											// This value wraps and should be compared using sequence space arithmetic.
	uInt32 RefreshInterval;					// A 32 bit time interval before the zone should be refreshed.
	uInt32 Retry;							// A 32 bit time interval that should elapse before a failed refresh should be retried.
	uInt32 Expire;							// A 32 bit time value that specifies the upper limit on the time interval that can elapse before the zone is nolonger authoritative.
	uInt32 MinimumTTL;						// The unsigned 32 bit minimum TTL field that should be exported with any RR from this zone.

	// All times are in units of seconds.
};
struct cResourceData_TXT
{
	static constexpr ItemType RType=ItemType::TXT;
	cSeqList<cString<char>> Text;	// One or more <character-string>s, which are used to hold descriptive text.  The semantics of the text depends on the domain where it is found.
};

// internet protocol record format

struct cResourceData_A
{
	static constexpr ItemType RType=ItemType::A;
	uInt32 IP;
};
struct cResourceData_WKS
{
	static constexpr ItemType RType=ItemType::WKS;
	uInt32 Address;
	uInt8 Protocol;
	cSeqList<bool> ActivePortBitmap;

	// The appropriate values and mnemonics for ports and protocols are specified in [RFC-1010].
};

typedef cnVar::cVariant
<
	cResourceData_CNAME,
	cResourceData_HINFO,
	cResourceData_MB,
	cResourceData_MG,
	cResourceData_MINFO,
	cResourceData_MR,
	cResourceData_MX,
	cResourceData_NS,
	cResourceData_PTR,
	cResourceData_SOA,
	cResourceData_TXT,

	cResourceData_A,
	cResourceData_WKS
> cResourceVariant;
//---------------------------------------------------------------------------
struct cQuestion
{
	cSeqList<cString<char>> DomainName;
	ItemType Type;
	ItemClass Class;
};
struct cResource
{
	cSeqList<cString<char>> DomainName;
	ItemClass Class;
	ufInt32 TimeToLive;
	cResourceVariant Data;
};
struct cMessage
{
	QueryType Operator;
	cSeqList<cQuestion> Questions;
	cSeqList<cResource> Answers;
};
//---------------------------------------------------------------------------
template<class TMemoryOperator>
struct cDNSMessageEncoder
{
public:
	cDNSMessageEncoder(cnDataStruct::cMemoryBuffer<TMemoryOperator> &Buffer);

	void String(const cString<char> &String);

	void DomainName(const cSeqList< cString<char> > &DomainName);

	void Resource(const cResourceData_CNAME &Data);
	void Resource(const cResourceData_HINFO &Data);
	void Resource(const cResourceData_MB &Data);
	void Resource(const cResourceData_MG &Data);
	void Resource(const cResourceData_MINFO &Data);
	void Resource(const cResourceData_MR &Data);
	void Resource(const cResourceData_MX &Data);
	void Resource(const cResourceData_NS &Data);
	void Resource(const cResourceData_PTR &Data);
	void Resource(const cResourceData_SOA &Data);
	void Resource(const cResourceData_TXT &Data);
	void Resource(const cResourceData_A &Data);
	void Resource(const cResourceData_WKS &Data);

	void Message(const cMessage &Message);
private:
	cnDataStruct::cMemoryBuffer<TMemoryOperator> &fMessageBuffer;
	cnRTL::cBinaryWriteStream< cnStream::cMemoryStorageBinaryWriteBuffer<TMemoryOperator> > WriteStream;
};
//---------------------------------------------------------------------------
class cDNSMessageDecoder
{
public:
	cDNSMessageDecoder(const void *Data,uIntn Size);

	void Message(cMessage &Message);

private:
	const void *fMessageData;
	uIntn fMessageSize;

	struct cDomainName : public cnRTL::cRTLAllocator
	{
		cDomainName *Next;
		cString<char> Label;
	};

	cSeqMap<ufInt16,autoPtr<cDomainName> > fNameMap;

	cDomainName* DomainName(cnRTL::cBinaryReadStream<cnStream::cStaticBinaryReadBuffer> &ReadStream);
	bool Resource(cResource &res,cnRTL::cBinaryReadStream<cnStream::cStaticBinaryReadBuffer> &ReadStream);
};
//---------------------------------------------------------------------------
void EncodeMessage(cMemoryBuffer &Buffer,const cMessage &Message);
void DecodeMessage(const void *Data,uIntn Size,cMessage &Message);
//---------------------------------------------------------------------------
class iResolveTask : public iAsyncTask
{
public:
	// GetResponse
	//	get response code for the message
	virtual ResponseCode GetResponse(void)=0;
	// QueryMessage
	//	get message structure
	virtual const cMessage& QueryMessage(void)=0;
	// QueryMessageBinary
	//	get binary form of the DNS message
	virtual cConstMemory QueryMessageBinary(void)=0;
};
//---------------------------------------------------------------------------
class cDNSService
{
public:
	cDNSService();
	~cDNSService();
	// Request
	//	start resolve name in the message
	iPtr<iResolveTask> Request(const cMessage &Message);
	// Request
	//	start resolve name in the binary DNS message
	iPtr<iResolveTask> RequestBuffer(const void *QuestionBuffer,uIntn Size);


	// cRequestTask
	//	DNS resolve task
	class cRequestTask : public iResolveTask
	{
		friend cDNSService;
	public:
		cRequestTask();
		~cRequestTask();

		cRequestTask *Next;

		cMessage Message;

		ResponseCode Response;
		bool Finished;

		cLinkList<cMessage> RecursiveMessage;

		void UpdateBuffer(void);
		cConstMemory GetBuffer(void);
		void SetBuffer(const void *Buffer,uIntn BufferSize);

		// iResolveTask
	
		virtual bool cnLib_FUNC IsDone(void)override;
		virtual bool cnLib_FUNC SetNotify(iProcedure *NotifyProcedure)override;
		virtual ResponseCode GetResponse(void)override;
		virtual const cMessage& QueryMessage(void)override;
		virtual cConstMemory QueryMessageBinary(void)override;
	private:
		cnRTL::cAsyncTaskState fTaskState;
		uIntn ResolverIndex;
		

		cMemoryBuffer fMessageBuffer;

		iPtr<cRequestTask> fRecursiveParent;
		cAtomicVar<ufInt16> fRecursiveCount;
	};

	// cResolver
	//	base class for name resolver
	class cResolver
	{
		friend cDNSService;
	public:
		cResolver();
		~cResolver();

	protected:

		// TaskStart
		//	retrive request task for resolving
		iPtr<cRequestTask> TaskStart(void);
		// TaskEnd
		//	mark a request as completed
		void TaskEnd(iPtr<cRequestTask> Task);

		// TaskNotify
		//	notify when there is task in queue
		virtual void TaskNotify(void)=0;

	private:

		cAtomicQueueS<cRequestTask> fSendQueue;	// Task queue
		cDNSService *Owner;	// Owner

		// enqueue task
		void DNSQueueTask(cRequestTask *Task);
	};

	void InsertResolver(cResolver *Resolver);
	void RemoveResolver(cResolver *Resolver);

private:

	cSeqList<cResolver*> fResolverList;

	void StartRequest(cRequestTask *Task);
	void PassRequest(cRequestTask *Task);
	void ResolverEndTask(cRequestTask *Task);
	void ResolverTerminateTask(cRequestTask *Task);

	void NotifyTaskEnd(cRequestTask *Task);
};
//---------------------------------------------------------------------------
class cUDPClient : public cDNSService::cResolver, private iProtocolProcessor
{
public:
	// Start
	//	start protocol
	bool Start(iProtocolHost *ProtocolHost);
	// Stop
	//	stop protocol
	void Stop(void);

	// Reference
	//	reference for owner. referenced when the object is active, and should not be deleted
	iReference *Reference;


	cUDPClient();
	~cUDPClient();
private:

	virtual void TaskNotify(void)override;

	rPtr<iProtocolHost> fProtocolHost;
	virtual uIntn InputPush(const void *Buffer,uIntn BufferSize)override;
	virtual void InputClose(bool GracefulClose)override;

	virtual uIntn OutputPull(void *Buffer,uIntn BufferSize)override;
	virtual void OutputClose(bool GracefulClose)override;

	void HostClose(void);
	cAtomicVar<uInt8> fHostRef;

	uInt16 fRequestID=0;
	cSeqMap<uInt16,iPtr<cDNSService::cRequestTask> > fRequestMap;
	rPtr<iMutex> fRequestMapLock;

	void CancelRequests(void);
};
//---------------------------------------------------------------------------

// cTCPDNSPacket
//	protocol stack for packing DNS message in tcp
class cTCPDNSPacket : public iProtocolHost, private iProtocolProcessor
{
public:
	cTCPDNSPacket();

	// SetProtocolHost
	//	set protocol host
	bool SetProtocolHost(iProtocolHost *ProtocolHost);

	// ConnectionReset
	//	reset state for reconnected protocol host
	void ConnectionReset(void);

	virtual bool StartProtocol(iProtocolProcessor *ProtocolProcessor)override;
	virtual void NotifyInput(void)override;
	virtual void TerminateInput(void)override;
	virtual void NotifyOutput(void)override;
	virtual void SetEndOfOutput(void)override;
	virtual void TerminateOutput(void)override;

private:
	iProtocolProcessor *fDownProtocolProcessor=nullptr;

	rPtr<iProtocolHost> fUpProtocolHost;
	virtual uIntn InputPush(const void *Buffer,uIntn BufferSize)override;
	virtual void InputClose(bool GracefulClose)override;

	virtual uIntn OutputPull(void *Buffer,uIntn BufferSize)override;
	virtual void OutputClose(bool GracefulClose)override;

	void StartProcess(void);

	cnRTL::cMemoryBufferQueueReadBuffer fRecvBufferPacker;
	uInt16 fRecvSize=0;
};
// cUDPPortManager
//	Manage UDP port for DNS service
class cUDPPortManager : protected iMultipointQueueCallback
{
public:
	// Reference
	//	reference for owner. referenced when the object is active, and should not be deleted
	iReference *Reference=nullptr;

	// Start
	//	start protocol
	bool Start(iMultipointQueue *Datagram);
	// Stop
	//	Stop protocol
	void Stop(void);

	// DNSService
	//	DNS service provider
	cnNet::DNS::cDNSService *DNSService=nullptr;

protected:
	iPtr<iMultipointQueue> fDatagram;
	bool fStopQueue;

	virtual void cnLib_FUNC AccessStarted(void)override;
	virtual void cnLib_FUNC AccessStopped(void)override;
	virtual void cnLib_FUNC ReadNotify(void)override;
	virtual void cnLib_FUNC WriteNotify(void)override;
private:

	class cUDPMessage : public iProcedure
	{
	public:
		cUDPPortManager *Owner;
		cUDPMessage *Next;
		iPtr<iAddress> Address;
		iPtr<iResolveTask> Task;
		uInt16 ID;
		virtual void cnLib_FUNC Execute(void)override;
	};

	cUDPMessage* AcquireMessage(void);
	void ReleaseMessage(cUDPMessage *Message);

	cAtomicQueueS<cUDPMessage> fResponseQueue;

	cnRTL::cGlobalLinkItemRecycler<cUDPMessage> fMessageRecycler;

};
//---------------------------------------------------------------------------
}	// namespace DNS
//---------------------------------------------------------------------------
}	// namespace cnNet
#endif // 0
//---------------------------------------------------------------------------
}	// namespace cnLibrary
//---------------------------------------------------------------------------
#endif	// __cplusplus
/*-------------------------------------------------------------------------*/
#endif

