// Earth & Beyond client (reconstruction): app\shared\rs_list.cpp, client 0x45A2B0-0x45C9D9 and its template copies
// to 0x45D820 (__FILE__ e:\build\beta\G\app\shared\rs_list.cpp in its asserts, lines 198 / 215 / 226 / 625 / 656).
// The render state list: built from rs.ini's '<n>' sections (RST_ flags, then one state per '<n>\STATE<i>',
// made by TYPE through the state maker 0xBDC0A0). Notes at the end.
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include "rs_track.h"
#include "ini.h"
#include "assetmgr.h"
#include "hanim.h"
#include "shader.h"
#include "mapper.h"
#include "..\shared\g_assert.h"

void Unknown_Debug_Print(const char *text);			// 0x509E30 (dbgmgr.cpp, D38)

Unknown45C430Class Unknown_BDC0A0;

// 0x45A2D0
int RenderStateListClass::Unknown_45A2D0(INIClass &ini, const char *section)
{
	return(ini.Get_Int(section, "ID", -1));
}

// 0x45A300
int RenderStateListClass::Unknown_45A300(INIClass &ini, const char *section, char *buffer)
{
	return(ini.Get_String(section, "FILE", "", buffer, MAX_PATH));
}

// 0x45A340
void RenderStateListClass::Unknown_45A340(const char *filename)
{
	WW3DAssetManager::Get_Instance()->Load_3D_Assets(filename);
}

// 0x45A370
int RenderStateListClass::Unknown_45A370(INIClass &ini, const char *section, char *buffer)
{
	return(ini.Get_String(section, "OBJECT", "", buffer, MAX_PATH));
}

// 0x45A3B0
int RenderStateListClass::Unknown_45A3B0(INIClass &ini, const char *section, char *anim, char *suffix)
{
	ini.Get_String(section, "ANIM", "", anim, MAX_PATH);
	ini.Get_String(section, "SUFFIX", "", suffix, MAX_PATH);
	return(strlen(anim));
}

// 0x45A430
int RenderStateListClass::Unknown_45A430(INIClass &ini, const char *section, char *buffer)
{
	return(ini.Get_String(section, "BONE", "", buffer, MAX_PATH));
}

// 0x45A470
int RenderStateListClass::Unknown_45A470(INIClass &ini, const char *section)
{
	return(ini.Get_Int(section, "NEXT", -1));
}

// 0x45A4A0
int RenderStateListClass::Unknown_45A4A0(INIClass &ini, const char *section)
{
	return(ini.Get_Int(section, "LINK", -1));
}

// 0x45A4D0
int RenderStateListClass::Unknown_45A4D0(INIClass &ini, const char *section)
{
	return(ini.Get_Int(section, "MESSAGE", 0));
}

// 0x45A500
float RenderStateListClass::Unknown_45A500(INIClass &ini, const char *section)
{
	return(ini.Get_Float(section, "DURATION", 0));
}

// 0x45A530
unsigned int RenderStateListClass::Unknown_45A530(INIClass &ini, const char *section)
{
	unsigned int flags = 0;
	if (ini.Get_Bool(section, "RS_DELETE", false)) flags |= 0x1;
	if (ini.Get_Bool(section, "RS_DEACTIVATE", false)) flags |= 0x2;
	if (ini.Get_Bool(section, "RS_STOP", false)) flags |= 0x4;
	if (ini.Get_Bool(section, "RS_ACTIVATE_NEXT", false)) flags |= 0x8;
	if (ini.Get_Bool(section, "RS_ENABLED", false)) flags |= 0x10;
	if (ini.Get_Bool(section, "RS_REMOVE_PROTOTYPE", false)) flags |= 0x800;
	if (ini.Get_Bool(section, "RS_PRECACHED", false)) flags |= 0x100;
	if (ini.Get_Bool(section, "RS_INSTANCE_DATA", false)) flags |= 0x400;
	if (ini.Get_Bool(section, "RS_ACTIVATE_NEXT_ONCE", false)) flags |= 0x20;
	if (ini.Get_Bool(section, "RS_STOP_ONCE", false)) flags |= 0x40;
	if (ini.Get_Bool(section, "RS_STATIONARY", false)) flags |= 0x4000;
	if (ini.Get_Bool(section, "RS_STARTS_ENABLED", false)) flags |= 0x10000;
	if (ini.Get_Bool(section, "RS_DONT_STOP_SOUND", false)) flags |= 0x20000;
	if (ini.Get_Bool(section, "RS_DONT_LOOP_SOUND", false)) flags |= 0x40000;
	return(flags);
}

