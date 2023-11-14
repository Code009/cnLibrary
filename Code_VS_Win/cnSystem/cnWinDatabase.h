/*- Windows - Database ----------------------------------------------------*/
/*         Developer : Code009                                             */
/*         Create on : 2015-01-25                                          */
/*-------------------------------------------------------------------------*/
#pragma once
/*-------------------------------------------------------------------------*/
#include <cnSystem\cnDatabase.h>
#include <cnSystem\cnUI.h>


#ifdef __cplusplus
//---------------------------------------------------------------------------
namespace cnLibrary{
//---------------------------------------------------------------------------
class cnLib_INTERFACE iODBC : public iReference
{
public:
	virtual iPtr<iSQLDBConnection> cnLib_FUNC EditAndConnect(HWND ParentWindow,const wchar_t *ConnectionString,short ConnectionStringLength,
		wchar_t *OutConnectionString,short &OutConnectionStringLength)=0;
	virtual iPtr<iSQLDBConnection> cnLib_FUNC Connect(const wchar_t *ConnectionString,short ConnectionStringLength)=0;

};
//---------------------------------------------------------------------------
namespace cnWindows{
//---------------------------------------------------------------------------
rPtr<iODBC> cnLib_FUNC ODBC(void);
//---------------------------------------------------------------------------
}	// namespace cnWindows
//---------------------------------------------------------------------------
}	// namespace cnLibrary
//---------------------------------------------------------------------------
#endif  /* __cplusplus */
/*-------------------------------------------------------------------------*/
