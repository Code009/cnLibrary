/*- cnAndroid - JNI - registration ----------------------------------------*/
/*         Developer : Code009                                             */
/*         Create on : 2024-04-01                                          */
/*-------------------------------------------------------------------------*/
#pragma once
/*-------------------------------------------------------------------------*/
#include <Java/JNIDecl.hpp>
#include <cnRTL/ThreadSynchronization.h>
#include <cnRTL/TextProcess.h>
#include <cnRTL/Logger.h>

#ifdef __cplusplus

//---------------------------------------------------------------------------
namespace cnLibrary{
//---------------------------------------------------------------------------
namespace jCPP{
//---------------------------------------------------------------------------
class cJNIInitialization
{
public:
	class cCallback
	{
	private:
		friend cJNIInitialization;
		cCallback *Next;

		virtual void Initialize(JNIEnv *env)noexcept=0;
	};


	void operator +=(cCallback *Initialization)noexcept(true);
	void operator ()(JNIEnv *env)const noexcept;

private:
	cCallback *fTail=nullptr;
	cCallback *fHead=nullptr;
};
//---------------------------------------------------------------------------
class cJNIFinalization
{
public:
	class cCallback
	{
	private:
		friend cJNIFinalization;
		cCallback *Next;

		virtual void Finalize(JNIEnv *env)noexcept=0;
	};

	cJNIFinalization()noexcept;
	~cJNIFinalization()noexcept;

	void operator +=(cCallback *Finalization)noexcept(true);
	void operator ()(JNIEnv *env)noexcept;
	cnRTL::lockPtr<iMutex*> operator !(void)const noexcept;

private:
	cCallback *fTop=nullptr;
	rPtr<iMutex> fMutex;
};
//---------------------------------------------------------------------------
template<class TJavaContext>
struct jRegistration
{
	static cJNIInitialization Initialization;
	static cJNIFinalization Finalization;

	template<class T>
	struct cClassFinalization : public cJNIFinalization::cCallback
	{
		jcClass *Value;
		virtual void Finalize(JNIEnv *env)noexcept{
			jInterface::DeleteGlobalRef(env,Value);
			jLogException<TJavaContext>(env);
		}
		static cClassFinalization<T> gInstance;
	};

	template<class T>
	static jcClass* Class(JNIEnv *env)noexcept{
		if(cClassFinalization<T>::gInstance.Value==nullptr){
			auto AutoLock=!Finalization;
			if(cClassFinalization<T>::gInstance.Value==nullptr){
				auto *cls=jFindClass<TJavaContext>(env,T::jClassName);
				if(cls!=nullptr){
					cClassFinalization<T>::gInstance.Value=jInterface::NewGlobalRef(env,cls);
					if(cClassFinalization<T>::gInstance.Value!=nullptr){
						Finalization+=&cClassFinalization<T>::gInstance;
					}
					else{
						jLogException<TJavaContext>(env);
					}
					jInterface::DeleteLocalRef(env,cls);
					jLogException<TJavaContext>(env);
				}
				else{
					auto Exception=jInterface::ExceptionOccurred(env);
					if(Exception!=nullptr){
						jInterface::ExceptionClear(env);
						auto Transcoder=cnRTL::UnicodeTranscoder(2,1);
						auto Stream=cnRTL::gRTLLog.MakeLogBuffer<1>(u"cnLibrary/jCPP");

						Stream+=cnRTL::ArrayStreamString(u"FindClass <");
						Stream+=cnRTL::StringStreamConvertEncoding(Transcoder,T::jClassName,cnMemory::ArrayLength(T::jClassName)-1);
						Stream+=cnRTL::ArrayStreamString(u"> Exception:");
						jWriteExceptionDescription<TJavaContext>(Stream,env,Exception);

						jInterface::DeleteLocalRef(env,Exception);
					}
				}
			}
		}
		return cClassFinalization<T>::gInstance.Value;
	}


	template<const char *FieldName,class TClass,class TField>
	static jField<TField>* InstanceField(JNIEnv *env)noexcept{
		static jField<TField> *Value=nullptr;
		if(Value==nullptr){
			auto cls=Class<TClass>(env);
			if(cls==nullptr)
				return nullptr;
			char TypeSignature[JNITypeSignatureWriter<TField>::Length+1];
			auto *p=JNITypeSignatureWriter<TField>::Write(TypeSignature);
			*p=0;
			Value=static_cast<jField<TField>*>(
				jInterface::GetFieldID(env,cls,FieldName,TypeSignature));
			if(Value==nullptr){
				jLogException<TJavaContext>(env);
			}
		}
		return Value;
	}
	
