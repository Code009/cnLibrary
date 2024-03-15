/*--- POSIX - Memory ------------------------------------------------------*/
/*         Developer : Code009                                             */
/*         Create on : 2012-05-26                                          */
/*-------------------------------------------------------------------------*/
#ifndef __cnLib_siOS_POSIX_POSIX_Memory_H__
#define __cnLib_siOS_POSIX_POSIX_Memory_H__

#include <siOS/POSIX/POSIXHeader.h>
#include <cnSystem/cnMemory.h>

/*-------------------------------------------------------------------------*/
#ifdef	__cplusplus
//---------------------------------------------------------------------------
namespace cnLibrary{
namespace siPOSIX{
//---------------------------------------------------------------------------
#if	0
namespace VirtualMemory{
//---------------------------------------------------------------------------
void* cnLib_FUNC Allocate(void *Address,uIntn &Size);
bool cnLib_FUNC Commit(void *Address,uIntn Size,int ProtectFlag);
bool cnLib_FUNC Decommit(void *Address,uIntn Size);
bool cnLib_FUNC Free(void *Address,uIntn Size);
bool cnLib_FUNC Protect(void *Address,uIntn Size,int ProtectFlag);
bool cnLib_FUNC Discard(void *Address,uIntn Size);
bool cnLib_FUNC Lock(void *Address,uIntn Size);
bool cnLib_FUNC Unlock(void *Address,uIntn Size);
uIntn cnLib_FUNC GetPageSize(void);
uIntn cnLib_FUNC GetAllocationGranularity(void);
//---------------------------------------------------------------------------
}	// namespace VirtualMemory
#endif
//---------------------------------------------------------------------------
#ifdef	siOS_POSIX_ENABLE_MAPFILE
//---------------------------------------------------------------------------
class cMMapFile : public iSwapMemory
{
	// _POSIX_MAPPED_FILE
	// _POSIX_SHARED_MEMORY_OBJECTS
	// _POSIX_TYPED_MEMORY_OBJECTS
public:
	cMMapFile(int FileHandle,uIntn FileSize,int ProtectFlag=PROT_READ|PROT_WRITE);
	~cMMapFile();

	virtual uInt64	cnLib_FUNC GetSize(void)override;
	virtual void*	cnLib_FUNC Map(uInt64 Offset,uIntn Size)override;
	virtual bool	cnLib_FUNC Unmap(void *Pointer,uIntn Size)override;
protected:
	int fFileHandle;
	int fProtectFlag;
	uInt64 fFileSize;
};
//---------------------------------------------------------------------------
#endif	// siOS_POSIX_ENABLE_MAPFILE
//---------------------------------------------------------------------------
}	// namespace siPOSIX
}	// namespace cnLibrary
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
#endif	// __cplusplus
/*-------------------------------------------------------------------------*/
#endif
