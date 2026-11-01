// Earth & Beyond client (reconstruction): app\client\galaxy_map_client.cpp. Notes at the end of the file.
#include "galaxy_map_client.h"
#include "sclient.h"
#include "option.h"
#include "ini.h"
#include "..\packets\pkgalaxymap.h"
#include "..\shared\auxdata\linearbu.h"
#include "..\shared\auxdata\bitset.h"
#include "..\shared\unknown_debug.h"
// 0x7A4830
GalaxyMapClientClass::GalaxyMapClientClass() :
	Version(0),
	Unknown7C(0),
	Unknown80(NULL),
	Size(100),
	SystemPosScale(1.0f),
	SectorPosScale(1.0f),
	GalaxyShift(0, 0),
	ShowAll(false)
{
	ShowAll = OptionClass::Instance->Get_Bool("Galaxymap_Show_All");
	Load_Ini();
}
// 0x7A49A0
GalaxyMapClientClass::~GalaxyMapClientClass()
{
	BoolOptionStruct *option = OptionClass::Instance->Find_Bool("Galaxymap_Show_All");
	if (option) option->Set_Value(ShowAll);
}
// 0x7A4B10
void GalaxyMapClientClass::Load_Ini()
{
	INIClass ini("galaxy_map_c.ini");
	Size = ini.Get_Int("GalaxyMap", "Size", Size);
	SystemPosScale = ini.Get_Float("GalaxyMap", "SystemPosScale", SystemPosScale);
	SectorPosScale = ini.Get_Float("GalaxyMap", "SectorPosScale", SectorPosScale);
	GalaxyShift.X = ini.Get_Float("GalaxyMap", "GalaxyShiftX", GalaxyShift.X);
	GalaxyShift.Y = ini.Get_Float("GalaxyMap", "GalaxyShiftY", GalaxyShift.Y);
}










































// 0x7A4C50
void GalaxyMapClientClass::Unknown_7A4C50(SClient *sclient, LinearBufferClass &buffer)
{
	FAIL_IF(!sclient) return;

	int channel = 0;
	buffer.Read(&channel, 4);
	char *string = buffer.Read_String();
	Unknown9C = string;
	delete string;
	string = buffer.Read_String();
	UnknownAC = string;
	delete string;
	string = buffer.Read_String();
	UnknownBC = string;
	delete string;
	Read(&buffer);

	int count = MaxDataObjects;
	BitSetClass bits(count);
	for (int i = 0; i < count; i++) {
		if (Get_Data(i)) {
			bits.Set(i);
		}
	}

	LinearBufferClass request(0x400, 0x400);
	int size = bits.NumBytes;
	request.Write(&size, 4);
	request.Write(bits.Buffer, size);
	sclient->Send_Packet(&GalaxyMapRequestPacketClass(0, request, channel, ShowAll));
	Version = 1;
	Unknown7C++;
}
















// 0x7A4FC0
void GalaxyMapClientClass::Unknown_7A4FC0(LinearBufferClass &buffer)
{
	RefCountPtr<GalaxyMapSystemClass> system = GalaxyMapSystemClass::Create();
	system->Read(&buffer);
	Add_System(system);

	G_ASSERT(Version);
	Version++;
}
// 0x7A50A0
void GalaxyMapClientClass::Unknown_7A50A0(LinearBufferClass &buffer)
{
	RefCountPtr<GalaxyMapSectorClass> sector = GalaxyMapSectorClass::Create();
	sector->Read(&buffer);
	Add_Sector(sector);

	G_ASSERT(Version);
	Version++;
}
// 0x7A5180
void GalaxyMapClientClass::Unknown_7A5180(LinearBufferClass &buffer)
{
	RefCountPtr<GalaxyMapGateClass> gate = GalaxyMapGateClass::Create();
	gate->Read(&buffer);
	Add_Gate(gate);

	G_ASSERT(Version);
	Version++;
}
// 0x7A5260
void GalaxyMapClientClass::Unknown_7A5260(LinearBufferClass &buffer)
{
	unsigned int count = buffer.Get_Size() / sizeof(unsigned int);
	while (count--) {
		unsigned int id;
		buffer.Read(&id, sizeof(id));
		Remove_Data(id);
	}

	G_ASSERT(Version);
	Version++;
}

// 0x7A52F0  NOT EXACT: the client keeps 0 in ebp and loads buffer.Buffer after new (register / order; attempts.py show 0x7A52F0)
void GalaxyMapClientClass::Unknown_7A52F0(LinearBufferClass &buffer)
{
	if (Unknown80 != NULL) {
		delete Unknown80;
		Unknown80 = NULL;
	}
	char *data = buffer.Buffer;
	Unknown80 = new BitSetClass(data, buffer.Get_Size());
	buffer.Skip(buffer.Get_Size());



	G_ASSERT(Version);
	Version++;
}

// 0x7A5400  NOT EXACT: the sector's GalaxyMapID lands in edx, the client's in eax (register; attempts.py show 0x7A5400)
bool GalaxyMapClientClass::Unknown_7A5400(const RefCountPtr<GalaxyMapSectorClass> &sector)
{


	FAIL_IF(!sector) return(false);
	if (Unknown80 == NULL) return(false);
	return(Unknown80->Is_Set(sector.Peek()->GalaxyMapID));
}

// 0x7A5470
Vector3 GalaxyMapClientClass::Unknown_7A5470(const Vector3 &position)
{
	Vector3 result = position;
	result.X *= SystemPosScale; result.Y *= SystemPosScale;
	result.X += GalaxyShift.X; result.Y += GalaxyShift.Y;
	return(result);
}

// 0x7A54F0
Vector3 GalaxyMapClientClass::Unknown_7A54F0(const Vector3 &position)
{
	Vector3 result = position;
	result.X = result.X * SectorPosScale;
	result.Y = result.Y * SectorPosScale;
	return(result);
}

// 0x7A5540
bool GalaxyMapClientClass::Unknown_7A5540(SClient *client)
{
	FAIL_IF(!client) return(ShowAll);
	ShowAll = !ShowAll;
	LinearBufferClass request(0x400, 0x400);
	int channel = client->Unknown1138;
	client->Send_Packet(&GalaxyMapRequestPacketClass(1, request, channel, ShowAll));
	return(ShowAll);
}

/*
**	Reconstruction notes
**
**	client.exe 0x7A4830-0x7A56ED (__FILE__ 0xB9E8A0): the constructor, the deleting destructor
**	0x7A4970, the destructor, Load_Ini, the packet handlers cpacket.cpp's GalaxyMapPacketClass::
**	Process_Packet 0x73AC60 calls (types 2-9, 11), the explored-sector test 0x7A5400, the two
**	position scalers and the PIP's Show All toggle 0x7A5540. The object also holds
**	GalaxyMapRequestPacketClass's implicit destructor 0x7A4FA0 (a jmp to the base's 0x871180) and
**	RefCountPtr<System / Sector / Gate>'s destructors 0x7A5670 / 0x7A56A0 / 0x7A56D0 (the unwind
**	funclets' and galaxy_map.cpp's callers). The asserts sit on the client's lines 85-224.
**	GalaxyShift is a Vector2: with two floats the constructor's stores come out of order. The
**	channel in 0x7A5540 is a local (the inline argument swaps eax / edx). Unknown_7A5260 counts with
**	'while (count--)' over Get_Size() / sizeof (the client's shr and dec / test).
*/
