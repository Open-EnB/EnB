// Earth & Beyond client (reconstruction): app\client\fontsys.cpp. Notes at the end of the file.
#include "fontmanager.h"
#include "fontdatabase.h"
#include "font32bit.h"
#include "..\shared\unknown_debug.h"
#include "..\..\ww3d2\ww3d.h"
#include "..\..\ww3d2\dx8wrapper.h"
#include "..\..\ww3d2\texture.h"
#include "..\..\ww3d2\rinfo.h"
#include "..\..\wwmath\aabox.h"
#include "..\..\wwmath\sphere.h"
#include <math.h>
#include <string.h>

// NOT EXACT: written from the client's code (0x6D47C0) for the POC, not yet matched
FontStyleClass::FontStyleClass(const char *name, int object, const char *text) :
	DynamicMeshClass(2, 4),
	Font(NULL)
{
	FontStringStruct *def = Unknown_BE67B8->Find_String(name);
	if (def != NULL) {
		Info = *def;
	}
	if (text != NULL) {
		Info.Text = text;
	}
	if (Info.Text.Is_Empty()) {
		Info.Text = " ";
	}
	Unknown_6D5170(object);
	Unknown_6D6390();
	Unknown_6D5D40();
	Unknown26C = (int)(Info.Wait * 1000.0f);
}

// NOT EXACT: written from the client's code (0x6D4C70) for the POC, not yet matched
FontStyleClass::FontStyleClass(FontStringStruct def, unsigned int object, const char *text) :
	DynamicMeshClass(2, 4),
	Font(NULL)
{
	Info = def;
	if (text != NULL) {
		Info.Text = text;
	}
	if (Info.Text.Is_Empty()) {
		Info.Text = " ";
	}
	Unknown_6D5170(object);
	Unknown_6D6390();
	Unknown_6D5D40();
	Unknown26C = (int)(Info.Wait * 1000.0f);
}

