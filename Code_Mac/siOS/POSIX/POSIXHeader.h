/*--- POSIX - Header ------------------------------------------------------*/
/*         Developer : Code009                                             */
/*         Create on : 2020-09-09                                          */
/*-------------------------------------------------------------------------*/
#ifndef __cnLib_siOS_POSIX_POSIXHeader_H__
#define __cnLib_siOS_POSIX_POSIXHeader_H__

#include <TargetConditionals.h>

#include <unistd.h>
#include <errno.h>

/* POSIX_Memory*/
/* _POSIX_MEMORY_PROTECTION */

#include <sys/mman.h>
#define	siOS_POSIX_ENABLE_MAPFILE


/* POSIX_LOCALE */

#if	TARGET_OS_OSX

#include <iconv.h>
#define	siOS_POSIX_ENABLE_CODESETCONVERT

#endif

/* POSIX_Thread */

#include <pthread.h>
#define	siOS_POSIX_ENABLE_THREAD
#define	siOS_POSIX_ENABLE_THREAD_PRIORITY

/* POSIX_FILE */

#include <fcntl.h>
#include <fts.h>
#include <sys/stat.h>

/* POSIX_Socket */

#include <sys/socket.h>
#include <netinet/in.h>


#include <cnRTL/cnRTL.h>

/*-------------------------------------------------------------------------*/
#ifdef	__cplusplus
//---------------------------------------------------------------------------
namespace cnLibrary{
namespace siPOSIX{
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
}	// namespace siPOSIX
//---------------------------------------------------------------------------
}	// namespace cnLibrary
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
#endif	// __cplusplus
/*-------------------------------------------------------------------------*/
#endif
