//---------------------------------------------------------------------------
#include "Logger.h"
#include "TextProcess.h"

using namespace cnLibrary;
using namespace cnRTL;


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
cLogStream::cLogStream(bcLog *Log,uInt8 Level,cString<uChar16> Path)noexcept
	: fLog(Log)
	, fLevel(Level)
	, fPath(cnVar::MoveCast(Path))
{
}
//---------------------------------------------------------------------------
cnStream::cStringStorageStreamWriteBuffer<cAllocationOperator,uChar16> cLogStream::StreamWriteBuffer(void)noexcept
{
	return fTextBuffer.StreamWriteBuffer();
}
//---------------------------------------------------------------------------
void cLogStream::CommitWriteBuffer(void)noexcept
{
	return Submit(cnVar::MoveCast(fTextBuffer));
}
//---------------------------------------------------------------------------
void cLogStream::Submit(cString<uChar16> Text)noexcept
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
		ArrayStream::WriteArray(WriteBuffer,Message->Path->Pointer,Message->Path->Length);
		StringStream::WriteFormatString(WriteBuffer,u" %.2d:%.2d:%.2d.%.3d - (%d) ",Date.Hour,Date.Minute,Date.Second,(Date.NanoSecond/Time_1ms),Message->Level);
		ArrayStream::WriteArray(WriteBuffer,Message->Text->Pointer,Message->Text->Length);
		ArrayStream::WriteElement(WriteBuffer,'\n');
	}
	fOutput->Print(Text->Pointer,Text->Length);
}
//---------------------------------------------------------------------------
