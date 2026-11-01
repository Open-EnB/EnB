// Earth & Beyond client (reconstruction): the proof-of-concept client's data globals and the movie call (D179). NOT THE CLIENT'S CODE.
/*
**	The globals app files declare and no tree file defines yet that gen_stubs.py --link cannot write
**	(objects, not pointers or scalars), each constructed as its type's default; and E&B's BINKMovie::Play
**	(the movie, its subtitles, a volume), which the tree's BINKMovie (Renegade's) lacks: it plays the movie
**	through Renegade's Play without subtitles and at full volume. Delete one here when a tree file defines it.
*/



#include <string>
#include <vector>
#include "..\..\stubs\stub_log.h"
#include "..\..\wwlib\_mono.h"
#include "..\..\wwlib\random.h"
#include "..\..\wwmath\rect.h"

MonoClass Mono;									// wwlib's mono screen (_mono.h)
Random2Class Unknown_BDADC8;					// 0xBDADC8 (soundlib.cpp's view)
RandomClass Unknown_BDBB84;						// 0xBDBB84 (eff_o.cpp's, ui_avatr.cpp's, ui_line_display.cpp's)
RectClass Unknown_C07550;						// 0xC07550 (sclient.cpp's, scriptedmovie.cpp's)
std::string Unknown_BE5BD0;						// 0xBE5BD0 (ui_cpit.cpp's)

struct TestEffectStruct { char Unknown00[0x64]; };	// sclient.cpp's record, 0x64 bytes (only its size matters here)
std::vector<TestEffectStruct> Unknown_BE7D80;		// 0xBE7D80 (sclient.cpp's)

// ---------------------------------------------------------------- the movie

class FontCharsClass;

class BINKMovie		// the two Play overloads (Renegade's in binkmovie.lib, E&B's here)
{
public:
	static void Play(const char *filename, const char *subtitlename, FontCharsClass *font);
	static void Play(const char *filename, const char *subtitlename, int volume);
	static bool Is_Complete();
};

// E&B's Play (client 0xA161B0-0xA162D0's run): here Renegade's, no subtitles, the volume ignored
void BINKMovie::Play(const char *filename, const char *subtitlename, int volume)
{
	Stub_Log_Line("POC BINKMovie::Play('%s', '%s', %d): Renegade's Play, no subtitles", filename, subtitlename, volume);
	Play(filename, (const char *)NULL, (FontCharsClass *)NULL);
	Stub_Log_Line("POC BINKMovie::Play: %s", Is_Complete() ? "complete at once (not opened?)" : "playing");
}
