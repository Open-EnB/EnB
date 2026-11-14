// Earth & Beyond client (reconstruction): app\client\html\image\gifload.h. Notes at the end of gifload.cpp.
#ifndef GIFLOAD_H
#define GIFLOAD_H

#include "image.h"

// The image the parser makes for <img src> and <body background>: 0x22A04 bytes (the parser's operator new),
// vtable 0xB043B8. Set_Data reads a GIF87a stream; Decode is Steven A. Bennett's LZW decoder (decoder /
// get_next_code / init_exp, its BAD_CODE_SIZE -20 and code_mask) made members, with a string length table
// (Lengths) that writes whole strings straight into the line, and asserts on the table indexes.
class HtmlGifImageClass : public HtmlImageClass  // NAME 1: gifload.cpp's image ('GIF87a'; 'image/gif' links)
{
public:
	HtmlGifImageClass(char *name, int bits);  // 0x76D3E0  // NAME 2: the src and the surface's bits per pixel
	virtual ~HtmlGifImageClass(void);  // 0x76D480 (its deleting destructor 0x76D450)

	virtual int Set_Data(char *data, int length);  // slot +4, 0x76D4A0  // NAME 2: the header, the palette (Colors through 0x76D340), the extensions and each image's Decode

	short Decode(short linewidth);  // 0x76D990  // NAME 2: Bennett's decoder(linewidth); 0, -1 at the end of the data, -20 for a bad code size
	short Get_Next_Code(void);  // 0x76DF60  // NAME 2: Bennett's get_next_code
	void Init(void);  // 0x76E200  // NAME 1: every field from +0x118 zeroed, the palette a grey ramp (no caller found)
	int Get_Byte(void);  // 0x76E310  // NAME 2: Bennett's get_byte, from Data (inlined everywhere; the copy has no caller)
	int Get_Bytes(unsigned char *buffer, int count);  // 0x76E350  // NAME 1: up to count bytes of Data (inlined in Get_Next_Code)
	void Out_Line(unsigned char *pixels, int length, int row);  // 0x76E3D0  // NAME 2: Bennett's out_line: the row's pixels through Colors into Surf (32 / 16 bits)

	char *Data;  // +0x10C  // NAME 2: Set_Data's data
	int Length;  // +0x110  // NAME 2: Set_Data's length
	int Position;  // +0x114  // NAME 2: Get_Byte's read offset
	int Width;  // +0x118  // NAME 2: the screen descriptor's width (Create_Surface's; Decode's linewidth)
	int Height;  // +0x11C  // NAME 2: the screen descriptor's height
	int ColorCount;  // +0x120  // NAME 2: 1 << (bits + 1) of the global color table
	int Unknown124;  // +0x124
	unsigned char Unknown128;  // +0x128
	unsigned char Palette[768];  // +0x129  // NAME 2: the global color table, read 3 bytes per color
	int Colors[256];  // +0x42C  // NAME 2: Out_Line's pixel values (0x76D340 of each palette entry)
	int Transparent;  // +0x82C  // NAME 2: the graphic control extension's color index (-1 none)
	int CodeSize;  // +0x830  // NAME 2: Bennett's curr_size
	int BytesLeft;  // +0x834  // NAME 2: Bennett's navail_bytes
	int BitsLeft;  // +0x838  // NAME 2: Bennett's nbits_left
	unsigned char Block[257];  // +0x83C  // NAME 2: Bennett's byte_buff
	unsigned char *BlockPointer;  // +0x940  // NAME 2: Bennett's pbytes
	int BadCodes;  // +0x944  // NAME 2: Bennett's bad_code_count
	short Lengths[4096];  // +0x948  // NAME 1: the length of each code's string, less one
	unsigned short Prefix[4096];  // +0x2948  // NAME 2: Bennett's prefix
	unsigned char Suffix[10001];  // +0x4948  // NAME 2: Bennett's suffix (10001: Init clears 10000 and Stack follows at +0x7059)
	unsigned char Stack[4096];  // +0x7059  // NAME 2: Bennett's stack
	unsigned char Line[2049];  // +0x8059  // NAME 2: Bennett's buf (a width up to 0x800)
	char Unknown885A[0x22A04 - 0x885A];  // +0x885A  // the size from the parser's operator new (0x22A04)
};

#endif
