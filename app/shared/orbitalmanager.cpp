// Earth & Beyond client (reconstruction): app\shared\orbitalmanager.cpp (the client's __FILE__ 0xB759A0). Notes at the end.
#include "orbitalmanager.h"
#include "..\client\sclient.h"
#include "..\client\co.h"
#include "spln_db.h"
#include "ini.h"
#include "curve.h"
#include "rendobj.h"
#include "camera.h"
#include "scene.h"
#include "mesh.h"
#include "matrix3.h"
#include "matrix3d.h"
#include "lineseg.h"
#include "colmath.h"
#include "castres.h"
#include "sampler.h"
#include "wwmath.h"
#include "wwprofile.h"
#include "..\shared\g_assert.h"
#include <math.h>
#include <stdio.h>

void Unknown_496AE0(RenderObjClass *robj, bool make_geometry_unique);	// 0x496AE0
void Unknown_75B7B0(RenderObjClass *robj, float phase);					// 0x75B7B0 (a planet's phase)  // NAME 1 [robj, phase]: neutral
void Unknown_75B8F0(RenderObjClass *robj, const Vector3 &dir, const Vector3 &camera);	// 0x75B8F0 (a planet lit from dir)  // NAME 1 [robj, dir, camera]: neutral

// the main view's callback, as orbitalmanager.cpp calls it (mainview.cpp defines it: table 0xAED01C)
class MainViewOrbitalCallbackClass
{
public:
	virtual SimpleSceneClass *Unknown_00();
	virtual unsigned Unknown_04();
};

// one orbital of orbital%04d.ini's [ORBITAL%3.3d] (0x70 bytes)
class OrbitalObject		// NAME 2: the client's (OrbitalObject::Update)
{
public:
	OrbitalObject();									// 0x4F28A0
	~OrbitalObject();									// 0x4F2930
	static OrbitalObject *Create(INIClass &ini, const char *section, ClientClass *client);	// 0x4F2960  // NAME 1 [Create, ini, section, client]: neutral
	void Update(CameraClass *camera, float time);		// 0x4F2CB0  // NAME 1 [camera, time]: what OrbitalManager::Update passes

	RenderObjClass *Object;								// +0x00  // NAME 2: the client's assert 'orb->Object'
	float StartAngle;									// +0x04  // NAME 2: the INI key
	float RX;											// +0x08  // NAME 2: the INI key
	float RZ;											// +0x0C  // NAME 2: the INI key
	float Distance;										// +0x10  // NAME 2: the INI key
	float OrbitsPerHour;								// +0x14  // NAME 2: the INI key
	float PhasePerHour;									// +0x18  // NAME 2: the INI key
	float LightRadius;									// +0x1C  // NAME 2: the INI key
	bool EmitsLight;									// +0x20  // NAME 2: the INI key
	Curve1DClass *LightIntensityR;						// +0x24  // NAME 2: the INI key
	Curve1DClass *LightIntensityG;						// +0x28  // NAME 2: the INI key
	Curve1DClass *LightIntensityB;						// +0x2C  // NAME 2: the INI key
	Curve1DClass *SkyIntensityR;						// +0x30  // NAME 2: the INI key
	Curve1DClass *SkyIntensityG;						// +0x34  // NAME 2: the INI key
	Curve1DClass *SkyIntensityB;						// +0x38  // NAME 2: the INI key
	Vector3 Direction;									// +0x3C (from the camera)  // NAME 1: neutral
	float Angle;										// +0x48 (the elevation)  // NAME 1: neutral
	float Time;											// +0x4C (the curves' time)  // NAME 1: neutral
	Vector3 LightValue;									// +0x50  // NAME 1: neutral
	Vector3 SkyValue;									// +0x5C  // NAME 1: neutral
	float Unknown68;									// +0x68 (how far it is up)
	float Unknown6C;									// +0x6C (how much of it is unblocked)
};


OrbitalObject::OrbitalObject() :
	Object(NULL),
	StartAngle(0),
	RX(0),
	RZ(0),
	Distance(100.0f),
	OrbitsPerHour(0),
	PhasePerHour(0),
	LightRadius(0.05f),
	EmitsLight(false),
	LightIntensityR(NULL),
	LightIntensityG(NULL),
	LightIntensityB(NULL),
	SkyIntensityR(NULL),
	SkyIntensityG(NULL),
	SkyIntensityB(NULL),
	Direction(1, 0, 0),
	Angle(0),
	Time(0),
	LightValue(0, 0, 0),
	SkyValue(0, 0, 0),
	Unknown68(0),
	Unknown6C(1.0f)
{
}


