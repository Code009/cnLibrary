/*- cnTK - Functor --------------------------------------------------------*/
/*         Developer : Code009                                             */
/*         Create on : 2006-01-17                                          */
/*-------------------------------------------------------------------------*/
#ifndef __cnLibrary_cnTK_Function_HPP__
#define	__cnLibrary_cnTK_Function_HPP__
/*-------------------------------------------------------------------------*/
#include <cnTK/Common.hpp>
#include <cnTK/TypeTraits.hpp>
#include <cnTK/Interface.hpp>
#include <cnTK/Memory.hpp>
/*-------------------------------------------------------------------------*/
#if	cnLibrary_CPPFEATURELEVEL >= 3
//---------------------------------------------------------------------------
#include <cnTK/TKMacrosDeclare.inc>
//---------------------------------------------------------------------------
namespace cnLibrary{
//---------------------------------------------------------------------------
namespace cnVar{
//---------------------------------------------------------------------------
	
template<class TFunction>
class iFunctorStorage : public iFunction<TFunction>
{
public:
	virtual void Destruct(void)noexcept(true)=0;
	virtual bool IsNull(void)const noexcept(true)=0;
	virtual uIntn SizeOf(void)const noexcept(true)=0;
	virtual void CopyTo(void *Dest)const noexcept(true)=0;
	virtual void MoveTo(void *Dest)noexcept(true)=0;
	virtual uIntn InplaceSizeOf(void)const noexcept(true)=0;
	virtual void CopyToInplace(void *Dest)const noexcept(true)=0;
	virtual void MoveToInplace(void *Dest)noexcept(true)=0;
};


//---------------------------------------------------------------------------
}	// namespace cnVar
//---------------------------------------------------------------------------
}	// namespace cnLibrary
//---------------------------------------------------------------------------
namespace cnLib_THelper{
namespace Var_TH{
//---------------------------------------------------------------------------
	
template<class TEnable,class TFunction,class TFunctor>
struct Function_FunctorIsCallable
	: cnVar::TConstantValueFalse{};

template<class TRet,class...TArgs,class TFunctor>
struct Function_FunctorIsCallable<decltype(cnVar::DeclVal<TFunctor>()(cnVar::DeclVal<TArgs>()...)),TRet(TArgs...),TFunctor>
	: cnVar::TConstantValueTrue{};

#if cnLibrary_CPPFEATURE_NOEXCEPT_FUNC_TYPE >= 201510L
template<class TRet,class...TArgs,class TFunctor>
struct Function_FunctorIsCallable<decltype(cnVar::DeclVal<TFunctor>()(cnVar::DeclVal<TArgs>()...)),TRet(TArgs...)noexcept,TFunctor>
	: cnVar::TConstantValueTrue{};
#endif // cnLibrary_CPPFEATURE_NOEXCEPT_FUNC_TYPE >= 201510L
//---------------------------------------------------------------------------
}	// namespace Var_TH
}	// namespace cnLib_THelper
//---------------------------------------------------------------------------
namespace cnLibrary{
//---------------------------------------------------------------------------
namespace cnVar{
//---------------------------------------------------------------------------

	template<class TFunction>	class cFunctionInterfaceStorage;

	template<class TRet,class...TArgs>
	class cFunctionInterfaceStorage<TRet (TArgs...)> : public iFunctorStorage<TRet (TArgs...)>
	{
	private:
		iFunction<TRet (TArgs...)> *fPtr;
	public:

		cFunctionInterfaceStorage(iFunction<TRet (TArgs...)> *Ptr)noexcept(true):fPtr(Ptr){}

		virtual TRet cnLib_FUNC Execute(TArgs...Args)override{
			return fPtr->Execute(static_cast<TArgs&&>(Args)...);
		}
		virtual void Destruct(void)noexcept(true) override{}
		virtual bool IsNull(void)const noexcept(true) override{	return fPtr==nullptr;		}
		virtual uIntn SizeOf(void)const noexcept(true)override{	return sizeof(cFunctionInterfaceStorage);	}
		virtual void CopyTo(void *Dest)const noexcept(true) override{
			cnClass::ManualConstruct(*static_cast<cFunctionInterfaceStorage*>(Dest),fPtr);
		}
		virtual void MoveTo(void *Dest)noexcept(true) override{
			cnClass::ManualConstruct(*static_cast<cFunctionInterfaceStorage*>(Dest),fPtr);
		}
		virtual uIntn InplaceSizeOf(void)const noexcept(true)override{	return sizeof(cFunctionInterfaceStorage);	}
		virtual void CopyToInplace(void *Dest)const noexcept(true) override{
			cnClass::ManualConstruct(*static_cast<cFunctionInterfaceStorage*>(Dest),fPtr);
		}
		virtual void MoveToInplace(void *Dest)noexcept(true) override{
			cnClass::ManualConstruct(*static_cast<cFunctionInterfaceStorage*>(Dest),fPtr);
		}
	};

#if cnLibrary_CPPFEATURE_NOEXCEPT_FUNC_TYPE >= 201510L

