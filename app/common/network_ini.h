// Earth & Beyond client (reconstruction): network.ini's holder (connect.cpp's; no client file of its own known)
#ifndef NETWORK_INI_H
#define NETWORK_INI_H

class INIClass;

// network.ini, loaded on first use (Get_Network_INI 0x821B40, the INI 0xBE8EA8). A class static: the client's
// atexit stub 0x820F60 is guarded (a file static's is not) and the assert reads the name unqualified. Its own
// header, not connect.h: sclient.cpp sees connect.h and tsk_game.h, and a declaration more there moves its
// send-pad window (workflow.md 3.4)
class NetworkINIClass	// NAME 1: describes what it is (interim holder, D137)
{
	public:
		static INIClass &Get_Network_INI();	// 0x821B40
		static INIClass Network_INI;			// 0xBE8EA8
};

#endif
