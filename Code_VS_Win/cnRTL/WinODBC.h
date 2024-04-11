/*- cnRTL - Windows - ODBC ------------------------------------------------*/
/*         Developer : Code009                                             */
/*         Create on : 2009-03-15                                          */
/*-------------------------------------------------------------------------*/
#pragma once
/*-------------------------------------------------------------------------*/
#include <cnRTL\WinSDKHeader.h>

#include <cnWin\Win_Common.h>

#ifdef __cplusplus
//---------------------------------------------------------------------------
namespace cnLibrary{
//---------------------------------------------------------------------------
namespace cnRTL{
//---------------------------------------------------------------------------
namespace cnWinRTL{
//---------------------------------------------------------------------------
struct cODBCTypeInfo
{
	eDBDataType Type;
	bool ArrayType;
	SQLSMALLINT CType;
	SQLSMALLINT SQLType;

	uInt8 ElementSize;
};
//---------------------------------------------------------------------------
struct cODBCEnvContext
{
	SQLHENV Handle;

	cODBCEnvContext();
	~cODBCEnvContext();
};

//---------------------------------------------------------------------------
struct cODBCDBContext
{
	aClsRef<cODBCEnvContext> EnvContext;
	SQLHDBC Handle;

	cODBCDBContext(aClsRef<cODBCEnvContext> Env);
	~cODBCDBContext();
};
//---------------------------------------------------------------------------
class cODBCQuery : public iDBQuery
{
public:
	cODBCQuery(aClsRef<cODBCDBContext> DBContext);
	~cODBCQuery();

	bool Prepare(const SQLWCHAR *SQL,uIntn Length);

	// iDBQuery

	virtual bool cnLib_FUNC Execute(void)override;
	virtual bool cnLib_FUNC Fetch(void)override;
	virtual void cnLib_FUNC Clear(void)override;

	virtual bool cnLib_FUNC GetRowsAffacted(uIntn &Value)override;

	virtual ufInt16 cnLib_FUNC ColumnCount(void)override;
	virtual cArray<const uChar16> cnLib_FUNC ColumnName(ufInt16 Index)override;
	virtual eDBDataType	cnLib_FUNC ColumnType(ufInt16 Index)override;
	virtual bool		cnLib_FUNC ColumnIsNullable(ufInt16 Index)override;
	virtual uIntn		cnLib_FUNC ColumnMaxLength(ufInt16 Index)override;
	virtual eDBDataType cnLib_FUNC ColumnGetType(ufInt16 Index)override;
	virtual bool			cnLib_FUNC ColumnBindBuffer(ufInt16 Index,eDBDataType Type,void *Buffer,uIntn BufferLength,uIntn *DataLength)override;
	virtual iDBReadMemory*	cnLib_FUNC ColumnQueryReadMemory(ufInt16 Index,eDBDataType Type)override;
	virtual iReadBuffer<void>*	cnLib_FUNC ColumnQueryReadBuffer(ufInt16 Index,eDBDataType Type)override;
	//bool cnLib_FUNC ColumnBindOutput(ufInt16 Index,eDBDataType Type,iDBWriteBuffer *Buffer);

	virtual ufInt16 cnLib_FUNC ParameterCount(void)override;
	virtual cArray<const uChar16>	cnLib_FUNC ParameterName(ufInt16 Index)override;
	virtual uIntn	cnLib_FUNC ParameterMaxLength(ufInt16 Index)override;

	virtual bool cnLib_FUNC ParameterSetNull(ufInt16 Index,bool Null)override;
	virtual bool cnLib_FUNC ParameterBindBuffer(ufInt16 Index,eDBDataType Type,const void *Buffer,const uIntn *DataLength)override;
	virtual bool cnLib_FUNC ParameterCopyBuffer(ufInt16 Index,eDBDataType Type,const void *Buffer,uIntn DataLength)override;
	virtual bool cnLib_FUNC ParameterBindInput(ufInt16 Index,eDBDataType Type,iReadBuffer<void> *Buffer)override;


private:
	aClsRef<cODBCDBContext> fDBContext;
	SQLHSTMT fSTMT;

	class cFetchProcess
	{
	public:
		cODBCQuery *Host;
		eDBDataType FetchType;
		virtual void cnLib_FUNC Destruct(void);
		virtual bool cnLib_FUNC Bind(SQLUSMALLINT ColumnNumber);
		virtual void cnLib_FUNC LateBind(void);
		virtual void cnLib_FUNC Postprocess(SQLUSMALLINT ColumnNumber);
	};