	template<class TClass,const char *FieldName,class TField>
	static jField<TField>* StaticField(JNIEnv *env)noexcept{
		static jField<TField> *Value=nullptr;
		if(Value==nullptr){
			auto cls=Class<TClass>(env);
			if(cls==nullptr)
				return nullptr;
			char TypeSignature[JNITypeSignatureWriter<TField>::Length+1];
			auto *p=JNITypeSignatureWriter<TField>::Write(TypeSignature);
			*p=0;

			Value=static_cast<jField<TField>*>(
				jInterface::GetStaticFieldID(env,cls,FieldName,TypeSignature));
			if(Value==nullptr){
				jLogException<TJavaContext>(env);
			}
		}
		return Value;
	}

	template<const char *FunctionName,class TClass,class TRet,class...TArgs>
	static jMethod<TRet (TClass::*)(TArgs...)>* InstanceMethod(JNIEnv *env)noexcept{
		static jMethod<TRet (TClass::*)(TArgs...)> *Value=nullptr;
		if(Value==nullptr){
			auto cls=Class<TClass>(env);
			if(cls==nullptr)
				return nullptr;
			JNIMethodSignatureGenerator<TRet,TArgs...> SigGen;
			Value=static_cast<jMethod<TRet (TClass::*)(TArgs...)>*>(
				jInterface::GetMethodID(env,cls,FunctionName,SigGen.Value));
			if(Value==nullptr){
				jLogException<TJavaContext>(env);
			}
		}
		return Value;
	}

	template<class TClass,const char *FunctionName,class TRet,class...TArgs>
	static jStaticMethod<TClass,TRet (TArgs...)>* StaticMethod(JNIEnv *env)noexcept{
		static jStaticMethod<TClass,TRet (TArgs...)> *Value=nullptr;
		if(Value==nullptr){
			auto cls=Class<TClass>(env);
			if(cls==nullptr)
				return nullptr;
			JNIMethodSignatureGenerator<TRet,TArgs...> SigGen;
			Value=static_cast<jStaticMethod<TClass,TRet (TArgs...)>*>(
				jInterface::GetStaticMethodID(env,cls,FunctionName,SigGen.Value));
			if(Value==nullptr){
				jLogException<TJavaContext>(env);
			}
		}
		return Value;
	}

	template<class TClass,class...TArgs>
	static jMethod<void (TArgs...)>* ConstructorMethod(JNIEnv *env)noexcept{
		static jMethod<void (TArgs...)> *Value=nullptr;
		if(Value==nullptr){
			auto cls=Class<TClass>(env);
			if(cls==nullptr)
				return nullptr;
			JNIMethodSignatureGenerator<void,TArgs...> SigGen;
			Value=static_cast<jMethod<void (TArgs...)>*>(
				jInterface::GetMethodID(env,cls,"<init>",SigGen.Value));
			if(Value==nullptr){
				jLogException<TJavaContext>(env);
			}
		}
		return Value;
	}

};
//---------------------------------------------------------------------------

template<class TJavaContext>
cJNIInitialization jRegistration<TJavaContext>::Initialization;

template<class TJavaContext>
cJNIFinalization jRegistration<TJavaContext>::Finalization;

template<class TJavaContext>
template<class T>
typename jRegistration<TJavaContext>::template cClassFinalization<T> jRegistration<TJavaContext>::cClassFinalization<T>::gInstance;

//---------------------------------------------------------------------------

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
class JNINativeMethodRegister : private cJNIInitialization::cCallback, private cJNIFinalization::cCallback
{
public:
	JNINativeMethodRegister()noexcept{
		jRegistration<jJavaContext>::Initialization+=this;
	}
	virtual void Initialize(JNIEnv *env)noexcept override{
		auto cls=jRegistration<jJavaContext>::Class<T>(env);
		if(cls==nullptr)
			return;
		cnLib_THelper::JNI::JNINativeMethodGenerator<Func...> ms;
		jInterface::RegisterNatives(env,cls,ms.Value,ms.Length);
		jLogException<jJavaContext>(env);

		{
			auto AutoLock=!jRegistration<jJavaContext>::Finalization;
			jRegistration<jJavaContext>::Finalization+=this;
		}
	}
	virtual void Finalize(JNIEnv *env)noexcept override{
		auto cls=jRegistration<jJavaContext>::Class<T>(env);
		if(cls==nullptr)
			return;
		jInterface::UnregisterNatives(env,cls);
		jLogException<jJavaContext>(env);
	}
};
//---------------------------------------------------------------------------
}	// namespace jCPP
//---------------------------------------------------------------------------
}	// namespace cnLibrary
//---------------------------------------------------------------------------

#endif
