// Earth & Beyond client (reconstruction)
//
// unknown_jpeg.cpp
//
// E&B's baseline JPEG writer (client 0x9E3850-0x9E531E, between srandom.cpp and realcrc.cpp): no EA
// copy has it. WW3D::Make_Screen_Shot (0x9182C0) uses it for its 'jpg' format (0x918547 constructor,
// 0x918594 Write, 0x91859D destructor). Written from the client: three non-interleaved baseline scans
// (Y, Cb, Cr; no subsampling), a direct 8x8 DCT, E&B's own quantization tables and the standard
// Huffman tables. File PROVISIONAL (D137: no __FILE__ string); names describe the code (NAME 2).
//

#include "unknown_jpeg.h"
#include "ffactory.h"
#include "wwfile.h"
#include <stdlib.h>
#include <string.h>

#define JPEG_SWAP(x)	((((x) >> 8) & 0xFF) | (((x) & 0xFF) << 8))

#pragma pack(push, 1)

struct JPEGAPP0Header								// NAME 2: the JFIF APP0 segment (15 bytes as written)
{
	unsigned short	Length;
	char				Identifier[5];
	unsigned short	Version;
	unsigned char	Units;
	unsigned short	XDensity;
	unsigned short	YDensity;
	unsigned char	Thumbnail;
};

struct JPEGDQTHeader									// NAME 2: both quantization tables in one DQT segment
{
	unsigned short	Length;
	unsigned char	LuminanceInfo;
	unsigned char	Luminance[64];
	unsigned char	ChrominanceInfo;
	unsigned char	Chrominance[64];
};

struct JPEGSOFComponent								// NAME 2: one SOF0 component
{
	unsigned char	Id;
	unsigned char	Sampling;
	unsigned char	QuantTable;
};

struct JPEGSOFHeader									// NAME 2: the SOF0 segment
{
	unsigned short		Length;
	unsigned char		Precision;
	unsigned short		Height;
	unsigned short		Width;
	unsigned char		Components;
	JPEGSOFComponent	Component[3];
};

struct JPEGSOSHeader									// NAME 2: a one-component SOS segment
{
	unsigned short	Length;
	unsigned char	Components;
	unsigned char	Id;
	unsigned char	Tables;
	unsigned char	SpectralStart;
	unsigned char	SpectralEnd;
	unsigned char	Approximation;
};

struct JPEGDHTDCHeader								// NAME 2: a DC DHT segment
{
	unsigned short	Length;
	unsigned char	Info;
	unsigned char	Bits[16];
	unsigned char	Values[12];
};

struct JPEGDHTACHeader								// NAME 2: an AC DHT segment
{
	unsigned short	Length;
	unsigned char	Info;
	unsigned char	Bits[16];
	unsigned char	Values[162];
};

#pragma pack(pop)

struct JPEGHuffmanCode								// NAME 2: one Huffman code (run/size, length, code)
{
	unsigned char	Symbol;
	unsigned char	Length;
	unsigned short	Code;
};

// 0xB14C4C: Cos[u][x] = cos((2x+1)u pi/16)
static const float JPEGCos[8][8] = {
	{ 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f },
	{ 0.9807850122451782f, 0.8314700126647949f, 0.5555700063705444f, 0.19508999586105347f, -0.19508999586105347f, -0.5555700063705444f, -0.8314700126647949f, -0.9807850122451782f },
	{ 0.9238799810409546f, 0.38268300890922546f, -0.38268300890922546f, -0.9238799810409546f, -0.9238799810409546f, -0.38268300890922546f, 0.38268399238586426f, 0.9238799810409546f },
	{ 0.8314700126647949f, -0.19508999586105347f, -0.9807850122451782f, -0.5555700063705444f, 0.5555700063705444f, 0.9807850122451782f, 0.19508999586105347f, -0.8314700126647949f },
	{ 0.7071070075035095f, -0.7071070075035095f, -0.7071070075035095f, 0.7071070075035095f, 0.7071070075035095f, -0.7071070075035095f, -0.7071070075035095f, 0.7071070075035095f },
	{ 0.5555700063705444f, -0.9807850122451782f, 0.19508999586105347f, 0.8314700126647949f, -0.8314700126647949f, -0.19508999586105347f, 0.9807850122451782f, -0.5555710196495056f },
	{ 0.38268300890922546f, -0.9238799810409546f, 0.9238799810409546f, -0.38268399238586426f, -0.38268300890922546f, 0.9238790273666382f, -0.9238799810409546f, 0.38268399238586426f },
	{ 0.19508999586105347f, -0.5555700063705444f, 0.8314700126647949f, -0.9807850122451782f, 0.9807850122451782f, -0.8314700126647949f, 0.5555710196495056f, -0.19509099423885345f },
};

// 0xB14D4C
static const unsigned char JPEGLuminanceQuant[64] = {
	2, 1, 1, 2, 3, 5, 6, 7, 1, 1, 1, 2, 3, 7, 7, 6,
	1, 1, 2, 3, 5, 7, 8, 7, 1, 2, 2, 3, 6, 10, 10, 7,
	2, 2, 4, 7, 8, 13, 12, 9, 3, 4, 6, 8, 10, 13, 14, 11,
	6, 8, 9, 10, 12, 15, 15, 12, 9, 11, 11, 12, 14, 12, 12, 12,
};

