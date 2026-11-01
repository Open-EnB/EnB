// Earth & Beyond client (reconstruction)
/*
**	The UDP socket CordMgr and CordCon use (CordCon::Udp_, +0x10): WWOnline's wnet UDP
**	(libs\wwonline\wnet\udp.h, E&B's 0x2C-byte revision; client 0x8BF990-0x8C0620). This header
**	only forwards to it, for the wwudp files' `#include "udp.h"`. The capture harness
**	(tests/udpreplay) defines the same class's members as its replay socket instead of linking
**	wnet's udp.cpp.
**
**	wnet's udp.h defines close, read and write as _close, _read and _write under _WINDOWS (for its
**	own udp.cpp, which calls none of them); here they would rename CordCon::read / write / close
**	in the files that include this header after cordcon.h, so they are undefined again.
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef WWUDP_UDP_H
#define WWUDP_UDP_H

#include "..\..\libs\wwonline\wnet\udp.h"

#undef close
#undef read
#undef write

#endif
