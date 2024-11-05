/*- cnAndroid - JNI - CPP fundamental -------------------------------------*/
/*         Developer : Code009                                             */
/*         Create on : 2024-04-01                                          */
/*-------------------------------------------------------------------------*/
#pragma once
/*-------------------------------------------------------------------------*/
#include <Java/JNIRegistration.h>
#include <cnRTL/TextProcess.h>
#include <cnRTL/Logger.h>

#ifdef __cplusplus

//---------------------------------------------------------------------------
namespace cnLibrary{
//---------------------------------------------------------------------------
namespace jCPP{
//---------------------------------------------------------------------------
template<class TJavaClass>
class jrLocal
{
public:
	jrLocal(JNIEnv *env)noexcept
		: fEnv(env)
		, fJavaRef(nullptr)
	{}

	jrLocal(JNIEnv *env,TJavaClass *Object)noexcept
		: fEnv(env)
		, fJavaRef(Object)
	{}

	~jrLocal()noexcept{
		if(fJavaRef!=nullptr){
			jInterface::DeleteLocalRef(fEnv,fJavaRef);
			jLogException(fEnv);
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
			jInterface::DeleteLocalRef(fEnv,fJavaRef);
			jLogException(fEnv);
		}
		fJavaRef=Src.fJavaRef;
		Src.fJavaRef=nullptr;
		return *this;
	}

	TJavaClass* Get(void)const noexcept{ return fJavaRef; }
	void Set(TJavaClass *Ref)noexcept{
		if(fJavaRef!=nullptr){
			jInterface::DeleteLocalRef(fEnv,fJavaRef);
			jLogException(fEnv);
		}
		fJavaRef=Ref;
	}
	void Clear(void)noexcept{
		if(fJavaRef!=nullptr){
			jInterface::DeleteLocalRef(fEnv,fJavaRef);
			jLogException(fEnv);
			fJavaRef=nullptr;
		}
	}

	template<class TDest>
	typename cnVar::TTypeConditional<jrLocal<TDest>&&,cnVar::TIsConvertible<TJavaClass*,TDest*>::Value>
		::Type Transfer(void)noexcept
	{
		return reinterpret_cast<jrLocal<TDest>&&>(*this);
	}


#ifdef JNI_OK
	jobject obj()const noexcept{ return reinterpret_cast<jobject>(fJavaRef); }
#endif // JNI_OK

	operator TJavaClass*()const noexcept{	return fJavaRef; }
	TJavaClass* operator ->()const noexcept{ return fJavaRef; }

	JNIEnv* env()const noexcept{ return fEnv; }

	TJavaClass* Return(void)noexcept{ auto Ret=fJavaRef;	fJavaRef=nullptr;	return Ret; }
protected:
	JNIEnv *fEnv;
	TJavaClass *fJavaRef;
};
template<class T>
inline jrLocal<T> RefLocal(JNIEnv *env,T *Object)noexcept
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
template<class TJavaClass>
class jrGlobal
{
public:
	jrGlobal()noexcept : fJavaRef(nullptr){}
	~jrGlobal()noexcept{
		if(fJavaRef!=nullptr){
			JNIEnv *env=jQueryEnv();
			jInterface::DeleteGlobalRef(env,fJavaRef);
			jLogException(env);
		}
	}


	jrGlobal(JNIEnv *env,TJavaClass *Ref)noexcept{
		if(Ref!=nullptr){
			fJavaRef=jInterface::NewGlobalRef(env,Ref);
			if(fJavaRef==nullptr)
				jLogException(env);
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
			JNIEnv *env=jQueryEnv();
			jInterface::DeleteGlobalRef(env,fJavaRef);
			jLogException(env);
		}
		fJavaRef=Src.fJavaRef;
		Src.fJavaRef=nullptr;
		return *this;
	}

	template<class TSrc>
	jrGlobal(const jrLocal<TSrc> &Src)noexcept{
		static_assert(cnVar::TIsConvertible<TSrc*,TJavaClass*>::Value,"incompatible source reference");
		if(Src!=nullptr){
			JNIEnv *env=jQueryEnv();
			if(env==Src.env()){
				fJavaRef=jInterface::NewGlobalRef<TSrc>(env,Src);
				if(fJavaRef==nullptr)
					jLogException(env);
			}
		}
		else{
			fJavaRef=nullptr;
		}
	}
	template<class TSrc>
	jrGlobal& operator=(const jrLocal<TSrc> &Src){
		static_assert(cnVar::TIsConvertible<TSrc*,TJavaClass*>::Value,"incompatible source reference");
		if(Src!=nullptr){
			JNIEnv *env=jQueryEnv();
			if(fJavaRef!=nullptr){
				jInterface::DeleteGlobalRef(env,fJavaRef);
				jLogException(env);
			}
			if(env==Src.env()){
				fJavaRef=jInterface::NewGlobalRef<TSrc>(env,Src);
				if(fJavaRef==nullptr)
					jLogException(env);
			}
			else{
				fJavaRef=nullptr;
			}
		}
		else{
			if(fJavaRef!=nullptr){
				JNIEnv *env=jQueryEnv();
				jInterface::DeleteGlobalRef(env,fJavaRef);
				jLogException(env);
				fJavaRef=nullptr;
			}
		}
		return *this;
	}


