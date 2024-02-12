/*- cnRTL - Stream Buffer -------------------------------------------------*/
/*         Developer : Code009                                             */
/*         Create on : 2010-03-31                                          */
/*-------------------------------------------------------------------------*/
#ifndef	__cnLibrary_cnRTL_StreamBuffer_H__
#define	__cnLibrary_cnRTL_StreamBuffer_H__
/*-------------------------------------------------------------------------*/
#include <cnRTL/cnRTLHeader.h>
#include <cnRTL/RuntimeFunction.h>
/*-------------------------------------------------------------------------*/
#ifdef __cplusplus
//---------------------------------------------------------------------------
namespace cnLibrary{
//---------------------------------------------------------------------------
namespace cnRTL{
//---------------------------------------------------------------------------
// replaced by cStreamReadPackingBuffer
class cnLib_DEPRECATED cMemoryBufferQueueReadBuffer
{
public:
	cMemoryBufferQueueReadBuffer();
	~cMemoryBufferQueueReadBuffer();

	void Clear(void);

	void StartBuffer(const void *Buffer,uIntn BufferSize);
	uIntn FinishBuffer(void);
	void QueueAllBuffer(void);

	cConstMemory PackReadBuffer(uIntn QuerySize);
	bool PackReadData(cConstMemory &Buffer,uIntn Size);

	bool QueryReadBuffer(cConstMemory &Buffer,uIntn QuerySize);
	void CompleteReadBuffer(uIntn Size);
private:
	cMemoryBuffer fDataBuffer;
	uIntn fDataStartIndex;

	const void *fReadBuffer;
	uIntn fReadBufferSize;
	uIntn fReadIndex;
};
//---------------------------------------------------------------------------
template<class TStreamReadBuffer>
class impReadBufferFromStreamBuffer : public iReadBuffer<typename cnVar::TRemoveConst<typename TStreamReadBuffer::tElement>::Type>
{
public:
	impReadBufferFromStreamBuffer(TStreamReadBuffer *ReadBuffer):fReadBuffer(ReadBuffer){}

	virtual cArray<typename TStreamReadBuffer::tElement const> cnLib_FUNC GatherReadBuffer(uIntn Length)override{
		return fReadBuffer->GatherReadBuffer(Length);
	}
	virtual void cnLib_FUNC DismissReadBuffer(uIntn Length)override{
		return fReadBuffer->DismissReadBuffer(Length);
	}
private:
	TStreamReadBuffer *fReadBuffer;
};
//---------------------------------------------------------------------------
template<class TStreamWriteBuffer>
class impWriteBufferFromStreamBuffer : public iWriteBuffer<typename TStreamWriteBuffer::tElement>
{
public:
	impWriteBufferFromStreamBuffer(TStreamWriteBuffer *WriteBuffer):fWriteBuffer(WriteBuffer){}

