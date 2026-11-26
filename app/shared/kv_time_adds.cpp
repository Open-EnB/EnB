// Earth & Beyond client (reconstruction): notes at the end of the file
#include "kv_encoder_base.h"
#include "kv_acc_time.h"

// The adds over them (cdecl, no caller in the client)
// @0x4B5F00
void Add_Nothing(KVEncoderBaseClass *encoder, int mask, const char *name)	// NAME 1: the prefix pushed and popped
{
	encoder->Push_Prefix(name);
	encoder->Pop_Prefix();
}

// @0x4B5F30
void Add_Tick_Seconds(KVEncoderBaseClass *encoder, int mask, const char *name, unsigned int *value)	// NAME 2: what it does
{
	encoder->Add_Value(mask, name, new KVTickSecondsAccessorClass(value));
}

// @0x4B60B0
void Add_Tenths_Seconds(KVEncoderBaseClass *encoder, int mask, const char *name, unsigned int *value)	// NAME 2: what it does
{
	encoder->Add_Value(mask, name, new KVTenthsSecondsAccessorClass(value));
}

/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: unknown; no assert, no __FILE__: the name is PROVISIONAL (D137). An object of its own, client 0x4B5EB0 (permstr.h's static)
**	- before kv_list.cpp's static 0x4B6280.
*/
