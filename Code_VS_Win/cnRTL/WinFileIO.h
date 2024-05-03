/*- cnRTL - Windows - File IO ---------------------------------------------*/
/*         Developer : Code009                                             */
/*         Create on : 2022-01-20                                          */
/*-------------------------------------------------------------------------*/
#pragma once
/*-------------------------------------------------------------------------*/
#include <cnRTL\WinCommon.h>

#include <cnRTL\AsyncTask.h>
#include <cnRTL\ThreadSynchronization.h>
#include <cnRTL\WinThread.h>
#include <cnRTL\WinFileName.h>

#ifdef __cplusplus
//---------------------------------------------------------------------------
namespace cnLibrary{
//---------------------------------------------------------------------------
namespace cnRTL{
//---------------------------------------------------------------------------
namespace cnWinRTL{
//---------------------------------------------------------------------------
HANDLE Win32FileOpenHandle(const wchar_t *FileName,eFileAccess AccessMode,eFileCreate CreateFlag,DWORD Flag)noexcept(true);
uInt64 Win32FileGetSize(const wchar_t *FileName)noexcept(true);
bool Win32FileIsExists(const wchar_t *FileName)noexcept(true);

//---------------------------------------------------------------------------
eStreamError Win32ErrorToStreamError(DWORD ErrorCode)noexcept(true);
//---------------------------------------------------------------------------
struct cWin32FileHandle
{
	HANDLE Handle;

	cWin32FileHandle()noexcept(true);
	cWin32FileHandle(HANDLE InitalHandle)noexcept(true);
	~cWin32FileHandle()noexcept(true);

	void TransferHandle(HANDLE NewHandle)noexcept(true);
	void Close(void)noexcept(true);
};
//---------------------------------------------------------------------------
struct cWin32FileMappingHandle
{
	HANDLE Handle;
	aClsRef<cWin32FileHandle> FileHandle;

	cWin32FileMappingHandle()noexcept(true);
	cWin32FileMappingHandle(HANDLE InitalHandle,aClsRef<cWin32FileHandle> InitialFileHandle)noexcept(true);
	~cWin32FileMappingHandle()noexcept(true);

	void TransferHandle(HANDLE InitalHandle,aClsRef<cWin32FileHandle> InitialFileHandle)noexcept(true);
	void Close(void)noexcept(true);
};
//---------------------------------------------------------------------------
class cWin32FileMapping : public iSwapMemory
{
public:
	cWin32FileMapping(aClsRef<cWin32FileMappingHandle> Handle,uInt64 Size,DWORD MapFlag)noexcept(true);
	~cWin32FileMapping()noexcept(true);

	static DWORD FileAccessToMapFlag(eFileAccess AccessMode)noexcept(true);

	virtual uInt64	cnLib_FUNC GetSize(void)noexcept(true)override;
	virtual void*	cnLib_FUNC Map(uInt64 Offset,uIntn Size)noexcept(true)override;
	virtual bool	cnLib_FUNC Unmap(void *Address,uIntn Size)noexcept(true)override;
protected:
	aClsRef<cWin32FileMappingHandle> fFileMappingHandle;
	uInt64 fSize;
	DWORD fMapFlag;
};
//---------------------------------------------------------------------------
class cWin32FileHandleSequenceStream : public iFileSyncStream, public iStreamErrorReport, public iWinFileHandle
{
public:
	cWin32FileHandleSequenceStream(aClsRef<cWin32FileHandle> FileHandle)noexcept(true);
	~cWin32FileHandleSequenceStream()noexcept(true);

	virtual void* cnLib_FUNC CastInterface(iTypeID InterfaceID)noexcept(true) override;

	virtual HANDLE	cnLib_FUNC GetFileHandle(void)noexcept(true)override;