OrbitalObject::~OrbitalObject()
{
	REF_PTR_RELEASE(Object);
}
















OrbitalObject *OrbitalObject::Create(INIClass &ini, const char *section, ClientClass *client)
{
	SplineDatabaseClass *splines = client->Splines;
	if (ini.Find_Section(section) == NULL) {
		return(NULL);
	}
	OrbitalObject *orb = new OrbitalObject;
	int object = ini.Get_Int(section, "OBJECT", 0);
	float scale = ini.Get_Float(section, "SCALE", 1.0f);
	float hsv[3];
	hsv[0] = ini.Get_Float(section, "HUE", 0);
	hsv[1] = ini.Get_Float(section, "SAT", 0);
	hsv[2] = ini.Get_Float(section, "VAL", 0);
	orb->Object = ClientObjectClass::Unknown_6F5140(object, client, scale, hsv);
	G_ASSERT(orb->Object);
	orb->StartAngle = ini.Get_Float(section, "STARTANGLE", 0);
	orb->RX = ini.Get_Float(section, "RX", 0);
	orb->RZ = ini.Get_Float(section, "RZ", 0);
	orb->Distance = ini.Get_Float(section, "DISTANCE", 100.0f);
	orb->OrbitsPerHour = ini.Get_Float(section, "ORBITSPERHOUR", 0);
	orb->PhasePerHour = ini.Get_Float(section, "PHASEPERHOUR", 0);
	orb->EmitsLight = ini.Get_Bool(section, "EMITS_LIGHT", false);
	if (orb->EmitsLight) {
		char name[100];
		ini.Get_String(section, "LIGHTINTENSITY", "LightIntensity.Sun", name, sizeof(name));
		splines->Get_Color_Curves(name, &orb->LightIntensityR, &orb->LightIntensityG, &orb->LightIntensityB);
		ini.Get_String(section, "SKYINTENSITY", "SkyIntensity.Sun", name, sizeof(name));
		splines->Get_Color_Curves(name, &orb->SkyIntensityR, &orb->SkyIntensityG, &orb->SkyIntensityB);
		orb->LightRadius = ini.Get_Float(section, "LIGHTRADIUS", 0);
		if (orb->LightRadius == 0) {
			orb->LightRadius = (float)atan(orb->Object->Get_Bounding_Sphere().Radius / orb->Distance);
		}
	}
	if (orb->Object == NULL) {
		delete orb;
		orb = NULL;
	}
	if (ini.Get_Bool(section, "FORCE_VISIBLE", false)) {
		orb->Object->Set_Force_Visible(true);
	}
	Unknown_496AE0(orb->Object, true);
	Set_MeshModel_Flag(orb->Object, 0x200, 0);
	Set_MeshModel_Flag(orb->Object, 0x800, 0);
	return(orb);
}


// NOT EXACT: the rotations' trig order and the temporaries' stack slots differ (similarity 0.41)
void OrbitalObject::Update(CameraClass *camera, float time)
{
	WWPROFILE("OrbitalObject::Update");
	float orbit = (time * OrbitsPerHour * 2.0f + StartAngle) * WWMATH_PI;
	Matrix3 rot(true);
	rot.Rotate_Y(RZ * WWMATH_PI);
	rot.Rotate_Z(RX * WWMATH_PI);
	rot.Rotate_Y(-orbit);
	Matrix3::Rotate_Vector(rot, Vector3(0, 0, -1), &Direction);
	Vector3 pos = camera->Get_Transform().Get_Translation() + Distance * Direction;
	Vector3 x = -Direction;
	Vector3 z = rot.Get_X_Vector();
	Vector3 y;
	Vector3::Cross_Product(z, x, &y);
	Matrix3D tm;
	tm.Set(x, y, z, pos);
	Object->Set_Transform(tm);
	if (EmitsLight) {
		Angle = (float)(WWMATH_PI / 2 - acos(Direction.Z));
		Time = Angle * (1.0f / WWMATH_PI) + 0.5f;
		LightIntensityR->Evaluate(Time, &LightValue.X);
		LightIntensityG->Evaluate(Time, &LightValue.Y);
		LightIntensityB->Evaluate(Time, &LightValue.Z);
		SkyIntensityR->Evaluate(Time, &SkyValue.X);
		SkyIntensityG->Evaluate(Time, &SkyValue.Y);
		SkyIntensityB->Evaluate(Time, &SkyValue.Z);
		Unknown68 = (Angle + LightRadius) / (LightRadius + LightRadius);
		if (Unknown68 > 1.0f) {
			Unknown68 = 1.0f;
		}
	}
}