	TJavaClass* Get(void)const noexcept{ return fJavaRef; }
	void Set(JNIEnv *env,TJavaClass* Ref)noexcept{
		if(fJavaRef!=nullptr){
			jInterface::DeleteGlobalRef(env,fJavaRef);
			jLogException(env);
		}
		if(Ref!=nullptr){
			fJavaRef=jInterface::NewGlobalRef(env,Ref);
			if(fJavaRef==nullptr)
				jLogException(env);
		}
		else{
			fJavaRef=nullptr;
		}
	}

	void Clear(JNIEnv *env)noexcept{
		if(fJavaRef!=nullptr){
			jInterface::DeleteGlobalRef(env,fJavaRef);
			jLogException(env);
			fJavaRef=nullptr;
		}
	}

	jobject obj()const noexcept{ return reinterpret_cast<jobject>(fJavaRef); }

	operator TJavaClass*()const noexcept{ return fJavaRef; }
	TJavaClass* operator ->()const noexcept{ return fJavaRef; }

protected:
	TJavaClass *fJavaRef;
};
//---------------------------------------------------------------------------
template<class TClass>
inline jrLocal<TClass> jNew(JNIEnv *env)noexcept
{
	auto cls=jRegistration<jJavaContext>::Class<TClass>(env);
	auto mid=jRegistration<jJavaContext>::ConstructorMethod<TClass>(env);
	TClass *Ret=jInterface::NewObjectA<TClass>(env,cls,mid,nullptr);
	if(Ret==nullptr){
		jLogException(env);
	}
	return RefLocal(env,Ret);
}
//---------------------------------------------------------------------------
template<class TClass,class...TArgs>
inline jrLocal<TClass> jNew(JNIEnv *env,TArgs...Args)noexcept
{
	auto cls=jRegistration<jJavaContext>::Class<TClass>(env);
	auto mid=jRegistration<jJavaContext>::ConstructorMethod<TClass,TArgs...>(env);
	jvalue args[sizeof...(TArgs)]={MakeJValue(Args)...};
	TClass *Ret=jInterface::NewObjectA<TClass>(env,cls,mid,args);
	if(Ret==nullptr){
		jLogException(env);
	}
	return RefLocal(env,Ret);
}
//---------------------------------------------------------------------------
template<class TField>
struct jFieldAccess
{
	jFieldAccess(JNIEnv *env,jcObject *JavaRef,jField<TField> *FieldID)noexcept
		: JavaEnv(env)
		, JavaRef(JavaRef)
		, FieldID(FieldID)
	{}

	TField Get(void)noexcept{
		TField Ret=jInterface::GetField<TField>(JavaEnv,JavaRef,FieldID);
		jLogException(JavaEnv);
		return Ret;
	}

	void Set(TField Value)noexcept{
		jInterface::SetField<TField>(JavaEnv,JavaRef,FieldID,Value);
		jLogException(JavaEnv);
	}

	JNIEnv *JavaEnv;
	jcObject *JavaRef;
	jField<TField> *FieldID;
};
//---------------------------------------------------------------------------
template<class TFieldClass>
struct jFieldAccess<TFieldClass*>
{
	jFieldAccess(JNIEnv *env,jcObject *JavaRef,jField<TFieldClass*> *FieldID)noexcept
		: JavaEnv(env)
		, JavaRef(JavaRef)
		, FieldID(FieldID)
	{}

	jrLocal<TFieldClass> Get(void)noexcept{
		TFieldClass* Ret=jInterface::GetField<TFieldClass*>(JavaEnv,JavaRef,FieldID);
		if(Ret==nullptr)
			jLogException(JavaEnv);
		return jrLocal<TFieldClass>(JavaEnv,Ret);
	}

	void Set(TFieldClass *Value)noexcept{
		jInterface::SetField<TFieldClass*>(JavaEnv,JavaRef,FieldID,Value);
		jLogException(JavaEnv);
	}

	JNIEnv *JavaEnv;
	jcObject *JavaRef;
	jField<TFieldClass*> *FieldID;
};
//---------------------------------------------------------------------------
template<class T>
struct jPointerFieldAccess
{
	jPointerFieldAccess(JNIEnv *env,jcObject *JavaRef,jField<jlong> *FieldID)noexcept
		: JavaEnv(env)
		, JavaRef(JavaRef)
		, FieldID(FieldID)
	{}

	T* Get(void)noexcept{
		jlong Ret=jInterface::GetField<jlong>(JavaEnv,JavaRef,FieldID);
		jLogException(JavaEnv);
		return reinterpret_cast<T*>(static_cast<uIntn>(Ret));
	}

	void Set(T *Value)noexcept{
		jInterface::SetField<jlong>(JavaEnv,JavaRef,FieldID,static_cast<jlong>(reinterpret_cast<uIntn>(Value)));
		jLogException(JavaEnv);
	}

	JNIEnv *JavaEnv;
	jcObject *JavaRef;
	jField<jlong> *FieldID;
};
//---------------------------------------------------------------------------
template<class TClass,const char *FieldName>
struct cFieldAccessMaker
{
	JNIEnv *env;
	TClass *obj;

	template<class TField>
	operator jFieldAccess<TField>()const noexcept{
		auto fid=jRegistration<jJavaContext>::InstanceField<FieldName,TClass,TField>(env);
		return {env,obj,fid};
	}

