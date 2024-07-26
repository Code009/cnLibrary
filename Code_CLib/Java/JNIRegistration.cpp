#include <cnRTL/Logger.h>

#include "JNIRegistration.h"
#include "JNICPP.hpp"


using namespace cnLibrary;
using namespace cnRTL;
using namespace jCPP;


//---------------------------------------------------------------------------
void jCPP::jLogException(JNIEnv *env,jcThrowable *Exception)noexcept
{
	// clear exception in order to read exception message
	jInterface::ExceptionDescribe(env);
	jInterface::ExceptionClear(env);
	// read exception message
	auto ExceptionString=Exception->toString(env);
	if(ExceptionString!=nullptr){
		// failed
		auto StrLength=ExceptionString->length(env);
		auto StrAccess=ExceptionString->AccessCritical(env);
		{
			auto Stream=gRTLLog.MakeLogBuffer<1>(u"cnLibrary/jCPP");

			Stream+=ArrayStreamArray(StrAccess.Pointer,StrLength);
		}
	}
	else{
		jInterface::ExceptionDescribe(env);
		jInterface::ExceptionClear(env);
	}
}
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
void cJNIInitializationRegistration::Initialize(JNIEnv *env)noexcept
{
	cInitialization *Component=fHead;
	if(Component==nullptr)
		return;

	fInitialized=fTail;
	while(Component!=fInitialized){
		Component->Initialize(env);
		Component=Component->Next;
	}
	fInitialized->Initialize(env);
}
//---------------------------------------------------------------------------
void cJNIInitializationRegistration::Finalize(JNIEnv *env)noexcept
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
