/*- cnSystem - File System ------------------------------------------------*/
/*         Developer : Code009                                             */
/*         Create on : 2009-10-26                                          */
/*-------------------------------------------------------------------------*/
#ifndef __cnLibrary_cnSystem_cnFileSystem_H__
#define	__cnLibrary_cnSystem_cnFileSystem_H__
/*-------------------------------------------------------------------------*/
#include <cnSystem/cnFundamental.h>
#include <cnSystem/cnData.h>
#include <cnSystem/cnMemory.h>
#include <cnSystem/cnStream.h>
/*-------------------------------------------------------------------------*/
#ifdef __cplusplus
//---------------------------------------------------------------------------
#ifdef	cnLibrary_CPPEXCLUDE_VIRTUAL_OVERRIDE
#define	override
#endif
//---------------------------------------------------------------------------
namespace cnLibrary{
//---------------------------------------------------------------------------
class iFile;
//---------------------------------------------------------------------------
class cnLib_INTERFACE iFileSyncStream : public iSyncStream
{
public:
	virtual uInt64 cnLib_FUNC GetPointer(void)noexcept(true)=0;
	virtual bool cnLib_FUNC MovePointerBegin(uInt64 Offset)noexcept(true)=0;
	virtual bool cnLib_FUNC MovePointerCurrent(sInt64 Offset)noexcept(true)=0;
	virtual bool cnLib_FUNC MovePointerEnd(sInt64 Offset)noexcept(true)=0;
};
//---------------------------------------------------------------------------
class cnLib_INTERFACE iFileStream : public iInterface
{
public:
	// Close
	virtual void cnLib_FUNC Close(void)noexcept(true)=0;

	// Read
	//	read from stream
	// [in]Buffer			Buffer to send,it must remain valid for the duration of the read operation
	// [in]Size				bytes to read
	// [in]Offset			location to read
	// [out]SizeCompleted	bytes read
	// return true if read successed
	virtual bool cnLib_FUNC Read(void *Buffer,uIntn Size,uInt64 Offset,uIntn &SizeCompleted)noexcept(true)=0;
	// Write
	//	write to stream
	// [in]Buffer			Buffer to send,it must remain valid for the duration of the write operation
	// [in]Size				bytes to write
	// [in]Offset			location to write
	// [out]SizeCompleted	bytes written
	// return true if write successed
	virtual bool cnLib_FUNC Write(const void *Buffer,uIntn Size,uInt64 Offset,uIntn &SizeCompleted)noexcept(true)=0;


	// ReadAsync
	//	read from stream
	// [in]Buffer			Buffer to send,it must remain valid for the duration of the read operation
	// [in]Len				bytes to read
	// return true if read operation started
	virtual iPtr<iStreamTask> cnLib_FUNC ReadAsync(void *Buffer,uIntn Size,uInt64 Offset)noexcept(true)=0;
	// WriteAsync
	//	write to stream
	// [in]Buffer			Buffer to send,it must remain valid for the duration of the write operation
	// [in]Len				bytes to write
	// return true if write operation started
	virtual iPtr<iStreamTask> cnLib_FUNC WriteAsync(const void *Buffer,uIntn Size,uInt64 Offset)noexcept(true)=0;

	// GetSize
	//	get size of the stream
	// [out]Size			pointer to variable receiving size
	// return true if the size variable is large enough to store the size
	virtual bool cnLib_FUNC GetSize(uInt64 &Size)noexcept(true)=0;
	// SetSize
	//	change size of the stream
	// [in]Size				new size to set
	// return true if size is set
	virtual bool cnLib_FUNC SetSize(uInt64 NewSize)noexcept(true)=0;

	// FlushBuffer
	//	flush file stream internal buffers
	virtual bool cnLib_FUNC FlushBuffer(void)noexcept(true)=0;
};
//---------------------------------------------------------------------------
class cnLib_INTERFACE iFileEnumerator : public iReference
{
public:
	virtual bool cnLib_FUNC Fetch(void)noexcept(true)=0;

