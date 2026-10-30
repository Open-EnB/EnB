// Earth & Beyond client (reconstruction): app\Client\client_lego_impl.cpp. Notes at the end of the file.
#include "client_lego_impl.h"
#include "..\..\wwlib\ini.h"
#include "..\..\wwlib\wwstring.h"
#include <stdio.h>

static const char *_Attach_Bone = "ATTACH";  // NAME 1 [_Attach_Bone]: neutral (D10), 0xB95070
static std::hash_map<int, RefCountPtr<LegoBoneMapClass> > _Global_Bone_Maps;  // 0xBE7D28: the client's assert (a file static: the asserts name it unqualified in the bone map's constructor too)
static INIClass *_CBASSET_INI = NULL;  // 0xBE7D44: the client's asserts

























































void ClientLegoImplClass::Init_Bone_Maps(const std::string &filename)
{
	FAIL_IF(_CBASSET_INI) {
		delete _CBASSET_INI;
	}
	_CBASSET_INI = new INIClass(filename.c_str());


	FAIL_IF(!_Global_Bone_Maps.empty()) {
		_Global_Bone_Maps.clear();
	}
}

void ClientLegoImplClass::Shutdown_Bone_Maps(void)
{
	_Global_Bone_Maps.clear();
	delete _CBASSET_INI;
	_CBASSET_INI = NULL;
}

RefCountPtr<LegoClass> ClientLegoImplClass::Create(const LegoIdentifierClass &id, int unknown, int asset_id, const RefCountPtr<RenderObjClass> &render_object)
{
	FAIL_IF(asset_id < 0) return NULL;

	RefCountPtr<ClientLegoImplClass> lego = Create_NEW(new ClientLegoImplClass(id, unknown));
	FAIL_IF(!lego) {
		return NULL;
	}
	lego->Init(asset_id, render_object);
	return lego;
}

ClientLegoImplClass::ClientLegoImplClass(const LegoIdentifierClass &id, int unknown) :
	Identifier(id),
	Unknown50(unknown),
	AssetId(-1),
	Scale(1.0f),
	Radius(0.0f),
	Unknown64(true),
	Unknown65(true),
	Transform(true),
	Parent(NULL)
{
}









ClientLegoImplClass::~ClientLegoImplClass(void)
{
	G_ASSERT(Children.empty() && Orphans.empty());
	std::vector<RefCountPtr<LegoClass> > legos;
	Detach_All(legos);
	Parent = NULL;
}

const LegoIdentifierClass &ClientLegoImplClass::Get_Identifier(void) const
{
	return Identifier;
}

float ClientLegoImplClass::Get_Scale(void) const
{
	return Scale;
}

void ClientLegoImplClass::Set_Scale(float scale)
{
	if (scale != Scale) {
		for (std::hash_map<std::string, LegoChildStruct>::iterator it = Children.begin(); it != Children.end(); ++it) {
			(*it).second.Child->Set_Scale(scale);
		}
		Update_Radius();
	}
}

float ClientLegoImplClass::Get_Radius(void) const
{
	return Radius;
}

void ClientLegoImplClass::Unknown_18(bool)
{
}

void ClientLegoImplClass::Unknown_1C(void)
{
	for (std::hash_map<std::string, LegoChildStruct>::iterator it = Children.begin(); it != Children.end(); ++it) {
		(*it).second.Child->Unknown_1C();
	}
}

void ClientLegoImplClass::Init(int asset_id, const RefCountPtr<RenderObjClass> &render_object)
{
	BoneMap = Get_Bone_Map(asset_id);
	AssetId = asset_id;
	Radius = BoneMap ? BoneMap->Radius : 0.0f;
	for (std::hash_map<std::string, LegoChildStruct>::iterator it = Children.begin(); it != Children.end(); ++it) {
		LegoChildStruct &child = (*it).second;
		const std::string &bone_name = (*it).first;
		if (0 != child.Knob) {
			Detach(child.Child, RenderObject, child.Knob);
		}
		child.Knob = BoneMap->Get_Knob(bone_name);
		if (child.Knob) {
			Attach(child.Child, render_object, child.Knob);
		}
	}
	RenderObject = render_object;
	if (RenderObject) {
		Matrix3D attach = RenderObject->Get_Bone_Transform(_Attach_Bone);
		attach.Get_Inverse(Transform);
	} else {
		Transform = Matrix3D::Identity;
	}
	Update_Radius();
	if (Parent) {
		Parent->Child_Moved(Get_Identifier());
	}
}

