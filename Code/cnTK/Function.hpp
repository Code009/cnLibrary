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
	virtual void CopyTo(void *Dest,uIntn Size)const noexcept(true)=0;
	virtual void MoveTo(void *Dest,uIntn Size)noexcept(true)=0;
};


template<class TFunction>	class cFunctionNullStorage;

template<class TRet,class...TArgs>
class cFunctionNullStorage<TRet (TArgs...)> : public iFunctorStorage<TRet (TArgs...)>
{
public:

	virtual TRet cnLib_FUNC Execute(TArgs...)override{
		return static_cast<TRet (*)(void)>(nullptr)();
	}
	virtual void Destruct(void)noexcept(true) override{}
	virtual bool IsNull(void)const noexcept(true) override{	return true;	}
	virtual void CopyTo(void *Dest,uIntn DestSize)const noexcept(true) override{
		cnLib_ASSERT(DestSize>=sizeof(cFunctionNullStorage));
		cnClass::ManualConstruct(*static_cast<cFunctionNullStorage*>(Dest));
	}
	virtual void MoveTo(void *Dest,uIntn DestSize)noexcept(true) override{
		cnLib_ASSERT(DestSize>=sizeof(cFunctionNullStorage));
		cnClass::ManualConstruct(*static_cast<cFunctionNullStorage*>(Dest));
	}

	static void MakeInMemory(void *Dest)noexcept(true){
		cnClass::ManualConstruct(*static_cast<cFunctionNullStorage*>(Dest));
	}
};

#if cnLibrary_CPPFEATURE_NOEXCEPT_FUNC_TYPE >= 201510L

template<class TRet,class...TArgs>
class cFunctionNullStorage<TRet (TArgs...)noexcept> : public iFunctorStorage<TRet (TArgs...)noexcept>
{
public:

	virtual TRet cnLib_FUNC Execute(TArgs...)noexcept override{
		return static_cast<TRet (*)(void)noexcept>(nullptr)();
	}
	virtual void Destruct(void)noexcept(true) override{}
	virtual bool IsNull(void)const noexcept(true) override{	return true;	}
	virtual void CopyTo(void *Dest,uIntn DestSize)const noexcept(true) override{
		cnLib_ASSERT(DestSize>=sizeof(cFunctionNullStorage));
		cnClass::ManualConstruct(*static_cast<cFunctionNullStorage*>(Dest));
	}
	virtual void MoveTo(void *Dest,uIntn DestSize)noexcept(true) override{
		cnLib_ASSERT(DestSize>=sizeof(cFunctionNullStorage));
		cnClass::ManualConstruct(*static_cast<cFunctionNullStorage*>(Dest));
	}

	static void MakeInMemory(void *Dest)noexcept(true){
		cnClass::ManualConstruct(*static_cast<cFunctionNullStorage*>(Dest));
	}
};

#endif // cnLibrary_CPPFEATURE_NOEXCEPT_FUNC_TYPE >= 201510L

