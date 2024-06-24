/*- cnAndroid - JNI -------------------------------------------------------*/
/*         Developer : Code009                                             */
/*         Create on : 2024-04-01                                          */
/*-------------------------------------------------------------------------*/
#pragma once
/*-------------------------------------------------------------------------*/
#include <cnTK/cnTK.h>

#ifdef __cplusplus

//---------------------------------------------------------------------------
namespace cnLibrary{
//---------------------------------------------------------------------------
template<auto T>
struct JNIMemberName;
//---------------------------------------------------------------------------
namespace jCPP{
//---------------------------------------------------------------------------
struct jcObject;
struct jcClass;
struct jcString;
struct jcThrowable;
struct jbcArray;
template<class T> struct jcArray;

template<class T>
struct TClassRef;


struct jbField{};	// field id
struct jbMethod{};	// method id

template<class TVariable>	struct jField : jbField{};
template<class TFunction>	struct jMethod : jbMethod{};
template<class TClass,class TFunction>	struct jStaticMethod : jbMethod{};


enum class RefType : uInt8 {
	Invalid = 0,
	Local = 1,
	Global = 2,
	WeakGlobal = 3
};

#ifdef JNI_OK

using ::JNINativeMethod;

inline jvalue MakeJValue(jobject l){	jvalue v;	v.l=l;	return v;	}
inline jvalue MakeJValue(jcObject*l){	jvalue v;	v.l=reinterpret_cast<jobject>(l);	return v;	}

#else


/* Primitive types that match up with Java equivalents. */
typedef uInt8		jboolean; /* unsigned 8 bits */
typedef sInt8		jbyte;    /* signed 8 bits */
typedef uInt16		jchar;    /* unsigned 16 bits */
typedef sInt16		jshort;   /* signed 16 bits */
typedef sInt32		jint;     /* signed 32 bits */
typedef sInt64		jlong;    /* signed 64 bits */
typedef Float32		jfloat;   /* 32-bit IEEE 754 */
typedef Float64		jdouble;  /* 64-bit IEEE 754 */

/* "cardinal indices and sizes" */
typedef jint     jsize;


union jvalue
{
	jboolean    z;
	jbyte       b;
	jchar       c;
	jshort      s;
	jint        i;
	jlong       j;
	jfloat      f;
	jdouble     d;
	jcObject*   l;
};

struct JNINativeMethod{
	const char* name;
	const char* signature;
	void*       fnPtr;
};


inline jvalue MakeJValue(jcObject*l){	jvalue v;	v.l=l;	return v;	}
inline jvalue MakeJValue(jobject l){	jvalue v;	v.l=reinterpret_cast<jcObject*>(l);	return v;	}


#endif

inline jvalue MakeJValue(jboolean z){	jvalue v;	v.z=z;	return v;	}
inline jvalue MakeJValue(jbyte b){		jvalue v;	v.b=b;	return v;	}
inline jvalue MakeJValue(jchar c){		jvalue v;	v.c=c;	return v;	}
inline jvalue MakeJValue(jshort s){		jvalue v;	v.s=s;	return v;	}
inline jvalue MakeJValue(jint i){		jvalue v;	v.i=i;	return v;	}
inline jvalue MakeJValue(jlong j){		jvalue v;	v.j=j;	return v;	}
inline jvalue MakeJValue(jfloat f){		jvalue v;	v.f=f;	return v;	}
inline jvalue MakeJValue(jdouble d){	jvalue v;	v.d=d;	return v;	}


#ifdef JNI_OK

struct jEnv;
struct jVM : _JavaVM
{
	jint AttachCurrentThread(jEnv* &env,void* thr_args)noexcept
	{	return _JavaVM::AttachCurrentThread(reinterpret_cast<JNIEnv**>(&env),thr_args);	}
	jint GetEnv(jEnv* &env,jint version)noexcept
	{	return _JavaVM::GetEnv(reinterpret_cast<void**>(&env),version);	}
	jint AttachCurrentThreadAsDaemon(jEnv* &env,void *thr_args)noexcept
	{	return _JavaVM::AttachCurrentThreadAsDaemon(reinterpret_cast<JNIEnv**>(&env),thr_args);	}
};

//---------------------------------------------------------------------------

struct jEnv : _JNIEnv
{
	jcClass* FindClass(const char *name)noexcept
	{ return reinterpret_cast<jcClass*>(_JNIEnv::FindClass(name)); }

	jcObject*	NewGlobalRef(jcObject *obj)noexcept
	{ return reinterpret_cast<jcObject*>(_JNIEnv::NewGlobalRef(reinterpret_cast<jobject>(obj))); }
	void		DeleteGlobalRef(jcObject *globalRef)noexcept
	{ return _JNIEnv::DeleteGlobalRef(reinterpret_cast<jobject>(globalRef)); }
	void		DeleteLocalRef(jcObject *localRef)noexcept
	{ return _JNIEnv::DeleteLocalRef(reinterpret_cast<jobject>(localRef)); }

	
	jbMethod*	GetMethodID(jcClass *clazz,const char* name,const char* sig)noexcept
	{ return reinterpret_cast<jbMethod*>(_JNIEnv::GetMethodID(reinterpret_cast<jclass>(clazz),name,sig)); }

	jbField*	GetFieldID(jcClass *clazz,const char* name,const char* sig)noexcept
	{ return reinterpret_cast<jbField*>(_JNIEnv::GetFieldID(reinterpret_cast<jclass>(clazz),name,sig)); }

	jbMethod*	GetStaticMethodID(jcClass *clazz,const char* name,const char* sig)noexcept
	{ return reinterpret_cast<jbMethod*>(_JNIEnv::GetStaticMethodID(reinterpret_cast<jclass>(clazz),name,sig)); }

	jbField*	GetStaticFieldID(jcClass *clazz,const char* name,const char* sig)noexcept
	{ return reinterpret_cast<jbField*>(_JNIEnv::GetStaticFieldID(reinterpret_cast<jclass>(clazz),name,sig)); }

	jboolean*		GetBooleanArrayElements(jcArray<jboolean> *array,jboolean* isCopy)noexcept
	{ return _JNIEnv::GetBooleanArrayElements(reinterpret_cast<jbooleanArray>(array),isCopy); }
	jbyte*			GetByteArrayElements(jcArray<jbyte> *array,jboolean* isCopy)noexcept
	{ return _JNIEnv::GetByteArrayElements(reinterpret_cast<jbyteArray>(array),isCopy); }
	jchar*			GetCharArrayElements(jcArray<jchar> *array,jboolean* isCopy)noexcept
	{ return _JNIEnv::GetCharArrayElements(reinterpret_cast<jcharArray>(array),isCopy); }
	jshort*			GetShortArrayElements(jcArray<jshort> *array,jboolean* isCopy)noexcept
	{ return _JNIEnv::GetShortArrayElements(reinterpret_cast<jshortArray>(array),isCopy); }
	jint*			GetIntArrayElements(jcArray<jint> *array,jboolean* isCopy)noexcept
	{ return _JNIEnv::GetIntArrayElements(reinterpret_cast<jintArray>(array),isCopy); }
	jlong*			GetLongArrayElements(jcArray<jlong> *array,jboolean* isCopy)noexcept
	{ return _JNIEnv::GetLongArrayElements(reinterpret_cast<jlongArray>(array),isCopy); }
	jfloat*			GetFloatArrayElements(jcArray<jfloat> *array,jboolean* isCopy)noexcept
	{ return _JNIEnv::GetFloatArrayElements(reinterpret_cast<jfloatArray>(array),isCopy); }
	jdouble*		GetDoubleArrayElements(jcArray<jdouble> *array,jboolean* isCopy)noexcept
	{ return _JNIEnv::GetDoubleArrayElements(reinterpret_cast<jdoubleArray>(array),isCopy); }

	void ReleaseBooleanArrayElements(jcArray<jboolean> *array,jboolean* elems,jint mode)noexcept
	{ return _JNIEnv::ReleaseBooleanArrayElements(reinterpret_cast<jbooleanArray>(array),elems,mode); }
	void ReleaseByteArrayElements(jcArray<jbyte> *array,jbyte *elems,jint mode)noexcept
	{ return _JNIEnv::ReleaseByteArrayElements(reinterpret_cast<jbyteArray>(array),elems,mode); }
	void ReleaseCharArrayElements(jcArray<jchar> *array,jchar *elems,jint mode)noexcept
	{ return _JNIEnv::ReleaseCharArrayElements(reinterpret_cast<jcharArray>(array),elems,mode); }
	void ReleaseShortArrayElements(jcArray<jshort> *array,jshort *elems,jint mode)noexcept
	{ return _JNIEnv::ReleaseShortArrayElements(reinterpret_cast<jshortArray>(array),elems,mode); }
	void ReleaseIntArrayElements(jcArray<jint> *array,jint *elems,jint mode)noexcept
	{ return _JNIEnv::ReleaseIntArrayElements(reinterpret_cast<jintArray>(array),elems,mode); }
	void ReleaseLongArrayElements(jcArray<jlong> *array,jlong *elems,jint mode)noexcept
	{ return _JNIEnv::ReleaseLongArrayElements(reinterpret_cast<jlongArray>(array),elems,mode); }
	void ReleaseFloatArrayElements(jcArray<jfloat> *array,jfloat *elems,jint mode)noexcept
	{ return _JNIEnv::ReleaseFloatArrayElements(reinterpret_cast<jfloatArray>(array),elems,mode); }
	void ReleaseDoubleArrayElements(jcArray<jdouble> *array,jdouble *elems,jint mode)noexcept
	{ return _JNIEnv::ReleaseDoubleArrayElements(reinterpret_cast<jdoubleArray>(array),elems,mode); }
	
	jcString*	NewString(const jchar* unicodeChars,jsize len)noexcept
	{ return reinterpret_cast<jcString*>(_JNIEnv::NewString(unicodeChars,len)); }
	jsize		GetStringLength(jcString *string)noexcept
	{ return _JNIEnv::GetStringLength(reinterpret_cast<jstring>(string)); }
	const jchar*GetStringChars(jcString *string,jboolean* isCopy)noexcept
	{ return _JNIEnv::GetStringChars(reinterpret_cast<jstring>(string),isCopy); }
	void		ReleaseStringChars(jcString *string,const jchar* chars)noexcept
	{ return _JNIEnv::ReleaseStringChars(reinterpret_cast<jstring>(string),chars); }
	jcString*	NewStringUTF(const char* bytes)noexcept
	{ return reinterpret_cast<jcString*>(_JNIEnv::NewStringUTF(bytes)); }
	jsize		GetStringUTFLength(jcString *string)noexcept
	{ return _JNIEnv::GetStringUTFLength(reinterpret_cast<jstring>(string)); }

	jsize		GetArrayLength(jbcArray *array)noexcept
	{ return _JNIEnv::GetArrayLength(reinterpret_cast<jarray>(array)); }

	jcArray<jcObject>*	NewObjectArray(jsize length,jcClass *elementClass,jcObject *initialElement)noexcept
	{ return reinterpret_cast<jcArray<jcObject>*>(_JNIEnv::NewObjectArray(length,reinterpret_cast<jclass>(elementClass),reinterpret_cast<jobject>(initialElement))); }
	jcObject*			GetObjectArrayElement(jbcArray *array,jsize index)noexcept
	{ return reinterpret_cast<jcObject*>(_JNIEnv::GetObjectArrayElement(reinterpret_cast<jobjectArray>(array),index)); }
	void				SetObjectArrayElement(jbcArray *array,jsize index,jcObject *value)noexcept
	{ return _JNIEnv::SetObjectArrayElement(reinterpret_cast<jobjectArray>(array),index,reinterpret_cast<jobject>(value)); }

	jcArray<jboolean>*	NewBooleanArray(jsize length)noexcept
	{ return reinterpret_cast<jcArray<jboolean>*>(_JNIEnv::NewBooleanArray(length)); }
	jcArray<jbyte>*		NewByteArray(jsize length)noexcept
	{ return reinterpret_cast<jcArray<jbyte>*>(_JNIEnv::NewByteArray(length)); }
	jcArray<jchar>*		NewCharArray(jsize length)noexcept
	{ return reinterpret_cast<jcArray<jchar>*>(_JNIEnv::NewCharArray(length)); }
	jcArray<jshort>*	NewShortArray(jsize length)noexcept
	{ return reinterpret_cast<jcArray<jshort>*>(_JNIEnv::NewShortArray(length)); }
	jcArray<jint>*		NewIntArray(jsize length)noexcept
	{ return reinterpret_cast<jcArray<jint>*>(_JNIEnv::NewIntArray(length)); }
	jcArray<jlong>*		NewLongArray(jsize length)noexcept
	{ return reinterpret_cast<jcArray<jlong>*>(_JNIEnv::NewLongArray(length)); }
	jcArray<jfloat>*	NewFloatArray(jsize length)noexcept
	{ return reinterpret_cast<jcArray<jfloat>*>(_JNIEnv::NewFloatArray(length)); }
	jcArray<jdouble>*	NewDoubleArray(jsize length)noexcept
	{ return reinterpret_cast<jcArray<jdouble>*>(_JNIEnv::NewDoubleArray(length)); }


	void	GetBooleanArrayRegion(jcArray<jboolean> *array,jsize start,jsize len,jboolean *buf)noexcept
	{ return _JNIEnv::GetBooleanArrayRegion(reinterpret_cast<jbooleanArray>(array),start,len,buf); }
	void	GetByteArrayRegion(jcArray<jbyte> *array,jsize start,jsize len,jbyte *buf)noexcept
	{ return _JNIEnv::GetByteArrayRegion(reinterpret_cast<jbyteArray>(array),start,len,buf); }
	void	GetCharArrayRegion(jcArray<jchar> *array,jsize start,jsize len,jchar *buf)noexcept
	{ return _JNIEnv::GetCharArrayRegion(reinterpret_cast<jcharArray>(array),start,len,buf); }
	void	GetShortArrayRegion(jcArray<jshort> *array,jsize start,jsize len,jshort *buf)noexcept
	{ return _JNIEnv::GetShortArrayRegion(reinterpret_cast<jshortArray>(array),start,len,buf); }
	void	GetIntArrayRegion(jcArray<jint> *array,jsize start,jsize len,jint *buf)noexcept
	{ return _JNIEnv::GetIntArrayRegion(reinterpret_cast<jintArray>(array),start,len,buf); }
	void	GetLongArrayRegion(jcArray<jlong> *array,jsize start,jsize len,jlong *buf)noexcept
	{ return _JNIEnv::GetLongArrayRegion(reinterpret_cast<jlongArray>(array),start,len,buf); }
	void	GetFloatArrayRegion(jcArray<jfloat> *array,jsize start,jsize len,jfloat *buf)noexcept
	{ return _JNIEnv::GetFloatArrayRegion(reinterpret_cast<jfloatArray>(array),start,len,buf); }
	void	GetDoubleArrayRegion(jcArray<jdouble> *array,jsize start,jsize len,jdouble *buf)noexcept
	{ return _JNIEnv::GetDoubleArrayRegion(reinterpret_cast<jdoubleArray>(array),start,len,buf); }

