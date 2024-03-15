/*- POSIX - File System ---------------------------------------------------*/
/*         Developer : Code009                                             */
/*         Create on : 2021-08-01                                          */
/*-------------------------------------------------------------------------*/
#ifndef __cnLibrary_cnSystem_POSIX_FileSystem_H__
#define	__cnLibrary_cnSystem_POSIX_FileSystem_H__
/*-------------------------------------------------------------------------*/
#include <cnSystem/cnFundamental.h>
#include <cnSystem/cnFileSystem.h>
/*-------------------------------------------------------------------------*/
#ifdef __cplusplus
//---------------------------------------------------------------------------
#ifdef	cnLibrary_CPPEXCLUDE_VIRTUAL_OVERRIDE
#define	override
#endif
//---------------------------------------------------------------------------
namespace cnLibrary{
//---------------------------------------------------------------------------
class cnLib_INTERFACE iPOSIXFileName : public iFileName
{
public:
	struct tInterfaceID{	static iTypeID Value;	};
	virtual void* CastInterface(iTypeID ID)noexcept(true) override{		return cnVar::ImplementCastInterface<iFileName>(this,ID);	}

	virtual const uChar8* cnLib_FUNC GetPOSIXFileName(void)=0;
	virtual uIntn cnLib_FUNC GetPOSIXFileNameLength(void)=0;
};
//---------------------------------------------------------------------------
namespace cnSystem{
//---------------------------------------------------------------------------
rPtr<iSwapMemory>	cnLib_FUNC CreateSwapMemoryFromFile(iFile *File,uInt64 Size,eFileAccess AccessMode);
//---------------------------------------------------------------------------
}   // namespace cnSystem
//---------------------------------------------------------------------------
}	// namespace cnLibrary
//---------------------------------------------------------------------------
#ifdef	cnLibrary_CPPEXCLUDE_VIRTUAL_OVERRIDE
#undef	override
#endif
//---------------------------------------------------------------------------
#endif  /* __cplusplus */
/*-------------------------------------------------------------------------*/
#endif