// 0xB14D8C
static const unsigned char JPEGChrominanceQuant[64] = {
	2, 2, 3, 5, 12, 12, 12, 12, 2, 2, 3, 8, 12, 12, 12, 12,
	3, 3, 7, 12, 12, 12, 12, 12, 5, 8, 12, 12, 12, 12, 12, 12,
	12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12,
	12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12,
};

// 0xB14DCC
static const int JPEGZigZag[64] = {
	0, 1, 5, 6, 14, 15, 27, 28, 2, 4, 7, 13, 16, 26, 29, 42,
	3, 8, 12, 17, 25, 30, 41, 43, 9, 11, 18, 24, 31, 40, 44, 53,
	10, 19, 23, 32, 39, 45, 52, 54, 20, 22, 33, 38, 46, 51, 55, 60,
	21, 34, 37, 47, 50, 56, 59, 61, 35, 36, 48, 49, 57, 58, 62, 63,
};

// 0xB14ECC: {run/size, length, code}
static const JPEGHuffmanCode JPEGLuminanceAC[162] = {
	{ 0x00, 4, 0x000A }, { 0xF0, 11, 0x07F9 }, { 0x01, 2, 0x0000 }, { 0x02, 2, 0x0001 }, { 0x03, 3, 0x0004 }, { 0x04, 4, 0x000B },
	{ 0x05, 5, 0x001A }, { 0x06, 7, 0x0078 }, { 0x07, 8, 0x00F8 }, { 0x08, 10, 0x03F6 }, { 0x09, 16, 0xFF82 }, { 0x0A, 16, 0xFF83 },
	{ 0x11, 4, 0x000C }, { 0x12, 5, 0x001B }, { 0x13, 7, 0x0079 }, { 0x14, 9, 0x01F6 }, { 0x15, 11, 0x07F6 }, { 0x16, 16, 0xFF84 },
	{ 0x17, 16, 0xFF85 }, { 0x18, 16, 0xFF86 }, { 0x19, 16, 0xFF87 }, { 0x1A, 16, 0xFF88 }, { 0x21, 5, 0x001C }, { 0x22, 8, 0x00F9 },
	{ 0x23, 10, 0x03F7 }, { 0x24, 12, 0x0FF4 }, { 0x25, 16, 0xFF89 }, { 0x26, 16, 0xFF8A }, { 0x27, 16, 0xFF8B }, { 0x28, 16, 0xFF8C },
	{ 0x29, 16, 0xFF8D }, { 0x2A, 16, 0xFF8E }, { 0x31, 6, 0x003A }, { 0x32, 9, 0x01F7 }, { 0x33, 12, 0x0FF5 }, { 0x34, 16, 0xFF8F },
	{ 0x35, 16, 0xFF90 }, { 0x36, 16, 0xFF91 }, { 0x37, 16, 0xFF92 }, { 0x38, 16, 0xFF93 }, { 0x39, 16, 0xFF94 }, { 0x3A, 16, 0xFF95 },
	{ 0x41, 6, 0x003B }, { 0x42, 10, 0x03F8 }, { 0x43, 16, 0xFF96 }, { 0x44, 16, 0xFF97 }, { 0x45, 16, 0xFF98 }, { 0x46, 16, 0xFF99 },
	{ 0x47, 16, 0xFF9A }, { 0x48, 16, 0xFF9B }, { 0x49, 16, 0xFF9C }, { 0x4A, 16, 0xFF9D }, { 0x51, 7, 0x007A }, { 0x52, 11, 0x07F7 },
	{ 0x53, 16, 0xFF9E }, { 0x54, 16, 0xFF9F }, { 0x55, 16, 0xFFA0 }, { 0x56, 16, 0xFFA1 }, { 0x57, 16, 0xFFA2 }, { 0x58, 16, 0xFFA3 },
	{ 0x59, 16, 0xFFA4 }, { 0x5A, 16, 0xFFA5 }, { 0x61, 7, 0x007B }, { 0x62, 12, 0x0FF6 }, { 0x63, 16, 0xFFA6 }, { 0x64, 16, 0xFFA7 },
	{ 0x65, 16, 0xFFA8 }, { 0x66, 16, 0xFFA9 }, { 0x67, 16, 0xFFAA }, { 0x68, 16, 0xFFAB }, { 0x69, 16, 0xFFAC }, { 0x6A, 16, 0xFFAD },
	{ 0x71, 8, 0x00FA }, { 0x72, 12, 0x0FF7 }, { 0x73, 16, 0xFFAE }, { 0x74, 16, 0xFFAF }, { 0x75, 16, 0xFFB0 }, { 0x76, 16, 0xFFB1 },
	{ 0x77, 16, 0xFFB2 }, { 0x78, 16, 0xFFB3 }, { 0x79, 16, 0xFFB4 }, { 0x7A, 16, 0xFFB5 }, { 0x81, 9, 0x01F8 }, { 0x82, 15, 0x7FC0 },
	{ 0x83, 16, 0xFFB6 }, { 0x84, 16, 0xFFB7 }, { 0x85, 16, 0xFFB8 }, { 0x86, 16, 0xFFB9 }, { 0x87, 16, 0xFFBA }, { 0x88, 16, 0xFFBB },
	{ 0x89, 16, 0xFFBC }, { 0x8A, 16, 0xFFBD }, { 0x91, 9, 0x01F9 }, { 0x92, 16, 0xFFBE }, { 0x93, 16, 0xFFBF }, { 0x94, 16, 0xFFC0 },
	{ 0x95, 16, 0xFFC1 }, { 0x96, 16, 0xFFC2 }, { 0x97, 16, 0xFFC3 }, { 0x98, 16, 0xFFC4 }, { 0x99, 16, 0xFFC5 }, { 0x9A, 16, 0xFFC6 },
	{ 0xA1, 9, 0x01FA }, { 0xA2, 16, 0xFFC7 }, { 0xA3, 16, 0xFFC8 }, { 0xA4, 16, 0xFFC9 }, { 0xA5, 16, 0xFFCA }, { 0xA6, 16, 0xFFCB },
	{ 0xA7, 16, 0xFFCC }, { 0xA8, 16, 0xFFCD }, { 0xA9, 16, 0xFFCE }, { 0xAA, 16, 0xFFCF }, { 0xB1, 10, 0x03F9 }, { 0xB2, 16, 0xFFD0 },
	{ 0xB3, 16, 0xFFD1 }, { 0xB4, 16, 0xFFD2 }, { 0xB5, 16, 0xFFD3 }, { 0xB6, 16, 0xFFD4 }, { 0xB7, 16, 0xFFD5 }, { 0xB8, 16, 0xFFD6 },
	{ 0xB9, 16, 0xFFD7 }, { 0xBA, 16, 0xFFD8 }, { 0xC1, 10, 0x03FA }, { 0xC2, 16, 0xFFD9 }, { 0xC3, 16, 0xFFDA }, { 0xC4, 16, 0xFFDB },
	{ 0xC5, 16, 0xFFDC }, { 0xC6, 16, 0xFFDD }, { 0xC7, 16, 0xFFDE }, { 0xC8, 16, 0xFFDF }, { 0xC9, 16, 0xFFE0 }, { 0xCA, 16, 0xFFE1 },
	{ 0xD1, 11, 0x07F8 }, { 0xD2, 16, 0xFFE2 }, { 0xD3, 16, 0xFFE3 }, { 0xD4, 16, 0xFFE4 }, { 0xD5, 16, 0xFFE5 }, { 0xD6, 16, 0xFFE6 },
	{ 0xD7, 16, 0xFFE7 }, { 0xD8, 16, 0xFFE8 }, { 0xD9, 16, 0xFFE9 }, { 0xDA, 16, 0xFFEA }, { 0xE1, 16, 0xFFEB }, { 0xE2, 16, 0xFFEC },
	{ 0xE3, 16, 0xFFED }, { 0xE4, 16, 0xFFEE }, { 0xE5, 16, 0xFFEF }, { 0xE6, 16, 0xFFF0 }, { 0xE7, 16, 0xFFF1 }, { 0xE8, 16, 0xFFF2 },
	{ 0xE9, 16, 0xFFF3 }, { 0xEA, 16, 0xFFF4 }, { 0xF1, 16, 0xFFF5 }, { 0xF2, 16, 0xFFF6 }, { 0xF3, 16, 0xFFF7 }, { 0xF4, 16, 0xFFF8 },
	{ 0xF5, 16, 0xFFF9 }, { 0xF6, 16, 0xFFFA }, { 0xF7, 16, 0xFFFB }, { 0xF8, 16, 0xFFFC }, { 0xF9, 16, 0xFFFD }, { 0xFA, 16, 0xFFFE },
};