	template<class TPointer>
	operator jPointerFieldAccess<TPointer>()const noexcept{
		auto fid=jRegistration<jJavaContext>::InstanceField<FieldName,TClass,jlong>(env);
		return {env,obj,fid};
	}
};
template<const char *FieldName,class TClass>
cFieldAccessMaker<TClass,FieldName> jFieldAccessMaker(JNIEnv *env,TClass *obj)noexcept
{
	cFieldAccessMaker<TClass,FieldName> r={env,obj};
	return r;
}

//---------------------------------------------------------------------------
template<class TField>
struct jStaticFieldAccess
{
	jStaticFieldAccess(JNIEnv *env,jcClass *cls,jField<TField> *FieldID)noexcept
		: JavaEnv(env)
		, Class(cls)
		, FieldID(FieldID)
	{}

	TField Get(void)noexcept{
		TField Ret=jInterface::GetStaticField<TField>(JavaEnv,Class,FieldID);
		jLogException(JavaEnv);
		return Ret;
	}

	void Set(TField Value)noexcept{
		jInterface::SetStaticField<TField>(JavaEnv,Class,FieldID,Value);
		jLogException(JavaEnv);
	}

	JNIEnv *JavaEnv;
	jcClass *Class;
	jField<TField> *FieldID;
};
//---------------------------------------------------------------------------
template<class TFieldClass>
struct jStaticFieldAccess<TFieldClass*>
{
	jStaticFieldAccess(JNIEnv *env,jcClass *cls,jField<TFieldClass*> *FieldID)noexcept
		: JavaEnv(env)
		, Class(cls)
		, FieldID(FieldID)
	{}

	jrLocal<TFieldClass> Get(void)noexcept{
		TFieldClass* Ret=jInterface::GetStaticField<TFieldClass*>(JavaEnv,Class,FieldID);
		if(Ret==nullptr)
			jLogException(JavaEnv);
		return jrLocal<TFieldClass>(JavaEnv,Ret);
	}

	void Set(TFieldClass *Value)noexcept{
		jInterface::SetStaticField<TFieldClass*>(JavaEnv,Class,FieldID,Value);
		jLogException(JavaEnv);
	}

	JNIEnv *JavaEnv;
	jcClass *Class;
	jField<TFieldClass*> *FieldID;
};
//---------------------------------------------------------------------------
template<class T>
struct jPointerStaticFieldAccess
{
	jPointerStaticFieldAccess(JNIEnv *env,jcClass *cls,jField<jlong> *FieldID)noexcept
		: JavaEnv(env)
		, Class(cls)
		, FieldID(FieldID)
	{}

	T* Get(void)noexcept{
		jlong Ret=jInterface::GetStaticField<jlong>(JavaEnv,Class,FieldID);
		jLogException(JavaEnv);
		return reinterpret_cast<T*>(static_cast<uIntn>(Ret));
	}

	void Set(T *Value)noexcept{
		jInterface::SetStaticField<jlong>(JavaEnv,Class,FieldID,static_cast<jlong>(reinterpret_cast<uIntn>(Value)));
		jLogException(JavaEnv);
	}

	JNIEnv *JavaEnv;
	jcClass *Class;
	jField<jlong> *FieldID;
};
//---------------------------------------------------------------------------
template<class TClass,const char *FieldName>
struct cStaticFieldAccessMaker
{
	JNIEnv *env;

	template<class TField>
	operator jStaticFieldAccess<TField>()const noexcept{
		auto fid=jRegistration<jJavaContext>::StaticField<TClass,FieldName,TField>(env);
		return {env,jRegistration<jJavaContext>::Class<TClass>(env),fid};
	}