OrbitalManager::OrbitalManager(SClient *client, MainViewOrbitalCallbackClass *callback) :
	Client(client),
	ViewInterface(callback)
{
	Unknown_4F3310();
	QMCSamplingClass sampler(2);
	for (int i = 0; i < 16; i++) {
		sampler.Sample(&Unknown150[i].X);
	}
}



















































































void OrbitalManager::Unknown_4F3310()
{
	G_ASSERT(ViewInterface);
	SimpleSceneClass *scene = ViewInterface->Unknown_00();
	for (std::vector<OrbitalObject *>::iterator it = Orbitals.begin(); it != Orbitals.end(); it++) {
		OrbitalObject *orb = *it;
		if (scene != NULL) {
			scene->Remove_Render_Object(orb->Object);
		}
		delete orb;
	}
	Orbitals.clear();
	Unknown14.Set(0, 0, 0);
	Unknown20.Set(0, 0, 1);
	Unknown2C = 0;
	Unknown30 = 0;
	Unknown34.Set(0, 0, 0);
	Unknown40.Set(0, 0, 0);
	Unknown4C = false;
	SkyColor[0] = NULL;
	SkyColor[1] = NULL;
	SkyColor[2] = NULL;
	LightColor[0] = NULL;
	LightColor[1] = NULL;
	LightColor[2] = NULL;
	SkyEmissive[0] = NULL;
	SkyEmissive[1] = NULL;
	SkyEmissive[2] = NULL;
	for (int i = 0; i < 5; i++) {
		Unknown88[i].Set(0, 0, 0, 0);
		UnknownD8[i].Set(0, 0, 0);
		Unknown114[i].Set(0.5f, 0.5f, 0);
	}
	Unknown50 = 1.0f;
	Unknown54 = 1.0f;
	Unknown58.Set(0, 0, 0);
}





void OrbitalManager::Unknown_4F34C0(INIClass &ini)
{
	G_ASSERT(ViewInterface);
	SimpleSceneClass *OrbitalScene = ViewInterface->Unknown_00();
	G_ASSERT(OrbitalScene);
	SplineDatabaseClass *splines = Client->Splines;
	char name[100];
	ini.Get_String("ORBITALS", "LIGHTCOLOR", "LightColor.Earth", name, sizeof(name));
	splines->Get_Color_Curves(name, &LightColor[0], &LightColor[1], &LightColor[2]);
	ini.Get_String("ORBITALS", "SKYCOLOR", "SkyColor.Earth", name, sizeof(name));
	splines->Get_Color_Curves(name, &SkyColor[0], &SkyColor[1], &SkyColor[2]);
	ini.Get_String("ORBITALS", "SKYEMISSIVE", "SkyEmissive.Default", name, sizeof(name));
	splines->Get_Color_Curves(name, &SkyEmissive[0], &SkyEmissive[1], &SkyEmissive[2]);
	Unknown_4F37C0(&ini);
	int count = ini.Get_Int("ORBITALS", "NUM_ORBITALS", 0);
	bool first = true;
	for (int i = 0; i < count; i++) {
		sprintf(name, "ORBITAL%3.3d", i);
		OrbitalObject *orb = OrbitalObject::Create(ini, name, Client);
		if (orb != NULL) {
			Orbitals.push_back(orb);
			OrbitalScene->Add_Render_Object(orb->Object);
			float radius = orb->Object->Get_Bounding_Sphere().Radius;
			if (first) {
				first = false;
				Unknown50 = orb->Distance - radius;
				Unknown54 = radius + orb->Distance;
			} else {
				Unknown50 = WWMath::Min(Unknown50, orb->Distance - radius);
				Unknown54 = WWMath::Max(Unknown54, radius + orb->Distance);
			}
		}
	}
	if (Unknown50 < 1.0f) {
		Unknown50 = 1.0f;
	}
	if (Unknown54 < Unknown50) {
		Unknown54 = Unknown50 + 1.0f;
	}
	SkyEmissive[0]->Evaluate(0, &Unknown58.X);
	SkyEmissive[1]->Evaluate(0, &Unknown58.Y);
	SkyEmissive[2]->Evaluate(0, &Unknown58.Z);
}











