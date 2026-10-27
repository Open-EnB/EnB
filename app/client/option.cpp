// Earth & Beyond client (reconstruction): app\Client\option.cpp. Notes at the end of the file.
#include <stdio.h>
#include "option.h"
#include "sclient.h"
#include "..\shared\unknown_debug.h"
#include "..\shared\graphics_settings.h"
#include "..\..\wwlib\ini.h"
#include "..\..\wwlib\point.h"
#include "..\..\wwlib\wwstring.h"

void Unknown_4BC000(const char *sender, const char *category, const char *file, int line, const char *format, ...);	// NAME 1 [Unknown_4BC000]: 0x4BC000, E&B's message reporter (tcpconn.cpp)

OptionClass *OptionClass::Instance;

template <class K, class T, class H, class E, class A> T Find_Option(std::hash_map<K, T, H, E, A> &options, const char *name)	// 0x7FCBF0, 0x7FCD10, 0x7FCE30, 0x7FCF50, 0x7FD070  // NAME 2: what it does
{
	std::hash_map<K, T, H, E, A>::iterator it = options.find(name);
	if (it == options.end()) return(NULL);
	return((*it).second);
}

template <class K, class T, class H, class E, class A> void Free_Option_Map(std::hash_map<K, T, H, E, A> &options)	// 0x7FC600, 0x7FC730, 0x7FC860, 0x7FC990, 0x7FCAC0  // NAME 2: what it does
{
	std::hash_map<K, T, H, E, A>::iterator it;
	for (it = options.begin(); it != options.end(); ++it) {
		delete (*it).second;
	}
	options.clear();
}

OptionClass::OptionClass() : CharacterID(0)
{
}

OptionClass::~OptionClass()
{
	Free_Options();
}

void OptionClass::Set_Instance(OptionClass *options)
{
	Instance = options;
}

void OptionClass::Free_Options()
{
	Free_Option_Map(Bools);
	Free_Option_Map(Ints);
	Free_Option_Map(Floats);
	Free_Option_Map(Vector3s);
	Free_Option_Map(Strings);
}

