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
template<class TError>
struct TErrorClassName{};
//---------------------------------------------------------------------------
template<class TStringPointer,class TError>
struct TErrorTypeWriteDescription
{
	static bool cnLib_FUNC Function(iWriteBuffer<uChar16> *WriteBuffer)noexcept(true)
	{
		*WriteBuffer+=ArrayStreamString(u"TypeID(");
		auto TypeID=cnVar::TTypeID<TError>::Value;
		if(StringStream::WriteFormatInt<16>(*WriteBuffer,reinterpret_cast<uIntn>(TypeID),sizeof(uIntn)*2,0)==false)
			return false;
		return *WriteBuffer+=ArrayStreamString(u")");
	}
};
//---------------------------------------------------------------------------
template<class TError>
struct TErrorTypeWriteDescription<decltype(TErrorClassName<TError>::Value+0),TError>
{
	static bool cnLib_FUNC Function(iWriteBuffer<uChar16> *WriteBuffer)noexcept(true)
	{
		return *WriteBuffer+=ArrayStreamArray(TErrorClassName<TError>::Value,cnMemory::ArrayLength(TErrorClassName<TError>::Value)-1);
	}
};
//---------------------------------------------------------------------------
template<class TError>
struct TErrorWriteDescription
{
	static bool cnLib_FUNC Function(iWriteBuffer<uChar16> *WriteBuffer,const void *Data)noexcept(true)
	{
		return StringStream::WriteFormatInt<10>(*WriteBuffer,reinterpret_cast<uIntn>(Data),0,0);
	}
};
//---------------------------------------------------------------------------

class cErrorFrame
{
public:
	cErrorFrame(const uChar16 *Function,uIntn Length)noexcept(true)
		: fMaker(Make(Function,Length)){}

	~cErrorFrame()noexcept(true);

	template<uIntn Length>
	cErrorFrame(const uChar16 (&Function)[Length])noexcept(true)
		: fMaker(Make(Function,Length-1)){}

	template<uIntn Length>
	cErrorFrame(uChar16 (&Function)[Length])noexcept(true)
		: fMaker(Make(Function,cnString::FindLength(Function,Length))){}

	void SetMsg(const uChar16 *Text,uIntn Length)noexcept(true);
	template<uIntn Length>
	void SetMsg(const uChar16 (&Text)[Length])noexcept(true)
	{
		return SetMsg(Text,Length-1);
	}

	template<uIntn Length>
	void SetMsg(uChar16 (&Text)[Length])noexcept(true)
	{
		return SetMsg(Text,cnString::FindLength(Text,Length));
	}

	template<class TError>
	void operator +=(TError Error){
		cErrorInfo ei;
		ei.ErrorTypeID=cnVar::TTypeID<TError>::Value;
		ei.Code=static_cast<uIntn>(Error);
		ei.DataLength=0;
		ei.WriteTypeDescription=TErrorTypeWriteDescription<const uChar16*,TError>::Function;
		ei.WriteDescription=TErrorWriteDescription<TError>::Function;
		fMaker.Append(&ei);
	}


protected:
	cnSystem::ErrorReportMaker fMaker;

	static cnSystem::ErrorReportMaker Make(const uChar16 *Function,uIntn Length)noexcept(true);
};
//---------------------------------------------------------------------------

class cErrorReport
{
public:
	void Retrieve(void)noexcept(true);

	template<class T>
	bool Query(T &Error)noexcept(true){
		union{
			uIntn Code;
			const void *Data;
		};
		if(Query(cnVar::TTypeID<T>::Value,Data)){
			Error=static_cast<T>(Code);
			return true;
		}
		return false;
	}
	template<class T>
	bool Query(const T* &Error)noexcept(true){
		return Query(cnVar::TTypeID<T>::Value,reinterpret_cast<const void* &>(Error));
	}
	
	bool Query(tTypeID ErrorTypeID,const void* &Data)noexcept(true);

	cStringBuffer<uChar16> Descripe(void)noexcept(true);
protected:
	rPtr<iErrorReport> fReport;

	static void WriteError(iWriteBuffer<uChar16> *WriteBuffer,iErrorReport *Report)noexcept(true);
};
//---------------------------------------------------------------------------
}	// namespace cnRTL
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
class cLogBuffer
{
public:
	cLogBuffer(bcLog *Log,ufInt8 Level,cString<uChar16> Path)noexcept(true);

	cnStream::cStringStorageStreamWriteBuffer<cAllocationOperator,uChar16> Stream(void)noexcept(true);
	void Reset(void)noexcept(true);
	void Commit(void)noexcept(true);

	template<class...TArgs>
	void Report(const uChar16 *FormatString,TArgs&&...Args)noexcept(true){
		fTextBuffer.Clear();
		StringStream::WriteFormatString(fTextBuffer.StreamWriteBuffer(),FormatString,cnVar::Forward<TArgs>(Args)...);
		return Commit();
	}

protected:
	bcLog *fLog;
	ufInt8 fLevel;
	cString<uChar16> fPath;
	cStringBuffer<uChar16> fTextBuffer;

	void Submit(cString<uChar16> Text)noexcept(true);

};
//---------------------------------------------------------------------------
class cLogVoidBuffer
{
public:
	cLogVoidBuffer(bcLog*,ufInt8,const uChar16*)noexcept(true){}

	cnStream::cVoidStreamBuffer<uChar16> Stream(void)noexcept(true){	return cnStream::VoidStreamWriteBuffer<uChar16>();	}
	void Reset(void)noexcept(true){}
	void Commit(void)noexcept(true){}

	template<class...TArgs>	void Report(const uChar16*,TArgs&&...)noexcept(true){}
};
//---------------------------------------------------------------------------
template<uIntn LogLevel>
class cLog : public bcLog
{
public:
	cLog()noexcept(true){}
	~cLog()noexcept(true){}

	template<uIntn Level,class TPath>
	typename cnVar::TSelect<Level<LogLevel,cLogBuffer,cLogVoidBuffer>::Type MakeLogBuffer(TPath &&Path)noexcept(true){
		return {this,Level,cnVar::MoveCast(Path)};
	}

	template<uIntn Level,class TPath,class...TArgs>
	void Report(TPath &&Path,const uChar16 *FormatString,TArgs&&...Args)noexcept(true){
		typename cnVar::TSelect<Level<LogLevel,cLogBuffer,cLogVoidBuffer>::Type LogBuffer(this,Level,cnVar::MoveCast(Path));
		return LogBuffer.Report(FormatString,cnVar::Forward<TArgs>(Args)...);
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
