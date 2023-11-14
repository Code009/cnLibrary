/*- cnRTL - Database ------------------------------------------------------*/
/*         Developer : Code009                                             */
/*         Create on : 2016-06-12                                          */
/*-------------------------------------------------------------------------*/
#ifndef __cnLibrary_cnRTL_Database_H__
#define __cnLibrary_cnRTL_Database_H__
/*-------------------------------------------------------------------------*/
#include <cnRTL/cnRTLHeader.h>
#ifdef __cplusplus
//---------------------------------------------------------------------------

namespace cnLibrary{
#if 0
namespace cnStream{

//---------------------------------------------------------------------------
template<class TMemoryOperator>
class cStringStorageBinaryReadBuffer
{
public:
	cStringStorageBinaryReadBuffer(cnDataStruct::cStringStorage<TMemoryOperator> &Storage,uIntn ReadIndex)
		:fStorage(Storage),fReadIndex(ReadIndex){}

	cnDataStruct::cStringStorage<TMemoryOperator> &GetStorage(void)const{	return fStorage;	}

	//TReadBuffer<void>

	bool QueryReadBuffer(cConstMemory &Buffer,uIntn){
		uIntn SizeAvailable=fStorage.Length*TMemoryOperator::ElementSize-fReadIndex;
		Buffer.Pointer=cnMemory::PointerAddByteOffset(fStorage.Pointer,fReadIndex);
		Buffer.Length=SizeAvailable;
		return true;
	}
	void CompleteReadBuffer(uIntn Length){
		uIntn NewIndex=fReadIndex+Length;
		uIntn StorageSize=fStorage.Length*TMemoryOperator::ElementSize;
		if(NewIndex>=StorageSize){
			fReadIndex=StorageSize;
		}
		else{
			fReadIndex=NewIndex;
		}
	}
protected:
	cnDataStruct::cStringStorage<TMemoryOperator> &fStorage;
	uIntn fReadIndex;
};
//---------------------------------------------------------------------------
template<class TMemoryOperator>
class cStringStorageBinaryWriteBuffer
{
public:
	cStringStorageBinaryWriteBuffer(cnDataStruct::cStringStorage<TMemoryOperator> &Storage,uIntn Increment)
		:fStorage(Storage),fIncrement(Increment)
	{
		if(fIncrement<128)
			fIncrement=128;
	}


	cnDataStruct::cStringStorage<TMemoryOperator> &GetStorage(void)const{	return fStorage;	}

	//TWriteBuffer<void>

	bool QueryWriteBuffer(cMemory &Buffer,uIntn QueryLength){
		uIntn SizeAvailable=(fStorage.Capacity-fStorage.Length)*TMemoryOperator::ElementSize;
		if(SizeAvailable==0 || SizeAvailable<QueryLength){
			if(QueryLength<fIncrement){
				QueryLength=fIncrement;
			}
			// mark length to preserve buffer content
			uIntn CurLength=fStorage.Length;
			fStorage.Length=fStorage.Capacity;
			// grow capacity
			fStorage.GrowTo(CurLength+QueryLength*TMemoryOperator::ElementSize);
			SizeAvailable=(fStorage.Capacity-CurLength)*TMemoryOperator::ElementSize;
			// restore length
			fStorage.Length=CurLength;
		}
		Buffer.Pointer=TMemoryOperator::PointerAddByteOffset(fStorage.Pointer,fStorage.Length);
		Buffer.Length=SizeAvailable;
		return true;
	}
	void CommitWriteBuffer(uIntn Length){
		uIntn NewLength=fStorage.Length+(Length+TMemoryOperator::ElementSize-1)/TMemoryOperator::ElementSize;
		if(NewLength>=fStorage.Capacity){
			fStorage.Length=fStorage.Capacity;
		}
		else{
			fStorage.Length=NewLength;
		}
	}
protected:
	cnDataStruct::cStringStorage<TMemoryOperator> &fStorage;
	uIntn fIncrement;
};
//---------------------------------------------------------------------------
template<class TMemoryOperator>
inline cBinaryReadStream<
	cStringStorageBinaryReadBuffer<TMemoryOperator>
> BinaryReadStream(cnDataStruct::cStringStorage<TMemoryOperator> &Storage,uIntn StartIndex=0)
{
	return cBinaryReadStream<
		cStringStorageBinaryReadBuffer<TMemoryOperator>
	>(Storage,StartIndex);
}
template<class TMemoryOperator>
inline cBinaryWriteStream<
	cStringStorageBinaryWriteBuffer<TMemoryOperator>
> BinaryWriteStream(cnDataStruct::cStringStorage<TMemoryOperator> &Storage,uIntn Increment=0)
{
	return cBinaryWriteStream<
		cStringStorageBinaryWriteBuffer<TMemoryOperator>
	>(Storage,Increment);
}

}
#endif

//---------------------------------------------------------------------------
namespace cnRTL{
//---------------------------------------------------------------------------
	/*
template<class TMemoryOperator>
class cDBOutputBuffer_Binary : protected iDBWriteBuffer
{
public:
	cDBOutputBuffer_Binary(cnDataStruct::cMemoryStorage<TMemoryOperator> &Storage):fWriteBuffer(Storage,0){}

protected:
	cnStream::cMemoryStorageBinaryWriteBuffer<TMemoryOperator> fWriteBuffer;
	virtual bool cnLib_FUNC QueryWriteBuffer(cMemory &Buffer,uIntn QuerySize)override{
		return fWriteBuffer.QueryWriteBuffer(Buffer,QuerySize);
	}
	virtual void cnLib_FUNC CommitWriteBuffer(uIntn Size)override{
		return fWriteBuffer.CommitWriteBuffer(Size);
	}
	virtual void cnLib_FUNC ResetBuffer(void)override{
		fWriteBuffer.GetStorage().Length=0;
	}
	virtual void cnLib_FUNC FinishBuffer(void)override{
	}
	
};
//---------------------------------------------------------------------------
template<class TMemoryOperator>
class cDBOutputBuffer_String : protected iDBWriteBuffer
{
public:
	cDBOutputBuffer_String(cnDataStruct::cStringStorage<TMemoryOperator> &Storage):fWriteBuffer(Storage,0){}

protected:
	cnStream::cStringStorageBinaryWriteBuffer<TMemoryOperator> fWriteBuffer;

	virtual bool cnLib_FUNC QueryWriteBuffer(cMemory &Buffer,uIntn QuerySize)override{
		return fWriteBuffer.QueryWriteBuffer(Buffer,QuerySize);
	}
	virtual void cnLib_FUNC CommitWriteBuffer(uIntn Size)override{
		return fWriteBuffer.CommitWriteBuffer(Size);
	}
	virtual void cnLib_FUNC ResetBuffer(void)override{
		fWriteBuffer.GetStorage().Length=0;
	}
	virtual void cnLib_FUNC FinishBuffer(void)override{
		auto &Storage=fWriteBuffer.GetStorage();
		Storage.Pointer[Storage.Length]=0;
	}
	
};
*/
//---------------------------------------------------------------------------
class cDBTranscation
{
private:
	iDBTranscation *fTranscation;
public:
	cDBTranscation(iDBConnection *Connection);
	~cDBTranscation();