// 0xB15154: {run/size, length, code}
static const JPEGHuffmanCode JPEGChrominanceAC[162] = {
	{ 0x00, 2, 0x0000 }, { 0xF0, 10, 0x03FA }, { 0x01, 2, 0x0001 }, { 0x02, 3, 0x0004 }, { 0x03, 4, 0x000A }, { 0x04, 5, 0x0018 },
	{ 0x05, 5, 0x0019 }, { 0x06, 6, 0x0038 }, { 0x07, 7, 0x0078 }, { 0x08, 9, 0x01F4 }, { 0x09, 10, 0x03F6 }, { 0x0A, 12, 0x0FF4 },
	{ 0x11, 4, 0x000B }, { 0x12, 6, 0x0039 }, { 0x13, 8, 0x00F6 }, { 0x14, 9, 0x01F5 }, { 0x15, 11, 0x07F6 }, { 0x16, 12, 0x0FF5 },
	{ 0x17, 16, 0xFF88 }, { 0x18, 16, 0xFF89 }, { 0x19, 16, 0xFF8A }, { 0x1A, 16, 0xFF8B }, { 0x21, 5, 0x001A }, { 0x22, 8, 0x00F7 },
	{ 0x23, 10, 0x03F7 }, { 0x24, 12, 0x0FF6 }, { 0x25, 15, 0x7FC2 }, { 0x26, 16, 0xFF8C }, { 0x27, 16, 0xFF8D }, { 0x28, 16, 0xFF8E },
	{ 0x29, 16, 0xFF8F }, { 0x2A, 16, 0xFF90 }, { 0x31, 5, 0x001B }, { 0x32, 8, 0x00F8 }, { 0x33, 10, 0x03F8 }, { 0x34, 12, 0x0FF7 },
	{ 0x35, 16, 0xFF91 }, { 0x36, 16, 0xFF92 }, { 0x37, 16, 0xFF93 }, { 0x38, 16, 0xFF94 }, { 0x39, 16, 0xFF95 }, { 0x3A, 16, 0xFF96 },
	{ 0x41, 6, 0x003A }, { 0x42, 9, 0x01F6 }, { 0x43, 16, 0xFF97 }, { 0x44, 16, 0xFF98 }, { 0x45, 16, 0xFF99 }, { 0x46, 16, 0xFF9A },
	{ 0x47, 16, 0xFF9B }, { 0x48, 16, 0xFF9C }, { 0x49, 16, 0xFF9D }, { 0x4A, 16, 0xFF9E }, { 0x51, 6, 0x003B }, { 0x52, 10, 0x03F9 },
	{ 0x53, 16, 0xFF9F }, { 0x54, 16, 0xFFA0 }, { 0x55, 16, 0xFFA1 }, { 0x56, 16, 0xFFA2 }, { 0x57, 16, 0xFFA3 }, { 0x58, 16, 0xFFA4 },
	{ 0x59, 16, 0xFFA5 }, { 0x5A, 16, 0xFFA6 }, { 0x61, 7, 0x0079 }, { 0x62, 11, 0x07F7 }, { 0x63, 16, 0xFFA7 }, { 0x64, 16, 0xFFA8 },
	{ 0x65, 16, 0xFFA9 }, { 0x66, 16, 0xFFAA }, { 0x67, 16, 0xFFAB }, { 0x68, 16, 0xFFAC }, { 0x69, 16, 0xFFAD }, { 0x6A, 16, 0xFFAE },
	{ 0x71, 7, 0x007A }, { 0x72, 11, 0x07F8 }, { 0x73, 16, 0xFFAF }, { 0x74, 16, 0xFFB0 }, { 0x75, 16, 0xFFB1 }, { 0x76, 16, 0xFFB2 },
	{ 0x77, 16, 0xFFB3 }, { 0x78, 16, 0xFFB4 }, { 0x79, 16, 0xFFB5 }, { 0x7A, 16, 0xFFB6 }, { 0x81, 8, 0x00F9 }, { 0x82, 16, 0xFFB7 },
	{ 0x83, 16, 0xFFB8 }, { 0x84, 16, 0xFFB9 }, { 0x85, 16, 0xFFBA }, { 0x86, 16, 0xFFBB }, { 0x87, 16, 0xFFBC }, { 0x88, 16, 0xFFBD },
	{ 0x89, 16, 0xFFBE }, { 0x8A, 16, 0xFFBF }, { 0x91, 9, 0x01F7 }, { 0x92, 16, 0xFFC0 }, { 0x93, 16, 0xFFC1 }, { 0x94, 16, 0xFFC2 },
	{ 0x95, 16, 0xFFC3 }, { 0x96, 16, 0xFFC4 }, { 0x97, 16, 0xFFC5 }, { 0x98, 16, 0xFFC6 }, { 0x99, 16, 0xFFC7 }, { 0x9A, 16, 0xFFC8 },
	{ 0xA1, 9, 0x01F8 }, { 0xA2, 16, 0xFFC9 }, { 0xA3, 16, 0xFFCA }, { 0xA4, 16, 0xFFCB }, { 0xA5, 16, 0xFFCC }, { 0xA6, 16, 0xFFCD },
	{ 0xA7, 16, 0xFFCE }, { 0xA8, 16, 0xFFCF }, { 0xA9, 16, 0xFFD0 }, { 0xAA, 16, 0xFFD1 }, { 0xB1, 9, 0x01F9 }, { 0xB2, 16, 0xFFD2 },
	{ 0xB3, 16, 0xFFD3 }, { 0xB4, 16, 0xFFD4 }, { 0xB5, 16, 0xFFD5 }, { 0xB6, 16, 0xFFD6 }, { 0xB7, 16, 0xFFD7 }, { 0xB8, 16, 0xFFD8 },
	{ 0xB9, 16, 0xFFD9 }, { 0xBA, 16, 0xFFDA }, { 0xC1, 9, 0x01FA }, { 0xC2, 16, 0xFFDB }, { 0xC3, 16, 0xFFDC }, { 0xC4, 16, 0xFFDD },
	{ 0xC5, 16, 0xFFDE }, { 0xC6, 16, 0xFFDF }, { 0xC7, 16, 0xFFE0 }, { 0xC8, 16, 0xFFE1 }, { 0xC9, 16, 0xFFE2 }, { 0xCA, 16, 0xFFE3 },
	{ 0xD1, 11, 0x07F9 }, { 0xD2, 16, 0xFFE4 }, { 0xD3, 16, 0xFFE5 }, { 0xD4, 16, 0xFFE6 }, { 0xD5, 16, 0xFFE7 }, { 0xD6, 16, 0xFFE8 },
	{ 0xD7, 16, 0xFFE9 }, { 0xD8, 16, 0xFFEA }, { 0xD9, 16, 0xFFEB }, { 0xDA, 16, 0xFFEC }, { 0xE1, 14, 0x3FE0 }, { 0xE2, 16, 0xFFED },
	{ 0xE3, 16, 0xFFEE }, { 0xE4, 16, 0xFFEF }, { 0xE5, 16, 0xFFF0 }, { 0xE6, 16, 0xFFF1 }, { 0xE7, 16, 0xFFF2 }, { 0xE8, 16, 0xFFF3 },
	{ 0xE9, 16, 0xFFF4 }, { 0xEA, 16, 0xFFF5 }, { 0xF1, 15, 0x7FC3 }, { 0xF2, 16, 0xFFF6 }, { 0xF3, 16, 0xFFF7 }, { 0xF4, 16, 0xFFF8 },
	{ 0xF5, 16, 0xFFF9 }, { 0xF6, 16, 0xFFFA }, { 0xF7, 16, 0xFFFB }, { 0xF8, 16, 0xFFFC }, { 0xF9, 16, 0xFFFD }, { 0xFA, 16, 0xFFFE },
};

