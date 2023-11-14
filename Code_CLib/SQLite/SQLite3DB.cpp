
#include "SQLite3DB.h"
#ifndef SQLITE3_H
#error	"need to include sqlite3.h"
#endif

using namespace cnLibrary;
using namespace cnRTL;

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cSQLite3DBHandle::cSQLite3DBHandle()
{
	Handle=nullptr;
}
//---------------------------------------------------------------------------
cSQLite3DBHandle::~cSQLite3DBHandle()
{
	if(Handle!=nullptr){
		sqlite3_close_v2(Handle);
	}
}
//---------------------------------------------------------------------------
void cSQLite3DBHandle::TransferHandle(sqlite3 *NewHandle)
{
	if(Handle==NewHandle){
		return;
	}
	if(Handle!=nullptr){
		sqlite3_close_v2(Handle);
	}
	Handle=NewHandle;
}
//---------------------------------------------------------------------------
int cSQLite3DBHandle::LastRowsChanged(void)
{
	return sqlite3_changes(Handle);
}
//---------------------------------------------------------------------------
sqlite3_int64 cSQLite3DBHandle::LastInsertedRowID(void)
{
	return sqlite3_last_insert_rowid(Handle);
}
//---------------------------------------------------------------------------
sqlite3_stmt* cSQLite3DBHandle::PrepareStatement(const uChar16 *SQL,uIntn Length)
{
	sqlite3_stmt *Statement;
	int sqlret=sqlite3_prepare16_v2(Handle,SQL,Length*2,&Statement,nullptr);
	if(sqlret!=SQLITE_OK){
		return nullptr;
	}
	return Statement;
}
//---------------------------------------------------------------------------
sqlite3_stmt* cSQLite3DBHandle::PrepareStatement(const uChar8 *SQL,uIntn Length)
{
	sqlite3_stmt *Statement;
	int sqlret=sqlite3_prepare_v2(Handle,SQL,Length,&Statement,nullptr);
	if(sqlret!=SQLITE_OK){
		return nullptr;
	}
	return Statement;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
int cSqlite3ColumnConverter<int>::Call(sqlite3_stmt *Statement,int Col)
{
	return sqlite3_column_int(Statement,Col);
}
//---------------------------------------------------------------------------
sqlite_int64 cSqlite3ColumnConverter<sqlite_int64>::Call(sqlite3_stmt *Statement,int Col)
{
	return sqlite3_column_int64(Statement,Col);
}
//---------------------------------------------------------------------------
double cSqlite3ColumnConverter<double>::Call(sqlite3_stmt *Statement,int Col)
{
	return sqlite3_column_double(Statement,Col);
}
//---------------------------------------------------------------------------
const uChar8* cSqlite3ColumnConverter<const uChar8*>::Call(sqlite3_stmt *Statement,int Col)
{
	return reinterpret_cast<const uChar8*>(sqlite3_column_text(Statement,Col));
}
//---------------------------------------------------------------------------
const uChar16* cSqlite3ColumnConverter<const uChar16*>::Call(sqlite3_stmt *Statement,int Col)
{
	return static_cast<const uChar16*>(sqlite3_column_text16(Statement,Col));
}
//---------------------------------------------------------------------------
const void* cSqlite3ColumnConverter<const void*>::Call(sqlite3_stmt *Statement,int Col)
{
	return sqlite3_column_blob(Statement,Col);
}
//---------------------------------------------------------------------------
cConstMemory cSqlite3ColumnConverter<cConstMemory>::Call(sqlite3_stmt *Statement,int Col)
{
	cConstMemory r;
	r.Pointer=sqlite3_column_blob(Statement,Col);
	r.Length=sqlite3_column_bytes(Statement,Col);
	return r;
}
//---------------------------------------------------------------------------
cArray<const uChar8> cSqlite3ColumnConverter< cArray< const uChar8> >::Call(sqlite3_stmt *Statement,int Col)
{
	cArray<const uChar8> r;
	r.Pointer=reinterpret_cast<const uChar8*>(sqlite3_column_text(Statement,Col));
	r.Length=sqlite3_column_bytes(Statement,Col);
	return r;
}
//---------------------------------------------------------------------------
cArray<const uChar16> cSqlite3ColumnConverter< cArray< const uChar16> >::Call(sqlite3_stmt *Statement,int Col)
{
	cArray<const uChar16> r;
	r.Pointer=static_cast<const uChar16*>(sqlite3_column_text16(Statement,Col));
	r.Length=sqlite3_column_bytes16(Statement,Col)/2;
	return r;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cSQLite3StatementHandle::cSQLite3StatementHandle()
{
	Handle=nullptr;
}
//---------------------------------------------------------------------------
cSQLite3StatementHandle::~cSQLite3StatementHandle()
{
	if(Handle!=nullptr){
		sqlite3_finalize(Handle);
	}
}
//---------------------------------------------------------------------------
void cSQLite3StatementHandle::TransferHandle(sqlite3_stmt *NewHandle)
{
	if(Handle==NewHandle){
		return;
	}
	if(Handle!=nullptr){
		sqlite3_finalize(Handle);
	}
	Handle=NewHandle;
}
//---------------------------------------------------------------------------
bool cSQLite3StatementHandle::Step(void)
{
	int sqlret=sqlite3_step(Handle);
	if(sqlret==SQLITE_ROW){
		return true;
	}
	return false;
}
//---------------------------------------------------------------------------
void cSQLite3StatementHandle::Reset(void)
{
	sqlite3_reset(Handle);
}
//---------------------------------------------------------------------------
bool cSQLite3StatementHandle::Execute(void)
{
	int sqlret=sqlite3_step(Handle);
	bool Result=(sqlret==SQLITE_ROW || sqlret==SQLITE_DONE);
	sqlite3_reset(Handle);
	return Result;
}
//---------------------------------------------------------------------------
ufInt16 cSQLite3StatementHandle::ColumnCount(void)
{
	return static_cast<ufInt16>(sqlite3_column_count(Handle));
}
//---------------------------------------------------------------------------
const uChar16* cSQLite3StatementHandle::ColumnName(ufInt16 Index)
{
	return static_cast<const uChar16*>(sqlite3_column_name16(Handle,Index));
}
//---------------------------------------------------------------------------
eDBDataType cSQLite3StatementHandle::ColumnGetType(ufInt16 Index)
{
	auto SQLiteType=sqlite3_column_type(Handle,Index);
	switch(SQLiteType){
	case SQLITE_NULL:
		return DBDataType::Null;
	case SQLITE_INTEGER:
		return DBDataType::SInt64;
	case SQLITE_FLOAT:
		return DBDataType::Float64;
	case SQLITE3_TEXT:
		return DBDataType::String16;
	case SQLITE_BLOB:
		return DBDataType::Binary;
	}
	return DBDataType::Unknown;
}
//---------------------------------------------------------------------------
bool cSQLite3StatementHandle::IsColumnNull(ufInt16 ColIndex)
{
	return sqlite3_column_type(Handle,ColIndex)==SQLITE_NULL;
}
//---------------------------------------------------------------------------
cSQLite3StatementHandle::cColumnData cSQLite3StatementHandle::ColumnData(ufInt16 ColIndex)const
{
	cColumnData r={Handle,static_cast<int>(ColIndex)};
	return r;
}
//---------------------------------------------------------------------------
ufInt16	cSQLite3StatementHandle::ParameterCount(void)
{
	return static_cast<ufInt16>(sqlite3_bind_parameter_count(Handle));
}
//---------------------------------------------------------------------------
cnRTL::cString<uChar16> cSQLite3StatementHandle::ParameterName(ufInt16 Index)
{
	auto Name=sqlite3_bind_parameter_name(Handle,Index+1);
	auto Transcoder=cnSystem::UnicodeTranscoder(2,1);

	cnRTL::cStringBuffer<uChar16> Buffer;
	cnString::ConvertEncodingToStream(Buffer.WriteStream(),Transcoder,Name,cnString::GetLength(Name));

	return cnVar::MoveCast(Buffer);
}
//---------------------------------------------------------------------------
bool cSQLite3StatementHandle::ParameterSetNull(ufInt16 Index)const
{
	int sqlret=sqlite3_bind_null(Handle,Index+1);
	return sqlret==SQLITE_OK;
}
//---------------------------------------------------------------------------
bool cSQLite3StatementHandle::ParameterSet(ufInt16 Index,int Value)const
{
	int sqlret=sqlite3_bind_int(Handle,Index+1,Value);
	return sqlret==SQLITE_OK;
}
//---------------------------------------------------------------------------
bool cSQLite3StatementHandle::ParameterSet(ufInt16 Index,sqlite_int64 Value)const
{
	int sqlret=sqlite3_bind_int64(Handle,Index+1,Value);
	return sqlret==SQLITE_OK;
}
//---------------------------------------------------------------------------
bool cSQLite3StatementHandle::ParameterSet(ufInt16 Index,double Value)const
{
	int sqlret=sqlite3_bind_double(Handle,Index+1,Value);
	return sqlret==SQLITE_OK;
}
//---------------------------------------------------------------------------
bool cSQLite3StatementHandle::ParameterSet(ufInt16 Index,const uChar8 *Text)const
{
	int sqlret=sqlite3_bind_text(Handle,Index+1,Text,-1,SQLITE_TRANSIENT);
	return sqlret==SQLITE_OK;
}
//---------------------------------------------------------------------------
bool cSQLite3StatementHandle::ParameterSet(ufInt16 Index,const uChar16 *Text)const
{
	int sqlret=sqlite3_bind_text16(Handle,Index+1,Text,-1,SQLITE_TRANSIENT);
	return sqlret==SQLITE_OK;
}
//---------------------------------------------------------------------------
bool cSQLite3StatementHandle::ParameterSet(ufInt16 Index,const uChar8 *Text,uIntn Length)const
{
	int sqlret=sqlite3_bind_text(Handle,Index+1,Text,Length,SQLITE_TRANSIENT);
	return sqlret==SQLITE_OK;
}
//---------------------------------------------------------------------------
bool cSQLite3StatementHandle::ParameterSet(ufInt16 Index,const uChar16 *Text,uIntn Length)const
{
	int sqlret=sqlite3_bind_text16(Handle,Index+1,Text,Length*2,SQLITE_TRANSIENT);
	return sqlret==SQLITE_OK;
}
//---------------------------------------------------------------------------
bool cSQLite3StatementHandle::ParameterSet(ufInt16 Index,const void *Buffer,uIntn BufferSize)const
{
	int sqlret=sqlite3_bind_blob(Handle,Index+1,Buffer,BufferSize,SQLITE_TRANSIENT);
	return sqlret==SQLITE_OK;
}
//---------------------------------------------------------------------------
bool cSQLite3StatementHandle::ParameterSet(ufInt16 Index,cConstMemory Memory)const
{
	int sqlret=sqlite3_bind_blob(Handle,Index+1,Memory.Pointer,Memory.Length,SQLITE_TRANSIENT);
	return sqlret==SQLITE_OK;
}
//---------------------------------------------------------------------------
bool cSQLite3StatementHandle::ParameterSet(ufInt16 Index,cArray<const uChar8> Array)const
{
	int sqlret=sqlite3_bind_text(Handle,Index+1,Array.Pointer,Array.Length,SQLITE_TRANSIENT);
	return sqlret==SQLITE_OK;
}
//---------------------------------------------------------------------------
bool cSQLite3StatementHandle::ParameterSet(ufInt16 Index,cArray<const uChar16> Array)const
{
	int sqlret=sqlite3_bind_text16(Handle,Index+1,Array.Pointer,Array.Length*2,SQLITE_TRANSIENT);
	return sqlret==SQLITE_OK;
}
//---------------------------------------------------------------------------
bool cSQLite3StatementHandle::ParameterBind(ufInt16 Index,const uChar8 *Text)const
{
	int sqlret=sqlite3_bind_text(Handle,Index+1,Text,-1,SQLITE_STATIC);
	return sqlret==SQLITE_OK;
}
//---------------------------------------------------------------------------
bool cSQLite3StatementHandle::ParameterBind(ufInt16 Index,const uChar16 *Text)const
{
	int sqlret=sqlite3_bind_text16(Handle,Index+1,Text,-1,SQLITE_STATIC);
	return sqlret==SQLITE_OK;
}
//---------------------------------------------------------------------------
bool cSQLite3StatementHandle::ParameterBind(ufInt16 Index,const uChar8 *Text,uIntn Length)const
{
	int sqlret=sqlite3_bind_text(Handle,Index+1,Text,Length,SQLITE_STATIC);
	return sqlret==SQLITE_OK;
}
//---------------------------------------------------------------------------
bool cSQLite3StatementHandle::ParameterBind(ufInt16 Index,const uChar16 *Text,uIntn Length)const
{
	int sqlret=sqlite3_bind_text16(Handle,Index+1,Text,Length*2,SQLITE_STATIC);
	return sqlret==SQLITE_OK;
}
//---------------------------------------------------------------------------
bool cSQLite3StatementHandle::ParameterBind(ufInt16 Index,const void *Buffer,uIntn BufferSize)const
{
	int sqlret=sqlite3_bind_blob(Handle,Index+1,Buffer,BufferSize,SQLITE_STATIC);
	return sqlret==SQLITE_OK;
}
//---------------------------------------------------------------------------
bool cSQLite3StatementHandle::ParameterBind(ufInt16 Index,cConstMemory Memory)const
{
	int sqlret=sqlite3_bind_blob(Handle,Index+1,Memory.Pointer,Memory.Length,SQLITE_STATIC);
	return sqlret==SQLITE_OK;
}
//---------------------------------------------------------------------------
bool cSQLite3StatementHandle::ParameterBind(ufInt16 Index,cArray<const uChar8> Array)const
{
	int sqlret=sqlite3_bind_text(Handle,Index+1,Array.Pointer,Array.Length,SQLITE_STATIC);
	return sqlret==SQLITE_OK;
}
//---------------------------------------------------------------------------
bool cSQLite3StatementHandle::ParameterBind(ufInt16 Index,cArray<const uChar16> Array)const
{
	int sqlret=sqlite3_bind_text16(Handle,Index+1,Array.Pointer,Array.Length*2,SQLITE_STATIC);
	return sqlret==SQLITE_OK;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cSQLite3DBContext::cSQLite3DBContext()
{
}
//---------------------------------------------------------------------------
cSQLite3DBContext::~cSQLite3DBContext()
{
}
//---------------------------------------------------------------------------
ufInt8 cSQLite3DBContext::GetUTFTextEncodingSize(void)
{
	if(Statement_QueryEncoding.Handle==nullptr){
		static constexpr uChar8 SQL_Pragma[]=u8"PRAGMA encoding";
		if(sqlite3_prepare_v3(Handle,SQL_Pragma,sizeof(SQL_Pragma)-1,SQLITE_PREPARE_PERSISTENT,&Statement_QueryEncoding.Handle,nullptr)!=SQLITE_OK)
			return 1;
	}


	int stmt_ret=sqlite3_step(Statement_QueryEncoding.Handle);

	ufInt8 Size=1;
	switch(stmt_ret){
	default:
		break;
	case SQLITE_ROW:
		{
			auto Encoding=Statement_QueryEncoding.Column<const char*>(0);

			if(cnString::DiffIndexRange(Encoding,"UTF-16",6)==IndexNotFound){
				if(Encoding[6]==0){	// "UTF-16"
					Size=2;
				}
				else if(Encoding[6]=='l' && Encoding[7]=='e' && Encoding[8]==0){	// "UTF-16le"
					Size=2;
				}
				else if(Encoding[6]=='b' && Encoding[7]=='e' && Encoding[8]==0){	// "UTF-16be"
					Size=2;
				}
			}
		}
		cnLib_SWITCH_FALLTHROUGH;
	case SQLITE_DONE:
		Statement_QueryEncoding.Reset();
		break;
	}

	return Size;
}
//---------------------------------------------------------------------------
bool cSQLite3DBContext::BeginTransaction(void)
{
	if(Statement_BeginTransaction.Handle==nullptr){
		static constexpr uChar8 SQL_Transaction[]=u8"BEGIN TRANSACTION";
		if(sqlite3_prepare_v3(Handle,SQL_Transaction,sizeof(SQL_Transaction)-1,SQLITE_PREPARE_PERSISTENT,&Statement_BeginTransaction.Handle,nullptr)!=SQLITE_OK)
			return false;
	}

	if(Statement_BeginTransaction.Execute()){
		Statement_BeginTransaction.Reset();
		return true;
	}
	return false;
}
//---------------------------------------------------------------------------
bool cSQLite3DBContext::ComitTransaction(void)
{
	if(Statement_CommitTransaction.Handle==nullptr){
		static constexpr uChar8 SQL_Transaction[]=u8"COMMIT TRANSACTION";
		if(sqlite3_prepare_v3(Handle,SQL_Transaction,sizeof(SQL_Transaction)-1,SQLITE_PREPARE_PERSISTENT,&Statement_CommitTransaction.Handle,nullptr)!=SQLITE_OK)
			return false;
	}

	if(Statement_CommitTransaction.Execute()){
		Statement_CommitTransaction.Reset();
		return true;
	}
	return false;
}
//---------------------------------------------------------------------------
bool cSQLite3DBContext::RollbackTransaction(void)
{
	if(Statement_RollbackTransaction.Handle==nullptr){
		static constexpr uChar8 SQL_Transaction[]=u8"ROLLBACK TRANSACTION";
		if(sqlite3_prepare_v3(Handle,SQL_Transaction,sizeof(SQL_Transaction)-1,SQLITE_PREPARE_PERSISTENT,&Statement_RollbackTransaction.Handle,nullptr)!=SQLITE_OK)
			return false;
	}

	if(Statement_RollbackTransaction.Execute()){
		Statement_RollbackTransaction.Reset();
		return true;
	}
	return false;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cSQLite3QueryContext::cSQLite3QueryContext()
{
}
//---------------------------------------------------------------------------
cSQLite3QueryContext::~cSQLite3QueryContext()
{
}
//---------------------------------------------------------------------------
eDBDataType	cSQLite3QueryContext::ColumnType(ufInt16 Index)
{
	//sqlite3_column_decltype(Handle,Index);
	return DBDataType::Unknown;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
rClsRef<cSQLite3DBContext> cnRTL::SQLite3OpenDB(const uChar8 *FileName)
{
	sqlite3 *Database;
	int sqlret=sqlite3_open(FileName,&Database);
	if(sqlret!=SQLITE_OK){
		return nullptr;
	}
	auto Context=rClsCreate<cSQLite3DBContext>();
	Context->Handle=Database;
	return Context;
}
//---------------------------------------------------------------------------
rClsRef<cSQLite3DBContext> cnRTL::SQLite3OpenDB(const uChar16 *FileName)
{
	sqlite3 *Database;
	int sqlret=sqlite3_open16(FileName,&Database);
	if(sqlret!=SQLITE_OK){
		return nullptr;
	}
	auto Context=rClsCreate<cSQLite3DBContext>();
	Context->Handle=Database;
	return Context;
}
//---------------------------------------------------------------------------
rClsRef<cSQLite3QueryContext> cnRTL::SQLite3CreateQuery(cSQLite3DBContext *DBContext,const uChar16 *SQL,uIntn Length)
{
	auto Statement=DBContext->PrepareStatement(SQL,Length);
	if(Statement==nullptr)
		return nullptr;
	auto QueryContext=rClsCreate<cSQLite3QueryContext>();
	QueryContext->Handle=Statement;
	return QueryContext;
}
//---------------------------------------------------------------------------
rClsRef<cSQLite3QueryContext> cnRTL::SQLite3CreateQuery(cSQLite3DBContext *DBContext,const uChar8 *SQL,uIntn Length)
{
	auto Statement=DBContext->PrepareStatement(SQL,Length);
	if(Statement==nullptr)
		return nullptr;
	auto QueryContext=rClsCreate<cSQLite3QueryContext>();
	QueryContext->Handle=Statement;
	return QueryContext;
}
//---------------------------------------------------------------------------