	bool Commit(void);
	void Rollback(void);
};
//---------------------------------------------------------------------------
template<class T>
struct TDBType{
	static const eDBDataType Code=DBDataType::Binary;	
};

template<>	struct TDBType<bool>{		static const eDBDataType Code=DBDataType::Bool;	};

template<>	struct TDBType<void*>{		static const eDBDataType Code=DBDataType::Binary;	};

template<>	struct TDBType<uInt8>{		static const eDBDataType Code=DBDataType::UInt8;	};
template<>	struct TDBType<sInt8>{		static const eDBDataType Code=DBDataType::SInt8;	};
template<>	struct TDBType<uInt16>{		static const eDBDataType Code=DBDataType::UInt16;	};
template<>	struct TDBType<sInt16>{		static const eDBDataType Code=DBDataType::SInt16;	};
template<>	struct TDBType<uInt32>{		static const eDBDataType Code=DBDataType::UInt32;	};
template<>	struct TDBType<sInt32>{		static const eDBDataType Code=DBDataType::SInt32;	};
template<>	struct TDBType<uInt64>{		static const eDBDataType Code=DBDataType::UInt64;	};
template<>	struct TDBType<sInt64>{		static const eDBDataType Code=DBDataType::SInt64;	};

template<>	struct TDBType<Float32>{	static const eDBDataType Code=DBDataType::Float32;	};
template<>	struct TDBType<Float64>{	static const eDBDataType Code=DBDataType::Float64;	};

template<>	struct TDBType<uChar8*>{	static const eDBDataType Code=DBDataType::String8;	};
template<>	struct TDBType<uChar16*>{	static const eDBDataType Code=DBDataType::String16;	};

template<>	struct TDBType<cDateTime>{	static const eDBDataType Code=DBDataType::DateTime;	};

//---------------------------------------------------------------------------

struct cDBVariantTypeOperator
{
	typedef eDBDataType tTypeIndex;
	typedef cnVar::cPolymorphicObject<void,cDateTime> tContent;

	static constexpr tTypeIndex DefaultTypeIndex=DBDataType::Null;
	template<tTypeIndex TypeIndex>
	struct TTypeByIndex;
	