	template<class TPointer>
	operator jPointerStaticFieldAccess<TPointer>()const noexcept{
		auto fid=jRegistration<jJavaContext>::StaticField<TClass,FieldName,jlong>(env);
		return {env,jRegistration<jJavaContext>::Class<TClass>(env),fid};
	}
};

template<class TClass,const char *FieldName>
cStaticFieldAccessMaker<TClass,FieldName> jStaticFieldAccessMaker(JNIEnv *env)noexcept
{
	cStaticFieldAccessMaker<TClass,FieldName> r={env};
	return r;
}

//---------------------------------------------------------------------------
template<const char *FunctionName,auto pFunction>
struct jMethodCall;
//---------------------------------------------------------------------------
template<const char *FunctionName,class TClass,class TRet,TRet (TClass::*pFunction)(JNIEnv*)noexcept>
struct jMethodCall<FunctionName,pFunction>
{
	static TRet Call(JNIEnv *env,TClass *obj)noexcept{
		auto mid=jRegistration<jJavaContext>::InstanceMethod<FunctionName,TClass,TRet>(env);
		return jInterface::CallMethodA<TRet>(env,obj,mid,nullptr);
	}
};
//---------------------------------------------------------------------------
template<const char *FunctionName,class TClass,class TRet,class TArg,class...TArgs,TRet (TClass::*pFunction)(JNIEnv*,TArg,TArgs...)noexcept>
struct jMethodCall<FunctionName,pFunction>
{
	static TRet Call(JNIEnv *env,TClass *obj,TArg Arg,TArgs...Args)noexcept{
		auto mid=jRegistration<jJavaContext>::InstanceMethod<FunctionName,TClass,TRet,TArg,TArgs...>(env);
		jvalue args[1+sizeof...(TArgs)]={MakeJValue(Arg),MakeJValue(Args)...};
		return jInterface::CallMethodA<TRet>(env,obj,mid,args);
	}
};
//---------------------------------------------------------------------------
template<const char *FunctionName,class TClass,class TRetClass,jrLocal<TRetClass> (TClass::*pFunction)(JNIEnv*)noexcept>
struct jMethodCall<FunctionName,pFunction>
{
	static jrLocal<TRetClass> Call(JNIEnv *env,TClass *obj)noexcept{
		auto mid=jRegistration<jJavaContext>::InstanceMethod<FunctionName,TClass,TRetClass*>(env);
		return RefLocal(env,jInterface::CallMethodA<TRetClass*>(env,obj,mid,nullptr));
	}
};
//---------------------------------------------------------------------------
template<const char *FunctionName,class TClass,class TRetClass,class TArg,class...TArgs,jrLocal<TRetClass> (TClass::*pFunction)(JNIEnv*,TArg,TArgs...)noexcept>
struct jMethodCall<FunctionName,pFunction>
{
	static jrLocal<TRetClass> Call(JNIEnv *env,TClass *obj,TArg Arg,TArgs...Args)noexcept{
		auto mid=jRegistration<jJavaContext>::InstanceMethod<FunctionName,TClass,TRetClass*,TArg,TArgs...>(env);
		jvalue args[1+sizeof...(TArgs)]={MakeJValue(Arg),MakeJValue(Args)...};
		return RefLocal(env,jInterface::CallMethodA<TRetClass*>(env,obj,mid,args));
	}
};
//---------------------------------------------------------------------------
template<class TJavaClass,const char *FunctionName,auto pFunction>
struct jStaticMethodCall;
//---------------------------------------------------------------------------
template<class TJavaClass,const char *FunctionName,class TRet,TRet (*pFunction)(JNIEnv*)noexcept>
struct jStaticMethodCall<TJavaClass,FunctionName,pFunction>
{
	static TRet Call(JNIEnv *env)noexcept{
		auto cls=jRegistration<jJavaContext>::Class<TJavaClass>(env);
		auto mid=jRegistration<jJavaContext>::StaticMethod<TJavaClass,FunctionName,TRet>(env);
		return jInterface::CallStaticMethodA<TRet>(env,cls,mid,nullptr);
	}
};
//---------------------------------------------------------------------------
template<class TJavaClass,const char *FunctionName,class TRet,class TArg,class...TArgs,TRet (*pFunction)(JNIEnv*,TArg,TArgs...)noexcept>
struct jStaticMethodCall<TJavaClass,FunctionName,pFunction>
{
	static TRet Call(JNIEnv *env,TArg Arg,TArgs...Args)noexcept{
		auto cls=jRegistration<jJavaContext>::Class<TJavaClass>(env);
		auto mid=jRegistration<jJavaContext>::StaticMethod<TJavaClass,FunctionName,TRet,TArg,TArgs...>(env);
		jvalue args[1+sizeof...(TArgs)]={MakeJValue(Arg),MakeJValue(Args)...};
		return jInterface::CallStaticMethodA<TRet>(env,cls,mid,args);
	}
};
//---------------------------------------------------------------------------
template<class TJavaClass,const char *FunctionName,class TRetClass,jrLocal<TRetClass> (*pFunction)(JNIEnv*)noexcept>
struct jStaticMethodCall<TJavaClass,FunctionName,pFunction>
{
	static jrLocal<TRetClass> Call(JNIEnv *env)noexcept{
		auto cls=jRegistration<jJavaContext>::Class<TJavaClass>(env);
		auto mid=jRegistration<jJavaContext>::StaticMethod<TJavaClass,FunctionName,TRetClass*>(env);
		return RefLocal(env,jInterface::CallStaticMethodA<TRetClass*>(env,cls,mid,nullptr));
	}
};
//---------------------------------------------------------------------------
template<class TJavaClass,const char *FunctionName,class TRetClass,class TArg,class...TArgs,jrLocal<TRetClass> (*pFunction)(JNIEnv*,TArg,TArgs...)noexcept>
struct jStaticMethodCall<TJavaClass,FunctionName,pFunction>
{
	static jrLocal<TRetClass> Call(JNIEnv *env,TArg Arg,TArgs...Args)noexcept{
		auto cls=jRegistration<jJavaContext>::Class<TJavaClass>(env);
		auto mid=jRegistration<jJavaContext>::StaticMethod<TJavaClass,FunctionName,TRetClass*,TArg,TArgs...>(env);
		jvalue args[1+sizeof...(TArgs)]={MakeJValue(Arg),MakeJValue(Args)...};
		return RefLocal(env,jInterface::CallStaticMethodA<TRetClass*>(env,cls,mid,args));
	}
};
//---------------------------------------------------------------------------
struct jcObject
{
	static constexpr const char jClassName[]="java/lang/Object";

	static constexpr const char jname_getClass[]="getClass";
	jrLocal<jcClass> getClass(JNIEnv *env)noexcept{
		return jMethodCall<jname_getClass,&jcObject::getClass>::Call(env,this);
	}
	
