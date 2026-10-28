/*
**	Earth & Beyond client (reconstruction)
**	Portions Copyright Electronic Arts Inc., released under the GNU General Public License v3.
**
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\Client\eff_o.cpp (the client's __FILE__)
**
**	The effect object's base (client 0x6E6710-0x6E6F60 so far): see eff_o.h. Notes at the end.
*/

#include "eff_o.h"
#include "client.h"
#include "co.h"
#include "rendobj.h"
#include "camera.h"
#include "random.h"
#include "wwmath.h"
#include "ini.h"
#include "colmath.h"
#include "lineseg.h"
#include "sphere.h"
#include "castres.h"
#include "nstrdup.h"
#include "..\audio\soundlib.h"
#include "..\..\wwaudio\Sound3D.h"

void Unknown_4BC260(const char *sender, const char *category, const char *file, int line, const char *format, ...);	// 0x4BC260 (unknown_debug.h's reporter)
EffectObjectClass *Unknown_6E9490(int desc_id, int effect_id, int duration, int time, ClientClass *client, float scale, const Vector3 &hsv_shift, float speedup, int linked_desc_id, bool unknown);	// 0x6E9490 (the factory, not rebuilt yet)  // NAME 1 [desc_id .. unknown]: what the effect packets pass
Matrix3D Unknown_82ECB0(const Vector3 &direction, const Vector3 &position);	// 0x82ECB0 (cdecl; a transform at the position facing along the direction)  // NAME 1 [direction, position]: what 0x6E8DD0 passes
// The render-state effect the factory's helper 0x6EC7D0 makes (0xE4 bytes; its file, between eff_robj.cpp and
// eff_seek.cpp, is not rebuilt): declared as far as the helper calls it
class Unknown6F0200EffectClass : public EffectObjectClass { public: Unknown6F0200EffectClass(ClientClass *client, unsigned id, unsigned start_time, unsigned duration, unsigned cycle_time, int render_state, int action, int action_time); virtual void Unknown_24(int next); char Unknown80[0xE4 - 0x80]; };	// NAME 1: neutral (D10); 0x6F0200  // NAME 1 [client .. action_time]: effect.ini's RenderStateID, ActionType, ActionTime (ms); [next]: the base's
// eff_seek.cpp's effect on a model that the helper 0x6EAFA0 makes (0xE8 bytes; not rebuilt): declared as far as it calls it
class Unknown6F07F0EffectClass : public EffectObjectClass { public: Unknown6F07F0EffectClass(ClientClass *client, unsigned id, unsigned start_time, unsigned duration, unsigned cycle_time, RenderObjClass *robj); virtual void Unknown_24(int next); char Unknown80[0xE8 - 0x80]; };	// NAME 1: neutral (D10); 0x6F07F0  // NAME 1 [client .. robj]: what the helper passes; [next]: the base's
// eff_seek.cpp's spiral effect that the helper 0x6EB150 makes (0xF0 bytes; not rebuilt)
class Unknown6F0B10EffectClass : public EffectObjectClass { public: Unknown6F0B10EffectClass(ClientClass *client, unsigned id, unsigned start_time, unsigned duration, unsigned cycle_time, RenderObjClass *robj, float radius, float spin_rate); virtual void Unknown_24(int next); char Unknown80[0xF0 - 0x80]; };	// NAME 1: neutral (D10); 0x6F0B10  // NAME 1 [client .. spin_rate]: effect.ini's SpiralRadius, SpiralSpinRate (radians per ms); [next]: the base's
// eff_seek.cpp's changing effect that the helper 0x6EB330 makes (0x12C bytes; not rebuilt)
class Unknown6F0FD0EffectClass : public EffectObjectClass { public: Unknown6F0FD0EffectClass(ClientClass *client, unsigned id, unsigned start_time, unsigned duration, unsigned cycle_time, RenderObjClass *robj, int change_time, float distance); virtual void Unknown_24(int next); char Unknown80[0x12C - 0x80]; };	// NAME 1: neutral (D10); 0x6F0FD0  // NAME 1 [client .. distance]: effect.ini's ChangeTime (ms), Distance; [next]: the base's
// eff_seek.cpp's timed changing effect that the helper 0x6EB590 makes (0x114 bytes; not rebuilt)
class Unknown6F1760EffectClass : public EffectObjectClass { public: Unknown6F1760EffectClass(ClientClass *client, unsigned id, unsigned start_time, unsigned duration, unsigned cycle_time, RenderObjClass *robj, int change_time, int time_to_change, float distance); virtual void Unknown_24(int next); char Unknown80[0x114 - 0x80]; };	// NAME 1: neutral (D10); 0x6F1760  // NAME 1 [client .. distance]: effect.ini's ChangeTime, TimeToChange (ms), Distance; [next]: the base's
class Unknown6EFC70EffectClass : public EffectObjectClass { public: Unknown6EFC70EffectClass(ClientClass *client, unsigned id, unsigned start_time, unsigned duration, unsigned cycle_time, RenderObjClass *robj); virtual void Unknown_24(int next); char Unknown80[0xE4 - 0x80]; };	// NAME 1: neutral (D10); 0x6EFC70, eff_robj.cpp's effect of 0x6EA5E0 (not rebuilt)  // NAME 1 [client .. robj, next]: what the helper passes
class Unknown6EEEA0EffectClass : public EffectObjectClass { public: Unknown6EEEA0EffectClass(ClientClass *client, unsigned id, unsigned start_time, unsigned duration, unsigned cycle_time, RenderObjClass *robj); virtual void Unknown_24(int next); char Unknown80[0xDC - 0x80]; };	// NAME 1: neutral (D10); 0x6EEEA0, eff_robj.cpp's effect of 0x6EA830 (not rebuilt)  // NAME 1 [client .. robj, next]: what the helper passes
class Unknown6F25A0EffectClass : public EffectObjectClass { public: Unknown6F25A0EffectClass(ClientClass *client, unsigned id, unsigned start_time, unsigned duration, unsigned cycle_time, RenderObjClass *robj); virtual void Unknown_24(int next); char Unknown80[0xE0 - 0x80]; };	// NAME 1: neutral (D10); 0x6F25A0, the effect of 0x6EAA40 (its file after eff_seek.cpp) (not rebuilt)  // NAME 1 [client .. robj, next]: what the helper passes
void Unknown_496AE0(RenderObjClass *robj, int unknown);	// 0x496AE0 (ro_funcs.cpp, cdecl: effect.ini's ResetMappers)  // NAME 1 [robj, unknown]: what the helpers pass
extern RandomClass Unknown_BDBB84;	// NAME 1 [Unknown_BDBB84]: neutral (D10), as ui_line_display.cpp's
static inline int Unknown_Time(const ClientClass *client) { return(client->Unknown10BC); }	// NAME 1 [Unknown_Time]: neutral (D10): the client clock as the link calls read it (evaluated before the other arguments)

