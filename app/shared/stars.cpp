// Earth & Beyond client (reconstruction): app\shared\stars.cpp. Notes at the end of the file.
// Load and Init are not byte-exact yet (notes); the rest of the file is.
#include "stars.h"
#include "pointgr.h"
#include "rinfo.h"
#include "camera.h"
#include "vp.h"
#include "ini.h"
#include "assetmgr.h"
#include "texture.h"
#include "shader.h"
#include "sampler.h"
#include "wwstring.h"
#include "wwmath.h"
#include <math.h>
#include <stdio.h>
#include "unknown_debug.h"

StarLayerClass::StarLayerClass(void) :
	PointGroup(NULL),
	NumStars(0),
	Quads(false)
{
}

StarLayerClass::~StarLayerClass(void)
{
	if (PointGroup) {
		delete PointGroup;
		PointGroup = NULL;
	}
}

void StarLayerClass::Render(RenderInfoClass &rinfo, unsigned int max_polys)
{
	static Matrix3D identity(true);
	unsigned int count = Quads ? max_polys >> 1 : max_polys;
	int n = (int)WWMath::Min((float)NumStars, (float)count);



	Matrix3D view = rinfo.Camera.Get_View_Matrix();
	view[0][3] = 0.0f;
	view[1][3] = 0.0f;
	view[2][3] = 0.0f;
	VectorProcessorClass::Transform(TransformedPoints->Get_Array(), Points->Get_Array(), view, n);
	PointGroup->Set_Arrays(TransformedPoints.Peek(), NULL, NULL, Sizes.Peek(), Orientations.Peek(),
		Frames.Peek(), n);
	PointGroup->Render(rinfo, identity);
}










































// NOT EXACT: order-only (the Hemisphere z load), parked: attempts.csv 0x481DB0
bool StarLayerClass::Load(const INIClass &ini, const char *section)
{
	FAIL_IF(!section) return false;
	FAIL_IF(!*section) return false;
	if (!ini.Find_Section(section)) {
		return false;
	}

	StringClass texture_name;
	QMCSamplingClass sampler(5);

	ShaderClass shader = ShaderClass::_PresetAdditiveSpriteShader;
	shader.Set_Depth_Compare(ShaderClass::PASS_ALWAYS);
	PointGroup = new PointGroupClass;
	PointGroup->Set_Shader(shader);
	PointGroup->Set_Point_Alpha(1.0f);
	PointGroup->Set_Point_Color(Vector3(1.0f, 1.0f, 1.0f));

	ini.Get_String(texture_name, section, "Texture");
	RefCountPtr<TextureClass> texture = Create_NEW(WW3DAssetManager::Get_Instance()->Get_Texture(texture_name));
	PointGroup->Set_Texture(texture.Peek());

	int log2_width = ini.Get_Int(section, "TextureLog2Width", 0);
	PointGroup->Set_Frame_Row_Column_Count_Log2(log2_width);
	Quads = ini.Get_Bool(section, "Quads", false);
	PointGroup->Set_Point_Mode(Quads ? PointGroupClass::QUADS : PointGroupClass::TRIS);

	NumStars = ini.Get_Int(section, "NumStars", 1000);
	float min_size = ini.Get_Float(section, "MinSize", 0.0f);
	float max_size = ini.Get_Float(section, "MaxSize", 1.0f);
	int qmc_offset = ini.Get_Int(section, "QMCOffset", 0);
	bool pick_randomly = ini.Get_Bool(section, "PickRandomly", true);
	bool hemisphere = ini.Get_Bool(section, "Hemisphere", false);

	int width = 1 << log2_width;
	float size_range = max_size - min_size;
	int frame_count = width * width;

	Points = Create_NEW(new ShareBufferClass<Vector3>(NumStars));
	TransformedPoints = Create_NEW(new ShareBufferClass<Vector3>(NumStars));
	Sizes = Create_NEW(new ShareBufferClass<float>(NumStars));
	Orientations = Create_NEW(new ShareBufferClass<unsigned char>(NumStars));
	Frames = Create_NEW(new ShareBufferClass<unsigned char>(NumStars));

	Vector3 *points = Points->Get_Array();
	float *sizes = Sizes->Get_Array();
	unsigned char *orientations = Orientations->Get_Array();
	unsigned char *frames = Frames->Get_Array();
	sampler.Set_Offset(qmc_offset);

	float r;
	for (int i = 0; i < NumStars; i++) {
		float sample[5];
		sampler.Sample(sample);
		if (hemisphere) {
			r = powf(1.0f - sample[1] * sample[1], 0.5f);
			points[i].Set((float)cos(sample[0] * 6.2831855f) * r, (float)sin(sample[0] * 6.2831855f) * r, sample[1]);
		} else {
			r = powf((1.0f - sample[1]) * sample[1], 0.5f);
			points[i].Set((float)cos(sample[0] * 6.2831855f) * r * 2.0f, (float)sin(sample[0] * 6.2831855f) * r * 2.0f, 1.0f - sample[1] * 2.0f);
		}
		points[i] *= 100.0f;
		sizes[i] = sample[2] * size_range + min_size;
		orientations[i] = (unsigned char)(sample[3] * 255.0f);
		frames[i] = pick_randomly ? (unsigned char)(frame_count * sample[4]) : (unsigned char)(i % frame_count);
	}
	return true;
}

