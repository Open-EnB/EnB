// Earth & Beyond client (reconstruction): debrsint.cpp, the debris fields' interface (notes at the end)
#include <stdio.h>
#include "debrsint.h"
#include "sclient.h"
#include "ini.h"
#include "assetmgr.h"
#include "texture.h"
#include "..\shared\g_assert.h"
// NOT EXACT: functional (D64), register-only (ebx / edi / ebp swapped; two rounds: the fade clamps' temporaries)
// 0x53D250
DebrisInterfaceClass::DebrisInterfaceClass(UIViewClass *view, SimpleSceneClass *scene, INIClass &ini) :
	InterfaceClass(view, (int)scene),
	Mode(0)
{
	char section[256];
	char texture_name[256];
	int index = 0;
	sprintf(section, "Debris%02d", index);
	while (ini.Find_Section(section)) {
		TextureClass *texture = NULL;
		float spacing = ini.Get_Float(section, "Spacing", 200.0f);
		float size = ini.Get_Float(section, "Size", 0.0001f);
		float min_dist = ini.Get_Float(section, "MinDist", 0.0f);
		if (min_dist < 0.0f) {
			min_dist = 0.0f;
		}
		float max_dist = ini.Get_Float(section, "MaxDist", 900.0f);
		if (max_dist < 1.0f) {
			max_dist = 1.0f;
		}
		if (max_dist <= min_dist) {
			max_dist = min_dist * 1.1f;
		}
		float r = ini.Get_Float(section, "R", 1.0f);
		float g = ini.Get_Float(section, "G", 1.0f);
		float b = ini.Get_Float(section, "B", 1.0f);
		float a = ini.Get_Float(section, "A", 1.0f);
		bool additive = ini.Get_Bool(section, "Additive", true);
		float fade_in; float fade_in_value = ini.Get_Float(section, "FadeIn", 0.01f);
		if (fade_in_value < 0.01f) {
			fade_in = 0.01f;
		} else if (fade_in_value > 0.99f) {
			fade_in = 0.99f;
		} else fade_in = fade_in_value;
		float fade_out; float fade_out_value = ini.Get_Float(section, "FadeOut", 0.01f);
		if (fade_out_value < 0.01f) {
			fade_out = 0.01f;
		} else if (fade_out_value > 0.99f) {
			fade_out = 0.99f;
		} else fade_out = fade_out_value;
		if (fade_in <= fade_out) {
			fade_in = (fade_out + 1.0f) * 0.5f;
		}
		int primitive = ini.Get_Int(section, "Primitive", 0);
		unsigned rows_columns_log2 = ini.Get_Int(section, "RowsColumnsLog2", 0);
		if (rows_columns_log2 > 4) {
			rows_columns_log2 = 4;
		}
		ini.Get_String(section, "Texture", "", texture_name, 255);
		if (texture_name[0] != '\0') {
			texture = WW3DAssetManager::Get_Instance()->Get_Texture(texture_name);
		}
		float blur_time = ini.Get_Float(section, "BlurTime", 0.0f);
		DebrisFieldClass *field = new DebrisFieldClass(spacing, size, min_dist, max_dist, r, g, b, a, additive, texture, fade_in, fade_out, primitive, rows_columns_log2, blur_time);
		REF_PTR_RELEASE(texture);
		Fields.Add(field);
		index++;
		sprintf(section, "Debris%02d", index);
	}
}

// 0x53D840
DebrisInterfaceClass::~DebrisInterfaceClass()
{
	int count = Fields.Count(); for (int i = 0; i < count; i++) {
		if (Fields[i]) {
			Fields[i]->Release_Ref();
			Fields[i] = NULL;
		}
	}
}

// 0x53D960
void DebrisInterfaceClass::Unknown_58()
{
	Fields[Mode]->Add(Peek_Scene());
	InterfaceClass::Unknown_58();
}

// 0x53D9A0
void DebrisInterfaceClass::Unknown_5C()
{
	InterfaceClass::Unknown_5C();
	Fields[Mode]->Remove();
}

// 0x53D9D0
void DebrisInterfaceClass::Unknown_60()
{
}

















































