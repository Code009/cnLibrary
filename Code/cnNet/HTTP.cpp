#include "HTTP.h"


//---------------------------------------------------------------------------
using namespace cnLibrary;
using namespace cnNet;
using namespace HTTP;

//---------------------------------------------------------------------------
const char HTTP::TextLineReturn[2]={'\r','\n'};
#if 0

//---------------------------------------------------------------------------
inline int str_toupper(int c){
	if(c>='a' && c<='z')
		return c-'a'+'A';
	return c;
}
//---------------------------------------------------------------------------
template<class T>
struct cStringUpperCaseView
{
	T *_ptr_;
	T operator *()const{	return static_cast<T>(str_toupper(*_ptr_));	}

	cStringUpperCaseView& operator ++(){	++_ptr_;	return *this;	}
	cStringUpperCaseView operator ++(int){	cStringUpperCaseView v={_ptr_++};	return v;	}

};
//---------------------------------------------------------------------------

static constexpr const char *MonthText[12]={"Jan","Feb","Mar","Apr","May","Jun","Jul","Aug","Sep","Oct","Nov","Dec"};

//---------------------------------------------------------------------------
URIPart HTTP::ParseURI(const char *URL,uIntn Length)
{
	// parse url

	URIPart Parts;
	Parts.ScLen=0;
	Parts.HostLen=0;
	Parts.PathLen=0;

	enum {sError,sProtocol,sProtocolSlash0,sProtocolSlash1,sHost,sPath}urlState;
	urlState=sProtocol;
	uIntn ScanPos=0;
	uIntn Index=0;
	for(Index=0;Index<Length;Index++){
		char c=URL[Index];
		if(c==0)
			break;
		switch(urlState){
		case sProtocol:
			if(c==':'){
				Parts.Sc=ScanPos;
				Parts.ScLen=Index-ScanPos;
				urlState=sProtocolSlash0;
			}
			break;
		case sProtocolSlash0:
			if(c=='/'){
				urlState=sProtocolSlash1;
			}
			else{
				urlState=sError;
			}
			break;
		case sProtocolSlash1:
			if(c=='/'){
				ScanPos=Index+1;
				urlState=sHost;
			}
			else{
				urlState=sError;
			}
			break;
		case sHost:
			if(c=='/'){
				Parts.Host=ScanPos;
				Parts.HostLen=Index-ScanPos;

				ScanPos=Index;

				urlState=sPath;
			}
			else{

			}
			break;
		case sPath:
			break;
		}
	}
	switch(urlState){
	case sError:
	case sProtocolSlash0:
	case sProtocolSlash1:
	case sHost:
		Parts.Host=ScanPos;
		Parts.HostLen=Index-ScanPos;
		break;
	case sPath:
		Parts.Path=ScanPos;
		Parts.PathLen=Index-ScanPos;
		break;
	}

	return Parts;
}
//---------------------------------------------------------------------------
bool HTTP::ParseLine(uIntn &LineLength,const void *Buffer,uIntn BufferSize)
{
	if(BufferSize==0){
		return false;
	}
	auto *Line=static_cast<const char*>(Buffer);
	uIntn LineIndex=1;
	uIntn LFIndex;
	while(( LFIndex=cnMemory::Search(Line+LineIndex,BufferSize-LineIndex,'\n') )!=IndexNotFound){
		LFIndex+=LineIndex;
		if(Line[LFIndex-1]=='\r'){
			// one line
			LineLength=LFIndex+1;
			return true;
		}
		LineIndex=LFIndex+1;
	}
	return false;
}
//---------------------------------------------------------------------------
uIntn HTTP::ParseHeaderMessage(ufInt16 *Pos,ufInt16 *Len,uIntn PosCount,const char *Line,uIntn LineLength)
{
	if(PosCount==0)
		return 0;

	uIntn LineIndex=0;
	uIntn PosIndex=0;
	

	uIntn SPIndex;
	while(( SPIndex=cnMemory::Search(Line+LineIndex,LineLength-LineIndex,' ') )!=IndexNotFound){
		SPIndex+=LineIndex;
		uIntn PartSize=SPIndex-LineIndex;
		if(PartSize!=0){
			Pos[PosIndex]=static_cast<ufInt16>(LineIndex);
			Len[PosIndex]=static_cast<ufInt16>(PartSize);
			PosIndex++;
			if(PosIndex>=PosCount){
				return PosCount;
			}
		}
		LineIndex=SPIndex+1;
	}
	uIntn PartSize=LineLength-LineIndex;
	if(PartSize!=0){
		Pos[PosIndex]=static_cast<ufInt16>(LineIndex);
		Len[PosIndex]=static_cast<ufInt16>(PartSize);
		PosIndex++;
	}
	return PosIndex;
}
//---------------------------------------------------------------------------
bool HTTP::ParseHeaderParameter(ufInt16 &NameLen,const char *Line,uIntn LineLength)
{
	uIntn SplitterIndex;
	SplitterIndex=cnMemory::Search(Line,LineLength,':');
	if(SplitterIndex==IndexNotFound){
		NameLen=static_cast<ufInt16>(LineLength);
		return false;
	}
	NameLen=static_cast<ufInt16>(SplitterIndex);
	return true;
}
//---------------------------------------------------------------------------
void HTTP::TrimLWS(ufInt16 &Pos,ufInt16 &Len,const char *Text,uIntn Length)
{
	for(ufInt16 i=0;i<Length;i++){
		switch(Text[i]){
		case ' ':
		case '\t':
			break;
		default:
			Pos=i;
			for(ufInt16 bi=static_cast<ufInt16>(Length)-1;bi>i;bi--){
				switch(Text[i]){
				case ' ':
				case '\t':
					break;
				default:
					Len=bi-i+1;
					return;
				}

			}
			Len=1;
			return;
		}
	}
	Pos=0;
	Len=0;
}
//---------------------------------------------------------------------------
void HTTP::AppendDate(cStringBuffer<char> &DateString,const iTimepoint *Time)
{
	cDateTime Date;
	if(cnSystem::UTCGregorianDateFromSystemTime(Date,Time->SystemTime())){
		auto m=MonthText[Date.Month-1];
		cnString::FormatStringToStream(DateString.WriteStream(),"%d %s %d %.2d:%.2d:%.2d GMT",Date.Day,m,Date.Year,Date.Hour,Date.Minute,Date.Second);
	}
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cClientSession::cClientHost::cClientHost(cClientSession *Session)
	: fSession(Session)
{
}
//---------------------------------------------------------------------------
bool cClientSession::cClientHost::HTTPStart(iClientProcessor *Processor)
{
	if(ClientProcessor!=nullptr)
		return false;

	ClientProcessor=Processor;
	RequestCompleted=false;
	fSession->MessageEnqueue(this);
	return true;
}
//---------------------------------------------------------------------------
void cClientSession::cClientHost::HTTPTerminate(void)
{
	fSession->MessageTerminate(this);
}
//---------------------------------------------------------------------------
void cClientSession::cClientHost::HTTPNotifySendRequest(void)
{
	fSession->MessageNotifySendRequest(this);
}
//---------------------------------------------------------------------------
void cClientSession::cClientHost::HTTPCompleteRequest(void)
{
	RequestCompleted=true;
	fSession->MessageCompleteRequest(this);
}
//---------------------------------------------------------------------------
void cClientSession::cClientHost::HTTPNotifyReceiveResponse(void)
{
	fSession->MessageNotifyReceiveResponse(this);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cClientSession::cClientSession()
{
}
//---------------------------------------------------------------------------
cClientSession::~cClientSession()
{
}
//---------------------------------------------------------------------------
rPtr<iClientHost> cClientSession::CreateHost(void)
{
	return rCreate<cClientHost>(this);
}
//---------------------------------------------------------------------------
void cClientSession::VirtualStopped(void)
{
	fSessionStopState=1;
	if(fProtocolProvider!=nullptr){
		fProtocolProvider->NotifyOutput(0);
	}
	InnerDecReference('self');
}
//---------------------------------------------------------------------------
bool cClientSession::SetProtocolHost(iProtocolProvider *Provider)
{
	if(Provider==nullptr)
		return false;
	if(fProtocolProvider!=nullptr)
		return false;

	fProtocolProvider=Provider;

	InnerIncReference('http');
	fHostChannelRefCount=2;

	fSessionStopState=0;
	fShutdownAfterRequest=false;

	fResponseState=rsHeader;
	fResponseHeaderState=hpsRequest;
	fExpectedResponseBodySize=IndexNotFound;


	fSendState=ssMethod;

	if(fProtocolProvider->SetupProtocol(this)==false){
		InnerDecReference('http');
		return false;
	}
	return true;
}
//---------------------------------------------------------------------------
void cClientSession::MessageEnqueue(cClientHost *ClientHost)
{
	rIncReference(ClientHost,'qsnd');
	fSendQueue.Enqueue(ClientHost);

	if(fProtocolProvider==nullptr)
		return;
	fProtocolProvider->NotifyOutput(0);
}
//---------------------------------------------------------------------------
void cClientSession::MessageTerminate(cClientHost *ClientHost)
{
#pragma message (cnLib_FILE_LINE ": TODO - terminate message")
}
//---------------------------------------------------------------------------
void cClientSession::MessageNotifySendRequest(cClientHost *ClientHost)
{
	fProtocolProvider->NotifyOutput(0);
}
//---------------------------------------------------------------------------
void cClientSession::MessageCompleteRequest(cClientHost *ClientHost)
{
	if(fSendingHost!=ClientHost){
		return;
	}
	fProtocolProvider->NotifyOutput(0);
}
//---------------------------------------------------------------------------
void cClientSession::MessageNotifyReceiveResponse(cClientHost *ClientHost)
{
	fProtocolProvider->NotifyInput();
}
//---------------------------------------------------------------------------
void cClientSession::HostChannelRelease(void)
{
	if(fHostChannelRefCount.Free--==1){
		fProtocolProvider=nullptr;
		//Callback->HTTPSessionStopped(this);
		
		InnerDecReference('http');
	}
}
//---------------------------------------------------------------------------
uIntn cClientSession::OutputPull(void *Buffer,uIntn BufferSize)
{
	if(fSendingHost==nullptr){
		auto QueueingHost=fSendQueue.Dequeue();
		if(QueueingHost==nullptr){
			if(fSessionStopState==1){
				fSessionStopState=2;
				fProtocolProvider->SetEndOfOutput();
			}
			return 0;	// no more requests
		}
		fSendState=ssMethod;
		fSendingHost=rAttach(QueueingHost,'qsnd');
	}

	uIntn SizeReturn=0;
	switch(fSendState){
	case ssMethod:
		fSendBuffer.Clear();
		{
			cArray<const char> Method;
			cArray<const char> Path;
			cArray<const char> Version;
			fSendingHost->ClientProcessor->HTTPRequestStart(Method,Path,Version);

			auto WriteStream=cnStream::BinaryWriteStream(fSendBuffer.Storage());
			WriteStream.Array(Method.Pointer,Method.Length);
			WriteStream.Value<char>(' ');
			WriteStream.Array(Path.Pointer,Path.Length);
			WriteStream.Value<char>(' ');
			WriteStream.Array(Version.Pointer,Version.Length);
			WriteStream.Array<char>(TextLineReturn,2);
		}
		fSendState=ssHeader;
		cnLib_SWITCH_FALLTHROUGH;
	case ssHeader:
		do{
			if(BufferSize<fSendBuffer->Length){
				cnMemory::Copy(Buffer,fSendBuffer->Pointer,BufferSize);
				SizeReturn+=BufferSize;
				// remove head
				fSendBuffer.Replace(0,BufferSize,0);
				return SizeReturn;
			}
			cnMemory::Copy(Buffer,fSendBuffer->Pointer,fSendBuffer->Length);
			SizeReturn+=fSendBuffer->Length;

			Buffer=cnMemory::PointerAddByteOffset(Buffer,fSendBuffer->Length);
			BufferSize-=fSendBuffer->Length;
		}while(PullHeader());
		fSendState=ssHeaderEnd1;
		cnLib_SWITCH_FALLTHROUGH;
	case ssHeaderEnd1:
		// append one more empty line
		if(BufferSize==0)
			return SizeReturn;
		if(BufferSize==1){
			static_cast<char*>(Buffer)[0]='\r';
			fSendState=ssHeaderEnd2;
			return SizeReturn+1;
	case ssHeaderEnd2:
			static_cast<char*>(Buffer)[0]='\n';
			Buffer=cnMemory::PointerAddByteOffset(Buffer,1);
			BufferSize-=1;
			SizeReturn+=1;
		}
		else{
			static_cast<char*>(Buffer)[0]='\r';
			static_cast<char*>(Buffer)[1]='\n';
			Buffer=cnMemory::PointerAddByteOffset(Buffer,2);
			BufferSize-=2;
			SizeReturn+=2;
		}
		fSendState=ssBody;
		cnLib_SWITCH_FALLTHROUGH;
	case ssBody:
		{
			uIntn BodySize=fSendingHost->ClientProcessor->HTTPRequestBody(Buffer,BufferSize);

			if(BodySize!=0){
				SizeReturn+=BodySize;
			}
			else{
				if(fSendingHost->RequestCompleted){
					// send to receive queue
					auto RecvHost=rDetach(fSendingHost,'qrcv');
					fReceiveQueue.Enqueue(RecvHost);
					fProtocolProvider->NotifyInput();
				}
			}
		}
	}
	// invalid state
	return SizeReturn;
}
//---------------------------------------------------------------------------
bool cClientSession::PullHeader(void)
{
	fSendBuffer.Clear();
	cArray<const char> Name;
	cArray<const char> Value;
	if(fSendingHost->ClientProcessor->HTTPRequestHeader(Name,Value)==false)
		return false;

	auto WriteStream=cnStream::BinaryWriteStream(fSendBuffer.Storage());
	WriteStream.Array(Name.Pointer,Name.Length);
	WriteStream.Value<char>(':');
	WriteStream.Value<char>(' ');
	WriteStream.Array(Value.Pointer,Value.Length);
	WriteStream.Array<char>(TextLineReturn,2);
	return true;
}
//---------------------------------------------------------------------------
void cClientSession::OutputClose(bool GracefulClose)
{
	HostChannelRelease();
}
//---------------------------------------------------------------------------
uIntn cClientSession::InputPush(const void *Buffer,uIntn BufferSize)
{
	if(fReceivingHost==nullptr){
		auto RecvHost=fReceiveQueue.Dequeue();
		if(RecvHost==nullptr){
			// wait for receive host
			return 0;
		}
		fReceivingHost=rAttach(RecvHost,'qrcv');
		// reset receive state
		fResponseState=rsHeader;
		fResponseHeaderState=hpsRequest;
		fExpectedResponseBodySize=IndexNotFound;
	}

	fInputBufferReader.StartBuffer(Buffer,BufferSize);

	switch(fResponseState){
	case rsHeader:
		{
			// find first line
			uIntn LineLength;
			if(HTTP::ParseLine(LineLength,Buffer,BufferSize)){
				ProcessInputHeader(LineLength);
			}
			else{
				// line incompleted
				fInputBufferReader.QueueAllBuffer();
				return BufferSize;
			}
		}
		break;
	case rsBody:
		ProcessInputBody();
		break;
	}
	return fInputBufferReader.FinishBuffer();
}
//---------------------------------------------------------------------------
void cClientSession::InputClose(bool GracefulClose)
{
	if(fReceivingHost!=nullptr){
		if(GracefulClose && fResponseState==rsBody && fExpectedResponseBodySize==IndexNotFound){
			// body completed
			fReceivingHost->ClientProcessor->HTTPResponseBodyFinish();
		}
		else{
			// error
			fReceivingHost->ClientProcessor->HTTPTerminated();
		}
		fReceivingHost=nullptr;
	}

	fProtocolProvider->SetEndOfOutput();

	HostChannelRelease();
}
//---------------------------------------------------------------------------
const char* cClientSession::ParseResponseHeaderLine(uIntn &LineLength)
{
	cConstMemory ReadMemory;
	uIntn ReadIndex=0;
	while(fInputBufferReader.QueryReadBuffer(ReadMemory,ReadIndex+128)){
		auto pLine=static_cast<const char*>(ReadMemory.Pointer);
		if(ReadIndex>ReadMemory.Length)
			return nullptr;
		if(HTTP::ParseLine(LineLength,pLine+ReadIndex,ReadMemory.Length-ReadIndex)){
			return pLine;
		}
		ReadIndex+=ReadMemory.Length;
	}
	return 0;
}
//---------------------------------------------------------------------------
void cClientSession::ProcessReponseHeaderLine(const char *Line,uIntn LineSize)
{
	switch(fResponseHeaderState){
	case hpsRequest:
		{
			// HTTP/1.1 200 OK
			ufInt16 Pos[3];
			ufInt16 Len[3]={0,0,0};

			uIntn Count=HTTP::ParseHeaderMessage(Pos,Len,3,Line,LineSize);
			if(Count!=3){
				fResponseHeaderState=hpsError;
				break;
			}

			// create request processor

			fReceivingHost->ClientProcessor->HTTPResponseStatus(Line+Pos[0],Len[0],Line+Pos[1],Len[1],Line+Pos[2],Len[2]);
			fResponseHeaderState=hpsParameter;
		}
		break;
	case hpsParameter:
		{
			ufInt16 NameLen;
			bool ValuePresent=HTTP::ParseHeaderParameter(NameLen,Line,LineSize);

			if(ValuePresent){
				ufInt16 ValuePos=NameLen+1;
				ufInt16 ValueLength=static_cast<ufInt16>(LineSize-ValuePos);

				{
					ufInt16 TrimmedValuePos;
					TrimLWS(TrimmedValuePos,ValueLength,Line+ValuePos,ValueLength);
					ValuePos+=TrimmedValuePos;
				}

				if(cnString::ViewDiffIndexRange(cStringUpperCaseView<const char>{Line},cStringUpperCaseView<const char>{"Content-Length"},NameLen+1)==IndexNotFound){
					cnString::ToInt(fExpectedResponseBodySize,Line+ValuePos,10,ValueLength);
				}

				fReceivingHost->ClientProcessor->HTTPResponseHeader(Line,NameLen,Line+ValuePos,ValueLength);
			}
			else{
				fReceivingHost->ClientProcessor->HTTPResponseHeader(Line,NameLen,nullptr,0);
			}
		}
		break;
	case hpsError:
		break;
	}
}
//---------------------------------------------------------------------------
void cClientSession::ProcessInputHeader(uIntn FirstLineLength)
{
	auto ReadStream=cnStream::BinaryReadStreamT(fInputBufferReader);

	auto Line=ReadStream.Ptr<char>(FirstLineLength);
	if(Line==nullptr)
		return;

	if(FirstLineLength>=2){
		ProcessReponseHeaderLine(Line,FirstLineLength-2);
	}
	ReadStream.Advance(FirstLineLength);

	// next line

	uIntn LineLength;
	while((Line=ParseResponseHeaderLine(LineLength))!=nullptr){
		// process header line
		if(LineLength<=2){
			// header completed
			goto HeaderCompleted;
		}
		ProcessReponseHeaderLine(Line,LineLength-2);

		// continue next line
		ReadStream.Advance<char>(LineLength);
	}
	// no more lines
	return;
HeaderCompleted:
	// header completed
	ReadStream.Advance(LineLength);

	// finish header
	fReceivingHost->ClientProcessor->HTTPResponseHeaderFinish(fExpectedResponseBodySize);

	// following body
	if(fExpectedResponseBodySize==0){
		// body finished
		fReceivingHost->ClientProcessor->HTTPResponseBodyFinish();
		fReceivingHost=nullptr;
		return;
	}
	fResponseState=rsBody;
	ProcessInputBody();
}
//---------------------------------------------------------------------------
void cClientSession::ProcessInputBody(void)
{
	cConstMemory ReadMemory;
	while(fInputBufferReader.QueryReadBuffer(ReadMemory,0)){

		// send to process
		if(ReadMemory.Length>fExpectedResponseBodySize){
			ReadMemory.Length=fExpectedResponseBodySize;
		}
		uIntn SizeProcessed=fReceivingHost->ClientProcessor->HTTPResponseBody(ReadMemory.Pointer,ReadMemory.Length);
		if(SizeProcessed==0){
			// wait for processor notification
			return;
		}
		if(fExpectedResponseBodySize!=IndexNotFound){
			if(fExpectedResponseBodySize<=SizeProcessed){
				// body finished
				fReceivingHost->ClientProcessor->HTTPResponseBodyFinish();
				fReceivingHost=nullptr;
		
				fInputBufferReader.CompleteReadBuffer(fExpectedResponseBodySize);
				return;
			}
			fExpectedResponseBodySize-=SizeProcessed;

		}

		fInputBufferReader.CompleteReadBuffer(SizeProcessed);
	}
}
//---------------------------------------------------------------------------
#if 1
//---------------------------------------------------------------------------
cServerSession::cMessage::cMessage(cServerSession *Session)
	: fSession(Session)
{
	ResponseCompleted=false;
}
//---------------------------------------------------------------------------
void cServerSession::cMessage::HTTPTerminate(void)
{
	fSession->MessageTerminate(this);
}
//---------------------------------------------------------------------------
void cServerSession::cMessage::HTTPNotifyReceiveRequest(void)
{
	fSession->MessageNotifyReceiveRequest(this);
}
//---------------------------------------------------------------------------
void cServerSession::cMessage::HTTPNotifySendResponse(void)
{
	fSession->MessageNotifySendResponse(this);
}
//---------------------------------------------------------------------------
void cServerSession::cMessage::HTTPCompleteResponse(void)
{
	ResponseCompleted=true;
	fSession->MessageCompleteResponse(this);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cServerSession::cServerSession()
{
	fSendingMessage=nullptr;
	fReceivingMessage=nullptr;
}
//---------------------------------------------------------------------------
cServerSession::~cServerSession()
{
}
//---------------------------------------------------------------------------
bool cServerSession::Start(iProtocolProvider *Provider)
{
	if(Provider==nullptr)
		return false;
	if(fProtocolProvider!=nullptr)
		return false;

	fProtocolProvider=Provider;

	rIncReference(this,'http');
	fHostChannelRefCount=2;

	fSessionStopState=0;

	fRequestState=rsHeader;
	fRequestHeaderState=hpsRequest;
	fExpectedRequestBodySize=IndexNotFound;



	fSendState=ssMethod;

	if(fProtocolProvider->SetupProtocol(this)==false){
		rDecReference(this,'http');
		return false;
	}
	return true;
}
//---------------------------------------------------------------------------
void cServerSession::Stop(void)
{
	fSessionStopState=1;
	if(fProtocolProvider!=nullptr){
		fProtocolProvider->NotifyOutput(0);
	}
}
//---------------------------------------------------------------------------
void cServerSession::MessageTerminate(cMessage *Message)
{
#pragma message (cnLib_FILE_LINE ": TODO - terminate message")
}
//---------------------------------------------------------------------------
void cServerSession::MessageNotifyReceiveRequest(cMessage *Message)
{
	fProtocolProvider->NotifyInput();
}
//---------------------------------------------------------------------------
void cServerSession::MessageNotifySendResponse(cMessage *Message)
{
	fProtocolProvider->NotifyOutput(0);
}
//---------------------------------------------------------------------------
void cServerSession::MessageCompleteResponse(cMessage *Message)
{
	if(fSendingMessage!=Message){
		return;
	}
	fProtocolProvider->NotifyOutput(0);
}
//---------------------------------------------------------------------------
void cServerSession::HostChannelRelease(void)
{
	if(fHostChannelRefCount.Free--==1){
		fProtocolProvider=nullptr;
		//Callback->HTTPSessionStopped(this);
		
		rDecReference(this,'http');
	}
}
//---------------------------------------------------------------------------
uIntn cServerSession::OutputPull(void *Buffer,uIntn BufferSize)
{
	if(fSendingMessage==nullptr){
		fSendingMessage=fPendingResponse.Free.Xchg(nullptr);
		if(fSendingMessage==nullptr){
			// no request
			return 0;
		}
		// notify next request
		fProtocolProvider->NotifyInput();

		// start send response
		fSendState=ssMethod;
	}

	uIntn SizeReturn=0;
	switch(fSendState){
	case ssMethod:
		fSendBuffer.Clear();
		{
			cArray<const char> Version;
			cArray<const char> Status;
			cArray<const char> StatusText;
			fSendingMessage->Processor->HTTPResponseStatus(Version,Status,StatusText);

			auto WriteStream=cnStream::BinaryWriteStream(fSendBuffer.Storage());
			WriteStream.Array(Version.Pointer,Version.Length);
			WriteStream.Value<char>(' ');
			WriteStream.Array(Status.Pointer,Status.Length);
			WriteStream.Value<char>(' ');
			WriteStream.Array(StatusText.Pointer,StatusText.Length);
			WriteStream.Array<char>(TextLineReturn,2);
		}
		fSendState=ssHeader;
		cnLib_SWITCH_FALLTHROUGH;
	case ssHeader:
		do{
			if(BufferSize<fSendBuffer->Length){
				cnMemory::Copy(Buffer,fSendBuffer->Pointer,BufferSize);
				SizeReturn+=BufferSize;
				// remove head
				fSendBuffer.Replace(0,BufferSize,0);
				return SizeReturn;
			}
			cnMemory::Copy(Buffer,fSendBuffer->Pointer,fSendBuffer->Length);
			SizeReturn+=fSendBuffer->Length;

			Buffer=cnMemory::PointerAddByteOffset(Buffer,fSendBuffer->Length);
			BufferSize-=fSendBuffer->Length;
		}while(PullHeader());
		fResponseHeaderFinalLine=0;
		fSendState=ssHeaderEnd;
		cnLib_SWITCH_FALLTHROUGH;
	case ssHeaderEnd:
		// append one more empty line
		if(BufferSize==0)
			return SizeReturn;
		if(BufferSize<fResponseHeaderFinalLine){
			static_cast<char*>(Buffer)[0]=TextLineReturn[0];
			fResponseHeaderFinalLine++;
			return SizeReturn+1;
		}
		else{
			uIntn CopySize=2-fResponseHeaderFinalLine;
			while(fResponseHeaderFinalLine<2){
				static_cast<char*>(Buffer)[fResponseHeaderFinalLine]=TextLineReturn[fResponseHeaderFinalLine];
				fResponseHeaderFinalLine++;
			}
			Buffer=cnMemory::PointerAddByteOffset(Buffer,CopySize);
			BufferSize-=CopySize;
			SizeReturn+=CopySize;
		}
		fSendState=ssBody;
		cnLib_SWITCH_FALLTHROUGH;
	case ssBody:
		{
			uIntn BodySize=fSendingMessage->Processor->HTTPResponseBody(Buffer,BufferSize);

			if(BodySize!=0){
				SizeReturn+=BodySize;
			}
			else{
				if(fSendingMessage->ResponseCompleted){
					delete fSendingMessage;
					fSendingMessage=nullptr;
				}
			}
		}
	}
	// invalid state
	return SizeReturn;
}
//---------------------------------------------------------------------------
bool cServerSession::PullHeader(void)
{
	fSendBuffer.Clear();
	cArray<const char> Name;
	cArray<const char> Value;
	if(fSendingMessage->Processor->HTTPResponseHeader(Name,Value)==false)
		return false;

	auto WriteStream=cnStream::BinaryWriteStream(fSendBuffer.Storage());
	WriteStream.Array(Name.Pointer,Name.Length);
	WriteStream.Value<char>(':');
	WriteStream.Value<char>(' ');
	WriteStream.Array(Value.Pointer,Value.Length);
	WriteStream.Array<char>(TextLineReturn,2);
	return true;
}
//---------------------------------------------------------------------------
void cServerSession::OutputClose(bool GracefulClose)
{
	HostChannelRelease();
}
//---------------------------------------------------------------------------
uIntn cServerSession::InputPush(const void *Buffer,uIntn BufferSize)
{
	if(fPendingResponse!=nullptr){
		// has pending response, wait for output
		return 0;
	}

	fInputBufferReader.StartBuffer(Buffer,BufferSize);

	switch(fRequestState){
	case rsHeader:
		{
			// find first line
			uIntn LineLength;
			if(HTTP::ParseLine(LineLength,Buffer,BufferSize)){
				ProcessInputHeader(LineLength);
			}
			else{
				// line incompleted
				fInputBufferReader.QueueAllBuffer();
				return BufferSize;
			}
		}
		break;
	case rsBody:
		ProcessInputBody();
		break;
	}
	return fInputBufferReader.FinishBuffer();
}
//---------------------------------------------------------------------------
void cServerSession::InputClose(bool GracefulClose)
{
	if(fReceivingMessage!=nullptr){
		if(GracefulClose && fExpectedRequestBodySize==IndexNotFound){
			// body completed
			fReceivingMessage->Processor->HTTPRequestBodyFinish();
		}
		else{
			// error
			fReceivingMessage->Processor->HTTPTerminated();
		}
		fReceivingMessage=nullptr;
	}

	fProtocolHost->SetEndOfOutput();

	HostChannelRelease();
}
//---------------------------------------------------------------------------
const char* cServerSession::ParseRequestHeaderLine(uIntn &LineLength)
{
	cConstMemory ReadMemory;
	uIntn ReadIndex=0;
	while(fInputBufferReader.QueryReadBuffer(ReadMemory,ReadIndex+128)){
		auto pLine=static_cast<const char*>(ReadMemory.Pointer);
		if(ReadIndex>ReadMemory.Length)
			return nullptr;
		if(HTTP::ParseLine(LineLength,pLine+ReadIndex,ReadMemory.Length-ReadIndex)){
			return pLine;
		}
		ReadIndex+=ReadMemory.Length;
	}
	return 0;
}
//---------------------------------------------------------------------------
void cServerSession::ProcessRequestHeaderLine(const char *Line,uIntn LineSize)
{
	switch(fRequestHeaderState){
	case hpsRequest:
		{
			// METHOD / HTTP/1.1
			ufInt16 Pos[3];
			ufInt16 Len[3]={0,0,0};

			uIntn Count=HTTP::ParseHeaderMessage(Pos,Len,3,Line,LineSize);
			if(Count!=3){
				fRequestHeaderState=hpsError;
				break;
			}

			// create request processor

			HTTPRequestStart(Line+Pos[0],Len[0],Line+Pos[1],Len[1],Line+Pos[2],Len[2]);
			fRequestHeaderState=hpsParameter;
		}
		break;
	case hpsParameter:
		{
			ufInt16 NameLen;
			bool ValuePresent=HTTP::ParseHeaderParameter(NameLen,Line,LineSize);

			if(ValuePresent){
				ufInt16 ValuePos=NameLen+1;
				ufInt16 ValueLength=static_cast<ufInt16>(LineSize-ValuePos);

				{
					ufInt16 TrimmedValuePos;
					TrimLWS(TrimmedValuePos,ValueLength,Line+ValuePos,ValueLength);
					ValuePos+=TrimmedValuePos;
				}

				if(cnString::ViewDiffIndexRange(cStringUpperCaseView<const char>{Line},cStringUpperCaseView<const char>{"Content-Length"},NameLen)==IndexNotFound){
					cnString::ToInt(fExpectedRequestBodySize,Line+ValuePos,10,ValueLength);
				}

				HTTPRequestHeader(Line,NameLen,Line+ValuePos,ValueLength);
			}
			else{
				HTTPRequestHeader(Line,NameLen,nullptr,0);
			}
		}
		break;
	case hpsError:
		break;
	}
}
//---------------------------------------------------------------------------
void cServerSession::ProcessInputHeader(uIntn FirstLineLength)
{
	auto ReadStream=cnStream::BinaryReadStreamT(fInputBufferReader);

	auto Line=ReadStream.Ptr<char>(FirstLineLength);
	if(Line==nullptr)
		return;

	if(FirstLineLength>=2){
		ProcessRequestHeaderLine(Line,FirstLineLength-2);
	}
	ReadStream.Advance(FirstLineLength);

	// next line

	uIntn LineLength;
	while((Line=ParseRequestHeaderLine(LineLength))!=nullptr){
		// process header line
		if(LineLength<=2){
			// header completed
			goto HeaderCompleted;
		}
		ProcessRequestHeaderLine(Line,LineLength-2);

		// continue next line
		ReadStream.Advance<char>(LineLength);
	}
	// no more lines
	return;
HeaderCompleted:
	// header completed
	ReadStream.Advance(LineLength);

	// finish header
	fReceivingMessage=new cMessage(this);
	fReceivingMessage->Processor=HTTPRequestHeaderFinish(fReceivingMessage,fExpectedRequestBodySize);


	// following body
	if(fExpectedRequestBodySize==0){
		// body finished
		fReceivingMessage->Processor->HTTPRequestBodyFinish();
		// put to response
		fPendingResponse.Free.Store(fReceivingMessage);
		fReceivingMessage=nullptr;
		// next message
		fRequestState=rsHeader;
		fRequestHeaderState=hpsRequest;
		
		// notify response
		fProtocolHost->NotifyOutput();
		return;
	}
	fRequestState=rsBody;
	ProcessInputBody();
}
//---------------------------------------------------------------------------
void cServerSession::ProcessInputBody(void)
{
	cConstMemory ReadMemory;
	while(fInputBufferReader.QueryReadBuffer(ReadMemory,0)){

		// send to process
		if(ReadMemory.Length>fExpectedRequestBodySize){
			ReadMemory.Length=fExpectedRequestBodySize;
		}
		uIntn SizeProcessed=fReceivingMessage->Processor->HTTPRequestBody(ReadMemory.Pointer,ReadMemory.Length);
		if(SizeProcessed==0){
			// wait for processor notification
			return;
		}
		if(fExpectedRequestBodySize!=IndexNotFound){
			if(fExpectedRequestBodySize<=SizeProcessed){
				// body finished
				fReceivingMessage->Processor->HTTPRequestBodyFinish();
				// put to response
				fPendingResponse.Free.Store(fReceivingMessage);
				fReceivingMessage=nullptr;
				// next message
				fRequestState=rsHeader;
				fRequestHeaderState=hpsRequest;
		
				fInputBufferReader.CompleteReadBuffer(fExpectedRequestBodySize);

				// notify response
				fProtocolHost->NotifyOutput();
				return;
			}
			fExpectedRequestBodySize-=SizeProcessed;

		}

		fInputBufferReader.CompleteReadBuffer(SizeProcessed);
	}
}
#endif // 0
//---------------------------------------------------------------------------
#if 0
//---------------------------------------------------------------------------
cServerSession::cResponseMessage::cResponseMessage(cServerSession *SessionHost,iHTTPServerResponder *MessageResponder)
	: Responder(MessageResponder)
{
	fSessionHost=SessionHost;
}
//---------------------------------------------------------------------------
cServerSession::cResponseMessage::~cResponseMessage()
{
}
//---------------------------------------------------------------------------
void cServerSession::cResponseMessage::CheckDelete(void)
{
	if(fDeleteExclusiveFlag.Acquire()==false)
		return;
	do{
		fDeleteExclusiveFlag.Continue();
		if(Responder==nullptr && fSessionEnded){
			delete this;
			return;
		}
	}while(fDeleteExclusiveFlag.Release()==false);
}
//---------------------------------------------------------------------------
void cServerSession::cResponseMessage::SessionMessageEnd(void)
{
	fSessionEnded=true;
	CheckDelete();
}
//---------------------------------------------------------------------------
void cnLib_FUNC cServerSession::cResponseMessage::HTTPTerminate(void)
{
	if(Responder==nullptr){
		return;
	}
	fSessionHost->HTTPTerminate(this);
	ResponderEnd();
}
//---------------------------------------------------------------------------
void cnLib_FUNC cServerSession::cResponseMessage::HTTPNotifyPull(void)
{
	if(Responder==nullptr){
		return;
	}
	fSessionHost->HTTPNotifyPull(this);
}
//---------------------------------------------------------------------------
void cnLib_FUNC cServerSession::cResponseMessage::HTTPEndPull(bool Shutdown)
{
	if(Responder==nullptr){
		return;
	}
	ShutdownAfterResponse=Shutdown;
	EndPull=true;
	fSessionHost->HTTPEndPull(this);
	ResponderEnd();
}
//---------------------------------------------------------------------------
void cServerSession::cResponseMessage::ResponderEnd(void)
{
	Responder=nullptr;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cServerSession::cServerSession()
{
}
//---------------------------------------------------------------------------
cServerSession::~cServerSession()
{
}
//---------------------------------------------------------------------------
bool cServerSession::Start(iProtocolHost *ProtocolHost,iServerProcessor *Processor)
{
	if(fProtocolHost!=nullptr)
		return false;

	fProcessor=Processor;
	fProtocolHost=ProtocolHost;

	
	fStreamRefCount=2;
	fProtocolHost->StartProtocol(this);

	fStopRequest=false;

	
	fRequestState=rsMethod;
	//fRequestHeaderState=rhsRequest;

	fStopPull=false;
	fShutdownAfterResponse=false;

	return true;
}
//---------------------------------------------------------------------------
void cServerSession::Stop(void)
{
#pragma message (cnLib_FILE_LINE ": TODO - Stop")
	if(fProtocolHost!=nullptr){
		fProtocolHost->OutputNotifyEnd(true);
	}
}
//---------------------------------------------------------------------------
void cServerSession::StreamClosed(void)
{
	fPullStream=nullptr;
	fPushStream=nullptr;
	
	auto Callback=fCallback;
	fCallback=nullptr;
	Callback->HTTPSessionStopped(this);
}
//---------------------------------------------------------------------------
void cServerSession::OutputClose(bool GracefulClose)
{
	fStopPull=true;
	NotifyPull();

	fPushStream->StopPush();
	if(fStreamRefCount.Free--==1){
		StreamClosed();
	}
}
//---------------------------------------------------------------------------
uIntn cServerSession::OutputPull(void *Buffer,uIntn BufferSize)
{
	if(fResponseMessage!=nullptr){
Pull:
		if(fResponseMessage->EndPull==false){
			return fResponseMessage->Responder->PullResponse(Buffer,BufferSize,SizeReturn);
		}
		fResponseMessage->SessionMessageEnd();
		fResponseMessage=nullptr;
	}
	auto PullMessage=fPullMessageQueue.Dequeue();
	if(PullMessage!=nullptr){
		fResponseMessage=PullMessage;
		goto Pull;
	}
	else{
		if(fStopRequest || fShutdownAfterResponse){
			fStopPull=true;
			fPullStreamEnded=true;
			NotifyPull();
		}
	}

	return false;
}
//---------------------------------------------------------------------------
void cServerSession::NotifyPull(void)
{
	if(!fPullStreamFlag.Acquire())
		return;
	do{
		fPullStreamFlag.Continue();


		if(fPullStream!=nullptr){
			if(fStopPull){
				fPullStream->StopPull(fPullStreamEnded);
				fPullStream=nullptr;
			}
			else{
				bool IdleNotify=fPullIdleNotify;
				fPullIdleNotify=false;
				fPullStream->NotifyPull(IdleNotify);
			}
		}

	}while(!fPullStreamFlag.Release());
}
//---------------------------------------------------------------------------
void cServerSession::InputClose(bool GracefulClose)
{
	if(fPushMessage!=nullptr){
		if(StreamEnded && fRequestState==rsBody && fExpectedBodySize==uintMax){
			RequestComplete();
		}
		else{
			fPushMessage->Responder->Terminated();
			fPushMessage->SessionMessageEnd();
			fPushMessage=nullptr;
		}
	}
	else{
		fStopRequest=true;
	}
	fPushStream->StopPush();

	if(fStopRequest || StreamEnded==false){
		fPullIdleNotify=true;
		NotifyPull();
	}

	if(fStreamRefCount.Free--==1){
		StreamClosed();
	}
}
//---------------------------------------------------------------------------
uIntn cServerSession::InputPush(const void *Buffer,uIntn BufferSize)
{
	fInputBufferReader.StartBuffer(Buffer,BufferSize);

	switch(fRequestState){
	case rsError:
		return 0;
	case rsHeader:
		{
			// find first line
			uIntn LineLength;
			if(HTTP::ParseLine(LineLength,Buffer,BufferSize)){
				ProcessInputHeader(LineLength);
			}
			else{
				// line incompleted
				fInputBufferReader.QueueAllBuffer();
				return BufferSize;
			}
		}
		break;
	case rsBody:
		ProcessInputBody();
		break;
	}
	return fInputBufferReader.FinishBuffer();


	if(fStopRequest){
		return false;
	}
	switch(fRequestState){
	default:
	case rsError:
		return false;
	case rsBody:
		if(fExpectedBodySize==IndexNotFound){
			fPushMessage->Responder->Body(Buffer,BufferSize);
			return true;
		}
		if(BufferSize<fExpectedBodySize){
			fExpectedBodySize-=BufferSize;
			fPushMessage->Responder->Body(fPushBufferPacker.PendingData(),BufferSize);
			fPushBufferPacker.Clear();
			return true;
		}
		fPushMessage->Responder->Body(fPushBufferPacker.PendingData(),fExpectedBodySize);
			
		RequestComplete();
		if(fStopRequest){
			return false;
		}

		fRequestState=rsHeader;

		Buffer=cnMemory::PointerAddByteOffset(Buffer,fExpectedBodySize);
		BufferSize-=fExpectedBodySize;
		// continue to header
		//[[fallthrough]];
	case rsHeader:
		{
			fPushBufferPacker.PutBuffer(Buffer,BufferSize);
			uIntn LineSize=0;
			while(HTTP::ParseLine(LineSize,fPushBufferPacker)){
				// one line
				if(LineSize>2){
					fPushBufferPacker.Pack(LineSize);
					auto PackedLine=fPushBufferPacker.PackedData<char>();

					switch(fHeaderProcessState){
					case hpsRequest:
						if(ProcessRequestHeaderLine(PackedLine,LineSize-2)==false){
							// error request
							fRequestState=rsError;
							fStopRequest=true;
							fShutdownAfterResponse=true;
							NotifyPull();
							return false;
						}
						break;
					case hpsParameter:
						ProcessRequestParameterLine(PackedLine,LineSize-2);
						break;
					}

					fPushBufferPacker.Advance(LineSize);

				}
				else{
					fPushBufferPacker.Advance(LineSize);

					fExpectedBodySize=uintMax;

					fHeaderProcessState=hpsRequest;
					fPushMessage->Responder->HeaderFinish(fExpectedBodySize);

					// empty line, following body
					fRequestState=rsBody;

					uIntn RemainSize=fPushBufferPacker.PendingSize();
					if(fExpectedBodySize==uintMax){
						fPushMessage->Responder->Body(fPushBufferPacker.PendingData(),RemainSize);
						fPushBufferPacker.Clear();
						return true;
					}
					if(RemainSize<fExpectedBodySize){
						fExpectedBodySize-=RemainSize;
						fPushMessage->Responder->Body(fPushBufferPacker.PendingData(),RemainSize);
						fPushBufferPacker.Clear();
						return true;
					}
					fPushMessage->Responder->Body(fPushBufferPacker.PendingData(),fExpectedBodySize);
					
					RequestComplete();
					if(fStopRequest){
						return false;
					}
					
					fPushBufferPacker.Advance(fExpectedBodySize);

					// continue next header
					fRequestState=rsHeader;
				}
			}
			if(fPushBufferPacker.PackedSize()>=2048){
				fPushBufferPacker.Clear();
			}
		}
		break;
	}


	return true;
}
//---------------------------------------------------------------------------
const char* cServerSession::ParseResponseHeaderLine(uIntn &LineLength)
{
	cConstMemory ReadMemory;
	uIntn ReadIndex=0;
	while(fInputBufferReader.QueryReadBuffer(ReadMemory,ReadIndex+128)){
		auto pLine=static_cast<const char*>(ReadMemory.Pointer);
		if(ReadIndex>ReadMemory.Length)
			return nullptr;
		if(HTTP::ParseLine(LineLength,pLine+ReadIndex,ReadMemory.Length-ReadIndex)){
			return pLine;
		}
		ReadIndex+=ReadMemory.Length;
	}
	return 0;
}
//---------------------------------------------------------------------------
void cServerSession::ProcessRequestHeaderLine(const char *Line,uIntn LineSize)
{
	switch(fHeaderProcessState){
	case hpsRequest:
		{
			// HTTP/1.1 200 OK
			ufInt16 Pos[3];
			ufInt16 Len[3]={0,0,0};

			uIntn Count=HTTP::ParseHeaderMessage(Pos,Len,3,Line,LineSize);
			if(Count!=3){
				fHeaderProcessState=hpsError;
				break;
			}

			// create request processor

			fReceivingHost->ClientProcessor->HTTPResponseStatus(Line+Pos[0],Len[0],Line+Pos[1],Len[1],Line+Pos[2],Len[2]);
			fHeaderProcessState=hpsParameter;
		}
		break;
	case hpsParameter:
		{
			ufInt16 NameLen;
			bool ValuePresent=HTTP::ParseHeaderParameter(NameLen,Line,LineSize);

			if(ValuePresent){
				ufInt16 ValuePos=NameLen+1;
				ufInt16 ValueLength=static_cast<ufInt16>(LineSize-ValuePos);

				{
					ufInt16 TrimmedValuePos;
					TrimLWS(TrimmedValuePos,ValueLength,Line+ValuePos,ValueLength);
					ValuePos+=TrimmedValuePos;
				}

				if(cnString::ViewDiffIndex(cStringUpperCaseView<const char>{Line},cStringUpperCaseView<const char>{"Content-Length"},NameLen)==IndexNotFound){
					cnString::ToInt(fExpectedBodySize,Line+ValuePos,10,ValueLength);
				}

				fReceivingHost->ClientProcessor->HTTPResponseHeader(Line,NameLen,Line+ValuePos,ValueLength);
			}
			else{
				fReceivingHost->ClientProcessor->HTTPResponseHeader(Line,NameLen,nullptr,0);
			}
		}
		break;
	case hpsError:
		break;
	}
}
//---------------------------------------------------------------------------
void cServerSession::ProcessInputHeader(uIntn FirstLineLength)
{
	auto ReadStream=cnStream::BinaryReadStreamT(fInputBufferReader);

	auto Line=ReadStream.Ptr<char>(FirstLineLength);
	if(Line==nullptr)
		return;

	if(FirstLineLength>=2){
		ProcessRequestHeaderLine(Line,FirstLineLength-2);
	}
	ReadStream.Advance(FirstLineLength);

	// next line

	uIntn LineLength;
	while((Line=ParseResponseHeaderLine(LineLength))!=nullptr){
		// process header line
		if(LineLength<=2){
			// header completed
			goto HeaderCompleted;
		}
		ProcessReponseHeaderLine(Line,LineLength-2);

		// continue next line
		ReadStream.Advance<char>(LineLength);
	}
	// no more lines
	return;
HeaderCompleted:
	// header completed
	ReadStream.Advance(LineLength);

	// finish header
	fReceivingHost->ClientProcessor->HTTPResponseHeaderFinish(fExpectedBodySize);

	// following body
	if(fExpectedBodySize==0){
		// body finished
		fReceivingHost->ClientProcessor->HTTPResponseBodyFinish();
		fReceivingHost=nullptr;
		return;
	}
	fResponseState=rsBody;
	ProcessInputBody();
}
//---------------------------------------------------------------------------
bool cServerSession::ProcessRequestHeaderLine(const char *Line,uIntn LineSize)
{
	// GET / HTTP/1.1
	uInt16 Pos[3];
	uInt16 Len[3]={0,0,0};

	uIntn Count=HTTP::ParseHeaderMessage(Pos,Len,3,Line,LineSize);
	if(Count!=3){
		return false;
	}

	// create request processor

	auto Responder=fCallback->HTTPCreateResponder(nullptr,Line+Pos[0],Len[0],Line+Pos[1],Len[1],Line+Pos[2],Len[2]);
	if(Responder==nullptr){
		return false;
	}
	fHeaderProcessState=hpsParameter;

	fPushMessage=new cResponseMessage(this,Responder);

	Responder->StartServer(fPushMessage);
	return true;
}
//---------------------------------------------------------------------------
void cServerSession::ProcessRequestParameterLine(const char *Line,uIntn LineSize)
{
	uInt16 NameLen;
	bool ValuePresent=HTTP::ParseHeaderParameter(NameLen,Line,LineSize);

	if(ValuePresent){
		uInt16 ValuePos=NameLen+1;
		uIntn ValueLength=LineSize-ValuePos;
		fPushMessage->Responder->Parameter(Line,NameLen,Line+ValuePos,ValueLength);
	}
	else{
		fPushMessage->Responder->Parameter(Line,NameLen,nullptr,0);
	}
}
//---------------------------------------------------------------------------
void cServerSession::RequestComplete(void)
{
	auto ResponseMessage=fPushMessage;
	fPushMessage=nullptr;

	fPullMessageQueue.Enqueue(ResponseMessage);
	ResponseMessage->Responder->BodyFinish();
}
//---------------------------------------------------------------------------
void cServerSession::HTTPTerminate(cResponseMessage *ResponseMessage)
{
	fStopRequest=true;
	if(ResponseMessage!=fResponseMessage)
		return;
	Terminate();
}
//---------------------------------------------------------------------------
void cServerSession::HTTPNotifyPull(cResponseMessage *ResponseMessage)
{
	if(ResponseMessage!=fResponseMessage)
		return;

	NotifyPull();
}
//---------------------------------------------------------------------------
void cServerSession::HTTPEndPull(cResponseMessage *ResponseMessage)
{
	if(ResponseMessage->ShutdownAfterResponse){
		// stop further requests
		fStopRequest=true;
	}
	if(ResponseMessage!=fResponseMessage)
		return;

	fPullIdleNotify=true;
	NotifyPull();
}
#endif // 1
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cClientQuery::cClientQuery()
{
	fState=ClientQueryState::Idle;
	fMethod="GET";
	fRequestReadBuffer=nullptr;
	fResponseWriteBuffer=nullptr;
}
//---------------------------------------------------------------------------
cClientQuery::~cClientQuery()
{
}
//---------------------------------------------------------------------------
bool cClientQuery::Start(iReference *Reference,iClientHost *ClientHost)
{
	if(ClientHost==nullptr)
		return false;
	if(fState!=ClientQueryState::Idle)
		return false;

	// check parameters

	// connect to host
	SetState(ClientQueryState::Connecting);
	fHTTPHost=ClientHost;

	if(fHTTPHost->HTTPStart(this)==false){

		SetState(ClientQueryState::Idle);
		fHTTPHost=nullptr;
		return false;
	}

	// start

	fHostReference=Reference;
	return true;
}
//---------------------------------------------------------------------------
bool cClientQuery::Finish(void)
{
	if(fState!=ClientQueryState::Done)
		return false;

	fResponseStatus=0;
	fResponseStatusString.Clear();
	fHTTPHost=nullptr;
	fState=ClientQueryState::Idle;
	return true;
}
//---------------------------------------------------------------------------
eClientQueryState cClientQuery::GetState(void)
{
	return fState;
}
//---------------------------------------------------------------------------
const cString<char>& cClientQuery::GetHost(void)
{
	return fHost;
}
//---------------------------------------------------------------------------
bool cClientQuery::SetURL(const char *URL)
{
	if(fState!=ClientQueryState::Idle)
		return false;

	auto URIPart=ParseURI(URL,cnString::GetLength(URL));

	fHost.Copy(URL+URIPart.Host,URIPart.HostLen);

	if(URIPart.PathLen!=0){
		fPath.Copy(URL+URIPart.Path,URIPart.PathLen);
	}
	else{
		fPath="/";
	}

	fHeaderMap["Host"]=fHost;

	return true;
}
//---------------------------------------------------------------------------
void cClientQuery::SetMethod(cString<char> Method)
{
	fMethod=cnVar::MoveCast(Method);
}
//---------------------------------------------------------------------------
void cClientQuery::SetHeader(cString<char> Name,cString<char> Value)
{
	fHeaderMap[cnVar::MoveCast(Name)]=cnVar::MoveCast(Value);
}
//---------------------------------------------------------------------------
void cClientQuery::SetRequestInput(iReadBuffer<void> *ReadBuffer)
{
	fRequestReadBuffer=ReadBuffer;
}
//---------------------------------------------------------------------------
void cClientQuery::SetResponseOutput(iWriteBuffer<void> *WriteBuffer)
{
	fResponseWriteBuffer=WriteBuffer;
}
//---------------------------------------------------------------------------
int cClientQuery::GetResponseStatus(void)
{
	return fResponseStatus;
}
//---------------------------------------------------------------------------
const cString<char>& cClientQuery::GetResponseStatusString(void)
{
	return fResponseStatusString;
}
//---------------------------------------------------------------------------
void cClientQuery::SetState(eClientQueryState State)
{
	if(fState!=State){
		fState=State;
		CallEvent(OnStateChange);
	}
}
//---------------------------------------------------------------------------
void cClientQuery::CallEvent(cnVar::cFunction<void (void)> &Event)
{
	if(Event!=nullptr){
		Event();
	}
}
//---------------------------------------------------------------------------
void cClientQuery::HTTPRequestStart(cArray<const char> &Method,cArray<const char> &Path,cArray<const char> &Version)
{
	fResponseWriteBufferFull=false;

	Method=fMethod.Array();
	Path=fPath.Array();
	Version.Pointer="HTTP/1.1";
	Version.Length=8;
	fHeaderEnumIndex=0;

	SetState(ClientQueryState::Requesting);
}
//---------------------------------------------------------------------------
bool cClientQuery::HTTPRequestHeader(cArray<const char> &Name,cArray<const char> &Value)
{
	if(fHeaderEnumIndex>=fHeaderMap.GetCount()){
		return false;
	}

	auto p=fHeaderMap.GetPairAt(fHeaderEnumIndex);
	Name=p->Key.Array();
	Value=p->Value.Array();

	fHeaderEnumIndex++;
	return true;
}
//---------------------------------------------------------------------------
uIntn cClientQuery::HTTPRequestBody(void *Buffer,uIntn BufferSize)
{
	SetState(ClientQueryState::WaitingResponse);

	fHTTPHost->HTTPCompleteRequest();
	return 0;
}
//---------------------------------------------------------------------------
void cClientQuery::HTTPResponseStatus(const char *Version,uIntn VersionLength,const char *Status,uIntn StatusLength,const char *StatusString,uIntn StatusStringLength)
{
	fResponseStatusString.Copy(StatusString,StatusStringLength);
	cnString::ToInt(fResponseStatus,Status,10,StatusLength);

	SetState(ClientQueryState::ReceiveHeader);
}
//---------------------------------------------------------------------------
void cClientQuery::HTTPResponseHeader(const char *Name,uIntn NameLength,const char *Value,uIntn ValueLength)
{
	if(OnResponseHeader!=nullptr){
		OnResponseHeader(Name,NameLength,Value,ValueLength);
	}
}
//---------------------------------------------------------------------------
void cClientQuery::HTTPResponseHeaderFinish(uIntn&)
{
	SetState(ClientQueryState::ReceiveBody);
}
//---------------------------------------------------------------------------
uIntn cClientQuery::HTTPResponseBody(const void *Buffer,uIntn Size)
{
	if(fResponseWriteBufferFull)
		return Size;

	cMemory WriteBuffer;
	uIntn WriteSize=Size;
	while(fResponseWriteBuffer->QueryWriteBuffer(WriteBuffer,WriteSize)){
		if(WriteBuffer.Length>WriteSize){
			cnMemory::Copy(WriteBuffer.Pointer,Buffer,WriteSize);
			fResponseWriteBuffer->CommitWriteBuffer(WriteSize);
			return WriteSize;
		}
		cnMemory::Copy(WriteBuffer.Pointer,Buffer,WriteBuffer.Length);
		fResponseWriteBuffer->CommitWriteBuffer(WriteBuffer.Length);
		WriteSize-=WriteBuffer.Length;
	}
	fResponseWriteBufferFull=true;
	return Size;
}
//---------------------------------------------------------------------------
void cClientQuery::HTTPResponseBodyFinish(void)
{
	
	fState=ClientQueryState::Done;
	CallEvent(OnCompleted);

	fHostReference=nullptr;
}
//---------------------------------------------------------------------------
void cClientQuery::HTTPTerminated(void)
{

	fState=ClientQueryState::Done;
	CallEvent(OnCompleted);

	fHostReference=nullptr;
}

#endif // 0

//---------------------------------------------------------------------------
#if 0

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cHTTPServerSession::cResponseMessage::cResponseMessage(cHTTPServerSession *SessionHost,iHTTPServerResponder *MessageResponder)
	: Responder(MessageResponder)
{
	fSessionHost=SessionHost;
}
//---------------------------------------------------------------------------
cHTTPServerSession::cResponseMessage::~cResponseMessage()
{
}
//---------------------------------------------------------------------------
void cHTTPServerSession::cResponseMessage::CheckDelete(void)
{
	if(fDeleteExclusiveFlag.Acquire()==false)
		return;
	do{
		fDeleteExclusiveFlag.Continue();
		if(Responder==nullptr && fSessionEnded){
			delete this;
			return;
		}
	}while(fDeleteExclusiveFlag.Release()==false);
}
//---------------------------------------------------------------------------
void cHTTPServerSession::cResponseMessage::SessionMessageEnd(void)
{
	fSessionEnded=true;
	CheckDelete();
}
//---------------------------------------------------------------------------
void cnLib_FUNC cHTTPServerSession::cResponseMessage::HTTPTerminate(void)
{
	if(Responder==nullptr){
		return;
	}
	fSessionHost->HTTPTerminate(this);
	ResponderEnd();
}
//---------------------------------------------------------------------------
void cnLib_FUNC cHTTPServerSession::cResponseMessage::HTTPNotifyPull(void)
{
	if(Responder==nullptr){
		return;
	}
	fSessionHost->HTTPNotifyPull(this);
}
//---------------------------------------------------------------------------
void cnLib_FUNC cHTTPServerSession::cResponseMessage::HTTPEndPull(bool Shutdown)
{
	if(Responder==nullptr){
		return;
	}
	ShutdownAfterResponse=Shutdown;
	EndPull=true;
	fSessionHost->HTTPEndPull(this);
	ResponderEnd();
}
//---------------------------------------------------------------------------
void cHTTPServerSession::cResponseMessage::ResponderEnd(void)
{
	Responder=nullptr;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cHTTPServerSession::cHTTPServerSession()
{
}
//---------------------------------------------------------------------------
cHTTPServerSession::~cHTTPServerSession()
{
}
//---------------------------------------------------------------------------
bool cHTTPServerSession::Start(iProtocolHost *ProtocolHost)
{
	if(fProtocolHost!=nullptr)
		return false;
	
	fHostRef=2;
	fProtocolHost=ProtocolHost;
	fProtocolHost->SetProcessor(this);
	Callback->HTTPSessionStarted(this);

	fStopRequest=false;

	fPushBufferPacker.Clear();

	fRequestState=rsHeader;
	fHeaderProcessState=hpsRequest;

	fStopPull=false;
	fShutdownAfterResponse=false;

	fStreamRefCount.Free++;

	return true;
}
//---------------------------------------------------------------------------
void cHTTPServerSession::Stop(void)
{
	if(fProtocolHost!=nullptr){
		fProtocolHost->OutputNotifyEnd(true);
	}
}
//---------------------------------------------------------------------------
void cHTTPServerSession::StreamClosed(void)
{
	fPullStream=nullptr;
	fPushStream=nullptr;
	
	auto Callback=fCallback;
	fCallback=nullptr;
	Callback->HTTPSessionStopped(this);
}
//---------------------------------------------------------------------------
void cHTTPServerSession::OutputClose(void)
{
	fStopPull=true;
	NotifyPull();

	fPushStream->StopPush();
	if(fStreamRefCount.Free--==1){
		StreamClosed();
	}
}
//---------------------------------------------------------------------------
bool cHTTPServerSession::OutputPull(void *Buffer,uIntn BufferSize,uIntn &SizeReturn)
{
	if(fResponseMessage!=nullptr){
Pull:
		if(fResponseMessage->EndPull==false){
			return fResponseMessage->Responder->PullResponse(Buffer,BufferSize,SizeReturn);
		}
		fResponseMessage->SessionMessageEnd();
		fResponseMessage=nullptr;
	}
	auto PullMessage=fPullMessageQueue.Dequeue();
	if(PullMessage!=nullptr){
		fResponseMessage=PullMessage;
		goto Pull;
	}
	else{
		if(fStopRequest || fShutdownAfterResponse){
			fStopPull=true;
			fPullStreamEnded=true;
			NotifyPull();
		}
	}

	return false;
}
//---------------------------------------------------------------------------
void cHTTPServerSession::NotifyPull(void)
{
	if(!fPullStreamFlag.Acquire())
		return;
	do{
		fPullStreamFlag.Continue();


		if(fPullStream!=nullptr){
			if(fStopPull){
				fPullStream->StopPull(fPullStreamEnded);
				fPullStream=nullptr;
			}
			else{
				bool IdleNotify=fPullIdleNotify;
				fPullIdleNotify=false;
				fPullStream->NotifyPull(IdleNotify);
			}
		}

	}while(!fPullStreamFlag.Release());
}
//---------------------------------------------------------------------------
void cHTTPServerSession::InputClose(bool StreamEnded)
{
	if(fPushMessage!=nullptr){
		if(StreamEnded && fRequestState==rsBody && fExpectedBodySize==uintMax){
			RequestComplete();
		}
		else{
			fPushMessage->Responder->Terminated();
			fPushMessage->SessionMessageEnd();
			fPushMessage=nullptr;
		}
	}
	else{
		fStopRequest=true;
	}
	fPushStream->StopPush();

	if(fStopRequest || StreamEnded==false){
		fPullIdleNotify=true;
		NotifyPull();
	}

	if(fStreamRefCount.Free--==1){
		StreamClosed();
	}
}
//---------------------------------------------------------------------------
bool cHTTPServerSession::InputPush(const void *Buffer,uIntn BufferSize)
{
	if(fStopRequest){
		return false;
	}
	switch(fRequestState){
	default:
	case rsError:
		return false;
	case rsBody:
		if(fExpectedBodySize==0xFFFFFFFF){
			fPushMessage->Responder->Body(Buffer,BufferSize);
			return true;
		}
		if(BufferSize<fExpectedBodySize){
			fExpectedBodySize-=BufferSize;
			fPushMessage->Responder->Body(fPushBufferPacker.PendingData(),BufferSize);
			fPushBufferPacker.Clear();
			return true;
		}
		fPushMessage->Responder->Body(fPushBufferPacker.PendingData(),fExpectedBodySize);
			
		RequestComplete();
		if(fStopRequest){
			return false;
		}

		fRequestState=rsHeader;

		Buffer=cnMemory::PointerAddByteOffset(Buffer,fExpectedBodySize);
		BufferSize-=fExpectedBodySize;
		// continue to header
		//[[fallthrough]];
	case rsHeader:
		{
			fPushBufferPacker.PutBuffer(Buffer,BufferSize);
			uIntn LineSize=0;
			while(HTTP::ParseLine(LineSize,fPushBufferPacker)){
				// one line
				if(LineSize>2){
					fPushBufferPacker.Pack(LineSize);
					auto PackedLine=fPushBufferPacker.PackedData<char>();

					switch(fHeaderProcessState){
					case hpsRequest:
						if(ProcessRequestHeaderLine(PackedLine,LineSize-2)==false){
							// error request
							fRequestState=rsError;
							fStopRequest=true;
							fShutdownAfterResponse=true;
							NotifyPull();
							return false;
						}
						break;
					case hpsParameter:
						ProcessRequestParameterLine(PackedLine,LineSize-2);
						break;
					}

					fPushBufferPacker.Advance(LineSize);

				}
				else{
					fPushBufferPacker.Advance(LineSize);

					fExpectedBodySize=uintMax;

					fHeaderProcessState=hpsRequest;
					fPushMessage->Responder->HeaderFinish(fExpectedBodySize);

					// empty line, following body
					fRequestState=rsBody;

					uIntn RemainSize=fPushBufferPacker.PendingSize();
					if(fExpectedBodySize==uintMax){
						fPushMessage->Responder->Body(fPushBufferPacker.PendingData(),RemainSize);
						fPushBufferPacker.Clear();
						return true;
					}
					if(RemainSize<fExpectedBodySize){
						fExpectedBodySize-=RemainSize;
						fPushMessage->Responder->Body(fPushBufferPacker.PendingData(),RemainSize);
						fPushBufferPacker.Clear();
						return true;
					}
					fPushMessage->Responder->Body(fPushBufferPacker.PendingData(),fExpectedBodySize);
					
					RequestComplete();
					if(fStopRequest){
						return false;
					}
					
					fPushBufferPacker.Advance(fExpectedBodySize);

					// continue next header
					fRequestState=rsHeader;
				}
			}
			if(fPushBufferPacker.PackedSize()>=2048){
				fPushBufferPacker.Clear();
			}
		}
		break;
	}


	return true;
}
//---------------------------------------------------------------------------
bool cHTTPServerSession::ProcessRequestHeaderLine(const char *Line,uIntn LineSize)
{
	// GET / HTTP/1.1
	uInt16 Pos[3];
	uInt16 Len[3]={0,0,0};

	uIntn Count=HTTP::ParseHeaderMessage(Pos,Len,3,Line,LineSize);
	if(Count!=3){
		return false;
	}

	// create request processor

	auto Responder=fCallback->HTTPCreateResponder(nullptr,Line+Pos[0],Len[0],Line+Pos[1],Len[1],Line+Pos[2],Len[2]);
	if(Responder==nullptr){
		return false;
	}
	fHeaderProcessState=hpsParameter;

	fPushMessage=new cResponseMessage(this,Responder);

	Responder->StartServer(fPushMessage);
	return true;
}
//---------------------------------------------------------------------------
void cHTTPServerSession::ProcessRequestParameterLine(const char *Line,uIntn LineSize)
{
	uInt16 NameLen;
	bool ValuePresent=HTTP::ParseHeaderParameter(NameLen,Line,LineSize);

	if(ValuePresent){
		uInt16 ValuePos=NameLen+1;
		uIntn ValueLength=LineSize-ValuePos;
		fPushMessage->Responder->Parameter(Line,NameLen,Line+ValuePos,ValueLength);
	}
	else{
		fPushMessage->Responder->Parameter(Line,NameLen,nullptr,0);
	}
}
//---------------------------------------------------------------------------
void cHTTPServerSession::RequestComplete(void)
{
	auto ResponseMessage=fPushMessage;
	fPushMessage=nullptr;

	fPullMessageQueue.Enqueue(ResponseMessage);
	ResponseMessage->Responder->BodyFinish();
}
//---------------------------------------------------------------------------
void cHTTPServerSession::HTTPTerminate(cResponseMessage *ResponseMessage)
{
	fStopRequest=true;
	if(ResponseMessage!=fResponseMessage)
		return;
	Terminate();
}
//---------------------------------------------------------------------------
void cHTTPServerSession::HTTPNotifyPull(cResponseMessage *ResponseMessage)
{
	if(ResponseMessage!=fResponseMessage)
		return;

	NotifyPull();
}
//---------------------------------------------------------------------------
void cHTTPServerSession::HTTPEndPull(cResponseMessage *ResponseMessage)
{
	if(ResponseMessage->ShutdownAfterResponse){
		// stop further requests
		fStopRequest=true;
	}
	if(ResponseMessage!=fResponseMessage)
		return;

	fPullIdleNotify=true;
	NotifyPull();
}
//---------------------------------------------------------------------------
template<class T>
struct cArrayReadStream
{
	T *Pointer;
	uIntn Length;

	cArrayReadStream operator +(uIntn AdvanceLength)const
	{
		return cArrayReadStream{Pointer+AdvanceLength,Length-AdvanceLength};
	}
	cArrayReadStream& operator +=(uIntn AdvanceLength)
	{
		Pointer+=AdvanceLength;
		Length-=AdvanceLength;
		return *this;
	}
	cArrayReadStream operator -(uIntn AdvanceLength)const
	{
		return cArrayReadStream{Pointer-AdvanceLength,Length+AdvanceLength};
	}
	cArrayReadStream& operator -=(uIntn AdvanceLength)
	{
		Pointer-=AdvanceLength;
		Length+=AdvanceLength;
		return *this;
	}
};

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cHTTPServerManager::cHTTPServerManager()
{
	fSessionListLock=cnSystem::CreateResLock();
}
//---------------------------------------------------------------------------
cHTTPServerManager::~cHTTPServerManager()
{
}
//---------------------------------------------------------------------------
bool cHTTPServerManager::Start(iConnectionQueue *Port)
{
	if(fPort!=nullptr){
		return false;
	}
	fReference=Reference;
	fPort=Port;
	if(fPort->StartPush(this)==false){
		fPort=nullptr;
		fReference=nullptr;
		return false;
	}
	fCallback=Callback;

	fPort->ResumePush();
	return true;
}
//---------------------------------------------------------------------------
void cHTTPServerManager::Stop(void)
{
	if(fPort!=nullptr){
		fPort->StopPush();
	}
}
//---------------------------------------------------------------------------
void cHTTPServerManager::QueueStarted(void)
{
	if(Reference!=nullptr)
		rIncReference(Reference,'http');
}
//---------------------------------------------------------------------------
void cHTTPServerManager::QueueStopped(void)
{
	fQueue=nullptr;
	if(Reference!=nullptr)
		rDecReference(Reference,'http');
}
//---------------------------------------------------------------------------
bool cHTTPServerManager::ProcessQueue(void)
{
	auto Connection=fQueue->FetchConnection();
	auto Session=QuerySession();

	auto EndpointConnection=iCast<iEndpointConnection>(Connection);

	auto Endpoint=EndpointConnection->GetEndpoint();
	auto ProtocolHost=rCreate<cnNet::cProtocolHostFromEndpoint>();
	ProtocolHost->Connect(Endpoint);
	Session->Start(ProtocolHost);

	Endpoint->SetWriteEnd();
	return true;
}
//---------------------------------------------------------------------------
void cHTTPServerManager::QueueNotify(void)
{
	while(fQueue->IsClosed()==false){
		if(ProcessQueue()==false)
			break;
	}
}
//---------------------------------------------------------------------------
rPtr<cHTTPServerSession> cHTTPServerManager::QuerySession(void)
{
	return rCreate<cHTTPServerSession>();
}
//---------------------------------------------------------------------------
void cHTTPServerManager::HTTPSessionStarted(cHTTPServerSession *Session)
{
	if(fReference!=nullptr)
		rIncReference(fReference,'http');
	auto AutoLock=ScopeLock(fSessionListLock);

	fSessionList.Append(Session);
}
//---------------------------------------------------------------------------
void cHTTPServerManager::HTTPSessionStopped(cHTTPServerSession *Session)
{
	bool Deleted=false;
	{
		auto AutoLock=ScopeLock(fSessionListLock);
		for(uIntn i=0,c=fSessionList.GetCount();i<c;i++){
			if(Session==fSessionList[i]){
				fSessionList.Remove(i);
				Deleted=true;
				break;
			}
		}
	}
	if(Deleted){
		if(fReference!=nullptr)
			rDecReference(fReference,'http');
	}

}
//---------------------------------------------------------------------------
iPtr<iHTTPServerResponder> cHTTPServerManager::HTTPCreateResponder(iAddress *ClientAddress,const char *Method,uIntn MethodLength,const char *Path,uIntn PathLength,const char *Version,uIntn VersionLength)
{
	return fCallback->CreateResponder(ClientAddress,Method,MethodLength,Path,PathLength,Version,VersionLength);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cUHTTPManager::cUHTTPManager()
{
}
//---------------------------------------------------------------------------
cUHTTPManager::~cUHTTPManager()
{
}
//---------------------------------------------------------------------------
bool cUHTTPManager::Start(iReference *Reference,iCallback *Callback,iDatagramPort *Port)
{
	if(fPort!=nullptr)
		return false;
	fStopFlag.Release.Store(false);
	fPort=Port;
	fReference=Reference;
	if(fPort->StartManage(this,true)==false){
		fReference=nullptr;
		fPort=nullptr;
		return false;
	}
	fCallback=Callback;

	fPort->ResumePush(false);
	fPort->NotifyPull(false);
	return true;
}
//---------------------------------------------------------------------------
void cUHTTPManager::Stop(void)
{
	if(fStopFlag.Acquire.Xchg(true)==false){
		if(fPort!=nullptr){
			fPort->StopManage();
		}
	}
}
//---------------------------------------------------------------------------
bool cUHTTPManager::StartClient(iAddress *ServerAddress,iHTTPClientRequestor *Requestor,bool NoResponse)
{
	if(fPort==nullptr){
		return false;
	}
	auto RequestItem=new cRequestItem;
	RequestItem->Manager=this;
	RequestItem->Address=ServerAddress;
	RequestItem->Requestor=Requestor;
	RequestItem->NoResponse=NoResponse;

	Requestor->StartClient(RequestItem);
	return true;
}
//---------------------------------------------------------------------------
void cnLib_FUNC cUHTTPManager::PortStarted(void)
{
	if(fReference!=nullptr){
		rIncReference(fReference,'http');
	}
}
//---------------------------------------------------------------------------
void cnLib_FUNC cUHTTPManager::PortStopped(void)
{
	fPort=nullptr;
	fCallback=nullptr;
	if(fReference!=nullptr){
		rDecReference(fReference,'http');
	}
}
//---------------------------------------------------------------------------
void cnLib_FUNC cUHTTPManager::PortTerminate(void)
{
	Stop();
}
//---------------------------------------------------------------------------
void cnLib_FUNC cUHTTPManager::cResponseItem::HTTPTerminate(void)
{
}
//---------------------------------------------------------------------------
void cnLib_FUNC cUHTTPManager::cResponseItem::HTTPNotifyPull(void)
{
	if(InQueue)
		return;
	InQueue=true;
	if(Manager->fResponseQueue.Enqueue(this)){
		if(Manager->fPort!=nullptr){
			Manager->fPort->NotifyPull(false);
		}
	}
}
//---------------------------------------------------------------------------
void cnLib_FUNC cUHTTPManager::cResponseItem::HTTPEndPull(bool)
{
	ResponseEnded=true;
}
//---------------------------------------------------------------------------
void cUHTTPManager::ProcessRequest(iAddress *Address,const char *Text,uIntn Length)
{
	{// insert request to map
		auto Request=fPendingResquestQueue.DequeueAll();
		while(Request!=nullptr){

			fPendingRequestMap.Insert(Request->Address,Request->Requestor);

			auto Next=static_cast<cRequestItem*>(Request->Next);

			delete Request;
			Request=Next;
		}
	}
	// process as a request from client

	cArrayReadStream<const char> Stream{Text,Length};
	// method
	uIntn LineLength;
	if(HTTP::ParseLine(LineLength,Stream.Pointer,Stream.Length)==false){
		return;
	}

	uInt16 MsgPos[3];
	uInt16 MsgLen[3];
	auto MsgParts=HTTP::ParseHeaderMessage(MsgPos,MsgLen,3,Stream.Pointer,LineLength-2);
	if(MsgParts!=3){
		// bad request
		return;
	}
	static const char HTTPHeader[5]={'H','T','T','P','/'};
	if(MsgLen[0]>=5 && cnMemory::IsEqual(HTTPHeader,Stream.Pointer+MsgPos[0],5)){
		// process as a response from server
		auto WaitingRequest=fPendingRequestMap.GetPair(Address);
		if(WaitingRequest!=nullptr){
			// process as a response from server
			auto Client=MoveCast(WaitingRequest->Value);
			// remove request
			fPendingRequestMap.RemovePair(WaitingRequest);

			Client->StatusLine(Stream.Pointer+MsgPos[0],MsgLen[0],Stream.Pointer+MsgPos[1],MsgLen[1],Stream.Pointer+MsgPos[2],MsgLen[2]);

			Stream+=LineLength;
			// process response
			return ProcessResponse(Client,Stream.Pointer,Stream.Length);
		}
		else if(fCallback!=nullptr){
			auto Client=fCallback->UnknownResponse(Address);

			if(Client!=nullptr){
				Client->StatusLine(Stream.Pointer+MsgPos[0],MsgLen[0],Stream.Pointer+MsgPos[1],MsgLen[1],Stream.Pointer+MsgPos[2],MsgLen[2]);

				Stream+=LineLength;
				// process response
				return ProcessResponse(Client,Stream.Pointer,Stream.Length);
			}

		}
		// no response handler
		return;
	}

	if(fCallback==nullptr){
		// not service
		return;
	}
	auto Responder=fCallback->CreateResponder(Address,Stream.Pointer+MsgPos[0],MsgLen[0],Stream.Pointer+MsgPos[1],MsgLen[1],Stream.Pointer+MsgPos[2],MsgLen[2]);
	if(Responder==nullptr){
		// not supported request
		return;
	}
	auto ReplyItem=new cResponseItem;
	ReplyItem->Manager=this;
	ReplyItem->Responder=Responder;
	Responder->StartServer(ReplyItem);

	Stream+=LineLength;
	
	// headers

	while(HTTP::ParseLine(LineLength,Stream.Pointer,Stream.Length)){
		if(LineLength<=2){
			// header finished
			uIntn ExpectedBodySize=uintMax;
			Responder->HeaderFinish(ExpectedBodySize);
			// send body
			Responder->Body(Stream.Pointer,Stream.Length);


			// reply
			ReplyItem->Address=Address;
			ReplyItem->Responder=Responder;
			Responder->BodyFinish();

			return;
		}

		uInt16 NameLength;
		bool ValuePresent=HTTP::ParseHeaderParameter(NameLength,Stream.Pointer,LineLength-2);
		if(ValuePresent){
			uInt16 ValuePos=NameLength+1;
			uInt16 ValueLength=static_cast<uInt16>(LineLength-2-ValuePos);
			auto ValueString=Stream.Pointer+ValuePos;
			Responder->Parameter(Stream.Pointer,NameLength,ValueString,ValueLength);

		}
		else{
			Responder->Parameter(Stream.Pointer,NameLength,nullptr,0);
		}
		Stream+=LineLength;
	}

	// incompleted header
	// ignore
	Responder->Terminated();
}
//---------------------------------------------------------------------------
void cUHTTPManager::ProcessResponse(iHTTPClientRequestor *Requestor,const char *Text,uIntn Length)
{
	cArrayReadStream<const char> Stream{Text,Length};
	uIntn LineLength=0;
	// headers
	while(HTTP::ParseLine(LineLength,Stream.Pointer,Stream.Length)){
		if(LineLength<=2){
			// header finished
			uIntn ExpectedBodySize=uintMax;
			Requestor->HeaderFinish(ExpectedBodySize);
			// send body
			Requestor->Body(Stream.Pointer,Stream.Length);
			Requestor->BodyFinish();

			return;
		}

		uInt16 NameLength;
		bool ValuePresent=HTTP::ParseHeaderParameter(NameLength,Stream.Pointer,LineLength-2);
		if(ValuePresent){
			uInt16 ValuePos=NameLength+1;
			uInt16 ValueLength=static_cast<uInt16>(LineLength-2-ValuePos);
			auto ValueString=Stream.Pointer+ValuePos;
			Requestor->Parameter(Stream.Pointer,NameLength,ValueString,ValueLength);

		}
		else{
			Requestor->Parameter(Stream.Pointer,NameLength,nullptr,0);
		}
		Stream+=LineLength;
	}

	// incompleted header
	Requestor->Terminated();
}
//---------------------------------------------------------------------------
bool cnLib_FUNC cUHTTPManager::PushPacket(iAddress *Address,const void *Buffer,uIntn Size)
{
	ProcessRequest(Address,static_cast<const char*>(Buffer),Size);
	return true;
}
//---------------------------------------------------------------------------
bool cnLib_FUNC cUHTTPManager::PullPacket(iPtr<iAddress> &Address,void *Buffer,uIntn BufferSize,uIntn &SizeReturn)
{
	for(auto ResponseItem=fResponseQueue.Dequeue();ResponseItem!=nullptr;ResponseItem=fResponseQueue.Dequeue()){
		Address=ResponseItem->Address;
		auto Responder=ResponseItem->Responder;
		ResponseItem->InQueue=false;

		uIntn PullResponseSize=0;
		if(Responder->PullResponse(Buffer,BufferSize,PullResponseSize)){
			SizeReturn=PullResponseSize;
			if(ResponseItem->ResponseEnded==false){
				// notify this response will terminated
				Responder->Terminated();
			}
			delete ResponseItem;
			return true;
		}
		if(ResponseItem->ResponseEnded==false){
			if(PullResponseSize==0){
				// no data for now
			}
			else{
				// not enough buffer
				Responder->Terminated();
				delete ResponseItem;
			}
		}
		// pull next
	}
	for(auto RequestItem=fResquestQueue.Dequeue();RequestItem!=nullptr;RequestItem=fResquestQueue.Dequeue()){
		RequestItem->InQueue=false;
		auto Requestor=RequestItem->Requestor;

		uIntn PullQuerySize=0;
		if(Requestor->PullRequest(Buffer,BufferSize,PullQuerySize)){
			Address=RequestItem->Address;
			SizeReturn=PullQuerySize;
			return true;
		}
		if(PullQuerySize!=0){
			// not enough buffer
			Requestor->Terminated();
		}
		// pull next
	}
	return false;
}
//---------------------------------------------------------------------------
void cnLib_FUNC cUHTTPManager::cRequestItem::HTTPTerminate(void)
{
}
//---------------------------------------------------------------------------
void cnLib_FUNC cUHTTPManager::cRequestItem::HTTPNotifyPull(void)
{
	if(InQueue)
		return;
	InQueue=true;
	if(Manager->fResquestQueue.Enqueue(this)){
		if(Manager->fPort!=nullptr){
			Manager->fPort->NotifyPull(false);
		}
	}
}
//---------------------------------------------------------------------------
void cnLib_FUNC cUHTTPManager::cRequestItem::HTTPEndPull(bool)
{
	if(NoResponse){
		//delete this;
		return;
	}
	// waiting for response
	RequestEnded=true;
	Manager->fPendingResquestQueue.Enqueue(this);
}
//---------------------------------------------------------------------------
#endif // 0
