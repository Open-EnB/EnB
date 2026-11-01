// Earth & Beyond client (reconstruction): app\client\scriptedmovieline.cpp (provisional name). Notes at the end of the file.
#include <string.h>
#include "scriptedmovieline.h"

// 0x7FFC50
ScriptedMovieLineClass::ScriptedMovieLineClass()
{
	Frame = 0;
	Text = NULL;
	Unknown08 = NULL;
}

// 0x7FFC70
ScriptedMovieLineClass::~ScriptedMovieLineClass()
{
	if (Text != NULL) {
		delete [] Text;
		Text = NULL;
	}
	Unknown08 = NULL;
}

// 0x7FFCB0
void ScriptedMovieLineClass::Unknown_7FFCB0(const char *text)
{
	Text = new char[strlen(text) + 1];
	strcpy(Text, text);
	strupr(Text);
	Unknown08 = Text;
}

// 0x7FFD30: the line starts with name (any case): the cursor past it and its comma
bool ScriptedMovieLineClass::Unknown_7FFD30(const char *name)
{
	int len = strlen(name);
	if (strnicmp(Unknown08, name, len) == 0) {
		Unknown08 += len;
		while (*Unknown08 != 0 && *Unknown08 != ',') {
			Unknown08++;
		}
		if (*Unknown08 == ',') {
			Unknown08++;
		}
		return(true);
	}
	return(false);
}

// 0x7FFDC0: the next parameter (up to a comma; leading blanks skipped, a quoted one unquoted, trailing blanks cut)
char *ScriptedMovieLineClass::Unknown_7FFDC0()
{
	if (Unknown08 == NULL || *Unknown08 == 0) {
		return(NULL);
	}
	char *param = Unknown08;
	while (*param != 0 && *param <= ' ') {
		param++;
	}
	if (*param == '"') {
		param++;
		Unknown08 = param;
		while (*Unknown08 != 0 && *Unknown08 != '"') {
			Unknown08++;
		}
	}
	while (*Unknown08 != 0 && *Unknown08 != ',') {
		Unknown08++;
	}
	if (*Unknown08 != 0) {
		*Unknown08 = 0;
		Unknown08++;
	}
	unsigned int len = strlen(param);
	while (len != 0 && param[len - 1] <= ' ') {
		param[--len] = 0;
	}
	if (len != 0 && param[len - 1] == '"') {
		param[len - 1] = 0;
	}
	return(param);
}

// Reconstruction notes (LC1, 2026-10-31; placement D137 from SM2's report):
// - The object: client 0x7FFC50-0x7FFE9F, after the screen fade / letterbox object (unknown_screenfade.cpp, 0x7FF010-0x7FFC4F) and before
//   scriptedmovie.cpp's (from 0x7FFEA0). No asserts, so no __FILE__: the name scriptedmovieline.cpp is provisional
//   (NAME 1).
// - 0x7FFEA0 (std::sort's order of the lines) is scriptedmovie.cpp's: it pushes the same 'CREATE_OBJECT' literal
//   (0xBA2438) as the dispatcher 0x803310, and the client doesn't share literals across objects (95 copies of "%s").
// - One class: the dispatcher passes the line itself to 0x7FFD30, whose cursor (+0x08) 0x7FFCB0 sets (P-SM1-1).
