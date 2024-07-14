#include "JNIRegistration.h"

using namespace cnLibrary;
using namespace jCPP;


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
