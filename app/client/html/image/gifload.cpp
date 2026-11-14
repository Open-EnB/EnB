// Earth & Beyond client (reconstruction): app\client\html\image\gifload.cpp. Notes at the end of the file.
#include "gifload.h"
#include "..\..\..\shared\g_assert.h"
#include <string.h>

#define BAD_CODE_SIZE	-20

static unsigned short code_mask[13] = {
	0,
	0x0001, 0x0003,
	0x0007, 0x000F,
	0x001F, 0x003F,
	0x007F, 0x00FF,
	0x01FF, 0x03FF,
	0x07FF, 0x0FFF
};

static unsigned char b1;
static unsigned short ret;


























































































































































































































































































































// NOT EXACT: 0x76D990, functional (D64): Bennett's structure and the client's branch order, but the register
// allocation differs throughout (the client keeps 0 in edi and bufptr in edi later, frame 0x40 vs 0x3C)
short HtmlGifImageClass::Decode(short linewidth)
{
	unsigned char *sp;
	unsigned char *bufptr;
	short code;
	short oc;
	short bufcnt;
	unsigned char fc;
	short c;
	short size;
	short clear;
	short ending;
	short newcodes;
	short top_slot;
	short slot;
	short xcount;
	short ycount;
	short direct;
	short len;
	short i;
	int row = 0;

	if ((size = Get_Byte()) < 0)
		return(size);
	if (size < 2 || 9 < size)
		return(BAD_CODE_SIZE);

	CodeSize = size + 1;
	top_slot = 1 << CodeSize;
	clear = 1 << size;
	ending = clear + 1;
	slot = newcodes = ending + 1;
	Lengths[slot] = 0;
	BytesLeft = BitsLeft = 0;
	for (i = 0; i < slot; i++)
		Lengths[i] = 0;

	oc = fc = 0;
	xcount = ycount = 0;
	sp = Stack;
	bufptr = Line;
	bufcnt = linewidth;

	while ((c = Get_Next_Code()) != ending) {
		if (c < 0)
			break;

		if (c == clear) {
			CodeSize = size + 1;
			Lengths[newcodes] = 0;
			top_slot = 1 << CodeSize;
			slot = newcodes;
			while ((c = Get_Next_Code()) == clear)
				;
			if (c == ending)
				break;
			if (c >= slot)
				c = 0;
			oc = fc = c;
			*sp++ = fc;
		} else {
			code = c;
			if (code >= slot) {
				if (code > slot) {
					++BadCodes;
					c = slot;
				}
				code = oc;
				*sp++ = fc;
			}

			direct = 0;
			while (code >= newcodes) {
				G_ASSERT(code < 4096);
				len = Lengths[code];
				if (len > 0 && bufcnt - len > 0) {
					direct = -1;
					i = len;
					do {


						G_ASSERT(code < 4096);
						bufptr[i--] = Suffix[code];
						code = Prefix[code];
					} while (i > 0);
					*bufptr = (unsigned char)code;
					bufptr += len + 1;
					bufcnt -= len + 1;
					if (bufcnt == 0) {
						if (--ycount < 0) {
							Out_Line(Line, bufptr - Line, row);
							row++;
							ycount = 0;
						}
						bufptr = Line;
						xcount = 0;
						bufcnt = linewidth;
					}
				} else {




					G_ASSERT(code < 4096);
					*sp++ = Suffix[code];
					code = Prefix[code];
				}
			}
			if (direct == 0)
				*sp++ = (unsigned char)code;

			if (slot < top_slot) {








				G_ASSERT(slot < 4096);
				Lengths[slot] = Lengths[oc] + 1;
				Suffix[slot] = fc = (unsigned char)code;
				Prefix[slot++] = oc;
				oc = c;
			}
			if (slot >= top_slot) {
				if (CodeSize < 12) {
					top_slot <<= 1;
					++CodeSize;
				}
			}
		}

		while (sp > Stack) {
			--sp;
			if (--xcount < 0) {
				xcount = 0;
				*bufptr++ = *sp;
			}
			if (--bufcnt == 0) {
				if (--ycount < 0) {
					Out_Line(Line, bufptr - Line, row);
					row++;
					ycount = 0;
				}
				bufptr = Line;
				xcount = 0;
				bufcnt = linewidth;
			}
		}
	}
	return(0);
}