	template<tTypeIndex TypeIndex> struct TTypeByIndex;
	static rtType GetTypeInfo(tTypeIndex TypeIndex);
};
template<> struct cDBVariantTypeOperator::TTypeByIndex<DBDataType::Null>		: cnVar::TTypeDef<void>{};
template<> struct cDBVariantTypeOperator::TTypeByIndex<DBDataType::Binary>		: cnVar::TTypeDef<cMemoryBuffer>{};
template<> struct cDBVariantTypeOperator::TTypeByIndex<DBDataType::Bool>		: cnVar::TTypeDef<bool>{};
template<> struct cDBVariantTypeOperator::TTypeByIndex<DBDataType::DateTime>	: cnVar::TTypeDef<cDateTime>{};
template<> struct cDBVariantTypeOperator::TTypeByIndex<DBDataType::Float32>		: cnVar::TTypeDef<Float32>{};
template<> struct cDBVariantTypeOperator::TTypeByIndex<DBDataType::Float64>		: cnVar::TTypeDef<Float64>{};
template<> struct cDBVariantTypeOperator::TTypeByIndex<DBDataType::SInt16>		: cnVar::TTypeDef<sInt16>{};
template<> struct cDBVariantTypeOperator::TTypeByIndex<DBDataType::SInt32>		: cnVar::TTypeDef<sInt32>{};
template<> struct cDBVariantTypeOperator::TTypeByIndex<DBDataType::SInt64>		: cnVar::TTypeDef<sInt64>{};
template<> struct cDBVariantTypeOperator::TTypeByIndex<DBDataType::SInt8>		: cnVar::TTypeDef<sInt8>{};
template<> struct cDBVariantTypeOperator::TTypeByIndex<DBDataType::String16>	: cnVar::TTypeDef< cString<uChar16> >{};
template<> struct cDBVariantTypeOperator::TTypeByIndex<DBDataType::String8>		: cnVar::TTypeDef< cString<uChar8> >{};
template<> struct cDBVariantTypeOperator::TTypeByIndex<DBDataType::SystemTime>	: cnVar::TTypeDef<void>{};
template<> struct cDBVariantTypeOperator::TTypeByIndex<DBDataType::UInt16>		: cnVar::TTypeDef<uInt16>{};
template<> struct cDBVariantTypeOperator::TTypeByIndex<DBDataType::UInt32>		: cnVar::TTypeDef<uInt32>{};
template<> struct cDBVariantTypeOperator::TTypeByIndex<DBDataType::UInt64>		: cnVar::TTypeDef<uInt64>{};
template<> struct cDBVariantTypeOperator::TTypeByIndex<DBDataType::UInt8>		: cnVar::TTypeDef<uInt8>{};
template<> struct cDBVariantTypeOperator::TTypeByIndex<DBDataType::Unknown>		: cnVar::TTypeDef<cMemoryBuffer>{};

#if 0
typedef cnVar::cVariant<
	bool,
	sInt8,				uInt8,
	sInt16,				uInt16,
	sInt32,				uInt32,
	sInt64,				uInt64,
	Float32,			Float64,
	cDateTime,
	cString<uChar8>,	cString<uChar16>,
	cMemoryBuffer
> cDBVariant;


//---------------------------------------------------------------------------
bool DBColumnIsNull(iDBQuery *Query,ufInt16 Column);
//---------------------------------------------------------------------------
bool DBBindColumn(iDBQuery *Query,ufInt16 Index,bool *Value);
bool DBBindColumn(iDBQuery *Query,ufInt16 Index,sInt8 *Value);
bool DBBindColumn(iDBQuery *Query,ufInt16 Index,uInt8 *Value);
bool DBBindColumn(iDBQuery *Query,ufInt16 Index,sInt16 *Value);
bool DBBindColumn(iDBQuery *Query,ufInt16 Index,uInt16 *Value);
bool DBBindColumn(iDBQuery *Query,ufInt16 Index,sInt32 *Value);
bool DBBindColumn(iDBQuery *Query,ufInt16 Index,uInt32 *Value);
bool DBBindColumn(iDBQuery *Query,ufInt16 Index,sInt64 *Value);
bool DBBindColumn(iDBQuery *Query,ufInt16 Index,uInt64 *Value);
bool DBBindColumn(iDBQuery *Query,ufInt16 Index,Float32 *Value);
bool DBBindColumn(iDBQuery *Query,ufInt16 Index,Float64 *Value);
bool DBBindColumn(iDBQuery *Query,ufInt16 Index,cDateTime *Value);
bool DBBindColumn(iDBQuery *Query,ufInt16 Index,uChar8 *Buffer,uIntn BuferLength,uIntn *DataLength);
bool DBBindColumn(iDBQuery *Query,ufInt16 Index,uChar16 *Buffer,uIntn BuferLength,uIntn *DataLength);
bool DBBindColumn(iDBQuery *Query,ufInt16 Index,uInt8 *Buffer,uIntn BuferSize,uIntn *DataSize);
//---------------------------------------------------------------------------
bool DBBindParameter(iDBQuery *Query,ufInt16 Index,const bool *Value);
bool DBBindParameter(iDBQuery *Query,ufInt16 Index,const sInt8 *Value);
bool DBBindParameter(iDBQuery *Query,ufInt16 Index,const uInt8 *Value);
bool DBBindParameter(iDBQuery *Query,ufInt16 Index,const sInt16 *Value);
bool DBBindParameter(iDBQuery *Query,ufInt16 Index,const uInt16 *Value);
bool DBBindParameter(iDBQuery *Query,ufInt16 Index,const sInt32 *Value);
bool DBBindParameter(iDBQuery *Query,ufInt16 Index,const uInt32 *Value);
bool DBBindParameter(iDBQuery *Query,ufInt16 Index,const sInt64 *Value);
bool DBBindParameter(iDBQuery *Query,ufInt16 Index,const uInt64 *Value);
bool DBBindParameter(iDBQuery *Query,ufInt16 Index,const Float32 *Value);
bool DBBindParameter(iDBQuery *Query,ufInt16 Index,const Float64 *Value);
bool DBBindParameter(iDBQuery *Query,ufInt16 Index,const cDateTime *Value);
bool DBBindParameter(iDBQuery *Query,ufInt16 Index,const uChar8 *Buffer,const uIntn *DataLength);
bool DBBindParameter(iDBQuery *Query,ufInt16 Index,const uChar16 *Buffer,const uIntn *DataLength);
bool DBBindParameter(iDBQuery *Query,ufInt16 Index,const uInt8 *Buffer,const uIntn *DataSize);
//---------------------------------------------------------------------------
/*
class cDBColumnOutput_Binary : public cDBOutputBuffer_Binary< cMemoryOperator<void> >
{
public:
	cDBColumnOutput_Binary(cMemoryBuffer *Buffer);

	bool Bind(iDBQuery *Query,ufInt16 Column);
};
cDBColumnOutput_Binary DBBindColumnOutput(cMemoryBuffer *Buffer);
//---------------------------------------------------------------------------
class cDBColumnOutput_String8 : public cDBOutputBuffer_String< cMemoryOperator<uChar8> >
{
public:
	cDBColumnOutput_String8(cStringBuffer<uChar8> *Buffer);

	bool Bind(iDBQuery *Query,ufInt16 Column);
};
cDBColumnOutput_String8 DBBindColumnOutput(cStringBuffer<uChar8> *Buffer);
//---------------------------------------------------------------------------
class cDBColumnOutput_String16 : public cDBOutputBuffer_String< cMemoryOperator<uChar16> >
{
public:
	cDBColumnOutput_String16(cStringBuffer<uChar16> *Buffer);

	bool Bind(iDBQuery *Query,ufInt16 Column);
};
cDBColumnOutput_String16 DBBindColumnOutput(cStringBuffer<uChar16> *Buffer);
//---------------------------------------------------------------------------
//bool DBQueryBindColumn(iDBQuery *Query,uInt16 Column,cDBVariant *Value);*/
//---------------------------------------------------------------------------
template<class T,eDBDataType Type>
class DBColumn_Value
{
public:
	T Value;

