/*- cnTK - Stream ---------------------------------------------------------*/
/*         Developer : Code009                                             */
/*         Create on : 2018-08-14                                          */
/*-------------------------------------------------------------------------*/
#ifndef __cnLibrary_cnTK_Stream_HPP__
#define	__cnLibrary_cnTK_Stream_HPP__
/*-------------------------------------------------------------------------*/
#include <cnTK/Common.hpp>
#include <cnTK/Memory.hpp>
#include <cnTK/Array.hpp>
/*-------------------------------------------------------------------------*/
#if	cnLibrary_CPPFEATURELEVEL >= 1
//---------------------------------------------------------------------------
#include <cnTK/TKMacrosDeclare.inc>
//---------------------------------------------------------------------------
#if cnLibrary_CPPFEATURE_VARIADIC_TEMPLATES >= 200704L
//---------------------------------------------------------------------------
namespace cnLibrary{
//---------------------------------------------------------------------------
namespace cnStream{
//---------------------------------------------------------------------------
// TStreamReadBuffer<T>
//{
//	typedef T tElement;
//
// GatherReadBuffer
//	gather read data to buffer
//	the parameter Length is the demand of the caller, the stream should return data at least has the specified length.
//	if the stream returns smaller data buffer than requested, the caller will assume the stream currently has no more data.
//	when the stream returns not null data buffer, the caller must pair this call with DismissReadBuffer. if the caller is not going to dismiss any data, call DismissReadBuffer with 0.
//	when the stream returns zero length of data , calling DismissReadBuffer is optional, and the only valid paramerter is 0.
// Length			number of elements needed to gather for reading.
// return array of data buffer for reading. length of the returned buffer may less or greater than Length requested
// return null array if stream currently has no data available
//	cArray<const T> GatherReadBuffer(uIntn Length);
//
// DismissReadBuffer
//	dismiss length of read data in the last call to GatherReadBuffer.
//	Length parameter must less or equal than the length of data returned from GatherReadBuffer. when passing a length greater than the returned data buffer , the behavior is undefined.
//	the function must pair with GatherReadBuffer which returning not null data buffer, if no data needs to be dismissed , pass 0 in parameter Length to complete the pair.
//	if this function is called without a call to GatherReadBuffer, the behavior is undefined.
//	All buffer return by last call to GatherReadBuffer will be unavailable after this call, caller should not access any part of the buffer after the function.
// Length		length of data to dismiss, in elements
//	void DismissReadBuffer(uIntn Length);
//};

// TStreamWriteBuffer<T>
//{
//	typedef T tElement;
//
// ReserveWriteBuffer
//	reserve write buffer. uncommited buffer content will not be preserved
//	the parameter Length is the demand of the caller, the stream buffer should return a buffer have at least specified length.
//	if the stream returns smaller data buffer than requested, the caller will assume the stream currently can not provide such large buffer.
//	when the stream returns not null data buffer, the caller must pair this call with CommitWriteBuffer. if the caller is not going to commit any data, call CommitWriteBuffer with 0.
//	when the stream returns zero length of data , calling CommitWriteBuffer is optional, and the only valid paramerter is 0.
// Length		number of elements to reserve for writing.
// return array of data buffer for writing. length of the returned buffer may greater than Length requested
// return null array if buffer cannot provide buffer at the specified length
//	cArray<T> ReserveWriteBuffer(uIntn Length);
//
// CommitWriteBuffer
//	commit length of read data in the last call to ReserveWriteBuffer.
//	Length parameter must less or equal than the length of data returned from ReserveWriteBuffer. when passing a length greater than the returned data buffer , the behavior is undefined.
//	the function must pair with ReserveWriteBuffer which returning not null data buffer, if no data needs to be commited , pass 0 in parameter Length to complete the pair.
//	if this function is called without a call to ReserveWriteBuffer, the behavior is undefined.
//	All buffer return by last call to ReserveWriteBuffer will be unavailable after this call, caller should not access any part of the buffer after the function.
// Length		length of data to commit, in elements
//	void CommitWriteBuffer(uIntn Length);
//};
//---------------------------------------------------------------------------

template<class TStreamReadBuffer>
struct cCountingStreamReadBuffer
{
	TStreamReadBuffer fReadBuffer;
	uIntn TotalRead;

