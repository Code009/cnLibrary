/*--- POSIX - File --------------------------------------------------------*/
/*         Developer : Code009                                             */
/*         Create on : 2016-06-07                                          */
/*-------------------------------------------------------------------------*/
#ifndef __cnLib_POSIX_POSIX_File_H__
#define __cnLib_POSIX_POSIX_File_H__

#include <POSIX/POSIXCommon.h>
#include <cnSystem/POSIX_FileSystem.h>

/*-------------------------------------------------------------------------*/
#ifdef	__cplusplus
//---------------------------------------------------------------------------
namespace cnLibrary{
namespace siPOSIX{
//---------------------------------------------------------------------------
namespace POSIXInterface{
//---------------------------------------------------------------------------
iPtr<iFileStream>	FSOpenFileStream(const char *FileName,eFileAccess AccessMode,eFileCreate CreateFlag)noexcept(true);
//---------------------------------------------------------------------------
}	// namespace POSIXInterface
//---------------------------------------------------------------------------
int ConvertOpenFileFlags(eFileAccess AccessMode,eFileCreate CreateFlag)noexcept(true);
//---------------------------------------------------------------------------
#ifdef	siOS_POSIX_ENABLE_CODESETCONVERT
//---------------------------------------------------------------------------
class cICONVTextEncodingConverter : public iTextEncodingConverter
{
	// _XOPEN_VERSION
public:
	cICONVTextEncodingConverter(iconv_t ConvHandle)noexcept(true);
	~cICONVTextEncodingConverter()noexcept(true);

	virtual uIntn cnLib_FUNC Convert(void *Dest,uIntn DestSize,const void *Src,uIntn SrcSize,uIntn *SrcConvertedSize)noexcept(true)override;
protected:
	iconv_t fConvHandle;
};
//---------------------------------------------------------------------------
#endif	// siOS_POSIX_ENABLE_CODESETCONVERT
//---------------------------------------------------------------------------
class cFileSeqStream : public iFileSyncStream
{
public:
	cFileSeqStream(int FileHandle)noexcept(true);
	~cFileSeqStream()noexcept(true);
	
	virtual void cnLib_FUNC Close(void)noexcept(true)override;
	virtual bool cnLib_FUNC IsEndOfReading(void)noexcept(true)override;
	virtual bool cnLib_FUNC SetEndOfStream(void)noexcept(true)override;

	virtual bool cnLib_FUNC Read(void *Buffer,uIntn BufferSize,uIntn &SizeCompleted)noexcept(true)override;
	virtual bool cnLib_FUNC Write(const void *Buffer,uIntn BufferSize,uIntn &SizeCompleted)noexcept(true)override;

	virtual uInt64 cnLib_FUNC GetPointer(void)noexcept(true)override;
	virtual bool cnLib_FUNC MovePointerBegin(uInt64 Offset)noexcept(true)override;
	virtual bool cnLib_FUNC MovePointerCurrent(sInt64 Offset)noexcept(true)override;
	virtual bool cnLib_FUNC MovePointerEnd(sInt64 Offset)noexcept(true)override;
private:
	int fFileHandle;

	bool fEndOfReading;
};
//---------------------------------------------------------------------------
class cStringFileName : public iPOSIXFileName
{
public:
	typedef cnRTL::cStringFilePathTranslated<
		uChar8,uChar16,'/',
		cnRTL::cUTFStringConverter<1,2>,
		cnRTL::cUTFStringConverter<2,1>
	> cStringPath;

	cStringFileName(cStringPath FilePath)noexcept(true);
	~cStringFileName()noexcept(true);

// iFile

	virtual bool	cnLib_FUNC IsExists(void)noexcept(true)override;
	virtual bool	cnLib_FUNC HasData(void)noexcept(true)override;
	virtual bool	cnLib_FUNC HasFolder(void)noexcept(true)override;
	virtual bool	cnLib_FUNC Delete(void)noexcept(true)override;