	void	SetBooleanArrayRegion(jcArray<jboolean> *array,jsize start,jsize len,const jboolean *buf)noexcept
	{ return _JNIEnv::SetBooleanArrayRegion(reinterpret_cast<jbooleanArray>(array),start,len,buf); }
	void	SetByteArrayRegion(jcArray<jbyte> *array,jsize start,jsize len,const jbyte *buf)noexcept
	{ return _JNIEnv::SetByteArrayRegion(reinterpret_cast<jbyteArray>(array),start,len,buf); }
	void	SetCharArrayRegion(jcArray<jchar> *array,jsize start,jsize len,const jchar *buf)noexcept
	{ return _JNIEnv::SetCharArrayRegion(reinterpret_cast<jcharArray>(array),start,len,buf); }
	void	SetShortArrayRegion(jcArray<jshort> *array,jsize start,jsize len,const jshort *buf)noexcept
	{ return _JNIEnv::SetShortArrayRegion(reinterpret_cast<jshortArray>(array),start,len,buf); }
	void	SetIntArrayRegion(jcArray<jint> *array,jsize start,jsize len,const jint *buf)noexcept
	{ return _JNIEnv::SetIntArrayRegion(reinterpret_cast<jintArray>(array),start,len,buf); }
	void	SetLongArrayRegion(jcArray<jlong> *array,jsize start,jsize len,const jlong *buf)noexcept
	{ return _JNIEnv::SetLongArrayRegion(reinterpret_cast<jlongArray>(array),start,len,buf); }
	void	SetFloatArrayRegion(jcArray<jfloat> *array,jsize start,jsize len,const jfloat *buf)noexcept
	{ return _JNIEnv::SetFloatArrayRegion(reinterpret_cast<jfloatArray>(array),start,len,buf); }
	void	SetDoubleArrayRegion(jcArray<jdouble> *array,jsize start,jsize len,const jdouble *buf)noexcept
	{ return _JNIEnv::SetDoubleArrayRegion(reinterpret_cast<jdoubleArray>(array),start,len,buf); }

	jint		RegisterNatives(jcClass *clazz,const JNINativeMethod* methods,jint nMethods)noexcept
	{ return _JNIEnv::RegisterNatives(reinterpret_cast<jclass>(clazz),methods,nMethods); }
	jint		UnregisterNatives(jcClass *clazz)noexcept
	{ return _JNIEnv::UnregisterNatives(reinterpret_cast<jclass>(clazz)); }

	jint		GetJavaVM(jVM* &vm)noexcept
	{ return _JNIEnv::GetJavaVM(reinterpret_cast<JavaVM**>(&vm)); }

	const jchar* GetStringCritical(jcString *string,jboolean* isCopy)noexcept
	{ return _JNIEnv::GetStringCritical(reinterpret_cast<jstring>(string),isCopy); }
	void ReleaseStringCritical(jcString *string,const jchar* carray)noexcept
	{ return _JNIEnv::ReleaseStringCritical(reinterpret_cast<jstring>(string),carray); }

	template<class TField>
	TField GetField(jcObject *Object,jbField *FieldID)noexcept;
	template<class TField>
	void SetField(jcObject *Object,jbField *FieldID,TField Value)noexcept;

	template<class TField>
	TField GetStaticField(jcClass *Class,jbField *FieldID)noexcept;
	template<class TField>
	void SetStaticField(jcClass *Class,jbField *FieldID,TField Value)noexcept;

	template<class TRet>
	TRet CallMethodA(jcObject *Object,jbMethod *MethodID,const jvalue *args)noexcept;
	template<class TClass,class TRet,class...TArgs>
	TRet Call(TClass *Object,jMethod<TRet (TClass::*)(TArgs...)> *MethodID,TArgs...Args)noexcept{
		jvalue args[sizeof...(TArgs)]={MakeJValue(Args)...};
		return CallMethodA<TRet>(Object,MethodID,args);
	}


	template<class TRet>
	TRet CallStaticMethodA(jcClass *Class,jbMethod *MethodID,const jvalue *args)noexcept;
	template<class TClass,class TRet,class...TArgs>
	TRet Call(jStaticMethod<TClass,TRet (TArgs...)> *MethodID,TArgs...Args)noexcept{
		jvalue args[sizeof...(TArgs)]={MakeJValue(Args)...};
		return CallStaticMethodA<TRet>(TClassRef<TClass>::Value,MethodID,args);
	}

	template<class TClass,class...TArgs>
	TClass* New(jMethod<void (TArgs...)> *MethodID,TArgs...Args)noexcept{
		jvalue args[sizeof...(TArgs)]={MakeJValue(Args)...};
		return reinterpret_cast<TClass*>(NewObjectA(reinterpret_cast<jclass>(TClassRef<TClass>::Value),reinterpret_cast<jmethodID>(MethodID),args));
	}

};

//---------------------------------------------------------------------------
template<class TField>
inline TField jEnv::GetField(jcObject *Object,jbField *FieldID)noexcept
{
	return reinterpret_cast<TField>(GetObjectField(reinterpret_cast<jobject>(Object),reinterpret_cast<jfieldID>(FieldID)));
}

template<>
inline jboolean jEnv::GetField(jcObject *Object,jbField *FieldID)noexcept
{
	return _JNIEnv::GetBooleanField(reinterpret_cast<jobject>(Object),reinterpret_cast<jfieldID>(FieldID));
}
template<>
inline jbyte jEnv::GetField(jcObject *Object,jbField *FieldID)noexcept
{
	return _JNIEnv::GetByteField(reinterpret_cast<jobject>(Object),reinterpret_cast<jfieldID>(FieldID));
}
template<>
inline jchar jEnv::GetField(jcObject *Object,jbField *FieldID)noexcept
{
	return _JNIEnv::GetCharField(reinterpret_cast<jobject>(Object),reinterpret_cast<jfieldID>(FieldID));
}
template<>
inline jshort jEnv::GetField(jcObject *Object,jbField *FieldID)noexcept
{
	return _JNIEnv::GetShortField(reinterpret_cast<jobject>(Object),reinterpret_cast<jfieldID>(FieldID));
}
template<>
inline jint jEnv::GetField(jcObject *Object,jbField *FieldID)noexcept
{
	return _JNIEnv::GetIntField(reinterpret_cast<jobject>(Object),reinterpret_cast<jfieldID>(FieldID));
}
template<>
inline jlong jEnv::GetField(jcObject *Object,jbField *FieldID)noexcept
{
	return _JNIEnv::GetLongField(reinterpret_cast<jobject>(Object),reinterpret_cast<jfieldID>(FieldID));
}
template<>
inline jfloat jEnv::GetField(jcObject *Object,jbField *FieldID)noexcept
{
	return _JNIEnv::GetFloatField(reinterpret_cast<jobject>(Object),reinterpret_cast<jfieldID>(FieldID));
}
template<>
inline jdouble jEnv::GetField(jcObject *Object,jbField *FieldID)noexcept
{
	return _JNIEnv::GetDoubleField(reinterpret_cast<jobject>(Object),reinterpret_cast<jfieldID>(FieldID));
}

//---------------------------------------------------------------------------

template<class TField>
inline void jEnv::SetField(jcObject *Object,jbField *FieldID,TField Value)noexcept
{
	return _JNIEnv::SetObjectField(reinterpret_cast<jobject>(Object),reinterpret_cast<jfieldID>(FieldID),reinterpret_cast<jobject>(Value));
}

template<>
inline void jEnv::SetField(jcObject *Object,jbField *FieldID,jboolean Value)noexcept
{
	return _JNIEnv::SetBooleanField(reinterpret_cast<jobject>(Object),reinterpret_cast<jfieldID>(FieldID),Value);
}
template<>
inline void jEnv::SetField(jcObject *Object,jbField *FieldID,jbyte Value)noexcept
{
	return _JNIEnv::SetByteField(reinterpret_cast<jobject>(Object),reinterpret_cast<jfieldID>(FieldID),Value);
}
template<>
inline void jEnv::SetField(jcObject *Object,jbField *FieldID,jchar Value)noexcept
{
	return _JNIEnv::SetCharField(reinterpret_cast<jobject>(Object),reinterpret_cast<jfieldID>(FieldID),Value);
}
template<>
inline void jEnv::SetField(jcObject *Object,jbField *FieldID,jshort Value)noexcept
{
	return _JNIEnv::SetShortField(reinterpret_cast<jobject>(Object),reinterpret_cast<jfieldID>(FieldID),Value);
}
template<>
inline void jEnv::SetField(jcObject *Object,jbField *FieldID,jint Value)noexcept
{
	return _JNIEnv::SetIntField(reinterpret_cast<jobject>(Object),reinterpret_cast<jfieldID>(FieldID),Value);
}
template<>
inline void jEnv::SetField(jcObject *Object,jbField *FieldID,jlong Value)noexcept
{
	return _JNIEnv::SetLongField(reinterpret_cast<jobject>(Object),reinterpret_cast<jfieldID>(FieldID),Value);
}
template<>
inline void jEnv::SetField(jcObject *Object,jbField *FieldID,jfloat Value)noexcept
{
	return _JNIEnv::SetFloatField(reinterpret_cast<jobject>(Object),reinterpret_cast<jfieldID>(FieldID),Value);
}
template<>
inline void jEnv::SetField(jcObject *Object,jbField *FieldID,jdouble Value)noexcept
{
	return _JNIEnv::SetDoubleField(reinterpret_cast<jobject>(Object),reinterpret_cast<jfieldID>(FieldID),Value);
}


//---------------------------------------------------------------------------
template<class TField>
inline TField jEnv::GetStaticField(jcClass *Class,jbField *FieldID)noexcept
{
	return reinterpret_cast<TField>(GetStaticObjectField(reinterpret_cast<jclass>(Class),reinterpret_cast<jfieldID>(FieldID)));
}

template<>
inline jboolean jEnv::GetStaticField(jcClass *Class,jbField *FieldID)noexcept
{
	return _JNIEnv::GetStaticBooleanField(reinterpret_cast<jclass>(Class),reinterpret_cast<jfieldID>(FieldID));
}
template<>
inline jbyte jEnv::GetStaticField(jcClass *Class,jbField *FieldID)noexcept
{
	return _JNIEnv::GetStaticByteField(reinterpret_cast<jclass>(Class),reinterpret_cast<jfieldID>(FieldID));
}
template<>
inline jchar jEnv::GetStaticField(jcClass *Class,jbField *FieldID)noexcept
{
	return _JNIEnv::GetStaticCharField(reinterpret_cast<jclass>(Class),reinterpret_cast<jfieldID>(FieldID));
}
template<>
inline jshort jEnv::GetStaticField(jcClass *Class,jbField *FieldID)noexcept
{
	return _JNIEnv::GetStaticShortField(reinterpret_cast<jclass>(Class),reinterpret_cast<jfieldID>(FieldID));
}
template<>
inline jint jEnv::GetStaticField(jcClass *Class,jbField *FieldID)noexcept
{
	return _JNIEnv::GetStaticIntField(reinterpret_cast<jclass>(Class),reinterpret_cast<jfieldID>(FieldID));
}
template<>
inline jlong jEnv::GetStaticField(jcClass *Class,jbField *FieldID)noexcept
{
	return _JNIEnv::GetStaticLongField(reinterpret_cast<jclass>(Class),reinterpret_cast<jfieldID>(FieldID));
}
template<>
inline jfloat jEnv::GetStaticField(jcClass *Class,jbField *FieldID)noexcept
{
	return _JNIEnv::GetStaticFloatField(reinterpret_cast<jclass>(Class),reinterpret_cast<jfieldID>(FieldID));
}
template<>
inline jdouble jEnv::GetStaticField(jcClass *Class,jbField *FieldID)noexcept
{
	return _JNIEnv::GetStaticDoubleField(reinterpret_cast<jclass>(Class),reinterpret_cast<jfieldID>(FieldID));
}

//---------------------------------------------------------------------------

template<class TField>
inline void jEnv::SetStaticField(jcClass *Class,jbField *FieldID,TField Value)noexcept
{
	return _JNIEnv::SetStaticObjectField(reinterpret_cast<jclass>(Class),reinterpret_cast<jfieldID>(FieldID),reinterpret_cast<jobject>(Value));
}

template<>
inline void jEnv::SetStaticField(jcClass *Class,jbField *FieldID,jboolean Value)noexcept
{
	return _JNIEnv::SetStaticBooleanField(reinterpret_cast<jclass>(Class),reinterpret_cast<jfieldID>(FieldID),Value);
}
template<>
inline void jEnv::SetStaticField(jcClass *Class,jbField *FieldID,jbyte Value)noexcept
{
	return _JNIEnv::SetStaticByteField(reinterpret_cast<jclass>(Class),reinterpret_cast<jfieldID>(FieldID),Value);
}
template<>
inline void jEnv::SetStaticField(jcClass *Class,jbField *FieldID,jchar Value)noexcept
{
	return _JNIEnv::SetStaticCharField(reinterpret_cast<jclass>(Class),reinterpret_cast<jfieldID>(FieldID),Value);
}
template<>
inline void jEnv::SetStaticField(jcClass *Class,jbField *FieldID,jshort Value)noexcept
{
	return _JNIEnv::SetStaticShortField(reinterpret_cast<jclass>(Class),reinterpret_cast<jfieldID>(FieldID),Value);
}
template<>
inline void jEnv::SetStaticField(jcClass *Class,jbField *FieldID,jint Value)noexcept
{
	return _JNIEnv::SetStaticIntField(reinterpret_cast<jclass>(Class),reinterpret_cast<jfieldID>(FieldID),Value);
}
template<>
inline void jEnv::SetStaticField(jcClass *Class,jbField *FieldID,jlong Value)noexcept
{
	return _JNIEnv::SetStaticLongField(reinterpret_cast<jclass>(Class),reinterpret_cast<jfieldID>(FieldID),Value);
}
template<>
inline void jEnv::SetStaticField(jcClass *Class,jbField *FieldID,jfloat Value)noexcept
{
	return _JNIEnv::SetStaticFloatField(reinterpret_cast<jclass>(Class),reinterpret_cast<jfieldID>(FieldID),Value);
}
template<>
inline void jEnv::SetStaticField(jcClass *Class,jbField *FieldID,jdouble Value)noexcept
{
	return _JNIEnv::SetStaticDoubleField(reinterpret_cast<jclass>(Class),reinterpret_cast<jfieldID>(FieldID),Value);
}

//---------------------------------------------------------------------------

template<class TJavaRef>
inline TJavaRef jEnv::CallMethodA(jcObject *Object,jbMethod *MethodID,const jvalue *args)noexcept
{
	return reinterpret_cast<TJavaRef>(CallObjectMethodA(reinterpret_cast<jobject>(Object),reinterpret_cast<jmethodID>(MethodID),args));
}

template<>
inline jboolean jEnv::CallMethodA(jcObject *Object,jbMethod *MethodID,const jvalue *args)noexcept
{
	return CallBooleanMethodA(reinterpret_cast<jobject>(Object),reinterpret_cast<jmethodID>(MethodID),args);
}

template<>
inline jbyte jEnv::CallMethodA(jcObject *Object,jbMethod *MethodID,const jvalue *args)noexcept
{
	return CallByteMethodA(reinterpret_cast<jobject>(Object),reinterpret_cast<jmethodID>(MethodID),args);
}

template<>
inline jchar jEnv::CallMethodA(jcObject *Object,jbMethod *MethodID,const jvalue *args)noexcept
{
	return CallCharMethodA(reinterpret_cast<jobject>(Object),reinterpret_cast<jmethodID>(MethodID),args);
}

template<>
inline jshort jEnv::CallMethodA(jcObject *Object,jbMethod *MethodID,const jvalue *args)noexcept
{
	return CallShortMethodA(reinterpret_cast<jobject>(Object),reinterpret_cast<jmethodID>(MethodID),args);
}

template<>
inline jint jEnv::CallMethodA(jcObject *Object,jbMethod *MethodID,const jvalue *args)noexcept
{
	return CallIntMethodA(reinterpret_cast<jobject>(Object),reinterpret_cast<jmethodID>(MethodID),args);
}

template<>
inline jlong jEnv::CallMethodA(jcObject *Object,jbMethod *MethodID,const jvalue *args)noexcept
{
	return CallLongMethodA(reinterpret_cast<jobject>(Object),reinterpret_cast<jmethodID>(MethodID),args);
}

template<>
inline jfloat jEnv::CallMethodA(jcObject *Object,jbMethod *MethodID,const jvalue *args)noexcept
{
	return CallFloatMethodA(reinterpret_cast<jobject>(Object),reinterpret_cast<jmethodID>(MethodID),args);
}

template<>
inline jdouble jEnv::CallMethodA(jcObject *Object,jbMethod *MethodID,const jvalue *args)noexcept
{
	return CallDoubleMethodA(reinterpret_cast<jobject>(Object),reinterpret_cast<jmethodID>(MethodID),args);
}

template<>
inline void jEnv::CallMethodA(jcObject *Object,jbMethod *MethodID,const jvalue *args)noexcept
{
	return CallVoidMethodA(reinterpret_cast<jobject>(Object),reinterpret_cast<jmethodID>(MethodID),args);
}

//---------------------------------------------------------------------------

template<class TJavaRef>
inline TJavaRef jEnv::CallStaticMethodA(jcClass *Class,jbMethod *MethodID,const jvalue *args)noexcept
{
	return reinterpret_cast<TJavaRef>(CallStaticObjectMethodA(reinterpret_cast<jclass>(Class),reinterpret_cast<jmethodID>(MethodID),args));
}

template<>
inline jboolean jEnv::CallStaticMethodA(jcClass *Class,jbMethod *MethodID,const jvalue *args)noexcept
{
	return CallStaticBooleanMethodA(reinterpret_cast<jclass>(Class),reinterpret_cast<jmethodID>(MethodID),args);
}

template<>
inline jbyte jEnv::CallStaticMethodA(jcClass *Class,jbMethod *MethodID,const jvalue *args)noexcept
{
	return CallStaticByteMethodA(reinterpret_cast<jclass>(Class),reinterpret_cast<jmethodID>(MethodID),args);
}

template<>
inline jchar jEnv::CallStaticMethodA(jcClass *Class,jbMethod *MethodID,const jvalue *args)noexcept
{
	return CallStaticCharMethodA(reinterpret_cast<jclass>(Class),reinterpret_cast<jmethodID>(MethodID),args);
}

template<>
inline jshort jEnv::CallStaticMethodA(jcClass *Class,jbMethod *MethodID,const jvalue *args)noexcept
{
	return CallStaticShortMethodA(reinterpret_cast<jclass>(Class),reinterpret_cast<jmethodID>(MethodID),args);
}

template<>
inline jint jEnv::CallStaticMethodA(jcClass *Class,jbMethod *MethodID,const jvalue *args)noexcept
{
	return CallStaticIntMethodA(reinterpret_cast<jclass>(Class),reinterpret_cast<jmethodID>(MethodID),args);
}

template<>
inline jlong jEnv::CallStaticMethodA(jcClass *Class,jbMethod *MethodID,const jvalue *args)noexcept
{
	return CallStaticLongMethodA(reinterpret_cast<jclass>(Class),reinterpret_cast<jmethodID>(MethodID),args);
}

template<>
inline jfloat jEnv::CallStaticMethodA(jcClass *Class,jbMethod *MethodID,const jvalue *args)noexcept
{
	return CallStaticFloatMethodA(reinterpret_cast<jclass>(Class),reinterpret_cast<jmethodID>(MethodID),args);
}

template<>
inline jdouble jEnv::CallStaticMethodA(jcClass *Class,jbMethod *MethodID,const jvalue *args)noexcept
{
	return CallStaticDoubleMethodA(reinterpret_cast<jclass>(Class),reinterpret_cast<jmethodID>(MethodID),args);
}

template<>
inline void jEnv::CallStaticMethodA(jcClass *Class,jbMethod *MethodID,const jvalue *args)noexcept
{
	return CallStaticVoidMethodA(reinterpret_cast<jclass>(Class),reinterpret_cast<jmethodID>(MethodID),args);
}


#else

class jEnv;
class jVM
{
private:
	virtual void reserved0(void)=0;
	virtual void reserved1(void)=0;
	virtual void reserved2(void)=0;
public:
	virtual jint cnLib_FUNC DestroyJavaVM(void)noexcept=0;
	virtual jint cnLib_FUNC AttachCurrentThread(jEnv* &env,void* thr_args)noexcept=0;
	virtual jint cnLib_FUNC DetachCurrentThread(void)noexcept=0;
	virtual jint cnLib_FUNC GetEnv(jEnv* &env,jint version)noexcept=0;
	virtual jint cnLib_FUNC AttachCurrentThreadAsDaemon(jEnv* &env,void *thr_args)noexcept=0;
};

//---------------------------------------------------------------------------

class jEnv
{
private:
	virtual void reserved0(void)=0;
	virtual void reserved1(void)=0;
	virtual void reserved2(void)=0;
	virtual void reserved3(void)=0;
public:
	virtual jint cnLib_FUNC GetVersion(void)noexcept=0;

