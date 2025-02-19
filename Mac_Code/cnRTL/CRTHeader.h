#ifndef __cnLibrary_cncnRTL_CRTHeader_H__
#define __cnLibrary_cncnRTL_CRTHeader_H__

#include <memory>
#include <string>
#include <type_traits>
#include <atomic>
#include <functional>
#include <iterator>
#include <algorithm>
#include <math.h>

#include <mutex>
#include <condition_variable>

//#include <experimental/coroutine>

#ifndef _LIBCPP_HAS_NO_COROUTINES
#define	cnLibrary_STL_EXPERIMENTAL_COROUTINE
#endif


#include <cnTK/cnTK.h>

#define cnCRT_BYTEORDER_LITTLEENDIAN	1
#define cnCRT_BYTEORDER_BIGENDIAN		0
#endif