//---------------------------------------------------------------------------
}	// namespace cnVar
//---------------------------------------------------------------------------
}	// namespace cnLibrary
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

	cFunctionInterfaceStorage()noexcept(true):fPtr(nullptr){}
	cFunctionInterfaceStorage(iFunction<TRet (TArgs...)> *Ptr)noexcept(true):fPtr(Ptr){}

	virtual TRet cnLib_FUNC Execute(TArgs...Args)override{
		return fPtr->Execute(static_cast<TArgs&&>(Args)...);
	}
	virtual void Destruct(void)noexcept(true) override{}
	virtual bool IsNull(void)const noexcept(true) override{	return fPtr==nullptr;		}
	virtual void CopyTo(void *Dest,uIntn DestSize)const noexcept(true) override{
		cnLib_ASSERT(DestSize>=sizeof(cFunctionInterfaceStorage));
		cnClass::ManualConstruct(*static_cast<cFunctionInterfaceStorage*>(Dest),fPtr);
	}
	virtual void MoveTo(void *Dest,uIntn DestSize)noexcept(true) override{
		cnLib_ASSERT(DestSize>=sizeof(cFunctionInterfaceStorage));
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

	cFunctionInterfaceStorage()noexcept:fPtr(nullptr){}
	cFunctionInterfaceStorage(iFunction<TRet (TArgs...)> *Ptr)noexcept:fPtr(Ptr){}

	virtual TRet cnLib_FUNC Execute(TArgs...Args)noexcept override{
		return fPtr->Execute(static_cast<TArgs&&>(Args)...);
	}
	virtual void Destruct(void)noexcept override{}
	virtual bool IsNull(void)const noexcept override{	return fPtr==nullptr;		}
	virtual void CopyTo(void *Dest,uIntn DestSize)const noexcept override{
		cnLib_ASSERT(DestSize>=sizeof(cFunctionInterfaceStorage));
		cnClass::ManualConstruct(*static_cast<cFunctionInterfaceStorage*>(Dest),fPtr);
	}
	virtual void MoveTo(void *Dest,uIntn DestSize)noexcept override{
		cnLib_ASSERT(DestSize>=sizeof(cFunctionInterfaceStorage));
		cnClass::ManualConstruct(*static_cast<cFunctionInterfaceStorage*>(Dest),fPtr);
	}
};

#endif // cnLibrary_CPPFEATURE_NOEXCEPT_FUNC_TYPE >= 201510L

//---------------------------------------------------------------------------
}	// namespace cnVar
//---------------------------------------------------------------------------
}	// namespace cnLibrary
//---------------------------------------------------------------------------
namespace cnLibrary{
//---------------------------------------------------------------------------
namespace cnVar{
//---------------------------------------------------------------------------


template<class TFunction,class TFunctor>
class cFunctionPtrStorage;

template<class TRet,class...TArgs,class TFunctor>
class cFunctionPtrStorage<TRet (TArgs...),TFunctor> : public iFunctorStorage<TRet (TArgs...)>
{
private:
	TFunctor fPtr;
public:
	cFunctionPtrStorage()noexcept(true):fPtr(nullptr){}
	cFunctionPtrStorage(TFunctor Ptr)noexcept(true):fPtr(Ptr){}

	virtual TRet cnLib_FUNC Execute(TArgs...Args)override{
		return (*fPtr)(static_cast<TArgs&&>(Args)...);
	}
	virtual void Destruct(void)noexcept(true) override{}
	virtual bool IsNull(void)const noexcept(true) override{			return fPtr==nullptr;		}
	virtual void CopyTo(void *Dest,uIntn DestSize)const noexcept(true) override{
		cnLib_ASSERT(DestSize>=sizeof(cFunctionPtrStorage));
		cnClass::ManualConstruct(*static_cast<cFunctionPtrStorage*>(Dest),fPtr);
	}
	virtual void MoveTo(void *Dest,uIntn DestSize)noexcept(true) override{
		cnLib_ASSERT(DestSize>=sizeof(cFunctionPtrStorage));
		cnClass::ManualConstruct(*static_cast<cFunctionPtrStorage*>(Dest),fPtr);
	}
};

#if cnLibrary_CPPFEATURE_NOEXCEPT_FUNC_TYPE >= 201510L

template<class TRet,class...TArgs,class TFunctor>
class cFunctionPtrStorage<TRet (TArgs...)noexcept,TFunctor> : public iFunctorStorage<TRet (TArgs...)noexcept>
{
private:
	TFunctor fPtr;
public:
	cFunctionPtrStorage()noexcept:fPtr(nullptr){}
	cFunctionPtrStorage(TFunctor Ptr)noexcept:fPtr(Ptr){}