	bool Bind(iDBQuery *Query,ufInt16 Column){
		return Query->ColumnBindBuffer(Column,Type,&Value,1,nullptr);
	}
};
//---------------------------------------------------------------------------
template<class T>
class cDBColumn;
//---------------------------------------------------------------------------
template<>	class cDBColumn<bool>		: public DBColumn_Value<bool,DBDataType::Bool>{};
template<>	class cDBColumn<sInt8>		: public DBColumn_Value<sInt8,DBDataType::SInt8>{};
template<>	class cDBColumn<uInt8>		: public DBColumn_Value<uInt8,DBDataType::UInt8>{};
template<>	class cDBColumn<sInt16>		: public DBColumn_Value<sInt16,DBDataType::SInt16>{};
template<>	class cDBColumn<uInt16>		: public DBColumn_Value<uInt16,DBDataType::UInt16>{};
template<>	class cDBColumn<sInt32>		: public DBColumn_Value<sInt32,DBDataType::SInt32>{};
template<>	class cDBColumn<uInt32>		: public DBColumn_Value<uInt32,DBDataType::UInt32>{};
template<>	class cDBColumn<sInt64>		: public DBColumn_Value<sInt64,DBDataType::SInt64>{};
template<>	class cDBColumn<uInt64>		: public DBColumn_Value<uInt64,DBDataType::UInt64>{};
template<>	class cDBColumn<Float32>	: public DBColumn_Value<Float32,DBDataType::Float32>{};
template<>	class cDBColumn<Float64>	: public DBColumn_Value<Float64,DBDataType::Float64>{};
template<>	class cDBColumn<cDateTime>	: public DBColumn_Value<cDateTime,DBDataType::DateTime>{};
//---------------------------------------------------------------------------
/*
template<>
class cDBColumn<cMemoryBuffer> : protected cDBOutputBuffer_Binary< cMemoryOperator<void> >
{
public:
	cMemoryBuffer Value;

	cDBColumn();
	bool Bind(iDBQuery *Query,ufInt16 Column);
protected:
};
//---------------------------------------------------------------------------
template<>
class cDBColumn< cStringBuffer<uChar8> > : protected cDBOutputBuffer_String< cMemoryOperator<uChar8> >
{
public:
	cStringBuffer<uChar8> Value;

	cDBColumn();
	bool Bind(iDBQuery *Query,ufInt16 Column);
};
//---------------------------------------------------------------------------
template<>
class cDBColumn< cStringBuffer<uChar16> > : protected cDBOutputBuffer_String< cMemoryOperator<uChar16> >
{
public:
	cStringBuffer<uChar16> Value;

	cDBColumn();
	bool Bind(iDBQuery *Query,ufInt16 Column);
};
//---------------------------------------------------------------------------
template<>
class cDBColumn<cDBVariant>
{
	cDBVariant Value;

	cDBColumn();
	bool Bind(iDBQuery *Query,ufInt16 Column);
};
*/
//---------------------------------------------------------------------------
bool DBQueryGetColumn(iDBQuery *Query,ufInt16 Column,cDBVariant &Value);
bool DBQueryGetColumn(iDBQuery *Query,ufInt16 Column,bool &Value);
bool DBQueryGetColumn(iDBQuery *Query,ufInt16 Column,sInt8 &Value);
bool DBQueryGetColumn(iDBQuery *Query,ufInt16 Column,uInt8 &Value);
bool DBQueryGetColumn(iDBQuery *Query,ufInt16 Column,sInt16 &Value);
bool DBQueryGetColumn(iDBQuery *Query,ufInt16 Column,uInt16 &Value);
bool DBQueryGetColumn(iDBQuery *Query,ufInt16 Column,sInt32 &Value);
bool DBQueryGetColumn(iDBQuery *Query,ufInt16 Column,uInt32 &Value);
bool DBQueryGetColumn(iDBQuery *Query,ufInt16 Column,sInt64 &Value);
bool DBQueryGetColumn(iDBQuery *Query,ufInt16 Column,uInt64 &Value);
bool DBQueryGetColumn(iDBQuery *Query,ufInt16 Column,Float32 &Value);
bool DBQueryGetColumn(iDBQuery *Query,ufInt16 Column,Float64 &Value);
bool DBQueryGetColumn(iDBQuery *Query,ufInt16 Column,uChar8 *Buffer,uIntn BuferLength,uIntn &DataLength);
bool DBQueryGetColumn(iDBQuery *Query,ufInt16 Column,uChar16 *Buffer,uIntn BuferLength,uIntn &DataLength);
bool DBQueryGetColumn(iDBQuery *Query,ufInt16 Column,uInt8 *Buffer,uIntn BuferSize,uIntn &DataSize);
bool DBQueryGetColumn(iDBQuery *Query,ufInt16 Column,cString<uChar8> &Buffer);
bool DBQueryGetColumn(iDBQuery *Query,ufInt16 Column,cString<uChar16> &Buffer);
bool DBQueryGetColumn(iDBQuery *Query,ufInt16 Column,cMemoryBuffer &Buffer);
bool DBQueryGetColumn(iDBQuery *Query,ufInt16 Column,cDateTime &Value);
//---------------------------------------------------------------------------
#if 0


//---------------------------------------------------------------------------
template<class TMemoryOperator>
class cMemoryStorageDBOutput : public iDBOutputBuffer, public cnStream::cMemoryStorageBinaryWriteBuffer<TMemoryOperator>
{
public:
	cMemoryStorageDBOutput(cnDataStruct::cMemoryStorage<TMemoryOperator> &Storage,uIntn Increment)
		:cnStream::cMemoryStorageBinaryWriteBuffer<TMemoryOperator>(Storage,Increment){}
	
