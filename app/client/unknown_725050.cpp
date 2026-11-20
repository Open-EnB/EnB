// Earth & Beyond client (reconstruction): app\client\unknown_725050.cpp, PROVISIONAL file name (D137). Notes at the end of the file.
#include "client.h"

// 0x725050
UnknownB02C90Class::UnknownB02C90Class(void *owner) :
	Unknown04(owner), Unknown08(owner), Unknown1C(true)
{
}

// 0x7250F0
UnknownB02C90Class::~UnknownB02C90Class()
{
}

#include "rendobj.h"

// 0x725610
bool UnknownB02C90Class::Unknown_0C(UnknownB02C90Class *child, int slot, int child_slot)
{
	if (child) {
		BufferStruct::ElementStruct *e = Buffer.Get(slot);
		BufferStruct::ElementStruct *ce = child->Buffer.Get(child_slot);
		if (e && ce) {
			e->Child = child;
			ce->Unknown3C = this;
			child->Unknown08 = Unknown08;
			RenderObjClass *model = ((ClientObjectClass *)Unknown04)->RenderObject;
			RenderObjClass *child_model = ((ClientObjectClass *)child->Unknown04)->RenderObject;
			if (model && child_model) {
				BufferStruct::ElementStruct *b = Buffer.Get(slot);
				model->Add_Sub_Object_To_Bone(child_model, (b == NULL) ? -1 : b->Bone);
			}
			Matrix3D::Multiply(e->Transform, Unknown1C, &child->Unknown1C);
			child->Unknown_14(this, slot, child_slot);
			if (((ClientObjectClass *)child->Unknown04)->Type != 29) {
				float radius = child->Unknown_725C60(Vector3(0, 0, 0), child->Buffer.Radius);
				((ClientObjectClass *)child->Unknown04)->Unknown_34(radius);
			}
			if (((ClientObjectClass *)Unknown04)->Type != 29) {
				float radius = Unknown_725C60(Vector3(0, 0, 0), Buffer.Radius);
				((ClientObjectClass *)Unknown04)->Unknown_34(radius);
			}
			return(true);
		}
	}
	return(false);
}

// 0x7257F0
bool UnknownB02C90Class::Unknown_10(UnknownB02C90Class *child, int slot, int child_slot)
{
	if (child) {
		if (slot == -1) {
			for (int i = 0; i < Buffer.Count; i++) {
				if (Buffer.Get(i)->Child == child) slot = i;
			}
		}
		if (child_slot == -1) {
			for (int i = 0; i < child->Buffer.Count; i++) {
				if (child->Buffer.Get(i)->Unknown3C == this) child_slot = i;
			}
		}
		BufferStruct::ElementStruct *e = Buffer.Get(slot);
		BufferStruct::ElementStruct *ce = child->Buffer.Get(child_slot);
		if (e && ce && e->Child == child && ce->Unknown3C == this) {
			RenderObjClass *model = ((ClientObjectClass *)Unknown04)->RenderObject;
			RenderObjClass *child_model = ((ClientObjectClass *)child->Unknown04)->RenderObject;
			if (model && child_model) {
				model->Remove_Sub_Object(child_model);
			}
			e->Child = NULL;
			ce->Unknown3C = NULL;
			child->Unknown08 = child->Unknown04;
			child->Unknown1C.Make_Identity();
			child->Unknown_18(this, slot, child_slot);
			if (((ClientObjectClass *)child->Unknown04)->Type != 29) {
				float radius = child->Unknown_725C60(Vector3(0, 0, 0), child->Buffer.Radius);
				((ClientObjectClass *)child->Unknown04)->Unknown_34(radius);
			}
			if (((ClientObjectClass *)Unknown04)->Type != 29) {
				float radius = Unknown_725C60(Vector3(0, 0, 0), Buffer.Radius);
				((ClientObjectClass *)Unknown04)->Unknown_34(radius);
			}
			return(true);
		}
	}
	return(false);
}