// NOT EXACT: written from the client's code (0x6D5170) for the POC, not yet matched; Info's colours (+0x494 / +0x4A4
// from Colors, 10 / 255 when none) are left out
void FontStyleClass::Unknown_6D5170(int object)
{
	int i;
	Unknown4DC = 1.0f;
	Unknown4E0 = 1.0f;
	Object = (RenderObjClass *)object;
	Unknown300 = true;
	Unknown301 = false;
	Unknown402 = false;
	Unknown302 = false;
	Unknown404 = false;
	Unknown405 = false;
	Unknown406 = false;
	Unknown408 = false;
	Unknown4E5 = true;
	Unknown440 = 0;
	FontIndex = 0;
	Unknown450 = 0;
	Unknown250 = 2;
	Unknown410 = 0;
	Unknown444 = 0;
	Unknown2F4[0] = 1.0f;
	Unknown2F4[1] = -1.0f;
	Unknown2F4[2] = 1.0f;
	Unknown448 = 0;
	Unknown454 = 0.0f;
	Unknown458 = 0;
	LineCount = 0;
	LineNumber = 0;
	Unknown418 = 0;
	Unknown41C = 0;
	Unknown420[0] = 0;
	Unknown420[2] = 0;
	Unknown420[1] = 1000;
	Unknown420[3] = 1000;
	Unknown488[2] = 0;
	Unknown488[1] = 0;
	Unknown488[0] = 0;
	Unknown4C5 = false;
	Unknown4C8[0] = -1;
	Unknown4C8[1] = -1;
	Unknown4D0.X = 0.0f;
	Unknown4D0.Y = 0.0f;
	Unknown4D0.Z = 0.0f;
	Unknown47C.Z = 0.0f;
	Unknown47C.X = Info.Offset[0] * 640.0f;
	Unknown47C.Y = Info.Offset[1] * 480.0f;
	Unknown470 = Unknown47C;
	Unknown430[0] = -320;
	Unknown430[2] = 240;
	Unknown430[1] = 320;
	Unknown430[3] = -240;
	Unknown4C5 = false;
	WW3D::Get_Device_Resolution(Width, Height, Bits, Windowed);
	Unknown2C4 = 0;
	Unknown2C8 = 0;
	Scale = 1.0f;
	Unknown26C = 0;
	Unknown270 = 0;
	for (i = 0; i < 16; i++) Unknown274[i] = 0;
	for (i = 0; i < 6; i++) Unknown254[i] = 0;
	for (i = 0; i < 4; i++) {
		Effects[0][i] = Unknown_BE67B8->Find_Effect(Info.EffectOn[i]);
		Effects[1][i] = Unknown_BE67B8->Find_Effect(Info.EffectConst[i]);
		Effects[2][i] = Unknown_BE67B8->Find_Effect(Info.EffectOff[i]);
		Effects[3][i] = Unknown_BE67B8->Find_Effect(Info.MouseOver[i]);
		Effects[4][i] = Unknown_BE67B8->Find_Effect(Info.MouseClick[i]);
		Effects[5][i] = Unknown_BE67B8->Find_Effect(Info.EffectTriggered[i]);
	}
	if (Effects[0][0] == NULL && Effects[0][1] == NULL && Effects[0][2] == NULL) {
		Unknown440 = 1;
	}
	Unknown44C = 0;
	Colors[0] = Unknown_BE67B8->Find_Color(Info.Color[0]);
	Colors[1] = Unknown_BE67B8->Find_Color(Info.Color[1]);
	if (Colors[1] == NULL) {
		Colors[1] = Colors[0];
	}
	memset(Unknown303, 0, sizeof(Unknown303));
	Unknown414 = 0;
	FontStruct *font = Unknown_BE67B8->Find_Font(Info.FontType[FontIndex], Scale);
	REF_PTR_SET(Font, font->font32Ptr.Peek());
	Underline = Info.Underline;
	if (Info.Underline) {
		Unknown2E0.Unknown_6D8DC0(&Font, "_", 0);
	}
	Unknown4C5 = false;
	Unknown4E4 = true;
	Unknown4E6 = false;
}

// NOT EXACT: written from the client's code (0x6D5920) for the POC, not yet matched
void FontStyleClass::Set_Container(RenderObjClass *con)
{
	if (con == NULL) {
		Set_Sort_Level(SORT_LEVEL_NONE);
	} else {
		int level = con->Get_Sort_Level();
		int count = con->Get_Num_Sub_Objects();
		for (int i = 0; i < count; i++) {
			RenderObjClass *sub = con->Get_Sub_Object(i);
			if (sub != NULL) {
				if (sub->Class_ID() != 0x1000D) {
					int sub_level = sub->Get_Sort_Level();
					if (sub_level != SORT_LEVEL_NONE && (sub_level < level || level == SORT_LEVEL_NONE)) {
						level = sub_level;
					}
				}
				sub->Release_Ref();
			}
		}
		if (level != SORT_LEVEL_NONE) {
			Set_Sort_Level(level == 1 ? 1 : level - 1);
			Disable_Sort();
			RenderObjClass::Set_Container(con);
			return;
		}
	}
	Enable_Sort();
	RenderObjClass::Set_Container(con);
}

// 0x6D5B90
void FontStyleClass::Get_Obj_Space_Bounding_Sphere(SphereClass &sphere) const
{
	AABoxClass box;
	Get_Obj_Space_Bounding_Box(box);
	sphere.Center = box.Center;
	sphere.Radius = (box.Extent.X > box.Extent.Y) ? box.Extent.X : box.Extent.Y;
}

