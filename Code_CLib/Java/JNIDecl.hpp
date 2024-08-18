/*- cnAndroid - JNI - interface declaration -------------------------------*/
/*         Developer : Code009                                             */
/*         Create on : 2024-06-21                                          */
/*-------------------------------------------------------------------------*/
#pragma once
#include <cnTK/cnTK.h>

#ifdef __cplusplus
//---------------------------------------------------------------------------
namespace cnLibrary{
//---------------------------------------------------------------------------
namespace jCPP{
//---------------------------------------------------------------------------

struct jcObject;
struct jcClass;
struct jcString;
struct jcThrowable;
struct jbcArray;
template<class T> struct jcArray;


struct jbField{};	// field id
struct jbMethod{};	// method id

template<class TVariable>	struct jField : jbField{};
template<class TFunction>	struct jMethod : jbMethod{};
template<class TJavaClass,class TFunction>	struct jStaticMethod : jbMethod{};

#ifdef JNI_OK
// <jni.h> is included, use declaration from <jni.h>

#define	JCPP_JNIHEADER	1

using ::JNINativeMethod;


using ::jboolean;
using ::jbyte;
using ::jchar;
using ::jshort;
using ::jint;
using ::jlong;
using ::jfloat;
using ::jdouble;

using ::jsize;
using ::jvalue;

static constexpr jboolean jtrue		=JNI_TRUE;
static constexpr jboolean jfalse	=JNI_FALSE;

static constexpr jint jVERSION_1_1	=JNI_VERSION_1_1;
static constexpr jint jVERSION_1_2	=JNI_VERSION_1_2;
static constexpr jint jVERSION_1_4	=JNI_VERSION_1_4;
static constexpr jint jVERSION_1_6	=JNI_VERSION_1_6;

static constexpr jint jRet_OK			=JNI_OK;
static constexpr jint jRet_ERR			=JNI_ERR;
static constexpr jint jRet_EDETACHED	=JNI_EDETACHED;
static constexpr jint jRet_EVERSION		=JNI_EVERSION;
static constexpr jint jRet_ENOMEM		=JNI_ENOMEM;
static constexpr jint jRet_EEXIST		=JNI_EEXIST;
static constexpr jint jRet_EINVAL		=JNI_EINVAL;

static constexpr jint jMode_COMMIT		=JNI_COMMIT;
static constexpr jint jMode_ABORT		=JNI_ABORT;

inline jvalue MakeJValue(jcObject *l){	jvalue v;	v.l=reinterpret_cast<jobject>(l);	return v;	}

#else
// <jni.h> is not included, provide compatible declaration, version 1.6
#define	JCPP_JNIHEADER	0


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

typedef jbMethod*	jmethodID;
typedef jbField*	jfieldID;


static constexpr jboolean jtrue		=1;
static constexpr jboolean jfalse	=0;

static constexpr jint jVERSION_1_1	=0x00010001;
static constexpr jint jVERSION_1_2	=0x00010002;
static constexpr jint jVERSION_1_4	=0x00010004;
static constexpr jint jVERSION_1_6	=0x00010006;

static constexpr jint jRet_OK			=0;		// no error
static constexpr jint jRet_ERR			=-1;	// generic error 
static constexpr jint jRet_EDETACHED	=-2;	// thread detached from the VM 
static constexpr jint jRet_EVERSION		=-3;	// JNI version error 
static constexpr jint jRet_ENOMEM		=-4;	// Out of memory 
static constexpr jint jRet_EEXIST		=-5;	// VM already created 
static constexpr jint jRet_EINVAL		=-6;	// Invalid argument 

static constexpr jint jMode_COMMIT		=1;		// copy content, do not free buffer
static constexpr jint jMode_ABORT		=2;		// free buffer w/o copying back


typedef jcObject*			jobject;
typedef jcClass*			jclass;
typedef jcString*			jstring;
typedef jbcArray*			jarray;
typedef jcArray<jcObject>*	jobjectArray;
typedef jcArray<jboolean>*	jbooleanArray;
typedef jcArray<jbyte>*		jbyteArray;
typedef jcArray<jchar>*		jcharArray;
typedef jcArray<jshort>*	jshortArray;
typedef jcArray<jint>*		jintArray;
typedef jcArray<jlong>*		jlongArray;
typedef jcArray<jfloat>*	jfloatArray;
typedef jcArray<jdouble>*	jdoubleArray;
typedef jcThrowable*		jthrowable;
typedef jcObject*			jweak;


enum jobjectRefType{
    JNIInvalidRefType = 0,
    JNILocalRefType = 1,
    JNIGlobalRefType = 2,
    JNIWeakGlobalRefType = 3
};

struct JavaVMAttachArgs
{
    jint version;
    char *name;    /* the name of the thread as a modified UTF-8 string, or NULL */
    jobject group; /* global ref of a ThreadGroup object, or NULL */
};

//---------------------------------------------------------------------------

#endif // JNI_OK

inline jvalue MakeJValue(jboolean z){	jvalue v;	v.d=0;	v.z=z;	return v;	}
inline jvalue MakeJValue(jbyte b){		jvalue v;	v.d=0;	v.b=b;	return v;	}
inline jvalue MakeJValue(jchar c){		jvalue v;	v.d=0;	v.c=c;	return v;	}
inline jvalue MakeJValue(jshort s){		jvalue v;	v.d=0;	v.s=s;	return v;	}
inline jvalue MakeJValue(jint i){		jvalue v;	v.d=0;	v.i=i;	return v;	}
inline jvalue MakeJValue(jlong j){		jvalue v;	v.d=0;	v.j=j;	return v;	}
inline jvalue MakeJValue(jfloat f){		jvalue v;	v.d=0;	v.f=f;	return v;	}
inline jvalue MakeJValue(jdouble d){	jvalue v;	v.d=d;	return v;	}
inline jvalue MakeJValue(jobject l){	jvalue v;	v.d=0;	v.l=l;	return v;	}

class jVM
{
private:
	virtual void reserved0(void)=0;
	virtual void reserved1(void)=0;
	virtual void reserved2(void)=0;
public:
	virtual jint cnLib_FUNC DestroyJavaVM(void)noexcept=0;
	virtual jint cnLib_FUNC AttachCurrentThread(void **env,void* thr_args)noexcept=0;
	virtual jint cnLib_FUNC DetachCurrentThread(void)noexcept=0;
	virtual jint cnLib_FUNC GetEnv(void **env,jint version)noexcept=0;
	virtual jint cnLib_FUNC AttachCurrentThreadAsDaemon(void **env,void *thr_args)noexcept=0;
};

#if JCPP_JNIHEADER
using ::JavaVM;
// JCPP_JNIHEADER
#else
// !JCPP_JNIHEADER
typedef jVM JavaVM;
#endif // !JCPP_JNIHEADER

template<class TJavaContext>
bool jLogExceptionT(JNIEnv *env)noexcept;	// define in JNICPP.hpp
bool jLogException(JNIEnv *env)noexcept;	// define in JNICPP.hpp

void jCPPInterfaceCallCheck(const char *Function,JNIEnv *env)noexcept;	// define in JNICPP.hpp
template<class TClass,class...TClasses>
void jCPPInterfaceCallCheck(const char *Function,JNIEnv *env,TClass *CheckObject,TClasses*...CheckObjects)noexcept;	// define in JNICPP.hpp

#ifndef JCPP_JNICHECK
#ifdef cnLib_DEBUG
#define	JCPP_JNICHECK
#endif // cnLib_DEBUG
#endif // !JCPP_JNICHECK

#ifdef JCPP_JNICHECK
#define	JCPP_INTERFACECALLCHECK(...)	jCPPInterfaceCallCheck(__func__,__VA_ARGS__)
#else
#define	JCPP_INTERFACECALLCHECK(...)
#endif // cnLib_DEBUG

class jEnv
{
private:
	virtual void reserved0(void)=0;
	virtual void reserved1(void)=0;
	virtual void reserved2(void)=0;
	virtual void reserved3(void)=0;
public:
	virtual jint cnLib_FUNC GetVersion(void)noexcept=0;

	virtual jclass cnLib_FUNC DefineClass(const char *name,jobject loader,const jbyte* buf,jsize bufLen)noexcept=0;

	virtual jclass cnLib_FUNC FindClass(const char *name)noexcept=0;

	virtual jmethodID cnLib_FUNC FromReflectedMethod(jobject method)noexcept=0;
	virtual jfieldID cnLib_FUNC FromReflectedField(jobject field)noexcept=0;

	virtual jobject cnLib_FUNC ToReflectedMethod(jclass cls,jmethodID *methodID,jboolean isStatic)noexcept=0;

	virtual jclass cnLib_FUNC GetSuperclass(jclass clazz)noexcept=0;
	virtual jboolean cnLib_FUNC IsAssignableFrom(jclass clazz1,jclass clazz2)noexcept=0;

	virtual jobject cnLib_FUNC ToReflectedField(jclass cls,jfieldID fieldID,jboolean isStatic)noexcept=0;

	virtual jint			cnLib_FUNC Throw(jthrowable obj)noexcept=0;
	virtual jint			cnLib_FUNC ThrowNew(jclass clazz,const char* message)noexcept=0;
	virtual jthrowable		cnLib_FUNC ExceptionOccurred(void)noexcept=0;
	virtual void			cnLib_FUNC ExceptionDescribe(void)noexcept=0;
	virtual void			cnLib_FUNC ExceptionClear(void)noexcept=0;
	virtual void			cnLib_FUNC FatalError(const char* msg)noexcept=0;

	virtual jint		cnLib_FUNC PushLocalFrame(jint capacity)noexcept=0;
	virtual jobject		cnLib_FUNC PopLocalFrame(jobject result)noexcept=0;

	virtual jobject		cnLib_FUNC NewGlobalRef(jobject obj)noexcept=0;
	virtual void		cnLib_FUNC DeleteGlobalRef(jobject globalRef)noexcept=0;
	virtual void		cnLib_FUNC DeleteLocalRef(jobject localRef)noexcept=0;
	virtual jboolean	cnLib_FUNC IsSameObject(jobject ref1,jobject ref2)noexcept=0;