	typedef typename cnVar::TRemoveReference<TStreamReadBuffer>::Type::tElement tElement;

	template<class T>
	cCountingStreamReadBuffer(T cnLib_UREF ReadBuffer)noexcept(true)
		: fReadBuffer(cnLib_UREFCAST(T)(ReadBuffer))
		, TotalRead(0)
	{}

	cArray<const tElement> GatherReadBuffer(uIntn Length)noexcept(noexcept(fReadBuffer.GatherReadBuffer(Length)))
	{	return fReadBuffer.GatherReadBuffer(Length);	}
	void DismissReadBuffer(uIntn Size)noexcept(noexcept(fReadBuffer.DismissReadBuffer(0)))
	{
		fReadBuffer.DismissReadBuffer(Size);
		TotalRead+=Size;
	}
};
template<class TStreamReadBuffer>
cCountingStreamReadBuffer<TStreamReadBuffer> CountingStreamReadBuffer(TStreamReadBuffer&& WriteBuffer)noexcept(true){
	return cCountingStreamReadBuffer<TStreamReadBuffer>(static_cast<TStreamReadBuffer&&>(WriteBuffer));
}

template<class TStreamWriteBuffer>
struct cCountingStreamWriteBuffer
{
	TStreamWriteBuffer fWriteBuffer;
	uIntn TotalWritten;

	typedef typename cnVar::TRemoveReference<TStreamWriteBuffer>::Type::tElement tElement;

	template<class T>
	cCountingStreamWriteBuffer(T cnLib_UREF WriteBuffer)noexcept(true)
		: fWriteBuffer(cnLib_UREFCAST(T)(WriteBuffer))
		, TotalWritten(0)
	{}

	cArray<tElement> ReserveWriteBuffer(uIntn Length)noexcept(noexcept(fWriteBuffer.ReserveWriteBuffer(Length)))
	{	return fWriteBuffer.ReserveWriteBuffer(Length);	}
	void CommitWriteBuffer(uIntn Size)noexcept(noexcept(fWriteBuffer.CommitWriteBuffer(0)))
	{
		fWriteBuffer.CommitWriteBuffer(Size);
		TotalWritten+=Size;
	}
};
template<class TStreamWriteBuffer>
cCountingStreamWriteBuffer<TStreamWriteBuffer> CountingStreamWriteBuffer(TStreamWriteBuffer&& WriteBuffer)noexcept(true)
{	return cCountingStreamWriteBuffer<TStreamWriteBuffer>(static_cast<TStreamWriteBuffer&&>(WriteBuffer));	}

//---------------------------------------------------------------------------
template<class T>
class cVoidStreamBuffer
{
public:
	cVoidStreamBuffer()noexcept(true){}

	// TElement
	//	type of element in buffer
	typedef T tElement;

	// TStreamReadBuffer<T>

	cArray<T> GatherReadBuffer(uIntn Length)noexcept(true){
		cArray<T> Empty;
		Empty.Pointer=nullptr;
		Empty.Length=0;
		return Empty;
	}
	void DismissReadBuffer(uIntn Length)noexcept(true){}
		
	// TStreamWriteBuffer<T>

	cArray<T> ReserveWriteBuffer(uIntn Length)noexcept(true){
		cArray<T> Empty;
		Empty.Pointer=nullptr;
		Empty.Length=0;
		return Empty;
	}
	void CommitWriteBuffer(uIntn Length)noexcept(true){}
};
//---------------------------------------------------------------------------

// ArrayStreamReadBuffer
//	define an array read stream buffer from fixed length array
// <T>			type of array element
// <Length>		Length of array
// [in]Pointer	pointer to array
// return read stream object
template<class T,uIntn Length>
inline cVoidStreamBuffer<T> VoidStreamReadBuffer(void)noexcept(true)
{	return cVoidStreamBuffer<T>();	}
template<class T>
inline cVoidStreamBuffer<T> VoidStreamWriteBuffer(void)noexcept(true)
{	return cVoidStreamBuffer<T>();	}

//---------------------------------------------------------------------------
template<class T>
class cStaticArrayStreamBuffer
{
public:
	cStaticArrayStreamBuffer(T *Pointer,uIntn Length)noexcept(true){	fArray.Pointer=Pointer;	fArray.Length=Length;	}