	virtual jcClass* cnLib_FUNC DefineClass(const char *name,jcObject *loader,const jbyte* buf,jsize bufLen)noexcept=0;

	virtual jcClass* cnLib_FUNC FindClass(const char *name)noexcept=0;

	virtual jbMethod* cnLib_FUNC FromReflectedMethod(jcObject *method)noexcept=0;
	virtual jbField* cnLib_FUNC FromReflectedField(jcObject *field)noexcept=0;

	virtual jcObject* cnLib_FUNC ToReflectedMethod(jcClass *cls,jbMethod *methodID,jboolean isStatic)noexcept=0;

	virtual jcClass* cnLib_FUNC GetSuperclass(jcClass *clazz)noexcept=0;
	virtual jboolean cnLib_FUNC IsAssignableFrom(jcClass *clazz1,jcClass *clazz2)noexcept=0;

	virtual jcObject* cnLib_FUNC ToReflectedField(jclass cls,jbField* fieldID,jboolean isStatic)noexcept=0;

	virtual jint			cnLib_FUNC Throw(jcThrowable *obj)noexcept=0;
	virtual jint			cnLib_FUNC ThrowNew(jcClass *clazz,const char* message)noexcept=0;
	virtual jcThrowable*	cnLib_FUNC ExceptionOccurred(void)noexcept=0;
	virtual void			cnLib_FUNC ExceptionDescribe(void)noexcept=0;
	virtual void			cnLib_FUNC ExceptionClear(void)noexcept=0;
	virtual void			cnLib_FUNC FatalError(const char* msg)noexcept=0;

	virtual jint		cnLib_FUNC PushLocalFrame(jint capacity)noexcept=0;
	virtual jcObject*	cnLib_FUNC PopLocalFrame(jcObject *result)noexcept=0;

	virtual jcObject*	cnLib_FUNC NewGlobalRef(jcObject *obj)noexcept=0;
	virtual void		cnLib_FUNC DeleteGlobalRef(jcObject *globalRef)noexcept=0;
	virtual void		cnLib_FUNC DeleteLocalRef(jcObject *localRef)noexcept=0;
	virtual jboolean	cnLib_FUNC IsSameObject(jcObject *ref1,jcObject *ref2)noexcept=0;

	virtual jcObject*	cnLib_FUNC NewLocalRef(jcObject *ref)noexcept=0;
	virtual jint		cnLib_FUNC EnsureLocalCapacity(jint capacity)noexcept=0;

	virtual jcObject*	cnLib_FUNC AllocObject(jcClass *clazz)noexcept=0;
	virtual jcObject*	cnLib_FUNC NewObject(jcClass *clazz,jbMethod *methodID,...)noexcept=0;
	virtual jcObject*	cnLib_FUNC NewObjectV(jcClass *clazz,jbMethod *methodID,va_list args)noexcept=0;
	virtual jcObject*	cnLib_FUNC NewObjectA(jcClass *clazz,jbMethod *methodID,const jvalue* args)noexcept=0;

	virtual jcClass*	cnLib_FUNC GetObjectClass(jcObject *obj)noexcept=0;
	virtual jboolean	cnLib_FUNC IsInstanceOf(jcObject *obj,jcClass *clazz)noexcept=0;
	virtual jbMethod*	cnLib_FUNC GetMethodID(jcClass *clazz,const char* name,const char* sig)noexcept=0;


	virtual jcObject*	cnLib_FUNC CallObjectMethod(jcObject *obj,jbMethod *methodID,...)noexcept=0;
	virtual jcObject*	cnLib_FUNC CallObjectMethodV(jcObject *obj,jbMethod *methodID,va_list)noexcept=0;
	virtual jcObject*	cnLib_FUNC CallObjectMethodA(jcObject *obj,jbMethod *methodID,const jvalue* args)noexcept=0;
	virtual jboolean	cnLib_FUNC CallBooleanMethod(jcObject *obj,jbMethod *methodID,...)noexcept=0;
	virtual jboolean	cnLib_FUNC CallBooleanMethodV(jcObject *obj,jbMethod *methodID,va_list)noexcept=0;
	virtual jboolean	cnLib_FUNC CallBooleanMethodA(jcObject *obj,jbMethod *methodID,const jvalue* args)noexcept=0;
	virtual jbyte		cnLib_FUNC CallByteMethod(jcObject *obj,jbMethod *methodID,...)noexcept=0;
	virtual jbyte		cnLib_FUNC CallByteMethodV(jcObject *obj,jbMethod *methodID,va_list)noexcept=0;
	virtual jbyte		cnLib_FUNC CallByteMethodA(jcObject *obj,jbMethod *methodID,const jvalue* args)noexcept=0;
	virtual jchar		cnLib_FUNC CallCharMethod(jcObject *obj,jbMethod *methodID,...)noexcept=0;
	virtual jchar		cnLib_FUNC CallCharMethodV(jcObject *obj,jbMethod *methodID,va_list)noexcept=0;
	virtual jchar		cnLib_FUNC CallCharMethodA(jcObject *obj,jbMethod *methodID,const jvalue* args)noexcept=0;
	virtual jshort		cnLib_FUNC CallShortMethod(jcObject *obj,jbMethod *methodID,...)noexcept=0;
	virtual jshort		cnLib_FUNC CallShortMethodV(jcObject *obj,jbMethod *methodID,va_list)noexcept=0;
	virtual jshort		cnLib_FUNC CallShortMethodA(jcObject *obj,jbMethod *methodID,const jvalue* args)noexcept=0;
	virtual jint		cnLib_FUNC CallIntMethod(jcObject *obj,jbMethod *methodID,...)noexcept=0;
	virtual jint		cnLib_FUNC CallIntMethodV(jcObject *obj,jbMethod *methodID,va_list)noexcept=0;
	virtual jint		cnLib_FUNC CallIntMethodA(jcObject *obj,jbMethod *methodID,const jvalue* args)noexcept=0;
	virtual jlong		cnLib_FUNC CallLongMethod(jcObject *obj,jbMethod *methodID,...)noexcept=0;
	virtual jlong		cnLib_FUNC CallLongMethodV(jcObject *obj,jbMethod *methodID,va_list)noexcept=0;
	virtual jlong		cnLib_FUNC CallLongMethodA(jcObject *obj,jbMethod *methodID,const jvalue* args)noexcept=0;
	virtual jfloat		cnLib_FUNC CallFloatMethod(jcObject *obj,jbMethod *methodID,...)noexcept=0;
	virtual jfloat		cnLib_FUNC CallFloatMethodV(jcObject *obj,jbMethod *methodID,va_list)noexcept=0;
	virtual jfloat		cnLib_FUNC CallFloatMethodA(jcObject *obj,jbMethod *methodID,const jvalue* args)noexcept=0;
	virtual jdouble		cnLib_FUNC CallDoubleMethod(jcObject *obj,jbMethod *methodID,...)noexcept=0;
	virtual jdouble		cnLib_FUNC CallDoubleMethodV(jcObject *obj,jbMethod *methodID,va_list)noexcept=0;
	virtual jdouble		cnLib_FUNC CallDoubleMethodA(jcObject *obj,jbMethod *methodID,const jvalue* args)noexcept=0;
	virtual void		cnLib_FUNC CallVoidMethod(jcObject *obj,jbMethod *methodID,...)noexcept=0;
	virtual void		cnLib_FUNC CallVoidMethodV(jcObject *obj,jbMethod *methodID,va_list)noexcept=0;
	virtual void		cnLib_FUNC CallVoidMethodA(jcObject *obj,jbMethod *methodID,const jvalue* args)noexcept=0;

