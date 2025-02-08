#include <cnRTL/Logger.h>

#include "JNIRegistration.h"
#include "JNICPP.hpp"


using namespace cnLibrary;
using namespace cnRTL;
using namespace jCPP;

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void cJNIInitialization::operator +=(cCallback *Initialization)noexcept(true)
{
	if(Initialization==nullptr)
		return;

	if(fTail!=nullptr){
		fTail->Next=Initialization;
	}
	else{
		fHead=Initialization;
	}
	Initialization->Next=nullptr;
	fTail=Initialization;
}
//---------------------------------------------------------------------------
void cJNIInitialization::operator ()(JNIEnv *env)const noexcept(true)
{
	cCallback *Callback=fHead;
	while(Callback!=nullptr){
		auto Current=Callback;
		Callback=Callback->Next;
		Current->Initialize(env);
	}
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cJNIFinalization::cJNIFinalization()noexcept(true)
	: fMutex(cnSystem::CreateRecursiveLock())
{
}
//---------------------------------------------------------------------------
cJNIFinalization::~cJNIFinalization()noexcept(true)
{
}
//---------------------------------------------------------------------------
void cJNIFinalization::operator +=(cCallback *Finalization)noexcept(true)
{
	if(Finalization==nullptr)
		return;

	Finalization->Next=fTop;
	fTop=Finalization;
}
//---------------------------------------------------------------------------
void cJNIFinalization::operator ()(JNIEnv *env)noexcept(true)
{
	cCallback *Callback=fTop;
	fTop=nullptr;

	while(Callback!=nullptr){
		auto Current=Callback;
		Callback=Callback->Next;
		Current->Finalize(env);
	}
}
//---------------------------------------------------------------------------
lockPtr<iMutex> cJNIFinalization::operator !(void)const noexcept(true)
{
	return fMutex;
}
//---------------------------------------------------------------------------