	static constexpr const char jname_toString[]="toString";
	jrLocal<jcString> toString(JNIEnv *env)noexcept{
		return jMethodCall<jname_toString,&jcObject::toString>::Call(env,this);
	} 
};
//---------------------------------------------------------------------------
struct jcClass : jcObject
{
	static constexpr const char jClassName[]="java/lang/Class";
};
//---------------------------------------------------------------------------
struct jStringAccess
{
	jStringAccess(JNIEnv *env,jcString *s)noexcept
		: env(env)
		, StringRef(s)
		, Pointer(reinterpret_cast<const uChar16*>(jInterface::GetStringChars(env,s,reinterpret_cast<jboolean*>(&IsCopy))))
	{}
	~jStringAccess()noexcept{
		if(Pointer!=nullptr){
			jInterface::ReleaseStringChars(env,StringRef,reinterpret_cast<const jchar*>(Pointer));
		}
	}

	JNIEnv *env;
	jcString *StringRef;
	const uChar16 *Pointer;
	bool IsCopy;
};
//---------------------------------------------------------------------------
struct jStringAccessCritical
{
	jStringAccessCritical(JNIEnv *env,jcString *s)noexcept
		: env(env)
		, StringRef(s)
		, Pointer(reinterpret_cast<const uChar16*>(jInterface::GetStringCritical(env,s,reinterpret_cast<jboolean*>(&IsCopy))))
	{}

	~jStringAccessCritical()noexcept{
		if(Pointer!=nullptr){
			jInterface::ReleaseStringCritical(env,StringRef,reinterpret_cast<const jchar*>(Pointer));
		}
	}


	JNIEnv *env;
	jcString *StringRef;
	const uChar16 *Pointer;
	bool IsCopy;
};
//---------------------------------------------------------------------------
struct jcString : jcObject
{
	static constexpr const char jClassName[]="java/lang/String";
	
	template<uIntn Length>
	static jrLocal<jcString> Make(JNIEnv *env,const uChar16 (&Text)[Length])noexcept{
		return RefLocal(env,jInterface::NewString(env,reinterpret_cast<const jchar*>(Text),static_cast<jsize>(Length-1)));
	}
	template<uIntn Length>
	static jrLocal<jcString> Make(JNIEnv *env,uChar16 (&Text)[Length])noexcept{
		return RefLocal(env,jInterface::NewString(env,reinterpret_cast<const jchar*>(Text),static_cast<jsize>(cnString::FindLength(Text,Length))));
	}

	static jrLocal<jcString> Make(JNIEnv *env,const uChar16 *Text,uIntn Length)noexcept{
		return RefLocal(env,jInterface::NewString(env,reinterpret_cast<const jchar*>(Text),static_cast<jsize>(Length)));
	}

	jsize length(JNIEnv *env)noexcept{
		return jInterface::GetStringLength(env,this);
	}

	jStringAccess Access(JNIEnv *env)noexcept{
		return {env,this};
	}

	jStringAccessCritical AccessCritical(JNIEnv *env)noexcept{
		return {env,this};
	}

};
//---------------------------------------------------------------------------
namespace java{
//---------------------------------------------------------------------------
namespace io{
//---------------------------------------------------------------------------
struct jcWriter : jcObject
{
	static constexpr const char jClassName[]="java/io/Writer";
};
//---------------------------------------------------------------------------
struct jcStringWriter : jcWriter
{
	static constexpr const char jClassName[]="java/io/StringWriter";
};
//---------------------------------------------------------------------------
struct jcPrintWriter : jcWriter
{
	static constexpr const char jClassName[]="java/io/PrintWriter";
};
//---------------------------------------------------------------------------
}	// namespace io
//---------------------------------------------------------------------------
}	// namespace java
//---------------------------------------------------------------------------
struct jcThrowable : jcObject
{
	static constexpr const char jClassName[]="java/lang/Throwable";


	static constexpr const char jname_getCause[]="getCause";
	jrLocal<jcThrowable> getCause(JNIEnv *env)noexcept{
		return jMethodCall<jname_getCause,&jcThrowable::getCause>::Call(env,this);
	}


	static constexpr const char jname_printStackTrace[]="printStackTrace";
	void printStackTrace(JNIEnv *env,java::io::jcPrintWriter *PrintWriter)noexcept{
		return jMethodCall<jname_printStackTrace,&jcThrowable::printStackTrace>::Call(env,this,PrintWriter);
	}
};
//---------------------------------------------------------------------------
template<class TJavaContext>
class cJNIExceptionDescription : private cJNIInitialization::cCallback, private cJNIFinalization::cCallback
{
public:
	cJNIExceptionDescription()noexcept{
		jRegistration<TJavaContext>::Initialization+=this;
	}

	bool IsInitialized(void)const noexcept{
		return midThrowableGetCause!=nullptr;
	}

