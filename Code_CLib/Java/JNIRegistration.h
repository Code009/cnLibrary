/*- cnAndroid - JNI - registration ----------------------------------------*/
/*         Developer : Code009                                             */
/*         Create on : 2024-04-01                                          */
/*-------------------------------------------------------------------------*/
#pragma once
/*-------------------------------------------------------------------------*/
#include <Java/JNIDecl.hpp>

#ifdef __cplusplus

//---------------------------------------------------------------------------
namespace cnLibrary{
//---------------------------------------------------------------------------
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

		virtual void Initialize(JNIEnv *env)noexcept(true)=0;
		virtual void Finalize(JNIEnv *env)noexcept(true)=0;
	};


	void Register(cInitialization *Initialization)noexcept(true);

	void Initialize(JNIEnv *env)noexcept(true);
	void Finalize(JNIEnv *env)noexcept(true);

private:
	cInitialization *fTail=nullptr;
	cInitialization *fHead=nullptr;
	cInitialization *fInitialized=nullptr;
};
//---------------------------------------------------------------------------
#ifndef jCPP_JAVACONTEXTNAMESPACE
#define	jCPP_JAVACONTEXTNAMESPACE	DefaultJavaContext
#endif

//---------------------------------------------------------------------------
inline namespace jCPP_JAVACONTEXTNAMESPACE{
//---------------------------------------------------------------------------
struct jJavaContext
{
	static JavaVM* vm(void)noexcept;
	static cJNIInitializationRegistration* reg(void)noexcept;
};
//---------------------------------------------------------------------------
inline JNIEnv* jQueryEnv(jint Version=jVERSION_1_6)noexcept
{
	JavaVM *vm=jJavaContext::vm();
	JNIEnv *env;
	auto ret=jInterface::GetEnv(vm,env,Version);
	if(ret==jRet_OK){
		return env;
	}
	return nullptr;
}
//---------------------------------------------------------------------------
class bcJNIInitialization : public cJNIInitializationRegistration::cInitialization
{
protected:
	bcJNIInitialization()noexcept{
		cJNIInitializationRegistration *Registration=jJavaContext::reg();
		Registration->Register(this);
	}

};
//---------------------------------------------------------------------------
}	// namespace jCPP_JAVACONTEXTNAMESPACE
//---------------------------------------------------------------------------
template<class TJavaContext,class T>
struct TClassRef
{
	class cInitialization : public bcJNIInitialization
	{
	public:
		jcClass *Value;

		virtual void Initialize(JNIEnv *env)noexcept{
			auto cls=jInterface::FindClass(env,T::jClassName);
			if(cls==nullptr){
				if(env->ExceptionCheck()){
					env->ExceptionDescribe();
					env->ExceptionClear();
				}
			}
			else{
				Value=jInterface::NewGlobalRef(env,cls);
				if(env->ExceptionCheck()){
					env->ExceptionDescribe();
					env->ExceptionClear();
				}
				jInterface::DeleteLocalRef(env,cls);
				if(env->ExceptionCheck()){
					env->ExceptionDescribe();
					env->ExceptionClear();
				}
			}
		}
		virtual void Finalize(JNIEnv *env)noexcept{
			jInterface::DeleteGlobalRef(env,reinterpret_cast<jcObject*>(Value));
			if(env->ExceptionCheck()){
				env->ExceptionDescribe();
				env->ExceptionClear();
			}
		}

	};
	static cInitialization Init;

	static constexpr jcClass*const &Value=Init.Value;
};
template<class TJavaContext,class T>
typename TClassRef<TJavaContext,T>::cInitialization TClassRef<TJavaContext,T>::Init;
//---------------------------------------------------------------------------
template<class TJavaContext,const char *FieldName,class TClass,class TField>
struct TInstanceField
{
	class cInitialization : public bcJNIInitialization
	{
	public:
		jField<TField> *Value;