	virtual bool cnLib_FUNC QueryWriteBuffer(cMemory &Buffer,uIntn QuerySize)override{
		return cnStream::cMemoryStorageBinaryWriteBuffer<TMemoryOperator>::QueryWriteBuffer(Buffer,QuerySize);
	}
	virtual void cnLib_FUNC CommitWriteBuffer(uIntn Size)override{
		return cnStream::cMemoryStorageBinaryWriteBuffer<TMemoryOperator>::CommitWriteBuffer(Size);
	}
	
	virtual void cnLib_FUNC ResetBuffer(void)override{
		this->fStorage.Length=0;
	}

	virtual const void* cnLib_FUNC FinishBuffer(void)override{
		return this->fStorage.Pointer;
	}
};
//---------------------------------------------------------------------------
template<class TMemoryOperator>
cMemoryStorageDBOutput<TMemoryOperator> StorageDBOutput(cnDataStruct::cMemoryStorage<TMemoryOperator> &Storage)
{
	return Storage;
}
//---------------------------------------------------------------------------
template<class TMemoryOperator>
class cStringStorageDBOutput : public iDBOutputBuffer
{
public:
	typedef cnDataStruct::cStringStorage<TMemoryOperator> TStringStorage;

	static constexpr uIntn ElementSize=sizeof(TMemoryOperator::TElement);

	cStringStorageDBOutput(TStringStorage &Storage):fStorage(Storage){}