// NOT EXACT: written from the client's code (0x6D5C10) for the POC, not yet matched
void FontStyleClass::Get_Obj_Space_Bounding_Box(AABoxClass &box) const
{
	float x = (float)(Unknown420[1] - Unknown420[0]) * (640.0f / (float)Width) * 0.5f;
	float y = (float)(Unknown420[3] - Unknown420[2]) * (480.0f / (float)Height) * 0.5f;
	if (Info.Justify[1] == 3) {
		box.Center.X = x;
	} else if (Info.Justify[1] == 4) {
		box.Center.X = -x;
	} else {
		box.Center.X = 0.0f;
	}
	if (Info.Justify[0] == 1) {
		box.Center.Y = -y;
	} else if (Info.Justify[0] == 2) {
		box.Center.Y = y;
	} else {
		box.Center.Y = 0.0f;
	}
	box.Center.Z = 0.0f;
	box.Extent.X = x;
	box.Extent.Y = y;
	box.Extent.Z = 0.0f;
}

// NOT EXACT: written from the client's code (0x6D5D40) for the POC, not yet matched
void FontStyleClass::Unknown_6D5D40()
{
	if (Object != NULL) {
		RenderObjClass *sub = Object->Get_Sub_Object_By_Name(Info.Attach);
		if (sub != NULL) {
			Vector3 pos = sub->Get_Position();
			const AABoxClass &box = sub->Get_Bounding_Box();
			int value = (int)(box.Center.X - box.Extent.X + Info.BoxOffset[0]);
			Unknown430[0] = value < -320 ? -320 : (value > 320 ? 320 : value);
			value = (int)(box.Center.X + box.Extent.X - Info.BoxOffset[0]);
			Unknown430[1] = value < -320 ? -320 : (value > 320 ? 320 : value);
			value = (int)(box.Extent.Y + box.Center.Y - Info.BoxOffset[1]);
			Unknown430[2] = value < -240 ? -240 : (value > 240 ? 240 : value);
			value = (int)(box.Center.Y - box.Extent.Y + Info.BoxOffset[1]);
			Unknown430[3] = value < -240 ? -240 : (value > 240 ? 240 : value);
			sub->Release_Ref();
			Unknown250 = 1;
			return;
		}
		Unknown250 = 2;
	}
}

// NOT EXACT: written from the client's code (0x6D6390) for the POC, not yet matched
void FontStyleClass::Unknown_6D6390()
{
	const char *text = Info.Text;
	Unknown408 = (strcmp(text, " ") == 0);
	Unknown4C4 = Unknown2D0.Unknown_6D8DC0(&Font, text, 0);
	Unknown2C4 = Unknown420[1] = Font->String_Pixel_Width(text);
	Unknown2C8 = Unknown420[3] = Font->Get_Height();
	Update_Text(false);
	if (Object != NULL && Info.Attach.Get_Length() > 0) {
		Object->Add_Sub_Object_To_Bone(this, Info.Attach);
	}
}

/*
**	0x6D5D40-0x6D8270 (not yet rebuilt; Render's assert holds the client's line 914 below):
**	0x6D5F40, 0x6D60C0, 0x6D64C0 FontSys::Update_Text
**	(7,104 bytes: ledger FNT7), 0x6D8080, 0x6D80A0.
*/














































































































































































































































































































































































































































































































































































































































