// NOT EXACT: functional (D64), order-only (key 10's colour: the red's spill after the blue's multiply)
// 0x53D9F0
bool DebrisInterfaceClass::Unknown_68(int key)
{
	DebrisFieldClass *field = Fields[Mode];
	switch (key) {
		case 0:
			if (Active) {
				Unknown_5C();
			} else {
				Unknown_58();
			}
			break;
		case 1:
			field->Spacing *= 0.7937;
			break;
		case 2:
			field->Spacing *= 1.256;
			break;
		case 3:
			field->Size *= 1.5;
			break;
		case 4:
			field->Size *= 0.6666667;
			break;
		case 5:
			field->MaxDist *= 1.5;
			break;
		case 6:
			field->MaxDist *= 0.6666667;
			break;
		case 7:
			field->MinDist *= 1.5;
			break;
		case 8:
			field->MinDist *= 0.6666667;
			break;
		case 9:
			switch (field->Unknown_6BB440()) {
				case 0:
					field->Alpha *= 0.83333f;
					return(true);
				case 1:
					field->Color.X *= 0.83333f;
					field->Color.Y *= 0.83333f;
					field->Color.Z *= 0.83333f;
					return(true);
				default:
					G_ASSERT(0);
					return(true);
			}
			break;


		case 10:
			switch (field->Unknown_6BB440()) {
				case 0:
					field->Alpha *= 1.2f;
					return(true);
				case 1:
				{
					float r = field->Color.X * 1.2f;
					float g = field->Color.Y * 1.2f;
					float b = field->Color.Z * 1.2f;
					field->Set_Color((r > 1.0f) ? 1.0 : r, (g > 1.0f) ? 1.0 : g, (b > 1.0f) ? 1.0 : b);
					return(true);
				}





				default:
					G_ASSERT(0);
					return(true);
			}
			break;
		case 11:
		{
			int mode = field->UnknownBC;
			switch (mode) {
				case 0:
					mode = 1;
					break;
				case 1:
					mode = 2;
					break;
				case 2:
					mode = 0;
					break;
			}
			field->UnknownBC = mode;
			field->Unknown_6BB470();
			return(true);
		}
		default:
			G_ASSERT(0);
			return(false);
	}
	return(true);
}
// NOT EXACT: functional (D64), frame (0x10C vs 0x108: the colour's copy; the client passes the red through the FPU, the green and blue from two spills)
// 0x53DDE0
void DebrisInterfaceClass::Unknown_6C(INIClass &ini)
{
	char section[256];
	int count = Fields.Count(); for (int i = 0; i < count; i++) {
		sprintf(section, "Debris%02d", i);
		ini.Put_Float(section, "Spacing", Fields[i]->Get_Spacing());
		ini.Put_Float(section, "Size", Fields[i]->Get_Size());
		ini.Put_Float(section, "MinDist", Fields[i]->Get_Min_Dist());
		ini.Put_Float(section, "MaxDist", Fields[i]->Get_Max_Dist());
		Vector3 color = Fields[i]->Get_Color();
		ini.Put_Float(section, "R", color.X);
		ini.Put_Float(section, "G", color.Y);
		ini.Put_Float(section, "B", color.Z);
		ini.Put_Float(section, "A", Fields[i]->Get_Alpha());
	}
}

// 0x53DFB0
void DebrisInterfaceClass::Set_Mode(unsigned mode)
{
	unsigned last = Fields.Count() - 1;
	if (mode >= last) {
		mode = last;
	}
	if (mode != Mode) {
		if (Active) {
			Fields[Mode]->Remove();
			Fields[mode]->Add(Peek_Scene());
		}
		Mode = mode;
	}
}

// 0x53E030
void DebrisInterfaceClass::Unknown_53E030(float value)
{
	DebrisFieldClass *field = Fields[Mode];
	if (value < 0.0f) {
		field->UnknownC8 = 0.0f;
	} else if (value > 1.0f) {
		field->UnknownC8 = 1.0f;
	} else {
		field->UnknownC8 = value;
	}
}

// 0x53E0B0
float DebrisInterfaceClass::Unknown_53E0B0()
{
	return(Fields[Mode]->UnknownC8);
}

// 0x53E0E0
float DebrisInterfaceClass::Get_Spacing()
{
	return(Fields[Mode]->Spacing);
}

