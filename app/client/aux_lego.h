// Earth & Beyond client (reconstruction)
/*
**	The lego constructors: Attachment, Lego (client_lego_root, tsk_smovie: 0x7331B0, 0x76D2C0).
**	The aux-data constructors are gated by family in aux_roots.h / aux_roots_send.h: an object compiles
**	(and numbers at start-up, the Delta templates' per-class globals: workflow.md 3.4) exactly the
**	families whose constructors it sees, as the client's objects do (their initialisers from 0xBE3B44).
**	Include this header first, before anything that includes aux_roots.h (the macros must come first).
**
**	// NAME 1 [aux_lego.h]: provisional file name (the client's headers are not named by any string)
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef __AUX_LEGO_H
#define __AUX_LEGO_H

#include "..\shared\auxdata\delta_numbering.h"
#define AUX_CTORS_LEGO
#include "aux_roots.h"

#endif //__AUX_LEGO_H