	virtual iFile*	cnLib_FUNC GetCurrentFile(void)noexcept(true)=0;
};
//---------------------------------------------------------------------------
cnLib_ENUM_BEGIN(ufInt8,FileChange)
{
	Update				=0x0,
	Insert				=0x1,
	Remove				=0x2,
	RenameNew			=0x3,
	RenameOld			=0x4,
}cnLib_ENUM_END(FileChange);
//---------------------------------------------------------------------------
class cnLib_INTERFACE iFileObserver : public iAsyncNotification
{
public:
	virtual iPtr<iFile> cnLib_FUNC FetchFileChange(eFileChange &Change)noexcept(true)=0;
	virtual void cnLib_FUNC DiscardChanges(void)noexcept(true)=0;
	virtual rPtr<iFileEnumerator> cnLib_FUNC ResetChanges(void)noexcept(true)=0;
};
//---------------------------------------------------------------------------
cnLib_INTENUM_BEGIN(ufInt8,FileAccess)
{
	None			=0x00,
	Read			=0x01,
	Write			=0x02,
	ReadWrite		=0x03,
	ShareRead		=0x04,
	ShareWrite		=0x08,
	ShareDelete		=0x10,
}cnLib_INTENUM_END(FileAccess);
//---------------------------------------------------------------------------
cnLib_ENUM_BEGIN(ufInt8,FileCreate)
{
	// Open
	//	open existing, fail if not exists
	Open				=0x0,
	// New
	//	new file, fail if already exists
	New					=0x1,
	// Create
	//	create file, open if existing
	Create				=0x2,
}cnLib_ENUM_END(FileCreate);
//---------------------------------------------------------------------------
class cnLib_INTERFACE iFile : public iInterface
{
public:

	virtual bool	cnLib_FUNC IsExists(void)noexcept(true)=0;
	virtual bool	cnLib_FUNC HasData(void)noexcept(true)=0;
	virtual bool	cnLib_FUNC HasFolder(void)noexcept(true)=0;
	virtual bool	cnLib_FUNC Delete(void)noexcept(true)=0;

	// data

	virtual uInt64	cnLib_FUNC GetDataSize(void)noexcept(true)=0;
	virtual iPtr<iFileSyncStream>	cnLib_FUNC OpenSequentialStream(eFileAccess AccessMode,eFileCreate CreateFlag)noexcept(true)=0;
	virtual iPtr<iFileStream>		cnLib_FUNC OpenFileStream(eFileAccess AccessMode,eFileCreate CreateFlag)noexcept(true)=0;

	// folder

	virtual rPtr<iFileEnumerator>		cnLib_FUNC EnumFolder(void)noexcept(true)=0;
	virtual rPtr<iAsyncNotification>	cnLib_FUNC WatchFolder(void)noexcept(true)=0;
	virtual rPtr<iFileObserver>			cnLib_FUNC ObserveFolder(void)noexcept(true)=0;
	virtual bool	cnLib_FUNC CreateFolder(void)noexcept(true)=0;
	virtual uIntn	cnLib_FUNC CreateFolderPath(void)noexcept(true)=0;

};
//---------------------------------------------------------------------------
class cnLib_INTERFACE iFileName : public iFile
{
public:
	struct tInterfaceID{	static iTypeID Value;	};
	virtual void* cnLib_FUNC CastInterface(iTypeID ID)noexcept(true) override{		return cnVar::ImplementCastInterface<iFile>(this,ID);	}

	virtual eiOrdering cnLib_FUNC Compare(iFileName *Dest)noexcept(true)=0;

	virtual iPtr<iFileName>	cnLib_FUNC ParentFileName(void)noexcept(true)=0;
	virtual rPtr<iFileEnumerator>	cnLib_FUNC EnumFileNames(const uChar16 *Filter)noexcept(true)=0;
	virtual const uChar16*	cnLib_FUNC GetName(void)noexcept(true)=0;
	virtual uIntn			cnLib_FUNC GetNameLength(void)noexcept(true)=0;
	virtual iPtr<iFileName>	cnLib_FUNC MakeName(const uChar16*const *Path,uIntn Depth)noexcept(true)=0;
};
//---------------------------------------------------------------------------
namespace cnSystem{
//---------------------------------------------------------------------------
rPtr<iSwapMemory>	cnLib_FUNC CreateSwapMemoryFromFile(iFile *File,uInt64 Size,eFileAccess AccessMode)noexcept(true);
//---------------------------------------------------------------------------
}   // namespace cnSystem
//---------------------------------------------------------------------------
}	// namespace cnLibrary
#ifdef	cnLibrary_CPPEXCLUDE_VIRTUAL_OVERRIDE
#undef	override
#endif
//---------------------------------------------------------------------------
#endif  /* __cplusplus */
/*-------------------------------------------------------------------------*/
#endif