	virtual void cnLib_FUNC Close(void)noexcept(true)override;
	virtual bool cnLib_FUNC IsEndOfReading(void)noexcept(true)override;
	virtual bool cnLib_FUNC SetEndOfStream(void)noexcept(true)override;
	virtual bool cnLib_FUNC Read(void *Buffer,uIntn Size,uIntn &SizeCompleted)noexcept(true)override;
	virtual bool cnLib_FUNC Write(const void *Buffer,uIntn Size,uIntn &SizeCompleted)noexcept(true)override;

	virtual uInt64 cnLib_FUNC GetPointer(void)noexcept(true)override;
	virtual bool cnLib_FUNC MovePointerBegin(uInt64 Offset)noexcept(true)override;
	virtual bool cnLib_FUNC MovePointerCurrent(sInt64 Offset)noexcept(true)override;
	virtual bool cnLib_FUNC MovePointerEnd(sInt64 Offset)noexcept(true)override;

	virtual eStreamError cnLib_FUNC GetStreamError(void)noexcept(true)override;
protected:
	aClsRef<cWin32FileHandle> fFileHandle;

	bool fCachedFilePos;

	uInt64 fFilePos;

	void SetupFilePos(void)noexcept(true);
};
//---------------------------------------------------------------------------
class bcNTFileOverlappedIOHandle : public iReference
{
public:
	bcNTFileOverlappedIOHandle()noexcept(true);
	~bcNTFileOverlappedIOHandle()noexcept(true);

	HANDLE Handle;
	
	class bcIOObject
	{
	public:
		OVERLAPPED Overlapped;
		DWORD BytesCompleted;
		DWORD ErrorCode;

		virtual void Completed(void)noexcept(true)=0;
	};
	virtual void Close(void)noexcept(true)=0;
	virtual void StartIO(void)noexcept(true)=0;
	virtual void CancelIO(void)noexcept(true)=0;
	virtual void CancelOperation(bcIOObject *Object)noexcept(true)=0;
};
//---------------------------------------------------------------------------
class cNTFileOverlappedIOHandleSyncIOObject : public bcNTFileOverlappedIOHandle::bcIOObject
{
public:
	cNTFileOverlappedIOHandleSyncIOObject()noexcept(true);
	~cNTFileOverlappedIOHandleSyncIOObject()noexcept(true);

	bool Read(bcNTFileOverlappedIOHandle *FileIO,void *Buffer,uIntn Size)noexcept(true);
	bool Write(bcNTFileOverlappedIOHandle *FileIO,const void *Buffer,uIntn Size)noexcept(true);

protected:
	cThreadOneTimeNotifier fNotifier;

	virtual void Completed(void)noexcept(true)override;

};
//---------------------------------------------------------------------------
class cNTFileOverlappedIOHandleAsyncIOTask : public iStreamTask, public iStreamErrorReport,public bcNTFileOverlappedIOHandle::bcIOObject
{
public:
	cNTFileOverlappedIOHandleAsyncIOTask()noexcept(true);
	~cNTFileOverlappedIOHandleAsyncIOTask()noexcept(true);

	void SetupOverlapped(void)noexcept(true);
	void SetupOverlapped(uInt64 Offset)noexcept(true);
	bool Read(bcNTFileOverlappedIOHandle *FileIO,void *Buffer,uIntn Size)noexcept(true);
	bool Write(bcNTFileOverlappedIOHandle *FileIO,const void *Buffer,uIntn Size)noexcept(true);

	virtual void* cnLib_FUNC CastInterface(iTypeID InterfaceID)noexcept(true) override;

	// iAsyncTask

	virtual bool cnLib_FUNC IsDone(void)noexcept(true)override;
	virtual bool cnLib_FUNC SetNotify(iProcedure *NotifyProcedure)noexcept(true)override;

	// iSteamTask

	virtual void cnLib_FUNC Cancel(void)noexcept(true)override;
	virtual bool cnLib_FUNC GetResult(uIntn &SizeCompleted)noexcept(true)override;

	// iStreamErrorReport

	virtual eStreamError cnLib_FUNC GetStreamError(void)noexcept(true)override;
protected:
	rPtr<bcNTFileOverlappedIOHandle> fFileIO;