void OptionClass::Set_Defaults(SClient *client)
{
	Free_Options();

	BoolOptionStruct *bool_option;
	FloatOptionStruct *float_option;
	IntOptionStruct *int_option;
	Vector3OptionStruct *vector_option;
	StringOptionStruct *string_option;
	bool_option = new BoolOptionStruct("Chat_Tell", true);
	Bools[bool_option->Get_Name()] = bool_option;
	bool_option = new BoolOptionStruct("Chat_NPC", true);
	Bools[bool_option->Get_Name()] = bool_option;
	bool_option = new BoolOptionStruct("Chat_Combat", true);
	Bools[bool_option->Get_Name()] = bool_option;
	bool_option = new BoolOptionStruct("Chat_Tips", true);
	Bools[bool_option->Get_Name()] = bool_option;
	bool_option = new BoolOptionStruct("Chat_Status_IC", true);
	Bools[bool_option->Get_Name()] = bool_option;
	bool_option = new BoolOptionStruct("Chat_Status_OOC", true);
	Bools[bool_option->Get_Name()] = bool_option;
	bool_option = new BoolOptionStruct("Chat_Warnings", true);
	Bools[bool_option->Get_Name()] = bool_option;
	bool_option = new BoolOptionStruct("Chat_Errors_IC", true);
	Bools[bool_option->Get_Name()] = bool_option;
	bool_option = new BoolOptionStruct("Chat_Errors_OOC", true);
	Bools[bool_option->Get_Name()] = bool_option;
	bool_option = new BoolOptionStruct("Chat_Errors_System", true);
	Bools[bool_option->Get_Name()] = bool_option;
	bool_option = new BoolOptionStruct("Chat_Spam", true);
	Bools[bool_option->Get_Name()] = bool_option;
	bool_option = new BoolOptionStruct("Chat_Attention", true);
	Bools[bool_option->Get_Name()] = bool_option;
	bool_option = new BoolOptionStruct("Channel_Broadcast", true);
	Bools[bool_option->Get_Name()] = bool_option;
	bool_option = new BoolOptionStruct("Channel_Local", true);
	Bools[bool_option->Get_Name()] = bool_option;
	bool_option = new BoolOptionStruct("Channel_Guild", true);
	Bools[bool_option->Get_Name()] = bool_option;
	bool_option = new BoolOptionStruct("Channel_Group", true);
	Bools[bool_option->Get_Name()] = bool_option;
	bool_option = new BoolOptionStruct("Channel_Private", true);
	Bools[bool_option->Get_Name()] = bool_option;
	bool_option = new BoolOptionStruct("Channel_General", false);
	Bools[bool_option->Get_Name()] = bool_option;
	bool_option = new BoolOptionStruct("Channel_OOC", false);
	Bools[bool_option->Get_Name()] = bool_option;
	bool_option = new BoolOptionStruct("Channel_Market", false);
	Bools[bool_option->Get_Name()] = bool_option;
	bool_option = new BoolOptionStruct("Channel_Newbie", true);
	Bools[bool_option->Get_Name()] = bool_option;
	bool_option = new BoolOptionStruct("Channel_Jenquai", false);
	Bools[bool_option->Get_Name()] = bool_option;
	bool_option = new BoolOptionStruct("Channel_Terran", false);
	Bools[bool_option->Get_Name()] = bool_option;
	bool_option = new BoolOptionStruct("Channel_Progen", false);
	Bools[bool_option->Get_Name()] = bool_option;
	bool_option = new BoolOptionStruct("Channel_Explorers", false);
	Bools[bool_option->Get_Name()] = bool_option;
	bool_option = new BoolOptionStruct("Channel_Warriors", false);
	Bools[bool_option->Get_Name()] = bool_option;
	bool_option = new BoolOptionStruct("Channel_Traders", false);
	Bools[bool_option->Get_Name()] = bool_option;
	bool_option = new BoolOptionStruct("Channel_Sentinels", false);
	Bools[bool_option->Get_Name()] = bool_option;
	bool_option = new BoolOptionStruct("Channel_Defenders", false);
	Bools[bool_option->Get_Name()] = bool_option;
	bool_option = new BoolOptionStruct("Channel_Enforcers", false);
	Bools[bool_option->Get_Name()] = bool_option;
	bool_option = new BoolOptionStruct("Camera_Ship", true);
	Bools[bool_option->Get_Name()] = bool_option;
	bool_option = new BoolOptionStruct("Camera_Warp", true);
	Bools[bool_option->Get_Name()] = bool_option;
	bool_option = new BoolOptionStruct("Camera_Loot", true);
	Bools[bool_option->Get_Name()] = bool_option;
	bool_option = new BoolOptionStruct("Camera_Gate", true);
	Bools[bool_option->Get_Name()] = bool_option;
	bool_option = new BoolOptionStruct("Camera_Land", true);
	Bools[bool_option->Get_Name()] = bool_option;
	bool_option = new BoolOptionStruct("Camera_Dock", true);
	Bools[bool_option->Get_Name()] = bool_option;
	bool_option = new BoolOptionStruct("Camera_Bridge", true);
	Bools[bool_option->Get_Name()] = bool_option;
	bool_option = new BoolOptionStruct("Graphics_Default", true);
	Bools[bool_option->Get_Name()] = bool_option;
	bool_option = new BoolOptionStruct("Doppler_Enabled", true);
	Bools[bool_option->Get_Name()] = bool_option;
	bool_option = new BoolOptionStruct("Interface_Attributes", false);
	Bools[bool_option->Get_Name()] = bool_option;
	bool_option = new BoolOptionStruct("Cpit_ThrottleAZLock", false);
	Bools[bool_option->Get_Name()] = bool_option;
	bool_option = new BoolOptionStruct("Cpit_ThrottleArrowsLock", false);
	Bools[bool_option->Get_Name()] = bool_option;
	bool_option = new BoolOptionStruct("Cpit_ThrottleClickLocks", true);
	Bools[bool_option->Get_Name()] = bool_option;
	bool_option = new BoolOptionStruct("Cpit_ThrottleRightMouse", true);
	Bools[bool_option->Get_Name()] = bool_option;
	bool_option = new BoolOptionStruct("Starbase_Default_Run", true);
	Bools[bool_option->Get_Name()] = bool_option;
	bool_option = new BoolOptionStruct("RestrictedStatus", false);
	Bools[bool_option->Get_Name()] = bool_option;
	bool_option = new BoolOptionStruct("Show_Self_Name", false);
	Bools[bool_option->Get_Name()] = bool_option;
	bool_option = new BoolOptionStruct("Trade_Sort_By_Name", false);
	Bools[bool_option->Get_Name()] = bool_option;
	bool_option = new BoolOptionStruct("Bridge_Mode", false);
	Bools[bool_option->Get_Name()] = bool_option;
	bool_option = new BoolOptionStruct("Starbase_Third_Person_Mode", true);
	Bools[bool_option->Get_Name()] = bool_option;
	bool_option = new BoolOptionStruct("FF_DECAL_REQUEST", true);
	Bools[bool_option->Get_Name()] = bool_option;
	bool_option = new BoolOptionStruct("Galaxymap_Show_All", false);
	Bools[bool_option->Get_Name()] = bool_option;
	bool_option = new BoolOptionStruct("Inv_Sort_Cargo_Reverse", false);
	Bools[bool_option->Get_Name()] = bool_option;
	bool_option = new BoolOptionStruct("Inv_Sort_Vault_Reverse", false);
	Bools[bool_option->Get_Name()] = bool_option;
	bool_option = new BoolOptionStruct("Starmap_Maximized", true);
	Bools[bool_option->Get_Name()] = bool_option;

	float_option = new FloatOptionStruct("Graphics_Gamma", Unknown_Percent(1.2f, 1.0f, 3.0f));
	Floats[float_option->Get_Name()] = float_option;
	float_option = new FloatOptionStruct("Graphics_Land_Poly", Unknown_Percent(client->LandPolyBudget, 8000, 40000));
	Floats[float_option->Get_Name()] = float_option;
	float_option = new FloatOptionStruct("Graphics_Land_Detail", Unknown_Percent(client->LandLOD, 0, 8));
	Floats[float_option->Get_Name()] = float_option;
	float_option = new FloatOptionStruct("Graphics_Space_Poly", Unknown_Percent(client->PolyBudget, 8000, 40000));
	Floats[float_option->Get_Name()] = float_option;
	float_option = new FloatOptionStruct("Graphics_Texture_Size", 100.0f - Unknown_Percent(client->TextureReduction, 0, 3));
	Floats[float_option->Get_Name()] = float_option;
	float_option = new FloatOptionStruct("Sound_Music", 30.0f);
	Floats[float_option->Get_Name()] = float_option;
	float_option = new FloatOptionStruct("Sound_SFX", 80.0f);
	Floats[float_option->Get_Name()] = float_option;
	float_option = new FloatOptionStruct("Sound_Dialog", 100.0f);
	Floats[float_option->Get_Name()] = float_option;
	float_option = new FloatOptionStruct("Interface_Tooltip_Delay", 50.0f);
	Floats[float_option->Get_Name()] = float_option;
	float_option = new FloatOptionStruct("Radar_Gutter_Opacity", 0.75f);
	Floats[float_option->Get_Name()] = float_option;
	float_option = new FloatOptionStruct("Scanner_X_Pos", 246.0f);
	Floats[float_option->Get_Name()] = float_option;
	float_option = new FloatOptionStruct("Scanner_Y_Pos", -161.0f);
	Floats[float_option->Get_Name()] = float_option;
	float_option = new FloatOptionStruct("Starmap_MinFrame", 29.0f);
	Floats[float_option->Get_Name()] = float_option;
	float_option = new FloatOptionStruct("Starmap_MaxFrame", 59.0f);
	Floats[float_option->Get_Name()] = float_option;
	float_option = new FloatOptionStruct("Gutter_Icon_Min_Distance", 1000.0f);
	Floats[float_option->Get_Name()] = float_option;
	float_option = new FloatOptionStruct("Gutter_Icon_Max_Distance", 6000.0f);
	Floats[float_option->Get_Name()] = float_option;
	float_option = new FloatOptionStruct("Gutter_Icon_Min_Scale", 0.5f);
	Floats[float_option->Get_Name()] = float_option;

	int_option = new IntOptionStruct("Starmap_Label", 1);
	Ints[int_option->Get_Name()] = int_option;
	int_option = new IntOptionStruct("Starmap_ShowCoords", 0);
	Ints[int_option->Get_Name()] = int_option;
	int_option = new IntOptionStruct("Chat_Default_Channel", 0);
	Ints[int_option->Get_Name()] = int_option;
	int_option = new IntOptionStruct("ChatWinSplit_NoneSize", 73);
	Ints[int_option->Get_Name()] = int_option;
	int_option = new IntOptionStruct("ChatWinSplit_NoneSplit", 0);
	Ints[int_option->Get_Name()] = int_option;
	int_option = new IntOptionStruct("ChatWinSplit_HorzSize", 61);
	Ints[int_option->Get_Name()] = int_option;
	int_option = new IntOptionStruct("ChatWinSplit_HorzSplit", 30);
	Ints[int_option->Get_Name()] = int_option;
	int_option = new IntOptionStruct("ChatWinSplit_VertSize", 73);
	Ints[int_option->Get_Name()] = int_option;
	int_option = new IntOptionStruct("ChatWinSplit_VertSplit", 224);
	Ints[int_option->Get_Name()] = int_option;
	int_option = new IntOptionStruct("DefaultChatWindow", 3);
	Ints[int_option->Get_Name()] = int_option;
	int_option = new IntOptionStruct("Current_Log_Page", 1);
	Ints[int_option->Get_Name()] = int_option;
	int_option = new IntOptionStruct("Inv_Sort0_primary", 5);
	Ints[int_option->Get_Name()] = int_option;
	int_option = new IntOptionStruct("Inv_Sort0_secondary", 4);
	Ints[int_option->Get_Name()] = int_option;
	int_option = new IntOptionStruct("Inv_Sort0_tertiary", 1);
	Ints[int_option->Get_Name()] = int_option;
	int_option = new IntOptionStruct("Inv_Sort1_primary", 1);
	Ints[int_option->Get_Name()] = int_option;
	int_option = new IntOptionStruct("Inv_Sort1_secondary", 4);
	Ints[int_option->Get_Name()] = int_option;
	int_option = new IntOptionStruct("Inv_Sort1_tertiary", 8);
	Ints[int_option->Get_Name()] = int_option;
	int_option = new IntOptionStruct("Inv_Sort2_primary", 10);
	Ints[int_option->Get_Name()] = int_option;
	int_option = new IntOptionStruct("Inv_Sort2_secondary", 1);
	Ints[int_option->Get_Name()] = int_option;
	int_option = new IntOptionStruct("Inv_Sort2_tertiary", 8);
	Ints[int_option->Get_Name()] = int_option;
	int_option = new IntOptionStruct("Inv_Sort_Cargo", 0);
	Ints[int_option->Get_Name()] = int_option;
	int_option = new IntOptionStruct("Inv_Sort_Vault", 0);
	Ints[int_option->Get_Name()] = int_option;

	vector_option = new Vector3OptionStruct("Color_Context", Vector3(0.31f, 1.0f, 0.0f));
	Vector3s[vector_option->Get_Name()] = vector_option;
	vector_option = new Vector3OptionStruct("Color_OOC", Vector3(0.9f, 0.9f, 0.9f));
	Vector3s[vector_option->Get_Name()] = vector_option;
	vector_option = new Vector3OptionStruct("Color_Broadcast", Vector3(1.0f, 0.6f, 0.0f));
	Vector3s[vector_option->Get_Name()] = vector_option;
	vector_option = new Vector3OptionStruct("Color_Tell", Vector3(0.99f, 0.33f, 0.99f));
	Vector3s[vector_option->Get_Name()] = vector_option;
	vector_option = new Vector3OptionStruct("Color_Group", Vector3(0.8f, 0.2f, 0.8f));
	Vector3s[vector_option->Get_Name()] = vector_option;
	vector_option = new Vector3OptionStruct("Color_NPC", Vector3(0.31f, 1.0f, 0.0f));
	Vector3s[vector_option->Get_Name()] = vector_option;
	vector_option = new Vector3OptionStruct("Color_Tip", Vector3(0.31f, 1.0f, 0.0f));
	Vector3s[vector_option->Get_Name()] = vector_option;
	vector_option = new Vector3OptionStruct("Color_Local", Vector3(1.0f, 0.6f, 0.0f));
	Vector3s[vector_option->Get_Name()] = vector_option;
	vector_option = new Vector3OptionStruct("Color_Guild", Vector3(0.7f, 0.7f, 1.0f));
	Vector3s[vector_option->Get_Name()] = vector_option;
	vector_option = new Vector3OptionStruct("Color_General_Channel", Vector3(1.0f, 0.1f, 0.3f));
	Vector3s[vector_option->Get_Name()] = vector_option;
	vector_option = new Vector3OptionStruct("Color_OOC_Channel", Vector3(0.33f, 0.99f, 0.99f));
	Vector3s[vector_option->Get_Name()] = vector_option;
	vector_option = new Vector3OptionStruct("Color_Newbie_Channel", Vector3(0.5f, 0.5f, 1.0f));
	Vector3s[vector_option->Get_Name()] = vector_option;
	vector_option = new Vector3OptionStruct("Color_Market_Channel", Vector3(0.5f, 1.0f, 0.5f));
	Vector3s[vector_option->Get_Name()] = vector_option;
	vector_option = new Vector3OptionStruct("Color_Private_Channel", Vector3(1.0f, 1.0f, 1.0f));
	Vector3s[vector_option->Get_Name()] = vector_option;
	vector_option = new Vector3OptionStruct("Color_Race_Channel", Vector3(0.8f, 0.8f, 0.8f));
	Vector3s[vector_option->Get_Name()] = vector_option;
	vector_option = new Vector3OptionStruct("Color_Class_Channel", Vector3(0.9f, 0.9f, 0.5f));
	Vector3s[vector_option->Get_Name()] = vector_option;
	vector_option = new Vector3OptionStruct("Color_Echo", Vector3(0.9f, 0.9f, 0.9f));
	Vector3s[vector_option->Get_Name()] = vector_option;
	vector_option = new Vector3OptionStruct("Color_Error_Context", Vector3(0.99f, 0.33f, 0.33f));
	Vector3s[vector_option->Get_Name()] = vector_option;
	vector_option = new Vector3OptionStruct("Color_Error_OOC", Vector3(0.9f, 0.9f, 0.9f));
	Vector3s[vector_option->Get_Name()] = vector_option;
	vector_option = new Vector3OptionStruct("Color_Error_System", Vector3(0.9f, 0.9f, 0.9f));
	Vector3s[vector_option->Get_Name()] = vector_option;
	vector_option = new Vector3OptionStruct("Color_Combat", Vector3(0.9f, 0.9f, 0.9f));
	Vector3s[vector_option->Get_Name()] = vector_option;
	vector_option = new Vector3OptionStruct("Color_Warning", Vector3(0.99f, 0.33f, 0.33f));
	Vector3s[vector_option->Get_Name()] = vector_option;
	vector_option = new Vector3OptionStruct("Color_Attention", Vector3(0.9f, 0.9f, 0.9f));
	Vector3s[vector_option->Get_Name()] = vector_option;
	vector_option = new Vector3OptionStruct("Color_Debug", Vector3(0.33f, 0.99f, 0.99f));
	Vector3s[vector_option->Get_Name()] = vector_option;

	string_option = new StringOptionStruct("Channel_Label", "Broadcast");
	Strings[string_option->Get_Name()] = string_option;
	string_option = new StringOptionStruct("FONT_CHAT_LINE_00", "");
	Strings[string_option->Get_Name()] = string_option;
	string_option = new StringOptionStruct("FONT_CHAT_LINE_01", "");
	Strings[string_option->Get_Name()] = string_option;
	string_option = new StringOptionStruct("Inv_Sort0_Name", "category");
	Strings[string_option->Get_Name()] = string_option;
	string_option = new StringOptionStruct("Inv_Sort1_Name", "name");
	Strings[string_option->Get_Name()] = string_option;
	string_option = new StringOptionStruct("Inv_Sort2_Name", "value");
	Strings[string_option->Get_Name()] = string_option;
	string_option = new StringOptionStruct("Inv_Filter_Cargo", "All");
	Strings[string_option->Get_Name()] = string_option;
	string_option = new StringOptionStruct("Inv_Filter_Vault", "All");
	Strings[string_option->Get_Name()] = string_option;
}

