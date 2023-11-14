/*- cnWin - File System ---------------------------------------------------*/
/*         Developer : Code009                                             */
/*         Create on : 2021-07-12                                          */
/*-------------------------------------------------------------------------*/
#pragma once
/*-------------------------------------------------------------------------*/
#include <cnWin\Win_Common.h>
#include <cnWin\Win_BasicStorage.h>

#ifdef __cplusplus
//---------------------------------------------------------------------------
namespace cnLibrary{
//---------------------------------------------------------------------------
namespace cnWin{
//---------------------------------------------------------------------------
class cWin32FileName : public iWin32FileName
{
public:
	cWin32FileName(cnRTL::cString<wchar_t> &&Path);
	~cWin32FileName();

	eiOrdering CompareWin32Name(const wchar_t *DestName,uIntn DestNameLength)const;
	

	static uIntn MakeDir(cnRTL::cStringBuffer<wchar_t> &Path);
	static uIntn MakrDirGetErrorLevel(cnRTL::cStringBuffer<wchar_t> &Path,uIntn Index);

	// iFile

	virtual bool	cnLib_FUNC IsExists(void)override;
	virtual bool	cnLib_FUNC HasData(void)override;
	virtual bool	cnLib_FUNC HasFolder(void)override;
	virtual bool	cnLib_FUNC Delete(void)override;

	virtual uInt64	cnLib_FUNC GetDataSize(void)override;
	virtual iPtr<iFileSyncStream>	cnLib_FUNC OpenSequentialStream(eFileAccess AccessMode,eFileCreate CreateFlag)override;
	virtual iPtr<iFileStream>		cnLib_FUNC OpenFileStream(eFileAccess AccessMode,eFileCreate CreateFlag)override;

	virtual rPtr<iFileEnumerator>		cnLib_FUNC EnumFolder(void)override;
	virtual rPtr<iAsyncNotification>	cnLib_FUNC WatchFolder(void)override;
	virtual rPtr<iFileObserver>			cnLib_FUNC ObserveFolder(void)override;
	virtual bool	cnLib_FUNC CreateFolder(void)override;
	virtual uIntn	cnLib_FUNC CreateFolderPath(void)override;

	// iFileName

	virtual eiOrdering cnLib_FUNC Compare(iFileName *Dest)override;
	virtual iPtr<iFileName>	cnLib_FUNC ParentFileName(void)override;
	virtual rPtr<iFileEnumerator>	cnLib_FUNC EnumFileNames(const uChar16 *Filter)override;
	virtual const uChar16*	cnLib_FUNC GetName(void)override;
	virtual uIntn			cnLib_FUNC GetNameLength(void)override;
	virtual iPtr<iFileName>	cnLib_FUNC MakeName(const uChar16*const *Path,uIntn Depth)override;

	// iWin32FileName

	virtual const wchar_t*	cnLib_FUNC GetFileName(void)override;
	virtual uIntn cnLib_FUNC GetFileNameLength(void)override;
	virtual void	cnLib_FUNC SetFileName(const wchar_t *FileName,uIntn Length)override;

	void CacheFileInfo(const WIN32_FIND_DATA &FindData);
	void CacheFileInfo(const WIN32_FILE_ATTRIBUTE_DATA &FileInfo);
protected:
	cnRTL::cString<wchar_t> fFileName;

	uIntn fNamePartIndex;
	void SetupNameInfo(void);

	bool fCachedFileInfo;

	bool fFileExists;

	WIN32_FILE_ATTRIBUTE_DATA fFileInfo;

	void SetupFileInfo(void);
};
//---------------------------------------------------------------------------
iPtr<iFileName> Win32CreateFileName(cnRTL::cString<wchar_t> FileName);
//---------------------------------------------------------------------------
iPtr<iFileName> FileEnumerationCreateFileName(cnRTL::cString<wchar_t> Path,const WIN32_FIND_DATA &FindData);
rPtr<cnRTL::cnWinRTL::cWin32FileNameEnum> CreateFileEnumeration(cnRTL::cString<wchar_t> Path,const wchar_t *Filter);
//---------------------------------------------------------------------------
aClsRef<cnRTL::cnWinRTL::cWin32FileMappingHandle> CreateSwapFileHandle(uInt64 Size);
aClsRef<cnRTL::cnWinRTL::cWin32FileMappingHandle> CreateFileMappingHandle(aCls<cnRTL::cnWinRTL::cWin32FileHandle> *FileHandle,uInt64 Size,eFileAccess AccessMode,const wchar_t *FileMappingName=nullptr);
rPtr<iSwapMemory> AllocSwapFile(uInt64 Size);
rPtr<iSwapMemory> Win32FileCreateSwapMemory(const wchar_t *FileName,uInt64 Size,eFileAccess AccessMode);
rPtr<iSwapMemory> CreateSwapMemoryFromFile(iFile *File,uInt64 Size,eFileAccess AccessMode);
//---------------------------------------------------------------------------
iPtr<iFileSyncStream> Win32FileOpenSequentialStream(const wchar_t *FileName,eFileAccess AccessMode,eFileCreate CreateFlag);
//---------------------------------------------------------------------------
#if _WIN32_WINNT >= _WIN32_WINNT_VISTA
//---------------------------------------------------------------------------
namespace cnWinNT6{
//---------------------------------------------------------------------------
namespace Win32FileSystem{
//---------------------------------------------------------------------------
cnRTL::cStringBuffer<wchar_t> MakeSystemFileName(cnWindows::eSystemFile File);
//---------------------------------------------------------------------------
iPtr<iFileStream>			OpenFileStream(const wchar_t *FileName,eFileAccess AccessMode,eFileCreate CreateFlag);
iPtr<iStream>				OpenDeviceStream(const wchar_t *FileName,eFileAccess AccessMode,eFileCreate CreateFlag);
iPtr<iEndpoint>				OpenDeviceEndpoint(const wchar_t *FileName,eFileAccess AccessMode,eFileCreate CreateFlag);
rPtr<iAsyncNotification>	OpenFileWatcher(const wchar_t *FolderName);
rPtr<iFileObserver>			OpenFileObserver(cnRTL::cString<wchar_t> FolderName);
//---------------------------------------------------------------------------
}	//	namespace Win32FileSystem
//---------------------------------------------------------------------------
}	// namespace cnWinNT6
//---------------------------------------------------------------------------
#if cnWin_FILESYSTEM_VERSION < _WIN32_WINNT_VISTA
#undef	cnWin_FILESYSTEM_VERSION
#undef	cnWin_FILESYSTEM_NAMESPACE
#define	cnWin_FILESYSTEM_VERSION		_WIN32_WINNT_VISTA
#define	cnWin_FILESYSTEM_NAMESPACE		cnWinNT6::Win32FileSystem
#endif // cnWin_FILESYSTEM_VERSION < _WIN32_WINNT_VISTA
//---------------------------------------------------------------------------
#endif	// _WIN32_WINNT_VISTA
//---------------------------------------------------------------------------
}	// namespace cnWin
//---------------------------------------------------------------------------
}	// namespace cnLibrary
//---------------------------------------------------------------------------
#endif  /* __cplusplus */
/*-------------------------------------------------------------------------*/
