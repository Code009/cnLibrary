/*- cnWin - DNet - Managed - Common ---------------------------------------*/
/*         Developer : Code009                                             */
/*         Create on : 2020-04-14                                          */
/*-------------------------------------------------------------------------*/
#pragma once
/*-------------------------------------------------------------------------*/

//#include <cnSystem\cnWindows.h>
#include <cnWinCLI\WinRTLM.h>

#ifdef __cplusplus
//---------------------------------------------------------------------------
namespace cnLibrary{
//---------------------------------------------------------------------------
namespace cnWin{
//---------------------------------------------------------------------------
class iCLIObject : public iInterface
{
public:
	struct tInterfaceID{	static iTypeID Value;	};
	virtual void* cnLib_FUNC CastInterface(iTypeID IID)noexcept(true) override;

	virtual cGCRef& cnLib_FUNC GetObjecHandle(void)=0;

#if _MANAGED

	System::Object^ __clrcall Get(void){
		auto &Handle=GetObjecHandle();
		return Handle;
	}

	template<class T>
	T __clrcall Get(void)
	{
		System::Object^ Target=GetObjecHandle();
		return dynamic_cast<T>(Target);
	}


#endif	// _MANAGED
};
//---------------------------------------------------------------------------
#if _MANAGED
//---------------------------------------------------------------------------
private ref class rcProcedureCaller sealed
{
public:
	rcProcedureCaller(iProcedure *Procedure);
	void Execute(void);
private:
	iProcedure *fProcedure;
};
//---------------------------------------------------------------------------
private ref class rcRefProcedureCaller sealed
{
public:
	rcRefProcedureCaller(iReference *Reference,iProcedure *Procedure);
	void Execute(void);
private:
	iProcedure *fProcedure;
	iReference *fReference;
};
//---------------------------------------------------------------------------
#endif	// _MANAGED
//---------------------------------------------------------------------------
void rcRefProcedureCaller_RefProc(iReference *Reference);
void rcRefProcedureCaller_CallProc(iReference *Reference,iProcedure *Procedure);
//---------------------------------------------------------------------------
}	// namespace cnWin
//---------------------------------------------------------------------------
}	// namespace cnLibrary
//---------------------------------------------------------------------------
#endif  /* __cplusplus */