	template<class TRet,class...TArgs>
	class cFunctionInterfaceStorage<TRet (TArgs...)noexcept> : public iFunctorStorage<TRet (TArgs...)noexcept>
	{
	private:
		iFunction<TRet (TArgs...)noexcept> *fPtr;
	public:

		cFunctionInterfaceStorage(iFunction<TRet (TArgs...)noexcept> *Ptr)noexcept:fPtr(Ptr){}

		virtual TRet cnLib_FUNC Execute(TArgs...Args)noexcept override{
			return fPtr->Execute(static_cast<TArgs&&>(Args)...);
		}
		virtual void Destruct(void)noexcept override{}
		virtual bool IsNull(void)const noexcept override{	return fPtr==nullptr;		}
		virtual uIntn SizeOf(void)const noexcept(true)override{	return sizeof(cFunctionInterfaceStorage);	}
		virtual void CopyTo(void *Dest)const noexcept override{
			cnClass::ManualConstruct(*static_cast<cFunctionInterfaceStorage*>(Dest),fPtr);
		}
		virtual void MoveTo(void *Dest)noexcept override{
			cnClass::ManualConstruct(*static_cast<cFunctionInterfaceStorage*>(Dest),fPtr);
		}
		virtual uIntn InplaceSizeOf(void)const noexcept(true)override{	return sizeof(cFunctionInterfaceStorage);	}
		virtual void CopyToInplace(void *Dest)const noexcept(true) override{
			cnClass::ManualConstruct(*static_cast<cFunctionInterfaceStorage*>(Dest),fPtr);
		}
		virtual void MoveToInplace(void *Dest)noexcept(true) override{
			cnClass::ManualConstruct(*static_cast<cFunctionInterfaceStorage*>(Dest),fPtr);
		}
	};

#endif // cnLibrary_CPPFEATURE_NOEXCEPT_FUNC_TYPE >= 201510L


template<class TFunction>
class cFunctionPtrStorage;

template<class TRet,class...TArgs>
class cFunctionPtrStorage<TRet (TArgs...)> : public iFunctorStorage<TRet (TArgs...)>
{
private:
	typedef TRet tFunction(TArgs...);
	tFunction *fPtr;
public:
	cFunctionPtrStorage(tFunction *Ptr)noexcept(true):fPtr(Ptr){}

	virtual TRet cnLib_FUNC Execute(TArgs...Args)override{
		return (*fPtr)(static_cast<TArgs&&>(Args)...);
	}
	virtual void Destruct(void)noexcept(true) override{}
	virtual bool IsNull(void)const noexcept(true) override{			return fPtr==nullptr;		}
	virtual uIntn SizeOf(void)const noexcept(true)override{	return sizeof(cFunctionPtrStorage);	}
	virtual void CopyTo(void *Dest)const noexcept(true) override{
		cnClass::ManualConstruct(*static_cast<cFunctionPtrStorage*>(Dest),fPtr);
	}
	virtual void MoveTo(void *Dest)noexcept(true) override{
		cnClass::ManualConstruct(*static_cast<cFunctionPtrStorage*>(Dest),fPtr);
	}
	virtual uIntn InplaceSizeOf(void)const noexcept(true)override{	return sizeof(cFunctionPtrStorage);	}
	virtual void CopyToInplace(void *Dest)const noexcept(true) override{
		cnClass::ManualConstruct(*static_cast<cFunctionPtrStorage*>(Dest),fPtr);
	}
	virtual void MoveToInplace(void *Dest)noexcept(true) override{
		cnClass::ManualConstruct(*static_cast<cFunctionPtrStorage*>(Dest),fPtr);
	}
};

#if cnLibrary_CPPFEATURE_NOEXCEPT_FUNC_TYPE >= 201510L

template<class TRet,class...TArgs>
class cFunctionPtrStorage<TRet (TArgs...)noexcept> : public iFunctorStorage<TRet (TArgs...)noexcept>
{
private:
	typedef TRet tFunction(TArgs...)noexcept;
	tFunction *fPtr;
public:
	cFunctionPtrStorage(tFunction *Ptr)noexcept:fPtr(Ptr){}