// 0x725C60
float UnknownB02C90Class::Unknown_725C60(const Vector3 &center, float radius)
{
	for (int i = 0; i < Buffer.Count; i++) {
		BufferStruct::ElementStruct *e = Buffer.Get(i);
		if (e->Child && ((ClientObjectClass *)e->Child->Unknown04)->Type == 29) {
			Vector3 pos; e->Child->Unknown1C.Get_Translation(&pos);
			Vector3 dir = pos + center;
			dir.Normalize();
			dir *= e->Child->Buffer.Radius;

			Vector3 d = (dir + pos) - center;
			float dist = d.Length();
			if (dist > radius) radius = dist;
			radius = e->Child->Unknown_725C60(pos, radius);
		}
	}
	return(radius);
}

// 0x725120
void UnknownB02C90Class::Unknown_08()
{
	for (int i = 0; i < Buffer.Count; i++) {
		BufferStruct::ElementStruct *e = Buffer.Get(i);
		UnknownB02C90Class *child = e->Child;
		if (child) {
			child->Unknown_08();
			Unknown_10(child, i, 0);
			((ClientObjectClass *)Unknown04)->Client->Remove_Object(((ClientObjectClass *)child->Unknown04)->Get_Game_ID());
			delete child;
		} else if (e->Unknown3C) {
			e->Unknown3C->Unknown_10(this, -1, -1);
		}
	}
}

#include "ini.h"
#include "strtok_r.h"
#include <stdio.h>

void Unknown_831950(Matrix3D &transform, const char *text);	// 0x831950 (cdecl; a Matrix3D from a Mount line's text)  // NAME 1 [transform, text]: what 0x7251C0 passes

// 0x7251C0 (the fallback element's fields set again after its constructor, the matrix last: the client's store order)
void UnknownB02C90Class::Unknown_04(int base_asset)
{
	static INIClass ini;
	if (!ini.Is_Loaded()) {
		ini.Load("cbasset.ini");
	}
	char *lasts = NULL;
	Matrix3D transform(true);
	if (base_asset != -1) {
		Buffer.Radius = ((ClientObjectClass *)Unknown04)->Unknown_30();
		char section[256];
		char entry[256];
		char value[256];
		sprintf(section, "BASE-%d", base_asset);
		int mount = 0;
		sprintf(entry, "Mount%d", mount);
		while (ini.Get_String(section, entry, "", value, sizeof(value))) {
			int field = 0;
			char *text = value;
			BufferStruct::ElementStruct element;
			char *token;
			do {
				token = strtok_r(text, " \t", &lasts);
				text = NULL;
				if (token != NULL) switch (field++) {
					case 0:
						element.Bone = atol(token);
						break;
					case 1:
						Unknown_831950(transform, token);
						break;
					case 2:
						element.Unknown34 = (atol(token) != 0);
						break;
					case 3:
						element.Unknown38 = atol(token);
						break;
				}
			} while (token != NULL);
			element.Transform = transform;
			Buffer.Add(element);
			mount++;
			sprintf(entry, "Mount%d", mount);
		}
		if (Buffer.Count == 0) {
			BufferStruct::ElementStruct element;
			element.Unknown3C = NULL; element.Child = NULL; element.Bone = 0; element.Unknown34 = false; element.Unknown38 = 0; element.Transform.Make_Identity();
			Buffer.Add(element);
		}
	}
}

/*
**	Notes
**
**	Client 0x725050-0x7260F0, an object of its own by the CRT initialiser table (SG, run 2026-11-18): UnknownB02C90Class's
**	members (co.h; the attachment slots of a client object: constructor 0x725050, destructor 0x7250F0, 0x725120,
**	0x7251C0 with its function-local INIClass and atexit stub 0x7255F0, 0x725610, 0x7257F0, 0x725C60), then <string>'s
**	guarded initialiser 0x7260D0 (entry 573), before client.cpp's object (ClientClass's constructor 0x726100, its own
**	guard 0x729450, entry 574). Moved out of client.cpp in the order client.cpp had them. No assert or string names
**	the file: PROVISIONAL (D137).
*/
