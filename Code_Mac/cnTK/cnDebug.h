/*- Debug -----------------------------------------------------------------*/
/*         Developer : Code009                                             */
/*         Create on : 2015-11-20                                          */
/*-------------------------------------------------------------------------*/
#pragma once

#include <assert.h>

#ifdef	DEBUG
#define	cnLib_DEBUG
#endif


//#define	cnLib_LOCAL	//__attribute__((visibility("hidden")))

/*-------------------------------------------------------------------------*/
#include <cnTK/cnPlatform.h>
#ifdef __cplusplus

//---------------------------------------------------------------------------
#define cnLib_TO_STR(_arg_)  #_arg_
#define	cnLib_EXP_TO_STR(_arg_)	cnLib_TO_STR(_arg_)

#define cnLib_FILE_LINE	__FILE__ "(" cnLib_EXP_TO_STR(__LINE__) ")"


//---------------------------------------------------------------------------
// assert
#ifdef	cnLib_DEBUG
#define	cnLib_ASSERT(_x_)	assert(_x_)
#else
#define	cnLib_ASSERT(_x_)	((void)(_x_))
#endif
//---------------------------------------------------------------------------
namespace cnLibrary{

//---------------------------------------------------------------------------
static_assert(sizeof(uIntn)==sizeof(void*),	"incorrect size of uIntn");
static_assert(sizeof(sIntn)==sizeof(void*),	"incorrect size of sIntn");

static_assert(sizeof(uInt64)==8,"incorrect size of uInt64");
static_assert(sizeof(sInt64)==8,"incorrect size of sInt64");
static_assert(sizeof(uInt32)==4,"incorrect size of uInt32");
static_assert(sizeof(sInt32)==4,"incorrect size of sInt32");
static_assert(sizeof(uInt16)==2,"incorrect size of uInt16");
static_assert(sizeof(sInt16)==2,"incorrect size of sInt16");
static_assert(sizeof(uInt8)==1,	"incorrect size of uInt8");
static_assert(sizeof(sInt8)==1,	"incorrect size of sInt8");
//---------------------------------------------------------------------------
}   // namespace cnLibrary
//---------------------------------------------------------------------------
#endif  /* __cplusplus */
/*-------------------------------------------------------------------------*/