RefCountPtr<RenderObjClass> ClientLegoImplClass::Get_Render_Object(void) const
{
	return RenderObject;
}























































RefCountPtr<LegoClass> ClientLegoImplClass::Find(const LegoIdentifierClass &id) const
{
	if (id.Is_Empty()) {
		return NULL;
	}
	LegoSplitStruct split(Identifier, id);
	FAIL_IF(split.Is_Invalid()) return NULL;
	std::hash_map<std::string, LegoChildStruct>::const_iterator child_find = Children.find(split.Name);
	if (split.State == 0) {
		if (child_find == Children.end()) {
			return NULL;
		}
		return (*child_find).second.Child;
	}
	if (child_find != Children.end()) {
		return (*child_find).second.Child->Find(id);
	}
	std::hash_map<std::string, RefCountPtr<LegoOrphanDataClass> >::const_iterator orphan_find = Orphans.find(split.Name);
	if (orphan_find == Orphans.end()) {
		return NULL;
	}
	const std::vector<RefCountPtr<LegoClass> > &legos = (*orphan_find).second->Legos;
	for (std::vector<RefCountPtr<LegoClass> >::const_iterator lego = legos.begin(); lego != legos.end(); ++lego) {
		const LegoIdentifierClass &lego_id = (*lego)->Get_Identifier();
		if (lego_id == id) {
			return *lego;
		}
	}
	return NULL;
}




























// NOT EXACT: inline budget: the client builds one more Matrix3D result (6 ??_H against our 5), 0.88 (attempts.csv)
Matrix3D ClientLegoImplClass::Get_Lego_Transform(const LegoIdentifierClass &id) const
{
	if (id.Is_Empty()) {
		FAIL_IF(!RenderObject) return Matrix3D::Identity;
		return RenderObject->Get_Transform();
	}
	LegoSplitStruct split(Identifier, id);



	FAIL_IF(split.Is_Invalid()) {
		FAIL_IF(!RenderObject) return Matrix3D::Identity;
		return RenderObject->Get_Transform();
	}
	std::hash_map<std::string, LegoChildStruct>::const_iterator child_find = Children.find(split.Name);
	if ((split.State == 0) | (child_find == Children.end())) {












		FAIL_IF(!RenderObject) return Matrix3D::Identity;
		if (!BoneMap) {
			return RenderObject->Get_Transform();
		}
		RefCountPtr<LegoKnobClass> knob = BoneMap->Get_Knob(split.Name);
		if (!knob) {
			return RenderObject->Get_Bone_Transform(split.Name.c_str());
		}
		return RenderObject->Get_Bone_Transform(knob->BoneIndex);
	}
	return (*child_find).second.Child->Get_Lego_Transform(id);
}












// NOT EXACT: inline budget: the last branch's orphan data assignment and destructor, inlined by the client, called by ours; 0.75 (attempts.csv)
void ClientLegoImplClass::Add(const RefCountPtr<LegoClass> &lego)
{
	LegoSplitStruct split(Identifier, lego->Get_Identifier());
	FAIL_IF(split.Is_Invalid()) return;
	std::hash_map<std::string, LegoChildStruct>::iterator child_find = Children.find(split.Name);
	if (split.State == 0) {
		if (child_find != Children.end()) {
			LegoChildStruct &record = (*child_find).second;
			std::vector<RefCountPtr<LegoClass> > legos;
			record.Child->Detach_All(legos);
			if (record.Knob) {
				Detach(record.Child, RenderObject, record.Knob);
			}
			record.Child = lego;
			if (record.Knob) {
				Attach(lego, RenderObject, record.Knob);
				Update_Radius();
			}
			for (std::vector<RefCountPtr<LegoClass> >::iterator it = legos.begin(); it != legos.end(); ++it) {
				lego->Add(*it);
			}
			return;
		}
		LegoChildStruct child;
		child.Child = lego;
		if (BoneMap) {
			child.Knob = BoneMap->Get_Knob(split.Name);
		}
		Children[split.Name] = child;
		if (child.Knob) {
			Attach(lego, RenderObject, child.Knob);
			Update_Radius();
		}
		std::hash_map<std::string, RefCountPtr<LegoOrphanDataClass> >::iterator orphan_find = Orphans.find(split.Name);
		if (orphan_find == Orphans.end()) {
			return;
		}
		RefCountPtr<LegoOrphanDataClass> child_orphan_data = (*orphan_find).second;






























		FAIL_IF(!child_orphan_data) return;
		Orphans.erase(orphan_find);
		for (std::vector<RefCountPtr<LegoClass> >::iterator it = child_orphan_data->Legos.begin(); it != child_orphan_data->Legos.end(); ++it) {
			lego->Add(*it);
		}
		return;
	}
	if (child_find != Children.end()) {
		RefCountPtr<LegoClass> child = (*child_find).second.Child;
		child->Add(lego);
		return;
	}
	std::hash_map<std::string, RefCountPtr<LegoOrphanDataClass> >::iterator orphan_find = Orphans.find(split.Name);
	RefCountPtr<LegoOrphanDataClass> data;
	if (orphan_find != Orphans.end()) {
		data = (*orphan_find).second;
	} else {
		data = Create_NEW(new LegoOrphanDataClass);
		Orphans[split.Name] = data;
	}
	data->Legos.push_back(lego);
}
