bool OrbitalManager::Unknown_4F37C0(INIClass *ini)
{
	G_ASSERT(ViewInterface);
	char filename[64];
	_snprintf(filename, sizeof(filename), "light_sector%04d.ini", ViewInterface->Unknown_04());
	INIClass light_ini(filename);
	if (light_ini.Is_Loaded()) {
		Unknown_4F38E0(light_ini);
		return(true);
	}
	if (ini != NULL) {
		Unknown_4F38E0(*ini);
		return(true);
	}
	return(false);
}


void OrbitalManager::Unknown_4F38E0(INIClass &ini)
{
	Unknown88[0].X = ini.Get_Float("LIGHTS", "KEYR", 0);
	Unknown88[0].Y = ini.Get_Float("LIGHTS", "KEYG", 0);
	Unknown88[0].Z = ini.Get_Float("LIGHTS", "KEYB", 0);
	Unknown88[0].W = ini.Get_Float("LIGHTS", "KEYI", 0);
	UnknownD8[0].X = ini.Get_Float("LIGHTS", "KEYX", 0);
	UnknownD8[0].Y = ini.Get_Float("LIGHTS", "KEYY", 0);
	UnknownD8[0].Z = ini.Get_Float("LIGHTS", "KEYZ", 0);
	Unknown88[1].X = ini.Get_Float("LIGHTS", "RKEYR", 0);
	Unknown88[1].Y = ini.Get_Float("LIGHTS", "RKEYG", 0);
	Unknown88[1].Z = ini.Get_Float("LIGHTS", "RKEYB", 0);
	Unknown88[1].W = ini.Get_Float("LIGHTS", "RKEYI", 0);
	UnknownD8[1].X = ini.Get_Float("LIGHTS", "RKEYX", 0);
	UnknownD8[1].Y = ini.Get_Float("LIGHTS", "RKEYY", 0);
	UnknownD8[1].Z = ini.Get_Float("LIGHTS", "RKEYZ", 0);
	Unknown88[2].X = ini.Get_Float("LIGHTS", "CAMERAR", 0);
	Unknown88[2].Y = ini.Get_Float("LIGHTS", "CAMERAG", 0);
	Unknown88[2].Z = ini.Get_Float("LIGHTS", "CAMERAB", 0);
	Unknown88[2].W = ini.Get_Float("LIGHTS", "CAMERAI", 0);
	UnknownD8[2].X = ini.Get_Float("LIGHTS", "CAMERAX", 0);
	UnknownD8[2].Y = ini.Get_Float("LIGHTS", "CAMERAY", 0);
	UnknownD8[2].Z = ini.Get_Float("LIGHTS", "CAMERAZ", 0);
	Unknown88[3].X = ini.Get_Float("LIGHTS", "RCAMERAR", 0);
	Unknown88[3].Y = ini.Get_Float("LIGHTS", "RCAMERAG", 0);
	Unknown88[3].Z = ini.Get_Float("LIGHTS", "RCAMERAB", 0);
	Unknown88[3].W = ini.Get_Float("LIGHTS", "RCAMERAI", 0);
	UnknownD8[3].X = ini.Get_Float("LIGHTS", "RCAMERAX", 0);
	UnknownD8[3].Y = ini.Get_Float("LIGHTS", "RCAMERAY", 0);
	UnknownD8[3].Z = ini.Get_Float("LIGHTS", "RCAMERAZ", 0);
	Unknown88[4].X = ini.Get_Float("LIGHTS", "AMBIENTR", 0);
	Unknown88[4].Y = ini.Get_Float("LIGHTS", "AMBIENTG", 0);
	Unknown88[4].Z = ini.Get_Float("LIGHTS", "AMBIENTB", 0);
	Unknown88[4].W = ini.Get_Float("LIGHTS", "AMBIENTI", 0);
	Unknown114[0].X = ini.Get_Float("PICKER", "PKX", 0.5f);
	Unknown114[0].Y = ini.Get_Float("PICKER", "PKY", 0.5f);
	Unknown114[0].Z = ini.Get_Float("PICKER", "PKZ", 0);
	Unknown114[1].X = ini.Get_Float("PICKER", "PRKX", 0.5f);
	Unknown114[1].Y = ini.Get_Float("PICKER", "PRKY", 0.5f);
	Unknown114[1].Z = ini.Get_Float("PICKER", "PRKZ", 0);
	Unknown114[2].X = ini.Get_Float("PICKER", "PCX", 0.5f);
	Unknown114[2].Y = ini.Get_Float("PICKER", "PCY", 0.5f);
	Unknown114[2].Z = ini.Get_Float("PICKER", "PCZ", 0);
	Unknown114[3].X = ini.Get_Float("PICKER", "PRCX", 0.5f);
	Unknown114[3].Y = ini.Get_Float("PICKER", "PRCY", 0.5f);
	Unknown114[3].Z = ini.Get_Float("PICKER", "PRCZ", 0);
	Unknown114[4].X = ini.Get_Float("PICKER", "PAX", 0.5f);
	Unknown114[4].Y = ini.Get_Float("PICKER", "PAY", 0.5f);
	Unknown114[4].Z = ini.Get_Float("PICKER", "PAZ", 0);
}