	// TElement
	//	type of element in buffer
	typedef T tElement;

	operator const cArray<T>&()const noexcept(true){	return fArray;	}
	const cArray<T>* operator ->()const noexcept(true){return &fArray;	}

	// TStreamReadBuffer<T>

	cArray<T> GatherReadBuffer(uIntn Length)noexcept(true){	return Query(Length);	}
	void DismissReadBuffer(uIntn Length)noexcept(true){	Advance(Length);	}
		
	// TStreamWriteBuffer<T>

	cArray<T> ReserveWriteBuffer(uIntn Length)noexcept(true){	return Query(Length);	}
	void CommitWriteBuffer(uIntn Length)noexcept(true){	Advance(Length);	}

	cArray<T> Query(uIntn)noexcept(true){
		return fArray;
	}
	uIntn Advance(uIntn Length)noexcept(true){
		if(Length<fArray.Length){
			fArray.Pointer=ArrayOffsetPointer(fArray.Pointer,Length);
			fArray.Length-=Length;
			return Length;
		}
		else{
			uIntn AdvanceLength=fArray.Length;
			fArray.Pointer+=AdvanceLength;
			fArray.Length=0;
			return AdvanceLength;
		}
	}
protected:
	cArray<T> fArray;

};
//---------------------------------------------------------------------------

// ArrayStreamReadBuffer
//	define an array read stream buffer from fixed length array
// <T>			type of array element
// <Length>		Length of array
// [in]Pointer	pointer to array
// return read stream object
template<class T,uIntn Length>
inline cStaticArrayStreamBuffer<const T> ArrayStreamReadBuffer(T (&Pointer)[Length])noexcept(true)
{
	return cStaticArrayStreamBuffer<const T>(Pointer,Length);
}
// ArrayStreamReadBuffer
//	define an array read stream buffer from fixed length array
// <T>			type of array element
// [in]Pointer	pointer to array
// Length		Length of array
// return read stream object
template<class T>
inline cStaticArrayStreamBuffer<const T> ArrayStreamReadBuffer(const T *Pointer,uIntn Length)noexcept(true)
{	return cStaticArrayStreamBuffer<const T>(Pointer,Length);	}
//---------------------------------------------------------------------------
// ArrayStreamWriteBuffer
//	define an array write stream from fixed length array
// <T>				type of array element
// <Length>			Length of array
// [out]Pointer		pointer to array
// return write stream object
template<class T,uIntn Length>
inline cStaticArrayStreamBuffer<T> ArrayStreamWriteBuffer(T (&Pointer)[Length])noexcept(true)
{	return cStaticArrayStreamBuffer<T>(Pointer,Length);	}
// ArrayWriteStream
//	define an array write stream from fixed length array
// <T>				type of array element
// [out]Pointer		pointer to array
// Length			Length of array
// return write stream object
template<class T>
inline cStaticArrayStreamBuffer<T> ArrayStreamWriteBuffer(T *Pointer,uIntn Length)noexcept(true)
{	return cStaticArrayStreamBuffer<T>(Pointer,Length);	}

//---------------------------------------------------------------------------
template<class TAllocationOperator,class TElement>
class cArrayStorageStreamReadBuffer
{
public:
	cArrayStorageStreamReadBuffer(cnDataStruct::cArrayStorage<TAllocationOperator,TElement> &Storage,uIntn ReadIndex)noexcept(true)
		:fStorage(Storage),fReadIndex(ReadIndex){}