// 0xB153DC: {run/size, length, code}
static const JPEGHuffmanCode JPEGLuminanceDC[12] = {
	{ 0x00, 2, 0x0000 }, { 0x01, 3, 0x0002 }, { 0x02, 3, 0x0003 }, { 0x03, 3, 0x0004 }, { 0x04, 3, 0x0005 }, { 0x05, 3, 0x0006 },
	{ 0x06, 4, 0x000E }, { 0x07, 5, 0x001E }, { 0x08, 6, 0x003E }, { 0x09, 7, 0x007E }, { 0x0A, 8, 0x00FE }, { 0x0B, 9, 0x01FE },
};

// 0xB1540C: {run/size, length, code}
static const JPEGHuffmanCode JPEGChrominanceDC[12] = {
	{ 0x00, 2, 0x0000 }, { 0x01, 2, 0x0001 }, { 0x02, 2, 0x0002 }, { 0x03, 3, 0x0006 }, { 0x04, 4, 0x000E }, { 0x05, 5, 0x001E },
	{ 0x06, 6, 0x003E }, { 0x07, 7, 0x007E }, { 0x08, 8, 0x00FE }, { 0x09, 9, 0x01FE }, { 0x0A, 10, 0x03FE }, { 0x0B, 11, 0x07FE },
};