	virtual jobject		cnLib_FUNC NewLocalRef(jobject ref)noexcept=0;
	virtual jint		cnLib_FUNC EnsureLocalCapacity(jint capacity)noexcept=0;

	virtual jobject	cnLib_FUNC AllocObject(jclass clazz)noexcept=0;
	virtual jobject	cnLib_FUNC NewObject(jclass clazz,jmethodID methodID,...)noexcept=0;
	virtual jobject	cnLib_FUNC NewObjectV(jclass clazz,jmethodID methodID,va_list args)noexcept=0;
	virtual jobject	cnLib_FUNC NewObjectA(jclass clazz,jmethodID methodID,const jvalue* args)noexcept=0;

	virtual jclass		cnLib_FUNC GetObjectClass(jobject obj)noexcept=0;
	virtual jboolean	cnLib_FUNC IsInstanceOf(jobject obj,jclass *clazz)noexcept=0;
	virtual jmethodID	cnLib_FUNC GetMethodID(jclass clazz,const char* name,const char* sig)noexcept=0;


	virtual jobject		cnLib_FUNC CallObjectMethod(jobject obj,jmethodID methodID,...)noexcept=0;
	virtual jobject		cnLib_FUNC CallObjectMethodV(jobject obj,jmethodID methodID,va_list)noexcept=0;
	virtual jobject		cnLib_FUNC CallObjectMethodA(jobject obj,jmethodID methodID,const jvalue* args)noexcept=0;
	virtual jboolean	cnLib_FUNC CallBooleanMethod(jobject obj,jmethodID methodID,...)noexcept=0;
	virtual jboolean	cnLib_FUNC CallBooleanMethodV(jobject obj,jmethodID methodID,va_list)noexcept=0;
	virtual jboolean	cnLib_FUNC CallBooleanMethodA(jobject obj,jmethodID methodID,const jvalue* args)noexcept=0;
	virtual jbyte		cnLib_FUNC CallByteMethod(jobject obj,jmethodID methodID,...)noexcept=0;
	virtual jbyte		cnLib_FUNC CallByteMethodV(jobject obj,jmethodID methodID,va_list)noexcept=0;
	virtual jbyte		cnLib_FUNC CallByteMethodA(jobject obj,jmethodID methodID,const jvalue* args)noexcept=0;
	virtual jchar		cnLib_FUNC CallCharMethod(jobject obj,jmethodID methodID,...)noexcept=0;
	virtual jchar		cnLib_FUNC CallCharMethodV(jobject obj,jmethodID methodID,va_list)noexcept=0;
	virtual jchar		cnLib_FUNC CallCharMethodA(jobject obj,jmethodID methodID,const jvalue* args)noexcept=0;
	virtual jshort		cnLib_FUNC CallShortMethod(jobject obj,jmethodID methodID,...)noexcept=0;
	virtual jshort		cnLib_FUNC CallShortMethodV(jobject obj,jmethodID methodID,va_list)noexcept=0;
	virtual jshort		cnLib_FUNC CallShortMethodA(jobject obj,jmethodID methodID,const jvalue* args)noexcept=0;
	virtual jint		cnLib_FUNC CallIntMethod(jobject obj,jmethodID methodID,...)noexcept=0;
	virtual jint		cnLib_FUNC CallIntMethodV(jobject obj,jmethodID methodID,va_list)noexcept=0;
	virtual jint		cnLib_FUNC CallIntMethodA(jobject obj,jmethodID methodID,const jvalue* args)noexcept=0;
	virtual jlong		cnLib_FUNC CallLongMethod(jobject obj,jmethodID methodID,...)noexcept=0;
	virtual jlong		cnLib_FUNC CallLongMethodV(jobject obj,jmethodID methodID,va_list)noexcept=0;
	virtual jlong		cnLib_FUNC CallLongMethodA(jobject obj,jmethodID methodID,const jvalue* args)noexcept=0;
	virtual jfloat		cnLib_FUNC CallFloatMethod(jobject obj,jmethodID methodID,...)noexcept=0;
	virtual jfloat		cnLib_FUNC CallFloatMethodV(jobject obj,jmethodID methodID,va_list)noexcept=0;
	virtual jfloat		cnLib_FUNC CallFloatMethodA(jobject obj,jmethodID methodID,const jvalue* args)noexcept=0;
	virtual jdouble		cnLib_FUNC CallDoubleMethod(jobject obj,jmethodID methodID,...)noexcept=0;
	virtual jdouble		cnLib_FUNC CallDoubleMethodV(jobject obj,jmethodID methodID,va_list)noexcept=0;
	virtual jdouble		cnLib_FUNC CallDoubleMethodA(jobject obj,jmethodID methodID,const jvalue* args)noexcept=0;
	virtual void		cnLib_FUNC CallVoidMethod(jobject obj,jmethodID methodID,...)noexcept=0;
	virtual void		cnLib_FUNC CallVoidMethodV(jobject obj,jmethodID methodID,va_list)noexcept=0;
	virtual void		cnLib_FUNC CallVoidMethodA(jobject obj,jmethodID methodID,const jvalue* args)noexcept=0;

	virtual jobject		cnLib_FUNC CallNonvirtualObjectMethod(jobject obj,jclass clazz,jmethodID methodID,...)noexcept=0;
	virtual jobject		cnLib_FUNC CallNonvirtualObjectMethodV(jobject obj,jclass clazz,jmethodID methodID,va_list)noexcept=0;
	virtual jobject		cnLib_FUNC CallNonvirtualObjectMethodA(jobject obj,jclass clazz,jmethodID methodID,const jvalue *args)noexcept=0;
	virtual jboolean	cnLib_FUNC CallNonvirtualBooleanMethod(jobject obj,jclass clazz,jmethodID methodID,...)noexcept=0;
	virtual jboolean	cnLib_FUNC CallNonvirtualBooleanMethodV(jobject obj,jclass clazz,jmethodID methodID,va_list)noexcept=0;
	virtual jboolean	cnLib_FUNC CallNonvirtualBooleanMethodA(jobject obj,jclass clazz,jmethodID methodID,const jvalue *args)noexcept=0;
	virtual jbyte		cnLib_FUNC CallNonvirtualByteMethod(jobject obj,jclass clazz,jmethodID methodID,...)noexcept=0;
	virtual jbyte		cnLib_FUNC CallNonvirtualByteMethodV(jobject obj,jclass clazz,jmethodID methodID,va_list)noexcept=0;
	virtual jbyte		cnLib_FUNC CallNonvirtualByteMethodA(jobject obj,jclass clazz,jmethodID methodID,const jvalue *args)noexcept=0;
	virtual jchar		cnLib_FUNC CallNonvirtualCharMethod(jobject obj,jclass clazz,jmethodID methodID,...)noexcept=0;
	virtual jchar		cnLib_FUNC CallNonvirtualCharMethodV(jobject obj,jclass clazz,jmethodID methodID,va_list)noexcept=0;
	virtual jchar		cnLib_FUNC CallNonvirtualCharMethodA(jobject obj,jclass clazz,jmethodID methodID,const jvalue *args)noexcept=0;
	virtual jshort		cnLib_FUNC CallNonvirtualShortMethod(jobject obj,jclass clazz,jmethodID methodID,...)noexcept=0;
	virtual jshort		cnLib_FUNC CallNonvirtualShortMethodV(jobject obj,jclass clazz,jmethodID methodID,va_list)noexcept=0;
	virtual jshort		cnLib_FUNC CallNonvirtualShortMethodA(jobject obj,jclass clazz,jmethodID methodID,const jvalue *args)noexcept=0;
	virtual jint		cnLib_FUNC CallNonvirtualIntMethod(jobject obj,jclass clazz,jmethodID methodID,...)noexcept=0;
	virtual jint		cnLib_FUNC CallNonvirtualIntMethodV(jobject obj,jclass clazz,jmethodID methodID,va_list)noexcept=0;
	virtual jint		cnLib_FUNC CallNonvirtualIntMethodA(jobject obj,jclass clazz,jmethodID methodID,const jvalue *args)noexcept=0;
	virtual jlong		cnLib_FUNC CallNonvirtualLongMethod(jobject obj,jclass clazz,jmethodID methodID,...)noexcept=0;
	virtual jlong		cnLib_FUNC CallNonvirtualLongMethodV(jobject obj,jclass clazz,jmethodID methodID,va_list)noexcept=0;
	virtual jlong		cnLib_FUNC CallNonvirtualLongMethodA(jobject obj,jclass clazz,jmethodID methodID,const jvalue *args)noexcept=0;
	virtual jfloat		cnLib_FUNC CallNonvirtualFloatMethod(jobject obj,jclass clazz,jmethodID methodID,...)noexcept=0;
	virtual jfloat		cnLib_FUNC CallNonvirtualFloatMethodV(jobject obj,jclass clazz,jmethodID methodID,va_list)noexcept=0;
	virtual jfloat		cnLib_FUNC CallNonvirtualFloatMethodA(jobject obj,jclass clazz,jmethodID methodID,const jvalue *args)noexcept=0;
	virtual jdouble		cnLib_FUNC CallNonvirtualDoubleMethod(jobject obj,jclass clazz,jmethodID methodID,...)noexcept=0;
	virtual jdouble		cnLib_FUNC CallNonvirtualDoubleMethodV(jobject obj,jclass clazz,jmethodID methodID,va_list)noexcept=0;
	virtual jdouble		cnLib_FUNC CallNonvirtualDoubleMethodA(jobject obj,jclass clazz,jmethodID methodID,const jvalue *args)noexcept=0;
	virtual void		cnLib_FUNC CallNonvirtualVoidMethod(jobject obj,jclass clazz,jmethodID methodID,...)noexcept=0;
	virtual void		cnLib_FUNC CallNonvirtualVoidMethodV(jobject obj,jclass clazz,jmethodID methodID,va_list)noexcept=0;
	virtual void		cnLib_FUNC CallNonvirtualVoidMethodA(jobject obj,jclass clazz,jmethodID methodID,const jvalue *args)noexcept=0;