	virtual TRet cnLib_FUNC Execute(TArgs...Args)noexcept override{
		return (*fPtr)(static_cast<TArgs&&>(Args)...);
	}
	virtual void Destruct(void)noexcept override{}
	virtual bool IsNull(void)const noexcept override{			return fPtr==nullptr;		}
	virtual uIntn SizeOf(void)const noexcept(true)override{	return sizeof(cFunctionPtrStorage);	}
	virtual void CopyTo(void *Dest)const noexcept override{
		cnClass::ManualConstruct(*static_cast<cFunctionPtrStorage*>(Dest),fPtr);
	}
	virtual void MoveTo(void *Dest)noexcept override{
		cnClass::ManualConstruct(*static_cast<cFunctionPtrStorage*>(Dest),fPtr);
	}
	virtual uIntn InplaceSizeOf(void)const noexcept(true)override{	return sizeof(cFunctionPtrStorage);	}
	virtual void CopyToInplace(void *Dest)const noexcept(true) override{
		cnClass::ManualConstruct(*static_cast<cFunctionPtrStorage*>(Dest),fPtr);
	}
	virtual void MoveToInplace(void *Dest)noexcept(true) override{
		cnClass::ManualConstruct(*static_cast<cFunctionPtrStorage*>(Dest),fPtr);
	}
};

#endif // cnLibrary_CPPFEATURE_NOEXCEPT_FUNC_TYPE >= 201510L


template<class TFunction,class TFunctor>	class cFunctorInplaceStorage;

template<class TRet,class...TArgs,class TFunctor>
class cFunctorInplaceStorage<TRet (TArgs...),TFunctor> : public iFunctorStorage<TRet (TArgs...)>, private TFunctor
{
public:
	cFunctorInplaceStorage(const TFunctor &Functor)noexcept(true):TFunctor(Functor){}
	cFunctorInplaceStorage(TFunctor &&Functor)noexcept(true):TFunctor(static_cast<TFunctor&&>(Functor)){}
	~cFunctorInplaceStorage()noexcept(true){}
	virtual TRet cnLib_FUNC Execute(TArgs...Args)override{
		return (*this)(static_cast<TArgs&&>(Args)...);
	}
	virtual void Destruct(void)noexcept(true) override{	cnClass::ManualDestruct(*this);	}
	virtual bool IsNull(void)const noexcept(true) override{			return false;			}
	virtual uIntn SizeOf(void)const noexcept(true)override{	return sizeof(cFunctorInplaceStorage);	}
	virtual void CopyTo(void *Dest)const noexcept(true) override{
		cnClass::ManualConstruct(*static_cast<cFunctorInplaceStorage*>(Dest),static_cast<const TFunctor&>(*this));
	}
	virtual void MoveTo(void *Dest)noexcept(true) override{
		cnClass::ManualConstruct(*static_cast<cFunctorInplaceStorage*>(Dest),static_cast<TFunctor&&>(*this));
	}
	virtual uIntn InplaceSizeOf(void)const noexcept(true)override{	return sizeof(cFunctorInplaceStorage);	}
	virtual void CopyToInplace(void *Dest)const noexcept(true) override{
		cnClass::ManualConstruct(*static_cast<cFunctorInplaceStorage*>(Dest),static_cast<const TFunctor&>(*this));
	}
	virtual void MoveToInplace(void *Dest)noexcept(true) override{
		cnClass::ManualConstruct(*static_cast<cFunctorInplaceStorage*>(Dest),static_cast<TFunctor&&>(*this));
	}
};

#if cnLibrary_CPPFEATURE_NOEXCEPT_FUNC_TYPE >= 201510L

template<class TRet,class...TArgs,class TFunctor>
class cFunctorInplaceStorage<TRet (TArgs...)noexcept,TFunctor> : public iFunctorStorage<TRet (TArgs...)noexcept>, private TFunctor
{
public:
	cFunctorInplaceStorage(const TFunctor &Functor)noexcept:TFunctor(Functor){}
	cFunctorInplaceStorage(TFunctor &&Functor)noexcept:TFunctor(static_cast<TFunctor&&>(Functor)){}
	~cFunctorInplaceStorage()noexcept{}
	virtual TRet cnLib_FUNC Execute(TArgs...Args)noexcept override{
		return (*this)(static_cast<TArgs&&>(Args)...);
	}
	virtual void Destruct(void)noexcept override{	cnClass::ManualDestruct(*this);	}
	virtual bool IsNull(void)const noexcept override{			return false;			}
	virtual uIntn SizeOf(void)const noexcept(true)override{	return sizeof(cFunctorInplaceStorage);	}
	virtual void CopyTo(void *Dest)const noexcept override{
		cnClass::ManualConstruct(*static_cast<cFunctorInplaceStorage*>(Dest),static_cast<const TFunctor&>(*this));
	}
	virtual void MoveTo(void *Dest)noexcept override{
		cnClass::ManualConstruct(*static_cast<cFunctorInplaceStorage*>(Dest),static_cast<TFunctor&&>(*this));
	}
	virtual uIntn InplaceSizeOf(void)const noexcept(true)override{	return sizeof(cFunctorInplaceStorage);	}
	virtual void CopyToInplace(void *Dest)const noexcept(true) override{
		cnClass::ManualConstruct(*static_cast<cFunctorInplaceStorage*>(Dest),static_cast<const TFunctor&>(*this));
	}
	virtual void MoveToInplace(void *Dest)noexcept(true) override{
		cnClass::ManualConstruct(*static_cast<cFunctorInplaceStorage*>(Dest),static_cast<TFunctor&&>(*this));
	}
};

#endif	// cnLibrary_CPPFEATURE_NOEXCEPT_FUNC_TYPE >= 201510L


template<class TFunction,class TFunctor,class TAllocationOperator>	class cFunctorAllocationStorage;

template<class TRet,class...TArgs,class TFunctor,class TAllocationOperator>
class cFunctorAllocationStorage<TRet (TArgs...),TFunctor,TAllocationOperator> : public iFunctorStorage<TRet (TArgs...)>
{
private:
	TFunctor *fFunctor;
public:
	cFunctorAllocationStorage(const TFunctor &Functor)noexcept(true)
		: fFunctor(static_cast<TFunctor*>(TAllocationOperator::Allocate(sizeof(TFunctor),cnMemory::TAlignmentOf<TFunctor>::Value)))
	{
		cnClass::ManualConstruct(*fFunctor,Functor);
	}
	cFunctorAllocationStorage(TFunctor &&Functor)noexcept
		: fFunctor(static_cast<TFunctor*>(TAllocationOperator::Allocate(sizeof(TFunctor),cnMemory::TAlignmentOf<TFunctor>::Value)))
	{
		cnClass::ManualConstruct(*fFunctor,static_cast<TFunctor&&>(Functor));
	}
	~cFunctorAllocationStorage()noexcept(true){
		if(fFunctor!=nullptr){
			cnClass::ManualDestruct(*fFunctor);
			TAllocationOperator::Deallocate(fFunctor,sizeof(TFunctor),cnMemory::TAlignmentOf<TFunctor>::Value);
		}
	}