	cnDataStruct::cArrayStorage<TAllocationOperator,TElement> &GetStorage(void)const noexcept(true){	return fStorage;	}

	// TElement
	//	type of element in buffer
	typedef TElement tElement;

	// TStreamReadBuffer<T>

	cArray<const tElement> GatherReadBuffer(uIntn)noexcept(true)
	{
		uIntn LengthAvailable=fStorage.Length-fReadIndex;
		cArray<const tElement> Buffer;
		Buffer.Pointer=ArrayOffsetPointer(fStorage.Pointer,fReadIndex);
		Buffer.Length=LengthAvailable;
		return Buffer;
	}
	void DismissReadBuffer(uIntn Length)noexcept(true)
	{
		if(Length<fStorage.Length-fReadIndex){
			fReadIndex+=Length;
		}
		else{
			fReadIndex=fStorage.Length;
		}
	}
protected:
	cnDataStruct::cArrayStorage<TAllocationOperator,TElement> &fStorage;
	uIntn fReadIndex;
};
//---------------------------------------------------------------------------
template<class TAllocationOperator,class TElement>
class cArrayStorageStreamWriteBuffer
{
public:
	cArrayStorageStreamWriteBuffer(cnDataStruct::cArrayStorage<TAllocationOperator,TElement> &Storage,uIntn Increment)noexcept(true)
		:fStorage(Storage),fIncrement(Increment)
	{
		if(fIncrement<128)
			fIncrement=128;
	}

	cnDataStruct::cArrayStorage<TAllocationOperator,TElement> &GetStorage(void)const noexcept(true){	return fStorage;	}

	// TElement
	//	type of element in buffer
	typedef TElement tElement;

	// TStreamWriteBuffer<T>

	cArray<tElement> ReserveWriteBuffer(uIntn QueryLength)noexcept(noexcept(fStorage.GrowCapacityTo(0)))
	{

		uIntn LengthAvailable=fStorage.Capacity-fStorage.Length;
		if(LengthAvailable==0 || LengthAvailable<QueryLength){
			if(QueryLength<fIncrement){
				QueryLength=fIncrement;
			}
			// mark length to preserve buffer content
			uIntn CurLength=fStorage.Length;
			fStorage.Length=fStorage.Capacity;
			// grow capacity
			fStorage.GrowCapacityTo(CurLength+QueryLength);
			LengthAvailable=fStorage.Capacity-CurLength;
			// restore length
			fStorage.Length=CurLength;
		}
		cArray<tElement> Buffer;
		Buffer.Pointer=ArrayOffsetPointer(fStorage.Pointer,fStorage.Length);
		Buffer.Length=LengthAvailable;
		return Buffer;
	}
	void CommitWriteBuffer(uIntn Length)noexcept(true){
		uIntn RemainLength=fStorage.Capacity-fStorage.Length;
		if(Length<RemainLength){
			fStorage.Length+=Length;
		}
		else{
			fStorage.Length=fStorage.Capacity;
		}
	}
protected:
	cnDataStruct::cArrayStorage<TAllocationOperator,TElement> &fStorage;
	uIntn fIncrement;
};
//---------------------------------------------------------------------------

// ArrayStreamReadBuffer
//	define an array read stream from memory strorage
// [in,out]Storage			reference to the storage
// StartIndex				index of element to start reading
// return read stream object
template<class TAllocationOperator,class TElement>
inline cArrayStorageStreamReadBuffer<TAllocationOperator,TElement> ArrayStreamReadBuffer(cnDataStruct::cArrayStorage<TAllocationOperator,TElement> &Storage,uIntn StartIndex=0)noexcept(true)
{
	return cArrayStorageStreamReadBuffer<TAllocationOperator,TElement>(Storage,StartIndex);
}

// ArrayStreamWriteBuffer
//	define an array write stream buffer from memory strorage
// [in,out]Storage			reference to the storage
// Increment				default increment for storage expanding
// return write stream object
template<class TAllocationOperator,class TElement>
inline cArrayStorageStreamWriteBuffer<TAllocationOperator,TElement> ArrayStreamWriteBuffer(cnDataStruct::cArrayStorage<TAllocationOperator,TElement> &Storage,uIntn Increment=0)noexcept(true)
{
	return cArrayStorageStreamWriteBuffer<TAllocationOperator,TElement>(Storage,Increment);
}

//---------------------------------------------------------------------------
template<class TStreamReadBuffer>
class cStreamReadBufferBinaryAdapter
{
public:
	typedef void tElement;