// 0x6E6710
EffectObjectClass::EffectObjectClass(ClientClass *client, unsigned id, unsigned start_time, unsigned duration, unsigned cycle_time) :
	Client(client),
	StartTime(start_time),
	CycleTime(cycle_time),
	ID(id),
	Unknown34(NULL),
	Unknown38(NULL),
	Unknown3C(0),
	Unknown40(0),
	Unknown44(0),
	Unknown48(0),
	Unknown4C(NULL),
	Camera(NULL),
	StartLinkID(-1),
	NextLinkID(-1),
	StartLink(NULL),
	NextLink(NULL),
	Unknown64(0),
	Scale(1.0f),
	HSVShift(0, 0, 0),
	Speedup(1.0f),
	Flags(0)
{
	Unknown_6E6F90(duration);
	Node.Set(this);
	Flag.State = 0;
}

// 0x6E6850
void EffectObjectClass::Unknown_04(int)
{
}

// 0x6E6870
Vector3 EffectObjectClass::Unknown_08(const Vector3 &)
{
	return(Vector3(0, 0, 0));
}

// 0x6E68A0
void EffectObjectClass::Unknown_18()
{
}

// 0x6E68C0
void EffectObjectClass::Unknown_1C()
{
}

// 0x6E68E0
void EffectObjectClass::Unknown_20()
{
}

// 0x6E6A90
void Unknown_6E6A90(EffectObjectClass *effect)
{
	if (effect->Flag.State == 1) {
		effect->Unknown_6E7330();
		effect->Unknown_20();
	}
	delete effect;
}

// 0x6E6930
EffectObjectClass::~EffectObjectClass()
{
	REF_PTR_RELEASE(Unknown4C);
	REF_PTR_RELEASE(Camera);
	if (Unknown34) {
		delete [] Unknown34;
		Unknown34 = NULL;
	}
	if (StartLink) {
		Unknown_6E6A90(StartLink);
	}
	if (NextLink) {
		Unknown_6E6A90(NextLink);
	}
}






