	cFunctorAllocationStorage(cFunctorAllocationStorage &&Src)noexcept(true){
		fFunctor=Src.fFunctor;
		Src.fFunctor=nullptr;
	}

	virtual TRet cnLib_FUNC Execute(TArgs...Args)override{
		return (*fFunctor)(static_cast<TArgs&&>(Args)...);
	}
	virtual void Destruct(void)noexcept(true) override{		cnClass::ManualDestruct(*this);		}
	virtual bool IsNull(void)const noexcept(true) override{	return fFunctor==nullptr;			}
	virtual uIntn SizeOf(void)const noexcept(true)override{	return sizeof(cFunctorAllocationStorage);	}
	virtual void CopyTo(void *Dest)const noexcept(true) override{
		cnClass::ManualConstruct(*static_cast<cFunctorAllocationStorage*>(Dest),*fFunctor);
	}
	virtual void MoveTo(void *Dest)noexcept(true) override{
		cnClass::ManualConstruct(*static_cast<cFunctorAllocationStorage*>(Dest),static_cast<cFunctorAllocationStorage&&>(*this));
	};
	virtual uIntn InplaceSizeOf(void)const noexcept(true)override{	return sizeof(cFunctorInplaceStorage<TRet (TArgs...)>);	}
	virtual void CopyToInplace(void *Dest)const noexcept(true) override{
		cnClass::ManualConstruct(*static_cast<cFunctorInplaceStorage<TRet (TArgs...),TFunctor>*>(Dest),*fFunctor);
	}
	virtual void MoveToInplace(void *Dest)noexcept(true) override{
		cnClass::ManualConstruct(*static_cast<cFunctorInplaceStorage<TRet (TArgs...),TFunctor>*>(Dest),static_cast<TFunctor&&>(*fFunctor));
	}
};

#if cnLibrary_CPPFEATURE_NOEXCEPT_FUNC_TYPE >= 201510L

template<class TFunction,class TFunctor,class TAllocationOperator>	class cFunctorAllocationStorage;

template<class TRet,class...TArgs,class TFunctor,class TAllocationOperator>
class cFunctorAllocationStorage<TRet (TArgs...)noexcept,TFunctor,TAllocationOperator> : public iFunctorStorage<TRet (TArgs...)noexcept>
{
private:
	TFunctor *fFunctor;
public:
	cFunctorAllocationStorage(const TFunctor &Functor)noexcept(true)
		: fFunctor(static_cast<TFunctor*>(TAllocationOperator::Allocate(sizeof(TFunctor),cnMemory::TAlignmentOf<TFunctor>::Value)))
	{
		cnClass::ManualConstruct(*fFunctor,Functor);
	}
	cFunctorAllocationStorage(TFunctor &&Functor)noexcept
		: fFunctor(static_cast<TFunctor*>(TAllocationOperator::Allocate(sizeof(TFunctor),cnMemory::TAlignmentOf<TFunctor>::Value)))
	{
		cnClass::ManualConstruct(*fFunctor,static_cast<TFunctor&&>(Functor));
	}
	~cFunctorAllocationStorage()noexcept(true){
		if(fFunctor!=nullptr){
			cnClass::ManualDestruct(*fFunctor);
			TAllocationOperator::Deallocate(fFunctor,sizeof(TFunctor),cnMemory::TAlignmentOf<TFunctor>::Value);
		}
	}