	virtual jfieldID	cnLib_FUNC GetFieldID(jclass clazz,const char* name,const char* sig)noexcept=0;

	virtual jobject		cnLib_FUNC GetObjectField(jobject obj,jfieldID fieldID)noexcept=0;
	virtual jboolean	cnLib_FUNC GetBooleanField(jobject obj,jfieldID fieldID)noexcept=0;
	virtual jbyte		cnLib_FUNC GetByteField(jobject obj,jfieldID fieldID)noexcept=0;
	virtual jchar		cnLib_FUNC GetCharField(jobject obj,jfieldID fieldID)noexcept=0;
	virtual jshort		cnLib_FUNC GetShortField(jobject obj,jfieldID fieldID)noexcept=0;
	virtual jint		cnLib_FUNC GetIntField(jobject obj,jfieldID fieldID)noexcept=0;
	virtual jlong		cnLib_FUNC GetLongField(jobject obj,jfieldID fieldID)noexcept=0;
	virtual jfloat		cnLib_FUNC GetFloatField(jobject obj,jfieldID fieldID)noexcept=0;
	virtual jdouble		cnLib_FUNC GetDoubleField(jobject obj,jfieldID fieldID)noexcept=0;

	virtual void		cnLib_FUNC SetObjectField(jobject obj,jfieldID fieldID,jobject value)noexcept=0;
	virtual void		cnLib_FUNC SetBooleanField(jobject obj,jfieldID fieldID,jboolean value)noexcept=0;
	virtual void		cnLib_FUNC SetByteField(jobject obj,jfieldID fieldID,jbyte value)noexcept=0;
	virtual void		cnLib_FUNC SetCharField(jobject obj,jfieldID fieldID,jchar value)noexcept=0;
	virtual void		cnLib_FUNC SetShortField(jobject obj,jfieldID fieldID,jshort value)noexcept=0;
	virtual void		cnLib_FUNC SetIntField(jobject obj,jfieldID fieldID,jint value)noexcept=0;
	virtual void		cnLib_FUNC SetLongField(jobject obj,jfieldID fieldID,jlong value)noexcept=0;
	virtual void		cnLib_FUNC SetFloatField(jobject obj,jfieldID fieldID,jfloat value)noexcept=0;
	virtual void		cnLib_FUNC SetDoubleField(jobject obj,jfieldID fieldID,jdouble value)noexcept=0;

	virtual jmethodID	cnLib_FUNC GetStaticMethodID(jclass clazz,const char* name,const char* sig)noexcept=0;


	virtual jobject		cnLib_FUNC CallStaticObjectMethod(jclass clazz,jmethodID methodID,...)noexcept=0;
	virtual jobject		cnLib_FUNC CallStaticObjectMethodV(jclass clazz,jmethodID methodID,va_list)noexcept=0;
	virtual jobject		cnLib_FUNC CallStaticObjectMethodA(jclass clazz,jmethodID methodID,const jvalue *args)noexcept=0;
	virtual jboolean	cnLib_FUNC CallStaticBooleanMethod(jclass clazz,jmethodID methodID,...)noexcept=0;
	virtual jboolean	cnLib_FUNC CallStaticBooleanMethodV(jclass clazz,jmethodID methodID,va_list)noexcept=0;
	virtual jboolean	cnLib_FUNC CallStaticBooleanMethodA(jclass clazz,jmethodID methodID,const jvalue *args)noexcept=0;
	virtual jbyte		cnLib_FUNC CallStaticByteMethod(jclass clazz,jmethodID methodID,...)noexcept=0;
	virtual jbyte		cnLib_FUNC CallStaticByteMethodV(jclass clazz,jmethodID methodID,va_list)noexcept=0;
	virtual jbyte		cnLib_FUNC CallStaticByteMethodA(jclass clazz,jmethodID methodID,const jvalue *args)noexcept=0;
	virtual jchar		cnLib_FUNC CallStaticCharMethod(jclass clazz,jmethodID methodID,...)noexcept=0;
	virtual jchar		cnLib_FUNC CallStaticCharMethodV(jclass clazz,jmethodID methodID,va_list)noexcept=0;
	virtual jchar		cnLib_FUNC CallStaticCharMethodA(jclass clazz,jmethodID methodID,const jvalue *args)noexcept=0;
	virtual jshort		cnLib_FUNC CallStaticShortMethod(jclass clazz,jmethodID methodID,...)noexcept=0;
	virtual jshort		cnLib_FUNC CallStaticShortMethodV(jclass clazz,jmethodID methodID,va_list)noexcept=0;
	virtual jshort		cnLib_FUNC CallStaticShortMethodA(jclass clazz,jmethodID methodID,const jvalue *args)noexcept=0;
	virtual jint		cnLib_FUNC CallStaticIntMethod(jclass clazz,jmethodID methodID,...)noexcept=0;
	virtual jint		cnLib_FUNC CallStaticIntMethodV(jclass clazz,jmethodID methodID,va_list)noexcept=0;
	virtual jint		cnLib_FUNC CallStaticIntMethodA(jclass clazz,jmethodID methodID,const jvalue *args)noexcept=0;
	virtual jlong		cnLib_FUNC CallStaticLongMethod(jclass clazz,jmethodID methodID,...)noexcept=0;
	virtual jlong		cnLib_FUNC CallStaticLongMethodV(jclass clazz,jmethodID methodID,va_list)noexcept=0;
	virtual jlong		cnLib_FUNC CallStaticLongMethodA(jclass clazz,jmethodID methodID,const jvalue *args)noexcept=0;
	virtual jfloat		cnLib_FUNC CallStaticFloatMethod(jclass clazz,jmethodID methodID,...)noexcept=0;
	virtual jfloat		cnLib_FUNC CallStaticFloatMethodV(jclass clazz,jmethodID methodID,va_list)noexcept=0;
	virtual jfloat		cnLib_FUNC CallStaticFloatMethodA(jclass clazz,jmethodID methodID,const jvalue *args)noexcept=0;
	virtual jdouble		cnLib_FUNC CallStaticDoubleMethod(jclass clazz,jmethodID methodID,...)noexcept=0;
	virtual jdouble		cnLib_FUNC CallStaticDoubleMethodV(jclass clazz,jmethodID methodID,va_list)noexcept=0;
	virtual jdouble		cnLib_FUNC CallStaticDoubleMethodA(jclass clazz,jmethodID methodID,const jvalue *args)noexcept=0;
	virtual void		cnLib_FUNC CallStaticVoidMethod(jclass clazz,jmethodID methodID,...)noexcept=0;
	virtual void		cnLib_FUNC CallStaticVoidMethodV(jclass clazz,jmethodID methodID,va_list)noexcept=0;
	virtual void		cnLib_FUNC CallStaticVoidMethodA(jclass clazz,jmethodID methodID,const jvalue *args)noexcept=0;

	virtual jfieldID	cnLib_FUNC GetStaticFieldID(jclass clazz,const char* name,const char* sig)noexcept=0;

	virtual jobject		cnLib_FUNC GetStaticObjectField(jclass cls,jfieldID fieldID)noexcept=0;
	virtual jboolean	cnLib_FUNC GetStaticBooleanField(jclass clsobj,jfieldID fieldID)noexcept=0;
	virtual jbyte		cnLib_FUNC GetStaticByteField(jclass cls,jfieldID fieldID)noexcept=0;
	virtual jchar		cnLib_FUNC GetStaticCharField(jclass cls,jfieldID fieldID)noexcept=0;
	virtual jshort		cnLib_FUNC GetStaticShortField(jclass cls,jfieldID fieldID)noexcept=0;
	virtual jint		cnLib_FUNC GetStaticIntField(jclass cls,jfieldID fieldID)noexcept=0;
	virtual jlong		cnLib_FUNC GetStaticLongField(jclass cls,jfieldID fieldID)noexcept=0;
	virtual jfloat		cnLib_FUNC GetStaticFloatField(jclass cls,jfieldID fieldID)noexcept=0;
	virtual jdouble		cnLib_FUNC GetStaticDoubleField(jclass cls,jfieldID fieldID)noexcept=0;

	virtual void		cnLib_FUNC SetStaticObjectField(jclass cls,jfieldID fieldID,jobject value)noexcept=0;
	virtual void		cnLib_FUNC SetStaticBooleanField(jclass cls,jfieldID fieldID,jboolean value)noexcept=0;
	virtual void		cnLib_FUNC SetStaticByteField(jclass cls,jfieldID fieldID,jbyte value)noexcept=0;
	virtual void		cnLib_FUNC SetStaticCharField(jclass cls,jfieldID fieldID,jchar value)noexcept=0;
	virtual void		cnLib_FUNC SetStaticShortField(jclass cls,jfieldID fieldID,jshort value)noexcept=0;
	virtual void		cnLib_FUNC SetStaticIntField(jclass cls,jfieldID fieldID,jint value)noexcept=0;
	virtual void		cnLib_FUNC SetStaticLongField(jclass cls,jfieldID fieldID,jlong value)noexcept=0;
	virtual void		cnLib_FUNC SetStaticFloatField(jclass cls,jfieldID fieldID,jfloat value)noexcept=0;
	virtual void		cnLib_FUNC SetStaticDoubleField(jclass cls,jfieldID fieldID,jdouble value)noexcept=0;

	virtual jstring		cnLib_FUNC NewString(const jchar* unicodeChars,jsize len)noexcept=0;
	virtual jsize		cnLib_FUNC GetStringLength(jstring string)noexcept=0;
	virtual const jchar*cnLib_FUNC GetStringChars(jstring string,jboolean* isCopy)noexcept=0;
	virtual void		cnLib_FUNC ReleaseStringChars(jstring string,const jchar* chars)noexcept=0;
	virtual jstring		cnLib_FUNC NewStringUTF(const char* bytes)noexcept=0;
	virtual jsize		cnLib_FUNC GetStringUTFLength(jstring string)noexcept=0;

