// Earth & Beyond client (reconstruction)
// clienttest: the font globals (fontdatabase.h's types since P-H12-3). 0xBE67B8 stays null: the harness loads no
// fonts. 0xBE67BC is a real FontStringFilesClass with no files: its constructor would load fc_default.ini through
// 0xBE67B8, so only the member is constructed; Unload (the loading view's fc_map_load00.ini) then finds nothing.
#include <new>
#include <string.h>
#include "..\..\..\app\client\fontdatabase.h"

FontDatabaseClass *Unknown_BE67B8 = 0;			// 0xBE67B8

static FontStringFilesClass *Empty_String_Files()
{
	void *memory = operator new(sizeof(FontStringFilesClass));
	memset(memory, 0, sizeof(FontStringFilesClass));
	FontStringFilesClass *files = (FontStringFilesClass *)memory;
	new (&files->Files) std::hash_map<std::string, FontStringFileStruct *>;
	return files;
}
FontStringFilesClass *Unknown_BE67BC = Empty_String_Files();	// 0xBE67BC
