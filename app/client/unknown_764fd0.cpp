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

// @0x765540
void Unknown764FD0Class::Unknown_765540()
{
	Vector3OptionStruct *option;

	option = OptionClass::Instance->Find_Vector3("Color_Context");
	if (option != NULL) {
		option->Set_Value(Unknown00[0]);
	}
	option = OptionClass::Instance->Find_Vector3("Color_OOC");
	if (option != NULL) {
		option->Set_Value(Unknown00[1]);
	}
	option = OptionClass::Instance->Find_Vector3("Color_Broadcast");
	if (option != NULL) {
		option->Set_Value(Unknown00[2]);
	}
	option = OptionClass::Instance->Find_Vector3("Color_Tell");
	if (option != NULL) {
		option->Set_Value(Unknown00[3]);
	}
	option = OptionClass::Instance->Find_Vector3("Color_Group");
	if (option != NULL) {
		option->Set_Value(Unknown00[4]);
	}
	option = OptionClass::Instance->Find_Vector3("Color_NPC");
	if (option != NULL) {
		option->Set_Value(Unknown00[5]);
	}
	option = OptionClass::Instance->Find_Vector3("Color_Tip");
	if (option != NULL) {
		option->Set_Value(Unknown00[6]);
	}
	option = OptionClass::Instance->Find_Vector3("Color_Local");
	if (option != NULL) {
		option->Set_Value(Unknown00[7]);
	}
	option = OptionClass::Instance->Find_Vector3("Color_Guild");
	if (option != NULL) {
		option->Set_Value(Unknown00[8]);
	}
	option = OptionClass::Instance->Find_Vector3("Color_General_Channel");
	if (option != NULL) {
		option->Set_Value(Unknown00[9]);
	}
	option = OptionClass::Instance->Find_Vector3("Color_OOC_Channel");
	if (option != NULL) {
		option->Set_Value(Unknown00[10]);
	}
	option = OptionClass::Instance->Find_Vector3("Color_Newbie_Channel");
	if (option != NULL) {
		option->Set_Value(Unknown00[11]);
	}
	option = OptionClass::Instance->Find_Vector3("Color_Market_Channel");
	if (option != NULL) {
		option->Set_Value(Unknown00[12]);
	}
	option = OptionClass::Instance->Find_Vector3("Color_Private_Channel");
	if (option != NULL) {
		option->Set_Value(Unknown00[13]);
	}
	option = OptionClass::Instance->Find_Vector3("Color_Race_Channel");
	if (option != NULL) {
		option->Set_Value(Unknown00[14]);
	}
	option = OptionClass::Instance->Find_Vector3("Color_Class_Channel");
	if (option != NULL) {
		option->Set_Value(Unknown00[15]);
	}
	option = OptionClass::Instance->Find_Vector3("Color_Echo");
	if (option != NULL) {
		option->Set_Value(Unknown00[16]);
	}
	option = OptionClass::Instance->Find_Vector3("Color_Error_Context");
	if (option != NULL) {
		option->Set_Value(Unknown00[17]);
	}
	option = OptionClass::Instance->Find_Vector3("Color_Error_OOC");
	if (option != NULL) {
		option->Set_Value(Unknown00[18]);
	}
	option = OptionClass::Instance->Find_Vector3("Color_Error_System");
	if (option != NULL) {
		option->Set_Value(Unknown00[19]);
	}
	option = OptionClass::Instance->Find_Vector3("Color_Combat");
	if (option != NULL) {
		option->Set_Value(Unknown00[20]);
	}
	option = OptionClass::Instance->Find_Vector3("Color_Warning");
	if (option != NULL) {
		option->Set_Value(Unknown00[21]);
	}
	option = OptionClass::Instance->Find_Vector3("Color_Attention");
	if (option != NULL) {
		option->Set_Value(Unknown00[22]);
	}
	option = OptionClass::Instance->Find_Vector3("Color_Debug");
	if (option != NULL) {
		option->Set_Value(Unknown00[23]);
	}
}

// @0x765BE0
void Unknown764FD0Class::Unknown_765BE0()
{
	Unknown00[0].Set(0.31f, 1.0f, 0.0f);
	Unknown00[1].Set(0.9f, 0.9f, 0.9f);
	Unknown00[2].Set(1.0f, 0.6f, 0.0f);
	Unknown00[3].Set(0.99f, 0.33f, 0.99f);
	Unknown00[4].Set(0.8f, 0.2f, 0.8f);
	Unknown00[5].Set(0.31f, 1.0f, 0.0f);
	Unknown00[6].Set(0.31f, 1.0f, 0.0f);
	Unknown00[7].Set(1.0f, 0.6f, 0.0f);
	Unknown00[8].Set(0.7f, 0.7f, 1.0f);
	Unknown00[9].Set(1.0f, 0.1f, 0.3f);
	Unknown00[10].Set(0.33f, 0.99f, 0.99f);
	Unknown00[11].Set(0.5f, 0.5f, 1.0f);
	Unknown00[12].Set(0.5f, 1.0f, 0.5f);
	Unknown00[13].Set(1.0f, 1.0f, 1.0f);
	Unknown00[14].Set(0.8f, 0.8f, 0.8f);
	Unknown00[15].Set(0.9f, 0.9f, 0.5f);
	Unknown00[16].Set(0.9f, 0.9f, 0.9f);
	Unknown00[17].Set(0.99f, 0.33f, 0.33f);
	Unknown00[18].Set(0.9f, 0.9f, 0.9f);
	Unknown00[19].Set(0.9f, 0.9f, 0.9f);
	Unknown00[20].Set(0.9f, 0.9f, 0.9f);
	Unknown00[21].Set(0.99f, 0.33f, 0.33f);
	Unknown00[22].Set(0.9f, 0.9f, 0.9f);
	Unknown00[23].Set(0.33f, 0.99f, 0.99f);
}

// @0x765DF0
Vector3 Unknown764FD0Class::Unknown_765DF0(int index)
{
	if (index < 24 && index >= 0) {
		return(Unknown00[index]);
	}
	return(Vector3(1.0f, 1.0f, 1.0f));
}

// @0x765E40
void Unknown764FD0Class::Unknown_765E40(int index, const Vector3 &color)
{
	if (index < 24 && index >= 0) {
		float red = WWMath::Clamp(color.X);
		float green = WWMath::Clamp(color.Y);
		float blue = WWMath::Clamp(color.Z);
		Unknown00[index].Set(red, green, blue);
	}
}

/*
**	Reconstruction notes (GS1, 2026-11-10). The object at SClient +0x13CC: 24 chat line colours, one per channel
**	type, from the options' 'Color_*' entries (0x765010, SClient::Init) and read by the chat window and SClient
**	0x750F70 (0x765DF0, white out of range). The client's object also holds 0x765540 (the colours back into the
**	options: Find_Vector3 / Vector3OptionStruct::Set_Value 0x765BB0), 0x765BE0 (the default colours) and 0x765E40
**	(one colour set, clamped to 0..1 through WWMath::Clamp; the three parts are read into locals first, in order),
**	all three called by SClient 0x747FA0 ('/resetcolor', '/setcolor'; B9-2, 2026-11-13). Interim placement (D137):
**	the file name is the constructor's address; move the functions when the client's file is known.
*/