// 0x45A6F0
char *RenderStateListClass::Unknown_45A6F0(INIClass &ini, const char *section)
{
	return(ini.Get_Alloc_String(section, "INITSTR", NULL));
}

// 0x45A720
RenderStateClass *RenderStateListClass::Unknown_45A720(int id)
{
	for (RenderStateClass *state = Head; state != NULL; state = state->Next) {
		if (state->ID == id) {
			return(state);
		}
	}
	return(NULL);
}

// 0x45A750
RenderStateClass *RenderStateListClass::Unknown_45A750(int id)
{
	if (id != -1) {
		RenderStateClass *state = Unknown_45A720(id);
		if (state != NULL) {
			return(state);
		}
		if (Source != NULL) {
			for (RenderStateClass *source = Source->Head; source != NULL; source = source->Next) {
				if (source->ID == id) {
					source->Unknown_10();
					state = source->Unknown_04(this);
					state->Unknown_457F60(this);
					return(state);
				}
			}
		}
	}
	return(NULL);
}

// 0x45A7E0
RenderStateClass *RenderStateListClass::Unknown_45A7E0(int id)
{
	if (id != -1) {
		RenderStateClass *state = Unknown_45A720(id);
		if (state != NULL) {
			return(state);
		}
		if (Source != NULL) {
			for (RenderStateClass *source = Source->Head; source != NULL; source = source->Next) {
				if (source->ID == id) {
					source->Unknown_10();
					state = source->Unknown_04(this);
					state->Unknown_457F60(this);
					return(state);
				}
			}
		}
	}
	return(NULL);
}






























// 0x45A870
void RenderStateListClass::Unknown_45A870(RsTrackClass *owner, float seconds)
{
	for (RenderStateClass *state = Unknown08.First_Valid(); state != NULL; state = state->Next_Valid()) {
		G_ASSERT(state->Is_Enabled());
		state->Unknown_30(owner, seconds);
	}
}









// 0x45A8F0
void RenderStateListClass::Unknown_45A8F0(RsTrackClass *owner, RenderInfoClass &rinfo)
{
	for (RenderStateClass *state = Unknown08.First_Valid(); state != NULL; state = state->Next_Valid()) {
		G_ASSERT(state->Is_Enabled());
		state->Unknown_28((int)owner, (int)&rinfo);
	}
}



// 0x45A970
void RenderStateListClass::Unknown_45A970(RsTrackClass *owner, RenderInfoClass &rinfo)
{
	for (RenderStateClass *state = Unknown08.First_Valid(); state != NULL; state = state->Next_Valid()) {
		G_ASSERT(state->Is_Enabled());
		state->Unknown_2C((int)owner, (int)&rinfo);
	}
}

// 0x45A9F0
void RenderStateListClass::Unknown_45A9F0(RsTrackClass *owner)
{
	for (RenderStateClass *state = Unknown08.First_Valid(); state != NULL; state = state->Next_Valid()) {
		state->Unknown_38((int)owner);
	}
}

// 0x45AA40
void RenderStateListClass::Unknown_45AA40(void *value, bool recursive)
{
	for (RenderStateClass *state = Head; state != NULL; state = state->Next) {
		state->Unknown_24(value, recursive);
	}
}

// 0x45AA80
RenderStateClass *RenderStateListClass::Unknown_45AA80(int id, int link_id, unsigned int flags, const char *name, int next_id, float duration)
{
	RenderStateClass *state = new Unknown45E850Class(this, id, link_id, flags, name, next_id, duration);
	state->Unknown_457F60(this);
	return(state);
}