	virtual void Completed(void)noexcept(true)override;
private:
	cAsyncTaskState fTaskState;
};
//---------------------------------------------------------------------------
class cNTFileOverlappedIOHandleStream : public iStream, public iWinFileHandle
{
public:
	cNTFileOverlappedIOHandleStream(rPtr<bcNTFileOverlappedIOHandle> FileIO)noexcept(true);
	~cNTFileOverlappedIOHandleStream()noexcept(true);

	virtual void* cnLib_FUNC CastInterface(iTypeID InterfaceID)noexcept(true) override;

// iWinFileHandle

	virtual HANDLE cnLib_FUNC GetFileHandle(void)noexcept(true)override;

// iStream

	virtual void cnLib_FUNC Close(void)noexcept(true)override;
	virtual bool cnLib_FUNC IsEndOfReading(void)noexcept(true)override;
	virtual bool cnLib_FUNC SetEndOfStream(void)noexcept(true)override;
	virtual bool cnLib_FUNC Read(void *Buffer,uIntn Size,uIntn &SizeCompleted)noexcept(true)override;
	virtual bool cnLib_FUNC Write(const void *Buffer,uIntn Size,uIntn &SizeCompleted)noexcept(true)override;
	virtual iPtr<iStreamTask> cnLib_FUNC ReadAsync(void *Buffer,uIntn Size)noexcept(true)override;
	virtual iPtr<iStreamTask> cnLib_FUNC WriteAsync(const void *Buffer,uIntn Size)noexcept(true)override;
private:
	rPtr<bcNTFileOverlappedIOHandle> fFileIO;
	bool fEOF;

	class cReadTask : public cNTFileOverlappedIOHandleAsyncIOTask
	{
	public:
		iPtr<cNTFileOverlappedIOHandleStream> Host;
		virtual void Completed(void)noexcept(true)override;
	};
};
//---------------------------------------------------------------------------
class cNTFileOverlappedIOHandleFileStream : public iFileStream,public iStreamErrorReport,public iWinFileHandle
{
public:
	cNTFileOverlappedIOHandleFileStream(rPtr<bcNTFileOverlappedIOHandle> FileIO)noexcept(true);
	~cNTFileOverlappedIOHandleFileStream()noexcept(true);

	virtual void* cnLib_FUNC CastInterface(iTypeID InterfaceID)noexcept(true) override;

// iWinFileHandle

	virtual HANDLE	cnLib_FUNC GetFileHandle(void)noexcept(true)override;

// iStream
	
	virtual void cnLib_FUNC Close(void)noexcept(true)override;
	virtual bool cnLib_FUNC Read(void *Buffer,uIntn Size,uInt64 Offset,uIntn &SizeCompleted)noexcept(true)override;
	virtual bool cnLib_FUNC Write(const void *Buffer,uIntn Size,uInt64 Offset,uIntn &SizeCompleted)noexcept(true)override;
	virtual iPtr<iStreamTask> cnLib_FUNC ReadAsync(void *Buffer,uIntn Size,uInt64 Offset)noexcept(true)override;
	virtual iPtr<iStreamTask> cnLib_FUNC WriteAsync(const void *Buffer,uIntn Size,uInt64 Offset)noexcept(true)override;
	virtual bool cnLib_FUNC GetSize(uInt64 &Size)noexcept(true)override;
	virtual bool cnLib_FUNC SetSize(uInt64 NewSize)noexcept(true)override;
	virtual bool cnLib_FUNC FlushBuffer(void)noexcept(true)override;

// iStreamErrorReport

	virtual eStreamError cnLib_FUNC GetStreamError(void)noexcept(true)override;
protected:
	rPtr<bcNTFileOverlappedIOHandle> fFileIO;
};
//---------------------------------------------------------------------------
class cNTFolderOverlappedIOHandleFileWatcher : public bcAsyncNotification, public cDualReference
{
public:
	cNTFolderOverlappedIOHandleFileWatcher(rPtr<bcNTFileOverlappedIOHandle> FolderIO)noexcept(true);
	~cNTFolderOverlappedIOHandleFileWatcher()noexcept(true);

protected:

