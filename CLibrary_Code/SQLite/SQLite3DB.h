/*- DB Interface for SQLite3 ----------------------------------------------*/
/*         Developer : Code009                                             */
/*         Create on : 2019-12-30                                          */
/*-------------------------------------------------------------------------*/
#include <SQLite/SQLite3Header.h>
#ifdef SQLITE3_H
#ifndef __cnLibrary_SQLite_SQLite3DB_H__
#define __cnLibrary_SQLite_SQLite3DB_H__
/*-------------------------------------------------------------------------*/
#include <cnRTL/cnRTL.h>
#ifdef __cplusplus
//---------------------------------------------------------------------------
namespace cnLib_Template_Helper{
//---------------------------------------------------------------------------
namespace SQLite3{
//---------------------------------------------------------------------------

template<class T>
inline typename cnVar::TTypeConditional<T,
		(sizeof(T)<=4)
>::Type ColumnInt(sqlite3_stmt *Statement,int Column)
{
	return static_cast<T>(sqlite3_column_int(Statement,Column));
}

template<class T>
inline typename cnVar::TTypeConditional<T,
	(sizeof(T)>4)
>::Type ColumnInt(sqlite3_stmt *Statement,int Column)
{
	return static_cast<T>(sqlite3_column_int64(Statement,Column));
}

template<class T,bool IsInteger>
struct cColumnConverter_Int
{
	static T Call(sqlite3_stmt *Statement,int Column){
		return ColumnInt<T>(Statement,Column);
	}
};
template<class T>
struct cColumnConverter_Int<T,false>{};


template<class T,bool IsFloat>
struct cColumnConverter_Float
{
	static T Call(sqlite3_stmt *Statement,int Column){
		return static_cast<T>(sqlite3_column_double(Statement,Column));
	}
};

template<class T>
struct cColumnConverter_Float<T,false>
{
};


//---------------------------------------------------------------------------
}	// namespace SQLite3
//---------------------------------------------------------------------------
}	// namespace cnLib_Template_Helper
//---------------------------------------------------------------------------
namespace cnLibrary{
//---------------------------------------------------------------------------
namespace cnRTL{
//---------------------------------------------------------------------------
struct cSQLite3DBHandle
{
	sqlite3 *Handle;

	cSQLite3DBHandle();
	~cSQLite3DBHandle();

	void TransferHandle(sqlite3 *NewHandle);

	sqlite3_stmt* PrepareStatement(const uChar16 *SQL,uIntn Length);
	
	template<uIntn Length>
	sqlite3_stmt* PrepareStatement(const uChar16 (&SQL)[Length]){
		return PrepareStatement(SQL,cnString::GetLength(SQL,Length));
	}

	sqlite3_stmt* PrepareStatement(const uChar8 *SQL,uIntn Length);
	
	template<uIntn Length>
	sqlite3_stmt* PrepareStatement(const uChar8 (&SQL)[Length]){
		return PrepareStatement(SQL,cnString::GetLength(SQL,Length));
	}