void OptionClass::Load(__int64 character_id, SClient *client)
{
	Set_Defaults(client);
	CharacterID = character_id % 1000000000;
	char filename[64];
	_snprintf(filename, 64, "player%d_%s", CharacterID, "options.ini");
	INIClass ini(filename);
	if (ini.Is_Loaded()) {
		char section[64];
































































































































































































































		int index = 0;
		while (true) {
			{
				_snprintf(section, 64, "%s-%03d", "BOOL", index);
				if (ini.Find_Section(section) == 0) {
					break;
				}
				StringClass name;
				ini.Get_String(name, section, "OptionName", "");
				if (name.Peek_Buffer() == NULL) {
					continue;
				}
				bool value = ini.Get_Bool(section, "OptionValue", false);
				BoolOptionStruct *option = Find_Bool(name);
				if (option != NULL) {
					option->Set_Value(value);
				} else {
				Unknown_4BC000("Client Options", "", __FILE__, __LINE__, "Obsolete bool option encountered");
				}
			}
			index++;
		}


		index = 0;
		while (true) {
			{
				_snprintf(section, 64, "%s-%03d", "INT", index);
				if (ini.Find_Section(section) == 0) {
					break;
				}
				StringClass name;
				ini.Get_String(name, section, "OptionName", "");
				if (name.Peek_Buffer() == NULL) {
					continue;
				}
				int value = ini.Get_Int(section, "OptionValue", 0);
				IntOptionStruct *option = Find_Int(name);
				if (option != NULL) {
					option->Set_Value(value);
				} else {
				Unknown_4BC000("Client Options", "", __FILE__, __LINE__, "Obsolete int option encountered");
				}
			}
			index++;
		}


		index = 0;
		while (true) {
			{
				_snprintf(section, 64, "%s-%03d", "FLOAT", index);
				if (ini.Find_Section(section) == 0) {
					break;
				}
				StringClass name;
				ini.Get_String(name, section, "OptionName", "");
				if (name.Peek_Buffer() == NULL) {
					continue;
				}
				float value = ini.Get_Float(section, "OptionValue", 0.0f);
				FloatOptionStruct *option = Find_Float(name);
				if (option != NULL) {
					option->Set_Value(value);
				} else {
				Unknown_4BC000("Client Options", "", __FILE__, __LINE__, "Obsolete float option encountered");
				}
			}
			index++;
		}



		index = 0;
		while (true) {
			{
				_snprintf(section, 64, "%s-%03d", "VECTOR3", index);
				if (ini.Find_Section(section) == 0) {
					break;
				}
				StringClass name;
				ini.Get_String(name, section, "OptionName", "");
				if (name.Peek_Buffer() == NULL) {
					continue;
				}
				TPoint3D<float> value = ini.Get_Point(section, "OptionValue", TPoint3D<float>(0.0f, 0.0f, 0.0f));
				Vector3OptionStruct *option = Find_Vector3(name);
				if (option != NULL) {
					option->Set_Value(Vector3(value.X, value.Y, value.Z));
				} else {
				Unknown_4BC000("Client Options", "", __FILE__, __LINE__, "Obsolete vec3 option encountered");
				}
			}
			index++;
		}


		index = 0;
		while (true) {
			{
				_snprintf(section, 64, "%s-%03d", "STRING", index);
				if (ini.Find_Section(section) == 0) {
					break;
				}
				StringClass name;
				ini.Get_String(name, section, "OptionName", "");
				if (name.Peek_Buffer() == NULL) {
					continue;
				}
				StringClass value;
				ini.Get_String(value, section, "OptionValue", "");
				StringOptionStruct *option = Find_String(name);
				if (option != NULL) {
					option->Set_Value(value);
				} else {
				Unknown_4BC000("Client Options", "", __FILE__, __LINE__, "Obsolete String option encountered");
				}
			}
			index++;
		}
	}
}

