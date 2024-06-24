#include "JNICPP.h"

using namespace cnLibrary;
using namespace jCPP;


//---------------------------------------------------------------------------
cJNIModule cJNIModule::gJNIModule;
cJNIInitializationRegistration cJNIModule::gRegistration;
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void cJNIInitializationRegistration::Register(cInitialization *Component)noexcept
{
	if(Component==nullptr)
		return;

	if(fTail!=nullptr){
		fTail->Next=Component;
	}
	else{
		fHead=Component;
	}
	Component->Prev=fTail;
	Component->Next=nullptr;
	fTail=Component;
}
//---------------------------------------------------------------------------
void cJNIInitializationRegistration::Initialize(jEnv *env)noexcept
{
	cInitialization *Component=fHead;
	if(Component==nullptr)
		return;

	fInitialized=fTail;
	do{
		Component->Initialize(env);
		Component=Component->Next;
	}while(Component!=fInitialized);
}
//---------------------------------------------------------------------------
void cJNIInitializationRegistration::Finalize(jEnv *env)noexcept
{
	auto Components=fInitialized;
	fInitialized=nullptr;
	while(Components!=nullptr){
		auto CurComponent=Components;
		Components=Components->Prev;
		
		CurComponent->Finalize(env);
	}
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
bcJNIInitialization::bcJNIInitialization()noexcept
{
	cJNIModule::gRegistration.Register(this);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
const cJNIModule::cVar* cJNIModule::operator ->()const noexcept
{
	return &fVar;
}
//---------------------------------------------------------------------------
void cJNIModule::NativeActivityCreated(jEnv *env,jVM *vm)noexcept
{
	if(fVar.vm!=nullptr)
		return;

	fVar.vm=vm;
	return gRegistration.Initialize(env);
}
//---------------------------------------------------------------------------
void cJNIModule::Initialize(jEnv *env)noexcept
{
	env->GetJavaVM(fVar.vm);
	return gRegistration.Initialize(env);
}
//---------------------------------------------------------------------------
void cJNIModule::Finalize(jEnv *env)noexcept
{
	return gRegistration.Finalize(env);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
jcEnv::jcEnv()noexcept
{
	auto ret=cJNIModule::gJNIModule->vm->GetEnv(fEnv,JNI_VERSION_1_6);
	if(ret==JNI_OK){
		fNeedDetach=false;
		return;
	}
	if(ret==JNI_EDETACHED){
		ret=cJNIModule::gJNIModule->vm->AttachCurrentThread(fEnv,nullptr);
		if(ret==JNI_OK){
			fNeedDetach=true;
			return;
		}
	}
	fEnv=nullptr;
	fNeedDetach=false;
}
//---------------------------------------------------------------------------
jcEnv::~jcEnv()noexcept
{
	if(fNeedDetach){
		cJNIModule::gJNIModule->vm->DetachCurrentThread();
	}
}
//---------------------------------------------------------------------------
jcEnv::operator jEnv*()const noexcept
{
	return fEnv;
}
//---------------------------------------------------------------------------
jEnv* jcEnv::operator ->()const noexcept
{
	return fEnv;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
template<>	struct JNIMemberName<&jcObject::getClass>{	static constexpr const char Value[]="getClass";	};
template<>	struct JNIMemberName<&jcObject::toString>{	static constexpr const char Value[]="toString";	};
//---------------------------------------------------------------------------
jrLocal<jcClass> jcObject::getClass(jEnv *env)noexcept
{
	return RefLocal(env,env->Call(this,TInstanceMethod<&jcObject::getClass>::Value));
}
//---------------------------------------------------------------------------
jrLocal<jcString> jcObject::toString(jEnv *env)noexcept
{
	return RefLocal(env,env->Call(this,TInstanceMethod<&jcObject::toString>::Value));
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
jStringAccessCritical::jStringAccessCritical(jEnv *env,jcString *s)noexcept
	: env(env)
	, StringRef(s)
	, Pointer(reinterpret_cast<const uChar16*>(env->GetStringCritical(s,reinterpret_cast<jboolean*>(&IsCopy))))
{
}
//---------------------------------------------------------------------------
jStringAccessCritical::~jStringAccessCritical()noexcept
{
	if(Pointer!=nullptr){
		env->ReleaseStringCritical(StringRef,reinterpret_cast<const jchar*>(Pointer));
	}
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
jrLocal<jcString> jcString::Make(jEnv *env,const uChar16 *Text,uIntn Length)noexcept
{
	return RefLocal(env,env->NewString(reinterpret_cast<const jchar*>(Text),Length));
}
//---------------------------------------------------------------------------
jsize jcString::length(jEnv *env)noexcept
{
	return env->GetStringLength(this);
}
//---------------------------------------------------------------------------
jStringAccessCritical jcString::AccessCritical(jEnv *env)noexcept
{
	return {env,this};
}
//---------------------------------------------------------------------------
jsize jbcArray::GetArrayLength(jEnv *env)noexcept
{
	return env->GetArrayLength(this);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
jrLocal< jcArray<jbyte> > jcArray<jbyte>::Make(jEnv *env,jsize length)noexcept
{
	return RefLocal(env,env->NewByteArray(length));
}
//---------------------------------------------------------------------------
jArrayAccess<jbyte> jcArray<jbyte>::AccessElements(jEnv *env)noexcept
{
	return jArrayAccess<jbyte>(env,this);
}
//---------------------------------------------------------------------------
void jcArray<jbyte>::GetRegion(jEnv *env,jsize start,jsize len,jbyte *buf)noexcept
{
	 return env->GetByteArrayRegion(this,start,len,buf);
}
//---------------------------------------------------------------------------
void jcArray<jbyte>::SetRegion(jEnv *env,jsize start,jsize len,const jbyte *buf)noexcept
{
	return env->SetByteArrayRegion(this,start,len,buf);
}
//---------------------------------------------------------------------------
