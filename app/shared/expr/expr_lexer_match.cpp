// Earth & Beyond client (reconstruction): notes at the end of the file
#include "expr_lexer_match.h"

/*
**	This file is a reconstruction for the exact build (D286): a skeleton translation unit, the file's own header
**	and nothing else, as Westwood kept beside a header-only class (Renegade's statemachine.cpp, ww3d2's
**	prim_anim.cpp). Retail's rich header counts 47 more C++ objects than our link, objects with no code, data,
**	initialiser or assert of their own (docs/binary.md 5.4, 5.5); the header has no .cpp in our tree.
**	Original location: PROVISIONAL (D137): the object is empty, so the image cannot confirm the name; it compiles to
**	no section but its directives (checked at its landing), and app.lib never pulls it into the one client build.
*/