void OptionClass::Save()
{
	char filename[64];
	_snprintf(filename, 64, "player%d_%s", CharacterID, "options.ini");
	INIClass ini(filename);
	char section[64];

	int index = 0;
	{
		std::hash_map<std::string, BoolOptionStruct *>::iterator it;
		for (it = Bools.begin(); it != Bools.end(); ++it) {
			BoolOptionStruct *option = (*it).second;
			_snprintf(section, 64, "%s-%03d", "BOOL", index);
			ini.Put_String(section, "OptionName", option->Get_Name());
			ini.Put_Bool(section, "OptionValue", option->Get_Value());
			index++;
		}
	}

	index = 0;
	{
		std::hash_map<std::string, IntOptionStruct *>::iterator it;
		for (it = Ints.begin(); it != Ints.end(); ++it) {
			IntOptionStruct *option = (*it).second;
			_snprintf(section, 64, "%s-%03d", "INT", index);
			ini.Put_String(section, "OptionName", option->Get_Name());
			ini.Put_Int(section, "OptionValue", option->Get_Value());
			index++;
		}
	}

	index = 0;
	{
		std::hash_map<std::string, FloatOptionStruct *>::iterator it;
		for (it = Floats.begin(); it != Floats.end(); ++it) {
			FloatOptionStruct *option = (*it).second;
			_snprintf(section, 64, "%s-%03d", "FLOAT", index);
			ini.Put_String(section, "OptionName", option->Get_Name());
			float value = option->Value;
			ini.Put_Float(section, "OptionValue", value);
			index++;
		}
	}

	index = 0;
	{
		std::hash_map<std::string, Vector3OptionStruct *>::iterator it;
		for (it = Vector3s.begin(); it != Vector3s.end(); ++it) {
			Vector3OptionStruct *option = (*it).second;
			_snprintf(section, 64, "%s-%03d", "VECTOR3", index);
			ini.Put_String(section, "OptionName", option->Get_Name());
			Vector3 value = option->Get_Value();
			ini.Put_Point(section, "OptionValue", TPoint3D<float>(value.X, value.Y, value.Z));
			index++;
		}
	}

	index = 0;
	{
		std::hash_map<std::string, StringOptionStruct *>::iterator it;
		for (it = Strings.begin(); it != Strings.end(); ++it) {
			StringOptionStruct *option = (*it).second;
			_snprintf(section, 64, "%s-%03d", "STRING", index);
			ini.Put_String(section, "OptionName", option->Get_Name());
			ini.Put_String(section, "OptionValue", option->Get_Value());
			index++;
		}
	}

	ini.Save(filename);
}

