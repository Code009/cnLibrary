#include <cnRTL\WinSDKHeader.h>
#include <sqlext.h>
#include <odbcinst.h>

#include "WinODBC.h"

using namespace cnLibrary;
using namespace cnRTL;
using namespace cnWinRTL;

#pragma comment(lib,"odbc32.lib")


static void ShowSQLError(SQLSMALLINT Type,SQLHANDLE Handle){
	SQLWCHAR       SqlState[6], Msg[SQL_MAX_MESSAGE_LENGTH];
	SQLINTEGER    NativeError;
	SQLSMALLINT   i, MsgLen;
	SQLRETURN     ret;

	i = 1;
	while ((ret = SQLGetDiagRec(Type, Handle, i, SqlState, &NativeError,Msg, sizeof(Msg), &MsgLen)) != SQL_NO_DATA){
		if(MsgLen<SQL_MAX_MESSAGE_LENGTH-1){
			Msg[MsgLen]='\n';
			Msg[MsgLen+1]=0;
		}
		OutputDebugString(Msg);
		//DisplayError(SqlState,NativeError,Msg,MsgLen);
		i++;
	}

}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
namespace cnLibrary::cnRTL::cnWinRTL::ODBCTypeInfo{
static const cODBCTypeInfo Null={
	DBDataType::Null,
	false,						// ArrayType
	SQL_C_DEFAULT,
	SQL_TYPE_NULL,
	0,							// ElementSize
};

static const cODBCTypeInfo Binary={
	DBDataType::Binary,
	true,						// ArrayType
	SQL_C_BINARY,
	SQL_BINARY,
	1,							// ElementSize
};
static const cODBCTypeInfo Bool={
	DBDataType::Bool,
	false,						// ArrayType
	SQL_C_BIT,
	SQL_BIT,
	1,							// ElementSize
};
static const cODBCTypeInfo SInt8={
	DBDataType::SInt8,
	false,						// ArrayType
	SQL_C_STINYINT,
	SQL_TINYINT,
	1,							// ElementSize
};
static const cODBCTypeInfo UInt8={
	DBDataType::UInt8,
	false,						// ArrayType
	SQL_C_UTINYINT,
	SQL_TINYINT,
	1,							// ElementSize
};
static const cODBCTypeInfo SInt16={
	DBDataType::SInt16,
	false,						// ArrayType
	SQL_C_USHORT,
	SQL_SMALLINT,
	2,							// ElementSize
};
static const cODBCTypeInfo UInt16={
	DBDataType::UInt16,
	false,						// ArrayType
	SQL_C_SSHORT,
	SQL_SMALLINT,
	2,							// ElementSize
};
static const cODBCTypeInfo SInt32={
	DBDataType::SInt32,
	false,						// ArrayType
	SQL_C_SLONG,
	SQL_INTEGER,
	4,							// ElementSize
};
static const cODBCTypeInfo UInt32={
	DBDataType::UInt32,
	false,						// ArrayType
	SQL_C_ULONG,
	SQL_INTEGER,
	4,							// ElementSize
};
static const cODBCTypeInfo SInt64={
	DBDataType::SInt64,
	false,						// ArrayType
	SQL_C_SBIGINT,
	SQL_BIGINT,
	8,							// ElementSize
};
static const cODBCTypeInfo UInt64={
	DBDataType::UInt64,
	false,						// ArrayType
	SQL_C_SBIGINT,
	SQL_BIGINT,
	8,							// ElementSize
};
static const cODBCTypeInfo Float32={
	DBDataType::Float32,
	false,						// ArrayType
	SQL_C_FLOAT,
	SQL_REAL,
	4,							// ElementSize
};
static const cODBCTypeInfo Float64={
	DBDataType::Float64,
	false,						// ArrayType
	SQL_C_DOUBLE,
	SQL_DOUBLE,	// SQL_FLOAT
	8,							// ElementSize
};
static const cODBCTypeInfo String8={
	DBDataType::String8,
	true,						// ArrayType
	SQL_C_CHAR,
	SQL_CHAR,
	1,							// ElementSize
};
static const cODBCTypeInfo String16={
	DBDataType::String16,
	true,						// ArrayType
	SQL_C_WCHAR,
	SQL_WCHAR,
	2,							// ElementSize
};
static const cODBCTypeInfo DateTime={
	DBDataType::DateTime,
	false,						// ArrayType
	SQL_C_TYPE_TIMESTAMP,
	SQL_TIMESTAMP,
	sizeof(cDateTime),			// ElementSize
};
static const cODBCTypeInfo SystemTime={
	DBDataType::SystemTime,
	false,						// ArrayType
	SQL_C_TYPE_TIMESTAMP,
	SQL_TIMESTAMP,
	8,							// ElementSize
};
}	// namespace cnLibrary::cnRTL::cnWinRTL::ODBCTypeInfo
//---------------------------------------------------------------------------
static const cODBCTypeInfo& ODBCCTypeInfoFromDBType(eDBDataType Type)
{
	switch(Type){
	default:
	case DBDataType::Null:			return ODBCTypeInfo::Null;

	case DBDataType::Binary:		return ODBCTypeInfo::Binary;
	case DBDataType::Bool:			return ODBCTypeInfo::Bool;

	case DBDataType::SInt8:			return ODBCTypeInfo::SInt8;
	case DBDataType::UInt8:			return ODBCTypeInfo::UInt8;
	case DBDataType::SInt16:		return ODBCTypeInfo::SInt16;
	case DBDataType::UInt16:		return ODBCTypeInfo::UInt16;
	case DBDataType::SInt32:		return ODBCTypeInfo::SInt32;
	case DBDataType::UInt32:		return ODBCTypeInfo::UInt32;
	case DBDataType::SInt64:		return ODBCTypeInfo::SInt64;
	case DBDataType::UInt64:		return ODBCTypeInfo::UInt64;

	case DBDataType::Float32:		return ODBCTypeInfo::Float32;
	case DBDataType::Float64:		return ODBCTypeInfo::Float64;

	case DBDataType::String8:		return ODBCTypeInfo::String8;
	case DBDataType::String16:		return ODBCTypeInfo::String16;
	//case DBDataType::GUID:			return SQL_C_GUID;
	case DBDataType::DateTime:		return ODBCTypeInfo::DateTime;
	case DBDataType::SystemTime:	return ODBCTypeInfo::SystemTime;

	}
}
//---------------------------------------------------------------------------
static const cODBCTypeInfo& ODBCCTypeInfoFromSQLType(SQLSMALLINT SQLType,SQLLEN SQLColumnSize)
{
	switch(SQLType){
	default:
		return ODBCTypeInfo::Null;
	case SQL_UNKNOWN_TYPE:
	case SQL_VARBINARY:
	case SQL_BINARY:
	case SQL_LONGVARBINARY:
		return ODBCTypeInfo::Binary;

	case SQL_BIT:				return ODBCTypeInfo::Bool;
	case SQL_TINYINT:			return ODBCTypeInfo::SInt8;
	case SQL_SMALLINT:			return ODBCTypeInfo::SInt16;
	case SQL_INTEGER:			return ODBCTypeInfo::SInt32;
	case SQL_BIGINT:			return ODBCTypeInfo::SInt64;
	case SQL_REAL:				return ODBCTypeInfo::Float32;
	case SQL_DOUBLE:			return ODBCTypeInfo::Float64;

	case SQL_FLOAT:
		if(SQLColumnSize>=32){
			return ODBCTypeInfo::Float64;
		}
		else{
			return ODBCTypeInfo::Float32;
		}
		break;
	case SQL_VARCHAR:			
	case SQL_CHAR:
	case SQL_LONGVARCHAR:
		return ODBCTypeInfo::String8;

	case SQL_WVARCHAR:			
	case SQL_WCHAR:
	case SQL_WLONGVARCHAR:
		return ODBCTypeInfo::String16;
	//case SQL_GUID:
	//	FieldInfo.DBType=DBDataType::GUID;
	//	FieldInfo.MaxSize=16;
	//	break;

	
	//case SQL_DATETIME:	//case SQL_DATE:
	case SQL_TIME:
	case SQL_TIMESTAMP:
	case SQL_TYPE_DATE:
	case SQL_TYPE_TIME:
	case SQL_TYPE_TIMESTAMP:
		return ODBCTypeInfo::DateTime;

	}
}
//---------------------------------------------------------------------------
static cDateTime SQLTimeToDateTime(const TIMESTAMP_STRUCT &SQLTime)
{
	cDateTime DateTime;
	DateTime.Year=SQLTime.year;
	DateTime.Month=static_cast<uInt8>(SQLTime.month);
	DateTime.Day=static_cast<uInt8>(SQLTime.day);
	DateTime.Hour=static_cast<uInt8>(SQLTime.hour);
	DateTime.Minute=static_cast<uInt8>(SQLTime.minute);
	DateTime.Second=static_cast<uInt8>(SQLTime.second);
	DateTime.NanoSecond=SQLTime.fraction;
	return DateTime;
}
//---------------------------------------------------------------------------
static TIMESTAMP_STRUCT DateTimeToSQLTime(const cDateTime &DateTime)
{
	TIMESTAMP_STRUCT SQLTime;
	SQLTime.year=DateTime.Year;
	SQLTime.month=DateTime.Month;
	SQLTime.day=DateTime.Day;
	SQLTime.hour=DateTime.Hour;
	SQLTime.minute=DateTime.Minute;
	SQLTime.second=DateTime.Second;
	SQLTime.fraction=DateTime.NanoSecond;
	return SQLTime;
}
//---------------------------------------------------------------------------
static bool FileTimeToSQLTime(TIMESTAMP_STRUCT &SQLTime,const FILETIME &ft)
{
	SYSTEMTIME st;
	if(FileTimeToSystemTime(&ft,&st)==FALSE){
		return false;
	}
	SQLTime.year=st.wYear;
	SQLTime.month=st.wMonth;
	SQLTime.day=st.wDay;
	SQLTime.hour=st.wHour;
	SQLTime.minute=st.wMinute;
	SQLTime.second=st.wSecond;
	SQLTime.fraction=st.wMilliseconds*Time_1ms;
	return true;
}
//---------------------------------------------------------------------------
static bool SystemTimeToSQLTime(TIMESTAMP_STRUCT &SQLTime,sInt64 SystemTime)
{
	cNTFileTime ft;
	ft.FromSystemTime(SystemTime);
	return FileTimeToSQLTime(SQLTime,ft.FileTime);
}
//---------------------------------------------------------------------------
static sInt64 SQLTimeToSystemTime(const TIMESTAMP_STRUCT &SQLTime)
{
	cNTFileTime ft;
	SYSTEMTIME st;
	st.wYear=SQLTime.year;
	st.wMonth=SQLTime.month;
	st.wDay=SQLTime.day;
	st.wHour=SQLTime.hour;
	st.wMinute=SQLTime.minute;
	st.wSecond=SQLTime.second;
	st.wMilliseconds=static_cast<uInt16>(SQLTime.fraction/Time_1ms);
	if(SystemTimeToFileTime(&st,&ft.FileTime)==false)
		return 0;
	return ft.ToSystemTime();
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cODBCEnvContext::cODBCEnvContext()
{
	SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &Handle);
}
//---------------------------------------------------------------------------
cODBCEnvContext::~cODBCEnvContext()
{
	SQLFreeHandle(SQL_HANDLE_ENV,Handle);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cODBCDBContext::cODBCDBContext(aClsRef<cODBCEnvContext> Env)
	: EnvContext(cnVar::MoveCast(Env))
{
	SQLAllocHandle(SQL_HANDLE_DBC,EnvContext->Handle,&Handle);
}
//---------------------------------------------------------------------------
cODBCDBContext::~cODBCDBContext()
{
	SQLFreeHandle(SQL_HANDLE_DBC,Handle);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void cODBCQuery::cFetchProcess::Destruct(void){	cFetchProcess::~cFetchProcess();	}
bool cODBCQuery::cFetchProcess::Bind(SQLUSMALLINT){	return true;	}
void cODBCQuery::cFetchProcess::LateBind(void){}
void cODBCQuery::cFetchProcess::Postprocess(SQLUSMALLINT){}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cODBCQuery::cFetchProcess_ServerValueBuffer::cFetchProcess_ServerValueBuffer(const cODBCTypeInfo &TypeInfo)
	: fTypeInfo(TypeInfo)
{
}
//---------------------------------------------------------------------------
void cODBCQuery::cFetchProcess_ServerValueBuffer::Destruct(void)
{
	cFetchProcess_ServerValueBuffer::~cFetchProcess_ServerValueBuffer();
}
//---------------------------------------------------------------------------
bool cODBCQuery::cFetchProcess_ServerValueBuffer::Bind(SQLUSMALLINT ColumnNumber)
{
	// bind to buffer
	auto ret=::SQLBindCol(Host->fSTMT,ColumnNumber,fTypeInfo.CType,&fSQLBindBuffer,sizeof(fSQLBindBuffer),&fSQLBindRet_Len_Ind);
	if(ret!=SQL_SUCCESS && ret!=SQL_SUCCESS_WITH_INFO){
		fSQLBindRet_Len_Ind=SQL_NULL_DATA;
	}
	fLateBinding=false;
	return true;
}
//---------------------------------------------------------------------------
void cODBCQuery::cFetchProcess_ServerValueBuffer::LateBind(void)
{
	fLateBinding=true;
}
//---------------------------------------------------------------------------
void cODBCQuery::cFetchProcess_ServerValueBuffer::Postprocess(SQLUSMALLINT ColumnNumber)
{
	if(fLateBinding){
		// fetch data now
		SQLRETURN ret;
		// fetch to temp buffer
		ret=::SQLGetData(Host->fSTMT,ColumnNumber,fTypeInfo.CType,&fSQLBindBuffer,sizeof(fSQLBindBuffer),&fSQLBindRet_Len_Ind);
		if(ret!=SQL_SUCCESS && ret!=SQL_SUCCESS_WITH_INFO){
			fSQLBindRet_Len_Ind=SQL_NULL_DATA;
		}
	}

	if(fSQLBindRet_Len_Ind==SQL_NULL_DATA){
		FetchType=DBDataType::Null;
	}
	else{
		FetchType=fTypeInfo.Type;
		switch(fTypeInfo.Type){
		case DBDataType::SystemTime:
			fSQLBindBuffer.SystemTime=SQLTimeToSystemTime(fSQLBindBuffer.SQLTime);
			break;
		case DBDataType::DateTime:
			fSQLBindBuffer.DateTime=SQLTimeToDateTime(fSQLBindBuffer.SQLTime);
			break;
		}
	}
}
//---------------------------------------------------------------------------
cConstMemory cODBCQuery::cFetchProcess_ServerValueBuffer::GatherReadBuffer(uIntn)
{
	if(fBufferReaded){
		return NullArray;
	}
	cConstMemory Buffer;
	Buffer.Pointer=&fSQLBindBuffer;
	Buffer.Length=sizeof(fSQLBindBuffer);
	return Buffer;
}
//---------------------------------------------------------------------------
void cODBCQuery::cFetchProcess_ServerValueBuffer::DismissReadBuffer(uIntn Size)
{
	fBufferReaded=true;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cODBCQuery::cFetchProcess_ServerArrayBuffer::cFetchProcess_ServerArrayBuffer(const cODBCTypeInfo &TypeInfo)
	: fTypeInfo(TypeInfo)
{
}
//---------------------------------------------------------------------------
void cODBCQuery::cFetchProcess_ServerArrayBuffer::Destruct(void)
{
	cFetchProcess_ServerArrayBuffer::~cFetchProcess_ServerArrayBuffer();
}
//---------------------------------------------------------------------------
bool cODBCQuery::cFetchProcess_ServerArrayBuffer::Bind(SQLUSMALLINT)
{
	return false;
}
//---------------------------------------------------------------------------
void cODBCQuery::cFetchProcess_ServerArrayBuffer::LateBind(void)
{
	switch(fTypeInfo.Type){
	default:
	case DBDataType::Binary:
		fDataTermination=0;
		break;
	case DBDataType::String8:
		fDataTermination=1;
		break;
	case DBDataType::String16:
		fDataTermination=2;
		break;
	}
}
//---------------------------------------------------------------------------
void cODBCQuery::cFetchProcess_ServerArrayBuffer::Postprocess(SQLUSMALLINT ColumnNumber)
{
	static constexpr uIntn BufferIncrement=32;
	uIntn CurCapacity;
	CurCapacity=fDataBuffer.GetCapacity();
	if(CurCapacity<BufferIncrement){
		fDataBuffer.SetCapacity(CurCapacity+BufferIncrement);
		CurCapacity=fDataBuffer.GetCapacity();
	}

	SQLLEN SQLBindRet_Len_Ind;
	uIntn CurSize=0;
	for(;;){
		auto ret=::SQLGetData(Host->fSTMT,ColumnNumber,fTypeInfo.CType,fDataBuffer.GetPointer(CurSize),CurCapacity-CurSize,&SQLBindRet_Len_Ind);
		if(ret==SQL_SUCCESS){
			if(SQLBindRet_Len_Ind==SQL_NULL_DATA){
				// null data
				break;
			}
			CurSize+=SQLBindRet_Len_Ind;
			fDataBuffer.SetSize(CurSize);
			FetchType=fTypeInfo.Type;
			return;
		}
		if(ret!=SQL_SUCCESS_WITH_INFO)
			break;

		CurSize=CurCapacity-fDataTermination;
		fDataBuffer.SetSize(CurSize);
		fDataBuffer.SetCapacity(CurSize+BufferIncrement);
		CurCapacity=fDataBuffer.GetCapacity();
	}

	FetchType=DBDataType::Null;
}
//---------------------------------------------------------------------------
cConstMemory cODBCQuery::cFetchProcess_ServerArrayBuffer::GatherReadBuffer(uIntn)
{
	if(fReadIndex>=fDataBuffer->Length){
		return NullArray;
	}
	cConstMemory Buffer;
	Buffer.Pointer=fDataBuffer[fReadIndex];
	Buffer.Length=fDataBuffer->Length-fReadIndex;
	return Buffer;
}
//---------------------------------------------------------------------------
void cODBCQuery::cFetchProcess_ServerArrayBuffer::DismissReadBuffer(uIntn Size)
{
	fReadIndex+=Size;
}
//---------------------------------------------------------------------------
/*
//---------------------------------------------------------------------------
cODBCQuery::cFetchProcess_Output::cFetchProcess_Output(const cODBCTypeInfo &TypeInfo,iDBWriteBuffer *OutputBuffer)
	: fTypeInfo(TypeInfo)
{
	fOutputBuffer=OutputBuffer;
}
//---------------------------------------------------------------------------
void cODBCQuery::cFetchProcess_Output::Destruct(void)
{
	cFetchProcess_Output::~cFetchProcess_Output();
}
//---------------------------------------------------------------------------
bool cODBCQuery::cFetchProcess_Output::Bind(SQLUSMALLINT)
{
	return false;
}
//---------------------------------------------------------------------------
void cODBCQuery::cFetchProcess_Output::LateBind(void)
{
	switch(fTypeInfo.Type){
	default:
	case DBDataType::Binary:
		fDataTermination=0;
		break;
	case DBDataType::String8:
		fDataTermination=1;
		break;
	case DBDataType::String16:
		fDataTermination=2;
		break;
	}
}
//---------------------------------------------------------------------------
void cODBCQuery::cFetchProcess_Output::Postprocess(SQLUSMALLINT ColumnNumber)
{
	fOutputBuffer->ResetBuffer();
	SQLLEN SQLBindRet_Len_Ind;
	uIntn TotalSize=0;
	cMemory WriteBuffer;
	while(fOutputBuffer->QueryWriteBuffer(WriteBuffer,0)){
		auto ret=::SQLGetData(Host->fSTMT,ColumnNumber,fTypeInfo.CType,WriteBuffer.Pointer,WriteBuffer.Length,&SQLBindRet_Len_Ind);
		if(ret==SQL_SUCCESS){
			if(SQLBindRet_Len_Ind==SQL_NULL_DATA){
				// null data
				FetchType=DBDataType::Null;
				return;
			}
			// buffer completed
			fOutputBuffer->CommitWriteBuffer(SQLBindRet_Len_Ind);
			TotalSize+=SQLBindRet_Len_Ind;

			
			FetchType=fTypeInfo.Type;
			fOutputBuffer->FinishBuffer();
			//FetchBuffer.Pointer=fOutputBuffer->FinishBuffer();
			//FetchBuffer.Length=TotalSize/fTypeInfo.ElementSize;
			return;
		}
		if(ret!=SQL_SUCCESS_WITH_INFO)
			break;

		uIntn SizeWritten=WriteBuffer.Length-fDataTermination;
		TotalSize+=SizeWritten;
		fOutputBuffer->CommitWriteBuffer(SizeWritten);
	}
	// error
	FetchType=DBDataType::Null;
}
*/
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cODBCQuery::cFetchProcess_ClientBuffer::cFetchProcess_ClientBuffer(eDBDataType Type,void *Buffer,uIntn BufferLength,uIntn *DataLength)
	: fTypeInfo(ODBCCTypeInfoFromDBType(Type))
{
	fBindType=Type;
	fBuffer=Buffer;
	fBufferSize=BufferLength*fTypeInfo.ElementSize;
	fRetLength=DataLength;
}
//---------------------------------------------------------------------------
void cODBCQuery::cFetchProcess_ClientBuffer::Destruct(void)
{
	cFetchProcess_ClientBuffer::~cFetchProcess_ClientBuffer();
}
//---------------------------------------------------------------------------
bool cODBCQuery::cFetchProcess_ClientBuffer::Bind(SQLUSMALLINT ColumnNumber)
{
	SQLRETURN ret;
	switch(fBindType){
	default:
		// bind to user buffer
		ret=::SQLBindCol(Host->fSTMT,ColumnNumber,fTypeInfo.CType,fBuffer,fBufferSize,&fSQLBindRet_Len_Ind);
		break;
	case DBDataType::DateTime:
	case DBDataType::SystemTime:
		// bind to temp buffer
		ret=::SQLBindCol(Host->fSTMT,ColumnNumber,fTypeInfo.CType,&fSQLBindBuffer,sizeof(fSQLBindBuffer),&fSQLBindRet_Len_Ind);
		break;
	}
	if(ret!=SQL_SUCCESS && ret!=SQL_SUCCESS_WITH_INFO){
		fSQLBindRet_Len_Ind=SQL_NULL_DATA;
	}
	fLateBinding=false;
	return true;
}
//---------------------------------------------------------------------------
void cODBCQuery::cFetchProcess_ClientBuffer::LateBind(void)
{
	fLateBinding=true;
}
//---------------------------------------------------------------------------
void cODBCQuery::cFetchProcess_ClientBuffer::Postprocess(SQLUSMALLINT ColumnNumber)
{
	if(fLateBinding){
		// fetch data now
		SQLRETURN ret;
		switch(fBindType){
		default:
			// fetch to user buffer
			ret=::SQLGetData(Host->fSTMT,ColumnNumber,fTypeInfo.CType,fBuffer,fBufferSize,&fSQLBindRet_Len_Ind);
			break;
		case DBDataType::DateTime:
		case DBDataType::SystemTime:
			// fetch to temp buffer
			ret=::SQLGetData(Host->fSTMT,ColumnNumber,fTypeInfo.CType,&fSQLBindBuffer,sizeof(fSQLBindBuffer),&fSQLBindRet_Len_Ind);
			break;
		}
		if(ret!=SQL_SUCCESS && ret!=SQL_SUCCESS_WITH_INFO){
			fSQLBindRet_Len_Ind=SQL_NULL_DATA;
		}
	}

	uIntn RetDataLen=0;
	if(fSQLBindRet_Len_Ind==SQL_NULL_DATA){
		FetchType=DBDataType::Null;
	}
	else{
		switch(fBindType){
		case DBDataType::DateTime:
			*static_cast<sInt64*>(fBuffer)=SQLTimeToSystemTime(fSQLBindBuffer.SQLTime);
			break;
		case DBDataType::SystemTime:
			*static_cast<cDateTime*>(fBuffer)=SQLTimeToDateTime(fSQLBindBuffer.SQLTime);
			break;
		}
		FetchType=fBindType;
		if(fTypeInfo.ArrayType){
			RetDataLen=fSQLBindRet_Len_Ind/fTypeInfo.ElementSize;
		}
		else{
			RetDataLen=1;
		}
	}
	if(fRetLength!=nullptr){
		*fRetLength=RetDataLen;
	}
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void cODBCQuery::cExecuteProcess::Destruct(void){	cExecuteProcess::~cExecuteProcess();	}
bool cODBCQuery::cExecuteProcess::Bind(SQLUSMALLINT){	return true;	}
void cODBCQuery::cExecuteProcess::Preprocess(SQLUSMALLINT,bool){}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cODBCQuery::cExecuteProcess_ClientBuffer::cExecuteProcess_ClientBuffer(eDBDataType Type,const void *Buffer,const uIntn *BufferLength)
	: fTypeInfo(ODBCCTypeInfoFromDBType(Type))
{
	fBindType=Type;
	fBuffer=Buffer;
	fBufferLength=BufferLength;
}
//---------------------------------------------------------------------------
void cODBCQuery::cExecuteProcess_ClientBuffer::Destruct(void)
{
	cExecuteProcess_ClientBuffer::~cExecuteProcess_ClientBuffer();
}
//---------------------------------------------------------------------------
bool cODBCQuery::cExecuteProcess_ClientBuffer::Bind(SQLUSMALLINT Index)
{
	SQLRETURN ret;
	switch(fBindType){
	default:
#pragma message (cnLib_FILE_LINE ": TODO ColumnSize ")
		// bind to user buffer
		ret=::SQLBindParameter(Host->fSTMT,Index,SQL_PARAM_INPUT,fTypeInfo.CType,fTypeInfo.SQLType,256,0,
			const_cast<void*>(fBuffer),fTypeInfo.ElementSize,&fSQLBind_Len_Ind);
		break;
	case DBDataType::DateTime:
	case DBDataType::SystemTime:
		// bind to temp buffer
		ret=::SQLBindParameter(Host->fSTMT,Index,SQL_PARAM_INPUT,fTypeInfo.CType,fTypeInfo.SQLType,0,0,
			&fSQLBindBuffer,0,&fSQLBind_Len_Ind);
		break;
	}
	if(ret!=SQL_SUCCESS && ret!=SQL_SUCCESS_WITH_INFO){
		::ShowSQLError(SQL_HANDLE_STMT,Host->fSTMT);
	}
	return true;
}
//---------------------------------------------------------------------------
void cODBCQuery::cExecuteProcess_ClientBuffer::Preprocess(SQLUSMALLINT Index,bool IsNull)
{
	if(IsNull){
		fSQLBind_Len_Ind=SQL_NULL_DATA;
	}
	else{
		if(fBufferLength!=nullptr){
			fSQLBind_Len_Ind=*fBufferLength*fTypeInfo.ElementSize;
		}
		else{
			fSQLBind_Len_Ind=0;
		}
	}
	// conversion
	switch(fBindType){
	case DBDataType::DateTime:
		fSQLBindBuffer.SQLTime=DateTimeToSQLTime(*static_cast<const cDateTime*>(fBuffer));
		break;
	case DBDataType::SystemTime:
		SystemTimeToSQLTime(fSQLBindBuffer.SQLTime,*static_cast<const sInt64*>(fBuffer));
		break;
	}
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cODBCQuery::cODBCQuery(aClsRef<cODBCDBContext> DBContext)
	: fDBContext(cnVar::MoveCast(DBContext))
{
	auto r=SQLAllocHandle(SQL_HANDLE_STMT,fDBContext->Handle,&fSTMT);
	if(r!=SQL_SUCCESS && r!=SQL_SUCCESS_WITH_INFO){
		fSTMT=SQL_NULL_HANDLE;
	}
}
//---------------------------------------------------------------------------
cODBCQuery::~cODBCQuery()
{
	ClearResult();
	Close();

	for(auto &ColBuffer : fColumnBufferList){

		ColBuffer.FetchProcess->Destruct();
	}

}
//---------------------------------------------------------------------------
bool cODBCQuery::Prepare(const SQLWCHAR *SQL,uIntn Length)
{
	auto r=::SQLPrepareW(fSTMT,const_cast<SQLWCHAR*>(SQL),static_cast<SQLINTEGER>(Length));
	if(r!=SQL_SUCCESS && r!=SQL_SUCCESS_WITH_INFO){
		return false;
	}
	

	SQLSMALLINT ColCount;
	r=SQLNumResultCols(fSTMT,&ColCount);
	if(r!=SQL_SUCCESS && r!=SQL_SUCCESS_WITH_INFO){
		return false;
	}


	SQLSMALLINT ParamCount;
	SQLNumParams(fSTMT,&ParamCount);
	if(r!=SQL_SUCCESS && r!=SQL_SUCCESS_WITH_INFO){
		return false;
	}


	fColumnBufferList.SetCount(ColCount);
	for(SQLSMALLINT i=0;i<ColCount;i++){
		auto &ColBuffer=fColumnBufferList[i];
		ColBuffer.InfoValid=false;

		ColBuffer.FetchProcess.ConstructAs<cFetchProcess>();
		ColBuffer.FetchProcess->Host=this;
	}

	fParameterBufferList.SetCount(ParamCount);
	for(SQLSMALLINT i=0;i<ParamCount;i++){
		SQLSMALLINT Nullable;
		auto &ParamBind=fParameterBufferList[i];
		ParamBind.InfoValid=false;

		ParamBind.ExecuteProcess.ConstructAs<cExecuteProcess>();
	
	}

//	fParamList.SetCount(ParamCount);
//	for(SQLSMALLINT i=0;i<ParamCount;i++){
//		SQLSMALLINT Nullable;
//		auto &ParamBind=fParamList[i];
//		r=SQLDescribeParam(fSTMT,i+1,&ParamBind.Type,&ParamBind.ColumnSize,&ParamBind.DecimalDigits,&Nullable);
//		if(r!=SQL_SUCCESS && r!=SQL_SUCCESS_WITH_INFO){
//			ShowSQLError(SQL_HANDLE_STMT,fSTMT);
//			ParamBind.Type=SQL_UNKNOWN_TYPE;
//			ParamBind.ColumnSize=0;
//			ParamBind.DecimalDigits=0;
//		}
//
//	}

	fNeedUpdateParameterBind=true;
	fNeedUpdateColumnBind=true;
	return true;
}
//---------------------------------------------------------------------------
void cODBCQuery::Close(void)
{
	if(fSTMT!=SQL_NULL_HANDLE){
		SQLFreeHandle(SQL_HANDLE_STMT,fSTMT);
		fSTMT=SQL_NULL_HANDLE;
	}
}
//---------------------------------------------------------------------------
bool cODBCQuery::Execute(void)
{
	if(fNeedUpdateParameterBind){
		SetupParamBinding();
		fNeedUpdateParameterBind=false;
	}

	// pre process
	for(uIntn pi=0,pc=fParameterBufferList.GetCount();pi<pc;pi++){
		auto &ParamBuffer=fParameterBufferList[pi];
		
		ParamBuffer.ExecuteProcess->Preprocess(static_cast<SQLUSMALLINT>(pi+1),ParamBuffer.IsNull);
	}

	// execute
	auto ret=SQLExecute(fSTMT);

	if(ret!=SQL_SUCCESS && ret!=SQL_SUCCESS_WITH_INFO){
		ShowSQLError(SQL_HANDLE_STMT,fSTMT);
		return false;
	}


	return true;
}
//---------------------------------------------------------------------------
bool cODBCQuery::Fetch(void)
{
	if(fNeedUpdateColumnBind){
		SetupColumnBinding();
		fNeedUpdateColumnBind=false;
	}

	auto ret=SQLFetch(fSTMT);
	if(ret!=SQL_SUCCESS && ret!=SQL_SUCCESS_WITH_INFO){
		return false;
	}
	// post process
	for(uIntn ci=0,cc=fColumnBufferList.GetCount();ci<cc;ci++){
		auto &ColBuffer=fColumnBufferList[ci];
		
		ColBuffer.FetchProcess->Postprocess(static_cast<SQLUSMALLINT>(ci+1));
	}
	return true;
}
//---------------------------------------------------------------------------
void cODBCQuery::Clear(void)
{
	ClearResult();
}
//---------------------------------------------------------------------------
bool cODBCQuery::ClearResult(void)
{
	auto ret=SQLCloseCursor(fSTMT);
	return ret==SQL_SUCCESS || ret==SQL_SUCCESS_WITH_INFO;
}
//---------------------------------------------------------------------------
ufInt16 cODBCQuery::ColumnCount(void)
{
	return fColumnBufferList.GetCount();
}
//---------------------------------------------------------------------------
void cODBCQuery::SetupColumnInfo(uInt16 Index)
{
	SQLSMALLINT NameLengthRet;
	auto &ColBuffer=fColumnBufferList[Index];
	cStringBuffer<uChar16> ColNameBuffer;
	auto ret=SQLDescribeColW(fSTMT,Index+1,nullptr,0,&NameLengthRet,
		&ColBuffer.ColSQLType,&ColBuffer.ColSQLSize,&ColBuffer.ColSQLDecimalDigits,&ColBuffer.ColSQLNullable);
	if(ret!=SQL_SUCCESS && ret!=SQL_SUCCESS_WITH_INFO){
		return;
	}

	ColNameBuffer.SetLength(NameLengthRet);
	ret=SQLDescribeColW(fSTMT,Index+1,reinterpret_cast<SQLWCHAR*>(ColNameBuffer.GetString()),NameLengthRet,&NameLengthRet,
		nullptr,nullptr,nullptr,nullptr);
	if(ret!=SQL_SUCCESS && ret!=SQL_SUCCESS_WITH_INFO){
		return;
	}
	ColBuffer.ColName=cnVar::MoveCast(ColNameBuffer);
	ColBuffer.InfoValid=true;
}
//---------------------------------------------------------------------------
cODBCQuery::cColumnBuffer& cODBCQuery::QueryColumnInfo(ufInt16 Index)
{
	auto &ColBuffer=fColumnBufferList[Index];
	if(ColBuffer.InfoValid==false){
		SetupColumnInfo(Index);
	}
	return ColBuffer;	
}
//---------------------------------------------------------------------------
cArray<const uChar16> cODBCQuery::ColumnName(ufInt16 Index)
{
	if(Index>=fColumnBufferList.GetCount()){
		cArray<const uChar16> NameArray;
		NameArray.Pointer=nullptr;
		NameArray.Length=0;
		return NameArray;
	}

	auto &ColBuffer=QueryColumnInfo(Index);
	return ColBuffer.ColName.GetArray();
}
//---------------------------------------------------------------------------
eDBDataType	cODBCQuery::ColumnType(ufInt16 Index)
{
	if(Index>=fColumnBufferList.GetCount()){
		return DBDataType::Null;
	}
	
	auto &ColBuffer=QueryColumnInfo(Index);
	auto &TypeInfo=ODBCCTypeInfoFromSQLType(ColBuffer.ColSQLType,ColBuffer.ColSQLSize);
	return TypeInfo.Type;
}
//---------------------------------------------------------------------------
bool cODBCQuery::ColumnIsNullable(ufInt16 Index)
{
	if(Index>=fColumnBufferList.GetCount()){
		return false;
	}
	
	auto &ColBuffer=QueryColumnInfo(Index);
	return ColBuffer.ColSQLNullable!=SQL_NO_NULLS;
}
//---------------------------------------------------------------------------
uIntn cODBCQuery::ColumnMaxLength(ufInt16 Index)
{
	if(Index>=fColumnBufferList.GetCount()){
		return false;
	}
	
	auto &ColBuffer=QueryColumnInfo(Index);
	auto &TypeInfo=ODBCCTypeInfoFromSQLType(ColBuffer.ColSQLType,ColBuffer.ColSQLSize);
	if(TypeInfo.ArrayType){
		return ColBuffer.ColSQLSize;
	}
	else{
		return 1;
	}
}
//---------------------------------------------------------------------------
void cODBCQuery::SetupColumnBinding(void)
{
	// clear binding
	::SQLFreeStmt(fSTMT,SQL_UNBIND);
	// setup binding
	uIntn ci=0;
	uIntn cc=fColumnBufferList.GetCount();
	for(ci=0;ci<cc;ci++){
		auto &ColBuffer=fColumnBufferList[ci];

		if(ColBuffer.FetchProcess->Bind(static_cast<SQLUSMALLINT>(ci+1))==false){
			// late binding
			goto LateBinding;
		}
	}

	return;

LateBinding:
	do{
		auto &ColBuffer=fColumnBufferList[ci];
		ColBuffer.FetchProcess->LateBind();

		ci++;
	}while(ci<cc);
}
//---------------------------------------------------------------------------
bool cODBCQuery::ColumnBindBuffer(ufInt16 Index,eDBDataType Type,void *Buffer,uIntn BufferLength,uIntn *DataLength)
{
	if(Index>=fColumnBufferList.GetCount()){
		return false;
	}
	auto &ColBuffer=fColumnBufferList[Index];
	auto &TypeInfo=ColumnBindOutputTypeInfo(Type,Index);

	ColBuffer.FetchProcess->Destruct();
	if(&TypeInfo==&ODBCTypeInfo::Null || Buffer==nullptr){
		ColBuffer.FetchProcess.ConstructAs<cFetchProcess>();
	}
	else{
		ColBuffer.FetchProcess.ConstructAs<cFetchProcess_ClientBuffer>(Type,Buffer,BufferLength,DataLength);
	}
	ColBuffer.FetchProcess->Host=this;

	fNeedUpdateColumnBind=true;
	return true;
}
//---------------------------------------------------------------------------
const cODBCTypeInfo &cODBCQuery::ColumnBindOutputTypeInfo(eDBDataType BindType,ufInt16 Index)
{
	if(BindType==DBDataType::Unknown){
		auto &ColBuffer=fColumnBufferList[Index];
		if(ColBuffer.InfoValid==false){
			SetupColumnInfo(Index);
		}

		return ODBCCTypeInfoFromSQLType(ColBuffer.ColSQLType,ColBuffer.ColSQLSize);
	}
	else{
		return ODBCCTypeInfoFromDBType(BindType);
	}
}
//---------------------------------------------------------------------------
iDBReadMemory* cODBCQuery::ColumnQueryReadMemory(ufInt16 Index,eDBDataType Type)
{
	return nullptr;
}
//---------------------------------------------------------------------------
iReadBuffer<void>* cODBCQuery::ColumnQueryReadBuffer(ufInt16 Index,eDBDataType Type)
{
	if(Index>=fColumnBufferList.GetCount()){
		return nullptr;
	}
	auto &ColBuffer=fColumnBufferList[Index];

	ColBuffer.FetchProcess->Destruct();

	iReadBuffer<void> *RetBuffer=nullptr;
	auto &TypeInfo=ColumnBindOutputTypeInfo(Type,Index);
	if(&TypeInfo==&ODBCTypeInfo::Null){
		ColBuffer.FetchProcess.ConstructAs<cFetchProcess>();
	}
	else{
		if(TypeInfo.ArrayType){
			ColBuffer.FetchProcess.ConstructAs<cFetchProcess_ServerArrayBuffer>(TypeInfo);
			RetBuffer=reinterpret_cast<cFetchProcess_ServerArrayBuffer*>(&ColBuffer.FetchProcess);
		}
		else{
			ColBuffer.FetchProcess.ConstructAs<cFetchProcess_ServerValueBuffer>(TypeInfo);
			RetBuffer=reinterpret_cast<cFetchProcess_ServerValueBuffer*>(&ColBuffer.FetchProcess);
		}
	}
	
	ColBuffer.FetchProcess->Host=this;
	
	fNeedUpdateColumnBind=true;
	return RetBuffer;
}
//---------------------------------------------------------------------------
/*
bool cODBCQuery::ColumnBindOutput(ufInt16 Index,eDBDataType Type,iDBWriteBuffer *Buffer)
{
	if(Index>=fColumnBufferList.GetCount()){
		return false;
	}
	auto &ColBuffer=fColumnBufferList[Index];

	ColBuffer.FetchProcess->Destruct();
	auto &TypeInfo=ColumnBindOutputTypeInfo(Type,Index);
	if(&TypeInfo==&ODBCTypeInfo::Null || Buffer==nullptr){
		ColBuffer.FetchProcess.ConstructAs<cFetchProcess>();
	}
	else{
		ColBuffer.FetchProcess.ConstructAs<cFetchProcess_Output>(TypeInfo,Buffer);
	}
	

	ColBuffer.FetchProcess->Host=this;
	fNeedUpdateColumnBind=true;
	return true;
}
*/
//---------------------------------------------------------------------------
eDBDataType cODBCQuery::ColumnGetType(ufInt16 Index)
{
	if(Index>=fColumnBufferList.GetCount()){
		return DBDataType::Null;
	}
	auto &ColBuffer=fColumnBufferList[Index];
	return ColBuffer.FetchProcess->FetchType;
}
//---------------------------------------------------------------------------
//cConstMemory cODBCQuery::ColumnGetBuffer(uInt16 Index)
//{
//	if(Index>=fColumnBufferList.GetCount()){
//		cConstMemory Mem;
//		Mem.Pointer=nullptr;
//		Mem.Length=0;
//		return Mem;
//	}
//	auto &ColBuffer=fColumnBufferList[Index];
//	return ColBuffer.FetchProcess->FetchBuffer;
//}
//---------------------------------------------------------------------------
ufInt16 cODBCQuery::ParameterCount(void)
{
	return fParameterBufferList.GetCount();
}
//---------------------------------------------------------------------------
cArray<const uChar16> cODBCQuery::ParameterName(ufInt16 Index)
{
	cArray<const uChar16> Name;
	Name.Pointer=nullptr;
	Name.Length=0;
	return Name;
}
//---------------------------------------------------------------------------
uIntn cODBCQuery::ParameterMaxLength(ufInt16 Index)
{
	return 0;
}
//---------------------------------------------------------------------------
/*
//---------------------------------------------------------------------------
static cDBFieldInfo SQLFieldInfoFromDesc(SQLSMALLINT SQLType,SQLSMALLINT SQLDecimalDigits,SQLLEN SQLColumnSize,SQLSMALLINT SQLNullable)
{
	auto &TypeInfo=ODBCCTypeInfoFromSQLType(SQLType,SQLColumnSize);
	cDBFieldInfo FInfo;
	FInfo.DBType=TypeInfo.Type;
	if(TypeInfo.ArrayType){
		FInfo.MaxLength=SQLColumnSize;
	}
	else{
		FInfo.MaxLength=1;
	}
	if(SQLNullable==SQL_NO_NULLS){
		FInfo.Nullable=false;
	}
	else{
		FInfo.Nullable=true;
	}
	return FInfo;
}
bool cODBCQuery::ParameterInfo(uIntn Index,cDBFieldInfo &Info)
{
	if(Index>=fParamList.GetCount())
		return false;

	auto &Bind=fParamList[Index];

	if(ParamBuffer.InfoValid==false){
		SetupParamInfo(Index);
	}
	Info=SQLFieldInfoFromDesc(ColBuffer.ColSQLType,ColBuffer.ColSQLDecimalDigits,ColBuffer.ColSQLSize,ColBuffer.ColSQLNullable);
	return true;
}
//---------------------------------------------------------------------------
bool cODBCQuery::BindInParameter(uIntn Index,const void *Buffer,uIntn BufferSize,cDBType Type,bool IsNull)
{
	if(Index>=fParamList.GetCount())
		return false;

	auto &BindParam=fParamList[Index];
	SQLSMALLINT BindType=BindParam.Type;

	SQLINTEGER BindBufferSize;
	SQLSMALLINT SQLCType;
	BindParam.Converter=ODBCBindInfo(Type,SQLCType);
	switch(BindParam.Converter){
	default:
	case ODBCBindConvert_Null:
		Buffer=nullptr;
		IsNull=true;
		BindBufferSize=0;
		break;
	case ODBCBindConvert_None:
		BindBufferSize=static_cast<SQLINTEGER>(BufferSize);
		break;
	case ODBCBindConvert_DateTime:
		BindParam.RetLen_Ind=0;
		BindParam.ConvertSrc_DateTime=static_cast<const cDateTime*>(Buffer);
		Buffer=&BindParam.Convert_SQLTime;
		BindBufferSize=0;
		break;
	case ODBCBindConvert_SystemTime:
		BindParam.ConvertSrc_SystemTime=static_cast<const sInt64*>(Buffer);
		Buffer=&BindParam.Convert_SQLTime;
		BindBufferSize=0;
		break;
	}
	if(IsNull){
		BindParam.RetLen_Ind=SQL_NULL_DATA;
	}
	else{
		BindParam.RetLen_Ind=BindBufferSize;
	}
	// bind
	if(BindType==SQL_UNKNOWN_TYPE){
		BindType=TypeToSQLType(Type);
		BindParam.ColumnSize=0;
		BindParam.DecimalDigits=0;
		switch(BindType){
		case SQL_BINARY:
			BindParam.ColumnSize=BufferSize;
			break;
		case SQL_WCHAR:
			BindParam.ColumnSize=BufferSize/sizeof(wchar_t);
			break;
		case SQL_CHAR:
			BindParam.ColumnSize=BufferSize/sizeof(char);
			break;
		}
	}

	auto ret=SQLBindParameter(fSTMT,static_cast<SQLSMALLINT>(Index+1),SQL_PARAM_INPUT,SQLCType,BindType,BindParam.ColumnSize,BindParam.DecimalDigits,
		const_cast<void*>(Buffer),0,&BindParam.RetLen_Ind);
	if(ret!=SQL_SUCCESS && ret!=SQL_SUCCESS_WITH_INFO){
		BindParam.Converter=ODBCBindConvert_Null;
		ShowSQLError(SQL_HANDLE_STMT,fSTMT);
		return false;
	}
	return true;
}
//---------------------------------------------------------------------------
bool cODBCQuery::SetInParameterStatus(uIntn Index,uIntn Size,bool IsNull)
{
	if(Index>=fParamList.GetCount())
		return false;

	auto &BindParam=fParamList[Index];
	if(IsNull){
		BindParam.RetLen_Ind=SQL_NULL_DATA;
		return true;
	}
	switch(BindParam.Converter){
	default:
	case ODBCBindConvert_Null:
		break;
	case ODBCBindConvert_None:
		BindParam.RetLen_Ind=Size;
		break;
	case ODBCBindConvert_DateTime:
		BindParam.RetLen_Ind=0;
		break;
	case ODBCBindConvert_SystemTime:
		BindParam.RetLen_Ind=0;
		break;
	}
	return true;
}*/
//---------------------------------------------------------------------------
void cODBCQuery::SetupParamBinding(void)
{
	// clear binding
	::SQLFreeStmt(fSTMT,SQL_RESET_PARAMS);
	// setup binding
	uIntn pi=0;
	uIntn pc=fParameterBufferList.GetCount();
	for(pi=0;pi<pc;pi++){
		auto &ParamBuffer=fParameterBufferList[pi];

		ParamBuffer.ExecuteProcess->Bind(static_cast<SQLUSMALLINT>(pi+1));
	}
}
//---------------------------------------------------------------------------
bool cODBCQuery::ParameterSetNull(ufInt16 Index,bool Null)
{
	if(Index>=fParameterBufferList.GetCount()){
		return false;
	}
	auto &ParamBuffer=fParameterBufferList[Index];
	ParamBuffer.IsNull=Null;
	return true;
}
//---------------------------------------------------------------------------
bool cODBCQuery::ParameterCopyBuffer(ufInt16 Index,eDBDataType Type,const void *Buffer,uIntn DataLength)
{
	return false;
}
//---------------------------------------------------------------------------
bool cODBCQuery::ParameterBindBuffer(ufInt16 Index,eDBDataType Type,const void *Buffer,const uIntn *BufferLength)
{
	if(Index>=fParameterBufferList.GetCount()){
		return false;
	}
	auto &ParamBuffer=fParameterBufferList[Index];

	ParamBuffer.IsNull=false;
	ParamBuffer.ExecuteProcess->Destruct();
	ParamBuffer.ExecuteProcess.ConstructAs<cExecuteProcess_ClientBuffer>(Type,Buffer,BufferLength);
	ParamBuffer.ExecuteProcess->Host=this;

	fNeedUpdateParameterBind=true;
	return true;
}
//---------------------------------------------------------------------------
bool cODBCQuery::ParameterBindInput(ufInt16 Index,eDBDataType Type,iReadBuffer<void> *Buffer)
{
#pragma message( cnLib_FILE_LINE ":TODO - Parameter")
	return false;
}
//---------------------------------------------------------------------------
bool cODBCQuery::GetRowsAffacted(uIntn &Value)
{
	SQLLEN rows;
	auto ret=SQLRowCount(fSTMT,&rows);
	if(ret==SQL_SUCCESS || ret==SQL_SUCCESS_WITH_INFO){
		Value=rows;
		return true;
	}
	return false;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cODBCConnection* cODBCConnection::cTranscation::GetHost(void)
{
	return cnMemory::GetObjectFromMemberPointer(this,&cODBCConnection::fTranscation);
}
//---------------------------------------------------------------------------
bool cnLib_FUNC cODBCConnection::cTranscation::Commit(void)
{
	if(GetHost()->Transcat(true)){
		Stop();
		return true;
	}
	return false;
}
//---------------------------------------------------------------------------
void cnLib_FUNC cODBCConnection::cTranscation::Rollback(void)
{
	GetHost()->Transcat(false);
	Stop();
}
//---------------------------------------------------------------------------
bool cODBCConnection::cTranscation::Start(void)
{
	if(fRefCount.Acquire.CmpStore(0,1)==false)
		return false;	// already started

	auto Host=GetHost();
	if(Host->SetTranscationMode(false)==false){
		// cannot set transcate mode
		fRefCount.Release.Store(0);
		return false;
	}

	iIncReference(Host,'tran');
	return true;
}
//---------------------------------------------------------------------------
void cODBCConnection::cTranscation::Stop(void)
{
	if(fRefCount.Free--==1){
		auto Host=GetHost();
		Host->SetTranscationMode(true);
		iDecReference(Host,'tran');
	}
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cODBCConnection::cODBCConnection(aClsRef<cODBCDBContext> DBContext)
	: fDBContext(cnVar::MoveCast(DBContext))
{
}
//---------------------------------------------------------------------------
cODBCConnection::~cODBCConnection()
{
}
//---------------------------------------------------------------------------
void cODBCConnection::Close(void)
{
	auto r=SQLDisconnect(fDBContext->Handle);
	if(r!=SQL_SUCCESS)
		return;
}
//---------------------------------------------------------------------------
bool cODBCConnection::Transcat(bool Commit)
{
	SQLSMALLINT CompletionType=SQL_ROLLBACK;
	if(Commit){
		CompletionType=SQL_COMMIT;
	}
	else{
		CompletionType=SQL_ROLLBACK;
	}
	auto ret=SQLEndTran(SQL_HANDLE_DBC,fDBContext->Handle,CompletionType);
	if(ret==SQL_SUCCESS || ret==SQL_SUCCESS_WITH_INFO){
		return true;
	}
	return false;
}
//---------------------------------------------------------------------------
bool cODBCConnection::SetTranscationMode(bool AutoCommit)
{
	uIntn Value;
	if(AutoCommit){
		Value=SQL_AUTOCOMMIT_ON;
	}
	else{
		Value=SQL_AUTOCOMMIT_OFF;
	}
	auto ret=SQLSetConnectAttrW(fDBContext->Handle,SQL_ATTR_AUTOCOMMIT,reinterpret_cast<SQLPOINTER>(Value),SQL_IS_UINTEGER);
	if(ret==SQL_SUCCESS || ret==SQL_SUCCESS_WITH_INFO){
		return true;
	}
	ShowSQLError(SQL_HANDLE_DBC,fDBContext->Handle);
	return false;
}
//---------------------------------------------------------------------------
iDBTranscation*	cnLib_FUNC cODBCConnection::Transcation(void)
{
	if(fTranscation.Start())
		return &fTranscation;
	return nullptr;
}
//---------------------------------------------------------------------------
iPtr<iDBQuery> cODBCConnection::CreateQuery(const uChar16 *SQL,uIntn Length)
{
	auto Query=iCreate<cODBCQuery>(fDBContext);
	if(Query->Prepare(utow(SQL),Length)==false)
		return nullptr;
	return Query;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cODBC::cODBC()
{
	fEnvContext=aClsCreate<cODBCEnvContext>();
	SQLSetEnvAttr(fEnvContext->Handle,SQL_ATTR_ODBC_VERSION,(SQLPOINTER)SQL_OV_ODBC3,0);
}
//---------------------------------------------------------------------------
cODBC::~cODBC()
{
}
//---------------------------------------------------------------------------
iPtr<iSQLDBConnection> cODBC::EditAndConnect(HWND ParentWindow,const wchar_t *ConnectionString,short ConnectionStringLength,wchar_t *OutConnectionString,short &OutConnectionStringLength)
{
	if(ParentWindow==NULL){
		ParentWindow=GetDesktopWindow();
	}
	auto DBContext=aClsCreate<cODBCDBContext>(fEnvContext);
	auto r=SQLDriverConnectW(
               DBContext->Handle,
               ParentWindow,
			   const_cast<wchar_t*>(ConnectionString), 
               ConnectionStringLength,
               OutConnectionString,
               OutConnectionStringLength,
               &OutConnectionStringLength,
               SQL_DRIVER_PROMPT  );
	if(r!=SQL_SUCCESS && r!=SQL_SUCCESS_WITH_INFO){
		OutConnectionStringLength=0;
		return nullptr;
	}
	return iCreate<cODBCConnection>(DBContext);
}
//---------------------------------------------------------------------------
iPtr<iSQLDBConnection> cODBC::Connect(const wchar_t *ConnectionString,short ConnectionStringLength)
{
	auto DBContext=aClsCreate<cODBCDBContext>(fEnvContext);
	auto r=SQLDriverConnectW(DBContext->Handle,nullptr,
				const_cast<wchar_t*>(ConnectionString), static_cast<SQLSMALLINT>(ConnectionStringLength),
				nullptr,0,nullptr,SQL_DRIVER_NOPROMPT);
	if(r!=SQL_SUCCESS && r!=SQL_SUCCESS_WITH_INFO){
		return nullptr;
	}
	return iCreate<cODBCConnection>(DBContext);
}
//---------------------------------------------------------------------------
