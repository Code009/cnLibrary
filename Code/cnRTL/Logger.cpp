//---------------------------------------------------------------------------
#include "Logger.h"
#include "TextProcess.h"

using namespace cnLibrary;
using namespace cnRTL;

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cTextStreamOutputLogger::cTextStreamOutputLogger(iTextStreamOutput *Output)
	: fOutput(Output)
{
}
//---------------------------------------------------------------------------
void cTextStreamOutputLogger::Log(aClsConstRef<cLogContent> Content)
{
	cStringBuffer<uChar16> Text;
	auto Converter=UnicodeTranscoder(1,2);
	{
		auto WriteBuffer=Text.StreamWriteBuffer();
		StringStream::WriteConvertEncoding(WriteBuffer,Converter,Content->Name->Pointer,Content->Name->Length);
		ArrayStream::WriteArray(WriteBuffer,u" - (",4);
		StringStream::WriteFormatInt(WriteBuffer,Content->Level,10,1,0);
		ArrayStream::WriteElement(WriteBuffer,')');
		StringStream::WriteConvertEncoding(WriteBuffer,Converter,Content->Type->Pointer,Content->Type->Length);
		ArrayStream::WriteArray(WriteBuffer,u" : ",3);
		StringStream::WriteConvertEncoding(WriteBuffer,Converter,Content->Text->Pointer,Content->Text->Length);
		ArrayStream::WriteElement(WriteBuffer,'\n');
	}
	fOutput->Print(Text->Pointer,Text->Length);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cLoggerSwitch::cLoggerSwitch()
{
	fLoggerListLock=cnSystem::CreateSharedLock();
}
//---------------------------------------------------------------------------
cLoggerSwitch::~cLoggerSwitch()
{
}
//---------------------------------------------------------------------------
void cLoggerSwitch::InsertLogger(iLogger *Logger)
{
	auto Lock=TakeLock(fLoggerListLock);
	fLoggerList.Insert(Logger);
}
//---------------------------------------------------------------------------
void cLoggerSwitch::RemoveLogger(iLogger *Logger)
{
	auto Lock=TakeLock(fLoggerListLock);
	fLoggerList.Remove(Logger);
}
//---------------------------------------------------------------------------
void cLoggerSwitch::ClearLogger(void)
{
	auto Lock=TakeLock(fLoggerListLock);
	fLoggerList.Clear();
}
//---------------------------------------------------------------------------
bool cLoggerSwitch::IsEmpty(void)const
{
	auto Lock=TakeSharedLock(fLoggerListLock);
	return fLoggerList.GetCount()==0;
}
//---------------------------------------------------------------------------
void cLoggerSwitch::Log(aClsConstRef<cLogContent> Content)
{
	auto Lock=TakeSharedLock(fLoggerListLock);
	for(auto &p : fLoggerList){
		p->Log(Content);
	}
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cLogWriter::cLogWriter()noexcept(true)
{
	fName=u"LogWriter"_cArray;
}
//---------------------------------------------------------------------------
cLogWriter::~cLogWriter()noexcept(true)
{
}
//---------------------------------------------------------------------------
iLogger* cLogWriter::GetLogger(void)const
{
	return fLogger;
}
//---------------------------------------------------------------------------
void cLogWriter::SetLogger(iLogger *Logger)
{
	fLogger=Logger;
}
//---------------------------------------------------------------------------
const cString<uChar16>& cLogWriter::GetName(void)const
{
	return fName;
}
//---------------------------------------------------------------------------
void cLogWriter::SetName(cString<uChar16> Name)
{
	fName=Name;
}
//---------------------------------------------------------------------------
void cLogWriter::Write(uInt8 Level,cString<uChar16> Text)
{
	if(fLogger==nullptr)
		return;
	auto Content=aClsCreate<cLogContent>();
	Content->Level=Level;
	Content->Name=fName;
	Content->Type=u"Log"_cArray;
	Content->Text=MoveCast(Text);
	Content->Time.SetTimeNow();

	fLogger->Log(MoveCast(Content));
}
//---------------------------------------------------------------------------
void cLogWriter::Write(uInt8 Level,cString<uChar16> Type,cString<uChar16> Text)
{
	if(fLogger==nullptr)
		return;
	auto Content=aClsCreate<cLogContent>();
	Content->Level=Level;
	Content->Name=fName;
	Content->Type=cnVar::MoveCast(Type);
	Content->Text=cnVar::MoveCast(Text);
	Content->Time.SetTimeNow();

	fLogger->Log(MoveCast(Content));
}
//---------------------------------------------------------------------------
cLogWriter::cTextBlock cLogWriter::Block(uInt8 Level)
{
	return {fLogger,Level,fName,u"Log"_cArray};
}
//---------------------------------------------------------------------------
cLogWriter::cTextBlock cLogWriter::Block(uInt8 Level,cString<uChar16> Type)
{
	return {fLogger,Level,fName,Type};
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cLogWriter::cTextBlock::cTextBlock(iLogger *Logger,uInt8 Level,cString<uChar16> Name,cString<uChar16> Type)
	: fLogger(Logger)
{
	fContent=aClsCreate<cLogContent>();
	fContent->Level=Level;
	fContent->Name=cnVar::MoveCast(Name);
	fContent->Type=cnVar::MoveCast(Type);
}
//---------------------------------------------------------------------------
cLogWriter::cTextBlock::~cTextBlock()
{
}
//---------------------------------------------------------------------------
void cLogWriter::cTextBlock::Commit(void)
{
	if(fLogger==nullptr)
		return;
	if(fContent==nullptr)
		return;
	fContent->Text=cnVar::MoveCast(fText);
	fContent->Time.SetTimeNow();

	fLogger->Log(cnVar::MoveCast(fContent));
}
//---------------------------------------------------------------------------