// 0x6E6AD0
bool EffectObjectClass::Unknown_6E6AD0()
{
	FAIL_IF(!Camera) return(true);
	bool done = false;
	switch (Flag.State) {
		case 0:
			if (Client->Unknown10BC - StartTime >= 0x80000000) {
				break;
			}
			StartTime = Client->Unknown10BC;
			Flag.State = 1;
			if (!(bool)Flag.Unknown3) {
				Unknown_6E70F0();
			}
			Unknown_18();
			Unknown_6E6FE0();
		case 1:
			if (Duration != (unsigned)-1 && (done = (Client->Unknown10BC - StartTime - Duration < 0x80000000)) != false) {
				Flag.State = 2;
				Unknown_20();
				if (NextLinkID >= 0) {
					NextLink = Unknown_6E9490(NextLinkID, 0, 0, Unknown_Time(Client), Client, Scale, HSVShift, Speedup, -1, false);
					Unknown_24(1);
				}
				Unknown_6E7330();
			} else {
				Unknown_1C();
				if ((bool)Flag.Unknown3) {
					Vector3 camera = Camera->Get_Position();
					if ((Unknown_08(camera) - camera).Length2() <= Unknown3C && Client->Unknown10BC - Unknown48 < 0x80000000) {
						Unknown_6E70F0();
						int delay = Unknown_BDBB84(Unknown40, Unknown44);
						Unknown48 = Client->Unknown10BC + delay;
					}
				}
				if (Unknown38) {
					Sound3DClass *sound = Unknown38->As_Sound3DClass();
					if (sound) {
						sound->Set_Position(Unknown_08(Camera->Get_Position()));
					}
				}
				break;
			}
		case 2:
			done = true;
			if (NextLink && !NextLink->Unknown_6E6AD0()) {
				Unknown_6E6A90(NextLink);
				NextLink = NULL;
			}
			break;
	}
	if (StartLink && !StartLink->Unknown_6E6AD0()) {
		Unknown_6E6A90(StartLink);
		StartLink = NULL;
	}
	return(!done || StartLink || NextLink);
}

// 0x6E6E30
void EffectObjectClass::Unknown_6E6E30()
{
	unsigned elapsed = Client->Unknown10BC - StartTime;
	if (elapsed < 0x80000000) {
		Unknown_6E6F90(elapsed);
	} else {
		Unknown_6E6F90(0);
	}
	Set_Flag(4, true);
	if (StartLink) {
		StartLink->Unknown_6E6E30();
	}
	if (NextLink) {
		NextLink->Unknown_6E6E30();
	}
}

// 0x6E6ED0
void EffectObjectClass::Unknown_6E6ED0(int arg)
{
	CameraClass *camera = (CameraClass *)arg;
	REF_PTR_SET(Camera, camera);
}

// 0x6E6F10
void EffectObjectClass::Unknown_0C(unsigned duration)
{
	Unknown_6E6F90(duration);
	if (StartLink && (bool)Flag.Unknown5) {
		StartLink->Unknown_0C(duration);
	}
}

// 0x6E6FE0
void EffectObjectClass::Unknown_6E6FE0()
{
	if (StartLinkID >= 0) {
		int start = Unknown_Time(Client) + Unknown64;
		unsigned duration = 0;
		if ((bool)Flag.Unknown5) {
			duration = Duration;
			if (!(bool)Flag.Unknown6) {
				if (duration > (unsigned)Unknown64) {
					duration -= Unknown64;
				} else {
					duration = 0;
				}
			}
		}
		StartLink = Unknown_6E9490(StartLinkID, 0, duration, start, Client, Scale, HSVShift, Speedup, -1, false);
		Unknown_24(0);
	}
}

// 0x6E7080
void EffectObjectClass::Unknown_6E7080()
{
	if (NextLinkID >= 0) {
		NextLink = Unknown_6E9490(NextLinkID, 0, 0, Unknown_Time(Client), Client, Scale, HSVShift, Speedup, -1, false);
		Unknown_24(1);
	}
}

// 0x6E70F0
void EffectObjectClass::Unknown_6E70F0()
{
	if (Client->Unknown_1C()) {
		return;
	}
	Unknown_6E7330();
	if (Unknown34 && *Unknown34) {
		if (!(bool)Flag.Unknown4) {
			Unknown38 = SoundLibraryClass::Play_3D_Sound(Unknown34, Matrix3D(true), 0, 1.0f);
		} else {
			Unknown38 = SoundLibraryClass::Play_2D_Sound(Unknown34, 0);
		}
	}
	if (Unknown38) {
		Sound3DClass *sound = Unknown38->As_Sound3DClass();
		if (sound) {
			sound->Set_Position(Unknown_08(Camera->Get_Position()));
			sound->Add_To_Scene(true);
		} else {
			Unknown38->Play(true);
		}
	}
}

// 0x6E7260
void EffectObjectClass::Unknown_6E7260()
{
	Vector3 camera = Camera->Get_Position();
	if ((Unknown_08(camera) - camera).Length2() <= Unknown3C && Client->Unknown10BC - Unknown48 < 0x80000000) {
		Unknown_6E70F0();
		int delay = Unknown_BDBB84(Unknown40, Unknown44);
		Unknown48 = Client->Unknown10BC + delay;
	}
}