	virtual const char*		cnLib_FUNC GetStringUTFChars(jstring string,jboolean* isCopy)noexcept=0;
	virtual void			cnLib_FUNC ReleaseStringUTFChars(jstring string,const char* utf)noexcept=0;
	virtual jsize			cnLib_FUNC GetArrayLength(jarray array)noexcept=0;
	virtual jobjectArray	cnLib_FUNC NewObjectArray(jsize length,jclass elementClass,jobject initialElement)noexcept=0;
	virtual jobject			cnLib_FUNC GetObjectArrayElement(jobjectArray array,jsize index)noexcept=0;
	virtual void			cnLib_FUNC SetObjectArrayElement(jobjectArray array,jsize index,jobject value)noexcept=0;

	virtual jbooleanArray	cnLib_FUNC NewBooleanArray(jsize length)noexcept=0;
	virtual jbyteArray		cnLib_FUNC NewByteArray(jsize length)noexcept=0;
	virtual jcharArray		cnLib_FUNC NewCharArray(jsize length)noexcept=0;
	virtual jshortArray		cnLib_FUNC NewShortArray(jsize length)noexcept=0;
	virtual jintArray		cnLib_FUNC NewIntArray(jsize length)noexcept=0;
	virtual jlongArray		cnLib_FUNC NewLongArray(jsize length)noexcept=0;
	virtual jfloatArray		cnLib_FUNC NewFloatArray(jsize length)noexcept=0;
	virtual jdoubleArray	cnLib_FUNC NewDoubleArray(jsize length)noexcept=0;

	virtual jboolean*		cnLib_FUNC GetBooleanArrayElements(jbooleanArray array,jboolean* isCopy)noexcept=0;
	virtual jbyte*			cnLib_FUNC GetByteArrayElements(jbyteArray array,jboolean* isCopy)noexcept=0;
	virtual jchar*			cnLib_FUNC GetCharArrayElements(jcharArray array,jboolean* isCopy)noexcept=0;
	virtual jshort*			cnLib_FUNC GetShortArrayElements(jshortArray array,jboolean* isCopy)noexcept=0;
	virtual jint*			cnLib_FUNC GetIntArrayElements(jintArray array,jboolean* isCopy)noexcept=0;
	virtual jlong*			cnLib_FUNC GetLongArrayElements(jlongArray array,jboolean* isCopy)noexcept=0;
	virtual jfloat*			cnLib_FUNC GetFloatArrayElements(jfloatArray array,jboolean* isCopy)noexcept=0;
	virtual jdouble*		cnLib_FUNC GetDoubleArrayElements(jdoubleArray array,jboolean* isCopy)noexcept=0;

	virtual void		cnLib_FUNC ReleaseBooleanArrayElements(jbooleanArray array,jboolean* elems,jint mode)noexcept=0;
	virtual void		cnLib_FUNC ReleaseByteArrayElements(jbyteArray array,jbyte *elems,jint mode)noexcept=0;
	virtual void		cnLib_FUNC ReleaseCharArrayElements(jcharArray array,jchar *elems,jint mode)noexcept=0;
	virtual void		cnLib_FUNC ReleaseShortArrayElements(jshortArray array,jshort *elems,jint mode)noexcept=0;
	virtual void		cnLib_FUNC ReleaseIntArrayElements(jintArray array,jint *elems,jint mode)noexcept=0;
	virtual void		cnLib_FUNC ReleaseLongArrayElements(jlongArray array,jlong *elems,jint mode)noexcept=0;
	virtual void		cnLib_FUNC ReleaseFloatArrayElements(jfloatArray array,jfloat *elems,jint mode)noexcept=0;
	virtual void		cnLib_FUNC ReleaseDoubleArrayElements(jdoubleArray array,jdouble *elems,jint mode)noexcept=0;

	virtual void		cnLib_FUNC GetBooleanArrayRegion(jbooleanArray array,jsize start,jsize len,jboolean *buf)noexcept=0;
	virtual void		cnLib_FUNC GetByteArrayRegion(jbyteArray array,jsize start,jsize len,jbyte *buf)noexcept=0;
	virtual void		cnLib_FUNC GetCharArrayRegion(jcharArray array,jsize start,jsize len,jchar *buf)noexcept=0;
	virtual void		cnLib_FUNC GetShortArrayRegion(jshortArray array,jsize start,jsize len,jshort *buf)noexcept=0;
	virtual void		cnLib_FUNC GetIntArrayRegion(jintArray array,jsize start,jsize len,jint *buf)noexcept=0;
	virtual void		cnLib_FUNC GetLongArrayRegion(jlongArray array,jsize start,jsize len,jlong *buf)noexcept=0;
	virtual void		cnLib_FUNC GetFloatArrayRegion(jfloatArray array,jsize start,jsize len,jfloat *buf)noexcept=0;
	virtual void		cnLib_FUNC GetDoubleArrayRegion(jdoubleArray array,jsize start,jsize len,jdouble *buf)noexcept=0;

	virtual void		cnLib_FUNC SetBooleanArrayRegion(jbooleanArray array,jsize start,jsize len,const jboolean *buf)noexcept=0;
	virtual void		cnLib_FUNC SetByteArrayRegion(jbyteArray array,jsize start,jsize len,const jbyte *buf)noexcept=0;
	virtual void		cnLib_FUNC SetCharArrayRegion(jcharArray array,jsize start,jsize len,const jchar *buf)noexcept=0;
	virtual void		cnLib_FUNC SetShortArrayRegion(jshortArray array,jsize start,jsize len,const jshort *buf)noexcept=0;
	virtual void		cnLib_FUNC SetIntArrayRegion(jintArray array,jsize start,jsize len,const jint *buf)noexcept=0;
	virtual void		cnLib_FUNC SetLongArrayRegion(jlongArray array,jsize start,jsize len,const jlong *buf)noexcept=0;
	virtual void		cnLib_FUNC SetFloatArrayRegion(jfloatArray array,jsize start,jsize len,const jfloat *buf)noexcept=0;
	virtual void		cnLib_FUNC SetDoubleArrayRegion(jdoubleArray array,jsize start,jsize len,const jdouble *buf)noexcept=0;

	virtual jint		cnLib_FUNC RegisterNatives(jclass clazz,const JNINativeMethod* methods,jint nMethods)noexcept=0;
	virtual jint		cnLib_FUNC UnregisterNatives(jclass clazz)noexcept=0;
	virtual jint		cnLib_FUNC MonitorEnter(jobject obj)noexcept=0;
	virtual jint		cnLib_FUNC MonitorExit(jobject obj)noexcept=0;
	virtual jint		cnLib_FUNC GetJavaVM(JavaVM* *vm)noexcept=0;

	virtual void		cnLib_FUNC GetStringRegion(jstring str,jsize start,jsize len,jchar* buf)noexcept=0;
	virtual void		cnLib_FUNC GetStringUTFRegion(jstring str,jsize start,jsize len,char* buf)noexcept=0;

	virtual void*		cnLib_FUNC GetPrimitiveArrayCritical(jarray array,jboolean* isCopy)noexcept=0;
	virtual void		cnLib_FUNC ReleasePrimitiveArrayCritical(jarray array,void* carray,jint mode)noexcept=0;

	virtual const jchar*cnLib_FUNC GetStringCritical(jstring string,jboolean* isCopy)noexcept=0;
	virtual void		cnLib_FUNC ReleaseStringCritical(jstring string,const jchar* carray)noexcept=0;

	virtual jobject		cnLib_FUNC NewWeakGlobalRef(jobject obj)noexcept=0;
	virtual void		cnLib_FUNC DeleteWeakGlobalRef(jobject obj)noexcept=0;

	virtual jboolean	cnLib_FUNC ExceptionCheck(void)noexcept=0;

	virtual jobject		cnLib_FUNC NewDirectByteBuffer(void* address,jlong capacity)noexcept=0;
	virtual void*		cnLib_FUNC GetDirectBufferAddress(jobject buf)noexcept=0;
	virtual jlong		cnLib_FUNC GetDirectBufferCapacity(jobject buf)noexcept=0;

	/* added in JNI 1.6 */
	virtual jobjectRefType		cnLib_FUNC GetObjectRefType(jobject obj)noexcept=0;

};

#if JCPP_JNIHEADER
using ::JNIEnv;
// JCPP_JNIHEADER
#else
// !JCPP_JNIHEADER
typedef jEnv JNIEnv;
#endif // !JCPP_JNIHEADER

//---------------------------------------------------------------------------


namespace jInterface
{

	inline jint DestroyJavaVM(JavaVM *vm)noexcept
	{	return vm->DestroyJavaVM(); }
	inline jint GetEnv(JavaVM *vm,JNIEnv* &env,jint version)noexcept
	{	return vm->GetEnv(reinterpret_cast<void**>(&env), version); }

	inline jint AttachCurrentThread(JavaVM *vm,JNIEnv* &env,void* thr_args)noexcept
	// AttachCurrentThread may have declaration of jint (JavaVM *vm,JNIEnv **penv,void* thr_args) in jni.h header, use jVM version to avoid error
	{	return reinterpret_cast<jVM*>(vm)->AttachCurrentThread(reinterpret_cast<void**>(&env), thr_args); }
	inline jint DetachCurrentThread(JavaVM *vm)noexcept
	{	return vm->DetachCurrentThread(); }
	inline jint AttachCurrentThreadAsDaemon(JavaVM *vm,JNIEnv* &env,void *thr_args)noexcept
	// AttachCurrentThreadAsDaemon may have declaration of jint (JavaVM *vm,JNIEnv **penv,void* thr_args) in jni.h header, use jVM version to avoid error
	{	return reinterpret_cast<jVM*>(vm)->AttachCurrentThreadAsDaemon(reinterpret_cast<void**>(&env), thr_args); }

	inline jcClass* FindClass(JNIEnv *env,const char *name)noexcept
	{	JCPP_INTERFACECALLCHECK(env);	return reinterpret_cast<jcClass*>(env->FindClass(name)); }


