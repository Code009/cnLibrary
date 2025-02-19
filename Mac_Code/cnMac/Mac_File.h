/*--- Mac - Socket --------------------------------------------------------*/
/*         Developer : Code009                                             */
/*         Create on : 2012-05-31                                          */
/*-------------------------------------------------------------------------*/

#pragma once

#include <sys/aio.h>
#include <fts.h>
#include <dispatch/dispatch.h>

//---------------------------------------------------------------------------
#include <siOS/POSIX/POSIX_AIO.h>
#include <siOS/POSIX/POSIX_File.h>

#include <cnMac/Mac_Header.h>
/*-------------------------------------------------------------------------*/
#ifdef	__cplusplus
//---------------------------------------------------------------------------
namespace cnLibrary{
namespace cnMac{
//---------------------------------------------------------------------------
class cAIOTaskBlockGCDManager
{
public:
	cAIOTaskBlockGCDManager();
	~cAIOTaskBlockGCDManager();

	operator siPOSIX::cAIOTaskBlockManager*()const;

private:
	dispatch_source_t fGCDSource;

	static void ContextDelete(void *p);
	static void SourceProcedure(void *p);

	siPOSIX::cAIOTaskBlockManager *fManager;

};
extern cAIOTaskBlockGCDManager gAIOTaskBlockGCDManager;
//---------------------------------------------------------------------------
class cAIOFileStream : public iFileStream
{
public:
	cAIOFileStream(int FileHandle);
	~cAIOFileStream();
	
	virtual void cnLib_FUNC Close(void)override;

	virtual bool cnLib_FUNC Read(void *Buffer,uIntn Size,uInt64 Offset,uIntn &SizeCompleted)override;
	virtual bool cnLib_FUNC Write(const void *Buffer,uIntn Size,uInt64 Offset,uIntn &SizeCompleted)override;

	virtual iPtr<iStreamTask> cnLib_FUNC ReadAsync(void *Buffer,uIntn Size,uInt64 Offset)override;
	virtual iPtr<iStreamTask> cnLib_FUNC WriteAsync(const void *Buffer,uIntn Size,uInt64 Offset)override;

	virtual bool cnLib_FUNC GetSize(uInt64 &Size)override;
	virtual bool cnLib_FUNC SetSize(uInt64 NewSize)override;
	virtual bool cnLib_FUNC FlushBuffer(void)override;
private:
	int fFileHandle;

	cnRTL::iSharedObjectRecycler<siPOSIX::cAIOStreamTask> fTaskRecycler;
};
//---------------------------------------------------------------------------
class cCRTErrorReport : public iStreamErrorReport
{
public:
	virtual StreamError GetStreamError(void)override;
};
//---------------------------------------------------------------------------
iPtr<iFileName>			FileSystemCreateTemporaryFile(const char *FolderName);
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
}	// namespace cnMac
}	// namespace cnLibrary
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
#endif	// __cplusplus
/*-------------------------------------------------------------------------*/