void OrbitalManager::Unknown_4F3EF0()
{
	INIClass ini;
	ini.Put_Float("LIGHTS", "KEYR", Unknown88[0].X);
	ini.Put_Float("LIGHTS", "KEYG", Unknown88[0].Y);
	ini.Put_Float("LIGHTS", "KEYB", Unknown88[0].Z);
	ini.Put_Float("LIGHTS", "KEYI", Unknown88[0].W);
	ini.Put_Float("LIGHTS", "KEYX", UnknownD8[0].X);
	ini.Put_Float("LIGHTS", "KEYY", UnknownD8[0].Y);
	ini.Put_Float("LIGHTS", "KEYZ", UnknownD8[0].Z);
	ini.Put_Float("LIGHTS", "RKEYR", Unknown88[1].X);
	ini.Put_Float("LIGHTS", "RKEYG", Unknown88[1].Y);
	ini.Put_Float("LIGHTS", "RKEYB", Unknown88[1].Z);
	ini.Put_Float("LIGHTS", "RKEYI", Unknown88[1].W);
	ini.Put_Float("LIGHTS", "RKEYX", UnknownD8[1].X);
	ini.Put_Float("LIGHTS", "RKEYY", UnknownD8[1].Y);
	ini.Put_Float("LIGHTS", "RKEYZ", UnknownD8[1].Z);
	ini.Put_Float("LIGHTS", "CAMERAR", Unknown88[2].X);
	ini.Put_Float("LIGHTS", "CAMERAG", Unknown88[2].Y);
	ini.Put_Float("LIGHTS", "CAMERAB", Unknown88[2].Z);
	ini.Put_Float("LIGHTS", "CAMERAI", Unknown88[2].W);
	ini.Put_Float("LIGHTS", "CAMERAX", UnknownD8[2].X);
	ini.Put_Float("LIGHTS", "CAMERAY", UnknownD8[2].Y);
	ini.Put_Float("LIGHTS", "CAMERAZ", UnknownD8[2].Z);
	ini.Put_Float("LIGHTS", "RCAMERAR", Unknown88[3].X);
	ini.Put_Float("LIGHTS", "RCAMERAG", Unknown88[3].Y);
	ini.Put_Float("LIGHTS", "RCAMERAB", Unknown88[3].Z);
	ini.Put_Float("LIGHTS", "RCAMERAI", Unknown88[3].W);
	ini.Put_Float("LIGHTS", "RCAMERAX", UnknownD8[3].X);
	ini.Put_Float("LIGHTS", "RCAMERAY", UnknownD8[3].Y);
	ini.Put_Float("LIGHTS", "RCAMERAZ", UnknownD8[3].Z);
	ini.Put_Float("LIGHTS", "AMBIENTR", Unknown88[4].X);
	ini.Put_Float("LIGHTS", "AMBIENTG", Unknown88[4].Y);
	ini.Put_Float("LIGHTS", "AMBIENTB", Unknown88[4].Z);
	ini.Put_Float("LIGHTS", "AMBIENTI", Unknown88[4].W);
	ini.Put_Float("PICKER", "PKX", Unknown114[0].X);
	ini.Put_Float("PICKER", "PKY", Unknown114[0].Y);
	ini.Put_Float("PICKER", "PKZ", Unknown114[0].Z);
	ini.Put_Float("PICKER", "PRKX", Unknown114[1].X);
	ini.Put_Float("PICKER", "PRKY", Unknown114[1].Y);
	ini.Put_Float("PICKER", "PRKZ", Unknown114[1].Z);
	ini.Put_Float("PICKER", "PCX", Unknown114[2].X);
	ini.Put_Float("PICKER", "PCY", Unknown114[2].Y);
	ini.Put_Float("PICKER", "PCZ", Unknown114[2].Z);
	ini.Put_Float("PICKER", "PRCX", Unknown114[3].X);
	ini.Put_Float("PICKER", "PRCY", Unknown114[3].Y);
	ini.Put_Float("PICKER", "PRCZ", Unknown114[3].Z);
	ini.Put_Float("PICKER", "PAX", Unknown114[4].X);
	ini.Put_Float("PICKER", "PAY", Unknown114[4].Y);
	ini.Put_Float("PICKER", "PAZ", Unknown114[4].Z);
	char filename[64];
	_snprintf(filename, sizeof(filename), "light_sector%04d.ini", ViewInterface->Unknown_04());
	ini.Save(filename);
}


