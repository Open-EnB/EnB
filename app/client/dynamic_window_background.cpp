// Earth & Beyond client (reconstruction): app\Client\dynamic_window_background.cpp (__FILE__ 0xB768D0). Notes at the end of the file.
#include "dynamic_window_background.h"
#include "rendobj.h"
#include "nullrobj.h"
#include "assetmgr.h"
#include "hanim.h"
#include "ini.h"
#include "wwstring.h"
#include "..\shared\unknown_debug.h"

void Unknown_496AE0(RenderObjClass *robj, int unknown);	// 0x496AE0 (ro_funcs.cpp, cdecl: effect.ini's ResetMappers)  // NAME 1 [robj, unknown]: what the helpers pass

static INIClass DynaWinIni;	// 0xBE59F8  // NAME 1 [DynaWinIni]: neutral, the file it loads
// NOT EXACT: the 'Use' string's register (esi) and the returned temporary's release (the client does not clear it)
RefCountPtr<DynamicWindowAnimationDataClass> DynamicWindowAnimationDataClass::Get(const std::string &name)
{
	if (!DynaWinIni.Is_Loaded()) {
		DynaWinIni.Load("dynawin.ini");
	}

	RefCountPtr<DynamicWindowAnimationDataClass> data = Create_NEW(new DynamicWindowAnimationDataClass);

	StringClass use;
	DynaWinIni.Get_String(use, name.c_str(), "Use", "");
	const char *use_name = use;
	if (use_name && *use_name) {
		data = Get(std::string(use_name));
	}

	data->Load(DynaWinIni, name.c_str());
	return data;
}

void DynamicWindowAnimationDataClass::Load(INIClass &ini, const char *section)
{
	AnimMin.I = ini.Get_Int(section, "MinX", AnimMin.I);
	AnimMin.J = ini.Get_Int(section, "MinY", AnimMin.J);
	AnimMax.I = ini.Get_Int(section, "MaxX", AnimMax.I);
	AnimMax.J = ini.Get_Int(section, "MaxY", AnimMax.J);
	Border.X = ini.Get_Float(section, "BorderX", Border.X);
	Border.Y = ini.Get_Float(section, "BorderY", Border.Y);

	StringClass value;
	MeshName = (const char *)ini.Get_String(value, section, "MeshFile", MeshName.c_str());
	AnimXName = (const char *)ini.Get_String(value, section, "AnimX", AnimXName.c_str());
	AnimYName = (const char *)ini.Get_String(value, section, "AnimY", AnimYName.c_str());































































	G_ASSERT(AnimMin.I >= 0);
	G_ASSERT(AnimMin.J >= 0);
	G_ASSERT(AnimMax.I >= 0);
	G_ASSERT(AnimMax.J >= 0);
}

DynamicWindowBackgroundClass::DynamicWindowBackgroundClass(const std::string &name, unsigned int flags) :
	DynamicWindowRenderObjectClass(flags),
	AnimCombo(NULL),
	AnimX(NULL),
	AnimY(NULL)
{
	AnimationData = DynamicWindowAnimationDataClass::Get(name);
	if (AnimationData && !AnimationData->MeshName.empty() && !AnimationData->AnimXName.empty() && !AnimationData->AnimYName.empty()) {
		RefCountPtr<RenderObjClass> robj = Create_NEW(WW3DAssetManager::Get_Instance()->Create_Render_Obj(AnimationData->MeshName.c_str()));
		Unknown_496AE0(robj.Peek(), 0);
		Set_Render_Object(robj);

		AnimCombo = new HAnimComboClass;
		AnimX = new HAnimComboDataClass(true);
		AnimY = new HAnimComboDataClass(true);





		HAnimClass *anim = WW3DAssetManager::Get_Instance()->Get_HAnim(AnimationData->AnimXName.c_str());
		G_ASSERT(anim);
		AnimX->Give_HAnim(anim);

		anim = WW3DAssetManager::Get_Instance()->Get_HAnim(AnimationData->AnimYName.c_str());
		G_ASSERT(anim);
		AnimY->Give_HAnim(anim);

		AnimX->Build_Active_Pivot_Map();
		AnimY->Build_Active_Pivot_Map();
	} else {
		G_ASSERT(AnimationData && !AnimationData->MeshName.empty());
		RefCountPtr<RenderObjClass> robj = Create_NEW((RenderObjClass *)new Null3DObjClass("DynaWinNull"));
		Set_Render_Object(robj);
	}
}