	virtual cArray<typename TStreamWriteBuffer::tElement> cnLib_FUNC ReserveWriteBuffer(uIntn Length)override{
		return fWriteBuffer->ReserveWriteBuffer(Length);
	}
	virtual void cnLib_FUNC CommitWriteBuffer(uIntn Length)override{
		return fWriteBuffer->CommitWriteBuffer(Length);
	}
private:
	TStreamWriteBuffer *fWriteBuffer;
};
//---------------------------------------------------------------------------
template<class TStreamReadBuffer>
impReadBufferFromStreamBuffer<TStreamReadBuffer> ReadBufferFromStreamBuffer(TStreamReadBuffer *ReadBuffer)
{
	return ReadBuffer;
}
template<class TStreamWriteBuffer>
impWriteBufferFromStreamBuffer<TStreamWriteBuffer> WriteBufferFromStreamBuffer(TStreamWriteBuffer *WriteBuffer)
{
	return WriteBuffer;
}
//---------------------------------------------------------------------------

#if 0
//---------------------------------------------------------------------------
template<class TStreamWriteBuffer>
uIntn WriteMemory(TStreamWriteBuffer &Buffer,typename TStreamWriteBuffer::tElement const *Src,uIntn Length)
{
	if(Buffer==nullptr || Src==nullptr || Length==0)
		return 0;
	uIntn TotalLengthWritten=0;
	
	cArray<typename TStreamWriteBuffer::tElement> CurBuffer;
	while((CurBuffer=Buffer.ReserveWriteBuffer(Length)).Length!=0){
		if(CurBuffer.Length>=Length){
			TKRuntime::TArray<sizeof(<typename TStreamWriteBuffer::tElement)>::Copy(CurBuffer.Pointer,Src,Length);
			Buffer.CommitWriteBuffer(Length);
			return TotalLengthWritten+Length;
		}
		// copy from src
		TKRuntime::TArray<sizeof(<typename TStreamWriteBuffer::tElement)>::Copy(CurBuffer.Pointer,Src,CurBuffer.Length);
		// offset src pointer
		Length-=CurBuffer.Length;
		Src=ArrayOffsetPointer(Src,CurBuffer.Length);
		TotalLengthWritten+=CurBuffer.Length;
		// next block
		Buffer.CommitWriteBuffer(CurBuffer.Length);
	}
	// read end
	return TotalLengthWritten;
}
//---------------------------------------------------------------------------
#if cnLibrary_CPPFEATURE_RVALUE_REFERENCES >= 200610L
template<class TStreamWriteBuffer>
uIntn WriteMemory(TStreamWriteBuffer &&Buffer,typename TStreamWriteBuffer::tElement const *Src,uIntn Length)
{
	return WriteMemory(Buffer,Src,Length);
}
#endif	// cnLibrary_CPPFEATURE_RVALUE_REFERENCES >= 200610L
//---------------------------------------------------------------------------
template<class TStreamReadBuffer>
uIntn ReadMemory(TStreamReadBuffer &Buffer,typename TStreamReadBuffer::tElement *Dest,uIntn Length)
{
	if(Buffer==nullptr || Dest==nullptr || Length==0)
		return 0;
	uIntn TotalLengthRead=0;

	cArray<typename TStreamReadBuffer::tElement const> CurBuffer;
	while((CurBuffer=Buffer.GatherReadBuffer(Length)).Length!=0){
		if(CurBuffer.Length>=Length){
			TKRuntime::TArray<sizeof(<typename TStreamWriteBuffer::tElement)>::Copy(Dest,CurBuffer.Pointer,Length);
			Buffer.DismissReadBuffer(Length);
			return TotalLengthRead+Length;
		}
		// copy to dest
		TKRuntime::TArray<sizeof(<typename TStreamWriteBuffer::tElement)>::Copy(Dest,CurBuffer.Pointer,CurBuffer.Length);
		// offset dest pointer
		Length-=CurBuffer.Length;
		Dest=ArrayOffsetPointer(Dest,CurBuffer.Length);
		TotalLengthRead+=CurBuffer.Length;
		// next block
		Buffer.DismissReadBuffer(CurBuffer.Length);
	}
	return TotalLengthRead;
}
//---------------------------------------------------------------------------
#if cnLibrary_CPPFEATURE_RVALUE_REFERENCES >= 200610L
template<class TStreamReadBuffer>
uIntn ReadMemory(TStreamReadBuffer &&Buffer,typename TStreamReadBuffer::tElement const *Src,uIntn Length)
{
	return ReadMemory(Buffer,Src,Length);
}
#endif	// cnLibrary_CPPFEATURE_RVALUE_REFERENCES >= 200610L

#endif // 0

template<class TStreamWriteBuffer>
uIntn WriteToStream(typename TStreamWriteBuffer::tElement const *Src,uIntn Length,TStreamWriteBuffer *Buffer)
{
	if(Buffer==nullptr || Src==nullptr || Length==0)
		return 0;
	uIntn TotalLengthWritten=0;
	
	cArray<typename TStreamWriteBuffer::tElement> CurBuffer;
	while((CurBuffer=Buffer->ReserveWriteBuffer(Length)).Length!=0){
		if(CurBuffer.Length>=Length){
			cnMemory::Copy(CurBuffer.Pointer,Src,Length);
			Buffer->CommitWriteBuffer(Length);
			return TotalLengthWritten+Length;
		}
		// copy from src
		cnMemory::Copy(CurBuffer.Pointer,Src,CurBuffer.Length);
		// offset src pointer
		Length-=CurBuffer.Length;
		Src=ArrayOffsetPointer(Src,CurBuffer.Length);
		TotalLengthWritten+=CurBuffer.Length;
		// next block
		Buffer->CommitWriteBuffer(CurBuffer.Length);
	}
	// read end
	return TotalLengthWritten;
}

template<class TStreamReadBuffer>
uIntn ReadFromStream(typename TStreamReadBuffer::tElement *Dest,uIntn Length,TStreamReadBuffer *Buffer)
{
	if(Buffer==nullptr || Dest==nullptr || Length==0)
		return 0;
	uIntn TotalLengthRead=0;

	cArray<typename TStreamReadBuffer::tElement const> CurBuffer;
	while((CurBuffer=Buffer->GatherReadBuffer(Length)).Length!=0){
		if(CurBuffer.Length>=Length){
			cnMemory::Copy(Dest,CurBuffer.Pointer,Length);
			Buffer->DismissReadBuffer(Length);
			return TotalLengthRead+Length;
		}
		// copy to dest
		cnMemory::Copy(Dest,CurBuffer.Pointer,CurBuffer.Length);
		// offset dest pointer
		Length-=CurBuffer.Length;
		Dest=ArrayOffsetPointer(Dest,CurBuffer.Length);
		TotalLengthRead+=CurBuffer.Length;
		// next block
		Buffer->DismissReadBuffer(CurBuffer.Length);
	}
	return TotalLengthRead;
}

//---------------------------------------------------------------------------
#if 1
// replaced by cStreamReadPackingBuffer
class cnLib_DEPRECATED cBufferPacker
{
public:
	cBufferPacker();
	~cBufferPacker();

