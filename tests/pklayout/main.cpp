// Earth & Beyond client (reconstruction)
// tests/pklayout (test plan LY-01): the checks are compile-time (pk_layout.cpp, tools/re/pklayout.py); the program
// only reports how many there are.
#include <stdio.h>
int Packet_Layout_Checks();
int main()
{
	printf("pklayout: %d layout checks compiled\n", Packet_Layout_Checks());
	return 0;
}