// The quantization and Huffman tables of the three components (Y, Cb, Cr)
static const unsigned char * JPEGQuantTables[3] = { JPEGLuminanceQuant, JPEGChrominanceQuant, JPEGChrominanceQuant };
static const JPEGHuffmanCode * JPEGACTables[3] = { JPEGLuminanceAC, JPEGChrominanceAC, JPEGChrominanceAC };
static const JPEGHuffmanCode * JPEGDCTables[3] = { JPEGLuminanceDC, JPEGChrominanceDC, JPEGChrominanceDC };
static char JPEGIdentifier[] = "JFIF";


JPEGWriterClass::JPEGWriterClass(void)
{
}


JPEGWriterClass::~JPEGWriterClass(void)
{
}


// NOT EXACT: inline budget (the client inlines Write_SOF here and calls the third SOS's Write_Marker out of line, ours the reverse; its frame is 0x14 bigger; attempts.py show 0x9E3870)
bool JPEGWriterClass::Write(const char * filename, const unsigned char * pixels, int width, int height, int pitch)
{
	width = width / 8 * 8;
	height = height / 8 * 8;

	File = _TheWritingFileFactory->Get_File(filename);
	if (File == NULL) {
		return false;
	}
	File->Open(FileClass::WRITE);

	Write_Marker(0xFFD8);
	Write_Marker(0xFFE0);

	JPEGAPP0Header app0;
	app0.Length = JPEG_SWAP(15);
	strcpy(app0.Identifier, JPEGIdentifier);
	app0.Version = 0x0101;
	app0.Units = 1;
	app0.XDensity = JPEG_SWAP(96);
	app0.YDensity = JPEG_SWAP(96);
	app0.Thumbnail = 0;
	File->Write(&app0, sizeof(app0));

	Write_DQT();
	Write_DHT_AC();
	Write_DHT_DC();
	Write_SOF(width, height);

	Write_SOS(1, 0x00);
	Encode_Component(0, pixels, width, height, pitch);
	Write_SOS(2, 0x11);
	Encode_Component(1, pixels, width, height, pitch);
	Write_SOS(3, 0x11);
	Encode_Component(2, pixels, width, height, pitch);

	Write_Marker(0xFFD9);
	File->Close();
	_TheWritingFileFactory->Return_File(File);
	File = NULL;
	return true;
}