	virtual jcObject*	cnLib_FUNC CallNonvirtualObjectMethod(jcObject *obj,jcClass *clazz,jbMethod *methodID,...)noexcept=0;
	virtual jcObject*	cnLib_FUNC CallNonvirtualObjectMethodV(jcObject *obj,jcClass *clazz,jbMethod *methodID,va_list)noexcept=0;
	virtual jcObject*	cnLib_FUNC CallNonvirtualObjectMethodA(jcObject *obj,jcClass *clazz,jbMethod *methodID,const jvalue *args)noexcept=0;
	virtual jboolean	cnLib_FUNC CallNonvirtualBooleanMethod(jcObject *obj,jcClass *clazz,jbMethod *methodID,...)noexcept=0;
	virtual jboolean	cnLib_FUNC CallNonvirtualBooleanMethodV(jcObject *obj,jcClass *clazz,jbMethod *methodID,va_list)noexcept=0;
	virtual jboolean	cnLib_FUNC CallNonvirtualBooleanMethodA(jcObject *obj,jcClass *clazz,jbMethod *methodID,const jvalue *args)noexcept=0;
	virtual jbyte		cnLib_FUNC CallNonvirtualByteMethod(jcObject *obj,jcClass *clazz,jbMethod *methodID,...)noexcept=0;
	virtual jbyte		cnLib_FUNC CallNonvirtualByteMethodV(jcObject *obj,jcClass *clazz,jbMethod *methodID,va_list)noexcept=0;
	virtual jbyte		cnLib_FUNC CallNonvirtualByteMethodA(jcObject *obj,jcClass *clazz,jbMethod *methodID,const jvalue *args)noexcept=0;
	virtual jchar		cnLib_FUNC CallNonvirtualCharMethod(jcObject *obj,jcClass *clazz,jbMethod *methodID,...)noexcept=0;
	virtual jchar		cnLib_FUNC CallNonvirtualCharMethodV(jcObject *obj,jcClass *clazz,jbMethod *methodID,va_list)noexcept=0;
	virtual jchar		cnLib_FUNC CallNonvirtualCharMethodA(jcObject *obj,jcClass *clazz,jbMethod *methodID,const jvalue *args)noexcept=0;
	virtual jshort		cnLib_FUNC CallNonvirtualShortMethod(jcObject *obj,jcClass *clazz,jbMethod *methodID,...)noexcept=0;
	virtual jshort		cnLib_FUNC CallNonvirtualShortMethodV(jcObject *obj,jcClass *clazz,jbMethod *methodID,va_list)noexcept=0;
	virtual jshort		cnLib_FUNC CallNonvirtualShortMethodA(jcObject *obj,jcClass *clazz,jbMethod *methodID,const jvalue *args)noexcept=0;
	virtual jint		cnLib_FUNC CallNonvirtualIntMethod(jcObject *obj,jcClass *clazz,jbMethod *methodID,...)noexcept=0;
	virtual jint		cnLib_FUNC CallNonvirtualIntMethodV(jcObject *obj,jcClass *clazz,jbMethod *methodID,va_list)noexcept=0;
	virtual jint		cnLib_FUNC CallNonvirtualIntMethodA(jcObject *obj,jcClass *clazz,jbMethod *methodID,const jvalue *args)noexcept=0;
	virtual jlong		cnLib_FUNC CallNonvirtualLongMethod(jcObject *obj,jcClass *clazz,jbMethod *methodID,...)noexcept=0;
	virtual jlong		cnLib_FUNC CallNonvirtualLongMethodV(jcObject *obj,jcClass *clazz,jbMethod *methodID,va_list)noexcept=0;
	virtual jlong		cnLib_FUNC CallNonvirtualLongMethodA(jcObject *obj,jcClass *clazz,jbMethod *methodID,const jvalue *args)noexcept=0;
	virtual jfloat		cnLib_FUNC CallNonvirtualFloatMethod(jcObject *obj,jcClass *clazz,jbMethod *methodID,...)noexcept=0;
	virtual jfloat		cnLib_FUNC CallNonvirtualFloatMethodV(jcObject *obj,jcClass *clazz,jbMethod *methodID,va_list)noexcept=0;
	virtual jfloat		cnLib_FUNC CallNonvirtualFloatMethodA(jcObject *obj,jcClass *clazz,jbMethod *methodID,const jvalue *args)noexcept=0;
	virtual jdouble		cnLib_FUNC CallNonvirtualDoubleMethod(jcObject *obj,jcClass *clazz,jbMethod *methodID,...)noexcept=0;
	virtual jdouble		cnLib_FUNC CallNonvirtualDoubleMethodV(jcObject *obj,jcClass *clazz,jbMethod *methodID,va_list)noexcept=0;
	virtual jdouble		cnLib_FUNC CallNonvirtualDoubleMethodA(jcObject *obj,jcClass *clazz,jbMethod *methodID,const jvalue *args)noexcept=0;
	virtual void		cnLib_FUNC CallNonvirtualVoidMethod(jcObject *obj,jcClass *clazz,jbMethod *methodID,...)noexcept=0;
	virtual void		cnLib_FUNC CallNonvirtualVoidMethodV(jcObject *obj,jcClass *clazz,jbMethod *methodID,va_list)noexcept=0;
	virtual void		cnLib_FUNC CallNonvirtualVoidMethodA(jcObject *obj,jcClass *clazz,jbMethod *methodID,const jvalue *args)noexcept=0;

	virtual jbField*	cnLib_FUNC GetFieldID(jcClass *clazz,const char* name,const char* sig)noexcept=0;

	virtual jcObject*	cnLib_FUNC GetObjectField(jcObject *obj,jbField *fieldID)noexcept=0;
	virtual jboolean	cnLib_FUNC GetBooleanField(jcObject *obj,jbField *fieldID)noexcept=0;
	virtual jbyte		cnLib_FUNC GetByteField(jcObject *obj,jbField *fieldID)noexcept=0;
	virtual jchar		cnLib_FUNC GetCharField(jcObject *obj,jbField *fieldID)noexcept=0;
	virtual jshort		cnLib_FUNC GetShortField(jcObject *obj,jbField *fieldID)noexcept=0;
	virtual jint		cnLib_FUNC GetIntField(jcObject *obj,jbField *fieldID)noexcept=0;
	virtual jlong		cnLib_FUNC GetLongField(jcObject *obj,jbField *fieldID)noexcept=0;
	virtual jfloat		cnLib_FUNC GetFloatField(jcObject *obj,jbField *fieldID)noexcept=0;
	virtual jdouble		cnLib_FUNC GetDoubleField(jcObject *obj,jbField *fieldID)noexcept=0;

	virtual void		cnLib_FUNC SetObjectField(jcObject *obj,jbField *fieldID,jcObject *value)noexcept=0;
	virtual void		cnLib_FUNC SetBooleanField(jcObject *obj,jbField *fieldID,jboolean value)noexcept=0;
	virtual void		cnLib_FUNC SetByteField(jcObject *obj,jbField *fieldID,jbyte value)noexcept=0;
	virtual void		cnLib_FUNC SetCharField(jcObject *obj,jbField *fieldID,jchar value)noexcept=0;
	virtual void		cnLib_FUNC SetShortField(jcObject *obj,jbField *fieldID,jshort value)noexcept=0;
	virtual void		cnLib_FUNC SetIntField(jcObject *obj,jbField *fieldID,jint value)noexcept=0;
	virtual void		cnLib_FUNC SetLongField(jcObject *obj,jbField *fieldID,jlong value)noexcept=0;
	virtual void		cnLib_FUNC SetFloatField(jcObject *obj,jbField *fieldID,jfloat value)noexcept=0;
	virtual void		cnLib_FUNC SetDoubleField(jcObject *obj,jbField *fieldID,jdouble value)noexcept=0;

	virtual jbMethod*	cnLib_FUNC GetStaticMethodID(jcClass *clazz,const char* name,const char* sig)noexcept=0;


	virtual jcObject*	cnLib_FUNC CallStaticObjectMethod(jcClass *clazz,jbMethod *methodID,...)noexcept=0;
	virtual jcObject*	cnLib_FUNC CallStaticObjectMethodV(jcClass *clazz,jbMethod *methodID,va_list)noexcept=0;
	virtual jcObject*	cnLib_FUNC CallStaticObjectMethodA(jcClass *clazz,jbMethod *methodID,const jvalue *args)noexcept=0;
	virtual jboolean	cnLib_FUNC CallStaticBooleanMethod(jcClass *clazz,jbMethod *methodID,...)noexcept=0;
	virtual jboolean	cnLib_FUNC CallStaticBooleanMethodV(jcClass *clazz,jbMethod *methodID,va_list)noexcept=0;
	virtual jboolean	cnLib_FUNC CallStaticBooleanMethodA(jcClass *clazz,jbMethod *methodID,const jvalue *args)noexcept=0;
	virtual jbyte		cnLib_FUNC CallStaticByteMethod(jcClass *clazz,jbMethod *methodID,...)noexcept=0;
	virtual jbyte		cnLib_FUNC CallStaticByteMethodV(jcClass *clazz,jbMethod *methodID,va_list)noexcept=0;
	virtual jbyte		cnLib_FUNC CallStaticByteMethodA(jcClass *clazz,jbMethod *methodID,const jvalue *args)noexcept=0;
	virtual jchar		cnLib_FUNC CallStaticCharMethod(jcClass *clazz,jbMethod *methodID,...)noexcept=0;
	virtual jchar		cnLib_FUNC CallStaticCharMethodV(jcClass *clazz,jbMethod *methodID,va_list)noexcept=0;
	virtual jchar		cnLib_FUNC CallStaticCharMethodA(jcClass *clazz,jbMethod *methodID,const jvalue *args)noexcept=0;
	virtual jshort		cnLib_FUNC CallStaticShortMethod(jcClass *clazz,jbMethod *methodID,...)noexcept=0;
	virtual jshort		cnLib_FUNC CallStaticShortMethodV(jcClass *clazz,jbMethod *methodID,va_list)noexcept=0;
	virtual jshort		cnLib_FUNC CallStaticShortMethodA(jcClass *clazz,jbMethod *methodID,const jvalue *args)noexcept=0;
	virtual jint		cnLib_FUNC CallStaticIntMethod(jcClass *clazz,jbMethod *methodID,...)noexcept=0;
	virtual jint		cnLib_FUNC CallStaticIntMethodV(jcClass *clazz,jbMethod *methodID,va_list)noexcept=0;
	virtual jint		cnLib_FUNC CallStaticIntMethodA(jcClass *clazz,jbMethod *methodID,const jvalue *args)noexcept=0;
	virtual jlong		cnLib_FUNC CallStaticLongMethod(jcClass *clazz,jbMethod *methodID,...)noexcept=0;
	virtual jlong		cnLib_FUNC CallStaticLongMethodV(jcClass *clazz,jbMethod *methodID,va_list)noexcept=0;
	virtual jlong		cnLib_FUNC CallStaticLongMethodA(jcClass *clazz,jbMethod *methodID,const jvalue *args)noexcept=0;
	virtual jfloat		cnLib_FUNC CallStaticFloatMethod(jcClass *clazz,jbMethod *methodID,...)noexcept=0;
	virtual jfloat		cnLib_FUNC CallStaticFloatMethodV(jcClass *clazz,jbMethod *methodID,va_list)noexcept=0;
	virtual jfloat		cnLib_FUNC CallStaticFloatMethodA(jcClass *clazz,jbMethod *methodID,const jvalue *args)noexcept=0;
	virtual jdouble		cnLib_FUNC CallStaticDoubleMethod(jcClass *clazz,jbMethod *methodID,...)noexcept=0;
	virtual jdouble		cnLib_FUNC CallStaticDoubleMethodV(jcClass *clazz,jbMethod *methodID,va_list)noexcept=0;
	virtual jdouble		cnLib_FUNC CallStaticDoubleMethodA(jcClass *clazz,jbMethod *methodID,const jvalue *args)noexcept=0;
	virtual void		cnLib_FUNC CallStaticVoidMethod(jcClass *clazz,jbMethod *methodID,...)noexcept=0;
	virtual void		cnLib_FUNC CallStaticVoidMethodV(jcClass *clazz,jbMethod *methodID,va_list)noexcept=0;
	virtual void		cnLib_FUNC CallStaticVoidMethodA(jcClass *clazz,jbMethod *methodID,const jvalue *args)noexcept=0;

	virtual jbField*	cnLib_FUNC GetStaticFieldID(jcClass *clazz,const char* name,const char* sig)noexcept=0;

	virtual jcObject*	cnLib_FUNC GetStaticObjectField(jcClass *cls,jbField *fieldID)noexcept=0;
	virtual jboolean	cnLib_FUNC GetStaticBooleanField(jcClass *clsobj,jbField *fieldID)noexcept=0;
	virtual jbyte		cnLib_FUNC GetStaticByteField(jcClass *cls,jbField *fieldID)noexcept=0;
	virtual jchar		cnLib_FUNC GetStaticCharField(jcClass *cls,jbField *fieldID)noexcept=0;
	virtual jshort		cnLib_FUNC GetStaticShortField(jcClass *cls,jbField *fieldID)noexcept=0;
	virtual jint		cnLib_FUNC GetStaticIntField(jcClass *cls,jbField *fieldID)noexcept=0;
	virtual jlong		cnLib_FUNC GetStaticLongField(jcClass *cls,jbField *fieldID)noexcept=0;
	virtual jfloat		cnLib_FUNC GetStaticFloatField(jcClass *cls,jbField *fieldID)noexcept=0;
	virtual jdouble		cnLib_FUNC GetStaticDoubleField(jcClass *cls,jbField *fieldID)noexcept=0;

	virtual void		cnLib_FUNC SetStaticObjectField(jcClass *cls,jbField *fieldID,jcObject *value)noexcept=0;
	virtual void		cnLib_FUNC SetStaticBooleanField(jcClass *cls,jbField *fieldID,jboolean value)noexcept=0;
	virtual void		cnLib_FUNC SetStaticByteField(jcClass *cls,jbField *fieldID,jbyte value)noexcept=0;
	virtual void		cnLib_FUNC SetStaticCharField(jcClass *cls,jbField *fieldID,jchar value)noexcept=0;
	virtual void		cnLib_FUNC SetStaticShortField(jcClass *cls,jbField *fieldID,jshort value)noexcept=0;
	virtual void		cnLib_FUNC SetStaticIntField(jcClass *cls,jbField *fieldID,jint value)noexcept=0;
	virtual void		cnLib_FUNC SetStaticLongField(jcClass *cls,jbField *fieldID,jlong value)noexcept=0;
	virtual void		cnLib_FUNC SetStaticFloatField(jcClass *cls,jbField *fieldID,jfloat value)noexcept=0;
	virtual void		cnLib_FUNC SetStaticDoubleField(jcClass *cls,jbField *fieldID,jdouble value)noexcept=0;

	virtual jcString*	cnLib_FUNC NewString(const jchar* unicodeChars,jsize len)noexcept=0;
	virtual jsize		cnLib_FUNC GetStringLength(jcString *string)noexcept=0;
	virtual const jchar*cnLib_FUNC GetStringChars(jcString *string,jboolean* isCopy)noexcept=0;
	virtual void		cnLib_FUNC ReleaseStringChars(jcString *string,const jchar* chars)noexcept=0;
	virtual jcString*	cnLib_FUNC NewStringUTF(const char* bytes)noexcept=0;
	virtual jsize		cnLib_FUNC GetStringUTFLength(jcString *string)noexcept=0;

