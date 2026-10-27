// Earth & Beyond client (reconstruction): app\Client\client_lego_impl.cpp. Notes at the end of the file.
#include "client_lego_impl.h"
#include "..\..\wwlib\ini.h"

std::hash_map<int, RefCountPtr<LegoBoneMapClass> > ClientLegoImplClass::_Global_Bone_Maps;
INIClass *ClientLegoImplClass::_CBASSET_INI = NULL;




























































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

void ClientLegoImplClass::Unknown_18(int)
{
}

void ClientLegoImplClass::Unknown_1C(void)
{
	for (std::hash_map<std::string, LegoChildStruct>::iterator it = Children.begin(); it != Children.end(); ++it) {
		(*it).second.Child->Unknown_1C();
	}
}

RefCountPtr<RenderObjClass> ClientLegoImplClass::Get_Render_Object(void) const
{
	return RenderObject;
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



// NOT EXACT: frame: ours 0x30, the client 0x60 (its bone translation goes through FPU temporaries; attempts.csv)
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
		if ((*it).second.Knob) {
			Vector3 bone = RenderObject->Get_Bone_Transform((*it).first.c_str()).Get_Translation();
			float dist = (RenderObject->Get_Position() - bone).Length();
			float reach = (*it).second.Child->Get_Radius() + dist;
			if (reach > radius) {
				radius = reach;
			}
		}
	}
	if (radius < min_radius) {
		Radius = min_radius;
		Notify_Parent();
		return;
	}
	Radius = radius;
	Notify_Parent();
}

void ClientLegoImplClass::Child_Changed(const LegoIdentifierClass &)
{
	Update_Radius();
}
/*
**	Reconstruction notes
**
**	client.exe: the window 0x729BF0-0x72DC10 (asserts on lines 69-809), then the containers'
**	instances (0x72E090-0x731F20). Rebuilt: the bone maps' Init / Shutdown, Create, the constructor
**	and destructor, the small virtuals, the parent links and Update_Radius. Not rebuilt yet: Init
**	0x72A7C0, the slots +0x28-+0x38 (0x72ABB0, 0x72B000, 0x72B7F0, 0x72C1D0, 0x72C8B0), the parent
**	slot 0x72CE40, Attach 0x72D0E0 / Detach 0x72D2A0, the bone map loader 0x72D3E0 / 0x72D5F0.
*/
