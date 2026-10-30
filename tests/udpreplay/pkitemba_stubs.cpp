// udpreplay: ITEM_BASE 37's item description (ItemBaseInfoClass, client 0x83F490-0x840400, not rebuilt).
// Here, not in stubs/app_stubs.cpp: these call RawPacketClass, and stubs.lib goes into every program
// (VIDTEST would then pull in pk.obj and the connection layer).
#include "..\..\..\app\packets\pkitemba.h"
#include <string.h>

// 0x83F490 / 0x840030 / 0x83FEF0 / 0x83FE30: ITEM_BASE's item description (opcode 37). The harness keeps
// its part of the message as bytes: its length depends on the client's attribute type table (filled at
// run time), so Read takes the first split after which the packet's fixed tail (22 bytes, then three
// strings of text with 2-byte lengths) ends exactly at the message's end.
ItemBaseInfoClass::ItemBaseInfoClass()
{
	memset(Unknown08, 0, sizeof(Unknown08));
}

void ItemBaseInfoClass::Read(RawPacketClass *raw)
{
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
	*(char **)Unknown08 = bytes;
	*(int *)(Unknown08 + 4) = len;
}

void ItemBaseInfoClass::Write(RawPacketClass *raw)
{
	raw->Add(*(char **)Unknown08, *(int *)(Unknown08 + 4));
}

int ItemBaseInfoClass::Get_Size()
{
	return *(int *)(Unknown08 + 4);
}
