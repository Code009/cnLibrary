#include "POSIX_Memory.h"

using namespace cnLibrary;
using namespace cnRTL;
using namespace siPOSIX;

//---------------------------------------------------------------------------
#if	0

#include <sys/file.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
//---------------------------------------------------------------------------
static int ProtectLib2Sys(int Protect)
{
	static constexpr int ProtectFlag[]={
		PROT_NONE,				//NoAccess
		PROT_READ,				//Read
		PROT_READ|PROT_WRITE,	//Write
		PROT_READ|PROT_WRITE,	//ReadWrite
		PROT_EXEC,				//Execute
		PROT_EXEC|PROT_READ,	//ExecuteRead
		PROT_EXEC|PROT_READ|PROT_WRITE,	//ExecuteWrite
		PROT_EXEC|PROT_READ|PROT_WRITE,	//ExecuteReadWrite
	};
	auto pi=static_cast<uInt8>(Protect);
	if(pi>=cnMemory::ArrayLength(ProtectFlag))
		return PROT_NONE;
	return ProtectFlag[pi];
}
//---------------------------------------------------------------------------
void* cnLib_FUNC VirtualMemory::Allocate(void *Address,uIntn &Size)
{
	int flag=MAP_SHARED;
	if(Address!=nullptr)
		flag|=MAP_FIXED;
	void *NewAddress=mmap(Address,Size,PROT_NONE,flag,0,0);
	if(NewAddress==nullptr)
		return nullptr;
	uIntn NewSize=Size;
	// TODO calculate actual size
	// mark as no use
	madvise(NewAddress,NewSize,MADV_DONTNEED);
	return NewAddress;
}
//---------------------------------------------------------------------------
bool cnLib_FUNC VirtualMemory::Commit(void *Address,uIntn Size,int ProtectFlag)
{
	int pflag=ProtectLib2Sys(ProtectFlag);
	if(mprotect(Address,Size,pflag)==0){
		// mark as will use
		madvise(Address,Size,MADV_WILLNEED);
		return true;
	}
	return false;
}
//---------------------------------------------------------------------------
bool cnLib_FUNC VirtualMemory::Decommit(void *Address,uIntn Size)
{
	if(mprotect(Address,Size,PROT_NONE)!=0)
		return false;
	// mark as discard
	madvise(Address,Size,MADV_DONTNEED);
	return true;
}
//---------------------------------------------------------------------------
bool cnLib_FUNC VirtualMemory::Free(void *Address,uIntn Size)
{
	return munmap(Address,Size)==0;
}
//---------------------------------------------------------------------------
bool cnLib_FUNC VirtualMemory::Protect(void *Address,uIntn Size,int ProtectFlag)
{
	return mprotect(Address,Size,ProtectLib2Sys(ProtectFlag))==0;
}
//---------------------------------------------------------------------------
bool cnLib_FUNC VirtualMemory::Discard(void *Address,uIntn Size)
{
	// mark as discard
	madvise(Address,Size,MADV_DONTNEED);
	return true;
}
//---------------------------------------------------------------------------
bool cnLib_FUNC VirtualMemory::Lock(void *Address,uIntn Size)
{
	return mlock(Address,Size)==0;
}
//---------------------------------------------------------------------------
bool cnLib_FUNC VirtualMemory::Unlock(void *Address,uIntn Size)
{
	return munlock(Address,Size)==0;
}
//---------------------------------------------------------------------------
uIntn cnLib_FUNC VirtualMemory::GetPageSize(void)
{
	return ::getpagesize();
}
//---------------------------------------------------------------------------
uIntn cnLib_FUNC VirtualMemory::GetAllocationGranularity(void)
{
	return ::getpagesize();
}
#endif
//---------------------------------------------------------------------------
#ifdef	siOS_POSIX_ENABLE_MAPFILE
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cMMapFile::cMMapFile(int FileHandle,uIntn FileSize,int ProtectFlag)
{
	fFileHandle=FileHandle;
	fFileSize=FileSize;
	fProtectFlag=ProtectFlag;
}
//---------------------------------------------------------------------------
cMMapFile::~cMMapFile()
{
	::close(fFileHandle);
}
//---------------------------------------------------------------------------
uInt64 cMMapFile::GetSize(void)
{
	return fFileSize;
}
//---------------------------------------------------------------------------
void* cMMapFile::Map(uInt64 Offset,uIntn Size)
{
	return mmap(nullptr,Size,fProtectFlag,MAP_SHARED,fFileHandle,Offset);
}
//---------------------------------------------------------------------------
bool cMMapFile::Unmap(void *Pointer,uIntn Size)
{
	return munmap(Pointer,Size)==0;
}
//---------------------------------------------------------------------------
#endif	// siOS_POSIX_ENABLE_MAPFILE
//---------------------------------------------------------------------------