BoolOptionStruct *OptionClass::Find_Bool(const char *name)
{
	return(Find_Option(Bools, name));
}

IntOptionStruct *OptionClass::Find_Int(const char *name)
{
	return(Find_Option(Ints, name));
}

FloatOptionStruct *OptionClass::Find_Float(const char *name)
{
	return(Find_Option(Floats, name));
}

Vector3OptionStruct *OptionClass::Find_Vector3(const char *name)
{
	return(Find_Option(Vector3s, name));
}

StringOptionStruct *OptionClass::Find_String(const char *name)
{
	return(Find_Option(Strings, name));
}

bool OptionClass::Get_Bool(const char *name)
{
	BoolOptionStruct *option = Find_Bool(name);
	if (option != NULL) {
		return(option->Value);
	}
	return(false);
}

int OptionClass::Get_Int(const char *name)
{
	IntOptionStruct *option = Find_Int(name);
	if (option != NULL) {
		return(option->Value);
	}
	return(0);
}

float OptionClass::Get_Float(const char *name)
{
	FloatOptionStruct *option = Find_Float(name);
	if (option != NULL) {
		return(option->Value);
	}
	return(0.0f);
}

Vector3 OptionClass::Get_Vector3(const char *name)
{
	Vector3OptionStruct *option = Find_Vector3(name);
	if (option != NULL) {
		return(option->Value);
	}
	return(Vector3(0.0f, 0.0f, 0.0f));
}

