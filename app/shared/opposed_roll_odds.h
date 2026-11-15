// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\shared\opposed_roll_odds.h (PROVISIONAL, D137: opposed_roll_odds.cpp's header,
**	after its __FILE__ 'e:\build\beta\G\app\shared\opposed_roll_odds.cpp' 0xB75884; the client holds no string of it)
**
**	The odds of an opposed roll: M against N (FAIL_IF 'M < 0', 'N < 0', 'C > M'). No client function calls them.
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef OPPOSED_ROLL_ODDS_H  // NAME 3 [opposed_roll_odds.h]: the header of opposed_roll_odds.cpp
#define OPPOSED_ROLL_ODDS_H

float Opposed_Roll_Odds(int M, int N);					// 0x4F25D0 (FAIL_IF l.8, 9)  // NAME 3 [Opposed_Roll_Odds]: the file's name; [M, N]: the FAIL_IFs'
float Opposed_Roll_Odds(int M, int N, int C);			// 0x4F2670 (FAIL_IF l.21, 22, 23)  // NAME 3 [Opposed_Roll_Odds]: the file's name; [M, N, C]: the FAIL_IFs'
float Opposed_Roll_Odds(int M, int N, float scale);		// 0x4F27B0 (FAIL_IF l.50, 51)  // NAME 3 [Opposed_Roll_Odds]: the file's name; [M, N]: the FAIL_IFs'; NAME 1 [scale]: what it does

#endif
