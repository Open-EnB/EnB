// Earth & Beyond client (reconstruction): app\Client\co_player_relationship.cpp. Notes at the end of the file.
#include "sclient.h"
#include "co.h"
#include "..\packets\pk_player_object_relationship.h"
#include "..\shared\unknown_debug.h"
#include "..\shared\auxdata\auxdata.h"
#include "aux_roots.h"
#include "..\..\wwlib\inisup.h"
#include "..\..\wwlib\ini.h"
#include <string>
#include <hash_map.h>
void Unknown_70CC40(SClient *sclient, PlayerObjectRelationshipPacket *packet);	// NAME 1 [Unknown_70CC40, sclient, packet]: cpacket.cpp's RELATIONSHIP 137 handler
void Unknown_70CC90(SClient *sclient, PlayerObjectReactionUpdatePacket *packet);	// NAME 1 [Unknown_70CC90, sclient, packet]: cpacket.cpp's 138 handler
void Unknown_70CD50(SClient *sclient, PlayerObjectIsAttackingUpdatePacket *packet);	// NAME 1 [Unknown_70CD50, sclient, packet]: cpacket.cpp's ATTACKER_UPDATES 139 handler
void Unknown_70A7E0(const std::string &ini);	// NAME 1 [Unknown_70A7E0]: sclient.cpp's name ('gutter_color.ini')
void Unknown_4BC260(const char *sender, const char *category, const char *file, int line, const char *format, ...);	// NAME 1 [Unknown_4BC260]: 0x4BC260, E&B's message reporter (buff_database.cpp)
bool String_To_Value(const char *text, Vector3 *value);	// 0x4A3810 (strconv_lib.cpp, not in strconv_lib.h: sscanf "%f %f %f")  // NAME 2: as strconv_lib.h's pairs

// One gutter colour: the ini's <name>1 and <name>2 entries (0.66 grey when missing; 0x18 bytes)
struct GutterColorStruct	// NAME 2: what it holds ('Failed to lookup gutter color %s')
{
	GutterColorStruct() {}
	GutterColorStruct(const Vector3 &color_1, const Vector3 &color_2) : Color1(color_1), Color2(color_2) {}	// 0x70CE10 (out of line in 0x70A7E0)

	Vector3 Color1;	// +0x00  // NAME 2: the assert's color_1
	Vector3 Color2;	// +0x0C  // NAME 2: the assert's color_2
};

// The colours of one gutter ini section (0x60 bytes; copied whole by 0x70ACA0)
struct GutterColorSetStruct	// NAME 2: what it holds
{
	GutterColorSetStruct() : Target(Vector3(0.66f, 0.66f, 0.66f), Vector3(0.66f, 0.66f, 0.66f)), Mouse(Vector3(0.66f, 0.66f, 0.66f), Vector3(0.66f, 0.66f, 0.66f)),
		NotTarget(Vector3(0.66f, 0.66f, 0.66f), Vector3(0.66f, 0.66f, 0.66f)) {}	// 0x70A790 (JumpStart left as it is)
	GutterColorSetStruct(INISection *section);	// 0x70A010
	void Read_Color(INISection *section, const std::string &name, GutterColorStruct &color);	// 0x70A2E0 (ecx unused)  // NAME 2: what it does

	GutterColorStruct Target;	// +0x00  // NAME 3: the ini name 'Target'
	GutterColorStruct Mouse;	// +0x18  // NAME 3: the ini name 'Mouse'
	GutterColorStruct NotTarget;	// +0x30  // NAME 3: the ini name 'NotTarget'
	GutterColorStruct JumpStart;	// +0x48  // NAME 3: the ini name 'JumpStart'
};

std::hash_map<std::string, GutterColorSetStruct> GutterColors;	// 0xBE70E8 (constructor 0x70CE50, destructor 0x709F40) by ini section  // NAME 2: what it holds

GutterColorSetStruct::GutterColorSetStruct(INISection *section)
{
	Read_Color(section, std::string("Target"), Target);
	Read_Color(section, std::string("Mouse"), Mouse);
	Read_Color(section, std::string("NotTarget"), NotTarget);
	Read_Color(section, std::string("JumpStart"), JumpStart);
}