	int LastRowsChanged(void);
	sqlite3_int64 LastInsertedRowID(void);
};
//---------------------------------------------------------------------------
template<class T>
struct cSqlite3ColumnConverter
	: cnLib_Template_Helper::SQLite3::cColumnConverter_Int<T,cnVar::TIsInteger<T>::Value>,
	cnLib_Template_Helper::SQLite3::cColumnConverter_Float<T,cnVar::TIsFloat<T>::Value>
{
};

//---------------------------------------------------------------------------
template<>	struct cSqlite3ColumnConverter<int>{			static int				Call(sqlite3_stmt *Statement,int Column);	};
template<>	struct cSqlite3ColumnConverter<sqlite_int64>{	static sqlite_int64		Call(sqlite3_stmt *Statement,int Column);	};
template<>	struct cSqlite3ColumnConverter<double>{			static double			Call(sqlite3_stmt *Statement,int Column);	};
template<>	struct cSqlite3ColumnConverter<const uChar8*>{	static const uChar8*	Call(sqlite3_stmt *Statement,int Column);	};
template<>	struct cSqlite3ColumnConverter<const uChar16*>{	static const uChar16*	Call(sqlite3_stmt *Statement,int Column);	};
template<>	struct cSqlite3ColumnConverter<const void*>{	static const void*		Call(sqlite3_stmt *Statement,int Column);	};

template<>	struct cSqlite3ColumnConverter<cConstMemory>{				static cConstMemory				Call(sqlite3_stmt *Statement,int Column);	};
template<>	struct cSqlite3ColumnConverter< cArray<const uChar8> >{		static cArray<const uChar8>		Call(sqlite3_stmt *Statement,int Column);	};
template<>	struct cSqlite3ColumnConverter< cArray<const uChar16> >{	static cArray<const uChar16>	Call(sqlite3_stmt *Statement,int Column);	};

//---------------------------------------------------------------------------
//template<class TMemoryOperator>
//struct cSqlite3ColumnConverter< cnDataStruct::cMemoryBuffer<TMemoryOperator> >
//{
//	static cnDataStruct::cMemoryBuffer<TMemoryOperator> Call(sqlite3_stmt *Statement,int Column){
//		return cnDataStruct::cMemoryBuffer<TMemoryOperator>(cSqlite3ColumnConverter< cArray<typename TMemoryOperator::TElement const> >::Call(Statement,Column));
//	}
//};
//template<class TMemoryOperator,class TStringOperator,class TStringReference>
//struct cSqlite3ColumnConverter< cnString::cString<TMemoryOperator,TStringOperator,TStringReference> >
//{
//	static cnString::cString<TMemoryOperator,TStringOperator,TStringReference> Call(sqlite3_stmt *Statement,int Column){
//		return cSqlite3ColumnConverter< cArray<typename TMemoryOperator::TElement const> >::Call(Statement,Column);
//	}
//};
//---------------------------------------------------------------------------
struct cSQLite3StatementHandle
{
	sqlite3_stmt *Handle;

	cSQLite3StatementHandle();
	~cSQLite3StatementHandle();

	void TransferHandle(sqlite3_stmt *NewHandle);


	void Reset(void);
	bool Step(void);

	bool Execute(void);

	// column info

	ufInt16	ColumnCount(void);
	const uChar16* ColumnName(ufInt16 Index);
	eDBDataType	ColumnGetType(ufInt16 Index);
	bool IsColumnNull(ufInt16 ColIndex);


	// get column
	template<class T>
	typename cnVar::TFunctionInfo<
		decltype(cSqlite3ColumnConverter<T>::Call)
	>::ReturnType Column(ufInt16 ColIndex)const{
		return cSqlite3ColumnConverter<T>::Call(Handle,static_cast<int>(ColIndex));
	}

	// get column data object
	struct cColumnData
	{
		sqlite3_stmt *Statement;
		int Column;

		template<class T,class=typename cnVar::TFunctionInfo<decltype(cSqlite3ColumnConverter<T>::Call)>::ReturnType>
		operator T()const{
			return cSqlite3ColumnConverter<T>::Call(Statement,Column);
		}
	};
	cColumnData ColumnData(ufInt16 ColIndex)const;


	// parameter info

	ufInt16	ParameterCount(void);
	cnRTL::cString<uChar16> ParameterName(ufInt16 Index);

	// set parameter value

	bool ParameterSetNull(ufInt16 Index)const;
	bool ParameterSet(ufInt16 Index,int Value)const;
	bool ParameterSet(ufInt16 Index,sqlite_int64 Value)const;
	bool ParameterSet(ufInt16 Index,double Value)const;
	bool ParameterSet(ufInt16 Index,const uChar8 *Text)const;
	bool ParameterSet(ufInt16 Index,const uChar16 *Text)const;
	bool ParameterSet(ufInt16 Index,const uChar8 *Text,uIntn Length)const;
	bool ParameterSet(ufInt16 Index,const uChar16 *Text,uIntn Length)const;
	bool ParameterSet(ufInt16 Index,const void *Buffer,uIntn BufferSize)const;
	bool ParameterSet(ufInt16 Index,cConstMemory Memory)const;
	bool ParameterSet(ufInt16 Index,cArray<const uChar8> Array)const;
	bool ParameterSet(ufInt16 Index,cArray<const uChar16> Array)const;



