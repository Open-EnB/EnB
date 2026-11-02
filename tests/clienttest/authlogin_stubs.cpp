// Earth & Beyond client (reconstruction)
// clienttest: tsk_login.obj (app.lib) imports authlogin.dll's AuthLoginLib_GetComponentFactory (unknown_authlogin.h's
// dllimport), and no such DLL sits beside the test. Defining the import pointer here keeps the linker from pulling
// authlogin.lib's import member, so the exe has no authlogin.dll dependency. No test reaches Fetch_EA_Ticket.
#include "clienttest.h"

static void *Auth_Factory_Stand_In()
{
	Trace("stub AuthLoginLib_GetComponentFactory (authlogin.dll)");
	return(0);
}
extern "C" void *(*_imp__AuthLoginLib_GetComponentFactory)() = Auth_Factory_Stand_In;
