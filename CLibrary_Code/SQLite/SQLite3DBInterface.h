/*- DB Interface for SQLite3 ----------------------------------------------*/
/*         Developer : Code009                                             */
/*         Create on : 2019-12-30                                          */
/*-------------------------------------------------------------------------*/
#include <SQLite/SQLite3Header.h>
#ifdef SQLITE3_H
#ifndef __cnLibrary_SQLite_SQLite3DBInterface_H__
#define __cnLibrary_SQLite_SQLite3DBInterface_H__
/*-------------------------------------------------------------------------*/
#include <SQLite/SQLite3DB.h>
/*-------------------------------------------------------------------------*/
#ifdef __cplusplus
//---------------------------------------------------------------------------
namespace cnLibrary{
//---------------------------------------------------------------------------
namespace cnRTL{
//---------------------------------------------------------------------------
class cSQLite3DBQuery : public iDBQuery
{
public:
	cSQLite3DBQuery(rClsRef<cSQLite3DBContext> fContext,sqlite3_stmt *Statement);
	~cSQLite3DBQuery();

	virtual bool cnLib_FUNC Execute(void)override;
	virtual bool cnLib_FUNC Fetch(void)override;
	virtual void cnLib_FUNC Clear(void)override;

	virtual bool cnLib_FUNC GetRowsAffacted(uIntn &RowCount)override;

	virtual ufInt16		cnLib_FUNC ColumnCount(void)override;
	virtual cArray<const uChar16>	cnLib_FUNC ColumnName(ufInt16 Index)override;
	virtual eDBDataType	cnLib_FUNC ColumnType(ufInt16 Index)override;
	virtual bool		cnLib_FUNC ColumnIsNullable(ufInt16 Index)override;
	virtual uIntn		cnLib_FUNC ColumnMaxLength(ufInt16 Index)override;
	virtual eDBDataType		cnLib_FUNC ColumnGetType(ufInt16 Index)override;
	virtual bool			cnLib_FUNC ColumnBindBuffer(ufInt16 Index,eDBDataType Type,void *Buffer,uIntn BufferLength,uIntn *DataLength)override;
	virtual iDBReadMemory*	cnLib_FUNC ColumnQueryReadMemory(ufInt16 Index,eDBDataType Type)override;
	virtual iReadBuffer<void>*	cnLib_FUNC ColumnQueryReadBuffer(ufInt16 Index,eDBDataType Type)override;

	virtual ufInt16	cnLib_FUNC ParameterCount(void)override;
	virtual cArray<const uChar16>	cnLib_FUNC ParameterName(ufInt16 Index)override;
	virtual uIntn	cnLib_FUNC ParameterMaxLength(ufInt16 Index)override;

	virtual bool cnLib_FUNC ParameterSetNull(ufInt16 Index,bool Null)override;
	virtual bool cnLib_FUNC ParameterBindBuffer(ufInt16 Index,eDBDataType Type,const void *Buffer,const uIntn *DataLength)override;
	virtual bool cnLib_FUNC ParameterCopyBuffer(ufInt16 Index,eDBDataType Type,const void *Buffer,uIntn DataLength)override;
	virtual bool cnLib_FUNC ParameterBindInput(ufInt16 Index,eDBDataType Type,iReadBuffer<void> *Buffer)override;
private:
	rClsRef<cSQLite3DBContext> fContext;
	sqlite3_stmt *fStatement;
	enum{
		qsIdle,
		qsDone,
		qsRow,
		qsExeRow,
	}fQueryState=qsIdle;

	class cFetchPostprocess
	{
	public:
		cSQLite3DBQuery *Owner;
		cFetchPostprocess();

		virtual void cnLib_FUNC Destruct(void);
		virtual void cnLib_FUNC Process(int col);
	};

	class cFetchPostprocess_UserBuffer : public cFetchPostprocess
	{
	public:
		cFetchPostprocess_UserBuffer(eDBDataType DataType,void *Buffer,uIntn BufferLength,uIntn *DataLength);

		virtual void cnLib_FUNC Destruct(void)override;
		virtual void cnLib_FUNC Process(int index)override;
	private:
		void *fBuffer;
		uIntn fBufferLength;
		uIntn *fDataLength;

		void (cFetchPostprocess_UserBuffer::*fProcessProcedure)(int index);

		void ProcessBool(int index);
		void ProcessSInt8(int index);
		void ProcessUInt8(int index);
		void ProcessSInt16(int index);
		void ProcessUInt16(int index);
		void ProcessSInt32(int index);
		void ProcessUInt32(int index);
		void ProcessSInt64(int index);
		void ProcessUInt64(int index);
		void ProcessFloat32(int index);
		void ProcessFloat64(int index);
	
		void ProcessBinary(int index);
		void ProcessString8(int index);
		void ProcessString16(int index);
	};

	class cFetchPostprocess_FixedSizeRead : public cFetchPostprocess, public iDBReadMemory, public iReadBuffer<void>
	{
	public:
		virtual cConstMemory cnLib_FUNC GetReadMemory(void)override;
		virtual cConstMemory cnLib_FUNC GatherReadBuffer(uIntn QuerySize)override;
		virtual uIntn cnLib_FUNC DismissReadBuffer(uIntn Size)override;
	protected:
		ufInt8 fReadOffset;
		ufInt8 fValueSize;
		union{
			bool	b;
			sInt8	s8;
			sInt16	s16;
			sInt32	s32;
			sInt64	s64;
			uInt8	u8;
			uInt16	u16;
			uInt32	u32;
			uInt64	u64;
			Float32	f32;
			Float64	f64;
		}fValue;
	};

	class cFetchPostprocess_BoolRead : public cFetchPostprocess_FixedSizeRead
	{
	public:
		cFetchPostprocess_BoolRead();