// NOT EXACT: inline budget (the client calls Convert_Color and Get_DC_Code, ours inlines them; last_dc's slot differs; attempts.py show 0x9E3BD0)
void JPEGWriterClass::Encode_Component(int component, const unsigned char * pixels, int width, int height, int pitch)
{
	int blocks_x = width / 8 + 1;
	int blocks_y = height / 8 + 1;
	int last_dc = 0;
	JPEGBitBufferClass bits;

	for (int by = 0; by < blocks_y - 1; by++) {
		for (int bx = 0; bx < blocks_x - 1; bx++) {
			unsigned char rgb[8 * 8 * 3];
			signed char ycc[8 * 8 * 3];
			short coefficients[64];
			short zigzag[64];

			const unsigned char * src = pixels + by * 8 * pitch + bx * 8 * 3;
			for (int row = 0; row < 8; row++) {
				memcpy(&rgb[row * 8 * 3], src, 8 * 3);
				src += pitch;
			}

			Convert_Color(rgb, ycc);
			Forward_DCT(component, ycc, coefficients);
			Quantize(component, coefficients);
			Zig_Zag(coefficients, zigzag);

			short diff = zigzag[0] - last_dc;
			unsigned char size = (unsigned char)(Bit_Count((short)abs(diff)) + 1);
			unsigned char length = 0;
			last_dc = 0;
			Get_DC_Code(component, size, &length, (unsigned short *)&last_dc);
			bits.Write_Bits(last_dc, length);
			int value = diff;
			if (diff < 0) {
				value += (1 << size) - 1;
			}
			bits.Write_Bits(value, size);
			last_dc = zigzag[0];

			Encode_AC(component, zigzag, &bits);
		}
	}

	bits.Flush();
	File->Write(bits.Get_Data(), bits.Length());
}


void JPEGWriterClass::Convert_Color(const unsigned char * rgb, signed char * ycc)
{
	int j = 0;
	for (int i = 0; i < 64; i++, j += 3) {
		unsigned char r = rgb[j];
		unsigned char g = rgb[j + 1];
		unsigned char b = rgb[j + 2];
		ycc[j] = (signed char)((r * 0.299f + g * 0.587f + b * 0.114f) - 128.0f);
		ycc[j + 1] = (signed char)((r * -0.1687f - g * 0.3313f) + b * 0.5f);
		ycc[j + 2] = (signed char)((r * 0.5f - g * 0.4187f) - b * 0.0813f);
	}
}


void JPEGWriterClass::Forward_DCT(int component, const signed char * ycc, short * out)
{
	int n = 0;
	for (int v = 0; v < 8; v++) {
		for (int u = 0; u < 8; u++) {
			float scale = 0.25f;
			if (u == 0) {
				scale = 0.17677669f;
			}
			if (v == 0) {
				scale *= 0.70710677f;
			}
			float sum = 0.0f;
			int index = component;
			for (int y = 0; y < 8; y++) {
				for (int x = 0; x < 8; x++) {
					sum += ycc[index] * JPEGCos[u][x] * JPEGCos[v][y];
					index += 3;
				}
			}
			out[n++] = (short)(sum * scale);
		}
	}
}


void JPEGWriterClass::Quantize(int component, short * data)
{
	for (int i = 0; i < 64; i++) {
		float d = data[i];
		float q = JPEGQuantTables[component][i];
		data[i] = (short)(d / q + 0.5f);
	}
}


void JPEGWriterClass::Zig_Zag(const short * in, short * out)
{
	for (int i = 0; i < 64; i++) {
		out[JPEGZigZag[i]] = in[i];
	}
}


void JPEGWriterClass::Encode_DC(int component, short last_dc, const short * block, JPEGBitBufferClass * bits)
{
	short diff = block[0] - last_dc;
	unsigned char size = (unsigned char)(Bit_Count((short)abs(diff)) + 1);
	bits->Write_Bits(JPEGDCTables[component][size].Code, JPEGDCTables[component][size].Length);
	int value = diff;
	if (diff < 0) {
		value += (1 << size) - 1;
	}
	bits->Write_Bits(value, size);
}


void JPEGWriterClass::Encode_AC(int component, const short * block, JPEGBitBufferClass * bits)
{
	unsigned char run = 0;
	for (int i = 1; i < 64; i++) {
		short v = block[i];
		if (v == 0 && run != 15) {
			run++;
		} else {
			unsigned char size = (unsigned char)(Bit_Count((short)abs(v)) + 1);
			int index = run * 10 + size + 1;
			if (size == 0) {
				index = (run != 0);
			}
			bits->Write_Bits(JPEGACTables[component][index].Code, JPEGACTables[component][index].Length);
			int value = v;
			if (v < 0) {
				value += (1 << size) - 1;
			}
			bits->Write_Bits(value, size);
			run = 0;
		}
	}
	if (run > 0) {
		bits->Write_Bits(JPEGACTables[component][0].Code, JPEGACTables[component][0].Length);
	}
}


short JPEGWriterClass::Bit_Count(short value)
{
	short i;
	for (i = 11; i >= 0; i--) {
		if (value & (1 << i)) {
			break;
		}
	}
	return i;
}


void JPEGWriterClass::Get_AC_Code(int component, unsigned char run, unsigned char size, unsigned char * length, unsigned short * code)
{
	int index = run * 10 + size + 1;
	if (size == 0) {
		index = (run != 0);
	}
	*length = JPEGACTables[component][index].Length;
	*code = JPEGACTables[component][index].Code;
}