	virtual bool cnLib_FUNC QueryWriteBuffer(cMemory &Buffer,uIntn QuerySize)override{
		static constexpr uIntn Increment=128;
		if(QuerySize<Increment)
			QuerySize=Increment;
		if(fStorage.Capacity<fStorage.Length+QuerySize){
			uIntn CurLength=fStorage.Length;
			fStorage.Length=fStorage.Capacity;
			fStorage.GrowTo(CurLength+QuerySize);
			fStorage.Length=CurLength;
		}

		Buffer.Pointer=fStorage.Pointer+fStorage.Length;
		Buffer.Length=(fStorage.Capacity-fStorage.Length)*ElementSize;
		return true;
	}
	virtual void cnLib_FUNC CommitWriteBuffer(uIntn Size)override{
		fStorage.Length+=Size/ElementSize;
	}
	
	virtual void cnLib_FUNC ResetBuffer(void)override{
		fStorage.Length=0;
	}

	virtual const void* cnLib_FUNC FinishBuffer(void)override{
		return fStorage.Pointer;
	}
private:
	TStringStorage &fStorage;
};
//---------------------------------------------------------------------------
template<class TMemoryOperator>
cStringStorageDBOutput<TMemoryOperator> StorageDBOutput(cnDataStruct::cStringStorage<TMemoryOperator> &Storage)
{
	return Storage;
}
//---------------------------------------------------------------------------
class cDBQuery
{
public:
	cDBQuery();
	~cDBQuery();

	void SetQuery(iDBQuery *Query);

	bool Execute(void);
	bool Fetch(void);
	void Clear(void);

	bool BindParameter(uInt16 Index,tNullptr);
	bool BindParameter(uInt16 Index,const cDBVariant *Value);
	bool BindParameter(uInt16 Index,const bool *Value);
	bool BindParameter(uInt16 Index,const sInt8 *Value);
	bool BindParameter(uInt16 Index,const uInt8 *Value);
	bool BindParameter(uInt16 Index,const sInt16 *Value);
	bool BindParameter(uInt16 Index,const uInt16 *Value);
	bool BindParameter(uInt16 Index,const sInt32 *Value);
	bool BindParameter(uInt16 Index,const uInt32 *Value);
	bool BindParameter(uInt16 Index,const sInt64 *Value);
	bool BindParameter(uInt16 Index,const uInt64 *Value);
	bool BindParameter(uInt16 Index,const Float32 *Value);
	bool BindParameter(uInt16 Index,const Float64 *Value);

	bool BindParameter(uInt16 Index,const uChar8 *Value,const uIntn *ValueLength);
	bool BindParameter(uInt16 Index,const uChar16 *Value,const uIntn *ValueLength);
	bool BindParameter(uInt16 Index,const uInt8 *Value,const uIntn *ValueSize);
	bool BindParameter(uInt16 Index,const cString<uChar8> *Value);
	bool BindParameter(uInt16 Index,const cString<uChar16> *Value);
	bool BindParameter(uInt16 Index,const cMemoryBuffer *Value);

	bool SetParameterNull(uInt16 Index,bool IsNull);
	bool SetParameter(uInt16 Index,const cDBVariant &Value);
	bool SetParameter(uInt16 Index,const int &Value);
	bool SetParameter(uInt16 Index,const unsigned int &Value);
	bool SetParameter(uInt16 Index,const Float32 &Value);
	bool SetParameter(uInt16 Index,const Float64 &Value);
	bool SetParameter(uInt16 Index,const cString<uChar8> &Value);
	bool SetParameter(uInt16 Index,const cString<uChar16> &Value);
	bool SetParameter(uInt16 Index,const cMemoryBuffer &Value);

	//int ColumnCount(void);
	//const wchar_t* ColumnName(int col);

	void SetupColumnFetchType(uInt16 Column,eDBDataType Type);


	// Bind
	//	bind buffer for column data

	bool BindColumn(uInt16 Column,cDBVariant *Value);
	
	// GetColumn
	//	copy column data

	bool GetColumn(uInt16 Column,cDBVariant &Value);
	bool GetColumn(uInt16 Column,bool &Value);
	bool GetColumn(uInt16 Column,sInt8 &Value);
	bool GetColumn(uInt16 Column,uInt8 &Value);
	bool GetColumn(uInt16 Column,sInt16 &Value);
	bool GetColumn(uInt16 Column,uInt16 &Value);
	bool GetColumn(uInt16 Column,sInt32 &Value);
	bool GetColumn(uInt16 Column,uInt32 &Value);
	bool GetColumn(uInt16 Column,sInt64 &Value);
	bool GetColumn(uInt16 Column,uInt64 &Value);
	bool GetColumn(uInt16 Column,Float32 &Value);
	bool GetColumn(uInt16 Column,Float64 &Value);
	bool GetColumn(uInt16 Column,uChar8 *Buffer,uIntn BuferLength,uIntn &DataLength);
	bool GetColumn(uInt16 Column,uChar16 *Buffer,uIntn BuferLength,uIntn &DataLength);
	bool GetColumn(uInt16 Column,uInt8 *Buffer,uIntn BuferSize,uIntn &DataSize);
	bool GetColumn(uInt16 Column,cString<uChar8> &Buffer);
	bool GetColumn(uInt16 Column,cString<uChar16> &Buffer);
	bool GetColumn(uInt16 Column,cMemoryBuffer &Buffer);
	bool GetColumn(uInt16 Column,cDateTime &Value);

