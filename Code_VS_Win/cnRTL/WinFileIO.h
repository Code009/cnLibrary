/*- cnRTL - Windows - File IO ---------------------------------------------*/
/*         Developer : Code009                                             */
/*         Create on : 2022-01-20                                          */
/*-------------------------------------------------------------------------*/
#pragma once
/*-------------------------------------------------------------------------*/
#include <cnRTL\WinCommon.h>

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
HANDLE Win32FileOpenHandle(const wchar_t *FileName,eFileAccess AccessMode,eFileCreate CreateFlag,DWORD Flag);
uInt64 Win32FileGetSize(const wchar_t *FileName);
bool Win32FileIsExists(const wchar_t *FileName);

//---------------------------------------------------------------------------
eStreamError Win32ErrorToStreamError(DWORD ErrorCode);
//---------------------------------------------------------------------------
struct cWin32FileHandle
{
	HANDLE Handle;

	cWin32FileHandle();
	cWin32FileHandle(HANDLE InitalHandle);
	~cWin32FileHandle();

	void TransferHandle(HANDLE NewHandle);
	void Close(void);
};
//---------------------------------------------------------------------------
struct cWin32FileMappingHandle
{
	HANDLE Handle;
	aClsRef<cWin32FileHandle> FileHandle;

	cWin32FileMappingHandle();
	cWin32FileMappingHandle(HANDLE InitalHandle,aClsRef<cWin32FileHandle> InitialFileHandle);
	~cWin32FileMappingHandle();

	void TransferHandle(HANDLE InitalHandle,aClsRef<cWin32FileHandle> InitialFileHandle);
	void Close(void);
};
//---------------------------------------------------------------------------
class cWin32FileMapping : public iSwapMemory
{
public:
	cWin32FileMapping(aClsRef<cWin32FileMappingHandle> Handle,uInt64 Size,DWORD MapFlag);
	~cWin32FileMapping();

	static DWORD FileAccessToMapFlag(eFileAccess AccessMode);

	virtual uInt64	cnLib_FUNC GetSize(void)override;
	virtual void*	cnLib_FUNC Map(uInt64 Offset,uIntn Size)override;
	virtual bool	cnLib_FUNC Unmap(void *Address,uIntn Size)override;
protected:
	aClsRef<cWin32FileMappingHandle> fFileMappingHandle;
	uInt64 fSize;
	DWORD fMapFlag;
};
//---------------------------------------------------------------------------
class cWin32FileHandleSequenceStream : public iFileSyncStream, public iStreamErrorReport, public iWinFileHandle
{
public:
	cWin32FileHandleSequenceStream(aClsRef<cWin32FileHandle> FileHandle);
	~cWin32FileHandleSequenceStream();

	virtual void* cnLib_FUNC CastInterface(iTypeID InterfaceID)noexcept(true) override;

	virtual HANDLE	cnLib_FUNC GetFileHandle(void)override;

	virtual void cnLib_FUNC Close(void)override;
	virtual bool cnLib_FUNC IsEndOfReading(void)override;
	virtual bool cnLib_FUNC SetEndOfStream(void)override;
	virtual bool cnLib_FUNC Read(void *Buffer,uIntn Size,uIntn &SizeCompleted)override;
	virtual bool cnLib_FUNC Write(const void *Buffer,uIntn Size,uIntn &SizeCompleted)override;

	virtual uInt64 cnLib_FUNC GetPointer(void)override;
	virtual bool cnLib_FUNC MovePointerBegin(uInt64 Offset)override;
	virtual bool cnLib_FUNC MovePointerCurrent(sInt64 Offset)override;
	virtual bool cnLib_FUNC MovePointerEnd(sInt64 Offset)override;

	virtual eStreamError cnLib_FUNC GetStreamError(void)override;
protected:
	aClsRef<cWin32FileHandle> fFileHandle;

	bool fCachedFilePos;

	uInt64 fFilePos;

	void SetupFilePos(void);
};
//---------------------------------------------------------------------------
class bcNTFileOverlappedIOHandle : public iReference
{
public:
	bcNTFileOverlappedIOHandle();
	~bcNTFileOverlappedIOHandle();

	HANDLE Handle;
	
	class bcIOObject
	{
	public:
		OVERLAPPED Overlapped;
		DWORD BytesCompleted;
		DWORD ErrorCode;

