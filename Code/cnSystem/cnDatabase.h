/*- cnSystem - Database ---------------------------------------------------*/
/*         Developer : Code009                                             */
/*         Create on : 2015-01-20                                          */
/*-------------------------------------------------------------------------*/
#ifndef __cnLibrary_cnSystem_cnDatabase_H__
#define	__cnLibrary_cnSystem_cnDatabase_H__
/*-------------------------------------------------------------------------*/
#include <cnSystem/cnFundamental.h>
#include <cnSystem/cnData.h>
/*-------------------------------------------------------------------------*/
#ifdef __cplusplus
//---------------------------------------------------------------------------
namespace cnLibrary{
//---------------------------------------------------------------------------
cnLib_ENUM_BEGIN(ufInt8,DBDataType)
{
	Null		=0x00,
	Unknown		=0x01,

	//GUID=0x08,
	SystemTime	=0x0A,	// sInt64 ( iTimepoint::SystemTime )
	DateTime	=0x0B,	// cDateTime



	Bool		=0x10,

	SInt8		=0x12,
	UInt8		=0x13,
	SInt16		=0x14,
	UInt16		=0x15,
	SInt32		=0x16,
	UInt32		=0x17,
	SInt64		=0x18,
	UInt64		=0x19,

	Float32		=0x1B,
	Float64		=0x1C,

	Binary		=0x20,		// byte stream
	String8		=0x21,		// uChar8*
	String16	=0x22,		// uChar16*
	//String32	=0x23,		// uChar32*


}cnLib_ENUM_END(DBDataType);

//---------------------------------------------------------------------------
class cnLib_INTERFACE iDBReadMemory
{
public:
	virtual cConstMemory cnLib_FUNC GetReadMemory(void)=0;
};
//---------------------------------------------------------------------------
class cnLib_INTERFACE iDBQuery : public iInterface
{
public:
	virtual bool cnLib_FUNC Execute(void)=0;
	virtual bool cnLib_FUNC Fetch(void)=0;
	virtual void cnLib_FUNC Clear(void)=0;

	virtual bool cnLib_FUNC GetRowsAffacted(uIntn &Value)=0;

	// column info
	
	// ColumnCount
	//	get number of column in returned rowset
	// return number of column
	virtual ufInt16		cnLib_FUNC ColumnCount(void)=0;
	virtual cArray<const uChar16>	cnLib_FUNC ColumnName(ufInt16 Index)=0;
	virtual eDBDataType	cnLib_FUNC ColumnType(ufInt16 Index)=0;
	virtual bool		cnLib_FUNC ColumnIsNullable(ufInt16 Index)=0;
	virtual uIntn		cnLib_FUNC ColumnMaxLength(ufInt16 Index)=0;

	// column fetch server buffer

	// ColumnGetType
	//	get type of column data in binded buffer
	// Index		index of column, index started by 0
	// return type of the fetched column data
	virtual eDBDataType		cnLib_FUNC ColumnGetType(ufInt16 Index)=0;
	// ColumnBindBuffer
	//	bind user buffer for specified type to returned column data
	//	setting new binding will clear any other bindings previously set in the same column.
	// Index			index of column, index started by 0
	// Type				type of data to be returned
	// [out]Buffer		buffer for storing returned data, the buffer cannot be null.
	// BufferLength		maxium length of the buffer
	// [out]DataLength	length of data stored in the buffer after each fetch
	// return true if binded
	virtual bool			cnLib_FUNC ColumnBindBuffer(ufInt16 Index,eDBDataType Type,void *Buffer,uIntn BufferLength,uIntn *DataLength)=0;
	// ColumnQueryReadMemory
	//	get interface for retrieving column data buffer
	//	setting new binding will clear any other bindings previously set in the same column.
	//	user can use returned memory to read fetched data.
	virtual iDBReadMemory*	cnLib_FUNC ColumnQueryReadMemory(ufInt16 Index,eDBDataType Type)=0;
	// ColumnQueryReadBuffer
	//	get interface for retrieving column data buffer
	//	setting new binding will clear any other bindings previously set in the same column.
	//	user can use returned buffer to read fetched data.
	virtual iReadBuffer<void>*	cnLib_FUNC ColumnQueryReadBuffer(ufInt16 Index,eDBDataType Type)=0;

