// udpreplay: what app\shared\avatar_desc.cpp calls that the harness doesn't compile (AVATAR_DESCRIPTION 97's
// member brings avatar_desc.cpp in: udpreplay.py's CONN). Its key/value side and the defaults loader's
// report are not used by the packets; each stand-in does nothing.
#define UNKNOWN_PACKETDATA_BODIES	// kv_encoder_base.h declares the typed adds under it (D104)
#include <string>
#include "..\..\..\app\shared\unknown_packetdata.h"

// 0x4BC260: E&B's message reporter (the defaults loader's missing-file message)
void Unknown_4BC260(const char *, const char *, const char *, int, const char *, ...)
{
}

// 0x4BE9D0: a name turned into another (kv_util.cpp's run)
std::string Unknown_4BE9D0(const char *name)
{
	return std::string(name);
}

// the key/value encoder's typed adds (0x4C2EB0, 0x4C2FE0, 0x4B2BB0, 0x4B4A70)
void KVEncoderBaseClass::Add(int, const char *, int *) {}
void KVEncoderBaseClass::Add(int, const char *, char *) {}
void KVEncoderBaseClass::Add(int, const char *, Vector3 *) {}
void KVEncoderBaseClass::Add(int, const char *, char *, int) {}
