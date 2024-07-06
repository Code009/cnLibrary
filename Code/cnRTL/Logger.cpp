//---------------------------------------------------------------------------
#include "Logger.h"
#include "TextProcess.h"
#include "StreamBuffer.h"

using namespace cnLibrary;
using namespace cnRTL;


//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cErrorFrame::~cErrorFrame()noexcept
{
	fMaker.Submit();
}
//---------------------------------------------------------------------------
void cErrorFrame::SetMsg(const uChar16 *Text,uIntn Length)noexcept
{
	fMaker.SetMessage(Text,Length);
}
//---------------------------------------------------------------------------
cnSystem::ErrorReportMaker cErrorFrame::Make(const uChar16 *Function,uIntn Length)noexcept
{
	// clear error in tls
	auto Report=cnSystem::ErrorReportManager::Fetch();	
	if(Report!=nullptr){
		// TODO : report ignored error report
	}

	return cnSystem::ErrorReportManager::MakeReport(Function,Length);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void cErrorReport::Retrieve(void)noexcept
{
	fReport=cnSystem::ErrorReportManager::Fetch();
}
//---------------------------------------------------------------------------
bool cErrorReport::Query(tTypeID ErrorTypeID,const void* &Data)noexcept(true){
	if(fReport==nullptr)
		return false;

	auto Errors=fReport->Errors();
	for(uIntn i=0;i<Errors.Length;i++){
		auto ErrorData=Errors.Pointer[i];
			
		if(ErrorData.ErrorTypeID==ErrorTypeID){
			Data=ErrorData.Data;
			return true;
		}
	}
	return false;
}
//---------------------------------------------------------------------------
void cErrorReport::WriteError(iWriteBuffer<uChar16> *WriteBuffer,iErrorReport *Report)noexcept
{
	auto Function=Report->FunctionName();
	auto Message=Report->ErrorMessage();
	auto Errors=Report->Errors();

	*WriteBuffer+=ArrayStreamArray(Function);
	*WriteBuffer+=ArrayStreamString(u" reports: ");
	*WriteBuffer+=ArrayStreamArray(Message);
	if(Errors.Length!=0){
		*WriteBuffer+=ArrayStreamString(u"\n errors: ");

		for(uIntn i=0;i<Errors.Length;i++){
			auto Error=Errors.Pointer[i];

			*WriteBuffer+=ArrayStreamElement(u'[');
			Error.WriteTypeDescription(WriteBuffer);
			*WriteBuffer+=ArrayStreamElement(u']');
			Error.WriteDescription(WriteBuffer,Error.Data);
			*WriteBuffer+=ArrayStreamString(u"; ");
		}
	}
}
//---------------------------------------------------------------------------
cStringBuffer<uChar16> cErrorReport::Descripe(void)noexcept
{
	cStringBuffer<uChar16> Desc;
	if(fReport!=nullptr){
		auto WriteBuffer=Desc.StreamWriteBuffer();
		auto WriteBufferInterface=WriteBufferFromStreamBuffer(&WriteBuffer);
		WriteError(&WriteBufferInterface,fReport);
		iErrorReport *CurReport=fReport->ParentReport();
		uIntn InnerReportIndex=0;
		while(CurReport!=nullptr){
			InnerReportIndex++;
			WriteBuffer+=StringStreamFormat(u"\n(%d) ",InnerReportIndex);
			WriteError(&WriteBufferInterface,CurReport);
			CurReport=CurReport->ParentReport();
		}
	}

	return Desc;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
bcLog::bcLog()noexcept
{
	fRecordListLock=cnSystem::CreateSharedLock();
}
//---------------------------------------------------------------------------
bcLog::~bcLog()noexcept
{
}
//---------------------------------------------------------------------------
void bcLog::InsertRecorder(iLogRecorder *Recorder)noexcept
{
	auto Lock=TakeLock(fRecordListLock);
	fRecordList.Insert(Recorder);
}
//---------------------------------------------------------------------------
void bcLog::RemoveRecorder(iLogRecorder *Recorder)noexcept
{
	auto Lock=TakeLock(fRecordListLock);
	fRecordList.Remove(Recorder);
}
//---------------------------------------------------------------------------
void bcLog::ClearRecorder(void)noexcept
{
	auto Lock=TakeLock(fRecordListLock);
	fRecordList.Clear();
}
//---------------------------------------------------------------------------
void bcLog::Submit(aClsConstRef<cLogMessage> Message)noexcept
{
	auto Lock=TakeSharedLock(fRecordListLock);
	for(auto &p : fRecordList){
		p->Write(Message);
	}
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cLogBuffer::cLogBuffer(bcLog *Log,uInt8 Level,cString<uChar16> Path)noexcept
	: fLog(Log)
	, fLevel(Level)
	, fPath(cnVar::MoveCast(Path))
{
}
//---------------------------------------------------------------------------
cnStream::cStringStorageStreamWriteBuffer<cAllocationOperator,uChar16> cLogBuffer::Stream(void)noexcept
{
	return fTextBuffer.StreamWriteBuffer();
}
//---------------------------------------------------------------------------
void cLogBuffer::Reset(void)noexcept
{
	fTextBuffer.Clear();
}
//---------------------------------------------------------------------------
void cLogBuffer::Commit(void)noexcept
{
	return Submit(cnVar::MoveCast(fTextBuffer));
}
//---------------------------------------------------------------------------
void cLogBuffer::Submit(cString<uChar16> Text)noexcept
{
	auto LogMessage=aClsCreate<cLogMessage>();
	LogMessage->Level=fLevel;
	LogMessage->Time=cnSystem::GetSystemTimeNow();
	LogMessage->Path=fPath;
	LogMessage->Text=cnVar::MoveCast(Text);
	fLog->Submit(LogMessage);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cTextStreamOutputLogRecorder::cTextStreamOutputLogRecorder(iTextStreamOutput *Output)noexcept
	: fOutput(Output)
{
}
//---------------------------------------------------------------------------
void cTextStreamOutputLogRecorder::Write(aClsConstRef<cLogMessage> Message)noexcept
{
	cStringBuffer<uChar16> Text;
	{
		cDateTime Date;
		cnSystem::UTCGregorianDateFromSystemTime(Date,Message->Time);		
		auto WriteBuffer=Text.StreamWriteBuffer();
		StringStream::WriteFormatString(WriteBuffer,u" %.2d:%.2d:%.2d.%.3d - (%d) ",Date.Hour,Date.Minute,Date.Second,(Date.NanoSecond/Time_1ms),Message->Level);
		ArrayStream::WriteArray(WriteBuffer,Message->Path->Pointer,Message->Path->Length);
		ArrayStream::WriteElement(WriteBuffer,':');
		ArrayStream::WriteElement(WriteBuffer,' ');
		ArrayStream::WriteArray(WriteBuffer,Message->Text->Pointer,Message->Text->Length);
		ArrayStream::WriteElement(WriteBuffer,'\n');
	}
	fOutput->Print(Text->Pointer,Text->Length);
}
//---------------------------------------------------------------------------