RefCountPtr<LegoClass> ClientLegoImplClass::Remove(const LegoIdentifierClass &id)
{
	LegoSplitStruct split(Identifier, id);
	FAIL_IF(split.Is_Invalid()) return NULL;
	std::hash_map<std::string, LegoChildStruct>::iterator child_find = Children.find(split.Name);
	if (split.State == 0) {
		if (child_find == Children.end()) {
			return NULL;
		}
		LegoChildStruct &record = (*child_find).second;
		RefCountPtr<LegoClass> child = record.Child;
		RefCountPtr<LegoKnobClass> knob = record.Knob;
		RefCountPtr<LegoOrphanDataClass> orphan_data = Create_NEW(new LegoOrphanDataClass);
		std::vector<RefCountPtr<LegoClass> > &legos = orphan_data->Legos;
		child->Detach_All(legos);
		if (!legos.empty()) {
			Orphans[split.Name] = orphan_data;
		}
		if (knob) {
			Detach(child, RenderObject, knob);
			Update_Radius();
		}
		Children.erase(child_find);
		return child;
	}
	if (child_find != Children.end()) {
		return (*child_find).second.Child->Remove(id);
	}
	std::hash_map<std::string, RefCountPtr<LegoOrphanDataClass> >::iterator orphan_find = Orphans.find(split.Name);
	if (orphan_find == Orphans.end()) {
		return NULL;
	}
	RefCountPtr<LegoOrphanDataClass> data = (*orphan_find).second;
	std::vector<RefCountPtr<LegoClass> > &legos = data->Legos;
	for (std::vector<RefCountPtr<LegoClass> >::iterator it = legos.begin(); it != legos.end(); ++it) {
		RefCountPtr<LegoClass> lego = *it; const LegoIdentifierClass &lego_id = lego->Get_Identifier();
		if (lego_id == id) {
			legos.erase(it);
			if (legos.size() == 0) Orphans.erase(orphan_find);
			return lego;
		}
	}
	return NULL;
}
void ClientLegoImplClass::Detach_All(std::vector<RefCountPtr<LegoClass> > &legos)
{
	for (std::hash_map<std::string, LegoChildStruct>::iterator it = Children.begin(); it != Children.end(); ++it) {
		LegoChildStruct &child = (*it).second;
		child.Child->Detach_All(legos);
		if (child.Knob) {
			Detach(child.Child, RenderObject, child.Knob);
		}
		legos.push_back(child.Child);
	}
	Children.clear();
	for (std::hash_map<std::string, RefCountPtr<LegoOrphanDataClass> >::iterator orphan = Orphans.begin(); orphan != Orphans.end(); ++orphan) {
		std::vector<RefCountPtr<LegoClass> > &orphan_legos = (*orphan).second->Legos;
		for (std::vector<RefCountPtr<LegoClass> >::iterator lego = orphan_legos.begin(); lego != orphan_legos.end(); ++lego) {
			legos.push_back(*lego);
		}
	}
	Orphans.clear();
	Update_Radius();
}























































void ClientLegoImplClass::Set_Parent(LegoParentClass *parent)
{
	G_ASSERT(parent);


	G_ASSERT(!Parent);
	Parent = parent;
}



void ClientLegoImplClass::Clear_Parent(void)
{
	G_ASSERT(Parent);
	Parent = NULL;
}