void GutterColorSetStruct::Read_Color(INISection *section, const std::string &name, GutterColorStruct &color)
{
	std::string color_1_name = name + "1";
	std::string color_2_name = name + "2";
	INIEntry *color_1_entry = section->Find_Entry(color_1_name.c_str());
	INIEntry *color_2_entry = section->Find_Entry(color_2_name.c_str());
	if (color_2_entry == NULL) {
		color_2_entry = color_1_entry;
	}
	FAIL_IF(!color_1_entry) {
		color.Color1.Set(0.66f, 0.66f, 0.66f);
		color.Color2.Set(0.66f, 0.66f, 0.66f);
		return;
	}

	Vector3 color_1;
	Vector3 color_2;
	bool color_1_result = String_To_Value(color_1_entry->Value, &color_1);
	bool color_2_result = String_To_Value(color_2_entry->Value, &color_2);

	G_ASSERT(color_1_result && color_2_result);
	color = GutterColorStruct(color_1, color_2);
}

GutterColorSetStruct Unknown_70A540()	// NAME 1 [Unknown_70A540]: the default colours by value
{
	static GutterColorSetStruct defaults;	// 0xBE6D10 (guard 0xBE7100)  // NAME 2: what it holds
	return(defaults);
}









void Unknown_70A7E0(const std::string &filename)
{
	INIClass ini;

	FAIL_IF(!ini.Load(filename.c_str())) {
		Unknown_4BC260("", "Relationship", __FILE__, __LINE__, "Failed to load gutter color ini (%s)", filename.c_str());
		return;
	}

	for (INISection *section = ini.Get_Section_List().First_Valid(); section != 0; section = section->Next_Valid()) {
		GutterColorSetStruct colors(section);
		std::string name = section->Section;
		GutterColors[name] = colors;
	}
}


void Unknown_70AD70()	// NAME 1 [Unknown_70AD70]: sclient.cpp's name (it empties the table)
{
	GutterColors.clear();
}
GutterColorSetStruct Unknown_70ADF0(const std::string &name)	// NAME 1 [Unknown_70ADF0]: the colours of an ini section (no direct caller; inlined by the relationship lookups)
{
	std::hash_map<std::string, GutterColorSetStruct>::iterator it = GutterColors.find(name);
	if (it == GutterColors.end()) {
		Unknown_4BC260("", "Relationship", __FILE__, __LINE__, "Failed to lookup gutter color %s", name.c_str());
		return(Unknown_70A540());
	} return((*it).second);
}


bool Unknown_70B450(SClient *client, ClientObjectClass *object, GutterColorSetStruct &colors);
bool Unknown_70B790(SClient *client, ClientObjectClass *object, GutterColorSetStruct &colors);
bool Unknown_70BC30(SClient *client, ClientObjectClass *object, GutterColorSetStruct &colors);	// 0x70BC30 'Group'  // NAME 1 [Unknown_70BC30]: neutral
bool Unknown_70BF90(SClient *client, ClientObjectClass *object, GutterColorSetStruct &colors);
bool Unknown_70C2A0(SClient *client, ClientObjectClass *object, GutterColorSetStruct &colors);
void Unknown_70C5D0(SClient *client, ClientObjectClass *object, GutterColorSetStruct &colors);	// 0x70C5D0 by reaction: 'KOS', 'Shun', 'Friendly', 'Adore' (not rebuilt)  // NAME 1 [Unknown_70C5D0]: neutral

GutterColorSetStruct Unknown_70B0A0(SClient *client, ClientObjectClass *object)	// NAME 1 [Unknown_70B0A0]: the gutter colours of an object (radar_data.cpp's callers)
{
	GutterColorSetStruct colors;
	if (Unknown_70B450(client, object, colors)) return(colors);
	if (Unknown_70B790(client, object, colors)) return(colors);
	if (Unknown_70BC30(client, object, colors)) return(colors);
	if (Unknown_70BF90(client, object, colors)) return(colors);
	if (Unknown_70C2A0(client, object, colors)) return(colors);
	Unknown_70C5D0(client, object, colors);
	return(colors);
}








// NOT EXACT: inline budget (the client calls the name string's assign and the copies out of line; attempts.py show 0x70B450)
bool Unknown_70B450(SClient *client, ClientObjectClass *object, GutterColorSetStruct &colors)	// NAME 1 [Unknown_70B450]: 'Neutral' unless the object's type is one of seven
{
	switch (object->Type) {
		case 0: case 1: case 2: case 11: case 12: case 28: case 33:
			return(false);
	}
	static GutterColorSetStruct neutral = Unknown_70ADF0(std::string("Neutral"));	// 0xBE6E30 (guard 0xBE7101)  // NAME 2: what it holds
	colors = neutral;
	return(true);
}