void JPEGWriterClass::Get_DC_Code(int component, unsigned char size, unsigned char * length, unsigned short * code)
{
	*length = JPEGDCTables[component][size].Length;
	*code = JPEGDCTables[component][size].Code;
}


void JPEGWriterClass::Write_DHT_DC(void)
{
	JPEGDHTDCHeader dht;
	unsigned char bits[16] = { 0, 1, 5, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0 };
	unsigned char values[12] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };
	dht.Length = JPEG_SWAP(31);
	dht.Info = 0x00;
	memcpy(dht.Bits, bits, sizeof(bits));
	memcpy(dht.Values, values, sizeof(values));
	Write_Marker(0xFFC4);
	File->Write(&dht, sizeof(dht));

	bits[0] = 0; bits[1] = 3; bits[2] = 1; bits[3] = 1; bits[4] = 1; bits[5] = 1; bits[6] = 1; bits[7] = 1;
	bits[8] = 1; bits[9] = 1; bits[10] = 1; bits[11] = 0; bits[12] = 0; bits[13] = 0; bits[14] = 0; bits[15] = 0;
	values[0] = 0; values[1] = 1; values[2] = 2; values[3] = 3; values[4] = 4; values[5] = 5;
	values[6] = 6; values[7] = 7; values[8] = 8; values[9] = 9; values[10] = 10; values[11] = 11;
	dht.Info = 0x01;
	memcpy(dht.Bits, bits, sizeof(bits));
	memcpy(dht.Values, values, sizeof(values));
	Write_Marker(0xFFC4);
	File->Write(&dht, sizeof(dht));
}


void JPEGWriterClass::Write_DHT_AC(void)
{
	JPEGDHTACHeader dht;
	dht.Length = JPEG_SWAP(181);
	unsigned char bits[16] = { 0, 2, 1, 3, 3, 2, 4, 3, 5, 5, 4, 4, 0, 0, 1, 0x7D };
	unsigned char values[162] = {
		0x01, 0x02, 0x03, 0x00, 0x04, 0x11, 0x05, 0x12, 0x21, 0x31, 0x41, 0x06, 0x13, 0x51, 0x61, 0x07,
		0x22, 0x71, 0x14, 0x32, 0x81, 0x91, 0xA1, 0x08, 0x23, 0x42, 0xB1, 0xC1, 0x15, 0x52, 0xD1, 0xF0,
		0x24, 0x33, 0x62, 0x72, 0x82, 0x09, 0x0A, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x25, 0x26, 0x27, 0x28,
		0x29, 0x2A, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49,
		0x4A, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5A, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69,
		0x6A, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7A, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89,
		0x8A, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9A, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6, 0xA7,
		0xA8, 0xA9, 0xAA, 0xB2, 0xB3, 0xB4, 0xB5, 0xB6, 0xB7, 0xB8, 0xB9, 0xBA, 0xC2, 0xC3, 0xC4, 0xC5,
		0xC6, 0xC7, 0xC8, 0xC9, 0xCA, 0xD2, 0xD3, 0xD4, 0xD5, 0xD6, 0xD7, 0xD8, 0xD9, 0xDA, 0xE1, 0xE2,
		0xE3, 0xE4, 0xE5, 0xE6, 0xE7, 0xE8, 0xE9, 0xEA, 0xF1, 0xF2, 0xF3, 0xF4, 0xF5, 0xF6, 0xF7, 0xF8,
		0xF9, 0xFA
	};
	dht.Info = 0x10;
	memcpy(dht.Bits, bits, sizeof(bits));
	memcpy(dht.Values, values, sizeof(values));
	Write_Marker(0xFFC4);
	File->Write(&dht, sizeof(dht));

	unsigned char chroma_bits[16] = { 0, 2, 1, 2, 4, 4, 3, 4, 7, 5, 4, 4, 0, 1, 2, 0x77 };
	unsigned char chroma_values[162] = {
		0x00, 0x01, 0x02, 0x03, 0x11, 0x04, 0x05, 0x21, 0x31, 0x06, 0x12, 0x41, 0x51, 0x07, 0x61, 0x71,
		0x13, 0x22, 0x32, 0x81, 0x08, 0x14, 0x42, 0x91, 0xA1, 0xB1, 0xC1, 0x09, 0x23, 0x33, 0x52, 0xF0,
		0x15, 0x62, 0x72, 0xD1, 0x0A, 0x16, 0x24, 0x34, 0xE1, 0x25, 0xF1, 0x17, 0x18, 0x19, 0x1A, 0x26,
		0x27, 0x28, 0x29, 0x2A, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48,
		0x49, 0x4A, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5A, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68,
		0x69, 0x6A, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7A, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87,
		0x88, 0x89, 0x8A, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9A, 0xA2, 0xA3, 0xA4, 0xA5,
		0xA6, 0xA7, 0xA8, 0xA9, 0xAA, 0xB2, 0xB3, 0xB4, 0xB5, 0xB6, 0xB7, 0xB8, 0xB9, 0xBA, 0xC2, 0xC3,
		0xC4, 0xC5, 0xC6, 0xC7, 0xC8, 0xC9, 0xCA, 0xD2, 0xD3, 0xD4, 0xD5, 0xD6, 0xD7, 0xD8, 0xD9, 0xDA,
		0xE2, 0xE3, 0xE4, 0xE5, 0xE6, 0xE7, 0xE8, 0xE9, 0xEA, 0xF2, 0xF3, 0xF4, 0xF5, 0xF6, 0xF7, 0xF8,
		0xF9, 0xFA
	};
	dht.Info = 0x11;
	memcpy(dht.Bits, chroma_bits, sizeof(chroma_bits));
	memcpy(dht.Values, chroma_values, sizeof(chroma_values));
	Write_Marker(0xFFC4);
	File->Write(&dht, sizeof(dht));
}


