#include "StreamBuffer.h"
//---------------------------------------------------------------------------
using namespace cnLibrary;
using namespace cnRTL;
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cMemoryBufferQueueReadBuffer::cMemoryBufferQueueReadBuffer()
{
	fDataStartIndex=0;
	fReadBuffer=nullptr;
	fReadBufferSize=0;
	fReadIndex=0;
}
//---------------------------------------------------------------------------
cMemoryBufferQueueReadBuffer::~cMemoryBufferQueueReadBuffer()
{
}
//---------------------------------------------------------------------------
void cMemoryBufferQueueReadBuffer::Clear(void)
{
	fDataBuffer.Clear();
	fDataStartIndex=0;

	fReadBuffer=nullptr;
	fReadBufferSize=0;
	fReadIndex=0;
}
//---------------------------------------------------------------------------
void cMemoryBufferQueueReadBuffer::StartBuffer(const void *Buffer,uIntn BufferSize)
{
	fReadBuffer=Buffer;
	fReadBufferSize=BufferSize;
	fReadIndex=0;
}
//---------------------------------------------------------------------------
uIntn cMemoryBufferQueueReadBuffer::FinishBuffer(void)
{
	auto SizeRead=fReadIndex;

	// remove unused head
	if(fDataStartIndex!=0){
		fDataBuffer.Replace(0,fDataStartIndex,0);
		fDataStartIndex=0;
	}

	// clear read buffer
	fReadBuffer=nullptr;
	fReadBufferSize=0;
	fReadIndex=0;

	return SizeRead;
}
//---------------------------------------------------------------------------
void cMemoryBufferQueueReadBuffer::QueueAllBuffer(void)
{
	// remove unused head
	if(fDataStartIndex!=0){
		fDataBuffer.Replace(0,fDataStartIndex,0);
		fDataStartIndex=0;
	}

	// append remaining buffer
	uIntn ReadSizeAvailable=fReadBufferSize-fReadIndex;
	if(ReadSizeAvailable!=0){
		auto AppendIndex=fDataBuffer->Length;
		fDataBuffer.Append(ReadSizeAvailable);
		cnMemory::Copy(fDataBuffer[AppendIndex],cnMemory::PointerAddByteOffset(fReadBuffer,fReadIndex),ReadSizeAvailable);
	}

	// clear read buffer
	fReadBuffer=nullptr;
	fReadBufferSize=0;
	fReadIndex=0;
}
//---------------------------------------------------------------------------
cConstMemory cMemoryBufferQueueReadBuffer::PackReadBuffer(uIntn QuerySize)
{
	uIntn ReadSizeAvailable=fReadBufferSize-fReadIndex;
	auto PackedSize=fDataBuffer.GetSize();
	if(PackedSize!=0){
		PackedSize-=fDataStartIndex;
		if(PackedSize<QuerySize){
			// try pack buffer
			if(fReadBuffer!=nullptr){
				uIntn AppendSize=QuerySize-PackedSize;	// size needs to append
				if(AppendSize>ReadSizeAvailable){
					AppendSize=ReadSizeAvailable;
				}
				// append read buffer to packed buffer
				auto BufferIndex=fDataBuffer.GetSize();
				fDataBuffer.Append(AppendSize);
				cnMemory::Copy(fDataBuffer[BufferIndex],cnMemory::PointerAddByteOffset(fReadBuffer,fReadIndex),AppendSize);
				fReadIndex+=AppendSize;
			}
		}
		// return any available in packed buffer
		cConstMemory RetBuffer;
		RetBuffer.Pointer=fDataBuffer[fDataStartIndex];
		RetBuffer.Length=fDataBuffer.GetSize()-fDataStartIndex;
		return RetBuffer;
	}
	else{
		// return data in read buffer
		cConstMemory RetBuffer;
		RetBuffer.Pointer=cnMemory::PointerAddByteOffset(fReadBuffer,fReadIndex);
		RetBuffer.Length=ReadSizeAvailable;

		return RetBuffer;
	}
}
//---------------------------------------------------------------------------
bool cMemoryBufferQueueReadBuffer::PackReadData(cConstMemory &Buffer,uIntn QuerySize)
{
	Buffer=PackReadBuffer(QuerySize);
	return Buffer.Length>=QuerySize;
}
//---------------------------------------------------------------------------
bool cMemoryBufferQueueReadBuffer::QueryReadBuffer(cConstMemory &Buffer,uIntn QuerySize)
{
	Buffer=PackReadBuffer(QuerySize);
	return Buffer.Length!=0;
}
//---------------------------------------------------------------------------
void cMemoryBufferQueueReadBuffer::CompleteReadBuffer(uIntn Size)
{
	auto PackedSize=fDataBuffer.GetSize();
	if(PackedSize!=0){
		PackedSize-=fDataStartIndex;
		if(PackedSize>Size){
			// advance in packed buffer
			fDataStartIndex+=Size;
			return;
		}
		Size-=PackedSize;
		// clear packed buffer
		fDataBuffer.Clear();
		fDataStartIndex=0;
	}
	// advance in read buffer
	uIntn SizeAvailable=fReadBufferSize-fReadIndex;
	if(Size>=SizeAvailable){
		// all read buffer read
		fReadIndex=fReadBufferSize;
	}
	else{
		fReadIndex+=Size;
	}
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cBufferPacker::cBufferPacker()
{
	fDataStartIndex=0;
	fPendingBuffer=nullptr;
	fPendingBufferSize=0;
}
//---------------------------------------------------------------------------
cBufferPacker::~cBufferPacker()
{
}
//---------------------------------------------------------------------------
void cBufferPacker::PutBuffer(const void *Buffer,uIntn BufferSize)
{
	fPendingBuffer=Buffer;
	fPendingBufferSize=BufferSize;
	if(fDataBuffer.GetSize()==0){
		fPendingPackedSize=0;
	}
}
//---------------------------------------------------------------------------
const void *cBufferPacker::PackedData(void)const
{
	if(fDataBuffer.GetSize()!=0){
		return fDataBuffer[fDataStartIndex];
	}
	// packed data in pending buffer
	return fPendingBuffer;
}
//---------------------------------------------------------------------------
uIntn cBufferPacker::PackedSize(void)const
{
	if(fDataBuffer.GetSize()!=0){
		return fDataBuffer.GetSize()-fDataStartIndex;
	}
	// packed data in pending buffer
	return fPendingPackedSize;
}
//---------------------------------------------------------------------------
const void *cBufferPacker::PendingData(void)const
{
	if(fDataBuffer.GetSize()!=0){
		return fPendingBuffer;
	}
	// packed data in pending buffer
	return cnMemory::PointerAddByteOffset(fPendingBuffer,fPendingPackedSize);
}
uIntn cBufferPacker::PendingSize(void)const
{
	if(fDataBuffer.GetSize()!=0){
		return fPendingBufferSize;
	}
	// packed data in pending buffer
	return fPendingBufferSize-fPendingPackedSize;
}
//---------------------------------------------------------------------------
bool cBufferPacker::Pack(uIntn SizeNeeded)
{
	auto CurSize=fDataBuffer.GetSize();
	if(CurSize!=0){
		CurSize-=fDataStartIndex;
		if(CurSize<SizeNeeded){
			if(fPendingBuffer==nullptr){
				// not enough buffer
				return false;
			}
			uIntn AppendSize=SizeNeeded-CurSize;	// size needs to append
			if(fPendingBufferSize<=AppendSize){
				bool Packed=fPendingBufferSize==AppendSize;
				// use up all pending buffer
				if(fDataStartIndex!=0){
					// clean invalid data
					fDataBuffer.Replace(0,fDataStartIndex,0);
					fDataStartIndex=0;
				}
				// merge pending buffer
				auto BufferIndex=fDataBuffer.GetSize();
				fDataBuffer.Append(fPendingBufferSize);
				cnMemory::Copy(fDataBuffer[BufferIndex],fPendingBuffer,fPendingBufferSize);
				CurSize+=fPendingBufferSize;

				fPendingBuffer=nullptr;
				fPendingBufferSize=0;
				return Packed;
			}
			// enough buffer, just append
			auto BufferIndex=fDataBuffer.GetSize();
			fDataBuffer.Append(AppendSize);
			cnMemory::Copy(fDataBuffer[BufferIndex],fPendingBuffer,AppendSize);
			fPendingBuffer=cnMemory::PointerAddByteOffset(fPendingBuffer,AppendSize);
			fPendingBufferSize-=AppendSize;
		}
		return true;
	}
	else{
		if(SizeNeeded>fPendingBufferSize){
			if(fPendingBuffer!=nullptr){
				// switch out pending buffer
				fDataBuffer.Copy(fPendingBuffer,fPendingBufferSize);
				fDataStartIndex=0;
				fPendingBuffer=nullptr;
				fPendingBufferSize=0;
			}
			return false;
		}
		else{
			fPendingPackedSize=SizeNeeded;
			return true;
		}
	}
}
//---------------------------------------------------------------------------
uIntn cBufferPacker::Advance(uIntn Size)
{
	auto PopSize=fDataBuffer.GetSize();
	if(PopSize!=0){
		PopSize-=fDataStartIndex;
		if(Size<PopSize){
			fDataStartIndex+=Size;
			return Size;
		}
		fDataBuffer.Clear();
		fDataStartIndex=0;
		if(Size==PopSize)
			return PopSize;
	}
	// advance in pending buffer
	if(fPendingBufferSize>Size){
		fPendingBuffer=cnMemory::PointerAddByteOffset(fPendingBuffer,Size);
		fPendingBufferSize-=Size;
		if(fPendingPackedSize>Size){
			fPendingPackedSize-=Size;
		}
		else{
			fPendingPackedSize=0;
		}
		PopSize+=Size;
	}
	else{
		PopSize+=fPendingBufferSize;
		fPendingBuffer=nullptr;
		fPendingBufferSize=0;
		fPendingPackedSize=0;
	}
	return PopSize;
}
//---------------------------------------------------------------------------
void cBufferPacker::Clear(void)
{
	fDataBuffer.Clear();
	fDataStartIndex=0;

	fPendingBuffer=nullptr;
	fPendingBufferSize=0;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cLoopbackStreamBuffer::cLoopbackStreamBuffer()noexcept
{
	fReadIndex=0;
}
//---------------------------------------------------------------------------
cLoopbackStreamBuffer::~cLoopbackStreamBuffer()noexcept
{
}
//---------------------------------------------------------------------------
cMemory cLoopbackStreamBuffer::GetReadBuffer(void)const noexcept
{
	cMemory ReadBuffer;
	ReadBuffer.Pointer=fBuffer[fReadIndex];
	ReadBuffer.Length=fBuffer->Length-fReadIndex;
	return ReadBuffer;
}
//---------------------------------------------------------------------------
cConstMemory cLoopbackStreamBuffer::GatherReadBuffer(uIntn)noexcept
{
	return GetReadBuffer();
}
//---------------------------------------------------------------------------
void cLoopbackStreamBuffer::DismissReadBuffer(uIntn Size)noexcept
{
	fReadIndex+=Size;
	if(fReadIndex<fBuffer->Length){
		return;
	}
	// reset buffer
	fBuffer.Clear();
	fReadIndex=0;
}
//---------------------------------------------------------------------------
cMemory cLoopbackStreamBuffer::ReserveWriteBuffer(uIntn QuerySize)noexcept
{
	uIntn NewCapacity=fBuffer->Length+QuerySize;
	if(NewCapacity>fBuffer->Capacity){
		if(NewCapacity>BufferSizeLimit){
			NewCapacity=BufferSizeLimit;
		}
		fBuffer.SetCapacity(NewCapacity);
	}
	cMemory WriteBuffer;
	WriteBuffer.Pointer=fBuffer[fBuffer->Length];
	WriteBuffer.Length=fBuffer->Capacity-fBuffer->Length;
	return WriteBuffer;
}
//---------------------------------------------------------------------------
void cLoopbackStreamBuffer::CommitWriteBuffer(uIntn Size)noexcept
{
	uIntn BufferCapacity=fBuffer->Capacity;
	uIntn BufferLength=fBuffer->Length;
	uIntn UnusedLength=BufferCapacity-BufferLength;
	if(Size<UnusedLength){
		fBuffer.SetSize(BufferLength+Size);
	}
	else{
		fBuffer.SetSize(BufferCapacity);
	}
}

//---------------------------------------------------------------------------
uIntn cLoopbackStreamBuffer::ReadTo(void *Buffer,uIntn Size)noexcept
{
	return ReadFromStream(Buffer,Size,this);
}
//---------------------------------------------------------------------------
uIntn cLoopbackStreamBuffer::WriteFrom(const void *Buffer,uIntn Size)noexcept
{
	return WriteToStream(Buffer,Size,this);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
#if 0

cStreamBufferIOQueue::cStreamBufferIOQueue()
{
	fReadIndex=0;
}
//---------------------------------------------------------------------------
cStreamBufferIOQueue::~cStreamBufferIOQueue()
{
}
//---------------------------------------------------------------------------
uIntn cStreamBufferIOQueue::GetAvailableReadSize(void)const
{
	return fBuffer->Length-fReadIndex;
}
//---------------------------------------------------------------------------
cConstMemory cStreamBufferIOQueue::QueryRead(uIntn QuerySize)
{
	cConstMemory ReadBuffer;
	UnusedParameter(QuerySize);
	ReadBuffer.Pointer=fBuffer[fReadIndex];
	ReadBuffer.Length=fBuffer->Length-fReadIndex;
	return ReadBuffer;
}
//---------------------------------------------------------------------------
void cStreamBufferIOQueue::AdvanceRead(uIntn Size)
{
	fReadIndex+=Size;
	if(fReadIndex>=fBuffer->Length){
		// reset buffer
		fBuffer.Clear();
		fReadIndex=0;
	}
}
//---------------------------------------------------------------------------
cMemory cStreamBufferIOQueue::QueryWrite(uIntn QuerySize)
{
	uIntn NewCapacity=fBuffer->Length+QuerySize;
	if(NewCapacity>fBuffer->Capacity){
		fBuffer.SetCapacity(NewCapacity);
	}
	cMemory WriteBuffer;
	WriteBuffer.Pointer=fBuffer[fBuffer->Length];
	WriteBuffer.Length=fBuffer->Capacity-fBuffer->Length;
	return WriteBuffer;
}
//---------------------------------------------------------------------------
void cStreamBufferIOQueue::CommitWrite(uIntn Size)
{
	uIntn NewLength=fBuffer->Length+Size;
	if(NewLength>fBuffer->Capacity){
		NewLength=fBuffer->Capacity;
	}
	fBuffer.SetSize(NewLength);
}
#endif // 0
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cAsyncLoopbackStreamBuffer::cAsyncLoopbackStreamBuffer(uIntn BufferSizeLimit)noexcept
	: fBufferSize(cAllocationOperator::RoundUpCapacity(BufferSizeLimit))
	, fReadingItem(fBufferItem)
	, fWritingItem(fBufferItem)
	, fReadBufferOffset(0)
	, fReadBufferMergedSize(0)
{
	fBufferItem[0].Next=nullptr;
	fBufferItem[0].WriteIndex=0;
	fBufferItem[0].Buffer=cAllocationOperator::Allocate(fBufferSize,0);

	fBufferItem[1].Buffer=cAllocationOperator::Allocate(fBufferSize,0);
	fBufferItem[2].Buffer=cAllocationOperator::Allocate(fBufferSize,0);
	fBufferItem[3].Buffer=cAllocationOperator::Allocate(fBufferSize,0);
	fBufferItem[1].Next=fBufferItem+2;
	fBufferItem[2].Next=fBufferItem+3;
	fBufferItem[3].Next=nullptr;
	fEmptyBufferStack.PushChain(fBufferItem+1,fBufferItem+3);
}
//---------------------------------------------------------------------------
cAsyncLoopbackStreamBuffer::~cAsyncLoopbackStreamBuffer()noexcept
{
	cAllocationOperator::Deallocate(fBufferItem[3].Buffer,fBufferSize,0);
	cAllocationOperator::Deallocate(fBufferItem[2].Buffer,fBufferSize,0);
	cAllocationOperator::Deallocate(fBufferItem[1].Buffer,fBufferSize,0);
	cAllocationOperator::Deallocate(fBufferItem[0].Buffer,fBufferSize,0);
}
//---------------------------------------------------------------------------
uIntn cAsyncLoopbackStreamBuffer::GetBufferSizeLimit(void)const noexcept
{
	return fBufferSize;
}
//---------------------------------------------------------------------------
//bool cAsyncLoopbackStreamBuffer::IsReadAvailable(void)const noexcept
//{
//	return fTotalBufferSize.Free.Load()!=0;
//	if(fReadingItem!=nullptr)
//		return true;
//	if(fReadyBufferItem!=nullptr)
//		return true;
//	return false;
//}
//---------------------------------------------------------------------------
cConstMemory cAsyncLoopbackStreamBuffer::GatherReadBuffer(uIntn QuerySize)noexcept
{
	cConstMemory ReadBuffer;

	uIntn SizeRemain=fReadingItem->WriteIndex-fReadBufferOffset;
	auto NextItem=fReadingItem->Next;
	if(QuerySize<=SizeRemain || NextItem==nullptr){
		ReadBuffer.Pointer=cnMemory::PointerAddByteOffset(fReadingItem->Buffer,fReadBufferOffset);
		ReadBuffer.Length=SizeRemain;
		fReadBufferMergedSize=0;
		return ReadBuffer;
	}
	// merge next buffer
	if(QuerySize>fBufferSize)
		QuerySize=fBufferSize;

	if(fReadBufferOffset!=0){
		cnMemory::Copy(fReadingItem->Buffer,cnMemory::PointerAddByteOffset(fReadingItem->Buffer,fReadBufferOffset),SizeRemain);
		fReadBufferOffset=0;
		fReadingItem->WriteIndex=SizeRemain;
	}
	uIntn SizeNeeded=QuerySize-fReadingItem->WriteIndex;
	while(NextItem->WriteIndex<SizeNeeded){
		uIntn NextSize=NextItem->WriteIndex;
		cnMemory::Copy(cnMemory::PointerAddByteOffset(fReadingItem->Buffer,fReadingItem->WriteIndex),NextItem->Buffer,NextSize);
		auto NextNext=NextItem->Next;
		if(NextNext==nullptr){
			ReadBuffer.Length=fReadingItem->WriteIndex+NextSize;
			ReadBuffer.Pointer=fReadingItem->Buffer;
			fReadBufferMergedSize=NextSize;
			return ReadBuffer;
		}
		fReadingItem->WriteIndex+=NextSize;
		fReadingItem->Next=NextNext;
		SizeNeeded-=NextSize;

		// recycle next item
		fEmptyBufferStack.Push(NextItem);
		NextItem=NextNext;
	}
	cnMemory::Copy(cnMemory::PointerAddByteOffset(fReadingItem->Buffer,fReadingItem->WriteIndex),NextItem->Buffer,SizeNeeded);
	ReadBuffer.Length=QuerySize;
	ReadBuffer.Pointer=fReadingItem->Buffer;
	fReadBufferMergedSize=SizeNeeded;
	return ReadBuffer;
}
//---------------------------------------------------------------------------
void cAsyncLoopbackStreamBuffer::DismissReadBuffer(uIntn Size)noexcept
{
	uIntn NewOffset=fReadBufferOffset+Size;
	if(NewOffset<fReadingItem->WriteIndex){
		fReadBufferOffset=NewOffset;
		return;
	}
	if(fReadingItem->Next==nullptr){
		fReadBufferOffset=fReadingItem->WriteIndex;
		return;
	}
	// recycle current read item
	auto RecycleItem=fReadingItem;
	fReadBufferOffset=NewOffset-fReadingItem->WriteIndex;
	if(fReadBufferOffset>fReadBufferMergedSize)
		fReadBufferOffset=fReadBufferMergedSize;

	fReadingItem=fReadingItem->Next;
	fEmptyBufferStack.Push(RecycleItem);
}
//---------------------------------------------------------------------------
cMemory cAsyncLoopbackStreamBuffer::ReserveWriteBuffer(uIntn QuerySize)noexcept
{
	if(QuerySize<=fBufferSize){
		if(fWritingItem->WriteIndex+QuerySize<fBufferSize){
			cMemory WriteBuffer;
			WriteBuffer.Pointer=cnMemory::PointerAddByteOffset(fWritingItem->Buffer,fWritingItem->WriteIndex);
			WriteBuffer.Length=fBufferSize-fWritingItem->WriteIndex;
			return WriteBuffer;
		}
		// not enough buffer remaining

		auto NextWritingItem=fEmptyBufferStack.Pop();
		if(NextWritingItem!=nullptr){
			// link writing item to next item
			NextWritingItem->WriteIndex=0;
			NextWritingItem->Next=nullptr;
			fWritingItem->Next=NextWritingItem;
			// switch to next item
			fWritingItem=NextWritingItem;
			// return all buffer
			cMemory WriteBuffer;
			WriteBuffer.Pointer=fWritingItem->Buffer;
			WriteBuffer.Length=fBufferSize;
			return WriteBuffer;
		}
		// no more buffer available
	}
	cMemory EmptyBuffer;
	EmptyBuffer.Pointer=nullptr;
	EmptyBuffer.Length=0;
	return EmptyBuffer;
}
//---------------------------------------------------------------------------
void cAsyncLoopbackStreamBuffer::CommitWriteBuffer(uIntn Size)noexcept
{
	if(Size!=0){
		uIntn NextWriteIndex=Size+fWritingItem->WriteIndex;
		if(NextWriteIndex>fBufferSize){
			fWritingItem->WriteIndex=fBufferSize;
		}
		else{
			fWritingItem->WriteIndex=NextWriteIndex;
		}
	}
}
//---------------------------------------------------------------------------
uIntn cAsyncLoopbackStreamBuffer::ReadTo(void *Buffer,uIntn Size)noexcept
{
	return ReadFromStream(Buffer,Size,this);
}
//---------------------------------------------------------------------------
uIntn cAsyncLoopbackStreamBuffer::WriteFrom(const void *Buffer,uIntn Size)noexcept
{
	uIntn Ret=WriteToStream(Buffer,Size,this);
	return Ret;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cStreamReadPackingBuffer::cStreamReadPackingBuffer()noexcept
{
	fDataStartIndex=0;
	fReadBuffer=nullptr;
	fReadBufferSize=0;
	fReadIndex=0;
}
//---------------------------------------------------------------------------
cStreamReadPackingBuffer::~cStreamReadPackingBuffer()noexcept
{
}
//---------------------------------------------------------------------------
void cStreamReadPackingBuffer::Clear(void)noexcept
{
	fDataBuffer.Clear();
	fDataStartIndex=0;

	fReadBuffer=nullptr;
	fReadBufferSize=0;
	fReadIndex=0;
}
//---------------------------------------------------------------------------
void cStreamReadPackingBuffer::StartBuffer(const void *Buffer,uIntn BufferSize)noexcept
{
	fReadBuffer=Buffer;
	fReadBufferSize=BufferSize;
	fReadIndex=0;
}
//---------------------------------------------------------------------------
uIntn cStreamReadPackingBuffer::FinishBuffer(void)noexcept
{
	auto SizeRead=fReadIndex;

	// remove unused head
	if(fDataStartIndex!=0){
		fDataBuffer.Replace(0,fDataStartIndex,0);
		fDataStartIndex=0;
	}

	// clear read buffer
	fReadBuffer=nullptr;
	fReadBufferSize=0;
	fReadIndex=0;

	return SizeRead;
}
//---------------------------------------------------------------------------
void cStreamReadPackingBuffer::QueueAllBuffer(void)noexcept
{
	// remove unused head
	if(fDataStartIndex!=0){
		fDataBuffer.Replace(0,fDataStartIndex,0);
		fDataStartIndex=0;
	}

	// append remaining buffer
	uIntn ReadSizeAvailable=fReadBufferSize-fReadIndex;
	if(ReadSizeAvailable!=0){
		auto AppendIndex=fDataBuffer->Length;
		fDataBuffer.Append(ReadSizeAvailable);
		cnMemory::Copy(fDataBuffer[AppendIndex],cnMemory::PointerAddByteOffset(fReadBuffer,fReadIndex),ReadSizeAvailable);
	}

	// clear read buffer
	fReadBuffer=nullptr;
	fReadBufferSize=0;
	fReadIndex=0;
}
//---------------------------------------------------------------------------
cConstMemory cStreamReadPackingBuffer::GatherReadBuffer(uIntn QuerySize)noexcept
{
	uIntn ReadSizeAvailable=fReadBufferSize-fReadIndex;
	auto PackedSize=fDataBuffer.GetSize();
	if(PackedSize!=0){
		PackedSize-=fDataStartIndex;
		if(PackedSize<QuerySize){
			// try pack buffer
			if(fReadBuffer!=nullptr){
				uIntn AppendSize=QuerySize-PackedSize;	// size needs to append
				if(AppendSize>ReadSizeAvailable){
					AppendSize=ReadSizeAvailable;
				}
				// append read buffer to packed buffer
				auto BufferIndex=fDataBuffer.GetSize();
				fDataBuffer.Append(AppendSize);
				cnMemory::Copy(fDataBuffer[BufferIndex],cnMemory::PointerAddByteOffset(fReadBuffer,fReadIndex),AppendSize);
				fReadIndex+=AppendSize;
			}
		}
		// return any available in packed buffer
		cConstMemory RetBuffer;
		RetBuffer.Pointer=fDataBuffer[fDataStartIndex];
		RetBuffer.Length=fDataBuffer.GetSize()-fDataStartIndex;
		return RetBuffer;
	}
	else{
		// return data in read buffer
		cConstMemory RetBuffer;
		RetBuffer.Pointer=cnMemory::PointerAddByteOffset(fReadBuffer,fReadIndex);
		RetBuffer.Length=ReadSizeAvailable;

		return RetBuffer;
	}
}
//---------------------------------------------------------------------------
void cStreamReadPackingBuffer::DismissReadBuffer(uIntn Size)noexcept
{
	uIntn AdvancReadBufferSize=Size;
	auto PackedSize=fDataBuffer.GetSize();
	if(PackedSize!=0){
		PackedSize-=fDataStartIndex;
		if(PackedSize>Size){
			// advance in packed buffer
			fDataStartIndex+=Size;
			return;
		}
		AdvancReadBufferSize-=PackedSize;
		// clear packed buffer
		fDataBuffer.Clear();
		fDataStartIndex=0;
	}
	// advance in read buffer
	uIntn SizeAvailable=fReadBufferSize-fReadIndex;
	if(AdvancReadBufferSize>=SizeAvailable){
		// all read buffer read
		fReadIndex=fReadBufferSize;
	}
	else{
		fReadIndex+=AdvancReadBufferSize;
	}
}
//---------------------------------------------------------------------------
