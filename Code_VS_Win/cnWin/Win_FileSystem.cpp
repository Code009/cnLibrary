#include "Win_FileSystem.h"

using namespace cnLibrary;
using namespace cnRTL;
using namespace cnWinRTL;
using namespace cnWin;
#if _WIN32_WINNT >= _WIN32_WINNT_VISTA
using namespace cnWinNT6;
#endif

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cWin32FileName::cWin32FileName(cString<wchar_t> &&Path)
	: fFileName(cnVar::MoveCast(Path))
{
	fNamePartIndex=IndexNotFound;
	fCachedFileInfo=false;
}
//---------------------------------------------------------------------------
cWin32FileName::~cWin32FileName()
{
}
//---------------------------------------------------------------------------
eiOrdering cWin32FileName::CompareWin32Name(const wchar_t *DestName,uIntn DestNameLength)const
{
	return static_cast<iOrdering>(cnString::Compare(fFileName.GetString(),fFileName.GetLength(),DestName,DestNameLength));
}
//---------------------------------------------------------------------------
eiOrdering cWin32FileName::Compare(iFileName *Dest)
{
	auto Win32File=iCast<iWin32FileName>(Dest);
	if(Win32File==nullptr)
		return iOrdering::Different;
	return CompareWin32Name(Win32File->GetFileName(),Win32File->GetFileNameLength());
}
//---------------------------------------------------------------------------
uIntn cWin32FileName::MakrDirGetErrorLevel(cnRTL::cStringBuffer<wchar_t> &Path,uIntn Index)
{
	uIntn CurLength;
	uIntn CurLevel=0;
	while((CurLength=cnString::Search(Path.GetString()+Index,Path->Length,'\\'))!=IndexNotFound){
		Index+=CurLength+1;
		CurLevel++;
	}
	return CurLevel;
}
//---------------------------------------------------------------------------
uIntn cWin32FileName::MakeDir(cnRTL::cStringBuffer<wchar_t> &Path)
{
	DWORD FileAttr;
	FileAttr=::GetFileAttributesW(Path->Pointer);
	if(FileAttr!=INVALID_FILE_ATTRIBUTES){
		// already exists
		if(FileAttr&FILE_ATTRIBUTE_DIRECTORY){
			// is directory
			return 0;
		}
		// is file, not directory
		return IndexNotFound;
	}


	uIntn CurPathIndex=0;
	if(Path[0]=='\\'){
		if(Path[1]=='\\'){
			// bypass prefix
			CurPathIndex=cnString::Search(Path.GetString()+2,Path->Length-2,':');
			if(CurPathIndex==IndexNotFound){
				// error folder
				return 0;
			}
			CurPathIndex+=4;
		}
		else{
			CurPathIndex=1;
		}
	}

	// create directory path
	uIntn CreatedPathIndex=CurPathIndex;
	uIntn CurLength;
	while((CurLength=cnString::Search(Path.GetString()+CurPathIndex,Path->Length-CurPathIndex,'\\'))!=IndexNotFound){
		CurPathIndex+=CurLength;

		// truncate to current path
		Path[CurPathIndex]=0;

		FileAttr=::GetFileAttributesW(Path->Pointer);
		if(FileAttr!=INVALID_FILE_ATTRIBUTES){
			// already exists
			if((FileAttr&FILE_ATTRIBUTE_DIRECTORY)==0){
				// is not directory, return current path
				Path[CurPathIndex]='\\';
				return MakrDirGetErrorLevel(Path,CurPathIndex);
			}
		}
		else{
			// create directory here
			if(::CreateDirectoryW(Path->Pointer,nullptr)==false){
				// cannot create directory, return last path
				Path[CurPathIndex]='\\';
				return MakrDirGetErrorLevel(Path,CreatedPathIndex);
			}
		}

		CreatedPathIndex=CurPathIndex;

		// restore full path
		Path[CurPathIndex]='\\';
		CurPathIndex++;
	}
	if(::CreateDirectoryW(Path->Pointer,nullptr)==false){
		// cannot create directory, error at last path
		return 1;
	}

	return 0;
}
//---------------------------------------------------------------------------
void cWin32FileName::CacheFileInfo(const WIN32_FIND_DATA &FindData)
{
	if(fCachedFileInfo==false){
		fFileInfo.dwFileAttributes=FindData.dwFileAttributes;
		fFileInfo.ftCreationTime=FindData.ftCreationTime;
		fFileInfo.ftLastAccessTime=FindData.ftLastAccessTime;
		fFileInfo.ftLastWriteTime=FindData.ftLastWriteTime;
		fFileInfo.nFileSizeLow=FindData.nFileSizeLow;
		fFileInfo.nFileSizeHigh=FindData.nFileSizeHigh;

		fFileExists=true;
		fCachedFileInfo=true;
	}
}
//---------------------------------------------------------------------------
void cWin32FileName::CacheFileInfo(const WIN32_FILE_ATTRIBUTE_DATA &FileInfo)
{
	if(fCachedFileInfo==false){

		fFileInfo=FileInfo;
		fFileExists=true;
		fCachedFileInfo=true;
	}
}
//---------------------------------------------------------------------------
void cWin32FileName::SetupFileInfo(void)
{
	if(fCachedFileInfo==false){
		if(::GetFileAttributesExW(fFileName,GetFileExInfoStandard,&fFileInfo)){
			fFileExists=true;
			fCachedFileInfo=true;
		}
		else{
			fFileExists=false;
			fFileInfo.dwFileAttributes=INVALID_FILE_ATTRIBUTES;
			fFileInfo.nFileSizeLow=0;
			fFileInfo.nFileSizeHigh=0;
			fCachedFileInfo=false;
		}
	}
}
//---------------------------------------------------------------------------
bool cWin32FileName::IsExists(void)
{
	SetupFileInfo();
	return fFileExists;
}
//---------------------------------------------------------------------------
uInt64 cWin32FileName::GetDataSize(void)
{
	SetupFileInfo();

	return Win32FileGetSize(fFileName);
}
//---------------------------------------------------------------------------
bool cWin32FileName::HasData(void)
{
	SetupFileInfo();
	return (fFileInfo.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY)==0;
}
//---------------------------------------------------------------------------
bool cWin32FileName::HasFolder(void)
{
	SetupFileInfo();
	if(fFileExists==false)
		return false;
	return (fFileInfo.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY)!=0;
}
//---------------------------------------------------------------------------
bool cWin32FileName::Delete(void)
{
	SetupFileInfo();
	if(fFileExists==false)
		return false;
	if(fFileInfo.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY){
		// Delete Folder
		return ::RemoveDirectoryW(fFileName)!=FALSE;
	}
	else{
		// Delete File
		return ::DeleteFileW(fFileName)!=FALSE;
	}
}
//---------------------------------------------------------------------------
void cWin32FileName::SetupNameInfo(void)
{
	if(fNamePartIndex==IndexNotFound){
		fNamePartIndex=cnMemory::ReverseSearch(fFileName.GetString(),fFileName.GetLength(),'\\');
		if(fNamePartIndex==IndexNotFound)
			fNamePartIndex=0;
		else
			fNamePartIndex++;
	}
}
//---------------------------------------------------------------------------
rPtr<iFileEnumerator> cWin32FileName::EnumFolder(void)
{
	SetupFileInfo();

	if(!fFileExists){
		return nullptr;
	}
	if((fFileInfo.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY)==0){
		// not a folder
		return nullptr;
	}
	return CreateFileEnumeration(fFileName,nullptr);
}
//---------------------------------------------------------------------------
rPtr<iAsyncNotification> cWin32FileName::WatchFolder(void)
{
	SetupFileInfo();

	if(!fFileExists){
		return nullptr;
	}
	if((fFileInfo.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY)==0){
		// not a folder
		return nullptr;
	}
	return WindowsInterface::Win32FileOpenFileWatcher(fFileName);
}
//---------------------------------------------------------------------------
rPtr<iFileObserver> cWin32FileName::ObserveFolder(void)
{
	SetupFileInfo();

	if(!fFileExists){
		return nullptr;
	}
	if((fFileInfo.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY)==0){
		// not a folder
		return nullptr;
	}
	return WindowsInterface::Win32FileOpenFileObserver(fFileName);
}
//---------------------------------------------------------------------------
iPtr<iFileSyncStream> cWin32FileName::OpenSequentialStream(eFileAccess AccessMode,eFileCreate CreateFlag)
{
	return Win32FileOpenSequentialStream(fFileName,AccessMode,CreateFlag);
}
//---------------------------------------------------------------------------
iPtr<iFileStream> cWin32FileName::OpenFileStream(eFileAccess AccessMode,eFileCreate CreateFlag)
{
	return WindowsInterface::Win32FileOpenFileStream(fFileName,AccessMode,CreateFlag);
}
//---------------------------------------------------------------------------
iPtr<iFileName>	cWin32FileName::ParentFileName(void)
{
	SetupNameInfo();
	if(fNamePartIndex==0){
		return nullptr;
	}

	return iCreate<cWin32FileName>(cString<wchar_t>(fFileName,fNamePartIndex-1));
}
//---------------------------------------------------------------------------
rPtr<iFileEnumerator> cWin32FileName::EnumFileNames(const uChar16 *Filter)
{
	return CreateFileEnumeration(fFileName,utow(Filter));
}
//---------------------------------------------------------------------------
iPtr<iFileName>	cWin32FileName::MakeName(const uChar16*const *FilePath,uIntn Depth)
{
	// create file name
	auto FileName=cnWinRTL::Win32FileMakeName(fFileName,fFileName.GetLength(),FilePath,Depth);
	if(FileName.GetLength()==0)
		return nullptr;

	return iCreate<cWin32FileName>(FileName.Swap());
}
//---------------------------------------------------------------------------
uIntn	cWin32FileName::CreateFolderPath(void)
{
	cnRTL::cStringBuffer<wchar_t> PathBuffer=fFileName.GetArray();

	return MakeDir(PathBuffer);
}
//---------------------------------------------------------------------------
bool cWin32FileName::CreateFolder(void)
{
	return ::CreateDirectoryW(fFileName,nullptr)!=FALSE;
}
//---------------------------------------------------------------------------
const uChar16*	cWin32FileName::GetName(void)
{
	SetupNameInfo();
	return wtou(fFileName.GetString())+fNamePartIndex;
}
//---------------------------------------------------------------------------
uIntn			cWin32FileName::GetNameLength(void)
{
	SetupNameInfo();
	return fFileName->Length-fNamePartIndex;
}
//---------------------------------------------------------------------------
const wchar_t* cWin32FileName::GetFileName(void)
{
	return fFileName;
}
//---------------------------------------------------------------------------
uIntn cWin32FileName::GetFileNameLength(void)
{
	return fFileName.GetLength();
}
//---------------------------------------------------------------------------
void cWin32FileName::SetFileName(const wchar_t *FileName,uIntn Length)
{
	fFileName.SetString(FileName,Length);
	fNamePartIndex=IndexNotFound;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
iPtr<iFileName> cnWin::Win32CreateFileName(cnRTL::cString<wchar_t> FileName)
{
	return iCreate<cWin32FileName>(cnVar::MoveCast(FileName));
}
//---------------------------------------------------------------------------
iPtr<iFileName> cnWin::FileEnumerationCreateFileName(cString<wchar_t> Path,const WIN32_FIND_DATA &FindData)
{
	// create file name
	auto pFileName=wtou(FindData.cFileName);
	auto FileName=Win32FileMakeName(Path,Path.GetLength(),&pFileName,1);
	if(FileName->Length==0)
		return nullptr;
	auto FileNameInterface=iCreate<cWin32FileName>(cnVar::MoveCast(FileName));

	FileNameInterface->CacheFileInfo(FindData);
	return FileNameInterface;
}
//---------------------------------------------------------------------------
rPtr<cWin32FileNameEnum> cnWin::CreateFileEnumeration(cString<wchar_t> Path,const wchar_t *Filter)
{
	auto FindPath=Win32FileMakeFindName(Path,Path->Length,Filter);

	return rCreate<cWin32FileNameEnum>(cnVar::MoveCast(Path),FindPath.Swap(),FileEnumerationCreateFileName);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
aClsRef<cWin32FileMappingHandle> cnWin::CreateFileMappingHandle(aCls<cWin32FileHandle> *FileHandle,uInt64 Size,eFileAccess AccessMode,const wchar_t *FileMappingName)
{
	DWORD ProtectFlag=0;

	switch(AccessMode&FileAccess::ReadWrite){
	default:
	case FileAccess::None:
		ProtectFlag=PAGE_NOACCESS;
		break;
	case FileAccess::Read:
		ProtectFlag=PAGE_READONLY;
		break;
	case FileAccess::Write:
	case FileAccess::ReadWrite:
		ProtectFlag=PAGE_READWRITE;
		break;
	}
	uInt32 SizeLo=static_cast<uInt32>(Size);
	uInt32 SizeHi=static_cast<uInt32>(Size>>32);
	HANDLE FileMappingHandle=::CreateFileMappingW(FileHandle->Handle,NULL,ProtectFlag|SEC_COMMIT,SizeHi,SizeLo,FileMappingName);
	if(FileMappingHandle==nullptr)
		return nullptr;

	return aClsCreate<cWin32FileMappingHandle>(FileMappingHandle,FileHandle);
}
//---------------------------------------------------------------------------
aClsRef<cWin32FileMappingHandle> cnWin::CreateSwapFileHandle(uInt64 Size)
{
	uInt32 SizeLo=static_cast<uInt32>(Size);
	uInt32 SizeHi=static_cast<uInt32>(Size>>32);
	HANDLE FileMappingHandle=::CreateFileMappingW(INVALID_HANDLE_VALUE,NULL,PAGE_READWRITE,SizeHi,SizeLo,NULL);
	if(FileMappingHandle==nullptr)	// create failed
		return nullptr;
	// create file mapping
	return aClsCreate<cWin32FileMappingHandle>(FileMappingHandle,nullptr);
}
//---------------------------------------------------------------------------
rPtr<iSwapMemory> cnWin::AllocSwapFile(uInt64 Size)
{
	auto FMHandle=CreateSwapFileHandle(Size);
	if(FMHandle==nullptr)	// create failed
		return nullptr;
	// create file mapping
	return rCreate<cWin32FileMapping>(cnVar::MoveCast(FMHandle),Size,FILE_MAP_READ|FILE_MAP_WRITE);
}
//---------------------------------------------------------------------------
rPtr<iSwapMemory> cnWin::Win32FileCreateSwapMemory(const wchar_t *FileName,uInt64 Size,eFileAccess AccessMode)
{
	DWORD Flag=0;
	auto FileHandle=Win32FileOpenHandle(FileName,AccessMode,FileCreate::Open,Flag);
	if(FileHandle==INVALID_HANDLE_VALUE)
		return nullptr;

	// create file mapping
	auto FileMapping=CreateFileMappingHandle(aClsCreate<cWin32FileHandle>(FileHandle),Size,AccessMode);
	if(FileMapping==nullptr){	// create failed
		return nullptr;
	}
	DWORD MapFlag=cWin32FileMapping::FileAccessToMapFlag(AccessMode);
	// create object
	return rCreate<cWin32FileMapping>(cnVar::MoveCast(FileMapping),Size,MapFlag);
}
//---------------------------------------------------------------------------
rPtr<iSwapMemory> cnWin::CreateSwapMemoryFromFile(iFile *File,uInt64 Size,eFileAccess AccessMode)
{
	auto WinFileName=iCast<iWin32FileName>(File);
	if(WinFileName==nullptr)
		return nullptr;
	
	return Win32FileCreateSwapMemory(WinFileName->GetFileName(),Size,AccessMode);
}
//---------------------------------------------------------------------------
iPtr<iFileSyncStream> cnWin::Win32FileOpenSequentialStream(const wchar_t *FileName,eFileAccess AccessMode,eFileCreate CreateFlag)
{
	auto FileHandle=Win32FileOpenHandle(FileName,AccessMode,CreateFlag,FILE_FLAG_SEQUENTIAL_SCAN);
	if(FileHandle==INVALID_HANDLE_VALUE)
		return nullptr;
	return iCreate<cWin32FileHandleSequenceStream>(aClsCreate<cWin32FileHandle>(FileHandle));
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
#if _WIN32_WINNT >= _WIN32_WINNT_VISTA
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
static cnRTL::cStringBuffer<wchar_t> Win32FileMakeShellFolderName(REFKNOWNFOLDERID rfid)
{
	cShellString Path;
	uIntn PathLen;
	if(SHGetKnownFolderPath(rfid,KF_FLAG_DEFAULT,nullptr,Path.RetPtr())!=S_OK){
		PathLen=0;
	}
	else{
		PathLen=cnString::GetLength(Path.GetString());
	}

	return NTFileNormalizeName(Path,PathLen);
}
//---------------------------------------------------------------------------
cnRTL::cStringBuffer<wchar_t> Win32FileSystem::MakeSystemFileName(cnWindows::eSystemFile File)
{
	switch(File){
	default:
		return nullptr;
	case cnWindows::SystemFile::Execuable:
		return NTFileMakeApplicationFileName();
	case cnWindows::SystemFile::ApplicationFolder:
		return NTFileMakeApplicationFolderName();
	case cnWindows::SystemFile::DocumentFolder:
		return Win32FileMakeShellFolderName(FOLDERID_Documents);
	case cnWindows::SystemFile::TempFolder:
		return NTFileMakeTempFolderName();
	}
}
//---------------------------------------------------------------------------
iPtr<iFileStream> Win32FileSystem::OpenFileStream(const wchar_t *FileName,eFileAccess AccessMode,eFileCreate CreateFlag)
{
	auto FileHandle=Win32FileOpenHandle(FileName,AccessMode,CreateFlag,FILE_FLAG_OVERLAPPED|FILE_FLAG_RANDOM_ACCESS);
	if(FileHandle==INVALID_HANDLE_VALUE)
		return nullptr;
	
	auto FileIO=rCreate<cNT6ThreadPoolFileIOHandle>();
	if(FileIO->Open(FileHandle)==false){
		::CloseHandle(FileHandle);
		return nullptr;
	}
	
	return iCreate<cNTFileOverlappedIOHandleFileStream>(cnVar::MoveCast(FileIO));
}
//---------------------------------------------------------------------------
iPtr<iStream>	Win32FileSystem::OpenDeviceStream(const wchar_t *FileName,eFileAccess AccessMode,eFileCreate CreateFlag)
{
	auto FileHandle=Win32FileOpenHandle(FileName,AccessMode,CreateFlag,FILE_FLAG_OVERLAPPED);
	if(FileHandle==INVALID_HANDLE_VALUE)
		return nullptr;

	auto FileIO=rCreate<cNT6ThreadPoolFileIOHandle>();
	if(FileIO->Open(FileHandle)==false){
		::CloseHandle(FileHandle);
		return nullptr;
	}

	return iCreate<cNTFileOverlappedIOHandleStream>(cnVar::MoveCast(FileIO));
}
//---------------------------------------------------------------------------
iPtr<iEndpoint>		Win32FileSystem::OpenDeviceEndpoint(const wchar_t *FileName,eFileAccess AccessMode,eFileCreate CreateFlag)
{
	auto FileHandle=Win32FileOpenHandle(FileName,AccessMode,CreateFlag,FILE_FLAG_OVERLAPPED);
	if(FileHandle==INVALID_HANDLE_VALUE)
		return nullptr;

	auto FileIO=rCreate<cNT6ThreadPoolFileIOHandle>();
	if(FileIO->Open(FileHandle)==false){
		::CloseHandle(FileHandle);
		return nullptr;
	}

	auto SeqStream=iCreate<cNTFileOverlappedIOHandleStream>(cnVar::MoveCast(FileIO));
	return CreateEndpointFromSteam(cnVar::MoveCast(SeqStream));
}
//---------------------------------------------------------------------------
rPtr<iAsyncNotification>	Win32FileSystem::OpenFileWatcher(const wchar_t *FolderName)
{
	HANDLE FolderHandle=::CreateFileW(FolderName,FILE_LIST_DIRECTORY,FILE_SHARE_READ|FILE_SHARE_WRITE|FILE_SHARE_DELETE,NULL,OPEN_EXISTING,FILE_FLAG_OVERLAPPED|FILE_FLAG_BACKUP_SEMANTICS,NULL);
	if(FolderHandle==INVALID_HANDLE_VALUE)
		return nullptr;
	auto FileIO=rCreate<cNT6ThreadPoolFileIOHandle>();
	if(FileIO->Open(FolderHandle)==false){
		::CloseHandle(FolderHandle);
		return nullptr;
	}
	return rCreate<cNTFolderOverlappedIOHandleFileWatcher>(cnVar::MoveCast(FileIO));
}
//---------------------------------------------------------------------------
rPtr<iFileObserver>			Win32FileSystem::OpenFileObserver(cnRTL::cString<wchar_t> FolderName)
{
	HANDLE FolderHandle=::CreateFileW(FolderName,FILE_LIST_DIRECTORY,FILE_SHARE_READ|FILE_SHARE_WRITE|FILE_SHARE_DELETE,NULL,OPEN_EXISTING,FILE_FLAG_OVERLAPPED|FILE_FLAG_BACKUP_SEMANTICS,NULL);
	if(FolderHandle==INVALID_HANDLE_VALUE)
		return nullptr;
	auto FileIO=rCreate<cNT6ThreadPoolFileIOHandle>();
	if(FileIO->Open(FolderHandle)==false){
		::CloseHandle(FolderHandle);
		return nullptr;
	}
	return rCreate<cNTFolderOverlappedIOHandleFileObserver>(cnVar::MoveCast(FileIO),cnVar::MoveCast(FolderName),Win32CreateFileName);
}
//---------------------------------------------------------------------------
#endif	// _WIN32_WINNT >= _WIN32_WINNT_VISTA
//---------------------------------------------------------------------------
