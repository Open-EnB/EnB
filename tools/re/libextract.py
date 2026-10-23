"""Extracts the object files from a static library (.lib, COFF archive) for byte matching.

    python libextract.py LIB OUTDIR

Import-library members (DLL stubs) are skipped; each object is written as NNNN_name.obj.
"""
import os
import re
import struct
import sys


def members(data):
    if data[:8] != b'!<arch>\n':
        raise ValueError('not a COFF archive')
    pos, longnames = 8, b''
    while pos + 60 <= len(data):
        hdr = data[pos:pos + 60]
        name = hdr[:16].decode('latin1').rstrip()
        size = int(hdr[48:58].decode().strip())
        body = data[pos + 60:pos + 60 + size]
        pos += 60 + size + (size & 1)
        if name == '//':
            longnames = body
            continue
        if name == '/':
            continue
        if name.startswith('/') and name[1:].isdigit():
            off = int(name[1:])
            name = longnames[off:longnames.index(b'\0', off)].decode('latin1')
        yield name.rstrip('/'), body


def main():
    lib, out = sys.argv[1], sys.argv[2]
    os.makedirs(out, exist_ok=True)
    n = skipped = 0
    for i, (name, body) in enumerate(members(open(lib, 'rb').read())):
        if len(body) < 20 or struct.unpack_from('<H', body, 0)[0] != 0x14C:
            skipped += 1   # import stubs (sig 0 / 0xFFFF) and anything that isn't x86 COFF
            continue
        base = re.sub(r'[^A-Za-z0-9_.]', '_', os.path.basename(name.replace('\\', '/')))
        open(os.path.join(out, f'{i:04d}_{base}'), 'wb').write(body)
        n += 1
    print(f'{lib}: {n} objects, {skipped} other members')


if __name__ == '__main__':
    main()