	virtual TRet cnLib_FUNC Execute(TArgs...Args)noexcept override{
		return (*fPtr)(static_cast<TArgs&&>(Args)...);
	}
	virtual void Destruct(void)noexcept override{}
	virtual bool IsNull(void)const noexcept override{			return fPtr==nullptr;		}
	virtual void CopyTo(void *Dest,uIntn DestSize)const noexcept override{
		cnLib_ASSERT(DestSize>=sizeof(cFunctionPtrStorage));
		cnClass::ManualConstruct(*static_cast<cFunctionPtrStorage*>(Dest),fPtr);
	}
	virtual void MoveTo(void *Dest,uIntn DestSize)noexcept override{
		cnLib_ASSERT(DestSize>=sizeof(cFunctionPtrStorage));
		cnClass::ManualConstruct(*static_cast<cFunctionPtrStorage*>(Dest),fPtr);
	}
};

#endif // cnLibrary_CPPFEATURE_NOEXCEPT_FUNC_TYPE >= 201510L

//---------------------------------------------------------------------------
}	// namespace cnVar
//---------------------------------------------------------------------------
}	// namespace cnLibrary
//---------------------------------------------------------------------------
namespace cnLibrary{
//---------------------------------------------------------------------------
namespace cnVar{
//---------------------------------------------------------------------------

//TFunctionStorageOperator
//{
//	static constexpr uIntn StorageSize;
//	typedef tAllocationOperator;
//
//	template<class TFunctor>
//	void MakeInStorage(void *Dest,TFunctor &&Functor)noexcept;
//	template<class TFunctor>
//	bool MakeInMemory(void *Dest,uIntn DestSize,TFunctor &&Functor)noexcept;
//
//};

template<class TFunction,class TFunctionStorageOperator>
class bcFunction
{
public:

	bcFunction()noexcept(true){	ConstructVoid();	}
	~bcFunction()noexcept(true){	Destruct();	}

	bcFunction(const bcFunction &Src)noexcept(true){	CopyConstruct(Src);	}

	bcFunction& operator =(const bcFunction &Src)noexcept(true){
		if(this==&Src)
			return *this;
		Destruct();
		CopyConstruct(Src);
		return *this;
	}
	template<class TSrcFunctionStorageOperator>
	bcFunction(const bcFunction<TFunction,TSrcFunctionStorageOperator> &Src)noexcept(true){	CopyConstruct(Src);	}

	template<class TSrcFunctionStorageOperator>
	bcFunction& operator =(const bcFunction<TFunction,TSrcFunctionStorageOperator> &Src)noexcept(true){
		Destruct();
		CopyConstruct(Src);
		return *this;
	}
	bcFunction(bcFunction &&Src)noexcept(true){			MoveConstruct(Src);	}
	// move assign
	bcFunction& operator =(bcFunction &&Src)noexcept(true){
		Destruct();
		MoveConstruct(Src);
		return *this;
	}
	template<class TSrcFunctionStorageOperator>
	bcFunction(bcFunction<TFunction,TSrcFunctionStorageOperator> &&Src)noexcept(true)
	{		MoveConstruct(Src);	}

	template<class TSrcFunctionStorageOperator>
	bcFunction& operator =(bcFunction<TFunction,TSrcFunctionStorageOperator> &&Src)noexcept(true){
		Destruct();
		MoveConstruct(Src);
		return *this;
	}


	template<class TFunctorPtr>
	bcFunction(TFunctorPtr *Ptr)noexcept(true){
		fFunctorStorage.template ConstructAs<cFunctionPtrStorage<TFunction,TFunction*> >(Ptr);
	}
	bcFunction(iFunction<TFunction> *Func)noexcept(true){
		fFunctorStorage.template ConstructAs< cFunctionInterfaceStorage<TFunction> >(Func);
	}

	bcFunction& operator =(TFunction *Func)noexcept(true){
		Destruct();
		fFunctorStorage.template ConstructAs<cFunctionPtrStorage<TFunction,TFunction*> >(Func);
		return *this;
	}

