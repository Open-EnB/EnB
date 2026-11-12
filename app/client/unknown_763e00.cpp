// Earth & Beyond client (reconstruction): the render state list table, 0x763E00-0x764480 (PROVISIONAL file, D137:
// no __FILE__; between proj_decal_manager.cpp and starbase_lights.cpp). Notes at the end.
#include <stdio.h>
#include "unknown_763e00.h"
#include "ini.h"
#include "..\shared\rs_track.h"

// 0x763E00
RSLRegistryClass::RSLRegistryClass()
{
}

// 0x763E30
RSLRegistryClass::~RSLRegistryClass()
{
}

// 0x763E70: one list per '<n>\STATE0' section, n from 0 until one is missing
void RSLRegistryClass::Unknown_763E70(const char *ini_name)
{
	INIClass ini;
	ini.Load(ini_name);
	char section[256];
	int i = 0;
	while (true) {
		sprintf(section, "%d\\STATE0", i);
		if (ini.Find_Section(section) == NULL) {
			break;
		}
		int len = Lists.Length();
		if (i >= len) {
			Lists.Resize(i + 10);
			for (int j = len; j < i + 10; j++) {
				Lists[j] = NULL;
			}
		}
		sprintf(section, "%d", i);
		Lists[i] = new RenderStateListClass(ini, section, &Unknown_BDC0A0);
		i++;
	}
}

// 0x763FF0
void RSLRegistryClass::Unknown_763FF0()
{
	for (int i = 0; i < Lists.Length(); i++) {
		if (Lists[i] != NULL) {
			delete Lists[i];
		}
	}
}

// 0x764030
void *RSLRegistryClass::Unknown_764030(int rslid)
{
	if (rslid >= Lists.Length()) {
		return(NULL);
	}
	return(Lists[rslid]);
}

// 0x764060
RenderStateListClass *RSLRegistryClass::Unknown_764060(const char *name)
{
	INIClass ini;
	int id = Lists.Length();
	char section[256];
	sprintf(section, "%d\\STATE0", id);
	ini.Put_String(section, "ID", "0");
	ini.Put_String(section, "TYPE", "object");
	ini.Put_String(section, "OBJECT", name);
	char file[256];
	sprintf(file, "%s.W3D", name);
	ini.Put_String(section, "FILE", file);
	Lists.Resize(id + 1);
	sprintf(section, "%d", id);
	Lists[id] = new RenderStateListClass(ini, section, &Unknown_BDC0A0);
	return(Lists[id]);
}

/*
** Reconstruction notes (RS1, 2026-11-07)
**
** The object is a VectorClass<RenderStateListClass *> member, not a VectorClass subclass: the constructor and
** destructor store VectorClass's own vtable 0xB03EC8, and ClientClass's destructor (0x726C60, exact) calls 0x763E30
** directly and then operator delete. Lists.Resize goes through the vtable (+0x08) as the client's does. The
** template copies 0x764220-0x764410 (VectorClass's ==, Resize, Clear, the two IDs, the deleting destructor) are
** this file's.
*/