DynamicWindowBackgroundClass::~DynamicWindowBackgroundClass()
{
	if (AnimCombo != NULL) {
		AnimCombo->Remove_Anim_Combo_Data(AnimX);
		AnimCombo->Remove_Anim_Combo_Data(AnimY);
	}
	delete AnimX;
	AnimX = NULL;
	delete AnimY;
	AnimY = NULL;
	delete AnimCombo;
	AnimCombo = NULL;
}

void DynamicWindowBackgroundClass::Set_Contents(const RefCountPtr<DynamicWindowElementClass> &contents)
{
	if (Contents) {
		Contents->Remove_From_Scene();
		Contents->Clear_Parent();
	}

	Contents = contents;

	if (Contents) {
		SceneClass *scene = Peek_Scene();
		if (scene != NULL) {
			Contents->Add_To_Scene(scene);
		}
		int level = Get_Level();
		if (AnimationData) {
			level--;
		}
		Contents->Set_Level(level);
		Contents->Set_Parent(this);
	}
	Invalidate();
}















void DynamicWindowBackgroundClass::Set_Min_Size(const Vector2 &min_size)
{
	DynamicWindowRenderObjectClass::Set_Min_Size(min_size);
	if (Contents) {
		if (!AnimationData) {
			Contents->Set_Min_Size(min_size);
			return;
		}
		Vector2 contents_min_size = min_size - (AnimationData->Border * 2.0f);
		FAIL_IF(contents_min_size.X <= 0.0f) { contents_min_size.X = 0.0f; }
		FAIL_IF(contents_min_size.Y <= 0.0f) { contents_min_size.Y = 0.0f; }
		Contents->Set_Min_Size(contents_min_size);
	}
}
// NOT EXACT: the last sum: the client adds 2.0f to both halves before storing either
Vector2 DynamicWindowBackgroundClass::Get_Min_Size()
{
	if (!Contents && !AnimationData) {
		return Vector2(0.0f, 0.0f);
	}
	if (!Contents) {
		return AnimationData->Border;
	}
	if (!AnimationData) {
		return Contents->Get_Min_Size();
	}
	return AnimationData->Border * 2.0f + Contents->Get_Min_Size() + Vector2(2.0f, 2.0f);
}

void DynamicWindowBackgroundClass::Layout(const Vector2 &position, const Vector2 &size)
{
	DynamicWindowRenderObjectClass::Layout(position, size);
	if (Contents) {
		Vector2 contents_position = position;
		Vector2 contents_size = size;
		if (AnimationData) {
			contents_position.X += AnimationData->Border.X;
			contents_position.Y -= AnimationData->Border.Y;
			contents_size -= AnimationData->Border * 2.0f;
		}
		Contents->Layout(contents_position, contents_size);
	}
}

DynamicWindowContainerClass *DynamicWindowBackgroundClass::Get_Container()
{
	return this;
}

void DynamicWindowBackgroundClass::Add_To_Scene(SceneClass *scene)
{
	DynamicWindowRenderObjectClass::Add_To_Scene(scene);
	if (Contents) {
		Contents->Add_To_Scene(scene);
	}
}

void DynamicWindowBackgroundClass::Remove_From_Scene()
{
	DynamicWindowRenderObjectClass::Remove_From_Scene();
	if (Contents) {
		Contents->Remove_From_Scene();
	}
}

void DynamicWindowBackgroundClass::Set_Level(int level)
{
	DynamicWindowRenderObjectClass::Set_Level(level);
	if (Contents) {
		int contents_level = level;
		if (AnimationData) {
			contents_level = level - 1;
		}
		Contents->Set_Level(contents_level);
	}
}

void DynamicWindowBackgroundClass::Set_Position(const Vector2 &position)
{
	DynamicWindowRenderObjectClass::Set_Position(position);
	if (Contents) {
		Contents->Set_Position(position);
	}
}





















