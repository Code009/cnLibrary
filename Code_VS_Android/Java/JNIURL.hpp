/*- Java - Android - URL --------------------------------------------------*/
/*         Developer : Code009                                             */
/*         Create on : 2024-09-13                                          */
/*-------------------------------------------------------------------------*/
#pragma once
/*-------------------------------------------------------------------------*/
#include <Java/JNIJavaBase.hpp>

//---------------------------------------------------------------------------
namespace cnLibrary{
//---------------------------------------------------------------------------
namespace jCPP{
//---------------------------------------------------------------------------
namespace java{
//---------------------------------------------------------------------------
namespace io{
//---------------------------------------------------------------------------
struct jcInputStream : jcObject
{
	static constexpr const char jClassName[]="java/io/InputStream";

	static constexpr const char jname_close[]="close";
	void close(JNIEnv *env)noexcept{
		return jMethodCall<jname_close,&jcInputStream::close>::Call(env,this);
	}

	static constexpr const char jname_read[]="read";
	int read(JNIEnv *env,jcArray<jbyte> *b)noexcept{
		return jMethodCall<jname_read,&jcInputStream::read>::Call(env,this,b);
	}

	int read_off(JNIEnv *env,jcArray<jbyte> *b,jint off,jint len)noexcept{
		return jMethodCall<jname_read,&jcInputStream::read_off>::Call(env,this,b,off,len);
	}

	static constexpr const char jname_readAllBytes[]="readAllBytes";
	jrLocal< jcArray<jbyte> > readAllBytes(JNIEnv *env)noexcept{
		return jMethodCall<jname_readAllBytes,&jcInputStream::readAllBytes>::Call(env,this);
	}
};
//---------------------------------------------------------------------------
struct jcOutputStream : jcObject
{
	static constexpr const char jClassName[]="java/io/OutputStream";

	static constexpr const char jname_close[]="close";
	void close(JNIEnv *env)noexcept{
		return jMethodCall<jname_close,&jcOutputStream::close>::Call(env,this);
	}
};
//---------------------------------------------------------------------------
}	// namespace io
//---------------------------------------------------------------------------
namespace net{
//---------------------------------------------------------------------------
struct jcURLConnection;
//---------------------------------------------------------------------------
struct jcURL : jcObject
{
	static constexpr const char jClassName[]="java/net/URL";

	static constexpr const char jname_openConnection[]="openConnection";
	jrLocal<jcURLConnection> openConnection(JNIEnv *env)noexcept{
		return jMethodCall<jname_openConnection,&jcURL::openConnection>::Call(env,this);
	}
};
//---------------------------------------------------------------------------
struct jcURLConnection : jcObject
{
	static constexpr const char jClassName[]="java/net/URLConnection";
	
	static constexpr const char jname_setIfModifiedSince[]="setIfModifiedSince";
	void setIfModifiedSince(JNIEnv *env,long ifmodifiedsince)noexcept{
		return jMethodCall<jname_setIfModifiedSince,&jcURLConnection::setIfModifiedSince>::Call(env,this,ifmodifiedsince);
	}

	static constexpr const char jname_getInputStream[]="getInputStream";
	jrLocal<io::jcInputStream> getInputStream(JNIEnv *env)noexcept{
		return jMethodCall<jname_getInputStream,&jcURLConnection::getInputStream>::Call(env,this);
	}

	static constexpr const char jname_getOutputStream[]="getOutputStream";
	jrLocal<io::jcOutputStream> getOutputStream(JNIEnv *env)noexcept{
		return jMethodCall<jname_getOutputStream,&jcURLConnection::getOutputStream>::Call(env,this);
	}

	static constexpr const char jname_getContentLength[]="getContentLength";
	jint getContentLength(JNIEnv *env)noexcept{
		return jMethodCall<jname_getContentLength,&jcURLConnection::getContentLength>::Call(env,this);
	}

	static constexpr const char jname_getLastModified[]="getLastModified";
	jlong getLastModified(JNIEnv *env)noexcept{
		return jMethodCall<jname_getLastModified,&jcURLConnection::getLastModified>::Call(env,this);
	}
};
//---------------------------------------------------------------------------
}	// namespace net
//---------------------------------------------------------------------------
}	// namespace java
//---------------------------------------------------------------------------
}	// namespace jCPP
//---------------------------------------------------------------------------
}	// namespace cnLibrary
//---------------------------------------------------------------------------
