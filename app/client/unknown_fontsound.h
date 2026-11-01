// Earth & Beyond client (reconstruction): app\client\unknown_fontsound.h (PROVISIONAL file name: no __FILE__; the
// object sits between font32bit.cpp and fontdatabase.cpp, 0x6BCFB0-0x6BD9E0). The font system's timed sounds.
#ifndef UNKNOWN_FONTSOUND_H
#define UNKNOWN_FONTSOUND_H

#include "..\..\wwlib\vector.h"
#include "..\..\wwlib\wwstring.h"

class AudibleSoundClass;

/*
**	One playing sound (new'd, 0x10 bytes).
*/
struct FontSoundStruct																// NAME 2 [FontSoundStruct]: a FontSys sound and its time left
{
	int Unknown00;																	// never written
	AudibleSoundClass * Sound;														// NAME 2: WWAudioClass::Create_Sound_Effect's result
	int Duration;																	// NAME 2: milliseconds left (60000 or more: no limit)
	unsigned long Time;																// NAME 2: timeGetTime of the last update
};

/*
**	The sounds FontSys plays (0x18 bytes, created by 0x526DE0 into 0xBE67B4).
*/
class Unknown_6BCFB0Class															// NAME 1 [Unknown_6BCFB0Class]: neutral (D10), as app/main declares it
{
	public:
		Unknown_6BCFB0Class(void);												// 0x6BCFB0
		~Unknown_6BCFB0Class(void);											// 0x6BD060

		void Clear(void);															// NAME 2 [Clear]: 0x6BD190, no caller
		void Play(StringClass name, float volume, float seconds);					// NAME 2 [Play]: 0x6BD270 (FontSys 0x6CD180 / 0x6CD6F0 / 0x6D60C0)
		void Update(void);															// NAME 2 [Update]: 0x6BD420 (FontSys 0x6D64C0)
		void Remove(FontSoundStruct * sound);										// NAME 2 [Remove]: 0x6BD530, no caller

	protected:
		DynamicVectorClass<FontSoundStruct *> Sounds;								// NAME 2 [Sounds]
};

#endif