	bcFunction& operator =(iFunction<TFunction> *Func)noexcept(true){
		Destruct();
		fFunctorStorage.template ConstructAs< cFunctionInterfaceStorage<TFunction> >(Func);
		return *this;
	}

	template<class TFunctor,typename cnVar::TTypeConditional<int,TFunctionStorageOperator::template TFunctorIsSupported<TFunctor>::Value>::Type=0>
	bcFunction(TFunctor&& Functor)noexcept(true){
		TFunctionStorageOperator::MakeInStorage(&fFunctorStorage,static_cast<TFunctor&&>(Functor));
	}

	template<class TFunctor,typename cnVar::TTypeConditional<int,TFunctionStorageOperator::template TFunctorIsSupported<TFunctor>::Value>::Type=0>
	bcFunction& operator = (TFunctor&& Functor)noexcept(true){
		Destruct();
		TFunctionStorageOperator::MakeInStorage(&fFunctorStorage,static_cast<TFunctor&&>(Functor));
		return *this;
	}

	bcFunction& operator =(tNullptr)noexcept(true){
		Destruct();
		ConstructVoid();
		return *this;
	}

	operator bool()const noexcept(true){
		return !fFunctorStorage->IsNull();
	}

	operator iFunction<TFunction> *()noexcept(true){
		if(fFunctorStorage->IsNull())
			return nullptr;
		return &fFunctorStorage;
	}
	iFunction<TFunction> *GetFunctionInterface()noexcept(true){
		return &fFunctorStorage;
	}


protected:
	template<class,class> friend class bcFunction;

	static_assert(TFunctionStorageOperator::StorageSize>=sizeof(cFunctionPtrStorage<TFunction,TFunction*>),"storage size is too small");
	cnVar::cPolymorphicInterface<iFunctorStorage<TFunction>,TFunctionStorageOperator::StorageSize,void*> fFunctorStorage;

	void ConstructVoid(void)noexcept(true){
		fFunctorStorage.template ConstructAs< cFunctionNullStorage<TFunction> >();
	}
	void Destruct(void)noexcept(true){
		fFunctorStorage->Destruct();
	}

	template<class TSrcFunctionStorageOperator>
	void CopyConstruct(const bcFunction<TFunction,TSrcFunctionStorageOperator> &Src)noexcept(true){
		Src.fFunctorStorage->CopyTo(&fFunctorStorage,TFunctionStorageOperator::StorageSize);
	}
	template<class TSrcFunctionStorageOperator>
	void MoveConstruct(bcFunction<TFunction,TSrcFunctionStorageOperator> &Src)noexcept(true){
		Src.fFunctorStorage->MoveTo(&fFunctorStorage,TFunctionStorageOperator::StorageSize);
		Src.Reset();
	}
	void Reset(void)noexcept(true){
		Destruct();
		ConstructVoid();
	}
};


template<class TFunction,class TFunctionStorageOperator>	class cFunction;

template<class TRet,class...TArgs,class TFunctionStorageOperator>
class cFunction<TRet (TArgs...),TFunctionStorageOperator> : public bcFunction<TRet (TArgs...),TFunctionStorageOperator>
{
public:
	typedef TRet tFunction(TArgs...);

	cFunction()noexcept(true){}
	cFunction(const cFunction &Src)noexcept(true)
		: bcFunction<TRet (TArgs...),TFunctionStorageOperator>(Src){}
	cFunction(cFunction &&Src)noexcept(true)
		: bcFunction<TRet (TArgs...),TFunctionStorageOperator>(static_cast<cFunction&&>(Src)){}


	using bcFunction<TRet (TArgs...),TFunctionStorageOperator>::operator =;
	
#if cnLibrary_CPPFEATURE_INHERIT_CONSTRUCTORS >= 200802L
	using bcFunction<TRet (TArgs...),TFunctionStorageOperator>::bcFunction;
#else


	template<class TSrcFunctionStorageOperator>
	cFunction(const bcFunction<tFunction,TSrcFunctionStorageOperator> &Src)noexcept(true){	this->CopyConstruct(Src);	}


