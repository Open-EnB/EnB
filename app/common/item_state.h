/*
**	item_state.h: the item state enum's text converters (item_state.cpp). Reconstructed (D23).
*/

#ifndef ITEM_STATE_H
#define ITEM_STATE_H

// NAME 5: the client's Get_Type_Name string 'ItemStateEnum' (KVPair vtable 0xB019F4, 0x707FF0 inline), leaf vtable 0xB017EC
enum ItemStateEnum { UNKNOWN_B017EC_0 };	// vtable 0xB017EC, 4 bytes (aux_roots.h declares it too: never include both)

bool String_To_Value(const char *text, ItemStateEnum *value);	// 0x842930
char *Value_To_String(char *buf, int size, ItemStateEnum value);	// 0x842970

#endif
