// Earth & Beyond client (reconstruction): app\Client\galaxy_map_robj.cpp (notes at the end)
#include <math.h>
#include "galaxy_map_robj.h"
#include "fontmanager.h"
#include "..\..\wwlib\ini.h"
#include "..\..\ww3d2\assetmgr.h"
#include "..\..\ww3d2\texture.h"
#include "..\..\ww3d2\vertmaterial.h"
#include "..\..\ww3d2\matinfo.h"
#include "..\..\ww3d2\ww3d.h"
#include "wwmath.h"
#include "..\shared\g_assert.h"

// segline_create.cpp's holder of a segmented line (as co.cpp declares it)
class Unknown4981E0Class	// NAME 1: neutral (D10)
{
public:
	Unknown4981E0Class(const RefCountPtr<RenderObjClass> &line);	// 0x4982F0  // NAME 1 [line]: what it holds
	~Unknown4981E0Class();						// 0x498320
	void Unknown_4983A0(INIClass *ini, const char *section, float scale, const Vector3 &hsv);	// 0x4983A0 (co.cpp declares the last as float *)  // NAME 1 [ini .. hsv]: what co.cpp's 0x6F51D0 passes
	RenderObjClass *Line;						// +0x00  // NAME 2: what it holds
};

static int _max_polys = 2;			// 0xB9EB1C  // NAME 1: what DynamicMeshClass takes
static int _max_verts = 4;			// 0xB9EB20  // NAME 1: what DynamicMeshClass takes
static float _name_scale = -1.0f;	// 0xB9EB24: galaxy_map_c.ini's SystemNameScale  // NAME 3: the FAIL_IF's
static float _max_tile = 0.0f;		// 0xBE8720: the line's tile factor at 256  // NAME 1: what it holds
























// NOT EXACT: registers only (Placement read through ecx, the client's eax; the Name temporary's address in edx, the client's ecx)
// @0x7B2B30
GalaxyMapRObjClass::GalaxyMapRObjClass(const RefCountPtr<GalaxyMapSystemClass> &system) :
	DynamicMeshClass(_max_polys, _max_verts), UVFlags(system->HashID),
	Color(system->Unknown20),
	NameOffset(0, 0),
	Blink(0),
	Placement(system->Unknown3C)
{

	if (_name_scale < 0.0f) {
		INIClass ini("galaxy_map_c.ini");
		_name_scale = ini.Get_Float("GalaxyMap", "SystemNameScale", 0.3f);
		FAIL_IF(_name_scale < WWMATH_EPSILON) { _name_scale = 0.3f; }
	}
	Name = Create_NEW(new FontSys("GALAXY_MAP_SYSTEM_NAME", 0, NULL));
	Name->Change_Text(system->Name.c_str(), 0);
	Name->Set_Color(Color);
	Name->Unknown_6D8940(_name_scale);
	Name->Unknown450 = 1;
	switch (Placement) {
		case 2: Name->Set_Justify(2, 0); break;
		case 0: Name->Set_Justify(0, 4); break;
		case 1: Name->Set_Justify(0, 3); break;
		case 3: Name->Set_Justify(1, 0); break;
	}
	Texture = Create_Get(WW3DAssetManager::Get_Instance()->Get_Texture("sm_system00.tga", MIP_LEVELS_1, WW3D_FORMAT_UNKNOWN, true));
	Material = Create_NEW(new VertexMaterialClass);
	Material->Set_Ambient(0, 0, 0);
	Material->Set_Diffuse(0, 0, 0);
	Material->Set_Specular(0, 0, 0);
	Material->Set_Emissive(Color);
	Material->Set_Lighting(true);
	Material->Make_Unique();
	Vector2 empty(-0.5f, -5.0f);
	Unknown_7B3770(empty, empty, false);
}

// @0x7B3100
GalaxyMapRObjClass::~GalaxyMapRObjClass()
{
}

// @0x7B31F0
void GalaxyMapRObjClass::Set_Position(const Vector3 &v)
{
	DynamicMeshClass::Set_Position(v);
	Unknown_7B3C60();
}

// @0x7B3260
void GalaxyMapRObjClass::Add(SceneClass *scene)
{
	Name->Set_Hidden(0);
	Name->Unknown_6CE7D0(0);
	Name->Add(scene);
	DynamicMeshClass::Add(scene);
}

// @0x7B32B0
void GalaxyMapRObjClass::Remove(void)
{
	Name->Remove();
	DynamicMeshClass::Remove();
}