	virtual uInt64	cnLib_FUNC GetDataSize(void)noexcept(true)override;
	virtual iPtr<iFileSyncStream>	cnLib_FUNC OpenSequentialStream(eFileAccess AccessMode,eFileCreate CreateFlag)noexcept(true)override;
	virtual iPtr<iFileStream>		cnLib_FUNC OpenFileStream(eFileAccess AccessMode,eFileCreate CreateFlag)noexcept(true)override;

	virtual rPtr<iFileEnumerator>	cnLib_FUNC EnumFolder(void)noexcept(true)override;
	virtual rPtr<iAsyncNotification>	cnLib_FUNC WatchFolder(void)noexcept(true)override;
	virtual rPtr<iFileObserver>		cnLib_FUNC ObserveFolder(void)noexcept(true)override;
	virtual bool	cnLib_FUNC CreateFolder(void)noexcept(true)override;
	virtual uIntn	cnLib_FUNC CreateFolderPath(void)noexcept(true)override;

// iFileName

	virtual eiOrdering cnLib_FUNC Compare(iFileName *Dest)noexcept(true)override;

	virtual iPtr<iFileName>	cnLib_FUNC ParentFileName(void)noexcept(true)override;
	virtual rPtr<iFileEnumerator>	cnLib_FUNC EnumFileNames(const uChar16 *Filter)noexcept(true)override;
	virtual const uChar16*	cnLib_FUNC GetName(void)noexcept(true)override;
	virtual uIntn			cnLib_FUNC GetNameLength(void)noexcept(true)override;
	virtual iPtr<iFileName>	cnLib_FUNC MakeName(const uChar16*const *Path,uIntn Depth)noexcept(true)override;


// iPOSIXFileName

	virtual const uChar8* cnLib_FUNC GetPOSIXFileName(void)noexcept(true)override;
	virtual uIntn cnLib_FUNC GetPOSIXFileNameLength(void)noexcept(true)override;
protected:
	cStringPath fFilePath;
};
//---------------------------------------------------------------------------
class cFolderEnum : public iFileEnumerator
{
public:
	cFolderEnum(FTS *FindHandle)noexcept(true);
	~cFolderEnum()noexcept(true);

	virtual bool cnLib_FUNC Fetch(void)noexcept(true)override;

	virtual iFile*	cnLib_FUNC GetCurrentFile(void)noexcept(true)override;
private:
	FTS *fFindHandle;
	FTSENT *fCurrentFile;
	iPtr<iFileName> fCurrentFileName;

};
//---------------------------------------------------------------------------
int						FileSystemOpenFD(const char *FileName,eFileAccess AccessMode,eFileCreate CreateFlag)noexcept(true);
iPtr<iFileSyncStream>	FileSystemOpenSeqStream(const char *FileName,eFileAccess AccessMode,eFileCreate CreateFlag)noexcept(true);
uInt64					FileSystemGetFileSize(const char *FileName)noexcept(true);
bool					FileSystemIsFileExists(const char *FileName)noexcept(true);
//---------------------------------------------------------------------------
rPtr<iFileEnumerator>	FileSystemEnum(const char *FolderName,uIntn FolderNameLength,const uChar16 *Filter)noexcept(true);
iPtr<iFileName>			FileSystemCreateTemporaryFile(const char *FolderName)noexcept(true);
//---------------------------------------------------------------------------
rPtr<iSwapMemory>		FileSystemCreateSwapMemoryFromTemporaryFile(uInt64 Size)noexcept(true);
rPtr<iSwapMemory>		FileSystemCreateSwapMemoryFromFile(const char *FileName,uInt64 Size,eFileAccess AccessMode)noexcept(true);
//---------------------------------------------------------------------------
}	// namespace siPOSIX
}	// namespace cnLibrary
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
#endif	// __cplusplus
/*-------------------------------------------------------------------------*/
#endif