		virtual void Completed(void)=0;
	};
	virtual void Close(void)=0;
	virtual void StartIO(void)=0;
	virtual void CancelIO(void)=0;
	virtual void CancelOperation(bcIOObject *Object)=0;
};
//---------------------------------------------------------------------------
class cNTFileOverlappedIOHandleSyncIOObject : public bcNTFileOverlappedIOHandle::bcIOObject
{
public:
	cNTFileOverlappedIOHandleSyncIOObject();
	~cNTFileOverlappedIOHandleSyncIOObject();

	bool Read(bcNTFileOverlappedIOHandle *FileIO,void *Buffer,uIntn Size);
	bool Write(bcNTFileOverlappedIOHandle *FileIO,const void *Buffer,uIntn Size);

protected:
	cThreadOneTimeNotifier fNotifier;

	virtual void Completed(void)override;

};
//---------------------------------------------------------------------------
class cNTFileOverlappedIOHandleAsyncIOTask : public iStreamTask, public iStreamErrorReport,public bcNTFileOverlappedIOHandle::bcIOObject
{
public:
	cNTFileOverlappedIOHandleAsyncIOTask();
	~cNTFileOverlappedIOHandleAsyncIOTask();

	void SetupOverlapped(void);
	void SetupOverlapped(uInt64 Offset);
	bool Read(bcNTFileOverlappedIOHandle *FileIO,void *Buffer,uIntn Size);
	bool Write(bcNTFileOverlappedIOHandle *FileIO,const void *Buffer,uIntn Size);

	virtual void* cnLib_FUNC CastInterface(iTypeID InterfaceID)noexcept(true) override;

	// iAsyncTask

	virtual bool cnLib_FUNC IsDone(void)override;
	virtual bool cnLib_FUNC SetNotify(iProcedure *NotifyProcedure)override;

	// iSteamTask

	virtual void cnLib_FUNC Cancel(void)override;
	virtual bool cnLib_FUNC GetResult(uIntn &SizeCompleted)override;

	// iStreamErrorReport

	virtual eStreamError cnLib_FUNC GetStreamError(void)override;
protected:
	rPtr<bcNTFileOverlappedIOHandle> fFileIO;

	virtual void Completed(void)override;
private:
	cAsyncTaskState fTaskState;
};
//---------------------------------------------------------------------------
class cNTFileOverlappedIOHandleStream : public iStream, public iWinFileHandle
{
public:
	cNTFileOverlappedIOHandleStream(rPtr<bcNTFileOverlappedIOHandle> FileIO);
	~cNTFileOverlappedIOHandleStream();

	virtual void* cnLib_FUNC CastInterface(iTypeID InterfaceID)noexcept(true) override;

// iWinFileHandle

	virtual HANDLE cnLib_FUNC GetFileHandle(void)override;

// iStream

	virtual void cnLib_FUNC Close(void)override;
	virtual bool cnLib_FUNC IsEndOfReading(void)override;
	virtual bool cnLib_FUNC SetEndOfStream(void)override;
	virtual bool cnLib_FUNC Read(void *Buffer,uIntn Size,uIntn &SizeCompleted)override;
	virtual bool cnLib_FUNC Write(const void *Buffer,uIntn Size,uIntn &SizeCompleted)override;
	virtual iPtr<iStreamTask> cnLib_FUNC ReadAsync(void *Buffer,uIntn Size)override;
	virtual iPtr<iStreamTask> cnLib_FUNC WriteAsync(const void *Buffer,uIntn Size)override;
private:
	rPtr<bcNTFileOverlappedIOHandle> fFileIO;
	bool fEOF;

	class cReadTask : public cNTFileOverlappedIOHandleAsyncIOTask
	{
	public:
		iPtr<cNTFileOverlappedIOHandleStream> Host;
		virtual void Completed(void)override;
	};
};
//---------------------------------------------------------------------------
class cNTFileOverlappedIOHandleFileStream : public iFileStream,public iStreamErrorReport,public iWinFileHandle
{
public:
	cNTFileOverlappedIOHandleFileStream(rPtr<bcNTFileOverlappedIOHandle> FileIO);
	~cNTFileOverlappedIOHandleFileStream();

	virtual void* cnLib_FUNC CastInterface(iTypeID InterfaceID)noexcept(true) override;

// iWinFileHandle

	virtual HANDLE	cnLib_FUNC GetFileHandle(void)override;

// iStream
	