// NOT EXACT: written from the client's code (0x6D8270) for the POC, not yet matched
void FontStyleClass::Render(RenderInfoClass &rinfo)
{
	Unknown4C4 = false;
	if (Unknown440 < 3) {
		G_ASSERT(SortLevel == SORT_LEVEL_NONE || !Model->Get_Flag(MeshGeometryClass::SORT));
		if (WW3D::Are_Static_Sort_Lists_Enabled() && SortLevel != SORT_LEVEL_NONE) {
			WW3D::Add_To_Static_Sort_List(this, SortLevel);
			return;
		}
		Vector3 pos = Get_Position();
		Update_Text(true);
		if (PolyCount != 0 && Is_Not_Hidden_At_All() && Unknown26C <= Unknown270) {
			Matrix3D tm = Transform;
			float x = tm[0][3];
			float y = tm[1][3];
			if (Unknown4DC > 1.0f) x *= Unknown4DC;
			if (Unknown4E0 > 1.0f) y *= Unknown4E0;
			float px = (float)floor(x / (640.0f / (float)Width));
			float py = (float)floor(y / (480.0f / (float)Height));
			x = (px + 0.5f) * (640.0f / (float)Width);
			y = (py + 0.5f) * (480.0f / (float)Height);
			if (Unknown4DC > 1.0f) x /= Unknown4DC;
			if (Unknown4E0 > 1.0f) y /= Unknown4E0;
			tm.Set_Translation(Vector3(x, y, 0.001f));
			DX8Wrapper::Set_Transform(D3DTS_WORLD, tm);
			Model->Render(rinfo);
		}
		Transform[0][3] = pos.X;
		Transform[1][3] = pos.Y;
		Transform[2][3] = pos.Z;
	}
}

// NOT EXACT: written from the client's code (0x6D87F0) for the POC, not yet matched
void FontStyleClass::Set_Transform(const Matrix3D &m)
{
	RenderObjClass::Set_Transform(m);
	Unknown4E5 = true;
	if (m[0][3] != Unknown4D0.X || Unknown4D0.Y != m[1][3] || Unknown4D0.Z != m[2][3]) {
		Unknown4D0 = m.Get_Translation();
		Unknown4C5 = false;
	}
}

// 0x6D88C0
void FontStyleClass::Set_Position(const Vector3 &v)
{
	RenderObjClass::Set_Position(v);
	Unknown4E5 = true;
	Unknown4C5 = false;
}

int Unknown_6D8B90Class::Count = 0;			// 0xBE67CC (interim here: fonttexture.cpp's, D137)
int Unknown_6D8B90Class::Unknown_BE67D8 = 0;	// 0xBE67D8 (interim here: fonttexture.cpp's, D137)

// NOT EXACT: written from the client's code (0x6D8B90) for the POC, not yet matched
Unknown_6D8B90Class::Unknown_6D8B90Class() :
	Size(0x20),
	Unknown04(0),
	Unknown08(0),
	Texture(NULL)
{
	Texture = new TextureClass(Size, Size, WW3D_FORMAT_A4R4G4B4, MIP_LEVELS_1, TextureClass::POOL_MANAGED, false);
	Count++;
	Unknown_BE67D8++;
}

/*
**	Reconstruction notes (fontsys.cpp)
**
**	The client's FontSys (the tree's FontStyleClass, fontmanager.h; rename pending): a DynamicMeshClass(2, 4) that
**	draws one string template. __FILE__ 'e:\build\beta\G\app\Client\fontsys.cpp' (0xB92828) is used once, by
**	Render 0x6D8270's G_ASSERT on line 914 (DynamicMeshClass::Render's check, copied). The file's start (the
**	fontmanager.cpp | fontsys.cpp boundary between 0x6D2A70 and 0x6D47C0) is not settled: the constructors
**	0x6D47C0 / 0x6D4C70 are here as the class's own code, interim (D137).
**	Vtable 0xAFECB4 (122 slots, as DynamicMeshClass's): destructor 0x6D4C40 (the deleting form of the inline
**	~FontStyleClass 0x67F230), Class_ID 0x67EF00, Get_Name 0x67EF20, Render 0x6D8270, Set_Container 0x6D5920,
**	Set_Transform 0x6D87F0, Set_Position 0x6D88C0, the bounding volumes 0x6D5B90 / 0x6D5C10, Set_Hidden 0x67EF40,
**	Is_Complete 0x67EEE0.
**	Functional (D64) for the POC client: TextGadgetClass's constructor needs a real object with a table. Not yet
**	rebuilt: Update_Text 0x6D64C0, 0x6D8C50, 0x6D8DC0 (fonttexture.cpp).
*/