void ClientLegoImplClass::Notify_Parent(void)
{
	if (Parent) {
		Parent->Child_Changed(Get_Identifier());
	}
}

const Matrix3D &ClientLegoImplClass::Get_Transform(void) const
{
	return Transform;
}

void ClientLegoImplClass::Unknown_3C(int)
{
}




void ClientLegoImplClass::Update_Radius(void)
{
	FAIL_IF(!BoneMap) {
		if (Radius != 0.0f) {
			Radius = 0.0f;
			Notify_Parent();
		}
		return;
	}
	float min_radius = BoneMap->Radius;
	float radius = 0.0f;
	if (!RenderObject) {
		Radius = min_radius;
		Notify_Parent();
		return;
	}
	for (std::hash_map<std::string, LegoChildStruct>::iterator it = Children.begin(); it != Children.end(); ++it) {
		LegoChildStruct &child = (*it).second;
		if (child.Knob) {
			Matrix3D bone_tm = RenderObject->Get_Bone_Transform((*it).first.c_str());
			Vector3 bone = bone_tm.Get_Translation();
			float dist = (RenderObject->Get_Position() - bone).Length();
			float reach = child.Child->Get_Radius() + dist;
			if (reach > radius) {
				radius = reach;
			}
		}
	}
	Radius = min_radius > radius ? min_radius : radius;
	Notify_Parent();
}















// NOT EXACT: shape, 4 rows: the client forms &child (lea ebp) before loading it, ours after (attempts.csv)
void ClientLegoImplClass::Child_Moved(const LegoIdentifierClass &id)
{
	LegoSplitStruct split(Identifier, id);
	FAIL_IF(split.Is_Invalid()) return;
	std::hash_map<std::string, LegoChildStruct>::iterator child_find = Children.find(split.Name);

	FAIL_IF(child_find == Children.end()) return;


	const RefCountPtr<LegoClass> &child = (*child_find).second.Child;

	FAIL_IF(!child) return;

	const RefCountPtr<LegoKnobClass> &knob = (*child_find).second.Knob;
	FAIL_IF(!knob) return;
	int bone = knob->BoneIndex;
	RenderObject->Release_Bone(bone);
	RenderObject->Capture_Bone(bone);
	RenderObject->Control_Bone(bone, child->Get_Transform());
}

void ClientLegoImplClass::Child_Changed(const LegoIdentifierClass &)
{
	Update_Radius();
}





// NOT EXACT: frame: the client keeps the child's render object in its own stack slot (sub esp 8), ours in a dead parameter's (attempts.csv)
void ClientLegoImplClass::Attach(const RefCountPtr<LegoClass> &child, const RefCountPtr<RenderObjClass> &render_object, const RefCountPtr<LegoKnobClass> &knob)
{
	FAIL_IF(!child) return;
	FAIL_IF(!knob) return;
	child->Unknown_18(Unknown65);
	if (render_object) {
		int bone = knob->BoneIndex;
		render_object->Capture_Bone(bone);
		render_object->Control_Bone(bone, child->Get_Transform());
		RefCountPtr<RenderObjClass> child_object = child->Get_Render_Object();
		if (child_object) {
			render_object->Add_Sub_Object_To_Bone(child_object.Peek(), knob->BoneName.c_str());
			child->Set_Parent(this);
		}
	}
}















// NOT EXACT: register-only, 4 rows: eax / edx swapped around the Get_Render_Object call (attempts.csv)
void ClientLegoImplClass::Detach(const RefCountPtr<LegoClass> &child, const RefCountPtr<RenderObjClass> &render_object, const RefCountPtr<LegoKnobClass> &knob)
{
	FAIL_IF(!child) return;
	FAIL_IF(!knob) return;
	if (render_object) {
		render_object->Release_Bone(knob->BoneIndex);
		render_object->Remove_Sub_Object(child->Get_Render_Object().Peek());
		child->Clear_Parent();
	}
}

RefCountPtr<LegoBoneMapClass> ClientLegoImplClass::Get_Bone_Map(int asset_id)
{
	std::hash_map<int, RefCountPtr<LegoBoneMapClass> >::const_iterator it = _Global_Bone_Maps.find(asset_id);
	if (it != _Global_Bone_Maps.end()) {
		return (*it).second;
	}
	RefCountPtr<LegoBoneMapClass> bone_map = Create_NEW(new LegoBoneMapClass(asset_id));
	_Global_Bone_Maps[asset_id] = bone_map;
	return bone_map;
}
















