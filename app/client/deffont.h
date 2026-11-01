// Earth & Beyond client (reconstruction): app\client\deffont.h (PROVISIONAL file name: the client names only
// deffont.cpp). deffont.cpp's built-in font, the font files of styles 1..4, fontpal.pcx's palette and remaps.
#ifndef DEFFONT_H
#define DEFFONT_H

class ConvertClass;
class PaletteClass;
class WWFontClass;

enum {
	STYLE_COUNT = 5,																// NAME 1 [STYLE_COUNT]: FontData's 5 entries (0x683940's loop)
	REMAP_COUNT = 27																// NAME 1 [REMAP_COUNT]: Remaps' 27 entries (0x683940's loop)
};

extern unsigned char DefaultFontData[0x16F2];
extern char const * FontNames[STYLE_COUNT];
extern void * FontData[STYLE_COUNT];
extern unsigned char * Remaps[REMAP_COUNT];
extern int RemapSizes[REMAP_COUNT];
extern ConvertClass * Converter;
extern PaletteClass Palette;
extern WWFontClass DefaultFont;
extern char Unknown_B8F870[768];

void Unknown_6832C0(void);															// NAME 1 [Unknown_6832C0]: deffont.cpp's start-up ('!FontData[style]')
void Unknown_683940(void);															// NAME 1 [Unknown_683940]: deffont.cpp's shut-down
void * Load_Alloc_Data(char const * filename);										// NAME 2 [Load_Alloc_Data]: Westwood's name for the whole-file load (wwlib's Load_Alloc_Data(FileClass &))
unsigned char * Unknown_683AC0(float val_norm, int type, unsigned char * remap);	// NAME 1 [Unknown_683AC0]: neutral (D10)
void Unknown_684E00(void);															// NAME 1 [Unknown_684E00]: loads fontpal.pcx into Unknown_B8F870

#endif
