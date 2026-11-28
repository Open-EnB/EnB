// KV list interface and checked vector access. Helper ownership is inferred.
#ifndef __KV_LIST_H
#define __KV_LIST_H

#include "kv_list_core.h"
#include "g_assert.h"
































































inline KVPairClass *Get_KV_Pair(const std::vector<KVPairClass *> &V, int index)  // NAME 2: checked vector access
{
	G_ASSERT(index < V.size());
	return V[index];
}

#endif //__KV_LIST_H