	virtual void cnLib_FUNC Close(void)override;
	virtual bool cnLib_FUNC Read(void *Buffer,uIntn Size,uInt64 Offset,uIntn &SizeCompleted)override;
	virtual bool cnLib_FUNC Write(const void *Buffer,uIntn Size,uInt64 Offset,uIntn &SizeCompleted)override;
	virtual iPtr<iStreamTask> cnLib_FUNC ReadAsync(void *Buffer,uIntn Size,uInt64 Offset)override;
	virtual iPtr<iStreamTask> cnLib_FUNC WriteAsync(const void *Buffer,uIntn Size,uInt64 Offset)override;
	virtual bool cnLib_FUNC GetSize(uInt64 &Size)override;
	virtual bool cnLib_FUNC SetSize(uInt64 NewSize)override;
	virtual bool cnLib_FUNC FlushBuffer(void)override;

// iStreamErrorReport

	virtual eStreamError cnLib_FUNC GetStreamError(void)override;
protected:
	rPtr<bcNTFileOverlappedIOHandle> fFileIO;
};
//---------------------------------------------------------------------------
class cNTFolderOverlappedIOHandleFileWatcher : public bcAsyncNotification, public cDualReference
{
public:
	cNTFolderOverlappedIOHandleFileWatcher(rPtr<bcNTFileOverlappedIOHandle> FolderIO);
	~cNTFolderOverlappedIOHandleFileWatcher();

protected:

	void VirtualStopped(void);

	virtual iReference* NotificationInnerReference(void)override;
	virtual void NotificationStarted(void)override;
	virtual CycleState NotificationCheckState(void)override;
	virtual void AsyncQueueNotify(void)override;

private:
	rPtr<bcNTFileOverlappedIOHandle> fFolderIO;

	class cDirectoryReadChangeObject : public bcNTFileOverlappedIOHandle::bcIOObject
	{
		cNTFolderOverlappedIOHandleFileWatcher* GetHost(void);
	public:
		bool Read(bcNTFileOverlappedIOHandle *FolderIO);

		virtual void Completed(void)override;

		
		uInt8 ReadChangeBuffer[1024];
	}fDirectoryReadChangeObject;

	cExclusiveFlag fReadChangeFlag;
	bool fReadInProgress=false;
	void ContinueRead(void);

	void DirectoryReadCompleted(void);

};
//---------------------------------------------------------------------------
class cNTFolderOverlappedIOHandleFileObserver : public iFileObserver, public bcAsyncQueue, public cDualReference
{
public:
	cNTFolderOverlappedIOHandleFileObserver(rPtr<bcNTFileOverlappedIOHandle> FolderIO,cString<wchar_t> Path,iPtr<iFileName> (*CreateFileName)(cString<wchar_t> Path));
	~cNTFolderOverlappedIOHandleFileObserver();


	// iAsyncNotification

	virtual bool cnLib_FUNC StartNotify(iReference *Reference,iAsyncNotificationCallback *Callback)override;
	virtual void cnLib_FUNC StopNotify(void)override;
	virtual void cnLib_FUNC NotifyCallback(bool IdleNotify)override;
	virtual bool cnLib_FUNC IsClosed(void)override;

	// iFileObserver

	virtual void cnLib_FUNC DiscardChanges(void)override;
	virtual rPtr<iFileEnumerator> cnLib_FUNC EnumCurrentFiles(void)override;
	virtual rPtr<iFileChangeEnumerator> cnLib_FUNC FetchFileChange(void)override;

protected:

	void VirtualStopped(void);

	virtual iReference* NotificationInnerReference(void)override;
	virtual void NotificationStarted(void)override;
	virtual void NotificationStopped(void)override;
	virtual CycleState NotificationCheckState(void)override;
	virtual void AsyncQueueNotify(void)override;

	iAsyncNotificationCallback *fCallback;
	rPtr<iReference> fCallbackReference;

	static bool IsDot(const WIN32_FIND_DATA &fd);
	static bool IsInvalidFile(const WIN32_FIND_DATA &fd);
	cString<wchar_t> MakeFileName(const WIN32_FIND_DATA &fd);

private:
	rPtr<bcNTFileOverlappedIOHandle> fFolderIO;
	cString<wchar_t> fPath;
	cString<wchar_t> fFindPath;
	iPtr<iFileName> (*fCreateFileName)(cString<wchar_t> Path);