	class cFetchProcess_ServerValueBuffer : public cFetchProcess, public iReadBuffer<void>
	{
	public:
		cFetchProcess_ServerValueBuffer(const cODBCTypeInfo &TypeInfo);

		virtual void cnLib_FUNC Destruct(void)override;
		virtual bool cnLib_FUNC Bind(SQLUSMALLINT ColumnNumber)override;
		virtual void cnLib_FUNC LateBind(void)override;
		virtual void cnLib_FUNC Postprocess(SQLUSMALLINT ColumnNumber)override;

		virtual cConstMemory cnLib_FUNC GatherReadBuffer(uIntn Size)noexcept(true)override;
		virtual void cnLib_FUNC DismissReadBuffer(uIntn Size)noexcept(true)override;
	private:
		SQLLEN fSQLBindRet_Len_Ind;
		union{
			sInt64 SInt;
			uInt64 UInt;
			float Float32;
			double Float64;
			cDateTime DateTime;
			sInt64 SystemTime;

			TIMESTAMP_STRUCT SQLTime;
			SQLGUID SQLGUID;


		}fSQLBindBuffer;
		const cODBCTypeInfo &fTypeInfo;
		bool fLateBinding;

		bool fBufferReaded;

	};

	class cFetchProcess_ServerArrayBuffer : public cFetchProcess, public iReadBuffer<void>
	{
	public:
		cFetchProcess_ServerArrayBuffer(const cODBCTypeInfo &TypeInfo);

		virtual void cnLib_FUNC Destruct(void)override;
		virtual bool cnLib_FUNC Bind(SQLUSMALLINT ColumnNumber)override;
		virtual void cnLib_FUNC LateBind(void)override;
		virtual void cnLib_FUNC Postprocess(SQLUSMALLINT ColumnNumber)override;

		virtual cConstMemory cnLib_FUNC GatherReadBuffer(uIntn Size)noexcept(true)override;
		virtual void cnLib_FUNC DismissReadBuffer(uIntn Size)noexcept(true)override;
	private:
		cnRTL::cMemoryBuffer fDataBuffer;
		const cODBCTypeInfo &fTypeInfo;
		uInt8 fDataTermination;
		uIntn fReadIndex;
	};
	/*
	class cFetchProcess_Output : public cFetchProcess
	{
	public:
		cFetchProcess_Output(const cODBCTypeInfo &TypeInfo,iDBWriteBuffer *OutputBuffer);

		virtual void cnLib_FUNC Destruct(void)override;
		virtual bool cnLib_FUNC Bind(SQLUSMALLINT ColumnNumber)override;
		virtual void cnLib_FUNC LateBind(void)override;
		virtual void cnLib_FUNC Postprocess(SQLUSMALLINT ColumnNumber)override;
	private:
		iDBWriteBuffer *fOutputBuffer;
		const cODBCTypeInfo &fTypeInfo;
		uInt8 fDataTermination;
	};*/

	class cFetchProcess_ClientBuffer : public cFetchProcess
	{
	public:
		cFetchProcess_ClientBuffer(eDBDataType Type,void *Buffer,uIntn BufferLength,uIntn *DataLength);

		virtual void cnLib_FUNC Destruct(void)override;
		virtual bool cnLib_FUNC Bind(SQLUSMALLINT ColumnNumber)override;
		virtual void cnLib_FUNC LateBind(void)override;
		virtual void cnLib_FUNC Postprocess(SQLUSMALLINT ColumnNumber)override;
	private:
		SQLLEN fSQLBindRet_Len_Ind;
		union{
			TIMESTAMP_STRUCT SQLTime;
			SQLGUID SQLGUID;
		}fSQLBindBuffer;
		const cODBCTypeInfo &fTypeInfo;
		eDBDataType fBindType;

		bool fLateBinding;

		void *fBuffer;
		uIntn fBufferSize;
		uIntn *fRetLength;
	};

	struct cColumnBuffer
	{
		cnRTL::cString<uChar16> ColName;
		SQLULEN ColSQLSize;
		SQLSMALLINT ColSQLType;
		SQLSMALLINT ColSQLDecimalDigits;
		SQLSMALLINT ColSQLNullable;
		bool InfoValid;