void JPEGWriterClass::Write_Marker(unsigned short marker)
{
	unsigned short value = (marker >> 8) | (marker << 8);
	File->Write(&value, 2);
}


void JPEGWriterClass::Write_SOF(int width, int height)
{
	Write_Marker(0xFFC0);
	JPEGSOFHeader sof;
	sof.Length = JPEG_SWAP(17);
	sof.Precision = 8;
	sof.Width = JPEG_SWAP(width);
	sof.Height = JPEG_SWAP(height);
	sof.Components = 3;
	sof.Component[0].Id = 1;
	sof.Component[0].Sampling = 0x11;
	sof.Component[0].QuantTable = 0;
	sof.Component[1].Id = 2;
	sof.Component[1].Sampling = 0x11;
	sof.Component[1].QuantTable = 1;
	sof.Component[2].Id = 3;
	sof.Component[2].Sampling = 0x11;
	sof.Component[2].QuantTable = 1;
	File->Write(&sof, sizeof(sof));
}


void JPEGWriterClass::Write_SOS(unsigned char id, unsigned char tables)
{
	Write_Marker(0xFFDA);
	JPEGSOSHeader sos;
	sos.Length = JPEG_SWAP(8);
	sos.Components = 1;
	sos.Id = id;
	sos.Tables = tables;
	sos.SpectralStart = 0;
	sos.SpectralEnd = 63;
	sos.Approximation = 0;
	File->Write(&sos, sizeof(sos));
}


void JPEGWriterClass::Write_DQT(void)
{
	JPEGDQTHeader dqt;
	dqt.Length = JPEG_SWAP(132);
	dqt.LuminanceInfo = 0;
	dqt.ChrominanceInfo = 1;
	for (int i = 0; i < 64; i++) {
		dqt.Luminance[JPEGZigZag[i]] = JPEGLuminanceQuant[i];
		dqt.Chrominance[JPEGZigZag[i]] = JPEGChrominanceQuant[i];
	}
	Write_Marker(0xFFDB);
	File->Write(&dqt, sizeof(dqt));
}


JPEGBitBufferClass::JPEGBitBufferClass(void) :
	Position(0),
	BitsLeft(8),
	Data(NULL),
	Size(0)
{
	Grow(0x1000);
	Data[Position] = 0;
}


JPEGBitBufferClass::~JPEGBitBufferClass(void)
{
	if (Data != NULL) {
		delete Data;
	}
}


void JPEGBitBufferClass::Grow(int bytes)
{
	int size = bytes + Size;
	unsigned char * data = new unsigned char[size];
	memcpy(data, Data, Size);
	if (Data != NULL) {
		delete Data;
	}
	Data = data;
	Size = size;
}


void JPEGBitBufferClass::Write_Bits(short value, int count)
{
	for (;;) {
		if (Position + 2 >= Size) {
			Grow(0x1000);
		}
		if (count <= BitsLeft) {
			break;
		}
		int shift = count - BitsLeft;
		Data[Position] |= (((1 << BitsLeft) - 1) << shift & value) >> shift;
		count -= BitsLeft;
		Next_Byte();
		value &= (1 << count) - 1;
	}
	Data[Position] |= ((unsigned char)((1 << count) - 1) & (unsigned char)value) << (BitsLeft - count);
	BitsLeft -= count;
	if (BitsLeft == 0) {
		Next_Byte();
	}
}


void JPEGBitBufferClass::Flush(void)
{
	if (BitsLeft > 0) {
		Data[Position] |= (unsigned char)((1 << BitsLeft) - 1);
		if (Data[Position] == 0xFF) {
			Position++;
			Data[Position] = 0;
			BitsLeft = 0;
		}
	}
}


int JPEGBitBufferClass::Length(void)
{
	int length = Position + 1;
	if (BitsLeft == 8) {
		length--;
	}
	return length;
}

// Earth & Beyond client (reconstruction): unknown_jpeg.cpp (frozen 2026-11-07 from importer.py)
// EnB: written from the client (RECONSTRUCTED): E&B's baseline JPEG writer (client 0x9E3850-0x9E531E, between
//       srandom.cpp and realcrc.cpp; WW3D::Make_Screen_Shot's 'jpg' format), written from the client: three
//       one-component scans, a direct 8x8 DCT, E&B's quantization tables, the standard Huffman tables; no EA copy has
//       it. File PROVISIONAL (D137: no __FILE__)