	cStreamReadBufferBinaryAdapter(TStreamReadBuffer *ReadBuffer)noexcept(true)
		: fReadBuffer(ReadBuffer){}

	// TStreamReadBuffer<void>

	cConstMemory GatherReadBuffer(uIntn Size)noexcept(noexcept(fReadBuffer->GatherReadBuffer(Size)))
	{
		typedef typename TStreamReadBuffer::tElement TUpstreamElement;
		uIntn QueryLength=Size+fReadByteOffset;
		QueryLength=(QueryLength+sizeof(TUpstreamElement)-1)/sizeof(TUpstreamElement);
		cArray<const TUpstreamElement> Array=fReadBuffer->GatherReadBuffer(QueryLength);
		if(Array.Length==0)
			return NullArray;
		cConstMemory Buffer;
		Buffer.Pointer=cnMemory::CastPointerAddByteOffset<const void>(Array.Pointer,fReadByteOffset);
		Buffer.Length=Array.Length*sizeof(TUpstreamElement)-fReadByteOffset;
		return Buffer;
	}
	void DismissReadBuffer(uIntn Size)noexcept(noexcept(fReadBuffer->DismissReadBuffer(Size)))
	{
		typedef typename TStreamReadBuffer::tElement TUpstreamElement;
		Size+=fReadByteOffset;
		//uIntn CurByteOffset=fReadByteOffset;
		uIntn AdvanceLength=Size/sizeof(TUpstreamElement);
		fReadByteOffset=Size%sizeof(TUpstreamElement);
		fReadBuffer->DismissReadBuffer(AdvanceLength);
	}
protected:
	TStreamReadBuffer *fReadBuffer;
	ufInt8 fReadByteOffset;
};
//---------------------------------------------------------------------------
template<class TStreamWrtieBuffer>
class cStreamWriteBufferBinaryAdapter
{
public:
	typedef void tElement;

	cStreamWriteBufferBinaryAdapter(TStreamWrtieBuffer *WriteBuffer)noexcept(true)
		: fWriteBuffer(WriteBuffer)
		, fWriteByteOffset(0)
	{}

	// TStreamWriteBuffer<void>