// 0x53E110
void DebrisInterfaceClass::Set_Spacing(float spacing)
{
	Fields[Mode]->Spacing = (1.0f > spacing) ? 1.0f : spacing;



}

// 0x53E160
float DebrisInterfaceClass::Get_Size()
{
	return(Fields[Mode]->Size);
}

// 0x53E190
void DebrisInterfaceClass::Set_Size(float size)
{
	Fields[Mode]->Size = (0.0f > size) ? 0.0f : size;



}

// 0x53E1E0
void DebrisInterfaceClass::Get_Color(Vector3 &color)
{
	DebrisFieldClass *field = Fields[Mode];
	color.X = field->Color.X; color.Y = field->Color.Y; color.Z = field->Color.Z;
}

// 0x53E220
void DebrisInterfaceClass::Set_Color(const Vector3 &color)
{
	Fields[Mode]->Set_Color(color.X, color.Y, color.Z);
}

// 0x53E260
float DebrisInterfaceClass::Get_Alpha()
{
	return(Fields[Mode]->Alpha);
}

// 0x53E290
void DebrisInterfaceClass::Set_Alpha(float alpha)
{
	Fields[Mode]->Alpha = alpha;
}

// 0x53E2C0
float DebrisInterfaceClass::Get_Min_Dist()
{
	return(Fields[Mode]->MinDist);
}
// NOT EXACT: functional (D64), literal (the client compares the MaxDist it loads with the clamped value kept in st(1): fld [MaxDist]; fcomp st(1))
// 0x53E2F0
void DebrisInterfaceClass::Set_Min_Dist(float min_dist)
{
	if (min_dist < 0.0f) {
		min_dist = 0.0f;
	}
	DebrisFieldClass *field = Fields[Mode];
	float max = field->MaxDist; if (max <= min_dist) {
		float max_dist = min_dist * 1.1f;
		if (max_dist < 1.0f) {
			max_dist = 1.0f;
		}
		if (max_dist <= field->MinDist) {
			max_dist = field->MinDist * 1.1f;
		}
		field->MaxDist = max_dist;
	}
	Fields[Mode]->MinDist = min_dist;
}

// 0x53E3C0
float DebrisInterfaceClass::Get_Max_Dist()
{
	return(Fields[Mode]->MaxDist);
}

// 0x53E3F0
void DebrisInterfaceClass::Set_Max_Dist(float max_dist)
{
	if (max_dist < 1.0f) {
		max_dist = 1.0f;
	}
	DebrisFieldClass *field = Fields[Mode];
	if (field->MinDist >= max_dist) {
		field->MaxDist = field->MinDist * 1.1f;
	} else {
		field->MaxDist = max_dist;
	}
}

// 0x53E470
void DebrisInterfaceClass::Unknown_53E470(const Vector3 &offset)
{
	Fields[Mode]->Unknown_6BB640(offset);
}

// 0x53E4A0
float DebrisInterfaceClass::Get_Blur_Time()
{
	return(Fields[Mode]->BlurTime);
}

// 0x53E4D0
void DebrisInterfaceClass::Set_Blur_Time(float blur_time)
{
	Fields[Mode]->BlurTime = blur_time;
}

/*
**	Reconstruction notes (from the retail client.exe, SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\client\debrsint.cpp (its __FILE__ 0xB79068: G_ASSERT '0' on lines 198 / 223
**	/ 246, slot +0x68's two inner switches and its default). Client 0x53D250-0x53E968: the constructor, the deleting
**	destructor 0x53D7D0, DynamicVectorClass<DebrisFieldClass *>'s destructor 0x53D800, the destructor 0x53D840, slots
**	+0x58 / +0x5C / +0x60 / +0x68 / +0x6C, the accessors 0x53DFB0-0x53E4D0 (what the main view's 0x513CD0-0x513F30
**	forward), the vector's instances 0x53E500-0x53E8F0 and the closing thunks 0x53E950 / 0x53E960 (no <string> static).
**	The class name follows the file (D24), as backint.cpp's BackInterfaceClass; the fields are debris.cpp's
**	DebrisFieldClass (its profile name 'DebrisFieldClass::Render', its constructor 0x6B8F20 takes the Debris%02d keys).
*/
