/*- cnWinCLI --------------------------------------------------------------*/
/*         Developer : Code009                                             */
/*         Create on : 2020-04-14                                          */
/*-------------------------------------------------------------------------*/
#pragma once
/*-------------------------------------------------------------------------*/

#if _MANAGED

#using <mscorlib.dll>
#using <System.dll>

#endif	/* _MANAGED */


#include <cnTK\Memory.hpp>
#include <cnTK\Interface.hpp>
#include <cnSystem\cnFundamental.h>

#if _MANAGED

#ifdef interface
#undef	interface
#endif // interface

#endif	/* _MANAGED */

#ifdef __cplusplus
//---------------------------------------------------------------------------
namespace cnLibrary{
//---------------------------------------------------------------------------
namespace cnWin{
//---------------------------------------------------------------------------
//TManagedClass
//{
//	static constexpr uIntn ManagedSize;
//}
//---------------------------------------------------------------------------
template<class TManagedClass>
class cManagedStruct : public TManagedClass, cnMemory::cPlainAlignedData<TManagedClass::ManagedSize-sizeof(TManagedClass),uIntn>
{
};
//---------------------------------------------------------------------------
#if _MANAGED
//---------------------------------------------------------------------------
template<class TManagedClass>
struct TManagedStructCheck
	: cnVar::TConstantValueBool<(TManagedClass::ManagedSize>=sizeof(TManagedClass))>
{
};
//---------------------------------------------------------------------------
template<class T>
T* mPassMemberPtr(interior_ptr<T> v)noexcept(true){	return v;	}
//---------------------------------------------------------------------------
template<class T>
inline T* __clrcall ManagedToPointer(interior_ptr<T> p)noexcept(true)
{
	return static_cast<T*>(*reinterpret_cast<void**>(&p));
}
//---------------------------------------------------------------------------
//template<class TMemberClass,class TMember>
//inline TMemberClass* GetObjectFromManagedPointer(interior_ptr<TMember> Pointer,TMember TMemberClass::*MemberPointer)noexcept(true)
//{
//	typedef const volatile uInt8 TInterpret;
//	TInterpret *MemberOffsetPointer=&reinterpret_cast<TInterpret&>(static_cast<TMemberClass*>(nullptr)->*MemberPointer);
//	return reinterpret_cast<TMemberClass*>(reinterpret_cast<TInterpret*>(ManagedToPointer(Pointer))-MemberOffsetPointer);
//}
//---------------------------------------------------------------------------
#endif	// _MANAGED
//---------------------------------------------------------------------------
enum class eGCHandleType : ufInt8
{
	Weak			=0,
	WeakTrackResurrection=1,
	Normal			=2,
	Pinned			=3,
};
//---------------------------------------------------------------------------
class cGCHandle
{
#if _MANAGED
public:
#ifdef cnLib_DEBUG
	~cGCHandle()noexcept(true){
		cnLib_ASSERT(fHandle.IsAllocated==false);
	}
#endif // cnLib_DEBUG

	__clrcall operator System::Object^(void)const noexcept(true){
		return const_cast<cGCHandle*>(this)->fHandle.Target;
	}

	template<class T>
	T^ __clrcall Cast(void)const noexcept(true){
		return static_cast<T^>(const_cast<cGCHandle*>(this)->fHandle.Target);
	}
	template<class T>
	T^ __clrcall DynamicCast(void)const noexcept(true){
		return dynamic_cast<T^>(const_cast<cGCHandle*>(this)->fHandle.Target);
	}

	void __clrcall Clear(void)noexcept(true){
		fHandle.Target=nullptr;
	}

	void __clrcall Alloc(System::Object ^Object,eGCHandleType RefType=eGCHandleType::Normal){
		fHandle=System::Runtime::InteropServices::GCHandle::Alloc(Object,static_cast<System::Runtime::InteropServices::GCHandleType>(RefType));
	}

	void __clrcall Free(void){
		fHandle.Free();
	}

	bool __clrcall IsAllocated(void)const{
		return const_cast<cGCHandle*>(this)->fHandle.IsAllocated;
	}
	void __clrcall SafeFree(void){
		if(fHandle.IsAllocated)
			fHandle.Free();
	}

protected:
	System::Runtime::InteropServices::GCHandle fHandle;

#else
protected:
	void* GCHandleStorage;
#endif
};
//---------------------------------------------------------------------------
#if _MANAGED
//---------------------------------------------------------------------------
static_assert(sizeof(void*)>=sizeof(cGCHandle),"incorrect GCHandle struct size");
//---------------------------------------------------------------------------
template<class TManaged,eGCHandleType RefType=eGCHandleType::Normal>
class mcGCHandle : public cGCHandle
{
public:

	void __clrcall Alloc(TManaged ^Object){
		fHandle=System::Runtime::InteropServices::GCHandle::Alloc(Object,static_cast<System::Runtime::InteropServices::GCHandleType>(RefType));
	}

	TManaged^ __clrcall FetchFree(void){
		TManaged ^Ret=static_cast<TManaged^>(fHandle.Target);
		fHandle.Free();
		return Ret;
	}

	TManaged^ __clrcall Get(void)const{
		return static_cast<TManaged^>(const_cast<mcGCHandle*>(this)->fHandle.Target);
	}

	void __clrcall Set(TManaged^ Target){
		fHandle.Target=Target;
	}
};
//---------------------------------------------------------------------------

#if cnLibrary_CPPFEATURE_VARIABLE_TEMPLATES >= 201304L

template<class T>
cnVar::cStaticVariable<T> ModuleInitializationVariable;

template<class T>
private ref class ModuleInitialization
{
public:
	static void Load(void){
		gInstance=gcnew ModuleInitialization();

        System::AppDomain::CurrentDomain->DomainUnload += ehUnload;
        System::AppDomain::CurrentDomain->ProcessExit  += ehUnload;
	}

private:
	initonly static System::EventHandler^ ehUnload = gcnew System::EventHandler(&ModuleInitialization::Unload);

	ModuleInitialization(){
		ModuleInitializationVariable<T>.Construct();
	}
	~ModuleInitialization(){
		this->!ModuleInitialization();
	}
	!ModuleInitialization(){
		ModuleInitializationVariable<T>.Destruct();
	}
	static ModuleInitialization ^gInstance;
	//T *Module;
	static void Unload(System::Object^, System::EventArgs^)
	{

        System::AppDomain::CurrentDomain->DomainUnload -= ehUnload;
        System::AppDomain::CurrentDomain->ProcessExit  -= ehUnload;
	
		gInstance=nullptr;
	}
};
#endif // cnLibrary_CPPFEATURE_VARIABLE_TEMPLATES >= 201304L

//---------------------------------------------------------------------------
#endif	// _MANAGED
//---------------------------------------------------------------------------
}	// namespace cnWin
//---------------------------------------------------------------------------
}	// namespace cnLibrary
//---------------------------------------------------------------------------
#endif  /* __cplusplus */