	cFunctorAllocationStorage(cFunctorAllocationStorage &&Src)noexcept{
		fFunctor=Src.fFunctor;
		Src.fFunctor=nullptr;
	}

	operator TFunctor&&()noexcept{	return static_cast<TFunctor&&>(*fFunctor);	}

	virtual TRet cnLib_FUNC Execute(TArgs...Args)noexcept override{
		return (*fFunctor)(static_cast<TArgs&&>(Args)...);
	}
	virtual void Destruct(void)noexcept override{		cnClass::ManualDestruct(*this);		}
	virtual bool IsNull(void)const noexcept override{	return fFunctor==nullptr;			}
	virtual uIntn SizeOf(void)const noexcept(true)override{	return sizeof(cFunctorAllocationStorage);	}
	virtual void CopyTo(void *Dest)const noexcept(true) override{
		cnClass::ManualConstruct(*static_cast<cFunctorAllocationStorage*>(Dest),*fFunctor);
	}
	virtual void MoveTo(void *Dest)noexcept(true) override{
		cnClass::ManualConstruct(*static_cast<cFunctorAllocationStorage*>(Dest),static_cast<cFunctorAllocationStorage&&>(*this));
	};
	virtual uIntn InplaceSizeOf(void)const noexcept(true)override{	return sizeof(cFunctorInplaceStorage<TRet (TArgs...)noexcept,TFunctor>);	}
	virtual void CopyToInplace(void *Dest)const noexcept(true) override{
		cnClass::ManualConstruct(*static_cast<cFunctorInplaceStorage<TRet (TArgs...)noexcept,TFunctor>*>(Dest),*fFunctor);
	}
	virtual void MoveToInplace(void *Dest)noexcept(true) override{
		cnClass::ManualConstruct(*static_cast<cFunctorInplaceStorage<TRet (TArgs...)noexcept,TFunctor>*>(Dest),static_cast<TFunctor&&>(*fFunctor));
	}
};


#endif	// cnLibrary_CPPFEATURE_NOEXCEPT_FUNC_TYPE >= 201510L

template<class TFunction,class TAllocationOperator>	class cFunctorStorageDuplication;

template<class TRet,class...TArgs,class TAllocationOperator>
class cFunctorStorageDuplication<TRet (TArgs...),TAllocationOperator> : public iFunctorStorage<TRet (TArgs...)>
{
private:
	iFunctorStorage<TRet (TArgs...)> *fStorage;
public:
	cFunctorStorageDuplication(const iFunctorStorage<TRet (TArgs...)> *Storage,uIntn StorageSize)noexcept(true)
		: fStorage(static_cast<iFunctorStorage<TRet (TArgs...)>*>(TAllocationOperator::Allocate(StorageSize,cnMemory::TAlignmentOf<void*>::Value)))
	{
		Storage->CopyToInplace(fStorage);
	}
	~cFunctorStorageDuplication()noexcept(true){
		if(fStorage!=nullptr){
			fStorage->Destruct();
			TAllocationOperator::Deallocate(fStorage,fStorage->InplaceSizeOf(),cnMemory::TAlignmentOf<void*>::Value);
		}
	}

