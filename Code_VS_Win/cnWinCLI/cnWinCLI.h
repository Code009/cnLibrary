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


#include <cnTK\cnTK.h>

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
#if _MANAGED
//---------------------------------------------------------------------------
template<class T>
T* mPassMemberPtr(interior_ptr<T> v){	return v;	}
//---------------------------------------------------------------------------
template<class T>
inline T* __clrcall ManagedToPointer(interior_ptr<T> p)
{
	return static_cast<T*>(*reinterpret_cast<void**>(&p));
}
//---------------------------------------------------------------------------
template<class TMemberClass,class TMember>
inline typename TMemberClass* GetObjectFromManagedPointer(interior_ptr<TMember> Pointer,TMember TMemberClass::*MemberPointer)noexcept(true)
{
	typedef const volatile uInt8 TInterpret;
	TInterpret *MemberOffsetPointer=&reinterpret_cast<TInterpret&>(static_cast<TMemberClass*>(nullptr)->*MemberPointer);
	return reinterpret_cast<TMemberClass*>(reinterpret_cast<TInterpret*>(ManagedToPointer(Pointer))-MemberOffsetPointer);
}
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
	cGCHandle():GCHandleStorage{0}{}
#ifdef cnLib_DEBUG
	~cGCHandle(){	cnLib_ASSERT(RefHandle().IsAllocated==false);	}
#endif // cnLib_DEBUG

	__clrcall operator System::Object^(void)const{
		auto &Handle=RefHandle();
		return Handle.Target;
	}

	template<class T>
	T^ __clrcall Cast(void)const{
		auto &Handle=RefHandle();
		return static_cast<T^>(Handle.Target);
	}
	template<class T>
	T^ __clrcall DynamicCast(void)const{
		auto &Handle=RefHandle();
		return dynamic_cast<T^>(Handle.Target);
	}

	System::Runtime::InteropServices::GCHandle& RefHandle(void)const{
		return reinterpret_cast<System::Runtime::InteropServices::GCHandle&>(const_cast<cGCHandle*>(this)->GCHandleStorage);
	}

#endif // _MANAGED

protected:
	void* GCHandleStorage[1];

	void Reset(void){
		for(void* &p : GCHandleStorage){
			p=0;
		}
	}
};
//---------------------------------------------------------------------------
#if _MANAGED
//---------------------------------------------------------------------------
#ifdef cnLib_DEBUG

struct cGCHandleStructure{
	System::Runtime::InteropServices::GCHandle fHandle;
};
static_assert(sizeof(cGCHandle)==sizeof(cGCHandleStructure),"error sizeof(cGCHandle)");
static_assert(alignof(cGCHandle)==alignof(cGCHandleStructure),"error alignof(cGCHandle)");

#endif // cnLib_DEBUG

//---------------------------------------------------------------------------
template<class T>
class mcGCHandleT : public cGCHandle
{
public:

	void __clrcall Alloc(T ^Object){
		auto &Handle=RefHandle();
		cnLib_ASSERT(!Handle.IsAllocated);
		Handle=System::Runtime::InteropServices::GCHandle::Alloc(Object);
	}

	void __clrcall Free(void){
		auto &Handle=RefHandle();
		cnLib_ASSERT(Handle.IsAllocated);
		Handle.Free();
	}

	template<class...VT>
	T^ __clrcall Create(VT...Args){
		auto &Handle=RefHandle();
		cnLib_ASSERT(!Handle.IsAllocated);
		T ^Object=gcnew T(Args...);
		Handle=System::Runtime::InteropServices::GCHandle::Alloc(Object);
		return Object;
	}

	T^ __clrcall Discard(void){
		auto &Handle=RefHandle();
		if(Handle.IsAllocated){
			T ^RetTarget=static_cast<T^>(Handle.Target);
			Handle.Free();
			return RetTarget;
		}
		return nullptr;
	}

	void __clrcall Store(T^ Object){
		auto &Handle=RefHandle();
		if(Handle.IsAllocated){
			Handle.Target=Object;
		}
		else{
			Handle=System::Runtime::InteropServices::GCHandle::Alloc(Object);
		}
	}

	T^ __clrcall Get(void)const{
		auto &Handle=RefHandle();
		cnLib_ASSERT(Handle.IsAllocated);
		return static_cast<T^>(Handle.Target);
	}

	void __clrcall Set(T ^Object){
		auto &Handle=RefHandle();
		cnLib_ASSERT(Handle.IsAllocated);
		return Handle.Target=Object;
	}

	__clrcall operator T^(void)const{	return Get();	}
	void __clrcall operator =(T^ Object){	return Set(Object);	}
};
//---------------------------------------------------------------------------
#endif // _MANAGED
//---------------------------------------------------------------------------
template<eGCHandleType RefType>
class cGCReference : public cGCHandle
{
#if _MANAGED
public:

	cGCReference(){
		auto &Handle=RefHandle();
		Handle=System::Runtime::InteropServices::GCHandle::Alloc(nullptr,static_cast<System::Runtime::InteropServices::GCHandleType>(RefType));
	}

	~cGCReference(){
		auto &Handle=RefHandle();
		Handle.Free();
	}

	cGCReference(const cGCReference &Src){
		auto &Handle=RefHandle();
		Handle=System::Runtime::InteropServices::GCHandle::Alloc(Src,static_cast<System::Runtime::InteropServices::GCHandleType>(RefType));
	}

