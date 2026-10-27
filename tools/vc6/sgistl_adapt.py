"""Writes libs/sgistl/stl_*.h from SGI STL 3.3 (C:\\EnBDevelopment\\SDK\\stl-3.3), adapted for VC6 (D87).

    python tools/vc6/sgistl_adapt.py

Every change is an exact (old, new) pair that must occur once; the result is CRLF.
"""
import os
import sys

sys.path.insert(0, os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', 're'))
import roots  # noqa: E402

SRC = roots.SGI_STL
DST = os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', '..', 'libs', 'sgistl')

TAG = '/* VC6 (E&B, D87): '

EDITS = {
    'stl_hash_fun.h': [],
    'stl_hashtable.h': [
        ('#include <stl_algobase.h>\n#include <stl_alloc.h>\n#include <stl_construct.h>\n#include <stl_tempbuf.h>\n'
         '#include <stl_algo.h>\n#include <stl_uninitialized.h>\n#include <stl_function.h>\n#include <stl_vector.h>\n'
         '#include <stl_hash_fun.h>\n',
         TAG + "VC6's own <vector>, <algorithm> and <xmemory> stand for SGI's; hash_map.h includes them */\n"
         '#include "stl_hash_fun.h"\n'),
        ('class _ExtractKey, class _EqualKey, class _Alloc = alloc>',
         'class _ExtractKey, class _EqualKey, class _Alloc = allocator<_Val> >'),
        ('  vector<_Node*,_Alloc> _M_buckets;',
         '  vector<_Node*, _Bucket_allocator<_Node*> > _M_buckets;  ' + TAG + "VC6's allocator has no rebind; the client's buckets allocate bytes (hash_map.h) */"),
        ('      _M_get_key(__ext),\n      _M_buckets(__a),', '      _M_get_key(__ext),\n      _M_buckets(),'),
        ('      _M_get_key(_ExtractKey()),\n      _M_buckets(__a),', '      _M_get_key(_ExtractKey()),\n      _M_buckets(),'),
        ('      _M_buckets(__ht.get_allocator()),', '      _M_buckets(),'),
        ('#else /* __STL_MEMBER_TEMPLATES */\n  friend bool __STD_QUALIFIER\n'
         '  operator== __STL_NULL_TMPL_ARGS (const hashtable&, const hashtable&);\n',
         '#else /* __STL_MEMBER_TEMPLATES */\n  ' + TAG + 'no friend operator== (VC6 has no <> friends) */\n'),
        ('      construct(&__n->_M_val, __obj);', '      _Construct(&__n->_M_val, __obj);'),
        ('    destroy(&__n->_M_val);', '    _Destroy(&__n->_M_val);'),
        ('      vector<_Node*, _All> __tmp(__n, (_Node*)(0),\n                                 _M_buckets.get_allocator());',
         '      vector<_Node*, _Bucket_allocator<_Node*> > __tmp(__n, (_Node*)(0));  ' + TAG + 'a default allocator: the client copies no allocator byte from the buckets (0x4B7B80) */'),
        ('            const _EqualKey&   __eql,\n            const allocator_type& __a = allocator_type())\n'
         '    : __HASH_ALLOC_INIT(__a)\n      _M_hash(__hf),',
         '            const _EqualKey&   __eql)  ' + TAG + "no allocator argument: the client's out-of-line copies take "
         'three (ret 0xC: buff_database.cpp 0x6778A0, tutorial_manager.cpp 0x6AE250) */\n'
         '    : _M_hash(__hf),'),
    ],
    'stl_hash_map.h': [
        ('#include <concept_checks.h>\n', ''),
        ('#else /* __STL_MEMBER_TEMPLATES */\n  friend bool __STD_QUALIFIER\n'
         '  operator== __STL_NULL_TMPL_ARGS (const hash_map&, const hash_map&);\n',
         '#else /* __STL_MEMBER_TEMPLATES */\n  ' + TAG + 'no friend operator== (VC6 has no <> friends) */\n'),
        ('#else /* __STL_MEMBER_TEMPLATES */\n  friend bool __STD_QUALIFIER\n'
         '  operator== __STL_NULL_TMPL_ARGS (const hash_multimap&,const hash_multimap&);\n',
         '#else /* __STL_MEMBER_TEMPLATES */\n  ' + TAG + 'no friend operator== (VC6 has no <> friends) */\n'),
    ],
    'stl_hash_set.h': [
        ('#include <concept_checks.h>\n', ''),
        ('#else /* __STL_MEMBER_TEMPLATES */\n  friend bool __STD_QUALIFIER\n'
         '  operator== __STL_NULL_TMPL_ARGS (const hash_set&, const hash_set&);\n',
         '#else /* __STL_MEMBER_TEMPLATES */\n  ' + TAG + 'no friend operator== (VC6 has no <> friends) */\n'),
        ('#else /* __STL_MEMBER_TEMPLATES */\n  friend bool __STD_QUALIFIER\n'
         '  operator== __STL_NULL_TMPL_ARGS (const hash_multiset&,const hash_multiset&);\n',
         '#else /* __STL_MEMBER_TEMPLATES */\n  ' + TAG + 'no friend operator== (VC6 has no <> friends) */\n'),
    ],
}

HEAD = ('/*\n * libs/sgistl (D87): SGI STL 3.3 (2000), adapted for VC6 as the Earth & Beyond client\'s\n'
        ' * hash_map. The changes are marked "VC6 (E&B, D87)"; tools/vc6/sgistl_adapt.py made them.\n */\n')


# Every occurrence: the containers' constructors call the table's three-argument constructor (above)
REPLACE_ALL = {
    'stl_hash_map.h': [('key_equal(), allocator_type())', 'key_equal())')],
    'stl_hash_set.h': [('key_equal(), allocator_type())', 'key_equal())')],
}


def main():
    for name, edits in EDITS.items():
        text = open(os.path.join(SRC, name), 'rb').read().decode('ascii').replace('\r\n', '\n')
        for old, new in edits:
            n = text.count(old)
            if n != 1:
                sys.exit('%s: %d matches for %r' % (name, n, old[:60]))
            text = text.replace(old, new)
        for old, new in REPLACE_ALL.get(name, []):
            if old not in text:
                sys.exit('%s: no match for %r' % (name, old[:60]))
            text = text.replace(old, new)
        text = HEAD + text
        with open(os.path.join(DST, name), 'wb') as f:
            f.write(text.replace('\n', '\r\n').encode('ascii'))
        print(name, 'written')


if __name__ == '__main__':
    main()
