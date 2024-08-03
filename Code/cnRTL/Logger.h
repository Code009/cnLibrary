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
class cLogMessageRecord : public iReference
{
public:
	ufInt8 Level;
	uInt64 Time;
	cString<uChar16> Path;
	cStringBuffer<uChar16> TextBuffer;
};
//---------------------------------------------------------------------------
class cLogMessageQueue
{
public:
	void Reset(void)noexcept(true);
	void Submit(rPtr<cLogMessageRecord> Record)noexcept(true);
	void Async(iAsyncExecution *Execution)noexcept(true);
	void Connect(iLogRecorder *Recorder)noexcept(true);

	template<bool Enable>
	struct tLogFunction;
private:

	struct cMsgItem : cRTLAllocator
	{
		cMsgItem *Next;
		rPtr<cLogMessageRecord> Record;
	};

	cExclusiveFlag fRecordExclusive;
	bool fNeedReplaceRecorder=false;
	bool fNeedAsync=false;
	bool fAsyncInitialized=false;
	uInt8 fRecordMissingCount=0;
	bool fAsyncWaitFlag=false;
	cAtomicVar<ufInt32> fAsyncRefCount=0;
	cAtomicQueueSO<cMsgItem> fMsgQueue;
	cAtomicStack<cMsgItem> fMsgRecycler;
	cAtomicVar<iLogRecorder*> fReplaceRecorder=nullptr;
	cAtomicVar<iAsyncExecution*> fReplaceAsyncExecution=nullptr;
	iLogRecorder *fRecorder=nullptr;
	iAsyncProcedure *fAsyncWork=nullptr;

	class cAsyncContext : public iReference, public iProcedure
	{
	public:
		virtual void cnLib_FUNC IncreaseReference(void)noexcept(true)override;
		virtual void cnLib_FUNC DecreaseReference(void)noexcept(true)override;

		virtual void cnLib_FUNC Execute(void)noexcept(true)override;
	
		iThread *AsyncWaitThread=nullptr;
	};
	cnVar::cStaticVariable<cAsyncContext> fAsyncContext;

	void ProcessMsgQueue(void)noexcept(true);
	void ProcessMsgQueueThread(void)noexcept(true);
	void ProcessUpdateAsync(void)noexcept(true);
	void ProcessUpdateRecorder(void)noexcept(true);
	void SubmitMsgRecord(cLogMessageRecord *Record)noexcept(true);
};
//---------------------------------------------------------------------------
class bcLogRecordSubmission
{
public:
	bcLogRecordSubmission(cLogMessageQueue *Queue,rPtr<cLogMessageRecord> Record)noexcept(true);
	~bcLogRecordSubmission()noexcept(true);
protected:
	cLogMessageQueue *fQueue;
	rPtr<cLogMessageRecord> fRecord;
};
//---------------------------------------------------------------------------
class cLogStreamBuffer : public bcLogRecordSubmission
{
public:
	cLogStreamBuffer(cLogMessageQueue *Queue,rPtr<cLogMessageRecord> Record)noexcept(true);
	~cLogStreamBuffer()noexcept(true);

	void Reset(void)noexcept(true);

	typedef uChar16 tElement;

	cArray<uChar16> ReserveWriteBuffer(uIntn QueryLength)noexcept(true);
	void CommitWriteBuffer(uIntn Length)noexcept(true);


protected:
	cnStream::cStringStorageStreamWriteBuffer<cAllocationOperator,uChar16> fWriteBuffer;
};
//---------------------------------------------------------------------------
class cLogVoidStreamBuffer : public cnStream::cVoidStreamBuffer<uChar16>
{
public:
	void Reset(void)noexcept(true){}
};
//---------------------------------------------------------------------------
template<bool LogEnabled>
struct cLogMessageQueue::tLogFunction
{
	typedef cLogStreamBuffer tLogBuffer;
	template<class TPath>
	static cLogStreamBuffer MakeBuffer(cLogMessageQueue *Queue,ufInt8 Level,TPath&& Path)noexcept(true){
		auto Record=rQuerySharedObject<cLogMessageRecord>();
		Record->Level=Level;
		Record->Path=cnVar::Forward<TPath>(Path);
		Record->TextBuffer.Clear();
		return cLogStreamBuffer(Queue,cnVar::MoveCast(Record));
	}

