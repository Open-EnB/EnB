// Earth & Beyond client (reconstruction)
// udpreplay: the effect descriptions of ITEM_BASE 37's item info (ItemEffectDescriptionClass, client
// 0x83DB00-0x83E7C0, item_effect_description.cpp, not rebuilt). The item info itself is the real
// app\common\item_info.cpp. Here, not in stubs/app_stubs.cpp: these call RawPacketClass, and stubs.lib
// goes into every program (VIDTEST would then pull in pk.obj and the connection layer).
#include "..\..\..\app\packets\pkitemba.h"
#include <string.h>

// 0x83E040 / 0x83DF10 / 0x83DE70: an item info ends with two effect descriptions (the activated effects,
// then the equip effects), whose length depends on effect_description.cpp's format. The harness keeps both
// as bytes in the first one: its Read takes the first split after which the packet's fixed tail (22 bytes,
// then three strings of text with 2-byte lengths) ends exactly at the message's end; the second one's Read
// takes nothing. The bytes and their length sit in Unknown18 / Unknown1C (zero from the constructor).
static bool SecondDescription = false;

void ItemEffectDescriptionClass::Read(RawPacketClass *raw)
{
	if (SecondDescription) {
		SecondDescription = false;
		Unknown18 = 0;
		Unknown1C = 0;
		return;
	}
	SecondDescription = true;
	const unsigned char *p = (const unsigned char *)raw->Data.Buffer + raw->Data.Pos;
	int left = raw->Data.Size - raw->Data.Pos;
	int len = 0;
	for (int k = 1; k + 28 <= left; k++) {
		int q = k + 22;
		int s;
		bool text = true;
		for (s = 0; s < 3 && q + 2 <= left; s++) {
			int n = p[q] | (p[q + 1] << 8);
			q += 2;
			for (int c = 0; c < n && q + c < left; c++) {
				if (p[q + c] < 0x20 && p[q + c] != '\n' && p[q + c] != '\r' && p[q + c] != '\t') {
					text = false;
				}
			}
			q += n;
		}
		if (s == 3 && q == left && text) {
			len = k;
			break;
		}
	}
	char *bytes = new char[len + 1];
	raw->Get(bytes, len);
	Unknown18 = (int)bytes;
	Unknown1C = len;
}

void ItemEffectDescriptionClass::Write(RawPacketClass *raw)
{
	raw->Add((char *)Unknown18, Unknown1C);
}

int ItemEffectDescriptionClass::Get_Size()
{
	return Unknown1C;
}