		virtual void cnLib_FUNC Destruct(void)override;
		virtual void cnLib_FUNC Process(int col)override;
	};
	class cFetchPostprocess_SIntRead : public cFetchPostprocess_FixedSizeRead
	{
	public:
		cFetchPostprocess_SIntRead(ufInt8 Size);

		virtual void cnLib_FUNC Destruct(void)override;
		virtual void cnLib_FUNC Process(int col)override;
	};
	class cFetchPostprocess_UIntRead : public cFetchPostprocess_FixedSizeRead
	{
	public:
		cFetchPostprocess_UIntRead(ufInt8 Size);

		virtual void cnLib_FUNC Destruct(void)override;
		virtual void cnLib_FUNC Process(int col)override;
	};
	class cFetchPostprocess_FloatRead : public cFetchPostprocess_FixedSizeRead
	{
	public:
		cFetchPostprocess_FloatRead(bool DP);

		virtual void cnLib_FUNC Destruct(void)override;
		virtual void cnLib_FUNC Process(int col)override;
	private:
		bool fDP;
	};

	class cFetchPostprocess_UTFTextRead : public cFetchPostprocess, public iDBReadMemory, public iReadBuffer<void>
	{
	public:
		cFetchPostprocess_UTFTextRead(ufInt8 CharSize);

		virtual void cnLib_FUNC Destruct(void)override;
		virtual void cnLib_FUNC Process(int col)override;

		virtual cConstMemory cnLib_FUNC GetReadMemory(void)override;
		virtual cConstMemory cnLib_FUNC GatherReadBuffer(uIntn QuerySize)override;
		virtual uIntn cnLib_FUNC DismissReadBuffer(uIntn Size)override;
	protected:
		bool fUTF16;
		int fCol;
		uIntn fReadOffset;
	};
	class cFetchPostprocess_BLOBRead : public cFetchPostprocess, public iDBReadMemory, public iReadBuffer<void>
	{
	public:
		virtual void cnLib_FUNC Destruct(void)override;
		virtual void cnLib_FUNC Process(int col)override;

		virtual cConstMemory cnLib_FUNC GetReadMemory(void)override;
		virtual cConstMemory cnLib_FUNC GatherReadBuffer(uIntn QuerySize)override;
		virtual uIntn cnLib_FUNC DismissReadBuffer(uIntn Size)override;
	protected:
		int fCol;
		uIntn fReadOffset;
	};

	struct cColumnBuffer
	{
		uIntn ColumnNameLength;

		cnVar::cPolymorphicObject<cFetchPostprocess,
			cFetchPostprocess_BoolRead,
			cFetchPostprocess_SIntRead,
			cFetchPostprocess_UIntRead,
			cFetchPostprocess_FloatRead,
			cFetchPostprocess_UTFTextRead,
			cFetchPostprocess_BLOBRead
		> Postprocess;
	};
	cSeqList<cColumnBuffer> fColumnBufferList;

	void FetchPostprocess(void);

	template<class T,class...TArgs>
	T* MakeColumnPostprocess(cColumnBuffer &Buffer,TArgs&&...Args);

	template<class T>
	T* QueryColumnPostprocess_Buffer(ufInt16 Index,eDBDataType Type);


	class cExecutePreprocess
	{
	public:
		cSQLite3DBQuery *Owner;
		cExecutePreprocess();

		virtual void cnLib_FUNC Destruct(void);
		virtual void cnLib_FUNC Process(int col);
	};

	class cExecutePreprocess_UserBuffer : public cExecutePreprocess
	{
	public:
		cExecutePreprocess_UserBuffer(eDBDataType DataType,const void *Buffer,const uIntn *DataLength);

		virtual void cnLib_FUNC Destruct(void)override;
		virtual void cnLib_FUNC Process(int index)override;
	private:
		const void *fBuffer;
		const uIntn *fDataLength;
		static const uIntn DataLength1;

		void (cExecutePreprocess_UserBuffer::*fProcessProcedure)(int index);

		void ProcessBool(int index);
		void ProcessSInt8(int index);
		void ProcessUInt8(int index);
		void ProcessSInt16(int index);
		void ProcessUInt16(int index);
		void ProcessSInt32(int index);
		void ProcessUInt32(int index);
		void ProcessSInt64(int index);
		void ProcessUInt64(int index);
		void ProcessFloat32(int index);
		void ProcessFloat64(int index);
	
		void ProcessBinary(int index);
		void ProcessString8(int index);
		void ProcessString16(int index);
	};

	struct cParameterBuffer
	{
		eDBDataType BindType;

		cString<uChar16> ParamName;
		bool ParamNameValid;

		bool Null;
		cnVar::cPolymorphicObject<cExecutePreprocess,
			cExecutePreprocess_UserBuffer
		> Preprocess;

	};
	cSeqList<cParameterBuffer> fParameterBufferList;

	void ExecutePreprocess(void);
};
//---------------------------------------------------------------------------
class cSQLite3DBConnection : public iSQLDBConnection
{
public:
	cSQLite3DBConnection(rClsRef<cSQLite3DBContext> Context);
	~cSQLite3DBConnection();

	virtual iDBTranscation*	cnLib_FUNC Transcation(void)override;
	virtual iPtr<iDBQuery> cnLib_FUNC CreateQuery(const uChar16 *SQL,uIntn Length)override;

private:
	rClsRef<cSQLite3DBContext> fContext;
};
//---------------------------------------------------------------------------
}	// namespace cnRTL
//---------------------------------------------------------------------------
}	// namespace cnLibrary
//---------------------------------------------------------------------------
#endif  /* __cplusplus */
/*-------------------------------------------------------------------------*/
#endif
#endif // SQLITE3_H