// @0x7B32E0
void GalaxyMapRObjClass::Render(RenderInfoClass &rinfo)
{
	if (Blink) {
		Blink += WW3D::Get_Frame_Time();
		int t = Blink % 1000;
		if (t > 500) {
			t = 1000 - t;
		}
		float f = t * 0.002f;
		Vector3 color = Lerp(Color, Vector3(0, 0, 0), f * 0.75f);
		Material->Set_Emissive(color);
		Name->Set_Color(color);
	}
	DynamicMeshClass::Render(rinfo);
}

// @0x7B3440
void GalaxyMapRObjClass::Unknown_7B3440(std::vector<Vector3> &points)
{
	if (points.begin() == points.end()) {
		Vector2 empty(-0.5f, -5.0f);
		Unknown_7B3770(empty, empty, true);
		return;
	}
	std::vector<Vector3>::iterator it = points.begin();
	float min_x = it->X; float max_x = it->X;
	float min_y = it->Y; float max_y = it->Y;

	std::vector<Vector3>::iterator best = it;
	float best_len = fabs(it->Y) + fabs(it->X);
	for (++it; it != points.end(); ++it) {
		if (min_x > it->X) {
			min_x = it->X;
		} else if (max_x < it->X) {
			max_x = it->X;
		}
		if (min_y > it->Y) {
			min_y = it->Y;
		} else if (max_y < it->Y) {
			max_y = it->Y;
		}
		float len = fabs(best->Y) + fabs(best->X);
		if (best_len < len) {
			best_len = len;
			best = it;
		}
	}
	UVFlags.FlipU = best->X > 0.0f;
	UVFlags.FlipV = best->Y < 0.0f;
	Vector2 min(min_x - 1.0f, min_y - 1.0f);
	Vector2 max(max_x + 1.0f, max_y + 1.0f);
	float dx = (max.X - min.X) * 0.125f;
	float dy = (max.Y - min.Y) * 0.125f;
	min.X -= dx;
	max.X += dx;
	min.Y -= dy;
	max.Y += dy;
	Unknown_7B3770(min, max, true);
}

// @0x7B3690
void GalaxyMapRObjClass::Unknown_7B3690(bool on)
{
	if (on) {
		if (!Blink) {
			Blink = 1;
		}
	} else if (Blink) {
		Blink = 0;
		Material->Set_Emissive(Color);
	}
}

// @0x7B3700
void GalaxyMapRObjClass::Unknown_7B3700(bool on)
{
	if (on) {
		Name->Set_Hidden(0);
		if (Name->Unknown440 == 2) {
			Name->Unknown_6CE7D0(0);
		}
	} else if (Name->Unknown440 != 2) {
		Name->Unknown_6CE7D0(2);
	}
}

static inline void Swap_UV(Vector2 &uv) { if (uv.X != uv.Y) { float t = uv.X; uv.X = uv.Y; uv.Y = t; } }	// (inlined four times in 0x7B3770)  // NAME 1: what it does





































// @0x7B3770
void GalaxyMapRObjClass::Unknown_7B3770(const Vector2 &min, const Vector2 &max, bool resize)
{
	if (resize) {
		Resize(_max_polys, _max_verts);
	}
	ShaderClass shader = ShaderClass::_PresetAlpha2DShader;
	shader.Set_Cull_Mode(ShaderClass::CULL_MODE_DISABLE);
	shader.Set_Primary_Gradient(ShaderClass::GRADIENT_MODULATE);
	Set_Shader(shader);
	Model->Set_Single_Material(Material.Peek());
	Model->Set_Single_Texture(Texture.Peek());
	MaterialInfoClass *matinfo = Peek_Material_Info();
	FAIL_IF(!matinfo) return;
	matinfo->Add_Vertex_Material(Material.Peek());
	matinfo->Add_Texture(Texture.Peek());
	Vector2 tr, br, tl, bl;
	tl.Set(UVFlags.FlipU, UVFlags.FlipV);
	bl.Set(UVFlags.FlipU, !UVFlags.FlipV);
	tr.Set(!UVFlags.FlipU, UVFlags.FlipV);
	br.Set(!UVFlags.FlipU, !UVFlags.FlipV);
	if (UVFlags.SwapUV) {
		Swap_UV(tl);
		Swap_UV(bl);
		Swap_UV(tr);
		Swap_UV(br);
	}
	Begin_Tri_Strip();
	Vertex(min.X, max.Y, 0, tl.U, tl.V);
	Vertex(max.X, max.Y, 0, tr.U, tr.V);
	Vertex(min.X, min.Y, 0, bl.U, bl.V);
	Vertex(max.X, min.Y, 0, br.U, br.V);
	End_Tri_Strip();
	Invalidate_Cached_Bounding_Volumes();
	switch (Placement) {
		case 2: NameOffset.Set((min.X + max.X) * 0.5f, max.Y); break;
		case 0: NameOffset.Set(min.X, (max.Y + min.Y) * 0.5f); break;
		case 1: NameOffset.Set(max.X, (max.Y + min.Y) * 0.5f); break;
		case 3: NameOffset.Set((min.X + max.X) * 0.5f, min.Y); break;
	}
	Unknown_7B3C60();
}

