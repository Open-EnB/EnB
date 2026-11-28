#ifndef __KV_PAIR_VALUE_H  // NAME 3 [kv_pair_value.h]: inferred private conversion-header name
#define __KV_PAIR_VALUE_H

#include "kv_pair.h"

inline char *Pair_Value_String(const KVPairClass *pair)  // NAME 2: describes the shared conversion
{
	static char buf[0x1000];  // NAME 2: shared conversion buffer (retail 0xBE3B60)
	return pair->Get_Value_String(buf, sizeof(buf));
}

#endif