	void PutBuffer(const void *Buffer,uIntn BufferSize);

	const void *PackedData(void)const;
	template<class T>	const T *PackedData(void)const{	return static_cast<const T*>(PackedData());	}
	uIntn PackedSize(void)const;


	const void *PendingData(void)const;
	template<class T>	const T *PendingData(void)const{	return static_cast<const T*>(PendingData());	}
	uIntn PendingSize(void)const;

	// Pack
	//	link up data in sequence
	bool Pack(uIntn Size);


	// Advance
	//	discard data in sequence
	uIntn Advance(uIntn Size);
	void Clear(void);
private:
	cMemoryBuffer fDataBuffer;
	union{
		uIntn fDataStartIndex;
		uIntn fPendingPackedSize;
	};

	const void *fPendingBuffer;
	uIntn fPendingBufferSize;
};
#endif // 0
//---------------------------------------------------------------------------
class cLoopbackStreamBuffer
{
public:
	typedef void TElement;

	cLoopbackStreamBuffer();
	~cLoopbackStreamBuffer();

	cMemory GetReadBuffer(void)const;
	cConstMemory GatherReadBuffer(uIntn Size);
	void DismissReadBuffer(uIntn Size);
	cMemory ReserveWriteBuffer(uIntn Size);
	void CommitWriteBuffer(uIntn Size);

private:
	cMemoryBuffer fBuffer;
	uIntn fReadIndex;
};
//---------------------------------------------------------------------------
class cAsyncLoopbackStreamBuffer
{
public:
	typedef void tElement;

	cAsyncLoopbackStreamBuffer();
	~cAsyncLoopbackStreamBuffer();

	uIntn BufferSizeLimit=cnVar::TIntegerValue<uIntn>::Max;

	bool IsReadAvailable(void)const;

	cConstMemory GatherReadBuffer(uIntn Size);
	void DismissReadBuffer(uIntn Size);

	cMemory ReserveWriteBuffer(uIntn Size);
	void CommitWriteBuffer(uIntn Size);

	uIntn ReadTo(void *Buffer,uIntn Size);
	uIntn WriteFrom(const void *Buffer,uIntn Size);

protected:
	struct cBufferItem
	{
		cBufferItem *Next;
		cMemoryBuffer Buffer;
	}fBufferItem[3];
	cAtomicStack<cBufferItem> fEmptyBufferItem;
	cAtomicVar<cBufferItem*> fReadyBufferItem;
	cBufferItem *fReadingItem;
	uIntn fReadBufferOffset;
	cBufferItem *fWritingItem;
};
//---------------------------------------------------------------------------
class cStreamReadPackingBuffer
{
public:
	typedef void TElement;
	cStreamReadPackingBuffer();
	~cStreamReadPackingBuffer();

	void Clear(void);

	void StartBuffer(const void *Buffer,uIntn BufferSize);
	uIntn FinishBuffer(void);
	void QueueAllBuffer(void);

	cConstMemory GatherReadBuffer(uIntn QuerySize);
	void DismissReadBuffer(uIntn Size);
private:
	cMemoryBuffer fDataBuffer;
	uIntn fDataStartIndex;

	const void *fReadBuffer;
	uIntn fReadBufferSize;
	uIntn fReadIndex;
};
//---------------------------------------------------------------------------
}	// namespace cnRTL
//---------------------------------------------------------------------------
}	// namespace cnLibrary
//---------------------------------------------------------------------------
#endif  /* __cplusplus */
/*-------------------------------------------------------------------------*/
#endif
