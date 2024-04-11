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
cAsyncLoopbackStreamBuffer::cAsyncLoopbackStreamBuffer()noexcept
	: fReadyBufferItem(nullptr)
	, fReadingItem(nullptr)
	, fWritingItem(nullptr)
{
	fBufferItem[0].Next=fBufferItem+1;
	fBufferItem[1].Next=fBufferItem+2;
	fBufferItem[2].Next=nullptr;
	fEmptyBufferItem.PushChain(fBufferItem,fBufferItem+2);
}
//---------------------------------------------------------------------------
cAsyncLoopbackStreamBuffer::~cAsyncLoopbackStreamBuffer()noexcept
{
}
//---------------------------------------------------------------------------
bool cAsyncLoopbackStreamBuffer::IsReadAvailable(void)const noexcept
{
	if(fReadingItem!=nullptr)
		return true;
	if(fReadyBufferItem!=nullptr)
		return true;
	return false;
}
//---------------------------------------------------------------------------
cConstMemory cAsyncLoopbackStreamBuffer::GatherReadBuffer(uIntn)noexcept
{
	if(fReadingItem==nullptr){
		// try to acquire read item
		fReadingItem=fReadyBufferItem.Acquire.Xchg(nullptr);
		if(fReadingItem==nullptr){
			cConstMemory EmptyBuffer;
			EmptyBuffer.Pointer=nullptr;
			EmptyBuffer.Length=0;
			return EmptyBuffer;
		}
		fReadBufferOffset=0;
	}

	cConstMemory ReadBuffer;

	auto &DataBuffer=fReadingItem->Buffer;
	ReadBuffer.Pointer=DataBuffer[fReadBufferOffset];
	ReadBuffer.Length=DataBuffer.GetSize()-fReadBufferOffset;
	return ReadBuffer;
}
//---------------------------------------------------------------------------
void cAsyncLoopbackStreamBuffer::DismissReadBuffer(uIntn Size)noexcept
{
	if(fReadingItem==nullptr)
		return;

	auto &DataBuffer=fReadingItem->Buffer;
	uIntn RemianSize=DataBuffer.GetSize()-fReadBufferOffset;
	if(Size<RemianSize){
		fReadBufferOffset+=Size;
		return;
	}

	// recycle empty buffer
	fReadingItem->Buffer.Clear();
	fEmptyBufferItem.Push(fReadingItem);
	fReadingItem=nullptr;
}
//---------------------------------------------------------------------------
cMemory cAsyncLoopbackStreamBuffer::ReserveWriteBuffer(uIntn QuerySize)noexcept
{
	if(fWritingItem==nullptr){
		// try to acquire free buffer
		fWritingItem=fEmptyBufferItem.Pop();
		if(fWritingItem==nullptr){
			cMemory EmptyBuffer;
			EmptyBuffer.Pointer=nullptr;
			EmptyBuffer.Length=0;
			return EmptyBuffer;
		}
	}

	cMemory WriteBuffer;
	auto &DataBuffer=fWritingItem->Buffer;
	uIntn WriteOffset=DataBuffer.GetSize();
	if(WriteOffset>BufferSizeLimit){
		// publish buffer
		if(fReadyBufferItem.Release.CmpStore(nullptr,fWritingItem)){
			fWritingItem=nullptr;
		}
		return NullArray;
	}
	if(QuerySize<128)
		QuerySize=128;
	WriteBuffer.Length=DataBuffer.GetCapacity()-WriteOffset;
	if(WriteBuffer.Length<QuerySize){
		DataBuffer.SetCapacity(WriteOffset+QuerySize);
		WriteBuffer.Length=DataBuffer.GetCapacity()-WriteOffset;
	}
	WriteBuffer.Pointer=DataBuffer[WriteOffset];
	return WriteBuffer;
}
//---------------------------------------------------------------------------
void cAsyncLoopbackStreamBuffer::CommitWriteBuffer(uIntn Size)noexcept
{
	if(fWritingItem==nullptr || Size==0)
		return;

	auto &DataBuffer=fWritingItem->Buffer;
	uIntn WriteOffset=DataBuffer.GetSize();
	uIntn WriteCapacity=DataBuffer.GetCapacity();
	if(Size+WriteOffset>WriteCapacity){
		Size=WriteCapacity-WriteOffset;
		DataBuffer.SetSize(WriteCapacity);
	}
	else{
		DataBuffer.SetSize(Size+WriteOffset);
	}
	if(WriteOffset!=0){
		// publish buffer
		if(fReadyBufferItem.Release.CmpStore(nullptr,fWritingItem)){
			fWritingItem=nullptr;
		}

	}
	else{
		cBufferItem *MergeItem=nullptr;
		if(fReadyBufferItem.Release.CmpXchg(MergeItem,fWritingItem)){
			fWritingItem=nullptr;
		}
		else{
			// merge to ready buffer
			auto &MergeBuffer=MergeItem->Buffer;
			// merge data
			uIntn MergeOffset=MergeBuffer.GetSize();
			uIntn MergeSize=MergeOffset+Size;
			if(MergeSize<BufferSizeLimit){
				MergeBuffer.SetSize(MergeSize);
				cnMemory::Copy(MergeBuffer[MergeOffset],DataBuffer->Pointer,Size);

				DataBuffer.SetSize(0);
			}

			// return buffer
			fReadyBufferItem.Release.Store(MergeItem);
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
	return WriteToStream(Buffer,Size,this);
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
