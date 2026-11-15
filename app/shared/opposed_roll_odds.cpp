// Earth & Beyond client (reconstruction): app\shared\opposed_roll_odds.cpp. Notes at the end of the file.
#include "opposed_roll_odds.h"
#include <math.h>
#include "g_assert.h"
// 0x4F25D0  // NOT EXACT (0.75, attempts.py 0x4F25D0): order: the client subtracts N before it forms M + 1, ours after
float Opposed_Roll_Odds(int M, int N)
{
	FAIL_IF(M < 0) M = 0;
	FAIL_IF(N < 0) N = 0;
	if (M >= N) {
		return((2.0f * M - N + 2.0f) / (2.0f * (M + 1)));
	}
	return((M + 2.0f) / (2.0f * (N + 1)));
}



// 0x4F2670
float Opposed_Roll_Odds(int M, int N, int C)
{
	FAIL_IF(M < 0) M = 0;
	FAIL_IF(N < 0) N = 0;
	FAIL_IF(C > M) C = M;
	if (M >= N) {
		if (C >= N) {
			return(((M - C) + 1.0f) / (M + 1.0f));
		}
		return((float)(((N + 1 + C) * (N - C)) / 2 + (M - N + 1) * (N + 1)) / ((M + 1) * (N + 1)));
	}
	return((((M + C) + 2.0f) * ((M - C) + 1.0f)) / ((float)(M + 1) * (N + 1) * 2.0f));
}















// 0x4F27B0
float Opposed_Roll_Odds(int M, int N, float scale)
{
	FAIL_IF(M < 0) M = 0;
	FAIL_IF(N < 0) N = 0;
	if (scale <= 0.0f) return(1.0f);
	float ratio = M / scale;
	if ((int)floor(ratio) >= N) {
		return((2.0f * (M + 1) - N * scale) / (2.0f * (M + 1)));
	}
	return((ratio + 2.0f) / (2.0f * (N + 1)));
}

/*
**	Reconstruction notes (opposed_roll_odds.cpp, client 0x4F25D0-0x4F2866; __FILE__ 0xB75884, FAIL_IF 'M < 0' / 'N < 0' on
**	the client's lines 8 / 9, 21 / 22 and 50 / 51, 'C > M' on line 23).
**
**	- Three overloads, no client caller. 0x4F25D0: M against N; 0x4F2670: with C (clamped to M); 0x4F27B0: with a float
**	  divisor of M (1 when it is not positive). Each FAIL_IF clamps its argument (M, N to 0; C to M).
**	- 0x4F25D0 is NOT EXACT (order: the client subtracts N before it forms M + 1, ours after).
*/