	cFunctorStorageDuplication(cFunctorStorageDuplication &&Src)noexcept(true){
		fStorage=Src.fStorage;
		Src.fStorage=nullptr;
	}

	virtual TRet cnLib_FUNC Execute(TArgs...Args)override{
		return fStorage->Execute(static_cast<TArgs&&>(Args)...);
	}
	virtual void Destruct(void)noexcept(true) override{		cnClass::ManualDestruct(*this);		}
	virtual bool IsNull(void)const noexcept(true) override{	return fStorage->IsNull();			}
	virtual uIntn SizeOf(void)const noexcept(true)override{	return sizeof(cFunctorStorageDuplication);	}
	virtual void CopyTo(void *Dest)const noexcept(true) override{
		cnClass::ManualConstruct(*static_cast<cFunctorStorageDuplication*>(Dest),fStorage,fStorage->SizeOf());
	}
	virtual void MoveTo(void *Dest)noexcept(true) override{
		cnClass::ManualConstruct(*static_cast<cFunctorStorageDuplication*>(Dest),static_cast<cFunctorStorageDuplication&&>(*this));
	};
	virtual uIntn InplaceSizeOf(void)const noexcept(true)override{	return fStorage->InplaceSizeOf();	}
	virtual void CopyToInplace(void *Dest)const noexcept(true) override{
		return fStorage->CopyToInplace(Dest);
	}
	virtual void MoveToInplace(void *Dest)noexcept(true) override{
		return fStorage->MoveToInplace(Dest);
	}
};

#if cnLibrary_CPPFEATURE_NOEXCEPT_FUNC_TYPE >= 201510L


template<class TRet,class...TArgs,class TAllocationOperator>
class cFunctorStorageDuplication<TRet (TArgs...)noexcept,TAllocationOperator> : public iFunctorStorage<TRet (TArgs...)noexcept>
{
private:
	iFunctorStorage<TRet (TArgs...)> *fStorage;
public:
	cFunctorStorageDuplication(iFunctorStorage<TRet (TArgs...)noexcept> *Storage,uIntn StorageSize)noexcept(true)
		: fStorage(static_cast<iFunctorStorage<TRet (TArgs...)noexcept>*>(TAllocationOperator::Allocate(StorageSize,cnMemory::TAlignmentOf<void*>::Value)))
	{
		Storage->CopyToInplace(fStorage);
	}
	~cFunctorStorageDuplication()noexcept(true){
		if(fStorage!=nullptr){
			fStorage->Destruct();
			TAllocationOperator::Deallocate(fStorage,fStorage->InplaceSizeOf(),cnMemory::TAlignmentOf<void*>::Value);
		}
	}
	cFunctorStorageDuplication(cFunctorStorageDuplication &&Src)noexcept(true){
		fStorage=Src.fStorage;
		Src.fStorage=nullptr;
	}

	virtual TRet cnLib_FUNC Execute(TArgs...Args)noexcept override{
		return fStorage->Execute(static_cast<TArgs&&>(Args)...);
	}
	virtual void Destruct(void)noexcept(true) override{		cnClass::ManualDestruct(*this);		}
	virtual bool IsNull(void)const noexcept(true) override{	return fStorage->IsNull();			}
	virtual uIntn SizeOf(void)const noexcept(true)override{	return sizeof(cFunctorStorageDuplication);	}
	virtual void CopyTo(void *Dest)const noexcept(true) override{
		cnClass::ManualConstruct(*static_cast<cFunctorStorageDuplication*>(Dest),fStorage,fStorage->SizeOf());
	}
	virtual void MoveTo(void *Dest)noexcept(true) override{
		cnClass::ManualConstruct(*static_cast<cFunctorStorageDuplication*>(Dest),static_cast<cFunctorStorageDuplication&&>(*this));
	};
	virtual uIntn InplaceSizeOf(void)const noexcept(true)override{	return fStorage->InplaceSizeOf();	}
	virtual void CopyToInplace(void *Dest)const noexcept(true) override{
		return fStorage->CopyToInplace(Dest);
	}
	virtual void MoveToInplace(void *Dest)noexcept(true) override{
		return fStorage->MoveToInplace(Dest);
	}
};


#endif	// cnLibrary_CPPFEATURE_NOEXCEPT_FUNC_TYPE >= 201510L

//---------------------------------------------------------------------------
}	// namespace cnVar
//---------------------------------------------------------------------------
}	// namespace cnLibrary
//---------------------------------------------------------------------------
namespace cnLibrary{
//---------------------------------------------------------------------------
namespace cnVar{
//---------------------------------------------------------------------------


template<class TFunction>
class bcFunction;

template<class TRet,class...TArgs>
class cnLib_INTERFACE bcFunction<TRet (TArgs...)>
{
public:
	iFunctorStorage<TRet (TArgs...)>* Storage(void)noexcept(true){
		return static_cast<iFunctorStorage<TRet (TArgs...)>*>(reinterpret_cast<iFunction<TRet (TArgs...)>*>(this));
	}
	const iFunctorStorage<TRet (TArgs...)>* Storage(void)const noexcept(true){
		return static_cast<const iFunctorStorage<TRet (TArgs...)>*>(reinterpret_cast<const iFunction<TRet (TArgs...)>*>(this));
	}

