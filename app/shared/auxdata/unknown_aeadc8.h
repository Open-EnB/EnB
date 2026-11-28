// Earth & Beyond client (reconstruction)
/*
**	PROVISIONAL file name (no assert or __FILE__ names it): the one-slot key/value interface
**	(vtable 0xAEADC8) that DeltaClass and the login's data objects derive from, out of delta.h so
**	that unknown_packetdata.h gets it without name.h / permstr.h. The client's CRT table shows the
**	objects including sclient.h or unknown_packetdata.h without permstr.h's _PermStringInit
**	initialiser (148 of them), so the header their interface came from did not reach permstr.h.
*/



#if defined(_MSC_VER)
#pragma once
#endif

#ifndef UNKNOWN_AEADC8_H  // NAME 1 [unknown_aeadc8.h]: neutral file name (D10)
#define UNKNOWN_AEADC8_H

class KVEncoderClass;

// The base DeltaClass's constructor stores first (vtable 0xAEADC8: one slot, _purecall).
class DeltaInterfaceClass  // NAME 2: describes what it does
{
public:
	virtual void Unknown_00(KVEncoderClass *visitor) = 0;
};

#endif