LegoBoneMapClass::LegoBoneMapClass(int asset_id) :
	Radius(0.0f)
{
	char base[16];
	sprintf(base, "BASE-%d", asset_id);
	FAIL_IF(!_CBASSET_INI) {
	} else {
		INIClass *ini = _CBASSET_INI;
		Radius = ini->Get_Float(base, "Radius", 1.0f);
		Radius += ini->Get_Float(base, "RadiusAdjust", 0.0f);
		int mount = 0;
		char section[32];
		_snprintf(section, sizeof(section), "%s/Mount%02d", base, mount);
		while (ini->Section_Present(section)) {
			StringClass bone_name_string;
			ini->Get_String(bone_name_string, section, "BoneName");






			FAIL_IF(!bone_name_string.Peek_Buffer()) return;
			std::string bone_name = bone_name_string.Peek_Buffer();
			int bone_index = ini->Get_Int(section, "BoneIndex", -1);




			FAIL_IF(bone_index <= 0) return;
			RefCountPtr<LegoKnobClass> knob = Create_NEW(new LegoKnobClass(bone_name, bone_index));
			Knobs[bone_name] = knob;
			_snprintf(section, sizeof(section), "%s/Mount%02d", base, ++mount);
		}
	}
}

LegoBoneMapClass::~LegoBoneMapClass(void)
{
	Knobs.clear();
}

RefCountPtr<LegoKnobClass> LegoBoneMapClass::Get_Knob(const std::string &bone_name)
{
	std::hash_map<std::string, RefCountPtr<LegoKnobClass> >::iterator it = Knobs.find(bone_name);
	if (it == Knobs.end()) {
		return NULL;
	}
	return (*it).second;
}

LegoKnobClass::LegoKnobClass(std::string bone_name, int bone_index) :
	BoneName(bone_name),
	BoneIndex(bone_index)
{
}
/*
**	Reconstruction notes
**
**	client.exe: the window 0x729BF0-0x72DC10 (asserts on lines 69-809), then the containers'
**	instances (0x72E090-0x731F20). Every function of the window is written; exact: the bone maps'
**	Init / Shutdown, Create, the constructor and destructor, the small virtuals, Detach_All, the
**	parent links, Get_Bone_Map, the bone map's constructor, destructor and Get_Knob, the knob's
**	constructor, Init, Update_Radius. Written but not byte-exact yet (attempts.csv):
**	Find, Get_Lego_Transform, Add, Remove, Child_Moved, Attach and Detach.
**	Get_Bone_Map is exact with a const_iterator for its find (H12's lever). The knob's constructor 0x72DE40 and the bone map's Get_Knob 0x72DDA0
**	follow the window, exact. Not here: lego_identifier.cpp's split 0x844ED0 / 0x8451A0.
**	* The bone map's constructor 0x72D5F0 is exact or 2 rows off (order) with the same source,
**	  depending on what else the file holds (the inline budget's file state): re-check it after
**	  any change here.
**	* _Global_Bone_Maps and _CBASSET_INI are file statics: the bone map's constructor names
**	  _CBASSET_INI unqualified in its assert.
**	* New children go through the children table's operator[] (0x72C040 / 0x72EBA0 / 0x730C10):
**	  sgistl's insert_unique_noresize does not compile in VC6 (C2899).
**	* Init's first knob test is `0 != child.Knob` (ref_ptr.h's template operator!=: the client's
**	  setne / test); a plain `if (child.Knob)` leaves the loop's registers swapped.
**	* Update_Radius: the bone's matrix is copied into a Matrix3D local (the client's frame 0x60 and
**	  its int-move copies of the translation), the result a min_radius > radius ternary (FPU copy).
**	* Written by builder H17 (2026-10-27).
**	* F6 (2026-10-27): E&B's ref_ptr.h has no G_ASSERT in RefCountPtr(DummyPtrType *) (importer.py's
**	  PATCHES, wwlib/ref_ptr.h): with it every `return NULL` spent the caller's inline budget on an
**	  expansion that folds away, and Find 0x72ABB0 called the split's string destructor at its last
**	  returns. Remove is Add's flat shape (the State==0 block returns, then a plain if, the orphan code
**	  at function scope: the client's frame 0x84 and slots), with the orphan data's vector through a
**	  reference in the State==0 branch and the LegoChildStruct through a reference (lea esi/add eax,0x14).
*/
