#include "WinFileIO.h"

using namespace cnLibrary;
using namespace cnRTL;
using namespace cnWinRTL;


//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
HANDLE cnWinRTL::Win32FileOpenHandle(const wchar_t *FileName,eFileAccess AccessMode,eFileCreate CreateFlag,DWORD Flag)
{
	DWORD WinAccessFlag=0;
	if((AccessMode&FileAccess::Read)!=FileAccess::None)
		WinAccessFlag|=GENERIC_READ;
	if((AccessMode&FileAccess::Write)!=FileAccess::None)
		WinAccessFlag|=GENERIC_WRITE;
	DWORD WinShareFlag=0;
	if((AccessMode&FileAccess::ShareRead)!=FileAccess::None)
		WinShareFlag|=FILE_SHARE_READ;
	if((AccessMode&FileAccess::ShareWrite)!=FileAccess::None)
		WinShareFlag|=FILE_SHARE_WRITE;
	if((AccessMode&FileAccess::ShareDelete)!=FileAccess::None)
		WinShareFlag|=FILE_SHARE_DELETE;
	DWORD WinCreateDispostion;
	switch(CreateFlag){
	default:
	case FileCreate::Open:
		WinCreateDispostion=OPEN_EXISTING;
		break;
	case FileCreate::New:
		WinCreateDispostion=CREATE_NEW;
		break;
	case FileCreate::Create:
		WinCreateDispostion=OPEN_ALWAYS;
		break;
	}
	return ::CreateFileW(FileName,WinAccessFlag,WinShareFlag,NULL,WinCreateDispostion,Flag,NULL);
}
//---------------------------------------------------------------------------
uInt64 cnWinRTL::Win32FileGetSize(const wchar_t *FileName)
{
    WIN32_FILE_ATTRIBUTE_DATA AttributeData;
	if(::GetFileAttributesExW(FileName,GetFileExInfoStandard,&AttributeData)==FALSE)
		return 0;
    LARGE_INTEGER size;
    size.HighPart=AttributeData.nFileSizeHigh;
    size.LowPart=AttributeData.nFileSizeLow;
    return size.QuadPart;
}
//---------------------------------------------------------------------------
bool cnWinRTL::Win32FileIsExists(const wchar_t *FileName)
{
	DWORD fa=::GetFileAttributesW(FileName);
	if(fa==INVALID_FILE_ATTRIBUTES){
		// not exists
		return false;
	}
	return true;
}
//---------------------------------------------------------------------------
eStreamError cnWinRTL::Win32ErrorToStreamError(DWORD ErrorCode)
{
	switch(ErrorCode){
	default:
		return StreamError::Unknown;
	case ERROR_HANDLE_EOF:
		return StreamError::Success;
	case ERROR_CANCELLED:
		return StreamError::Cancelled;
	case ERROR_INVALID_HANDLE:
		return StreamError::Closed;
	}
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cWin32FileHandle::cWin32FileHandle()
	: Handle(INVALID_HANDLE_VALUE)
{
}
//---------------------------------------------------------------------------
cWin32FileHandle::cWin32FileHandle(HANDLE InitalHandle)
	: Handle(InitalHandle)
{
}
//---------------------------------------------------------------------------
cWin32FileHandle::~cWin32FileHandle()
{
	if(Handle!=INVALID_HANDLE_VALUE){
		::CloseHandle(Handle);
	}
}
//---------------------------------------------------------------------------
void cWin32FileHandle::TransferHandle(HANDLE NewHandle)
{
	if(Handle!=INVALID_HANDLE_VALUE){
		::CloseHandle(Handle);
	}
	Handle=NewHandle;
}
//---------------------------------------------------------------------------
void cWin32FileHandle::Close(void)
{
	TransferHandle(INVALID_HANDLE_VALUE);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cWin32FileMappingHandle::cWin32FileMappingHandle()
	: Handle(nullptr)
{
}
//---------------------------------------------------------------------------
cWin32FileMappingHandle::cWin32FileMappingHandle(HANDLE InitalHandle,aClsRef<cWin32FileHandle> InitialFileHandle)
	: Handle(InitalHandle)
	, FileHandle(cnVar::MoveCast(InitialFileHandle))
{
}
//---------------------------------------------------------------------------
cWin32FileMappingHandle::~cWin32FileMappingHandle()
{
	if(Handle!=nullptr){
		::CloseHandle(Handle);
	}
}
//---------------------------------------------------------------------------
void cWin32FileMappingHandle::TransferHandle(HANDLE NewHandle,aClsRef<cWin32FileHandle> NewFileHandle)
{
	if(Handle!=nullptr){
		::CloseHandle(Handle);
	}
	Handle=NewHandle;
	FileHandle=cnVar::MoveCast(NewFileHandle);
}
//---------------------------------------------------------------------------
void cWin32FileMappingHandle::Close(void)
{
	TransferHandle(nullptr,nullptr);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cWin32FileMapping::cWin32FileMapping(aClsRef<cWin32FileMappingHandle> Handle,uInt64 Size,DWORD MapFlag)
	: fFileMappingHandle(cnVar::MoveCast(Handle))
{
	fSize=Size;
	fMapFlag=MapFlag;
}
//---------------------------------------------------------------------------
cWin32FileMapping::~cWin32FileMapping()
{
}
//---------------------------------------------------------------------------
DWORD cWin32FileMapping::FileAccessToMapFlag(eFileAccess AccessMode)
{
	DWORD MapFlag=0;
	if((AccessMode&FileAccess::Read)!=FileAccess::None){
		MapFlag|=FILE_MAP_READ;
	}
	if((AccessMode&FileAccess::Write)!=FileAccess::None){
		MapFlag|=FILE_MAP_WRITE;
	}
	return MapFlag;
}
//---------------------------------------------------------------------------
uInt64 cWin32FileMapping::GetSize(void)
{
	return fSize;
}
//---------------------------------------------------------------------------
void* cWin32FileMapping::Map(uInt64 Offset,uIntn Size)
{

	return ::MapViewOfFile(fFileMappingHandle->Handle,fMapFlag,static_cast<DWORD>(Offset>>32),static_cast<DWORD>(Offset),Size);
}
//---------------------------------------------------------------------------
bool cWin32FileMapping::Unmap(void *Pointer,uIntn Size)
{	Size;
	return ::UnmapViewOfFile(Pointer)!=0;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cWin32FileHandleSequenceStream::cWin32FileHandleSequenceStream(aClsRef<cWin32FileHandle> FileHandle)
	: fFileHandle(cnVar::MoveCast(FileHandle))
{
	fCachedFilePos=false;
}
//---------------------------------------------------------------------------
cWin32FileHandleSequenceStream::~cWin32FileHandleSequenceStream()
{
}
//---------------------------------------------------------------------------
void* cWin32FileHandleSequenceStream::CastInterface(iTypeID InterfaceID)noexcept(true)
{
	return ImpCastInterface<iWinFileHandle,iStreamErrorReport>(this,InterfaceID);
}
//---------------------------------------------------------------------------
static StreamError ERROR_to_StreamError(DWORD Error){
	switch(Error){
	default:
		return StreamError::Unknown;
	case ERROR_SUCCESS:
		return StreamError::Success;
	case ERROR_TIMEOUT:
		return StreamError::Timeout;
	case ERROR_CANCELLED:
		return StreamError::Cancelled;
	case ERROR_INVALID_HANDLE:
		return StreamError::Closed;
	}
}
//---------------------------------------------------------------------------
eStreamError cWin32FileHandleSequenceStream::GetStreamError(void)
{
	if(fFileHandle->Handle==INVALID_HANDLE_VALUE)
		return StreamError::Closed;
	DWORD Error=::GetLastError();
	return ERROR_to_StreamError(Error);
}
//---------------------------------------------------------------------------
HANDLE cWin32FileHandleSequenceStream::GetFileHandle(void)
{
	return fFileHandle->Handle;
}
//---------------------------------------------------------------------------
void cWin32FileHandleSequenceStream::SetupFilePos(void)
{
	if(fCachedFilePos)
		return;

	LARGE_INTEGER NewFilePointer;
	NewFilePointer.QuadPart=0;
	if(::SetFilePointerEx(fFileHandle->Handle,NewFilePointer,&NewFilePointer,FILE_CURRENT)){
		fFilePos=NewFilePointer.QuadPart;
	}
	else{
		fFilePos=0;
	}

	fCachedFilePos=true;
}
//---------------------------------------------------------------------------
void cWin32FileHandleSequenceStream::Close(void)
{
	fFileHandle->Close();
}
//---------------------------------------------------------------------------
bool cWin32FileHandleSequenceStream::IsEndOfReading(void)
{
	if(fFileHandle->Handle==INVALID_HANDLE_VALUE)
		return false;
	SetupFilePos();
	LARGE_INTEGER FileSize;
	if(::GetFileSizeEx(fFileHandle->Handle,&FileSize)==FALSE){
		return false;
	}
	return fFilePos>=static_cast<uInt64>(FileSize.QuadPart);
}
//---------------------------------------------------------------------------
bool cWin32FileHandleSequenceStream::SetEndOfStream(void)
{
	if(fFileHandle->Handle==INVALID_HANDLE_VALUE)
		return false;
	if(::SetEndOfFile(fFileHandle->Handle)==FALSE){
		return false;
	}
	return true;
}
//---------------------------------------------------------------------------
bool cWin32FileHandleSequenceStream::Read(void *Buffer,uIntn Size,uIntn &SizeCompleted)
{
	SizeCompleted=0;
	if(fFileHandle->Handle==INVALID_HANDLE_VALUE)
		return false;
	fCachedFilePos=false;
	DWORD SizeRead;
	if(::ReadFile(fFileHandle->Handle,Buffer,static_cast<DWORD>(Size),&SizeRead,nullptr)==FALSE){
		return false;
	}
	SizeCompleted=SizeRead;
	return true;
}
//---------------------------------------------------------------------------
bool cWin32FileHandleSequenceStream::Write(const void *Buffer,uIntn Size,uIntn &SizeCompleted)
{
	SizeCompleted=0;
	if(fFileHandle->Handle==INVALID_HANDLE_VALUE)
		return false;
	fCachedFilePos=false;
	DWORD SizeWritten;
	if(::WriteFile(fFileHandle->Handle,Buffer,static_cast<DWORD>(Size),&SizeWritten,nullptr)==FALSE){
		return false;
	}
	SizeCompleted=SizeWritten;
	return true;
}
//---------------------------------------------------------------------------
uInt64 cWin32FileHandleSequenceStream::GetPointer(void)
{
	if(fFileHandle->Handle==INVALID_HANDLE_VALUE)
		return 0;
	SetupFilePos();
	return fFilePos;
}
//---------------------------------------------------------------------------
bool cWin32FileHandleSequenceStream::MovePointerBegin(uInt64 Offset)
{
	if(fFileHandle->Handle==INVALID_HANDLE_VALUE)
		return false;
	LARGE_INTEGER Dist;
	Dist.QuadPart=Offset;
	if(::SetFilePointerEx(fFileHandle->Handle,Dist,&Dist,FILE_BEGIN)){
		fFilePos=Dist.QuadPart;
		fCachedFilePos=true;
		return true;
	}
	return false;
}
//---------------------------------------------------------------------------
bool cWin32FileHandleSequenceStream::MovePointerCurrent(sInt64 Offset)
{
	if(fFileHandle->Handle==INVALID_HANDLE_VALUE)
		return false;
	LARGE_INTEGER Dist;
	Dist.QuadPart=Offset;
	if(::SetFilePointerEx(fFileHandle->Handle,Dist,&Dist,FILE_CURRENT)){
		fFilePos=Dist.QuadPart;
		fCachedFilePos=true;
		return true;
	}
	return false;
}
//---------------------------------------------------------------------------
bool cWin32FileHandleSequenceStream::MovePointerEnd(sInt64 Offset)
{
	if(fFileHandle->Handle==INVALID_HANDLE_VALUE)
		return false;
	LARGE_INTEGER Dist;
	Dist.QuadPart=Offset;
	if(::SetFilePointerEx(fFileHandle->Handle,Dist,&Dist,FILE_END)){
		fFilePos=Dist.QuadPart;
		fCachedFilePos=true;
		return true;
	}
	return false;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
bcNTFileOverlappedIOHandle::bcNTFileOverlappedIOHandle()
	: Handle(INVALID_HANDLE_VALUE)
{
}
//---------------------------------------------------------------------------
bcNTFileOverlappedIOHandle::~bcNTFileOverlappedIOHandle()
{
	cnLib_ASSERT(Handle==INVALID_HANDLE_VALUE);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cNTFileOverlappedIOHandleSyncIOObject::cNTFileOverlappedIOHandleSyncIOObject()
{
}
//---------------------------------------------------------------------------
cNTFileOverlappedIOHandleSyncIOObject::~cNTFileOverlappedIOHandleSyncIOObject()
{
}
//---------------------------------------------------------------------------
void cNTFileOverlappedIOHandleSyncIOObject::Completed(void)
{
	fNotifier.Notify();
}
//---------------------------------------------------------------------------
bool cNTFileOverlappedIOHandleSyncIOObject::Read(bcNTFileOverlappedIOHandle *FileIO,void *Buffer,uIntn Size)
{
	if(FileIO->Handle==INVALID_HANDLE_VALUE)
		return false;

	BytesCompleted=0;

	FileIO->StartIO();

	if(::ReadFile(FileIO->Handle,Buffer,static_cast<DWORD>(Size),&BytesCompleted,&Overlapped)){
		// wait for notification
		fNotifier.Wait();
		return true;
	}
	
	DWORD ReadErrorCode=::GetLastError();
	switch(ReadErrorCode){
	case ERROR_IO_PENDING:
		// wait for notification
		fNotifier.Wait();
		return true;
	default:
		// error
		FileIO->CancelIO();
		ErrorCode=ReadErrorCode;
		break;
	}
	return false;
}
//---------------------------------------------------------------------------
bool cNTFileOverlappedIOHandleSyncIOObject::Write(bcNTFileOverlappedIOHandle *FileIO,const void *Buffer,uIntn Size)
{
	if(FileIO->Handle==INVALID_HANDLE_VALUE)
		return false;

	BytesCompleted=0;

	FileIO->StartIO();

	if(::WriteFile(FileIO->Handle,Buffer,static_cast<DWORD>(Size),&BytesCompleted,&Overlapped)){
		// wait for notification
		fNotifier.Wait();
		return true;
	}

	DWORD WriteErrorCode=::GetLastError();
	switch(WriteErrorCode){
	case ERROR_IO_PENDING:
		// wait for notification
		fNotifier.Wait();
		return true;
	default:
		// error
		FileIO->CancelIO();
		ErrorCode=WriteErrorCode;
		break;
	}
	return false;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cNTFileOverlappedIOHandleAsyncIOTask::cNTFileOverlappedIOHandleAsyncIOTask()
{
}
//---------------------------------------------------------------------------
cNTFileOverlappedIOHandleAsyncIOTask::~cNTFileOverlappedIOHandleAsyncIOTask()
{
}
//---------------------------------------------------------------------------
void* cNTFileOverlappedIOHandleAsyncIOTask::CastInterface(iTypeID InterfaceID)noexcept(true)
{
	return ImpCastInterface<iStreamErrorReport>(this,InterfaceID);
}
//---------------------------------------------------------------------------
void cNTFileOverlappedIOHandleAsyncIOTask::Completed(void)
{
	fTaskState.SetDone();
	iDecReference(this,'task');
}
//---------------------------------------------------------------------------
bool cNTFileOverlappedIOHandleAsyncIOTask::IsDone(void)
{
	return fTaskState.IsDone();
}
//---------------------------------------------------------------------------
bool cNTFileOverlappedIOHandleAsyncIOTask::SetNotify(iProcedure *NotifyProcedure)
{
	return fTaskState.SetNotify(NotifyProcedure);
}
//---------------------------------------------------------------------------
void cNTFileOverlappedIOHandleAsyncIOTask::Cancel(void)
{
	fFileIO->CancelOperation(this);
}
//---------------------------------------------------------------------------
bool cNTFileOverlappedIOHandleAsyncIOTask::GetResult(uIntn &retSizeCompleted)
{
	retSizeCompleted=BytesCompleted;
	return ErrorCode==ERROR_SUCCESS;
}
//---------------------------------------------------------------------------
eStreamError cNTFileOverlappedIOHandleAsyncIOTask::GetStreamError(void)
{
	return Win32ErrorToStreamError(ErrorCode);
}
//---------------------------------------------------------------------------
void cNTFileOverlappedIOHandleAsyncIOTask::SetupOverlapped(void)
{
	cnMemory::ZeroFill(Overlapped);
}
//---------------------------------------------------------------------------
void cNTFileOverlappedIOHandleAsyncIOTask::SetupOverlapped(uInt64 Offset)
{
	Overlapped.hEvent=nullptr;
	Overlapped.Internal=0;
	Overlapped.InternalHigh=0;
	Overlapped.Offset=static_cast<DWORD>(Offset);
	Overlapped.OffsetHigh=static_cast<DWORD>(Offset>>32);
}
//---------------------------------------------------------------------------
bool cNTFileOverlappedIOHandleAsyncIOTask::Read(bcNTFileOverlappedIOHandle *FileIO,void *Buffer,uIntn Size)
{
	if(FileIO->Handle==INVALID_HANDLE_VALUE)
		return false;

	fFileIO=cnVar::MoveCast(FileIO);
	BytesCompleted=0;

	iIncReference(this,'task');

	fFileIO->StartIO();

	if(::ReadFile(fFileIO->Handle,Buffer,static_cast<DWORD>(Size),&BytesCompleted,&Overlapped)){
		// wait for notification
		return true;
	}
	DWORD ReadErrorCode=::GetLastError();
	switch(ReadErrorCode){
	case ERROR_IO_PENDING:
		// wait for notification
		return true;
	default:
		// error
		fFileIO->CancelIO();
		ErrorCode=ReadErrorCode;
		break;
	}
	fFileIO=nullptr;
	iDecReference(this,'task');
	return false;
}
//---------------------------------------------------------------------------
bool cNTFileOverlappedIOHandleAsyncIOTask::Write(bcNTFileOverlappedIOHandle *FileIO,const void *Buffer,uIntn Size)
{
	if(FileIO->Handle==INVALID_HANDLE_VALUE)
		return false;

	fFileIO=cnVar::MoveCast(FileIO);
	BytesCompleted=0;

	iIncReference(this,'task');

	fFileIO->StartIO();

	if(::WriteFile(fFileIO->Handle,Buffer,static_cast<DWORD>(Size),&BytesCompleted,&Overlapped)){
		// wait for notification
		return true;
	}
	
	DWORD WriteErrorCode=::GetLastError();
	switch(WriteErrorCode){
	case ERROR_IO_PENDING:
		// wait for notification
		return true;
	default:
		// error
		fFileIO->CancelIO();
		ErrorCode=WriteErrorCode;
		break;
	}
	fFileIO=nullptr;
	iDecReference(this,'task');
	return false;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cNTFileOverlappedIOHandleStream::cNTFileOverlappedIOHandleStream(rPtr<bcNTFileOverlappedIOHandle> FileIO)
	: fFileIO(cnVar::MoveCast(FileIO))
{
	fEOF=false;
}
//---------------------------------------------------------------------------
cNTFileOverlappedIOHandleStream::~cNTFileOverlappedIOHandleStream()
{
}
//---------------------------------------------------------------------------
void* cNTFileOverlappedIOHandleStream::CastInterface(iTypeID InterfaceID)noexcept(true)
{
	return iWinFileHandle::CastInterface(InterfaceID);
}
//---------------------------------------------------------------------------
HANDLE cNTFileOverlappedIOHandleStream::GetFileHandle(void)
{
	return fFileIO->Handle;
}
//---------------------------------------------------------------------------
void cNTFileOverlappedIOHandleStream::Close(void)
{
	fFileIO->Close();
}
//---------------------------------------------------------------------------
bool cNTFileOverlappedIOHandleStream::IsEndOfReading(void)
{
	return fEOF;
}
//---------------------------------------------------------------------------
bool cNTFileOverlappedIOHandleStream::SetEndOfStream(void)
{
	return ::SetEndOfFile(fFileIO->Handle)!=FALSE;
}
//---------------------------------------------------------------------------
bool cNTFileOverlappedIOHandleStream::Read(void *Buffer,uIntn Size,uIntn &SizeCompleted)
{
	cNTFileOverlappedIOHandleSyncIOObject IOObject;

	cnMemory::ZeroFill(IOObject.Overlapped);
	IOObject.BytesCompleted=0;

	if(IOObject.Read(fFileIO,Buffer,Size)==false)
		return false;

	SizeCompleted=IOObject.BytesCompleted;
	if(IOObject.ErrorCode==ERROR_HANDLE_EOF){
		fEOF=true;
		return true;
	}

	return IOObject.ErrorCode==ERROR_SUCCESS;
}
//---------------------------------------------------------------------------
bool cNTFileOverlappedIOHandleStream::Write(const void *Buffer,uIntn Size,uIntn &SizeCompleted)
{
	cNTFileOverlappedIOHandleSyncIOObject IOObject;

	cnMemory::ZeroFill(IOObject.Overlapped);
	IOObject.BytesCompleted=0;

	if(IOObject.Write(fFileIO,Buffer,Size)==false)
		return false;

	SizeCompleted=IOObject.BytesCompleted;
	return IOObject.ErrorCode==ERROR_SUCCESS;
}
//---------------------------------------------------------------------------
void cNTFileOverlappedIOHandleStream::cReadTask::Completed(void)
{
	if(ErrorCode==ERROR_HANDLE_EOF){
		Host->fEOF=true;
	}
	Host=nullptr;

	cNTFileOverlappedIOHandleAsyncIOTask::Completed();
}
//---------------------------------------------------------------------------
iPtr<iStreamTask> cNTFileOverlappedIOHandleStream::ReadAsync(void *Buffer,uIntn Size)
{
	auto Task=iCreate<cReadTask>();
	Task->Host=this;
	Task->SetupOverlapped();
	if(Task->Read(fFileIO,Buffer,Size)==false)
		return nullptr;

	return Task;
}
//---------------------------------------------------------------------------
iPtr<iStreamTask> cNTFileOverlappedIOHandleStream::WriteAsync(const void *Buffer,uIntn Size)
{
	auto Task=iCreate<cNTFileOverlappedIOHandleAsyncIOTask>();
	Task->SetupOverlapped();
	if(Task->Write(fFileIO,Buffer,Size)==false)
		return nullptr;

	return Task;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cNTFileOverlappedIOHandleFileStream::cNTFileOverlappedIOHandleFileStream(rPtr<bcNTFileOverlappedIOHandle> FileIO)
	: fFileIO(cnVar::MoveCast(FileIO))
{
}
//---------------------------------------------------------------------------
cNTFileOverlappedIOHandleFileStream::~cNTFileOverlappedIOHandleFileStream()
{
	if(fFileIO->Handle!=INVALID_HANDLE_VALUE)
		fFileIO->Close();
}
//---------------------------------------------------------------------------
void* cNTFileOverlappedIOHandleFileStream::CastInterface(iTypeID InterfaceID)noexcept(true)
{
	return ImpCastInterface<iWinFileHandle,iStreamErrorReport>(this,InterfaceID);
}
//---------------------------------------------------------------------------
eStreamError cNTFileOverlappedIOHandleFileStream::GetStreamError(void)
{
	DWORD Error=::GetLastError();
	return Win32ErrorToStreamError(Error);
}
//---------------------------------------------------------------------------
HANDLE cNTFileOverlappedIOHandleFileStream::GetFileHandle(void)
{
	return fFileIO->Handle;
}
//---------------------------------------------------------------------------
void cNTFileOverlappedIOHandleFileStream::Close(void)
{
	if(fFileIO->Handle!=INVALID_HANDLE_VALUE)
		fFileIO->Close();
}
//---------------------------------------------------------------------------
bool cNTFileOverlappedIOHandleFileStream::Read(void *Buffer,uIntn Size,uInt64 Offset,uIntn &SizeCompleted)
{
	cNTFileOverlappedIOHandleSyncIOObject IOObject;

	IOObject.Overlapped.hEvent=nullptr;
	IOObject.Overlapped.Internal=0;
	IOObject.Overlapped.InternalHigh=0;
	IOObject.Overlapped.Offset=static_cast<DWORD>(Offset);
	IOObject.Overlapped.OffsetHigh=static_cast<DWORD>(Offset>>32);

	
	if(IOObject.Read(fFileIO,Buffer,Size)==false){
		return false;
	}

	SizeCompleted=IOObject.BytesCompleted;
	if(IOObject.ErrorCode==ERROR_HANDLE_EOF){
		return true;
	}
	return IOObject.ErrorCode==ERROR_SUCCESS;
}
//---------------------------------------------------------------------------
bool cNTFileOverlappedIOHandleFileStream::Write(const void *Buffer,uIntn Size,uInt64 Offset,uIntn &SizeCompleted)
{
	cNTFileOverlappedIOHandleSyncIOObject IOObject;

	IOObject.Overlapped.hEvent=nullptr;
	IOObject.Overlapped.Internal=0;
	IOObject.Overlapped.InternalHigh=0;
	IOObject.Overlapped.Offset=static_cast<DWORD>(Offset);
	IOObject.Overlapped.OffsetHigh=static_cast<DWORD>(Offset>>32);

	if(IOObject.Write(fFileIO,Buffer,Size)==false){
		return false;
	}

	SizeCompleted=IOObject.BytesCompleted;
	return IOObject.ErrorCode==ERROR_SUCCESS;
}
//---------------------------------------------------------------------------
iPtr<iStreamTask> cNTFileOverlappedIOHandleFileStream::ReadAsync(void *Buffer,uIntn Size,uInt64 Offset)
{
	auto Task=iCreate<cNTFileOverlappedIOHandleAsyncIOTask>();
	Task->SetupOverlapped(Offset);
	if(Task->Read(fFileIO,Buffer,Size)==false)
		return nullptr;

	return Task;
}
//---------------------------------------------------------------------------
iPtr<iStreamTask> cNTFileOverlappedIOHandleFileStream::WriteAsync(const void *Buffer,uIntn Size,uInt64 Offset)
{
	auto Task=iCreate<cNTFileOverlappedIOHandleAsyncIOTask>();
	Task->SetupOverlapped(Offset);
	if(Task->Write(fFileIO,Buffer,Size)==false)
		return nullptr;

	return Task;

}
//---------------------------------------------------------------------------
bool cNTFileOverlappedIOHandleFileStream::GetSize(uInt64 &Size)
{
	LARGE_INTEGER CurFileSize;
	if(::GetFileSizeEx(fFileIO->Handle,&CurFileSize)==false)
		return false;
	Size=CurFileSize.QuadPart;
	return true;
}
//---------------------------------------------------------------------------
bool cNTFileOverlappedIOHandleFileStream::SetSize(uInt64 NewSize)
{
	LARGE_INTEGER NewFileSize;
	NewFileSize.QuadPart=NewSize;

	if(::SetFilePointerEx(fFileIO->Handle,NewFileSize,NULL,FILE_BEGIN)==FALSE){
		return false;
	}
	if(::SetEndOfFile(fFileIO->Handle)==FALSE){
		return false;
	}
	return true;
}
//---------------------------------------------------------------------------
bool cNTFileOverlappedIOHandleFileStream::FlushBuffer(void)
{
	return ::FlushFileBuffers(fFileIO->Handle)!=FALSE;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cNTFolderOverlappedIOHandleFileWatcher::cNTFolderOverlappedIOHandleFileWatcher(rPtr<bcNTFileOverlappedIOHandle> FolderIO)
	: fFolderIO(cnVar::MoveCast(FolderIO))
{
}
//---------------------------------------------------------------------------
cNTFolderOverlappedIOHandleFileWatcher::~cNTFolderOverlappedIOHandleFileWatcher()
{
	fFolderIO->Close();
}
//---------------------------------------------------------------------------
void cNTFolderOverlappedIOHandleFileWatcher::VirtualStopped(void)
{
	CloseQueue();

	InnerDecReference('self');
}
//---------------------------------------------------------------------------
iReference* cNTFolderOverlappedIOHandleFileWatcher::NotificationInnerReference(void)
{
	return &fInnerReference;
}
//---------------------------------------------------------------------------
void cNTFolderOverlappedIOHandleFileWatcher::NotificationStarted(void)
{
	bcAsyncNotification::NotificationStarted();

	ContinueRead();
}
//---------------------------------------------------------------------------
cNTFolderOverlappedIOHandleFileWatcher::CycleState cNTFolderOverlappedIOHandleFileWatcher::NotificationCheckState(void)
{
	return CycleState::Normal;
}
//---------------------------------------------------------------------------
void cNTFolderOverlappedIOHandleFileWatcher::AsyncQueueNotify(void)
{
	ContinueRead();
	
	bcAsyncNotification::AsyncQueueNotify();
}
//---------------------------------------------------------------------------
void cNTFolderOverlappedIOHandleFileWatcher::ContinueRead(void)
{
	if(fReadChangeFlag.Acquire()==false)
		return;

	do{
		fReadChangeFlag.Continue();

		if(IsNotificationClosed())
			continue;

		if(fReadInProgress==false){
			if(fDirectoryReadChangeObject.Read(fFolderIO)){
				InnerIncReference('noti');
				fReadInProgress=true;
			}
			else{
				CloseQueue();
			}
		}

	}while(fReadChangeFlag.Release()==false);
}
//---------------------------------------------------------------------------
cNTFolderOverlappedIOHandleFileWatcher* cNTFolderOverlappedIOHandleFileWatcher::cDirectoryReadChangeObject::GetHost(void)
{
	return cnMemory::GetObjectFromMemberPointer(this,&cNTFolderOverlappedIOHandleFileWatcher::fDirectoryReadChangeObject);
}
//---------------------------------------------------------------------------
bool cNTFolderOverlappedIOHandleFileWatcher::cDirectoryReadChangeObject::Read(bcNTFileOverlappedIOHandle *FolderIO)
{
	cnMemory::ZeroFill(Overlapped);

	FolderIO->StartIO();
	if(::ReadDirectoryChangesW(FolderIO->Handle,ReadChangeBuffer,sizeof(ReadChangeBuffer),FALSE,FILE_NOTIFY_CHANGE_FILE_NAME|FILE_NOTIFY_CHANGE_DIR_NAME|FILE_NOTIFY_CHANGE_ATTRIBUTES|FILE_NOTIFY_CHANGE_SIZE|FILE_NOTIFY_CHANGE_LAST_WRITE,&BytesCompleted,&Overlapped,nullptr)){
		// queued
		return true;
	}

	ErrorCode=::GetLastError();

	FolderIO->CancelIO();

	return false;
}
//---------------------------------------------------------------------------
void cNTFolderOverlappedIOHandleFileWatcher::cDirectoryReadChangeObject::Completed(void)
{
	auto Host=cnMemory::GetObjectFromMemberPointer(this,&cNTFolderOverlappedIOHandleFileWatcher::fDirectoryReadChangeObject);
	return Host->DirectoryReadCompleted();
}
//---------------------------------------------------------------------------
void cNTFolderOverlappedIOHandleFileWatcher::DirectoryReadCompleted(void)
{
	fReadInProgress=false;
	if(fDirectoryReadChangeObject.ErrorCode==ERROR_SUCCESS){
		AsyncQueueSetAvailable(false);
	}
	else{
		// error
		CloseQueue();
	}

	InnerDecReference('noti');
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cNTFolderOverlappedIOHandleFileObserver::cNTFolderOverlappedIOHandleFileObserver(rPtr<bcNTFileOverlappedIOHandle> FolderIO,cnRTL::cString<wchar_t> Path,iPtr<iFileName> (*CreateFileName)(cString<wchar_t> Path))
	: fFolderIO(cnVar::MoveCast(FolderIO))
	, fPath(cnVar::MoveCast(Path))
	, fFindPath(Win32FileMakeFindName(fPath,fPath->Length,L"*"))
	, fCallback(nullptr)
	, fRestartTrack(true)
	, fCreateFileName(CreateFileName)
{
	fDirectoryReadChangeObject.ReadChangeBuffer=cnSystem::DefaultHeap::Alloc(fDirectoryReadChangeObject.ReadChangeBufferSize,sizeof(DWORD));
}
//---------------------------------------------------------------------------
cNTFolderOverlappedIOHandleFileObserver::~cNTFolderOverlappedIOHandleFileObserver()
{
	cnSystem::DefaultHeap::Free(fDirectoryReadChangeObject.ReadChangeBuffer,fDirectoryReadChangeObject.ReadChangeBufferSize,sizeof(DWORD));

	fFolderIO->Close();

	auto Items=fPendingChangeStack.Swap(nullptr);
	while(Items!=nullptr){
		auto DeleteItem=Items;
		Items=Items->Next;
		delete DeleteItem;
	}
}
//---------------------------------------------------------------------------
void cNTFolderOverlappedIOHandleFileObserver::VirtualStopped(void)
{
	CloseQueue();

	InnerDecReference('self');
}
//---------------------------------------------------------------------------
bool cNTFolderOverlappedIOHandleFileObserver::StartNotify(iReference *Reference,iAsyncNotificationCallback *Callback)
{
	if(PrepareStartNotify()==false)
		return false;

	fCallbackReference=Reference;
	fCallback=Callback;

	ConfirmStartNotify();
	UpdateQueueState(false);
	return true;
}
//---------------------------------------------------------------------------
void cNTFolderOverlappedIOHandleFileObserver::StopNotify(void)
{
	if(bcAsyncQueue::StopNotify()){
		UpdateQueueState(false);
	}
}
//---------------------------------------------------------------------------
void cNTFolderOverlappedIOHandleFileObserver::NotifyCallback(bool IdleNotify)
{
	NotifyQueue(IdleNotify);
}
//---------------------------------------------------------------------------
bool cNTFolderOverlappedIOHandleFileObserver::IsClosed(void)
{
	return IsNotificationClosed();
}
//---------------------------------------------------------------------------
iReference* cNTFolderOverlappedIOHandleFileObserver::NotificationInnerReference(void)
{
	return &fInnerReference;
}
//---------------------------------------------------------------------------
void cNTFolderOverlappedIOHandleFileObserver::NotificationStarted(void)
{
	fCallback->AsyncStarted();
}
//---------------------------------------------------------------------------
void cNTFolderOverlappedIOHandleFileObserver::NotificationStopped(void)
{
	auto Reference=cnVar::MoveCast(fCallbackReference);
	auto Callback=fCallback;
	fCallback=nullptr;
	Callback->AsyncStopped();
}
//---------------------------------------------------------------------------
cNTFolderOverlappedIOHandleFileObserver::CycleState cNTFolderOverlappedIOHandleFileObserver::NotificationCheckState(void)
{
	return CycleState::Normal;
}
//---------------------------------------------------------------------------
void cNTFolderOverlappedIOHandleFileObserver::AsyncQueueNotify(void)
{
	fCallback->AsyncNotify();
}
//---------------------------------------------------------------------------
bool cNTFolderOverlappedIOHandleFileObserver::IsDot(const WIN32_FIND_DATA &fd)
{
	if((fd.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY)==0)
		return false;
	
	if(fd.cAlternateFileName[0]!=0)
		return false;
	return Win32FileNameCheckIsDot(fd.cFileName);
}
//---------------------------------------------------------------------------
bool cNTFolderOverlappedIOHandleFileObserver::IsInvalidFile(const WIN32_FIND_DATA &fd)
{
	if(IsDot(fd))
		return true;
	return false;
}
//---------------------------------------------------------------------------
cnRTL::cString<wchar_t> cNTFolderOverlappedIOHandleFileObserver::MakeFileName(const WIN32_FIND_DATA &fd)
{
	// create file name
	auto pFileName=wtou(fd.cFileName);
	return Win32FileMakeName(fPath,fPath.GetLength(),&pFileName,1);
}
//---------------------------------------------------------------------------
void cNTFolderOverlappedIOHandleFileObserver::RefreshFileMap(void)
{
	if(fRestartTrack){
		ContinueRead();
		// replace current file list
		ReloadFileMap();
	}
	else{
		// apply changes to list
		CommitPendingChanges(nullptr);
	}
}
//---------------------------------------------------------------------------
void cNTFolderOverlappedIOHandleFileObserver::DiscardChanges(void)
{
	RefreshFileMap();
}
//---------------------------------------------------------------------------
rPtr<iFileEnumerator> cNTFolderOverlappedIOHandleFileObserver::EnumCurrentFiles(void)
{
	RefreshFileMap();
	auto Enum=rCreate<cFileEnum>();
	Enum->CreateFileName=fCreateFileName;
	Enum->FolderPath=fPath;

	for(auto p : fFileMap){
		auto Item=Enum->FileList.Append();
		Item->Name=p.Key;
	}
	
	return Enum;
}
//---------------------------------------------------------------------------
rPtr<iFileChangeEnumerator> cNTFolderOverlappedIOHandleFileObserver::FetchFileChange(void)
{
	if(fRestartTrack){
		ContinueRead();
		// compare whole file list to get changes
		return CompareFileMap();
	}
	else{
		// apply changes to list and create enumeration
		auto ChangeEnum=rCreate<cFileChangeEnum>();
		ChangeEnum->CreateFileName=fCreateFileName;
		CommitPendingChanges(ChangeEnum);
		return ChangeEnum;
	}
}
//---------------------------------------------------------------------------
void cNTFolderOverlappedIOHandleFileObserver::AssignFileInfo(cFileInfo &Info,const WIN32_FIND_DATAW &fd)
{
	Info.CreationTime=fd.ftCreationTime;
	Info.FileAttributes=fd.dwFileAttributes;
	Info.FileSize=fd.nFileSizeHigh;
	Info.FileSize<<=32;
	Info.FileSize+=fd.nFileSizeLow;
	Info.LastAccessTime=fd.ftLastAccessTime;
	Info.LastWriteTime=fd.ftLastWriteTime;
}
//---------------------------------------------------------------------------
void cNTFolderOverlappedIOHandleFileObserver::SetupFileInfo(cFileInfo &Info,const wchar_t *FileName)
{
	auto FilePath=fPath;
	FilePath.Append(L"\\");
	FilePath.Append(FileName);
	
	Info.FileAttributes=::GetFileAttributesW(FilePath);
	WIN32_FIND_DATAW fd;
	HANDLE FindHandle=::FindFirstFileW(FilePath,&fd);
	if(FindHandle==INVALID_HANDLE_VALUE){
		Info.FileSize=0;
		Info.FileAttributes=INVALID_FILE_ATTRIBUTES;
	}
	else{
		AssignFileInfo(Info,fd);
		::FindClose(FindHandle);
	}
}
//---------------------------------------------------------------------------
void cNTFolderOverlappedIOHandleFileObserver::ReloadFileMap(void)
{
	WIN32_FIND_DATAW fd;

	HANDLE FindHandle=::FindFirstFileW(fFindPath,&fd);
	if(FindHandle==INVALID_HANDLE_VALUE){
		// error enumerate files
		return;
	}

	do{
		auto &FileItem=fFileMap[fd.cFileName];
		AssignFileInfo(FileItem,fd);
	}while(::FindNextFileW(FindHandle,&fd));

	::FindClose(FindHandle);
}
//---------------------------------------------------------------------------
rPtr<iFileChangeEnumerator> cNTFolderOverlappedIOHandleFileObserver::CompareFileMap(void)
{
	auto ChangeEnum=rCreate<cFileChangeEnum>();
	ChangeEnum->CreateFileName=fCreateFileName;
	ChangeEnum->OldFiles=cnVar::MoveCast(fFileMap);
	WIN32_FIND_DATAW fd;

	HANDLE FindHandle=::FindFirstFileW(fFindPath,&fd);
	if(FindHandle==INVALID_HANDLE_VALUE){
		// error enumerate files
		return nullptr;
	}

	do{
		auto OldFileItemPair=ChangeEnum->OldFiles.GetPair(fd.cFileName);
		if(OldFileItemPair!=nullptr){
			// exists
			auto &FileItem=fFileMap[OldFileItemPair->Key];
			AssignFileInfo(FileItem,fd);

			bool Changed=false;
			if(Changed){
				auto ChangeItem=ChangeEnum->ChangeList.Append();
				ChangeItem->Name=OldFileItemPair->Key;
				ChangeItem->Change=FileChange::Update;
			}
		
			ChangeEnum->OldFiles.RemovePair(OldFileItemPair);
		}
		else{
			// new file
			auto ChangeItem=ChangeEnum->ChangeList.Append();
			ChangeItem->Name=fd.cFileName;
			ChangeItem->Change=FileChange::Insert;

			auto &FileItem=fFileMap[ChangeItem->Name];
			AssignFileInfo(FileItem,fd);

		}

	}while(::FindNextFileW(FindHandle,&fd));

	::FindClose(FindHandle);

	return ChangeEnum;
}
//---------------------------------------------------------------------------
void cNTFolderOverlappedIOHandleFileObserver::ContinueRead(void)
{
	if(fReadChangeFlag.Acquire()==false)
		return;

	do{
		fReadChangeFlag.Continue();

		if(IsNotificationClosed())
			continue;

		if(fReadInProgress==false){
			if(fDirectoryReadChangeObject.Read(fFolderIO)){
				InnerIncReference('noti');
				fReadInProgress=true;
			}
			else{
				DirectoryReadError();
			}
		}

	}while(fReadChangeFlag.Release()==false);
}
//---------------------------------------------------------------------------
cNTFolderOverlappedIOHandleFileObserver* cNTFolderOverlappedIOHandleFileObserver::cDirectoryReadChangeObject::GetHost(void)
{
	return cnMemory::GetObjectFromMemberPointer(this,&cNTFolderOverlappedIOHandleFileObserver::fDirectoryReadChangeObject);
}
//---------------------------------------------------------------------------
bool cNTFolderOverlappedIOHandleFileObserver::cDirectoryReadChangeObject::Read(bcNTFileOverlappedIOHandle *FolderIO)
{
	cnMemory::ZeroFill(Overlapped);

	FolderIO->StartIO();
	if(::ReadDirectoryChangesW(FolderIO->Handle,ReadChangeBuffer,ReadChangeBufferSize,FALSE,FILE_NOTIFY_CHANGE_FILE_NAME|FILE_NOTIFY_CHANGE_DIR_NAME|FILE_NOTIFY_CHANGE_ATTRIBUTES|FILE_NOTIFY_CHANGE_SIZE|FILE_NOTIFY_CHANGE_LAST_WRITE,&BytesCompleted,&Overlapped,nullptr)){
		// queued
		return true;
	}

	ErrorCode=::GetLastError();

	FolderIO->CancelIO();

	return false;
}
//---------------------------------------------------------------------------
void cNTFolderOverlappedIOHandleFileObserver::cDirectoryReadChangeObject::Completed(void)
{
	auto Host=cnMemory::GetObjectFromMemberPointer(this,&cNTFolderOverlappedIOHandleFileObserver::fDirectoryReadChangeObject);
	return Host->DirectoryReadCompleted();
}
//---------------------------------------------------------------------------
void cNTFolderOverlappedIOHandleFileObserver::DirectoryReadCompleted(void)
{
	fReadInProgress=false;
	if(fDirectoryReadChangeObject.ErrorCode==ERROR_SUCCESS){
		if(fDirectoryReadChangeObject.BytesCompleted==0){
			// not enough buffer, lost track of changes
			fRestartTrack=true;
			AsyncQueueSetAvailable(false);
		}
		else{
			// parse change info
			DirectoryReadParse();
		}

		if(IsNotificationClosed()==false){
			ContinueRead();
		}
	}
	else{
		DirectoryReadError();
	}

	InnerDecReference('noti');
}
//---------------------------------------------------------------------------
void cNTFolderOverlappedIOHandleFileObserver::DirectoryReadError(void)
{
	// cannot track of changes
	fRestartTrack=true;
}
//---------------------------------------------------------------------------
void cNTFolderOverlappedIOHandleFileObserver::DirectoryReadParse(void)
{
	DWORD CurOffset=0;
	bool AnyUpdate=false;
	auto DirNotifyInfo=static_cast<FILE_NOTIFY_INFORMATION*>(fDirectoryReadChangeObject.ReadChangeBuffer);
	while(DirNotifyInfo->NextEntryOffset!=0){
		cnLib_ASSERT(DirNotifyInfo->NextEntryOffset>=sizeof(FILE_NOTIFY_INFORMATION));
		CurOffset+=DirNotifyInfo->NextEntryOffset;
		if(CurOffset<fDirectoryReadChangeObject.BytesCompleted){
			break;
		}

		auto ChangeItem=new cPendingChangeItem;
		switch(DirNotifyInfo->Action){
		case FILE_ACTION_ADDED:
			ChangeItem->Change=FileChange::Insert;
			break;
		case FILE_ACTION_REMOVED:
			ChangeItem->Change=FileChange::Remove;
			break;
		default:
		case FILE_ACTION_MODIFIED:
			ChangeItem->Change=FileChange::Update;
			break;
		case FILE_ACTION_RENAMED_OLD_NAME:
			ChangeItem->Change=FileChange::RenameOld;
			break;
		case FILE_ACTION_RENAMED_NEW_NAME:
			ChangeItem->Change=FileChange::RenameNew;
			break;
		}
		ChangeItem->Name.SetString(DirNotifyInfo->FileName,DirNotifyInfo->FileNameLength);

		fPendingChangeStack.Push(ChangeItem);

		DirNotifyInfo=cnMemory::PointerAddByteOffset(DirNotifyInfo,DirNotifyInfo->NextEntryOffset);
		AnyUpdate=true;
	}
	if(AnyUpdate){
		AsyncQueueSetAvailable(false);
	}
}
//---------------------------------------------------------------------------
void cNTFolderOverlappedIOHandleFileObserver::CommitPendingChanges(cFileChangeEnum *ChangeEnum)
{
	auto ChangeList=fPendingChangeStack.Swap(nullptr);
	if(ChangeList==nullptr)
		return;

	// reverse
	cPendingChangeItem *Next=nullptr;
	do{
		cPendingChangeItem *ItemPrev=ChangeList->Next;

		ChangeList->Next=Next;
		Next=ChangeList;

		ChangeList=ItemPrev;
	}while(ChangeList!=nullptr);
	
	do{
		ChangeList=Next;
		Next=ChangeList->Next;

		switch(ChangeList->Change){
		case FileChange::Insert:
		case FileChange::RenameNew:
		case FileChange::Update:
			{
				auto FileItem=fFileMap[ChangeList->Name];
				SetupFileInfo(FileItem,ChangeList->Name);
			}
			break;
		case FileChange::Remove:
		case FileChange::RenameOld:
			{
				fFileMap.Remove(ChangeList->Name);
			}
			break;
		}
		
		if(ChangeEnum!=nullptr){
			auto Item=ChangeEnum->ChangeList.Append();
			Item->Change=ChangeList->Change;
			Item->Name=cnVar::MoveCast(ChangeList->Name);
		}
		delete ChangeList;

	}while(Next!=nullptr);
}
//---------------------------------------------------------------------------
bool cNTFolderOverlappedIOHandleFileObserver::cFileEnum::Fetch(void)
{
	uIntn NextIndex=EnumIndex+1;
	if(NextIndex<FileList.GetCount()){
		EnumIndex=NextIndex;
		return true;
	}
	return false;
}
//---------------------------------------------------------------------------
iFile* cNTFolderOverlappedIOHandleFileObserver::cFileEnum::GetCurrentFile(void)
{
	if(CurrentFile==nullptr){
		auto &CurFile=FileList[EnumIndex];
		cnRTL::cStringBuffer<wchar_t> PathBuffer=FolderPath.GetArray();
		if(Win32FileNameStreamAppend(PathBuffer.StreamWriteBuffer(),CurFile.Name)!=0){
			CurrentFile=(*CreateFileName)(cnVar::MoveCast(PathBuffer));
		}
	}
	return CurrentFile;
}
//---------------------------------------------------------------------------
bool cNTFolderOverlappedIOHandleFileObserver::cFileChangeEnum::Fetch(void)
{
	switch(EnumPart){
	default:
		return false;
	case 0:	// removed
		{
			uIntn NextIndex=EnumIndex+1;
			if(NextIndex<OldFiles.GetCount()){
				EnumIndex=NextIndex;
				return true;
			}
		}
		EnumIndex=cnVar::TIntegerValue<uIntn>::Max;
		EnumPart=1;
		cnLib_SWITCH_FALLTHROUGH;
	case 1:	// changed
		{
			uIntn NextIndex=EnumIndex+1;
			if(NextIndex<ChangeList.GetCount()){
				EnumIndex=NextIndex;
				return true;
			}
		}
		EnumPart=2;
		return false;
	}
}
//---------------------------------------------------------------------------
iFile* cNTFolderOverlappedIOHandleFileObserver::cFileChangeEnum::GetCurrentFile(void)
{
	if(CurrentFile==nullptr){
		switch(EnumPart){
		default:
			return nullptr;
		case 0:	// removed
			{
				auto CurFile=OldFiles.GetPairAt(EnumIndex);
				cnRTL::cStringBuffer<wchar_t> PathBuffer=FolderPath.GetArray();
				if(Win32FileNameStreamAppend(PathBuffer.StreamWriteBuffer(),CurFile->Key)!=0){
					CurrentFile=(*CreateFileName)(cnVar::MoveCast(PathBuffer));
				}
			}
		case 1:	// changed
			{
				auto &CurFile=ChangeList[EnumIndex];
				cnRTL::cStringBuffer<wchar_t> PathBuffer=FolderPath.GetArray();
				if(Win32FileNameStreamAppend(PathBuffer.StreamWriteBuffer(),CurFile.Name)!=0){
					CurrentFile=(*CreateFileName)(cnVar::MoveCast(PathBuffer));
				}
			}
		}
	}
	return CurrentFile;
}
//---------------------------------------------------------------------------
eFileChange cNTFolderOverlappedIOHandleFileObserver::cFileChangeEnum::GetChange(void)
{
	switch(EnumPart){
	default:
		return FileChange::Update;
	case 0:	// removed
		return FileChange::Remove;
	case 1:	// changed
		{
			auto CurFile=ChangeList[EnumIndex];
			return CurFile.Change;
		}
	}
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
#if _WIN32_WINNT >= _WIN32_WINNT_VISTA
//---------------------------------------------------------------------------
cNT6ThreadPoolFileIOHandle::cNT6ThreadPoolFileIOHandle()
	: fIOThreadPool(nullptr)
{
}
//---------------------------------------------------------------------------
cNT6ThreadPoolFileIOHandle::~cNT6ThreadPoolFileIOHandle()
{
	cnLib_ASSERT(Handle==INVALID_HANDLE_VALUE);
}
//---------------------------------------------------------------------------
bool cNT6ThreadPoolFileIOHandle::Open(HANDLE FileHandle,PTP_CALLBACK_ENVIRON Environment)
{
	fIOThreadPool=::CreateThreadpoolIo(FileHandle,IoCompletionCallback,this,Environment);
	if(fIOThreadPool==nullptr)
		return false;

	Handle=FileHandle;
	return true;
}
//---------------------------------------------------------------------------
void cNT6ThreadPoolFileIOHandle::Close(void)
{
	cnLib_ASSERT(Handle!=INVALID_HANDLE_VALUE);

	::CloseThreadpoolIo(fIOThreadPool);
	::CloseHandle(Handle);
	Handle=INVALID_HANDLE_VALUE;

}
//---------------------------------------------------------------------------
void cNT6ThreadPoolFileIOHandle::StartIO(void)
{
	::StartThreadpoolIo(fIOThreadPool);
}
//---------------------------------------------------------------------------
void cNT6ThreadPoolFileIOHandle::CancelIO(void)
{
	::CancelThreadpoolIo(fIOThreadPool);
}
//---------------------------------------------------------------------------
void cNT6ThreadPoolFileIOHandle::CancelOperation(bcIOObject *Object)
{
	::CancelIoEx(Handle,&Object->Overlapped);
}
//---------------------------------------------------------------------------
VOID CALLBACK cNT6ThreadPoolFileIOHandle::IoCompletionCallback(
	_Inout_     PTP_CALLBACK_INSTANCE Instance,
	_Inout_opt_ PVOID                 Context,
	_Inout_opt_ PVOID                 Overlapped,
	_In_        ULONG                 IoResult,
	_In_        ULONG_PTR             NumberOfBytesTransferred,
	_Inout_     PTP_IO                Io
){
Instance,Context,Io;
	auto IOObject=cnMemory::GetObjectFromMemberPointer(static_cast<OVERLAPPED*>(Overlapped),&bcIOObject::Overlapped);
	IOObject->ErrorCode=IoResult;
	IOObject->BytesCompleted=static_cast<DWORD>(NumberOfBytesTransferred);
	IOObject->Completed();
}
//---------------------------------------------------------------------------
#endif	// _WIN32_WINNT_VISTA
//---------------------------------------------------------------------------