	template<class TSrcFunctionStorageOperator>
	cFunction(bcFunction<tFunction,TSrcFunctionStorageOperator> &&Src)noexcept(true)
	{		this->MoveConstruct(Src);	}


	template<class TFunctorPtr>
	cFunction(TFunctorPtr *Ptr)noexcept(true)
		: bcFunction<TRet (TArgs...),TFunctionStorageOperator>(Ptr){}
	cFunction(iFunction<TRet (TArgs...)> *Func)noexcept(true)
		: bcFunction<TRet (TArgs...),TFunctionStorageOperator>(Func){}


	template<class TFunctor,typename cnVar::TTypeConditional<int,TFunctionStorageOperator::template TFunctorIsSupported<TFunctor>::Value>::Type=0>
	cFunction(TFunctor&& Functor)noexcept(true)
		: bcFunction<TRet (TArgs...),TFunctionStorageOperator>(static_cast<TFunctor&&>(Functor)){}

#endif // cnLibrary_CPPFEATURE_INHERIT_CONSTRUCTORS >= 200802L


	TRet operator()(TArgs...Args){
		return this->fFunctorStorage->Execute(static_cast<TArgs&&>(Args)...);
	}

};

#if cnLibrary_CPPFEATURE_NOEXCEPT_FUNC_TYPE >= 201510L

template<class TRet,class...TArgs,class TFunctionStorageOperator>
class cFunction<TRet (TArgs...)noexcept,TFunctionStorageOperator> : public bcFunction<TRet (TArgs...)noexcept,TFunctionStorageOperator>
{
public:
	typedef TRet tFunction(TArgs...)noexcept;

	cFunction()noexcept{}
	~cFunction()noexcept{}

	cFunction(const cFunction &Src)noexcept
		: bcFunction<tFunction,TFunctionStorageOperator>(Src){}

	cFunction& operator =(const cFunction &Src)noexcept{
		bcFunction<tFunction,TFunctionStorageOperator>::operator = (Src);
		return *this;
	}
#if cnLibrary_CPPFEATURE_RVALUE_REFERENCES >= 200610L
	cFunction(cFunction &&Src)noexcept
		: bcFunction<tFunction,TFunctionStorageOperator>(static_cast<cFunction&&>(Src)){}
	// move assign
	cFunction& operator =(cFunction &&Src)noexcept{
		bcFunction<tFunction,TFunctionStorageOperator>::operator = (static_cast<cFunction&&>(Src));
		return *this;
	}
#endif	// cnLibrary_CPPFEATURE_RVALUE_REFERENCES >= 200610L


#if cnLibrary_CPPFEATURE_INHERIT_CONSTRUCTORS >= 200802L
	using bcFunction<TRet (TArgs...)noexcept,TFunctionStorageOperator>::bcFunction;
#else
#error	"no inherit constructors ??"
#endif // cnLibrary_CPPFEATURE_INHERIT_CONSTRUCTORS >= 200802L

	using bcFunction<TRet (TArgs...)noexcept,TFunctionStorageOperator>::operator =;