// 0x6E7330
void EffectObjectClass::Unknown_6E7330()
{
	if (Unknown38) {
		if (Unknown38->Get_Loop_Count() == 0) {
			Sound3DClass *sound = Unknown38->As_Sound3DClass();
			if (sound) {
				if (Camera) {
					sound->Set_Position(Unknown_08(Camera->Get_Position()));
				}
				sound->Remove_From_Scene();
			} else {
				Unknown38->Stop(true);
			}
		}
		Unknown38->Release_Ref();
		Unknown38 = NULL;
	}
}

// 0x6E73F0
void EffectObjectClass::Unknown_6E73F0(const char *name, bool sfx_2d, float radius, int min_time, int max_time)
{
	Unknown_6E7330();
	if (Unknown34) {
		delete [] Unknown34;
	}
	Unknown34 = nstrdup(name);
	if (radius >= 0.0f) {
		Set_Flag(8, true);
		Unknown3C = radius * radius;
		Unknown40 = min_time;
		Unknown44 = max_time;
		Unknown48 = Client->Unknown10BC;
	} else {
		Set_Flag(8, false);
	}
	Set_Flag(0x10, sfx_2d);
}

// 0x6E74B0
void EffectObjectClass::Unknown_6E74B0(int arg)
{
	RenderObjClass *robj = (RenderObjClass *)arg;
	if (robj != Unknown4C) {
		if (Flag.State == 1) {
			Unknown_20();
			REF_PTR_SET(Unknown4C, robj);
			Unknown_18();
		} else {
			REF_PTR_SET(Unknown4C, robj);
		}
	}
}

// 0x6E7540
UnknownB00060EffectClass::UnknownB00060EffectClass(ClientClass *client, unsigned id, unsigned start_time, unsigned duration, unsigned cycle_time) :
	EffectObjectClass(client, id, start_time, duration, cycle_time),
	SourceObject(NULL),
	SourceNode(NULL),
	Transform(true)
{
}

// 0x6E7690
void UnknownB00060EffectClass::Unknown_04(int arg)
{
	if ((DataNode<UnknownAF0DD0Class *> *)arg == &SourceNode) {
		SourceObject = NULL;
		if ((bool)Flag.Unknown8) {
			Unknown_6E6E30();
		}
	}
}





























































































// 0x6E7770
void UnknownB00060EffectClass::Unknown_28(int game_id)
{
	SourceObject = Client->Find_Object(game_id);
	if (SourceObject) {
		Unknown_73FDB0(&SourceNode, SourceObject);
		Unknown_6E7AA0();
		return;
	}
	Unknown_4BC260("A/V Effects", "", __FILE__, __LINE__, "Set_Source_Object_ID couldn't find the object!");
	Flag.State = 2;
	Unknown_20();
	Unknown_6E7330();
}

// 0x6E7810
void UnknownB00060EffectClass::Unknown_6E7810(int game_id, const LegoIdentifierClass &lego)
{
	SourceLego = lego;
	Unknown_28(game_id);
}

// 0x6E7850
void UnknownB00060EffectClass::Unknown_18()
{
	if (SourceObject) {
		Unknown_6E7AA0();
	}
}

// 0x6E7870
void UnknownB00060EffectClass::Unknown_1C()
{
	if (SourceObject && (bool)Flag.Unknown7) {
		Unknown_6E7AA0();
	}
}
















// 0x6E78A0
void UnknownB00060EffectClass::Unknown_24(int next)
{
	EffectObjectClass *link = (next == 0) ? StartLink : NextLink;
	if (link) {
		link->Unknown_6E74B0((int)Unknown4C);
		link->Unknown_6E6ED0((int)Camera);
		if ((bool)Flag.Unknown2) {
			link->Unknown_6E6E30();
		}
		UnknownB00060EffectClass *unary = link->Unknown_10();
		if (unary) {
			if (SourceObject) {
				if (!SourceLego.Is_Empty()) {
					unary->Unknown_6E7810(SourceObject->Get_Game_ID(), SourceLego);
				} else {
					unary->Unknown_28(SourceObject->Get_Game_ID());
				}
			} else {
				unary->Unknown_2C(Transform);
			}
			return;
		}
		G_ASSERT(0);

		Unknown_4BC260("A/V Effects", "", __FILE__, __LINE__, "Unary effect linked to non-unary effect %d", (next == 0) ? StartLinkID : NextLinkID);
	}
}