	TRet operator()(TArgs...Args){
		return Storage()->Execute(static_cast<TArgs&&>(Args)...);
	}

	operator bool()const noexcept(true){
		return !Storage()->IsNull();
	}
	iFunction<TRet (TArgs...)>* operator &()const noexcept(true){
		return reinterpret_cast<iFunction<TRet (TArgs...)>*>(this);
	}
	operator iFunction<TRet (TArgs...)> *()noexcept(true){
		auto Functor=Storage();
		if(Functor->IsNull())
			return nullptr;
		return Functor;
	}
};

#if cnLibrary_CPPFEATURE_NOEXCEPT_FUNC_TYPE >= 201510L

template<class TRet,class...TArgs>
class cnLib_INTERFACE bcFunction<TRet (TArgs...)noexcept>
{
public:
	iFunctorStorage<TRet (TArgs...)noexcept>* Storage(void)noexcept(true){
		return static_cast<iFunctorStorage<TRet (TArgs...)noexcept>*>(reinterpret_cast<iFunction<TRet (TArgs...)noexcept>*>(this));
	}
	const iFunctorStorage<TRet (TArgs...)noexcept>* Storage(void)const noexcept(true){
		return static_cast<const iFunctorStorage<TRet (TArgs...)noexcept>*>(reinterpret_cast<const iFunction<TRet (TArgs...)noexcept>*>(this));
	}

	TRet operator()(TArgs...Args)noexcept{
		return Storage()->Execute(static_cast<TArgs&&>(Args)...);
	}

	operator bool()const noexcept(true){
		return !Storage()->IsNull();
	}
	iFunction<TRet (TArgs...)noexcept>* operator &()const noexcept(true){
		return reinterpret_cast<iFunction<TRet (TArgs...)noexcept>*>(this);
	}

	operator iFunction<TRet (TArgs...)noexcept> *()noexcept(true){
		auto Functor=Storage();
		if(Functor->IsNull())
			return nullptr;
		return Functor;
	}
};

#endif	// cnLibrary_CPPFEATURE_NOEXCEPT_FUNC_TYPE >= 201510L



template<class TFunction,class TAllocationOperator>
class cFunction : public bcFunction<TFunction>
{
public:
	typedef TFunction tFunction;

	cFunction()noexcept(true){	Construct(nullptr);	}
	~cFunction()noexcept(true){	Destruct();	}

	cFunction(tNullptr)noexcept(true){	Construct(nullptr);	}
	cFunction& operator =(tNullptr)noexcept(true){
		Destruct();
		Construct(nullptr);
		return *this;
	}

	cFunction(const cFunction &Src)noexcept(true){	CopyConstruct(&Src.fStorage);	}

	cFunction& operator =(const cFunction &Src)noexcept(true){
		if(&fStorage==&Src.fStorage)
			return *this;
		Destruct();
		CopyConstruct(&Src.fStorage);
		return *this;
	}

	cFunction(cFunction &&Src)noexcept(true){		MoveConstruct(&Src.fStorage);	}

	// move assign
	cFunction& operator =(cFunction &&Src)noexcept(true){
		Destruct();
		MoveConstruct(&Src.fStorage);
		return *this;
	}

	template<class TFunctionPtr>
	cFunction(TFunctionPtr *Ptr)noexcept(true){
		ConstructPtr(Ptr);
	}
	template<class TFunctionPtr>
	cFunction& operator =(TFunctionPtr *Ptr)noexcept(true){
		if(this==reinterpret_cast<cFunction*>(Ptr))
			return *this;
		Destruct();
		ConstructPtr(Ptr);
		return *this;
	}