	void VirtualStopped(void)noexcept(true);

	virtual iReference* NotificationInnerReference(void)noexcept(true)override;
	virtual void NotificationStarted(void)noexcept(true)override;
	virtual CycleState NotificationCheckState(void)noexcept(true)override;
	virtual void AsyncQueueNotify(void)noexcept(true)override;

private:
	rPtr<bcNTFileOverlappedIOHandle> fFolderIO;

	class cDirectoryReadChangeObject : public bcNTFileOverlappedIOHandle::bcIOObject
	{
		cNTFolderOverlappedIOHandleFileWatcher* GetHost(void)noexcept(true);
	public:
		bool Read(bcNTFileOverlappedIOHandle *FolderIO)noexcept(true);

		virtual void Completed(void)noexcept(true)override;

		
		uInt8 ReadChangeBuffer[1024];
	}fDirectoryReadChangeObject;

	cExclusiveFlag fReadChangeFlag;
	bool fReadInProgress=false;
	void ContinueRead(void)noexcept(true);

	void DirectoryReadCompleted(void)noexcept(true);

};
//---------------------------------------------------------------------------
class cNTFolderOverlappedIOHandleFileObserver : public iFileObserver, public bcAsyncQueue, public cDualReference
{
public:
	cNTFolderOverlappedIOHandleFileObserver(rPtr<bcNTFileOverlappedIOHandle> FolderIO,cString<wchar_t> Path,iPtr<iFileName> (*CreateFileName)(cString<wchar_t> Path)noexcept(true))noexcept(true);
	~cNTFolderOverlappedIOHandleFileObserver()noexcept(true);


	// iAsyncNotification

	virtual bool cnLib_FUNC StartNotify(iReference *Reference,iAsyncNotificationCallback *Callback)noexcept(true)override;
	virtual void cnLib_FUNC StopNotify(void)noexcept(true)override;
	virtual void cnLib_FUNC NotifyCallback(bool IdleNotify)noexcept(true)override;
	virtual bool cnLib_FUNC IsClosed(void)noexcept(true)override;

	// iFileObserver

	virtual iPtr<iFile> cnLib_FUNC FetchFileChange(eFileChange &Change)noexcept(true)override;
	virtual void cnLib_FUNC DiscardChanges(void)noexcept(true)override;
	virtual rPtr<iFileEnumerator> cnLib_FUNC ResetChanges(void)noexcept(true)override;

protected:

	void VirtualStarted(void)noexcept(true);
	void VirtualStopped(void)noexcept(true);

	virtual iReference* NotificationInnerReference(void)noexcept(true)override;
	virtual void NotificationStarted(void)noexcept(true)override;
	virtual void NotificationStopped(void)noexcept(true)override;
	virtual CycleState NotificationCheckState(void)noexcept(true)override;
	virtual void AsyncQueueNotify(void)noexcept(true)override;

	iAsyncNotificationCallback *fCallback;
	rPtr<iReference> fCallbackReference;

	static bool IsDot(const WIN32_FIND_DATA &fd)noexcept(true);
	static bool IsInvalidFile(const WIN32_FIND_DATA &fd)noexcept(true);
	cString<wchar_t> MakeFileName(const WIN32_FIND_DATA &fd)noexcept(true);

private:
	rPtr<bcNTFileOverlappedIOHandle> fFolderIO;
	cString<wchar_t> fPath;
	cString<wchar_t> fFindPath;
	iPtr<iFileName> (*fCreateFileName)(cString<wchar_t> Path)noexcept(true);

	struct cFileInfo
	{
		DWORD FileAttributes;
		uInt64 FileSize;
		FILETIME CreationTime;
		FILETIME LastAccessTime;
		FILETIME LastWriteTime;
	};
	cSeqMap<cString<wchar_t>,cFileInfo> fFileMap;
	cCriticalSection fFileMapCS;
	
