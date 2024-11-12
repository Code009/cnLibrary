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
namespace util{
//---------------------------------------------------------------------------
struct jcDisplayMetrics : jcObject
{
	static constexpr const char jClassName[]="android/util/DisplayMetrics";


	static constexpr const char jname_density[]="density";
	jFieldAccess<jfloat> density(JNIEnv *env)noexcept{
		return jFieldAccessMaker<jname_density>(env,this);
	}

	static constexpr const char jname_densityDpi[]="densityDpi";
	jFieldAccess<jint> densityDpi(JNIEnv *env)noexcept{
		return jFieldAccessMaker<jname_densityDpi>(env,this);
	}

	static constexpr const char jname_heightPixels[]="heightPixels";
	jFieldAccess<jint> heightPixels(JNIEnv *env)noexcept{
		return jFieldAccessMaker<jname_heightPixels>(env,this);
	}
	static constexpr const char jname_widthPixels[]="widthPixels";
	jFieldAccess<jint> widthPixels(JNIEnv *env)noexcept{
		return jFieldAccessMaker<jname_widthPixels>(env,this);
	}


	static constexpr const char jname_scaledDensity[]="scaledDensity";
	jFieldAccess<jfloat> scaledDensity(JNIEnv *env)noexcept{
		return jFieldAccessMaker<jname_scaledDensity>(env,this);
	}

	static constexpr const char jname_xdpi[]="xdpi";
	jFieldAccess<jfloat> xdpi(JNIEnv *env)noexcept{
		return jFieldAccessMaker<jname_xdpi>(env,this);
	}

	static constexpr const char jname_ydpi[]="ydpi";
	jFieldAccess<jfloat> ydpi(JNIEnv *env)noexcept{
		return jFieldAccessMaker<jname_ydpi>(env,this);
	}

};
//---------------------------------------------------------------------------
struct jcTypedValue : jcObject
{
	static constexpr const char jClassName[]="android/util/TypedValue";

	static constexpr jint COMPLEX_UNIT_DIP=1;

	static constexpr const char jname_applyDimension[]="applyDimension";
	static jfloat applyDimension(JNIEnv *env,jint unit,jfloat value,jcDisplayMetrics *metrics)noexcept{
		return jStaticMethodCall<jcTypedValue,jname_applyDimension,&jcTypedValue::applyDimension>::Call(env,unit,value,metrics);
	}
};
//---------------------------------------------------------------------------
}	// namespace util
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
struct jcResources : jcObject
{
	static constexpr const char jClassName[]="android/content/res/Resources";

	static constexpr const char jname_getDisplayMetrics[]="getDisplayMetrics";
	jrLocal<util::jcDisplayMetrics> getDisplayMetrics(JNIEnv *env)noexcept{
		return jMethodCall<jname_getDisplayMetrics,&jcResources::getDisplayMetrics>::Call(env,this);
	}
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
	jFieldAccess<jcString*> versionName(JNIEnv *env)noexcept{
		return jFieldAccessMaker<jname_versionName>(env,this);
	}

	static constexpr const char jname_versionCode[]="versionCode";
	jFieldAccess<jint> versionCode(JNIEnv *env)noexcept{
		return jFieldAccessMaker<jname_versionCode>(env,this);
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
	static jStaticFieldAccess<jcString*> BLUETOOTH_SERVICE(JNIEnv *env)noexcept{
		return jStaticFieldAccessMaker<jcContext,jname_BLUETOOTH_SERVICE>(env);
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

	static constexpr const char jname_getResources[]="getResources";
	jrLocal<res::jcResources> getResources(JNIEnv *env)noexcept{
		return jMethodCall<jname_getResources,&jcContext::getResources>::Call(env,this);
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
	static jStaticFieldAccess<jcString*> ACTION_VIEW(JNIEnv *env)noexcept{
		return jStaticFieldAccessMaker<jcIntent,jname_ACTION_VIEW>(env);
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