StarFieldClass::StarFieldClass(unsigned int budget) :
	TotalPolys(0),
	Budget(budget)
{
	Set_Force_Visible(true);
}

StarFieldClass::~StarFieldClass(void)
{
	Free_Layers();
}

void StarFieldClass::Free_Layers(void)
{
	TotalPolys = 0;
	for (std::vector<StarLayerClass *>::iterator i = Layers.begin(); i != Layers.end(); i++) {
		delete *i;
	}
	Layers.clear();
}

// NOT EXACT: register-only (cmp al,bl on Load's result), parked: attempts.csv 0x4829E0
void StarFieldClass::Init(int system)
{
	Free_Layers();

	INIClass ini;
	ini.Load("stars.ini");
	if (ini.Is_Loaded()) {
		char section[100];
		StarLayerClass *layer;
		int count = 0;
		for (;;) {
			_snprintf(section, 100, "STAR%.4d\\%d", system, count);
			layer = new StarLayerClass;
			if (layer->Load(ini, section) == false) break;
			Layers.push_back(layer);
			count++;
		}
		delete layer;

		if (count == 0) {
			for (;;) {
				_snprintf(section, 100, "STAR%.4d\\%d", (system / 100) * 100, count);
				layer = new StarLayerClass;
				if (layer->Load(ini, section) == false) break;
				Layers.push_back(layer);
				count++;
			}
			delete layer;

			if (count == 0) {
				for (;;) {
					sprintf(section, "Default\\%d", count);
					layer = new StarLayerClass;
					if (layer->Load(ini, section) == false) break;
					Layers.push_back(layer);
					count++;
				}
				delete layer;
			}
		}

		TotalPolys = 0;
		for (std::vector<StarLayerClass *>::iterator i = Layers.begin(); i != Layers.end(); i++) {
			TotalPolys += (*i)->Get_Num_Polys();
		}
	}
}

void StarFieldClass::Render(RenderInfoClass &rinfo)
{
	int remaining = Budget;
	for (std::vector<StarLayerClass *>::iterator i = Layers.begin(); i != Layers.end(); i++) {
		(*i)->Render(rinfo, remaining);
		remaining -= (*i)->Get_Num_Polys();
		if (remaining <= 0) break;
	}
}

int StarFieldClass::Get_Num_Polys(void) const
{
	return TotalPolys;
}

float StarFieldClass::Get_Cost(void) const
{
	float cost = (float)TotalPolys;
	if ((float)Budget < cost) {
		cost = (float)Budget;
	}
	return cost;
}

/*
**	Reconstruction notes
**
**	client.exe 0x481A20-0x482FC0: the star field (D46: the client's 2004 version, which VIDTEST's
**	2002 copy 0x421CE0-0x422ED0 follows closely: StarLayer Load 0x421F90 ~ 0x481DB0 0.99). The
**	asserts sit on the client's lines (96 '!section', 97 '!*section', FAIL_IF, D25).
**
**	  0x481A20 StarLayerClass::StarLayerClass     0x4826E0 StarFieldClass::StarFieldClass
**	  0x481A50 StarLayerClass::~StarLayerClass    0x482790 StarFieldClass::~StarFieldClass
**	  0x481B70 StarLayerClass::Render             0x482920 StarFieldClass::Free_Layers
**	  0x481DB0 StarLayerClass::Load               0x4829E0 StarFieldClass::Init
**	                                              0x482D80 StarFieldClass::Render
**	                                              0x482E00 / 0x482E20 Get_Num_Polys / Get_Cost
**	  0x482EB0 vector erase, 0x483250 / 0x483020 vector inserts, 0x482F00 / 0x482F60 / 0x482FC0
**	  Create_NEW for the three ShareBufferClass types, 0x47D010 Create_NEW<TextureClass> (outside).
**
**	Load: the shader is _PresetAdditiveSpriteShader (0xC010F8) with depth compare PASS_ALWAYS (or 7);
**	the sampler is QMCSamplingClass(5) offset by QMCOffset; a point is uniform on the sphere
**	(Hemisphere: on the upper half), radius 100. Render: the view matrix without its translation,
**	the points transformed into camera space and drawn with an identity transform; the budget
**	counts polygons (two per star in quad mode).
**
**	NOT EXACT: Load and Init only (the other functions are exact). Load (0.70): the asserts on the
**	client's lines, the frames store as one ternary and powf (r stored before pow) match; left: the
**	Hemisphere branch loads z (sample[1]) last where the client loads it first, the fifth
**	Create_NEW temporary zeroed after its release, frame 0x4C vs 0x50. Init (0.72): the client
**	compares Load's result with cmp al,bl (ebx 0) and calls the vector's insert(it, x) 0x483020 out
**	of line in the Default loop. Both parked (attempts.csv).
*/