// 0x6E7D70
UnknownB0009CEffectClass::UnknownB0009CEffectClass(ClientClass *client, unsigned id, unsigned start_time, unsigned duration, unsigned cycle_time) :
	EffectObjectClass(client, id, start_time, duration, cycle_time),
	SourceObject(NULL),
	TargetObject(NULL),
	SourceNode(NULL),
	TargetNode(NULL),
	SourcePosition(0, 0, 0),
	TargetPosition(0, 0, 0),
	TargetOffset(0, 0, 0)
{
}

// 0x6E7ED0
void UnknownB0009CEffectClass::Unknown_04(int arg)
{
	if ((DataNode<UnknownAF0DD0Class *> *)arg == &SourceNode) {
		SourceObject = NULL;
		if (Get_Flag(9)) {
			Unknown_6E6E30();
		}
	} else if ((DataNode<UnknownAF0DD0Class *> *)arg == &TargetNode) {
		TargetObject = NULL;
		if (Get_Flag(10)) {
			Unknown_6E6E30();
		}
	}
}








































// 0x6E80D0
void UnknownB0009CEffectClass::Unknown_28(int game_id)
{
	SourceObject = Client->Find_Object(game_id);
	if (SourceObject) {
		Unknown_73FDB0(&SourceNode, SourceObject);
		Unknown_6E8F80();
		return;
	}
	Unknown_4BC260("A/V Effects", "", __FILE__, __LINE__, "Set_Source_Object_ID couldn't find the object!");
	Flag.State = 2;
	Unknown_20();
	Unknown_6E7330();
}

// 0x6E8170
void UnknownB0009CEffectClass::Unknown_2C(int game_id, const LegoIdentifierClass &lego)
{
	SourceLego = lego;
	Unknown_28(game_id);
}






// 0x6E81B0
void UnknownB0009CEffectClass::Unknown_34(int game_id)
{
	TargetObject = Client->Find_Object(game_id);
	Unknown_73FDB0(&TargetNode, TargetObject);
	if (TargetObject) {
		TargetPosition = TargetObject->Unknown_24() + TargetOffset;
		return;
	}
	Unknown_4BC260("A/V Effects", "", __FILE__, __LINE__, "Set_Target_Object_ID couldn't find the object!");
	Flag.State = 2;
	Unknown_20();
	Unknown_6E7330();
}

// 0x6E87C0
void UnknownB0009CEffectClass::Unknown_6E87C0(const Vector3 &offset)
{
	TargetOffset = offset;
}

// 0x6E8800
void UnknownB0009CEffectClass::Unknown_18()
{
	if (SourceObject) {
		Unknown_6E8F80();
	}
	if (TargetObject) {
		TargetPosition = TargetObject->Unknown_24() + TargetOffset;
	}
}

// NOT EXACT: frame (0x8C bytes of locals, the client's 0x80: one Vector3 temporary more; the target's position plus TargetOffset also loads in the other order; 4 forms)
// 0x6E8890
void UnknownB0009CEffectClass::Unknown_1C()
{
	if (SourceObject && (bool)Flag.Unknown7) {
		Unknown_6E8F80();
	}
	if (TargetObject && (bool)Flag.Unknown8) {
		TargetPosition = TargetObject->Unknown_24() + TargetOffset;
	}
	if ((bool)Flag.Unknown13 && TargetObject) {
		SphereClass sphere(TargetObject->Unknown_24(), TargetObject->Unknown_30());
		LineSegClass line(SourcePosition, TargetPosition);
		CastResultStruct result;
		if (CollisionMath::Collide(line, sphere, &result)) {
			TargetPosition = line.Get_P0() + line.Get_DP() * result.Fraction;
		}
	}
}












































































// 0x6E8B60
void UnknownB0009CEffectClass::Unknown_24(int next)
{
	EffectObjectClass *link = (next == 0) ? StartLink : NextLink;
	if (link) {
		link->Unknown_6E74B0((int)Unknown4C);
		link->Unknown_6E6ED0((int)Camera);
		if ((bool)Flag.Unknown2) {
			link->Unknown_6E6E30();
		}
		UnknownB0009CEffectClass *binary = link->Unknown_14();
		if (binary) {
			if (SourceObject) {
				if (!SourceLego.Is_Empty()) {
					binary->Unknown_2C(SourceObject->Get_Game_ID(), SourceLego);
				} else {
					binary->Unknown_28(SourceObject->Get_Game_ID());
				}
			} else {
				binary->Unknown_30(SourcePosition);
			}
			if (TargetObject) {
				binary->Unknown_34(TargetObject->Get_Game_ID());
			} else {
				binary->Unknown_38(TargetPosition);
			}
			binary->TargetOffset = TargetOffset;
			return;
		}
		UnknownB00060EffectClass *unary = link->Unknown_10();
		if (unary) {
			Unknown_3C(unary, next);
			return;
		}
		G_ASSERT(0);

		Unknown_4BC260("A/V Effects", "", __FILE__, __LINE__, "Invalid effect %d (neither unary nor binary!)", (next == 0) ? StartLinkID : NextLinkID);
	}
}