	template<class T>
	T GetColumn(uInt16 Column){
		T Value={};
		GetColumn(Column,Value);
		return Value;
	}

	bool IsColumnNull(uInt16 Column);
	uIntn GetColumnLength(uInt16 Column,eDBDataType Type);

private:
	iPtr<iDBQuery> fCommand;

	void SetupCommand(void);
	void ClearCommand(void);
	
	
	bool BindColumnPtr_Value(uInt16 Column,eDBDataType Type,const void* *Ptr);
	bool BindColumnPtr_Array(uInt16 Column,eDBDataType Type,cConstMemory *Ptr);
	bool BindColumnPtr_BufferPtr(uInt16 Column,eDBDataType Type,const void **Ptr,uIntn *Length);
	

	bool GetColumn(uInt16 Column,eDBDataType Type,void *Buffer,uIntn BufferLength);
	bool GetColumn(uInt16 Column,eDBDataType Type,void *Buffer,uIntn BufferLength,uIntn &DataLength);
	template<class TCharacter> bool GetColumnString(uInt16 Column,eDBDataType Type,cString<TCharacter> &Value);
	

	bool BindParameterBuffer(uInt16 Column,eDBDataType Type,const void *Buffer,const uIntn *Length);

	class cFetchProcess
	{
	public:
		cDBQuery *Host;
		virtual void Destruct(void){}
		virtual iDBOutputBuffer* GetOutputBuffer(void){	return nullptr;	}
		virtual void Postprocess(uInt16){}
	};
	class cFetchProcess_GetPtr :  public cFetchProcess
	{
	public:
		cFetchProcess_GetPtr(const void* *Ptr):BufferPtr(Ptr){}
		const void* *BufferPtr;
		virtual void Destruct(void){		cFetchProcess_GetPtr::~cFetchProcess_GetPtr();	}
		virtual void Postprocess(uInt16 Column)override{
			auto Mem=Host->fCommand->ColumnGetBuffer(Column);
			*BufferPtr=Mem.Pointer;
		}
	};
	class cFetchProcess_GetPtrLength :  public cFetchProcess
	{
	public:
		cFetchProcess_GetPtrLength(const void* *Ptr,uIntn *LengthPtr):BufferPtr(Ptr),BufferLengthPtr(LengthPtr){}
		const void* *BufferPtr;
		uIntn *BufferLengthPtr;
		virtual void Destruct(void){		cFetchProcess_GetPtrLength::~cFetchProcess_GetPtrLength();	}
		virtual void Postprocess(uInt16 Column)override{
			auto Mem=Host->fCommand->ColumnGetBuffer(Column);
			*BufferPtr=Mem.Pointer;
			*BufferLengthPtr=Mem.Length;
		}
	};

	template<class TMemoryOperator>
	class cFetchProcess_OutputMemory :  public cFetchProcess
	{
	public:
		cMemoryStorageDBOutput<TMemoryOperator> Output;

		typedef cnDataStruct::cMemoryStorage<TMemoryOperator> TStringStorage;

		cFetchProcess_OutputMemory(cnDataStruct::cMemoryStorage<TMemoryOperator> &Storage):Output(Storage,0){}

		const void* *BufferPtr;
		uIntn *BufferLengthPtr;
		virtual void Destruct(void){		cFetchProcess_OutputMemory::~cFetchProcess_OutputMemory();	}
		virtual iDBOutputBuffer* GetOutputBuffer(void){	return &Output;	}
	};

	template<class TMemoryOperator>
	class cFetchProcess_OutputString :  public cFetchProcess
	{
	public:
		cStringStorageDBOutput<TMemoryOperator> Output;

		typedef cnDataStruct::cStringStorage<TMemoryOperator> TStringStorage;

		cFetchProcess_OutputString(cnDataStruct::cStringStorage<TMemoryOperator> &Storage):Output(Storage){}