// 0x45AB20
// NOT EXACT: schedule (0.86): the client stores the float DURATION before the next push
RenderStateClass *RenderStateListClass::Unknown_45AB20(INIClass &ini, const char *section, int id)
{
	char name[MAX_PATH];
	if (Unknown_45A370(ini, section, name) > 0) {
		unsigned int flags = Unknown_45A530(ini, section);
		char file[MAX_PATH];
		char *filename = Unknown_45A300(ini, section, file) ? file : NULL;
		if (flags & 0x100) {
			Unknown_45A340(filename);
		}
		int next_id = Unknown_45A470(ini, section);
		int link_id = Unknown_45A4A0(ini, section);
		char *initstr = Unknown_45A6F0(ini, section);
		float duration = Unknown_45A500(ini, section);
		RenderStateClass *state = Unknown_45AA80(id, link_id, flags, name, next_id, duration);
		if (filename != NULL) {
			state->Unknown_457EE0(filename);
		}
		if (initstr != NULL) {
			state->Unknown_458540(initstr);
			free(initstr);
		}
		return(state);
	}
	return(NULL);
}

// 0x45ACD0
RenderStateClass *RenderStateListClass::Unknown_45ACD0(int id, int link_id, unsigned int flags, const char *anim, const char *suffix, int next_id, HAnimComboDataClass *combo)
{
	RenderStateClass *state = new Unknown4596A0Class(this, id, link_id, flags, anim, suffix, next_id, combo);
	state->Unknown_457F60(this);
	return(state);
}

// 0x45AD90
// NOT EXACT: regs (0.67): the client keeps 0 in ebp from the start and one more local
RenderStateClass *RenderStateListClass::Unknown_45AD90(INIClass &ini, const char *section, int id)
{
	char file[MAX_PATH];
	if (Unknown_45A300(ini, section, file)) {
		Unknown_45A340(file);
	}
	char anim[MAX_PATH];
	char suffix[MAX_PATH];
	Unknown_45A3B0(ini, section, anim, suffix);
	int next_id = Unknown_45A470(ini, section);
	int link_id = Unknown_45A4A0(ini, section);
	unsigned int flags = Unknown_45A530(ini, section);
	HAnimComboDataClass combo(false);
	HAnimComboDataClass *data;
	char name[MAX_PATH];
	sprintf(name, "%s\\PIVOTMAP0", section);
	if (!ini.Is_Present(name)) {
		data = NULL;
	} else {
		data = &combo;
		NamedPivotMapClass *map = new NamedPivotMapClass;
		int index = 0;
		while (ini.Is_Present(name)) {
			char bone[MAX_PATH];
			if (Unknown_45A430(ini, name, bone) > 0) {
				map->Add(bone, ini.Get_Float(name, "WEIGHT", 0));
			}
			index++;
			sprintf(name, "%s\\PIVOTMAP%d", section, index);
		}
	}
	return(Unknown_45ACD0(id, link_id, flags, anim, suffix, next_id, data));
}

// 0x45B1E0
RenderStateClass *RenderStateListClass::Unknown_45B1E0(int id, int link_id, unsigned int flags, int message)
{
	RenderStateClass *state = new Unknown45D840Class(this, id, link_id, flags, message);
	state->Unknown_457F60(this);
	return(state);
}

// 0x45B280
RenderStateClass *RenderStateListClass::Unknown_45B280(INIClass &ini, const char *section, int id)
{
	int message = Unknown_45A4D0(ini, section);
	unsigned int flags = Unknown_45A530(ini, section);
	int link_id = Unknown_45A4A0(ini, section);
	return(Unknown_45B1E0(id, link_id, flags, message));
}

// 0x45B300
RenderStateClass *RenderStateListClass::Unknown_45B300(int id, int link_id, unsigned int flags)
{
	RenderStateClass *state = new Unknown45A090Class(this, id, link_id, flags);
	state->Unknown_457F60(this);
	return(state);
}

// 0x45B390
RenderStateClass *RenderStateListClass::Unknown_45B390(INIClass &ini, const char *section, int id)
{
	unsigned int flags = Unknown_45A530(ini, section);
	int link_id = Unknown_45A4A0(ini, section);
	return(Unknown_45B300(id, link_id, flags));
}

// 0x45B3E0
RenderStateClass *RenderStateListClass::Unknown_45B3E0(int id, int link_id, unsigned int flags, int message, const char *sound)
{
	RenderStateClass *state = new Unknown45F390Class(this, id, link_id, flags, sound, message);
	state->Unknown_457F60(this);
	return(state);
}