// NOT EXACT: 0x76DF60, one instruction: the client reads ret for the final mask with a word load, ours a dword
short HtmlGifImageClass::Get_Next_Code(void)
{
	if (BitsLeft == 0) {
		if (BytesLeft <= 0) {
			BlockPointer = Block;
			if ((BytesLeft = (short)Get_Byte()) < 0)
				return(BytesLeft);
			else if (BytesLeft)
				Get_Bytes(Block, BytesLeft);
		}
		b1 = *BlockPointer++;
		BitsLeft = 8;
		--BytesLeft;
	}

	ret = b1 >> (8 - BitsLeft);
	while (CodeSize > BitsLeft) {
		if (BytesLeft <= 0) {
			BlockPointer = Block;
			if ((BytesLeft = (short)Get_Byte()) < 0)
				return(BytesLeft);
			else if (BytesLeft)
				Get_Bytes(Block, BytesLeft);
		}
		b1 = *BlockPointer++;
		ret |= b1 << BitsLeft;
		BitsLeft += 8;
		--BytesLeft;
	}
	BitsLeft -= CodeSize;
	return(code_mask[CodeSize] & ret);
}

void HtmlGifImageClass::Init(void)
{
	Width = 0;
	Height = 0;
	ColorCount = 0;
	Unknown124 = 0;
	Unknown128 = 0;
	for (int i = 0; i < 768; i++)
		Palette[i] = i / 3;
	Transparent = -1;
	CodeSize = 0;
	BytesLeft = 0;
	BitsLeft = 0;
	memset(Block, 0, sizeof(Block));
	BlockPointer = NULL;
	BadCodes = 0;
	memset(Lengths, 0, 4096);
	memset(Prefix, 0, 4096);
	memset(Suffix, 0, 10000);
	memset(Stack, 0, sizeof(Stack));
	memset(Line, 0, sizeof(Line));
}

int HtmlGifImageClass::Get_Byte(void)
{
	if (Position < Length)
		return((unsigned char)Data[Position++]);
	return(-1);
}

int HtmlGifImageClass::Get_Bytes(unsigned char *buffer, int count)
{
	int left = Length - Position;
	if (left <= 0)
		return(-1);
	if (left < count)
		count = left;
	memcpy(buffer, Data + Position, count);
	Position += count;
	return(count);
}

void HtmlGifImageClass::Out_Line(unsigned char *pixels, int length, int row)
{
	if (Bits == 32) {
		unsigned long *buffer = (unsigned long *)Surf->Lock();
		int pitch = Surf->Stride() / 4;
		for (int i = 0; i < length; i++)
			buffer[pitch * row + i] = Colors[pixels[i]];
	} else if (Bits == 24) {
	} else if (Bits == 16) {
		unsigned short *buffer = (unsigned short *)Surf->Lock();
		int pitch = Surf->Stride() / 2;
		for (int i = 0; i < length; i++)
			buffer[pitch * row + i] = (unsigned short)Colors[pixels[i]];
	}
	Surf->Unlock();
}

/*
** Notes (reconstruction).
**
**	* The file: its __FILE__ 'e:\build\beta\G\app\Client\html\image\gifload.cpp' (0xB9C900) in Decode's asserts
**	  ('code < 4096' lines 409 / 417 / 439, 'slot < 4096' 456). Its .data: code_mask (0xB9C8B8), then 'GIF87a'
**	  (0xB9C8D8, Set_Data 0x76D4A0's), then the assert texts: Set_Data and so the gif's constructor 0x76D3E0, its
**	  destructors 0x76D450 / 0x76D480 and the inline Get_Surface / Get_Name copies 0x76D410 / 0x76D430 are this
**	  file's too (0x76D3E0-0x76D990: not rebuilt yet; the blank lines above Decode are their room). Its bss: b1
**	  0xBE82DC, ret 0xBE82DE.
**	* Get_Byte and Get_Bytes are ordinary members defined after their callers: /Ob2 inlines them into Set_Data and
**	  Get_Next_Code, and their copies (0x76E310, 0x76E350) stay with no caller; Init 0x76E200 has none either.
**	  So 0x76E200-0x76E4A4 are this file's, not image.cpp's (whose object starts at its constructor 0x76E4F0).
**	* Decode is Bennett's decoder() with: no malloc (Line is a member), no out_line of a last partial line,
**	  the clear code's first code pushed on the stack (drained by the shared loop), x / y skip counters fixed
**	  at 0, and the Lengths table: a string shorter than the room left in the line is written into it backwards
**	  at once.
*/
