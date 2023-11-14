/*--- POSIX - Memory ------------------------------------------------------*/
/*         Developer : Code009                                             */
/*         Create on : 2012-05-26                                          */
/*-------------------------------------------------------------------------*/
#ifndef __cnLib_siOS_Mach_Memory_H__
#define __cnLib_siOS_Mach_Memory_H__

#include <cnSystem/cnSystem.h>
/*-------------------------------------------------------------------------*/
#ifdef	__cplusplus
//---------------------------------------------------------------------------
namespace cnLibrary{
namespace siMach{
//---------------------------------------------------------------------------
#if	0
namespace VM{
//---------------------------------------------------------------------------
	void* cnLib_FUNC Reserve(void *Address,uintn Size);
	void* cnLib_FUNC Commit(void *Address,uintn Size,VMProtect ProtectFlag);
	bool cnLib_FUNC Decommit(void *Address,uintn Size);
	bool cnLib_FUNC Free(void *Address,uintn Size);
	bool cnLib_FUNC Protect(void *Address,uintn Size,VMProtect ProtectFlag);
	bool cnLib_FUNC Discard(void *Address,uintn Size);
	bool cnLib_FUNC Lock(void *Address,uintn Size);
	bool cnLib_FUNC Unlock(void *Address,uintn Size);
	uint8 cnLib_FUNC GetPageSizeB(void);
	uint8 cnLib_FUNC GetAllocationGranularityB(void);
//---------------------------------------------------------------------------
}	// namespace VM
//---------------------------------------------------------------------------
class cMMapFile : public iSwapMemory
{
public:
	cMMapFile(int FileHandle,int ProtectFlag=VM_PROT_READ|VM_PROT_WRITE);

	virtual uInt64	cnLib_FUNC GetSize(void)override;
	virtual void*	cnLib_FUNC Map(uIntn Offset,uIntn Size)override;
	virtual bool	cnLib_FUNC Unmap(void *Pointer,uIntn Size)override;

private:
	int fFileHandle;
	int ProtectFlag;
	uint64 fFileSize;

};
#endif
//---------------------------------------------------------------------------
}	// namespace siMach
}	// namespace cnLibrary
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
#endif	// __cplusplus
/*-------------------------------------------------------------------------*/
#endif
