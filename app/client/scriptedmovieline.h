// Earth & Beyond client (reconstruction): app\client\scriptedmovieline.h, one line of the scripted movie's script
// (the object between option.cpp and scriptedmovie.cpp, client 0x7FFC50-0x7FFE9F, no __FILE__; the file's name is
// provisional, NAME 1). Moved out of sclient.h (LC1, 2026-10-31, D137), where it was two classes
// (ScriptedMovieLineClass and ScriptedMovieParamsClass: the dispatcher 0x803310 passes the line itself to 0x7FFD30).
#ifndef SCRIPTEDMOVIELINE_H  // NAME 1 [scriptedmovieline.h]: provisional header name
#define SCRIPTEDMOVIELINE_H

// one line of the script (0x0C bytes; the loader 0x8099B0 makes them, std::sort orders them by 0x7FFEA0)
class ScriptedMovieLineClass	// NAME 1: neutral (D10) (a class: clienttest's gen_stubs declares pointer parameters' types as classes)
{
public:
	ScriptedMovieLineClass();							// 0x7FFC50: all zero
	~ScriptedMovieLineClass();							// 0x7FFC70: the text freed
	void Unknown_7FFCB0(const char *text);				// 0x7FFCB0: the text copied, upper case; the cursor at its start  // NAME 1 [text]: what the loader 0x8099B0 passes
	bool Unknown_7FFD30(const char *name);				// 0x7FFD30: the line starts with the command name (any case): past it and its comma  // NAME 1 [name]: what the dispatcher 0x803310 passes
	char *Unknown_7FFDC0();								// 0x7FFDC0: the next comma-separated parameter, trimmed and unquoted; NULL at the end
	unsigned int Frame;									// +0x00 (the movie's time it runs at)  // NAME 2: what the order compares
	char *Text;											// +0x04: the command line  // NAME 2: what it holds
	char *Unknown08;									// +0x08: the cursor (0x7FFCB0 sets it to the text; 0x7FFD30 / 0x7FFDC0 move it)
};

#endif // SCRIPTEDMOVIELINE_H
