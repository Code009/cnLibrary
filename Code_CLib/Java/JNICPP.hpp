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

	TJavaClass Get(void)const noexcept{ return fJavaRef; }
	void Set(TJavaClass Ref)noexcept{
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
	TClass *Ret=jInterface::NewObjectA<TClass>(env,TClassRef<jJavaContext,TClass>::Value,TConstructorMethod<jJavaContext,TClass,&jNew<TClass> >::Value,nullptr);
	if(Ret==nullptr){
		jLogException(env);
	}
	return RefLocal(env,Ret);
}
//---------------------------------------------------------------------------
template<class TClass,class...TArgs>
inline jrLocal<TClass> jNew(JNIEnv *env,TArgs...Args)noexcept
{
	jvalue args[sizeof...(TArgs)]={MakeJValue(Args)...};
	TClass *Ret=jInterface::NewObjectA<TClass>(env,TClassRef<jJavaContext,TClass>::Value,TConstructorMethod<jJavaContext,TClass,&jNew<TClass,TArgs...> >::Value,args);
	if(Ret==nullptr){
		jLogException(env);
	}
	return RefLocal(env,Ret);
}
//---------------------------------------------------------------------------
template<class TField>
struct jFieldAccess
{
	jFieldAccess(jcObject *JavaRef,jField<TField> *FieldID)noexcept
		: JavaRef(JavaRef)
		, FieldID(FieldID)
	{}

	TField Get(JNIEnv *env)noexcept{
		TField Ret=jInterface::GetField<TField>(env,JavaRef,FieldID);
		jLogException(env);
		return Ret;
	}