	template<class T>
	inline T*			NewGlobalRef(JNIEnv *env,T *obj)noexcept
	{	JCPP_INTERFACECALLCHECK(env,obj);	return reinterpret_cast<T*>(env->NewGlobalRef(reinterpret_cast<jobject>(obj)));	}
	template<class T>
	inline void			DeleteGlobalRef(JNIEnv *env,T *globalRef)noexcept
	{	JCPP_INTERFACECALLCHECK(env,globalRef);	return env->DeleteGlobalRef(reinterpret_cast<jobject>(globalRef)); }
	inline void			DeleteLocalRefNoCheck(JNIEnv *env,jcObject *localRef)noexcept
	{	return env->DeleteLocalRef(reinterpret_cast<jobject>(localRef)); }
	template<class T>
	inline void			DeleteLocalRef(JNIEnv *env,T *localRef)noexcept
	{	JCPP_INTERFACECALLCHECK(env,localRef);	return env->DeleteLocalRef(reinterpret_cast<jobject>(localRef)); }

	inline jbMethod*	GetMethodID(JNIEnv *env,jcClass *clazz,const char* name,const char* sig)noexcept
	{	JCPP_INTERFACECALLCHECK(env,clazz,name,sig);	return reinterpret_cast<jbMethod*>(env->GetMethodID(reinterpret_cast<jclass>(clazz),name,sig)); }
	inline jbField*	GetFieldID(JNIEnv *env,jcClass *clazz,const char* name,const char* sig)noexcept
	{	JCPP_INTERFACECALLCHECK(env,clazz,name,sig);	return reinterpret_cast<jbField*>(env->GetFieldID(reinterpret_cast<jclass>(clazz),name,sig)); }
	inline jbMethod*	GetStaticMethodID(JNIEnv *env,jcClass *clazz,const char* name,const char* sig)noexcept
	{	JCPP_INTERFACECALLCHECK(env,clazz,name,sig);	return reinterpret_cast<jbMethod*>(env->GetStaticMethodID(reinterpret_cast<jclass>(clazz),name,sig)); }
	inline jbField*	GetStaticFieldID(JNIEnv *env,jcClass *clazz,const char* name,const char* sig)noexcept
	{	JCPP_INTERFACECALLCHECK(env,clazz,name,sig);	return reinterpret_cast<jbField*>(env->GetStaticFieldID(reinterpret_cast<jclass>(clazz),name,sig)); }

	inline jcString*	NewString(JNIEnv *env,const jchar* unicodeChars,jsize len)noexcept
	{	JCPP_INTERFACECALLCHECK(env);	return reinterpret_cast<jcString*>(env->NewString(unicodeChars,len)); }
	inline jsize		GetStringLength(JNIEnv *env,jcString *string)noexcept
	{	JCPP_INTERFACECALLCHECK(env,string);	return env->GetStringLength(reinterpret_cast<jstring>(string)); }
	inline const jchar*GetStringChars(JNIEnv *env,jcString *string,jboolean* isCopy)noexcept
	{	JCPP_INTERFACECALLCHECK(env,string);	return env->GetStringChars(reinterpret_cast<jstring>(string),isCopy); }
	inline void			ReleaseStringChars(JNIEnv *env,jcString *string,const jchar* chars)noexcept
	{	JCPP_INTERFACECALLCHECK(env,string);	return env->ReleaseStringChars(reinterpret_cast<jstring>(string),chars); }
	//inline jcString*	NewStringUTF(JNIEnv *env,const char* bytes)noexcept
	//{	JCPP_INTERFACECALLCHECK(env);	return reinterpret_cast<jcString*>(env->NewStringUTF(bytes)); }
	//inline jsize		GetStringUTFLength(JNIEnv *env,jcString *string)noexcept
	//{	JCPP_INTERFACECALLCHECK(env);	return env->GetStringUTFLength(reinterpret_cast<jstring>(string)); }

	inline jsize	GetArrayLength(JNIEnv *env,jbcArray *array)noexcept
	{	JCPP_INTERFACECALLCHECK(env,array);	return env->GetArrayLength(reinterpret_cast<jarray>(array)); }

	template<class TObjectElement>
	inline jcArray<TObjectElement*>*	NewObjectArray(JNIEnv *env,jsize length,jcClass *elementClass,TObjectElement *initialElement)noexcept
	{	JCPP_INTERFACECALLCHECK(env,elementClass,initialElement);	return reinterpret_cast<jcArray<TObjectElement*>*>(env->NewObjectArray(length,reinterpret_cast<jclass>(elementClass),reinterpret_cast<jobject>(initialElement))); }

	template<class TObjectElement>
	inline TObjectElement*		GetObjectArrayElement(JNIEnv *env,jcArray<TObjectElement*> *array,jsize index)noexcept
	{	JCPP_INTERFACECALLCHECK(env,array);	return reinterpret_cast<TObjectElement*>(env->GetObjectArrayElement(reinterpret_cast<jobjectArray>(array),index)); }
	template<class TObjectElement>
	inline void					SetObjectArrayElement(JNIEnv *env,jcArray<TObjectElement*> *array,jsize index,TObjectElement *value)noexcept
	{	JCPP_INTERFACECALLCHECK(env,array,value);	return env->SetObjectArrayElement(reinterpret_cast<jobjectArray>(array),index,reinterpret_cast<jobject>(value)); }

	inline jint	RegisterNatives(JNIEnv *env,jcClass *clazz,const JNINativeMethod* methods,jint nMethods)noexcept
	{	JCPP_INTERFACECALLCHECK(env,clazz);	return env->RegisterNatives(reinterpret_cast<jclass>(clazz),methods,nMethods); }
	inline jint	UnregisterNatives(JNIEnv *env,jcClass *clazz)noexcept
	{	JCPP_INTERFACECALLCHECK(env,clazz);	return env->UnregisterNatives(reinterpret_cast<jclass>(clazz)); }

	inline jint	GetJavaVM(JNIEnv *env,JavaVM* &vm)noexcept
	{	JCPP_INTERFACECALLCHECK(env);	return env->GetJavaVM(reinterpret_cast<JavaVM**>(&vm)); }

	inline const jchar* GetStringCritical(JNIEnv *env,jcString *string,jboolean* isCopy)noexcept
	{	JCPP_INTERFACECALLCHECK(env,string);	return env->GetStringCritical(reinterpret_cast<jstring>(string),isCopy); }
	inline void ReleaseStringCritical(JNIEnv *env,jcString *string,const jchar* carray)noexcept
	{	return env->ReleaseStringCritical(reinterpret_cast<jstring>(string),carray); }

	inline jint Throw(JNIEnv *env,jcThrowable *Throwable)noexcept
	{	return env->Throw(reinterpret_cast<jthrowable>(Throwable));	}	
	inline bool ExceptionCheck(JNIEnv *env)noexcept
	{	return env->ExceptionCheck();	}
	inline void ExceptionClear(JNIEnv *env)noexcept
	{	return env->ExceptionClear();	}
	inline void ExceptionDescribe(JNIEnv *env)noexcept
	{	return env->ExceptionDescribe();	}
	inline jcThrowable* ExceptionOccurred(JNIEnv *env)noexcept
	{	return reinterpret_cast<jcThrowable*>(env->ExceptionOccurred());	}


	template<class T>
	struct tTypeOp;

	template<class T>
	static jcArray<T>*	NewArray(JNIEnv *env,jsize length)noexcept
	{	JCPP_INTERFACECALLCHECK(env);	return tTypeOp<T>::NewArray(env,length);	}

	template<class TField>
	inline TField GetField(JNIEnv *env,jcObject *Object,jbField *FieldID)noexcept
	{	JCPP_INTERFACECALLCHECK(env,Object,FieldID);	return tTypeOp<TField>::GetField(env,Object,FieldID);	}
	template<class TField>
	inline void SetField(JNIEnv *env,jcObject *Object,jbField *FieldID,TField Value)noexcept
	{	JCPP_INTERFACECALLCHECK(env,Object,FieldID);	return tTypeOp<TField>::SetField(env,Object,FieldID,Value);	}

	template<class TField>
	inline TField GetStaticField(JNIEnv *env,jcClass *Class,jbField *FieldID)noexcept
	{	JCPP_INTERFACECALLCHECK(env,Class,FieldID);	return tTypeOp<TField>::GetStaticField(env,Class,FieldID);	}
	template<class TField>
	inline void SetStaticField(JNIEnv *env,jcClass *Class,jbField *FieldID,TField Value)noexcept
	{	JCPP_INTERFACECALLCHECK(env,Class,FieldID);	return tTypeOp<TField>::SetStaticField(env,Class,FieldID,Value);	}

	template<class TRet>
	inline TRet CallMethodA(JNIEnv *env,jcObject *Object,jbMethod *MethodID,const jvalue *args)noexcept
	{	JCPP_INTERFACECALLCHECK(env,Object,MethodID,args);	return tTypeOp<TRet>::CallMethodA(env,Object,MethodID,args);	}
	template<class TRet>
	inline TRet CallStaticMethodA(JNIEnv *env,jcClass *Class,jbMethod *MethodID,const jvalue *args)noexcept
	{	JCPP_INTERFACECALLCHECK(env,Class,MethodID,args);	return tTypeOp<TRet>::CallStaticMethodA(env,Class,MethodID,args);	}

	template<class TClass>
	inline TClass* NewObjectA(JNIEnv *env,jcClass *cls,jbMethod *MethodID,const jvalue *args)noexcept
	{	JCPP_INTERFACECALLCHECK(env,cls,MethodID,args);	return reinterpret_cast<TClass*>(env->NewObjectA(reinterpret_cast<jclass>(cls),reinterpret_cast<jmethodID>(MethodID),args));	}

	template<class T>
	inline T*	GetArrayElements(JNIEnv *env,jcArray<T> *array,jboolean* isCopy)noexcept
	{	JCPP_INTERFACECALLCHECK(env,array);	return tTypeOp<T>::GetArrayElements(env,array,isCopy); }