// 0x6E8DD0
void UnknownB0009CEffectClass::Unknown_3C(UnknownB00060EffectClass *unary, int next)
{
	if (Flags & ((next != 0) ? 0x1000 : 0x800)) {
		if (TargetObject) {
			unary->Unknown_28(TargetObject->Get_Game_ID());
			return;
		}
		unary->Unknown_2C(Unknown_82ECB0(SourcePosition - TargetPosition, TargetPosition));
	} else {
		if (SourceObject) {
			if (!SourceLego.Is_Empty()) {
				unary->Unknown_6E7810(SourceObject->Get_Game_ID(), SourceLego);
			} else {
				unary->Unknown_28(SourceObject->Get_Game_ID());
			}
			return;
		}
		unary->Unknown_2C(Unknown_82ECB0(TargetPosition - SourcePosition, SourcePosition));
	}
}







// 0x6E8F80
void UnknownB0009CEffectClass::Unknown_6E8F80()
{
	FAIL_IF(!SourceObject) return;
	if (SourceLego.Is_Empty()) {
		SourcePosition = SourceObject->Unknown_24();
		return;
	}
	LegoIdentifierClass lego(SourceLego);
	lego.Add(std::string("EMITTER"));
	SourcePosition = SourceObject->Unknown_6FED80(lego).Get_Translation();
}









// 0x6E9190
void UnknownB0009CEffectClass::Unknown_6E9190()
{
	FAIL_IF(!TargetObject) return;
	TargetPosition = TargetObject->Unknown_24() + TargetOffset;
}

// 0x6E9230
UnknownB000F0EffectClass::UnknownB000F0EffectClass(ClientClass *client, unsigned id, unsigned start_time, unsigned duration, unsigned cycle_time) :
	UnknownB00060EffectClass(client, id, start_time, duration, cycle_time)
{
	Set_Flag(0x80, true);
}

// 0x6E9360
Vector3 UnknownB000F0EffectClass::Unknown_08(const Vector3 &)
{
	return(Transform.Get_Translation());
}

// 0x6EA550
INIClass *Unknown_6EA550()
{
	static INIClass ini;
	if (!ini.Is_Loaded()) {
		ini.Load("effect.ini");
	}
	return(&ini);
}











































































































































































































































































































































// 0x6EA5E0
EffectObjectClass *Unknown_6EA5E0(INIClass *ini, const char *section, unsigned id, unsigned start_time, unsigned duration, unsigned cycle_time, ClientClass *client, float scale, float *hsv)
{
	int base_asset = ini->Get_Int(section, "BaseAssetID", -1);
	RenderObjClass *robj = ClientObjectClass::Unknown_6F5140(base_asset, client, scale, hsv);
	FAIL_IF(!robj) return(NULL);
	bool scale_to_radius = ini->Get_Bool(section, "ScaleToRadius", false);
	bool source_based = ini->Get_Bool(section, "SourceBased", false);
	bool target_center_to_point = ini->Get_Bool(section, "TargetCenterToPoint", false);
	bool next_link_to_robj = ini->Get_Bool(section, "UnaryNextLinkToRObj", false);
	bool reset_mappers = ini->Get_Bool(section, "ResetMappers", false);
	if (reset_mappers) {
		Unknown_496AE0(robj, 0);
	}
	Unknown6EFC70EffectClass *effect = new Unknown6EFC70EffectClass(client, id, start_time, duration, cycle_time, robj);
	effect->Set_Flag(0x10000, scale_to_radius);
	effect->Set_Flag(0x20000, source_based);
	effect->Set_Flag(0x40000, target_center_to_point);
	effect->Set_Flag(0x4000, next_link_to_robj);
	effect->Set_Flag(0x8000, reset_mappers);
	robj->Release_Ref();
	return(effect);
}







// 0x6EA830
EffectObjectClass *Unknown_6EA830(INIClass *ini, const char *section, unsigned id, unsigned start_time, unsigned duration, unsigned cycle_time, ClientClass *client, float scale, float *hsv)
{
	int base_asset = ini->Get_Int(section, "BaseAssetID", -1);
	RenderObjClass *robj = ClientObjectClass::Unknown_6F5140(base_asset, client, scale, hsv);
	FAIL_IF(!robj) return(NULL);
	bool scale_to_radius = ini->Get_Bool(section, "ScaleToRadius", false);
	bool in_front_of_object = ini->Get_Bool(section, "InFrontOfObject", false);
	bool face_camera = ini->Get_Bool(section, "FaceCamera", false);
	bool reset_mappers = ini->Get_Bool(section, "ResetMappers", false);
	if (reset_mappers) {
		Unknown_496AE0(robj, 0);
	}
	Unknown6EEEA0EffectClass *effect = new Unknown6EEEA0EffectClass(client, id, start_time, duration, cycle_time, robj);
	effect->Set_Flag(0x200, scale_to_radius);
	effect->Set_Flag(0x400, in_front_of_object);
	effect->Set_Flag(0x800, face_camera);
	effect->Set_Flag(0x1000, reset_mappers);
	robj->Release_Ref();
	return(effect);
}