// NOT EXACT: shape, block order: this order gives the client's inlining (the "" path calls _Grow) but places the "" path first; the client places the copy first
std::string OptionClass::Get_String(const char *name)
{
	StringOptionStruct *option = Find_String(name);
	if (option == NULL) {
		return("");
	}
	return(option->Value);
}

bool OptionClass::Get_Default_Bool(const char *name)
{
	BoolOptionStruct *option = Find_Bool(name);
	if (option != NULL) {
		return(option->Default);
	}
	return(false);
}

int OptionClass::Get_Default_Int(const char *name)
{
	IntOptionStruct *option = Find_Int(name);
	if (option != NULL) {
		return(option->Default);
	}
	return(0);
}

float OptionClass::Get_Default_Float(const char *name)
{
	FloatOptionStruct *option = Find_Float(name);
	if (option != NULL) {
		return(option->Default);
	}
	return(0.0f);
}

Vector3 OptionClass::Get_Default_Vector3(const char *name)
{
	Vector3OptionStruct *option = Find_Vector3(name);
	if (option != NULL) {
		return(option->Default);
	}
	return(Vector3(0.0f, 0.0f, 0.0f));
}

// NOT EXACT: shape, as Get_String
std::string OptionClass::Get_Default_String(const char *name)
{
	StringOptionStruct *option = Find_String(name);
	if (option == NULL) {
		return("");
	}
	return(option->Default);
}


