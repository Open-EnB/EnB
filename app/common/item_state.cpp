// item_state.cpp: the item state enum's text converters (see the notes at the end)

#include "item_state.h"
#include <stdio.h>
#include <string.h>
#include "..\shared\g_assert.h"





































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

/*
**	Reconstruction notes (RECONSTRUCTED): the start of the client's item_state.cpp object (__FILE__ 0xBAE428),
**	after item_info.cpp's closing initialiser 0x8428F0: ItemStateEnum's String_To_Value 0x842930 ('ItemStateEnum:%x',
**	KVPairTemplateClass<ItemStateEnum>::Set_Value_String 0x709770 calls it) and Value_To_String 0x842970 (FAIL_IF at
**	line 57). The rest of the object (0x842A60, the state's message text and sound, FAIL_IF at line 136) is not
**	rebuilt yet.
*/
