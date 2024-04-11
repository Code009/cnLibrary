/*- cnRTL - Logger --------------------------------------------------------*/
/*         Developer : Code009                                             */
/*         Create on : 2007-08-14                                          */
/*-------------------------------------------------------------------------*/
#ifndef __cnLibrary_cnRTL_Logger_H__
#define __cnLibrary_cnRTL_Logger_H__
/*-------------------------------------------------------------------------*/
#include <cnRTL/cnRTLCore.h>
#include <cnRTL/RuntimeFunction.h>
#include <cnRTL/String.h>
#include <cnRTL/ObjectReference.h>

#ifdef __cplusplus
//---------------------------------------------------------------------------
namespace cnLibrary{
//---------------------------------------------------------------------------
namespace cnRTL{
//---------------------------------------------------------------------------
struct cLogMessage
{
	uInt8 Level;
	uInt64 Time;
	cString<uChar16> Path;
	cString<uChar16> Text;
};
//---------------------------------------------------------------------------
class cnLib_INTERFACE iLogRecorder : public iReference
{
public:
	virtual void Write(aClsConstRef<cLogMessage> Message)noexcept(true)=0;
};
//---------------------------------------------------------------------------
class bcLog
{
public:
	bcLog()noexcept(true);
	~bcLog()noexcept(true);

	void InsertRecorder(iLogRecorder *Recorder)noexcept(true);
	void RemoveRecorder(iLogRecorder *Recorder)noexcept(true);
	void ClearRecorder(void)noexcept(true);
	void Submit(aClsConstRef<cLogMessage> Message)noexcept(true);
protected:
	rPtr<iSharedMutex> fRecordListLock;
	cSeqSet< rPtr<iLogRecorder> > fRecordList;
};
//---------------------------------------------------------------------------
class cLogStream
{
public:
	cLogStream(bcLog *Log,cString<uChar16> Path)noexcept(true);

	cnStream::cStringStorageStreamWriteBuffer<cAllocationOperator,uChar16> StreamWriteBuffer(void)noexcept(true);
	void CommitWriteBuffer(void)noexcept(true);

	template<class...TArgs>
	void Report(const uChar16 *FormatString,TArgs&&...Args)noexcept(true){
		fTextBuffer.Clear();
		StringStream::WriteFormatString(fTextBuffer.StreamWriteBuffer(),FormatString,cnVar::Forward<TArgs>(Args)...);
		return CommitWriteBuffer();
	}

protected:
	bcLog *fLog;
	cString<uChar16> fPath;
	cStringBuffer<uChar16> fTextBuffer;

	void Submit(cString<uChar16> Text)noexcept(true);

};
//---------------------------------------------------------------------------
class cLogVoidStream
{
public:
	cLogVoidStream(bcLog*,const uChar16*)noexcept(true){}

	cnStream::cVoidStreamBuffer<uChar16> StreamWriteBuffer(void)noexcept(true){	return cnStream::VoidStreamWriteBuffer<uChar16>();	}
	void CommitWriteBuffer(void)noexcept(true){}

	template<class...TArgs>	void Report(const uChar16*,TArgs&&...)noexcept(true){}
};
//---------------------------------------------------------------------------
template<int LogLevel>
class cLog : public bcLog
{
public:
	cLog()noexcept(true){}
	~cLog()noexcept(true){}

	template<uIntn Level,class TPath>
	typename cnVar::TSelect<Level<LogLevel,cLogStream,cLogVoidStream>::Type MakeLogStream(TPath &&Path)noexcept(true){
		return {this,cnVar::MoveCast(Path)};
	}

	template<uIntn Level,class TPath,class...TArgs>
	void Report(TPath &&Path,const uChar16 *FormatString,TArgs&&...Args)noexcept(true){
		typename cnVar::TSelect<Level<LogLevel,cLogStream,cLogVoidStream>::Type LogStream(this,cnVar::MoveCast(Path));
		return LogStream.Report(FormatString,cnVar::Forward<TArgs>(Args)...);
	}
};
//---------------------------------------------------------------------------
class cTextStreamOutputLogRecorder : public iLogRecorder
{
public:
	cTextStreamOutputLogRecorder(iTextStreamOutput *Output)noexcept(true);
	virtual void Write(aClsConstRef<cLogMessage> Message)noexcept(true)override;
protected:
	rPtr<iTextStreamOutput> fOutput;
};
//---------------------------------------------------------------------------
}   // namespace cnRTL
//---------------------------------------------------------------------------
}   // namespace cnLibrary
//---------------------------------------------------------------------------
#endif  /* __cplusplus */
/*-------------------------------------------------------------------------*/
#endif
