/*- Java - Android - basic class ------------------------------------------*/
/*         Developer : Code009                                             */
/*         Create on : 2024-04-01                                          */
/*-------------------------------------------------------------------------*/
#pragma once
/*-------------------------------------------------------------------------*/
#include <Java/JNICPP.hpp>

//---------------------------------------------------------------------------
namespace cnLibrary{
//---------------------------------------------------------------------------
namespace jCPP{
//---------------------------------------------------------------------------
namespace java{
//---------------------------------------------------------------------------
namespace lang{
//---------------------------------------------------------------------------
struct jcRunnable : jcObject
{
	static constexpr const char jClassName[]="java/lang/Runnable";

	static constexpr const char jname_run[]="run";
	void run(JNIEnv *env)noexcept{
		return jMethodCall<jname_run,&jcRunnable::run>::Call(env,this);
	}
};
//---------------------------------------------------------------------------
}	// namespace lang
//---------------------------------------------------------------------------
namespace util{
//---------------------------------------------------------------------------
struct jbcList : jcObject
{
	static constexpr const char jClassName[]="java/util/List";

	static constexpr const char jname_get[]="get";
	jrLocal<jcObject> get(JNIEnv *env,jint Index)noexcept{
		return jMethodCall<jname_get,&jbcList::get>::Call(env,this,Index);
	}
	
	static constexpr const char jname_size[]="size";
	jint size(JNIEnv *env)noexcept{
		return jMethodCall<jname_size,&jbcList::size>::Call(env,this);
	}
};
//---------------------------------------------------------------------------
template<class TClass>
struct jcList;
//---------------------------------------------------------------------------
template<class TClass>
struct jcList<TClass*> : jbcList
{

	jrLocal<TClass> get(JNIEnv *env,jint Index)noexcept{
		return jbcList::get(env,Index).template Transfer<TClass>();
	}
};
//---------------------------------------------------------------------------
struct jcUUID : jcObject
{
	static constexpr const char jClassName[]="java/util/UUID";
	
	static void ConvertUUID(jlong &msb,jlong &lsb,const cUUID &ID)noexcept
	{
		auto pID=reinterpret_cast<const uInt64*>(&ID);

		lsb=cnMemory::SwapBigEndian(pID[1]);
		msb=pID[0]>>48;
		msb|=(pID[0]>>16)&0xFFFF0000;
		msb|=pID[0]<<32;
	}
	static jrLocal<jcUUID> Make(JNIEnv *env,jlong msb,jlong lsb)noexcept{
		return jNew<jcUUID>(env,msb,lsb);
	}
	static jrLocal<jcUUID> Make(JNIEnv *env,const cUUID &ID)noexcept{
		jlong msb,lsb;
		ConvertUUID(msb,lsb,ID);
		return jNew<jcUUID>(env,msb,lsb);
	}
	
	static constexpr const char jname_getLeastSignificantBits[]="getLeastSignificantBits";
	jlong getLeastSignificantBits(JNIEnv *env)noexcept{
		return jMethodCall<jname_getLeastSignificantBits,&jcUUID::getLeastSignificantBits>::Call(env,this);
	}
	static constexpr const char jname_getMostSignificantBits[]="getMostSignificantBits";
	jlong getMostSignificantBits(JNIEnv *env)noexcept{
		return jMethodCall<jname_getMostSignificantBits,&jcUUID::getMostSignificantBits>::Call(env,this);
	}


	static constexpr const char jname_fromString[]="fromString";
	static jrLocal<jcUUID> fromString(JNIEnv *env,jcString *uuidString)noexcept{
		return jStaticMethodCall<jcUUID,jname_fromString,fromString>::Call(env,uuidString);
	}

	cUUID ToUUID(JNIEnv *env)noexcept
	{
		union{
			cUUID UUID;
			jlong IDValue[2];
		};
		jlong lsb=getLeastSignificantBits(env);
		jLogException(env);
		jlong msb=getMostSignificantBits(env);
		jLogException(env);

		IDValue[1]=cnMemory::SwapBigEndian(lsb);
		IDValue[0]=msb>>32;
		IDValue[0]|=(msb<<16)&0xFFFF00000000;
		IDValue[0]|=msb<<48;
		return UUID;
	}
};
//---------------------------------------------------------------------------
struct jcParcelUuid : jcObject
{
	static constexpr const char jClassName[]="android/os/ParcelUuid";

	static constexpr const char jname_getUuid[]="getUuid";
	jrLocal<jcUUID> getUuid(JNIEnv *env)noexcept{
		return jMethodCall<jname_getUuid,&jcParcelUuid::getUuid>::Call(env,this);
	}
};
//---------------------------------------------------------------------------
}	// namespace util
//---------------------------------------------------------------------------
namespace io{
//---------------------------------------------------------------------------
struct jcFile : jcObject
{
	static constexpr const char jClassName[]="java/io/File";

	static jrLocal<jcFile> Make(JNIEnv *env,const uChar16 *Path,uIntn Length)noexcept{
		auto String=jcString::Make(env,Path,Length);
		return jNew<jcFile>(env,String.Get());
	}

	static constexpr const char jname_getPath[]="getPath";
	jrLocal<jcString> getPath(JNIEnv *env)noexcept{
		return jMethodCall<jname_getPath,&jcFile::getPath>::Call(env,this);
	}
};
//---------------------------------------------------------------------------
}	// namespace io
//---------------------------------------------------------------------------
}	// namespace java
//---------------------------------------------------------------------------
}	// namespace jCPP
//---------------------------------------------------------------------------
}	// namespace cnLibrary
//---------------------------------------------------------------------------
