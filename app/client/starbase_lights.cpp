// Earth & Beyond client (reconstruction): the starbase lights manager, 0x764480-0x764519 (PROVISIONAL file, D137)
#include "starbase_lights.h"

Unknown_764480Class *Unknown_BE82D0;	// 0xBE82D0

// 0x764480
Unknown_764480Class::Unknown_764480Class()
{
	INI.Load("starbase_lights.ini");
}

// 0x7644F0
Unknown_764480Class::~Unknown_764480Class()
{
}

// 0x764510
void Unknown_764510(Unknown_764480Class *manager)
{
	Unknown_BE82D0 = manager;
}
