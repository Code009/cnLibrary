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
	cWin32FileName(cnRTL::cString<wchar_t> &&Path)noexcept(true);
	~cWin32FileName()noexcept(true);

	eiOrdering CompareWin32Name(const wchar_t *DestName,uIntn DestNameLength)const noexcept(true);
	

	static uIntn MakeDir(cnRTL::cStringBuffer<wchar_t> &Path)noexcept(true);
	static uIntn MakrDirGetErrorLevel(cnRTL::cStringBuffer<wchar_t> &Path,uIntn Index)noexcept(true);

	// iFile

	virtual bool	cnLib_FUNC IsExists(void)noexcept(true)override;
	virtual bool	cnLib_FUNC HasData(void)noexcept(true)override;
	virtual bool	cnLib_FUNC HasFolder(void)noexcept(true)override;
	virtual bool	cnLib_FUNC Delete(void)noexcept(true)override;

	virtual uInt64	cnLib_FUNC GetDataSize(void)noexcept(true)override;
	virtual iPtr<iFileSyncStream>	cnLib_FUNC OpenSequentialStream(eFileAccess AccessMode,eFileCreate CreateFlag)noexcept(true)override;
	virtual iPtr<iFileStream>		cnLib_FUNC OpenFileStream(eFileAccess AccessMode,eFileCreate CreateFlag)noexcept(true)override;

	virtual rPtr<iFileEnumerator>		cnLib_FUNC EnumFolder(void)noexcept(true)override;
	virtual rPtr<iAsyncNotification>	cnLib_FUNC WatchFolder(void)noexcept(true)override;
	virtual rPtr<iFileObserver>			cnLib_FUNC ObserveFolder(void)noexcept(true)override;
	virtual bool	cnLib_FUNC CreateFolder(void)noexcept(true)override;
	virtual uIntn	cnLib_FUNC CreateFolderPath(void)noexcept(true)override;

	// iFileName

	virtual eiOrdering cnLib_FUNC Compare(iFileName *Dest)noexcept(true)override;
	virtual iPtr<iFileName>	cnLib_FUNC ParentFileName(void)noexcept(true)override;
	virtual rPtr<iFileEnumerator>	cnLib_FUNC EnumFileNames(const uChar16 *Filter)noexcept(true)override;
	virtual const uChar16*	cnLib_FUNC GetName(void)noexcept(true)override;
	virtual uIntn			cnLib_FUNC GetNameLength(void)noexcept(true)override;
	virtual iPtr<iFileName>	cnLib_FUNC MakeName(const uChar16*const *Path,uIntn Depth)noexcept(true)override;

	// iWin32FileName

	virtual const wchar_t*	cnLib_FUNC GetFileName(void)noexcept(true)override;
	virtual uIntn cnLib_FUNC GetFileNameLength(void)noexcept(true)override;
	virtual void	cnLib_FUNC SetFileName(const wchar_t *FileName,uIntn Length)noexcept(true)override;

	void CacheFileInfo(const WIN32_FIND_DATA &FindData)noexcept(true);
	void CacheFileInfo(const WIN32_FILE_ATTRIBUTE_DATA &FileInfo)noexcept(true);
protected:
	cnRTL::cString<wchar_t> fFileName;

	uIntn fNamePartIndex;
	void SetupNameInfo(void)noexcept(true);

	bool fCachedFileInfo;

	bool fFileExists;

	WIN32_FILE_ATTRIBUTE_DATA fFileInfo;

	void SetupFileInfo(void)noexcept(true);
};
//---------------------------------------------------------------------------
iPtr<iFileName> Win32CreateFileName(cnRTL::cString<wchar_t> FileName)noexcept(true);
//---------------------------------------------------------------------------
iPtr<iFileName> FileEnumerationCreateFileName(cnRTL::cString<wchar_t> Path,const WIN32_FIND_DATA &FindData)noexcept(true);
rPtr<cnRTL::cnWinRTL::cWin32FileNameEnum> CreateFileEnumeration(cnRTL::cString<wchar_t> Path,const wchar_t *Filter)noexcept(true);
//---------------------------------------------------------------------------
aClsRef<cnRTL::cnWinRTL::cWin32FileMappingHandle> CreateSwapFileHandle(uInt64 Size)noexcept(true);
aClsRef<cnRTL::cnWinRTL::cWin32FileMappingHandle> CreateFileMappingHandle(aCls<cnRTL::cnWinRTL::cWin32FileHandle> *FileHandle,uInt64 Size,eFileAccess AccessMode,const wchar_t *FileMappingName=nullptr)noexcept(true);
rPtr<iSwapMemory> AllocSwapFile(uInt64 Size)noexcept(true);
rPtr<iSwapMemory> Win32FileCreateSwapMemory(const wchar_t *FileName,uInt64 Size,eFileAccess AccessMode)noexcept(true);
rPtr<iSwapMemory> CreateSwapMemoryFromFile(iFile *File,uInt64 Size,eFileAccess AccessMode)noexcept(true);
//---------------------------------------------------------------------------
iPtr<iFileSyncStream> Win32FileOpenSequentialStream(const wchar_t *FileName,eFileAccess AccessMode,eFileCreate CreateFlag)noexcept(true);
//---------------------------------------------------------------------------
#if _WIN32_WINNT >= _WIN32_WINNT_VISTA
//---------------------------------------------------------------------------
namespace cnWinNT6{
//---------------------------------------------------------------------------
namespace Win32FileSystem{
//---------------------------------------------------------------------------
cnRTL::cStringBuffer<wchar_t> MakeSystemFileName(cnWindows::eSystemFile File)noexcept(true);
//---------------------------------------------------------------------------
iPtr<iFileStream>			OpenFileStream(const wchar_t *FileName,eFileAccess AccessMode,eFileCreate CreateFlag)noexcept(true);
iPtr<iStream>				OpenDeviceStream(const wchar_t *FileName,eFileAccess AccessMode,eFileCreate CreateFlag)noexcept(true);
iPtr<iEndpoint>				OpenDeviceEndpoint(const wchar_t *FileName,eFileAccess AccessMode,eFileCreate CreateFlag)noexcept(true);
rPtr<iAsyncNotification>	OpenFileWatcher(const wchar_t *FolderName)noexcept(true);
rPtr<iFileObserver>			OpenFileObserver(cnRTL::cString<wchar_t> FolderName)noexcept(true);
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