	virtual const char*		cnLib_FUNC GetStringUTFChars(jcString *string,jboolean* isCopy)noexcept=0;
	virtual void			cnLib_FUNC ReleaseStringUTFChars(jcString *string,const char* utf)noexcept=0;
	virtual jsize			cnLib_FUNC GetArrayLength(jbcArray *array)noexcept=0;

	virtual jcArray<jcObject>*		cnLib_FUNC NewObjectArray(jsize length,jcClass *elementClass,jcObject *initialElement)noexcept=0;
	virtual jcObject*				cnLib_FUNC GetObjectArrayElement(jbcArray *array,jsize index)noexcept=0;
	virtual void					cnLib_FUNC SetObjectArrayElement(jbcArray *array,jsize index,jcObject *value)noexcept=0;

	virtual jcArray<jboolean>*	cnLib_FUNC NewBooleanArray(jsize length)noexcept=0;
	virtual jcArray<jbyte>*	cnLib_FUNC NewByteArray(jsize length)noexcept=0;
	virtual jcArray<jchar>*	cnLib_FUNC NewCharArray(jsize length)noexcept=0;
	virtual jcArray<jshort>*	cnLib_FUNC NewShortArray(jsize length)noexcept=0;
	virtual jcArray<jint>*		cnLib_FUNC NewIntArray(jsize length)noexcept=0;
	virtual jcArray<jlong>*	cnLib_FUNC NewLongArray(jsize length)noexcept=0;
	virtual jcArray<jfloat>*	cnLib_FUNC NewFloatArray(jsize length)noexcept=0;
	virtual jcArray<jdouble>*	cnLib_FUNC NewDoubleArray(jsize length)noexcept=0;

	virtual jboolean*		cnLib_FUNC GetBooleanArrayElements(jcArray<jboolean> *array,jboolean* isCopy)noexcept=0;
	virtual jbyte*			cnLib_FUNC GetByteArrayElements(jcArray<jbyte> *array,jboolean* isCopy)noexcept=0;
	virtual jchar*			cnLib_FUNC GetCharArrayElements(jcArray<jchar> *array,jboolean* isCopy)noexcept=0;
	virtual jshort*			cnLib_FUNC GetShortArrayElements(jcArray<jshort> *array,jboolean* isCopy)noexcept=0;
	virtual jint*			cnLib_FUNC GetIntArrayElements(jcArray<jint> *array,jboolean* isCopy)noexcept=0;
	virtual jlong*			cnLib_FUNC GetLongArrayElements(jcArray<jlong> *array,jboolean* isCopy)noexcept=0;
	virtual jfloat*			cnLib_FUNC GetFloatArrayElements(jcArray<jfloat> *array,jboolean* isCopy)noexcept=0;
	virtual jdouble*		cnLib_FUNC GetDoubleArrayElements(jcArray<jdouble> *array,jboolean* isCopy)noexcept=0;

	virtual void		cnLib_FUNC ReleaseBooleanArrayElements(jcArray<jboolean> *array,jboolean* elems,jint mode)noexcept=0;
	virtual void		cnLib_FUNC ReleaseByteArrayElements(jcArray<jbyte> *array,jbyte *elems,jint mode)noexcept=0;
	virtual void		cnLib_FUNC ReleaseCharArrayElements(jcArray<jchar> *array,jchar *elems,jint mode)noexcept=0;
	virtual void		cnLib_FUNC ReleaseShortArrayElements(jcArray<jshort> *array,jshort *elems,jint mode)noexcept=0;
	virtual void		cnLib_FUNC ReleaseIntArrayElements(jcArray<jint> *array,jint *elems,jint mode)noexcept=0;
	virtual void		cnLib_FUNC ReleaseLongArrayElements(jcArray<jlong> *array,jlong *elems,jint mode)noexcept=0;
	virtual void		cnLib_FUNC ReleaseFloatArrayElements(jcArray<jfloat> *array,jfloat *elems,jint mode)noexcept=0;
	virtual void		cnLib_FUNC ReleaseDoubleArrayElements(jcArray<jdouble> *array,jdouble *elems,jint mode)noexcept=0;

	virtual void		cnLib_FUNC GetBooleanArrayRegion(jcArray<jboolean> *array,jsize start,jsize len,jboolean *buf)noexcept=0;
	virtual void		cnLib_FUNC GetByteArrayRegion(jcArray<jbyte> *array,jsize start,jsize len,jbyte *buf)noexcept=0;
	virtual void		cnLib_FUNC GetCharArrayRegion(jcArray<jchar> *array,jsize start,jsize len,jchar *buf)noexcept=0;
	virtual void		cnLib_FUNC GetShortArrayRegion(jcArray<jshort> *array,jsize start,jsize len,jshort *buf)noexcept=0;
	virtual void		cnLib_FUNC GetIntArrayRegion(jcArray<jint> *array,jsize start,jsize len,jint *buf)noexcept=0;
	virtual void		cnLib_FUNC GetLongArrayRegion(jcArray<jlong> *array,jsize start,jsize len,jlong *buf)noexcept=0;
	virtual void		cnLib_FUNC GetFloatArrayRegion(jcArray<jfloat> *array,jsize start,jsize len,jfloat *buf)noexcept=0;
	virtual void		cnLib_FUNC GetDoubleArrayRegion(jcArray<jdouble> *array,jsize start,jsize len,jdouble *buf)noexcept=0;

	virtual void		cnLib_FUNC SetBooleanArrayRegion(jcArray<jboolean> *array,jsize start,jsize len,const jboolean *buf)noexcept=0;
	virtual void		cnLib_FUNC SetByteArrayRegion(jcArray<jbyte> *array,jsize start,jsize len,const jbyte *buf)noexcept=0;
	virtual void		cnLib_FUNC SetCharArrayRegion(jcArray<jchar> *array,jsize start,jsize len,const jchar *buf)noexcept=0;
	virtual void		cnLib_FUNC SetShortArrayRegion(jcArray<jshort> *array,jsize start,jsize len,const jshort *buf)noexcept=0;
	virtual void		cnLib_FUNC SetIntArrayRegion(jcArray<jint> *array,jsize start,jsize len,const jint *buf)noexcept=0;
	virtual void		cnLib_FUNC SetLongArrayRegion(jcArray<jlong> *array,jsize start,jsize len,const jlong *buf)noexcept=0;
	virtual void		cnLib_FUNC SetFloatArrayRegion(jcArray<jfloat> *array,jsize start,jsize len,const jfloat *buf)noexcept=0;
	virtual void		cnLib_FUNC SetDoubleArrayRegion(jcArray<jdouble> *array,jsize start,jsize len,const jdouble *buf)noexcept=0;

	virtual jint		cnLib_FUNC RegisterNatives(jcClass *clazz,const JNINativeMethod* methods,jint nMethods)noexcept=0;
	virtual jint		cnLib_FUNC UnregisterNatives(jcClass *clazz)noexcept=0;
	virtual jint		cnLib_FUNC MonitorEnter(jcObject *obj)noexcept=0;
	virtual jint		cnLib_FUNC MonitorExit(jcObject *obj)noexcept=0;
	virtual jint		cnLib_FUNC GetJavaVM(jVM* &vm)noexcept=0;

	virtual void		cnLib_FUNC GetStringRegion(jcString *str,jsize start,jsize len,jchar* buf)noexcept=0;
	virtual void		cnLib_FUNC GetStringUTFRegion(jcString *str,jsize start,jsize len,char* buf)noexcept=0;

	virtual void*		cnLib_FUNC GetPrimitiveArrayCritical(jbcArray *array,jboolean* isCopy)noexcept=0;
	virtual void		cnLib_FUNC ReleasePrimitiveArrayCritical(jbcArray *array,void* carray,jint mode)noexcept=0;

	virtual const jchar*cnLib_FUNC GetStringCritical(jcString *string,jboolean* isCopy)noexcept=0;
	virtual void		cnLib_FUNC ReleaseStringCritical(jcString *string,const jchar* carray)noexcept=0;

	virtual jcObject*	cnLib_FUNC NewWeakGlobalRef(jcObject obj)noexcept=0;
	virtual void		cnLib_FUNC DeleteWeakGlobalRef(jcObject *obj)noexcept=0;

	virtual jboolean	cnLib_FUNC ExceptionCheck(void)noexcept=0;

	virtual jcObject*	cnLib_FUNC NewDirectByteBuffer(void* address,jlong capacity)noexcept=0;
	virtual void*		cnLib_FUNC GetDirectBufferAddress(jcObject *buf)noexcept=0;
	virtual jlong		cnLib_FUNC GetDirectBufferCapacity(jcObject *buf)noexcept=0;

	/* added in JNI 1.6 */
	virtual RefType		cnLib_FUNC GetObjectRefType(jcObject *obj)noexcept=0;

	// helper functions

	template<class TField>
	TField GetField(jcObject *Object,jbField *FieldID)noexcept;
	template<class TField>
	void SetField(jcObject *Object,jbField *FieldID,TField Value)noexcept;


	template<class TRet>
	TRet CallMethodA(jcObject *Object,jbMethod *MethodID,const jvalue *args)noexcept;
	template<class TClass,class TRet,class...TArgs>
	TRet Call(TClass *Object,jMethod<TRet (TClass::*)(TArgs...)> *MethodID,TArgs...Args)noexcept{
		jvalue args[sizeof...(TArgs)]={MakeJValue(Args)...};
		return CallMethodA<TRet>(Object,MethodID,args);
	}


	template<class TRet>
	TRet CallStaticMethodA(jcClass *Class,jbMethod *MethodID,const jvalue *args)noexcept;
	template<class TClass,class TRet,class...TArgs>
	TRet Call(jStaticMethod<TClass,TRet (TArgs...)> *MethodID,TArgs...Args)noexcept{
		jvalue args[sizeof...(TArgs)]={MakeJValue(Args)...};
		return CallStaticMethodA<TRet>(TClassRef<TClass>::Value,MethodID,args);
	}

	template<class TClass,class...TArgs>
	TClass* New(jMethod<void (TArgs...)> *MethodID,TArgs...Args)noexcept{
		jvalue args[sizeof...(TArgs)]={MakeJValue(Args)...};
		return static_cast<TClass*>(NewObjectA(TClassRef<TClass>::Value,MethodID,args));
	}

};

//---------------------------------------------------------------------------
template<class TField>
inline TField jEnv::GetField(jcObject *Object,jbField *FieldID)noexcept
{
	return static_cast<TField>(GetObjectField(Object,FieldID));
}

template<>
inline jboolean jEnv::GetField(jcObject *Object,jbField *FieldID)noexcept
{
	return GetBooleanField(Object,FieldID);
}
template<>
inline jbyte jEnv::GetField(jcObject *Object,jbField *FieldID)noexcept
{
	return GetByteField(Object,FieldID);
}
template<>
inline jchar jEnv::GetField(jcObject *Object,jbField *FieldID)noexcept
{
	return GetCharField(Object,FieldID);
}
template<>
inline jshort jEnv::GetField(jcObject *Object,jbField *FieldID)noexcept
{
	return GetShortField(Object,FieldID);
}
template<>
inline jint jEnv::GetField(jcObject *Object,jbField *FieldID)noexcept
{
	return GetIntField(Object,FieldID);
}
template<>
inline jlong jEnv::GetField(jcObject *Object,jbField *FieldID)noexcept
{
	return GetLongField(Object,FieldID);
}
template<>
inline jfloat jEnv::GetField(jcObject *Object,jbField *FieldID)noexcept
{
	return GetFloatField(Object,FieldID);
}
template<>
inline jdouble jEnv::GetField(jcObject *Object,jbField *FieldID)noexcept
{
	return GetDoubleField(Object,FieldID);
}

//---------------------------------------------------------------------------

template<class TField>
inline void jEnv::SetField(jcObject *Object,jbField *FieldID,TField Value)noexcept
{
	return SetObjectField(Object,FieldID,Value);
}

template<>
inline void jEnv::SetField(jcObject *Object,jbField *FieldID,jboolean Value)noexcept
{
	return SetBooleanField(Object,FieldID,Value);
}
template<>
inline void jEnv::SetField(jcObject *Object,jbField *FieldID,jbyte Value)noexcept
{
	return SetByteField(Object,FieldID,Value);
}
template<>
inline void jEnv::SetField(jcObject *Object,jbField *FieldID,jchar Value)noexcept
{
	return SetCharField(Object,FieldID,Value);
}
template<>
inline void jEnv::SetField(jcObject *Object,jbField *FieldID,jshort Value)noexcept
{
	return SetShortField(Object,FieldID,Value);
}
template<>
inline void jEnv::SetField(jcObject *Object,jbField *FieldID,jint Value)noexcept
{
	return SetIntField(Object,FieldID,Value);
}
template<>
inline void jEnv::SetField(jcObject *Object,jbField *FieldID,jlong Value)noexcept
{
	return SetLongField(Object,FieldID,Value);
}
template<>
inline void jEnv::SetField(jcObject *Object,jbField *FieldID,jfloat Value)noexcept
{
	return SetFloatField(Object,FieldID,Value);
}
template<>
inline void jEnv::SetField(jcObject *Object,jbField *FieldID,jdouble Value)noexcept
{
	return SetDoubleField(Object,FieldID,Value);
}


//---------------------------------------------------------------------------

template<class TJavaRef>
inline TJavaRef jEnv::CallMethodA(jcObject *Object,jbMethod *MethodID,const jvalue *args)noexcept
{
	return static_cast<TJavaRef>(CallObjectMethodA(Object,MethodID,args));
}

template<>
inline jboolean jEnv::CallMethodA(jcObject *Object,jbMethod *MethodID,const jvalue *args)noexcept
{
	return CallBooleanMethodA(Object,MethodID,args);
}

template<>
inline jbyte jEnv::CallMethodA(jcObject *Object,jbMethod *MethodID,const jvalue *args)noexcept
{
	return CallByteMethodA(Object,MethodID,args);
}

template<>
inline jchar jEnv::CallMethodA(jcObject *Object,jbMethod *MethodID,const jvalue *args)noexcept
{
	return CallCharMethodA(Object,MethodID,args);
}

template<>
inline jshort jEnv::CallMethodA(jcObject *Object,jbMethod *MethodID,const jvalue *args)noexcept
{
	return CallShortMethodA(Object,MethodID,args);
}

template<>
inline jint jEnv::CallMethodA(jcObject *Object,jbMethod *MethodID,const jvalue *args)noexcept
{
	return CallIntMethodA(Object,MethodID,args);
}

template<>
inline jlong jEnv::CallMethodA(jcObject *Object,jbMethod *MethodID,const jvalue *args)noexcept
{
	return CallLongMethodA(Object,MethodID,args);
}

template<>
inline jfloat jEnv::CallMethodA(jcObject *Object,jbMethod *MethodID,const jvalue *args)noexcept
{
	return CallFloatMethodA(Object,MethodID,args);
}

template<>
inline jdouble jEnv::CallMethodA(jcObject *Object,jbMethod *MethodID,const jvalue *args)noexcept
{
	return CallDoubleMethodA(Object,MethodID,args);
}

template<>
inline void jEnv::CallMethodA(jcObject *Object,jbMethod *MethodID,const jvalue *args)noexcept
{
	return CallVoidMethodA(Object,MethodID,args);
}

//---------------------------------------------------------------------------

template<class TJavaRef>
inline TJavaRef jEnv::CallStaticMethodA(jcClass *Class,jbMethod *MethodID,const jvalue *args)noexcept
{
	return static_cast<TJavaRef>(CallStaticObjectMethodA(Class,MethodID,args));
}

template<>
inline jboolean jEnv::CallStaticMethodA(jcClass *Class,jbMethod *MethodID,const jvalue *args)noexcept
{
	return CallStaticBooleanMethodA(Class,MethodID,args);
}

template<>
inline jbyte jEnv::CallStaticMethodA(jcClass *Class,jbMethod *MethodID,const jvalue *args)noexcept
{
	return CallStaticByteMethodA(Class,MethodID,args);
}

template<>
inline jchar jEnv::CallStaticMethodA(jcClass *Class,jbMethod *MethodID,const jvalue *args)noexcept
{
	return CallStaticCharMethodA(Class,MethodID,args);
}

template<>
inline jshort jEnv::CallStaticMethodA(jcClass *Class,jbMethod *MethodID,const jvalue *args)noexcept
{
	return CallStaticShortMethodA(Class,MethodID,args);
}

template<>
inline jint jEnv::CallStaticMethodA(jcClass *Class,jbMethod *MethodID,const jvalue *args)noexcept
{
	return CallStaticIntMethodA(Class,MethodID,args);
}

template<>
inline jlong jEnv::CallStaticMethodA(jcClass *Class,jbMethod *MethodID,const jvalue *args)noexcept
{
	return CallStaticLongMethodA(Class,MethodID,args);
}

template<>
inline jfloat jEnv::CallStaticMethodA(jcClass *Class,jbMethod *MethodID,const jvalue *args)noexcept
{
	return CallStaticFloatMethodA(Class,MethodID,args);
}

template<>
inline jdouble jEnv::CallStaticMethodA(jcClass *Class,jbMethod *MethodID,const jvalue *args)noexcept
{
	return CallStaticDoubleMethodA(Class,MethodID,args);
}

template<>
inline void jEnv::CallStaticMethodA(jcClass *Class,jbMethod *MethodID,const jvalue *args)noexcept
{
	return CallStaticVoidMethodA(Class,MethodID,args);
}

/*
 * Manifest constants.
 */
#define JNI_FALSE   0
#define JNI_TRUE    1

#define JNI_VERSION_1_1 0x00010001
#define JNI_VERSION_1_2 0x00010002
#define JNI_VERSION_1_4 0x00010004
#define JNI_VERSION_1_6 0x00010006

#define JNI_OK          (0)         /* no error */
#define JNI_ERR         (-1)        /* generic error */
#define JNI_EDETACHED   (-2)        /* thread detached from the VM */
#define JNI_EVERSION    (-3)        /* JNI version error */
#define JNI_ENOMEM      (-4)        /* Out of memory */
#define JNI_EEXIST      (-5)        /* VM already created */
#define JNI_EINVAL      (-6)        /* Invalid argument */

#define JNI_COMMIT      1           /* copy content, do not free buffer */
#define JNI_ABORT       2           /* free buffer w/o copying back */

#endif // JNI_OK

//---------------------------------------------------------------------------
}	// namespace jCPP
//---------------------------------------------------------------------------
}	// namespace cnLibrary
//---------------------------------------------------------------------------
namespace cnLib_THelper{
namespace JNI{
//---------------------------------------------------------------------------

template<class T>
struct JNITypeSignatureWriterT
{
	static constexpr uIntn Length=2+cnMemory::ArrayLength(T::jClassName)-1;

