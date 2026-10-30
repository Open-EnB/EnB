// Earth & Beyond client (reconstruction)
// udpreplay: what app\shared\shipcust_desc.cpp (brought in from app.lib by RECUSTOMIZE_SHIP_DONE 130's
// ShipCustDescClass, pkrecustomize.cpp) needs that the harness doesn't compile: the races' default settings
// (0xBE56D8, filled at run time), the key/value visitor's typed adds (not used by the packets: they do
// nothing) and the description's assignment (0x55D8A0, ui_avsel.cpp's window: a copy of the members after
// the two vtable pointers here). VC6 can't define the member template's specialisations out of the class
// (C1001) and mangles them without their template arguments, so this file declares its own minimal
// classes (stand-ins only: never include shipcust_desc.h or unknown_packetdata.h here).
#include <string.h>

// the encoder's bool add (kv_encoder_base.h's member template Add_Typed<bool>; the profession and race
// adds are shipcust_desc.cpp's own since 2026-10-28, their converters prof.cpp's and race.cpp's)
class KVEncoderBaseClass
{
public:
	void Add_Typed(int mask, const char *name, bool *value);
};

void KVEncoderBaseClass::Add_Typed(int, const char *, bool *) {}

class ShipCustSettingsClass
{
public:
	char Data[0x20];
};

ShipCustSettingsClass Unknown_BE56D8[4][2];

class ShipCustDescClass
{
public:
	ShipCustDescClass &operator=(const ShipCustDescClass &that);
	char Data[0x160];
};

ShipCustDescClass &ShipCustDescClass::operator=(const ShipCustDescClass &that)
{
	memcpy(Data + 8, that.Data + 8, sizeof(Data) - 8);
	return(*this);
}
