/*- MACRO - Event Handler -------------------------------------------------*/
/*         Developer : Code009                                             */
/*         Create on : 2009-12-24                                          */
/*-------------------------------------------------------------------------*/
#ifndef	__cnLib_EventHandler_Header__
#define	__cnLib_EventHandler_Header__

#ifdef	__cplusplus
//---------------------------------------------------------------------------
namespace cnLib_Template_Helper{
namespace RTL{
template<class TFunction>
struct cMACRO_EventHandler{
    cMACRO_EventHandler *Object;
	TFunction cMACRO_EventHandler::*Func;

	operator bool()const{	return Func!=0;	}
	void Clear(void){	Object=0;	Func=0;	}
	template<class TObject,class TFuncClass,class TFunc>
	void Set(TObject *EventObject,TFunc TFuncClass::*EventFunc){
		Object=reinterpret_cast<cMACRO_EventHandler*>(static_cast<TFuncClass*>(EventObject));
		Func=reinterpret_cast<TFunction (cMACRO_EventHandler::*)>(EventFunc);
	}
	template<class TImpClass,class TObject,class TFuncClass,class TFunc>
	static TImpClass Make(TObject *EventObject,TFunc TFuncClass::*EventFunc){
		TImpClass RetEvent;
		RetEvent.Set(EventObject,EventFunc);
		return RetEvent;
	}
};
}	// namespace RTL
}	// namespace cnLib_Template_Helper
//---------------------------------------------------------------------------

// Macro - define an event

#define	DECLEAR_EVENT_HANDLER(_name_,_return_type_,_call_type_,_arguments_)\
	typedef cnLib_Template_Helper::RTL::cMACRO_EventHandler<_return_type_ _call_type_ _arguments_> _name_
#define CALL_EVENT_HANDLER(_handler_,_arguments_)    ( ((_handler_).Object->*(_handler_).Func)_arguments_ )
#define CLEAR_EVENT_HANDLER(_handler_)		( (_handler_).Clear() )
#define IS_EMPTY_EVENT_HANDLER(_handler_)	( !(_handler_) )
#define ASSIGN_EVENT_HANDLER(_handler_,_event_object_,_event_func_)		( (_handler_).Set(_event_object_,_event_func_) )
#define	GET_EVENT_HANDLER(_handler_type_,_event_object_,_event_func_)	( _handler_type_::Make<_handler_type_>(_event_object_,_event_func_) )
#define SAFE_CALL_EVENT_HANDLER(_handler_,_arguments_,_no_handler_expression_)    ( (_handler_)?CALL_EVENT_HANDLER((_handler_),_arguments_):(_no_handler_expression_) )
#define SAFE_CALL_EVENT_PROCEDURE(_handler_,_arguments_)    ( (_handler_)?CALL_EVENT_HANDLER((_handler_),_arguments_):void(_handler_) )

//---------------------------------------------------------------------------
#endif	/* __cplusplus */
#endif

