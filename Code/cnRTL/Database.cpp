//---------------------------------------------------------------------------
#include "Database.h"

using namespace cnLibrary;
using namespace cnRTL;

extern const int SupressWarning_LNK4221_cnRTL_Database=0;
#if 0


#pragma message(cnLib_FILE_LINE ": TODO - DBBindColumn - cDBVariant")
#pragma message(cnLib_FILE_LINE ": TODO - cDBColumn<cDBVariant>")
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cDBTranscation::cDBTranscation(iDBConnection *Connection){
	fTranscation=Connection->Transcation();
}
//---------------------------------------------------------------------------
cDBTranscation::~cDBTranscation(){
	Rollback();
}
//---------------------------------------------------------------------------
bool cDBTranscation::Commit(void){
	if(fTranscation){
		if(fTranscation->Commit()){
			fTranscation=nullptr;
			return true;
		}
	}
	return false;
}
//---------------------------------------------------------------------------
void cDBTranscation::Rollback(void){
	if(fTranscation){
		fTranscation->Rollback();
		fTranscation=nullptr;
	}
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
bool cnRTL::DBColumnIsNull(iDBQuery *Query,ufInt16 Column)
{
	return Query->ColumnGetType(Column)==DBDataType::Null;
}
//---------------------------------------------------------------------------
bool cnRTL::DBBindColumn(iDBQuery *Query,ufInt16 Index,bool *Value){		return Query->ColumnBindBuffer(Index,DBDataType::Bool,Value,1,nullptr);	}
bool cnRTL::DBBindColumn(iDBQuery *Query,ufInt16 Index,sInt8 *Value){		return Query->ColumnBindBuffer(Index,DBDataType::SInt8,Value,1,nullptr);	}
bool cnRTL::DBBindColumn(iDBQuery *Query,ufInt16 Index,uInt8 *Value){		return Query->ColumnBindBuffer(Index,DBDataType::UInt8,Value,1,nullptr);	}
bool cnRTL::DBBindColumn(iDBQuery *Query,ufInt16 Index,sInt16 *Value){		return Query->ColumnBindBuffer(Index,DBDataType::SInt16,Value,1,nullptr);	}
bool cnRTL::DBBindColumn(iDBQuery *Query,ufInt16 Index,uInt16 *Value){		return Query->ColumnBindBuffer(Index,DBDataType::UInt16,Value,1,nullptr);	}
bool cnRTL::DBBindColumn(iDBQuery *Query,ufInt16 Index,sInt32 *Value){		return Query->ColumnBindBuffer(Index,DBDataType::SInt32,Value,1,nullptr);	}
bool cnRTL::DBBindColumn(iDBQuery *Query,ufInt16 Index,uInt32 *Value){		return Query->ColumnBindBuffer(Index,DBDataType::UInt32,Value,1,nullptr);	}
bool cnRTL::DBBindColumn(iDBQuery *Query,ufInt16 Index,sInt64 *Value){		return Query->ColumnBindBuffer(Index,DBDataType::SInt64,Value,1,nullptr);	}
bool cnRTL::DBBindColumn(iDBQuery *Query,ufInt16 Index,uInt64 *Value){		return Query->ColumnBindBuffer(Index,DBDataType::UInt64,Value,1,nullptr);	}
bool cnRTL::DBBindColumn(iDBQuery *Query,ufInt16 Index,Float32 *Value){		return Query->ColumnBindBuffer(Index,DBDataType::Float32,Value,1,nullptr);	}
bool cnRTL::DBBindColumn(iDBQuery *Query,ufInt16 Index,Float64 *Value){		return Query->ColumnBindBuffer(Index,DBDataType::Float64,Value,1,nullptr);	}
bool cnRTL::DBBindColumn(iDBQuery *Query,ufInt16 Index,cDateTime *Value){	return Query->ColumnBindBuffer(Index,DBDataType::DateTime,Value,1,nullptr);}
bool cnRTL::DBBindColumn(iDBQuery *Query,ufInt16 Index,uChar8 *Buffer,uIntn BuferLength,uIntn *DataLength){		return Query->ColumnBindBuffer(Index,DBDataType::String8,Buffer,BuferLength,DataLength);	}
bool cnRTL::DBBindColumn(iDBQuery *Query,ufInt16 Index,uChar16 *Buffer,uIntn BuferLength,uIntn *DataLength){	return Query->ColumnBindBuffer(Index,DBDataType::String16,Buffer,BuferLength,DataLength);	}
bool cnRTL::DBBindColumn(iDBQuery *Query,ufInt16 Index,uInt8 *Buffer,uIntn BuferSize,uIntn *DataSize){			return Query->ColumnBindBuffer(Index,DBDataType::Binary,Buffer,BuferSize,DataSize);	}
//---------------------------------------------------------------------------
bool cnRTL::DBBindParameter(iDBQuery *Query,ufInt16 Index,const bool *Value){		return Query->ParameterBindBuffer(Index,DBDataType::Bool,Value,nullptr);	}
bool cnRTL::DBBindParameter(iDBQuery *Query,ufInt16 Index,const sInt8 *Value){		return Query->ParameterBindBuffer(Index,DBDataType::SInt8,Value,nullptr);	}
bool cnRTL::DBBindParameter(iDBQuery *Query,ufInt16 Index,const uInt8 *Value){		return Query->ParameterBindBuffer(Index,DBDataType::UInt8,Value,nullptr);	}
bool cnRTL::DBBindParameter(iDBQuery *Query,ufInt16 Index,const sInt16 *Value){		return Query->ParameterBindBuffer(Index,DBDataType::SInt16,Value,nullptr);	}
bool cnRTL::DBBindParameter(iDBQuery *Query,ufInt16 Index,const uInt16 *Value){		return Query->ParameterBindBuffer(Index,DBDataType::UInt16,Value,nullptr);	}
bool cnRTL::DBBindParameter(iDBQuery *Query,ufInt16 Index,const sInt32 *Value){		return Query->ParameterBindBuffer(Index,DBDataType::SInt32,Value,nullptr);	}
bool cnRTL::DBBindParameter(iDBQuery *Query,ufInt16 Index,const uInt32 *Value){		return Query->ParameterBindBuffer(Index,DBDataType::UInt32,Value,nullptr);	}
bool cnRTL::DBBindParameter(iDBQuery *Query,ufInt16 Index,const sInt64 *Value){		return Query->ParameterBindBuffer(Index,DBDataType::SInt64,Value,nullptr);	}
bool cnRTL::DBBindParameter(iDBQuery *Query,ufInt16 Index,const uInt64 *Value){		return Query->ParameterBindBuffer(Index,DBDataType::UInt64,Value,nullptr);	}
bool cnRTL::DBBindParameter(iDBQuery *Query,ufInt16 Index,const Float32 *Value){	return Query->ParameterBindBuffer(Index,DBDataType::Float32,Value,nullptr);	}
bool cnRTL::DBBindParameter(iDBQuery *Query,ufInt16 Index,const Float64 *Value){	return Query->ParameterBindBuffer(Index,DBDataType::Float64,Value,nullptr);	}
bool cnRTL::DBBindParameter(iDBQuery *Query,ufInt16 Index,const cDateTime *Value){	return Query->ParameterBindBuffer(Index,DBDataType::DateTime,Value,nullptr);}
bool cnRTL::DBBindParameter(iDBQuery *Query,ufInt16 Index,const uChar8 *Buffer,const uIntn *DataLength){	return Query->ParameterBindBuffer(Index,DBDataType::String8,Buffer,DataLength);	}
bool cnRTL::DBBindParameter(iDBQuery *Query,ufInt16 Index,const uChar16 *Buffer,const uIntn *DataLength){	return Query->ParameterBindBuffer(Index,DBDataType::String16,Buffer,DataLength);	}
bool cnRTL::DBBindParameter(iDBQuery *Query,ufInt16 Index,const uInt8 *Buffer,const uIntn *DataSize){		return Query->ParameterBindBuffer(Index,DBDataType::Binary,Buffer,DataSize);	}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cDBColumnOutput_Binary::cDBColumnOutput_Binary(cMemoryBuffer *Buffer)
	:cDBOutputBuffer_Binary< cMemoryOperator<void> >(Buffer->MemoryStorage())
{
}
//---------------------------------------------------------------------------
bool cDBColumnOutput_Binary::Bind(iDBQuery *Query,ufInt16 Column)
{
	return Query->ColumnBindOutput(Column,DBDataType::Binary,this);
}
//---------------------------------------------------------------------------
cDBColumnOutput_Binary cnRTL::DBBindColumnOutput(cMemoryBuffer *Buffer)
{
	return Buffer;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cDBColumnOutput_String8::cDBColumnOutput_String8(cStringBuffer<uChar8> *Buffer)
	: cDBOutputBuffer_String< cMemoryOperator<uChar8> >(Buffer->StringStorage())
{
}
//---------------------------------------------------------------------------
bool cDBColumnOutput_String8::Bind(iDBQuery *Query,ufInt16 Column)
{
	return Query->ColumnBindOutput(Column,DBDataType::String8,this);
}
//---------------------------------------------------------------------------
cDBColumnOutput_String8 cnRTL::DBBindColumnOutput(cStringBuffer<uChar8> *Buffer)
{
	return Buffer;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cDBColumnOutput_String16::cDBColumnOutput_String16(cStringBuffer<uChar16> *Buffer)
	: cDBOutputBuffer_String< cMemoryOperator<uChar16> >(Buffer->StringStorage())
{
}
//---------------------------------------------------------------------------
bool cDBColumnOutput_String16::Bind(iDBQuery *Query,ufInt16 Column)
{
	return Query->ColumnBindOutput(Column,DBDataType::String16,this);
}
//---------------------------------------------------------------------------
cDBColumnOutput_String16 cnRTL::DBBindColumnOutput(cStringBuffer<uChar16> *Buffer)
{
	return Buffer;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cDBColumn<cMemoryBuffer>::cDBColumn()
	: cDBOutputBuffer_Binary< cMemoryOperator<void> >(Value.MemoryStorage())
{
}
//---------------------------------------------------------------------------
bool cDBColumn<cMemoryBuffer>::Bind(iDBQuery *Query,ufInt16 Column)
{
	return Query->ColumnBindOutput(Column,DBDataType::Binary,this);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cDBColumn< cStringBuffer<uChar8> >::cDBColumn()
	: cDBOutputBuffer_String< cMemoryOperator<uChar8> >(Value.StringStorage())
{
}
//---------------------------------------------------------------------------
bool cDBColumn< cStringBuffer<uChar8> >::Bind(iDBQuery *Query,ufInt16 Column)
{
	return Query->ColumnBindOutput(Column,DBDataType::String8,this);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cDBColumn< cStringBuffer<uChar16> >::cDBColumn()
	: cDBOutputBuffer_String< cMemoryOperator<uChar16> >(Value.StringStorage())
{
}
//---------------------------------------------------------------------------
bool cDBColumn< cStringBuffer<uChar16> >::Bind(iDBQuery *Query,ufInt16 Column)
{
	return Query->ColumnBindOutput(Column,DBDataType::String16,this);
}
//---------------------------------------------------------------------------
#if 0
//---------------------------------------------------------------------------
cDBQuery::cDBQuery()
{

}
//---------------------------------------------------------------------------
cDBQuery::~cDBQuery()
{
	if(fCommand!=nullptr){
		ClearCommand();
	}
}
//---------------------------------------------------------------------------
void cDBQuery::SetQuery(iDBQuery *Command)
{
	if(fCommand!=nullptr){
		ClearCommand();
	}
	fCommand=Command;
	if(fCommand!=nullptr){
		SetupCommand();
	}
}
//---------------------------------------------------------------------------
void cDBQuery::SetupCommand(void)
{
	//auto ParameterCount=fCommand->ParameterCount();
	auto ColumnCount=fCommand->ColumnCount();
	fColumnBinding.SetCount(ColumnCount);
	for(auto &ColBinding : fColumnBinding){

		ColBinding.RetLength=0;
		ColBinding.FetchProcess.ConstructAs<cFetchProcess>();
	}
}
//---------------------------------------------------------------------------
void cDBQuery::ClearCommand(void)
{
	for(auto &ColBinding : fColumnBinding){
		ColBinding.FetchProcess->Destruct();
	}
	fColumnBinding.Clear();
}
//---------------------------------------------------------------------------
bool cDBQuery::Execute(void)
{
	// preprocess parameter


	if(fCommand->Execute()==false)
		return false;

	// postprocess output

	return true;
}
//---------------------------------------------------------------------------
bool cDBQuery::Fetch(void)
{
	// preprocess column

	for(ufInt16 ColumnIndex=0,ColumnCount=static_cast<ufInt16>(fColumnBinding.GetCount());ColumnIndex<ColumnCount;ColumnIndex++){
		auto &Binding=fColumnBinding[ColumnIndex];
	}

	if(fCommand->Fetch()==false)
		return false;

	// postprocess columns

	for(ufInt16 ColumnIndex=0,ColumnCount=static_cast<ufInt16>(fColumnBinding.GetCount());ColumnIndex<ColumnCount;ColumnIndex++){
		auto &Binding=fColumnBinding[ColumnIndex];
		
		Binding.FetchProcess->Postprocess(ColumnIndex);
	}
	return true;
}
//---------------------------------------------------------------------------
void cDBQuery::Clear(void)
{
	return fCommand->Clear();
}
//---------------------------------------------------------------------------
bool cDBQuery::BindParameterBuffer(uInt16 Index,eDBDataType Type,const void *Buffer,const uIntn *Length)
{
	//if(Column>=fColumnBinding.GetCount()){
	//	return false;
	//}
	//auto &Binding=fColumnBinding[Column];
	//Binding.SetNullFetchProcess();
	return fCommand->ParameterBindBuffer(Index,Type,Buffer,Length);
}
//---------------------------------------------------------------------------
bool cDBQuery::BindParameter(uInt16 Index,const bool *Value){		return BindParameterBuffer(Index,DBDataType::Bool,Value,nullptr);		}
bool cDBQuery::BindParameter(uInt16 Index,const sInt8 *Value){		return BindParameterBuffer(Index,DBDataType::SInt8,Value,nullptr);		}
bool cDBQuery::BindParameter(uInt16 Index,const uInt8 *Value){		return BindParameterBuffer(Index,DBDataType::UInt8,Value,nullptr);		}
bool cDBQuery::BindParameter(uInt16 Index,const sInt16 *Value){		return BindParameterBuffer(Index,DBDataType::SInt16,Value,nullptr);		}
bool cDBQuery::BindParameter(uInt16 Index,const uInt16 *Value){		return BindParameterBuffer(Index,DBDataType::UInt16,Value,nullptr);		}
bool cDBQuery::BindParameter(uInt16 Index,const sInt32 *Value){		return BindParameterBuffer(Index,DBDataType::SInt32,Value,nullptr);		}
bool cDBQuery::BindParameter(uInt16 Index,const uInt32 *Value){		return BindParameterBuffer(Index,DBDataType::UInt32,Value,nullptr);		}
bool cDBQuery::BindParameter(uInt16 Index,const sInt64 *Value){		return BindParameterBuffer(Index,DBDataType::SInt64,Value,nullptr);		}
bool cDBQuery::BindParameter(uInt16 Index,const uInt64 *Value){		return BindParameterBuffer(Index,DBDataType::UInt64,Value,nullptr);		}
bool cDBQuery::BindParameter(uInt16 Index,const Float32 *Value){	return BindParameterBuffer(Index,DBDataType::Float32,Value,nullptr);	}
bool cDBQuery::BindParameter(uInt16 Index,const Float64 *Value){	return BindParameterBuffer(Index,DBDataType::Float64,Value,nullptr);	}
bool cDBQuery::BindParameter(uInt16 Index,const uChar8 *Value,const uIntn *ValueLength){	return BindParameterBuffer(Index,DBDataType::String8,Value,ValueLength);	}
bool cDBQuery::BindParameter(uInt16 Index,const uChar16 *Value,const uIntn *ValueLength){	return BindParameterBuffer(Index,DBDataType::String16,Value,ValueLength);	}
bool cDBQuery::BindParameter(uInt16 Index,const uInt8 *Value,const uIntn *ValueLength){		return BindParameterBuffer(Index,DBDataType::Binary,Value,ValueLength);	}
//---------------------------------------------------------------------------
bool cDBQuery::SetParameterNull(uInt16 Index,bool IsNull)
{
	return fCommand->ParameterSetNull(Index,IsNull);
}
//---------------------------------------------------------------------------
bool cDBQuery::BindParameter(uInt16 Index,tNullptr)
{
	return false;
}
//---------------------------------------------------------------------------
bool cDBQuery::BindParameter(uInt16 Index,const cDBVariant *Value)
{
	return false;
}
//---------------------------------------------------------------------------
bool cDBQuery::BindParameter(uInt16 Index,const cString<uChar8> *Value)
{
	return false;
}
//---------------------------------------------------------------------------
bool cDBQuery::BindParameter(uInt16 Index,const cString<uChar16> *Value)
{
	return false;
}
//---------------------------------------------------------------------------
bool cDBQuery::BindParameter(uInt16 Index,const cMemoryBuffer *Value)
{
	return false;
}
//---------------------------------------------------------------------------
bool cDBQuery::IsColumnNull(uInt16 Column)
{
	auto Type=fCommand->ColumnGetType(Column);
	return Type==DBDataType::Null;
}
//---------------------------------------------------------------------------
void cDBQuery::SetupColumnFetchType(uInt16 Column,eDBDataType Type)
{
	if(Column>=fColumnBinding.GetCount()){
		return;
	}
	auto &Binding=fColumnBinding[Column];
	Binding.SetNullFetchProcess();
	fCommand->ColumnBindOutput(Column,Type,nullptr);
}
//---------------------------------------------------------------------------
bool cDBQuery::BindColumnPtr_Value(uInt16 Column,eDBDataType Type,const void **Ptr)
{
	if(Column>=fColumnBinding.GetCount()){
		return false;
	}
	auto &Binding=fColumnBinding[Column];
	if(fCommand->ColumnBindOutput(Column,Type,nullptr)==false)
		return false;
	Binding.FetchProcess->Destruct();
	Binding.FetchProcess.ConstructAs<cFetchProcess_GetPtr>(Ptr);
	Binding.FetchProcess->Host=this;
	return true;
}

bool cDBQuery::BindColumnPtr(uInt16 Column,const bool* *Ptr){	return BindColumnPtr_Value(Column,DBDataType::Bool,reinterpret_cast<const void**>(Ptr));	}
bool cDBQuery::BindColumnPtr(uInt16 Column,const sInt8* *Ptr){	return BindColumnPtr_Value(Column,DBDataType::SInt8,reinterpret_cast<const void**>(Ptr));	}
bool cDBQuery::BindColumnPtr(uInt16 Column,const uInt8* *Ptr){	return BindColumnPtr_Value(Column,DBDataType::UInt8,reinterpret_cast<const void**>(Ptr));	}
bool cDBQuery::BindColumnPtr(uInt16 Column,const sInt16* *Ptr){	return BindColumnPtr_Value(Column,DBDataType::SInt16,reinterpret_cast<const void**>(Ptr));	}
bool cDBQuery::BindColumnPtr(uInt16 Column,const uInt16* *Ptr){	return BindColumnPtr_Value(Column,DBDataType::UInt16,reinterpret_cast<const void**>(Ptr));	}
bool cDBQuery::BindColumnPtr(uInt16 Column,const sInt32* *Ptr){	return BindColumnPtr_Value(Column,DBDataType::SInt32,reinterpret_cast<const void**>(Ptr));	}
bool cDBQuery::BindColumnPtr(uInt16 Column,const uInt32* *Ptr){	return BindColumnPtr_Value(Column,DBDataType::UInt32,reinterpret_cast<const void**>(Ptr));	}
bool cDBQuery::BindColumnPtr(uInt16 Column,const sInt64* *Ptr){	return BindColumnPtr_Value(Column,DBDataType::SInt64,reinterpret_cast<const void**>(Ptr));	}
bool cDBQuery::BindColumnPtr(uInt16 Column,const uInt64* *Ptr){	return BindColumnPtr_Value(Column,DBDataType::UInt64,reinterpret_cast<const void**>(Ptr));	}
bool cDBQuery::BindColumnPtr(uInt16 Column,const Float32* *Ptr){return BindColumnPtr_Value(Column,DBDataType::Float32,reinterpret_cast<const void**>(Ptr));	}
bool cDBQuery::BindColumnPtr(uInt16 Column,const Float64* *Ptr){return BindColumnPtr_Value(Column,DBDataType::Float64,reinterpret_cast<const void**>(Ptr));	}
//---------------------------------------------------------------------------
bool cDBQuery::BindColumnPtr_Array(uInt16 Column,eDBDataType Type,cConstMemory *Ptr)
{
	if(Column>=fColumnBinding.GetCount()){
		return false;
	}
	auto &Binding=fColumnBinding[Column];
	if(fCommand->ColumnBindOutput(Column,Type,nullptr)==false)
		return false;
	Binding.FetchProcess->Destruct();
	Binding.FetchProcess.ConstructAs<cFetchProcess_GetPtrLength>(&Ptr->Pointer,&Ptr->Length);
	Binding.FetchProcess->Host=this;
	return true;
}
bool cDBQuery::BindColumnPtr(uInt16 Column,cArray<const uChar8> *Ptr){	return BindColumnPtr_Array(Column,DBDataType::String8,reinterpret_cast<cConstMemory*>(Ptr));	}
bool cDBQuery::BindColumnPtr(uInt16 Column,cArray<const uChar16> *Ptr){	return BindColumnPtr_Array(Column,DBDataType::String16,reinterpret_cast<cConstMemory*>(Ptr));	}
bool cDBQuery::BindColumnPtr(uInt16 Column,cConstMemory *Ptr){			return BindColumnPtr_Array(Column,DBDataType::Binary,Ptr);	}
//---------------------------------------------------------------------------
bool cDBQuery::BindColumnPtr_BufferPtr(uInt16 Column,eDBDataType Type,const void* *Ptr,uIntn *Length)
{
	if(Column>=fColumnBinding.GetCount()){
		return false;
	}
	auto &Binding=fColumnBinding[Column];
	if(fCommand->ColumnBindOutput(Column,Type,nullptr)==false)
		return false;
	Binding.FetchProcess->Destruct();
	Binding.FetchProcess.ConstructAs<cFetchProcess_GetPtrLength>(Ptr,Length);
	Binding.FetchProcess->Host=this;
	return true;
}

bool cDBQuery::BindColumnPtr(uInt16 Column,const uChar8* *DataPtr,uIntn *LengthPtr){	return BindColumnPtr_BufferPtr(Column,DBDataType::String8,reinterpret_cast<const void**>(DataPtr),LengthPtr);	}
bool cDBQuery::BindColumnPtr(uInt16 Column,const uChar16* *DataPtr,uIntn *LengthPtr){	return BindColumnPtr_BufferPtr(Column,DBDataType::String16,reinterpret_cast<const void**>(DataPtr),LengthPtr);	}
bool cDBQuery::BindColumnPtr(uInt16 Column,const uInt8* *DataPtr,uIntn *LengthPtr){		return BindColumnPtr_BufferPtr(Column,DBDataType::Binary,reinterpret_cast<const void**>(DataPtr),LengthPtr);	}
//---------------------------------------------------------------------------
bool cDBQuery::BindColumn(uInt16 Column,cStringBuffer<uChar8> *Value)
{
	if(Column>=fColumnBinding.GetCount()){
		return false;
	}

	auto &Binding=fColumnBinding[Column];

	Binding.SetStringOutputFetchProcess(*Value);
	Binding.FetchProcess->Host=this;
	if(fCommand->ColumnBindOutput(Column,DBDataType::String8,Binding.FetchProcess->GetOutputBuffer())==false)
		return false;
	return true;
}
//---------------------------------------------------------------------------
bool cDBQuery::BindColumn(uInt16 Column,cStringBuffer<uChar16> *Value)
{
	if(Column>=fColumnBinding.GetCount()){
		return false;
	}

	auto &Binding=fColumnBinding[Column];

	Binding.SetStringOutputFetchProcess(*Value);
	Binding.FetchProcess->Host=this;
	if(fCommand->ColumnBindOutput(Column,DBDataType::String16,Binding.FetchProcess->GetOutputBuffer())==false)
		return false;
	return true;
}
//---------------------------------------------------------------------------
bool cDBQuery::BindColumn(uInt16 Column,cMemoryBuffer *Value)
{
	if(Column>=fColumnBinding.GetCount()){
		return false;
	}

	auto &Binding=fColumnBinding[Column];

	Binding.SetMemoryOutputFetchProcess(*Value);
	Binding.FetchProcess->Host=this;
	if(fCommand->ColumnBindOutput(Column,DBDataType::Binary,Binding.FetchProcess->GetOutputBuffer())==false)
		return false;
	return true;
}
//---------------------------------------------------------------------------
bool cDBQuery::BindColumn(uInt16 Column,cDBVariant *Value)
{
	if(Column>=fColumnBinding.GetCount()){
		return false;
	}
	auto &Binding=fColumnBinding[Column];
	Binding.FetchProcess->Destruct();
	Binding.FetchProcess.ConstructAs<cFetchProcess_GetVariant>(Value);
	Binding.FetchProcess->Host=this;

	if(fCommand->ColumnBindOutput(Column,DBDataType::Unknown,Binding.FetchProcess->GetOutputBuffer())==false){
		Binding.SetNullFetchProcess();
		return true;
	}
	return true;
}
//---------------------------------------------------------------------------
#endif // 0

#if 0



bool cDBQuery::GetColumn(uInt16 Column,eDBDataType Type,void *Buffer,uIntn BufferLength)
{
	if(Column>=fColumnBinding.GetCount()){
		return false;
	}

	auto ColType=fCommand->ColumnGetType(Column);
	auto ColBuffer=fCommand->ColumnGetBuffer(Column);

	return CastColumn(Type,Buffer,BufferLength,ColType,ColBuffer.Pointer,ColBuffer,Length);
}

uIntn cDBQuery::GetColumnLength(uInt16 Column,eDBDataType Type)
{
	if(Column>=fColumnBinding.GetCount()){
		return 0;
	}

	uIntn DataLength;
	if(fCommand->ColumnGetDataLength(Column,Type,DataLength)==false)
		return 0;

	return DataLength;
}


bool cDBQuery::GetColumn(uInt16 Column,eDBDataType Type,void *Buffer,uIntn BufferLength,uIntn &DataLength)
{
	if(Column>=fColumnBinding.GetCount()){
		return false;
	}

	if(fCommand->ColumnGetDataLength(Column,Type,DataLength)==false)
		return false;

	if(fCommand->ColumnGetData(Column,Type,Buffer,BufferLength)==false)
		return false;

	return true;
}


bool cDBQuery::GetColumn(uInt16 Column,cDBVariant &Value)
{
	if(Column>=fColumnBinding.GetCount()){
		return false;
	}

	auto ColumnDataType=fCommand->ColumnGetDataType(Column);
	switch(ColumnDataType){
	case DBDataType::Null:
		Value.SetType<void>();
		return true;
	case DBDataType::Bool:
		Value.SetType<bool>();
		return GetColumn(Column,DBDataType::Bool,Value.Pointer(),sizeof(bool));
	case DBDataType::sInt8:
		Value.SetType<sInt8>();
		return GetColumn(Column,DBDataType::sInt8,Value.Pointer(),sizeof(sInt8));
	case DBDataType::uInt8:
		Value.SetType<uInt8>();
		return GetColumn(Column,DBDataType::uInt8,Value.Pointer(),sizeof(uInt8));
	case DBDataType::sInt16:
		Value.SetType<sInt16>();
		return GetColumn(Column,DBDataType::sInt16,Value.Pointer(),sizeof(sInt16));
	case DBDataType::uInt16:
		Value.SetType<uInt16>();
		return GetColumn(Column,DBDataType::uInt16,Value.Pointer(),sizeof(uInt16));
	case DBDataType::sInt32:
		Value.SetType<sInt32>();
		return GetColumn(Column,DBDataType::sInt32,Value.Pointer(),sizeof(sInt32));
	case DBDataType::uInt32:
		Value.SetType<uInt32>();
		return GetColumn(Column,DBDataType::uInt32,Value.Pointer(),sizeof(uInt32));
	case DBDataType::sInt64:
		Value.SetType<sInt64>();
		return GetColumn(Column,DBDataType::sInt64,Value.Pointer(),sizeof(sInt64));
	case DBDataType::uInt64:
		Value.SetType<uInt64>();
		return GetColumn(Column,DBDataType::uInt64,Value.Pointer(),sizeof(uInt64));
	case DBDataType::Float32:
		Value.SetType<Float32>();
		return GetColumn(Column,DBDataType::Float32,Value.Pointer(),sizeof(Float32));
	case DBDataType::Float64:
		Value.SetType<Float64>();
		return GetColumn(Column,DBDataType::Float64,Value.Pointer(),sizeof(Float64));
	case DBDataType::DateTime:
		Value.SetType<cDateTime>();
		return GetColumn(Column,DBDataType::DateTime,Value.Pointer(),sizeof(cDateTime));
	case DBDataType::String8:
		Value.SetType< cString<uChar8> >();
		return GetColumn(Column,Value.Ref< cString<uChar8> >());
	case DBDataType::String16:
		Value.SetType< cString<uChar16> >();
		return GetColumn(Column,Value.Ref< cString<uChar16> >());
	case DBDataType::Binary:
		Value.SetType<cMemoryBuffer>();
		return GetColumn(Column,Value.Ref<cMemoryBuffer>());
//	case DBDataType::SystemTime:
//		Value.SetType<cMemoryBuffer>();
//		GetColumn(Column,Value.Ref<cMemoryBuffer>());
//		break;
	}
	return false;
}

bool cDBQuery::GetColumn(uInt16 Column,bool &Value){	return GetColumn(Column,DBDataType::Bool,&Value,sizeof(Value));		}
bool cDBQuery::GetColumn(uInt16 Column,sInt8 &Value){	return GetColumn(Column,DBDataType::sInt8,&Value,sizeof(Value));	}
bool cDBQuery::GetColumn(uInt16 Column,uInt8 &Value){	return GetColumn(Column,DBDataType::uInt8,&Value,sizeof(Value));	}
bool cDBQuery::GetColumn(uInt16 Column,sInt16 &Value){	return GetColumn(Column,DBDataType::sInt16,&Value,sizeof(Value));	}
bool cDBQuery::GetColumn(uInt16 Column,uInt16 &Value){	return GetColumn(Column,DBDataType::uInt16,&Value,sizeof(Value));	}
bool cDBQuery::GetColumn(uInt16 Column,sInt32 &Value){	return GetColumn(Column,DBDataType::sInt32,&Value,sizeof(Value));	}
bool cDBQuery::GetColumn(uInt16 Column,uInt32 &Value){	return GetColumn(Column,DBDataType::uInt32,&Value,sizeof(Value));	}
bool cDBQuery::GetColumn(uInt16 Column,sInt64 &Value){	return GetColumn(Column,DBDataType::sInt64,&Value,sizeof(Value));	}
bool cDBQuery::GetColumn(uInt16 Column,uInt64 &Value){	return GetColumn(Column,DBDataType::uInt64,&Value,sizeof(Value));	}

bool cDBQuery::GetColumn(uInt16 Column,Float32 &Value){	return GetColumn(Column,DBDataType::Float32,&Value,sizeof(Value));	}
bool cDBQuery::GetColumn(uInt16 Column,Float64 &Value){	return GetColumn(Column,DBDataType::Float64,&Value,sizeof(Value));	}

bool cDBQuery::GetColumn(uInt16 Column,cDateTime &Value){	return GetColumn(Column,DBDataType::DateTime,&Value,sizeof(Value));	}

bool cDBQuery::GetColumn(uInt16 Column,uChar8 *Buffer,uIntn BuferLength,uIntn &DataLength){		return GetColumn(Column,DBDataType::String8,Buffer,BuferLength,DataLength);		}
bool cDBQuery::GetColumn(uInt16 Column,uChar16 *Buffer,uIntn BuferLength,uIntn &DataLength){	return GetColumn(Column,DBDataType::String16,Buffer,BuferLength,DataLength);	}
bool cDBQuery::GetColumn(uInt16 Column,uInt8 *Buffer,uIntn BuferSize,uIntn &DataSize){			return GetColumn(Column,DBDataType::Binary,Buffer,BuferSize,DataSize);			}

template<class TCharacter>
bool cDBQuery::GetColumnString(uInt16 Column,eDBDataType Type,cString<TCharacter> &Value)
{
	cStringBuffer<TCharacter> StrBuffer;
	uIntn DataLength;
	if(fCommand->ColumnGetDataLength(Column,Type,DataLength)==false)
		return false;

	StrBuffer.SetLength(DataLength);
	
	if(fCommand->ColumnGetData(Column,Type,StrBuffer->Pointer,DataLength)==false)
		return false;


	return true;
}

bool cDBQuery::GetColumn(uInt16 Column,cString<uChar8> &Value){		return GetColumnString(Column,DBDataType::String8,Value);	}
bool cDBQuery::GetColumn(uInt16 Column,cString<uChar16> &Value){	return GetColumnString(Column,DBDataType::String16,Value);	}

bool cDBQuery::GetColumn(uInt16 Column,cMemoryBuffer &Value)
{
	uIntn DataLength;
	if(fCommand->ColumnGetDataLength(Column,DBDataType::Binary,DataLength)==false)
		return false;

	Value.SetSize(DataLength);
	
	if(fCommand->ColumnGetData(Column,DBDataType::Binary,Value->Pointer,DataLength)==false)
		return false;

	return true;
}
#endif // 0
//---------------------------------------------------------------------------
#endif // 0