		virtual void Initialize(JNIEnv *env)noexcept override{
			char TypeSignature[JNITypeSignatureWriter<TField>::Length+1];
			auto *p=JNITypeSignatureWriter<TField>::Write(TypeSignature);
			*p=0;
			Value=static_cast<jField<TField>*>(
				jInterface::GetFieldID(env,TClassRef<TJavaContext,TClass>::Value,FieldName,TypeSignature));
			if(Value==nullptr){
				if(env->ExceptionCheck()){
					env->ExceptionDescribe();
					env->ExceptionClear();
				}
			}
		}
		virtual void Finalize(JNIEnv*)noexcept override{
			Value=nullptr;
		}
	};
	static cInitialization Init;
	typedef TClass tClass;
	typedef TField tField;

	static constexpr jField<TField>*const &Value=Init.Value;
};
template<class TJavaContext,const char *FieldName,class TClass,class TField>
typename TInstanceField<TJavaContext,FieldName,TClass,TField>::cInitialization TInstanceField<TJavaContext,FieldName,TClass,TField>::Init;
//---------------------------------------------------------------------------
template<class TJavaContext,class TClass,const char *FieldName,class TField>
struct TStaticField
{
	class cInitialization : public bcJNIInitialization
	{
	public:
		jField<TField> *Value;

		virtual void Initialize(JNIEnv *env)noexcept override{
			char TypeSignature[JNITypeSignatureWriter<TField>::Length+1];
			auto *p=JNITypeSignatureWriter<TField>::Write(TypeSignature);
			*p=0;

			Value=static_cast<jField<TField>*>(
				jInterface::GetStaticFieldID(env,TClassRef<TJavaContext,TClass>::Value,FieldName,TypeSignature));
			if(Value==nullptr){
				if(env->ExceptionCheck()){
					env->ExceptionDescribe();
					env->ExceptionClear();
				}
			}
		}
		virtual void Finalize(JNIEnv*)noexcept override{
			Value=nullptr;
		}
	};
	static cInitialization Init;
	typedef TField tField;

	static constexpr jField<TField>*const &Value=Init.Value;
};
template<class TJavaContext,class TClass,const char *FieldName,class TField>
typename TStaticField<TJavaContext,TClass,FieldName,TField>::cInitialization TStaticField<TJavaContext,TClass,FieldName,TField>::Init;

//---------------------------------------------------------------------------
template<class TJavaContext,const char *FunctionName,class TClass,class TRet,class...TArgs>
struct TInstanceMethod
{
	class cInitialization : public bcJNIInitialization
	{
	public:
		jMethod<TRet (TClass::*)(TArgs...)> *Value;

		virtual void Initialize(JNIEnv *env)noexcept override{
			JNIMethodSignatureGenerator<TRet,TArgs...> SigGen;
			Value=static_cast<jMethod<TRet (TClass::*)(TArgs...)>*>(
				jInterface::GetMethodID(env,TClassRef<TJavaContext,TClass>::Value,FunctionName,SigGen.Value));
			if(Value==nullptr){
				if(env->ExceptionCheck()){
					env->ExceptionDescribe();
					env->ExceptionClear();
				}
			}
		}
		virtual void Finalize(JNIEnv*)noexcept override{
			Value=nullptr;
		}
	};
	static cInitialization Init;

	static constexpr jMethod<TRet (TClass::*)(TArgs...)>*const &Value=Init.Value;
};
template<class TJavaContext,const char *FunctionName,class TClass,class TRet,class...TArgs>
typename TInstanceMethod<TJavaContext,FunctionName,TClass,TRet,TArgs...>::cInitialization TInstanceMethod<TJavaContext,FunctionName,TClass,TRet,TArgs...>::Init;
//---------------------------------------------------------------------------
template<class TJavaContext,class TClass,const char *FunctionName,class TRet,class...TArgs>
struct TStaticMethod
{
	class cInitialization : public bcJNIInitialization
	{
	public:
		jStaticMethod<TClass,TRet (TArgs...)> *Value;

