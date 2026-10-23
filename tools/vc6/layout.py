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
    lines = ['#define protected public', '#define private public', '#include <stddef.h>', '#include <stdio.h>',
             f'#include "{header}"', 'int main(void)', '{',
             f'\tprintf("sizeof({a.cls}) = 0x%X\\n", sizeof({a.cls}));']
    for n in names:
        lines.append(f'\tprintf("+0x%03X  {n}\\n", offsetof({a.cls}, {n}));')
    lines += ['\treturn 0;', '}', '']
    open(src, 'w', newline='\r\n').write('\n'.join(lines))
    incs = [os.path.join(REPO, d) for d in DEFAULT_INCLUDES] + a.includes + vc6_sdk()
    vc6.sync([os.path.dirname(header), work] + incs)
    exe = vc6.WORK + r'\layout.exe'
    cmd = ['cl /nologo /MT /O2 /Ob2 /w ' + ' '.join(f'/I "{i}"' for i in incs) + f' /Fe"{exe}" /Fo"{vc6.WORK}\\\\" "{src}"',
           f'"{exe}"']
    p = vc6.run_bat(cmd)
    print(p.stdout + p.stderr)


def vc6_sdk():
    return [r'C:\EnBDevelopment\SDK\directx8\include', r'C:\EnBDevelopment\SDK\radtools\include']


if __name__ == '__main__':
    main()