	static char* Write(char *Dest)noexcept{
		*Dest++='L';
		for(uIntn i=0;i<cnMemory::ArrayLength(T::jClassName)-1;i++){
			*Dest++=T::jClassName[i];
		}
		*Dest++=';';
		return Dest;
	}
};

template<char c>
struct JNITypeSignatureWriterT_SingleCharacter
{
	static constexpr uIntn Length=1;

	static char* Write(char *Dest)noexcept{
		*Dest++=c;
		return Dest;
	}
};

template<>	struct JNITypeSignatureWriterT<void>	: JNITypeSignatureWriterT_SingleCharacter<'V'>{};
template<>	struct JNITypeSignatureWriterT<jboolean>: JNITypeSignatureWriterT_SingleCharacter<'Z'>{};
template<>	struct JNITypeSignatureWriterT<jbyte>	: JNITypeSignatureWriterT_SingleCharacter<'B'>{};
template<>	struct JNITypeSignatureWriterT<jshort>	: JNITypeSignatureWriterT_SingleCharacter<'S'>{};
template<>	struct JNITypeSignatureWriterT<jint>	: JNITypeSignatureWriterT_SingleCharacter<'I'>{};
template<>	struct JNITypeSignatureWriterT<jlong>	: JNITypeSignatureWriterT_SingleCharacter<'J'>{};
template<>	struct JNITypeSignatureWriterT<jfloat>	: JNITypeSignatureWriterT_SingleCharacter<'F'>{};
template<>	struct JNITypeSignatureWriterT<jdouble>	: JNITypeSignatureWriterT_SingleCharacter<'D'>{};
template<>	struct JNITypeSignatureWriterT<jchar>	: JNITypeSignatureWriterT_SingleCharacter<'C'>{};


template<class TEnable,class T>
struct TSelectSignatureType
	: cnVar::TTypeDef<T>{};

template<class T>
struct TSelectSignatureType<typename cnVar::TTypeConditional<void,cnVar::TIsConvertible<T*,jCPP::jcObject*>::Value>::Type,T*>

	: cnVar::TTypeDef<T>{};

template<class T>
struct JNITypeSignatureWriter	: JNITypeSignatureWriterT<typename TSelectSignatureType<void,T>::Type>{};

template<class...VT>
struct JNITypeSequenceSignatureWriter;

template<>
struct JNITypeSequenceSignatureWriter<>
{
	static constexpr uIntn Length=0;

	static char* Write(char *p)noexcept{
		return p;
	}
};


template<class T,class...VT>
struct JNITypeSequenceSignatureWriter<T,VT...>
{
	static constexpr uIntn Length=JNITypeSignatureWriter<T>::Length+JNITypeSequenceSignatureWriter<VT...>::Length;

	static char* Write(char *p)noexcept{
		p=JNITypeSignatureWriter<T>::Write(p);
		return JNITypeSequenceSignatureWriter<VT...>::Write(p);
	}
};


template<class TRet,class...TArgs>
struct JNIMethodSignatureGenerator
{
	char Value[cnLib_THelper::JNI::JNITypeSequenceSignatureWriter<TArgs...>::Length+2+cnLib_THelper::JNI::JNITypeSignatureWriter<TRet>::Length+1];
	JNIMethodSignatureGenerator()noexcept{
		Value[0]='(';
		char *p=Value+1;
		p=cnLib_THelper::JNI::JNITypeSequenceSignatureWriter<TArgs...>::Write(p);
		*p++=')';
		p=cnLib_THelper::JNI::JNITypeSignatureWriter<TRet>::Write(p);
		*p=0;
	}
};



template<auto...NativeFunctionPointers>
struct JNINativeMethodWriter;

template<>
struct JNINativeMethodWriter<>
{
	static void Write(jCPP::JNINativeMethod*,cnVar::cVarPackNoAccess)noexcept{}
};
template<class TRet,class...TArgs,TRet (*pFunction)(TArgs...)noexcept,auto...NativeFunctionPointers>
struct JNINativeMethodWriter<pFunction,NativeFunctionPointers...>
{
	template<class TVarPackAccessor>
	static void Write(jCPP::JNINativeMethod *p,TVarPackAccessor Accessor)noexcept{
		p->fnPtr=reinterpret_cast<void*>(pFunction);
		p->name=JNIMemberName<pFunction>::Value;
		p->signature=(*Accessor).Value;
		return JNINativeMethodWriter<NativeFunctionPointers...>::Write(p+1,+Accessor);
	}
};

template<auto Func>
struct TNativeSelectMethodSignatureGenerator;

template<class TThis,class TRet,class...TArgs,TRet (*pFunc)(jCPP::jEnv *env,TThis *obj,TArgs...)>
struct TNativeSelectMethodSignatureGenerator<pFunc>
	: cnVar::TTypeDef< JNIMethodSignatureGenerator<TRet,TArgs...> >
{
};

template<auto...NativeFunctions>
struct JNINativeMethodGenerator
{
	jCPP::JNINativeMethod Value[sizeof...(NativeFunctions)];
	static constexpr jint Length=sizeof...(NativeFunctions);

	cnVar::cVarPack<typename TNativeSelectMethodSignatureGenerator<NativeFunctions>::Type...> Packs;

	JNINativeMethodGenerator()noexcept{
		JNINativeMethodWriter<NativeFunctions...>::Write(Value,Packs.All());
	}
};
//---------------------------------------------------------------------------
}	// namespace JNI
}	// namespace cnLib_THelper
//---------------------------------------------------------------------------
namespace cnLibrary{
namespace jCPP{
//---------------------------------------------------------------------------
class cJNIInitializationRegistration
{
public:
	class cInitialization
	{
	private:
		friend cJNIInitializationRegistration;
		cInitialization *Next;
		cInitialization *Prev;

		virtual void Initialize(jEnv *env)noexcept(true)=0;
		virtual void Finalize(jEnv *env)noexcept(true)=0;
	};


	void Register(cInitialization *Initialization)noexcept(true);

	void Initialize(jEnv *env)noexcept(true);
	void Finalize(jEnv *env)noexcept(true);

private:
	cInitialization *fTail=nullptr;
	cInitialization *fHead=nullptr;
	cInitialization *fInitialized=nullptr;
};
//---------------------------------------------------------------------------
class bcJNIInitialization : public cJNIInitializationRegistration::cInitialization
{
protected:
	bcJNIInitialization()noexcept;
};
//---------------------------------------------------------------------------
class cJNIModule
{
public:

	struct cVar
	{
		jVM *vm;
	};
	const cVar* operator ->()const noexcept;

	void NativeActivityCreated(jEnv *env,jVM *vm)noexcept;
	void Initialize(jEnv *env)noexcept;
	void Finalize(jEnv *env)noexcept;

	static cJNIModule gJNIModule;
private:
	cVar fVar;
	friend bcJNIInitialization;
	static cJNIInitializationRegistration gRegistration;
};
//---------------------------------------------------------------------------
class jcEnv
{
public:
	jcEnv()noexcept;
	~jcEnv()noexcept;

	operator jEnv*()const noexcept;
	jEnv* operator ->()const noexcept;
protected:
	jEnv *fEnv;
	bool fNeedDetach;
};
//---------------------------------------------------------------------------
template<class T>
class jrLocal
{
public:
	jrLocal(jEnv *env)noexcept
		: fEnv(env)
		,fJavaRef(nullptr)
	{}

	jrLocal(jEnv *env,T *Object)noexcept
		: fEnv(env)
		,fJavaRef(Object)
	{}

	~jrLocal()noexcept{
		if(fJavaRef!=nullptr){
			fEnv->DeleteLocalRef(fJavaRef);
			if(fEnv->ExceptionCheck()){
				fEnv->ExceptionDescribe();
				fEnv->ExceptionClear();
			}
		}
	}

	jrLocal(const jrLocal&)=delete;
	jrLocal(jrLocal &&Src)noexcept{
		fEnv=Src.fEnv;
		fJavaRef=Src.fJavaRef;
		Src.fJavaRef=nullptr;
	}

	jrLocal& operator=(const jrLocal &Src)=delete;
	jrLocal& operator=(jrLocal &&Src){
		cnLib_ASSERT(fEnv==Src.fEnv);
		if(fJavaRef!=nullptr){
			fEnv->DeleteLocalRef(fJavaRef);
			if(fEnv->ExceptionCheck()){
				fEnv->ExceptionDescribe();
				fEnv->ExceptionClear();
			}
		}
		fJavaRef=Src.fJavaRef;
		Src.fJavaRef=nullptr;
		return *this;
	}

	T Get(void)const noexcept{ return fJavaRef; }
	void Set(T Ref)noexcept{
		if(fJavaRef!=nullptr){
			fEnv->DeleteLocalRef(fJavaRef);
			if(fEnv->ExceptionCheck()){
				fEnv->ExceptionDescribe();
				fEnv->ExceptionClear();
			}
		}
		fJavaRef=Ref;
	}
	void Clear(void)noexcept{
		if(fJavaRef!=nullptr){
			fEnv->DeleteLocalRef(fJavaRef);
			if(fEnv->ExceptionCheck()){
				fEnv->ExceptionDescribe();
				fEnv->ExceptionClear();
			}
			T::v=nullptr;
		}
	}

	template<class TDest>
	typename cnVar::TTypeConditional<jrLocal<TDest>&&,cnVar::TIsConvertible<T*,TDest*>::Value>
		::Type Transfer(void)noexcept
	{
		return reinterpret_cast<jrLocal<TDest>&&>(*this);
	}


#ifdef JNI_OK
	jobject obj()const noexcept{ return reinterpret_cast<jobject>(fJavaRef); }
#endif // JNI_OK

	operator T*()const noexcept{ return fJavaRef; }
	T* operator ->()const noexcept{ return fJavaRef; }

	jEnv* env()const noexcept{ return fEnv; }

	T* Return(void)noexcept{ auto Ret=fJavaRef;	fJavaRef=nullptr;	return Ret; }
protected:
	jEnv *fEnv;
	T *fJavaRef;
};
template<class T>
inline jrLocal<T> RefLocal(jEnv *env,T *Object)noexcept
{
	return jrLocal<T>(env,Object);
}

template<class T,class TSrc>
inline typename cnVar::TTypeConditional<jrLocal<T>,
	cnVar::TIsConvertible<TSrc*,T*>::Value
>::Type jRefCast(jrLocal<TSrc> &&Object)noexcept(true)
{
	return Object.template Transfer<T>();
}
//---------------------------------------------------------------------------
template<class T>
class jrGlobal
{
public:
	jrGlobal()noexcept : fJavaRef(nullptr){}
	~jrGlobal()noexcept{
		if(fJavaRef!=nullptr){
			jcEnv env;
			env->DeleteGlobalRef(fJavaRef);
			if(env->ExceptionCheck()){
				env->ExceptionDescribe();
				env->ExceptionClear();
			}
		}
	}


