// Earth & Beyond client (reconstruction): app\client\unknown_robj_hash.h, the SGI hash of a render-object key (its
// address, as co_data.h's hash<ClientObjectClass *>): shared by interface_static_sort_list.h and fontmanager.h
// (P-FU1-2, 2026-11-03), so a file may include both.
#ifndef UNKNOWN_ROBJ_HASH_H  // NAME 1 [unknown_robj_hash.h]: ours (D137 interim): no client file holds it alone
#define UNKNOWN_ROBJ_HASH_H

#include <hash_map.h>

class RenderObjClass;

__STL_BEGIN_NAMESPACE
__STL_TEMPLATE_NULL struct hash<RenderObjClass *> {  // the key's address (as co_data.h's)
	size_t operator()(RenderObjClass *robj) const { return((size_t)robj); }
};
__STL_END_NAMESPACE

#endif
