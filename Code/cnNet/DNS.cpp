#include "DNS.h"

#if 0

using namespace cnLibrary;
using namespace cnRTL;
using namespace cnNet;
using namespace DNS;

#if 1

//---------------------------------------------------------------------------
#if 0


#ifdef	_DEBUG
#define	DCU_DEBUG_TEXT_LEVEL	5
#else
#define	DCU_DEBUG_TEXT_LEVEL	0
#endif

#define	DCU_DEBUG_TEXT(x)	( SelectDebugOutput<x<=DCU_DEBUG_TEXT_LEVEL>() )

//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
#endif // 0

//---------------------------------------------------------------------------
template<class TMemoryOperator>
cDNSMessageEncoder<TMemoryOperator>::cDNSMessageEncoder(cnDataStruct::cMemoryBuffer<TMemoryOperator> &Buffer)
	:fMessageBuffer(Buffer),WriteStream(Buffer.Storage(),0)
{
}
//---------------------------------------------------------------------------
template<class TMemoryOperator>
void cDNSMessageEncoder<TMemoryOperator>::String(const cString<char> &String)
{
	auto Length=String.GetLength();
	WriteStream.Value(static_cast<uInt8>(Length));

	WriteStream.Block(String->Pointer,Length);
}
//---------------------------------------------------------------------------
template<class TMemoryOperator>
void cDNSMessageEncoder<TMemoryOperator>::DomainName(const cSeqList< cString<char> > &DomainName)
{
	for(auto &Name : DomainName){
		auto Length=Name.GetLength();
		WriteStream.Value(static_cast<uInt8>(Length));

		WriteStream.Block(Name->Pointer,Length);
	}
	WriteStream.Value<uInt8>(0);
}
//---------------------------------------------------------------------------
template<class TMemoryOperator>
void cDNSMessageEncoder<TMemoryOperator>::Resource(const cResourceData_CNAME &Data)
{
	DomainName(Data.Name);
}
//---------------------------------------------------------------------------
template<class TMemoryOperator>
void cDNSMessageEncoder<TMemoryOperator>::Resource(const cResourceData_HINFO &Data)
{
	String(Data.CPU);
	String(Data.OS);
}
//---------------------------------------------------------------------------
template<class TMemoryOperator>
void cDNSMessageEncoder<TMemoryOperator>::Resource(const cResourceData_MB &Data)
{
	DomainName(Data.MailboxName);
}
//---------------------------------------------------------------------------
template<class TMemoryOperator>
void cDNSMessageEncoder<TMemoryOperator>::Resource(const cResourceData_MG &Data)
{
	DomainName(Data.MailboxName);
}
//---------------------------------------------------------------------------
template<class TMemoryOperator>
void cDNSMessageEncoder<TMemoryOperator>::Resource(const cResourceData_MINFO &Data)
{
	DomainName(Data.ResponseMailboxName);
	DomainName(Data.ErrorMailboxName);
}
//---------------------------------------------------------------------------
template<class TMemoryOperator>
void cDNSMessageEncoder<TMemoryOperator>::Resource(const cResourceData_MR &Data)
{
	DomainName(Data.NewMailboxName);
}
//---------------------------------------------------------------------------
template<class TMemoryOperator>
void cDNSMessageEncoder<TMemoryOperator>::Resource(const cResourceData_MX &Data)
{
	DomainName(Data.MailExchangeName);
}
//---------------------------------------------------------------------------
template<class TMemoryOperator>
void cDNSMessageEncoder<TMemoryOperator>::Resource(const cResourceData_NS &Data)
{
	DomainName(Data.NS);
}
//---------------------------------------------------------------------------
template<class TMemoryOperator>
void cDNSMessageEncoder<TMemoryOperator>::Resource(const cResourceData_PTR &Data)
{
	DomainName(Data.PTR);
}
//---------------------------------------------------------------------------
template<class TMemoryOperator>
void cDNSMessageEncoder<TMemoryOperator>::Resource(const cResourceData_SOA &Data)
{
	DomainName(Data.M);
	DomainName(Data.R);

	{
		
		auto pData=WriteStream.Ptr<uInt32>(5);
		pData[0]=Data.Serial;
		pData[1]=Data.RefreshInterval;
		pData[2]=Data.Retry;
		pData[3]=Data.Expire;
		pData[4]=Data.MinimumTTL;
		WriteStream.Advance(pData,5);
	}
}
//---------------------------------------------------------------------------
template<class TMemoryOperator>
void cDNSMessageEncoder<TMemoryOperator>::Resource(const cResourceData_TXT &Data)
{
	for(auto &TextLine : Data.Text){
		String(TextLine);
	}
}
//---------------------------------------------------------------------------
template<class TMemoryOperator>
void cDNSMessageEncoder<TMemoryOperator>::Resource(const cResourceData_A &Data)
{
	WriteStream.Value<uInt32>(cnMemory::SwapBigEndian(Data.IP));
}
//---------------------------------------------------------------------------
template<class TMemoryOperator>
void cDNSMessageEncoder<TMemoryOperator>::Resource(const cResourceData_WKS &Data)
{
	WriteStream.Value<uInt32>(cnMemory::SwapBigEndian(Data.Address));
	WriteStream.Value<uInt8>(Data.Protocol);
}
//---------------------------------------------------------------------------
template<class TMemoryOperator>
void cDNSMessageEncoder<TMemoryOperator>::Message(const cMessage &Message)
{
	auto *Header=WriteStream.Ptr<MessageHeader>();
	Header->ID=0;
	Header->RD=false;
	Header->TC=false;
	Header->AA=false;
	Header->OPCode=static_cast<uInt8>(Message.Operator);
	Header->QR=false;
	Header->RCODE=0;
	Header->Reserved=0;
	Header->RA=false;

	Header->QuestionCount=cnMemory::SwapBigEndian(static_cast<uInt16>(Message.Questions.GetCount()));
	Header->AnswerCount=cnMemory::SwapBigEndian(static_cast<uInt16>(Message.Answers.GetCount()));
	Header->ARCount=0;
	Header->NSCount=0;

	WriteStream.Advance<MessageHeader>();


	
	for(uIntn qi=0,qc=Message.Questions.GetCount();qi<qc;qi++){
		auto &q=Message.Questions[qi];
		DomainName(q.DomainName);

		auto *pInfo=WriteStream.Ptr<QuestionInfo>();
		pInfo->QClass=cnMemory::SwapBigEndian(static_cast<uInt16>(q.Class));
		pInfo->QType=cnMemory::SwapBigEndian(static_cast<uInt16>(q.Type));
		WriteStream.Advance<QuestionInfo>();
	}
	for(uIntn ai=0,ac=Message.Answers.GetCount();ai<ac;ai++){
		auto &a=Message.Answers[ai];
		DomainName(a.DomainName);

		uIntn ResOffset=fMessageBuffer.GetSize();

		auto *rInfo=WriteStream.Ptr<ResourceInfo>();
		rInfo->RRClass=cnMemory::SwapBigEndian(static_cast<uInt16>(a.Class));
		uInt32 TTL=cnMemory::SwapBigEndian(a.TimeToLive);
		rInfo->TTL=TTL;

		WriteStream.Advance(sizeof(ResourceInfo)+sizeof(uInt16));

		a.Data.CallByType([this,ResOffset](const auto &Resource){
			auto pResInfo=static_cast<ResourceInfo*>(fMessageBuffer.GetMemory(ResOffset));
			pResInfo->RRType=cnMemory::SwapBigEndian(static_cast<uInt16>(Resource.RType));

			this->Resource(Resource);
		});

		uIntn DataLength=fMessageBuffer.GetSize();
		DataLength-=ResOffset+sizeof(ResourceInfo)+sizeof(uInt16);
		auto pRDLength=static_cast<uInt16*>(fMessageBuffer.GetMemory(ResOffset+sizeof(ResourceInfo)));
		*pRDLength=cnMemory::SwapBigEndian(static_cast<uInt16>(DataLength));

	}
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cDNSMessageDecoder::cDNSMessageDecoder(const void *Data,uIntn Size)
{
	fMessageData=Data;
	fMessageSize=Size;
}
//---------------------------------------------------------------------------
cDNSMessageDecoder::cDomainName* cDNSMessageDecoder::DomainName(cnRTL::cBinaryReadStream<cnStream::cStaticBinaryReadBuffer> &ReadStream)
{
	cDomainName Root;
	Root.Next=nullptr;

	cDomainName *LastDomainName=&Root;
	for(;;){
		auto *pLength=ReadStream.Ptr<char>();
		if(pLength==nullptr)
			return nullptr;
		ufInt16 CurOffset=static_cast<ufInt16>(pLength-static_cast<const char*>(fMessageData));
		{
			auto pName=fNameMap.GetPair(CurOffset);
			if(pName!=nullptr){
				LastDomainName->Next=pName->Value.Pointer();
				break;
			}
		}
		ufInt8 Length=*reinterpret_cast<const uInt8*>(pLength);
		ReadStream.Advance(1);

		ufInt8 LengthType=Length>>6;
		if(LengthType==3){
			uInt8 OffsetLo;
			if(ReadStream.Value(OffsetLo)==false){
				// terminate domain name
				break;
			}
			ufInt16 Offset=Length&0x3F;
			Offset<<=8;
			Offset|=OffsetLo;

			auto LabelReadStream=cnRTL::BinaryReadStream(cnMemory::PointerAddByteOffset(fMessageData,Offset),fMessageSize-Offset);
			LastDomainName->Next=DomainName(LabelReadStream);
			// end of domain name
			break;
		}
		if(LengthType==0){
			if(Length==0){
				// end of domain name
				break;
			}
			// normal string
			Length&=0x3F;
			
			auto Name=cnVar::AutoPtrCreate<cDomainName>();
			cStringBuffer<char> StrBuf;
			StrBuf.SetLength(Length);
			if(ReadStream.Block(StrBuf.GetString(),Length)==false){
				// terminate domain name
				break;
			}

			Name->Label=cnVar::MoveCast(StrBuf);
			Name->Next=nullptr;
			LastDomainName->Next=Name;

			LastDomainName=Name;
			fNameMap[CurOffset]=cnVar::MoveCast(Name);
		}
		else{
			// not supported name
			break;
		}
	}
	return Root.Next;
}
//---------------------------------------------------------------------------
bool cDNSMessageDecoder::Resource(cResource &res,cnRTL::cBinaryReadStream<cnStream::cStaticBinaryReadBuffer> &ReadStream)
{
	{	// domain name
		auto NamePtr=DomainName(ReadStream);
		while(NamePtr!=nullptr){
			res.DomainName.AppendMake(NamePtr->Label);
			NamePtr=NamePtr->Next;
		}
	}
	auto Info=ReadStream.Ptr<ResourceInfo>();
	if(Info==nullptr){
		// incompleted resource
		res.Class=ItemClass::QAny;
		res.TimeToLive=0;
		return false;
	}
	res.Class=static_cast<ItemClass>(cnMemory::SwapBigEndian(Info->RRClass));
	res.TimeToLive=cnMemory::SwapBigEndian(Info->TTL);

	auto ResType=static_cast<ItemType>(cnMemory::SwapBigEndian(Info->RRType));

	ReadStream.Advance<ResourceInfo>();

	uInt16 DataLength;
	if(ReadStream.Value(DataLength)==false)
		return false;
	DataLength=cnMemory::SwapBigEndian(DataLength);
	auto pData=ReadStream.Ptr(DataLength);
	if(pData==nullptr)
		return false;

	switch(ResType){
	case ItemType::A:
		res.Data.SetType<cResourceData_A>();
		{
			auto &Addr=res.Data.Ref<cResourceData_A>();
			Addr.IP=cnMemory::SwapBigEndian(*static_cast<const uInt32*>(pData));
		}
		break;
	}
	ReadStream.Advance(DataLength);
	return true;
}
//---------------------------------------------------------------------------
void cDNSMessageDecoder::Message(cMessage &Message)
{
	auto ReadStream=cnRTL::BinaryReadStream(fMessageData,fMessageSize);

	auto *Header=ReadStream.Ptr<MessageHeader>();
	if(Header==nullptr){
		return;
	}

	Message.Operator=static_cast<QueryType>(Header->OPCode);

	ufInt16 QuestionCount=cnMemory::SwapBigEndian(Header->QuestionCount);
	ufInt16 AnswerCount=cnMemory::SwapBigEndian(Header->AnswerCount);
	//uInt16 NSCount=cnMemory::SwapBigEndian(Header->NSCount);
	//uInt16 ARCount=cnMemory::SwapBigEndian(Header->ARCount);

	ReadStream.Advance(Header);

	// questions
	for(ufInt16 qi=0;qi<QuestionCount;qi++){
		cQuestion q;
		{	// domain name
			auto NamePtr=DomainName(ReadStream);
			while(NamePtr!=nullptr){
				q.DomainName.AppendMake(NamePtr->Label);
				NamePtr=NamePtr->Next;
			}
		}

		// question info
		auto Info=ReadStream.Ptr<QuestionInfo>();
		if(Info==nullptr)
			return;
			
		q.Class=static_cast<ItemClass>(cnMemory::SwapBigEndian(Info->QClass));
		q.Type=static_cast<ItemType>(cnMemory::SwapBigEndian(Info->QType));

		ReadStream.Advance<QuestionInfo>();
	
		Message.Questions.AppendMake(cnVar::MoveCast(q));
	}
	// answers
	for(ufInt16 ai=0;ai<AnswerCount;ai++){
		cResource res;
		if(Resource(res,ReadStream)==false)
			return;
		Message.Answers.AppendMake(cnVar::MoveCast(res));
	}
}
//---------------------------------------------------------------------------
void DNS::EncodeMessage(cMemoryBuffer &Buffer,const cMessage &Message)
{
	cDNSMessageEncoder< cMemoryOperator<void> > Encoder(Buffer);
	Encoder.Message(Message);
}
//---------------------------------------------------------------------------
void DNS::DecodeMessage(const void *Data,uIntn Size,cMessage &Message)
{
	cDNSMessageDecoder Decoder(Data,Size);
	Decoder.Message(Message);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cDNSService::cRequestTask::cRequestTask()
{
}
//---------------------------------------------------------------------------
cDNSService::cRequestTask::~cRequestTask()
{
}
//---------------------------------------------------------------------------
bool cDNSService::cRequestTask::IsDone(void)
{
	return fTaskState.IsDone();
}
//---------------------------------------------------------------------------
bool cDNSService::cRequestTask::SetNotify(iProcedure *NotifyProcedure)
{
	return fTaskState.SetNotify(NotifyProcedure);
}
//---------------------------------------------------------------------------
ResponseCode cDNSService::cRequestTask::GetResponse(void)
{
	return Response;
}
//---------------------------------------------------------------------------
const cMessage& cDNSService::cRequestTask::QueryMessage(void)
{
	return Message;
}
//---------------------------------------------------------------------------
cConstMemory cDNSService::cRequestTask::QueryMessageBinary(void)
{
	return GetBuffer();
}
//---------------------------------------------------------------------------
void cDNSService::cRequestTask::UpdateBuffer(void)
{
	fMessageBuffer.Clear();
}
//---------------------------------------------------------------------------
cConstMemory cDNSService::cRequestTask::GetBuffer(void)
{
	if(fMessageBuffer->Length==0){
		// encode to buffer
		EncodeMessage(fMessageBuffer,Message);
	}
	return fMessageBuffer.Storage();
}
//---------------------------------------------------------------------------
void cDNSService::cRequestTask::SetBuffer(const void *Buffer,uIntn BufferSize)
{
	fMessageBuffer.Clear();
	fMessageBuffer.Copy(Buffer,BufferSize);

	// decode to message
	DecodeMessage(fMessageBuffer->Pointer,fMessageBuffer->Length,Message);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cDNSService::cResolver::cResolver()
{
}
//---------------------------------------------------------------------------
cDNSService::cResolver::~cResolver()
{
	auto Task=fSendQueue.DequeueAll();
	while(Task!=nullptr){

		auto CurTask=Task;
		Task=Task->Next;

		Owner->ResolverTerminateTask(CurTask);
		iDecReference(CurTask,'queu');
	}
}
//---------------------------------------------------------------------------
iPtr<cDNSService::cRequestTask> cDNSService::cResolver::TaskStart(void)
{
	auto p=fSendQueue.Dequeue();
	if(p==nullptr)
		return nullptr;

	return iAttach(p,'queu');
}
//---------------------------------------------------------------------------
void cDNSService::cResolver::DNSQueueTask(cRequestTask *Task)
{
	iIncReference(Task,'queu');
	fSendQueue.Enqueue(Task);
}
//---------------------------------------------------------------------------
void cDNSService::cResolver::TaskEnd(iPtr<cRequestTask> Task)
{
	Owner->ResolverEndTask(Task);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cDNSService::cDNSService()
{
}
//---------------------------------------------------------------------------
cDNSService::~cDNSService()
{
}
//---------------------------------------------------------------------------
void cDNSService::InsertResolver(cResolver *Resolver)
{
	Resolver->Owner=this;
	fResolverList.AppendMake(Resolver);
}
void cDNSService::RemoveResolver(cResolver *Resolver)
{
	for(uIntn i=0,c=fResolverList.GetCount();i<c;i++){
		if(fResolverList[i]==Resolver){
			fResolverList.RemoveAt(i);
			Resolver->Owner=nullptr;
			return;
		}
	}
}
//---------------------------------------------------------------------------
void cDNSService::ResolverEndTask(cRequestTask *Task)
{
	if(Task->Finished){
		if(Task->RecursiveMessage.GetCount()!=0){

			Task->fRecursiveCount=static_cast<ufInt16>(Task->RecursiveMessage.GetCount());
			for(auto &Message : Task->RecursiveMessage){
				auto RecursiveTask=iCreate<cRequestTask>();
				RecursiveTask->Message=Message;

				RecursiveTask->fRecursiveParent=Task;
				StartRequest(RecursiveTask);
			}
			return;
		}
		NotifyTaskEnd(Task);
		return;
	}
	// continue search
	PassRequest(Task);
}
//---------------------------------------------------------------------------
void cDNSService::NotifyTaskEnd(cRequestTask *Task)
{
	if(Task->fRecursiveParent!=nullptr){
		// merge answers
		for(auto &Ans : Task->Message.Answers){
			Task->fRecursiveParent->Message.Answers.AppendMake(Ans);
		}
		Task->fRecursiveParent->UpdateBuffer();
		if(Task->fRecursiveParent->fRecursiveCount.Free--==1){
			NotifyTaskEnd(Task->fRecursiveParent);
		}
	}
	Task->fTaskState.SetDone();
}
//---------------------------------------------------------------------------
void cDNSService::ResolverTerminateTask(cRequestTask *Task)
{
	NotifyTaskEnd(Task);
}
//---------------------------------------------------------------------------
void cDNSService::StartRequest(cRequestTask *Task)
{
	Task->Response=ResponseCode::NameError;
	Task->Finished=false;
	Task->ResolverIndex=0;
	PassRequest(Task);
}
//---------------------------------------------------------------------------
void cDNSService::PassRequest(cRequestTask *Task)
{
	if(Task->ResolverIndex>=fResolverList.GetCount()){
		// no record found
		Task->Response=ResponseCode::NameError;
		NotifyTaskEnd(Task);
		return;
	}
	auto Resolver=fResolverList[Task->ResolverIndex];
	Task->ResolverIndex++;
	Resolver->DNSQueueTask(Task);
	Resolver->TaskNotify();
}
//---------------------------------------------------------------------------
iPtr<iResolveTask> cDNSService::Request(const cMessage &Message)
{
	auto Task=iCreate<cRequestTask>();
	Task->Message=Message;

	StartRequest(Task);
	return Task;
}
//---------------------------------------------------------------------------
iPtr<iResolveTask> cDNSService::RequestBuffer(const void *QuestionBuffer,uIntn Size)
{
	auto Task=iCreate<cRequestTask>();
	Task->SetBuffer(QuestionBuffer,Size);
	StartRequest(Task);
	return Task;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cUDPClient::cUDPClient()
{
	fRequestMapLock=cnSystem::CreateRecursiveLock();
}
//---------------------------------------------------------------------------
cUDPClient::~cUDPClient()
{
	cnLib_ASSERT(fRequestMap.GetCount()==0);
}
//---------------------------------------------------------------------------
bool cUDPClient::Start(iProtocolHost *ProtocolHost)
{
	if(fProtocolHost!=nullptr)
		return false;
	
	if(Reference!=nullptr){
		rIncReference(Reference);
	}
	fHostRef=2;
	fProtocolHost=ProtocolHost;
	fProtocolHost->StartProtocol(this);
	return true;
}
//---------------------------------------------------------------------------
void cUDPClient::Stop(void)
{
	if(fProtocolHost!=nullptr){
		fProtocolHost->TerminateOutput();
	}
}
//---------------------------------------------------------------------------
void cUDPClient::TaskNotify(void)
{
	if(fProtocolHost!=nullptr){
		fProtocolHost->NotifyOutput();
	}
}
//---------------------------------------------------------------------------
uIntn cUDPClient::InputPush(const void *Buffer,uIntn BufferSize)
{
	auto Header=static_cast<const MessageHeader*>(Buffer);

	iPtr<cDNSService::cRequestTask> Task;
	fRequestMapLock->Acquire();
	auto Pair=fRequestMap.GetPair(Header->ID);
	if(Pair!=nullptr){
		Task=cnVar::MoveCast(*Pair.Value);
		fRequestMap.RemovePair(Pair);
	}
	fRequestMapLock->Release();


	if(Task!=nullptr){
		Task->SetBuffer(Buffer,BufferSize);
		Task->Response=static_cast<ResponseCode>(Header->RCODE);
		Task->Finished=true;
		TaskEnd(cnVar::MoveCast(Task));
	}

	return BufferSize;
}
//---------------------------------------------------------------------------
void cUDPClient::CancelRequests(void)
{
	for(auto &p : fRequestMap){
		p.Value->Response=ResponseCode::ServerFailure;
		p.Value->Finished=true;
		TaskEnd(p.Value);
	}
}
//---------------------------------------------------------------------------
void cUDPClient::HostClose(void)
{
	if(fHostRef.Free--==1){
		CancelRequests();
		if(Reference!=nullptr){
			rDecReference(Reference);
		}
	}
}
//---------------------------------------------------------------------------
void cUDPClient::InputClose(bool)
{
	fProtocolHost->TerminateOutput();

	HostClose();
}
//---------------------------------------------------------------------------
uIntn cUDPClient::OutputPull(void *Buffer,uIntn BufferSize)
{
	auto Task=TaskStart();
	if(Task==nullptr)
		return 0;

	fRequestMapLock->Acquire();
	fRequestMap[fRequestID]=Task;
	fRequestMapLock->Release();


	auto TaskBuffer=Task->GetBuffer();

	uIntn CopySize=TaskBuffer.Length;
	if(CopySize>BufferSize){
		CopySize=BufferSize;
	}
	cnMemory::Copy(Buffer,TaskBuffer.Pointer,CopySize);

	// set header
	auto Header=static_cast<MessageHeader*>(Buffer);
	Header->ID=fRequestID;
	Header->QR=0;
	Header->RD=1;

	fRequestID++;
	return CopySize;
}
//---------------------------------------------------------------------------
void cUDPClient::OutputClose(bool)
{
	fProtocolHost->TerminateInput();

	HostClose();
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cTCPDNSPacket::cTCPDNSPacket()
{
}
//---------------------------------------------------------------------------
bool cTCPDNSPacket::SetProtocolHost(iProtocolHost *ProtocolHost)
{
	if(fUpProtocolHost!=nullptr)
		return false;
	
	fUpProtocolHost=ProtocolHost;

	if(fDownProtocolProcessor!=nullptr){
		StartProcess();
	}
	return true;
}
//---------------------------------------------------------------------------
void cTCPDNSPacket::StartProcess(void)
{
	fUpProtocolHost->StartProtocol(this);
}
//---------------------------------------------------------------------------
void cTCPDNSPacket::ConnectionReset(void)
{
	fRecvBufferPacker.Clear();
	fRecvSize=0;
}
//---------------------------------------------------------------------------
bool cTCPDNSPacket::StartProtocol(iProtocolProcessor *ProtocolProcessor)
{
	if(fDownProtocolProcessor!=nullptr)
		return false;
	fDownProtocolProcessor=ProtocolProcessor;

	if(fUpProtocolHost!=nullptr){
		StartProcess();
	}
	return true;
}
//---------------------------------------------------------------------------
void cTCPDNSPacket::NotifyInput(void)
{
	fUpProtocolHost->NotifyInput();
}
//---------------------------------------------------------------------------
void cTCPDNSPacket::TerminateInput(void)
{
	fUpProtocolHost->TerminateInput();
}
//---------------------------------------------------------------------------
void cTCPDNSPacket::NotifyOutput(void)
{
	fUpProtocolHost->NotifyOutput();
}
//---------------------------------------------------------------------------
void cTCPDNSPacket::SetEndOfOutput(void)
{
	fUpProtocolHost->SetEndOfOutput();
}
//---------------------------------------------------------------------------
void cTCPDNSPacket::TerminateOutput(void)
{
	fUpProtocolHost->TerminateOutput();
}
//---------------------------------------------------------------------------
uIntn cTCPDNSPacket::InputPush(const void *Buffer,uIntn BufferSize)
{
	fRecvBufferPacker.StartBuffer(Buffer,BufferSize);

	cConstMemory ReadMemory;
	if(fRecvSize==0){
		
		if(fRecvBufferPacker.PackReadData(ReadMemory,2)==false)
			return BufferSize;
		fRecvSize=*static_cast<const uInt16*>(ReadMemory.Pointer);
		fRecvSize=cnMemory::SwapBigEndian(fRecvSize);
		fRecvBufferPacker.CompleteReadBuffer(2);
	}
	while(fRecvBufferPacker.PackReadData(ReadMemory,fRecvSize)){

		if(fDownProtocolProcessor->InputPush(ReadMemory.Pointer,fRecvSize)==0)
			return 0;
		fRecvBufferPacker.CompleteReadBuffer(fRecvSize);
		fRecvSize=0;

		if(fRecvBufferPacker.PackReadData(ReadMemory,2)==false)
			break;
		fRecvSize=*static_cast<const uInt16*>(ReadMemory.Pointer);
		fRecvSize=cnMemory::SwapBigEndian(fRecvSize);
		fRecvBufferPacker.CompleteReadBuffer(2);
	}
	return BufferSize;
}
//---------------------------------------------------------------------------
void cTCPDNSPacket::InputClose(bool GracefulClose)
{
	fDownProtocolProcessor->InputClose(GracefulClose);
}
//---------------------------------------------------------------------------
uIntn cTCPDNSPacket::OutputPull(void *Buffer,uIntn BufferSize)
{
	if(fDownProtocolProcessor==nullptr)
		return 0;
	uIntn PulledSize=fDownProtocolProcessor->OutputPull(cnMemory::PointerAddByteOffset(Buffer,2),BufferSize-2);
	if(PulledSize==0){
		return 0;
	}

	*static_cast<uInt16*>(Buffer)=cnMemory::SwapBigEndian(static_cast<uInt16>(PulledSize));
	return PulledSize+2;
}
//---------------------------------------------------------------------------
void cTCPDNSPacket::OutputClose(bool GracefulClose)
{
	fDownProtocolProcessor->OutputClose(GracefulClose);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
bool cUDPPortManager::Start(iMultipointQueue *Datagram)
{
	if(fDatagram!=nullptr)
		return false;
	fStopQueue=false;
	fDatagram=Datagram;
	if(fDatagram->StartAccess(this)==false){
		fDatagram=nullptr;
		return false;
	}

	return true;
}
//---------------------------------------------------------------------------
void cUDPPortManager::Stop(void)
{
	if(fDatagram!=nullptr){
		fStopQueue=true;
		fDatagram->NotifyWrite(true);
	}
}
//---------------------------------------------------------------------------
void cUDPPortManager::AccessStarted(void)
{
	if(Reference!=nullptr)
		rIncReference(Reference,'dnsp');
}
//---------------------------------------------------------------------------
void cUDPPortManager::AccessStopped(void)
{
	if(Reference!=nullptr)
		rDecReference(Reference,'dnsp');
}
//---------------------------------------------------------------------------
cUDPPortManager::cUDPMessage* cUDPPortManager::AcquireMessage(void)
{
	auto NewMessage=fMessageRecycler.Query();
	NewMessage->Owner=this;
	return NewMessage;
}
//---------------------------------------------------------------------------
void cUDPPortManager::ReleaseMessage(cUDPMessage *Message)
{
	fMessageRecycler.Recycle(Message);
}
//---------------------------------------------------------------------------
void cUDPPortManager::cUDPMessage::Execute(void)
{
	Owner->fResponseQueue.Enqueue(this);
	if(Owner->fDatagram!=nullptr){
		Owner->fDatagram->NotifyWrite(false);
	}
}
//---------------------------------------------------------------------------
void cUDPPortManager::ReadNotify(void)
{
	if(fStopQueue){
		return;
	}
	if(fDatagram==nullptr)
		return;

	cConstMemory ReadBuffer;
	iPtr<iAddress> ReadAddress;
	while(fDatagram->QueryReadBuffer(ReadAddress,ReadBuffer)){
		auto Header=static_cast<const cnNet::DNS::MessageHeader*>(ReadBuffer.Pointer);

		auto Message=AcquireMessage();
		Message->Address=ReadAddress;
		Message->ID=Header->ID;

		Message->Task=DNSService->RequestBuffer(ReadBuffer.Pointer,ReadBuffer.Length);
		if(Message->Task->SetNotify(Message)==false){
			Message->Execute();
		}

		fDatagram->CompleteReadBuffer();
	}
}
//---------------------------------------------------------------------------
void cUDPPortManager::WriteNotify(void)
{
	if(fDatagram==nullptr)
		return;
	if(fStopQueue){
		auto Datagram=fDatagram;
		fDatagram=nullptr;
		Datagram->StopAccess();
		return;
	}
	cMemory WriteBuffer;
	while(fDatagram->QueryWriteBuffer(WriteBuffer)){
		auto Response=fResponseQueue.Dequeue();
		if(Response==nullptr)
			return;


		//Address=Response->Address;
		auto ResponseBuffer=Response->Task->QueryMessageBinary();

		uIntn CopySize=ResponseBuffer.Length;
		if(CopySize>WriteBuffer.Length)
			CopySize=WriteBuffer.Length;
		cnMemory::Copy(WriteBuffer.Pointer,ResponseBuffer.Pointer,CopySize);

		auto Header=static_cast<cnNet::DNS::MessageHeader*>(WriteBuffer.Pointer);
		Header->ID=Response->ID;
		Header->QR=true;
		Header->RCODE=static_cast<uInt8>(Response->Task->GetResponse());
		Header->AA=false;
		Header->RA=false;
		Header->RD=true;
		Header->TC=false;

		fDatagram->CommitWriteBuffer(Response->Address,CopySize);

		ReleaseMessage(Response);
	}
}
//---------------------------------------------------------------------------


#endif // 0



#endif // 0
