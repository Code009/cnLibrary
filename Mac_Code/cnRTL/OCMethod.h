/*--- Objective-C Type ----------------------------------------------------*/
/*         Developer : Code009                                             */
/*         Create on : 2013-04-29                                          */
/*-------------------------------------------------------------------------*/
#pragma once

#ifdef __cplusplus
#ifdef	__OBJC__

//#import <Foundation/Foundation.h>

#include <cnRTL/cnRTL.h>
#include <cnRTL/OCType.h>

//---------------------------------------------------------------------------
namespace cnLibrary{
//---------------------------------------------------------------------------
namespace cnRTL{
//---------------------------------------------------------------------------
struct cOCMethodTypeMatching
{
	const char *TypeEncoding;

	void Reset(Method m);
	void Advance(uIntn EncodingLength);
	template<class T>
	uIntn TestType(void)const
	{
		static constexpr uIntn EncodingLength=sizeof(@encode(T))-1;
		if(memcmp(TypeEncoding,@encode(T),EncodingLength)==0){
			return EncodingLength;
		}
		return 0;
	}

	template<uIntn MatchCount>
	uIntn Check(void){
		return MatchCount;
	}
	template<uIntn MatchCount,class T,class...TArgs>
	uIntn Check(void){
		uIntn Length=TestType<T>();
		if(Length!=0){
			Advance(Length);
			return Check<MatchCount+1,TArgs...>();
		}
		return false;
	}

	template<class...TArgs>
	uIntn Check(void){
		return Check<0,TArgs...>();
	}
};
//---------------------------------------------------------------------------
template<class TFunction>
struct cOCMethod;

template<class TRet,class...TArgs>
struct cOCMethod<TRet (TArgs...)>
{
	typedef TRet (*TMethod)(id,SEL,TArgs...);
	static bool CheckMethodSignature(Method m){
		// test argument count
		if(sizeof...(TArgs)+2!=method_getNumberOfArguments(m))
			return false;
		cOCMethodTypeMatching ms;
		ms.Reset(m);

		// test return type
		if(ms.Check<TRet>()==false){
			return false;
		}

		// test hidden arguments
		if(ms.Check<id>()==false){
			return false;
		}
		if(ms.Check<SEL>()==false){
			return false;
		}
		// test arguments
		if(ms.Check<TArgs...>()!=sizeof...(TArgs)){
			return false;
		}
		return true;
	}

	static bool Check(id object,SEL sel){
		auto m=class_getInstanceMethod(object_getClass(object),sel);
		if(m==nullptr)
			return false;
		return CheckMethodSignature(m);
	}
	static bool CheckClass(Class cls,SEL sel){
		auto m=class_getClassMethod(cls,sel);
		if(m==nullptr)
			return false;
		return CheckMethodSignature(m);
	}

	static TRet Call(id self,SEL sel,TArgs...Args){
		cnLib_ASSERT([self respondsToSelector:sel]);
		cnLib_ASSERT(Check(self,sel));

		auto MethodIMP=reinterpret_cast<TMethod>([self methodForSelector:sel]);
		return MethodIMP(self,sel,Args...);
	}

	template<class T>
	static TRet CallForClass(id self,SEL sel,TArgs...Args){
		auto cls=[T class];
		cnLib_ASSERT([self isKindOfClass:cls]);
		cnLib_ASSERT([T instancesRespondToSelector:sel]);
		cnLib_ASSERT(Check(self,sel));

		auto MethodIMP=reinterpret_cast<TMethod>([cls instanceMethodForSelector:sel]);
		return MethodIMP(self,sel,Args...);
	}

};
//---------------------------------------------------------------------------

#if	0
template<class T>
class ocEventNotifyReceiver;

template<class...TArgs>
class ocEventNotifyReceiver<void (TArgs...)> : public iFunction<void (TArgs...)>
{
protected:
	id __unsafe_unretained fObject;
	SEL fSelector;

	typedef iFunctionSet<void (TArgs...)> TFunctionSet;
	TFunctionSet *fSource;

	void SetSource(TFunctionSet *EventNotifyList){
		if(fSource!=nullptr){
			fSource->Remove(this);
		}
		fSource=EventNotifyList;
		if(fSource!=nullptr){
			fSource->Insert(this);
		}
	}
public:

	ocEventNotifyReceiver():fSource(nullptr){}
	~ocEventNotifyReceiver(){
		if(fSource!=nullptr)
			fSource->Remove(this);
	}

	void Set(id<NSObject> __unsafe_unretained Object,SEL Selector){

		if(Selector==nil){
			fObject=nil;
			fSelector=nil;
			return;
		}
		if([Object respondsToSelector:Selector]==false){
			// 无效的selector
			return;
		}
		typedef cOCMethod<void (TArgs...)> TMethod;
		// veryfy selector existence
		if(TMethod::Check(Object,Selector)==false){
			// 方法参数不匹配
			return;
		}
		fSelector=Selector;
		fObject=Object;
	}

	void operator <<=(TFunctionSet &EventNotifyList){
		SetSource(&EventNotifyList);
	}

	void operator <<=(TFunctionSet *EventNotifyList){
		SetSource(EventNotifyList);
	}

	virtual void Execute(TArgs...Args)override{
		if(fObject==nil)
			return;
		typedef cOCMethod<void (TArgs...)> TMethod;
		return TMethod::Call(fObject,fSelector,Args...);
	}
};


template<class T>
class ocEventNotifyReceiverMainThread;

template<class...TArgs>
class ocEventNotifyReceiverMainThread<void (TArgs...)> : public ocEventNotifyReceiver<void (TArgs...)>
{
public:
	virtual void Execute(TArgs...Args)override{
		if(this->fObject==nil)
			return;
		if([NSThread isMainThread]){
			ocEventNotifyReceiver<void (TArgs...)>::Execute(Args...);
		}
		else{
			dispatch_async(dispatch_get_main_queue(), ^{
				ocEventNotifyReceiver<void (TArgs...)>::Execute(Args...);
			});
		}
	}
};
#endif

//---------------------------------------------------------------------------
}	// namespace cnRTL
//---------------------------------------------------------------------------
}	// namespace cnLibrary
//---------------------------------------------------------------------------

#endif	// __OBJC__
#endif	/* __cplusplus */