	class cFileEnum : public iFileEnumerator
	{
	public:
		virtual bool cnLib_FUNC Fetch(void)noexcept(true)override;

		virtual iFile*	cnLib_FUNC GetCurrentFile(void)noexcept(true)override;
		
		cString<wchar_t> FolderPath;

		struct cFileItem
		{
			cString<wchar_t> Name;
		};
		cSeqList<cFileItem> FileList;

		iPtr<iFileName> (*CreateFileName)(cString<wchar_t> Path)noexcept(true);
		iPtr<iFileName> CurrentFile;
		
		uIntn EnumIndex=cnVar::TIntegerValue<uIntn>::Max;
	
	};

	static void AssignFileInfo(cFileInfo &Info,const WIN32_FIND_DATAW &fd)noexcept(true);
	void SetupFileInfo(cFileInfo &Info,const wchar_t *FileName)noexcept(true);

	class cDirectoryReadChangeObject : public bcNTFileOverlappedIOHandle::bcIOObject
	{
		cNTFolderOverlappedIOHandleFileObserver* GetHost(void)noexcept(true);
	public:
		bool Read(bcNTFileOverlappedIOHandle *FolderIO)noexcept(true);

		virtual void Completed(void)noexcept(true)override;

		
		void *ReadChangeBuffer;
		static const DWORD ReadChangeBufferSize=65536-512;
	}fDirectoryReadChangeObject;

	cExclusiveFlag fProcessChangeFlag;
	bool fLostTrack=false;
	bool fReadInProgress=false;
	void NotifyProcessChanges(void)noexcept(true);

	void DirectoryReadCompleted(void)noexcept(true);
	struct cPendingChangeItem : cRTLAllocator
	{
		cPendingChangeItem *Next;
		cString<wchar_t> Name;
		eFileChange Change;
	};
	cAtomicQueueSO<cPendingChangeItem> fPendingChangeQueue;
	cAtomicStack<cPendingChangeItem> fChangeItemRecycler;
	void CatchFileChanges(void)noexcept(true);
	void ResetChangeQueue(void)noexcept(true);
	void ResetFileChanges(void)noexcept(true);
	bool DirectoryReadParse(void)noexcept(true);

};
//---------------------------------------------------------------------------
#if _WIN32_WINNT >= _WIN32_WINNT_VISTA
//---------------------------------------------------------------------------
class cNT6ThreadPoolFileIOHandle : public bcNTFileOverlappedIOHandle
{
public:
	cNT6ThreadPoolFileIOHandle()noexcept(true);
	~cNT6ThreadPoolFileIOHandle()noexcept(true);

	bool Open(HANDLE FileHandle,PTP_CALLBACK_ENVIRON Environment=nullptr)noexcept(true);

	virtual void Close(void)noexcept(true)override;
	virtual void StartIO(void)noexcept(true)override;
	virtual void CancelIO(void)noexcept(true)override;
	virtual void CancelOperation(bcIOObject *Object)noexcept(true)override;

protected:
	PTP_IO fIOThreadPool;

	static VOID CALLBACK IoCompletionCallback(
	  _Inout_     PTP_CALLBACK_INSTANCE Instance,
	  _Inout_opt_ PVOID                 Context,
	  _Inout_opt_ PVOID                 Overlapped,
	  _In_        ULONG                 IoResult,
	  _In_        ULONG_PTR             NumberOfBytesTransferred,
	  _Inout_     PTP_IO                Io
	)noexcept(true);
};
//---------------------------------------------------------------------------
#endif	// _WIN32_WINNT_VISTA
//---------------------------------------------------------------------------
}	// namespace cnWinRTL
//---------------------------------------------------------------------------
}	// namespace cnRTL
//---------------------------------------------------------------------------
}	// namespace cnLibrary
//---------------------------------------------------------------------------
#endif  /* __cplusplus */
/*-------------------------------------------------------------------------*/