	TRet operator()(TArgs...Args)noexcept{
		return this->fFunctorStorage->Execute(static_cast<TArgs&&>(Args)...);
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

template<class TFunction,class TFunctor,class TFunctionStorageOperator>	class cFunctorInplaceCopyStorage;

template<class TRet,class...TArgs,class TFunctor,class TFunctionStorageOperator>
class cFunctorInplaceCopyStorage<TRet (TArgs...),TFunctor,TFunctionStorageOperator> : private TFunctor, public iFunctorStorage<TRet (TArgs...)>
{
public:
	cFunctorInplaceCopyStorage(const TFunctor &Functor)noexcept(true):TFunctor(Functor){}

	cFunctorInplaceCopyStorage(TFunctor &&Functor)noexcept(true):TFunctor(static_cast<TFunctor&&>(Functor)){}
	~cFunctorInplaceCopyStorage()noexcept(true){}
	virtual TRet cnLib_FUNC Execute(TArgs...Args)override{
		return (*this)(static_cast<TArgs&&>(Args)...);
	}
	virtual void Destruct(void)noexcept(true) override{	cnClass::ManualDestruct(*this);	}
	virtual bool IsNull(void)const noexcept(true) override{			return false;			}
	virtual void CopyTo(void *Dest,uIntn DestSize)const noexcept(true) override{
		if(TFunctionStorageOperator::MakeInMemory(Dest,DestSize,static_cast<const TFunctor&>(*this))==false){
			cFunctionNullStorage<TRet (TArgs...) >::MakeInMemory(Dest);
		}
	}
	virtual void MoveTo(void *Dest,uIntn DestSize)noexcept(true) override{
		if(TFunctionStorageOperator::MakeInMemory(Dest,DestSize,static_cast<TFunctor&&>(*this))==false){
			cFunctionNullStorage<TRet(TArgs...) >::MakeInMemory(Dest);
		}
	}

};

#if cnLibrary_CPPFEATURE_NOEXCEPT_FUNC_TYPE >= 201510L

template<class TRet,class...TArgs,class TFunctor,class TFunctionStorageOperator>
class cFunctorInplaceCopyStorage<TRet (TArgs...)noexcept,TFunctor,TFunctionStorageOperator> : private TFunctor, public iFunctorStorage<TRet (TArgs...)noexcept>
{
public:
	cFunctorInplaceCopyStorage(const TFunctor &Functor)noexcept:TFunctor(Functor){}

#if cnLibrary_CPPFEATURE_RVALUE_REFERENCES >= 200610L
	cFunctorInplaceCopyStorage(TFunctor &&Functor)noexcept:TFunctor(static_cast<TFunctor&&>(Functor)){}
#endif	// cnLibrary_CPPFEATURE_RVALUE_REFERENCES >= 200610L
	~cFunctorInplaceCopyStorage()noexcept{}
	virtual TRet cnLib_FUNC Execute(TArgs...Args)noexcept override{
		return (*this)(static_cast<TArgs&&>(Args)...);
	}
	virtual void Destruct(void)noexcept override{	cnClass::ManualDestruct(*this);	}
	virtual bool IsNull(void)const noexcept override{			return false;			}
	virtual void CopyTo(void *Dest,uIntn DestSize)const noexcept override{
		if(TFunctionStorageOperator::MakeInMemory(Dest,DestSize,static_cast<const TFunctor&>(*this))==false){
			cFunctionNullStorage<TRet (TArgs...)noexcept>::MakeInMemory(Dest);
		}
	}
	virtual void MoveTo(void *Dest,uIntn DestSize)noexcept override{
		if(TFunctionStorageOperator::MakeInMemory(Dest,DestSize,static_cast<TFunctor&&>(*this))==false){
			cFunctionNullStorage<TRet(TArgs...)noexcept>::MakeInMemory(Dest);
		}
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

template<class TFunction,class TFunctor,class TFunctionStorageOperator>	class cFunctorDuplicationStorage;

template<class TRet,class...TArgs,class TFunctor,class TFunctionStorageOperator>
class cFunctorDuplicationStorage<TRet (TArgs...),TFunctor,TFunctionStorageOperator> : public iFunctorStorage<TRet (TArgs...)>
{
private:
	TFunctor *fFunctor;
public:
	cFunctorDuplicationStorage()noexcept(true):fFunctor(nullptr){}
	cFunctorDuplicationStorage(cFunctorDuplicationStorage &&Src)noexcept(true){
		fFunctor=Src.fFunctor;
		Src.fFunctor=nullptr;
	}
	template<class TArg>
	cFunctorDuplicationStorage(TArg&& Functor)noexcept(true){
		fFunctor=static_cast<TFunctor*>(TFunctionStorageOperator::tAllocationOperator::Allocate(sizeof(TFunctor),cnMemory::TAlignmentOf<TFunctor>::Value));
		cnClass::ManualConstruct(*fFunctor,static_cast<TArg&&>(Functor));

	}

	~cFunctorDuplicationStorage()noexcept(true){
		if(fFunctor!=nullptr){
			cnClass::ManualDestruct(*fFunctor);
			TFunctionStorageOperator::tAllocationOperator::Deallocate(fFunctor,sizeof(TFunctor),cnMemory::TAlignmentOf<TFunctor>::Value);
		}
	}

	operator TFunctor&&()noexcept(true){	return static_cast<TFunctor&&>(*fFunctor);	}

	virtual TRet cnLib_FUNC Execute(TArgs...Args)override{
		return (*fFunctor)(static_cast<TArgs&&>(Args)...);
	}
	virtual void Destruct(void)noexcept(true) override{		cnClass::ManualDestruct(*this);		}
	virtual bool IsNull(void)const noexcept(true) override{	return fFunctor==nullptr;			}
	virtual void CopyTo(void *Dest,uIntn DestSize)const noexcept(true) override{
		if(TFunctionStorageOperator::MakeInMemory(Dest,DestSize,*fFunctor)==false){
			cFunctionNullStorage<TRet(TArgs...)>::MakeInMemory(Dest);
		}
	}
	virtual void MoveTo(void *Dest,uIntn DestSize)noexcept(true) override{
		// TODO : move allocated memory
		//if(TFunctionStorageOperator::MakeInMemory(Dest,DestSize,static_cast<cFunctorDuplicationStorage&&>(*this))==false){
		if(TFunctionStorageOperator::MakeInMemory(Dest,DestSize,*fFunctor)==false){
			cFunctionNullStorage<TRet(TArgs...)>::MakeInMemory(Dest);
		}
		//}
	};
};

#if cnLibrary_CPPFEATURE_NOEXCEPT_FUNC_TYPE >= 201510L


template<class TRet,class...TArgs,class TFunctor,class TFunctionStorageOperator>
class cFunctorDuplicationStorage<TRet (TArgs...)noexcept,TFunctor,TFunctionStorageOperator> : public iFunctorStorage<TRet (TArgs...)noexcept>
{
private:
	TFunctor *fFunctor;
public:
	cFunctorDuplicationStorage()noexcept:fFunctor(nullptr){}
#if cnLibrary_CPPFEATURE_RVALUE_REFERENCES >= 200610L
	cFunctorDuplicationStorage(cFunctorDuplicationStorage &&Src)noexcept{
		fFunctor=Src.fFunctor;
		Src.fFunctor=nullptr;
	}
#endif	// cnLibrary_CPPFEATURE_RVALUE_REFERENCES >= 200610L
	template<class TArg>
	cFunctorDuplicationStorage(TArg&& Functor)noexcept{
		fFunctor=static_cast<TFunctor*>(TFunctionStorageOperator::tAllocationOperator::Allocate(sizeof(TFunctor),cnMemory::TAlignmentOf<TFunctor>::Value));
		cnClass::ManualConstruct(*fFunctor,static_cast<TArg&&>(Functor));

	}

	~cFunctorDuplicationStorage()noexcept{
		if(fFunctor!=nullptr){
			cnClass::ManualDestruct(*fFunctor);
			TFunctionStorageOperator::tAllocationOperator::Deallocate(fFunctor,sizeof(TFunctor),cnMemory::TAlignmentOf<TFunctor>::Value);
		}
	}

#if cnLibrary_CPPFEATURE_RVALUE_REFERENCES >= 200610L
	operator TFunctor&&()noexcept{	return static_cast<TFunctor&&>(*fFunctor);	}
#endif	//cnLibrary_CPPFEATURE_RVALUE_REFERENCES >= 200610L

	virtual TRet cnLib_FUNC Execute(TArgs...Args)noexcept override{
		return (*fFunctor)(static_cast<TArgs&&>(Args)...);
	}
	virtual void Destruct(void)noexcept override{		cnClass::ManualDestruct(*this);		}
	virtual bool IsNull(void)const noexcept override{	return fFunctor==nullptr;			}
	virtual void CopyTo(void *Dest,uIntn DestSize)const noexcept override{
		if(TFunctionStorageOperator::MakeInMemory(Dest,DestSize,*fFunctor)==false){
			cFunctionNullStorage<TRet(TArgs...)>::MakeInMemory(Dest);
		}
	}
	virtual void MoveTo(void *Dest,uIntn DestSize)noexcept override{
		// TODO : move allocated memory
		//if(TFunctionStorageOperator::MakeInMemory(Dest,DestSize,static_cast<cFunctorDuplicationStorage&&>(*this))==false){
		if(TFunctionStorageOperator::MakeInMemory(Dest,DestSize,*fFunctor)==false){
			cFunctionNullStorage<TRet(TArgs...)>::MakeInMemory(Dest);
		}
		//}
	};
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

typedef TConstantValueUIntn<sizeof(cFunctionPtrStorage<void(void),void(*)(void)>)> TFunctionStorageDefaultSize;

template<class TAllocationOperator,class TFunction,uIntn TCStorageSize>
struct cFunctionStorageOperator
{
	static cnLib_CONSTVAR uIntn StorageSize=TCStorageSize;
	typedef TAllocationOperator tAllocationOperator;

	template<class TFunctor>
	struct TFunctorIsSupported : cnVar::TConstantValueTrue{};

	template<class TFunctor>
	static void MakeInStorage(void *Dest,TFunctor&& Functor)noexcept(true){
		typedef typename cnVar::TRemoveCVRef<TFunctor>::Type TFunctorVariable;
		if cnLib_IFCONSTEXPR(StorageSize>=sizeof(cFunctorInplaceCopyStorage<TFunction,TFunctorVariable,cFunctionStorageOperator>)){
			if(cnMemory::PointerIsAligned<TFunctor>(Dest)){
				cnClass::ManualConstruct(*static_cast<cFunctorInplaceCopyStorage<TFunction,TFunctorVariable,cFunctionStorageOperator>*>(Dest),static_cast<TFunctor&&>(Functor));
				return;
			}
		}

		static_assert(StorageSize>=sizeof(cFunctorDuplicationStorage<TFunction,TFunctorVariable,cFunctionStorageOperator>),"not enough storage for the functor");
		cnClass::ManualConstruct(*static_cast<cFunctorDuplicationStorage<TFunction,TFunctorVariable,cFunctionStorageOperator>*>(Dest),static_cast<TFunctor&&>(Functor));
	}

	template<class TFunctor>
	static bool MakeInMemory(void *Dest,uIntn DestSize,TFunctor&& Functor)noexcept(true){
		typedef typename cnVar::TRemoveCVRef<TFunctor>::Type TFunctorVariable;
		if(cnMemory::PointerIsAligned<TFunctorVariable>(Dest)){
			if(DestSize>=sizeof(cFunctorInplaceCopyStorage<TFunction,TFunctorVariable,cFunctionStorageOperator>)){
				cnClass::ManualConstruct(*static_cast<cFunctorInplaceCopyStorage<TFunction,TFunctorVariable,cFunctionStorageOperator>*>(Dest),static_cast<TFunctor&&>(Functor));
				return true;
			}
		}
		if(DestSize>=sizeof(cFunctorDuplicationStorage<TFunction,TFunctorVariable,cFunctionStorageOperator>)){
			cnClass::ManualConstruct(*static_cast<cFunctorDuplicationStorage<TFunction,TFunctorVariable,cFunctionStorageOperator>*>(Dest),static_cast<TFunctor&&>(Functor));
			return true;
		}
		return false;
	}

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