	jrGlobal(jEnv *env,T *Ref)noexcept{
		if(Ref!=nullptr){
			fJavaRef=static_cast<T*>(env->NewGlobalRef(Ref));
			if(env->ExceptionCheck()){
				env->ExceptionDescribe();
				env->ExceptionClear();
			}
		}
		else{
			fJavaRef=nullptr;
		}
	}


	jrGlobal(const jrGlobal&)=delete;
	jrGlobal(jrGlobal &&Src)noexcept{
		fJavaRef=Src.fJavaRef;
		Src.fJavaRef=nullptr;
	}

	jrGlobal& operator=(const jrGlobal &Src)=delete;
	jrGlobal& operator=(jrGlobal &&Src){
		if(fJavaRef!=nullptr){
			jcEnv env;
			env->DeleteGlobalRef(fJavaRef);
			if(env->ExceptionCheck()){
				env->ExceptionDescribe();
				env->ExceptionClear();
			}
		}
		fJavaRef=Src.fJavaRef;
		Src.fJavaRef=nullptr;
		return *this;
	}

	template<class TSrc>
	jrGlobal(const jrLocal<TSrc> &Src)noexcept{
		static_assert(cnVar::TIsConvertible<TSrc*,T*>::Value,"incompatible source reference");
		if(Src!=nullptr){
			auto env=Src.env();
			fJavaRef=static_cast<T*>(env->NewGlobalRef(Src));
			if(env->ExceptionCheck()){
				env->ExceptionDescribe();
				env->ExceptionClear();
			}
		}
		else{
			fJavaRef=nullptr;
		}
	}
	template<class TSrc>
	jrGlobal& operator=(const jrLocal<TSrc> &Src){
		static_assert(cnVar::TIsConvertible<TSrc*,T*>::Value,"incompatible source reference");
		if(Src!=nullptr){
			jcEnv env;
			if(fJavaRef!=nullptr){
				env->DeleteGlobalRef(fJavaRef);
				if(env->ExceptionCheck()){
					env->ExceptionDescribe();
					env->ExceptionClear();
				}
			}
			fJavaRef=static_cast<T*>(Src.env()->NewGlobalRef(Src));
			if(env->ExceptionCheck()){
				env->ExceptionDescribe();
				env->ExceptionClear();
			}
		}
		else{
			if(fJavaRef!=nullptr){
				jcEnv env;
				env->DeleteGlobalRef(fJavaRef);
				if(env->ExceptionCheck()){
					env->ExceptionDescribe();
					env->ExceptionClear();
				}
				fJavaRef=nullptr;
			}
		}
		return *this;
	}


	T Get(void)const noexcept{ return fJavaRef; }
	void Set(jEnv *env,T* Ref)noexcept{
		if(fJavaRef!=nullptr){
			env->DeleteGlobalRef(fJavaRef);
			if(env->ExceptionCheck()){
				env->ExceptionDescribe();
				env->ExceptionClear();
			}
		}
		if(Ref!=nullptr){
			fJavaRef=static_cast<T*>(env->NewGlobalRef(Ref));
			if(env->ExceptionCheck()){
				env->ExceptionDescribe();
				env->ExceptionClear();
			}
		}
		else{
			fJavaRef=nullptr;
		}
	}

	void Clear(jEnv *env)noexcept{
		if(fJavaRef!=nullptr){
			env->DeleteGlobalRef(fJavaRef);
			if(env->ExceptionCheck()){
				env->ExceptionDescribe();
				env->ExceptionClear();
			}
			fJavaRef=nullptr;
		}
	}

#ifdef JNI_OK
	jobject obj()const noexcept{ return reinterpret_cast<jobject>(fJavaRef); }
#endif // JNI_OK

	operator T*()const noexcept{ return fJavaRef; }
	T* operator ->()const noexcept{ return fJavaRef; }

protected:
	T *fJavaRef;
};
//---------------------------------------------------------------------------
template<class TField>
struct jFieldAccess
{
	jFieldAccess(jcObject *JavaRef,jField<TField> *FieldID)noexcept
		: JavaRef(JavaRef)
		, FieldID(FieldID)
	{}

	TField Get(jEnv *env)noexcept{
		TField Ret=env->GetField<TField>(JavaRef,FieldID);
		if(env->ExceptionCheck()){
			env->ExceptionDescribe();
			env->ExceptionClear();
		}
		return Ret;
	}

	void Set(jEnv *env,TField Value)noexcept{
		env->SetField<TField>(JavaRef,FieldID,Value);
		if(env->ExceptionCheck()){
			env->ExceptionDescribe();
			env->ExceptionClear();
		}
	}

	jcObject *JavaRef;
	jField<TField> *FieldID;
};
//---------------------------------------------------------------------------
template<class TFieldClass>
struct jFieldAccess<TFieldClass*>
{
	jFieldAccess(jcObject *JavaRef,jField<TFieldClass*> *FieldID)noexcept
		: JavaRef(JavaRef)
		, FieldID(FieldID)
	{}

	jrLocal<TFieldClass> Get(jEnv *env)noexcept{
		TFieldClass* Ret=env->GetField<TFieldClass*>(JavaRef,FieldID);
		if(env->ExceptionCheck()){
			env->ExceptionDescribe();
			env->ExceptionClear();
		}
		return jrLocal<TFieldClass>(env,Ret);
	}

	void Set(jEnv *env,TFieldClass *Value)noexcept{
		env->SetField<TFieldClass*>(JavaRef,FieldID,Value);
		if(env->ExceptionCheck()){
			env->ExceptionDescribe();
			env->ExceptionClear();
		}
	}

	jcObject *JavaRef;
	jField<TFieldClass*> *FieldID;
};
//---------------------------------------------------------------------------
template<class T>
struct jPointerFieldAccess
{
	jPointerFieldAccess(jcObject *JavaRef,jField<jlong> *FieldID)noexcept
		: JavaRef(JavaRef)
		, FieldID(FieldID)
	{}

	T* Get(jEnv *env)noexcept{
		jlong Ret=env->GetField<jlong>(JavaRef,FieldID);
		if(env->ExceptionCheck()){
			env->ExceptionDescribe();
			env->ExceptionClear();
		}
		return reinterpret_cast<T*>(static_cast<uIntn>(Ret));
	}

	void Set(jEnv *env,T *Value)noexcept{
		env->SetField<jlong>(JavaRef,FieldID,static_cast<jlong>(reinterpret_cast<uIntn>(Value)));
		if(env->ExceptionCheck()){
			env->ExceptionDescribe();
			env->ExceptionClear();
		}
	}

	jcObject *JavaRef;
	jField<jlong> *FieldID;
};
//---------------------------------------------------------------------------
template<class TField>
struct jStaticFieldAccess
{
	jStaticFieldAccess(jcClass *cls,jField<TField> *FieldID)noexcept
		: Class(cls)
		, FieldID(FieldID)
	{}

	TField Get(jEnv *env)noexcept{
		TField Ret=env->GetStaticField<TField>(Class,FieldID);
		if(env->ExceptionCheck()){
			env->ExceptionDescribe();
			env->ExceptionClear();
		}
		return Ret;
	}

	void Set(jEnv *env,TField Value)noexcept{
		env->SetStaticField<TField>(Class,FieldID,Value);
		if(env->ExceptionCheck()){
			env->ExceptionDescribe();
			env->ExceptionClear();
		}
	}

	jcClass *Class;
	jField<TField> *FieldID;
};
//---------------------------------------------------------------------------
template<class TFieldClass>
struct jStaticFieldAccess<TFieldClass*>
{
	jStaticFieldAccess(jcClass *cls,jField<TFieldClass*> *FieldID)noexcept
		: Class(cls)
		, FieldID(FieldID)
	{}

	jrLocal<TFieldClass> Get(jEnv *env)noexcept{
		TFieldClass* Ret=env->GetStaticField<TFieldClass*>(Class,FieldID);
		if(env->ExceptionCheck()){
			env->ExceptionDescribe();
			env->ExceptionClear();
		}
		return jrLocal<TFieldClass>(env,Ret);
	}

	void Set(jEnv *env,TFieldClass *Value)noexcept{
		env->SetStaticField<TFieldClass*>(Class,FieldID,Value);
		if(env->ExceptionCheck()){
			env->ExceptionDescribe();
			env->ExceptionClear();
		}
	}

	jcClass *Class;
	jField<TFieldClass*> *FieldID;
};
//---------------------------------------------------------------------------
template<class T>
struct jPointerStaticFieldAccess
{
	jPointerStaticFieldAccess(jcClass *cls,jField<jlong> *FieldID)noexcept
		: Class(cls)
		, FieldID(FieldID)
	{}

	T* Get(jEnv *env)noexcept{
		jlong Ret=env->GetStaticField<jlong>(Class,FieldID);
		if(env->ExceptionCheck()){
			env->ExceptionDescribe();
			env->ExceptionClear();
		}
		return reinterpret_cast<T*>(static_cast<uIntn>(Ret));
	}

	void Set(jEnv *env,T *Value)noexcept{
		env->SetStaticField<jlong>(Class,FieldID,static_cast<jlong>(reinterpret_cast<uIntn>(Value)));
		if(env->ExceptionCheck()){
			env->ExceptionDescribe();
			env->ExceptionClear();
		}
	}

	jcClass *Class;
	jField<jlong> *FieldID;
};
//---------------------------------------------------------------------------
template<class T>
struct TClassRef
{
	class cInitialization : public bcJNIInitialization
	{
	public:
		jcClass *Value;

		virtual void Initialize(jEnv *env)noexcept{
			auto cls=env->FindClass(T::jClassName);
			if(cls==nullptr){
				if(env->ExceptionCheck()){
					env->ExceptionDescribe();
					env->ExceptionClear();
				}
			}
			else{
				Value=static_cast<jcClass*>(env->NewGlobalRef(cls));
				if(env->ExceptionCheck()){
					env->ExceptionDescribe();
					env->ExceptionClear();
				}
				env->DeleteLocalRef(cls);
				if(env->ExceptionCheck()){
					env->ExceptionDescribe();
					env->ExceptionClear();
				}
			}
		}
		virtual void Finalize(jEnv *env)noexcept{
			env->DeleteGlobalRef(reinterpret_cast<jcObject*>(Value));
			if(env->ExceptionCheck()){
				env->ExceptionDescribe();
				env->ExceptionClear();
			}
		}

	};
	static cInitialization Init;

	static constexpr jcClass*const &Value=Init.Value;
};
template<class T>
typename TClassRef<T>::cInitialization TClassRef<T>::Init;

//---------------------------------------------------------------------------
template<auto pFunction>
struct TInstanceField;
//---------------------------------------------------------------------------
template<class TClass,class TField,jFieldAccess<TField> (TClass::*pFunction)(void)noexcept>
struct TInstanceField<pFunction>
{
	class cInitialization : public bcJNIInitialization
	{
	public:
		jField<TField> *Value;

		virtual void Initialize(jEnv *env)noexcept override{
			char TypeSignature[cnLib_THelper::JNI::JNITypeSignatureWriter<TField>::Length+1];
			auto *p=cnLib_THelper::JNI::JNITypeSignatureWriter<TField>::Write(TypeSignature);
			*p=0;
			Value=static_cast<jField<TField>*>(
				env->GetFieldID(TClassRef<TClass>::Value,JNIMemberName<pFunction>::Value,TypeSignature));
			if(Value==nullptr){
				if(env->ExceptionCheck()){
					env->ExceptionDescribe();
					env->ExceptionClear();
				}
			}
		}
		virtual void Finalize(jEnv*)noexcept override{
			Value=nullptr;
		}
	};
	static cInitialization Init;

	static constexpr jField<TField>*const &Value=Init.Value;
};
template<class TClass,class TField,jFieldAccess<TField> (TClass::*pFunction)(void)noexcept>
typename TInstanceField<pFunction>::cInitialization TInstanceField<pFunction>::Init;
//---------------------------------------------------------------------------
template<class TClass,class T,jPointerFieldAccess<T> (TClass::*pFunction)(void)noexcept>
struct TInstanceField<pFunction>
{
	class cInitialization : public bcJNIInitialization
	{
	public:
		jField<jlong> *Value;

		virtual void Initialize(jEnv *env)noexcept override{
			char TypeSignature[cnLib_THelper::JNI::JNITypeSignatureWriter<jlong>::Length+1];
			auto p=cnLib_THelper::JNI::JNITypeSignatureWriter<jlong>::Write(TypeSignature);
			*p=0;
			Value=static_cast<jField<jlong>*>(
				env->GetFieldID(TClassRef<TClass>::Value,JNIMemberName<pFunction>::Value,TypeSignature));
			if(Value==nullptr){
				if(env->ExceptionCheck()){
					env->ExceptionDescribe();
					env->ExceptionClear();
				}
			}
		}
		virtual void Finalize(jEnv*)noexcept override{
			Value=nullptr;
		}
	};
	static cInitialization Init;

	static constexpr jField<jlong>*const &Value=Init.Value;
};
template<class TClass,class T,jPointerFieldAccess<T> (TClass::*pFunction)(void)noexcept>
typename TInstanceField<pFunction>::cInitialization TInstanceField<pFunction>::Init;
//---------------------------------------------------------------------------
template<class TClass,auto pFunction>
struct TStaticField;
//---------------------------------------------------------------------------
template<class TClass,class TField,jStaticFieldAccess<TField> (*pFunction)(void)noexcept>
struct TStaticField<TClass,pFunction>
{
	class cInitialization : public bcJNIInitialization
	{
	public:
		jField<TField> *Value;

		virtual void Initialize(jEnv *env)noexcept override{
			char TypeSignature[cnLib_THelper::JNI::JNITypeSignatureWriter<TField>::Length+1];
			auto *p=cnLib_THelper::JNI::JNITypeSignatureWriter<TField>::Write(TypeSignature);
			*p=0;

			Value=static_cast<jField<TField>*>(
				env->GetStaticFieldID(TClassRef<TClass>::Value,JNIMemberName<pFunction>::Value,TypeSignature));
			if(Value==nullptr){
				if(env->ExceptionCheck()){
					env->ExceptionDescribe();
					env->ExceptionClear();
				}
			}
		}
		virtual void Finalize(jEnv*)noexcept override{
			Value=nullptr;
		}
	};
	static cInitialization Init;
	typedef TField tField;