	struct cFileInfo
	{
		DWORD FileAttributes;
		uInt64 FileSize;
		FILETIME CreationTime;
		FILETIME LastAccessTime;
		FILETIME LastWriteTime;
	};
	cSeqMap<cString<wchar_t>,cFileInfo> fFileMap;
	
	class cFileEnum : public iFileEnumerator
	{
	public:
		virtual bool cnLib_FUNC Fetch(void)override;

		virtual iFile*	cnLib_FUNC GetCurrentFile(void)override;
		
		cString<wchar_t> FolderPath;

		struct cFileItem
		{
			cString<wchar_t> Name;
		};
		cSeqList<cFileItem> FileList;

		iPtr<iFileName> (*CreateFileName)(cString<wchar_t> Path);
		iPtr<iFileName> CurrentFile;
		
		uIntn EnumIndex=cnVar::TIntegerValue<uIntn>::Max;
	
	};

	static void AssignFileInfo(cFileInfo &Info,const WIN32_FIND_DATAW &fd);
	void SetupFileInfo(cFileInfo &Info,const wchar_t *FileName);
	void ReloadFileMap(void);
	rPtr<iFileChangeEnumerator> CompareFileMap(void);
	void RefreshFileMap(void);

	class cDirectoryReadChangeObject : public bcNTFileOverlappedIOHandle::bcIOObject
	{
		cNTFolderOverlappedIOHandleFileObserver* GetHost(void);
	public:
		bool Read(bcNTFileOverlappedIOHandle *FolderIO);

		virtual void Completed(void)override;

		
		void *ReadChangeBuffer;
		static const DWORD ReadChangeBufferSize=65536-512;
	}fDirectoryReadChangeObject;

	cExclusiveFlag fReadChangeFlag;
	bool fRestartTrack;
	bool fReadInProgress=false;
	void ContinueRead(void);

	void DirectoryReadCompleted(void);
	void DirectoryReadError(void);
	struct cPendingChangeItem : cRTLAllocator
	{
		cPendingChangeItem *Next;
		cString<wchar_t> Name;
		eFileChange Change;
	};
	cAtomicStackSO<cPendingChangeItem> fPendingChangeStack;
	void DirectoryReadParse(void);

	class cFileChangeEnum : public iFileChangeEnumerator
	{
	public:
		virtual bool cnLib_FUNC Fetch(void)override;

		virtual iFile*	cnLib_FUNC GetCurrentFile(void)override;
		virtual eFileChange cnLib_FUNC GetChange(void)override;
	
		cString<wchar_t> FolderPath;

		cSeqMap<cString<wchar_t>,cFileInfo> OldFiles;
		struct cFileChangeInfo
		{
			cString<wchar_t> Name;
			eFileChange Change;
		};
		cSeqList<cFileChangeInfo> ChangeList;

		iPtr<iFileName> (*CreateFileName)(cString<wchar_t> Path);
		iPtr<iFileName> CurrentFile;
		
		uIntn EnumIndex=cnVar::TIntegerValue<uIntn>::Max;
		ufInt8 EnumPart=0;
	};
	void CommitPendingChanges(cFileChangeEnum *ChangeEnum);
};
//---------------------------------------------------------------------------
#if _WIN32_WINNT >= _WIN32_WINNT_VISTA
//---------------------------------------------------------------------------
class cNT6ThreadPoolFileIOHandle : public bcNTFileOverlappedIOHandle
{
public:
	cNT6ThreadPoolFileIOHandle();
	~cNT6ThreadPoolFileIOHandle();

	bool Open(HANDLE FileHandle,PTP_CALLBACK_ENVIRON Environment=nullptr);

	virtual void Close(void)override;
	virtual void StartIO(void)override;
	virtual void CancelIO(void)override;
	virtual void CancelOperation(bcIOObject *Object)override;

protected:
	PTP_IO fIOThreadPool;

	static VOID CALLBACK IoCompletionCallback(
	  _Inout_     PTP_CALLBACK_INSTANCE Instance,
	  _Inout_opt_ PVOID                 Context,
	  _Inout_opt_ PVOID                 Overlapped,
	  _In_        ULONG                 IoResult,
	  _In_        ULONG_PTR             NumberOfBytesTransferred,
	  _Inout_     PTP_IO                Io
	);
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