	template<class T>
	inline void ReleaseArrayElements(JNIEnv *env,jcArray<T> *array,T *elems,jint mode)noexcept
	{	JCPP_INTERFACECALLCHECK(env,array);	return tTypeOp<T>::ReleaseArrayElements(env,array,elems,mode); }

	template<class T>
	inline void	GetArrayRegion(JNIEnv *env,jcArray<T> *array,jsize start,jsize len,T *buf)noexcept
	{	JCPP_INTERFACECALLCHECK(env,array);	return tTypeOp<T>::GetArrayRegion(env,array,start,len,buf); }

	template<class T>
	inline void	SetArrayRegion(JNIEnv *env,jcArray<T> *array,jsize start,jsize len,const T *buf)noexcept
	{	JCPP_INTERFACECALLCHECK(env,array);	return tTypeOp<T>::SetArrayRegion(env,array,start,len,buf); }

	template<class T>
	struct tTypeOp_ArrayElementsCritical;

	template<class T>
	inline T*	GetArrayElementsCritical(JNIEnv *env,jcArray<T> *array,jboolean* isCopy)noexcept
	{	JCPP_INTERFACECALLCHECK(env,array);	return tTypeOp<T>::GetArrayElementsCritical(env,array,isCopy); }

	template<class T>
	inline void ReleaseArrayElementsCritical(JNIEnv *env,jcArray<T> *array,T *elems,jint mode)noexcept
	{	return tTypeOp<T>::ReleaseArrayElementsCritical(env,array,elems,mode); }

};
//---------------------------------------------------------------------------
template<class T>
struct jInterface::tTypeOp_ArrayElementsCritical
{
	static T*	GetArrayElementsCritical(JNIEnv *env,jcArray<T> *array,jboolean* isCopy)noexcept
	{	return static_cast<T*>(env->GetPrimitiveArrayCritical(reinterpret_cast<jarray>(array),isCopy)); }

	static void ReleaseArrayElementsCritical(JNIEnv *env,jcArray<T> *array,T *elems,jint mode)noexcept
	{	return env->ReleasePrimitiveArrayCritical(reinterpret_cast<jarray>(array),elems,mode); }
};
//---------------------------------------------------------------------------
template<>
struct jInterface::tTypeOp<void>
{
	static void CallMethodA(JNIEnv *env,jcObject *Object,jbMethod *MethodID,const jvalue *args)noexcept
	{	return env->CallVoidMethodA(reinterpret_cast<jobject>(Object),reinterpret_cast<jmethodID>(MethodID),args);	}

	static void CallStaticMethodA(JNIEnv *env,jcClass *Class,jbMethod *MethodID,const jvalue *args)noexcept
	{	return env->CallStaticVoidMethodA(reinterpret_cast<jclass>(Class),reinterpret_cast<jmethodID>(MethodID),args);	}
};
//---------------------------------------------------------------------------
template<class TClass>
struct jInterface::tTypeOp<TClass*>
{
	static TClass* GetField(JNIEnv *env,jcObject *Object,jbField *FieldID)noexcept
	{	return reinterpret_cast<TClass*>(env->GetObjectField(reinterpret_cast<jobject>(Object),reinterpret_cast<jfieldID>(FieldID)));	}

	static void SetField(JNIEnv *env,jcObject *Object,jbField *FieldID,TClass *Value)noexcept
	{	return env->SetObjectField(reinterpret_cast<jobject>(Object),reinterpret_cast<jfieldID>(FieldID),reinterpret_cast<jobject>(Value));	}

	static TClass* GetStaticField(JNIEnv *env,jcClass *Class,jbField *FieldID)noexcept
	{	return reinterpret_cast<TClass*>(env->GetStaticObjectField(reinterpret_cast<jclass>(Class),reinterpret_cast<jfieldID>(FieldID)));	}

	static void SetStaticField(JNIEnv *env,jcClass *Class,jbField *FieldID,TClass* Value)noexcept
	{	return env->SetStaticObjectField(reinterpret_cast<jclass>(Class),reinterpret_cast<jfieldID>(FieldID),reinterpret_cast<jobject>(Value));	}

	static TClass* CallMethodA(JNIEnv *env,jcObject *Object,jbMethod *MethodID,const jvalue *args)noexcept
	{	return reinterpret_cast<TClass*>(env->CallObjectMethodA(reinterpret_cast<jobject>(Object),reinterpret_cast<jmethodID>(MethodID),args));	}