// 0x6EAA40
EffectObjectClass *Unknown_6EAA40(INIClass *ini, const char *section, unsigned id, unsigned start_time, unsigned duration, unsigned cycle_time, ClientClass *client, float scale, float *hsv)
{
	int base_asset = ini->Get_Int(section, "BaseAssetID", -1);
	RenderObjClass *robj = ClientObjectClass::Unknown_6F5140(base_asset, client, scale, hsv);
	FAIL_IF(!robj) return(NULL);
	bool scale_to_radius = ini->Get_Bool(section, "ScaleToRadius", false);
	bool in_front_of_object = ini->Get_Bool(section, "InFrontOfObject", false);
	bool face_camera = ini->Get_Bool(section, "FaceCamera", false);
	bool reset_mappers = ini->Get_Bool(section, "ResetMappers", false);
	if (reset_mappers) {
		Unknown_496AE0(robj, 0);
	}
	Unknown6F25A0EffectClass *effect = new Unknown6F25A0EffectClass(client, id, start_time, duration, cycle_time, robj);
	effect->Set_Flag(0x200, scale_to_radius);
	effect->Set_Flag(0x400, in_front_of_object);
	effect->Set_Flag(0x800, face_camera);
	effect->Set_Flag(0x1000, reset_mappers);
	robj->Release_Ref();
	return(effect);
}
















































// 0x6EAFA0
EffectObjectClass *Unknown_6EAFA0(INIClass *ini, const char *section, unsigned id, unsigned start_time, unsigned duration, unsigned cycle_time, ClientClass *client, float scale, float *hsv)
{
	int base_asset = ini->Get_Int(section, "BaseAssetID", -1);
	RenderObjClass *robj = ClientObjectClass::Unknown_6F5140(base_asset, client, scale, hsv);
	FAIL_IF(!robj) return(NULL);
	bool next_link_to_robj = ini->Get_Bool(section, "UnaryNextLinkToRObj", false);
	bool reset_mappers = ini->Get_Bool(section, "ResetMappers", false);
	if (reset_mappers) {
		Unknown_496AE0(robj, 0);
	}
	Unknown6F07F0EffectClass *effect = new Unknown6F07F0EffectClass(client, id, start_time, duration, cycle_time, robj);
	effect->Set_Flag(0x4000, next_link_to_robj);
	effect->Set_Flag(0x8000, reset_mappers);
	robj->Release_Ref();
	return(effect);
}






// NOT EXACT: order-only (the client reloads the ResetMappers bool before the spin rate's fmul; 4 forms)
// 0x6EB150
EffectObjectClass *Unknown_6EB150(INIClass *ini, const char *section, unsigned id, unsigned start_time, unsigned duration, unsigned cycle_time, ClientClass *client, float scale, float *hsv)
{
	int base_asset = ini->Get_Int(section, "BaseAssetID", -1);
	RenderObjClass *robj = ClientObjectClass::Unknown_6F5140(base_asset, client, scale, hsv);
	FAIL_IF(!robj) return(NULL);
	bool next_link_to_robj = ini->Get_Bool(section, "UnaryNextLinkToRObj", false);
	bool reset_mappers = ini->Get_Bool(section, "ResetMappers", false);
	float radius = ini->Get_Float(section, "SpiralRadius", 0.0f);
	float spin_rate = ini->Get_Float(section, "SpiralSpinRate", 0.0f) * (WWMATH_PI * 2.0f / 1000.0f);
	if (reset_mappers) {
		Unknown_496AE0(robj, 0);
	}
	Unknown6F0B10EffectClass *effect = new Unknown6F0B10EffectClass(client, id, start_time, duration, cycle_time, robj, radius, spin_rate);
	effect->Set_Flag(0x4000, next_link_to_robj);
	effect->Set_Flag(0x8000, reset_mappers);
	robj->Release_Ref();
	return(effect);
}