	virtual void Initialize(JNIEnv *env)noexcept override{
		clsObject=jRegistration<TJavaContext>::template Class<jcObject>(env);
		if(clsObject!=nullptr){
			midObjectToString=jRegistration<TJavaContext>::template InstanceMethod<jcObject::jname_toString,jcObject,jcString*>(env);
		}
		clsStringWriter=jRegistration<TJavaContext>::template Class<java::io::jcStringWriter>(env);
		if(clsStringWriter!=nullptr){
			midConstructorStringWriter=jRegistration<TJavaContext>::template ConstructorMethod<java::io::jcStringWriter>(env);

		}
		clsPrintWriter=jRegistration<TJavaContext>::template Class<java::io::jcPrintWriter>(env);
		if(clsPrintWriter!=nullptr){
			midConstructorPrintWriter=jRegistration<TJavaContext>::template ConstructorMethod<java::io::jcPrintWriter,java::io::jcWriter*>(env);

		}
		clsThrowable=jRegistration<TJavaContext>::template Class<jcThrowable>(env);
		if(clsThrowable!=nullptr){
			midThrowablePrintStackTrace=jRegistration<TJavaContext>::template InstanceMethod<jcThrowable::jname_printStackTrace,jcThrowable,void,java::io::jcPrintWriter*>(env);
			midThrowableGetCause=jRegistration<TJavaContext>::template InstanceMethod<jcThrowable::jname_getCause,jcThrowable,jcThrowable*>(env);
		}

		{
			auto AutoLock=!jRegistration<TJavaContext>::Finalization;
			jRegistration<TJavaContext>::Finalization+=this;
		}

	}
	virtual void Finalize(JNIEnv *env)noexcept override{
		clsObject=nullptr;
		midObjectToString=nullptr;
		clsStringWriter=nullptr;
		midConstructorStringWriter=nullptr;
		clsPrintWriter=nullptr;
		midConstructorPrintWriter=nullptr;
		clsThrowable=nullptr;
		midThrowablePrintStackTrace=nullptr;
		midThrowableGetCause=nullptr;
	}

	jrLocal<jcString> Make(JNIEnv *env,jcThrowable *Exception)noexcept{
		if(midThrowableGetCause==nullptr)
			return nullptr;

		jvalue args[3];
		auto *Writer=jInterface::NewObjectA<java::io::jcStringWriter>(env,clsStringWriter,midConstructorStringWriter,nullptr);
		if(Writer==nullptr){
			return nullptr;
		}
		args[0]=MakeJValue(Writer);
		auto *Printer=jInterface::NewObjectA<java::io::jcPrintWriter>(env,clsPrintWriter,midConstructorPrintWriter,args);
		if(Printer==nullptr){
			return nullptr;
		}

		args[0]=MakeJValue(Printer);
		jInterface::CallMethodA<void>(env,Exception,midThrowablePrintStackTrace,args);
		if(jInterface::ExceptionCheck(env)){
			return nullptr;
		}

		jcThrowable* CauseException=jInterface::CallMethodA<jcThrowable*>(env,Exception,midThrowableGetCause,nullptr);
		while(CauseException!=nullptr){
			//args[0]=MakeJValue(Printer);
			jInterface::CallMethodA<void>(env,CauseException,midThrowablePrintStackTrace,args);
			if(jInterface::ExceptionCheck(env)){
				return nullptr;
			}
			auto CurException=CauseException;
			CauseException=jInterface::CallMethodA<jcThrowable*>(env,CauseException,midThrowableGetCause,nullptr);

			jInterface::DeleteLocalRef(env,CurException);
			if(jInterface::ExceptionCheck(env)){
				return nullptr;
			}
		}
		if(jInterface::ExceptionCheck(env)){
			return nullptr;
		}
		auto RetString=jInterface::CallMethodA<jcString*>(env,Writer,midObjectToString,nullptr);
		if(RetString==nullptr){
			return nullptr;
		}

		jInterface::DeleteLocalRef(env,Writer);
		if(jInterface::ExceptionCheck(env)){
			return nullptr;
		}
		jInterface::DeleteLocalRef(env,Printer);
		if(jInterface::ExceptionCheck(env)){
			return nullptr;
		}
		return RefLocal(env,RetString);
	}

	static cJNIExceptionDescription Instance;
private:

	jcClass *clsObject;
	jMethod<jcString* (jcObject::*)(void)> *midObjectToString;

