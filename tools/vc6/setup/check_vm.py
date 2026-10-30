#!/usr/bin/env python3
"""Proves a build VM compiles as the client's compiler: VC6 SP4, @comp.id build 8799.

    ENB_VC6_HOSTS=user@ip python tools/vc6/setup/check_vm.py

Runs `vc6.py check` (cl 12.00.8804, link 6.00.8447), then compiles a C++ file (/O2 /GX /MT, as
docs/toolchain.md 1.2 measured) and reads the object's @comp.id: product 11 (C++), build 8799.
Exit status 1 on any difference. New-BuildVM.ps1 runs it before making the pool's base.
"""
import os
import struct
import subprocess
import sys
import tempfile

HERE = os.path.dirname(os.path.abspath(__file__))
VC6 = os.path.join(os.path.dirname(HERE), 'vc6.py')
REPO = os.path.dirname(os.path.dirname(os.path.dirname(HERE)))
SOURCE = 'struct A { virtual int f(int x) { return x * 3 + 1; } };\nint g(A *a) { return a->f(7); }\n'


def comp_id(obj):
    """The @comp.id symbol's value of a COFF object: (product << 16) | build."""
    b = open(obj, 'rb').read()
    nsyms_off, nsyms = struct.unpack_from('<II', b, 8)
    strtab = nsyms_off + 18 * nsyms
    i = 0
    while i < nsyms:
        e = b[nsyms_off + 18 * i: nsyms_off + 18 * (i + 1)]
        name = e[:8].rstrip(b'\0') if e[:4] != b'\0\0\0\0' else \
            b[strtab + struct.unpack_from('<I', e, 4)[0]:].split(b'\0', 1)[0]
        if name == b'@comp.id':
            return struct.unpack_from('<I', e, 8)[0]
        i += 1 + e[17]
    return None


def main():
    ok = True
    p = subprocess.run([sys.executable, VC6, 'check'], capture_output=True, text=True)
    print(p.stdout.strip() or p.stderr.strip())
    if '12.00.8804' not in p.stdout or '6.00.8447' not in p.stdout:
        print('FAIL: expected cl 12.00.8804 and link 6.00.8447 (VC6 SP4)')
        ok = False
    # vc6.py compiles files under C:\EnBDevelopment only (the VM mirrors that folder)
    work = tempfile.mkdtemp(prefix='vmcheck_', dir=os.path.join(REPO, '.work'))
    src = os.path.join(work, 'vmcheck.cpp')
    with open(src, 'w', newline='\r\n') as f:
        f.write(SOURCE)
    p = subprocess.run([sys.executable, VC6, 'compile', '--flags', '/nologo /c /MT /O2 /GX', '--out', work, src],
                       capture_output=True, text=True)
    obj = os.path.join(work, 'vmcheck.obj')
    if not os.path.exists(obj):
        print('FAIL: no object\n' + p.stdout[-2000:] + p.stderr[-2000:])
        return 1
    cid = comp_id(obj)
    prod, build = (cid >> 16, cid & 0xFFFF) if cid is not None else (None, None)
    print(f'@comp.id product {prod} build {build}')
    if (prod, build) != (11, 8799):
        print('FAIL: expected product 11 (C++) build 8799 (VC6 SP4)')
        ok = False
    print('PASS' if ok else 'FAIL')
    return 0 if ok else 1


if __name__ == '__main__':
    sys.exit(main())
