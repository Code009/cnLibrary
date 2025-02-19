
#include "SQLite3DBInterface.h"
#ifndef SQLITE3_H
#error	"need to include sqlite3.h"
#endif

using namespace cnLibrary;
using namespace cnRTL;

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cSQLite3DBQuery::cFetchPostprocess::cFetchPostprocess()
{
}
//---------------------------------------------------------------------------
void cSQLite3DBQuery::cFetchPostprocess::Destruct(void)
{
	this->~cFetchPostprocess();
}
//---------------------------------------------------------------------------
void cSQLite3DBQuery::cFetchPostprocess::Process(int)
{
}
//---------------------------------------------------------------------------
cSQLite3DBQuery::cFetchPostprocess_UserBuffer::cFetchPostprocess_UserBuffer(eDBDataType DataType,void *Buffer,uIntn BufferLength,uIntn *DataLength)
{
	static const uIntn LengthAlways1=1;
	fBuffer=Buffer;
	fBufferLength=BufferLength;
	fDataLength=DataLength;
	switch(DataType){
	default:
		fProcessProcedure=nullptr;
		break;
	//case DBDataType::Unknown:

	//case DBDataType::SystemTime:
	//	Buffer=MakeColumnPostProcess<cFetchPostprocess_UserBuffer>(ColumnBuffer);
	//	break;
	//case DBDataType::DateTime:
	//	Buffer=MakeColumnPostProcess<cFetchPostprocess_UserBuffer>(ColumnBuffer);
	//	break;

	case DBDataType::Bool:
		fProcessProcedure=&cFetchPostprocess_UserBuffer::ProcessBool;
		break;
	case DBDataType::SInt8:
		fProcessProcedure=&cFetchPostprocess_UserBuffer::ProcessSInt8;
		break;
	case DBDataType::UInt8:
		fProcessProcedure=&cFetchPostprocess_UserBuffer::ProcessUInt8;
		break;
	case DBDataType::SInt16:
		fProcessProcedure=&cFetchPostprocess_UserBuffer::ProcessSInt16;
		break;
	case DBDataType::UInt16:
		fProcessProcedure=&cFetchPostprocess_UserBuffer::ProcessUInt16;
		break;
	case DBDataType::SInt32:
		fProcessProcedure=&cFetchPostprocess_UserBuffer::ProcessSInt32;
		break;
	case DBDataType::UInt32:
		fProcessProcedure=&cFetchPostprocess_UserBuffer::ProcessUInt32;
		break;
	case DBDataType::SInt64:
		fProcessProcedure=&cFetchPostprocess_UserBuffer::ProcessSInt64;
		break;
	case DBDataType::UInt64:
		fProcessProcedure=&cFetchPostprocess_UserBuffer::ProcessUInt64;
		break;
	case DBDataType::Float32:
		fProcessProcedure=&cFetchPostprocess_UserBuffer::ProcessFloat32;
		break;
	case DBDataType::Float64:
		fProcessProcedure=&cFetchPostprocess_UserBuffer::ProcessFloat64;
		break;
	case DBDataType::String8:
		fProcessProcedure=&cFetchPostprocess_UserBuffer::ProcessString8;
		break;
	case DBDataType::String16:
		fProcessProcedure=&cFetchPostprocess_UserBuffer::ProcessString16;
		break;
	case DBDataType::Binary:
		fProcessProcedure=&cFetchPostprocess_UserBuffer::ProcessBinary;
		break;
	}
}
//---------------------------------------------------------------------------
void cSQLite3DBQuery::cFetchPostprocess_UserBuffer::Destruct(void)
{
	this->~cFetchPostprocess_UserBuffer();
}
//---------------------------------------------------------------------------
void cSQLite3DBQuery::cFetchPostprocess_UserBuffer::Process(int col)
{
	if(fProcessProcedure==nullptr){
		return;
	}
	(this->*fProcessProcedure)(col);
}
//---------------------------------------------------------------------------
void cSQLite3DBQuery::cFetchPostprocess_UserBuffer::ProcessBool(int col)
{
	auto v=sqlite3_column_int64(Owner->fStatement,col);
	*static_cast<bool*>(fBuffer)=v!=0;
	if(fDataLength!=nullptr)
		*fDataLength=1;
}
//---------------------------------------------------------------------------
void cSQLite3DBQuery::cFetchPostprocess_UserBuffer::ProcessSInt8(int col)
{
	auto v=sqlite3_column_int(Owner->fStatement,col);
	*static_cast<sInt8*>(fBuffer)=static_cast<sInt8>(v);
	if(fDataLength!=nullptr)
		*fDataLength=1;
}
//---------------------------------------------------------------------------
void cSQLite3DBQuery::cFetchPostprocess_UserBuffer::ProcessUInt8(int col)
{
	auto v=sqlite3_column_int(Owner->fStatement,col);
	*static_cast<uInt8*>(fBuffer)=static_cast<uInt8>(static_cast<uInt32>(v));
	if(fDataLength!=nullptr)
		*fDataLength=1;
}
//---------------------------------------------------------------------------
void cSQLite3DBQuery::cFetchPostprocess_UserBuffer::ProcessSInt16(int col)
{
	auto v=sqlite3_column_int(Owner->fStatement,col);
	*static_cast<sInt16*>(fBuffer)=static_cast<sInt16>(v);
	if(fDataLength!=nullptr)
		*fDataLength=1;
}
//---------------------------------------------------------------------------
void cSQLite3DBQuery::cFetchPostprocess_UserBuffer::ProcessUInt16(int col)
{
	auto v=sqlite3_column_int(Owner->fStatement,col);
	*static_cast<uInt16*>(fBuffer)=static_cast<uInt16>(static_cast<uInt32>(v));
	if(fDataLength!=nullptr)
		*fDataLength=1;
}
//---------------------------------------------------------------------------
void cSQLite3DBQuery::cFetchPostprocess_UserBuffer::ProcessSInt32(int col)
{
	*static_cast<sInt32*>(fBuffer)=sqlite3_column_int(Owner->fStatement,col);
	if(fDataLength!=nullptr)
		*fDataLength=1;
}
//---------------------------------------------------------------------------
void cSQLite3DBQuery::cFetchPostprocess_UserBuffer::ProcessUInt32(int col)
{
	auto v=sqlite3_column_int(Owner->fStatement,col);
	*static_cast<uInt32*>(fBuffer)=static_cast<uInt32>(v);
	if(fDataLength!=nullptr)
		*fDataLength=1;
}
//---------------------------------------------------------------------------
void cSQLite3DBQuery::cFetchPostprocess_UserBuffer::ProcessSInt64(int col)
{
	*static_cast<sInt64*>(fBuffer)=sqlite3_column_int64(Owner->fStatement,col);
	if(fDataLength!=nullptr)
		*fDataLength=1;
}
//---------------------------------------------------------------------------
void cSQLite3DBQuery::cFetchPostprocess_UserBuffer::ProcessUInt64(int col)
{
	auto v=sqlite3_column_int64(Owner->fStatement,col);
	*static_cast<uInt64*>(fBuffer)=static_cast<uInt64>(v);
	if(fDataLength!=nullptr)
		*fDataLength=1;
}
//---------------------------------------------------------------------------
void cSQLite3DBQuery::cFetchPostprocess_UserBuffer::ProcessFloat32(int col)
{
	auto v=sqlite3_column_double(Owner->fStatement,col);
	*static_cast<Float32*>(fBuffer)=static_cast<Float32>(v);
	if(fDataLength!=nullptr)
		*fDataLength=1;
}
//---------------------------------------------------------------------------
void cSQLite3DBQuery::cFetchPostprocess_UserBuffer::ProcessFloat64(int col)
{
	*static_cast<Float64*>(fBuffer)=sqlite3_column_double(Owner->fStatement,col);
	if(fDataLength!=nullptr)
		*fDataLength=1;
}
//---------------------------------------------------------------------------
void cSQLite3DBQuery::cFetchPostprocess_UserBuffer::ProcessBinary(int col)
{
	const void *Ptr=sqlite3_column_blob(Owner->fStatement,col);
	uIntn Size=sqlite3_column_bytes(Owner->fStatement,col);

	uIntn CopySize=Size;
	if(CopySize>fBufferLength){
		CopySize=fBufferLength;
	}

	cnMemory::Copy(fBuffer,Ptr,CopySize);

	if(fDataLength!=nullptr)
		*fDataLength=Size;
}
//---------------------------------------------------------------------------
void cSQLite3DBQuery::cFetchPostprocess_UserBuffer::ProcessString8(int col)
{
	const void *Ptr=sqlite3_column_text(Owner->fStatement,col);
	uIntn Size=sqlite3_column_bytes(Owner->fStatement,col);

	uIntn CopySize=Size;
	if(CopySize>fBufferLength-1){
		CopySize=fBufferLength-1;
	}

	cnMemory::Copy(fBuffer,Ptr,CopySize);
	// null-termination
	static_cast<char*>(fBuffer)[CopySize]=0;

	if(fDataLength!=nullptr)
		*fDataLength=Size;
}
//---------------------------------------------------------------------------
void cSQLite3DBQuery::cFetchPostprocess_UserBuffer::ProcessString16(int col)
{
	const void *Ptr=sqlite3_column_text16(Owner->fStatement,col);
	uIntn Size=sqlite3_column_bytes16(Owner->fStatement,col);

	uIntn CopyLength=Size/2;
	if(CopyLength>fBufferLength-1){
		CopyLength=fBufferLength-1;
	}

	cnMemory::Copy(fBuffer,Ptr,CopyLength*2);
	// null-termination
	static_cast<uChar16*>(fBuffer)[CopyLength]=0;

	if(fDataLength!=nullptr)
		*fDataLength=CopyLength;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cConstMemory cSQLite3DBQuery::cFetchPostprocess_FixedSizeRead::GetReadMemory(void)
{
	cConstMemory Memory;
	Memory.Pointer=&fValue;
	Memory.Length=fValueSize;
	return Memory;
}
//---------------------------------------------------------------------------
cConstMemory cSQLite3DBQuery::cFetchPostprocess_FixedSizeRead::GatherReadBuffer(uIntn)
{
	if(fReadOffset>=fValueSize){
		return NullArray;
	}
	cConstMemory Buffer;
	Buffer.Pointer=&fValue.u8+fReadOffset;
	Buffer.Length=fValueSize-fReadOffset;
	return Buffer;
}
//---------------------------------------------------------------------------
uIntn cSQLite3DBQuery::cFetchPostprocess_FixedSizeRead::DismissReadBuffer(uIntn Size)
{
	uIntn SizeRemian=fValueSize-fReadOffset;
	if(SizeRemian<Size){
		fReadOffset=fValueSize;
		return SizeRemian;
	}
	fReadOffset+=static_cast<ufInt8>(Size);
	return Size;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cSQLite3DBQuery::cFetchPostprocess_BoolRead::cFetchPostprocess_BoolRead()
{
	fValueSize=1;
}
//---------------------------------------------------------------------------
void cSQLite3DBQuery::cFetchPostprocess_BoolRead::Destruct(void)
{
	this->~cFetchPostprocess_BoolRead();
}
//---------------------------------------------------------------------------
void cSQLite3DBQuery::cFetchPostprocess_BoolRead::Process(int col)
{
	auto v=sqlite3_column_int64(Owner->fStatement,col);
	fValue.b=v!=0;
	fReadOffset=0;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cSQLite3DBQuery::cFetchPostprocess_SIntRead::cFetchPostprocess_SIntRead(ufInt8 Size)
{
	fValueSize=Size;
}
//---------------------------------------------------------------------------
void cSQLite3DBQuery::cFetchPostprocess_SIntRead::Destruct(void)
{
	this->~cFetchPostprocess_SIntRead();
}
//---------------------------------------------------------------------------
void cSQLite3DBQuery::cFetchPostprocess_SIntRead::Process(int col)
{
	switch(fValueSize){
	case 1:
		fValue.s8=static_cast<sInt8>(sqlite3_column_int(Owner->fStatement,col));
		break;
	case 2:
		fValue.s16=static_cast<sInt16>(sqlite3_column_int(Owner->fStatement,col));
		break;
	case 4:
		fValue.s32=sqlite3_column_int(Owner->fStatement,col);
		break;
	case 8:
		fValue.s64=sqlite3_column_int64(Owner->fStatement,col);
		break;
	}
	fReadOffset=0;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cSQLite3DBQuery::cFetchPostprocess_UIntRead::cFetchPostprocess_UIntRead(ufInt8 Size)
{
	fValueSize=Size;
}
//---------------------------------------------------------------------------
void cSQLite3DBQuery::cFetchPostprocess_UIntRead::Destruct(void)
{
	this->~cFetchPostprocess_UIntRead();
}
//---------------------------------------------------------------------------
void cSQLite3DBQuery::cFetchPostprocess_UIntRead::Process(int col)
{
	switch(fValueSize){
	case 1:
		fValue.u8=static_cast<uInt8>(static_cast<sInt8>(sqlite3_column_int(Owner->fStatement,col)));
		break;
	case 2:
		fValue.u16=static_cast<uInt16>(static_cast<sInt16>(sqlite3_column_int(Owner->fStatement,col)));
		break;
	case 4:
		fValue.u32=static_cast<uInt32>(sqlite3_column_int(Owner->fStatement,col));
		break;
	case 8:
		fValue.u64=static_cast<uInt64>(sqlite3_column_int64(Owner->fStatement,col));
		break;
	}
	fReadOffset=0;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cSQLite3DBQuery::cFetchPostprocess_FloatRead::cFetchPostprocess_FloatRead(bool DP)
{
	fDP=DP;
	if(fDP){
		fValueSize=8;
	}
	else{
		fValueSize=4;
	}
}
//---------------------------------------------------------------------------
void cSQLite3DBQuery::cFetchPostprocess_FloatRead::Destruct(void)
{
	this->~cFetchPostprocess_FloatRead();
}
//---------------------------------------------------------------------------
void cSQLite3DBQuery::cFetchPostprocess_FloatRead::Process(int col)
{
	if(fDP){
		fValue.f32=static_cast<Float32>(sqlite3_column_double(Owner->fStatement,col));
	}
	else{
		fValue.f64=sqlite3_column_double(Owner->fStatement,col);
	}
	fReadOffset=0;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cSQLite3DBQuery::cFetchPostprocess_UTFTextRead::cFetchPostprocess_UTFTextRead(ufInt8 CharSize)
{
	fUTF16=CharSize==2;
}
//---------------------------------------------------------------------------
void cSQLite3DBQuery::cFetchPostprocess_UTFTextRead::Destruct(void)
{
	this->~cFetchPostprocess_UTFTextRead();
}
//---------------------------------------------------------------------------
void cSQLite3DBQuery::cFetchPostprocess_UTFTextRead::Process(int col)
{
	fCol=col;
	fReadOffset=0;
}
//---------------------------------------------------------------------------
cConstMemory cSQLite3DBQuery::cFetchPostprocess_UTFTextRead::GetReadMemory(void)
{
	cConstMemory Memory;
	if(fUTF16){
		Memory.Pointer=sqlite3_column_text16(Owner->fStatement,fCol);
		Memory.Length=sqlite3_column_bytes16(Owner->fStatement,fCol);
	}
	else{
		Memory.Pointer=sqlite3_column_text(Owner->fStatement,fCol);
		Memory.Length=sqlite3_column_bytes(Owner->fStatement,fCol);
	}
	return Memory;
}
//---------------------------------------------------------------------------
cConstMemory cSQLite3DBQuery::cFetchPostprocess_UTFTextRead::GatherReadBuffer(uIntn QuerySize)
{
	const void *Ptr;
	uIntn Size;
	if(fUTF16){
		Ptr=sqlite3_column_text16(Owner->fStatement,fCol);
		Size=sqlite3_column_bytes16(Owner->fStatement,fCol);
	}
	else{
		Ptr=sqlite3_column_text(Owner->fStatement,fCol);
		Size=sqlite3_column_bytes(Owner->fStatement,fCol);
	}
	if(fReadOffset>=Size){
		return NullArray;
	}

	cConstMemory Buffer;
	Buffer.Pointer=cnMemory::PointerAddByteOffset(Ptr,fReadOffset);
	Buffer.Length=Size-fReadOffset;
	return Buffer;
}
//---------------------------------------------------------------------------
uIntn cSQLite3DBQuery::cFetchPostprocess_UTFTextRead::DismissReadBuffer(uIntn Size)
{
	fReadOffset+=Size;
	return Size;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void cSQLite3DBQuery::cFetchPostprocess_BLOBRead::Destruct(void)
{
	this->~cFetchPostprocess_BLOBRead();
}
//---------------------------------------------------------------------------
void cSQLite3DBQuery::cFetchPostprocess_BLOBRead::Process(int col)
{
	fCol=col;
	fReadOffset=0;
}
//---------------------------------------------------------------------------
cConstMemory cSQLite3DBQuery::cFetchPostprocess_BLOBRead::GetReadMemory(void)
{
	cConstMemory Memory;
	Memory.Pointer=sqlite3_column_blob(Owner->fStatement,fCol);
	Memory.Length=sqlite3_column_bytes(Owner->fStatement,fCol);
	return Memory;
}
//---------------------------------------------------------------------------
cConstMemory cSQLite3DBQuery::cFetchPostprocess_BLOBRead::GatherReadBuffer(uIntn QuerySize)
{
	auto Ptr=sqlite3_column_blob(Owner->fStatement,fCol);
	uIntn Size=sqlite3_column_bytes(Owner->fStatement,fCol);
	if(fReadOffset>=Size){
		return NullArray;
	}

	cConstMemory Buffer;
	Buffer.Pointer=cnMemory::PointerAddByteOffset(Ptr,fReadOffset);
	Buffer.Length=Size-fReadOffset;
	return Buffer;
}
//---------------------------------------------------------------------------
uIntn cSQLite3DBQuery::cFetchPostprocess_BLOBRead::DismissReadBuffer(uIntn Size)
{
	fReadOffset+=Size;
	return Size;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cSQLite3DBQuery::cExecutePreprocess::cExecutePreprocess()
{
}
//---------------------------------------------------------------------------
void cSQLite3DBQuery::cExecutePreprocess::Destruct(void)
{
	this->~cExecutePreprocess();
}
//---------------------------------------------------------------------------
void cSQLite3DBQuery::cExecutePreprocess::Process(int)
{
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
const uIntn cSQLite3DBQuery::cExecutePreprocess_UserBuffer::DataLength1=1;
//---------------------------------------------------------------------------
cSQLite3DBQuery::cExecutePreprocess_UserBuffer::cExecutePreprocess_UserBuffer(eDBDataType DataType,const void *Buffer,const uIntn *DataLength)
{
	static const uIntn LengthAlways1=1;
	fBuffer=Buffer;
	fDataLength=DataLength;
	if(fDataLength==nullptr){
		fDataLength=&DataLength1;
	}
	switch(DataType){
	default:
		fProcessProcedure=nullptr;
		break;
	//case DBDataType::Unknown:

	//case DBDataType::SystemTime:
	//	Buffer=MakeColumnPostProcess<cFetchPostprocess_SystemTime>(ColumnBuffer);
	//	break;
	//case DBDataType::DateTime:
	//	Buffer=MakeColumnPostProcess<cFetchPostprocess_DateTime>(ColumnBuffer);
	//	break;

	case DBDataType::Bool:
		fProcessProcedure=&cExecutePreprocess_UserBuffer::ProcessBool;
		break;
	case DBDataType::SInt8:
		fProcessProcedure=&cExecutePreprocess_UserBuffer::ProcessSInt8;
		break;
	case DBDataType::UInt8:
		fProcessProcedure=&cExecutePreprocess_UserBuffer::ProcessUInt8;
		break;
	case DBDataType::SInt16:
		fProcessProcedure=&cExecutePreprocess_UserBuffer::ProcessSInt16;
		break;
	case DBDataType::UInt16:
		fProcessProcedure=&cExecutePreprocess_UserBuffer::ProcessUInt16;
		break;
	case DBDataType::SInt32:
		fProcessProcedure=&cExecutePreprocess_UserBuffer::ProcessSInt32;
		break;
	case DBDataType::UInt32:
		fProcessProcedure=&cExecutePreprocess_UserBuffer::ProcessUInt32;
		break;
	case DBDataType::SInt64:
		fProcessProcedure=&cExecutePreprocess_UserBuffer::ProcessSInt64;
		break;
	case DBDataType::UInt64:
		fProcessProcedure=&cExecutePreprocess_UserBuffer::ProcessUInt64;
		break;
	case DBDataType::Float32:
		fProcessProcedure=&cExecutePreprocess_UserBuffer::ProcessFloat32;
		break;
	case DBDataType::Float64:
		fProcessProcedure=&cExecutePreprocess_UserBuffer::ProcessFloat64;
		break;
	case DBDataType::String8:
		fProcessProcedure=&cExecutePreprocess_UserBuffer::ProcessString8;
		break;
	case DBDataType::String16:
		fProcessProcedure=&cExecutePreprocess_UserBuffer::ProcessString16;
		break;
	case DBDataType::Binary:
		fProcessProcedure=&cExecutePreprocess_UserBuffer::ProcessBinary;
		break;
	}
}
//---------------------------------------------------------------------------
void cSQLite3DBQuery::cExecutePreprocess_UserBuffer::Destruct(void)
{
	this->~cExecutePreprocess_UserBuffer();
}
//---------------------------------------------------------------------------
void cSQLite3DBQuery::cExecutePreprocess_UserBuffer::Process(int index)
{
	if(fProcessProcedure==nullptr || *fDataLength==0){
		sqlite3_bind_null(Owner->fStatement,index);
	}
	else{
		(this->*fProcessProcedure)(index);
	}
}
//---------------------------------------------------------------------------
void cSQLite3DBQuery::cExecutePreprocess_UserBuffer::ProcessBool(int index)
{
	int Value=*static_cast<const bool*>(fBuffer);
	sqlite3_bind_int(Owner->fStatement,index,Value);
}
//---------------------------------------------------------------------------
void cSQLite3DBQuery::cExecutePreprocess_UserBuffer::ProcessSInt8(int index)
{
	int Value=*static_cast<const sInt8*>(fBuffer);
	sqlite3_bind_int(Owner->fStatement,index,Value);
}
//---------------------------------------------------------------------------
void cSQLite3DBQuery::cExecutePreprocess_UserBuffer::ProcessUInt8(int index)
{
	int Value=*static_cast<const uInt8*>(fBuffer);
	sqlite3_bind_int(Owner->fStatement,index,Value);
}
//---------------------------------------------------------------------------
void cSQLite3DBQuery::cExecutePreprocess_UserBuffer::ProcessSInt16(int index)
{
	int Value=*static_cast<const sInt16*>(fBuffer);
	sqlite3_bind_int(Owner->fStatement,index,Value);
}
//---------------------------------------------------------------------------
void cSQLite3DBQuery::cExecutePreprocess_UserBuffer::ProcessUInt16(int index)
{
	int Value=*static_cast<const uInt16*>(fBuffer);
	sqlite3_bind_int(Owner->fStatement,index,Value);
}
//---------------------------------------------------------------------------
void cSQLite3DBQuery::cExecutePreprocess_UserBuffer::ProcessSInt32(int index)
{
	int Value=*static_cast<const sInt32*>(fBuffer);
	sqlite3_bind_int(Owner->fStatement,index,Value);
}
//---------------------------------------------------------------------------
void cSQLite3DBQuery::cExecutePreprocess_UserBuffer::ProcessUInt32(int index)
{
	int Value=*static_cast<const uInt32*>(fBuffer);
	sqlite3_bind_int(Owner->fStatement,index,Value);
}
//---------------------------------------------------------------------------
void cSQLite3DBQuery::cExecutePreprocess_UserBuffer::ProcessSInt64(int index)
{
	sqlite3_int64 Value=*static_cast<const sInt64*>(fBuffer);
	sqlite3_bind_int64(Owner->fStatement,index,Value);
}
//---------------------------------------------------------------------------
void cSQLite3DBQuery::cExecutePreprocess_UserBuffer::ProcessUInt64(int index)
{
	sqlite3_int64 Value=*static_cast<const uInt64*>(fBuffer);
	sqlite3_bind_int64(Owner->fStatement,index,Value);
}
//---------------------------------------------------------------------------
void cSQLite3DBQuery::cExecutePreprocess_UserBuffer::ProcessFloat32(int index)
{
	float Value=*static_cast<const Float32*>(fBuffer);
	sqlite3_bind_double(Owner->fStatement,index,Value);
}
//---------------------------------------------------------------------------
void cSQLite3DBQuery::cExecutePreprocess_UserBuffer::ProcessFloat64(int index)
{
	double Value=*static_cast<const Float64*>(fBuffer);
	sqlite3_bind_double(Owner->fStatement,index,Value);
}
//---------------------------------------------------------------------------
void cSQLite3DBQuery::cExecutePreprocess_UserBuffer::ProcessBinary(int index)
{
	sqlite3_bind_blob(Owner->fStatement,index,fBuffer,*fDataLength,SQLITE_STATIC);
}
//---------------------------------------------------------------------------
void cSQLite3DBQuery::cExecutePreprocess_UserBuffer::ProcessString8(int index)
{
	sqlite3_bind_text(Owner->fStatement,index,static_cast<const char*>(fBuffer),*fDataLength,SQLITE_STATIC);
}
//---------------------------------------------------------------------------
void cSQLite3DBQuery::cExecutePreprocess_UserBuffer::ProcessString16(int index)
{
	sqlite3_bind_text16(Owner->fStatement,index,fBuffer,*fDataLength*sizeof(uChar16),SQLITE_STATIC);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cSQLite3DBQuery::cSQLite3DBQuery(rClsRef<cSQLite3DBContext> Context,sqlite3_stmt *Statement)
	: fContext(cnVar::MoveCast(Context))
{
	fStatement=Statement;

	auto ColumnCount=sqlite3_column_count(fStatement);
	fColumnBufferList.SetCount(ColumnCount);
	for(auto &ColBuffer : fColumnBufferList){
		ColBuffer.ColumnNameLength=0;
		// empty post process
		ColBuffer.Postprocess.ConstructAs<cFetchPostprocess>();
	}

	auto ParamCount=sqlite3_bind_parameter_count(fStatement);
	fParameterBufferList.SetCount(ParamCount);
	for(auto &ParamBuffer : fParameterBufferList){
		ParamBuffer.ParamNameValid=false;
		ParamBuffer.Preprocess.ConstructAs<cExecutePreprocess>();
		ParamBuffer.Preprocess->Owner=this;
		ParamBuffer.Null=false;
	}
}
//---------------------------------------------------------------------------
cSQLite3DBQuery::~cSQLite3DBQuery()
{
}
//---------------------------------------------------------------------------
bool cSQLite3DBQuery::Execute(void)
{
	if(fQueryState!=qsIdle){
		return false;
	}
	ExecutePreprocess();
	int sqlret=sqlite3_step(fStatement);

	switch(sqlret){
	default:
		return false;
	case SQLITE_ROW:
		fQueryState=qsExeRow;
		break;
	case SQLITE_DONE:
		fQueryState=qsDone;
		break;
	}
	return true;
}
//---------------------------------------------------------------------------
bool cSQLite3DBQuery::Fetch(void)
{
	switch(fQueryState){
	case qsIdle:
		return false;
	case qsExeRow:
		fQueryState=qsRow;
		FetchPostprocess();
		return true;
	case qsRow:
		{
			int sqlret=sqlite3_step(fStatement);
			switch(sqlret){
			default:
			case SQLITE_DONE:
				fQueryState=qsDone;
				break;
			case SQLITE_ROW:
				FetchPostprocess();
				return true;
			}
		}
		break;
	}
	return false;
}
//---------------------------------------------------------------------------
void cSQLite3DBQuery::Clear(void)
{
	if(fQueryState!=qsIdle){
		sqlite3_reset(fStatement);
		fQueryState=qsIdle;
	}
}
//---------------------------------------------------------------------------
bool cSQLite3DBQuery::GetRowsAffacted(uIntn &RowCount)
{
	RowCount=fContext->LastRowsChanged();
	return true;
}
//---------------------------------------------------------------------------
ufInt16 cSQLite3DBQuery::ColumnCount(void)
{
	return static_cast<ufInt16>(fColumnBufferList.GetCount());
}
//---------------------------------------------------------------------------
cArray<const uChar16> cSQLite3DBQuery::ColumnName(ufInt16 Index)
{
	cArray<const uChar16> NameArray;
	if(Index>=fColumnBufferList.GetCount()){
		NameArray.Pointer=reinterpret_cast<uChar16*>(&NameArray.Length);
		NameArray.Length=0;
		return NameArray;
	}
	NameArray.Pointer=static_cast<const uChar16*>(sqlite3_column_name16(fStatement,Index));
	if(NameArray.Pointer==nullptr){
		NameArray.Pointer=reinterpret_cast<uChar16*>(&NameArray.Length);
		NameArray.Length=0;
		return NameArray;
	}
	auto &ColumnBuffer=fColumnBufferList[Index];
	if(ColumnBuffer.ColumnNameLength==0){
		ColumnBuffer.ColumnNameLength=cnString::GetLength(NameArray.Pointer);
	}
	NameArray.Length=ColumnBuffer.ColumnNameLength;
	return NameArray;
}
//---------------------------------------------------------------------------
eDBDataType	cSQLite3DBQuery::ColumnType(ufInt16 Index)
{
	return DBDataType::Unknown;
	//sqlite3_column_decltype(fStatement,Index);
}
//---------------------------------------------------------------------------
eDBDataType cSQLite3DBQuery::ColumnGetType(ufInt16 Index)
{
	auto SQLiteType=sqlite3_column_type(fStatement,Index);
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
bool cSQLite3DBQuery::ColumnIsNullable(ufInt16)
{
	return true;
}
//---------------------------------------------------------------------------
uIntn cSQLite3DBQuery::ColumnMaxLength(ufInt16)
{
	return IndexNotFound;
}
//---------------------------------------------------------------------------
void cSQLite3DBQuery::FetchPostprocess(void)
{
	auto ColCount=fColumnBufferList.GetCount();
	for(uIntn ColIndex=0;ColIndex<ColCount;ColIndex++){
		auto &ColBuffer=fColumnBufferList[ColIndex];
		ColBuffer.Postprocess->Process(ColIndex);
	}
}
//---------------------------------------------------------------------------
bool cSQLite3DBQuery::ColumnBindBuffer(ufInt16 Index,eDBDataType Type,void *Buffer,uIntn BufferLength,uIntn *DataLength)
{
	if(Index>=fColumnBufferList.GetCount()){
		return false;
	}
	auto &ColBuffer=fColumnBufferList[Index];

	ColBuffer.Postprocess->Destruct();
	if(Buffer!=nullptr){
		ColBuffer.Postprocess.ConstructAs<cFetchPostprocess_UserBuffer>(Type,Buffer,BufferLength,DataLength);
	}
	else{
		ColBuffer.Postprocess.ConstructAs<cExecutePreprocess>();
	}
	ColBuffer.Postprocess->Owner=this;
	return true;
}
//---------------------------------------------------------------------------
template<class T,class...TArgs>
T* cSQLite3DBQuery::MakeColumnPostprocess(cColumnBuffer &Buffer,TArgs&&...Args)
{
	Buffer.Postprocess->Destruct();
	Buffer.Postprocess.ConstructAs<T>(cnVar::Forward<TArgs>(Args)...);
	Buffer.Postprocess->Owner=this;
	return static_cast<T*>(&Buffer.Postprocess);
}
//---------------------------------------------------------------------------
template<class T>
T* cSQLite3DBQuery::QueryColumnPostprocess_Buffer(ufInt16 Index,eDBDataType Type)
{
	if(Index>=fColumnBufferList.GetCount()){
		return nullptr;
	}

	auto &ColumnBuffer=fColumnBufferList[Index];

	T *Buffer;

	switch(Type){
	default:
		return nullptr;
	case DBDataType::Null:
		ColumnBuffer.Postprocess->Destruct();
		ColumnBuffer.Postprocess.ConstructAs<cFetchPostprocess>();
		return nullptr;
	//case DBDataType::Unknown:

	//case DBDataType::SystemTime:
	//	Buffer=MakeColumnPostprocess<cFetchPostprocess_SystemTime>(ColumnBuffer);
	//	break;
	//case DBDataType::DateTime:
	//	Buffer=MakeColumnPostprocess<cFetchPostprocess_DateTime>(ColumnBuffer);
	//	break;

	case DBDataType::Bool:
		Buffer=MakeColumnPostprocess<cFetchPostprocess_BoolRead>(ColumnBuffer);
		break;
	case DBDataType::SInt8:
		Buffer=MakeColumnPostprocess<cFetchPostprocess_SIntRead>(ColumnBuffer,static_cast<ufInt8>(1));
		break;
	case DBDataType::UInt8:
		Buffer=MakeColumnPostprocess<cFetchPostprocess_UIntRead>(ColumnBuffer,static_cast<ufInt8>(1));
		break;
	case DBDataType::SInt16:
		Buffer=MakeColumnPostprocess<cFetchPostprocess_SIntRead>(ColumnBuffer,static_cast<ufInt8>(2));
		break;
	case DBDataType::UInt16:
		Buffer=MakeColumnPostprocess<cFetchPostprocess_UIntRead>(ColumnBuffer,static_cast<ufInt8>(2));
		break;
	case DBDataType::SInt32:
		Buffer=MakeColumnPostprocess<cFetchPostprocess_SIntRead>(ColumnBuffer,static_cast<ufInt8>(4));
		break;
	case DBDataType::UInt32:
		Buffer=MakeColumnPostprocess<cFetchPostprocess_UIntRead>(ColumnBuffer,static_cast<ufInt8>(4));
		break;
	case DBDataType::SInt64:
		Buffer=MakeColumnPostprocess<cFetchPostprocess_SIntRead>(ColumnBuffer,static_cast<ufInt8>(8));
		break;
	case DBDataType::UInt64:
		Buffer=MakeColumnPostprocess<cFetchPostprocess_UIntRead>(ColumnBuffer,static_cast<ufInt8>(8));
		break;
	case DBDataType::Float32:
		Buffer=MakeColumnPostprocess<cFetchPostprocess_FloatRead>(ColumnBuffer,false);
		break;
	case DBDataType::Float64:
		Buffer=MakeColumnPostprocess<cFetchPostprocess_FloatRead>(ColumnBuffer,false);
		break;
	case DBDataType::String8:
		Buffer=MakeColumnPostprocess<cFetchPostprocess_UTFTextRead>(ColumnBuffer,static_cast<ufInt8>(1));
		break;
	case DBDataType::String16:
		Buffer=MakeColumnPostprocess<cFetchPostprocess_UTFTextRead>(ColumnBuffer,static_cast<ufInt8>(2));
		break;
	case DBDataType::Binary:
		Buffer=MakeColumnPostprocess<cFetchPostprocess_BLOBRead>(ColumnBuffer);
		break;
	}
	return Buffer;
}
//---------------------------------------------------------------------------
iDBReadMemory* cSQLite3DBQuery::ColumnQueryReadMemory(ufInt16 Index,eDBDataType Type)
{
	return QueryColumnPostprocess_Buffer<iDBReadMemory>(Index,Type);
}
//---------------------------------------------------------------------------
iReadBuffer<void>* cSQLite3DBQuery::ColumnQueryReadBuffer(ufInt16 Index,eDBDataType Type)
{
	return QueryColumnPostprocess_Buffer<iReadBuffer<void> >(Index,Type);
}
//---------------------------------------------------------------------------
void cSQLite3DBQuery::ExecutePreprocess(void)
{
	int ParamIndex=1;
	for(auto &ParamBuffer : fParameterBufferList){
		if(ParamBuffer.Null){
			// null
			sqlite3_bind_null(fStatement,ParamIndex);
		}
		else{
			// parameter
			ParamBuffer.Preprocess->Process(ParamIndex);
		}
		ParamIndex++;
	}
}
//---------------------------------------------------------------------------
ufInt16	cSQLite3DBQuery::ParameterCount(void)
{
	return static_cast<ufInt16>(fParameterBufferList.GetCount());
}
//---------------------------------------------------------------------------
cArray<const uChar16> cSQLite3DBQuery::ParameterName(ufInt16 Index)
{
	if(Index>=fParameterBufferList.GetCount()){
		cArray<const uChar16> NullName;
		NullName.Pointer=reinterpret_cast<uChar16*>(&NullName.Length);
		NullName.Length=0;
		return NullName;
	}
	auto &ParamBuffer=fParameterBufferList[Index];
	if(ParamBuffer.ParamNameValid==false){
		auto Name=sqlite3_bind_parameter_name(fStatement,Index+1);
		auto Transcoder=cnSystem::UnicodeTranscoder(2,1);

		cStringBuffer<uChar16> Buffer;
		cnString::ConvertEncodingToStream(Buffer.WriteStream(),Transcoder,Name,cnString::GetLength(Name));

		ParamBuffer.ParamName=cnVar::MoveCast(Buffer);
	
		ParamBuffer.ParamNameValid=true;
	}

	return ParamBuffer.ParamName.Array();
}
//---------------------------------------------------------------------------
uIntn cSQLite3DBQuery::ParameterMaxLength(ufInt16)
{
	return IndexNotFound;
}
//---------------------------------------------------------------------------
bool cSQLite3DBQuery::ParameterSetNull(ufInt16 Index,bool Null)
{
	if(Index>=fParameterBufferList.GetCount()){
		return false;
	}
	auto &ParamBuffer=fParameterBufferList[Index];
	ParamBuffer.Null=Null;
	return true;
}
//---------------------------------------------------------------------------
bool cSQLite3DBQuery::ParameterCopyBuffer(ufInt16 Index,eDBDataType Type,const void *Buffer,uIntn DataLength)
{
	if(Index>=fParameterBufferList.GetCount()){
		return false;
	}
	auto &ParamBuffer=fParameterBufferList[Index];
	int pindex=Index+1;
	int sqlret;
	if(Buffer==nullptr || DataLength==0){
		sqlret=sqlite3_bind_null(fStatement,pindex);
	}
	else{
		switch(Type){
		default:
			return false;
		case DBDataType::Null:
			sqlret=sqlite3_bind_null(fStatement,pindex);
			break;
		//case DBDataType::Unknown:

		//case DBDataType::SystemTime:
		//	Buffer=MakeColumnPostProcess<cFetchPostprocess_SystemTime>(ColumnBuffer);
		//	break;
		//case DBDataType::DateTime:
		//	Buffer=MakeColumnPostProcess<cFetchPostprocess_DateTime>(ColumnBuffer);
		//	break;

		case DBDataType::Bool:
			sqlret=sqlite3_bind_int(fStatement,pindex,*static_cast<const bool*>(Buffer));
			break;
		case DBDataType::SInt8:
			sqlret=sqlite3_bind_int(fStatement,pindex,*static_cast<const sInt8*>(Buffer));
			break;
		case DBDataType::UInt8:
			sqlret=sqlite3_bind_int(fStatement,pindex,*static_cast<const uInt8*>(Buffer));
			break;
		case DBDataType::SInt16:
			sqlret=sqlite3_bind_int(fStatement,pindex,*static_cast<const sInt16*>(Buffer));
			break;
		case DBDataType::UInt16:
			sqlret=sqlite3_bind_int(fStatement,pindex,*static_cast<const uInt16*>(Buffer));
			break;
		case DBDataType::SInt32:
			sqlret=sqlite3_bind_int(fStatement,pindex,*static_cast<const sInt32*>(Buffer));
			break;
		case DBDataType::UInt32:
			sqlret=sqlite3_bind_int(fStatement,pindex,*static_cast<const uInt32*>(Buffer));
			break;
		case DBDataType::SInt64:
			sqlret=sqlite3_bind_int64(fStatement,pindex,*static_cast<const sInt64*>(Buffer));
			break;
		case DBDataType::UInt64:
			sqlret=sqlite3_bind_int64(fStatement,pindex,*static_cast<const uInt64*>(Buffer));
			break;
		case DBDataType::Float32:
			sqlret=sqlite3_bind_double(fStatement,pindex,*static_cast<const Float32*>(Buffer));
			break;
		case DBDataType::Float64:
			sqlret=sqlite3_bind_double(fStatement,pindex,*static_cast<const Float64*>(Buffer));
			break;
		case DBDataType::String8:
			sqlret=sqlite3_bind_text(fStatement,pindex,static_cast<const char*>(Buffer),DataLength,SQLITE_TRANSIENT);
			break;
		case DBDataType::String16:
			sqlret=sqlite3_bind_text16(fStatement,pindex,Buffer,DataLength*2,SQLITE_TRANSIENT);
			break;
		case DBDataType::Binary:
			sqlret=sqlite3_bind_blob(fStatement,pindex,Buffer,DataLength,SQLITE_TRANSIENT);
			break;
		}
	}
	return true;
}
//---------------------------------------------------------------------------
bool cSQLite3DBQuery::ParameterBindBuffer(ufInt16 Index,eDBDataType Type,const void *Buffer,const uIntn *DataLength)
{
	if(Index>=fParameterBufferList.GetCount()){
		return false;
	}
	auto &ParamBuffer=fParameterBufferList[Index];

	if(Buffer!=nullptr){
		ParamBuffer.Preprocess->Destruct();
		ParamBuffer.Preprocess.ConstructAs<cExecutePreprocess_UserBuffer>(Type,Buffer,DataLength);
		ParamBuffer.Preprocess->Owner=this;
	}
	else{
		ParamBuffer.Preprocess->Destruct();
		ParamBuffer.Preprocess.ConstructAs<cExecutePreprocess>();
		ParamBuffer.Preprocess->Owner=this;
	}
	return true;
}
//---------------------------------------------------------------------------
bool cSQLite3DBQuery::ParameterBindInput(ufInt16 Index,eDBDataType Type,iReadBuffer<void> *Buffer)
{
	return false;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cSQLite3DBConnection::cSQLite3DBConnection(rClsRef<cSQLite3DBContext> Context)
	: fContext(cnVar::MoveCast(Context))
{
}
//---------------------------------------------------------------------------
cSQLite3DBConnection::~cSQLite3DBConnection()
{
}
//---------------------------------------------------------------------------
iDBTranscation*	cSQLite3DBConnection::Transcation(void)
{
	return nullptr;
}
//---------------------------------------------------------------------------
iPtr<iDBQuery> cSQLite3DBConnection::CreateQuery(const uChar16 *SQL,uIntn Length)
{
	auto Statement=fContext->PrepareStatement(SQL,Length);
	if(Statement==nullptr){
		return nullptr;
	}

	return iCreate<cSQLite3DBQuery>(fContext,Statement);
}
//---------------------------------------------------------------------------