// 0x45B480
RenderStateClass *RenderStateListClass::Unknown_45B480(INIClass &ini, const char *section, int id)
{
	int message = Unknown_45A4D0(ini, section);
	int link_id = Unknown_45A4A0(ini, section);
	unsigned int flags = Unknown_45A530(ini, section);
	char sound[MAX_PATH];
	if (ini.Get_String(section, "SOUND", "", sound, MAX_PATH) > 0) {
		return(Unknown_45B3E0(id, link_id, flags, message, sound));
	}
	return(Unknown_45B3E0(id, link_id, flags, message, NULL));
}













































































































































































// 0x45B550
// NOT EXACT: switch (0.71): the client's SHADER jump table starts at 0 (case 0 on the default's code); ours subtracts 1
RenderStateClass *RenderStateListClass::Unknown_45B550(const char *filename, int link_id, unsigned int flags, int id, int next_id)
{
	INIClass ini;
	ini.Load(filename);
	int type = ini.Get_List_Index("Effect_Pass_Description", "EFFECT_PASS_TYPE", 0, Unknown_B70E40);
	char texture[MAX_PATH];
	ini.Get_String("Effect_Pass_Description", "TEXTURE", "", texture, MAX_PATH);
	ShaderClass shader;
	switch (ini.Get_Int("Effect_Pass_Description", "SHADER", 0)) {
		case 1:
			shader = ShaderClass::_PresetAdditiveSpriteShader;
			break;
		case 2:
			shader = ShaderClass::_PresetAlphaSpriteShader;
			break;
		case 3:
			shader = ShaderClass::_PresetATestSpriteShader;
			break;
		case 4:
			shader = ShaderClass::_PresetATestBlendSpriteShader;
			break;
		case 5:
			shader = ShaderClass::_PresetScreenSpriteShader;
			break;
		case 6:
			shader = ShaderClass::_PresetMultiplicativeSpriteShader;
			break;
		case 0:
			shader = ShaderClass::_PresetOpaqueSolidShader;
			break;
		default:
			shader = ShaderClass::_PresetOpaqueSolidShader;
			break;
	}
	bool two_sided = ini.Get_Bool("Effect_Pass_Description", "TWO_SIDED", false);
	bool hidden = ini.Get_Bool("Effect_Pass_Description", "HIDDEN", false);
	if (two_sided && !hidden) {
		Unknown_Debug_Print("RenderStateEffectPassClass::Two-sided is set, and hidden is not.");
	}
	switch (type) {
		case 2:
		{
			Unknown458C10Class *pass = new Unknown458C10Class(this, id, link_id, flags, two_sided, hidden, texture, 1, 1, shader);
			TextureMapperClass *mapper = new EnvironmentMapperClass(0);
			pass->Unknown_459120(mapper);
			mapper->Release_Ref();
			return(pass);
		}
		case 3:
		{
			Unknown458C10Class *pass = new Unknown458C10Class(this, id, link_id, flags, two_sided, hidden, texture, 1, 1, shader);
			TextureMapperClass *mapper = NULL;
			pass->Unknown_459120(mapper);
			mapper->Release_Ref();
			return(pass);
		}
		default:
		{
			if (type != 0) {
				type = 1;
			}
			float cycle_time = ini.Get_Float("Effect_Pass_Description", "CYCLE_TIME", 0);
			float start_v = ini.Get_Float("Effect_Pass_Description", "START_V_COORD", 0);
			float end_v = ini.Get_Float("Effect_Pass_Description", "END_V_COORD", 1.0f);
			switch (type) {
				case 0:
				case 1:
					break;
				default:
					G_ASSERT(0);
					break;
			}
			return(NULL);
		}
	}
}










// 0x45B990
RenderStateClass *RenderStateListClass::Unknown_45B990(INIClass &ini, const char *section, int id)
{
	int link_id = Unknown_45A4A0(ini, section);
	unsigned int flags = Unknown_45A530(ini, section);
	int next_id = Unknown_45A470(ini, section);
	char file[MAX_PATH];
	if (Unknown_45A300(ini, section, file) && file != NULL) {
		RenderStateClass *state = Unknown_45B550(file, link_id, flags, id, next_id);
		if (state != NULL) {
			state->Unknown_457F60(this);
		}
		return(state);
	}
	G_ASSERT(0);
	return(NULL);
}

