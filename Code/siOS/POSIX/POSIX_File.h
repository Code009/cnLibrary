/*--- POSIX - File --------------------------------------------------------*/
/*         Developer : Code009                                             */
/*         Create on : 2016-06-07                                          */
/*-------------------------------------------------------------------------*/
#ifndef __cnLib_siOS_POSIX_POSIX_File_H__
#define __cnLib_siOS_POSIX_POSIX_File_H__

#include <siOS/POSIX/POSIXHeader.h>
#include <cnSystem/POSIX_FileSystem.h>

/*-------------------------------------------------------------------------*/
#ifdef	__cplusplus
//---------------------------------------------------------------------------
namespace cnLibrary{
namespace siPOSIX{
//---------------------------------------------------------------------------
namespace POSIXInterface{
//---------------------------------------------------------------------------
iPtr<iFileStream>	FSOpenFileStream(const char *FileName,eFileAccess AccessMode,eFileCreate CreateFlag);
//---------------------------------------------------------------------------
}	// namespace POSIXInterface
//---------------------------------------------------------------------------
int ConvertOpenFileFlags(eFileAccess AccessMode,eFileCreate CreateFlag);
//---------------------------------------------------------------------------
#ifdef	siOS_POSIX_ENABLE_CODESETCONVERT
//---------------------------------------------------------------------------
class cICONVTextEncodingConverter : public iTextEncodingConverter
{
	// _XOPEN_VERSION
public:
	cICONVTextEncodingConverter(iconv_t ConvHandle);
	~cICONVTextEncodingConverter();

	virtual uIntn cnLib_FUNC Convert(void *Dest,uIntn DestSize,const void *Src,uIntn SrcSize,uIntn *SrcConvertedSize)override;
protected:
	iconv_t fConvHandle;
};
//---------------------------------------------------------------------------
#endif	// siOS_POSIX_ENABLE_CODESETCONVERT
//---------------------------------------------------------------------------
class cFileSeqStream : public iFileSyncStream
{
private:
	int fFileHandle;

	bool fEndOfReading;

public:
	cFileSeqStream(int FileHandle);
	~cFileSeqStream();
	
	virtual void cnLib_FUNC Close(void)override;
	virtual bool cnLib_FUNC IsEndOfReading(void)override;
	virtual bool cnLib_FUNC SetEndOfStream(void)override;

	virtual bool cnLib_FUNC Read(void *Buffer,uIntn BufferSize,uIntn &SizeCompleted)override;
	virtual bool cnLib_FUNC Write(const void *Buffer,uIntn BufferSize,uIntn &SizeCompleted)override;

	virtual uInt64 cnLib_FUNC GetPointer(void)override;
	virtual bool cnLib_FUNC MovePointerBegin(uInt64 Offset)override;
	virtual bool cnLib_FUNC MovePointerCurrent(sInt64 Offset)override;
	virtual bool cnLib_FUNC MovePointerEnd(sInt64 Offset)override;
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

	cStringFileName(cStringPath FilePath);
	~cStringFileName();

	virtual eOrdering cnLib_FUNC Compare(const iFileName *Dest)const override;

// iFile

	virtual bool	cnLib_FUNC IsExists(void)override;
	virtual bool	cnLib_FUNC HasData(void)override;
	virtual bool	cnLib_FUNC HasFolder(void)override;

	virtual uInt64	cnLib_FUNC GetDataSize(void)override;
	virtual iPtr<iFileSyncStream>	cnLib_FUNC OpenSequentialStream(eFileAccess AccessMode,eFileCreate CreateFlag)override;
	virtual iPtr<iFileStream>		cnLib_FUNC OpenFileStream(eFileAccess AccessMode,eFileCreate CreateFlag)override;

	virtual rPtr<iFileEnumerator>	cnLib_FUNC EnumFolder(void)override;
	virtual rPtr<iAsyncNotification>	cnLib_FUNC WatchFolder(void)override;
	virtual rPtr<iFileObserver>		cnLib_FUNC ObserveFolder(void)override;
	virtual bool	cnLib_FUNC CreateFolder(void)override;
	virtual uIntn	cnLib_FUNC CreateFolderPath(void)override;

// iFileName

	virtual iPtr<iFileName>	cnLib_FUNC ParentFileName(void)override;
	virtual rPtr<iFileEnumerator>	cnLib_FUNC EnumFileNames(const uChar16 *Filter)override;
	virtual const uChar16*	cnLib_FUNC GetName(void)override;
	virtual uIntn			cnLib_FUNC GetNameLength(void)override;
	virtual iPtr<iFileName>	cnLib_FUNC MakeName(const uChar16*const *Path,uIntn Depth)override;

// iPOSIXFileName

	virtual const uChar8* cnLib_FUNC GetPOSIXFileName(void)override;
	virtual uIntn cnLib_FUNC GetPOSIXFileNameLength(void)override;
protected:
	cStringPath fFilePath;
};
//---------------------------------------------------------------------------
class cFolderEnum : public iFileEnumerator
{
public:
	cFolderEnum(FTS *FindHandle);
	~cFolderEnum();

	virtual bool cnLib_FUNC Fetch(void)override;

	virtual iFile*	cnLib_FUNC GetCurrentFile(void)override;
private:
	FTS *fFindHandle;
	FTSENT *fCurrentFile;
	iPtr<iFileName> fCurrentFileName;

};
//---------------------------------------------------------------------------
int						FileSystemOpenFD(const char *FileName,eFileAccess AccessMode,eFileCreate CreateFlag);
iPtr<iFileSyncStream>	FileSystemOpenSeqStream(const char *FileName,eFileAccess AccessMode,eFileCreate CreateFlag);
uInt64					FileSystemGetFileSize(const char *FileName);
bool					FileSystemIsFileExists(const char *FileName);
//---------------------------------------------------------------------------
rPtr<iFileEnumerator>	FileSystemEnum(const char *FolderName,uIntn FolderNameLength,const uChar16 *Filter);
iPtr<iFileName>			FileSystemCreateTemporaryFile(const char *FolderName);
//---------------------------------------------------------------------------
rPtr<iSwapMemory>		FileSystemCreateSwapMemoryFromTemporaryFile(uInt64 Size);
rPtr<iSwapMemory>		FileSystemCreateSwapMemoryFromFile(const char *FileName,uInt64 Size,eFileAccess AccessMode);
//---------------------------------------------------------------------------
}	// namespace siPOSIX
}	// namespace cnLibrary
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
#endif	// __cplusplus
/*-------------------------------------------------------------------------*/
#endif