	jcClass *clsStringWriter;
	jMethod<void (void)> *midConstructorStringWriter;
	jcClass *clsPrintWriter;
	jMethod<void (java::io::jcWriter*)> *midConstructorPrintWriter;
	jcClass *clsThrowable;
	jMethod<void (jcThrowable::*)(java::io::jcPrintWriter *PrintWriter)> *midThrowablePrintStackTrace;
	jMethod<jcThrowable* (jcThrowable::*)(void)> *midThrowableGetCause;

};
//---------------------------------------------------------------------------
template<class TJavaContext>
cJNIExceptionDescription<TJavaContext> cJNIExceptionDescription<TJavaContext>::Instance;
//---------------------------------------------------------------------------
template<class TJavaContext>
inline jrLocal<jcString> jMakeExceptionDescription(JNIEnv *env,jcThrowable *Exception)noexcept
{
	return cJNIExceptionDescription<TJavaContext>::Instance.Make(env,Exception);
}
//---------------------------------------------------------------------------
template<class TJavaContext,class TStreamWriteBuffer>
inline void jWriteExceptionDescription(TStreamWriteBuffer&& WriteBuffer,JNIEnv *env,jcThrowable *Exception)noexcept
{
	auto ExceptionString=cJNIExceptionDescription<TJavaContext>::Instance.Make(env,Exception);
	if(ExceptionString!=nullptr){
		// failed
		auto StrLength=ExceptionString->length(env);
		auto StrAccess=ExceptionString->AccessCritical(env);
		WriteBuffer+=cnRTL::ArrayStreamArray(StrAccess.Pointer,StrLength);
	}
}
//---------------------------------------------------------------------------
template<class TJavaContext>
inline bool jLogException(JNIEnv *env)noexcept
{
	if(cJNIExceptionDescription<TJavaContext>::Instance.IsInitialized()==false){
		if(jInterface::ExceptionCheck(env)){
			jInterface::ExceptionDescribe(env);
			jInterface::ExceptionClear(env);
			return true;
		}
		return false;
	}

	auto Exception=jInterface::ExceptionOccurred(env);
	if(Exception==nullptr){
		return false;
	}
	else{
		// clear exception in order to read exception message
		jInterface::ExceptionClear(env);

		// read exception message
		auto ExceptionString=jMakeExceptionDescription<TJavaContext>(env,Exception);
		if(ExceptionString!=nullptr){
			// failed
			auto StrLength=ExceptionString->length(env);
			auto StrAccess=ExceptionString->AccessCritical(env);
			{
				auto Stream=cnRTL::gRTLLog.MakeLogBuffer<1>(u"cnLibrary/jCPP");

				Stream+=cnRTL::ArrayStreamArray(StrAccess.Pointer,StrLength);
			}
		}

		jInterface::DeleteLocalRef(env,Exception);
	}
	return true;
}
//---------------------------------------------------------------------------
inline void jCPPInterfaceCallCheck(const char *Function,JNIEnv *env)noexcept
{
	if(env==nullptr){
		auto Stream=cnRTL::gRTLLog.MakeLogBuffer<1>(u"cnLibrary/jCPP");
		Stream+=cnRTL::ArrayStreamCString(u"passing null method id when calling jni interface ");
		Stream+=cnRTL::StringStreamConvertEncoding(cnRTL::UnicodeTranscoder(2,1),Function,cnString::FindLength(Function));
		return;
	}
	if(cJNIExceptionDescription<jJavaContext>::Instance.IsInitialized()==false){
		if(jInterface::ExceptionCheck(env)){
			jInterface::ExceptionDescribe(env);
		}
		return;
	}

	auto Exception=jInterface::ExceptionOccurred(env);
	if(Exception==nullptr)
		return;

	// clear exception in order to read exception message
	jInterface::ExceptionDescribe(env);
	jInterface::ExceptionClear(env);


	{
		auto Stream=cnRTL::gRTLLog.MakeLogBuffer<1>(u"cnLibrary/jCPP");
		Stream+=cnRTL::ArrayStreamArray(u"Exception pending when calling jni interface ");
		Stream+=cnRTL::StringStreamConvertEncoding(cnRTL::UnicodeTranscoder(2,1),Function,cnString::FindLength(Function));
		Stream+=cnRTL::ArrayStreamArray(u":\n");
		// read exception message
		auto ExceptionString=jMakeExceptionDescription<jJavaContext>(env,Exception);
		if(ExceptionString!=nullptr){
			// failed
			auto StrLength=ExceptionString->length(env);
			if(StrLength!=0){
				auto StrAccess=ExceptionString->AccessCritical(env);
				{
					Stream+=cnRTL::ArrayStreamArray(StrAccess.Pointer,StrLength);
				}
			}
			else{
				jInterface::ExceptionClear(env);
			}
		}
		else{
			jInterface::ExceptionClear(env);
		}
	}
	// rethrow exception
	jInterface::Throw(env,Exception);
	jInterface::DeleteLocalRefNoCheck(env,Exception);
}
//---------------------------------------------------------------------------
template<class TClass>
inline void jCPPInterfaceCallCheckPointer(const char *Function,TClass *CheckObject)noexcept
{
	if(CheckObject!=nullptr)
		return;

	auto Transcoder=cnRTL::UnicodeTranscoder(2,1);
	auto Stream=cnRTL::gRTLLog.MakeLogBuffer<1>(u"cnLibrary/jCPP");
	Stream+=cnRTL::ArrayStreamCString(u"pass null object when calling jni interface ");
	Stream+=cnRTL::StringStreamConvertEncoding(Transcoder,Function,cnString::FindLength(Function));
	Stream+=cnRTL::ArrayStreamCString(u": instance of class: ");
	Stream+=cnRTL::StringStreamConvertEncoding(Transcoder,TClass::jClassName,sizeof(TClass::jClassName)-1);
}
//---------------------------------------------------------------------------
inline void jCPPInterfaceCallCheckPointer(const char *Function,jbMethod *Method)
{
	if(Method!=nullptr)
		return;

	auto Transcoder=cnRTL::UnicodeTranscoder(2,1);
	auto Stream=cnRTL::gRTLLog.MakeLogBuffer<1>(u"cnLibrary/jCPP");
	Stream+=cnRTL::ArrayStreamCString(u"passing null method id when calling jni interface ");
	Stream+=cnRTL::StringStreamConvertEncoding(Transcoder,Function,cnString::FindLength(Function));
}
//---------------------------------------------------------------------------
inline void jCPPInterfaceCallCheckPointer(const char *Function,jbField *Field)
{
	if(Field!=nullptr)
		return;

	auto Transcoder=cnRTL::UnicodeTranscoder(2,1);
	auto Stream=cnRTL::gRTLLog.MakeLogBuffer<1>(u"cnLibrary/jCPP");
	Stream+=cnRTL::ArrayStreamCString(u"passing null field id when calling jni interface ");
	Stream+=cnRTL::StringStreamConvertEncoding(Transcoder,Function,cnString::FindLength(Function));
}
//---------------------------------------------------------------------------
inline void jCPPInterfaceCallCheckPointer(const char *Function,const jvalue *args)
{
	if(args!=nullptr)
		return;

	auto Transcoder=cnRTL::UnicodeTranscoder(2,1);
	auto Stream=cnRTL::gRTLLog.MakeLogBuffer<1>(u"cnLibrary/jCPP");
	Stream+=cnRTL::ArrayStreamCString(u"passing null for <const jvalue *> when calling jni interface ");
	Stream+=cnRTL::StringStreamConvertEncoding(Transcoder,Function,cnString::FindLength(Function));
}
//---------------------------------------------------------------------------
inline void jCPPInterfaceCallCheckPointer(const char *Function,const char *string)
{
	if(string!=nullptr)
		return;

	auto Transcoder=cnRTL::UnicodeTranscoder(2,1);
	auto Stream=cnRTL::gRTLLog.MakeLogBuffer<1>(u"cnLibrary/jCPP");
	Stream+=cnRTL::ArrayStreamCString(u"passing null for C string when calling jni interface ");
	Stream+=cnRTL::StringStreamConvertEncoding(Transcoder,Function,cnString::FindLength(Function));
}
//---------------------------------------------------------------------------
template<class TClass,class...TClasses>
inline void jCPPInterfaceCallCheck(const char *Function,JNIEnv *env,TClass *CheckPointer,TClasses*...CheckPointers)noexcept
{
	jCPPInterfaceCallCheckPointer(Function,CheckPointer);
	return jCPPInterfaceCallCheck(Function,env,CheckPointers...);
}
//---------------------------------------------------------------------------
struct jbcArray : jcObject
{
	jsize GetArrayLength(JNIEnv *env)noexcept{
		return jInterface::GetArrayLength(env,this);
	}
};
//---------------------------------------------------------------------------
template<class TElement>
struct jArrayAccess
{
	jArrayAccess(JNIEnv *env,jcArray<TElement> *aref)noexcept
		: env(env)
		, JavaArray(aref)
		, Pointer(jInterface::GetArrayElements(env,aref,reinterpret_cast<jboolean*>(&IsCopy)))
	{
	}