bool Unknown_70B790(SClient *client, ClientObjectClass *object, GutterColorSetStruct &colors)	// NAME 1 [Unknown_70B790]: 'Attacking' while the relationship says so
{
	if (!((RelationshipClass *)object->Relationship)->IsAttacking) return(false);
	static GutterColorSetStruct attacking = Unknown_70ADF0(std::string("Attacking"));	// NAME 2: what it holds
	colors = attacking;
	return(true);
}





























// A field under root that is set (+0x70), else NULL: a wrapper over auxdata.h's lookup; the four dead size() reads spend the inline budget so rtti.h's cast stays out of line (0x598D80), as the client's (three are too few, six push the lookup out too)
template<class C> inline C *Unknown_Find_Set_Field(DeltaCompoundClass *root, const NameClass &name)	// NAME 1: neutral (D10)
{
	root->Children.size(); root->Children.size(); root->Children.size(); root->Children.size(); C *t = Unknown_Find_Field<C>(root, name);
	return((t && t->m_Unknown70) ? t : NULL);
}
bool Unknown_70BAC0(SClient *client, ClientObjectClass *object)	// NAME 1 [Unknown_70BAC0]: the object is a player in the client's group (GroupInfo.Members' game ids)
{
	if (object->Type == 1) {
		DeltaCompoundClass *aux_data = client->AuxData;
		FAIL_IF(!aux_data) return(false);
		DeltaGroupInfoClass *group_data = (DeltaGroupInfoClass *)Unknown_Find_Set_Field<DeltaCompoundClass>(aux_data, "GroupInfo");

		FAIL_IF(!group_data) return(false);
		int count = group_data->Members.Children.size();
		for (int i = 0; i < count; i++) {
			if (((DeltaGroupMemberClass *)group_data->Members.Children[i])->GameID.Value == object->GameID) return(true);
		}
	}
	return(false);
}
bool Unknown_70BC30(SClient *client, ClientObjectClass *object, GutterColorSetStruct &colors)	// NAME 1 [Unknown_70BC30]: 'Group' for a member of the client's group
{
	if (Unknown_70BAC0(client, object)) {
		static GutterColorSetStruct group = Unknown_70ADF0(std::string("Group"));	// NAME 2: what it holds
		colors = group;
		return(true);
	} return(false);
}
bool Unknown_70BF60(SClient *client, ClientObjectClass *object)	// NAME 1 [Unknown_70BF60]: type 1 and SClient's PvP test 0x74D780 (no direct caller; inlined by 0x70BF90)
{
	if (object->Type != 1) return(false);
	return(client->Unknown_74D780(object));
}
bool Unknown_70BF90(SClient *client, ClientObjectClass *object, GutterColorSetStruct &colors)	// NAME 1 [Unknown_70BF90]: 'PVPAttackable' when 0x70BF60 says so
{
	if (Unknown_70BF60(client, object)) {
		static GutterColorSetStruct pvp_attackable = Unknown_70ADF0(std::string("PVPAttackable"));	// NAME 2: what it holds
		colors = pvp_attackable;
		return(true);
	} return(false);
}

bool Unknown_70C2A0(SClient *client, ClientObjectClass *object, GutterColorSetStruct &colors)	// NAME 1 [Unknown_70C2A0]: 'Player' for type 1
{
	if (object->Type != 1) return(false);
	static GutterColorSetStruct player_colors = Unknown_70ADF0(std::string("Player"));	// NAME 2: what it holds
	colors = player_colors;
	return(true);
}





