	template<class TPath,class...TArgs>
	static void Report(cLogMessageQueue *Queue,ufInt8 Level,TPath &&Path,const uChar16 *FormatString,TArgs&&...Args)noexcept(true){
		auto LogBuffer=MakeBuffer(Queue,Level,cnVar::Forward<TPath>(Path));
		StringStream::WriteFormatString(LogBuffer,FormatString,cnVar::Forward<TArgs>(Args)...);
	}

};
//---------------------------------------------------------------------------
template<>
struct cLogMessageQueue::tLogFunction<false>
{
	typedef cLogVoidStreamBuffer tLogBuffer;
	template<class TPath>
	static cLogVoidStreamBuffer MakeBuffer(cLogMessageQueue*,ufInt8,TPath&&)noexcept(true){
		return cLogVoidStreamBuffer();
	}

	template<class TPath,class...TArgs>
	static void Report(cLogMessageQueue*,ufInt8,TPath &&,const uChar16 *,TArgs&&...)noexcept(true){}
};
//---------------------------------------------------------------------------
template<uIntn LogLevel>
class cLog : public cLogMessageQueue
{
public:
	template<uIntn Level,class TPath>
	typename cLogMessageQueue::tLogFunction<LogLevel<=Level>::tLogBuffer MakeLogBuffer(TPath &&Path)noexcept(true){
		return cLogMessageQueue::tLogFunction<LogLevel<=Level>::MakeBuffer(this,Level,cnVar::Forward<TPath>(Path));
	}

	template<uIntn Level,class TPath,class...TArgs>
	void Report(TPath &&Path,const uChar16 *FormatString,TArgs&&...Args)noexcept(true){
		return cLogMessageQueue::tLogFunction<LogLevel<=Level>::Report(this,Level,cnVar::Forward<TPath>(Path),FormatString,cnVar::Forward<TArgs>(Args)...);
	}
};
//---------------------------------------------------------------------------
class cLogRecordHub : public iLogRecorder
{
public:
	cLogRecordHub()noexcept(true);
	~cLogRecordHub()noexcept(true);

	virtual bool Acquire(void *Owner)noexcept(true)override;
	virtual bool Release(void *Owner)noexcept(true)override;
	virtual void Submit(iReference *Reference,const cLogMessage &Message)noexcept(true)override;

	void InsertRecorder(iLogRecorder *Recorder)noexcept(true);
	void RemoveRecorder(iLogRecorder *Recorder)noexcept(true);
	void ClearRecorder(void)noexcept(true);
	void SubmitMessage(iReference *Reference,const cLogMessage &Message)noexcept(true);

private:
	cAtomicVar<void*> fOwner;
	cSeqSet< rPtr<iLogRecorder> > fRecorderSet;
};
//---------------------------------------------------------------------------
class cTextStreamOutputLogRecorder : public iLogRecorder
{
public:
	cTextStreamOutputLogRecorder(iTextStreamOutput *Output)noexcept(true);
	virtual bool Acquire(void *Owner)noexcept(true)override;
	virtual bool Release(void *Owner)noexcept(true)override;
	virtual void Submit(iReference *Reference,const cLogMessage &Message)noexcept(true)override;
protected:
	rPtr<iTextStreamOutput> fOutput;
	cAtomicVar<void*> fOwner;
};
//---------------------------------------------------------------------------
#ifndef cnRTL_LOGLEVEL

#ifdef cnLib_DEBUG
#define	cnRTL_LOGLEVEL	0
#else
#define	cnRTL_LOGLEVEL	1
#endif // cnLib_DEBUG

#endif // !cnRTL_LOGLEVEL
//---------------------------------------------------------------------------
extern cLog<cnRTL_LOGLEVEL> gRTLLog;
//---------------------------------------------------------------------------
}   // namespace cnRTL
//---------------------------------------------------------------------------
}   // namespace cnLibrary
//---------------------------------------------------------------------------
#endif  /* __cplusplus */
/*-------------------------------------------------------------------------*/
#endif
