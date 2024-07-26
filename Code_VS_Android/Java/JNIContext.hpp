/*- Java - Android - Context ----------------------------------------------*/
/*         Developer : Code009                                             */
/*         Create on : 2024-04-29                                          */
/*-------------------------------------------------------------------------*/
#pragma once
/*-------------------------------------------------------------------------*/
#include <Java/JNIJavaBase.hpp>

//---------------------------------------------------------------------------
namespace cnLibrary{
//---------------------------------------------------------------------------
namespace jCPP{
//---------------------------------------------------------------------------
namespace android{
//---------------------------------------------------------------------------
namespace content{
//---------------------------------------------------------------------------
namespace res{
//---------------------------------------------------------------------------
struct jcAssetManager : jcObject
{
	static constexpr const char jClassName[]="android/content/res/AssetManager";
};
//---------------------------------------------------------------------------
}	// namespace res
//---------------------------------------------------------------------------
struct jcContext : jcObject
{
	static constexpr const char jClassName[]="android/content/Context";

	static constexpr const char jname_BLUETOOTH_SERVICE[]="BLUETOOTH_SERVICE";
	static jStaticFieldAccess<jcString*> BLUETOOTH_SERVICE(void)noexcept{
		return jStaticFieldAccessMaker<jcContext,jname_BLUETOOTH_SERVICE>();
	}

	static constexpr const char jname_getSystemService[]="getSystemService";
	jrLocal<jcObject> getSystemService(JNIEnv *env,jcString *name)noexcept{
		return jMethodCall<jname_getSystemService,&jcContext::getSystemService>::Call(env,this,name);
	}
	static constexpr const char jname_getFilesDir[]="getFilesDir";
	jrLocal<java::io::jcFile> getFilesDir(JNIEnv *env)noexcept{
		return jMethodCall<jname_getFilesDir,&jcContext::getFilesDir>::Call(env,this);
	}

	static constexpr const char jname_getExternalFilesDir[]="getExternalFilesDir";
	jrLocal<java::io::jcFile> getExternalFilesDir(JNIEnv *env,jcString *type)noexcept{
		return jMethodCall<jname_getExternalFilesDir,&jcContext::getExternalFilesDir>::Call(env,this,type);
	}

	static constexpr const char jname_getAssets[]="getAssets";
	jrLocal<res::jcAssetManager> getAssets(JNIEnv *env)noexcept{
		return jMethodCall<jname_getAssets,&jcContext::getAssets>::Call(env,this);
	}
};
//---------------------------------------------------------------------------
}	// namespace content
//---------------------------------------------------------------------------
}	// namespace android
//---------------------------------------------------------------------------
}	// namespace jCPP
//---------------------------------------------------------------------------
}	// namespace cnLibrary
//---------------------------------------------------------------------------

