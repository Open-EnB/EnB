// Earth & Beyond client (reconstruction): app\Common\chartemplate.cpp (PARTIAL: 0x82F350, 0x82F3C0). Notes at the end of the file.
#include "..\shared\unknown_debug.h"

#define NUMBER_PLAYER_RACES		3	// the FAIL_IF's (Terran, Jenquai, Progen)
#define MAX_PLAYER_PROFESSIONS	3	// the FAIL_IF's (warrior, trader, explorer)
#define NUM_CHAR_TEMPLATES		9	// the FAIL_IF's: race * 3 + profession

// 0xBA8A08: the templates' names, 0x82F3C0's
static const char *Unknown_BA8A08[NUM_CHAR_TEMPLATES] = {	// NAME 1: neutral (D10)
	"Enforcer", "Tradesman", "Scout", "Defender", "Seeker", "Explorer", "Warrior", "Privateer", "Sentinel"
};






































































// @0x82F350
int Unknown_82F350(int race, int prof)	// NAME 2 [race, prof]: the FAIL_IFs'; NAME 1 [Unknown_82F350]: neutral (D10)
{
	FAIL_IF((unsigned int)race >= NUMBER_PLAYER_RACES) return(9);
	FAIL_IF((unsigned int)prof >= MAX_PLAYER_PROFESSIONS) return(9);
	return(race * 3 + prof);
}
// @0x82F3C0
const char *Unknown_82F3C0(int template_idx)	// NAME 2 [template_idx]: the FAIL_IF's; NAME 1 [Unknown_82F3C0]: as ui_class.cpp's
{
	FAIL_IF((unsigned int)template_idx >= NUM_CHAR_TEMPLATES) return(0);
	return(Unknown_BA8A08[template_idx]);
}


/*
**	Notes.
**	The client's chartemplate.cpp (0x82F2A0 .. 0x8303B0: 'chartemplate.ini', CharTemplateEnum) is not rebuilt; these two
**	are what the class select window (ui_class.cpp) needs: its titles (0x5745B0's table) and the skills' tooltip key
**	(0x576360). The FAIL_IFs sit on the client's lines 85, 86 and 92 (D25).
*/