// 0x6EB330
EffectObjectClass *Unknown_6EB330(INIClass *ini, const char *section, unsigned id, unsigned start_time, unsigned duration, unsigned cycle_time, ClientClass *client, float scale, float *hsv)
{
	int base_asset = ini->Get_Int(section, "BaseAssetID", -1);
	RenderObjClass *robj = ClientObjectClass::Unknown_6F5140(base_asset, client, scale, hsv);
	FAIL_IF(!robj) return(NULL);
	bool one_change = ini->Get_Bool(section, "OneChange", false);
	bool next_link_to_robj = ini->Get_Bool(section, "UnaryNextLinkToRObj", false);
	bool reset_mappers = ini->Get_Bool(section, "ResetMappers", false);
	float distance = ini->Get_Float(section, "Distance", 0.0f);
	int change_time = WWMath::Clamp(ini->Get_Float(section, "ChangeTime", 0.0f), 0.0f, 4000000.0f) * 1000.0f;
	if (reset_mappers) {
		Unknown_496AE0(robj, 0);
	}
	Unknown6F0FD0EffectClass *effect = new Unknown6F0FD0EffectClass(client, id, start_time, duration, cycle_time, robj, change_time, distance);
	effect->Set_Flag(0x10000, one_change);
	effect->Set_Flag(0x4000, next_link_to_robj);
	effect->Set_Flag(0x8000, reset_mappers);
	robj->Release_Ref();
	return(effect);
}













// 0x6EB590
EffectObjectClass *Unknown_6EB590(INIClass *ini, const char *section, unsigned id, unsigned start_time, unsigned duration, unsigned cycle_time, ClientClass *client, float scale, float *hsv)
{
	int base_asset = ini->Get_Int(section, "BaseAssetID", -1);
	RenderObjClass *robj = ClientObjectClass::Unknown_6F5140(base_asset, client, scale, hsv);
	FAIL_IF(!robj) return(NULL);
	bool one_change = ini->Get_Bool(section, "OneChange", false);
	bool next_link_to_robj = ini->Get_Bool(section, "UnaryNextLinkToRObj", false);
	bool reset_mappers = ini->Get_Bool(section, "ResetMappers", false);
	float distance = ini->Get_Float(section, "Distance", 0.0f);
	int change_time = WWMath::Clamp(ini->Get_Float(section, "ChangeTime", 0.0f), 0.0f, 4000000.0f) * 1000.0f;
	int time_to_change = WWMath::Clamp(ini->Get_Float(section, "TimeToChange", 0.0f), 0.0f, 4000000.0f) * 1000.0f;
	if (reset_mappers) {
		Unknown_496AE0(robj, 0);
	}
	Unknown6F1760EffectClass *effect = new Unknown6F1760EffectClass(client, id, start_time, duration, cycle_time, robj, change_time, time_to_change, distance);
	effect->Set_Flag(0x10000, one_change);
	effect->Set_Flag(0x4000, next_link_to_robj);
	effect->Set_Flag(0x8000, reset_mappers);
	robj->Release_Ref();
	return(effect);
}

// 0x6EB850
EffectObjectClass *Unknown_6EB850(INIClass *ini, const char *section, unsigned id, unsigned start_time, unsigned duration, unsigned cycle_time, ClientClass *client)
{
	return(new UnknownB000F0EffectClass(client, id, start_time, duration, cycle_time));
}

// 0x6EC7D0
EffectObjectClass *Unknown_6EC7D0(INIClass *ini, const char *section, unsigned id, unsigned start_time, unsigned duration, unsigned cycle_time, ClientClass *client)
{
	int render_state = ini->Get_Int(section, "RenderStateID", -1);
	char *actions[] = { "Activate", "Deactivate", "Linger", "Ramp", "ActivateNextStateOnce", NULL };
	int action = ini->Get_List_Index(section, "ActionType", -1, actions);
	int action_time = ini->Get_Float(section, "ActionTime", 1.0f) * 1000.0f;
	bool force = ini->Get_Bool(section, "ForceReactivate", false);
	Unknown6F0200EffectClass *effect = new Unknown6F0200EffectClass(client, id, start_time, duration, cycle_time, render_state, action, action_time);
	effect->Set_Flag(0x200, force);
	return(effect);
}

/*
**	Reconstruction notes (EF1, 2026-10-28)
**
**	The window: eff_line.cpp's <string> guarded static ends at 0x6E66E0; the base's constructor
**	0x6E6710 is this file's first function. The base's slots +0x10 / +0x14 are inline in eff_o.h
**	(their copies 0x6E5420 / 0x6E4860 sit in eff_line.cpp's and eff_attach.cpp's windows), the
**	empty slots +0x04 / +0x18 / +0x1C / +0x20 are here. Unknown_6E6A90 is called directly by
**	SClient (Shutdown, Process_AI) and inlined here, so it comes before the destructor. Slot +0x0C
**	tests flag 0x20 as (bool) (shr 5 / test 1; the plain bitfield test is a test byte).
*/
