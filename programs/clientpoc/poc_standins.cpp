// Earth & Beyond client (reconstruction): the proof-of-concept client's stand-ins (D179). NOT THE CLIENT'S CODE.
/*
**	The side systems WinMain (app\main\main_client.cpp) calls that the tree does not have yet and the POC
**	(start-up videos -> login -> character select) can do without. Each stand-in logs its first call
**	(stubs/stub_log.h: OutputDebugString, stderr and the stub log) and returns a neutral value. Delete one
**	here when its real function lands in app/ (the linker reports the duplicate otherwise).
**
**	This file lives with the POC program, not in stubs/ (every program links stubs.lib, and one object
**	defining several of these would clash with a test program's own definitions), and not in app/ (it
**	is not the client's code, so it must never be matched or counted). The class declarations are
**	main_client.cpp's views, copied: keep them the same.
*/



#include <windows.h>
#include "..\..\stubs\stub_log.h"
#include "..\..\app\kernel\kernel.h"

// ---------------------------------------------------------------- ui_registration.cpp, the dialogs

// 0x5FADE0: false, the retail (not the demo) build: no demo EULA
bool Unknown_5FADE0()
{
	STUB_CALLED("Unknown_5FADE0 (0x5FADE0, ui_registration.cpp): false");
	return false;
}

// 0x5FAD50: false
bool Unknown_5FAD50()
{
	STUB_CALLED("Unknown_5FAD50 (0x5FAD50, ui_registration.cpp): false");
	return false;
}

// 0x847D20: the EULA / ROC text in a dialog; true, as if accepted
bool Unknown_847D20(const char *filename)
{
	STUB_CALLED("Unknown_847D20 (0x847D20, the EULA / ROC dialog): accepted");
	return true;
}

// 0x846C90: the -MENU dialog; true, go on
bool Unknown_846C90()
{
	STUB_CALLED("Unknown_846C90 (0x846C90, the -MENU dialog): go on");
	return true;
}

// ---------------------------------------------------------------- msgserv.cpp's message server (-ENABLE_MESSAGES only)

class Unknown_8B20D0Class : public TaskClass
{
public:
	Unknown_8B20D0Class();
	virtual int Get_Interval();
	virtual bool Delete_On_Remove();
	void Unknown_8B2230();
	char Unknown0C[0x48 - 0x0C];
};

Unknown_8B20D0Class::Unknown_8B20D0Class()
{
	STUB_CALLED("Unknown_8B20D0Class (0x8B20D0, msgserv.cpp's message server)");
}

int Unknown_8B20D0Class::Get_Interval()
{
	return 1000;
}

bool Unknown_8B20D0Class::Delete_On_Remove()
{
	return true;
}

void Unknown_8B20D0Class::Unknown_8B2230()
{
	STUB_CALLED("Unknown_8B20D0Class::Unknown_8B2230 (0x8B2230, msgserv.cpp)");
}

// ---------------------------------------------------------------- the personality manager

class Unknown_790560Class { public: Unknown_790560Class(); ~Unknown_790560Class(); char Unknown00[0x14]; };

Unknown_790560Class *Unknown_BE856C;	// 0xBE856C

Unknown_790560Class::Unknown_790560Class()
{
	STUB_CALLED("Unknown_790560Class (0x790560, the personality manager): no personalities");
}

Unknown_790560Class::~Unknown_790560Class()
{
}

void Unknown_790940(Unknown_790560Class *manager)
{
	Unknown_BE856C = manager;
}

// ---------------------------------------------------------------- the starbase lights manager

class Unknown_764480Class { public: Unknown_764480Class(); ~Unknown_764480Class(); char Unknown00[0x10]; };

Unknown_764480Class *Unknown_BE82D0;	// 0xBE82D0

Unknown_764480Class::Unknown_764480Class()
{
	STUB_CALLED("Unknown_764480Class (0x764480, the starbase lights manager): no lights");
}

Unknown_764480Class::~Unknown_764480Class()
{
}

void Unknown_764510(Unknown_764480Class *manager)
{
	Unknown_BE82D0 = manager;
}
