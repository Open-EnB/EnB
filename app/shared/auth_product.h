// Earth & Beyond client (reconstruction)
/*
**	The auth product id "2184" (docs/binary.md 5.8). An interim header (D137: name and place provisional, the
**	client's own is not in the binary): a file-static non-const pointer, so each including object opens its .data
**	with an unread copy, as 221 of retail's app objects do (one literal, /GF). Read by tsk_login.cpp
**	(Fetch_EA_Ticket 0x76A7B0, 0xB9B9CC); ui_registration.cpp keeps its own RegistrationGameId (0xB85EF0).
*/
#ifndef AUTH_PRODUCT_H
#define AUTH_PRODUCT_H

static char *AUTH_PRODUCT = "2184";

#endif