	~jArrayAccess()noexcept{
		if(Pointer!=nullptr){
			jInterface::ReleaseArrayElements(env,JavaArray,Pointer,Abort?jMode_ABORT:0);
		}
	}

	void Commit(void)noexcept{
		if(Pointer!=nullptr){
			jInterface::ReleaseArrayElements(env,JavaArray,Pointer,jMode_COMMIT);
		}
	}

	JNIEnv *env;
	jcArray<TElement> *JavaArray;
	TElement *Pointer;
	bool IsCopy;
	bool Abort=false;
};
//---------------------------------------------------------------------------
template<class TElement>
struct jArrayAccessCritical
{
	jArrayAccessCritical(JNIEnv *env,jcArray<TElement> *aref)noexcept
		: env(env)
		, JavaArray(aref)
		, Pointer(jInterface::GetArrayElementsCritical(env,aref,reinterpret_cast<jboolean*>(&IsCopy)))
	{
	}

	~jArrayAccessCritical()noexcept{
		if(Pointer!=nullptr){
			jInterface::ReleaseArrayElementsCritical(env,JavaArray,Pointer,Abort?jMode_ABORT:0);
		}
	}

	void Commit(void)noexcept{
		if(Pointer!=nullptr){
			jInterface::ReleaseArrayElementsCritical(env,JavaArray,Pointer,jMode_COMMIT);
		}
	}

	JNIEnv *env;
	jcArray<TElement> *JavaArray;
	TElement *Pointer;
	bool IsCopy;
	bool Abort=false;
};
//---------------------------------------------------------------------------
template<class TElement>
struct jcArray : jbcArray
{
	static jrLocal< jcArray<TElement> > Make(JNIEnv *env,jsize length)noexcept{
		return RefLocal(env,jInterface::NewArray<TElement>(env,length));
	}
	jArrayAccess<TElement> AccessElements(JNIEnv *env)noexcept{
		return jArrayAccess<TElement>(env,this);
	}

	jArrayAccessCritical<TElement> AccessElementsCritical(JNIEnv *env)noexcept{
		return jArrayAccessCritical<TElement>(env,this);
	}
	
	void GetRegion(JNIEnv *env,jsize start,jsize len,jbyte *buf)noexcept{
		return jInterface::GetArrayRegion(env,this,start,len,buf);
	}
	void SetRegion(JNIEnv *env,jsize start,jsize len,const jbyte *buf)noexcept{
		return jInterface::SetArrayRegion(env,this,start,len,buf);
	}

};
//---------------------------------------------------------------------------
template<class TObjectElement>
struct jcArray<TObjectElement*> : jbcArray
{
	static jrLocal< jcArray<TObjectElement*> > Make(JNIEnv *env,uIntn Length)noexcept{
		auto cls=jRegistration<jJavaContext>::Class<TObjectElement>(env);
		return RefLocal(env,jInterface::NewObjectArray<TObjectElement>(env,Length,cls,nullptr));
	}

	jrLocal<TObjectElement> GetElement(JNIEnv *env,jint Index)noexcept{
		return RefLocal(env,jInterface::GetObjectArrayElement(env,this,Index));
	}
	void SetElement(JNIEnv *env,jint Index,TObjectElement *Value)noexcept{
		return jInterface::SetObjectArrayElement(env,this,Index,Value);
	}
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

#if JCPP_JNIHEADER


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

#endif // JCPP_JNIHEADER
//---------------------------------------------------------------------------
}	// namespace JNI
}	// namespace cnLib_THelper
//---------------------------------------------------------------------------

#endif