	template<class T>
	typename cnVar::TTypeConditional<bool,
		cnVar::TIsInteger<T>::Value && (sizeof(T)<=4)
	>::Type ParameterSet(ufInt16 Index,T Value)const
	{
		return ParameterSet(Index,static_cast<int>(Value));
	}
	template<class T>
	typename cnVar::TTypeConditional<bool,
		cnVar::TIsInteger<T>::Value && (sizeof(T)>4)
	>::Type ParameterSet(ufInt16 Index,T Value)const
	{
		return ParameterSet(Index,static_cast<sqlite_int64>(Value));
	}
	template<class T>
	typename cnVar::TTypeConditional<bool,
		cnVar::TIsFloat<T>::Value
	>::Type ParameterSet(ufInt16 Index,T Value)const
	{
		return ParameterSet(Index,static_cast<double>(Value));
	}

	template<class TMemoryOperator,class TStringOperator,class TStringReference>
	bool ParameterSet(ufInt16 Index,const cnString::cString<TMemoryOperator,TStringOperator,TStringReference> &Value)const{
		return ParameterSet(Index,Value->Pointer,Value->Length);
	};

	// bind parameter

	bool ParameterBind(ufInt16 Index,const uChar8 *Text)const;
	bool ParameterBind(ufInt16 Index,const uChar16 *Text)const;
	bool ParameterBind(ufInt16 Index,const uChar8 *Text,uIntn Length)const;
	bool ParameterBind(ufInt16 Index,const uChar16 *Text,uIntn Length)const;
	bool ParameterBind(ufInt16 Index,const void *Buffer,uIntn BufferSize)const;
	bool ParameterBind(ufInt16 Index,cConstMemory Memory)const;
	bool ParameterBind(ufInt16 Index,cArray<const uChar8> Array)const;
	bool ParameterBind(ufInt16 Index,cArray<const uChar16> Array)const;

	template<class TMemoryOperator,class TStringOperator,class TStringReference>
	bool ParameterBind(ufInt16 Index,const cnString::cString<TMemoryOperator,TStringOperator,TStringReference> &Value)const{
		return ParameterBind(Index,Value->Pointer,Value->Length);
	};
};
//---------------------------------------------------------------------------
struct cSQLite3DBContext : cSQLite3DBHandle
{
	cSQLite3DBContext();
	~cSQLite3DBContext();
	
	cSQLite3StatementHandle Statement_QueryEncoding;
	ufInt8 GetUTFTextEncodingSize(void);

	cSQLite3StatementHandle Statement_BeginTransaction;
	cSQLite3StatementHandle Statement_CommitTransaction;
	cSQLite3StatementHandle Statement_RollbackTransaction;

	bool BeginTransaction(void);
	bool ComitTransaction(void);
	bool RollbackTransaction(void);
};
//---------------------------------------------------------------------------
struct cSQLite3QueryContext : cSQLite3StatementHandle
{
	cSQLite3QueryContext();
	~cSQLite3QueryContext();


	// column info

	eDBDataType	ColumnType(ufInt16 Index);
	
};
//---------------------------------------------------------------------------
rClsRef<cSQLite3DBContext> SQLite3OpenDB(const uChar8 *FileName);
rClsRef<cSQLite3DBContext> SQLite3OpenDB(const uChar16 *FileName);

rClsRef<cSQLite3QueryContext> SQLite3CreateQuery(cSQLite3DBContext *DBContext,const uChar16 *SQL,uIntn Length);
template<uIntn Length>
rClsRef<cSQLite3QueryContext> SQLite3CreateQuery(cSQLite3DBContext *DBContext,const uChar16 (&SQL)[Length]){
	return SQLite3CreateQuery(DBContext,SQL,cnString::GetLength(SQL,Length));
}
rClsRef<cSQLite3QueryContext> SQLite3CreateQuery(cSQLite3DBContext *DBContext,const uChar8 *SQL,uIntn Length);
template<uIntn Length>
rClsRef<cSQLite3QueryContext> SQLite3CreateQuery(cSQLite3DBContext *DBContext,const uChar8 (&SQL)[Length]){
	return SQLite3CreateQuery(DBContext,SQL,cnString::GetLength(SQL,Length));
}
//---------------------------------------------------------------------------
}	// namespace cnRTL
//---------------------------------------------------------------------------
}	// namespace cnLibrary
//---------------------------------------------------------------------------
#endif  /* __cplusplus */
/*-------------------------------------------------------------------------*/
#endif
#endif // SQLITE3_H