// NOT EXACT: from the pseudocode (similarity 0.54)
void OrbitalManager::Update(CameraClass *camera, float time, bool planet)
{
	WWPROFILE("OrbitalManager::Update");
	std::vector<OrbitalObject *>::iterator it;
	for (it = Orbitals.begin(); it != Orbitals.end(); it++) {
		(*it)->Update(camera, time);
	}
	Unknown14.Set(0, 0, 0);
	Unknown20.Set(0, 0, 1);
	Unknown2C = 0;
	Unknown30 = 0;
	Unknown34.Set(0, 0, 0);
	Unknown40.Set(0, 0, 0);
	Unknown4C = false;
	float brightest = -1.0f;
	for (it = Orbitals.begin(); it != Orbitals.end(); it++) {
		OrbitalObject *orb = *it;
		if (orb->EmitsLight) {
			if (!planet || orb->Unknown68 >= 0.0f) {
				const SphereClass &sphere = orb->Object->Get_Bounding_Sphere();
				Vector3 center = sphere.Center;
				float radius = sphere.Radius * 0.6f;
				int blocked = 0;
				for (int i = 0; i < 16; i++) {
					float angle = Unknown150[i].X * (WWMATH_PI * 2.0f);
					CastResultStruct result;
					float r = WWMath::Sqrt((1.0f - Unknown150[i].Y) * Unknown150[i].Y);
					float d = r * radius * 2.0f;
					float s = sinf(angle);
					float c = cosf(angle);
					Vector3 point = center + Vector3(c * d, s * d, (1.0f - 2.0f * Unknown150[i].Y) * radius);
					LineSegClass ray(camera->Get_Position(), point);
					for (std::vector<OrbitalObject *>::iterator other = Orbitals.begin(); other != Orbitals.end(); other++) {
						if (orb != *other) {
							if (CollisionMath::Collide(ray, (*other)->Object->Get_Bounding_Sphere(), &result)) {
								blocked++;
								break;
							}
						}
					}
				}
				orb->Unknown6C = (float)(16 - blocked) * 0.0625f;
			} else {
				orb->Unknown6C = 0;
			}
		}
	}
	OrbitalObject *sun = NULL;
	for (it = Orbitals.begin(); it != Orbitals.end(); it++) {
		OrbitalObject *orb = *it;
		if (orb->EmitsLight) {
			if (!planet) {
				orb->Unknown68 = 1.0f;
			}
			if (orb->Unknown68 > 0.0f) {
				bool is_sun = false;
				float bright = orb->LightValue.Length2() * orb->Unknown68 * orb->Unknown6C;
				if (Unknown4C) {
					if (brightest < bright) {
						brightest = bright;
						is_sun = true;
					}
				} else {
					brightest = bright;
					Unknown4C = true;
					is_sun = true;
				}
				Vector3 sky;
				SkyColor[0]->Evaluate(orb->Time, &sky.X);
				SkyColor[1]->Evaluate(orb->Time, &sky.Y);
				SkyColor[2]->Evaluate(orb->Time, &sky.Z);
				sky.X *= orb->SkyValue.X;
				sky.Y *= orb->SkyValue.Y;
				sky.Z *= orb->SkyValue.Z;
				Unknown34 += orb->Unknown68 * sky * orb->Unknown6C;
				Vector3 light;
				LightColor[0]->Evaluate(orb->Time, &light.X);
				LightColor[1]->Evaluate(orb->Time, &light.Y);
				LightColor[2]->Evaluate(orb->Time, &light.Z);
				light.X *= orb->LightValue.X;
				light.Y *= orb->LightValue.Y;
				light.Z *= orb->LightValue.Z;
				Unknown40 += orb->Unknown68 * light * orb->Unknown6C;
				if (is_sun) {
					Unknown14 = light;
					Unknown20 = orb->Direction;
					Unknown2C = orb->Angle;
					Unknown30 = orb->LightRadius;
					sun = orb;
				}
			}
		}
	}
	Unknown14.X = WWMath::Clamp(Unknown14.X, 0.0f, 1.0f);
	Unknown14.Y = WWMath::Clamp(Unknown14.Y, 0.0f, 1.0f);
	Unknown14.Z = WWMath::Clamp(Unknown14.Z, 0.0f, 1.0f);
	Unknown34.X = WWMath::Clamp(Unknown34.X, 0.0f, 1.0f);
	Unknown34.Y = WWMath::Clamp(Unknown34.Y, 0.0f, 1.0f);
	Unknown34.Z = WWMath::Clamp(Unknown34.Z, 0.0f, 1.0f);
	Unknown40.X = WWMath::Clamp(Unknown40.X, 0.0f, 1.0f);
	Unknown40.Y = WWMath::Clamp(Unknown40.Y, 0.0f, 1.0f);
	Unknown40.Z = WWMath::Clamp(Unknown40.Z, 0.0f, 1.0f);
	for (it = Orbitals.begin(); it != Orbitals.end(); it++) {
		OrbitalObject *orb = *it;
		if (orb->PhasePerHour == 0.0f && !planet) {
			if (orb != sun) {
				Vector3 dir = sun->Object->Get_Position() - orb->Object->Get_Position();
				dir.Normalize();
				Unknown_75B8F0(orb->Object, dir, camera->Get_Position());
			}
		} else {
			float phase = time * orb->PhasePerHour;
			Unknown_75B7B0(orb->Object, phase);
		}
	}
}






























































































