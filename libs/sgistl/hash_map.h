/*
 * Copyright (c) 1996-1998
 * Silicon Graphics Computer Systems, Inc.
 *
 * Permission to use, copy, modify, distribute and sell this software
 * and its documentation for any purpose is hereby granted without fee,
 * provided that the above copyright notice appear in all copies and
 * that both that copyright notice and this permission notice appear
 * in supporting documentation.  Silicon Graphics makes no
 * representations about the suitability of this software for any
 * purpose.  It is provided "as is" without express or implied warranty.
 *
 *
 * Copyright (c) 1994
 * Hewlett-Packard Company
 *
 * Permission to use, copy, modify, distribute and sell this software
 * and its documentation for any purpose is hereby granted without fee,
 * provided that the above copyright notice appear in all copies and
 * that both that copyright notice and this permission notice appear
 * in supporting documentation.  Hewlett-Packard Company makes no
 * representations about the suitability of this software for any
 * purpose.  It is provided "as is" without express or implied warranty.
 */

/*
 * libs/sgistl (D87): SGI STL 3.3's hash_map (stl_hashtable.h, stl_hash_map.h, stl_hash_fun.h, from
 * C:\EnBDevelopment\SDK\stl-3.3) on top of VC6's own STL, as the Earth & Beyond client has it:
 * the hashtable's buckets a VC6 std::vector<_Node *> (hashtable +4), the nodes from operator new,
 * no exception handling inside the table, __stl_prime_list a static array (one copy per translation
 * unit: 0xAE701C, 0xAEB764, ...). This file stands for SGI's configuration (stl_config.h) and the two
 * pieces of SGI's own allocator and function headers the table needs (simple_alloc, _Select1st).
 */

#ifndef __ENB_SGISTL_HASH_MAP_H
#define __ENB_SGISTL_HASH_MAP_H

#if defined(_MSC_VER)
#pragma once
#endif

#include <stddef.h>
#include <utility>
#include <functional>
#include <algorithm>
#include <vector>
#include <string>

/* stl_config.h, for VC6: namespaces; no member templates, no partial specialisation or ordering,
   no exceptions inside the containers, SGI's (non-standard) allocator interface */
#define __STL_BEGIN_NAMESPACE namespace std {
#define __STL_END_NAMESPACE }
#define __STD std
#define __STD_QUALIFIER std::
#define __STL_TEMPLATE_NULL template<>
#define __STL_NULL_TMPL_ARGS
#define __STL_TRY
#define __STL_UNWIND(action)
#define __STL_DEFAULT_ALLOCATOR(T) allocator< T >
#define __STL_DEPENDENT_DEFAULT_TMPL(_Tp) = _Tp
#define __STL_CLASS_REQUIRES(__type_var, __concept)
#define __STL_CLASS_UNARY_FUNCTION_CHECK(__fn, __ret, __arg)
#define __STL_CLASS_BINARY_FUNCTION_CHECK(__fn, __ret, __first, __second)

namespace std {

/* stl_alloc.h's simple_alloc, on VC6's operator new / delete (the client: push sizeof(node); call new) */
template<class _Tp, class _Alloc>
class simple_alloc {
public:
	static _Tp* allocate(size_t __n)
		{ return 0 == __n ? 0 : (_Tp*) operator new(__n * sizeof (_Tp)); }
	static void deallocate(_Tp* __p, size_t __n)
		{ if (0 != __n) operator delete(__p); }
};

/* The buckets' allocator: VC6's, except that it allocates bytes (_Charalloc), so the client tests
   the byte count for < 0, not the element count (the bucket reserve in KVListClass's constructor
   0x4B62B0, vector::insert 0x4B7420). What the client's E&B build really used here isn't known. */
template<class _Ty> class _Bucket_allocator : public allocator<_Ty> {  // NAME 2: describes what it does
public:
	_Ty *allocate(size_t _N, const void *) { return ((_Ty *)_Charalloc(_N * sizeof (_Ty))); }
};

/* stl_function.h */
template <class _Pair>
struct _Select1st : public unary_function<_Pair, typename _Pair::first_type> {
	const typename _Pair::first_type& operator()(const _Pair& __x) const {
		return __x.first;
	}
};

}

#include "stl_hash_fun.h"
#include "stl_hashtable.h"
#include "stl_hash_map.h"

namespace std {

/* E&B's, not SGI's: the hash of a std::string key, the key by value (ret 0x10) and SGI's
   __stl_hash_string of its c_str(); one COMDAT copy at 0x42DB00 (65 callers) */
template<> struct hash<string> {
	size_t operator()(string __s) const { return __stl_hash_string(__s.c_str()); }
};

}

#endif /* __ENB_SGISTL_HASH_MAP_H */
