#include <sys/stat.h>
#include "Mac_File.h"
#include "Mac_GCD.h"
#include <siOS/POSIX/POSIX_Memory.h>

using namespace cnLibrary;
using namespace cnRTL;
using namespace cnMac;

//---------------------------------------------------------------------------
StreamError cCRTErrorReport::GetStreamError(void)
{
	auto ErrorCode=errno;
	switch(ErrorCode){
	default:
		return StreamError::Unknown;
	case EINTR:
		return StreamError::Cancelled;
	case EINVAL:
		return StreamError::AccessError;
	case EAGAIN:
		return StreamError::AccessError;
	}
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cAIOTaskBlockGCDManager::cAIOTaskBlockGCDManager()
{
	fManager=new siPOSIX::cAIOTaskBlockManager;

	fGCDSource=dispatch_source_create(DISPATCH_SOURCE_TYPE_SIGNAL,SIGIO,0,GCDDefaultQueue());
	dispatch_set_context(fGCDSource,fManager);
	dispatch_set_finalizer_f(fGCDSource,ContextDelete);
	dispatch_source_set_event_handler_f(fGCDSource,SourceProcedure);
	dispatch_resume(fGCDSource);
}
//---------------------------------------------------------------------------
cAIOTaskBlockGCDManager::~cAIOTaskBlockGCDManager()
{
	dispatch_source_cancel(fGCDSource);
	dispatch_release(fGCDSource);
}
//---------------------------------------------------------------------------
void cAIOTaskBlockGCDManager::ContextDelete(void *p)
{
	delete static_cast<siPOSIX::cAIOTaskBlockManager*>(p);
}
//---------------------------------------------------------------------------
void cAIOTaskBlockGCDManager::SourceProcedure(void *p)
{
	static_cast<siPOSIX::cAIOTaskBlockManager*>(p)->AIOSignalEvent();
}
//---------------------------------------------------------------------------
cAIOTaskBlockGCDManager::operator siPOSIX::cAIOTaskBlockManager*()const
{
	return fManager;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cAIOFileStream::cAIOFileStream(int FileHandle)
{
	fFileHandle=FileHandle;
}
//---------------------------------------------------------------------------
cAIOFileStream::~cAIOFileStream()
{
}
//---------------------------------------------------------------------------
void cAIOFileStream::Close(void)
{
	close(fFileHandle);
}
//---------------------------------------------------------------------------
bool cAIOFileStream::Read(void *Buffer,uIntn Size,uInt64 Offset,uIntn &SizeCompleted)
{
	auto ret=pread(fFileHandle,Buffer,Size,Offset);
	if(ret>=0){
		SizeCompleted=ret;
		return true;
	}
	return false;
}
//---------------------------------------------------------------------------
bool cAIOFileStream::Write(const void *Buffer,uIntn Size,uInt64 Offset,uIntn &SizeCompleted)
{
	auto ret=pwrite(fFileHandle,Buffer,Size,Offset);
	if(ret>=0){
		SizeCompleted=ret;
		return true;
	}
	return false;
}
//---------------------------------------------------------------------------
iPtr<iStreamTask> cAIOFileStream::ReadAsync(void *Buffer,uIntn Size,uInt64 Offset)
{
	auto Task=fTaskRecycler.Query();
	Task->AIORegister(gAIOTaskBlockGCDManager);

	if(Task->AIORead(fFileHandle,Buffer,Size,Offset))
		return Task;
	Task->AIODiscard(gAIOTaskBlockGCDManager);
	return nullptr;
}
//---------------------------------------------------------------------------
iPtr<iStreamTask> cAIOFileStream::WriteAsync(const void *Buffer,uIntn Size,uInt64 Offset)
{
	auto Task=fTaskRecycler.Query();
	Task->AIORegister(gAIOTaskBlockGCDManager);
	if(Task->AIOWrite(fFileHandle,Buffer,Size,Offset))
		return Task;
	Task->AIODiscard(gAIOTaskBlockGCDManager);
	return nullptr;
}
//---------------------------------------------------------------------------
bool cAIOFileStream::GetSize(uInt64 &Size)
{
	struct stat FileState;
	if(fstat(fFileHandle,&FileState)!=0)
		return false;
	Size=FileState.st_size;
	return true;
}
//---------------------------------------------------------------------------
bool cAIOFileStream::SetSize(uInt64 NewSize)
{
	off_t size=NewSize;
	return ftruncate(fFileHandle,size)==0;
}
//---------------------------------------------------------------------------
bool cAIOFileStream::FlushBuffer(void)
{
	return false;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
iPtr<iFileName> cnMac::FileSystemCreateTemporaryFile(const char *FolderName)
{
	char TempFileTemplate[]="tmpXXXXXX.tmp";
	mkstemp(TempFileTemplate);

	cStringBuffer<char> Name=FolderName;
	Name+=TempFileTemplate;
	return iCreate<siPOSIX::cStringFileName>(siPOSIX::cStringFileName::cStringPath(cnVar::MoveCast(Name)));
}
//---------------------------------------------------------------------------
iPtr<iFileStream>	siPOSIX::POSIXInterface::FSOpenFileStream(const char *FileName,eFileAccess AccessMode,eFileCreate CreateFlag)
{
	int fd=siPOSIX::FileSystemOpenFD(FileName,AccessMode,CreateFlag);
	if(fd<0)
		return nullptr;
	return iCreate<cAIOFileStream>(fd);
}
//---------------------------------------------------------------------------