	static constexpr jField<TField>*const &Value=Init.Value;
};
template<class TClass,class TField,jStaticFieldAccess<TField> (*pFunction)(void)noexcept>
typename TStaticField<TClass,pFunction>::cInitialization TStaticField<TClass,pFunction>::Init;

template<class TClass,auto pFunction>
jStaticFieldAccess<typename TStaticField<TClass,pFunction>::tField> StaticFieldAccess(void)noexcept
{
	return {TClassRef<TClass>::Value,TStaticField<TClass,pFunction>::Value};
}
//---------------------------------------------------------------------------
template<auto pFunction>
struct TInstanceMethod;
//---------------------------------------------------------------------------
template<class TClass,class TRet,class...TArgs,TRet (TClass::*pFunction)(jEnv*,TArgs...)noexcept>
struct TInstanceMethod<pFunction>
{
	class cInitialization : public bcJNIInitialization
	{
	public:
		jMethod<TRet (TClass::*)(TArgs...)> *Value;

		virtual void Initialize(jEnv *env)noexcept override{
			cnLib_THelper::JNI::JNIMethodSignatureGenerator<TRet,TArgs...> SigGen;
			Value=static_cast<jMethod<TRet (TClass::*)(TArgs...)>*>(
				env->GetMethodID(TClassRef<TClass>::Value,JNIMemberName<pFunction>::Value,SigGen.Value));
			if(Value==nullptr){
				if(env->ExceptionCheck()){
					env->ExceptionDescribe();
					env->ExceptionClear();
				}
			}
		}
		virtual void Finalize(jEnv*)noexcept override{
			Value=nullptr;
		}
	};
	static cInitialization Init;

	static constexpr jMethod<TRet (TClass::*)(TArgs...)>*const &Value=Init.Value;
};
template<class TClass,class TRet,class...TArgs,TRet (TClass::*pFunction)(jEnv*,TArgs...)noexcept>
typename TInstanceMethod<pFunction>::cInitialization TInstanceMethod<pFunction>::Init;
//---------------------------------------------------------------------------
template<class TClass,class TRet,class...TArgs,jrLocal<TRet> (TClass::*pFunction)(jEnv*,TArgs...)noexcept>
struct TInstanceMethod<pFunction>
{
	class cInitialization : public bcJNIInitialization
	{
	public:
		jMethod<TRet* (TClass::*)(TArgs...)> *Value;

		virtual void Initialize(jEnv *env)noexcept override{
			cnLib_THelper::JNI::JNIMethodSignatureGenerator<TRet*,TArgs...> SigGen;
			Value=static_cast<jMethod<TRet* (TClass::*)(TArgs...)>*>(
				env->GetMethodID(TClassRef<TClass>::Value,JNIMemberName<pFunction>::Value,SigGen.Value));
			if(Value==nullptr){
				if(env->ExceptionCheck()){
					env->ExceptionDescribe();
					env->ExceptionClear();
				}
			}
		}
		virtual void Finalize(jEnv*)noexcept override{
			Value=nullptr;
		}
	};
	static cInitialization Init;

	static constexpr jMethod<TRet* (TClass::*)(TArgs...)>*const &Value=Init.Value;
};
template<class TClass,class TRet,class...TArgs,jrLocal<TRet> (TClass::*pFunction)(jEnv*,TArgs...)noexcept>
typename TInstanceMethod<pFunction>::cInitialization TInstanceMethod<pFunction>::Init;
//---------------------------------------------------------------------------
template<class TClass,auto pFunction>
struct TStaticMethod;
//---------------------------------------------------------------------------
template<class TClass,class TRet,class...TArgs,TRet (*pFunction)(jEnv*,TArgs...)noexcept>
struct TStaticMethod<TClass,pFunction>
{
	class cInitialization : public bcJNIInitialization
	{
	public:
		jStaticMethod<TClass,TRet (TArgs...)> *Value;

		virtual void Initialize(jEnv *env)noexcept override{
			cnLib_THelper::JNI::JNIMethodSignatureGenerator<TRet,TArgs...> SigGen;
			Value=static_cast<jStaticMethod<TClass,TRet (TArgs...)>*>(
				env->GetStaticMethodID(TClassRef<TClass>::Value,JNIMemberName<pFunction>::Value,SigGen.Value));
			if(Value==nullptr){
				if(env->ExceptionCheck()){
					env->ExceptionDescribe();
					env->ExceptionClear();
				}
			}
		}
		virtual void Finalize(jEnv*)noexcept override{
			Value=nullptr;
		}
	};
	static cInitialization Init;

	static constexpr jStaticMethod<TClass,TRet (TArgs...)>*const &Value=Init.Value;
};
template<class TClass,class TRet,class...TArgs,TRet (*pFunction)(jEnv*,TArgs...)noexcept>
typename TStaticMethod<TClass,pFunction>::cInitialization TStaticMethod<TClass,pFunction>::Init;
//---------------------------------------------------------------------------
template<class TClass,class TRet,class...TArgs,jrLocal<TRet> (*pFunction)(jEnv*,TArgs...)noexcept>
struct TStaticMethod<TClass,pFunction>
{
	class cInitialization : public bcJNIInitialization
	{
	public:
		jStaticMethod<TClass,TRet* (TArgs...)> *Value;

		virtual void Initialize(jEnv *env)noexcept override{
			cnLib_THelper::JNI::JNIMethodSignatureGenerator<TRet*,TArgs...> SigGen;
			Value=static_cast<jStaticMethod<TClass,TRet* (TArgs...)>*>(
				env->GetStaticMethodID(TClassRef<TClass>::Value,JNIMemberName<pFunction>::Value,SigGen.Value));
			if(Value==nullptr){
				if(env->ExceptionCheck()){
					env->ExceptionDescribe();
					env->ExceptionClear();
				}
			}
		}
		virtual void Finalize(jEnv*)noexcept override{
			Value=nullptr;
		}
	};
	static cInitialization Init;

	static constexpr jStaticMethod<TClass,TRet* (TArgs...)>*const &Value=Init.Value;
};
template<class TClass,class TRet,class...TArgs,jrLocal<TRet> (*pFunction)(jEnv*,TArgs...)noexcept>
typename TStaticMethod<TClass,pFunction>::cInitialization TStaticMethod<TClass,pFunction>::Init;

template<class TClass,auto pFunction>
struct TConstructorMethod;

template<class TClass,class...TArgs,jrLocal<TClass> (*pFunction)(jEnv*,TArgs...)noexcept>
struct TConstructorMethod<TClass,pFunction>
{
	class cInitialization : public bcJNIInitialization
	{
	public:
		jMethod<void (TArgs...)> *Value;

		virtual void Initialize(jEnv *env)noexcept override{
			cnLib_THelper::JNI::JNIMethodSignatureGenerator<void,TArgs...> SigGen;
			Value=static_cast<jMethod<void (TArgs...)>*>(
				env->GetMethodID(TClassRef<TClass>::Value,"<init>",SigGen.Value));
			if(Value==nullptr){
				if(env->ExceptionCheck()){
					env->ExceptionDescribe();
					env->ExceptionClear();
				}
			}
		}
		virtual void Finalize(jEnv*)noexcept override{
			Value=nullptr;
		}
	};
	static cInitialization Init;

	static constexpr jMethod<void (TArgs...)>*const &Value=Init.Value;
};
template<class TClass,class...TArgs,jrLocal<TClass> (*pFunction)(jEnv*,TArgs...)noexcept>
typename TConstructorMethod<TClass,pFunction>::cInitialization TConstructorMethod<TClass,pFunction>::Init;
//---------------------------------------------------------------------------
template<class TClass,class...TArgs>
jrLocal<TClass> NewJavaObject(jEnv *env,TArgs...Args)noexcept
{
	TClass *Ret=env->New<TClass>(TConstructorMethod<TClass,&NewJavaObject<TClass,TArgs...> >::Value,Args...);
	if(Ret==nullptr){
		if(env->ExceptionCheck()){
			env->ExceptionDescribe();
			env->ExceptionClear();
		}
	}
	return RefLocal(env,Ret);
}
//---------------------------------------------------------------------------
template<class T,auto...Func>
class JNINativeMethodRegister : public bcJNIInitialization
{
public:
	virtual void Initialize(jEnv *env)noexcept override{
		cnLib_THelper::JNI::JNINativeMethodGenerator<Func...> ms;
		env->RegisterNatives(TClassRef<T>::Value,ms.Value,ms.Length);
		if(env->ExceptionCheck()){
			env->ExceptionDescribe();
			env->ExceptionClear();
		}
	}
	virtual void Finalize(jEnv *env)noexcept override{
		env->UnregisterNatives(TClassRef<T>::Value);
		if(env->ExceptionCheck()){
			env->ExceptionDescribe();
			env->ExceptionClear();
		}
	}
};
//---------------------------------------------------------------------------
struct jcObject
{
	static constexpr const char jClassName[]="java/lang/Object";

	jrLocal<jcClass> getClass(jEnv *env)noexcept;
	jrLocal<jcString> toString(jEnv *env)noexcept;
};
//---------------------------------------------------------------------------
struct jcClass : jcObject
{
	static constexpr const char jClassName[]="java/lang/Class";
};
//---------------------------------------------------------------------------
struct jStringAccessCritical
{
	jStringAccessCritical(jEnv *env,jcString *s)noexcept;
	~jStringAccessCritical()noexcept;

	jEnv *env;
	jcString *StringRef;
	const uChar16 *Pointer;
	bool IsCopy;
};
//---------------------------------------------------------------------------
struct jcString : jcObject
{
	static constexpr const char jClassName[]="java/lang/String";
	
	template<uIntn Length>
	static jrLocal<jcString> Make(jEnv *env,const uChar16 (&Text)[Length])noexcept{
		return Make(env,Text,cnString::FindLength(Text,Length));
	}
	static jrLocal<jcString> Make(jEnv *env,const uChar16 *Text,uIntn Length)noexcept;

	jsize length(jEnv *env)noexcept;

	jStringAccessCritical AccessCritical(jEnv *env)noexcept;

};
//---------------------------------------------------------------------------
struct jcThrowable : jcObject
{
};
//---------------------------------------------------------------------------
struct jbcArray : jcObject
{
	jsize GetArrayLength(jEnv *env)noexcept;
};
//---------------------------------------------------------------------------
template<class TElement>
struct jcArray;
//---------------------------------------------------------------------------
template<class TElement>
struct jArrayAccess;
//---------------------------------------------------------------------------
template<class TObjectElement>
struct jcArray : jbcArray
{
	jsize GetArrayLength(jEnv *env)noexcept{
		return env->GetArrayLength(this);
	}

	jrLocal<TObjectElement> GetElement(jEnv *env,jint Index)noexcept{
		jobject ArrayObjectItem=env->GetObjectArrayElement(this,Index);
		return jrLocal<TObjectElement>(env,ArrayObjectItem);
	}
	void SetElement(jEnv *env,jint Index,TObjectElement *Value)noexcept{
		return env->SetObjectArrayElement(this,Index,Value);
	}

	static jrLocal< jcArray<TObjectElement> > Make(jEnv *env,uIntn Length)noexcept{
		return RefLocal(env,reinterpret_cast<jcArray<TObjectElement>*>(env->NewObjectArray(Length,TClassRef<TObjectElement>::Value,nullptr)));
	}
};
//---------------------------------------------------------------------------
template<>
struct jcArray<jboolean> : jbcArray
{
	//jArrayAccess<jboolean> AccessElements(jEnv *env)noexcept;

};
//---------------------------------------------------------------------------
template<>
struct jArrayAccess<jbyte>
{
	jArrayAccess(jEnv *env,jcArray<jbyte> *aref)noexcept
		: env(env)
		,JavaArray(aref)
		,Pointer(env->GetByteArrayElements(aref,reinterpret_cast<jboolean*>(&IsCopy)))
	{
	}

	~jArrayAccess()noexcept{
		if(Pointer!=nullptr){
			env->ReleaseByteArrayElements(JavaArray,Pointer,Abort?JNI_ABORT:0);
		}
	}

	void Commit(void)noexcept{
		if(Pointer!=nullptr){
			env->ReleaseByteArrayElements(JavaArray,Pointer,JNI_COMMIT);
		}
	}

	jEnv *env;
	jcArray<jbyte> *JavaArray;
	jbyte *Pointer;
	bool IsCopy;
	bool Abort=false;
};
//---------------------------------------------------------------------------
template<>
struct jcArray<jbyte> : jbcArray
{
	static jrLocal< jcArray<jbyte> > Make(jEnv *env,jsize length)noexcept;
	jArrayAccess<jbyte> AccessElements(jEnv *env)noexcept;
	
	void GetRegion(jEnv *env,jsize start,jsize len,jbyte *buf)noexcept;
	void SetRegion(jEnv *env,jsize start,jsize len,const jbyte *buf)noexcept;
};
//---------------------------------------------------------------------------
}	// namespace jCPP
//---------------------------------------------------------------------------
}	// namespace cnLibrary
//---------------------------------------------------------------------------
namespace cnLib_THelper{
namespace JNI{
// array
template<class T>
struct JNITypeSignatureWriterT< jCPP::jcArray<T> >
{
	static constexpr uIntn Length=1+JNITypeSignatureWriter<T>::Length;

	static char* Write(char *Dest)noexcept{
		*Dest++='[';
		return JNITypeSignatureWriter<T>::Write(Dest);
	}
};


template<>	struct JNITypeSignatureWriterT<jobject>		: JNITypeSignatureWriterT<jCPP::jcObject>{};
template<>	struct JNITypeSignatureWriterT<jclass>		: JNITypeSignatureWriterT<jCPP::jcClass>{};
template<>	struct JNITypeSignatureWriterT<jstring>		: JNITypeSignatureWriterT<jCPP::jcString>{};
template<>	struct JNITypeSignatureWriterT<jthrowable>	: JNITypeSignatureWriterT<jCPP::jcThrowable>{};


template<>	struct JNITypeSignatureWriterT<jbooleanArray>	: JNITypeSignatureWriterT< jCPP::jcArray<jboolean> >{};
template<>	struct JNITypeSignatureWriterT<jbyteArray>		: JNITypeSignatureWriterT< jCPP::jcArray<jbyte> >{};
template<>	struct JNITypeSignatureWriterT<jcharArray>		: JNITypeSignatureWriterT< jCPP::jcArray<jchar> >{};
template<>	struct JNITypeSignatureWriterT<jshortArray>		: JNITypeSignatureWriterT< jCPP::jcArray<jshort> >{};
template<>	struct JNITypeSignatureWriterT<jintArray>		: JNITypeSignatureWriterT< jCPP::jcArray<jint> >{};
template<>	struct JNITypeSignatureWriterT<jlongArray>		: JNITypeSignatureWriterT< jCPP::jcArray<jlong> >{};
template<>	struct JNITypeSignatureWriterT<jfloatArray>		: JNITypeSignatureWriterT< jCPP::jcArray<jfloat> >{};
template<>	struct JNITypeSignatureWriterT<jdoubleArray>	: JNITypeSignatureWriterT< jCPP::jcArray<jdouble> >{};
//---------------------------------------------------------------------------
}	// namespace JNI
}	// namespace cnLib_THelper
//---------------------------------------------------------------------------

#endif
