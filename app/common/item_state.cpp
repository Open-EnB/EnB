// item_state.cpp: the item state enum's text converters (see the notes at the end)

#include "item_state.h"
#include <stdio.h>
#include <string.h>
#include "..\shared\g_assert.h"
#include <string>




































// @0x842930
bool String_To_Value(const char *text, ItemStateEnum *value)
{
	*value = (ItemStateEnum)0;
	return(sscanf(text, "ItemStateEnum:%x", value) == 1);
}

// @0x842970
char *Value_To_String(char *buf, int size, ItemStateEnum value)
{
	char str_enum[64];	// NAME 3: the FAIL_IF's 'str_enum'
	int slen = strlen("ItemStateEnum:");	// NAME 3: the FAIL_IF's 'slen'
	sprintf(str_enum, "%x", value);
	FAIL_IF((int)(slen + strlen(str_enum) + 1 ) > size) return(NULL);
	strcpy(buf, "ItemStateEnum:");
	strcat(buf, str_enum);
	return(buf);
}




















// @0x842A60
int Item_State_Message(std::string &text, std::string &sound, const char *name, int state)	// NAME 2: what it does (no caller found in the client)
{
	char buffer[128];
	const char *format = NULL;
	int result = 0x11;
	switch (state) {
		case 0:
			text.assign("No State");
			break;
		case 1:
			format = "%s is on.";
			result = 0;
			break;
		case 2:
			text = "You are not currently permitted to engage in player vs. player combat.";
			sound = "item_state_no_pvp.wav";
			break;
		case 0x10:
			format = "No target for %s.";
			sound = "item_state_no_target.wav";
			break;
		case 0x20:
			format = "Invalid target for %s.";
			break;
		case 8:
			format = "%s has no ammo.";
			sound = "item_state_no_ammo.wav";
			break;
		case 4:
			format = "%s is no longer operational.";
			sound = "item_state_destroyed.wav";
			break;
		case 0x40:
			format = "Ammo exploded in %s.";
			sound = "item_state_ammo_explosion.wav";
			break;
		case 0x200:
			format = "%s is being installed.";
			sound = "item_state_not_ready.wav";
			break;
		case 0x100:
			format = "%s misfired!";
			sound = "item_state_misfire.wav";
			break;
		case 0x80:
			format = "Insufficient energy to use %s.";
			sound = "item_state_insufficient_energy.wav";
			break;
		case 0x400:
			format = "%s not ready to activate.";
			sound = "item_state_not_ready.wav";
			break;
		default:
			G_ASSERT(false);
			text = "Unkown Item Failure";
			break;
		case 0x2000:
			format = "Autofire Enabled";
			break;
		case 0x1000:
			format = "Must be facing target to fire this weapon.";
			sound = "item_state_out_of_firing_arc.wav";
			break;
		case 0x800:
			format = "Out of range for %s.";
			sound = "item_state_out_of_range.wav";
			break;
	}
	if (format != NULL) {
		sprintf(buffer, format, name);
		text = buffer;
	}
	return(result);
}

/*
**	Reconstruction notes (RECONSTRUCTED): the start of the client's item_state.cpp object (__FILE__ 0xBAE428),
**	after item_info.cpp's closing initialiser 0x8428F0: ItemStateEnum's String_To_Value 0x842930 ('ItemStateEnum:%x',
**	KVPairTemplateClass<ItemStateEnum>::Set_Value_String 0x709770 calls it) and Value_To_String 0x842970 (FAIL_IF at
**	line 57). Then 0x842A60 (QC, 2026-11-16), the item state's message text and sound and the chat type it returns
**	(0x11, 0 for '%s is on.'; G_ASSERT(false) at line 136 for an unknown state); no caller found in the client. Its
**	cases are in the client's code order (VC6 lays the case bodies out in source order); case 0 and the default break
**	to the shared tail (format NULL), which keeps result (0x11) on the stack as the client does (run 2026-11-25-evening).
*/