		const void* *BufferPtr;
		uIntn *BufferLengthPtr;
		virtual void Destruct(void){		cFetchProcess_OutputString::~cFetchProcess_OutputString();	}
		virtual iDBOutputBuffer* GetOutputBuffer(void){	return &Output;	}
	};

	
	class cFetchProcess_GetVariant :  public cFetchProcess_OutputMemory< cMemoryOperator<void> >
	{
	public:
		cFetchProcess_GetVariant(cDBVariant *Variant)
			:cFetchProcess_OutputMemory< cMemoryOperator<void> >(fBuffer.MemoryStorage()),
				fVariant(*Variant)
		{}
		cMemoryBuffer fBuffer;
		cDBVariant &fVariant;
		virtual void Destruct(void){		cFetchProcess_GetVariant::~cFetchProcess_GetVariant();	}
		virtual void Postprocess(uInt16 Column)override{
			auto Type=Host->fCommand->ColumnGetType(Column);
			if(fBuffer->Length==0){
				fVariant.SetType<void>();
			}
			else switch(Type){
			default:
			case DBDataType::Null:
				fVariant.SetType<void>();
				break;
		
			case DBDataType::Binary:
				fVariant.SetType<cMemoryBuffer>();
				{
					auto &VarBuf=fVariant.Ref<cMemoryBuffer>();
					VarBuf=cnVar::MoveCast(fBuffer);
				}
				break;
			case DBDataType::Bool:
				fVariant=*static_cast<const bool*>(fBuffer->Pointer);
				break;

			case DBDataType::SInt8:
				fVariant=*static_cast<const sInt8*>(fBuffer->Pointer);
				break;
			case DBDataType::UInt8:
				fVariant=*static_cast<const uInt8*>(fBuffer->Pointer);
				break;
			case DBDataType::SInt16:
				fVariant=*static_cast<const sInt16*>(fBuffer->Pointer);
				break;
			case DBDataType::UInt16:
				fVariant=*static_cast<const uInt16*>(fBuffer->Pointer);
				break;
			case DBDataType::SInt32:
				fVariant=*static_cast<const sInt32*>(fBuffer->Pointer);
				break;
			case DBDataType::UInt32:
				fVariant=*static_cast<const uInt32*>(fBuffer->Pointer);
				break;
			case DBDataType::SInt64:
				fVariant=*static_cast<const sInt64*>(fBuffer->Pointer);
				break;
			case DBDataType::UInt64:
				fVariant=*static_cast<const uInt64*>(fBuffer->Pointer);
				break;

			case DBDataType::Float32:
				fVariant=*static_cast<const Float32*>(fBuffer->Pointer);
				break;
			case DBDataType::Float64:
				fVariant=*static_cast<const Float64*>(fBuffer->Pointer);
				break;

			case DBDataType::String8:
				fVariant.SetType<cString<uChar8> >();
				{
					auto &VarBuf=fVariant.Ref<cString<uChar8> >();
					auto Storage=fBuffer.Swap();
					
					cHeapArray<uChar8> Array;
					Array.Pointer=static_cast<uChar8*>(Storage.Pointer);
					Array.Length=Storage.Length;
					Storage.Pointer=nullptr;
					Storage.Length=0;
					cStringBuffer<uChar8> StrBuffer=cnVar::MoveCast(Array);
					VarBuf=cnVar::MoveCast(StrBuffer);
				}
				break;
			case DBDataType::String16:
				fVariant.SetType<cString<uChar16> >();
				{
					auto &VarBuf=fVariant.Ref<cString<uChar16> >();
					auto Storage=fBuffer.Swap();
					
					cHeapArray<uChar16> Array;
					Array.Pointer=static_cast<uChar16*>(Storage.Pointer);
					Array.Length=Storage.Length/2;
					Array.Capacity=Storage.Capacity/2;
					Storage.Pointer=nullptr;
					Storage.Length=0;
					Storage.Capacity=0;
					cStringBuffer<uChar16> StrBuffer=cnVar::MoveCast(Array);
					VarBuf=cnVar::MoveCast(StrBuffer);
				}
				break;
			//case cDBType::DateTime:
			//	break;
			case DBDataType::SystemTime:
				fVariant=*static_cast<const sInt64*>(fBuffer->Pointer);
				break;

			}
		}
	};
	struct cOutputBinding
	{
		uIntn RetLength;

		cnVar::cPolymorphicObject<cFetchProcess,
			cFetchProcess_GetPtr,
			cFetchProcess_GetPtrLength,
			cFetchProcess_OutputMemory< cMemoryOperator<void> >,
			cFetchProcess_OutputString< cMemoryOperator<uChar8> >,
			cFetchProcess_OutputString< cMemoryOperator<uChar16> >,
			cFetchProcess_GetVariant
		> FetchProcess;

		void SetNullFetchProcess(void){
			FetchProcess->Destruct();
			FetchProcess.ConstructAs<cFetchProcess>();
		}

		
		
		template<class TMemoryOperator>
		void SetMemoryOutputFetchProcess(cnDataStruct::cMemoryBuffer<TMemoryOperator> &MemoryBuffer){
			FetchProcess->Destruct();
			FetchProcess.ConstructAs< cFetchProcess_OutputMemory<TMemoryOperator> >(MemoryBuffer.MemoryStorage());
		}
		template<class TMemoryOperator,class TStringOperator>
		void SetStringOutputFetchProcess(cnString::cStringBuffer<TMemoryOperator,TStringOperator> &StringBuffer){
			FetchProcess->Destruct();
			FetchProcess.ConstructAs< cFetchProcess_OutputString<TMemoryOperator> >(StringBuffer.StringStorage());
		}

	};
	cSeqList<cOutputBinding> fColumnBinding;

};
#endif // 0
#endif
//---------------------------------------------------------------------------
}   // namespace cnRTL
//---------------------------------------------------------------------------
}   // namespace cnLibrary
#endif // 0
//---------------------------------------------------------------------------
#endif  /* __cplusplus */
/*-------------------------------------------------------------------------*/

