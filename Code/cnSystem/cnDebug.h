/*- cnSystem - Debug ------------------------------------------------------*/
/*         Developer : Code009                                             */
/*         Create on : 2016-09-08                                          */
/*-------------------------------------------------------------------------*/
#ifndef __cnLibrary_cnSystem_cnDebug_H__
#define	__cnLibrary_cnSystem_cnDebug_H__
/*-------------------------------------------------------------------------*/
#include <cnSystem/cnFundamental.h>
#include <cnSystem/cnData.h>
/*-------------------------------------------------------------------------*/
#ifdef __cplusplus
//---------------------------------------------------------------------------
namespace cnLibrary{
//---------------------------------------------------------------------------
struct cLogMessage
{
	uInt8 Level;
	uInt64 Time;
	cArray<uChar16> Path;
	cArray<uChar16> Text;
};
//---------------------------------------------------------------------------
class cnLib_INTERFACE iLogRecorder : public iReference
{
public:
	virtual bool Acquire(void *Owner)noexcept(true)=0;
	virtual bool Release(void *Owner)noexcept(true)=0;
	
	// Submit
	//	free single threaded
	virtual void Submit(iReference *Reference,const cLogMessage &Message)noexcept(true)=0;
};
//---------------------------------------------------------------------------
struct cErrorInfo
{
	union{
		uIntn Code;
		const void *Data;
	};
	uIntn DataLength;
	const void *ErrorTypeID;
	bool (cnLib_FUNC *WriteTypeDescription)(iWriteBuffer<uChar16> *ErrorBuffer)noexcept(true);
	bool (cnLib_FUNC *WriteDescription)(iWriteBuffer<uChar16> *ErrorBuffer,const void *Data)noexcept(true);
};
//---------------------------------------------------------------------------
class iErrorReport : public iReference
{
public:
	virtual iErrorReport* cnLib_FUNC ParentReport(void)noexcept(true)=0;
	virtual cArray<const uChar16> cnLib_FUNC FunctionName(void)noexcept(true)=0;
	virtual cArray<const uChar16> cnLib_FUNC ErrorMessage(void)noexcept(true)=0;
	virtual cArray<const cErrorInfo> cnLib_FUNC Errors(void)noexcept(true)=0;
};
//---------------------------------------------------------------------------
namespace cnSystem{
//---------------------------------------------------------------------------
void AssertionMessage(const char *Message)noexcept(true);
void LogConnectRecorder(iLogRecorder *Recorder)noexcept(true);
//---------------------------------------------------------------------------
class ErrorReportMaker
{
public:
	ErrorReportMaker(){}
	ErrorReportMaker(rPtr<iErrorReport> Report) : fReport(cnVar::MoveCast(Report)){}

	void cnLib_FUNC Submit(void)noexcept(true);
	void cnLib_FUNC SetMessage(const uChar16 *Name,uIntn Length)noexcept(true); 
	void cnLib_FUNC Append(const cErrorInfo *ErrorInfo)noexcept(true);
protected:
	rPtr<iErrorReport> fReport;
};
struct ErrorReportManager
{
	static ErrorReportMaker cnLib_FUNC MakeReport(const uChar16 *Name,uIntn Length)noexcept(true);
	static rPtr<iErrorReport> cnLib_FUNC Fetch(void)noexcept(true);
};
//---------------------------------------------------------------------------
}   // namespace cnSystem
//---------------------------------------------------------------------------
}   // namespace cnLibrary
//---------------------------------------------------------------------------
#endif  /* __cplusplus */
/*-------------------------------------------------------------------------*/
#endif