// @0x7B3C60
void GalaxyMapRObjClass::Unknown_7B3C60()
{
	Vector3 pos = Get_Position();
	pos.X += NameOffset.X;
	pos.Y += NameOffset.Y;
	Name->Set_Position(pos);
}

// @0x7B3CD0
void GalaxyMapRObjClass::Unknown_7B3CD0()
{
	if (!Blink) {
		Material->Set_Emissive(Color);
		return;
	}
	Blink += WW3D::Get_Frame_Time();
	int t = Blink % 1000;
	if (t > 500) {
		t = 1000 - t;
	}
	float f = t * 0.002f;
	Vector3 color = Lerp(Color, Vector3(0, 0, 0), f * 0.75f);
	Material->Set_Emissive(color);
	Name->Set_Color(color);
}

// @0x7B3E30
GalaxyMapLineRObjClass::GalaxyMapLineRObjClass() :
	Color(0, 0, 0),
	Hidden(false),
	Fade(0)
{
}

// @0x7B3ED0
GalaxyMapLineRObjClass::~GalaxyMapLineRObjClass()
{
}









































// @0x7B3F00
bool GalaxyMapLineRObjClass::Unknown_7B3F00(INIClass &ini, const char *name, const Vector3 &color, RefCountPtr<RenderObjClass> from, RefCountPtr<RenderObjClass> to)
{
	FAIL_IF(!from) return(false);
	FAIL_IF(!to) return(false);
	Unknown4981E0Class segline(Create_Peek((RenderObjClass *)this));

	segline.Unknown_4983A0(&ini, name, 1.0f, Vector3(0, 0, 0));
	RefCountPtr<TextureClass> texture = Create_Get(Get_Texture());


	FAIL_IF(!texture) return(false);
	texture->Set_Mip_Mapping(TextureClass::FILTER_TYPE_NONE);

	Color = color;
	Hidden = false;
	Fade = 0;
	Set_Color(Vector3(0, 0, 0));
	float tile = Get_Texture_Tile_Factor();
	if (_max_tile == 0.0f) {
		Set_Texture_Tile_Factor(256.0f);
		_max_tile = Get_Texture_Tile_Factor();
	}
	Vector3 start = from->Get_Position();
	Vector3 end = to->Get_Position();
	Vector3 delta = end - start;
	float length = delta.Length();
	tile = length * tile;
	int count = 1;
	while (tile > _max_tile) {
		tile *= 0.5f;
		count += count;
	}
	Vector3 step = delta; step /= (float)count;
	while (Get_Num_Points()) {
		Delete_Point(Get_Num_Points() - 1);
	}
	while (count--) {
		Add_Point(start);
		start += step;
	}
	Add_Point(end);
	Set_Texture_Tile_Factor(tile);
	return(true);
}

// NOT EXACT: x87 operand order (the client multiplies Color's members by the clamped Fade held in st(1), ours keeps a copy with fld st(0))
// @0x7B43F0
void GalaxyMapLineRObjClass::Render(RenderInfoClass &rinfo)
{
	float time = WW3D::Get_Frame_Time() * 0.001;
	if (Hidden) {
		if (Fade <= 0.0f) {
			return;
		}
		Fade = WWMath::Clamp(Fade - time, 0.0f, 1.0f);
		Set_Color(Color * Fade);
	} else if (Fade < 1.0f) {
		Fade = WWMath::Clamp(time + Fade, 0.0f, 1.0f);
		Set_Color(Color * Fade);
	}
	SegmentedLineClass::Render(rinfo);
}

/*
**	Reconstruction notes
**
**	The client's __FILE__ 0xB9EB80 (e:\build\beta\G\app\Client\galaxy_map_robj.cpp), FAIL_IFs at lines
**	65, 256, 369, 370 and 377; client 0x7B2B30-0x7B46EA, the object's <string> initialiser at
**	0x7B4640 followed by the tables' adjustor thunks 0x7B4670-0x7B46A0 and PermStringInit's
**	initialiser 0x7B46B0. Out-of-line templates here: Create_Peek 0x7B45A0, DynamicVectorClass<
**	VertexMaterialClass *>::Add 0x7B45D0; the classes' inline Class_ID 0x7B30B0 (DynamicMeshClass's)
**	and 0x7B3E80 (SegmentedLineClass's).
*/