// 0x45BAB0
RenderStateClass *RenderStateListClass::Unknown_45BAB0(int id, int link_id, unsigned int flags, const char *name, int next_id, float duration, const char *bone, int message)
{
	RenderStateClass *state = new Unknown458710Class(this, id, link_id, flags, name, next_id, duration, bone, message);
	state->Unknown_457F60(this);
	return(state);
}

// 0x45BB70
RenderStateClass *RenderStateListClass::Unknown_45BB70(int id, int link_id, unsigned int flags, RenderObjClass *robj, int next_id, float duration, const char *bone, int message)
{
	RenderStateClass *state = new Unknown458710Class(this, id, link_id, flags, robj, next_id, duration, bone, message);
	state->Unknown_457F60(this);
	return(state);
}

// 0x45BC30
// NOT EXACT: schedule (0.86): the client stores the float DURATION before the next push
RenderStateClass *RenderStateListClass::Unknown_45BC30(INIClass &ini, const char *section, int id)
{
	char name[MAX_PATH];
	char bone[MAX_PATH];
	char *bone_path = Unknown_45A430(ini, section, bone) > 0 ? bone : NULL;
	int link_id = Unknown_45A4A0(ini, section);
	unsigned int flags = Unknown_45A530(ini, section);
	int name_len = Unknown_45A370(ini, section, name);
	int message = Unknown_45A4D0(ini, section);
	int next_id = Unknown_45A470(ini, section);
	float duration = Unknown_45A500(ini, section);
	char *initstr = Unknown_45A6F0(ini, section);
	char file[MAX_PATH];
	Unknown_45A300(ini, section, file);
	char *filename;
	char *object;
	if (name_len > 0) {
		Unknown_45A300(ini, section, file);
		filename = file;
		if (flags & 0x8) {
			Unknown_45A340(file);
		}
		object = name;
	} else {
		filename = NULL;
		object = NULL;
	}
	RenderStateClass *state = Unknown_45BAB0(id, link_id, flags, object, next_id, duration, bone_path, message);
	state->Unknown_457EE0(filename);
	if (initstr != NULL) {
		state->Unknown_458540(initstr);
		free(initstr);
	}
	return(state);
}

// 0x45BE40
RenderStateListClass::RenderStateListClass(INIClass &ini, const char *section, Unknown45C430Class *factory) :
	Head(NULL),
	Current(NULL),
	Source(NULL),
	Unknown2C(0)
{
	if (ini.Get_Bool(section, "RST_USE_CULLING", false)) Unknown2C |= 0x08;
	if (ini.Get_Bool(section, "RST_HINT_STATIONARY", false)) Unknown2C |= 0x10;
	if (ini.Get_Bool(section, "RST_SHADOW_SOURCE", false)) Unknown2C |= 0x28;
	if (ini.Get_Bool(section, "RST_PROJECTION_TARGET", false)) Unknown2C |= 0x48;
	if (ini.Get_Bool(section, "RST_SIMPLE_SHADOW", false)) Unknown2C |= 0xA8;
	char type[MAX_PATH];
	char name[MAX_PATH];
	int index = 0;
	while (true) {
		sprintf(name, "%s\\STATE%d", section, index);
		if (!ini.Is_Present(name)) {
			break;
		}
		if (ini.Get_String(name, "TYPE", "", type, MAX_PATH)) {
			for (char *c = type; *c != '\0'; c++) {
				*c = (char)tolower(*c);
			}
			factory->Unknown_00(this, ini, name, type, index);
		}
		index++;
	}
}

// 0x45C1B0
RenderStateListClass::RenderStateListClass(RenderStateListClass *source) :
	Head(NULL),
	Current(NULL),
	Source(source),
	Unknown2C(source->Unknown2C)
{
}

// 0x45C270
RenderStateListClass::RenderStateListClass() :
	Head(NULL),
	Current(NULL),
	Unknown2C(0)
{
}

// 0x45C310
RenderStateListClass::~RenderStateListClass()
{
	if (Head != NULL) {
		delete Head;
	}
}