		virtual void Initialize(JNIEnv *env)noexcept override{
			JNIMethodSignatureGenerator<TRet,TArgs...> SigGen;
			Value=static_cast<jStaticMethod<TClass,TRet (TArgs...)>*>(
				jInterface::GetStaticMethodID(env,TClassRef<TJavaContext,TClass>::Value,FunctionName,SigGen.Value));
			if(Value==nullptr){
				if(env->ExceptionCheck()){
					env->ExceptionDescribe();
					env->ExceptionClear();
				}
			}
		}
		virtual void Finalize(JNIEnv*)noexcept override{
			Value=nullptr;
		}
	};
	static cInitialization Init;

	static constexpr jStaticMethod<TClass,TRet (TArgs...)>*const &Value=Init.Value;
};
template<class TJavaContext,class TClass,const char *FunctionName,class TRet,class...TArgs>
typename TStaticMethod<TJavaContext,TClass,FunctionName,TRet,TArgs...>::cInitialization TStaticMethod<TJavaContext,TClass,FunctionName,TRet,TArgs...>::Init;
//---------------------------------------------------------------------------
template<class TJavaContext,class TClass,auto pFunction>
struct TConstructorMethod;

template<class TJavaContext,class TClass,class...TArgs,jrLocal<TClass> (*pFunction)(JNIEnv*,TArgs...)noexcept>
struct TConstructorMethod<TJavaContext,TClass,pFunction>
{
	class cInitialization : public bcJNIInitialization
	{
	public:
		jMethod<void (TArgs...)> *Value;

		virtual void Initialize(JNIEnv *env)noexcept override{
			JNIMethodSignatureGenerator<void,TArgs...> SigGen;
			Value=static_cast<jMethod<void (TArgs...)>*>(
				jInterface::GetMethodID(env,TClassRef<TJavaContext,TClass>::Value,"<init>",SigGen.Value));
			if(Value==nullptr){
				if(env->ExceptionCheck()){
					env->ExceptionDescribe();
					env->ExceptionClear();
				}
			}
		}
		virtual void Finalize(JNIEnv*)noexcept override{
			Value=nullptr;
		}
	};
	static cInitialization Init;

	static constexpr jMethod<void (TArgs...)>*const &Value=Init.Value;
};
template<class TJavaContext,class TClass,class...TArgs,jrLocal<TClass> (*pFunction)(JNIEnv*,TArgs...)noexcept>
typename TConstructorMethod<TJavaContext,TClass,pFunction>::cInitialization TConstructorMethod<TJavaContext,TClass,pFunction>::Init;


template<auto FunctionMember>
struct JNINativeMethodName;
//---------------------------------------------------------------------------
}	// namespace jCPP
//---------------------------------------------------------------------------
}	// namespace cnLibrary
//---------------------------------------------------------------------------
namespace cnLib_THelper{
namespace JNI{


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
		p->name=JNINativeMethodName<pFunction>::Value;
		p->signature=(*Accessor).Value;
		return JNINativeMethodWriter<NativeFunctionPointers...>::Write(p+1,+Accessor);
	}
};

template<auto Func>
struct TNativeSelectMethodSignatureGenerator;

template<class TThis,class TRet,class...TArgs,TRet (*pFunc)(jCPP::JNIEnv *env,TThis *obj,TArgs...)>
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
//---------------------------------------------------------------------------
namespace jCPP{
//---------------------------------------------------------------------------
template<class T,auto...Func>
class JNINativeMethodRegister : public bcJNIInitialization
{
public:
	virtual void Initialize(JNIEnv *env)noexcept override{
		cnLib_THelper::JNI::JNINativeMethodGenerator<Func...> ms;
		jInterface::RegisterNatives(env,TClassRef<jJavaContext,T>::Value,ms.Value,ms.Length);
		if(env->ExceptionCheck()){
			env->ExceptionDescribe();
			env->ExceptionClear();
		}
	}
	virtual void Finalize(JNIEnv *env)noexcept override{
		jInterface::UnregisterNatives(env,TClassRef<jJavaContext,T>::Value);
		if(env->ExceptionCheck()){
			env->ExceptionDescribe();
			env->ExceptionClear();
		}
	}
};
//---------------------------------------------------------------------------
}	// namespace jCPP
//---------------------------------------------------------------------------
}	// namespace cnLibrary
//---------------------------------------------------------------------------

#endif