/*
**	Reconstruction notes
**
**	client.exe 0x7F0F60-0x7FEBE0 (the object: after the previous file's guarded <string> static at
**	0x7F0F32, through its own at 0x7FEB32): the constructor 0x7F0F60 (five default hash_maps of 100
**	buckets, then CharacterID), the maps' out-of-line destructors for the constructor's unwinding
**	(0x7F1130-0x7F13A0, 0x7F1770), the destructor 0x7F1470, Set_Instance 0x7F1840 (0xBE8B38, called
**	with the constructor from 0x526DE0), Free_Options 0x7F1860 (no caller: inlined into the destructor
**	and Set_Defaults), Set_Defaults 0x7F1AD0 (126 options: the first 52 bools with their constructor
**	and the map's operator[] inlined, the rest called; the option constructors and name accessors
**	0x7F7E10-0x7F83F0 and graphics_settings.h's 0x7F8410 / 0x7F8480 are its out-of-line copies),
**	Load 0x7F84F0 (the reports on the client's lines 571, 595, 619, 644, 669: D25), Save 0x7F8EF0,
**	the Find / Get / Get_Default accessors 0x7F9520-0x7F9970, then the hash_map instances
**	0x7F9AF0-0x7FE980 (libs/sgistl, the string hash 0x42DB00: D87) and the helpers Find_Option
**	0x7FCBF0... / Free_Option_Map 0x7FC600...
**	Set_Defaults reads SClient's render budgets (soptions.h: PolyBudget +0x1158, LandPolyBudget
**	+0x1160, LandLOD +0x1164, TextureReduction +0x1168) into the graphics sliders' percentages.
**	Load: an option the defaults don't have is reported ('Obsolete ... option encountered'); a
**	section without OptionName is skipped without advancing the index (as the client).
**	Forms: Load's sets go through the structs' inline Set_Value (that spends the inline budget so
**	the bool and int finds stay calls, as the client's); Save reads through the inline Get_Value, a
**	Vector3 local and a plain float local. Get_String / Get_Default_String (NOT EXACT): written with
**	the empty-string return first they inline as the client's (the "" path calls _Grow) but the
**	blocks come out in the other order; with the copy first the "" path inlines _Grow.
*/