	cMemory ReserveWriteBuffer(uIntn Size)noexcept(noexcept(fWriteBuffer->ReserveWriteBuffer(Size)))
	{
		typedef typename TStreamWrtieBuffer::tElement TUpstreamElement;
		uIntn QueryLength=Size+fWriteByteOffset;
		QueryLength=(QueryLength+sizeof(TUpstreamElement)-1)/sizeof(TUpstreamElement);

		cArray<TUpstreamElement> Array=fWriteBuffer->ReserveWriteBuffer(QueryLength);
		if(Array.Length==0)
			return NullArray;
		cMemory Buffer;
		Buffer.Pointer=cnMemory::CastPointerAddByteOffset<const void>(Array.Pointer,fWriteByteOffset);
		Buffer.Length=Array.Length*sizeof(TUpstreamElement)-fWriteByteOffset;
		return Buffer;
	}
	void CommitWriteBuffer(uIntn Size)noexcept(noexcept(fWriteBuffer->CommitWriteBuffer(Size)))
	{
		typedef typename TStreamWrtieBuffer::tElement TUpstreamElement;
		Size+=fWriteByteOffset;
		//uIntn CurByteOffset=fWriteByteOffset;
		uIntn AdvanceLength=Size/sizeof(TUpstreamElement);
		fWriteByteOffset=Size%sizeof(TUpstreamElement);
		fWriteBuffer->CommitWriteBuffer(AdvanceLength);
	}

protected:
	TStreamWrtieBuffer *fWriteBuffer;
	ufInt8 fWriteByteOffset;
};
//---------------------------------------------------------------------------
template<class TStreamReadBuffer>
inline cStreamReadBufferBinaryAdapter<TStreamReadBuffer> StreamReadBufferBinaryAdapter(TStreamReadBuffer *ReadBuffer)noexcept(true)
{
	return cStreamReadBufferBinaryAdapter<TStreamReadBuffer>(ReadBuffer);
}
//---------------------------------------------------------------------------
template<class TStreamWrtieBuffer>
inline cStreamWriteBufferBinaryAdapter<TStreamWrtieBuffer> StreamWriteBufferBinaryAdapter(TStreamWrtieBuffer *WriteBuffer)noexcept(true)
{
	return cStreamWriteBufferBinaryAdapter<TStreamWrtieBuffer>(WriteBuffer);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

template<class TStreamReadBuffer>
class cBinaryStreamBufferReader
{
public:
	class cNestedReadBuffer
	{
	public:
		cNestedReadBuffer(cBinaryStreamBufferReader &OwnerStream,uIntn &SizeRead)
			:fOwnerStream(OwnerStream),fSizeRead(SizeRead){}

		// TStreamReadBuffer<void>

		cConstMemory GatherReadBuffer(uIntn QuerySize){
			if(fSizeRead+QuerySize>fOwnerStream.fReadMemory.Length){
				fOwnerStream.fReadMemory=fOwnerStream.fReadBuffer.GatherReadBuffer(fSizeRead+QuerySize);
				if(fOwnerStream.fReadMemory.Length==0){
					// no more space
					return NullArray;
				}
			}
			cConstMemory Buffer;
			Buffer.Pointer=cnMemory::PointerAddByteOffset(fOwnerStream.fReadMemory.Pointer,fSizeRead);
			Buffer.Length=fOwnerStream.fReadMemory.Length-fSizeRead;
			return Buffer;
		}
		void DismissReadBuffer(uIntn Size){
			fSizeRead+=Size;
			if(fSizeRead>=fOwnerStream.fReadMemory.Length)
				fSizeRead=fOwnerStream.fReadMemory.Length;
		}
	protected:
		cBinaryStreamBufferReader &fOwnerStream;
		uIntn &fSizeRead;
	};
protected:
	TStreamReadBuffer fReadBuffer;
	cConstMemory fReadMemory;
public:
	cBinaryStreamBufferReader(TStreamReadBuffer *ReadBuffer)
		:fReadBuffer(ReadBuffer),fReadMemory{0,0}{}

	cNestedReadBuffer NestedBuffer(uIntn &SizeRead){
		SizeRead=0;
		return cNestedReadBuffer(*this,SizeRead);
	}

	bool Prepare(uIntn Size){
		if(Size<=fReadMemory.Length){
			return true;
		}
		fReadMemory=fReadBuffer.GatherReadBuffer(Size);
		return Size<=fReadMemory.Length;
	}

	void Advance(uIntn Size){
		fReadBuffer.CompleteReadBuffer(Size);
		fReadMemory.Pointer=nullptr;
		fReadMemory.Length=0;
	}

	uIntn CheckSize(uIntn Size){
		if(Size>fReadMemory.Length){
			fReadMemory=fReadBuffer.GatherReadBuffer(Size);
			if(fReadMemory.Length==0)
				return 0;	// no more buffer
			if(Size>fReadMemory.Length){
				Size=fReadMemory.Length;
			}
		}
		return Size;
	}

	const void *GetAccessPtr(void)const{
		return fReadMemory.Pointer;
	}

#if 0
	uIntn Access(void *Dest,uIntn Size){
		Size=CheckSize(Size);
		if(Size!=0){
			cnMemory::Copy(Dest,fReadMemory.Pointer,Size);
			Advance(Size);
		}
		return Size;
	}
#endif
	
	template<class T>
	const T* OffsetPtr(uIntn Offset,uIntn Length=1){
		if(Prepare(Offset+sizeof(T)*Length)==false)
			return nullptr;

		return reinterpret_cast<const T*>(cnMemory::PointerAddByteOffset(fReadMemory.Pointer,Offset));
	}

	template<class T>
	const T* Ptr(uIntn Length=1){
		if(Prepare(sizeof(T)*Length)==false)
			return nullptr;

		return reinterpret_cast<const T*>(fReadMemory.Pointer);
	}
	const void* Ptr(uIntn Size){
		if(Prepare(Size)==false)
			return nullptr;

		return fReadMemory.Pointer;
	}
	template<class T>
	void Advance(uIntn Length=1){
		Advance(sizeof(T)*Length);
	}
	template<class T>
	void Advance(const T *,uIntn Length=1){
		Advance(sizeof(T)*Length);
	}

	template<class T>
	bool Value(T &Dest){
		if(Prepare(sizeof(T))==false){
			return false;
		}

		Dest=*reinterpret_cast<const T*>(fReadMemory.Pointer);
		Advance(sizeof(T));
		return true;
	}
#if 0

	bool Block(void *Dest,uIntn Size){
		auto ReadPtr=Ptr(Size);
		if(ReadPtr==nullptr)
			return false;
		cnMemory::Copy(Dest,ReadPtr,Size);
		Advance(Size);
		return true;
	}
#endif // 0
	template<class T>
	bool Array(const T *Dest,uIntn Length){
		uIntn ArraySize=Length*sizeof(T);
		auto ReadPtr=Ptr(ArraySize);
		if(ReadPtr==nullptr){
			return false;
		}
		for(uIntn i=0;i<ArraySize;i++){
			Dest[i]=ReadPtr[i];
		}
		Advance(ArraySize);
		return true;
	}
};
//---------------------------------------------------------------------------
template<class TStreamWriteBuffer>
class cBinaryStreamBufferWriter
{
public:
	class cNestedWriteBuffer
	{
	public:
		cNestedWriteBuffer(cBinaryStreamBufferWriter &OwnerStream,uIntn &SizeWritten)
			:fOwnerStream(OwnerStream),fSizeWritten(SizeWritten){}

		// TStreamWriteBuffer<void>

		cMemory ReserveWriteBuffer(uIntn QuerySize){
			if(fSizeWritten+QuerySize>fOwnerStream.fWriteMemory.Length){
				fOwnerStream.fWriteMemory=fOwnerStream.fWriteBuffer.ReserveWriteBuffer(fSizeWritten+QuerySize);
				if(fOwnerStream.fWriteMemory.Length==0){
					// no more space
					return NullArray;
				}
			}
			cMemory Buffer;
			Buffer.Pointer=cnMemory::PointerAddByteOffset(fOwnerStream.fWriteMemory.Pointer,fSizeWritten);
			Buffer.Length=fOwnerStream.fWriteMemory.Length-fSizeWritten;
			return Buffer;
		}
		void CommitWriteBuffer(uIntn Size){
			fSizeWritten+=Size;
			if(fSizeWritten>=fOwnerStream.fWriteMemory.Length)
				fSizeWritten=fOwnerStream.fWriteMemory.Length;
		}
	protected:
		cBinaryStreamBufferWriter &fOwnerStream;
		uIntn &fSizeWritten;
	};
protected:
	TStreamWriteBuffer *fWriteBuffer;
	cMemory fWriteMemory;
public:
	cBinaryStreamBufferWriter(TStreamWriteBuffer *WriteBuffer)
		:fWriteBuffer(WriteBuffer),fWriteMemory{0,0}{}
	
	cNestedWriteBuffer NestedBuffer(uIntn &SizeWritten){
		SizeWritten=0;
		return cNestedWriteBuffer(*this,SizeWritten);
	}

	bool Prepare(uIntn Size){
		if(Size<=fWriteMemory.Length){
			return true;
		}
		fWriteMemory=fWriteBuffer->ReserveWriteBuffer(Size);
		return Size<=fWriteMemory.Length;
	}

	void Advance(uIntn Size){
		fWriteBuffer->CommitWriteBuffer(Size);
		fWriteMemory.Pointer=nullptr;
		fWriteMemory.Length=0;
	}

	uIntn CheckSize(uIntn Size){
		if(Size>fWriteMemory.Length){
			fWriteMemory=fWriteBuffer->ReserveWriteBuffer(Size);
			if(fWriteMemory.Length==0)
				return 0;	// no more buffer
			if(Size>fWriteMemory.Length){
				Size=fWriteMemory.Length;
			}
		}
		return Size;
	}

	const void *GetAccessPtr(void)const{
		return fWriteMemory.Pointer;
	}
#if 0
	uIntn Access(const void *Dest,uIntn Size){
		Size=CheckSize(Size);
		if(CheckSize!=0){
			cnMemory::Copy(Dest,fWriteMemory.Pointer,Size);
		}
		Advance(Size);
		return Size;
	}
#endif // 0

	template<class T>
	T* OffsetPtr(uIntn Offset,uIntn Length=1){
		if(Prepare(Offset+sizeof(T)*Length)==false)
			return nullptr;

		return reinterpret_cast<T*>(cnMemory::PointerAddByteOffset(fWriteMemory.Pointer,Offset));
	}

	template<class T>
	T* Ptr(uIntn Length=1){
		if(Prepare(sizeof(T)*Length)==false)
			return nullptr;

		return reinterpret_cast<T*>(fWriteMemory.Pointer);
	}
	void* Ptr(uIntn Size){
		if(Prepare(Size)==false)
			return nullptr;

		return fWriteMemory.Pointer;
	}

	template<class T>
	void Advance(uIntn Length=1){
		Advance(sizeof(T)*Length);
	}
	template<class T>
	void Advance(const T *,uIntn Length=1){
		Advance(sizeof(T)*Length);
	}

	template<class T>
	bool Value(const T &Src){
		if(Prepare(sizeof(T))==false){
			return false;
		}

		*reinterpret_cast<T*>(fWriteMemory.Pointer)=Src;
		Advance(sizeof(T));
		return true;
	}
#if 0
	bool Block(const void *Src,uIntn Size){
		auto WritePtr=Ptr(Size);
		if(WritePtr==nullptr){
			return false;
		}
		cnMemory::Copy(WritePtr,Src,Size);
		Advance(Size);
		return true;
	}
#endif // 0
	template<class T>
	bool Array(const T *Src,uIntn Length){
		auto WritePtr=Ptr<T>(Length);
		if(WritePtr==nullptr){
			return false;
		}
		for(uIntn i=0;i<Length;i++){
			WritePtr[i]=Src[i];
		}
		Advance<T>(Length);
		return true;
	}
};
//---------------------------------------------------------------------------
template<class TStreamReadBuffer>
inline cBinaryStreamBufferReader<TStreamReadBuffer> BinaryStreamBufferReader(TStreamReadBuffer *ReadBuffer)
{
	return cBinaryStreamBufferReader<TStreamReadBuffer>(ReadBuffer);
}
//---------------------------------------------------------------------------
template<class TStreamWriteBuffer>
inline cBinaryStreamBufferWriter<TStreamWriteBuffer> BinaryStreamBufferWriter(TStreamWriteBuffer *WriteBuffer)
{
	return cBinaryStreamBufferWriter<TStreamWriteBuffer>(WriteBuffer);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
}	// namespace cnStream
//---------------------------------------------------------------------------
}	// namespace cnLibrary
//---------------------------------------------------------------------------
#endif	// cnLibrary_CPPFEATURE_VARIADIC_TEMPLATES >= 200704L
//---------------------------------------------------------------------------
#include <cnTK/TKMacrosCleanup.inc>
//---------------------------------------------------------------------------
#endif	/* cnLibrary_CPPFEATURELEVEL >= 1 */
/*-------------------------------------------------------------------------*/
#endif