void DynamicWindowBackgroundClass::Child_Changed(DynamicWindowElementClass &child)
{
	G_ASSERT(Contents.Peek() == &child);
	Invalidate();
}

void DynamicWindowBackgroundClass::Remove_All_Children(bool recursive)
{
	if (Contents) {
		if (recursive) {
			DynamicWindowContainerClass *container = Contents->Get_Container();
			if (container != NULL) {
				container->Remove_All_Children(true);
			}
		}
		Contents->Remove_From_Scene();
		Contents->Clear_Parent();
		Contents = NULL;
		Invalidate();
	}
}






bool DynamicWindowBackgroundClass::Remove_Child(const RefCountPtr<DynamicWindowElementClass> &child)
{
	FAIL_IF(!child) return false;
	if (child.Peek() != Contents.Peek()) {
		return false;
	}
	Contents->Remove_From_Scene();
	Contents->Clear_Parent();
	Contents = NULL;
	Invalidate();
	return true;
}

Vector2 DynamicWindowBackgroundClass::Fit_Size(const Vector2 &position, const Vector2 &size)
{
	Vector2 result = position;
	if (AnimCombo != NULL && AnimationData) {
		AnimCombo->Reset();

		int x = (int)(size.X + 0.5f);
		int y = (int)(size.Y + 0.5f);
		if (x < AnimationData->AnimMin.I) {
			x = AnimationData->AnimMin.I;
		} else if (x > AnimationData->AnimMax.I) {
			x = AnimationData->AnimMax.I;
		}
		if (y < AnimationData->AnimMin.J) {
			y = AnimationData->AnimMin.J;
		} else if (y > AnimationData->AnimMax.J) {
			y = AnimationData->AnimMax.J;
		}
		AnimX->Set_Frame((float)(x - AnimationData->AnimMin.I));
		AnimY->Set_Frame((float)(y - AnimationData->AnimMin.J));
		AnimCombo->Append_Anim_Combo_Data(AnimX);
		AnimCombo->Append_Anim_Combo_Data(AnimY);
		if (RenderObject) {
			RenderObject->Set_Animation(AnimCombo);
		}

		float dx = size.X - (float)x;
		if (!Has_Flags(0x10)) {
			if (Has_Flags(0x20)) {
				result.X += dx;
			} else {
				result.X += dx * 0.5f;
			}
		}
		float dy = size.Y - (float)y;
		if (!Has_Flags(0x40)) {
			if (Has_Flags(0x80)) {
				result.Y -= dy;
			} else {
				result.Y -= dy * 0.5f;
			}
		}
	}
	return result;
}

/*
**	Reconstruction notes (DW1, 2026-11-01)
**
**	Client 0x515AB0-0x5170F0: DynamicWindowAnimationDataClass::Get 0x515AB0 (dynawin.ini, loaded on first
**	use into the file's INIClass 0xBE59F8; a section's 'Use' entry names the section it starts from), its
**	deleting destructor 0x515DF0 / destructor 0x515E20, Load 0x515F00 (G_ASSERT 'AnimMin.I >= 0' on line
**	110); the background's constructor 0x516250 ('anim' on lines 137 / 141, 'AnimationData && ...' on 147),
**	deleting destructor 0x516650, destructor 0x516680, Set_Contents 0x516810, the slots 0x5168F0-0x516E00
**	(FAIL_IF 'contents_min_size.X <= 0.0f' on 214, G_ASSERT 'Contents.Peek() == &child' on 312, FAIL_IF
**	'!child' on 339); then the template instances RefCountPtr<RenderObjClass>::Clear-like 0x517030,
**	Create_NEW<DynamicWindowAnimationDataClass> 0x517060 and AutoPoolClass<HAnimComboDataClass,256>'s
**	allocator 0x5170F0. NOT EXACT (D64): Get 0x515AB0 and Get_Min_Size 0x516A00 (marked); the data's
**	inline destructor 0x515E20 is 6 bytes longer than the client's (its three strings' destructors).
**	The direct calls of the base's slots are qualified calls (DynamicWindowRenderObjectClass::...).
*/
