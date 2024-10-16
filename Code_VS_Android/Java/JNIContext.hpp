/*- Java - Android - Context ----------------------------------------------*/
/*         Developer : Code009                                             */
/*         Create on : 2024-04-29                                          */
/*-------------------------------------------------------------------------*/
#pragma once
/*-------------------------------------------------------------------------*/
#include <Java/JNIJavaBase.hpp>
#include <Java/JNIURL.hpp>

//---------------------------------------------------------------------------
namespace cnLibrary{
//---------------------------------------------------------------------------
namespace jCPP{
//---------------------------------------------------------------------------
namespace android{
//---------------------------------------------------------------------------
namespace net{
//---------------------------------------------------------------------------
struct jcUri : jcObject
{
	static constexpr const char jClassName[]="android/net/Uri";

};
//---------------------------------------------------------------------------
}	// namespace net
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
namespace pm{
//---------------------------------------------------------------------------
struct jcPackageInfo : jcObject
{
	static constexpr const char jClassName[]="android/content/pm/PackageInfo";
	
	static constexpr const char jname_versionName[]="versionName";
	jFieldAccess<jcString*> versionName(void)noexcept{
		return jFieldAccessMaker<jname_versionName>(this);
	}

	static constexpr const char jname_versionCode[]="versionCode";
	jFieldAccess<jint> versionCode(void)noexcept{
		return jFieldAccessMaker<jname_versionCode>(this);
	}
	
	static constexpr const char jname_getLongVersionCode[]="getLongVersionCode";
	jlong getLongVersionCode(JNIEnv *env)noexcept{
		return jMethodCall<jname_getLongVersionCode,&jcPackageInfo::getLongVersionCode>::Call(env,this);
	}

};
//---------------------------------------------------------------------------
struct jcPackageManager : jcObject
{
	static constexpr const char jClassName[]="android/content/pm/PackageManager";

	static constexpr const char jname_getPackageInfo[]="getPackageInfo";
	jrLocal<jcPackageInfo> getPackageInfo(JNIEnv *env,jcString *packageName, jint flags)noexcept{
		return jMethodCall<jname_getPackageInfo,&jcPackageManager::getPackageInfo>::Call(env,this,packageName,flags);
	}
};
//---------------------------------------------------------------------------
}	// namespace pm
//---------------------------------------------------------------------------
struct jcIntent;
struct jcIntentFilter;
struct jcBroadcastReceiver;
//---------------------------------------------------------------------------
struct jcContentProvider : jcObject
{
};
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

	static constexpr const char jname_getPackageName[]="getPackageName";
	jrLocal<jcString> getPackageName(JNIEnv *env)noexcept{
		return jMethodCall<jname_getPackageName,&jcContext::getPackageName>::Call(env,this);
	}

	static constexpr const char jname_getPackageManager[]="getPackageManager";
	jrLocal<pm::jcPackageManager> getPackageManager(JNIEnv *env)noexcept{
		return jMethodCall<jname_getPackageManager,&jcContext::getPackageManager>::Call(env,this);
	}

	static constexpr const char jname_getFileStreamPath[]="getFileStreamPath";
	jrLocal<java::io::jcFile> getFileStreamPath(JNIEnv *env,jcString *name)noexcept{
		return jMethodCall<jname_getFileStreamPath,&jcContext::getFileStreamPath>::Call(env,this,name);
	}

	static constexpr const char jname_registerReceiver[]="registerReceiver";
	jrLocal<jcIntent> registerReceiver(JNIEnv *env,jcBroadcastReceiver *receiver,jcIntentFilter *filter)noexcept{
		return jMethodCall<jname_registerReceiver,&jcContext::registerReceiver>::Call(env,this,receiver,filter);
	}
};
//---------------------------------------------------------------------------
struct jcIntent : jcObject
{
	static constexpr const char jClassName[]="android/content/Intent";

	static constexpr jint FLAG_GRANT_READ_URI_PERMISSION		=1;
	static constexpr jint FLAG_GRANT_WRITE_URI_PERMISSION		=2;	

	static constexpr const char jname_ACTION_VIEW[]="ACTION_VIEW";
	static jStaticFieldAccess<jcString*> ACTION_VIEW(void)noexcept{
		return jStaticFieldAccessMaker<jcIntent,jname_ACTION_VIEW>();
	}

	static constexpr const char jname_setDataAndType[]="setDataAndType";
	jrLocal<jcIntent> setDataAndType(JNIEnv *env,net::jcUri *data,jcString *type)noexcept{
		return jMethodCall<jname_setDataAndType,&jcIntent::setDataAndType>::Call(env,this,data,type);
	}


	static constexpr const char jname_addFlags[]="addFlags";
	jrLocal<jcIntent> addFlags(JNIEnv *env,jint flags)noexcept{
		return jMethodCall<jname_addFlags,&jcIntent::addFlags>::Call(env,this,flags);
	}
};
//---------------------------------------------------------------------------
struct jcIntentFilter : jcObject
{
	static constexpr const char jClassName[]="android/content/IntentFilter";
};
//---------------------------------------------------------------------------
struct jcBroadcastReceiver : jcObject
{
	static constexpr const char jClassName[]="android/content/BroadcastReceiver";
};
//---------------------------------------------------------------------------
}	// namespace content
//---------------------------------------------------------------------------
namespace app{
//---------------------------------------------------------------------------
struct jcActivity : content::jcContext
{
	static constexpr const char jClassName[]="android/app/Activity";

	static constexpr const char jname_startActivity[]="startActivity";
	void startActivity(JNIEnv *env,content::jcIntent *intent)noexcept{
		return jMethodCall<jname_startActivity,&jcActivity::startActivity>::Call(env,this,intent);
	}
};
//---------------------------------------------------------------------------
}	// namespace app
//---------------------------------------------------------------------------
}	// namespace android
//---------------------------------------------------------------------------
namespace androidx{
//---------------------------------------------------------------------------
namespace core{
//---------------------------------------------------------------------------
namespace content{
//---------------------------------------------------------------------------
struct jcFileProvider : android::content::jcContentProvider
{
	static constexpr const char jClassName[]="androidx/core/content/FileProvider";

	static constexpr const char jname_getUriForFile[]="getUriForFile";
	static jrLocal<android::net::jcUri> getUriForFile(JNIEnv *env,android::content::jcContext *context,jcString *authority,java::io::jcFile *file)noexcept{
		return jStaticMethodCall<jcFileProvider,jname_getUriForFile,&jcFileProvider::getUriForFile>::Call(env,context,authority,file);
	}
};
//---------------------------------------------------------------------------
}	// namespace content
//---------------------------------------------------------------------------
}	// namespace core
//---------------------------------------------------------------------------
}	// namespace androidx
//---------------------------------------------------------------------------
}	// namespace jCPP
//---------------------------------------------------------------------------
}	// namespace cnLibrary
//---------------------------------------------------------------------------

