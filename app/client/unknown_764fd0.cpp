// Earth & Beyond client (reconstruction): the chat line colours 0x764FD0 .. 0x765E40 (interim placement, D137: the
// client's object is not known; it sits between proj_decal_manager.cpp and tsk_game.cpp). Notes at the end of the file.
#include "sclient.h"
#include "option.h"

// @0x764FD0
Unknown764FD0Class::Unknown764FD0Class()
{
	for (int i = 0; i < 24; i++) {
		Unknown00[i].Set(1.0f, 1.0f, 1.0f);
	}
}

// @0x765010
void Unknown764FD0Class::Unknown_765010()
{
	Unknown00[0] = OptionClass::Instance->Get_Vector3("Color_Context");
	Unknown00[1] = OptionClass::Instance->Get_Vector3("Color_OOC");
	Unknown00[2] = OptionClass::Instance->Get_Vector3("Color_Broadcast");
	Unknown00[3] = OptionClass::Instance->Get_Vector3("Color_Tell");
	Unknown00[4] = OptionClass::Instance->Get_Vector3("Color_Group");
	Unknown00[5] = OptionClass::Instance->Get_Vector3("Color_NPC");
	Unknown00[6] = OptionClass::Instance->Get_Vector3("Color_Tip");
	Unknown00[7] = OptionClass::Instance->Get_Vector3("Color_Local");
	Unknown00[8] = OptionClass::Instance->Get_Vector3("Color_Guild");
	Unknown00[9] = OptionClass::Instance->Get_Vector3("Color_General_Channel");
	Unknown00[10] = OptionClass::Instance->Get_Vector3("Color_OOC_Channel");
	Unknown00[11] = OptionClass::Instance->Get_Vector3("Color_Newbie_Channel");
	Unknown00[12] = OptionClass::Instance->Get_Vector3("Color_Market_Channel");
	Unknown00[13] = OptionClass::Instance->Get_Vector3("Color_Private_Channel");
	Unknown00[14] = OptionClass::Instance->Get_Vector3("Color_Race_Channel");
	Unknown00[15] = OptionClass::Instance->Get_Vector3("Color_Class_Channel");
	Unknown00[16] = OptionClass::Instance->Get_Vector3("Color_Echo");
	Unknown00[17] = OptionClass::Instance->Get_Vector3("Color_Error_Context");
	Unknown00[18] = OptionClass::Instance->Get_Vector3("Color_Error_OOC");
	Unknown00[19] = OptionClass::Instance->Get_Vector3("Color_Error_System");
	Unknown00[20] = OptionClass::Instance->Get_Vector3("Color_Combat");
	Unknown00[21] = OptionClass::Instance->Get_Vector3("Color_Warning");
	Unknown00[22] = OptionClass::Instance->Get_Vector3("Color_Attention");
	Unknown00[23] = OptionClass::Instance->Get_Vector3("Color_Debug");
}

// @0x765DF0
Vector3 Unknown764FD0Class::Unknown_765DF0(int index)
{
	if (index < 24 && index >= 0) {
		return(Unknown00[index]);
	}
	return(Vector3(1.0f, 1.0f, 1.0f));
}

/*
**	Reconstruction notes (GS1, 2026-11-10). The object at SClient +0x13CC: 24 chat line colours, one per channel
**	type, from the options' 'Color_*' entries (0x765010, SClient::Init) and read by the chat window and SClient
**	0x750F70 (0x765DF0, white out of range). The client's object also holds 0x765540 (the colours back into the
**	options: Find_Vector3 / Vector3OptionStruct::Set_Value 0x765BB0), 0x765BE0 (the default colours) and 0x765E40
**	(one colour set, clamped to 0..1): not rebuilt yet. Interim placement (D137): the file name is the constructor's
**	address; move the functions when the client's file is known.
*/