	static TClass* CallStaticMethodA(JNIEnv *env,jcClass *Class,jbMethod *MethodID,const jvalue *args)noexcept
	{	return reinterpret_cast<TClass*>(env->CallStaticObjectMethodA(reinterpret_cast<jclass>(Class),reinterpret_cast<jmethodID>(MethodID),args));	}
};
//---------------------------------------------------------------------------
template<>
struct jInterface::tTypeOp<jboolean>
	: tTypeOp_ArrayElementsCritical<jboolean>
{
	static jcArray<jboolean>*	NewArray(JNIEnv *env,jsize length)noexcept
	{	return reinterpret_cast<jcArray<jboolean>*>(env->NewBooleanArray(length)); }

	static jboolean GetField(JNIEnv *env,jcObject *Object,jbField *FieldID)noexcept
	{	return env->GetBooleanField(reinterpret_cast<jobject>(Object),reinterpret_cast<jfieldID>(FieldID));	}

	static void SetField(JNIEnv *env,jcObject *Object,jbField *FieldID,jboolean Value)noexcept
	{	return env->SetBooleanField(reinterpret_cast<jobject>(Object),reinterpret_cast<jfieldID>(FieldID),Value);	}

	static jboolean GetStaticField(JNIEnv *env,jcClass *Class,jbField *FieldID)noexcept
	{	return env->GetStaticBooleanField(reinterpret_cast<jclass>(Class),reinterpret_cast<jfieldID>(FieldID));	}

	static void SetStaticField(JNIEnv *env,jcClass *Class,jbField *FieldID,jboolean Value)noexcept
	{	return env->SetStaticBooleanField(reinterpret_cast<jclass>(Class),reinterpret_cast<jfieldID>(FieldID),Value);	}

	static jboolean CallMethodA(JNIEnv *env,jcObject *Object,jbMethod *MethodID,const jvalue *args)noexcept
	{	return env->CallBooleanMethodA(reinterpret_cast<jobject>(Object),reinterpret_cast<jmethodID>(MethodID),args);	}

	static jboolean CallStaticMethodA(JNIEnv *env,jcClass *Class,jbMethod *MethodID,const jvalue *args)noexcept
	{	return env->CallStaticBooleanMethodA(reinterpret_cast<jclass>(Class),reinterpret_cast<jmethodID>(MethodID),args);	}

	static jboolean*	GetArrayElements(JNIEnv *env,jcArray<jboolean> *array,jboolean* isCopy)noexcept
	{	return env->GetBooleanArrayElements(reinterpret_cast<jbooleanArray>(array),isCopy); }

	static void ReleaseArrayElements(JNIEnv *env,jcArray<jboolean> *array,jboolean* elems,jint mode)noexcept
	{	return env->ReleaseBooleanArrayElements(reinterpret_cast<jbooleanArray>(array),elems,mode); }

	static void	GetArrayRegion(JNIEnv *env,jcArray<jboolean> *array,jsize start,jsize len,jboolean *buf)noexcept
	{	return env->GetBooleanArrayRegion(reinterpret_cast<jbooleanArray>(array),start,len,buf); }

	static void	SetArrayRegion(JNIEnv *env,jcArray<jboolean> *array,jsize start,jsize len,const jboolean *buf)noexcept
	{	return env->SetBooleanArrayRegion(reinterpret_cast<jbooleanArray>(array),start,len,buf); }
};
//---------------------------------------------------------------------------
template<>
struct jInterface::tTypeOp<jbyte>
	: tTypeOp_ArrayElementsCritical<jbyte>
{
	static jcArray<jbyte>*		NewArray(JNIEnv *env,jsize length)noexcept
	{	return reinterpret_cast<jcArray<jbyte>*>(env->NewByteArray(length)); }

	static jbyte GetField(JNIEnv *env,jcObject *Object,jbField *FieldID)noexcept
	{	return env->GetByteField(reinterpret_cast<jobject>(Object),reinterpret_cast<jfieldID>(FieldID));	}

	static void SetField(JNIEnv *env,jcObject *Object,jbField *FieldID,jbyte Value)noexcept
	{	return env->SetByteField(reinterpret_cast<jobject>(Object),reinterpret_cast<jfieldID>(FieldID),Value);	}

	static jbyte GetStaticField(JNIEnv *env,jcClass *Class,jbField *FieldID)noexcept
	{	return env->GetStaticByteField(reinterpret_cast<jclass>(Class),reinterpret_cast<jfieldID>(FieldID));	}

	static void SetStaticField(JNIEnv *env,jcClass *Class,jbField *FieldID,jbyte Value)noexcept
	{	return env->SetStaticByteField(reinterpret_cast<jclass>(Class),reinterpret_cast<jfieldID>(FieldID),Value);	}

	static jbyte CallMethodA(JNIEnv *env,jcObject *Object,jbMethod *MethodID,const jvalue *args)noexcept
	{	return env->CallByteMethodA(reinterpret_cast<jobject>(Object),reinterpret_cast<jmethodID>(MethodID),args);	}

	static jbyte CallStaticMethodA(JNIEnv *env,jcClass *Class,jbMethod *MethodID,const jvalue *args)noexcept
	{	return env->CallStaticByteMethodA(reinterpret_cast<jclass>(Class),reinterpret_cast<jmethodID>(MethodID),args);	}

	static jbyte*		GetArrayElements(JNIEnv *env,jcArray<jbyte> *array,jboolean* isCopy)noexcept
	{	return env->GetByteArrayElements(reinterpret_cast<jbyteArray>(array),isCopy); }

	static void ReleaseArrayElements(JNIEnv *env,jcArray<jbyte> *array,jbyte *elems,jint mode)noexcept
	{	return env->ReleaseByteArrayElements(reinterpret_cast<jbyteArray>(array),elems,mode); }

	static void	GetArrayRegion(JNIEnv *env,jcArray<jbyte> *array,jsize start,jsize len,jbyte *buf)noexcept
	{	return env->GetByteArrayRegion(reinterpret_cast<jbyteArray>(array),start,len,buf); }

	static void	SetArrayRegion(JNIEnv *env,jcArray<jbyte> *array,jsize start,jsize len,const jbyte *buf)noexcept
	{	return env->SetByteArrayRegion(reinterpret_cast<jbyteArray>(array),start,len,buf); }
};
//---------------------------------------------------------------------------
template<>
struct jInterface::tTypeOp<jchar>
	: tTypeOp_ArrayElementsCritical<jchar>
{
	static jcArray<jchar>*		NewArray(JNIEnv *env,jsize length)noexcept
	{	return reinterpret_cast<jcArray<jchar>*>(env->NewCharArray(length)); }

	static jchar GetField(JNIEnv *env,jcObject *Object,jbField *FieldID)noexcept
	{	return env->GetCharField(reinterpret_cast<jobject>(Object),reinterpret_cast<jfieldID>(FieldID));	}

	static void SetField(JNIEnv *env,jcObject *Object,jbField *FieldID,jchar Value)noexcept
	{	return env->SetCharField(reinterpret_cast<jobject>(Object),reinterpret_cast<jfieldID>(FieldID),Value);	}

	static jchar GetStaticField(JNIEnv *env,jcClass *Class,jbField *FieldID)noexcept
	{	return env->GetStaticCharField(reinterpret_cast<jclass>(Class),reinterpret_cast<jfieldID>(FieldID));	}

	static void SetStaticField(JNIEnv *env,jcClass *Class,jbField *FieldID,jchar Value)noexcept
	{	return env->SetStaticCharField(reinterpret_cast<jclass>(Class),reinterpret_cast<jfieldID>(FieldID),Value);	}

	static jchar CallMethodA(JNIEnv *env,jcObject *Object,jbMethod *MethodID,const jvalue *args)noexcept
	{	return env->CallCharMethodA(reinterpret_cast<jobject>(Object),reinterpret_cast<jmethodID>(MethodID),args);	}

	static jchar CallStaticMethodA(JNIEnv *env,jcClass *Class,jbMethod *MethodID,const jvalue *args)noexcept
	{	return env->CallStaticCharMethodA(reinterpret_cast<jclass>(Class),reinterpret_cast<jmethodID>(MethodID),args);	}

	static jchar*		GetArrayElements(JNIEnv *env,jcArray<jchar> *array,jboolean* isCopy)noexcept
	{	return env->GetCharArrayElements(reinterpret_cast<jcharArray>(array),isCopy); }

	static void ReleaseArrayElements(JNIEnv *env,jcArray<jchar> *array,jchar *elems,jint mode)noexcept
	{	return env->ReleaseCharArrayElements(reinterpret_cast<jcharArray>(array),elems,mode); }

	static void	GetArrayRegion(JNIEnv *env,jcArray<jchar> *array,jsize start,jsize len,jchar *buf)noexcept
	{	return env->GetCharArrayRegion(reinterpret_cast<jcharArray>(array),start,len,buf); }

	static void	SetArrayRegion(JNIEnv *env,jcArray<jchar> *array,jsize start,jsize len,const jchar *buf)noexcept
	{	return env->SetCharArrayRegion(reinterpret_cast<jcharArray>(array),start,len,buf); }
};
//---------------------------------------------------------------------------
template<>
struct jInterface::tTypeOp<jshort>
	: tTypeOp_ArrayElementsCritical<jshort>
{
	static jcArray<jshort>*		NewArray(JNIEnv *env,jsize length)noexcept
	{	return reinterpret_cast<jcArray<jshort>*>(env->NewShortArray(length)); }

	static jshort GetField(JNIEnv *env,jcObject *Object,jbField *FieldID)noexcept
	{	return env->GetShortField(reinterpret_cast<jobject>(Object),reinterpret_cast<jfieldID>(FieldID));	}

	static void SetField(JNIEnv *env,jcObject *Object,jbField *FieldID,jshort Value)noexcept
	{	return env->SetShortField(reinterpret_cast<jobject>(Object),reinterpret_cast<jfieldID>(FieldID),Value);}

	static jshort GetStaticField(JNIEnv *env,jcClass *Class,jbField *FieldID)noexcept
	{	return env->GetStaticShortField(reinterpret_cast<jclass>(Class),reinterpret_cast<jfieldID>(FieldID));	}

	static void SetStaticField(JNIEnv *env,jcClass *Class,jbField *FieldID,jshort Value)noexcept
	{	return env->SetStaticShortField(reinterpret_cast<jclass>(Class),reinterpret_cast<jfieldID>(FieldID),Value);	}

	static jshort CallMethodA(JNIEnv *env,jcObject *Object,jbMethod *MethodID,const jvalue *args)noexcept
	{	return env->CallShortMethodA(reinterpret_cast<jobject>(Object),reinterpret_cast<jmethodID>(MethodID),args);	}

	static jint CallStaticMethodA(JNIEnv *env,jcClass *Class,jbMethod *MethodID,const jvalue *args)noexcept
	{	return env->CallStaticIntMethodA(reinterpret_cast<jclass>(Class),reinterpret_cast<jmethodID>(MethodID),args);	}

	static jshort*		GetArrayElements(JNIEnv *env,jcArray<jshort> *array,jboolean* isCopy)noexcept
	{	return env->GetShortArrayElements(reinterpret_cast<jshortArray>(array),isCopy); }

	static void ReleaseArrayElements(JNIEnv *env,jcArray<jshort> *array,jshort *elems,jint mode)noexcept
	{	return env->ReleaseShortArrayElements(reinterpret_cast<jshortArray>(array),elems,mode); }

	static void	GetArrayRegion(JNIEnv *env,jcArray<jshort> *array,jsize start,jsize len,jshort *buf)noexcept
	{	return env->GetShortArrayRegion(reinterpret_cast<jshortArray>(array),start,len,buf); }

	static void	SetArrayRegion(JNIEnv *env,jcArray<jshort> *array,jsize start,jsize len,const jshort *buf)noexcept
	{	return env->SetShortArrayRegion(reinterpret_cast<jshortArray>(array),start,len,buf); }
};
//---------------------------------------------------------------------------
template<>
struct jInterface::tTypeOp<jint>
	: tTypeOp_ArrayElementsCritical<jint>
{
	static jcArray<jint>*		NewArray(JNIEnv *env,jsize length)noexcept
	{	return reinterpret_cast<jcArray<jint>*>(env->NewIntArray(length)); }

	static jint GetField(JNIEnv *env,jcObject *Object,jbField *FieldID)noexcept
	{	return env->GetIntField(reinterpret_cast<jobject>(Object),reinterpret_cast<jfieldID>(FieldID));	}

	static void SetField(JNIEnv *env,jcObject *Object,jbField *FieldID,jint Value)noexcept
	{	return env->SetIntField(reinterpret_cast<jobject>(Object),reinterpret_cast<jfieldID>(FieldID),Value);	}

	static jint GetStaticField(JNIEnv *env,jcClass *Class,jbField *FieldID)noexcept
	{	return env->GetStaticIntField(reinterpret_cast<jclass>(Class),reinterpret_cast<jfieldID>(FieldID));	}

	static void SetStaticField(JNIEnv *env,jcClass *Class,jbField *FieldID,jint Value)noexcept
	{	return env->SetStaticIntField(reinterpret_cast<jclass>(Class),reinterpret_cast<jfieldID>(FieldID),Value);	}

	static jint CallMethodA(JNIEnv *env,jcObject *Object,jbMethod *MethodID,const jvalue *args)noexcept
	{	return env->CallIntMethodA(reinterpret_cast<jobject>(Object),reinterpret_cast<jmethodID>(MethodID),args);	}

	static jshort CallStaticMethodA(JNIEnv *env,jcClass *Class,jbMethod *MethodID,const jvalue *args)noexcept
	{	return env->CallStaticShortMethodA(reinterpret_cast<jclass>(Class),reinterpret_cast<jmethodID>(MethodID),args);	}

	static jint*		GetArrayElements(JNIEnv *env,jcArray<jint> *array,jboolean* isCopy)noexcept
	{	return env->GetIntArrayElements(reinterpret_cast<jintArray>(array),isCopy); }

	static void ReleaseArrayElements(JNIEnv *env,jcArray<jint> *array,jint *elems,jint mode)noexcept
	{	return env->ReleaseIntArrayElements(reinterpret_cast<jintArray>(array),elems,mode); }

	static void	GetArrayRegion(JNIEnv *env,jcArray<jint> *array,jsize start,jsize len,jint *buf)noexcept
	{	return env->GetIntArrayRegion(reinterpret_cast<jintArray>(array),start,len,buf); }

	static void	SetArrayRegion(JNIEnv *env,jcArray<jint> *array,jsize start,jsize len,const jint *buf)noexcept
	{	return env->SetIntArrayRegion(reinterpret_cast<jintArray>(array),start,len,buf); }
};
//---------------------------------------------------------------------------
template<>
struct jInterface::tTypeOp<jlong>
	: tTypeOp_ArrayElementsCritical<jlong>
{
	static jcArray<jlong>*		NewArray(JNIEnv *env,jsize length)noexcept
	{	return reinterpret_cast<jcArray<jlong>*>(env->NewLongArray(length)); }

	static jlong GetField(JNIEnv *env,jcObject *Object,jbField *FieldID)noexcept
	{	return env->GetLongField(reinterpret_cast<jobject>(Object),reinterpret_cast<jfieldID>(FieldID));	}

	static void SetField(JNIEnv *env,jcObject *Object,jbField *FieldID,jlong Value)noexcept
	{	return env->SetLongField(reinterpret_cast<jobject>(Object),reinterpret_cast<jfieldID>(FieldID),Value);	}

	static jlong GetStaticField(JNIEnv *env,jcClass *Class,jbField *FieldID)noexcept
	{	return env->GetStaticLongField(reinterpret_cast<jclass>(Class),reinterpret_cast<jfieldID>(FieldID));	}

	static void SetStaticField(JNIEnv *env,jcClass *Class,jbField *FieldID,jlong Value)noexcept
	{	return env->SetStaticLongField(reinterpret_cast<jclass>(Class),reinterpret_cast<jfieldID>(FieldID),Value);	}

	static jlong CallMethodA(JNIEnv *env,jcObject *Object,jbMethod *MethodID,const jvalue *args)noexcept
	{	return env->CallLongMethodA(reinterpret_cast<jobject>(Object),reinterpret_cast<jmethodID>(MethodID),args);	}

	static jlong CallStaticMethodA(JNIEnv *env,jcClass *Class,jbMethod *MethodID,const jvalue *args)noexcept
	{	return env->CallStaticLongMethodA(reinterpret_cast<jclass>(Class),reinterpret_cast<jmethodID>(MethodID),args);	}

	static jlong*		GetArrayElements(JNIEnv *env,jcArray<jlong> *array,jboolean* isCopy)noexcept
	{	return env->GetLongArrayElements(reinterpret_cast<jlongArray>(array),isCopy); }

	static void ReleaseArrayElements(JNIEnv *env,jcArray<jlong> *array,jlong *elems,jint mode)noexcept
	{	return env->ReleaseLongArrayElements(reinterpret_cast<jlongArray>(array),elems,mode); }

	static void	GetArrayRegion(JNIEnv *env,jcArray<jlong> *array,jsize start,jsize len,jlong *buf)noexcept
	{	return env->GetLongArrayRegion(reinterpret_cast<jlongArray>(array),start,len,buf); }

	static void	SetArrayRegion(JNIEnv *env,jcArray<jlong> *array,jsize start,jsize len,const jlong *buf)noexcept
	{	return env->SetLongArrayRegion(reinterpret_cast<jlongArray>(array),start,len,buf); }
};
//---------------------------------------------------------------------------
template<>
struct jInterface::tTypeOp<jfloat>
	: tTypeOp_ArrayElementsCritical<jfloat>
{
	static jcArray<jfloat>*		NewArray(JNIEnv *env,jsize length)noexcept
	{	return reinterpret_cast<jcArray<jfloat>*>(env->NewFloatArray(length)); }

	static jfloat GetField(JNIEnv *env,jcObject *Object,jbField *FieldID)noexcept
	{	return env->GetFloatField(reinterpret_cast<jobject>(Object),reinterpret_cast<jfieldID>(FieldID));	}

	static void SetField(JNIEnv *env,jcObject *Object,jbField *FieldID,jfloat Value)noexcept
	{	return env->SetFloatField(reinterpret_cast<jobject>(Object),reinterpret_cast<jfieldID>(FieldID),Value);	}

	static jfloat GetStaticField(JNIEnv *env,jcClass *Class,jbField *FieldID)noexcept
	{	return env->GetStaticFloatField(reinterpret_cast<jclass>(Class),reinterpret_cast<jfieldID>(FieldID));	}

	static void SetStaticField(JNIEnv *env,jcClass *Class,jbField *FieldID,jfloat Value)noexcept
	{	return env->SetStaticFloatField(reinterpret_cast<jclass>(Class),reinterpret_cast<jfieldID>(FieldID),Value);	}

	static jfloat CallMethodA(JNIEnv *env,jcObject *Object,jbMethod *MethodID,const jvalue *args)noexcept
	{	return env->CallFloatMethodA(reinterpret_cast<jobject>(Object),reinterpret_cast<jmethodID>(MethodID),args);	}

	static jfloat CallStaticMethodA(JNIEnv *env,jcClass *Class,jbMethod *MethodID,const jvalue *args)noexcept
	{	return env->CallStaticFloatMethodA(reinterpret_cast<jclass>(Class),reinterpret_cast<jmethodID>(MethodID),args);	}

	static jfloat*		GetArrayElements(JNIEnv *env,jcArray<jfloat> *array,jboolean* isCopy)noexcept
	{	return env->GetFloatArrayElements(reinterpret_cast<jfloatArray>(array),isCopy); }

	static void ReleaseArrayElements(JNIEnv *env,jcArray<jfloat> *array,jfloat *elems,jint mode)noexcept
	{	return env->ReleaseFloatArrayElements(reinterpret_cast<jfloatArray>(array),elems,mode); }

	static void	GetArrayRegion(JNIEnv *env,jcArray<jfloat> *array,jsize start,jsize len,jfloat *buf)noexcept
	{	return env->GetFloatArrayRegion(reinterpret_cast<jfloatArray>(array),start,len,buf); }

	static void	SetArrayRegion(JNIEnv *env,jcArray<jfloat> *array,jsize start,jsize len,const jfloat *buf)noexcept
	{	return env->SetFloatArrayRegion(reinterpret_cast<jfloatArray>(array),start,len,buf); }
};
//---------------------------------------------------------------------------
template<>
struct jInterface::tTypeOp<jdouble>
	: tTypeOp_ArrayElementsCritical<jdouble>
{
	static jcArray<jdouble>*	NewArray(JNIEnv *env,jsize length)noexcept
	{	return reinterpret_cast<jcArray<jdouble>*>(env->NewDoubleArray(length)); }

	static jdouble GetField(JNIEnv *env,jcObject *Object,jbField *FieldID)noexcept
	{	return env->GetDoubleField(reinterpret_cast<jobject>(Object),reinterpret_cast<jfieldID>(FieldID));	}

	static void SetField(JNIEnv *env,jcObject *Object,jbField *FieldID,jdouble Value)noexcept
	{	return env->SetDoubleField(reinterpret_cast<jobject>(Object),reinterpret_cast<jfieldID>(FieldID),Value);	}

	static jdouble GetStaticField(JNIEnv *env,jcClass *Class,jbField *FieldID)noexcept
	{	return env->GetStaticDoubleField(reinterpret_cast<jclass>(Class),reinterpret_cast<jfieldID>(FieldID));	}

	static void SetStaticField(JNIEnv *env,jcClass *Class,jbField *FieldID,jdouble Value)noexcept
	{	return env->SetStaticDoubleField(reinterpret_cast<jclass>(Class),reinterpret_cast<jfieldID>(FieldID),Value);	}

	static jdouble CallMethodA(JNIEnv *env,jcObject *Object,jbMethod *MethodID,const jvalue *args)noexcept
	{	return env->CallDoubleMethodA(reinterpret_cast<jobject>(Object),reinterpret_cast<jmethodID>(MethodID),args);	}

	static jdouble CallStaticMethodA(JNIEnv *env,jcClass *Class,jbMethod *MethodID,const jvalue *args)noexcept
	{	return env->CallStaticDoubleMethodA(reinterpret_cast<jclass>(Class),reinterpret_cast<jmethodID>(MethodID),args);	}

	static jdouble*	GetArrayElements(JNIEnv *env,jcArray<jdouble> *array,jboolean* isCopy)noexcept
	{	return env->GetDoubleArrayElements(reinterpret_cast<jdoubleArray>(array),isCopy); }

	static void ReleaseArrayElements(JNIEnv *env,jcArray<jdouble> *array,jdouble *elems,jint mode)noexcept
	{	return env->ReleaseDoubleArrayElements(reinterpret_cast<jdoubleArray>(array),elems,mode); }

	static void	GetArrayRegion(JNIEnv *env,jcArray<jdouble> *array,jsize start,jsize len,jdouble *buf)noexcept
	{	return env->GetDoubleArrayRegion(reinterpret_cast<jdoubleArray>(array),start,len,buf); }

	static void	SetArrayRegion(JNIEnv *env,jcArray<jdouble> *array,jsize start,jsize len,const jdouble *buf)noexcept
	{	return env->SetDoubleArrayRegion(reinterpret_cast<jdoubleArray>(array),start,len,buf); }
};
//---------------------------------------------------------------------------
}	// namespace jCPP
//---------------------------------------------------------------------------
}	//namespace cnLibrary
//---------------------------------------------------------------------------
namespace cnLib_THelper{
namespace JNI{
	using namespace jCPP;
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

//---------------------------------------------------------------------------
}	// namespace JNI
}	// namespace cnLib_THelper
//---------------------------------------------------------------------------
namespace cnLibrary{
//---------------------------------------------------------------------------
namespace jCPP{
//---------------------------------------------------------------------------
template<class T>
struct JNITypeSignatureWriter	: cnLib_THelper::JNI::JNITypeSignatureWriterT<typename cnLib_THelper::JNI::TSelectSignatureType<void,T>::Type>{};
//---------------------------------------------------------------------------
}	// namespace jCPP
//---------------------------------------------------------------------------
}	// namespace cnLibrary
//---------------------------------------------------------------------------
namespace cnLib_THelper{
namespace JNI{

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
//---------------------------------------------------------------------------
}	// namespace JNI
}	// namespace cnLib_THelper
//---------------------------------------------------------------------------
namespace cnLibrary{
//---------------------------------------------------------------------------
namespace jCPP{
//---------------------------------------------------------------------------

template<class TRet,class...TArgs>
struct JNIMethodSignatureGenerator
{
	char Value[cnLib_THelper::JNI::JNITypeSequenceSignatureWriter<TArgs...>::Length+2+JNITypeSignatureWriter<TRet>::Length+1];
	JNIMethodSignatureGenerator()noexcept{
		Value[0]='(';
		char *p=Value+1;
		p=cnLib_THelper::JNI::JNITypeSequenceSignatureWriter<TArgs...>::Write(p);
		*p++=')';
		p=JNITypeSignatureWriter<TRet>::Write(p);
		*p=0;
	}
};


template<class TJavaClass>
class jrLocal;

//---------------------------------------------------------------------------
}	// namespace jCPP
//---------------------------------------------------------------------------
}	// namespace cnLibrary
//---------------------------------------------------------------------------

#endif