#include "WinCOMIO.h"

using namespace cnLibrary;
using namespace cnRTL;
using namespace cnWinRTL;

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cCOMSyncStream::cCOMSyncStream(COMPtr<IStream> Stream)
	: fStream(cnVar::MoveCast(Stream))
	, fIsEndOfReading(false)
	, fStreamStateCached(false)
{
	cnMemory::ZeroFill(fCachedState);
}
//---------------------------------------------------------------------------
cCOMSyncStream::~cCOMSyncStream()
{
}
//---------------------------------------------------------------------------
void cCOMSyncStream::Close(void)
{
	fStream=nullptr;
}
//---------------------------------------------------------------------------
void cCOMSyncStream::SetupState(void)
{
	if(fStreamStateCached){
		return;
	}

	HRESULT hr;
	hr=fStream->Stat(&fCachedState,STATFLAG_NONAME);
	if(FAILED(hr))
		return;

	fStreamStateCached=true;
}
//---------------------------------------------------------------------------
bool cCOMSyncStream::IsEndOfReading(void)
{
	if(fIsEndOfReading)
		return true;

	if(fStream==nullptr)
		return false;

	SetupState();

	HRESULT hr;
	union{
		LARGE_INTEGER Position;
		ULARGE_INTEGER NewPos;
	};
	Position.QuadPart=0;
	hr=fStream->Seek(Position,STREAM_SEEK_CUR,&NewPos);
	if(FAILED(hr))
		return false;

	if(NewPos.QuadPart<fCachedState.cbSize.QuadPart){
		return false;
	}
	fIsEndOfReading=true;
	return true;
}
//---------------------------------------------------------------------------
bool cCOMSyncStream::SetEndOfStream(void)
{
	if(fStream==nullptr)
		return false;

	HRESULT hr;
	union{
		LARGE_INTEGER Size;
		ULARGE_INTEGER uSize;
	};
	Size.QuadPart=0;
	hr=fStream->Seek(Size,STREAM_SEEK_CUR,&uSize);
	if(FAILED(hr))
		return false;

	hr=fStream->SetSize(uSize);
	if(FAILED(hr))
		return false;

	return true;
}
//---------------------------------------------------------------------------
bool cCOMSyncStream::Read(void *Buffer,uIntn Size,uIntn &SizeCompleted)
{
	SizeCompleted=0;
	if(fStream==nullptr)
		return false;

	HRESULT hr;
	ULONG SizeRead;
	hr=fStream->Read(Buffer,static_cast<ULONG>(Size),&SizeRead);
	if(FAILED(hr)){
		return false;
	}
	if(hr==S_FALSE){
		fIsEndOfReading=true;
	}
	SizeCompleted=SizeRead;
	return true;
}
//---------------------------------------------------------------------------
bool cCOMSyncStream::Write(const void *Buffer,uIntn Size,uIntn &SizeCompleted)
{
	SizeCompleted=0;
	if(fStream==nullptr)
		return false;

	HRESULT hr;
	ULONG SizeWritten;
	hr=fStream->Write(Buffer,static_cast<ULONG>(Size),&SizeWritten);
	if(FAILED(hr)){
		return false;
	}
	SizeCompleted=SizeWritten;
	return true;
}
//---------------------------------------------------------------------------
uInt64 cCOMSyncStream::GetPointer(void)
{
	if(fStream==nullptr)
		return 0;

	HRESULT hr;
	union{
		LARGE_INTEGER Position;
		ULARGE_INTEGER uPosition;
	};
	Position.QuadPart=0;
	hr=fStream->Seek(Position,STREAM_SEEK_CUR,&uPosition);
	if(FAILED(hr))
		return 0;

	return uPosition.QuadPart;
}
//---------------------------------------------------------------------------
bool cCOMSyncStream::MovePointerBegin(uInt64 Offset)
{
	if(fStream==nullptr)
		return false;

	HRESULT hr;
	LARGE_INTEGER Position;
	Position.QuadPart=Offset;
	hr=fStream->Seek(Position,STREAM_SEEK_SET,nullptr);
	if(FAILED(hr))
		return false;

	return true;
}
//---------------------------------------------------------------------------
bool cCOMSyncStream::MovePointerCurrent(sInt64 Offset)
{
	if(fStream==nullptr)
		return false;

	HRESULT hr;
	LARGE_INTEGER Position;
	Position.QuadPart=Offset;
	hr=fStream->Seek(Position,STREAM_SEEK_CUR,nullptr);
	if(FAILED(hr))
		return false;

	return true;
}
//---------------------------------------------------------------------------
bool cCOMSyncStream::MovePointerEnd(sInt64 Offset)
{
	if(fStream==nullptr)
		return false;

	HRESULT hr;
	LARGE_INTEGER Position;
	Position.QuadPart=Offset;
	hr=fStream->Seek(Position,STREAM_SEEK_END,nullptr);
	if(FAILED(hr))
		return false;

	return true;
}
//---------------------------------------------------------------------------
