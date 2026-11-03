// Earth & Beyond client (reconstruction)
//
// unknown_jpeg.h
//
// E&B's baseline JPEG writer (unknown_jpeg.cpp): WW3D::Make_Screen_Shot's 'jpg' format.
// Header PROVISIONAL (D137); names describe the code (NAME 2).
//

#ifndef UNKNOWN_JPEG_H
#define UNKNOWN_JPEG_H

class FileClass;

/*
**	The entropy-coded data of one scan: bytes in a buffer grown 4 KB at a time, a 0x00 stuffed after
**	every 0xFF.
*/
class JPEGBitBufferClass							// NAME 2: layout from 0x9E50A0 (0x10 bytes)
{
public:
	JPEGBitBufferClass(void);
	~JPEGBitBufferClass(void);

	void						Grow(int bytes);										// NAME 2: 0x9E5120
	void						Write_Bits(short value, int count);				// NAME 2: 0x9E5180
	void						Flush(void);											// NAME 2: 0x9E52C0
	int						Length(void);											// NAME 2: 0x9E5310
	unsigned char *		Get_Data(void) { return Data; }					// NAME 2: the scan's write

private:
	void						Next_Byte(void)										// NAME 2: inlined in Write_Bits
	{
		if (Data[Position] == 0xFF) {
			Position++;
			Data[Position] = 0;
		}
		Position++;
		Data[Position] = 0;
		BitsLeft = 8;
	}

	int						Position;												// NAME 2: +0x00
	int						BitsLeft;												// NAME 2: +0x04
	unsigned char *		Data;														// NAME 2: +0x08
	int						Size;														// NAME 2: +0x0C
};


class JPEGWriterClass								// NAME 2: one member, the open file (WW3D::Make_Screen_Shot's stack object)
{
public:
	JPEGWriterClass(void);
	~JPEGWriterClass(void);

	bool			Write(const char * filename, const unsigned char * pixels, int width, int height, int pitch);	// NAME 2: 0x9E3870

	void			Encode_Component(int component, const unsigned char * pixels, int width, int height, int pitch);	// NAME 2: 0x9E3BD0
	void			Convert_Color(const unsigned char * rgb, signed char * ycc);					// NAME 2: 0x9E3EC0
	void			Forward_DCT(int component, const signed char * ycc, short * out);				// NAME 2: 0x9E3FB0
	void			Quantize(int component, short * data);												// NAME 2: 0x9E4090
	void			Zig_Zag(const short * in, short * out);												// NAME 2: 0x9E40E0
	void			Encode_DC(int component, short last_dc, const short * block, JPEGBitBufferClass * bits);	// NAME 2: 0x9E4110
	void			Encode_AC(int component, const short * block, JPEGBitBufferClass * bits);	// NAME 2: 0x9E41A0
	short			Bit_Count(short value);																	// NAME 2: 0x9E42D0
	void			Get_AC_Code(int component, unsigned char run, unsigned char size, unsigned char * length, unsigned short * code);	// NAME 2: 0x9E4300
	void			Get_DC_Code(int component, unsigned char size, unsigned char * length, unsigned short * code);	// NAME 2: 0x9E4360
	void			Write_DHT_DC(void);																		// NAME 2: 0x9E43A0
	void			Write_DHT_AC(void);																		// NAME 2: 0x9E4580
	void			Write_Marker(unsigned short marker);																// NAME 2: 0x9E4F10
	void			Write_SOF(int width, int height);													// NAME 2: 0x9E4F30
	void			Write_SOS(unsigned char id, unsigned char tables);								// NAME 2: 0x9E4FC0
	void			Write_DQT(void);																			// NAME 2: 0x9E5020

private:
	FileClass *	File;																							// NAME 2: +0x00
};

#endif