	cGCReference(cGCReference &&Src){
		auto &Handle=RefHandle();
		auto &SrcHandle=Src.RefHandle();
		Handle=SrcHandle;
		SrcHandle=System::Runtime::InteropServices::GCHandle::Alloc(nullptr,static_cast<System::Runtime::InteropServices::GCHandleType>(RefType));
	}

	__clrcall cGCReference(System::Object ^Object){
		auto &Handle=RefHandle();
		Handle=System::Runtime::InteropServices::GCHandle::Alloc(Object,static_cast<System::Runtime::InteropServices::GCHandleType>(RefType));
	}
	// _MANAGED
#else
	// !_MANAGED
protected:
	cGCReference();
	~cGCReference();
#endif // !_MANAGED

};
//---------------------------------------------------------------------------
typedef cGCReference<eGCHandleType::Normal> cGCRef;
typedef cGCReference<eGCHandleType::Weak> cGCWeakRef;
//---------------------------------------------------------------------------
static_assert(sizeof(cGCRef)==sizeof(cGCHandle),"error sizeof(cGCReference)");
//---------------------------------------------------------------------------
#if _MANAGED
//---------------------------------------------------------------------------
template<class TManaged,eGCHandleType RefType>
class mcGCReferenceT : public cGCReference<RefType>
{
public:

	__clrcall mcGCReferenceT(TManaged ^Object=nullptr)	: cGCRef(Object){}

	__clrcall mcGCReferenceT(const mcGCReferenceT &Src)	: cGCRef(Src){}
	__clrcall mcGCReferenceT(mcGCReferenceT &&Src)		: cGCRef(static_cast<mcGCReferenceT&&>(Src)){}

	template<class TManagedSrc>
	__clrcall mcGCReferenceT(mcGCReferenceT<TManagedSrc,RefType> &&Src)
		: cGCRef(static_cast<mcGCReferenceT<TManagedSrc,RefType>&&>(Src))
	{
		static_cast<TManaged^>(static_cast<TManagedSrc^>(Src));
	}

	__clrcall operator TManaged^(void)const{
		auto &Handle=RefHandle();
		return static_cast<TManaged^>(Handle.Target);
	}
	void __clrcall operator =(TManaged^ Object){
		auto &Handle=RefHandle();
		Handle.Target=Object;
	}

	TManaged^ __clrcall Get(void)const{
		auto &Handle=RefHandle();
		return static_cast<TManaged^>(Handle.Target);
	}

	void __clrcall Set(TManaged^ Target){
		auto &Handle=RefHandle();
		Handle.Target=Target;
	}

};

//---------------------------------------------------------------------------
template<class T>	using mcGCRefT=mcGCReferenceT<T,eGCHandleType::Normal> ;
template<class T>	using mcGCWeakRefT=mcGCReferenceT<T,eGCHandleType::Weak> ;

static_assert(sizeof(cGCRef)==sizeof(mcGCRefT<System::Object>),"error sizeof(mcGCReferenceT)");

//---------------------------------------------------------------------------
#endif	// _MANAGED
//---------------------------------------------------------------------------
template<class TManaged,uIntn Size>
class cManagedValueStruct
{
	uIntn _space_[(Size+sizeof(uIntn)+1)/sizeof(uIntn)];
public:
#if _MANAGED

#ifdef cnLib_DEBUG


	struct cSize{
		TManaged m;
	};

	static_assert(sizeof(cSize)<=sizeof(cManagedValueStruct::_space_),"not enough size");
	static_assert(sizeof(cSize)+sizeof(void*)*4>sizeof(cManagedValueStruct::_space_),"size too large");

#endif // cnLib_DEBUG

	void __clrcall Initialize(void){
		*reinterpret_cast<TManaged*>(this)=TManaged();
	}

	TManaged% operator *()noexcept(true){			return *reinterpret_cast<TManaged*>	(this);	}
	TManaged% operator *()const noexcept(true){		return *reinterpret_cast<TManaged*>	(const_cast<cManagedValueStruct*>(this));	}
	TManaged* operator ->()noexcept(true){			return reinterpret_cast<TManaged*>	(this);	}
	TManaged* operator ->()const noexcept(true){	return reinterpret_cast<TManaged*>	(const_cast<cManagedValueStruct*>(this));	}

#endif // _MANAGED
};
//---------------------------------------------------------------------------
#if _MANAGED
#define	cnCLI_MANAGED_VALUE_STRUCT(__TManaged__,__PointerCount__,__Size__)	cManagedValueStruct<__TManaged__,sizeof(void*)*(__PointerCount__)+(__Size__)>
#else
#define	cnCLI_MANAGED_VALUE_STRUCT(__TManaged__,__PointerCount__,__Size__)	cManagedValueStruct<void,sizeof(void*)*(__PointerCount__)+(__Size__)>
#endif // _MANAGED
//---------------------------------------------------------------------------
#if _MANAGED
//---------------------------------------------------------------------------
template<class TMemberClass,class TManagedStruct,uIntn Size>
inline TMemberClass* GetObjectFromManagedValueStruct(TManagedStruct *Pointer,cManagedValueStruct<TManagedStruct,Size> TMemberClass::*MemberPointer)noexcept(true)
{
	typedef const volatile uInt8 TInterpret;
	TInterpret *MemberOffsetPointer=&reinterpret_cast<TInterpret&>(static_cast<TMemberClass*>(nullptr)->*MemberPointer);
	return reinterpret_cast<TMemberClass*>(reinterpret_cast<TInterpret*>(Pointer)-MemberOffsetPointer);
}
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