// 0x45C430
// NOT EXACT: inlining (0.62): the client calls the effect pass maker 0x45B990 out of line; ours inlines it
int Unknown45C430Class::Unknown_00(RenderStateListClass *list, INIClass &ini, const char *section, const char *type, int index)
{
	int id = ini.Get_Int(section, "ID", index);
	if (strcmp(type, "object") == 0) {
		list->Unknown_45AB20(ini, section, id);
		return(0);
	}
	if (strcmp(type, "message") == 0) {
		list->Unknown_45B280(ini, section, id);
		return(1);
	}
	if (strcmp(type, "link") == 0) {
		list->Unknown_45B390(ini, section, id);
		return(5);
	}
	if (strcmp(type, "bone") == 0) {
		list->Unknown_45BC30(ini, section, id);
		return(2);
	}
	if (strcmp(type, "effect_pass") == 0) {
		list->Unknown_45B990(ini, section, id);
		return(6);
	}
	if (strcmp(type, "sound") == 0) {
		list->Unknown_45B480(ini, section, id);
		return(3);
	}
	if (strcmp(type, "anim") == 0) {
		list->Unknown_45AD90(ini, section, id);
		return(4);
	}
	return(-1);
}

// 0x45C8B0
void RenderStateListClass::Unknown_45C8B0()
{
	for (RenderStateClass *state = Head; state != NULL; state = state->Next) {
		state->Unknown_0C();
	}
}

// 0x45C8E0
void RenderStateListClass::Unknown_45C8E0(int id)
{
	RenderStateClass *state = Unknown_45A720(id);
	if (state != NULL) {
		state->Unknown_0C();
	}
}

// 0x45C920
void RenderStateListClass::Unknown_45C920()
{
	for (RenderStateClass *state = Head; state != NULL; state = state->Next) {
		state->Unknown_10();
	}
}

// 0x45C950
void RenderStateListClass::Unknown_45C950(int id)
{
	RenderStateClass *state = Unknown_45A720(id);
	if (state != NULL) {
		state->Unknown_10();
	}
}

// 0x45C990
void RenderStateListClass::Unknown_04(float scale, float *hsv)
{
	if (Source != NULL) {
		for (RenderStateClass *source = Source->Head; source != NULL; source = source->Next) {
			source->Unknown_10();
			RenderStateClass *state = source->Unknown_04(this);
			state->Unknown_40(scale, hsv);
			state->Unknown_457F60(this);
		}
	}
}

/*
** Reconstruction notes (RS1, 2026-11-07)
**
** Extent: the global maker's initialiser 0x45A2B0 (it stores vtable 0xAE7F30 into 0xBDC0A0) opens the object; the
** literals start at 'ID' 0xB70F94 (0x45A2D0's), so 0x45A2D0-0x45A6F0 are this file's INI readers, not rs_hanim.cpp's.
** 0x45C8B0-0x45C950 have no callers (incremental link, no /OPT:REF). The message state 0x45D840-0x45D9C6 is a
** separate object (unknown_45d840.cpp): the maker 0x45B1E0 calls its constructor through a thunk.
**
** The readers are RenderStateListClass members whose this goes unused (thiscall, ret 8 / 0xC: 0x45B280 passes
** ecx); /Ob2 inlines them into the makers too. The client's 'state->Is_Enabled()' (lines 198 / 215 / 226) names
** RenderStateClass's flag 4 accessor. The rs.ini constructor's section test is INIClass::Is_Present (setne / cmp):
** Find_Section and Section_Present give a plain test. The anim maker 0x45AD90 makes a NamedPivotMapClass from the
** '<section>\PIVOTMAP<n>' sections and never hands it to the combo (the client's code: no Set_Pivot_Map call). The
** effect pass maker reads CYCLE_TIME / START_V_COORD / END_V_COORD and drops them for the types other than
** Environment (2) and Screen (3), and releases its NULL mapper for Screen (the client reads address 4 there).
**
** NOT EXACT (functional, D64): 0x45AB20 and 0x45BC30 (the float DURATION's store is scheduled before the next push
** in the client), 0x45AD90 (register allocation: the client keeps 0 in ebp from the start, one more local),
** 0x45B550 (the SHADER switch: the client's jump table starts at 0 with case 0 on the default's code; ours
** subtracts 1), 0x45C430 (the client calls the effect pass maker 0x45B990 out of line; ours inlines it).
*/