OrbitalManager::~OrbitalManager()
{
	Unknown_4F3310();
	G_ASSERT(ViewInterface);
	delete ViewInterface;
	ViewInterface = NULL;
}


float OrbitalManager::Unknown_4F5090(RenderObjClass *obj)
{
	for (std::vector<OrbitalObject *>::iterator it = Orbitals.begin(); it != Orbitals.end(); it++) {
		OrbitalObject *orb = *it;
		if (orb->Object == obj) {
			return(orb->Unknown6C);
		}
	}
	return(1.0f);
}


const Vector4 &OrbitalManager::Unknown_4F50D0(int index)
{
	return(Unknown88[index]);
}


void OrbitalManager::Unknown_4F5100(int index, const Vector4 &value)
{
	Unknown88[index] = value;
}


const Vector3 &OrbitalManager::Unknown_4F5140(int index)
{
	return(UnknownD8[index]);
}


void OrbitalManager::Unknown_4F5160(int index, const Vector3 &value)
{
	UnknownD8[index] = value;
}


const Vector3 &OrbitalManager::Unknown_4F51A0(int index)
{
	return(Unknown114[index]);
}


void OrbitalManager::Unknown_4F51C0(int index, const Vector3 &value)
{
	Unknown114[index] = value;
}

/*
**	Reconstruction notes (WE4, 2026-11-07)
**	The client's window 0x4F28A0-0x4F51C0 (__FILE__ 0xB759A0 'e:\build\beta\G\app\shared\orbitalmanager.cpp';
**	asserts 'orb->Object' l.131, 'ViewInterface' l.294 / 336 / 390 / 728, 'OrbitalScene' l.338), then its
**	vector<OrbitalObject *> instantiations 0x4F5200 (destructor), 0x4F5240 (_Destroy), 0x4F5260 (insert),
**	0x4F5500 (copy). Profile labels 'OrbitalObject::Update', 'OrbitalManager::Update' (WWPROFILE, as cordmgr.cpp).
**	The INI keys of orbital%04d.ini's [ORBITALS] / [ORBITAL%3.3d] and light_sector%04d.ini's [LIGHTS] / [PICKER]
**	name the members. 0x4F3EF0 writes [LIGHTS] / [PICKER] back to light_sector%04d.ini (the light editor 0x586800).
*/
