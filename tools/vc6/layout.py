"""Prints a class's memory layout as compiled by VC6: every data member's offset and the size.

    python layout.py CLASS HEADER [-I dir]...

Generates a small program that includes HEADER with `protected`/`private` redefined as `public`,
prints offsetof() of each non-static data member it finds in the class body (inherited members
are reported by their own classes), compiles and runs it in the build VM. Used to turn the
displacement shifts found by tools/re/layoutdiff.py into the names of the members that moved.
"""
import argparse
import os
import re
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.dirname(os.path.dirname(HERE))
sys.path.insert(0, HERE)
sys.path.insert(0, os.path.join(REPO, 'tools', 're'))
import vc6  # noqa: E402
from easource import strip_comments  # noqa: E402

DEFAULT_INCLUDES = ['wwlib', 'wwdebug', 'wwmath', 'wwsaveload', 'wwaudio', 'ww3d2', 'wwshade']
MEMBER = re.compile(r'^\s*(?:mutable\s+)?(?!static\b|typedef\b|friend\b|enum\b|class\b|struct\b|return\b|virtual\b|using\b)'
                    r'[A-Za-z_][\w:<>,\s\*&]*?[\s\*&]([A-Za-z_]\w*)\s*(\[[^\]]+\])?\s*;', re.M)


def members(header, cls):
    text = strip_comments(open(header, encoding='latin1').read())
    m = re.search(r'\b(class|struct)\s+' + re.escape(cls) + r'\b[^;{]*\{', text)
    if not m:
        raise SystemExit(f'{cls} not found in {header}')
    depth, i = 1, m.end()
    body_start = i
    while depth and i < len(text):
        depth += {'{': 1, '}': -1}.get(text[i], 0)
        i += 1
    body = text[body_start:i - 1]
    # drop nested bodies (inline functions, nested types) so only declarations at depth 0 remain
    flat, depth = [], 0
    for ch in body:
        if ch == '{':
            depth += 1
            continue
        if ch == '}':
            depth -= 1
            flat.append(';')
            continue
        if depth == 0:
            flat.append(ch)
    names = []
    for stmt in ''.join(flat).split(';'):
        stmt = stmt.strip()
        if not stmt or '(' in stmt or stmt.startswith(('static', 'typedef', 'friend', 'enum', 'class', 'struct',
                                                        'public', 'protected', 'private', 'using')):
            stmt = re.sub(r'^(public|protected|private)\s*:\s*', '', stmt)
            if not stmt or '(' in stmt or stmt.startswith(('static', 'typedef', 'friend', 'enum', 'class', 'struct', 'using')):
                continue
        mm = re.match(r'(?:mutable\s+)?[A-Za-z_][\w:<>,\s\*&]*?[\s\*&]([A-Za-z_]\w*)\s*(\[[^\]]+\])?$', stmt)
        if mm:
            names.append(mm.group(1))
    return names


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('cls')
    ap.add_argument('header')
    ap.add_argument('-I', dest='includes', action='append', default=[])
    a = ap.parse_args()
    header = os.path.abspath(a.header)
    names = members(header, a.cls)
    work = os.path.join(REPO, '.work', 'layout')
    os.makedirs(work, exist_ok=True)
    src = os.path.join(work, f'layout_{a.cls}.cpp')
    # The offsets go into a constant array; the object is only compiled, never linked, and the
    # array is read back from it.
    lines = ['#define protected public', '#define private public', '#include <stddef.h>',
             f'#include "{header}"',
             'extern "C" const unsigned int ENB_LAYOUT[] = {',
             f'	sizeof({a.cls}),']
    lines += [f'	offsetof({a.cls}, {n}),' for n in names]
    lines += ['};', '']
    open(src, 'w', newline='').write(chr(13).join(l + chr(10) for l in lines))
    incs = [os.path.join(REPO, d) for d in DEFAULT_INCLUDES] + a.includes + vc6_sdk()
    out = os.path.join(work, 'obj_' + a.cls)
    rc, text = vc6.compile_sources([src], incs, flags='/nologo /c /MT /W0 /O2 /Ob2', out=out)
    objpath = os.path.join(out, f'layout_{a.cls}.obj')
    if not os.path.exists(objpath):
        print(text)
        raise SystemExit('layout program did not compile')
    values = read_array(objpath, '_ENB_LAYOUT', len(names) + 1)
    print(f'sizeof({a.cls}) = 0x{values[0]:X}')
    for n, v in zip(names, values[1:]):
        print(f'+0x{v:03X}  {n}')


def read_array(objpath, symbol, count):
    """The dword values of a constant array defined in a COFF object."""
    sys.path.insert(0, os.path.join(REPO, 'tools', 're'))
    import struct
    from objmatch import CoffObject
    obj = CoffObject(objpath)
    s = next(x for x in obj.symbols if x['name'] == symbol and x['section'] > 0)
    sec = obj.sections[s['section'] - 1]
    o = sec['raw'] + s['value']
    return list(struct.unpack_from(f'<{count}I', obj.data, o))


def vc6_sdk():
    return [r'C:\EnBDevelopment\SDK\directx8\include', r'C:\EnBDevelopment\SDK\radtools\include_enb']


if __name__ == '__main__':
    main()