	void Set(JNIEnv *env,TField Value)noexcept{
		jInterface::SetField<TField>(env,JavaRef,FieldID,Value);
		jLogException(env);
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

	jrLocal<TFieldClass> Get(JNIEnv *env)noexcept{
		TFieldClass* Ret=jInterface::GetField<TFieldClass*>(env,JavaRef,FieldID);
		if(Ret==nullptr)
			jLogException(env);
		return jrLocal<TFieldClass>(env,Ret);
	}

	void Set(JNIEnv *env,TFieldClass *Value)noexcept{
		jInterface::SetField<TFieldClass*>(env,JavaRef,FieldID,Value);
		jLogException(env);
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

	T* Get(JNIEnv *env)noexcept{
		jlong Ret=jInterface::GetField<jlong>(env,JavaRef,FieldID);
		jLogException(env);
		return reinterpret_cast<T*>(static_cast<uIntn>(Ret));
	}

	void Set(JNIEnv *env,T *Value)noexcept{
		jInterface::SetField<jlong>(env,JavaRef,FieldID,static_cast<jlong>(reinterpret_cast<uIntn>(Value)));
		jLogException(env);
	}

	jcObject *JavaRef;
	jField<jlong> *FieldID;
};
//---------------------------------------------------------------------------
template<class TClass,const char *FieldName>
struct cFieldAccessMaker
{
	TClass *obj;

	template<class TField>
	operator jFieldAccess<TField>()const noexcept{
		return {obj,TInstanceField<jJavaContext,FieldName,TClass,TField>::Value};
	}

	template<class TPointer>
	operator jPointerFieldAccess<TPointer>()const noexcept{
		return {obj,TInstanceField<jJavaContext,FieldName,TClass,jlong>::Value};
	}
};
template<const char *FieldName,class TClass>
cFieldAccessMaker<TClass,FieldName> jFieldAccessMaker(TClass *obj)noexcept
{
	cFieldAccessMaker<TClass,FieldName> r={obj};
	return r;
}

//---------------------------------------------------------------------------
template<class TField>
struct jStaticFieldAccess
{
	jStaticFieldAccess(jcClass *cls,jField<TField> *FieldID)noexcept
		: Class(cls)
		, FieldID(FieldID)
	{}

	TField Get(JNIEnv *env)noexcept{
		TField Ret=jInterface::GetStaticField<TField>(env,Class,FieldID);
		jLogException(env);
		return Ret;
	}

	void Set(JNIEnv *env,TField Value)noexcept{
		jInterface::SetStaticField<TField>(env,Class,FieldID,Value);
		jLogException(env);
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

	jrLocal<TFieldClass> Get(JNIEnv *env)noexcept{
		TFieldClass* Ret=jInterface::GetStaticField<TFieldClass*>(env,Class,FieldID);
		if(Ret==nullptr)
			jLogException(env);
		return jrLocal<TFieldClass>(env,Ret);
	}

	void Set(JNIEnv *env,TFieldClass *Value)noexcept{
		jInterface::SetStaticField<TFieldClass*>(env,Class,FieldID,Value);
		jLogException(env);
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

	T* Get(JNIEnv *env)noexcept{
		jlong Ret=jInterface::GetStaticField<jlong>(env,Class,FieldID);
		jLogException(env);
		return reinterpret_cast<T*>(static_cast<uIntn>(Ret));
	}

	void Set(JNIEnv *env,T *Value)noexcept{
		jInterface::SetStaticField<jlong>(env,Class,FieldID,static_cast<jlong>(reinterpret_cast<uIntn>(Value)));
		jLogException(env);
	}

	jcClass *Class;
	jField<jlong> *FieldID;
};
//---------------------------------------------------------------------------
template<class TClass,const char *FieldName>
struct jStaticFieldAccessMaker
{
	template<class TField>
	operator jStaticFieldAccess<TField>()const noexcept{
		return {TClassRef<jJavaContext,TClass>::Value,TStaticField<jJavaContext,TClass,FieldName,TField>::Value};
	}

	template<class TPointer>
	operator jPointerStaticFieldAccess<TPointer>()const noexcept{
		return {TClassRef<jJavaContext,TClass>::Value,TStaticField<jJavaContext,TClass,FieldName,jlong>::Value};
	}
};
//---------------------------------------------------------------------------
template<const char *FunctionName,auto pFunction>
struct jMethodCall;
//---------------------------------------------------------------------------
template<const char *FunctionName,class TClass,class TRet,TRet (TClass::*pFunction)(JNIEnv*)noexcept>
struct jMethodCall<FunctionName,pFunction>
{
	static TRet Call(JNIEnv *env,TClass *obj)noexcept{
		return jInterface::CallMethodA<TRet>(env,obj,TInstanceMethod<jJavaContext,FunctionName,TClass,TRet>::Value,nullptr);
	}
};
//---------------------------------------------------------------------------
template<const char *FunctionName,class TClass,class TRet,class TArg,class...TArgs,TRet (TClass::*pFunction)(JNIEnv*,TArg,TArgs...)noexcept>
struct jMethodCall<FunctionName,pFunction>
{
	static TRet Call(JNIEnv *env,TClass *obj,TArg Arg,TArgs...Args)noexcept{
		jvalue args[1+sizeof...(TArgs)]={MakeJValue(Arg),MakeJValue(Args)...};
		return jInterface::CallMethodA<TRet>(env,obj,TInstanceMethod<jJavaContext,FunctionName,TClass,TRet,TArg,TArgs...>::Value,args);
	}
};
//---------------------------------------------------------------------------
template<const char *FunctionName,class TClass,class TRetClass,jrLocal<TRetClass> (TClass::*pFunction)(JNIEnv*)noexcept>
struct jMethodCall<FunctionName,pFunction>
{
	static jrLocal<TRetClass> Call(JNIEnv *env,TClass *obj)noexcept{
		return RefLocal(env,jInterface::CallMethodA<TRetClass*>(env,obj,TInstanceMethod<jJavaContext,FunctionName,TClass,TRetClass*>::Value,nullptr));
	}
};
//---------------------------------------------------------------------------
template<const char *FunctionName,class TClass,class TRetClass,class TArg,class...TArgs,jrLocal<TRetClass> (TClass::*pFunction)(JNIEnv*,TArg,TArgs...)noexcept>
struct jMethodCall<FunctionName,pFunction>
{
	static jrLocal<TRetClass> Call(JNIEnv *env,TClass *obj,TArg Arg,TArgs...Args)noexcept{
		jvalue args[1+sizeof...(TArgs)]={MakeJValue(Arg),MakeJValue(Args)...};
		return RefLocal(env,jInterface::CallMethodA<TRetClass*>(env,obj,TInstanceMethod<jJavaContext,FunctionName,TClass,TRetClass*,TArg,TArgs...>::Value,args));
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
		return jInterface::CallStaticMethodA<TRet>(env,TClassRef<jJavaContext,TJavaClass>::Value,TStaticMethod<jJavaContext,TJavaClass,FunctionName,TRet>::Value,nullptr);
	}
};
//---------------------------------------------------------------------------
template<class TJavaClass,const char *FunctionName,class TRet,class TArg,class...TArgs,TRet (*pFunction)(JNIEnv*,TArg,TArgs...)noexcept>
struct jStaticMethodCall<TJavaClass,FunctionName,pFunction>
{
	static TRet Call(JNIEnv *env,TArg Arg,TArgs...Args)noexcept{
		jvalue args[1+sizeof...(TArgs)]={MakeJValue(Arg),MakeJValue(Args)...};
		return jInterface::CallStaticMethodA<TRet>(env,TClassRef<jJavaContext,TJavaClass>::Value,TStaticMethod<jJavaContext,TJavaClass,FunctionName,TRet,TArg,TArgs...>::Value,args);
	}
};
//---------------------------------------------------------------------------
template<class TJavaClass,const char *FunctionName,class TRetClass,jrLocal<TRetClass> (*pFunction)(JNIEnv*)noexcept>
struct jStaticMethodCall<TJavaClass,FunctionName,pFunction>
{
	static jrLocal<TRetClass> Call(JNIEnv *env)noexcept{
		return RefLocal(env,jInterface::CallStaticMethodA<TRetClass*>(env,TClassRef<jJavaContext,TJavaClass>::Value,TStaticMethod<jJavaContext,TJavaClass,FunctionName,TRetClass*>::Value,nullptr));
	}
};
//---------------------------------------------------------------------------
template<class TJavaClass,const char *FunctionName,class TRetClass,class TArg,class...TArgs,jrLocal<TRetClass> (*pFunction)(JNIEnv*,TArg,TArgs...)noexcept>
struct jStaticMethodCall<TJavaClass,FunctionName,pFunction>
{
	static jrLocal<TRetClass> Call(JNIEnv *env,TArg Arg,TArgs...Args)noexcept{
		jvalue args[1+sizeof...(TArgs)]={MakeJValue(Arg),MakeJValue(Args)...};
		return RefLocal(env,jInterface::CallStaticMethodA<TRetClass*>(env,TClassRef<jJavaContext,TJavaClass>::Value,TStaticMethod<jJavaContext,TJavaClass,FunctionName,TRetClass*,TArg,TArgs...>::Value,args));
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
inline jrLocal<jcString> jMakeExceptionDescription(JNIEnv *env,jcThrowable *Exception)noexcept
{
	auto Writer=jNew<java::io::jcStringWriter>(env);
	if(Writer==nullptr){
		return nullptr;
	}
	auto Printer=jNew<java::io::jcPrintWriter,java::io::jcWriter*>(env,Writer);
	if(Printer==nullptr){
		return nullptr;
	}
	Exception->printStackTrace(env,Printer);
	if(jInterface::ExceptionCheck(env)){
		return nullptr;
	}

	auto CauseException=Exception->getCause(env);
	while(CauseException!=nullptr){
		CauseException->printStackTrace(env,Printer);
		if(jInterface::ExceptionCheck(env)){
			return nullptr;
		}

		CauseException=CauseException->getCause(env);
	}
	if(jInterface::ExceptionCheck(env)){
		return nullptr;
	}
	return Writer->toString(env);
}
//---------------------------------------------------------------------------
template<class TJavaContext>
inline bool jLogExceptionT(JNIEnv *env)noexcept
{
	if(TClassRef<TJavaContext,jcThrowable>::Value==nullptr){
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
		jInterface::ExceptionDescribe(env);
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
		else{
			jInterface::ExceptionDescribe(env);
			jInterface::ExceptionClear(env);
		}
	}
	return true;
}
//---------------------------------------------------------------------------
inline bool jLogException(JNIEnv *env)noexcept
{
	return jLogExceptionT<jJavaContext>(env);
}
//---------------------------------------------------------------------------
inline void jCPPInterfaceCallCheck(JNIEnv *env,const char *Function)noexcept
{
	if(TClassRef<jJavaContext,jcThrowable>::Value==nullptr){
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
		return RefLocal(env,jInterface::NewObjectArray<TObjectElement>(env,Length,TClassRef<jJavaContext,TObjectElement>::Value,nullptr));
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