	// ParameterCount
	//	get parameter count of the query
	virtual ufInt16	cnLib_FUNC ParameterCount(void)=0;
	// ParameterMaxLength
	//	get name of the parameter
	// Index			index of parameter, index started by 0
	virtual cArray<const uChar16>	cnLib_FUNC ParameterName(ufInt16 Index)=0;
	// ParameterMaxLength
	//	get maxium length of the parameter can accept
	// Index			index of parameter, index started by 0
	virtual uIntn	cnLib_FUNC ParameterMaxLength(ufInt16 Index)=0;

	// ParameterSetNull
	//	set parameter null flag
	//	setting null flag will not affect bindings
	virtual bool cnLib_FUNC ParameterSetNull(ufInt16 Index,bool Null)=0; 

	// ParameterBindBuffer
	//	bind user buffer for specified type to the parameter
	//	setting new binding will clear any bindings previously set in the same parameter.
	// Index			index of parameter, index started by 0
	// Type				type of data to be set
	// [in]Buffer		buffer for loading parameter when executing, the buffer cannot be null.
	// [in]DataLength	length of data to be load into parameter when executing.
	// return true if binded
	virtual bool cnLib_FUNC ParameterBindBuffer(ufInt16 Index,eDBDataType Type,const void *Buffer,const uIntn *DataLength)=0;
	// ParameterCopyBuffer
	//	copy data for specified type to the parameter
	//	this function will clear any bindings previously set in the same parameter.
	// Index			index of parameter, index started by 0
	// Type				type of data to be copy
	// [in]Buffer		buffer to copy to parameter.
	// [in]DataLength	length of data to be copy into parameter.
	// return true if binded
	virtual bool cnLib_FUNC ParameterCopyBuffer(ufInt16 Index,eDBDataType Type,const void *Buffer,uIntn DataLength)=0;
	// ParameterBindInput
	//	bind buffer for specified type to load parameter data
	//	setting new binding will clear any bindings previously set in the same parameter.
	// Index		index of parameter, index started by 0
	// Type			type of data to be set
	// [in]Buffer	buffer of data provided by user for parameter, the query use the buffer for loading parameter data.
	//				the buffer cannot be null.
	// return true if binded
	virtual bool cnLib_FUNC ParameterBindInput(ufInt16 Index,eDBDataType Type,iReadBuffer<void> *Buffer)=0;

};
//---------------------------------------------------------------------------
class cnLib_INTERFACE iDBTranscation
{
public:
	virtual bool cnLib_FUNC Commit(void)=0;
	virtual void cnLib_FUNC Rollback(void)=0;
};
//---------------------------------------------------------------------------
class cnLib_INTERFACE iDBConnection : public iInterface
{
public:
	//virtual void cnLib_FUNC Close(void)=0;

	virtual iDBTranscation*	cnLib_FUNC Transcation(void)=0;
};
//---------------------------------------------------------------------------
// SQL DB
//---------------------------------------------------------------------------
class cnLib_INTERFACE iSQLDBConnection : public iDBConnection
{
public:
	virtual iPtr<iDBQuery> cnLib_FUNC CreateQuery(const uChar16 *SQL,uIntn Length)=0;

	template<uIntn Length>
	iPtr<iDBQuery> CreateQuery(const uChar16 (&SQL)[Length])
	{	return CreateQuery(SQL,Length-1);	}
};
//---------------------------------------------------------------------------
}	// namespace cnLibrary
//---------------------------------------------------------------------------
#endif  /* __cplusplus */
/*-------------------------------------------------------------------------*/
#endif
