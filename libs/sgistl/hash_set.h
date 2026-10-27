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
 * libs/sgistl (D87): SGI STL 3.3's hash_set (stl_hash_set.h) over hash_map.h. Kept out of hash_map.h:
 * hash_map.h reaches most app files (delta.h -> kv_encoder_base.h -> kv_list.h), and declaring
 * hash_set there moved the inlining of DeltaItemArrayClass constructors in aux_roots.cpp and
 * tsk_login.cpp (3 pairs traded for 3, 2026-10-26).
 */

#ifndef __ENB_SGISTL_HASH_SET_H
#define __ENB_SGISTL_HASH_SET_H

#if defined(_MSC_VER)
#pragma once
#endif

#include "hash_map.h"

namespace std {

/* stl_function.h */
template <class _Tp>
struct _Identity : public unary_function<_Tp,_Tp> {
	const _Tp& operator()(const _Tp& __x) const { return __x; }
};

}

#include "stl_hash_set.h"

#endif /* __ENB_SGISTL_HASH_SET_H */