		cnVar::cPolymorphicObject<cFetchProcess,
			cFetchProcess_ServerValueBuffer,
			cFetchProcess_ServerArrayBuffer,
			cFetchProcess_ClientBuffer
			//cFetchProcess_Output
		> FetchProcess;
	};
	cnRTL::cSeqList<cColumnBuffer> fColumnBufferList;
	cColumnBuffer& QueryColumnInfo(ufInt16 Index);

	class cExecuteProcess
	{
	public:
		cODBCQuery *Host;
		virtual void cnLib_FUNC Destruct(void);
		virtual bool cnLib_FUNC Bind(SQLUSMALLINT Index);
		virtual void cnLib_FUNC Preprocess(SQLUSMALLINT Index,bool Null);
	};

	class cExecuteProcess_ClientBuffer : public cExecuteProcess
	{
	public:
		cExecuteProcess_ClientBuffer(eDBDataType Type,const void *Buffer,const uIntn *BufferLength);

		virtual void cnLib_FUNC Destruct(void)override;
		virtual bool cnLib_FUNC Bind(SQLUSMALLINT Index)override;
		virtual void cnLib_FUNC Preprocess(SQLUSMALLINT Index,bool Null)override;
	private:
		SQLLEN fSQLBind_Len_Ind;
		union{
			TIMESTAMP_STRUCT SQLTime;
			SQLGUID SQLGUID;
		}fSQLBindBuffer;
		const cODBCTypeInfo &fTypeInfo;
		eDBDataType fBindType;

		const void *fBuffer;
		const uIntn *fBufferLength;
	};
	
	struct cParameterBuffer
	{
		cnRTL::cString<uChar16> ParamName;
		SQLSMALLINT ParamType;
		SQLULEN ParamSize;
		SQLSMALLINT ParamDecimalDigits;
		SQLSMALLINT ParamNullable;
		bool InfoValid;

		bool IsNull;

		cnVar::cPolymorphicObject<cExecuteProcess,
			cExecuteProcess_ClientBuffer
		> ExecuteProcess;
	};
	cnRTL::cSeqList<cParameterBuffer> fParameterBufferList;

	bool fNeedUpdateParameterBind;
	bool fNeedUpdateColumnBind;

	void SetupColumnInfo(uInt16 Index);
	void SetupColumnBinding(void);
	
	const cODBCTypeInfo &ColumnBindOutputTypeInfo(eDBDataType BindType,ufInt16 Index);

	void SetupParamBinding(void);


	bool ClearResult(void);
	void Close(void);

};
//---------------------------------------------------------------------------
class cODBCConnection : public iSQLDBConnection
{
private:
	bool SetTranscationMode(bool AutoCommit);

	bool Transcat(bool Commit);
	class cTranscation : public iDBTranscation
	{
	private:
		cnRTL::cAtomicVar<uInt8> fRefCount;
		cODBCConnection* GetHost(void);
	public:

		virtual bool cnLib_FUNC Commit(void)override;
		virtual void cnLib_FUNC Rollback(void)override;

		bool Start(void);
		void Stop(void);
	}fTranscation;

	void Close(void);
public:
	cODBCConnection(aClsRef<cODBCDBContext> DBContext);
	~cODBCConnection();

	virtual iDBTranscation*	cnLib_FUNC Transcation(void)override;

	virtual iPtr<iDBQuery> cnLib_FUNC CreateQuery(const uChar16 *SQL,uIntn Length)override;
private:
	aClsRef<cODBCDBContext> fDBContext;
};
//---------------------------------------------------------------------------
class cODBC : public iODBC
{
public:
	cODBC();
	~cODBC();

	virtual iPtr<iSQLDBConnection> cnLib_FUNC EditAndConnect(HWND ParentWindow,const wchar_t *ConnectionString,short ConnectionStringLength,wchar_t *OutConnectionString,short &OutConnectionStringLength)noexcept(true)override;
	virtual iPtr<iSQLDBConnection> cnLib_FUNC Connect(const wchar_t *ConnectionString,short ConnectionStringLength)noexcept(true)override;
private:
	aClsRef<cODBCEnvContext> fEnvContext;
};
//---------------------------------------------------------------------------
}	// namespace cnWinRTL
//---------------------------------------------------------------------------
}	// namespace cnRTL
//---------------------------------------------------------------------------
}	// namespace cnLibrary
//---------------------------------------------------------------------------
#endif  /* __cplusplus */