	template<class TFunctor,typename TTypeConditional<int,(cnLib_THelper::Var_TH::Function_FunctorIsCallable<typename cnVar::TTypeComponent<TFunction>::tReturn,TFunction,TFunctor>::Value && cnVar::TIsConstructibleBy<typename TRemoveCVRef<TFunctor>::Type,TFunctor&&>::Value)>::Type=0>
	cFunction(TFunctor&& Functor)noexcept(true){
		typedef TSelect< (sizeof( cFunctorInplaceStorage<TFunction,typename TRemoveCVRef<TFunctor>::Type>)<=sizeof(cFunction))
			, cFunctorAllocationStorage<TFunction,typename TRemoveCVRef<TFunctor>::Type,TAllocationOperator>
			, cFunctorInplaceStorage<TFunction,typename TRemoveCVRef<TFunctor>::Type>
		>::Type tStorage;

		fStorage.ConstructAs<tStorage>(static_cast<TFunctor&&>(Functor));
	}

	template<class TFunctor,typename TTypeConditional<int,(cnLib_THelper::Var_TH::Function_FunctorIsCallable<typename cnVar::TTypeComponent<TFunction>::tReturn,TFunction,TFunctor>::Value && cnVar::TIsConstructibleBy<typename TRemoveCVRef<TFunctor>::Type,TFunctor&&>::Value)>::Type=0>
	cFunction& operator = (TFunctor&& Functor)noexcept(true){
		Destruct();
		typedef TSelect< (sizeof( cFunctorInplaceStorage<TFunction,typename TRemoveCVRef<TFunctor>::Type>)<=sizeof(cFunction))
			, cFunctorAllocationStorage<TFunction,typename TRemoveCVRef<TFunctor>::Type,TAllocationOperator>
			, cFunctorInplaceStorage<TFunction,typename TRemoveCVRef<TFunctor>::Type>
		>::Type tStorage;

		fStorage.ConstructAs<tStorage>(static_cast<TFunctor&&>(Functor));
		return *this;
	}

protected:

	void Construct(iFunction<TFunction> *Function)noexcept(true){
		fStorage.ConstructAs< cFunctionInterfaceStorage<TFunction> >(Function);
	}
	void Destruct(void)noexcept(true){
		fStorage->Destruct();
	}


	void ConstructPtr(iFunctorStorage<TFunction> *Src)noexcept(true){
		if(Src->IsNull()){
			return Construct(nullptr);
		}

		uIntn SrcSize=Src->InplaceSizeOf();
		if(SrcSize<=sizeof(cFunction)){
			return Src->CopyToInplace(&fStorage);
		}
		fStorage.ConstructAs< cFunctorStorageDuplication<TFunction,TAllocationOperator> >(Src,SrcSize);
	}
	void ConstructPtr(iFunction<TFunction> *Func)noexcept(true){		return Construct(Func);		}
	void ConstructPtr(TFunction *Ptr)noexcept(true){
		fStorage.ConstructAs< cFunctionPtrStorage<TFunction> >(Ptr);
	}

	void CopyConstruct(const iFunctorStorage<TFunction> *Src)noexcept(true){
		if(Src->IsNull()){
			return Construct(nullptr);
		}

		uIntn SrcSize=Src->SizeOf();
		if(SrcSize<=sizeof(cFunction)){
			return Src->CopyTo(&fStorage);
		}
		fStorage.ConstructAs< cFunctorStorageDuplication<TFunction,TAllocationOperator> >(Src,SrcSize);
	}

	void MoveConstruct(iFunctorStorage<TFunction> *Src)noexcept(true){
		if(Src->IsNull()){
			return Construct(nullptr);
		}
		uIntn SrcSize=Src->SizeOf();
		if(SrcSize<=sizeof(cFunction)){
			return Src->MoveTo(&fStorage);
		}
		fStorage.ConstructAs< cFunctorStorageDuplication<TFunction,TAllocationOperator> >(Src,SrcSize);
	}
private:
	cnVar::cPolymorphicObject< cnVar::iFunctorStorage<TFunction>,cFunctionInterfaceStorage<TFunction> > fStorage;
};

//---------------------------------------------------------------------------
}	// namespace cnVar
//---------------------------------------------------------------------------
}	// namespace cnLibrary
//---------------------------------------------------------------------------
#include <cnTK/TKMacrosCleanup.inc>
//---------------------------------------------------------------------------
#endif	/* cnLibrary_CPPFEATURELEVEL >= 3 */
/*-------------------------------------------------------------------------*/
#endif