void Unknown_70C5D0(SClient *client, ClientObjectClass *object, GutterColorSetStruct &colors)
{
	static GutterColorSetStruct kos = Unknown_70ADF0(std::string("KOS"));	// 0xBE7018  // NAME 2: what it holds
	static GutterColorSetStruct shun = Unknown_70ADF0(std::string("Shun"));	// 0xBE6DD0  // NAME 2: what it holds
	static GutterColorSetStruct friendly = Unknown_70ADF0(std::string("Friendly"));	// 0xBE6D70  // NAME 2: what it holds
	static GutterColorSetStruct adore = Unknown_70ADF0(std::string("Adore"));	// 0xBE6E90  // NAME 2: what it holds
	switch (((RelationshipClass *)object->Relationship)->Reaction) {
		case 0:
			colors = kos;
			break;
		case 2:
			colors = friendly;
			break;
		case 3:
			colors = adore;
			break;
		default:
			G_ASSERT(0);
		case 1:
			colors = shun;
			break;
	}
}
void Unknown_70CC40(SClient *sclient, PlayerObjectRelationshipPacket *packet)
{
	ClientObjectClass *object = sclient->Find_Object(packet->ObjectID);
	FAIL_IF(!object) return;
	object->Unknown_6FEEE0(packet->Relationship);
}

void Unknown_70CC90(SClient *sclient, PlayerObjectReactionUpdatePacket *packet)
{
	ReactionUpdateIteratorClass *it = packet->Get_Iterator();
	while (!it->Is_Done()) {
		ReactionUpdateStruct update = it->Get();
		it->Next();
		ClientObjectClass *object = sclient->Find_Object(update.ObjectID);
		if (object == NULL) return;
		RelationshipClass relationship = *(RelationshipClass *)object->Relationship;
		relationship.Reaction = update.Reaction;
		object->Unknown_6FEEE0(relationship);
	}
	delete it;
}

void Unknown_70CD50(SClient *sclient, PlayerObjectIsAttackingUpdatePacket *packet)
{
	AttackerUpdateIteratorClass *it = packet->Get_Iterator();
	while (!it->Is_Done()) {
		int id = it->Get();
		it->Next();
		ClientObjectClass *object = sclient->Find_Object(id);
		if (object == NULL) return;
		RelationshipClass relationship = *(RelationshipClass *)object->Relationship;
		relationship.IsAttacking = packet->Attacking;
		object->Unknown_6FEEE0(relationship);
	}
	delete it;
}

/*
**	Reconstruction notes
**
**	The object 0x709EA0-0x70DC00 (asserts' __FILE__ 'e:\build\beta\G\app\Client\co_player_relationship.cpp'):
**	the static initialisers 0x709EA0 (permstr.h's PermStringInitClass at 0xBE7078, through auxdata.h's name.h)
**	and 0x709EF0 (GutterColors, constructor 0x70CE50, destructor 0x709F40); gutter_color.ini's colour sets
**	(0x70A010 / 0x70A2E0, the loader 0x70A7E0 sclient.cpp calls, the lookup 0x70ADF0); the dispatcher 0x70B0A0
**	(radar_data.cpp's callers) over the checks Neutral 0x70B450 (NOT EXACT), Attacking 0x70B790, Group 0x70BC30
**	(through 0x70BAC0, the GroupInfo aux-data test with asserts on lines 207 / 210 on SClient's AuxData
**	+0x12C0; rtti.h's DeltaCompoundClass cast 0x598D80 out of line), PVPAttackable 0x70BF90 (through 0x70BF60),
**	Player 0x70C2A0 and the reaction colours 0x70C5D0;
**	the three packet handlers cpacket.cpp calls (0x70CC40, 0x70CC90, 0x70CD50) and the table's out-of-line
**	helpers (0x70CDF0-0x70DBC0). Not counted: GutterColorSetStruct's operator= 0x70CAF0 (the client's window
**	runs on over the four statics' empty atexit destructors 0x70CBC0-0x70CC20), the copies 0x70AC60 / 0x70ACA0
**	(ambiguous), 0x70B390 / 0x70B3D0 (not emitted yet); the static initialisers (`_$E`, D94); GutterColors' destructor
**	0x709F40 and the finds 0x70D030 / 0x70D150 (exact, but 25 / 36 copies D80 can't tell apart); its constructor
**	0x70CE50 (the client inlines vector::size() in the buckets' reserve: inline budget, attempts.py show 0x70CE50).
**	Forms that mattered: GutterColors is a global (a file static's [8] reference was not matched); the set has
**	no destructor (a declared one gave the loader's local an EH state); the lookup tests the miss first with a
**	plain iterator and no dead read; PVPAttackable shares its `return false` tail. The handlers and checks read
**	the object's relationship at +0x12C / +0x130 (co.h's Relationship) through a cast, as co.cpp's 0x6FEEE0 does.
*/
